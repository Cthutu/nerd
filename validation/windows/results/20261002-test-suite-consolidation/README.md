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
- **Complete native `just test` and `just test-release` passed on `caaef691`**:
  1,147 fixture passes, zero failures, 15 platform skips each. No changes were
  made to executable code or fixtures during or after these final gates.
- The debug gate passed all 289 LLVM/C differential fixtures at O0/O2, with two
  platform skips; Dungeon's Linux PTY check was skipped on Windows. Cleanup,
  memory/runtime/compiler threads, capabilities, thread/network contracts,
  profiling, concurrent rendering, unchanged 6,000-term expression depth,
  scheduler/frontend, direct toolchain, temporary installation, format workflow,
  build settings and all four Windows stdio modes passed.
- The release gate passed its fixture suite, capabilities, both thread backends
  and release-generated LLVM/C network contracts. It does not repeat the entire
  debug auxiliary/differential matrix; that is the existing recipe's scope.

Evidence:

- [Machine-readable counts and compiler hashes](summary.json)
- [Final inventory](final-inventory.json)
- [Debug common gate log](just-test.log)
- [Release common gate log](just-test-release.log)
- [Clang debug rebuild for compiler fix](build-debug-fix.log)
- [Fresh Clang release build](build-release.log)
- [Native tool/environment versions](environment.json)

Scope excludes global install, desktop/editor observations, full Windows benchmark
matrix, Linux and WSL. Default suite scope, platform skips, HIR/LLVM snapshots and
all inherited integer destination-range regressions are preserved.
