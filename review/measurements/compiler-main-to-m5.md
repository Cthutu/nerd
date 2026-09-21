# Cumulative compiler comparison: main to M5

Date: 2026-09-21. Baseline **`a15e965d`** (`main`, verified against GitHub);
experiment **`4ee4f53d`** (M5; compiler implementation `81a566dd`).

## Outcome

The cumulative optimizations make a much larger difference than the final M5
slice alone. On one pinned core, Pixels builds are **4.11× faster in debug** and
**2.59× faster in release**, Quill **2.45× / 1.68×**, and Dungeon **1.29× / 1.14×**.
Front-end checking is **3.79× faster for Dungeon**, **4.25× for Pixels** and
**2.44× for Quill**. These improvements already apply at the default one job.

The extra-worker comparison below measures the incremental contribution of
scheduling separately. It does not change the decision to retain one job as the
default: most of the cumulative gain is available from single-core optimizations.

## Setup and reproducibility

- Linux x86-64, AMD Ryzen 9 7950X, 16 physical cores / 32 logical CPUs.
- Built the original `main` release compiler in `/home/matt/nerd-main-compare`
  with `just build-release nerd --skip-mod-sync`, without modifying its source.
- Installed that compiler and its library under `~/.local/bin`. The installed
  `nerd` is now **main**, as requested. The experiment remains available at
  `/home/matt/nerd/_bin/nerd`; the main worktree remains available for comparisons.
  Editor extensions were left at their existing versions.
- Preserved compiler snapshots as `/tmp/nerd-comparison-main` and
  `/tmp/nerd-comparison-experiment`. Exact binary hashes are in the raw data.
- Both compilers used identical input/output paths and the checkout's `mods`.
  There are no library or example source changes between these revisions.
- Clang, opt, llc and LLD are version 22.1.8. Main's executable generation still
  uses its historical Clang driver. The experiment uses direct LLVM tools and
  never invokes Clang. Thus full-build comparisons include the backend/toolchain
  transition as well as compiler optimizations. `nerd check` isolates front-end
  behavior and launches neither pipeline.
- One warmup and **five unprofiled samples per cell**, with variant order rotated
  each iteration. Numbers below are median command wall times, with warm file
  caches. No builds or tests ran concurrently with the benchmarks.
- First series pinned the compiler and descendants to **CPU 2**. Second series
  allowed **CPUs 0–15**, one logical CPU per physical core, and compared main
  against experiment jobs 1, 2 and 4 on the same affinity. Worker counts control
  Nerd tasks; external tools can have their own threading.
- Each build's combined LLVM was hashed on every sample. **All old/new and
  worker-count outputs matched byte for byte.** All synthetic executables
  returned zero. The installed main compiler also passed the headless Pixels
  layout/runtime smoke test.

Commands (run sequentially):

```sh
python3 build/benchmark_main.py --before /tmp/nerd-comparison-main \
  --after /tmp/nerd-comparison-experiment --cpus 2 \
  --output /tmp/nerd-main-serial.json
python3 build/benchmark_main.py --before /tmp/nerd-comparison-main \
  --after /tmp/nerd-comparison-experiment --cpus 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 \
  --after-jobs 1 2 4 --modes debug release --output /tmp/nerd-main-multicore.json
```

Raw data: [serial.json.gz](compiler-main-to-m5/serial.json.gz) and
[multicore.json.gz](compiler-main-to-m5/multicore.json.gz). Each contains exact
commands, samples, compiler hashes, affinity, LLVM hashes, process accounting and
separate timing-table observations.

## One-core front-end checking

`nerd check` includes loading, lexing, parsing, semantic checking and cleanup;
it omits HIR, LLVM generation and external tools. Times are milliseconds.

| Input | Main | Experiment | Speedup | Less elapsed time |
| --- | ---: | ---: | ---: | ---: |
| tiny | 1.19 | 0.82 | 1.45× | 31.2% |
| dungeon | 246.40 | 65.04 | 3.79× | 73.6% |
| pixels | 281.80 | 66.26 | 4.25× | 76.5% |
| quill | 21.99 | 9.00 | 2.44× | 59.1% |
| wide | 15.11 | 7.19 | 2.10× | 52.4% |
| deep | 15.42 | 7.44 | 2.07× | 51.7% |
| large | 160.71 | 46.15 | 3.48× | 71.3% |

## One-core full builds

Times are milliseconds. Debug/release specify the output program configuration;
both compiler executables themselves are release builds.

| Input | Debug main | Debug experiment | Speedup | Release main | Release experiment | Speedup |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| tiny | 33.78 | 20.24 | 1.67× | 33.38 | 28.52 | 1.17× |
| dungeon | 2186.11 | 1691.98 | 1.29× | 3438.20 | 3012.42 | 1.14× |
| pixels | 852.88 | 207.40 | 4.11× | 816.44 | 315.53 | 2.59× |
| quill | 143.81 | 58.68 | 2.45× | 133.73 | 79.71 | 1.68× |
| wide | 198.18 | 66.76 | 2.97× | 179.50 | 113.43 | 1.58× |
| deep | 199.55 | 66.18 | 3.02× | 179.37 | 112.72 | 1.59× |
| large | 413.04 | 107.10 | 3.86× | 326.64 | 82.02 | 3.98× |

