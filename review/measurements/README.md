# Measurements

Store repeatable profiling, timing, and allocation results here.

Each note should include command, commit or working-tree context, environment,
raw summary, and interpretation.

- `task-scheduler-baseline.md`: current exploratory compiler timings and raw
  samples supporting the task scheduler experiment.

Current backend timing notes:

- `backend-phase-timing.md`: phase-level timings from the compiler `--timing`
  report.
- `llvm-cli-tool-comparison.md`: coarse comparison of clang text input versus
  `llvm-as`/`llc` object generation.

- `compiler-m1.md`: pinned direct-LLVM baseline, module imbalance, CPU samples,
  allocation churn, and revised optimisation priorities.
- `compiler-m2-scratch.md`: alternating before/after comparison of LLVM combiner
  scratch reuse, with byte-identical LLVM and allocation/RSS measurements.
- `compiler-m2-function-names.md`: LLVM function-name conflict index timings,
  byte-identical output checks and peak process memory comparison.
- `compiler-m2-lines.md`: program-owned source-line indexes, LLVM/C timings,
  paired checking comparisons and memory measurements.
- `compiler-m2-usage.md`: usage-inference scope filtering, paired check/build
  timings and fresh CPU samples identifying declaration collection.
- `compiler-m2-decls.md`: declaration-scope filtering, paired checks/builds,
  refreshed CPU samples and completion of the Linux M2 serial work.
- `compiler-m3-results.md`: module result ownership, partial cleanup and the
  single-core overhead/output-identity check before introducing workers.
- `compiler-m3-diagnostics.md`: task-owned diagnostic queues, lifetime and output
  parity checks, and single-core overhead before introducing workers.
- `compiler-m3-memory.md`: synchronized allocator bookkeeping, cross-thread
  ownership tests and serial overhead measurements.
- `compiler-m3-metrics.md`: thread-local activity, deferred render timing records,
  concurrent attribution tests and serial overhead measurements.
- `compiler-m3-threads.md`: portable worker primitives, lifecycle/failure tests
  and the initial LLVM shared-input review.
- `compiler-m3-render.md`: completed render ownership review, private type
  metadata fixes, concurrent render/sanitizer tests and serial overhead.
- `compiler-m4-scheduler.md`: production bounded task batch, `--jobs` CLI,
  sanitizer/failure tests, serial overhead and worker-count scaling.
- `compiler-m4-contention.md`: corrected-target scaling, opt-in lock-acquisition
  timing, render dispatch/drain boundaries and remaining system CPU evidence.
- `compiler-m4-metadata.md`: contiguous metadata-text copying, exact LLVM
  comparisons, fewer bookkeeping acquisitions and single-core latency gains.
- `compiler-m4-render-scratch.md`: per-module reuse of function scratch arenas,
  sanitizer/growth regression, serial/parallel comparisons and fresh scaling.

- `compiler-m5-front-end.md`: completed Linux M5 ownership/scheduling and
  graph/sanitizer gates, paired timings, worker scaling, and the decision to
  retain the one-job default because gains are mixed.

- `compiler-main-to-m5.md`: fresh cumulative comparison against original `main`,
  including installed baseline, one-core checking/full builds, incremental
  worker scaling, byte-identical LLVM and raw five-sample measurements.
