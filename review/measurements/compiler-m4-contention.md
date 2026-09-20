# M4: allocator acquisition and render dispatch measurements

2026-09-20, Linux x86-64, Ryzen 9 7950X, LLVM 22.1.8. Baseline `d4f87ab2`
includes the Pixels release fix. Both baseline and changed benchmark compilers
are release builds. These measurements replace the earlier targetless release
scaling evidence for adoption decisions; the earlier report remains historical.

## Instrumentation

`NERD_PROFILE=1 NERD_PROFILE_LOCKS=1` adds per-phase thread-local memory-bookkeeping
lock acquisition counts and elapsed acquisition time. Scopes restore their
previous settings; nested probes are inclusive. Counter updates and duration
conversion occur after unlocking. Disabled scopes perform no additional clock
reads, but do test the thread-local enable flag. Ordinary profiling and lock
profiling are distinct modes. The final process-wide heap snapshot is outside
the probe's own lock measurement.

An acquisition measurement includes the native lock call, uncontended overhead,
clock cost and descheduling. Two clock readings per acquisition perturb the
workload, including a reading immediately after acquiring the shared lock.
These diagnostic durations are not pure blocked time and are not used for
unprofiled speedup claims. The Windows frequency cache is now thread-local so
simultaneous first-time duration conversions cannot race on lazy initialization.
Native Windows validation is still pending.

Profiled builds with more than one requested job emit an ordered scheduler
record after the batch joins. It captures total batch elapsed time, submission
to first callback, last callback to batch return, requested/capped slots,
completed tasks and cumulative submission-to-callback delay. The latter includes
startup and waiting behind earlier module work. These are callback boundaries,
not isolated native creation/join or queue-mutex wait measurements. A one-job
build still renders inline and emits its per-module records without a batch
record. All result storage is owned by fixed module slots until joining.

## Validation

- `just test`: 1,121 compiler tests, 9 skipped, plus profiling, concurrent-render,
  production jobs, direct LLVM/doctor, install checks and 279 C differential
  fixtures at two optimization levels pass.
- The production jobs suite passes with full-compiler ThreadSanitizer and
  AddressSanitizer builds, including lock-profile runs. Lock instrumentation
  leaves LLVM bytes and phase label/order unchanged.
- Standalone allocator tests pass normally and with both sanitizers, in debug
  and release configurations. Alternating instrumented/uninstrumented workers
  verify thread-local counts, disabled behavior and setting restoration alongside
  cross-thread allocation ownership and coherent heap counters.
- Scheduler records satisfy completion, slot-cap and elapsed-boundary checks
  across jobs 2/4/8/256. The inline path produces no scheduler record.

## Method

Serial overhead: CPU 2, one warmup and five alternating unprofiled samples per
compiler, plus separate profiles. Scaling: CPUs 0–15, one logical CPU per physical
core, jobs 1/2/4/8/16 rotated across one warmup and five unprofiled samples, one
ordinary profile and one separate lock-instrumented profile per cell. Both sets
use identical source/output paths within comparisons and verify combined LLVM
hashes for every invocation. No competing tests/builds run during measurement.
The programs are compiled, not executed by the benchmark. The Pixels headless
runtime regression runs in the separate correctness suite.

Ordinary scheduler/profile intervals and lock timings are single diagnostic
samples. Unprofiled whole-command medians include the serial front end, LLVM
merge and native LLVM tools. They must not be confused with render-only time.

## Disabled instrumentation overhead

Whole-command medians in milliseconds. Differences are small on most cells;
the largest measured slowdown is 0.9%. Apparent improvements are not attributed
to this instrumentation change. All 14 comparisons retain identical LLVM.

| Input | Target | Baseline | Changed | Change |
| --- | --- | ---: | ---: | ---: |
| tiny | debug | 22.24 | 22.44 | +0.9% |
| tiny | release | 31.57 | 29.44 | -6.7% |
| dungeon | debug | 1803.56 | 1802.36 | -0.1% |
| dungeon | release | 3149.93 | 3143.96 | -0.2% |
| pixels | debug | 219.79 | 219.82 | +0.0% |
| pixels | release | 325.40 | 325.44 | +0.0% |
| quill | debug | 63.70 | 63.55 | -0.2% |
| quill | release | 81.85 | 82.40 | +0.7% |
| wide | debug | 87.39 | 88.07 | +0.8% |
| wide | release | 130.23 | 129.73 | -0.4% |
| deep | debug | 86.85 | 87.23 | +0.4% |
| deep | release | 131.08 | 131.02 | -0.0% |
| large | debug | 129.77 | 127.74 | -1.6% |
| large | release | 99.06 | 96.08 | -3.0% |

## Corrected worker scaling

Whole-command medians in milliseconds. Slots are capped at module count.

