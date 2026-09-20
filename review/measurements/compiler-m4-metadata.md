# M4: batch unchanged LLVM text during metadata remapping

2026-09-20, Linux x86-64, Ryzen 9 7950X, LLVM 22.1.8. Baseline `3af92498`
includes the corrected native LLVM target and the contention instrumentation.
Both compared compilers are release builds, with the default one execution slot.

## Change and evidence

The [contention investigation](compiler-m4-contention.md) found a serial
combiner path that appended unchanged LLVM text one byte at a time. Every byte
advanced the arena and entered shared bookkeeping. Metadata remapping now
copies contiguous unchanged spans with one append, while retaining the same
numeric-reference scan and renumbering behavior. It does not change allocator
locking, result lifetimes or worker scheduling. This is a single-core improvement
that also reduces the serial portion of multi-worker builds.

The existing combiner test covers long lines, scratch growth/reuse and metadata
surviving subsequent resets. Added cases cover adjacent numeric references,
empty input, nonnumeric exclamation marks and a trailing reference. Exact LLVM
identity across representative debug/release builds is the primary acceptance
check; diagnostic/debugger and runtime tests remain separate gates.

## Measurement method

One warmup, five alternating unprofiled samples per compiler, plus separate
ordinary profiles. CPU 2 affinity applies to compiler and tools. Source/output
paths and checkout modules are identical within comparisons, and every run
checks combined LLVM identity. No competing tests or builds run during latency
measurement. All samples include front-end, combining and LLVM tool work.
Separate lock-instrumented runs compare acquisition counts in the combine phase;
their elapsed times are not used for performance claims.

## Results

Whole-command medians in milliseconds; the combine columns are separate
single-profile samples, not repeated latency measurements.

| Input | Target | Before | After | Change | Combine before | Combine after |
| --- | --- | ---: | ---: | ---: | ---: | ---: |
| tiny | debug | 22.57 | 22.26 | -1.4% | 0.30 | 0.14 |
| tiny | release | 29.72 | 28.67 | -3.5% | 0.10 | 0.06 |
| dungeon | debug | 1788.13 | 1776.29 | -0.7% | 14.00 | 6.57 |
| dungeon | release | 3015.26 | 3011.52 | -0.1% | 6.60 | 3.36 |
| pixels | debug | 219.78 | 212.92 | -3.1% | 15.99 | 7.61 |
| pixels | release | 323.12 | 319.17 | -1.2% | 7.22 | 3.91 |
| quill | debug | 62.46 | 59.70 | -4.4% | 4.58 | 2.38 |
| quill | release | 80.05 | 78.88 | -1.5% | 2.46 | 1.30 |
| wide | debug | 85.99 | 82.90 | -3.6% | 7.04 | 3.62 |
| wide | release | 128.53 | 128.07 | -0.4% | 1.64 | 1.48 |
| deep | debug | 86.53 | 83.00 | -4.1% | 6.88 | 4.00 |
| deep | release | 128.30 | 128.13 | -0.1% | 1.65 | 1.23 |
| large | debug | 126.36 | 124.77 | -1.3% | 8.98 | 5.12 |
| large | release | 94.85 | 95.84 | +1.0% | 4.31 | 3.04 |

Debug whole-build medians improve 3.1% for Pixels, 4.4% for Quill, 3.6% for wide
and 4.1% for deep. Dungeon improves only 0.7%, because its front end/tools dominate.
Profiled combining is roughly halved on several inputs. The deterministic
mechanism is fewer accounting operations, not a change to emitted LLVM.

The initial large-release cell is 1.0% slower. An 11-sample alternating repeat
is effectively flat: 95.99 to 96.05 ms (+0.1%). Large debug improves 127.18 to
124.08 ms in that repeat. Retain both runs; there is no demonstrated release
speedup for the large input, and small deltas remain noise-sensitive.

Separate debug lock profiles count combine-phase acquisitions:

| Input | Before | After | Reduction |
| --- | ---: | ---: | ---: |
| wide | 530,345 | 70,281 | 86.7% |
| Pixels | 1,222,615 | 130,117 | 89.4% |

These instrumented runs demonstrate the change in work, not a wall-time speedup.
No allocator synchronization is removed, and global/task accounting contracts
remain unchanged.

## Validation and remaining work

All 1,121 compiler tests pass (9 skipped), along with profiling, concurrent-render,
production scheduler, direct LLVM/doctor, install and C differential suites.
The production jobs suite also passes under AddressSanitizer after the change.
Every benchmark comparison retains byte-identical LLVM, including the larger
repeat. Debugger stepping and a live Pixels debug/release smoke test at jobs 1/4
verify rendered colors and clean exit; the automated headless Pixels release
regression remains part of the normal suite.

This change improves serial combining; it does not remove the render phase's
high-worker-count regression. Keep default jobs=1. The next measured experiment
is render scratch-arena reuse and reduced accounting pressure, with particular
attention to increased system CPU under concurrency. Preserve output lifetimes,
coherent snapshots and sanitizer gates.

[Raw comparison](compiler-m4-metadata/comparison.json.gz),
[large-input repeat](compiler-m4-metadata/large-repeat.json.gz), and
[separate lock-count profiles](compiler-m4-metadata/lock-counts.json.gz).
