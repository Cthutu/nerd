#!/usr/bin/env python3
"""Opt-in `on` batching experiment. Leaves production fixtures/suites unchanged."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import statistics
import subprocess
import tempfile
import time

import test as runner

ROOT = Path(__file__).resolve().parents[1]
CASES = (
    "062-run-llvm-nested-on-branch",
    "129-run-on-expression-return-branches",
    "252-run-nested-partial-on-statement",
)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--nerd", required=True, type=Path)
    parser.add_argument("--output", required=True, type=Path)
    parser.add_argument("--repeats", type=int, default=3)
    args = parser.parse_args()
    if args.repeats < 1:
        parser.error("--repeats must be positive")
    nerd = args.nerd.resolve()
    env = runner.env()
    suffix = ".exe" if os.name == "nt" else ""
    originals = [runner.split_sections(ROOT / "tests/commands" / (n + ".cmd")) for n in CASES]
    declarations, calls, expected = [], [], []
    for index, (name, sections) in enumerate(zip(CASES, originals)):
        source = sections[0]
        assert source.count("main :: fn") == 1
        declarations.append(source.replace("main :: fn", f"case_{index} :: fn"))
        calls.append(f'    prn("begin {name}")\n'
                     f'    result_{index} := case_{index}()\n'
                     f'    assert result_{index} == {int(sections[1])}\n'
                     f'    prn("pass {name}")')
        expected.append(f"begin {name}\n" + sections[2].rstrip("\n")
                        + ("\n" if sections[2].rstrip("\n") else "") + f"pass {name}\n")
    expected.append("completed 3 cases\n")
    expected_output = "".join(expected)
    body = "\n\n".join(declarations)
    tail = '\n    prn("completed 3 cases")\n}\n'
    combined = body + "\n\nmain :: fn () {\n" + "\n".join(calls) + tail

    def run(command):
        return subprocess.run([str(a) for a in command], cwd=ROOT, env=env,
                              capture_output=True, text=True, encoding="utf-8", timeout=60)

    def build_run(source, executable):
        build = run([nerd, "build", source, "-o", executable])
        assert build.returncode == 0, build.stderr
        return run([executable])

    def verify(result, exit_code, stdout):
        assert runner.normalized_returncode(result.returncode) == exit_code, result.stderr
        assert result.stdout.rstrip("\n") == stdout.rstrip("\n"), (result.stdout, stdout)
        assert not result.stderr, result.stderr

    with tempfile.TemporaryDirectory(prefix="nerd-audit-on-") as temporary:
        work = Path(temporary)
        paths = []
        for index, sections in enumerate(originals):
            source = work / f"separate-{index}.n"
            source.write_text(sections[0], encoding="utf-8")
            paths.append(source)
        batch = work / "on-batch.n"
        batch.write_text(combined, encoding="utf-8")
        samples = {"separate": [], "batch": []}
        for iteration in range(args.repeats + 1):
            start = time.perf_counter()
            for index, (source, sections) in enumerate(zip(paths, originals)):
                verify(build_run(source, work / (f"separate-{index}" + suffix)),
                       int(sections[1]), sections[2])
            separate_seconds = time.perf_counter() - start
            start = time.perf_counter()
            verify(build_run(batch, work / ("batch" + suffix)), 0, expected_output)
            batch_seconds = time.perf_counter() - start
            if iteration:
                samples["separate"].append(separate_seconds)
                samples["batch"].append(batch_seconds)

        generated = work / "batch.c"
        cgen = run([nerd, "build", "--cgen", batch, "-o", generated])
        assert cgen.returncode == 0, cgen.stderr
        for optimisation in ("-O0", "-O2"):
            output = work / ("batch-c" + suffix)
            built = run(["clang", "-std=gnu11", "-Werror", optimisation,
                         generated, "-o", output, *([] if os.name == "nt" else ["-lm"])])
            assert built.returncode == 0, built.stderr
            verify(run([output]), 0, expected_output)

        # Prove the golden transcript catches a silently omitted case, even
        # when the program returns success and prints the completion footer.
        omitted = work / "omitted.n"
        omitted.write_text(body + "\n\nmain :: fn () {\n" + "\n".join(calls[::2]) + tail,
                           encoding="utf-8")
        result = build_run(omitted, work / ("omitted" + suffix))
        assert result.returncode == 0
        assert result.stdout != expected_output, "Missing case unexpectedly passed the output oracle"

    report = {
        "revision": subprocess.check_output(["git", "rev-parse", "HEAD"], cwd=ROOT, text=True).strip(),
        "compiler_sha256": hashlib.sha256(nerd.read_bytes()).hexdigest(),
        "platform": runner.current_platform(), "repeats": args.repeats,
        "cases": list(CASES), "seconds": samples,
        "median_seconds": {k: statistics.median(v) for k, v in samples.items()},
        "method": "Sequential LLVM build+execution; separate cases then batch; one excluded warmup iteration. C generation/O0/O2 and omitted-case mutation verified but excluded from timing.",
        "verified": ["original golden outputs/exits", "batch golden output/exit",
                     "C -O0 and -O2 golden output/exit", "omitted-case mutation rejected"],
        "combined_source": combined, "expected_stdout": expected_output,
    }
    args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8", newline="\n")
    print(json.dumps(report["median_seconds"], indent=2))


if __name__ == "__main__":
    main()
