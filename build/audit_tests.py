#!/usr/bin/env python3
"""Inventory regression fixtures; optionally time exact duplicate runtime cases.

Does not modify the suite or delete fixtures. --measure uses the existing runner
and its normal generated-artifact cleanup, sequentially, with checkout modules.
"""
from __future__ import annotations

import argparse
from collections import Counter
from functools import cache
import hashlib
import json
from pathlib import Path
import platform
import statistics
import subprocess
import time

import test as runner
import test_cgen

ROOT = Path(__file__).resolve().parents[1]


def rel(path):
    return path.relative_to(ROOT).as_posix()


@cache
def parts(path):
    return runner.split_sections(path)


def command_metadata(path):
    p = parts(path)
    return {
        "mode": p[3].strip() if len(p) > 3 else "delete",
        "flags": p[4].strip() if len(p) > 4 else "",
        "command": (p[5].strip() if len(p) > 5 else "") or "run",
        "stderr": p[6] if len(p) > 6 else "",
    }


def inventory():
    cases = runner.collect()
    counts = Counter(kind for kind, _ in cases)
    skips = Counter(kind for kind, p in cases
                    if runner.case_platforms(p)
                    and runner.current_platform() not in runner.case_platforms(p))
    languages = [p for k, p in cases if k == "language"]
    commands = [p for k, p in cases if k == "commands"]
    matches = []
    for cmd in commands:
        cp = parts(cmd)
        for lang in languages:
            lp = parts(lang)
            # Conservative: only ignore boundary whitespace, never comments,
            # declarations, interior whitespace, or expected output differences.
            if cp[0].strip() != lp[0].strip():
                continue
            metadata = command_metadata(cmd)
            same_oracle = (cp[1].strip() == lp[1].strip()
                           and cp[2].rstrip("\n") == lp[2].rstrip("\n"))
            matches.append({
                "command": rel(cmd), "language": rel(lang),
                "same_exit_stdout": same_oracle,
                "same_platforms": runner.case_platforms(cmd) == runner.case_platforms(lang),
                "metadata": metadata,
                "runtime_consolidation_candidate": (
                    same_oracle and metadata == {
                        "mode": "delete", "flags": "", "command": "run", "stderr": ""}
                    and runner.case_platforms(cmd) == runner.case_platforms(lang)
                    and len(lp) <= 5),
                "in_command_differential_list": cmd.stem in test_cgen.COMMANDS,
            })
    differential = languages + [ROOT / "tests/commands" / (n + ".cmd") for n in test_cgen.COMMANDS]
    differential += sorted((ROOT / "tests/cgen").glob("*.n"))
    return {
        "revision": subprocess.check_output(["git", "rev-parse", "HEAD"], cwd=ROOT, text=True).strip(),
        "platform": runner.current_platform(),
        "runner_fixtures": dict(sorted(counts.items())),
        "platform_skips": dict(sorted(skips.items())),
        "command_commands": dict(sorted(Counter(command_metadata(p)["command"] for p in commands).items())),
        "command_modes": dict(sorted(Counter(command_metadata(p)["mode"] for p in commands).items())),
        "error_source_expected_pairs": sum(len(parts(p)) // 2 for k, p in cases if k == "errors"),
        "commands_nonzero_exit_without_text_oracle": [
            rel(p) for p in commands if int(parts(p)[1].strip() or "0") != 0
            and not parts(p)[2].strip() and not command_metadata(p)["stderr"].strip()],
        "language_snapshot_sections": {
            "hir_nonempty": sum(bool(parts(p)[3].strip()) for p in languages),
            "llvm_nonempty": sum(bool(parts(p)[4].strip()) for p in languages),
        },
        "differential": {
            "language": len(languages), "selected_commands": len(test_cgen.COMMANDS),
            "cgen_only": len(list((ROOT / "tests/cgen").glob("*.n"))),
            "total": len(differential),
            "platform_skips": sum(bool(runner.case_platforms(p)) and runner.current_platform() not in runner.case_platforms(p) for p in differential),
        },
        "exact_source_matches": matches,
    }


def measure(report, compiler, repeats, limit):
    runner.NERD = compiler.resolve()
    if not runner.NERD.is_file():
        raise ValueError(f"Compiler does not exist: {runner.NERD}")
    result = {
        "compiler": str(runner.NERD),
        "compiler_sha256": hashlib.sha256(runner.NERD.read_bytes()).hexdigest(),
        "host": platform.platform(), "repeats": repeats,
        "method": "Sequential existing harness; one excluded warmup per fixture; seconds wall clock including cleanup. No full-suite timing.",
        "fixtures": [],
    }
    eligible = [m for m in report["exact_source_matches"] if m["runtime_consolidation_candidate"]]
    eligible = [m for m in eligible if not runner.case_platforms(ROOT / m["command"])
                or runner.current_platform() in runner.case_platforms(ROOT / m["command"])]
    for match in eligible[:limit]:
        for kind, function in (("command", runner.test_command), ("language", runner.test_language)):
            path = ROOT / match[kind]
            elapsed = []
            for iteration in range(repeats + 1):
                start = time.perf_counter()
                failures = function(path)
                duration = time.perf_counter() - start
                if failures:
                    raise RuntimeError("\n".join(f.message for f in failures))
                if iteration:
                    elapsed.append(duration)
            result["fixtures"].append({"path": rel(path), "seconds": elapsed,
                                       "median_seconds": statistics.median(elapsed)})
    report["measurement"] = result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", type=Path, help="Write JSON instead of stdout")
    parser.add_argument("--measure", type=Path, metavar="COMPILER", help="Opt in to sequential timing")
    parser.add_argument("--repeats", type=int, default=3)
    parser.add_argument("--limit", type=int, default=5, help="Maximum exact-match pairs to time")
    args = parser.parse_args()
    if args.repeats < 1 or args.limit < 1:
        parser.error("--repeats and --limit must be positive")
    report = inventory()
    if args.measure:
        measure(report, args.measure, args.repeats, args.limit)
    output = json.dumps(report, indent=2) + "\n"
    if args.output:
        args.output.write_text(output, encoding="utf-8", newline="\n")
    else:
        print(output, end="")


if __name__ == "__main__":
    main()
