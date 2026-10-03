# Typed FFI callbacks — 2026-10-02

Branch `std-library`, starting revision `915d18c0`; results cover the compiler,
bindings and tests committed with this report. Fresh Clang debug/release builds.

- Native Windows compiler suite: **1187 passed, 0 failed, 15 platform skips**
  ([log](compiler-suite.log)). This run includes fixtures 341–346. Two further
  fixtures were then added; the final focused set 341–348 passes with both debug
  and release compilers ([debug](focused-debug.log), [release](focused-release.log)).
- LLVM/C differential suite: **299 fixtures at O0/O2 passed, 2 platform skips**,
  including native `qsort` calling a typed Nerd comparator ([log](cgen.log)).
- Native Windows thread layout/lifecycle, temporary arena and pipeline tests:
  LLVM and C output, debug and optimized programs passed. Repeated with the fresh
  release compiler ([log](threads.log)); debug compiler runs also passed.
- Arch WSL: fresh Clang build and Linux thread C-output tests passed at both
  optimization levels ([log](wsl.log)). Native LLVM remains untested there because
  `opt` and `llc` are absent. No standalone Linux result is claimed.

The initial focused run caught an unused-result mistake in the new positive
fixture, corrected by binding the result to `_` ([log](focused-initial.log)).
An initial WSL shell invocation lost its quoted destination and failed with
permission errors ([log](wsl-initial.log)); the successful retry used Python to
create and populate an isolated temporary checkout.

No full `just test`/`just do`, desktop validation, benchmarks or global install
was performed for this focused change. Callback aggregates passed by value remain
unsupported. Function-returning-function signatures have semantic regression
coverage; the native execution regressions cover comparator and thread callbacks.
