# Native Windows worker sweep — 16 October 2026

Tested implementation: `05c6e0235f622d8b184f1f0e872594d42160f628`.
Release compiler, 14 workload/target cells, jobs 1/2/4/8/16/auto. Each count
has one warm-up, five unprofiled samples, and one separate profiled sample.
Counts rotate between iterations. All combined LLVM hashes match across
worker counts and profiled/unprofiled runs. No other validation builds or tests
ran alongside these measurements.

Host: Threadripper PRO 5955WX, 16 physical / 32 logical CPUs, Windows 11 Pro
build 26200, Balanced power scheme, no CPU affinity. Jobs 16 exercises the
physical-core count. Existing VS Code language servers and an idle `just run`
process were left running; background activity and frequency were not controlled.

## Median whole-build wall time (milliseconds)

| Workload | Target | jobs 1 | jobs 2 | jobs 4 | jobs 8 | jobs 16 | jobs auto |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: |
| tiny | debug | 112.62 | 112.59 | 115.99 | 111.75 | 111.97 | 107.17 |
| tiny | release | 119.17 | 120.94 | 119.57 | 120.62 | 122.00 | 125.69 |
| dungeon | debug | 2549.19 | 2517.65 | 2525.07 | 2532.67 | 2521.77 | 2535.79 |
| dungeon | release | 3710.53 | 3716.35 | 3729.83 | 3704.13 | 3724.06 | 3785.37 |
| pixels | debug | 342.35 | 337.53 | 324.83 | 321.09 | 324.35 | 319.63 |
| pixels | release | 544.30 | 536.13 | 534.06 | 537.24 | 537.49 | 527.78 |
| quill | debug | 177.17 | 173.12 | 173.81 | 178.44 | 174.72 | 173.36 |
| quill | release | 203.32 | 201.55 | 199.83 | 200.70 | 204.29 | 201.62 |
| wide | debug | 211.67 | 214.54 | 213.79 | 219.57 | 214.59 | 211.92 |
| wide | release | 273.49 | 276.94 | 273.55 | 274.95 | 283.32 | 274.36 |
| deep | debug | 216.27 | 214.32 | 212.56 | 214.03 | 213.64 | 211.10 |
| deep | release | 275.85 | 277.22 | 278.89 | 278.21 | 281.79 | 275.53 |
| large | debug | 208.85 | 200.68 | 210.19 | 200.71 | 203.17 | 204.47 |
| large | release | 141.22 | 141.12 | 138.39 | 141.31 | 139.84 | 140.53 |

## Ratios relative to jobs 1

These geometric means weight each workload/target cell equally. They summarise
this run and are not an application-frequency-weighted result.

| Jobs | Geometric mean wall ratio | Range of cell wall ratios | Maximum median compiler peak-WS ratio |
| --- | ---: | ---: | ---: |
| 2 | 0.9946 | 0.9609–1.0149 | 1.0784 |
| 4 | 0.9936 | 0.9488–1.0299 | 1.0990 |
| 8 | 0.9939 | 0.9379–1.0373 | 1.1308 |
| 16 | 0.9968 | 0.9474–1.0359 | 1.1263 |
| auto | 0.9887 | 0.9336–1.0547 | 1.1024 |

Automatic mode has approximately 1.1% lower geometric-mean wall time, with
individual cells ranging from about 6.6% lower to 5.5% higher. No selected worker
count achieves a 15% median whole-build reduction in any cell. This run provides
no basis for changing the one-job default or claiming the adoption gates pass.

The accounting sanity probe passed with nonzero CPU and peak working set.
Windows CPU and peak-working-set samples cover the Nerd compiler process only;
wall time includes LLVM/linker children. They do not establish whole-process-tree
memory/CPU bounds or long-duration Nexus resource stability. Profiling is separate
from the latency samples. No sanitiser or macOS results are claimed.

See [raw measurements](benchmark.json), [benchmark output](benchmark.log),
[accounting probe](benchmark-accounting.log), [hardware](hardware.json), and
the [scheduler adoption policy](../../../../docs/compiler-profiling.md#scheduler-adoption-policy).
