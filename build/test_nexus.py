#!/usr/bin/env python3
"""Bounded Nexus contracts and bidirectional TCP/UDP wire compatibility checks."""
from __future__ import annotations

import argparse
import json
import os
from pathlib import Path
import queue
import re
import shlex
import signal
import socket
import struct
import subprocess
import sys
import tempfile
import threading
import time

from test_network import ROOT, run

PINNED = "9ae32dd295241bd3889204537dfa476f222e753c"


def interop(server: Path, peer: Path, env: dict[str, str], protocol: str = "TCP") -> None:
    process = subprocess.Popen([str(server)], cwd=ROOT, env=env, stdout=subprocess.PIPE,
                               stderr=subprocess.STDOUT, text=True)
    assert process.stdout is not None
    ready: queue.Queue[str] = queue.Queue()
    reader = threading.Thread(target=lambda: ready.put(process.stdout.readline()), daemon=True)
    reader.start()
    try:
        port = ready.get(timeout=10).strip()
        if not port.isdecimal() or not 0 < int(port) <= 65535:
            raise RuntimeError(f"Invalid server readiness: {port!r}")
        output = run([str(peer), port], env, timeout=10)
        if output.strip() != ("C peer passed" if protocol == "TCP" else "C UDP peer passed"):
            raise RuntimeError(f"Unexpected C peer output: {output!r}")
        output, _ = process.communicate(timeout=10)
        if process.returncode or output.strip() != f"Nexus {protocol} C interoperability passed":
            raise RuntimeError(f"Nexus server failed ({process.returncode}): {output!r}")
    finally:
        if process.poll() is None:
            process.kill()
        process.wait(timeout=10)
        process.stdout.close()


def start_timing(command: list[str], env: dict[str, str], *, shutdown_group: bool = False):
    flags = subprocess.CREATE_NEW_PROCESS_GROUP if shutdown_group and os.name == "nt" else 0
    process = subprocess.Popen(command, cwd=ROOT, env=env, stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                               stderr=subprocess.STDOUT, text=True, creationflags=flags)
    assert process.stdout is not None
    lines: queue.Queue[str] = queue.Queue()
    reader = threading.Thread(target=lambda: lines.put(process.stdout.readline()), daemon=True)
    reader.start()
    return process, lines


def stop_timing(process) -> None:
    if process.poll() is None:
        process.kill()
    process.wait(timeout=10)
    assert process.stdout is not None
    process.stdout.close()
    if process.stdin is not None:
        process.stdin.close()


def read_exact(peer: socket.socket, count: int) -> bytes:
    result = bytearray()
    while len(result) != count:
        part = peer.recv(count - len(result))
        if not part:
            raise RuntimeError("Timing peer closed before completing its frame")
        result.extend(part)
    return bytes(result)


