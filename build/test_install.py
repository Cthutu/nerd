#!/usr/bin/env python3
import argparse
import os
import pathlib
import re
import shutil
import subprocess
import sys
import tempfile


ROOT = pathlib.Path(__file__).resolve().parents[1]
FIXTURES = ROOT / "tests" / "install"
ANSI_RE = re.compile(r"\x1b\[[0-9;]*m")


def run(cmd: list[str], cwd: pathlib.Path, env: dict[str, str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        cmd,
        cwd=cwd,
        env=env,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )


def check(proc: subprocess.CompletedProcess[str], label: str, expected_stdout: str | None = None) -> None:
    if proc.returncode != 0:
        raise AssertionError(
            f"{label} failed with {proc.returncode}\nstdout:\n{proc.stdout}\nstderr:\n{proc.stderr}"
        )
    actual_stdout = ANSI_RE.sub("", proc.stdout)
    if expected_stdout is not None and actual_stdout != expected_stdout:
        raise AssertionError(
            f"{label} stdout mismatch\nexpected:\n{expected_stdout!r}\nactual:\n{actual_stdout!r}"
        )


def main() -> int:
    parser = argparse.ArgumentParser(description="Run installed nerd smoke tests")
    parser.add_argument(
        "--nerd",
        default=os.environ.get("NERD_BIN", shutil.which("nerd") or "nerd"),
        help="nerd executable to test",
    )
    args = parser.parse_args()

    nerd = pathlib.Path(args.nerd)
    if nerd.is_absolute():
        nerd_cmd = str(nerd)
    elif (ROOT / nerd).exists():
        nerd_cmd = str((ROOT / nerd).resolve())
    else:
        resolved = shutil.which(str(nerd))
        if resolved is None:
            raise SystemExit(f"nerd executable not found: {nerd}")
        nerd_cmd = resolved

    env = os.environ.copy()
    env.setdefault("NERD_LIB_PATH", str(ROOT / "mods"))

    with tempfile.TemporaryDirectory(prefix="nerd-install-smoke-") as temp_name:
        temp = pathlib.Path(temp_name)
        for fixture in FIXTURES.glob("*.n"):
            shutil.copy2(fixture, temp / fixture.name)

        build_proc = run(
            [
                nerd_cmd,
                "build",
                "--hir",
                "--llvm",
                "build_smoke.n",
                "--output",
                "build_smoke",
            ],
            temp,
            env,
        )
        check(build_proc, "build --hir --llvm")
        if not (temp / "build_smoke").exists():
            raise AssertionError("build did not produce executable")
        if not any(temp.glob("_build_smoke*.hir")):
            raise AssertionError("build --hir did not produce a HIR sidecar")
        if not any(temp.glob("_build_smoke*.ll")):
            raise AssertionError("build --llvm did not produce an LLVM sidecar")

        run_proc = run([nerd_cmd, "run", "run_smoke.n"], temp, env)
        check(run_proc, "run", "installed smoke\n")

        test_proc = run([nerd_cmd, "test", "source_tests.n"], temp, env)
        check(test_proc, "test", "2 tests passed\n")

        format_proc = run(
            [nerd_cmd, "format", "--stdout", "format_smoke.n"], temp, env
        )
        check(format_proc, "format --stdout", "main :: fn () => 1\n")

    print("installed compiler smoke tests passed")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except AssertionError as exc:
        print(str(exc), file=sys.stderr)
        raise SystemExit(1)
