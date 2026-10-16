"""Bounded multi-client traffic and real socket backpressure for Nexus runners."""
from __future__ import annotations

from collections import Counter, deque
import contextlib
import os
from pathlib import Path
import re
import signal
import socket
import struct
import subprocess
import threading
import time


class Server:
    """Drain stdout continuously, retaining lifecycle counts and bounded diagnostics."""

    def __init__(self, executable: Path, env: dict[str, str], telnet: bool = False, *, lifetime=60):
        self.telnet = telnet
        self.condition = threading.Condition()
        self.tail: deque[str] = deque(maxlen=30)
        self.connected: Counter[int] = Counter()
        self.disconnected: Counter[int] = Counter()
        self.blocked = 0
        self.resumed = 0
        self.port = None
        self.finished = False
        self.unexpected: deque[str] = deque(maxlen=10)
        self.peers: list[socket.socket] = []
        flags = subprocess.CREATE_NEW_PROCESS_GROUP if os.name == "nt" else 0
        self.process = subprocess.Popen([str(executable)], env=env, stdout=subprocess.PIPE,
                                        stderr=subprocess.STDOUT, text=True, creationflags=flags)
        self.reader = threading.Thread(target=self.capture, daemon=True)
        self.reader.start()
        self.watchdog = threading.Timer(lifetime, self.expire)
        self.watchdog.daemon = True
        self.watchdog.start()

    def expire(self):
        if self.process.poll() is None:
            self.process.kill()

    def capture(self):
        assert self.process.stdout is not None
        for raw in self.process.stdout:
            line = raw.rstrip("\n")
            with self.condition:
                self.tail.append(line)
                event = re.fullmatch(r"\[Client (\d+)\] (connected|disconnected)", line)
                ready = re.fullmatch(r"Listening on tcp://127\.0\.0\.1:(\d+)", line)
                if event:
                    counts = self.connected if event[2] == "connected" else self.disconnected
                    counts[int(event[1])] += 1
                elif ready:
                    self.port = int(ready[1])
                elif line == "Send blocked":
                    self.blocked += 1
                elif line == "Send resumed":
                    self.resumed += 1
                elif line == ("Telnet server stopped." if self.telnet else "Nexus stress stopped."):
                    self.finished = True
                elif not (self.telnet and (line.startswith("[Client ") or
                          line.startswith("Connect with telnet,") or line == "Shutdown requested.")):
                    self.unexpected.append(line)
                self.condition.notify_all()
        with self.condition:
            self.condition.notify_all()

    def wait(self, predicate, timeout=10):
        end = time.monotonic() + timeout
        with self.condition:
            while not predicate():
                remaining = end - time.monotonic()
                if remaining <= 0 or self.process.poll() is not None:
                    raise RuntimeError(f"Stress server stalled/exited: {list(self.tail)!r}")
                self.condition.wait(min(remaining, 0.1))

    def connect(self, *, slow=False):
        self.wait(lambda: self.port is not None)
        peer = socket.socket()
        self.peers.append(peer)
        peer.settimeout(10)
        if slow:
            # Set before connect to negotiate a small receive window. We still
            # require an observed WouldBlock; buffer-size assumptions cannot pass.
            peer.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, 4096)
        peer.connect(("127.0.0.1", self.port))
        if self.telnet and read_exact(peer, 3) != b"\xff\xfd\x1f":
            raise RuntimeError("Missing Telnet negotiation")
        return peer

    def shutdown(self):
        self.process.send_signal(signal.CTRL_BREAK_EVENT if os.name == "nt" else signal.SIGINT)
        self.process.wait(timeout=10)
        self.reader.join(timeout=5)
        if self.reader.is_alive() or self.process.returncode or not self.finished or self.unexpected:
            raise RuntimeError(f"Stress shutdown failed: {list(self.tail)!r}")
        if not self.connected or self.connected != self.disconnected or any(n != 1 for n in self.connected.values()):
            raise RuntimeError(f"Lost/duplicate lifecycle events: {self.connected}, {self.disconnected}")

    def close(self):
        self.watchdog.cancel()
        self.watchdog.join(timeout=5)
        for peer in self.peers:
            peer.close()
        if self.process.poll() is None:
            try:
                self.process.wait(timeout=0.2)
            except subprocess.TimeoutExpired:
                self.process.kill()
        self.process.wait(timeout=10)
        self.reader.join(timeout=5)
        if not self.finished:
            print(f"Stress server diagnostics: {list(self.tail)!r}")
        assert self.process.stdout is not None
        self.process.stdout.close()


def read_exact(peer: socket.socket, count: int) -> bytes:
    result = bytearray()
    deadline = time.monotonic() + 20
    previous_timeout = peer.gettimeout()
    try:
        while len(result) < count:
            remaining = deadline - time.monotonic()
            if remaining <= 0:
                raise TimeoutError("Stress reply exceeded its total receive budget")
            peer.settimeout(remaining)
            data = peer.recv(count - len(result))
            if not data:
                raise RuntimeError(f"Truncated stress reply: {len(result)}/{count} bytes")
            result.extend(data)
        return bytes(result)
    finally:
        peer.settimeout(previous_timeout)


