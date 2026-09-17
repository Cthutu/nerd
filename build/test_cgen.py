#!/usr/bin/env python3
"""Compile checked HIR to C and compare its execution with the LLVM backend."""
import argparse
import os
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
SMOKE = ("001-number", "006-global-vars", "019-interpolated-strings", "026-local-function-forward-ref",
         "027-on-bool", "044-for-break-again", "061-on-structural-patterns", "063-enum-unit-variants",
         "065-enum-payloads", "099-dynamic-arrays", "113-ffi-blocks", "123-mutual-pointer-plex-types",
         "152-on-break-for-expression", "183-for-in-fixed-array", "198-loop-dynamic-array-count")


def run(args, env, *, check=True):
    result = subprocess.run([str(a) for a in args], env=env, cwd=ROOT, capture_output=True, timeout=30)
    if check and result.returncode:
        raise AssertionError(f"{' '.join(map(str, args))}\n{result.stderr.decode(errors='replace')}")
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--nerd", type=Path, default=ROOT / "_bin/nerd-debug")
    parser.add_argument("--all", action="store_true", help="Exercise every language fixture")
    options = parser.parse_args()
    env = {**os.environ, "NERD_LIB_PATH": os.pathsep.join((str(ROOT / "tests/mods"), str(ROOT / "mods")))}
    fixtures = sorted((ROOT / "tests/language").glob("*.t"))
    if not options.all:
        fixtures = [f for f in fixtures if f.stem in SMOKE]
    (ROOT / "_tmp").mkdir(exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="cgen-", dir=ROOT / "_tmp") as directory:
        tmp = Path(directory)
        for fixture in fixtures:
            source = tmp / (fixture.stem + ".n")
            source.write_text(fixture.read_text().split("\n¬\n")[0])
            output = tmp / "program"
            run([options.nerd, "build", "-o", output, source], env)
            expected = run([output], env, check=False)
            output.unlink()
            run([options.nerd, "build", "--genc", "-o", output, source], env)
            assert not output.exists(), "C generation must not create a binary"
            generated = output.with_suffix(".c")
            assert generated.is_file(), "C output must replace the binary extension"
            run(["clang", "-std=gnu11", "-fwrapv", "-w", generated, "-o", output, "-lm"], env)
            actual = run([output], env, check=False)
            assert (actual.returncode, actual.stdout, actual.stderr) == (expected.returncode, expected.stdout, expected.stderr), (
                f"{fixture.name}: C and LLVM behaviour differs\nC: {actual}\nLLVM: {expected}")
            print(f"[PASS] C: {fixture.name}")
        for flags in (("--llvm",), ("--obj",), ("--lib",), ("--dll",)):
            result = run([options.nerd, "build", "--genc", *flags, "main :: fn () {}"], env, check=False)
            assert result.returncode, f"--genc must reject {flags}"
    print(f"C generation: {len(fixtures)} differential fixtures and CLI conflicts passed")


if __name__ == "__main__":
    main()
