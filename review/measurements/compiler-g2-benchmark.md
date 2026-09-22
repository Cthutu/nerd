# Work-stealing compiler benchmark: original, pre-graph and G2

2026-09-22. Current implementation: `598a2d1a`, validation revision `7f08bf0e`.
Benchmark harness revision: `4a61a525`. [Plan](../audits/compiler-task-graph.md).
Raw samples, compiler hashes, commands, profiles and stdout are retained in
[compiler-g2-benchmark](compiler-g2-benchmark/).

## Outcome

Cumulative gains against original main remain substantial at one job: Pixels
4.22×/2.63× faster for debug/release builds, Quill 2.46×/1.69×, Dungeon
1.29×/1.15×. Front-end checking is 4.40× faster for Pixels and 3.99× for Dungeon.

The G1/G2 scheduler revision has no material serial regression in this run
(2.2% faster to 0.6% slower across the one-job cells). Its parallel results are
mixed and sensitive to affinity and repetitions. The initial four-worker Pixels
debug improvement of 2.8% reverses to a 6.1% regression in a longer repeat; a
four-physical-core repeat shows about 1% improvement. These results do not
establish a reliable general scheduler speedup or justify default adoption.
Retain all repetitions; do not select only the favorable first run.

G1/G2 provides the dependency/pool infrastructure and passes correctness checks.
G3 dynamic discovery and G4/G5 finer semantic dependencies are still needed to
expose the additional independent work proposed in the new plan. These benchmark
results do not establish that those future stages will improve performance.

## Method

Linux x86-64, Ryzen 9 7950X (16 physical / 32 logical CPUs), LLVM 22.1.8.
All compiler executables are release builds. Debug/release below describe the
generated target. Nine unprofiled samples per cell after one warm-up; compilers
alternate/rotate order. No compiler builds or tests run alongside measurements.
Caches are warm; frequency boost remains enabled and this is not a thermally
isolated or statistically conclusive cross-platform result.

Original main is `a15e965d`; its preserved binary SHA matches the previously
published comparison. Pre-graph is freshly built `f3af9bb1` in an isolated
worktree. Current is the G2 release compiler. All use identical source/output
paths and the current checkout's libraries; library/example sources are unchanged
from original main. The global installation is not modified.

Original/current and pre-graph/current jobs=1 comparisons are pinned to CPU 2.
Four-worker and auto comparisons use CPUs 0–15 (one logical CPU per physical
core), so auto's ceiling is eight. Every incremental comparison passes the same
jobs setting to both revisions. Do not compare absolute times across these
separate affinity/run series as if they were paired.

Whole-build timings include the external LLVM tools. Original main uses its
historical driver; current uses direct LLVM tools. `check` isolates loading,
lexing, parsing and semantics, excluding HIR/emission and external tools. Old/new
LLVM can differ because the cumulative history includes correctness changes;
the old-main harness records identity, rather than asserting it universally.
The pre-graph/current comparisons assert byte-identical combined LLVM on every
invocation. Synthetic original/current programs are also executed on warm-up.

Source-to-IR figures are medians of three separate instrumented runs per compiler
and cell, from first tokenization through combined-IR completion. They exclude
initial file opening and external tools, and must not be treated as unprofiled
whole-command medians. Single timing tables from original main are retained as
observations, not added across incompatible phase clocks. Instrumented samples
are collected as a group for each compiler, separately from the alternating
unprofiled samples; profile deltas are supporting observations, not confidence
bounds or proof of a whole-build gain.

## Cumulative gains versus original main (one core)

| Input | Check old/current ms | Check speedup | Debug old/current ms | Debug speedup | Release old/current ms | Release speedup |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| tiny | 1.19/0.80 | 1.48× | 33.06/18.92 | 1.75× | 30.33/25.40 | 1.19× |
| dungeon | 246.89/61.84 | 3.99× | 2196.98/1698.23 | 1.29× | 3446.37/3008.88 | 1.15× |
| pixels | 282.15/64.15 | 4.40× | 862.07/204.15 | 4.22× | 821.61/312.79 | 2.63× |
| quill | 21.92/9.09 | 2.41× | 145.20/59.01 | 2.46× | 134.82/79.61 | 1.69× |
| wide | 15.09/7.21 | 2.09× | 201.37/66.88 | 3.01× | 181.66/115.57 | 1.57× |
| deep | 15.41/7.39 | 2.09× | 206.21/66.91 | 3.08× | 182.29/113.92 | 1.60× |
| large | 160.27/45.30 | 3.54× | 415.23/106.67 | 3.89× | 327.32/80.49 | 4.07× |