def frame(payload: bytes) -> bytes:
    return struct.pack("!I", len(payload)) + payload


def traffic(executable: Path, env: dict[str, str], *, telnet=False, rounds=12):
    # Eight concurrent peers per round, twelve ordered messages per peer. More
    # than 63 lifetime connections forces reuse of Nexus's bounded peer slots.
    with contextlib.closing(Server(executable, env, telnet, lifetime=max(60, rounds * 2))) as server:
        for cycle in range(rounds):
            peers = [server.connect() for _ in range(8)]
            expected = []
            for client, peer in enumerate(peers):
                messages = []
                for sequence in range(12):
                    label = f"round={cycle} client={client} sequence={sequence}:".encode()
                    payload = label + bytes([65 + client]) * (sequence * 13)
                    messages.append(payload + b"\r\n" if telnet else frame(payload + b"\x00\xff"))
                wire = b"".join(messages)
                expected.append(wire)
                # Split a header/line prefix, then coalesce the remaining messages.
                peer.sendall(wire[:2])
                peer.sendall(wire[2:])
            for peer, wire in zip(peers, expected):
                if read_exact(peer, len(wire)) != wire:
                    raise RuntimeError(f"Corrupted/misrouted/duplicated stress reply in round {cycle}")
            for index, peer in enumerate(peers):
                if not index % 2:
                    peer.shutdown(socket.SHUT_WR)
                    if peer.recv(1):
                        raise RuntimeError("Extra reply bytes after completed stream")
                else:
                    peer.setsockopt(socket.SOL_SOCKET, socket.SO_LINGER,
                                    struct.pack("hh" if os.name == "nt" else "ii", 1, 0))
                peer.close()
            count = (cycle + 1) * 8
            server.wait(lambda: sum(server.disconnected.values()) == count)
            if set(server.connected) != set(range(1, count + 1)):
                raise RuntimeError("Connection identities were reused or skipped")
            # Closed sockets do not need to accumulate throughout an extended run.
            server.peers.clear()
        # Shutdown with live peers and partial incoming frames/lines as well.
        for _ in range(4):
            peer = server.connect()
            peer.sendall(b"unfinished" if telnet else b"\x00\x00")
        server.wait(lambda: sum(server.connected.values()) == rounds * 8 + 4)
        server.shutdown()
        for peer in server.peers:
            try:
                if peer.recv(1):
                    raise RuntimeError("Unexpected data after stress shutdown")
            except ConnectionResetError:
                pass
    print(f"PASS {'Telnet' if telnet else 'TCP'} stress: {rounds * 8} connections, "
          f"{rounds * 96} exact replies, FIN/reset churn and partial-input shutdown")


def backpressure(executable: Path, env: dict[str, str]):
    with contextlib.closing(Server(executable, env)) as server:
        slow = server.connect(slow=True)
        fast = server.connect()
        # Eight distinct half-megabyte payloads exceed ordinary TCP send buffers.
        # The writer may block too, so give it its own bounded I/O operation.
        wire = b"".join(frame(bytes([i]) * (512 * 1024)) for i in range(8))
        failures = []

        def send():
            try:
                slow.sendall(wire)
            except OSError as error:
                failures.append(error)

        writer = threading.Thread(target=send, daemon=True)
        writer.start()
        try:
            server.wait(lambda: server.blocked > 0)
            for i in range(20):
                probe = frame(f"healthy-{i}".encode())
                fast.sendall(probe)
                if read_exact(fast, len(probe)) != probe:
                    raise RuntimeError("Slow reader blocked or corrupted a healthy peer")
            # Only drain after the healthy peer has completed, proving isolation.
            if read_exact(slow, len(wire)) != wire:
                raise RuntimeError("Pending send recovery lost, repeated or reordered bytes")
            writer.join(timeout=10)
            if writer.is_alive() or failures:
                raise RuntimeError(f"Slow writer failed: {failures!r}")
            server.wait(lambda: server.resumed > 0)
            probe = frame(b"recovered")
            slow.sendall(probe)
            if read_exact(slow, len(probe)) != probe:
                raise RuntimeError("Extra reply bytes or unusable route after recovery")
            # Re-enter backpressure, then cancel while output remains pending.
            before = server.blocked
            writer = threading.Thread(target=send, daemon=True)
            writer.start()
            server.wait(lambda: server.blocked > before)
            server.shutdown()
        finally:
            slow.close()
            writer.join(timeout=10)
            if writer.is_alive():
                raise RuntimeError("Stress writer survived shutdown")
    print("PASS TCP backpressure: healthy-peer progress, exact recovery and pending-send shutdown")