def telnet_echo_server(server: Path, env: dict[str, str], *, blocked_send: bool = False) -> None:
    # Collect all console output on one reader: wait for lifecycle messages even
    # when clients send no data, and avoid competing buffered stdout readers.
    for connected in (False, True):
        flags = subprocess.CREATE_NEW_PROCESS_GROUP if os.name == "nt" else 0
        process = subprocess.Popen([str(server)], cwd=ROOT, env=env,
                                   stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                                   text=True, creationflags=flags)
        assert process.stdout is not None
        lines: queue.Queue[str] = queue.Queue()
        captured: list[str] = []

        def capture():
            for line in process.stdout:
                captured.append(line.rstrip("\n"))
                lines.put(line.rstrip("\n"))

        reader = threading.Thread(target=capture, daemon=True)
        reader.start()

        def expect_log(expected: str) -> None:
            end = time.monotonic() + 5
            while True:
                line = lines.get(timeout=max(0.001, end - time.monotonic()))
                if line == expected:
                    return
                if time.monotonic() >= end:
                    raise RuntimeError(f"Missing log {expected!r}: {captured!r}")

        peers = []
        try:
            line = lines.get(timeout=5)
            prefix = "Listening on tcp://127.0.0.1:"
            if not line.startswith(prefix) or not line[len(prefix):].isdecimal():
                raise RuntimeError(f"Invalid echo server readiness: {line!r}")
            port = int(line[len(prefix):])

            def connect(client_id: int):
                # The very first attempt must succeed: no application retry.
                peer = socket.create_connection(("127.0.0.1", port), timeout=5)
                peers.append(peer)
                peer.settimeout(5)
                if read_exact(peer, 3) != b"\xff\xfd\x1f":
                    raise RuntimeError("Echo server did not negotiate Telnet")
                expect_log(f"[Client {client_id}] connected")
                return peer

            if connected:
                first = connect(1)
                second = connect(2)
                if blocked_send:
                    first.sendall(b"stall\r\n")
                    expect_log("Nexus text send blocked")
                    # The shim blocks only the first client's application send.
                    # Its pending echo must not stop a different client's reply.
                    second.sendall(b"second\r\n")
                    if read_exact(second, 8) != b"second\r\n":
                        raise RuntimeError("Stalled echo blocked another client")
                    expect_log("[Client 2] second")
                else:
                    coalesced = b"first\r\n\r\nliteral\xff\xffIAC\r\n"
                    first.sendall(coalesced)
                    if read_exact(first, len(coalesced)) != coalesced:
                        raise RuntimeError("Echo lost a line, empty payload or escaped IAC")
                    expect_log("[Client 1] first")
                    expect_log("[Client 1] ")
                    expect_log("[Client 1] literal\\xffIAC")
                    first.sendall(b"unfinished")
                    second.sendall(b"second\r\n")
                    if read_exact(second, 8) != b"second\r\n":
                        raise RuntimeError("Echo went to the wrong client")
                    expect_log("[Client 2] second")
                    # A reset must remove only this peer, and IDs cannot be
                    # recycled when the newly freed connection slot is reused.
                    reset = connect(3)
                    reset.setsockopt(socket.SOL_SOCKET, socket.SO_LINGER,
                                     struct.pack("hh" if os.name == "nt" else "ii", 1, 0))
                    reset.close()
                    expect_log("[Client 3] disconnected")
                    fourth = connect(4)
                    fourth.sendall(b"after reset\r\n")
                    if read_exact(fourth, 13) != b"after reset\r\n":
                        raise RuntimeError("Client reset stopped the listener")
                    expect_log("[Client 4] after reset")
                    first.close()
                    expect_log("[Client 1] disconnected")
            process.send_signal(signal.CTRL_BREAK_EVENT if os.name == "nt" else signal.SIGINT)
            process.wait(timeout=5)
            reader.join(timeout=5)
            if reader.is_alive() or process.returncode or not captured or captured[-1] != "Telnet server stopped.":
                raise RuntimeError(f"Echo shutdown failed ({process.returncode}): {captured!r}")
            if "Shutdown requested." not in captured:
                raise RuntimeError("Echo server missed shutdown notification")
            for peer in peers:
                if peer.fileno() == -1:
                    continue
                try:
                    if peer.recv(1) != b"":
                        raise RuntimeError("Echo peer remained open after shutdown")
                except ConnectionResetError:
                    pass
            if connected:
                for client_id in range(1, 3 if blocked_send else 5):
                    if captured.count(f"[Client {client_id}] connected") != 1 or captured.count(f"[Client {client_id}] disconnected") != 1:
                        raise RuntimeError(f"Missing or duplicate client lifecycle log: {captured!r}")
        finally:
            for peer in peers:
                peer.close()
            stop_timing(process)
            reader.join(timeout=5)


def telnet_original(driver: Path, peer: Path, env: dict[str, str]) -> None:
    process, ready = start_timing([str(driver), "echo"], env)
    try:
        port = ready.get(timeout=5).strip()
        if run([str(peer), port], env, timeout=10).strip() != "C Telnet peer passed":
            raise RuntimeError("Original Telnet client failed")
        output, _ = process.communicate(timeout=5)
        if process.returncode or output.strip() != "Nexus Telnet wire passed":
            raise RuntimeError(f"Nerd Telnet server failed: {output!r}")
    finally:
        stop_timing(process)
    process, ready = start_timing([str(peer), "server"], env)
    try:
        port = ready.get(timeout=5).strip()
        url = f"tcp://127.0.0.1:{int(port)}"
        if run([str(driver), "original", url], env, timeout=10).strip() != "Nexus Telnet wire passed":
            raise RuntimeError("Nerd Telnet client failed")
        output, _ = process.communicate(timeout=5)
        if process.returncode or output.strip() != "C Telnet peer passed":
            raise RuntimeError(f"Original Telnet server failed: {output!r}")
    finally:
        stop_timing(process)


