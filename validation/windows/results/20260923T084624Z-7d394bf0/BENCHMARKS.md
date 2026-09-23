# Windows worker measurements — 2026-09-23

Tested implementation: `c32eb92dfccbfc64e337b84440d9f906c33fc675`. Fresh release compiler.
Windows 11 Pro 10.0.26200 x64, Threadripper PRO 5955WX: **16 physical / 32 logical CPUs**, Balanced power plan.
One warmup and five unprofiled samples per worker count, rotating order; separate profiling invocations.
All correctness stages completed before this sweep. No concurrent agent builds/tests or desktop capture.

## Whole-build latency

Median milliseconds; percentage change compares each row with its own jobs=1 baseline.
Combined LLVM SHA-256 is identical across all worker counts, warmups, samples and separate profiles in each row.

| Scenario | Target | j1 | j2 | j4 | j8 | j16 (physical) | auto | Auto change |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| tiny | debug | 91.83 | 90.49 | 92.96 | 91.80 | 92.23 | 92.03 | +0.2% |
| tiny | release | 114.22 | 114.59 | 115.10 | 116.83 | 115.48 | 114.39 | +0.1% |
| dungeon | debug | 2505.36 | 2510.29 | 2525.15 | 2527.00 | 2500.14 | 2522.05 | +0.7% |
| dungeon | release | 4864.92 | 4869.17 | 4920.43 | 4894.52 | 4918.33 | 4893.60 | +0.6% |
| pixels | debug | 323.51 | 310.06 | 303.22 | 305.17 | 306.99 | 304.37 | -5.9% |
| pixels | release | 504.90 | 502.19 | 495.62 | 499.73 | 502.85 | 499.50 | -1.1% |
| quill | debug | 159.67 | 157.79 | 155.11 | 155.72 | 155.26 | 157.68 | -1.2% |
| quill | release | 189.95 | 190.08 | 190.03 | 191.43 | 194.13 | 191.46 | +0.8% |
| wide | debug | 195.20 | 192.99 | 191.76 | 194.00 | 195.58 | 192.10 | -1.6% |
| wide | release | 268.58 | 271.12 | 267.02 | 263.66 | 271.22 | 266.73 | -0.7% |
| deep | debug | 194.97 | 197.96 | 195.88 | 195.86 | 196.09 | 196.42 | +0.7% |
| deep | release | 285.63 | 312.33 | 291.55 | 300.05 | 296.03 | 297.24 | +4.1% |
| large | debug | 193.10 | 190.98 | 191.65 | 196.36 | 194.70 | 193.74 | +0.3% |
| large | release | 128.64 | 128.74 | 130.12 | 129.84 | 130.52 | 129.58 | +0.7% |

## Compiler CPU, memory and separate profile spans

CPU seconds and peak working set MiB are medians of the five unprofiled samples.
**Accounting scope: compiler-process-only**. These exclude LLVM/linker children; wall latency includes the full build.
They are not Linux waited-child accounting and do not establish a whole-process-tree memory gate.
The accounting sanity stage exercises known CPU work and a 32 MiB allocation; see [its log](benchmark-accounting.log).
Source-to-IR, frontend and render spans are elapsed envelopes from one separate instrumented run per cell; they are not repeated latency estimates.