Original/current combined LLVM differs for: dungeon/debug, dungeon/release, pixels/debug, pixels/release, quill/debug, quill/release.

## Incremental scheduler effect: One worker, CPU 2

Positive time saved means faster; negative means slower. CPU is summed user and
system time including waited-for descendants. RSS is the median maximum process
high-water mark, not simultaneous process-tree memory and not compiler-only RSS.

| Input / target | Before ms | G2 ms | Whole time saved | IR time saved | CPU change | RSS before/G2 MiB |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| tiny / debug | 17.78 | 17.84 | -0.4% | +3.4% | +0.3% | 50.1/50.3 |
| tiny / release | 23.10 | 23.12 | -0.1% | -3.1% | +0.0% | 47.5/47.5 |
| dungeon / debug | 1700.95 | 1694.53 | +0.4% | +4.1% | -0.4% | 73.7/73.8 |
| dungeon / release | 3019.10 | 2995.80 | +0.8% | +6.3% | -0.8% | 144.0/143.8 |
| pixels / debug | 206.27 | 205.52 | +0.4% | +1.2% | -0.4% | 60.4/60.5 |
| pixels / release | 313.53 | 311.55 | +0.6% | +1.7% | -0.6% | 66.1/66.1 |
| quill / debug | 58.54 | 58.91 | -0.6% | -0.6% | +0.7% | 54.3/54.3 |
| quill / release | 78.80 | 78.88 | -0.1% | -0.7% | +0.1% | 60.5/60.3 |
| wide / debug | 65.89 | 65.12 | +1.2% | +1.2% | -1.2% | 54.6/54.6 |
| wide / release | 113.49 | 113.22 | +0.2% | +1.9% | -0.3% | 50.2/50.2 |
| deep / debug | 66.19 | 66.06 | +0.2% | -0.5% | -0.3% | 54.7/54.6 |
| deep / release | 114.60 | 113.66 | +0.8% | +1.4% | -0.8% | 50.2/50.2 |
| large / debug | 108.57 | 107.87 | +0.6% | +2.9% | -0.7% | 54.7/54.7 |
| large / release | 83.20 | 81.34 | +2.2% | +3.8% | -2.3% | 47.5/47.7 |

Unprofiled sample ranges (milliseconds), provided to expose variability:

| Input / target | Before min–max | G2 min–max |
| --- | ---: | ---: |
| tiny / debug | 17.65–18.56 | 17.76–18.70 |
| tiny / release | 22.93–23.36 | 22.93–23.46 |
| dungeon / debug | 1686.22–1708.69 | 1682.40–1715.77 |
| dungeon / release | 2988.12–3033.16 | 2990.93–3025.08 |
| pixels / debug | 205.40–208.44 | 204.32–206.53 |
| pixels / release | 312.26–315.34 | 311.17–319.23 |
| quill / debug | 57.93–60.62 | 57.78–59.67 |
| quill / release | 78.31–79.36 | 78.06–79.31 |
| wide / debug | 65.33–67.34 | 65.04–66.39 |
| wide / release | 111.09–115.76 | 111.26–114.71 |
| deep / debug | 65.63–66.66 | 65.38–66.62 |
| deep / release | 112.92–115.09 | 111.57–115.68 |
| large / debug | 107.70–111.54 | 106.05–111.34 |
| large / release | 82.59–85.77 | 80.37–83.45 |

## Incremental scheduler effect: Four workers, CPUs 0–15

Positive time saved means faster; negative means slower. CPU is summed user and
system time including waited-for descendants. RSS is the median maximum process
high-water mark, not simultaneous process-tree memory and not compiler-only RSS.

