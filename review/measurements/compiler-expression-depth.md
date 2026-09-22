# LLVM arithmetic-depth fix

2026-09-22, Linux x86-64. The preceding symbol-map change is `39c7d26e`.

A 6,000-term subtraction chain crashes the saved release compiler in recursive
LLVM emission. Nested eager arithmetic now uses an explicit array of traversal
frames, retaining the original tree, left-to-right effects and instruction order.
The generated source regression checks non-associative subtraction, right-nested
expressions, an expected ordinal at every operand call, and exact LLVM identity
between jobs=1 and jobs=4. Operators without arithmetic children avoid allocation.

Validation:

- Release compiler: `python build/test_expression_depth.py --nerd _bin/nerd --terms 6000`
  passes for jobs=1/4 with correct executable results.
- Debug compiler and AddressSanitizer: default 512-term source test passes for
  jobs=1/4. The hash-map combiner self-test also passes under AddressSanitizer.
- Original temporary 24,000-function / four-module project compiles using the
  release compiler with jobs=4 and its executable returns success.
- Exact before/after LLVM parity across seven projects (tiny, Dungeon, Pixels,
  Quill, wide, deep, large), both debug/release targets, four workers. This run
  overlapped other validation and is **not performance evidence**.
- Full `just test`: 1,122 fixtures, zero failures, nine platform skips, core,
  profiling, renderer, expression-depth, jobs, frontend, toolchain/install and
  280 C differential fixtures at two optimisation levels. Release rebuild passes.
- Windows runner unit tests pass on Linux; native Windows/macOS remain untested.

## Remaining semantic-inference depth limit

At 6,000 terms, the Linux debug and ASan compilers overflow the native stack in
`sema_infer_node_type`, before LLVM emission. The failing sanitizer trace is
preserved as `compiler-expression-depth/asan-6000-failure.log.gz`. The default
regression uses 512 terms to exercise traversal and ordering in these builds;
the release run uses the original 6,000-term reproducer. This is a scoped LLVM
fix, not a claim that arbitrary source depth is supported by every compiler phase.
The native runner includes 512 terms for debug and 6,000 for release to expose
platform limits rather than hiding them. Semantic depth/performance remains
follow-up work.

Logs and raw parity records are in `compiler-expression-depth/`. Original large
inputs remain under `/tmp/nerd-scale-bench-Xo1BUG`; the reduced generated regression
is reproducible from `build/test_expression_depth.py`.

## Timing check and tradeoff

Separate alternating runs after correctness work finished, CPU 2, jobs=1,
five samples / three profiles (nine samples in the repeat), preserve exact LLVM.
Pixels elapsed time changes about +1.1% debug / +0.3% release; wide changes
+1.3% / +0.5%. Those small differences are inconclusive on this host.

The single-module synthetic case regresses: the nine-sample repeat changes
102.58 → 106.86 ms debug (+4.2%) and 76.67 → 81.35 ms release (+6.1%). The
profiled frontend envelope rises about 4.5 ms; LLVM rendering remains close.
No semantic-analysis code changed. Binary layout changes are a possible cause,
not an established explanation. Preserve this unfavourable result as a follow-up
for semantic hot-path investigation. The traversal is adopted for correctness,
not claimed as a speed optimisation; it does not erase the independent symbol
lookup gain measured before this change. Background activity remained possible.
Raw timing and profile data are retained alongside the other logs.
