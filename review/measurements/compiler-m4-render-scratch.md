# M4: reuse LLVM function-render scratch arenas

2026-09-20, Linux x86-64, Ryzen 9 7950X, LLVM 22.1.8. Baseline `e6b2aff2`
includes the native-target fix, contention instrumentation and metadata span
copying. Both compared compilers are release builds.

## Ownership change

Each function body previously created and destroyed three arenas: temporary
values, entry-block text and body text. Global initialization used another set.
A module render now owns one lazy `LlvmRenderScratch` set, reused by global
initialization and its function bodies. Declarations without bodies do not
initialize it. Each new body resets the three cursors; storage capacity follows
the largest body seen in that module.

The ownership boundary is unchanged: entry/body text is copied into module output
before scratch is reused. Debug annotation produces scratch text that is also
copied immediately. Module output and debug metadata have separate longer-lived
arenas; function-local arrays are freed after each function. The scratch set is
destroyed after the function loop, before export wrappers. There is no shared
scratch between workers, no pool across modules, and no allocator-lock removal.

The tradeoff is retaining each scratch arena's high-water committed capacity
until the module's functions finish. Previously a large function's scratch could
be released before later functions grew the module output. This can affect peak
memory; cumulative arena-commit counters are not a measurement of resident memory.

## Correctness gates

- Full `just test`: 1,121 compiler tests, 9 skips; profiling, render concurrency,
  jobs, direct LLVM/doctor, installation and C differential suites pass.
- The jobs suite adds a global initializer, a 1,500-statement function that grows
  scratch, then a small function using the global. Debug/release builds at jobs
  1/2/4/8/256 must retain output identity and execute with the expected result.
- That extended production jobs suite passes under both ThreadSanitizer and
  AddressSanitizer, including lock-profile, output-failure and artifact-mode tests.
- Existing concurrent-render checks cover real examples, private semantic state,
  repeated inputs and parallel renders of the same module.

## Measurement method

Serial comparison: CPU 2, one warmup and five alternating unprofiled samples per
compiler, plus one separate profile. Paired 16-job comparison: CPUs 0–15 (one
logical CPU per physical core), same alternation and fixed paths, covering Pixels,
wide, deep and large inputs. A fresh worker sweep rotates jobs 1/2/4/8/16 over all
seven scenarios and both target modes. Each cell has one warmup, five unprofiled
samples and one separate profile. Every invocation checks combined LLVM bytes.
No competing builds/tests run while measuring. Benchmark programs are compiled,
not executed; runtime checks are separate correctness gates.

The serial and parallel affinity sets differ, so compare within their tables.
Whole-command medians include the front end and LLVM tools. Render spans and
phase counters come from single profiled runs. RSS is maximum process high-water
usage across the compiler/waited children, not simultaneous total memory.


## One-job comparison

Whole-command median milliseconds; all LLVM comparisons are byte-identical.

| Input | Target | Before | After | Change |
| --- | --- | ---: | ---: | ---: |
| tiny | debug | 20.45 | 20.12 | -1.6% |
| tiny | release | 25.95 | 26.43 | +1.9% |
| dungeon | debug | 1741.21 | 1734.86 | -0.4% |
| dungeon | release | 3059.23 | 3060.29 | +0.0% |
| pixels | debug | 215.23 | 206.47 | -4.1% |
| pixels | release | 323.74 | 317.84 | -1.8% |
| quill | debug | 61.70 | 59.90 | -2.9% |
| quill | release | 80.73 | 79.36 | -1.7% |
| wide | debug | 83.43 | 66.23 | -20.6% |
| wide | release | 131.06 | 114.00 | -13.0% |
| deep | debug | 85.22 | 67.99 | -20.2% |
| deep | release | 131.67 | 116.00 | -11.9% |
| large | debug | 124.96 | 105.77 | -15.4% |
| large | release | 96.22 | 80.55 | -16.3% |

## Paired sixteen-job comparison

Whole-command median milliseconds; all LLVM comparisons are byte-identical.

| Input | Target | Before | After | Change |
| --- | --- | ---: | ---: | ---: |
| pixels | debug | 205.25 | 196.61 | -4.2% |
| pixels | release | 320.60 | 319.59 | -0.3% |
| wide | debug | 97.76 | 73.10 | -25.2% |
| wide | release | 139.53 | 119.88 | -14.1% |
| deep | debug | 98.78 | 72.83 | -26.3% |
| deep | release | 140.17 | 122.03 | -12.9% |
| large | debug | 125.75 | 107.31 | -14.7% |
| large | release | 99.12 | 83.27 | -16.0% |

## Render work and memory

Single-profile one-job observations; committed MiB is cumulative new arena
commitment during rendering, not current resident memory. RSS is the median
maximum-process high-water metric from unprofiled samples.

