#!/usr/bin/env python3
"""Bounded native Raptor contract tests, including deterministic stealing."""
import argparse
import os
from pathlib import Path
import shlex
import sys
import tempfile

from test_std_thread_sync import ROOT, run


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--nerd", type=Path, required=True)
    parser.add_argument("--cgen", action="store_true")
    args = parser.parse_args()
    env = dict(os.environ, NERD_LIB_PATH=str(ROOT / "mods"))
    sources = [
        ("tests/stdlib-raptor/contracts.n",
         "Raptor deque, stealing, serial, nested waits, producers and drain passed"),
        ("examples/task-parallel/task-parallel.n",
         "Raptor tasks: sum of squares = 204"),
    ]
    suffix = ".exe" if os.name == "nt" else ""
    with tempfile.TemporaryDirectory(prefix="nerd-raptor-") as temporary:
        preload = None
        if sys.platform.startswith("linux"):
            preload = Path(temporary) / "start-failure.so"
            run(["clang", "-shared", "-fPIC", "-Wall", "-Wextra", "-Werror",
                 "tests/stdlib-raptor/start-failure.c", "-ldl", "-pthread",
                 "-o", str(preload)], env)
            sources.append(("tests/stdlib-raptor/start-failure.n",
                            "Raptor partial startup cleanup passed"))
        else:
            print("[SKIP] Partial native startup injection requires Linux")
        for release in (False, True):
            for index, (source, expected) in enumerate(sources):
                executable = Path(temporary) / (f"test-{index}-{release}" + suffix)
                output = executable.with_suffix(".c") if args.cgen else executable
                command = [str(args.nerd.resolve()), "build", source, "-o", str(output)]
                if release:
                    command.append("-r")
                if args.cgen:
                    command.extend(["--cgen", "--copts"])
                    options = run(command, env)
                    run(["clang", str(output), *shlex.split(options),
                         "-o", str(executable)], env)
                else:
                    run(command, env)
                # Repeat native contracts to exercise scheduling variation. Each
                # execution has a deadline and kills its process tree on failure.
                runtime_env = env
                if source.endswith("start-failure.n"):
                    runtime_env = dict(env, LD_PRELOAD=str(preload))
                for _ in range(16 if index == 0 else 1):
                    actual = run([str(executable)], runtime_env, 15)
                    if actual != expected:
                        raise AssertionError(f"Unexpected output: {actual!r}")
                print(f"{'CGen' if args.cgen else 'LLVM'} "
                      f"{'release' if release else 'debug'} {source}: passed")


if __name__ == "__main__":
    main()
