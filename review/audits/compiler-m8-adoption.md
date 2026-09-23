# M8: compiler scheduler adoption decision

Date: 2026-09-21. Compiler revision: `590105cd`; final test refinements: `c0c9fd8a`.

## Decision

Keep the measured single-core improvements and direct LLVM toolchain. Keep
`--jobs 1` as the default, and retain `--jobs N` as an explicit experiment.
Do not enable automatic multicore selection, add an uncalibrated source-size
threshold, or relax semantic closure ownership. Parallel C emission remains
deferred; compatibility C output is still produced without invoking Clang.

The Linux adoption review is complete. The cross-platform adoption gate is
**not complete**: native Windows correctness now passes; macOS execution and
Windows CPU/RSS measurements remain outstanding. See the Windows addendum below.
Default parallel adoption is therefore deferred.

## Gates

The original audit proposed at most 5% single-worker regression outside noise,
at least 15% representative whole-build improvement before default parallelism,
and at most 1.5x peak RSS at the selected default without an explicit tradeoff.
The cumulative gains against main are already available at one job; they must
not be counted as gains from extra workers. See the separate
[cumulative comparison](../measurements/compiler-main-to-m5.md).

| Gate | Result | Decision |
| --- | --- | --- |
| Single-worker cost | Fixed compiler vs M5: -1.99% to +0.93% across 14 cells; identical combined LLVM | No material regression from the correctness fix |
| Representative whole-build gain | Best real-workload improvement in this sweep is about 7.2% (Pixels debug); release builds are flat or slower | Does not clear the 15% gate |
| Memory at selected default | One worker retained; no material high-water RSS increase observed | No automatic worker/memory policy justified |
| Native-platform validation | Linux and native Windows exercised; macOS and Windows CPU/RSS remain open | Broader adoption remains gated |

## Final compiler measurements

Linux x86-64, Ryzen 9 7950X, LLVM 22.1.8. Both compilers are release builds;
debug/release below describes the generated program. One warmup and five
unprofiled samples per cell, plus a separate profile invocation. Tests/builds
finished before timing began. Serial pairs alternate order; worker counts rotate.
Each sample verifies the combined LLVM hash at identical source/output paths.

The one-core comparison pins CPU 2 and compares M5 (`81a566dd` implementation,
snapshot from the cumulative comparison) against `590105cd`. The scaling sweep
uses CPUs 0–15, one logical CPU per physical core, at jobs 1/2/4/8/16. Exact
compiler hashes, commands, affinity, samples and profiles are retained in
[serial.json.gz](../measurements/compiler-m8-adoption/serial.json.gz) and
[scaling.json.gz](../measurements/compiler-m8-adoption/scaling.json.gz).
All benchmark combined LLVM output remained byte-identical across revisions and
worker counts. These are warm-cache results on one host, not confidence bounds
or native-platform performance predictions.

```sh
python3 build/benchmark_compare.py --before /tmp/nerd-comparison-experiment \
  --cpu 2 --output /tmp/nerd-m8-serial.json
python3 build/benchmark_jobs.py --cpus 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 \
  --output /tmp/nerd-m8-scaling.json
```

Whole-command medians in milliseconds:

