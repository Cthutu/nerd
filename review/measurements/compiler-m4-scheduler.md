# M4: bounded LLVM module scheduler

2026-09-20, Linux x86-64, Ryzen 9 7950X, LLVM 22.1.8. This is the first M4
implementation slice; the experimental default remains one execution slot.

## Behavior and ownership

`nerd build --jobs N` / `-j N` accepts 1–256 execution slots, including the
calling thread, capped at module count. One slot renders inline. More slots
use a finite index queue, a mutex, a startup condition and joinable workers.
No callback begins until all requested workers have started. Failed startup
joins every started worker and executes no callbacks. Failed callbacks stop
further dispatch and allow active callbacks to finish before joining.

The production callback renders one module and its optional alternate LLVM
sidecar into a stable result slot. Diagnostics and timings are captured there.
The coordinator joins all workers before replaying diagnostics, emitting
profiles, writing sidecars, collecting initialization order or combining LLVM.
It retains the read-only program/name index until workers finish. A sidecar
write failure discards remaining results safely. The one-slot path preserves
early file-error behavior. LLVM render callbacks currently succeed or raise a
fatal internal error; generic callback cancellation is tested at core level.

Source discovery, parsing, semantic checking, HIR production, LLVM text merging
and LLVM tool invocation remain serial. C compatibility output is unchanged.
The installed compiler invokes LLVM tools directly and never invokes Clang.

## Correctness evidence

- `just test`: 1,120 compiler tests passed, 9 skipped; existing profiling,
  concurrent-render, no-Clang/toolchain/doctor, installation and 279 differential
  C fixtures at both optimization levels passed.
- New `build/test_jobs.py` runs production builds at jobs 1/2/4/8/256 against the
  default serial path. Tiny/wide/deep/large synthetic inputs produce byte-identical
  debug/release combined LLVM and sidecars, stable profile labels/order, and
  executable programs returning the expected result. Object/archive/shared
  library LLVM, C output and C options also match across job counts.
- Invalid worker counts and the short option are checked. Blocking the second
  module sidecar with a directory reports the expected error before any LLVM
  tools run; removing it allows a successful rebuild and execution.
- The complete production jobs suite passes with separately built full-compiler
  ThreadSanitizer and AddressSanitizer binaries. Core lifecycle tests pass under
  both sanitizers, including injected partial native-thread creation failure,
  failed callbacks, empty batches, clamping and repeated reuse.
- Native Windows/macOS execution remains pending.

## Measurement method

Both benchmark compilers are release builds. The serial comparison uses baseline
`a5f5453f` and the working tree containing this slice, pinned to CPU 2: one warmup,
five alternating unprofiled samples and one separate profile per compiler/input.
Scaling uses CPUs 0–15 (one logical CPU per physical core on this host), rotating
jobs 1/2/4/8/16 with one warmup, five unprofiled samples and one separate profile
each. Inputs, output paths and checkout modules are identical within each run.
Every invocation checks the combined LLVM hash. Tests/builds were stopped while
benchmarking. Programs are compiled but not executed by the benchmark runners.

Whole-command timings include all compiler/tool work. Render spans below come
from single profiled runs: earliest module start to latest module finish. They
exclude startup, final joins and merging, and must not be treated as repeated
latency measurements. Aggregate task durations overlap. RSS is maximum process
high-water usage across compiler/waited children, not simultaneous total memory.

## Serial overhead

The one-slot path shows no measured regression in this sample. Small changes
should be treated as run-to-run variation, not attributed to the scheduler.
All 14 debug/release comparisons retain byte-identical LLVM.

| Input | Target | Before ms | After ms | Change |
| --- | --- | ---: | ---: | ---: |
| tiny | debug | 22.32 | 21.78 | -2.4% |
| tiny | release | 29.15 | 28.55 | -2.1% |
| dungeon | debug | 1775.84 | 1762.82 | -0.7% |
| dungeon | release | 2959.40 | 2948.38 | -0.4% |
| pixels | debug | 221.38 | 215.47 | -2.7% |
| pixels | release | 312.82 | 308.03 | -1.5% |
| quill | debug | 62.52 | 62.37 | -0.3% |
| quill | release | 79.64 | 79.10 | -0.7% |
| wide | debug | 85.55 | 85.63 | +0.1% |
| wide | release | 127.92 | 127.39 | -0.4% |
| deep | debug | 85.37 | 84.97 | -0.5% |
| deep | release | 126.45 | 126.02 | -0.3% |
| large | debug | 126.51 | 122.37 | -3.3% |
| large | release | 95.13 | 91.45 | -3.9% |

## Worker scaling

Whole-command medians in milliseconds; job counts are requested slots, capped
at module count. Affinity differs from the serial-overhead table, so compare
within each table. All 70 scenario/target/job cells retain identical LLVM.

