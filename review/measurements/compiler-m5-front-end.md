# M5: scheduled module front end

Date: 2026-09-20. Linux x86-64; Ryzen 9 7950X (16 physical cores).
Baseline: `0a5b6648`. Implementation: `81a566dd`, preceded by `0661fca0`
(HIR scheduling) and `9f6ed1c1` (sibling parsing). All three commits are on
`experiment/task-scheduler-performance`.

## Result and scope

M5 is complete for the Linux experiment: parallel sibling parsing, a stable
module registry between discovery and checking, dependency-ready semantic
scheduling, parallel HIR, ordered diagnostics/results, and graph/concurrency
regressions. M4's Linux CLI/output/runtime/failure gates also pass. This is not
an adoption recommendation: default `--jobs` remains **1**, and native Windows
and macOS validation, memory-budget worker sizing, and thresholds remain M8 work.

Semantic checking conservatively claims each module's complete transitive
import closure, including implicit core. Imported generic instantiation really
mutates dependency types, symbols and specialization arrays. Disjoint closures
can run concurrently; shared closures retain the old DFS check order. In typical
bundled-library programs, shared core currently serializes checking. This is a
known limit of this implementation, not evidence that semantic parallelism is
impossible. A future finer-grained design must isolate or deterministically
merge those mutations before permitting more overlap.

The measured end-to-end results are mixed. Default one-job performance is
essentially unchanged. Additional front-end concurrency does not consistently
improve the existing four-job LLVM scheduler, and Pixels debug regressed in the
paired run. These results do not meet the 15% representative whole-build gain
proposed for default adoption. Keep the experiment opt-in.

## Method

`build/benchmark_compare.py` alternated the baseline and new release compiler,
with one warmup and five unprofiled samples per compiler/input/target. Serial
comparison used CPU 2; the jobs=4 comparison used CPUs 0–15 (one logical CPU per
physical core). `build/benchmark_jobs.py` rotated jobs 1/2/4/8/16, with one warmup
and five samples per count on CPUs 0–15. Each series also recorded separate
ordinary profiles. Benchmarks ran sequentially, after tests/builds finished.

Every run checked combined LLVM hashes at identical source/output paths. Raw
commands, compiler hashes, CPU affinity, timings, profiles and memory observations
are in [serial.json.gz](compiler-m5-front-end/serial.json.gz),
[parallel.json.gz](compiler-m5-front-end/parallel.json.gz), and
[scaling.json.gz](compiler-m5-front-end/scaling.json.gz).

## Paired timings

Medians in milliseconds; positive change means slower.

| Input / target | One job before | One job after | Change | Four jobs before | Four jobs after | Change |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| tiny / debug | 18.65 | 18.15 | -2.6% | 20.77 | 20.28 | -2.3% |
| tiny / release | 22.79 | 22.59 | -0.9% | 26.44 | 26.78 | +1.3% |
| dungeon / debug | 1687.73 | 1685.53 | -0.1% | 1683.12 | 1685.03 | +0.1% |
| dungeon / release | 2997.73 | 2981.65 | -0.5% | 2993.90 | 2986.09 | -0.3% |
| pixels / debug | 205.36 | 205.41 | +0.0% | 193.53 | 210.36 | +8.7% |
| pixels / release | 314.32 | 314.73 | +0.1% | 315.54 | 319.39 | +1.2% |
| quill / debug | 58.99 | 58.65 | -0.6% | 59.72 | 58.00 | -2.9% |
| quill / release | 79.48 | 79.09 | -0.5% | 82.08 | 83.45 | +1.7% |
| wide / debug | 66.29 | 66.36 | +0.1% | 62.37 | 63.57 | +1.9% |
| wide / release | 114.25 | 115.43 | +1.0% | 117.69 | 118.71 | +0.9% |
| deep / debug | 66.04 | 65.93 | -0.2% | 63.47 | 66.31 | +4.5% |
| deep / release | 114.96 | 114.89 | -0.1% | 116.02 | 118.16 | +1.8% |
| large / debug | 105.42 | 106.34 | +0.9% | 106.48 | 108.18 | +1.6% |
| large / release | 79.43 | 79.93 | +0.6% | 83.54 | 84.29 | +0.9% |

The four-job Pixels debug medians were 193.53 → 210.36 ms (+8.7%). The five
samples ranged 191.95–212.92 ms before and 194.60–211.78 ms after, so the median
shift accompanies substantial scheduling variability. The single profiled run
does not isolate a cause. Do not attribute the difference solely to parsing or
claim a speedup from summed overlapping phase times.

## Worker-count scaling

Medians in milliseconds for the new compiler, on the same 16-CPU affinity.