| Scenario | Target | Jobs | CPU s | Peak MiB | Source→IR ms | Frontend ms | Render ms |
| --- | --- | --- | ---: | ---: | ---: | ---: | ---: |
| tiny | debug | 1 | 0.016 | 5.363 | 6.681 | 4.726 | 1.167 |
| tiny | debug | 2 | 0.016 | 5.402 | 7.169 | 0.950 | 0.950 |
| tiny | debug | 4 | 0.016 | 5.410 | 7.051 | 0.937 | 0.952 |
| tiny | debug | 8 | 0.016 | 5.398 | 7.360 | 0.949 | 0.917 |
| tiny | debug | 16 | 0.016 | 5.398 | 7.363 | 1.022 | 0.795 |
| tiny | debug | auto | 0.016 | 5.387 | 6.468 | 0.886 | 1.202 |
| tiny | release | 1 | 0.016 | 5.355 | 9.033 | 7.796 | 0.636 |
| tiny | release | 2 | 0.016 | 5.406 | 6.984 | 0.973 | 0.307 |
| tiny | release | 4 | 0.016 | 5.395 | 6.310 | 0.947 | 0.314 |
| tiny | release | 8 | 0.016 | 5.395 | 6.626 | 0.969 | 0.261 |
| tiny | release | 16 | 0.016 | 5.414 | 6.602 | 0.942 | 0.269 |
| tiny | release | auto | 0.031 | 5.379 | 5.880 | 0.901 | 0.665 |
| dungeon | debug | 1 | 0.141 | 17.594 | 145.233 | 73.511 | 62.450 |
| dungeon | debug | 2 | 0.156 | 18.129 | 141.356 | 59.931 | 56.000 |
| dungeon | debug | 4 | 0.188 | 18.297 | 141.540 | 58.744 | 53.949 |
| dungeon | debug | 8 | 0.172 | 18.387 | 142.279 | 59.398 | 53.887 |
| dungeon | debug | 16 | 0.141 | 18.426 | 145.767 | 59.874 | 56.342 |
| dungeon | debug | auto | 0.156 | 18.082 | 143.867 | 58.764 | 54.177 |
| dungeon | release | 1 | 0.125 | 16.094 | 119.793 | 73.668 | 42.287 |
| dungeon | release | 2 | 0.125 | 16.703 | 116.845 | 60.198 | 36.253 |
| dungeon | release | 4 | 0.125 | 16.848 | 116.418 | 60.052 | 36.406 |
| dungeon | release | 8 | 0.141 | 16.906 | 119.525 | 58.993 | 36.913 |
| dungeon | release | 16 | 0.125 | 17.086 | 118.034 | 59.089 | 39.019 |
| dungeon | release | auto | 0.141 | 16.676 | 115.663 | 58.845 | 36.201 |
| pixels | debug | 1 | 0.109 | 17.898 | 129.203 | 68.404 | 51.885 |
| pixels | debug | 2 | 0.125 | 18.406 | 120.319 | 55.574 | 35.413 |
| pixels | debug | 4 | 0.188 | 18.789 | 108.169 | 53.436 | 28.506 |
| pixels | debug | 8 | 0.203 | 19.020 | 114.095 | 54.163 | 30.646 |
| pixels | debug | 16 | 0.188 | 18.996 | 112.744 | 54.976 | 31.116 |
| pixels | debug | auto | 0.156 | 18.719 | 109.937 | 54.650 | 28.861 |
| pixels | release | 1 | 0.109 | 16.500 | 98.686 | 70.285 | 25.045 |
| pixels | release | 2 | 0.109 | 16.945 | 94.316 | 57.042 | 15.560 |
| pixels | release | 4 | 0.109 | 17.129 | 93.559 | 55.608 | 16.108 |
| pixels | release | 8 | 0.109 | 17.523 | 95.393 | 55.987 | 17.407 |
| pixels | release | 16 | 0.125 | 17.516 | 96.639 | 55.516 | 19.578 |
| pixels | release | auto | 0.109 | 16.969 | 93.001 | 54.689 | 17.809 |
| quill | debug | 1 | 0.047 | 10.594 | 45.070 | 28.346 | 13.371 |
| quill | debug | 2 | 0.047 | 11.035 | 43.725 | 17.537 | 8.153 |
| quill | debug | 4 | 0.047 | 11.250 | 41.868 | 17.443 | 6.371 |
| quill | debug | 8 | 0.047 | 11.336 | 42.485 | 17.551 | 6.621 |
| quill | debug | 16 | 0.062 | 11.328 | 42.832 | 17.766 | 7.004 |
| quill | debug | auto | 0.047 | 10.977 | 43.083 | 17.489 | 7.682 |
| quill | release | 1 | 0.047 | 10.340 | 36.294 | 28.510 | 5.944 |
| quill | release | 2 | 0.031 | 10.758 | 37.956 | 17.694 | 2.973 |
| quill | release | 4 | 0.047 | 10.926 | 38.366 | 18.903 | 2.662 |
| quill | release | 8 | 0.047 | 11.062 | 38.012 | 17.831 | 3.381 |
| quill | release | 16 | 0.047 | 11.121 | 38.648 | 17.925 | 4.090 |
| quill | release | auto | 0.031 | 10.688 | 37.358 | 17.529 | 3.000 |
| wide | debug | 1 | 0.047 | 8.539 | 84.110 | 49.876 | 29.202 |
| wide | debug | 2 | 0.062 | 9.098 | 84.197 | 14.863 | 20.429 |
| wide | debug | 4 | 0.094 | 9.359 | 79.275 | 14.862 | 16.011 |
| wide | debug | 8 | 0.094 | 9.562 | 82.201 | 15.450 | 17.694 |
| wide | debug | 16 | 0.109 | 9.676 | 82.208 | 16.630 | 17.072 |
| wide | debug | auto | 0.094 | 9.242 | 81.487 | 14.938 | 16.849 |
| wide | release | 1 | 0.031 | 7.945 | 62.737 | 51.965 | 9.022 |
| wide | release | 2 | 0.047 | 8.547 | 64.381 | 15.661 | 2.159 |
| wide | release | 4 | 0.047 | 8.762 | 61.921 | 15.595 | 2.103 |
| wide | release | 8 | 0.047 | 8.832 | 68.575 | 17.168 | 3.643 |
| wide | release | 16 | 0.078 | 9.043 | 69.235 | 16.608 | 7.928 |
| wide | release | auto | 0.031 | 8.562 | 64.007 | 17.025 | 2.105 |
| deep | debug | 1 | 0.062 | 8.449 | 92.253 | 59.540 | 27.783 |
| deep | debug | 2 | 0.062 | 9.023 | 86.645 | 18.346 | 19.575 |
| deep | debug | 4 | 0.078 | 9.023 | 85.550 | 17.945 | 18.298 |
| deep | debug | 8 | 0.094 | 9.160 | 84.660 | 18.366 | 18.016 |
| deep | debug | 16 | 0.125 | 9.254 | 86.138 | 19.003 | 17.930 |
| deep | debug | auto | 0.094 | 9.062 | 86.180 | 18.377 | 16.585 |
| deep | release | 1 | 0.047 | 7.859 | 67.562 | 57.032 | 9.182 |
| deep | release | 2 | 0.062 | 8.395 | 68.719 | 19.851 | 2.220 |
| deep | release | 4 | 0.047 | 8.453 | 73.147 | 20.527 | 2.046 |
| deep | release | 8 | 0.047 | 8.422 | 66.537 | 18.616 | 3.180 |
| deep | release | 16 | 0.188 | 8.512 | 71.552 | 18.034 | 9.104 |
| deep | release | auto | 0.062 | 8.438 | 66.612 | 18.538 | 1.905 |
| large | debug | 1 | 0.047 | 10.891 | 39.438 | 13.668 | 20.945 |
| large | debug | 2 | 0.047 | 11.332 | 44.962 | 9.957 | 25.051 |
| large | debug | 4 | 0.047 | 11.281 | 43.216 | 10.295 | 23.116 |
| large | debug | 8 | 0.047 | 11.336 | 40.233 | 10.121 | 19.818 |
| large | debug | 16 | 0.062 | 11.336 | 44.694 | 9.654 | 24.729 |
| large | debug | auto | 0.047 | 10.945 | 41.881 | 10.040 | 23.283 |
| large | release | 1 | 0.031 | 10.277 | 17.486 | 13.411 | 2.347 |
| large | release | 2 | 0.031 | 10.621 | 18.204 | 10.025 | 1.876 |
| large | release | 4 | 0.016 | 10.625 | 17.954 | 10.339 | 1.720 |
| large | release | 8 | 0.031 | 10.621 | 17.377 | 9.957 | 1.671 |
| large | release | 16 | 0.031 | 10.621 | 17.181 | 9.813 | 1.649 |
| large | release | auto | 0.031 | 10.309 | 17.097 | 9.535 | 2.362 |

## Interpretation

Best measured gain is Pixels debug at jobs=4: 6.3% (323.51 to 303.22 ms). Auto ranges from 5.9% faster (Pixels debug) to 4.1% slower (deep release).
The largest numeric-worker slowdown is deep release at jobs=2: 9.3%. No row reaches the 15% adoption improvement threshold.
Retain the one-worker default. This is a current-revision worker sweep, not a paired before/after serial-optimization experiment.
Explicit jobs=16 covers the physical-core count. Auto has a half-logical-CPU ceiling of 16 on this host; individual phase budgets follow workload policy.
Inspect scheduler-policy records in the [raw results](benchmark.json) for decisions; the ceiling is not a promise of 16 active workers.
macOS, Windows native sanitizers and a whole-process-tree memory comparison remain outside this run.
