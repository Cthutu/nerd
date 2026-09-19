# M2: LLVM function-name conflict index

Measured 2026-09-19 on Linux x86-64, Ryzen 9 7950X, LLVM 22.1.8.
Before is `ce78311c` (including combiner scratch reuse); after is that commit
plus the function-name index in this report's commit. Both compilers are release
builds. [Raw samples and compiler hashes](compiler-m2-function-names/comparison.json.gz)
include all commands, phase records, affinity and output hashes.

## Change

Previously every conflict query scanned all functions in all modules, resolving
each function's first HIR binding again. LLVM emission now prepares one map from
name spelling to occurrence count, capped at two. Queries retain canonical
binding lookup but replace the whole-program scan with a map lookup.

The map counts functions, not aliases. Unbound functions are excluded; repeated
spellings within one module still count. The table is shared by module and
sidecar renders and freed after emission, including normal failure returns.
It is not built for checking or C emission. Standalone HIR rendering without
an index retains the original scan. This introduces no global mutable cache.
Construction still scans bindings once per function; it is not a fully linear
function-to-binding index. That remaining work can be measured separately.

## Method

```sh
cp _bin/nerd /tmp/nerd-m2-index-before  # before rebuilding the changed compiler
just build-release nerd --skip-mod-sync
python3 build/benchmark_compare.py --before /tmp/nerd-m2-index-before --cpu 2 \
  --output /tmp/nerd-m2-index.json
```

One warm-up and five unprofiled samples per compiler/cell, alternating order,
plus one separate profiled sample each. All processes pinned to logical CPU 2,
with no concurrent tests or sampling. Warm local runs; frequency and SMT sibling
activity were not fixed. Both compilers used identical sources, checkout modules,
output paths and LLVM tools. Combined LLVM was retained and checked byte-for-byte
by SHA-256 on all 196 invocations; every comparison passed. The benchmark builds
but does not execute programs. End-to-end timings include index construction,
retained LLVM writing, object generation and linking through direct LLVM tools.

## End-to-end results

Milliseconds; five-sample medians with observed ranges. Positive reduction
means faster. Tiny changes are within ordinary timing variation.

| Workload | Target | Before median (range) | After median (range) | Reduction |
| --- | --- | ---: | ---: | ---: |
| tiny | debug | 21.34 (21.08–22.78) | 21.37 (20.70–22.63) | -0.1% |
| tiny | release | 26.54 (25.45–27.41) | 26.65 (25.87–27.74) | -0.4% |
| dungeon | debug | 2099.16 (2080.46–2113.76) | 2031.64 (2009.83–2037.00) | 3.2% |
| dungeon | release | 3392.54 (3371.06–3443.41) | 3354.40 (3302.48–3378.54) | 1.1% |
| pixels | debug | 684.30 (679.07–687.63) | 493.37 (490.22–498.05) | 27.9% |
| pixels | release | 720.95 (715.34–731.07) | 575.92 (572.27–580.01) | 20.1% |
| quill | debug | 86.44 (85.72–88.15) | 77.93 (77.62–79.75) | 9.8% |
| quill | release | 100.53 (99.94–100.77) | 94.27 (93.70–94.43) | 6.2% |
| wide | debug | 122.15 (121.38–122.86) | 95.51 (94.81–97.02) | 21.8% |
| wide | release | 160.67 (159.29–161.27) | 140.64 (139.77–141.27) | 12.5% |
| deep | debug | 122.57 (121.45–124.72) | 97.75 (94.92–97.93) | 20.2% |
| deep | release | 163.46 (160.89–165.23) | 144.13 (141.74–145.00) | 11.8% |
| large | debug | 323.38 (318.62–334.24) | 237.49 (235.54–238.73) | 26.6% |
| large | release | 283.62 (272.10–286.30) | 199.66 (197.86–200.02) | 29.6% |

## Phase and memory evidence

Render times are sums across modules from one profiled invocation per compiler,
not timing medians. Index construction happens before these render probes and
is included in the end-to-end timings above. RSS is the median of five maximum
per-process high-water marks for the compiler and waited-for children; it is
not simultaneous process-tree memory or an isolated index footprint.

| Workload | Target | LLVM render before/after ms | Peak process RSS before/after MiB |
| --- | --- | ---: | ---: |
| tiny | debug | 0.80 / 0.75 | 50.13 / 50.38 |
| tiny | release | 0.35 / 0.34 | 47.59 / 47.62 |
| dungeon | debug | 122.95 / 58.10 | 73.09 / 73.45 |
| dungeon | release | 71.16 / 21.77 | 144.31 / 143.98 |
| pixels | debug | 256.14 / 66.81 | 60.67 / 60.64 |
| pixels | release | 162.45 / 17.29 | 66.04 / 65.97 |
| quill | debug | 17.19 / 9.10 | 54.16 / 54.22 |
| quill | release | 9.22 / 3.09 | 60.11 / 60.07 |
| wide | debug | 56.52 / 30.36 | 54.72 / 54.76 |
| wide | release | 34.62 / 14.59 | 50.41 / 50.34 |
| deep | debug | 59.51 / 29.48 | 54.76 / 54.75 |
| deep | release | 34.93 / 16.71 | 50.33 / 50.06 |
| large | debug | 127.57 / 36.94 | 54.39 / 54.34 |
| large | release | 109.96 / 15.14 | 47.65 / 47.58 |

Pixels debug builds improve 27.9%, with module LLVM rendering falling from
256.14 ms to 66.81 ms in the profiled samples. Large release builds improve
29.6%. Dungeon's end-to-end improvement is smaller because other phases dominate.
No material peak RSS regression appears in this run. The table retains one
entry per distinct canonical spelling for the duration of LLVM emission.

## Correctness and next step

The `llvm-function-names` internal test compares indexed answers with the scan
and explicit expected answers. It covers cross-module duplicate spellings with
different local handles, duplicates within a module, aliases, unbound functions
and map growth. All benchmark LLVM outputs are identical to the baseline.

This completes the second M2 slice. Source-line lookup indexing is next, followed
by investigation of semantic usage-context inference. Multicore scheduling still
requires the M3 ownership and diagnostic preparation.

Validation on Linux: `just test` passed with 1,116 compiler tests and nine skips,
plus profiling, direct-toolchain and installed-compiler checks and 279 C
compatibility fixtures at both optimization levels. The CodeLLDB stepping
check passed. Native Windows and macOS validation remains outstanding.