| Input | Target | Jobs 1 | Jobs 2 | Jobs 4 | Jobs 8 | Jobs 16 |
| --- | --- | ---: | ---: | ---: | ---: | ---: |
| tiny | debug | 20.82 | 20.12 | 20.78 | 20.34 | 20.91 |
| tiny | release | 26.13 | 25.90 | 25.60 | 26.47 | 25.70 |
| dungeon | debug | 1698.10 | 1688.13 | 1686.41 | 1702.62 | 1694.21 |
| dungeon | release | 2951.20 | 2938.61 | 2950.24 | 2950.52 | 2958.02 |
| pixels | debug | 216.25 | 206.09 | 213.18 | 199.32 | 198.04 |
| pixels | release | 308.70 | 305.95 | 312.75 | 305.27 | 304.06 |
| quill | debug | 63.47 | 60.19 | 62.33 | 61.49 | 60.58 |
| quill | release | 80.37 | 80.74 | 80.81 | 81.79 | 81.14 |
| wide | debug | 86.98 | 83.53 | 88.95 | 87.64 | 97.43 |
| wide | release | 130.90 | 127.52 | 133.26 | 133.61 | 137.01 |
| deep | debug | 86.54 | 82.87 | 87.45 | 90.36 | 97.46 |
| deep | release | 130.40 | 129.21 | 136.08 | 132.90 | 136.57 |
| large | debug | 125.51 | 124.04 | 124.73 | 125.04 | 124.64 |
| large | release | 94.43 | 96.53 | 95.26 | 94.66 | 94.75 |

Profiled render envelopes in milliseconds (one diagnostic sample per cell):

| Input | Target | Jobs 1 | Jobs 2 | Jobs 4 | Jobs 8 | Jobs 16 |
| --- | --- | ---: | ---: | ---: | ---: | ---: |
| tiny | debug | 0.76 | 0.86 | 0.74 | 0.88 | 0.73 |
| tiny | release | 0.40 | 0.38 | 0.38 | 0.39 | 0.37 |
| dungeon | debug | 48.37 | 42.49 | 46.82 | 40.87 | 45.33 |
| dungeon | release | 21.96 | 20.24 | 22.18 | 21.98 | 19.48 |
| pixels | debug | 46.70 | 47.14 | 41.17 | 48.40 | 27.93 |
| pixels | release | 21.02 | 14.48 | 14.26 | 23.89 | 14.24 |
| quill | debug | 9.41 | 9.05 | 7.80 | 5.60 | 5.35 |
| quill | release | 3.91 | 4.56 | 2.52 | 5.28 | 4.98 |
| wide | debug | 29.18 | 33.55 | 29.67 | 33.44 | 39.88 |
| wide | release | 16.10 | 14.17 | 14.37 | 21.43 | 22.67 |
| deep | debug | 29.90 | 22.86 | 31.17 | 28.79 | 38.93 |
| deep | release | 15.90 | 13.48 | 19.82 | 21.27 | 22.20 |
| large | debug | 28.77 | 28.95 | 27.93 | 28.85 | 29.28 |
| large | release | 14.31 | 14.31 | 14.41 | 14.45 | 14.52 |

## Interpretation and next steps

- Pixels debug improves from 216.25 to 198.04 ms with 16 requested slots (8.4%);
  Quill debug improves from 63.47 to 60.58 ms (4.6%). Dungeon is effectively flat.
  These are whole-command medians from this host, not universal speedups.
- High worker counts regress the wide/deep debug workloads by about 12–13%.
  Two slots are faster than one in those cells, but additional workers do not
  continue that trend. Keep the default at one; automatic sizing is not justified.
- Dungeon's serial debug render envelope is only 48.37 ms out of a 1,698.10 ms
  whole build. Its largest imported module takes 37.41 ms alone. Both serial
  front-end/tool work and module imbalance limit this scheduler's potential.
- Individual render durations often increase under concurrency. The global
  allocation-accounting lock is a plausible contributor, alongside allocator,
  memory-bandwidth, scheduling and frequency effects; these data do not isolate
  the cause. Next measure lock contention and queue/startup overhead, then test
  a targeted accounting optimization before adding more parallel phases.
- Median maximum-process RSS stays close across counts (for example, Pixels
  debug 60.4 versus 60.8 MiB; Dungeon debug 73.1 versus 73.7 MiB at 1/16 slots).
  This metric can be dominated by a child tool and is not a proof of constant
  compiler memory. Memory-budget sizing remains an adoption gate.
- Linux LLDB stepping passes with both the default and four requested jobs.
  Native Windows/macOS validation, queue-wait metrics and a memory-budget cap
  remain open. M4 stays in progress; M5 front-end work follows the scaling evidence.

[Raw serial comparison](compiler-m4-scheduler/comparison.json.gz) and
[raw scaling results](compiler-m4-scheduler/scaling.json.gz) retain executable
hashes, exact commands, sample timings, profiles, output hashes and affinity.
Reproduce using `build/benchmark_compare.py` and `build/benchmark_jobs.py`;
see `docs/compiler-profiling.md` for arguments and timing limitations.
