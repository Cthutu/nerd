# M2: source-line lookup index

Measured 2026-09-19 on Linux x86-64, Ryzen 9 7950X, LLVM 22.1.8.
Before is `3173a4b8`; after is that commit plus this report's source-line index
change. Both are release compiler builds. [Raw comparison](compiler-m2-lines/comparison.json.gz)
retains compiler hashes, all samples, commands, phase data and LLVM hashes.

## Change and ownership

Whole-program compilation now builds a table of source line starts after
semantic checking and before HIR generation. HIR source locations and LLVM
local-variable debug locations find the matching source buffer, then binary
search its line starts. Mapped fragments retain their existing source/offset
translation and have indexes for their original buffer views. Equal pointer and
length pairs share one index. Other buffers and standalone HIR without a
program table retain the scanner.

The program owns this immutable table and frees it before source snapshots.
Checking allocates no index. Lexer and module layouts are unchanged. Keeping
index storage outside those structures avoids the checking regressions observed
in early prototypes. Entries use 32-bit byte offsets; source buffers beyond
that range fall back to scanning. Storage is proportional to indexed line
count, with possible duplicate line data across combined and fragment views.
Buffer selection scans the small index table; it is not an all-source hash map.

## Method

```sh
cp _bin/nerd /tmp/nerd-m2-lines-before  # baseline, before rebuilding
just build-release nerd --skip-mod-sync
python3 build/benchmark_compare.py --before /tmp/nerd-m2-lines-before --cpu 2 \
  --output /tmp/nerd-m2-lines.json
```

One warm-up, five unprofiled samples and one separate profiled sample per
compiler/cell; unprofiled compiler order alternates. All processes pinned to
logical CPU 2, with no concurrent tests or sampling. Warm local runs; frequency
and SMT sibling activity were not fixed. Input and output paths, checkout
modules and LLVM tools were shared. All 196 invocations retained byte-identical
combined LLVM within their cell. Generated programs were not run by the runner.
End-to-end times include index construction, retained LLVM writing and direct
LLVM object generation/linking. Target debug/release is independent of the
release compiler build.

## End-to-end medians

Milliseconds with five-sample ranges. Positive reduction means faster.

| Workload | Target | Before median (range) | After median (range) | Reduction |
| --- | --- | ---: | ---: | ---: |
| tiny | debug | 22.60 (21.62–23.59) | 22.99 (20.88–24.56) | -1.7% |
| tiny | release | 29.30 (27.35–29.33) | 28.47 (26.98–28.95) | 2.8% |
| dungeon | debug | 2098.67 (2075.53–2153.63) | 2004.15 (1996.99–2022.75) | 4.5% |
| dungeon | release | 3417.02 (3362.90–3473.30) | 3338.34 (3302.70–3368.47) | 2.3% |
| pixels | debug | 497.62 (496.63–506.61) | 408.54 (407.16–415.27) | 17.9% |
| pixels | release | 573.03 (569.02–574.27) | 511.71 (503.21–516.29) | 10.7% |
| quill | debug | 78.57 (77.64–80.71) | 77.24 (76.04–77.64) | 1.7% |
| quill | release | 96.42 (95.44–97.45) | 93.22 (92.03–95.06) | 3.3% |
| wide | debug | 97.34 (96.91–104.69) | 96.94 (94.69–98.40) | 0.4% |
| wide | release | 141.55 (139.46–142.48) | 142.53 (139.77–144.42) | -0.7% |
| deep | debug | 97.59 (96.13–100.42) | 96.96 (94.72–97.37) | 0.7% |
| deep | release | 145.61 (139.87–147.60) | 143.91 (142.36–147.50) | 1.2% |
| large | debug | 236.24 (235.01–237.05) | 217.37 (216.90–220.93) | 8.0% |
| large | release | 199.24 (194.59–201.06) | 187.09 (185.74–187.79) | 6.1% |

Pixels improves 17.9% in debug and 10.7% in release; large improves 8.0% and
6.1%. Tiny and synthetic multi-module changes are mostly small enough to treat
as timing variation. No broad speedup claim is made for these small changes.

## Phase and memory observations

Phase values are sums across modules in one profiled run per compiler, not
medians. Index construction precedes HIR probes; it is included in end-to-end
times but excluded from the HIR column. RSS is the median maximum per-process
high-water mark for the compiler and waited-for children, not their concurrent
sum or the isolated index footprint.