def telnet_wire(driver: Path, env: dict[str, str]) -> None:
    # Both directions are independently checked against raw Telnet wire bytes.
    # Send each command byte separately: IAC, option and SB parsing must resume.
    process, ready = start_timing([str(driver), "oversized"], env)
    try:
        port = int(ready.get(timeout=5).strip())
        with socket.create_connection(("127.0.0.1", port), timeout=5) as peer:
            if read_exact(peer, 3) != b"\xff\xfd\x1f":
                raise RuntimeError("Missing oversized peer session request")
            peer.sendall(b"x" * (1_048_576 + 1))
            output, _ = process.communicate(timeout=5)
        if process.returncode or output.strip() != "Nexus Telnet limit passed":
            raise RuntimeError(f"Telnet input limit failed: {output!r}")
    finally:
        stop_timing(process)
    for character in (False, True):
        mode = "character" if character else "line"
        process, ready = start_timing([str(driver), mode], env)
        try:
            port = int(ready.get(timeout=5).strip())
            with socket.create_connection(("127.0.0.1", port), timeout=5) as peer:
                greeting = b"\xff\xfd\x1f"
                if character:
                    greeting += b"\xff\xfb\x01\xff\xfb\x03\xff\xfd\x03"
                if read_exact(peer, len(greeting)) != greeting:
                    raise RuntimeError("Wrong Telnet session negotiation")
                if character:
                    peer.sendall(b"ab\r\n\xff\xffz")
                    if read_exact(peer, 5) != b"ab\n\xffz":
                        raise RuntimeError("Character mode did not preserve/normalise bytes")
                else:
                    commands = (b"\xff\xfb\x01\xff\xfd\x03\xff\xfb\x1f"
                                b"\xff\xfa\x1f\x00\xff\xff\x00\x19\xff\xf0")
                    for byte in commands:
                        peer.sendall(bytes([byte]))
                        time.sleep(0.001)
                    peer.sendall(b"hi\xff\xff!\r\n\r\nlast\n")
                    expected = b"\xff\xfe\x01\xff\xfc\x03reply\r\n\r\nlast\r\n"
                    if read_exact(peer, len(expected)) != expected:
                        raise RuntimeError("Telnet negotiation/line reply mismatch")
            output, _ = process.communicate(timeout=5)
            if process.returncode or output.strip() != "Nexus Telnet wire passed":
                raise RuntimeError(f"Telnet server failed: {output!r}")
        finally:
            stop_timing(process)
        with socket.socket() as listener:
            listener.bind(("127.0.0.1", 0))
            listener.listen()
            listener.settimeout(5)
            url = f"tcp://127.0.0.1:{listener.getsockname()[1]}"
            process, finished = start_timing([str(driver), mode, url], env)
            try:
                with listener.accept()[0] as peer:
                    peer.settimeout(5)
                    expected = b"xy" if character else b"xy\r\n\r\n"
                    if read_exact(peer, len(expected)) != expected:
                        raise RuntimeError("Telnet outgoing bytes were framed or incorrectly terminated")
                    peer.sendall(b"!!" if character else b"final\r\n")
                output, _ = process.communicate(timeout=5)
                output = finished.get(timeout=5) + output
                if process.returncode or output.strip() != "Nexus Telnet wire passed":
                    raise RuntimeError(f"Telnet {mode} client failed ({process.returncode}): {output!r}")
            finally:
                stop_timing(process)


def delayed_connect(client: Path, env: dict[str, str]) -> None:
    # Keep the port bound continuously: it cannot be stolen between choosing
    # an ephemeral address and starting to listen. Bind alone refuses connects.
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as listener:
        listener.bind(("127.0.0.1", 0))
        listener.settimeout(5)
        url = f"tcp://127.0.0.1:{listener.getsockname()[1]}"
        process, ready = start_timing([str(client), "connect", url], env)
        try:
            if ready.get(timeout=5).strip() != "retry-ready":
                raise RuntimeError("Client did not observe the absent listener")
            time.sleep(0.12)
            listener.listen()
            with listener.accept()[0] as peer:
                peer.settimeout(5)
                count, = struct.unpack("!I", read_exact(peer, 4))
                expected = struct.pack("!I", 15) + b"delayed-connect"
                if count != len(expected) or read_exact(peer, count) != expected:
                    raise RuntimeError("Delayed connection lost/corrupted its message")
                peer.sendall(struct.pack("!II", 4, 42))
            output, _ = process.communicate(timeout=5)
            if process.returncode or output.strip() != "Nexus delayed connect passed":
                raise RuntimeError(f"Delayed connect failed ({process.returncode}): {output!r}")
        finally:
            stop_timing(process)