| Input / target | Jobs 1 | Jobs 2 | Jobs 4 | Jobs 8 | Jobs 16 |
| --- | ---: | ---: | ---: | ---: | ---: |
| tiny / debug | 20.29 | 19.72 | 20.72 | 20.16 | 20.24 |
| tiny / release | 26.91 | 25.44 | 27.15 | 25.81 | 26.15 |
| dungeon / debug | 1689.98 | 1685.94 | 1694.19 | 1687.75 | 1680.38 |
| dungeon / release | 2976.41 | 2978.13 | 2990.43 | 2990.64 | 2963.71 |
| pixels / debug | 204.51 | 197.07 | 189.88 | 189.97 | 189.76 |
| pixels / release | 313.85 | 315.59 | 323.14 | 313.51 | 315.16 |
| quill / debug | 59.62 | 59.75 | 58.15 | 57.80 | 57.65 |
| quill / release | 80.99 | 82.94 | 82.52 | 82.00 | 81.01 |
| wide / debug | 66.88 | 63.84 | 62.31 | 64.97 | 72.34 |
| wide / release | 117.32 | 116.64 | 117.03 | 116.06 | 118.99 |
| deep / debug | 66.94 | 63.47 | 66.09 | 65.80 | 71.06 |
| deep / release | 115.48 | 116.97 | 117.59 | 117.21 | 118.89 |
| large / debug | 106.53 | 106.25 | 107.13 | 106.27 | 106.07 |
| large / release | 82.62 | 84.04 | 83.33 | 82.96 | 82.85 |

Pixels debug improves 7.2% at four jobs in this run, whereas the immediately
preceding cumulative comparison had it 2.0% slower. Wide debug improves 6.8%
here, but becomes 8.2% slower at 16 jobs. None of the representative workloads
clears the default-adoption gate, and larger worker counts are not uniformly
better. Do not attribute this run-to-run variation to the generic fix: its
single-worker paired measurements show no material cost change.

Summed CPU work also matters. Pixels debug uses 205.29 ms CPU at one job versus
230.11 ms at four jobs; wide debug uses 68.44 ms versus 83.76 ms, rising to
226.44 ms at 16 jobs. CPU/wall ratios for wide are 1.02, 1.34 and 3.13 respectively:
more occupied cores can accompany a slower build. These values include the
compiler and its tooling descendants, not just semantic checking.

The separate profiles also address the source-to-IR focus. From first lexing
start through combined-IR completion, debug intervals at jobs 1/4 are Dungeon
115.20/116.03 ms, Pixels 114.04/98.83 ms and Quill 21.47/22.57 ms. These are single
profiled wall intervals, not sums of overlapping task times or repeated latency
medians. They show a useful Pixels opportunity, not a general source-to-IR win.

Median maximum-process RSS at jobs 1/4 is 72.4/72.9 MiB for Dungeon debug,
60.0/60.1 MiB for Pixels and 53.9/53.9 MiB for Quill. The apparent stability does
not establish a hard memory bound; see the accounting limitation below.

## Task granularity and memory policy

- One job executes inline; a batch with fewer than two tasks cannot start an
  extra worker. Requested jobs include the caller and are capped by available
  batch work and the CLI's limit of 256.
- Keep that policy for explicit jobs. Do not introduce a byte-count or
  function-count crossover: equal module counts in Dungeon, Pixels and Quill
  have substantially different costs and scaling. The wide synthetic workload
  does not establish a portable threshold for real source files.
- Tiny builds stay inline by default. Forcing parallelism on them is an explicit
  user choice, not an automatic policy. Source-size cutoffs alone would not fix
  shared-core checking serialization or allocator contention.
- Do not add a free-memory-based automatic worker count. Existing RSS values
  are the largest observed process high-water mark, potentially an LLVM child,
  not simultaneous process-tree memory or compiler-only live storage. They do
  not support a safe per-worker memory estimate.
- Explicit job counts remain concurrency limits, not memory limits. Parsed
  modules and rendered results can outlive their tasks; reducing worker count
  does not bound all retained storage. Automatic sizing needs live arena/result
  accounting and native-platform measurements before a budget can be promised.

## Why core cannot simply be shared read-only

The source review confirms writes on normal imported semantic paths:

1. `sema_instantiate_imported_generic_function` imports argument types into the
   source module, then emits the specialization there.
2. `sema_emit_generic_function_instantiation` interns a symbol, appends type
   parameter metadata, collects locals/scopes, temporarily changes node tables,
   and appends an instantiation with snapshots of those tables.
3. Imported trait checking also imports caller types into the source module;
   this is not limited to explicitly written generic calls.
