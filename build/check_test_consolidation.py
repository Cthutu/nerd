#!/usr/bin/env python3
"""Verify the first consolidation's mapping/oracles; optionally time changed work.

Uses historical fixtures from Git without restoring them into the active suite.
No default recipe changes; run explicitly after editing the migration map/batch.
"""
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
import test_cgen

ROOT = Path(__file__).resolve().parents[1]
MAP = ROOT / "docs/test-suite-consolidation-map.json"


def historical_text(revision, path):
    return subprocess.check_output(["git", "show", f"{revision}:{path}"],
                                   cwd=ROOT, text=True, encoding="utf-8")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--compiler", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--measure", action="store_true")
    parser.add_argument("--repeats", type=int, default=3)
    args = parser.parse_args()
    if args.repeats < 1:
        parser.error("--repeats must be positive")
    runner.NERD = args.compiler.resolve()
    assert runner.NERD.is_file()
    mapping = json.loads(MAP.read_text(encoding="utf-8"))
    baseline = mapping["baseline_revision"]
    batch = ROOT / mapping["feature_batch"]
    batch_parts = runner.split_sections(batch)
    originals = [m["original"] for m in mapping["duplicate_commands"] + mapping["scenarios"]]
    assert len(originals) == len(set(originals)) == 47
    for path in originals:
        assert not (ROOT / path).exists(), f"Original still in active suite: {path}"
    for path in mapping["preserved_cli_contracts"]:
        assert (ROOT / path).read_text(encoding="utf-8") == historical_text(baseline, path)
    for scenario in mapping["scenarios"]:
        marker = scenario["scenario"]
        assert batch_parts[2].count(f"begin {marker}\n") == 1
        assert batch_parts[2].count(f"pass {marker}\n") == 1
        assert Path(scenario["original"]).stem not in test_cgen.COMMANDS
    # The language collector automatically includes this fixture in C differential.
    assert batch in sorted((ROOT / "tests/language").glob("*.t"))
    samples = {"before_commands": [], "after_batch": []}
    (ROOT / "_tmp").mkdir(exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="consolidation-", dir=ROOT / "_tmp") as temporary:
        work = Path(temporary)
        paths = []
        for original in originals:
            path = work / Path(original).name
            path.write_text(historical_text(baseline, original), encoding="utf-8", newline="\n")
            paths.append(path)
        old_paths = dict(zip(originals, paths))
        for duplicate in mapping["duplicate_commands"]:
            previous = runner.split_sections(old_paths[duplicate["original"]])
            replacement = runner.split_sections(ROOT / duplicate["replacement"])
            assert previous[0].strip() == replacement[0].strip()
            assert previous[1].strip() == replacement[1].strip()
            assert previous[2].rstrip("\n") == replacement[2].rstrip("\n")
        for iteration in range(args.repeats + 1 if args.measure else 1):
            start = time.perf_counter()
            for path in paths:
                failures = runner.test_command(path)
                assert not failures, "\n".join(f.message for f in failures)
            before = time.perf_counter() - start
            start = time.perf_counter()
            failures = runner.test_language(batch)
            after = time.perf_counter() - start
            assert not failures, "\n".join(f.message for f in failures)
            if args.measure and iteration:
                samples["before_commands"].append(before)
                samples["after_batch"].append(after)
            print(f"PASS iteration {iteration}: old commands {before:.3f}s; batch {after:.3f}s", flush=True)

        # Golden stdout must reject an omitted scenario even if exit is success.
        omitted = work / "omitted.t"
        omitted_source = batch_parts[0].replace(
            '    prn("begin 252-run-nested-partial-on-statement")\n'
            '    assert case_252() == 0\n'
            '    prn("pass 252-run-nested-partial-on-statement")', "")
        assert omitted_source != batch_parts[0]
        omitted.write_text("\n¬\n".join([omitted_source, *batch_parts[1:]]),
                           encoding="utf-8", newline="\n")
        failures = runner.test_language(omitted)
        assert any("stdout mismatch" in f.message for f in failures)
        assert not any("exit mismatch" in f.message for f in failures)

        # Retaining labels/footer while returning the wrong value must also fail.
        wrong = work / "wrong-result.t"
        wrong_source = batch_parts[0].replace("    return result - 42", "    return result - 41")
        assert wrong_source != batch_parts[0]
        wrong.write_text("\n¬\n".join([wrong_source, *batch_parts[1:]]),
                         encoding="utf-8", newline="\n")
        failures = runner.test_language(wrong)
        assert any("exit mismatch" in f.message for f in failures)

    result = {
        "baseline_revision": baseline,
        "tested_revision": subprocess.check_output(["git", "rev-parse", "HEAD"], cwd=ROOT, text=True).strip(),
        "compiler": str(runner.NERD),
        "compiler_sha256": hashlib.sha256(runner.NERD.read_bytes()).hexdigest(),
        "platform": runner.current_platform(),
        "old_commands": len(originals), "batch_scenarios": len(mapping["scenarios"]),
        "verified": ["33 exact source/exit/stdout mappings", "dedicated CLI fixtures unchanged",
                     "47 original command fixtures", "batch golden output and return assertions",
                     "omitted scenario rejected despite successful exit", "wrong result rejected",
                     "batch in language differential discovery; old command entries removed"],
        "timings": samples,
        "median_seconds": {k: statistics.median(v) for k, v in samples.items() if v},
        "measurement_scope": "Only changed fixture work. Same fresh compiler and modules on both sides; original commands extracted to temporary files; batch runs through language harness with HIR/LLVM flags. Sequential, one excluded warmup when measuring. Retained alternative language fixtures are unchanged and outside timing. Not a full-gate speedup.",
    }
    args.output.write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8", newline="\n")
    print("PASS mapping, original/batch behaviour, omission and wrong-result checks")


if __name__ == "__main__":
    main()
