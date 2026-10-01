#!/usr/bin/env python3
"""Bounded native tests for the thread/synchronisation foundation."""
import argparse
import os
import shlex
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]


def run(command, env, timeout=60):
    process = subprocess.Popen(command, cwd=ROOT, env=env,
                               stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                               text=True, encoding="utf-8", errors="replace",
                               start_new_session=os.name != "nt")
    try:
        out, err = process.communicate(timeout=timeout)
    except subprocess.TimeoutExpired:
        if os.name == "nt":
            subprocess.run(["taskkill", "/PID", str(process.pid), "/T", "/F"],
                           capture_output=True, timeout=10)
        else:
            import signal
            os.killpg(process.pid, signal.SIGKILL)
        process.kill()
        process.communicate()
        raise AssertionError(f"Timed out: {command}")
    if process.returncode or err:
        raise AssertionError(f"Failed {command}: {process.returncode}\n{out}\n{err}")
    return out.strip()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--nerd", type=Path,
                        default=ROOT / "_bin" / ("nerd-debug.exe" if os.name == "nt" else "nerd-debug"))
    parser.add_argument("--cgen", action="store_true",
                        help="Explicitly test optional generated C using native Clang")
    args = parser.parse_args()
    env = dict(os.environ, NERD_LIB_PATH=str(ROOT / "mods"))
    suffix = ".exe" if os.name == "nt" else ""
    with tempfile.TemporaryDirectory(prefix="nerd-thread-sync-") as temporary:
        output = Path(temporary)
        native = output / ("layout" + suffix)
        run(["clang", str(ROOT / "tests/stdlib-thread-sync/native-layout.c"),
             "-o", str(native)], env)
        run([str(native)], env, 10)
        print("Native thread/sync layouts passed")
        sources = [
            (ROOT / "tests/stdlib-thread-sync/temp-arena-alias-reproducer.n",
             "current"),
            (ROOT / "tests/stdlib-thread-sync/lifecycle.n",
             "Thread lifecycle and predicate waits passed"),
            (ROOT / "examples/thread-pipeline/thread-pipeline.n",
             "Consumed 100 values; total = 5050"),
        ]
        for release in (False, True):
            for index, (source, expected) in enumerate(sources):
                executable = output / (f"test-{index}-{release}" + suffix)
                command = [str(args.nerd.resolve()), "build", str(source), "-o",
                           str(executable.with_suffix(".c") if args.cgen else executable)]
                if release:
                    command.append("-r")
                if args.cgen:
                    command.extend(["--cgen", "--copts"])
                    copts = run(command, env)
                    run(["clang", str(executable.with_suffix(".c")),
                         *shlex.split(copts), "-o", str(executable)], env)
                else:
                    run(command, env)
                actual = run([str(executable)], env, 15)
                if actual != expected:
                    raise AssertionError(f"Unexpected output: {actual!r}")
                print(f"{'CGen' if args.cgen else 'LLVM'} {'release' if release else 'debug'} {source.name}: passed")


if __name__ == "__main__":
    main()
