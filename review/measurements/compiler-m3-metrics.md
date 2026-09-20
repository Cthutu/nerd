# M3: task measurement and coordinator reporting

2026-09-20, Linux x86-64, Ryzen 9 7950X, LLVM 22.1.8. Baseline `2f57a9bd`
compared with that commit plus this metrics slice. Both compilers are release
builds. [Raw comparison](compiler-m3-metrics/comparison.json.gz) retains compiler
hashes, commands, samples, phase records and output hashes.

## Attribution and ownership

Allocation events now update both synchronized process counters and thread-local
activity counters. Differences of thread-local snapshots exclude other threads'
work. Frees/reallocations belong to the executing task, including operations on
handed-off blocks. Thread-local live/peak fields remain zero: subtraction on the
freeing thread cannot correctly track ownership or retained memory. Those
observations remain synchronized process-wide values.

A timing probe must begin and finish on the same OS thread without executing
unrelated tasks in between. Nested probes include nested work; task migration
and cooperative helping inside an active probe are not supported. Thread CPU
clock availability and null JSON values retain their previous behavior.

Finishing a probe produces a value-only record without emitting output or
retaining source pointers. LLVM module results own primary and optional sidecar
records. They finish before diagnostic replay; the coordinator supplies labels
and source paths and emits JSON in module order. Existing serial callers retain
the finish-and-emit wrapper. Measurements exclude deferred diagnostic rendering,
JSON emission and subsequent sidecar file writes. Process live/peak values are
observations at completion, not task-local memory footprints.

No worker scheduler is enabled. Worker primitives and the borrowed-input audit
remain. Legacy memory-profile output, dependency records and human timing
aggregation still require coordinator/serial use. Lock contention remains to be
measured once compiler workers exist.

## Validation

The allocator harness now asserts exact per-worker activity during overlapping
allocation, cross-thread reallocation and free phases, including arena/array
recording. The coordinator's local activity stays unchanged while global counts
include every worker. Each configuration runs three times in debug and release;
ThreadSanitizer and AddressSanitizer cover the same concurrent harness.

The profiling integration test checks that finishing records emits nothing,
that subsequent work does not change a finished record, and that the coordinator
can emit in a different order. It checks exact allocation attribution, success
and failure labels, output sizes, escaped module names, clock fields and disabled
record suppression. Existing tests exercise primary/alternate sidecars, output
identity with profiling disabled and partial module failures.

## Serial overhead

```sh
cp _bin/nerd /tmp/nerd-m3-metrics-before
just build-release nerd --skip-mod-sync
python3 build/benchmark_compare.py --before /tmp/nerd-m3-metrics-before --cpu 2 \
  --output /tmp/nerd-m3-metrics.json
```

One warm-up, five alternating unprofiled samples and one separate profile per
compiler for each of 14 workload/target cells. Warm CPU-2 runs with no competing
tests/builds; frequency and SMT activity were not fixed. Both compilers used the
same sources, paths, checkout modules and direct LLVM tools. Target settings
are independent of compiler build configuration. The benchmark does not execute
generated programs; correctness is validated separately by the full suite.

Times include output writing, object generation and linking. Positive change
means slower. RSS is the median maximum per-process high-water mark for the
compiler and waited-for children, not their simultaneous sum or isolated metrics
storage. These are ownership-preparation measurements, not multicore scaling.

| Workload | Target | Before median ms | After median ms | Change | Peak process RSS before/after MiB |
| --- | --- | ---: | ---: | ---: | ---: |
| tiny | debug | 24.08 | 23.51 | -2.4% | 50.18 / 50.32 |
| tiny | release | 29.60 | 29.67 | +0.2% | 47.56 / 47.59 |
| dungeon | debug | 1850.56 | 1847.20 | -0.2% | 73.38 / 73.74 |
| dungeon | release | 3198.37 | 3256.59 | +1.8% | 144.07 / 144.10 |
| pixels | debug | 224.98 | 224.01 | -0.4% | 60.64 / 60.72 |
| pixels | release | 320.23 | 320.03 | -0.1% | 65.98 / 65.98 |
| quill | debug | 65.27 | 65.12 | -0.2% | 54.17 / 54.18 |
| quill | release | 82.91 | 81.07 | -2.2% | 60.09 / 60.10 |
| wide | debug | 89.13 | 87.96 | -1.3% | 54.77 / 54.77 |
| wide | release | 134.87 | 134.67 | -0.1% | 50.34 / 50.35 |
| deep | debug | 88.97 | 87.71 | -1.4% | 54.74 / 54.80 |
| deep | release | 134.55 | 133.51 | -0.8% | 50.20 / 50.29 |
| large | debug | 127.58 | 128.89 | +1.0% | 54.35 / 54.37 |
| large | release | 94.69 | 96.00 | +1.4% | 47.59 / 47.57 |

All 196 invocations produced byte-identical combined LLVM within their cell.
Timing changes are small and mixed; no speedup is claimed. This run shows no
material overall single-core regression or peak-process memory increase.

Linux validation: `just test` passed with 1,119 compiler tests and nine skips,
concurrent allocator/activity tests, profiling/output parity, direct LLVM/no-Clang
and doctor checks, installation smoke tests and 279 C compatibility fixtures at
both optimization levels. CodeLLDB stepping passed. ThreadSanitizer and
AddressSanitizer each passed three debug and three release runs of the allocator
harness with the new per-worker activity assertions. Native Windows/macOS and
actual concurrent LLVM rendering remain unvalidated.
