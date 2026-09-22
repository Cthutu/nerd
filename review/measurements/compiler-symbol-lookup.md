# LLVM symbol lookup benchmark

2026-09-22, Linux Ryzen 7950X, one worker pinned to CPU 2. Before: `93960b5c`; after: string hash membership in the LLVM text combiner. Five alternating unprofiled samples after one warmup, followed by three separately profiled alternating samples. Both binaries use the same source and output paths. Background game/Wine activity remained possible, so small differences are inconclusive.

| Project | Before build | After build | Before combine | After combine |
|---|---:|---:|---:|---:|
| wide50 | 556.4 ms | 503.2 ms | 21.9 ms | 16.5 ms |
| wide1000 | 8652.4 ms | 7235.2 ms | 1555.5 ms | 239.0 ms |
| pixels | 242.4 ms | 239.4 ms | 8.7 ms | 6.2 ms |

The 1,000-module / 24,000-function synthetic case uses 16.4% less whole-build elapsed time (1.20× speedup); profiled combination time falls by 84.6%. Output LLVM is byte-identical before/after in every run. Synthetic binaries pass their runtime checks. Pixels is compiled for output parity, not launched in this benchmark.

This measures debug binaries produced by the release Nerd compiler through LLVM tools. Combined-text phase times are intrusive profiling results, not subtractions from the unprofiled build measurements. RSS and CPU samples, binary hashes and commands are preserved in `compiler-symbol-lookup/results.json.gz`. RSS is largest individual process peak, not aggregate process-tree memory.

Original synthetic inputs remain in `/tmp/nerd-scale-bench-Xo1BUG`; the retained benchmark script references that temporary folder. These measurements do not establish release-target speedups or native Windows/macOS performance.

Validation: release rebuild and LLVM text self-test (including map growth) passed. The full debug `just test` passed: 1,122 fixtures, zero failures, nine platform skips, all scheduler/toolchain/integration checks and 280 C differential fixtures at two optimisation levels. Logs are alongside the measurements.
