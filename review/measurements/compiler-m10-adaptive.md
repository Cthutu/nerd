# Adaptive dispatch: Linux calibration and default decision

2026-09-22. Implementation starts at `45f8e8f5`; measurement tooling and native
handoff are in `53a6cf5f`. Raw evidence is in [compiler-m10-adaptive](compiler-m10-adaptive/).
Compiler hashes, arguments, samples and policy records are retained in each JSON.

## Outcome

Work-aware `--jobs auto` is implemented as an opt-in experiment. Keep omitted
jobs at one. The Linux M11 adoption gate fails: the measured real-workload gains
do not reach 15%, CPU consumption commonly rises, and some release workloads
regress. Passing correctness checks cannot turn this into an adoption pass.
Native Windows/macOS checks and Windows whole-tree CPU/memory remain open.

## Method and policy calibration

Release-built Nerd on Linux/Ryzen 7950X, LLVM 22.1.8. The main runs restrict the
process to logical CPUs 0–15 (one thread per physical core), giving auto a ceiling
of eight. Worker counts 1/2/4/8/16/auto rotate each iteration, with one warm-up and
five unprofiled samples per cell. Combined LLVM bytes must match in every cell.
Debug and release refer to the generated target, not the compiler. Benchmarks
run without concurrent builds/tests; no governor or thermal isolation is claimed.
Single separate profiled samples provide phase spans, not reliable latency
medians. Source-to-IR spans start at first tokenization and end after combining
LLVM, excluding initial file opening and subsequent LLVM tools.

Policy A uses grains of 65,536 source bytes, 8,192 AST nodes and 512 HIR nodes.
Policy B uses 16,384 / 2,048 / 1,024. Both cap work by count, CPU ceiling and work
outside the largest task. B is retained for further opt-in testing: fewer render
workers reduce some over-dispatch costs, while moderate parse/HIR batches become
eligible. This is an empirical candidate, not a proven optimal universal cutoff.
The exact crossover depends on phase, graph shape and target configuration.

For the default 16-module/48-functions-per-module synthetic input, B dispatches
more front-end work and less render work than A. With four functions per module,
small batches stay inline; nine-sample repeat regressions are 0.4–1.8% for wide
and deep, and tiny debug is 3.9% slower (0.90 ms). With 256 functions per module,
B wide debug improves 3.5%, wide release 2.3%, deep debug 3.2%, while deep release
regresses 2.1%. A also has mixed results: large deep debug improves 8.3% but wide
release regresses 5.9%. These separate runs drift in their serial baselines;
compare each policy with its own paired serial samples, not their absolute times.
No stable graph-independent crossover or 15% representative gain was found.

## Policy B: paired medians

Positive gain means faster. CPU is total user+system time including waited-for
LLVM/linker descendants. RSS is the maximum individual process peak reported by
wait4, not the simultaneous process-tree peak or a memory-budget guarantee.

| Input | Target | Serial ms | Auto ms | Gain | CPU change | Serial/auto RSS MiB |
| --- | --- | ---: | ---: | ---: | ---: | ---: |
| tiny | debug | 23.13 | 23.63 | -2.1% | +1.6% | 50.0/49.6 |
| tiny | release | 30.63 | 30.15 | +1.6% | -1.8% | 47.3/47.5 |
| pixels | debug | 226.22 | 213.37 | +5.7% | +20.9% | 60.0/59.7 |
| pixels | release | 325.63 | 329.59 | -1.2% | +13.7% | 65.2/65.4 |
| quill | debug | 64.90 | 65.03 | -0.2% | +12.5% | 53.8/53.8 |
| quill | release | 87.56 | 87.21 | +0.4% | +2.8% | 60.1/60.2 |
| wide | debug | 72.21 | 69.67 | +3.5% | +19.5% | 54.2/54.4 |
| wide | release | 122.77 | 124.45 | -1.4% | +6.3% | 49.5/49.8 |
| deep | debug | 72.47 | 70.15 | +3.2% | +22.2% | 54.4/54.3 |
| deep | release | 123.67 | 125.07 | -1.1% | +3.7% | 50.0/49.8 |
| large | debug | 115.34 | 115.16 | +0.2% | +0.2% | 54.3/54.5 |
| large | release | 88.99 | 90.41 | -1.6% | +1.5% | 47.3/47.4 |

Policy A's full sweep includes Dungeon: debug 1878.69 → 1894.36 ms and release
3288.19 → 3327.75 ms. LLVM tool time dominates this case; concurrency in Nerd's
front end/render cannot remove that serial external-tool work. Policy B Dungeon
correctness is covered separately; these Dungeon performance numbers are A,
not B. The earlier A exploratory sweep and full sweep disagree on some small
regressions, reinforcing the need for paired repetitions and native evidence.

The nine-sample unrestricted repeat sees all 32 logical CPUs (auto ceiling 16):
Pixels debug 221.73 → 210.52 ms (+5.1%), release 324.53 → 326.60 ms (−0.6%).
Tiny debug is 2.0% faster and release 1.8% slower; Quill is effectively unchanged
in debug and 1.2% faster in release. This repeat also fails the 15% adoption gate.

## Worker reuse decision

A's LLVM batch profiles show first dispatch after 75–396 microseconds, with
10–61 microseconds draining after the final callback. Tiny and dominant-module
cases create no render pool. Parse/HIR policy records identify which phases
actually dispatch; most A real-input batches are inline, and B exposes more
front-end batches. A reusable pool could remove some repeated thread lifecycle
cost but not array/allocator contention or serial semantic/LLVM work. These
observed delays are too small to explain the missing whole-build gain. Retain
one-shot pools for this experiment; no measured persistent-pool speedup is
claimed. A future reuse prototype must measure all phases and parked-worker
costs before replacing the simpler lifetime model.

## Correctness and remaining gates

The production scheduler and front-end suites include automatic mode in
LLVM/HIR/C output, runtime and ordered-error comparisons. Core tests cover small
batches, a dominant task, independent-work limits, count/CPU caps and extreme
arithmetic. Policy records are buffered during speculative front-end work and
are excluded from semantic phase-order comparisons. Numeric counts bypass the
adaptive policy; CPU-affinity tests verify the auto ceiling separately from the
selected render count. Profiling-disabled builds do not allocate policy records.

`just test` passed: 1,122 fixtures, zero failures, nine platform skips;
280 C differential fixtures at both optimization levels, including Dungeon
rendering and Q-to-quit parity. Sanitizer results are recorded below once complete. Windows's
updated runner includes auto/16-worker measurements and a known CPU/memory child
sanity check. Its new native accounting path still requires execution on Windows;
compiler-only Windows counters must not be equated with POSIX descendant totals.

M12's [ownership design](../audits/compiler-m12-semantic-ownership.md) is separate
work. It identifies imported generic and trait mutations and the required
canonical identities, request publication and continuation boundaries. It is not
an implemented semantic-parallelism feature, and shared-core exclusion remains.
