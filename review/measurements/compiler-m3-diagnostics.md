# M3: task-owned diagnostics

2026-09-20, Linux x86-64, Ryzen 9 7950X, LLVM 22.1.8. Release compiler
baseline `38be4b0b` compared with that commit plus this diagnostic-context slice.
[Raw comparison](compiler-m3-diagnostics/comparison.json.gz) retains compiler
hashes, commands, samples, phase records and output hashes.

## Ownership and ordering

Each LLVM module result now owns a diagnostic context. Contexts isolate message
scratch, output settings, rendering mode and last-rendered text. A thread-local
binding lets existing diagnostic APIs use the selected context. Deferred queues
deep-copy messages, references, notes, help and source/fragment snapshots.
Restoring the previous binding and replaying on the coordinator preserves module
order. Replay consumes the queue; discarded queues and unstarted slots are safe
to clean up. Arenas allocate lazily, including on paths with no diagnostics.

Rendering still uses the coordinator's global temporary arena. Fatal internal
compiler errors remain immediate process exits. Allocator bookkeeping, task
metrics, worker primitives and the remaining borrowed-input audit are still
required before enabling workers. M3 remains in progress; no concurrent
execution is enabled or validated by this slice. Copying source snapshots can
increase memory on error-heavy workloads; successful-build measurements below
do not quantify that cost.

## Validation

The internal context test checks independent settings and scratch storage,
queue order, nested capture, replay after original source storage is freed,
discarded runtime diagnostics and repeatable cleanup without tracked heap leaks.
Immediate and deferred output match byte-for-byte for normal diagnostics and
test JSON. The context test also compares diagnostics JSON, including mapped
source fragments, references, notes and help. Integration coverage retains the
partial-module sidecar failure test from the preceding ownership slice.

## Single-core overhead

```sh
cp _bin/nerd /tmp/nerd-m3-diagnostics-before
just build-release nerd --skip-mod-sync
python3 build/benchmark_compare.py --before /tmp/nerd-m3-diagnostics-before --cpu 2 \
  --output /tmp/nerd-m3-diagnostics.json
```

Fourteen workload/target cells, one warm-up and five alternating unprofiled
samples per compiler, followed by one separate profiled sample each. All 196
invocations produced byte-identical combined LLVM within their cell. The
benchmark does not execute generated programs. Both compilers use the same
sources, paths, checkout modules and direct LLVM tools. Runs were warm and
pinned to CPU 2 without competing tests; frequency and SMT activity were not
fixed. Target configuration is independent of compiler build configuration.

Times include LLVM output writing, object generation and linking. Positive
change means slower. RSS is the median maximum per-process high-water mark for
the compiler and waited-for children, not their concurrent sum or the isolated
context footprint.

| Workload | Target | Before median ms | After median ms | Change | Peak process RSS before/after MiB |
| --- | --- | ---: | ---: | ---: | ---: |
| tiny | debug | 19.80 | 19.48 | -1.6% | 50.11 / 50.23 |
| tiny | release | 23.52 | 23.32 | -0.9% | 47.58 / 47.78 |
| dungeon | debug | 1702.41 | 1700.79 | -0.1% | 73.66 / 72.75 |
| dungeon | release | 2957.56 | 2955.38 | -0.1% | 144.32 / 144.25 |
| pixels | debug | 209.65 | 210.21 | +0.3% | 60.62 / 60.64 |
| pixels | release | 304.91 | 304.64 | -0.1% | 66.01 / 65.97 |
| quill | debug | 58.59 | 58.88 | +0.5% | 54.20 / 54.17 |
| quill | release | 75.36 | 74.98 | -0.5% | 60.09 / 60.07 |
| wide | debug | 82.52 | 82.67 | +0.2% | 54.76 / 54.81 |
| wide | release | 126.33 | 126.82 | +0.4% | 50.11 / 50.42 |
| deep | debug | 83.47 | 83.13 | -0.4% | 54.74 / 54.75 |
| deep | release | 126.02 | 126.52 | +0.4% | 50.43 / 50.25 |
| large | debug | 124.46 | 124.18 | -0.2% | 54.35 / 54.38 |
| large | release | 95.06 | 94.70 | -0.4% | 47.59 / 47.61 |

No material single-core timing regression appears in this comparison. Small
variations are not claimed as speedups or evidence of multicore scaling.
Native Windows/macOS validation remains pending.

Linux validation: `just test` passed with 1,119 compiler tests and nine skips,
profiling/output parity, direct LLVM/no-Clang and doctor checks, installation
smoke tests, and 279 C compatibility fixtures at both optimization levels.
CodeLLDB stepping passed.
