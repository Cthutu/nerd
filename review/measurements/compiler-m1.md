# M1: direct LLVM baseline and module profiles

Date: 2026-09-19. Branch: `experiment/task-scheduler-performance`.
Source context: `63903fed` plus the M1 instrumentation and benchmark runner in
this change. Compiler SHA-256, source hashes, tool versions and raw samples are
stored with each run. These measurements precede any M2 optimisation.

## Conclusions

1. **First reduce LLVM combiner scratch-arena churn.** It allocates and destroys
   an arena for each emitted line. Pixels' combining sample committed a cumulative
   1,454,899,200 bytes to produce 1,287,026 bytes of IR. These are repeated
   commitments, not 1.45 GB of live memory. Reusing bounded scratch storage is a
   more directly supported first experiment than changing the symbol index.
2. **Cache program-wide function-name conflict results.** This lookup accounts
   for 26% of sampled user CPU in a pixels build and rescans functions across
   every module for repeated queries. A prepared immutable name-frequency map
   can help both serial execution and future parallel rendering.
3. **Investigate usage-context inference before type canonicalization.** It
   accounts for 70.82% of sampled user CPU in dungeon checking. Declaration
   collection adds 16.07%. The current evidence does not identify type interning
   as the leading semantic bottleneck.
4. **Index source line starts for debug-location lookup.**
   `lex_offset_to_line_col` accounts for 12.24% of sampled user CPU in pixels;
   it scans source from the beginning on each query. Preserve source-fragment
   mapping and debug stepping when changing this.
5. **Start multicore work with LLVM module rendering, but expect bounded gains.**
   Real programs have uneven modules; import dependencies further restrict
   semantic parallelism. The wide synthetic case provides the useful balanced
   control, while the large single-module case exposes the limit.

These are candidates and upper bounds, not demonstrated optimisation speedups.
The scheduler and M2 changes are not implemented by this milestone.

## Method and reproducibility

Linux x86-64, Ryzen 9 7950X (16 physical cores / 32 logical CPUs), LLVM 22.1.8.
Release compiler, pinned to logical CPU 2 including child tools. CPU frequency,
SMT sibling activity and background system load were not fixed. There were no
concurrent test suites or sampling sessions during latency runs.

```sh
just build-release nerd --skip-mod-sync
python3 build/benchmark_compiler.py --cpu 2 --samples 5 --output /tmp/nerd-m1-warm.json
python3 build/benchmark_compiler.py --cpu 2 --samples 3 --cache cold-inputs --output /tmp/nerd-m1-cold-inputs.json
```

Each matrix contains 42 cells: seven inputs, three modes and two target
configurations. Each cell has one warm-up, unprofiled timing samples, and a
separate profiled sample. Total: 336 retained unprofiled samples and 84 profiled
invocations, plus 84 warm-ups. Generated programs are not run.

`cold-inputs` requests source-page eviction with `posix_fadvise`; it is not a
fully cold toolchain or filesystem. Some advised runs are faster than warm runs,
which illustrates normal variance and frequency drift. Do not interpret these
as cache speedups. The full [matrix](compiler-m1/matrix.md) includes warm ranges.

Raw data (gzip-compressed JSON): [warm](compiler-m1/warm.json.gz),
[cold-input advice](compiler-m1/cold-inputs.json.gz),
[disabled-profiling comparison](compiler-m1/disabled-overhead.json.gz).
Read with `json.load(gzip.open(path, 'rt'))`. See
[profiling documentation](../../docs/compiler-profiling.md) for field semantics,
runner options, cold-cache limitations and memory accounting.

## Warm end-to-end medians

Milliseconds, five unprofiled samples. Check and C columns below use debug target
configuration; the complete matrix also includes their release configurations.
C generation excludes compiling the generated C.

| Input | Check | LLVM debug binary | LLVM release binary | C generation |
| --- | ---: | ---: | ---: | ---: |
| tiny | 1.51 | 32.34 | 38.14 | 2.59 |
| dungeon | 300.48 | 3030.44 | 4571.12 | 443.65 |
| pixels | 347.82 | 1116.34 | 1072.25 | 464.85 |
| quill | 28.89 | 185.42 | 169.98 | 41.55 |
| wide | 18.25 | 274.18 | 240.79 | 26.80 |
| deep | 19.88 | 263.89 | 249.24 | 27.00 |
| large | 204.41 | 530.23 | 437.52 | 217.19 |

These are fresh baselines, not a comparison against the earlier unpinned Clang
measurements. The remainder focuses on Nerd's source-to-IR work. Tool records
separate `opt`, `llc` and linking, so external compilation is no longer conflated
with Nerd's own module work in the profiling data.

## Source-to-IR phase observations

Milliseconds from each cell's single profiled debug LLVM sample, not phase
medians. The measured subtotal includes lex/parse and the four columns below;
it excludes loading, uninstrumented overhead and external tool work.