| Input | Target | Render ms before | After | Commit MiB before | After | RSS MiB before | After |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: |
| tiny | debug | 0.77 | 0.48 | 4.31 | 1.88 | 50.2 | 50.3 |
| tiny | release | 0.39 | 0.16 | 2.94 | 0.50 | 47.6 | 47.6 |
| dungeon | debug | 49.06 | 44.87 | 76.69 | 28.31 | 73.5 | 73.7 |
| dungeon | release | 22.13 | 18.78 | 50.88 | 2.50 | 144.3 | 144.1 |
| pixels | debug | 46.96 | 40.03 | 116.19 | 53.38 | 60.7 | 60.6 |
| pixels | release | 20.95 | 15.67 | 65.25 | 2.44 | 66.3 | 66.3 |
| quill | debug | 9.69 | 8.09 | 28.75 | 14.31 | 54.1 | 54.1 |
| quill | release | 3.99 | 2.56 | 17.75 | 3.31 | 60.4 | 60.4 |
| wide | debug | 30.33 | 13.50 | 203.31 | 56.88 | 54.8 | 54.8 |
| wide | release | 16.08 | 1.96 | 150.94 | 4.50 | 50.1 | 50.2 |
| deep | debug | 30.58 | 13.31 | 203.31 | 56.88 | 54.8 | 54.8 |
| deep | release | 16.06 | 2.01 | 150.94 | 4.50 | 50.3 | 50.2 |
| large | debug | 29.05 | 12.35 | 197.06 | 50.62 | 54.8 | 54.8 |
| large | release | 14.68 | 1.24 | 147.00 | 0.56 | 47.6 | 47.6 |

## Fresh worker-count sweep

Whole-command median milliseconds on CPUs 0–15, using the changed compiler.

| Input | Target | Jobs 1 | Jobs 2 | Jobs 4 | Jobs 8 | Jobs 16 |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: |
| tiny | debug | 20.50 | 20.58 | 20.72 | 20.53 | 20.65 |
| tiny | release | 26.77 | 26.98 | 26.81 | 27.08 | 27.58 |
| dungeon | debug | 1702.27 | 1717.96 | 1703.31 | 1706.96 | 1708.48 |
| dungeon | release | 3023.89 | 3024.05 | 3029.98 | 3054.10 | 3027.72 |
| pixels | debug | 206.16 | 195.75 | 193.25 | 194.70 | 195.31 |
| pixels | release | 315.40 | 312.55 | 314.76 | 316.14 | 314.66 |
| quill | debug | 61.21 | 58.79 | 60.06 | 59.78 | 57.74 |
| quill | release | 81.03 | 82.22 | 83.88 | 81.70 | 82.11 |
| wide | debug | 67.04 | 64.80 | 62.82 | 66.19 | 69.30 |
| wide | release | 117.38 | 116.59 | 117.48 | 117.63 | 117.93 |
| deep | debug | 66.92 | 65.93 | 64.64 | 66.49 | 69.64 |
| deep | release | 116.64 | 117.03 | 117.63 | 118.98 | 120.24 |
| large | debug | 107.67 | 106.04 | 108.04 | 106.84 | 107.42 |
| large | release | 83.28 | 82.52 | 84.27 | 83.95 | 83.94 |

## Conclusions

- Reuse improves one-job wide/deep debug builds by about 20%, and large debug/
  release builds by 15–16%. Pixels improves 4.1% debug and 1.8% release. Dungeon
  remains nearly flat because rendering is a small fraction of its build time.
  Tiny-target deltas are small and noise-sensitive; no universal speedup is claimed.
- The paired sixteen-job comparison improves wide/deep debug builds by 25–26%
  and release builds by 13–14%. Wide debug median system CPU (including child
  tools) falls from 241.78 to 152.83 ms; deep falls from 234.21 to 156.94 ms.
  Repeated scratch mapping was therefore a useful target, though these counters
  do not identify every remaining kernel cost.
- Release render envelopes drop dramatically on function-heavy synthetic inputs:
  wide 16.08 to 1.96 ms and large 14.68 to 1.24 ms at one job. These are single
  profiled observations; whole-command gains are smaller and independently sampled.
- Cumulative arena commitments shrink without a material maximum-process RSS
  increase on these workloads. The high-water retention tradeoff still applies
  to modules whose largest function occurs early; do not interpret cumulative
  commitment reductions as resident-memory savings.
- Keep the default at one. In the new sweep, four jobs improve Pixels debug from
  206.16 to 193.25 ms (6.3%) and wide debug from 67.04 to 62.82 ms (6.3%), but
  high counts still regress some targets. Better serial rendering also reduces
  the amount of work available for parallelization.
- Next investigate the remaining per-function debug-name arenas, then reassess
  allocator accounting contention. These arenas still reserve/commit/release for
  every function in debug targets. Native Windows/macOS validation and memory-budget
  worker sizing remain open; this is not an automatic-worker adoption decision.

The live Pixels smoke check verifies varied rendered colors and clean Escape
exit in debug/release mode at jobs 1/4; LLDB stepping passes with four jobs.
The updated compiler is installed globally. Normal Nerd compilation continues
using direct LLVM tools and does not invoke Clang.

[Raw serial comparison](compiler-m4-render-scratch/serial.json.gz),
[paired sixteen-job comparison](compiler-m4-render-scratch/parallel.json.gz), and
[fresh scaling sweep](compiler-m4-render-scratch/scaling.json.gz) retain compiler
hashes, commands, samples, profiles, output hashes and affinity.
`build/benchmark_compare.py` now accepts `--jobs N --cpus ...` for repeatable
paired parallel comparisons as well as its existing one-core mode.
