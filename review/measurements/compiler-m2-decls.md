# M2: avoid irrelevant declaration scope scans

Measured 2026-09-20 on Linux x86-64, Ryzen 9 7950X, LLVM 22.1.8.
Before is `a9fa67d4`; after is that commit plus this report's declaration
collection change. Both are release compiler builds.
[Raw comparison](compiler-m2-decls/comparison.json.gz) retains compiler hashes,
commands, all samples, phase data and LLVM output hashes.

## Change

Declaration collection now rejects irrelevant AST kinds before scanning for
enclosing top-level conditional bodies. Only implementations, conditional
blocks, bindings, variables and FFI definitions reach that scan. The original
recursive traversal and declaration/diagnostic order are retained.

The FFI wrapper search still examines bindings in order and unwraps annotations,
but now checks whether a binding refers to the exact FFI node before scanning
whether it lies inside a function. Unrelated local declarations no longer
trigger repeated whole-AST function-scope queries. Parser-produced binding
payloads are valid AST nodes for both local and top-level bindings. The same
first matching non-local wrapper is selected. No indexes, caches or retained
allocations are added; the remaining scans retain their original complexity.

## Method

```sh
cp _bin/nerd /tmp/nerd-m2-decls-before  # baseline before rebuilding
just build-release nerd --skip-mod-sync
python3 build/benchmark_compare.py --before /tmp/nerd-m2-decls-before --cpu 2 \
  --output /tmp/nerd-m2-decls.json
```

One warm-up, five alternating unprofiled samples and one separate profiled
sample per compiler/cell. Warm local runs pinned to CPU 2; no concurrent tests
or sampling. Frequency and SMT sibling activity were not fixed. Both compilers
used identical inputs, checkout modules, output paths and LLVM tools. All 196
invocations produced byte-identical combined LLVM within their cell. The runner
builds but does not run generated programs. End-to-end timings include direct
LLVM object generation and linking. Debug/release below describes the target,
independently of the release compiler build.

## End-to-end build medians

Milliseconds, five-sample ranges; positive reduction means faster.

| Workload | Target | Before median (range) | After median (range) | Reduction |
| --- | --- | ---: | ---: | ---: |
| tiny | debug | 21.62 (19.25–27.00) | 21.12 (18.90–22.98) | 2.3% |
| tiny | release | 24.04 (23.91–25.15) | 24.44 (23.80–28.90) | -1.7% |
| dungeon | debug | 1752.08 (1745.06–1783.69) | 1710.77 (1701.98–1720.78) | 2.4% |
| dungeon | release | 3074.95 (3019.68–3126.53) | 2984.84 (2952.92–3037.95) | 2.9% |
| pixels | debug | 300.32 (297.69–305.77) | 217.52 (214.40–221.94) | 27.6% |
| pixels | release | 397.26 (392.08–412.60) | 312.61 (306.12–320.58) | 21.3% |
| quill | debug | 63.41 (63.24–64.08) | 60.66 (60.06–61.38) | 4.3% |
| quill | release | 80.34 (80.17–81.20) | 77.45 (77.07–78.55) | 3.6% |
| wide | debug | 84.84 (83.75–86.19) | 84.43 (82.43–85.36) | 0.5% |
| wide | release | 127.14 (126.26–128.32) | 125.55 (125.15–127.54) | 1.3% |
| deep | debug | 84.43 (83.67–85.14) | 82.70 (82.19–86.52) | 2.1% |
| deep | release | 128.74 (126.82–129.25) | 126.84 (126.01–128.11) | 1.5% |
| large | debug | 144.51 (143.87–148.77) | 124.79 (123.40–127.57) | 13.6% |
| large | release | 116.02 (114.63–119.40) | 95.92 (94.43–101.08) | 17.3% |

Pixels builds improve 27.6% debug and 21.3% release. Tiny and wide/deep changes
are small and should not be interpreted as robust workload-wide speedups.

## Checking and phase measurements

A separate ten-sample alternating `check` comparison follows one warm-up per
compiler on CPU 2. The tiny input comes from the same synthetic generator;
dungeon and pixels use the checkout examples. These times exclude HIR and LLVM
tools. [Raw check samples](compiler-m2-decls/check-paired.json.gz).

| Workload | Before median ms | After median ms | Reduction |
| --- | ---: | ---: | ---: |
| tiny | 0.902 | 0.778 | 13.8% |
| dungeon | 97.693 | 63.048 | 35.5% |
| pixels | 150.113 | 64.057 | 57.3% |

Semantic phase values below are sums across modules from a single profiled
debug build, not medians. RSS is the median maximum per-process high-water mark
for the compiler and waited-for children, not the simultaneous tree total or
isolated semantic memory. No material RSS regression appears in these samples.

| Workload | Sema before/after ms | Peak process RSS before/after MiB |
| --- | ---: | ---: |
| tiny | 0.34 / 0.24 | 50.32 / 50.16 |
| dungeon | 94.60 / 59.27 | 72.16 / 73.57 |
| pixels | 144.11 / 59.24 | 60.61 / 60.62 |
| quill | 8.93 / 6.17 | 54.23 / 54.36 |
| wide | 6.41 / 5.00 | 54.75 / 54.81 |
| deep | 6.66 / 5.30 | 54.73 / 54.74 |
| large | 64.79 / 44.72 | 54.35 / 54.34 |

## Remaining work and M2 completion

A separate CPU sample ran ten checks each of dungeon and pixels serially on
CPU 2, using `perf record -F 999 --call-graph dwarf` with `NERD_LIB_PATH` pointing
to checkout modules. The [flat sample report](compiler-m2-decls/perf-check.txt)
puts declaration collection at 12.15% of cycles, compared with 56.79% in the
previous change's combined-workload sample. Usage-context inference is now
29.76%, and top-level import collection 11.92%. Percentages are shares of the
remaining work, not direct timing ratios or evidence those other functions
became slower. Reproduce using the command in the
[previous report](compiler-m2-usage.md#remaining-hotspot), substituting the
current compiler and a fresh perf output path.

These results complete the planned M2 serial improvements on Linux: combiner
scratch reuse, function-name and source-line indexes, usage-context investigation
and filtering, and declaration filtering. Further scope indexes or worklists
remain possible, but are not prerequisites for the next milestone. M3 now
prepares task-owned diagnostics, safe allocation bookkeeping and result lifetimes
for parallel LLVM module rendering. No scheduler or concurrent compilation has
been introduced yet.

## Correctness

Existing regressions cover conditional declarations/assertions, FFI bindings,
annotations, methods/traits, local declarations and duplicate-binding diagnostics.
The full suite verifies their outputs; the LLVM matrix verifies byte identity.
The change reorders read-only predicates without changing declaration traversal.

Validation on Linux: `just test` passed with 1,117 compiler tests and nine skips,
plus profiling, direct-toolchain and installed-compiler checks and 279 C
compatibility fixtures at both optimization levels. CodeLLDB stepping passed.
Native Windows and macOS validation remains outstanding.
