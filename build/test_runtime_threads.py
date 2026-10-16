#!/usr/bin/env python3
"""Exercise Nerd runtime worker TLS and concurrent diagnostic bookkeeping."""
import os
from pathlib import Path
import shlex
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]

def main():
    cc = shlex.split(os.environ.get("CC", "clang"))
    with tempfile.TemporaryDirectory(prefix="nerd-runtime-threads-") as directory:
        output = Path(directory) / ("runtime.exe" if os.name == "nt" else "runtime")
        for release in (False, True):
            flags = ["-std=c23", "-Wall", "-Wextra", "-Werror", "-O2"]
            flags += ["-DNDEBUG"] if release else []
            flags += [] if os.name == "nt" else ["-D_GNU_SOURCE", "-pthread"]
            subprocess.run(cc + flags + [str(ROOT / "tests/core/runtime-threads.c"), "-o", str(output)], check=True)
            for _ in range(3):
                result = subprocess.run([str(output)], capture_output=True, text=True,
                                        encoding="utf-8", timeout=60)
                assert result.returncode == 0, result.stderr
                assert result.stdout == "runtime threads passed\n", result.stdout
                assert result.stderr == "", result.stderr
            print("[PASS] runtime workers:", "release" if release else "debug")

if __name__ == "__main__":
    main()
