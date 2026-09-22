# Single-core completion benchmark

2026-09-22, Linux x86-64, Ryzen 7950X, LLVM 22.1.8. Final compiler implementation: `6dd4b36e`. Original compiler: `a15e965d`; turn-start compiler: `990cfb5a`.

The remaining arithmetic stack failures in semantic inference and HIR lowering are fixed for the 6,000-term regression. Semantic scope queries use existing parser side tables instead of repeated whole-AST scans. The withdrawn dependency scheduler stays removed; the established batch scheduler and opt-in worker policy remain.

Linux correctness: 1,122 fixtures, zero failures, nine platform skips; 280 C differential cases at two optimisation levels; 6,000-term debug/release/ASan/TSan checks with jobs=1/4; frontend output/diagnostic/runtime parity and Windows-runner unit tests. Native Windows/macOS execution is still pending.

## Conclusions

- Compared with original main, one-worker debug builds are 1.33× faster for
  Dungeon, 5.16× for Pixels and 2.66× for Quill. Release builds improve by
  1.16×, 2.99× and 1.78× respectively.
- This pass alone reduces Pixels debug build time by 18.0% and the synthetic large-module
  debug/release times by 39.5%/52.3%. The gains come from
  removing semantic work, without changing emitted LLVM.
- The largest heavy stress case (24,000 functions in four modules) falls from
  90.29s to 31.56s with one worker, or 29.47s with four. The 1,000-module wide
  case falls from 6.07s to 5.75s with one worker, or 5.22s with four.
- Extra workers help selected shapes, but auto still fails the representative
  adoption gate. Keep omitted jobs at one and retain the simpler batch scheduler.
  These results do not justify restoring the dependency scheduler.

## Method

Original and turn-start comparisons use one worker pinned to CPU 2, one warmup and seven alternating/rotating unprofiled samples per cell. Turn-start phase envelopes use three separate intrusive profile samples. The worker sweep uses five samples and one separate profile; scaled inputs use three samples and one profile. No compiler builds or tests ran concurrently with final timing runs. Existing Wine/desktop processes were left running, so small differences remain inconclusive. The governor was `performance`; frequency/SMT-sibling activity was not controlled.

Worker/scaled comparisons share a mask of 16 physical cores (CPU IDs 0–15), so `auto` has an eight-worker ceiling. This mask differs from normal unrestricted 32-logical-CPU discovery. The actual automatic phase budget can be lower than its ceiling. All timing tables are medians. Speedup = before / after.

CPU samples include waited child processes on this Linux host. RSS is the largest individual process peak, not aggregate process-tree memory. Profiles measure elapsed first frontend phase to completed LLVM combination; external LLVM tool time is excluded from that envelope.

## Original compiler → final, one worker

| Project | Check before → final (ms) | Debug build before → final (ms) | Release build before → final (ms) | Debug / release speedup |
|---|---:|---:|---:|---:|
| tiny | 1.18 → 0.73 | 33.25 → 19.48 | 30.89 → 25.25 | 1.71× / 1.22× |
| dungeon | 247.09 → 25.49 | 2187.26 → 1649.16 | 3421.83 → 2962.39 | 1.33× / 1.16× |
| pixels | 281.60 → 28.13 | 858.85 → 166.44 | 823.75 → 275.73 | 5.16× / 2.99× |
| quill | 22.04 → 5.63 | 143.87 → 54.10 | 133.90 → 75.03 | 2.66× / 1.78× |
| wide | 15.10 → 4.33 | 198.96 → 62.90 | 181.66 → 110.14 | 3.16× / 1.65× |
| deep | 15.41 → 4.53 | 204.58 → 63.19 | 183.25 → 112.13 | 3.24× / 1.63× |
| large | 161.02 → 5.83 | 415.16 → 64.97 | 326.00 → 38.34 | 6.39× / 8.50× |

This cumulative comparison includes earlier correctness fixes and the change from the historical Clang subprocess backend to direct LLVM tools. The final Nerd compiler never invokes Clang for normal builds. Original/final real-example LLVM differs; synthetic stability and runtime results are recorded separately. Do not attribute all cumulative gains to this pass.

