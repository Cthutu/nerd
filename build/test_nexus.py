#!/usr/bin/env python3
"""Bounded Nexus contracts and bidirectional TCP wire compatibility checks."""
from __future__ import annotations

import argparse
import os
from pathlib import Path
import queue
import shlex
import subprocess
import sys
import tempfile
import threading

from test_network import ROOT, run

PINNED = "9ae32dd295241bd3889204537dfa476f222e753c"


def interop(server: Path, peer: Path, env: dict[str, str]) -> None:
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
        if output.strip() != "C peer passed":
            raise RuntimeError(f"Unexpected C peer output: {output!r}")
        output, _ = process.communicate(timeout=10)
        if process.returncode or output.strip() != "Nexus TCP C interoperability passed":
            raise RuntimeError(f"Nexus server failed ({process.returncode}): {output!r}")
    finally:
        if process.poll() is None:
            process.kill()
        process.wait(timeout=10)
        process.stdout.close()


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--compiler", type=Path, default=ROOT / "_bin" / ("nerd-debug.exe" if os.name == "nt" else "nerd-debug"))
    parser.add_argument("--release", action="store_true")
    parser.add_argument("--cgen", action="store_true")
    parser.add_argument("--upstream-dir", type=Path, help="Also build the pinned cthutu/dev C implementation")
    args = parser.parse_args()
    env = dict(os.environ, NERD_LIB_PATH=str(ROOT / "mods"))
    with tempfile.TemporaryDirectory(prefix="nerd-nexus-") as temporary:
        directory = Path(temporary)
        suffix = ".exe" if os.name == "nt" else ""
        executables = []
        for index, source in enumerate(("tests/nexus/main.n", "tests/nexus/interop.n", "examples/nexus-message/nexus-message.n")):
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
        contracts, server, example = executables
        for target, expected in ((contracts, "Nexus buffer and framing contracts passed"),
                                 (example, "Nexus message: hello nexus, reply 43")):
            output = run([str(target)], env, timeout=20)
            if output.strip() != expected:
                raise RuntimeError(f"Unexpected output: {output!r}")
            print(f"PASS {target.name}: {expected}")
        if sys.platform.startswith("linux"):
            shim = directory / "fragmented.so"
            run(["clang", "-shared", "-fPIC", "-Wall", "-Wextra", "-Werror",
                 "tests/nexus/fragmented.c", "-ldl", "-o", str(shim)], env)
            injected = dict(env, LD_PRELOAD=str(shim))
            for _ in range(3):
                if run([str(contracts)], injected, timeout=20).strip() != "Nexus buffer and framing contracts passed":
                    raise RuntimeError("Fragmented I/O contracts failed")
            print("PASS deterministic partial I/O and would-block retries")
        else:
            print("SKIP short-I/O injection requires Linux")
        peer = directory / ("wire-peer" + suffix)
        command = ["clang", "-std=c11", "-Wall", "-Wextra", "-Werror", "tests/nexus/peer.c", "-o", str(peer)]
        if os.name == "nt":
            command.append("-lws2_32")
        run(command, env)
        interop(server, peer, env)
        print("PASS bidirectional TCP golden-wire C peer")
        if args.upstream_dir:
            upstream = args.upstream_dir.resolve()
            revision = run(["git", "-C", str(upstream), "rev-parse", "HEAD"], env).strip()
            if revision != PINNED or run(["git", "-C", str(upstream), "status", "--porcelain", "--", "src/core", "src/nexus"], env).strip():
                raise RuntimeError("Upstream must have clean core/Nexus sources at the pinned revision")
            sources = sorted((upstream / "src/core").glob("*.c")) + sorted((upstream / "src/nexus").glob("*.c"))
            original = directory / ("original-peer" + suffix)
            command = ["clang", "-std=c2x", "-D_GNU_SOURCE", "-D_POSIX_C_SOURCE=200809L", "-DUPSTREAM_NEXUS",
                       "-I", str(upstream / "src"), "tests/nexus/peer.c", *map(str, sources), "-o", str(original)]
            command += ["-lws2_32"] if os.name == "nt" else ["-pthread", "-lm"]
            run(command, env)
            interop(server, original, env)
            print(f"PASS original Nexus TCP implementation at {PINNED}")


if __name__ == "__main__":
    main()