4. The coordinator publishes the whole exclusive closure after each batch.
   Two task views borrowing the same core arrays would therefore race on both
   allocation and semantic state if the closure exclusion were removed.

Relevant code: [semantic analysis](../../src/compiler/sema/sema.c) and
[closure scheduling](../../src/compiler/build/front/program.c).
The generic bug fixed in this milestone was a serial code-generation identity
bug, not a scheduler race; it provides no justification for relaxing ownership.

A future semantic-parallelism experiment needs immutable declaration/type
snapshots plus task-owned specialization requests, canonical identities across
modules, deterministic deduplication/publication, and ordered diagnostics.
Generic and trait inference may depend on newly produced specialization results,
so a mutex around one append is not enough. Defer that redesign rather than
expanding this adoption milestone without evidence of an end-to-end benefit.

## Correctness and validation

- Full fixture run: 1,119 passed, nine skipped; the remaining two differed
  only in HIR specialization names now preserved by the fix. Updated both
  expected snapshots and reran those fixtures successfully (1,121 passing in
  total).
- The complete auxiliary `just test` sequence passed after those updates:
  allocator/thread lifecycle, profiling, render ownership, jobs, front-end
  graph/diagnostics, LLVM toolchain/doctor, installation, and 279 C/LLVM
  differential fixtures at both C optimization levels. Dungeon rendering and
  quit behavior are included in that C differential run.
- The expanded front-end suite passed under AddressSanitizer and
  ThreadSanitizer, including randomized disjoint semantic closures, failure
  cleanup and deterministic output checks.
- The release compiler passed all four generic variants for debug/release
  targets at jobs 1/4. Final regression inputs additionally use values exceeding
  32 bits, so choosing an i32 specialization cannot accidentally pass. The
  complete front-end suite was rerun with the release compiler after this
  strengthening and the portable executable-path adjustment.
- The original Linux review did not claim native Windows/macOS or new visible
  Pixels captures. The Windows addendum now records actual native results and
  captures; macOS remains unrun.

The original imported-generic reproduction now returns zero. HIR preserves the
selected specialization symbol, and LLVM resolves imported specializations by
symbol text instead of comparing module-local type IDs. Regression variants
cover an unused result followed by a return, a used result, inferred type
arguments and a specialized function stored in a local variable. LLVM and
externally compiled C results execute at jobs 1/2/4/8. See the
[reproduction and fix](imported-generic-specialization-repro.md).

| Platform | Evidence | Adoption status |
| --- | --- | --- |
| Linux x86-64 | Compiler/runtime parity, scheduler failures, sanitizer checks and pinned timings | One-job default retained; explicit jobs available |
| Native Windows | Full debug/release validation, 48 desktop cases, editor smoke and repeated worker timings | One-job default retained; CPU/RSS and paired baseline gates unmeasured |
| Native macOS | No native runner/SDK available in this session | Runtime, SDK/linker, debugger and performance gates pending |

For each native platform, build debug and release Nerd, run `nerd doctor`, the
normal regression suite, `build/test_threads.py`, `build/test_jobs.py` and
`build/test_front_threads.py` using that compiler's path. Exercise executable,
object, static/shared library and C output modes, debug/release Pixels layout and
window lifecycle, source debugger stepping, and missing-tool diagnostics. Compare
jobs 1/2/4 and physical-core count with repeated unprofiled latency samples and
separate profiles; retain output hashes and memory observations. An emulated or
cross-compiled executable would not substitute for these native measurements.

## What is accepted and what stops here

Accept the serial optimizations, ownership/diagnostic preparation, direct LLVM
pipeline and doctor, and the imported-generic correctness fix. Retain the
scheduler as opt-in infrastructure with its Linux test coverage. Stop further
worker-count tuning as a route to a default change on the current architecture.
Defer automatic worker/memory sizing, source-size thresholds, shared-core
semantic checking and parallel C rendering. Reopen adoption only after a concrete
workload clears the performance gate and native correctness validation passes.


