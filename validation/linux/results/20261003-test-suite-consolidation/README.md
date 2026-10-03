# Native Linux consolidation review — 3 October 2026

Reviewed `test-suite-consolidation` at `87e4567a` against `std-library`
`db680b1d` in a detached worktree. All eight earlier standard-library task
branches, plus the diagnostics/LSP branch, were already ancestors of
`std-library`. The audit branch is included in consolidation's history.
No task branch was overwritten or deleted.

## Repairs found during pickup

The first full debug gate failed with 29 LSP mismatches and one compiler crash:
see [original gate](just-test-before-repair.log). LSP fixture URI expansion
replaced `__REPO_URI__` and then replaced the historical repository prefix
inside its own result. This doubled the worktree suffix and made virtual
imports resolve incorrectly. A single-pass replacement fixes both fixture
spellings; all 210 applicable LSP fixtures then passed.

Command 223 (`on` with an imported `abort` overload in its else branch) crashed
the compiler during inference, before executable generation. The
[native backtrace](never-on-crash.log) showed allocator corruption;
[AddressSanitizer](never-on-asan-before.log) identified the earlier invalid
write. Inferring a function body imported overload proxies and reallocated
`sema->decls`, invalidating the output pointer to the callee's type.
Commit **`209e8857`** uses a stack result and reacquires the declaration by
index. The existing command and C differential case are retained; a new
frontend ownership case covers LLVM/C, generated-program modes and worker
counts under the sanitizer suite.

## Final validation

Fresh Clang builds at **`209e8857`** passed:

- **`just test` and `just test-release`: 1,153 fixture passes, zero failures,
  nine existing Linux platform skips each.** Exact category counts, skips and
  compiler hashes are in [summary.json](summary.json).
- All debug auxiliary stages, including memory/runtime/compiler threads,
  capabilities, LLVM/C thread/network contracts, profiling, rendering,
  unchanged 6,000-term expression depth at jobs 1/4, frontend concurrency,
  direct LLVM toolchain, temporary installation, formatter workflow and
  source build settings.
- **291 LLVM/C differential fixtures at C O0/O2, zero platform skips**;
  native Linux PTY Dungeon renders before input and quits on Q.
- Release gate's existing scope: fixtures, capabilities, thread contracts
  through LLVM/C and release-generated network contracts through LLVM/C.
- [Full frontend AddressSanitizer suite](frontend-asan.log), including the
  new imported-overload lifetime regression, ordered diagnostics and randomized
  completion checks.
- Direct LLVM [doctor](doctor.log), explicit generated-C
  [capability/layout probes](capabilities-cgen.log),
  [final migration verification](mapping-final.json), and the three
  `just run-example` commands: [thread-pipeline](thread-pipeline.log),
  [network-echo](network-echo.log), [network-datagram](network-datagram.log).

Logs: [debug gate](just-test.log), [release gate](just-test-release.log),
[initial build](build-debug.log), [repair rebuild](build-debug-repair.log),
[LSP repair verification](lsp-path-fix.log), [focused command](never-on-fixed.log).
Host/tool versions are in [environment.json](environment.json); the
[final inventory](final-inventory.json) is collected from the actual harness.

The Windows stdio auxiliary stage correctly skips on Linux, separately from
the nine fixture skips. No new platform exclusions were introduced.

## Measurement and limits

[Changed-work timings](changed-work-timings.json) used one fresh compiler at
`87e4567a` on both sides, sequentially, with one excluded warmup and three
recorded samples. The old 47 commands took **1.485 s median**; the replacement
batch took **0.0619 s**. Retained alternative language fixtures are outside
both sides. This measures changed fixture work only, not a whole-gate speedup.
The mapping and omission/wrong-result mutations were reverified after the
inference repair without repeating performance measurements.

No global install, `just do`, new Windows run, full desktop/editor observation,
or benchmark adoption run occurred. Existing Windows results cover
`caaef691`, not the new inference-lifetime repair. Native Linux evidence and
Windows/WSL evidence remain distinct. Stronger negative-command diagnostics,
further batching and shared same-run differential baselines remain later
audit milestones.
