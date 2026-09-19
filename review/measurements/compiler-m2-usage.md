# M2: skip irrelevant usage-inference scope scans

Measured 2026-09-20 on Linux x86-64, Ryzen 9 7950X, LLVM 22.1.8.
Before is `66a7d837`; after is that commit with this report's semantic-analysis
change. Both executables are release compiler builds. Compiler hashes, commands,
all samples, phase data and LLVM hashes are retained in the
[raw comparison](compiler-m2-usage/comparison.json.gz).

## Finding and change

`sema_seed_usage_context_local_types` iterates AST nodes until local/literal
constraints stabilize. Previously it checked every node for disabled top-level
`on` bodies, generic functions and generic implementations before determining
whether that node could contribute a constraint. Those scope predicates scan
AST ranges; calling them for every node caused substantial redundant work.

The loop now checks node kind first. Calls, assignments, equality/order
comparisons, compound-assignment-capable binary expressions and returns still
run the same scope predicates and inference code in the same order. All other
nodes bypass those scans. Scope checks are read-only, and skipped nodes never
reached constraint-processing code previously. The fixed-point condition,
local-declaration pass, constraint propagation and final seeding are unchanged.
No cache, retained allocation or inference-result reuse is introduced.

## Method

```sh
cp _bin/nerd /tmp/nerd-m2-usage-before  # preserve baseline before rebuilding
just build-release nerd --skip-mod-sync
python3 build/benchmark_compare.py --before /tmp/nerd-m2-usage-before --cpu 2 \
  --output /tmp/nerd-m2-usage.json
```

One warm-up, five alternating unprofiled samples and one separate profiled
sample per compiler/cell. Warm runs pinned to logical CPU 2, with no concurrent
tests or profiling. Frequency and SMT sibling load were not fixed. Both
compilers used identical inputs, checkout modules, output paths and LLVM tools.
All 196 invocations produced identical combined LLVM within their cell. The
runner builds but does not execute programs. End-to-end timings include direct
LLVM object generation and linking, which dilutes front-end gains on dungeon.

## End-to-end build medians

Milliseconds with observed five-sample ranges. Positive reduction means faster.
Target debug/release is separate from the release compiler build configuration.

| Workload | Target | Before median (range) | After median (range) | Reduction |
| --- | --- | ---: | ---: | ---: |
| tiny | debug | 19.71 (18.44–21.83) | 19.07 (18.73–23.73) | 3.3% |
| tiny | release | 24.02 (23.60–26.59) | 23.69 (23.11–29.38) | 1.4% |
| dungeon | debug | 1867.18 (1861.43–1890.31) | 1770.70 (1747.06–1781.92) | 5.2% |
| dungeon | release | 3174.67 (3137.66–3278.89) | 3060.29 (3051.57–3088.70) | 3.6% |
| pixels | debug | 394.85 (392.86–409.10) | 300.13 (298.92–302.31) | 24.0% |
| pixels | release | 495.40 (492.76–500.03) | 398.13 (396.02–413.26) | 19.6% |
| quill | debug | 71.78 (71.16–72.42) | 63.55 (63.19–63.85) | 11.5% |
| quill | release | 88.74 (88.61–89.95) | 81.64 (81.09–82.22) | 8.0% |
| wide | debug | 91.83 (91.35–92.20) | 86.82 (86.61–87.47) | 5.5% |
| wide | release | 137.21 (135.68–138.45) | 131.88 (130.85–132.93) | 3.9% |
| deep | debug | 91.72 (91.37–92.28) | 87.19 (86.35–87.34) | 4.9% |
| deep | release | 137.83 (137.51–138.74) | 133.08 (132.85–134.64) | 3.4% |
| large | debug | 210.70 (208.37–213.65) | 146.96 (146.47–147.70) | 30.3% |
| large | release | 182.98 (179.71–189.34) | 119.13 (117.68–121.48) | 34.9% |

## Checking and semantic phase evidence

A separate paired `check` experiment uses ten samples per compiler, alternating
order after one warm-up each, pinned to CPU 2. It uses the same tiny generated
source and dungeon/pixels examples as the build comparison. These end-to-end
check timings exclude HIR and external LLVM tools.
[Raw checking samples](compiler-m2-usage/check-paired.json.gz).

| Workload | Before median ms | After median ms | Reduction |
| --- | ---: | ---: | ---: |
| tiny | 1.108 | 0.905 | 18.3% |
| dungeon | 202.712 | 98.530 | 51.4% |
| pixels | 244.733 | 151.170 | 38.2% |

Semantic phase values below are sums across modules from individual profiled
debug-target builds, not medians. RSS is the median of five maximum per-process
high-water marks reported for the compiler and waited-for children, not their
concurrent sum or an isolated semantic-analysis footprint.

| Workload | Sema before/after ms | Peak process RSS before/after MiB |
| --- | ---: | ---: |
| tiny | 0.57 / 0.36 | 50.06 / 50.29 |
| dungeon | 200.42 / 94.63 | 73.04 / 73.67 |
| pixels | 243.90 / 146.50 | 60.69 / 60.69 |
| quill | 16.51 / 9.34 | 54.14 / 54.14 |
| wide | 11.24 / 6.46 | 54.75 / 54.79 |
| deep | 11.41 / 6.99 | 54.77 / 54.75 |
| large | 128.98 / 65.51 | 54.43 / 54.40 |

## Remaining hotspot

A separate CPU sample after timing ran ten checks each of dungeon and pixels,
serially on CPU 2:

```sh
DEBUGINFOD_URLS= NERD_LIB_PATH=/home/matt/nerd/mods \
  perf record -q -F 999 --call-graph dwarf -o /tmp/nerd-m2-usage-perf.data -- \
  taskset -c 2 bash -c 'for i in {1..10}; do _bin/nerd check examples/dungeon/dungeon.n; _bin/nerd check examples/pixels/pixels.n; done'
DEBUGINFOD_URLS= perf report -i /tmp/nerd-m2-usage-perf.data --stdio \
  --no-children --sort symbol --percent-limit 1 -g none
```

The [flat sample report](compiler-m2-usage/perf-check.txt) shows declaration
collection at 56.79% of sampled cycles, usage-context seeding at 14.81%, and
top-level import collection at 6.02%. These are combined workload samples, not
per-module timings or directly comparable percentages against earlier profiles.
The result supports investigating `sema_collect_decls_in_range` next, before
attempting a more complicated usage-inference worklist. Remaining scope scans
can still be quadratic; this change reduces their callers, not their complexity.

## Correctness

Existing regression fixtures cover usage inference, explicit-type conflicts,
nominal/plex constraints, disabled top-level bodies and generic code. The full
suite compares their outputs and diagnostics. All benchmark LLVM outputs are
byte-identical; no new test duplicates the node-kind predicate itself.

This completes another measured M2 improvement. M3 ownership preparation and
multicore scheduling remain ahead.

Validation on Linux: `just test` passed with 1,117 compiler tests and nine skips,
plus profiling, direct-toolchain and installed-compiler checks and 279 C
compatibility fixtures at both optimization levels. CodeLLDB stepping passed.
Native Windows and macOS validation remains outstanding.
