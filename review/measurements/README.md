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