| Input / target | Before ms | G2 ms | Whole time saved | IR time saved | CPU change | RSS before/G2 MiB |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| tiny / debug | 20.43 | 20.22 | +1.0% | +2.4% | -0.9% | 49.9/50.1 |
| tiny / release | 26.76 | 26.79 | -0.1% | +4.2% | -1.0% | 47.4/47.3 |
| dungeon / debug | 1684.34 | 1686.00 | -0.1% | +4.5% | -0.1% | 73.3/73.0 |
| dungeon / release | 3001.75 | 3003.30 | -0.1% | +6.4% | -0.1% | 143.7/143.8 |
| pixels / debug | 204.59 | 198.96 | +2.8% | +13.2% | -7.0% | 60.2/59.9 |
| pixels / release | 318.34 | 318.99 | -0.2% | +4.0% | +0.7% | 65.7/65.6 |
| quill / debug | 58.92 | 59.34 | -0.7% | -10.4% | +1.4% | 53.9/54.0 |
| quill / release | 82.65 | 83.79 | -1.4% | +2.1% | +2.2% | 60.1/60.0 |
| wide / debug | 63.86 | 65.45 | -2.5% | +10.9% | +5.0% | 54.5/54.4 |
| wide / release | 117.95 | 118.04 | -0.1% | +3.6% | +2.5% | 49.9/49.7 |
| deep / debug | 64.63 | 66.62 | -3.1% | -3.5% | +7.2% | 54.4/54.2 |
| deep / release | 119.48 | 117.97 | +1.3% | +5.2% | +0.2% | 49.7/49.8 |
| large / debug | 109.70 | 107.95 | +1.6% | +4.2% | -1.2% | 54.2/54.5 |
| large / release | 86.73 | 83.99 | +3.2% | +4.1% | -2.6% | 47.4/47.3 |

Unprofiled sample ranges (milliseconds), provided to expose variability:

| Input / target | Before min–max | G2 min–max |
| --- | ---: | ---: |
| tiny / debug | 19.41–21.77 | 19.54–21.29 |
| tiny / release | 26.39–28.80 | 25.97–27.67 |
| dungeon / debug | 1672.66–1716.62 | 1675.26–1707.07 |
| dungeon / release | 2981.69–3035.86 | 2979.47–3058.58 |
| pixels / debug | 189.78–207.03 | 190.49–206.64 |
| pixels / release | 312.66–325.12 | 312.15–329.89 |
| quill / debug | 56.58–59.94 | 56.22–61.79 |
| quill / release | 80.39–83.55 | 82.06–85.54 |
| wide / debug | 62.30–69.07 | 63.27–67.52 |
| wide / release | 115.57–120.75 | 115.23–120.21 |
| deep / debug | 63.44–66.67 | 63.28–67.46 |
| deep / release | 116.32–120.83 | 115.91–120.75 |
| large / debug | 108.29–110.65 | 106.94–109.84 |
| large / release | 85.64–88.11 | 82.81–85.42 |

## Incremental scheduler effect: Automatic workers, CPUs 0–15

Positive time saved means faster; negative means slower. CPU is summed user and
system time including waited-for descendants. RSS is the median maximum process
high-water mark, not simultaneous process-tree memory and not compiler-only RSS.

| Input / target | Before ms | G2 ms | Whole time saved | IR time saved | CPU change | RSS before/G2 MiB |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| tiny / debug | 19.83 | 20.28 | -2.3% | -0.0% | +1.7% | 50.0/49.8 |
| tiny / release | 26.38 | 26.61 | -0.9% | +1.9% | +1.1% | 47.3/47.2 |
| dungeon / debug | 1693.68 | 1685.05 | +0.5% | +6.1% | -1.0% | 73.0/73.1 |
| dungeon / release | 2996.65 | 2997.13 | -0.0% | +9.6% | +0.3% | 143.2/143.2 |
| pixels / debug | 194.87 | 191.93 | +1.5% | +5.6% | +0.6% | 59.9/60.1 |
| pixels / release | 315.61 | 317.31 | -0.5% | +5.0% | +2.9% | 65.7/65.5 |
| quill / debug | 60.54 | 59.60 | +1.6% | +9.5% | -2.0% | 54.1/54.3 |
| quill / release | 84.10 | 83.65 | +0.5% | +3.9% | +0.4% | 59.9/60.1 |
| wide / debug | 65.02 | 67.02 | -3.1% | -9.8% | +7.1% | 54.4/54.0 |
| wide / release | 119.63 | 117.51 | +1.8% | +1.2% | -0.7% | 49.8/49.8 |
| deep / debug | 66.11 | 64.83 | +1.9% | +2.9% | -2.0% | 54.4/54.6 |
| deep / release | 117.23 | 117.22 | +0.0% | +10.1% | -0.1% | 49.8/49.5 |
| large / debug | 109.77 | 107.17 | +2.4% | +4.3% | -2.4% | 54.4/54.6 |
| large / release | 85.99 | 83.79 | +2.6% | +3.2% | -2.7% | 47.3/47.4 |

Unprofiled sample ranges (milliseconds), provided to expose variability:

