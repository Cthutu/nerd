# Native Windows benchmark results

Tested code: `828d4b56ec08ea7f2001334e33cf660da9d82bbb`. Windows 11 Pro
10.0.26200 x64; Threadripper PRO 5955WX, 16 physical cores / 32 logical
processors; Balanced power plan; LLVM 22.1.8. Fresh release Nerd compiler.
One warmup, five unprofiled samples per cell and a separate profile.
No concurrent builds, tests or desktop examples during either sweep.

Values below are whole-command wall medians in milliseconds. Each sweep has
its own one-worker baseline; do not compare jobs 16 against the earlier sweep.

## [benchmark.json](benchmark.json)

| Input / target | Jobs 1 | Jobs 2 | Jobs 4 | Jobs 8 |
| --- | ---: | ---: | ---: | ---: |
| tiny / debug | 113.74 | 106.35 | 107.18 | 107.51 |
| tiny / release | 130.33 | 138.59 | 136.03 | 132.47 |
| dungeon / debug | 2733.68 | 2848.39 | 2896.60 | 2777.33 |
| dungeon / release | 5174.99 | 5027.58 | 5054.65 | 5200.87 |
| pixels / debug | 371.52 | 360.18 | 360.60 | 359.61 |
| pixels / release | 554.44 | 552.67 | 557.44 | 554.70 |
| quill / debug | 168.65 | 166.62 | 167.16 | 164.43 |
| quill / release | 205.98 | 204.79 | 210.31 | 210.03 |
| wide / debug | 221.01 | 224.79 | 226.95 | 230.51 |
| wide / release | 274.22 | 273.10 | 273.58 | 276.63 |
| deep / debug | 206.80 | 207.39 | 200.89 | 208.13 |
| deep / release | 274.54 | 276.47 | 276.14 | 278.36 |
| large / debug | 233.96 | 242.71 | 238.22 | 237.97 |
| large / release | 176.54 | 174.65 | 173.33 | 176.18 |

## [benchmark-physical16.json](benchmark-physical16.json)

| Input / target | Jobs 1 | Jobs 16 |
| --- | ---: | ---: |
| tiny / debug | 93.14 | 93.05 |
| tiny / release | 112.20 | 116.62 |
| dungeon / debug | 2701.58 | 2779.30 |
| dungeon / release | 5311.65 | 5307.89 |
| pixels / debug | 388.60 | 370.09 |
| pixels / release | 582.32 | 573.98 |
| quill / debug | 167.36 | 166.15 |
| quill / release | 194.40 | 199.46 |
| wide / debug | 202.81 | 203.88 |
| wide / release | 266.99 | 272.29 |
| deep / debug | 205.07 | 206.23 |
| deep / release | 269.05 | 275.25 |
| large / debug | 230.77 | 231.09 |
| large / release | 187.44 | 186.16 |

The best representative gain in jobs 1/2/4/8 is 3.2% for Pixels debug at eight
workers. The separate physical-core sweep gives 4.8% for Pixels debug at 16
workers. Neither meets the 15% adoption threshold; other workloads are largely
flat or slower. Review each sweep against its own baseline. These are warm-cache
measurements on one host, not confidence bounds or predictions for other hosts.
Combined LLVM hashes matched across workers within every scenario/target
in both sweeps. Keep `--jobs 1` as the default.

Windows CPU time and peak RSS remain null in this helper. This is not
evidence that the memory gate passed. There is no Windows comparison
against main or M5 here, so these measurements do not establish the
single-worker regression gate against either revision.

Commands (after applying the process-only LLVM/SDK environment):

```powershell
python validation/windows/run.py
python build/benchmark_jobs.py --nerd _bin/nerd.exe --jobs 1 16 --output validation/windows/results/20260922T084515Z-f15c8c4e/benchmark-physical16.json
```
