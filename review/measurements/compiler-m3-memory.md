# M3: synchronized allocator bookkeeping

2026-09-20, Linux x86-64, Ryzen 9 7950X, LLVM 22.1.8. Baseline `317daa1d`,
compared with that commit plus this allocator slice. Both compilers are release
builds. [Raw comparison](compiler-m3-memory/comparison.json.gz) retains compiler
hashes, commands, samples, phase records and output hashes.

## Change and limits

A statically initialized process-wide lock protects memory counters and the
debug allocation list, index and breakpoint setting. Global live/peak bytes
remain correct when a block changes threads. Snapshots copy the counters under
the same lock. Debug headers now have previous/next pointers, making unlinking
constant-time rather than scanning all allocations while holding the lock.
Headers explicitly preserve `max_align_t` alignment in both configurations.
Marked application-lifetime blocks remain in byte accounting but outside leak
reports, including after reallocations and repeated marking.

Libc allocation calls run outside the bookkeeping lock. During reallocation a
tracked block is temporarily absent from debug list queries, while byte counters
retain its old size until the reallocation is recorded. Counters measure logical
tracked payloads, not physical allocator/RSS peaks. Leak reporting uses libc
output directly to avoid recursively allocating a Nerd output buffer under the
lock. Leak reports should normally run after workers have joined.

This makes bookkeeping safe, not arbitrary shared blocks or arenas. Ownership
handoffs still need synchronization. Global counter deltas include all threads;
task attribution needs separate metrics. The lock is deliberately simple and
contention must be measured when workers exist. The compiler remains sequential;
M3 still requires task metrics, worker primitives and the borrowed-input audit.

## Concurrency validation

`python3 build/test_memory.py` compiles a standalone harness against the actual
allocator in debug and release configurations, with assertions enabled in the
harness. Four workers allocate 8,000 blocks, hand them to different threads for
growth/shrink reallocations, and hand them off again for frees. Half are marked
as application-lifetime allocations twice before reallocation. Concurrent churn
also exercises arena/array event recording. An observer checks coherent,
monotonic snapshots and traverses debug bookkeeping during worker activity.
Joins provide ownership handoff boundaries. Final counts, byte totals, leak-list
contents, pointer alignment and return to initial live bytes are checked exactly. The harness also
exercises leak reporting with and without a tracked block.

Each configuration runs three times. The normal harness is part of `just test`.
Separate `--sanitize thread` and `--sanitize address` runs exercise the same
checks. The initial observer loop starved workers by continuously traversing
the debug list; spacing observations by 100 microseconds on POSIX (1 ms on
Windows) removed that test artifact. Native Windows/macOS execution remains
pending; Linux sanitizer coverage is limited to the standalone allocator.

## Serial overhead

```sh
cp _bin/nerd /tmp/nerd-m3-memory-before
just build-release nerd --skip-mod-sync
python3 build/benchmark_compare.py --before /tmp/nerd-m3-memory-before --cpu 2 \
  --output /tmp/nerd-m3-memory.json
```

One warm-up, five alternating unprofiled samples and one separate profile per
compiler for each of 14 workload/target cells. Warm CPU-2 runs, without competing
tests or builds; frequency and SMT activity were not fixed. Sources, paths,
checkout modules and direct LLVM tools were identical. Target configuration is
independent of compiler configuration. The benchmark does not execute generated
programs; correctness is covered separately by the test suite.

Times include output writing, direct LLVM object generation and linking.
Positive change means slower. RSS is the median maximum per-process high-water
mark for the compiler and waited-for children, not their simultaneous sum or
isolated bookkeeping memory.

| Workload | Target | Before median ms | After median ms | Change | Peak process RSS before/after MiB |
| --- | --- | ---: | ---: | ---: | ---: |
| tiny | debug | 18.73 | 18.37 | -1.9% | 50.32 / 50.31 |
| tiny | release | 28.57 | 27.45 | -3.9% | 47.61 / 47.63 |
| dungeon | debug | 1777.53 | 1779.56 | +0.1% | 73.71 / 73.68 |
| dungeon | release | 3087.82 | 3076.83 | -0.4% | 144.01 / 143.99 |
| pixels | debug | 209.14 | 213.50 | +2.1% | 60.68 / 60.70 |
| pixels | release | 304.85 | 305.57 | +0.2% | 65.97 / 66.00 |
| quill | debug | 60.32 | 61.74 | +2.4% | 54.38 / 54.13 |
| quill | release | 76.57 | 76.95 | +0.5% | 60.08 / 60.06 |
| wide | debug | 85.74 | 86.38 | +0.7% | 54.79 / 54.82 |
| wide | release | 126.53 | 127.68 | +0.9% | 50.29 / 50.28 |
| deep | debug | 84.05 | 85.68 | +1.9% | 54.79 / 54.82 |
| deep | release | 127.81 | 128.10 | +0.2% | 50.19 / 50.36 |
| large | debug | 126.44 | 123.32 | -2.5% | 54.38 / 54.42 |
| large | release | 96.24 | 93.20 | -3.2% | 47.57 / 47.56 |

All 196 invocations produced byte-identical combined LLVM within each cell.
Several debug-target workloads show modest overhead (roughly 2%, up to 4.36 ms
in these medians); other changes are mixed. This is accepted ownership/safety
preparation, not a speedup claim. Peak-process RSS is essentially unchanged at
this measurement resolution. Multicore lock contention is not measured here.

Linux validation: `just test` passed with 1,119 compiler tests and nine skips,
the new concurrent allocator harness, profiling, direct LLVM/no-Clang and doctor
checks, installation smoke tests and 279 C compatibility fixtures at both
optimization levels. CodeLLDB stepping passed. The final allocator also passed
three debug and three release harness runs under each of ThreadSanitizer and
AddressSanitizer. Native Windows/macOS validation remains pending.