| Input | Sema | HIR | LLVM render | Combine | Measured source-to-IR subtotal |
| --- | ---: | ---: | ---: | ---: | ---: |
| dungeon | 295.73 | 83.90 | 149.44 | 212.34 | 745.26 |
| pixels | 345.98 | 80.81 | 328.81 | 224.85 | 984.37 |
| quill | 24.51 | 4.29 | 22.90 | 69.12 | 122.87 |
| wide | 16.82 | 1.96 | 71.98 | 106.17 | 197.93 |
| deep | 15.20 | 1.58 | 64.24 | 96.73 | 178.76 |
| large | 192.31 | 16.53 | 149.28 | 100.36 | 459.21 |

Dungeon's `std.term` module contributes 260.41 ms of sema, 77.95 ms of HIR and
100.93 ms of LLVM rendering. Pixels' `std.opengl` contributes 176.21 ms, 53.20 ms
and 136.43 ms respectively. Several other modules contribute materially to
pixels rendering, making it more suitable for module-level parallelism.

### Optimistic parallelism bounds

For rendering, the stage bound is total module time divided by the largest
module's time. For semantic analysis, it is total time divided by the longest
weighted import path, using the recorded dependency graph. Both assume unlimited
workers, unchanged per-module costs and zero scheduling overhead. Semantic
ownership safety is not established by this arithmetic.

The last column applies only rendering parallelism to the measured source-to-IR
subtotal: `subtotal / (subtotal - render_total + largest_render)`.

| Input | Modules | Sema stage bound | Render stage bound | Source-to-IR bound from rendering alone |
| --- | ---: | ---: | ---: | ---: |
| dungeon | 10 | 1.02x | 1.48x | 1.07x |
| pixels | 10 | 1.02x | 2.41x | 1.24x |
| quill | 10 | 1.59x | 4.51x | 1.17x |
| wide | 18 | 8.82x | 14.88x | 1.51x |
| deep | 18 | 1.05x | 13.45x | 1.50x |
| large | 2 | 1.00x | 1.03x | 1.01x |

Wide/deep include the root and implicit core module. A deep import graph limits
checking concurrency but need not limit rendering once all HIR is ready. These
bounds apply to the current costs; repeat them after reducing serial work.

## CPU sampling and allocation evidence

Separate unprofiled executions, pinned to CPU 2:

```sh
NERD_LIB_PATH="$PWD/mods" perf record -F 999 -e cpu-clock:u --call-graph dwarf -o /tmp/pixels.perf -- taskset -c 2 _bin/nerd build -o /tmp/pixels examples/pixels/pixels.n
NERD_LIB_PATH="$PWD/mods" perf record -F 999 -e cpu-clock:u --call-graph dwarf -o /tmp/dungeon.perf -- taskset -c 2 _bin/nerd check examples/dungeon/dungeon.n
DEBUGINFOD_URLS= perf report -i /tmp/pixels.perf --stdio --no-children --no-call-graph --comms nerd --sort symbol --percent-limit 1
```

Retained reports: [pixels](compiler-m1/pixels-perf.txt),
[dungeon checking](compiler-m1/dungeon-perf.txt). They contain 850 and 305 user
CPU samples respectively, with no lost samples. Percentages are sampled user
CPU, not percentages of elapsed build time. Kernel work and LLVM subprocesses
are excluded from these filtered reports; inlining limits attribution precision.

Pixels samples: function-name conflict scans 26.00%, usage-context inference
21.76%, declaration collection 12.59%, offset-to-line/column scans 12.24%.
Dungeon check samples: usage-context inference 70.82%, declaration collection
16.07%. These favour targeted caches and reducing repeated AST scans over an
unmeasured blanket type-table rewrite.

The combiner's arena churn is visible independently in source and counters:
`back_end_append_llvm_without_satisfied_declarations` creates `line_arena` inside
its line loop and destroys it on every emission path. Pixels requested only
2,581,556 arena bytes in this phase while cumulatively committing 1,454,899,200.
Quill committed 454,950,912 bytes for 359,279 output bytes; wide committed
713,097,216 for 570,243 output bytes. A syscall-count check could not be run
because `strace` is not installed. No syscall counts are claimed here.

Peak process RSS across warm debug-binary samples was 73.9 MiB for dungeon and
61.2 MiB for pixels. That includes the maximum process high-water mark observed
through `wait4`, not a simultaneous tree-wide peak. Cumulative arena commitments
must not be presented as retained memory.

## Disabled instrumentation cost

An alternating pinned comparison against the previously installed `63903fed`
compiler used two warm-ups per compiler, 60 tiny checks and 12 pixels checks.
Tiny medians were 1.144 ms before and 1.148 ms after (+0.31%); pixels medians were
288.889 ms before and 283.730 ms after (-1.79%). This does not show a material
disabled-profiling regression on these inputs; it is not a claim of speedup.

## Next milestone

M1 is complete for the Linux baseline. Actual queue wait instrumentation belongs
with the scheduler; this milestone supplies module phase boundaries and dependency
facts without inventing ready-queue timings. Other-host measurements remain open.

M2 should first reuse combiner scratch storage and verify byte-identical LLVM,
debug behavior, allocation counts and before/after latency. Then evaluate a
program-wide function-name conflict index and line-start lookup. Investigate
usage-context inference with a separate semantic regression strategy. Retain
symbol-membership and type-canonicalization indexing as candidates, rather than
assuming they lead the measured costs.