| Input | Target | Jobs 1 | Jobs 2 | Jobs 4 | Jobs 8 | Jobs 16 |
| --- | --- | ---: | ---: | ---: | ---: | ---: |
| tiny | debug | 21.10 | 21.01 | 21.32 | 21.04 | 21.99 |
| tiny | release | 26.80 | 27.58 | 27.51 | 26.94 | 26.99 |
| dungeon | debug | 1714.90 | 1709.80 | 1715.09 | 1713.89 | 1714.43 |
| dungeon | release | 3045.62 | 3039.67 | 3037.22 | 3042.51 | 3042.68 |
| pixels | debug | 221.12 | 207.65 | 201.57 | 206.32 | 204.92 |
| pixels | release | 326.03 | 321.76 | 322.35 | 322.56 | 332.23 |
| quill | debug | 64.91 | 61.86 | 61.92 | 61.18 | 62.57 |
| quill | release | 83.99 | 84.07 | 84.66 | 85.46 | 85.58 |
| wide | debug | 88.08 | 84.16 | 85.22 | 91.15 | 98.81 |
| wide | release | 132.32 | 131.18 | 135.31 | 136.46 | 140.92 |
| deep | debug | 87.26 | 84.73 | 89.50 | 90.46 | 99.13 |
| deep | release | 131.08 | 132.76 | 134.29 | 135.09 | 140.03 |
| large | debug | 128.18 | 129.82 | 128.41 | 128.79 | 129.20 |
| large | release | 98.27 | 98.08 | 99.56 | 98.44 | 98.65 |

## Lock acquisition and batch boundaries

Diagnostic samples for debug targets, jobs 1 versus 16 requested slots.
Acquisition time is summed across module tasks and can overlap. Startup/drain
are from ordinary profiling, not lock-instrumented runs.

| Input | Lock acquisitions | Acquire ms, j1 | Acquire ms, j16 | First dispatch ms, j16 | Drain ms, j16 |
| --- | ---: | ---: | ---: | ---: | ---: |
| tiny | 3,698 | 0.06 | 0.09 | 0.025 | 0.012 |
| dungeon | 248,436 | 4.20 | 58.89 | 0.162 | 0.071 |
| pixels | 457,862 | 7.74 | 126.83 | 0.171 | 0.032 |
| quill | 66,569 | 1.12 | 20.78 | 0.153 | 0.031 |
| wide | 81,512 | 1.38 | 105.29 | 0.260 | 0.054 |
| deep | 81,443 | 1.38 | 108.38 | 0.244 | 0.026 |
| large | 88,611 | 1.53 | 2.38 | 0.029 | 0.005 |

## Conclusions and next experiments

The default remains one job. Pixels debug improves from 221.12 to 201.57 ms at
four jobs (8.8%), while wide/deep debug regress about 12–14% at 16 jobs.
Release improvements are smaller and high worker counts can regress. First
callback and drain intervals are sub-millisecond here, so replacing the scheduler
or introducing work stealing is not the first intervention supported by these data.

Bookkeeping-lock acquisition grows under concurrency, but it is not the whole
explanation. Wide debug's aggregate render CPU time in ordinary profiling grows
from 27.91 to 206.37 ms at 1/16 jobs. Unprofiled whole-command median system CPU
(including child tools) grows from 36.97 to 191.69 ms, while user CPU grows from
51.70 to 80.23 ms. The same instrumentation run measures 105.29 ms cumulative
lock acquisition out of 648.46 ms aggregate task wall time at 16 jobs. The latter
values overlap and the instrumented execution is perturbed; they do not partition
unprofiled elapsed time.

Arena creation reserves virtual memory, changes page protections and destroys
mappings repeatedly. This is a candidate for the remaining system CPU increase,
not a proven kernel-stack attribution. Prefer measuring/reusing per-render
scratch arenas and reducing accounting pressure before more scheduling machinery.
Preserve coherent heap snapshots, cross-thread frees and exact task counters.

A separate user-space `perf record --call-graph dwarf -F 999` run of the wide
debug build at 16 jobs also identifies a serial opportunity: LLVM metadata
remapping appends unchanged text one character at a time, repeatedly entering
arena bookkeeping. The call chain reaches `sb_append_char`, `arena_alloc` and
`mem_stats_record_arena_alloc` from the combiner. Batched copying of unchanged
runs is a bounded next optimization with exact-output validation. The profile
contains child tool samples and excludes kernel stacks on this host; it cannot
identify the kernel source of the extra system CPU.

[Raw overhead](compiler-m4-contention/overhead.json.gz),
[raw scaling and lock profiles](compiler-m4-contention/scaling.json.gz), and
[user-space sample summary](compiler-m4-contention/perf-wide-jobs16.txt).
Reproduce with `build/benchmark_compare.py` and `build/benchmark_jobs.py
--lock-profile`; commands, executable hashes, samples and affinities are retained.