### Real-project CPU and peak-process memory

| Project | Mode | Original CPU s / RSS MiB | Final CPU s / RSS MiB |
|---|---|---:|---:|
| dungeon | check | 0.246 / 24.1 | 0.025 / 24.1 |
| dungeon | debug | 2.174 / 90.1 | 1.644 / 73.8 |
| dungeon | release | 3.408 / 167.6 | 2.954 / 143.8 |
| pixels | check | 0.281 / 24.1 | 0.028 / 24.1 |
| pixels | debug | 0.849 / 76.9 | 0.165 / 60.5 |
| pixels | release | 0.817 / 88.2 | 0.274 / 66.1 |

## This pass: turn start → final, one worker

| Project | Target | Before → final build (ms) | Speedup | Before → final source-to-IR (ms) |
|---|---|---:|---:|---:|
| tiny | debug | 22.35 → 21.73 | 1.03× | 1.48 → 1.44 |
| tiny | release | 29.94 → 29.11 | 1.03× | 1.03 → 0.97 |
| dungeon | debug | 1784.78 → 1739.65 | 1.03× | 119.01 → 80.08 |
| dungeon | release | 3013.70 → 2993.84 | 1.01× | 88.42 → 49.53 |
| pixels | debug | 203.43 → 166.84 | 1.22× | 113.39 → 77.18 |
| pixels | release | 311.24 → 274.45 | 1.13× | 85.48 → 48.81 |
| quill | debug | 57.99 → 54.38 | 1.07× | 20.79 → 17.54 |
| quill | release | 77.96 → 74.70 | 1.04× | 14.26 → 11.20 |
| wide | debug | 65.64 → 62.47 | 1.05× | 26.91 → 23.91 |
| wide | release | 113.53 → 110.27 | 1.03× | 13.39 → 10.41 |
| deep | debug | 65.25 → 62.42 | 1.05× | 27.02 → 24.00 |
| deep | release | 114.90 → 110.69 | 1.04× | 13.62 → 10.61 |
| large | debug | 106.36 → 64.31 | 1.65× | 64.37 → 23.08 |
| large | release | 82.48 → 39.33 | 2.10× | 51.65 → 9.61 |

Every before/after LLVM comparison in this matrix is byte-identical. The previous single-module regression is evaluated here against the turn-start compiler, which already included the LLVM-depth fix. The additional scope-query changes remove substantially more work than that historical regression introduced.

## Final compiler worker sweep

| Project | Target | 1 | 2 | 4 | 8 | 16 | auto |
|---|---|---:|---:|---:|---:|---:|---:|
| tiny | debug | 19.98 | 20.38 | 20.58 | 20.04 | 20.29 | 20.11 |
| tiny | release | 27.26 | 27.03 | 27.21 | 27.31 | 26.69 | 26.66 |
| dungeon | debug | 1654.94 | 1659.11 | 1666.16 | 1646.25 | 1645.37 | 1655.19 |
| dungeon | release | 2950.78 | 2962.23 | 2960.69 | 2950.96 | 2959.47 | 2971.01 |
| pixels | debug | 167.72 | 156.39 | 165.94 | 152.22 | 152.80 | 153.30 |
| pixels | release | 278.36 | 277.37 | 285.28 | 277.02 | 277.12 | 278.50 |
| quill | debug | 56.39 | 54.90 | 54.60 | 53.73 | 53.62 | 55.85 |
| quill | release | 78.90 | 78.54 | 79.40 | 78.74 | 78.09 | 79.24 |
| wide | debug | 63.88 | 60.94 | 62.29 | 62.28 | 66.69 | 61.03 |
| wide | release | 111.85 | 114.57 | 115.17 | 113.07 | 116.66 | 113.53 |
| deep | debug | 63.79 | 62.32 | 60.59 | 62.50 | 67.08 | 63.10 |
| deep | release | 111.50 | 113.97 | 113.27 | 112.95 | 114.30 | 115.00 |
| large | debug | 65.45 | 65.17 | 65.17 | 65.67 | 65.44 | 64.37 |
| large | release | 41.07 | 41.69 | 41.29 | 42.06 | 41.81 | 42.09 |

