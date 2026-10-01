#!/usr/bin/env python3
"""Bounded loopback contract/example checks using this checkout's modules."""
from __future__ import annotations

import argparse
import os
from pathlib import Path
import signal
import shlex
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]


def run(command: list[str], env: dict[str, str], *, timeout: int = 60) -> str:
    # Compilers can spawn native tool children; kill their process tree on timeout.
    process = subprocess.Popen(command, cwd=ROOT, env=env, stdout=subprocess.PIPE,
                               stderr=subprocess.STDOUT, text=True, encoding="utf-8",
                               errors="replace", start_new_session=os.name != "nt")
    try:
        output, _ = process.communicate(timeout=timeout)
    except subprocess.TimeoutExpired:
        if os.name == "nt":
            subprocess.run(["taskkill", "/PID", str(process.pid), "/T", "/F"],
                           capture_output=True, check=False)
        else:
            os.killpg(process.pid, signal.SIGKILL)
        output, _ = process.communicate()
        raise RuntimeError(f"Timed out: {command!r}\n{output}") from None
    if process.returncode:
        raise RuntimeError(f"Exit {process.returncode}: {command!r}\n{output}")
    return output


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    default = ROOT / "_bin" / ("nerd-debug.exe" if os.name == "nt" else "nerd-debug")
    parser.add_argument("--compiler", type=Path, default=default)
    parser.add_argument("--release", action="store_true", help="Optimise the Nerd programs")
    parser.add_argument("--cgen", action="store_true", help="Explicit C output compatibility check via Clang")
    args = parser.parse_args()
    compiler = str(args.compiler.resolve())
    if not Path(compiler).is_file():
        parser.error(f"Build the compiler first: {compiler}")
    env = dict(os.environ, NERD_LIB_PATH=str(ROOT / "mods"))
    cases = [("tests/network/main.n", "network contracts passed"),
             ("examples/network-echo/network-echo.n", "TCP echo: hello sockets"),
             ("examples/network-datagram/network-datagram.n", "UDP reply: hello datagrams")]
    with tempfile.TemporaryDirectory(prefix="nerd-network-") as temporary:
        directory = Path(temporary)
        suffix = ".exe" if os.name == "nt" else ""
        layout = directory / ("layout" + suffix)
        run(["clang", "-std=c11", str(ROOT / "tests/network/layout.c"), "-o", str(layout)], env)
        run([str(layout)], env, timeout=20)
        print("PASS native socket layout")
        for index, (source, expected) in enumerate(cases):
            executable = directory / (f"case-{index}" + suffix)
            command = [compiler, "build", source, "-o", str(executable)]
            if args.release:
                command.append("--release")
            if args.cgen:
                command.append("--cgen")
            run(command, env)
            if args.cgen:
                flags = shlex.split(run(command + ["--copts"], env))
                run(["clang", str(executable.with_suffix(".c")), *flags, "-o", str(executable)], env)
            output = run([str(executable)], env, timeout=20)
            if output.strip() != expected:
                raise RuntimeError(f"Unexpected {source} output: {output!r}")
            print(f"PASS {source}")


if __name__ == "__main__":
    main()