## Native Windows addendum — 2026-09-22

Tested implementation: `828d4b56ec08ea7f2001334e33cf660da9d82bbb`.
Windows 11 Pro 10.0.26200 x64, Threadripper PRO 5955WX (16 physical cores,
32 logical processors), Balanced power plan, LLVM 22.1.8 and SDK 10.0.26100.0.
The [Windows handoff](../../validation/windows/results/HANDOFF.md) contains
environment setup, original failures, repair commits and Linux follow-up.

The [final full run](../../validation/windows/results/20260922T084515Z-f15c8c4e/SUMMARY.md)
passed 1,116 fixtures with zero failures and 15 declared platform skips, plus
all debug/release auxiliary suites. C parity covers 278 fixtures per compiler
at both O0/O2. Native desktop captures and input/resize/Q checks passed all
48 combinations of Pixels/Dungeon/Triangle, both compilers, debug/release
targets, jobs 1/4 and LLVM/C. Automated LLDB/editor checks pass; the separate
VS Code workflow has a broad user-reported smoke pass.

The desktop checks found an optimized Dungeon crash that automated tests had
missed: a 24-byte dynamic-array prefix misaligned wide enum values. Padding the
LLVM and C headers to 32 bytes repairs it; new regressions cover allocation,
growth, reserve and values under optimization. Other Windows repairs address
main return width, direct vprintf linkage, C output naming and bounded retries
for native sharing violations. No Clang fallback or default-worker change.

The [final Windows timings](../../validation/windows/results/20260922T084515Z-f15c8c4e/BENCHMARKS.md)
retain raw five-sample jobs 1/2/4/8 and separate jobs 1/16 sweeps with independent
one-worker baselines. Combined LLVM hashes match within every scenario/target.
The best representative gains are 3.2% for Pixels debug at eight workers and
4.8% at 16 in the separate sweep; neither clears the 15% adoption threshold.
Windows CPU/RSS remain null, and no Windows paired main/M5 comparison was made.
Do not interpret these omissions as passing memory or serial-regression gates.
Earlier Linux measurements above remain Linux results. macOS stays a separate
native gate; automatic parallel adoption remains deferred.

## Follow-up: adaptive defaults (2026-09-22)

The [M9–M12 plan](task-scheduler-performance.md#adaptive-performance-default--follow-up-plan-2026-09-22)
reopens automatic sizing as a bounded experiment. M9 adds opt-in `--jobs auto`
for the half-logical-CPU ceiling. M10 will measure workload thresholds and worker
reuse, and M11 will evaluate enabling automatic behavior by default. This does
not change M8's measured conclusions or the current one-job default. Semantic
ownership redesign remains a separate M12 experiment.

## Native Windows single-core follow-up — 2026-09-23

The [new Windows handoff](../../validation/windows/results/HANDOFF.md) tests
`c32eb92dfccbfc64e337b84440d9f906c33fc675`. Both fresh Clang-built compilers pass
the 6,000-term jobs 1/4 regression and full auxiliary suites; fixtures pass
1,116/0/15 (pass/fail/platform-skip). The 48-case LLVM/C desktop matrix passes.
Automated debugger/editor checks pass; fresh human VS Code observation is still
outstanding. The previous dated user smoke report is not applied to this build.

The [current worker sweep](../../validation/windows/results/20260923T084624Z-7d394bf0/BENCHMARKS.md)
covers jobs 1/2/4/8/16/auto with five unprofiled samples and LLVM identity.
New Windows process accounting passes its known CPU/memory sanity check and
records positive compiler peak working sets. **Compiler-process-only** counters
exclude LLVM/linker children; full-build wall time includes them. This improves
on the September 22 null counters without satisfying whole-process-tree memory
or paired Windows baseline gates. Earlier Linux/Windows measurements remain
dated evidence; no native macOS or Windows sanitizer pass is claimed. Retain
jobs=1 by default; M8 is not declared fully validated.
