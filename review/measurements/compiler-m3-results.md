# M3: per-module LLVM result ownership

2026-09-20, Linux x86-64, Ryzen 9 7950X, LLVM 22.1.8. Baseline `8e3723d3`,
compared with that commit plus this change. Both compilers are release builds.
[Raw comparison](compiler-m3-results/comparison.json.gz) retains compiler hashes,
commands, every sample, phase records and output hashes.

## Ownership change

The coordinator sizes a result array before rendering. Each stable slot owns
an arena and its module LLVM text. A second render for a non-debug sidecar uses
the same module arena. Output paths, runtime glue and combined LLVM remain in
the coordinator arena. Input HIR, lexer and semantic snapshots retain program
ownership. Rendering, sidecar writes, profiling and initializer collection still
run sequentially in the original module order.

The combiner copies input text into coordinator-owned storage. Module arenas
and borrowed module views are then released before the combined file is written
or external LLVM tools run. For sidecar-only output or an early error, the same
cleanup releases initialized arenas and skips unstarted slots. It is safe to
call final cleanup after render-result cleanup has already run.

This is result-lifetime preparation, not a scheduler. Global memory counters and
debug allocation bookkeeping remain unsafe for concurrent access. Diagnostics
still have global modes/arenas/output state. These, task-local profiling and the
remaining borrowed-input audit must be addressed before enabling workers. The
first M3 result-ownership slice is complete; M3 as a whole remains in progress.

## Correctness checks

`internal-test llvm-result-lifetime` initializes two of five slots, combines
their text, releases the module arenas and verifies the combined text remains
readable and unchanged. Arena counters check both initialized arenas were
released; repeated cleanup must not release them again.

The profiling integration test makes the second module's sidecar path a
directory. It checks that two renders completed, the first sidecar exists,
external tools never ran, and the normal file-write diagnostic identifies the
blocked path. Removing the obstruction allows a subsequent build and execution
to succeed. This exercises cleanup with started and unstarted slots. Existing
profiling checks cover alternate sidecar rendering, and the full toolchain suite
covers executable, object, archive and shared-library output.

## Single-core overhead comparison

```sh
cp _bin/nerd /tmp/nerd-m3-results-before  # before rebuilding this change
just build-release nerd --skip-mod-sync
python3 build/benchmark_compare.py --before /tmp/nerd-m3-results-before --cpu 2 \
  --output /tmp/nerd-m3-results.json
```

One warm-up and five alternating unprofiled samples per compiler/cell, followed
by one separate profiled sample each. Warm local runs on CPU 2 without competing
tests or sampling; frequency and SMT sibling activity were not fixed. Both
compilers used identical sources, paths, checkout modules and LLVM tools.
All 196 invocations produced byte-identical combined LLVM within their cell.
Generated programs were not executed by the benchmark. Target configuration is
independent of the release compiler configuration.

Milliseconds below include retained LLVM writing, direct LLVM object generation
and linking. Positive change means slower. RSS is the median per-process
maximum high-water mark for the compiler and waited-for children, not their
simultaneous sum or the isolated result storage. Arena reservations are virtual
address space; per-module arenas can increase minimum commitments even while
earlier release shortens retained lifetimes.

| Workload | Target | Before median ms | After median ms | Change | Peak process RSS before/after MiB |
| --- | --- | ---: | ---: | ---: | ---: |
| tiny | debug | 19.59 | 19.39 | -1.0% | 50.13 / 50.31 |
| tiny | release | 23.87 | 23.89 | +0.1% | 47.60 / 47.59 |
| dungeon | debug | 1722.94 | 1728.78 | +0.3% | 73.65 / 73.93 |
| dungeon | release | 2975.29 | 2969.13 | -0.2% | 143.99 / 144.27 |
| pixels | debug | 210.98 | 210.54 | -0.2% | 60.66 / 60.63 |
| pixels | release | 306.07 | 303.81 | -0.7% | 65.96 / 66.02 |
| quill | debug | 60.37 | 60.25 | -0.2% | 54.20 / 54.14 |
| quill | release | 77.71 | 78.48 | +1.0% | 60.12 / 60.08 |
| wide | debug | 83.81 | 83.88 | +0.1% | 54.76 / 54.73 |
| wide | release | 127.21 | 127.19 | -0.0% | 50.32 / 50.35 |
| deep | debug | 83.48 | 83.30 | -0.2% | 54.68 / 54.71 |
| deep | release | 128.45 | 127.49 | -0.7% | 50.17 / 50.29 |
| large | debug | 124.21 | 125.59 | +1.1% | 54.35 / 54.36 |
| large | release | 96.98 | 95.54 | -1.5% | 47.60 / 47.57 |

These samples show no material single-core timing or peak-process-memory
regression; the timing differences are small and are not claimed as speedups.
They establish a baseline for ownership preparation, not multicore scaling.
Next: task-owned diagnostics and allocator synchronization/accounting, followed
by portable worker primitives and concurrency-specific tests.

Validation on Linux: `just test` passed with 1,118 compiler tests and nine skips,
plus profiling (including the new partial-render failure), toolchain and
installation checks and 279 C compatibility fixtures at both optimization
levels. CodeLLDB stepping passed. Native Windows/macOS and actual concurrent
execution remain unvalidated; this change does not enable worker threads.