| Workload | Target | HIR before/after ms | LLVM render before/after ms | Peak process RSS before/after MiB |
| --- | --- | ---: | ---: | ---: |
| tiny | debug | 0.15 / 0.07 | 0.75 / 0.72 | 50.30 / 50.33 |
| tiny | release | 0.15 / 0.05 | 0.34 / 0.34 | 47.61 / 47.56 |
| dungeon | debug | 76.44 / 1.95 | 58.80 / 48.48 | 73.27 / 73.33 |
| dungeon | release | 75.06 / 1.97 | 22.49 / 20.93 | 144.01 / 144.17 |
| pixels | debug | 68.07 / 1.86 | 67.53 / 42.32 | 60.71 / 60.62 |
| pixels | release | 69.71 / 2.11 | 17.77 / 17.83 | 65.91 / 66.00 |
| quill | debug | 3.20 / 0.53 | 9.83 / 8.08 | 54.17 / 54.14 |
| quill | release | 2.82 / 0.54 | 3.14 / 3.05 | 60.07 / 60.05 |
| wide | debug | 1.52 / 0.62 | 30.93 / 30.21 | 54.77 / 54.77 |
| wide | release | 1.49 / 0.61 | 14.83 / 15.03 | 50.29 / 50.29 |
| deep | debug | 1.53 / 0.59 | 29.53 / 29.59 | 54.74 / 54.77 |
| deep | release | 1.52 / 0.62 | 16.63 / 16.46 | 50.30 / 50.29 |
| large | debug | 14.68 / 1.96 | 37.01 / 28.41 | 54.38 / 54.36 |
| large | release | 14.76 / 2.00 | 16.29 / 14.45 | 47.63 / 47.58 |

No material peak RSS regression appears in these workloads. The speedup is
primarily in HIR location lookup; debug LLVM local locations also benefit.
Semantic analysis still dominates source-to-IR work on dungeon and pixels.

## Checking and C emission

Supplementary five-sample runs used `build/benchmark_compiler.py --modes check
cgen --scenarios tiny dungeon pixels --cpu 2`, once per compiler. These are
sequential baseline/changed batches and more exposed to time drift than the
alternating LLVM comparison. Raw data: [before](compiler-m2-lines/other-before.json.gz),
[after](compiler-m2-lines/other-after.json.gz). C times exclude C compilation.

| Workload | Mode | Target | Before ms | After ms |
| --- | --- | --- | ---: | ---: |
| tiny | check | debug | 1.11 | 1.11 |
| tiny | check | release | 1.18 | 1.10 |
| tiny | cgen | debug | 1.98 | 1.88 |
| tiny | cgen | release | 2.10 | 1.84 |
| dungeon | check | debug | 208.90 | 206.68 |
| dungeon | check | release | 211.13 | 206.20 |
| dungeon | cgen | debug | 305.15 | 230.48 |
| dungeon | cgen | release | 302.16 | 228.09 |
| pixels | check | debug | 251.76 | 248.48 |
| pixels | check | release | 251.63 | 249.91 |
| pixels | cgen | debug | 341.34 | 278.93 |
| pixels | cgen | release | 343.69 | 280.08 |

A separate ten-sample alternating `check` comparison, with one warm-up per
compiler on CPU 2, verifies that checking has no material regression in the
final implementation. [Raw paired checking samples](compiler-m2-lines/check-paired.json.gz).

| Workload | Before median ms | After median ms |
| --- | ---: | ---: |
| tiny | 1.183 | 1.152 |
| dungeon | 208.748 | 208.890 |
| pixels | 253.532 | 251.949 |

## Correctness and next step

The `lexer-line-index` internal test compares every offset with the original
scanner across empty input, no trailing newline, LF, CRLF, consecutive newlines,
tabs, UTF-8 byte columns, EOF and out-of-range offsets. It checks repeated buffer
indexing, mapped fragment preparation, cleanup and the unindexed fallback.
All benchmark LLVM output remains byte-identical.

This completes the third M2 slice. The next step is investigating repeated
semantic usage-context inference before changing its algorithm. M3 ownership
preparation and multicore scheduling remain ahead.

Validation on Linux: `just test` passed with 1,117 compiler tests and nine skips,
plus profiling, direct-toolchain and installed-compiler checks and 279 C
compatibility fixtures at both optimization levels. CodeLLDB stepping passed.
Native Windows and macOS validation remains outstanding.
