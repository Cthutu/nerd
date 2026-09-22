# Native Windows benchmark results

Tested code: `a618456b5d0013917b22a50933f9408cedfd5dbb`. Windows 11 Pro
10.0.26200 x64; Threadripper PRO 5955WX, 16 physical cores / 32 logical
processors; Balanced power plan; LLVM 22.1.8. Fresh release Nerd compiler.
One warmup, five unprofiled samples per cell and a separate profile.
No concurrent builds, tests or desktop examples during either sweep.

Values below are whole-command wall medians in milliseconds. Each sweep has
its own one-worker baseline; do not compare jobs 16 against the earlier sweep.

## [benchmark.json](benchmark.json)

| Input / target | Jobs 1 | Jobs 2 | Jobs 4 | Jobs 8 |
| --- | ---: | ---: | ---: | ---: |
| tiny / debug | 104.52 | 105.94 | 106.58 | 110.68 |
| tiny / release | 127.38 | 124.67 | 123.19 | 122.81 |
| dungeon / debug | 2698.65 | 2681.88 | 2656.91 | 2664.08 |
| dungeon / release | 4949.18 | 4965.90 | 4978.37 | 4943.99 |
| pixels / debug | 376.46 | 363.15 | 356.92 | 359.15 |
| pixels / release | 560.40 | 554.30 | 554.39 | 555.01 |
| quill / debug | 174.90 | 175.56 | 172.50 | 172.31 |
| quill / release | 207.05 | 210.15 | 206.02 | 210.59 |
| wide / debug | 212.53 | 212.08 | 208.24 | 211.86 |
| wide / release | 287.04 | 288.17 | 286.83 | 289.19 |
| deep / debug | 239.76 | 231.59 | 229.96 | 239.90 |
| deep / release | 288.48 | 292.18 | 289.76 | 295.24 |
| large / debug | 243.03 | 244.54 | 242.93 | 245.19 |
| large / release | 185.18 | 188.18 | 186.14 | 186.01 |

## [benchmark-physical16.json](benchmark-physical16.json)

| Input / target | Jobs 1 | Jobs 16 |
| --- | ---: | ---: |
| tiny / debug | 101.73 | 104.69 |
| tiny / release | 119.43 | 121.12 |
| dungeon / debug | 2607.05 | 2605.86 |
| dungeon / release | 4879.07 | 4901.44 |
| pixels / debug | 378.25 | 364.97 |
| pixels / release | 573.31 | 567.09 |
| quill / debug | 170.86 | 172.60 |
| quill / release | 210.37 | 214.26 |
| wide / debug | 211.27 | 208.68 |
| wide / release | 274.98 | 283.25 |
| deep / debug | 211.59 | 211.47 |
| deep / release | 277.34 | 284.28 |
| large / debug | 235.24 | 237.60 |
| large / release | 184.36 | 182.36 |

The best representative gain in jobs 1/2/4/8 is about 5.2% for Pixels
debug at four workers. The separate physical-core sweep gives about 3.5%
for Pixels debug at 16 workers. Neither meets the 15% adoption threshold.
Combined LLVM hashes matched across workers within every scenario/target
in both sweeps. Keep `--jobs 1` as the default.

Windows CPU time and peak RSS remain null in this helper. This is not
evidence that the memory gate passed. There is no Windows comparison
against main or M5 here, so these measurements do not establish the
single-worker regression gate against either revision.

Commands (after applying the process-only LLVM/SDK environment):

```powershell
python validation/windows/run.py
python build/benchmark_jobs.py --nerd _bin/nerd.exe --jobs 1 16 --output validation/windows/results/20260922T075856Z-e3a49814/benchmark-physical16.json
```
