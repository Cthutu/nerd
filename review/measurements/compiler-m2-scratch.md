# M2: LLVM combiner scratch reuse

Measured 2026-09-19 on Linux x86-64, Ryzen 9 7950X, LLVM 22.1.8.
Both compilers are release builds. Before is `54e93279`; after is that commit
with the scratch-arena change in this report's commit. Compiler hashes, all
samples and phase records are in [the raw comparison](compiler-m2-scratch/comparison.json.gz).

The combiner previously reserved, committed and released a scratch arena for
every emitted line. It now resets one arena per input module, freeing it when
that input finishes. Output and named metadata builders copy the rendered text
before the reset. Scratch commitment is bounded by the longest rendered line
in that input, rounded to allocator chunks; it is not retained across modules.

## Method

```sh
# Preserve the baseline release compiler before rebuilding the changed compiler.
cp _bin/nerd /tmp/nerd-m2-before
just build-release nerd --skip-mod-sync
python3 build/benchmark_compare.py --before /tmp/nerd-m2-before --cpu 2 \
  --output /tmp/nerd-m2-scratch.json
```

One warm-up and five unprofiled samples per compiler and cell, alternating
compiler order, followed by one separate profiled sample per compiler. All
processes were pinned to logical CPU 2; no competing test suite was running.
These are warm local runs, not cold-cache measurements. Affinity does not
control frequency or SMT sibling activity. The same inputs, output path,
checkout modules and LLVM tools were used for both compilers.

Every invocation retained combined LLVM and checked its SHA-256 against the
baseline. All 196 invocations produced identical LLVM bytes within their cell.
Generated programs were not executed by the benchmark. Total times include
writing retained combined LLVM, object generation and linking through direct
LLVM tooling. No Clang invocation is involved.

## End-to-end medians

Target configuration is independent of the release compiler build.

| Workload | Target | Before ms | After ms | Reduction |
| --- | --- | ---: | ---: | ---: |
| tiny | debug | 24.28 | 20.38 | 16.1% |
| tiny | release | 28.19 | 26.60 | 5.7% |
| dungeon | debug | 2294.36 | 2084.57 | 9.1% |
| dungeon | release | 3476.24 | 3338.84 | 4.0% |
| pixels | debug | 873.81 | 674.45 | 22.8% |
| pixels | release | 824.47 | 700.44 | 15.0% |
| quill | debug | 137.58 | 84.59 | 38.5% |
| quill | release | 130.52 | 99.05 | 24.1% |
| wide | debug | 198.66 | 120.33 | 39.4% |
| wide | release | 183.67 | 157.40 | 14.3% |
| deep | debug | 201.21 | 121.77 | 39.5% |
| deep | release | 184.22 | 157.04 | 14.8% |
| large | debug | 427.54 | 305.11 | 28.6% |
| large | release | 341.83 | 283.49 | 17.1% |

## Combiner and memory

Phase timings below are individual profiled samples, not timing medians.
Commitments count cumulative new arena commitments during combining, including
output storage; they are not peak live memory. Requested text sizes are unchanged.

| Workload | Target | Combine before/after ms | Commitments before/after MiB | Median peak process RSS before/after MiB |
| --- | --- | ---: | ---: | ---: |
| tiny | debug | 2.71 / 0.18 | 24.38 / 0.38 | 50.13 / 50.29 |
| tiny | release | 1.11 / 0.08 | 11.38 / 0.38 | 47.62 / 47.58 |
| dungeon | debug | 162.38 / 9.41 | 1381.12 / 2.00 | 73.91 / 73.53 |
| dungeon | release | 95.02 / 4.75 | 877.75 / 1.31 | 144.14 / 143.53 |
| pixels | debug | 167.73 / 10.62 | 1387.50 / 2.12 | 60.62 / 60.64 |
| pixels | release | 81.10 / 5.21 | 768.38 / 1.31 | 65.96 / 65.98 |
| quill | debug | 52.00 / 3.18 | 433.88 / 1.25 | 54.16 / 54.15 |
| quill | release | 27.93 / 1.86 | 265.44 / 1.06 | 60.07 / 60.06 |
| wide | debug | 80.14 / 4.93 | 680.06 / 1.94 | 54.76 / 54.76 |
| wide | release | 26.36 / 1.48 | 263.56 / 1.44 | 50.39 / 50.35 |
| deep | debug | 81.33 / 4.64 | 681.00 / 1.94 | 54.80 / 54.90 |
| deep | release | 27.49 / 1.50 | 264.50 / 1.44 | 50.42 / 50.42 |
| large | debug | 78.94 / 6.71 | 697.12 / 0.94 | 54.38 / 54.37 |
| large | release | 30.82 / 3.12 | 299.56 / 0.50 | 47.58 / 47.56 |

Peak RSS is the maximum per-process high-water mark reported for the compiler
and waited-for children, not their simultaneous sum. These samples show no
material RSS regression; they do not isolate the combiner's live footprint.

The allocation-churn reduction is substantial and deterministic. End-to-end
medians improve in every cell, with the largest proportional improvement in
deep debug (39.5%). Exact timing gains depend on workload and host conditions.
This completes the first M2 slice, not M2 or the scheduler milestones. The next
candidate is indexing LLVM function-name conflicts, followed by source-line
lookup and investigation of repeated semantic usage-context inference.

## Correctness

The combiner self-test covers scratch growth beyond the initial commitment,
short lines after growth, copied/rebased named metadata across two inputs, and
a final line without a newline. The comparison preserves byte-identical LLVM
for all seven workloads in both target modes. The full compiler suite and
CodeLLDB stepping check provide runtime and source-debugging coverage.

Validation on Linux: `just test` passed (1,115 compiler tests, nine skipped;
profiling, toolchain and installation checks; 279 C differential fixtures at
both optimization levels). `check_debugger_stepping.py` passed. Native Windows
and macOS validation remains outstanding.