## Where the compiler work decreased

One **separate** `--timing` run per cell, excluded from the latency samples.
These are indicative phase observations, not five-sample medians. On Linux the
front-end table uses thread CPU time, while back-end entries use wall time;
those quantities should not be added and called source-to-IR elapsed time.
At one job, phase sums do not overlap. Times below are milliseconds.

| Input / phase | Clock | Main | Experiment | Ratio |
| --- | --- | ---: | ---: | ---: |
| dungeon / analyse AST semantics | thread CPU | 241.68 | 59.64 | 4.05× |
| dungeon / generate HIR from sema | thread CPU | 72.32 | 1.82 | 39.65× |
| dungeon / render module LLVM | wall | 120.79 | 42.23 | 2.86× |
| dungeon / combine LLVM text | wall | 154.75 | 6.17 | 25.09× |
| pixels / analyse AST semantics | thread CPU | 275.20 | 61.20 | 4.50× |
| pixels / generate HIR from sema | thread CPU | 65.92 | 1.78 | 37.00× |
| pixels / render module LLVM | wall | 246.28 | 39.66 | 6.21× |
| pixels / combine LLVM text | wall | 159.29 | 7.16 | 22.25× |
| quill / analyse AST semantics | thread CPU | 19.25 | 6.28 | 3.07× |
| quill / generate HIR from sema | thread CPU | 3.13 | 0.51 | 6.10× |
| quill / render module LLVM | wall | 17.66 | 7.50 | 2.35× |
| quill / combine LLVM text | wall | 48.56 | 2.27 | 21.43× |

Earlier milestone reports attribute these reductions to narrower semantic scans,
source-line/function-name indexes, and less allocation and copying in LLVM
rendering/combining. The large check-only improvement independently confirms
front-end gains without the changed binary toolchain.

Dungeon's full debug build remains dominated by external code generation/linking:
the separate timing sample reports about 1.58 seconds there for main and 1.57
seconds for the experiment. Its compiler phases improved markedly, while that
remaining external cost limits the whole-build speedup.

## Additional benefit from multiple cores

Both compilers and their children can use CPUs 0–15 in this series. Times are
milliseconds. The final columns compare extra workers with the **experiment's
own one-job baseline**, not with old main. Positive reduction means faster.

| Input / target | Main | Experiment j1 | j2 | j4 | j2 vs j1 | j4 vs j1 |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| tiny / debug | 34.36 | 20.94 | 20.92 | 20.42 | +0.1% | +2.5% |
| tiny / release | 32.17 | 27.10 | 26.98 | 27.31 | +0.5% | -0.8% |
| dungeon / debug | 2189.66 | 1694.20 | 1693.28 | 1690.95 | +0.1% | +0.2% |
| dungeon / release | 3419.85 | 3002.89 | 3013.91 | 3003.82 | -0.4% | -0.0% |
| pixels / debug | 855.02 | 206.23 | 212.90 | 210.36 | -3.2% | -2.0% |
| pixels / release | 814.97 | 316.45 | 317.89 | 323.80 | -0.5% | -2.3% |
| quill / debug | 144.67 | 60.20 | 57.70 | 58.82 | +4.2% | +2.3% |
| quill / release | 134.41 | 80.71 | 82.83 | 82.17 | -2.6% | -1.8% |
| wide / debug | 198.18 | 67.54 | 65.26 | 63.60 | +3.4% | +5.8% |
| wide / release | 180.58 | 115.83 | 118.21 | 118.18 | -2.1% | -2.0% |
| deep / debug | 204.54 | 67.72 | 66.81 | 65.02 | +1.3% | +4.0% |
| deep / release | 182.42 | 116.88 | 117.91 | 118.26 | -0.9% | -1.2% |
| large / debug | 414.80 | 108.16 | 108.07 | 109.27 | +0.1% | -1.0% |
| large / release | 327.97 | 84.45 | 84.38 | 83.67 | +0.1% | +0.9% |

Parallel benefits remain much smaller and less consistent than the cumulative
single-core gains. Shared core imports currently serialize semantic checking,
and large single-module work limits available concurrency. Retain the one-job
default and treat worker counts as workload-specific options.

## Memory and limits

Median maximum-process RSS for one-core debug builds, in MiB:

| Input | Main | Experiment |
| --- | ---: | ---: |
| tiny | 65.5 | 50.2 |
| dungeon | 90.1 | 72.7 |
| pixels | 76.9 | 60.5 |
| quill | 70.0 | 54.3 |
| wide | 69.9 | 54.7 |
| deep | 70.1 | 54.7 |
| large | 71.1 | 54.7 |

This is the largest process high-water RSS, not summed live process-tree memory.
It can be dominated by Clang/LLVM, so the reduction cannot be assigned solely to
Nerd's allocator changes. Results are specific to this Linux host, warm caches,
these inputs and these tool versions; native Windows/macOS and cold-cache
comparisons were not performed. LLVM equality and synthetic runtime checks
establish benchmark output parity, not universal compiler correctness.
