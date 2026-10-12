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


def start_timing(command: list[str], env: dict[str, str]):
    process = subprocess.Popen(command, cwd=ROOT, env=env, stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                               stderr=subprocess.STDOUT, text=True)
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
    for udp in (False, True):
        mode = "ready-deadline-udp" if udp else "ready-deadline"
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
    print("PASS TCP/UDP deadlines despite persistent ready-but-would-block I/O")


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
    if counts != {"covered": 22, "pending": 11}:
        raise RuntimeError(f"Parity inventory/status changed: {counts!r}")
    if upstream is not None:
        source_names = set()
        for path in (upstream / "tests/nexus").glob("*.c"):
            source_names.update(f"{path.name}::{name}" for name in
                                re.findall(r"TEST_CASE\(nexus,\s*(\w+)\)", path.read_text()))
        if source_names != set(names):
            raise RuntimeError(f"Source parity inventory mismatch: {source_names ^ set(names)}")
    print("PASS Nexus source parity map: 22 core equivalents; 11 Telnet pending")


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
        for index, source in enumerate(("tests/nexus/main.n", "tests/nexus/interop.n", "examples/nexus-message/nexus-message.n", "tests/nexus/socket-api.n", "tests/nexus/udp-interop.n", "examples/nexus-multi-client/nexus-multi-client.n", "examples/nexus-request-reply/nexus-request-reply.n", "tests/nexus/parity.n", "tests/nexus/timing-driver.n")):
            target = directory / (f"case-{index}" + suffix)
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
        contracts, server, example, socket_api, udp_server, multi_client, request_reply, parity, timing = executables
        for target, expected in ((contracts, "Nexus buffer and framing contracts passed"),
                                 (example, "Nexus message: hello nexus, reply 43"),
                                 (socket_api, "Nexus socket API contracts passed"),
                                 (multi_client, "Nexus multi-client: replies 11 and 21"),
                                 (request_reply, "Nexus request/reply: two exchanges passed"),
                                 (parity, "Nexus parity contracts passed")):
            output = run([str(target)], env, timeout=20)
            if output.strip() != expected:
                raise RuntimeError(f"Unexpected output: {output!r}")
            print(f"PASS {target.name}: {expected}")
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


if __name__ == "__main__":
    main()
