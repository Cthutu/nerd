# Dependency scheduler withdrawal validation

2026-09-22, Linux x86-64; parent revision `1072e00b`.

Removed the G1/G2 implementation introduced by `55883666` and `598a2d1a`.
Compiler sources, core API, test recipe, frontend parity test, Windows runner
and INTERNALS now match `f3af9bb1` exactly. Earlier batch parallelism,
single-core optimisations, correctness fixes and opt-in adaptive dispatch remain.
Benchmark harness improvements and all historical measurement evidence remain.
The [plan](../audits/compiler-task-graph.md) records why G3–G6 are deferred.

Validation after withdrawal:

- Release compiler rebuild passed.
- Full `just test` passed: 1,122 fixtures, zero failures, nine platform skips;
  allocator/thread tests, profiling, render/frontend/jobs parity, toolchain and
  install checks; 280 C differential fixtures at both optimisation levels.
  Pixels standalone C and Dungeon render/input parity passed.
- Release frontend parity and jobs suites passed, including automatic sizing,
  affinity, explicit overrides and LLVM/C output parity.
- Windows runner unit tests passed on Linux; this is not native Windows evidence.
- `git diff --check` passed. Comparing the restored implementation paths against
  `f3af9bb1` produces an empty diff. No new sanitizer or native platform runs
  were performed for this withdrawal.

Compressed logs are in `compiler-task-graph/withdraw-*.log.gz`.
The temporary large-project benchmark and crash reproducer remain outside the
repository in `/tmp/nerd-scale-bench-Xo1BUG`; they were not changed or rerun as
part of this withdrawal. The expression-depth crash and proposed single-core
optimisations remain follow-up work.