| Input / target | Before min–max | G2 min–max |
| --- | ---: | ---: |
| tiny / debug | 19.38–20.62 | 19.88–20.80 |
| tiny / release | 25.33–27.00 | 25.99–27.95 |
| dungeon / debug | 1684.32–1728.44 | 1669.51–1694.38 |
| dungeon / release | 2982.50–3038.43 | 2981.02–3009.71 |
| pixels / debug | 190.44–211.80 | 188.55–208.13 |
| pixels / release | 313.83–319.02 | 313.35–327.45 |
| quill / debug | 58.43–62.08 | 57.30–62.72 |
| quill / release | 81.21–85.39 | 80.78–87.07 |
| wide / debug | 63.47–67.75 | 62.76–68.91 |
| wide / release | 116.45–120.82 | 115.55–121.04 |
| deep / debug | 63.38–68.83 | 62.89–68.02 |
| deep / release | 116.49–120.97 | 115.72–119.75 |
| large / debug | 108.60–111.48 | 105.97–109.59 |
| large / release | 85.18–87.97 | 82.70–85.80 |

## Pixels repetition and affinity sensitivity

The first four-worker Pixels debug result had overlapping sample ranges. Repeat
with 21 unprofiled samples and seven separate profiles per compiler, first with
the same CPUs 0–15, then restricted to CPUs 0–3. Keep four workers in both cases.

| Series | Target | Before ms | G2 ms | Time saved | Before range ms | G2 range ms |
| --- | --- | ---: | ---: | ---: | ---: | ---: |
| 9 samples, CPUs 0–15 | debug | 204.59 | 198.96 | +2.8% | 189.78–207.03 | 190.49–206.64 |
| 9 samples, CPUs 0–15 | release | 318.34 | 318.99 | -0.2% | 312.66–325.12 | 312.15–329.89 |
| 21 samples, CPUs 0–15 | debug | 192.66 | 204.44 | -6.1% | 190.17–208.10 | 189.52–209.10 |
| 21 samples, CPUs 0–15 | release | 322.14 | 319.60 | +0.8% | 313.28–337.52 | 312.52–336.92 |
| 21 samples, CPUs 0–3 | debug | 188.89 | 187.08 | +1.0% | 187.13–192.42 | 186.01–189.31 |
| 21 samples, CPUs 0–3 | release | 311.05 | 310.29 | +0.2% | 309.54–313.04 | 308.66–311.27 |

With the broad affinity mask, both revisions exhibit debug timings clustered
around 190 ms and 205 ms, accompanied by different CPU totals. The tighter mask
reduces the spread substantially. Worker placement/cache interactions are a
possible explanation, not a diagnosed cause. In the four-core repeat, median
profiled source-to-IR time changes from 99.24 to 98.57 ms in debug and 85.00 to
83.49 ms in release. The larger phase deltas in the broad-mask runs should not
be presented as a stable source-to-IR speedup.

## Output checks and scope

Every pre-graph/G2 timing and profiling invocation produced byte-identical
combined LLVM, including the longer repetitions. Synthetic cumulative benchmarks
also executed successfully. These checks complement the previously recorded
full-suite/ASan/TSan validation; they do not replace native Windows/macOS testing.
The original-versus-current real-example LLVM differences are expected to include
historical correctness changes. A separate Pixels debug IR audit after all timing
runs confirms dynamic-array header offsets/allocation sizes changing from 24 to
32 bytes, consistent with `828d4b56`; the excerpt and hashes are in
`original-ir-audit.json`. Cumulative timings therefore include correctness and
toolchain changes as well as optimization; the incremental scheduler comparison
is the byte-identical comparison.

## Reproduction

Build release compilers from `a15e965d`, `f3af9bb1` and `598a2d1a` in separate
worktrees. Use the current harness with the checkout's shared modules. The raw
metadata contains each binary SHA and exact commands. Representative invocations:

```sh
python3 build/benchmark_main.py --before /path/to/original/nerd --before-ref a15e965d --after _bin/nerd --cpus 2 --samples 9 --output /tmp/original-current.json
python3 build/benchmark_compare.py --before /path/to/pre-graph/nerd --after _bin/nerd --cpu 2 --jobs 1 --samples 9 --profile-samples 3 --output /tmp/serial.json
python3 build/benchmark_compare.py --before /path/to/pre-graph/nerd --after _bin/nerd --cpus 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 --jobs 4 --samples 9 --profile-samples 3 --output /tmp/four-workers.json
```

Repeat the final command with `--jobs auto` for the automatic policy. For the
Pixels repeats use `--scenarios pixels --samples 21 --profile-samples 7`, then
reduce `--cpus` to `0 1 2 3`. Do not run builds/tests alongside timing.