def delayed_receive(server: Path, env: dict[str, str], udp: bool) -> None:
    mode = "arrival-udp" if udp else "arrival"
    process, ready = start_timing([str(server), mode], env)
    try:
        port = ready.get(timeout=5).strip()
        if not port.isdecimal() or not 0 < int(port) <= 65535:
            raise RuntimeError(f"Invalid timing server readiness: {port!r}")
        time.sleep(0.05)
        payload = struct.pack("!I", 7) + b"arrived"
        kind = socket.SOCK_DGRAM if udp else socket.SOCK_STREAM
        with socket.socket(socket.AF_INET, kind) as peer:
            peer.settimeout(5)
            peer.connect(("127.0.0.1", int(port)))
            peer.sendall(payload if udp else struct.pack("!I", len(payload)) + payload)
        output, _ = process.communicate(timeout=5)
        if process.returncode or output.strip() != "Nexus delayed receive passed":
            raise RuntimeError(f"Delayed receive failed ({process.returncode}): {output!r}")
    finally:
        stop_timing(process)


def ready_deadlines(driver: Path, env: dict[str, str]) -> None:
    receiving = dict(env, NEXUS_TEST_RECV_WOULD_BLOCK="1")
    for mode, udp in (("ready-deadline", False), ("ready-deadline-udp", True),
                      ("ready-deadline-telnet", False)):
        process, ready = start_timing([str(driver), mode], receiving)
        try:
            port = ready.get(timeout=5).strip()
            if not port.isdecimal() or not 0 < int(port) <= 65535:
                raise RuntimeError(f"Invalid deadline readiness: {port!r}")
            with socket.socket(socket.AF_INET, socket.SOCK_DGRAM if udp else socket.SOCK_STREAM) as peer:
                peer.settimeout(5)
                peer.connect(("127.0.0.1", int(port)))
                peer.sendall(struct.pack("!I", 42) if udp else struct.pack("!II", 4, 42))
                output, _ = process.communicate(input="g", timeout=5)
            if process.returncode or output.strip() != "Nexus ready deadline passed":
                raise RuntimeError(f"Ready deadline failed ({process.returncode}): {output!r}")
        finally:
            stop_timing(process)
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as peer:
        peer.bind(("127.0.0.1", 0))
        url = f"udp://127.0.0.1:{peer.getsockname()[1]}"
        output = run([str(driver), "udp-send-deadline", url], dict(env, NEXUS_TEST_SEND_WOULD_BLOCK="1"), timeout=5)
        if output.strip() != "Nexus UDP send deadline passed":
            raise RuntimeError(f"Ready send deadline failed: {output!r}")
    print("PASS TCP/UDP/Telnet deadlines despite persistent ready-but-would-block I/O")


