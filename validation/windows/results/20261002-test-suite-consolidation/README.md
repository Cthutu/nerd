# Native Windows test consolidation

Branch: `test-suite-consolidation`, based on `std-library` `6bc00639`.
Compiler fix: `58818307` preserves optional/result flags during enum payload
materialisation. Fresh debug/release compilers were built with Clang in this
worktree; installed Nerd and editor extensions were not changed.

The [consolidation report](../../../../docs/test-suite-consolidation.md) and
[complete migration map](../../../../docs/test-suite-consolidation-map.json)
describe the 33 duplicate removals and 14-scenario feature batch.

- Focused batch passes native LLVM with debug/release compilers and generated C
  at O0/O2 with the debug compiler.
- [Changed-work timings](changed-work-timings.json) and [log](changed-work-timings.log)
  verify all 47 original command cases, the batch, duplicate mappings and both
  omitted-scenario/wrong-result negative mutations. These were collected using
  the fixed compiler from `58818307`, with the consolidation deletions/list/map
  present in the working tree. Median changed work: 10.273 s before, 0.317 s after.
  This is not a full-gate percentage speedup.
- Complete `just test` and `just test-release` will be recorded after committing
  the consolidation so the tested implementation has a stable revision.

Scope excludes global install, desktop/editor observations, full Windows benchmark
matrix, Linux and WSL. Default suite scope, platform skips, HIR/LLVM snapshots and
all inherited integer destination-range regressions are preserved.
