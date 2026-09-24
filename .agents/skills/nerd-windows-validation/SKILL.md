---
name: nerd-windows-validation
description: Prepare, execute, or receive Nerd's native Windows validation and repair handoff. Use for Windows compiler validation, fixing its test failures, collecting native results, or pulling the Windows return package on Linux.
---

# Nerd Windows validation

Use the repository's existing runner and handoff documents. All command paths
below are relative to the Nerd repository root, three levels above this skill.
Follow the user's requested scope: preparing or receiving a handoff does not
mean starting a new native validation run. Explicit user instructions override
this workflow.

## Select the requested operation

### Prepare a Windows handoff

Read [the procedure](../../../validation/windows/PROMPT.md),
[runner instructions](../../../validation/windows/README.md), and the newest
section of [the handoff](../../../validation/windows/results/HANDOFF.md).
Record the intended branch/revision, changes since the last tested Windows
revision, focused regressions and remaining native checks in a new dated
handoff section. Preserve previous evidence. Check the runner's stage list and
run `python validation/windows/test_runner.py` if changing its instructions or
implementation. Commit/push the prepared handoff when requested or already
authorized. Give the user the invocation below for the Windows machine.

### Execute on native Windows

Read and execute [PROMPT.md](../../../validation/windows/PROMPT.md). It defines
prerequisites, fresh debug/release builds, failure repair, final full validation,
desktop/editor observations, serial benchmark runs and the return package.
Use the user's selected branch, otherwise the current branch and its verified
upstream; do not recreate historical experiment branches from old evidence.
The full workflow includes committing/pushing tested fixes and evidence when
requested or authorized in the session. Preserve narrower requests such as
read-only diagnosis, tests only, or no push.

Use native Windows Python and a configured SDK/CRT shell. WSL/Linux results do
not satisfy this gate. If invoked elsewhere, prepare the handoff and clearly
report that execution still requires Windows; do not simulate a native pass.

### Receive the Windows return on Linux

Inspect local changes and the intended upstream, fetch and fast-forward where
possible without discarding work, then read the newest handoff and linked full
run, manual outcomes and benchmark summary. Review the returned diff against
the prior local HEAD. Run Linux regressions appropriate to implementation
changes; evidence-only returns do not require repeating the compiler suite.
Report received/tested revisions, failures or fixes, remaining gates and local
working-tree state. Do not merge or delete branches unless requested.

## Invariants

- Normal Nerd binary generation uses LLVM tools directly, never a Clang
  fallback. External Clang may build Nerd or test compatibility C output.
- Use fresh repository binaries, not globally installed Nerd. Do not install
  globally or replace editor extensions as part of validation.
- Preserve deterministic LLVM/diagnostics, semantic ownership and the current
  worker default. Do not lower depth-test counts or weaken checks to pass.
- Keep failed runs, suite counts, manual evidence and raw measurements under
  `validation/windows/results/`; put the latest outcome at the top of
  `HANDOFF.md`. Distinguish native results, platform skips and blocked checks.
- Windows CPU/working-set metrics cover Nerd only; wall time includes child
  tools. Do not claim whole-tree memory or cross-platform accounting parity.

User invocation: `Use $nerd-windows-validation to run the full Windows validation,
fix failures, and commit/push the results and return handoff.`