| Input / target | Jobs 1 | Jobs 2 | Jobs 4 | Jobs 8 | Jobs 16 |
| --- | ---: | ---: | ---: | ---: | ---: |
| tiny / debug | 20.61 | 20.06 | 20.18 | 20.69 | 20.63 |
| tiny / release | 26.32 | 26.51 | 26.26 | 26.77 | 26.60 |
| dungeon / debug | 1685.54 | 1683.40 | 1684.60 | 1684.36 | 1677.84 |
| dungeon / release | 2978.00 | 2983.81 | 2988.30 | 2976.90 | 2975.19 |
| pixels / debug | 205.82 | 193.99 | 210.29 | 192.56 | 193.44 |
| pixels / release | 314.51 | 329.44 | 320.48 | 316.30 | 314.20 |
| quill / debug | 59.55 | 57.86 | 58.22 | 58.99 | 58.94 |
| quill / release | 80.47 | 82.87 | 82.96 | 81.88 | 81.76 |
| wide / debug | 66.93 | 66.84 | 66.73 | 65.67 | 71.46 |
| wide / release | 114.95 | 117.40 | 119.15 | 117.86 | 118.47 |
| deep / debug | 66.88 | 65.16 | 64.05 | 65.89 | 72.22 |
| deep / release | 115.54 | 116.44 | 118.41 | 118.36 | 120.37 |
| large / debug | 107.22 | 107.22 | 107.02 | 108.36 | 107.30 |
| large / release | 83.10 | 84.17 | 84.51 | 84.46 | 83.94 |

## Memory and measurement limits

Paired runs use `max_process_rss_bytes`: the largest process high-water RSS,
not simultaneous process-tree memory, and potentially dominated by an LLVM tool.
It does not measure reserved virtual address space. Private parse-source arenas
remain alive with their adopted modules; semantic views shallow-copy records
while exclusively borrowing the closure's arrays. Neither is a whole-program
deep copy. Worker count is capped by ready work; no memory-budget auto-sizing
has been added.

| Input / target | Four-job RSS before (MiB) | After (MiB) |
| --- | ---: | ---: |
| tiny / debug | 49.8 | 49.9 |
| tiny / release | 47.2 | 47.3 |
| dungeon / debug | 72.8 | 71.6 |
| dungeon / release | 143.6 | 144.3 |
| pixels / debug | 60.3 | 60.0 |
| pixels / release | 65.7 | 65.2 |
| quill / debug | 53.8 | 53.5 |
| quill / release | 59.9 | 60.0 |
| wide / debug | 54.2 | 54.4 |
| wide / release | 49.7 | 49.8 |
| deep / debug | 54.3 | 54.2 |
| deep / release | 49.7 | 49.6 |
| large / debug | 54.4 | 54.5 |
| large / release | 47.4 | 47.4 |

Front-end profiles retain their original publication order, but `start_ns`
reflects actual execution. Discovery completes before checking, and HIR starts
after checking completes. Profile publication itself now happens after the
scheduled front end, so before/after phase envelopes are not a substitute for
unprofiled command timings. On a failed parallel attempt, only the serial retry's
profiles/diagnostics are published; total command latency includes the attempt.

## Validation

- `just test`: 1,121 compiler/command/tooling fixtures passed, 9 skipped; allocator,
  worker lifecycle, profiling, render ownership, production jobs, direct LLVM,
  doctor and installation checks passed. All 279 C/LLVM differential fixtures
  passed at both C optimization levels.
- `build/test_front_threads.py`: debug/release LLVM and C identity, root HIR
  sidecar identity, real Dungeon/Pixels/Quill sources, diamond and duplicate
  imports, folder parts, conditional/missing/cyclic imports, shared FFI symbols,
  imported generics, ordered diagnostics and cleanup. Invalid inputs include
  an earlier semantic error followed by a later sibling parse error.
- The complete new front-end suite passed under ThreadSanitizer and
  AddressSanitizer. Randomized body sizes/job counts exercise disjoint imported
  generic closures and different compile-time specializations, with runtime
  checks and observed overlap in lexing, parsing, sema and HIR.
- Successful multi-module profiles verify discovery/check separation instead of
  relying silently on the serial error fallback. The ordinary serial path
  remains in use for custom source callbacks, partial results, verbose dumps
  and legacy process-wide memory profiling.
- Debugger stepping passed with jobs=4. Actual Pixels binaries stayed alive and
  exited cleanly on Escape in debug/release at jobs=1/4. The Xwayland compositor
  marked the windows hidden, so XGetImage returned BadMatch: this run did **not**
  repeat the earlier M4 pixel-color/visible-render verification.
- Nerd's executable path still invokes LLVM tools directly, never Clang.
  Standalone C emission remains the compatibility path; its renderer is serial.

## Additional correctness finding

Fixture development exposed an existing imported-generic specialization issue:
using two integer specializations from sibling modules can lose an explicit
return. The minimal [reproduction and baseline comparison](../audits/imported-generic-specialization-repro.md)
records the incorrect exit status with both `0a5b6648` and M5 at one job. This
issue was subsequently fixed in `590105cd` during the
[M8 adoption review](../audits/compiler-m8-adoption.md), with runtime regressions
for distinct imported specializations, inference and function values.

## Next decision

M6 parallel C emission remains deferred. M8 should first address whether useful
semantic work can safely share read-only core metadata, how specialization
mutations would be ordered, and whether worker thresholds or stopping further
parallelization is warranted by the measured cost. Remaining LLVM debug-name
arena work is a separate possible single-core optimization, not an unfulfilled
M5 implementation item. Preserve the one-job default until measured gains and
native-platform validation justify changing it.