def audit_parity(upstream: Path | None = None) -> None:
    mapping = json.loads((ROOT / "tests/nexus/parity-map.json").read_text())
    if mapping["source_revision"] != PINNED:
        raise RuntimeError("Parity map revision differs from interoperability pin")
    cases = mapping["cases"]
    names = [case["source"] for case in cases]
    if len(names) != len(set(names)) or len(names) != 33:
        raise RuntimeError("Parity map must contain the 33 unique pinned scenarios")
    counts = {"covered": 0, "pending": 0}
    doc = (ROOT / "docs/nexus-parity.md").read_text()
    for case in cases:
        status = case["status"]
        if status not in counts or bool(case["coverage"]) != (status == "covered"):
            raise RuntimeError(f"Invalid parity status/coverage: {case!r}")
        if f"| `{case['source']}` | {status.capitalize()} |" not in doc:
            raise RuntimeError(f"Parity documentation is stale: {case['source']}")
        counts[status] += 1
        for reference in case["coverage"]:
            path = ROOT / reference["file"]
            if path.suffix == ".n":
                pattern = rf"(?m)^\s*(?:pub\s+)?{re.escape(reference['function'])}\s*::\s*fn\b"
            else:
                pattern = rf"(?m)^def {re.escape(reference['function'])}\("
            if not re.search(pattern, path.read_text()):
                raise RuntimeError(f"Missing parity function: {reference!r}")
    if counts != {"covered": 33, "pending": 0}:
        raise RuntimeError(f"Parity inventory/status changed: {counts!r}")
    if upstream is not None:
        source_names = set()
        for path in (upstream / "tests/nexus").glob("*.c"):
            source_names.update(f"{path.name}::{name}" for name in
                                re.findall(r"TEST_CASE\(nexus,\s*(\w+)\)", path.read_text()))
        if source_names != set(names):
            raise RuntimeError(f"Source parity inventory mismatch: {source_names ^ set(names)}")
    print("PASS Nexus source parity map: 33 source equivalents including Telnet")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--compiler", type=Path, default=ROOT / "_bin" / ("nerd-debug.exe" if os.name == "nt" else "nerd-debug"))
    parser.add_argument("--release", action="store_true")
    parser.add_argument("--cgen", action="store_true")
    parser.add_argument("--upstream-dir", type=Path, help="Also build the pinned cthutu/dev C implementation")
    args = parser.parse_args()
    audit_parity()
    env = dict(os.environ, NERD_LIB_PATH=str(ROOT / "mods"))
    with tempfile.TemporaryDirectory(prefix="nerd-nexus-") as temporary:
        directory = Path(temporary)
        suffix = ".exe" if os.name == "nt" else ""
        executables = []
        for index, source in enumerate((
            "tests/nexus/main.n",
            "tests/nexus/interop.n",
            "examples/nexus-message/nexus-message.n",
            "tests/nexus/socket-api.n",
            "tests/nexus/udp-interop.n",
            "examples/nexus-multi-client/nexus-multi-client.n",
            "examples/nexus-request-reply/nexus-request-reply.n",
            "tests/nexus/parity.n",
            "tests/nexus/timing-driver.n",
            "tests/nexus/telnet.n",
            "examples/nexus-telnet/nexus-telnet.n",
            "examples/nexus-telnet-server/nexus-telnet-server.n",
            "examples/nexus-timeouts/nexus-timeouts.n",
        )):
            target = directory / (f"case-{index}" + suffix)
            if source == "examples/nexus-telnet-server/nexus-telnet-server.n":
                # The example has a fixed endpoint and no test/argument modes.
                # Change only its port in a temporary copy so independent runs
                # use OS-assigned ports and cannot collide with a user's server.
                original = (ROOT / source).read_text()
                instrumented, count = re.subn(r'"tcp://127\.0\.0\.1:[0-9]+"',
                                              '"tcp://127.0.0.1:0"', original)
                assert count == 1
                copy = directory / "nexus-telnet-server.n"
                copy.write_text(instrumented)
                source = str(copy)
            command = [str(args.compiler.resolve()), "build", source, "-o", str(target)]
            if args.release:
                command.append("--release")
            if args.cgen:
                command.append("--cgen")
            run(command, env)
            if args.cgen:
                flags = shlex.split(run(command + ["--copts"], env))
                run(["clang", str(target.with_suffix(".c")), *flags, "-o", str(target)], env)
            executables.append(target)
        (contracts, server, example, socket_api, udp_server, multi_client,
         request_reply, parity, timing, telnet, telnet_example, echo_server,
         timeouts) = executables
        for target, expected in ((contracts, "Nexus buffer and framing contracts passed"),
                                 (example, "Nexus message: hello nexus, reply 43"),
                                 (socket_api, "Nexus socket API contracts passed"),
                                 (multi_client, "Nexus multi-client: replies 11 and 21"),
                                 (request_reply, "Nexus request/reply: two exchanges passed"),
                                 (parity, "Nexus parity contracts passed"),
                                 (telnet, "Nexus Telnet contracts passed"),
                                 (telnet_example, "Nexus Telnet: echo: hello terminal"),
                                 (timeouts, "Receive timed out\nReceive would block\nReceived: hello after retry")):
            output = run([str(target)], env, timeout=20)
            if output.strip() != expected:
                raise RuntimeError(f"Unexpected output: {output!r}")
            print(f"PASS {target.name}: {expected}")
        telnet_echo_server(echo_server, env)
        print("PASS Telnet echo server client lifecycle, reset recovery, routes and shutdown")
        telnet_wire(telnet, env)
        print("PASS Telnet raw line/character wire, negotiation, NAWS and EOF")
        delayed_connect(timing, env)
        delayed_receive(timing, env, False)
        delayed_receive(timing, env, True)
        print("PASS delayed listener retry and TCP/UDP arrival before timeout")
        if sys.platform.startswith("linux"):
            shim = directory / "fragmented.so"
            run(["clang", "-shared", "-fPIC", "-Wall", "-Wextra", "-Werror",
                 "tests/nexus/fragmented.c", "-ldl", "-o", str(shim)], env)
            injected = dict(env, LD_PRELOAD=str(shim))
            for _ in range(3):
                if run([str(contracts)], injected, timeout=20).strip() != "Nexus buffer and framing contracts passed":
                    raise RuntimeError("Fragmented I/O contracts failed")
            if run([str(socket_api)], injected, timeout=20).strip() != "Nexus socket API contracts passed":
                raise RuntimeError("Socket API fragmented I/O contracts failed")
            if run([str(parity)], injected, timeout=20).strip() != "Nexus parity contracts passed":
                raise RuntimeError("Parity fragmented I/O contracts failed")
            if run([str(telnet)], injected, timeout=20).strip() != "Nexus Telnet contracts passed":
                raise RuntimeError("Telnet fragmented I/O contracts failed")
            telnet_echo_server(echo_server, injected)
            telnet_echo_server(echo_server, dict(injected, NEXUS_TEST_TEXT_SEND_WOULD_BLOCK="1"), blocked_send=True)
            telnet_wire(telnet, injected)
            delayed_connect(timing, dict(injected, NEXUS_TEST_CONNECT_REFUSALS="3"))
            ready_deadlines(timing, injected)
            print("PASS deterministic partial I/O, refused connects and would-block retries")
        else:
            print("SKIP short-I/O injection requires Linux")
        peer = directory / ("wire-peer" + suffix)
        command = ["clang", "-std=c11", "-Wall", "-Wextra", "-Werror", "tests/nexus/peer.c", "-o", str(peer)]
        if os.name == "nt":
            command.append("-lws2_32")
        run(command, env)
        interop(server, peer, env)
        print("PASS bidirectional TCP golden-wire C peer")
        udp_peer = directory / ("udp-peer" + suffix)
        command = ["clang", "-std=c11", "-Wall", "-Wextra", "-Werror", "tests/nexus/udp-peer.c", "-o", str(udp_peer)]
        if os.name == "nt":
            command.append("-lws2_32")
        run(command, env)
        interop(udp_server, udp_peer, env, "UDP")
        print("PASS bidirectional UDP golden-wire C peer")
        if args.upstream_dir:
            upstream = args.upstream_dir.resolve()
            revision = run(["git", "-C", str(upstream), "rev-parse", "HEAD"], env).strip()
            if revision != PINNED or run(["git", "-C", str(upstream), "status", "--porcelain", "--", "src/core", "src/nexus", "tests/nexus"], env).strip():
                raise RuntimeError("Upstream must have clean core/Nexus sources at the pinned revision")
            audit_parity(upstream)
            sources = sorted((upstream / "src/core").glob("*.c")) + sorted((upstream / "src/nexus").glob("*.c"))
            original = directory / ("original-peer" + suffix)
            command = ["clang", "-std=c2x", "-D_GNU_SOURCE", "-D_POSIX_C_SOURCE=200809L", "-DUPSTREAM_NEXUS",
                       "-I", str(upstream / "src"), "tests/nexus/peer.c", *map(str, sources), "-o", str(original)]
            command += ["-lws2_32"] if os.name == "nt" else ["-pthread", "-lm"]
            run(command, env)
            interop(server, original, env)
            print(f"PASS original Nexus TCP implementation at {PINNED}")
            udp_original = directory / ("original-udp-peer" + suffix)
            udp_command = [str(udp_original) if arg == str(original) else "tests/nexus/udp-peer.c" if arg == "tests/nexus/peer.c" else arg for arg in command]
            run(udp_command, env)
            interop(udp_server, udp_original, env, "UDP")
            print(f"PASS original Nexus UDP implementation at {PINNED}")
            telnet_peer = directory / ("original-telnet-peer" + suffix)
            telnet_command = [str(telnet_peer) if arg == str(original) else "tests/nexus/telnet-peer.c" if arg == "tests/nexus/peer.c" else arg for arg in command]
            run(telnet_command, env)
            telnet_original(telnet, telnet_peer, env)
            print(f"PASS original Nexus Telnet client and server at {PINNED}")


if __name__ == "__main__":
    main()