Values are whole-build milliseconds. All worker variants must emit identical LLVM. Small median differences are not an adoption argument; omitted jobs remains one unless the established representative gain and native-platform gates pass.

## Scaled temporary projects

Each unit supplies 24 functions using records, explicit generic calls and arithmetic. Wide imports independent modules; shared imports a common foundation; chain imports the next module; heavy puts the same total function count into four modules. Every function contributes to a checked runtime result. Heavy inputs deliberately include deep arithmetic aggregates; they are stress cases, not representative real projects.

| Units | Shape | Source files | Before j1 (ms) | Final j1 | Final j4 | Final auto | Single-worker speedup |
|---:|---|---:|---:|---:|---:|---:|---:|
| 50 | wide | 51 | 261.4 | 248.8 | 256.3 | 228.0 | 1.05× |
| 50 | shared | 52 | 260.4 | 244.2 | 255.4 | 258.2 | 1.07× |
| 50 | chain | 51 | 260.1 | 248.9 | 253.0 | 257.5 | 1.05× |
| 50 | heavy | 5 | 445.8 | 297.2 | 266.4 | 276.6 | 1.50× |
| 200 | wide | 201 | 1033.9 | 984.4 | 875.9 | 1063.1 | 1.05× |
| 200 | shared | 202 | 1018.5 | 965.2 | 868.4 | 1040.9 | 1.06× |
| 200 | chain | 201 | 1034.9 | 981.0 | 972.5 | 1068.7 | 1.05× |
| 200 | heavy | 5 | 4344.0 | 1969.6 | 1818.4 | 1803.8 | 2.21× |
| 1000 | wide | 1001 | 6065.4 | 5753.5 | 5218.9 | 5916.5 | 1.05× |
| 1000 | shared | 1002 | 5906.4 | 5623.5 | 5164.5 | 6006.3 | 1.05× |
| 1000 | chain | 1001 | 5381.5 | 5106.7 | 4631.5 | 5526.2 | 1.05× |
| 1000 | heavy | 5 | 90293.4 | 31563.5 | 29471.6 | 29575.8 | 2.86× |

The scaled baseline is the turn-start compiler, not original main. Every successful build is executed and checked, and LLVM is required to match across compilers and worker counts. Sources remain under `/tmp/nerd-scale-bench-Xo1BUG`; the archived generator recreates them. Per-file source hashes are in the raw scaled results.

### Largest-project resource comparison

| Shape (1,000 units) | Before j1 CPU s / RSS MiB | Final j1 | Final j4 | Final auto |
|---|---:|---:|---:|---:|
| wide | 6.05 / 363.7 | 5.74 / 363.5 | 6.73 / 362.8 | 13.32 / 363.7 |
| shared | 5.89 / 354.7 | 5.61 / 355.1 | 6.74 / 355.3 | 14.31 / 355.2 |
| chain | 5.36 / 370.4 | 5.09 / 370.4 | 6.16 / 370.6 | 13.84 / 371.1 |
| heavy | 90.07 / 2608.7 | 31.49 / 2606.8 | 32.07 / 2612.4 | 32.13 / 2616.4 |

On the 1,000-module wide/shared/chain inputs, auto consumes about 2.3–2.7×
as much CPU as one worker without reducing elapsed time. Four workers improve
latency with a smaller CPU increase. Heavy-case peak process memory remains
about 2.55 GiB; this pass does not materially reduce its memory requirement.

## Evidence and remaining gates

Raw JSON, logs, commands, compiler hashes, tool versions, the scaled generator and scripts are archived under `compiler-single-core-final/` as gzip files. `provenance.json.gz` records the exact final timing commands. The active [completion plan](../audits/compiler-single-core-follow-up.md) tracks the milestones. Native Windows/macOS validation remains external; neither the default worker policy nor the withdrawn task-graph decision is changed by these Linux synthetic results.
