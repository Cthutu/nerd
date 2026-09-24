# Execute the Nerd Windows validation and repair handoff

Use this procedure for a requested full native Windows validation and repair
run. Complete tests, fix failures and preserve a return handoff. Commit and push
fixes/evidence when requested or already authorized in the session; respect
narrower instructions such as diagnosis only or no push. Preparing this prompt
or reading historical results does not itself request execution.

## Select the current validation scope

Read the latest section of `validation/windows/results/HANDOFF.md` and the
user's requested branch/revision. Record the exact starting HEAD and compare it
with the last tested Windows revision. Historical commit IDs and old branch
names in evidence are context, not checkout instructions. Read the relevant
active audit when a change has additional gates; the completed single-core
work is documented in `review/audits/compiler-single-core-follow-up.md`.

The full runner includes `expression-depth-debug` and
`expression-depth-release`: keep the default **6,000 terms**, jobs=1/4, exact
LLVM comparison and runtime/order checks. Do not lower the count or enlarge
the stack to mask a failure. Full frontend checks cover platform conditionals,
nested declarations, traits, generics and deterministic diagnostics/output.
The dependency scheduler G1/G2 was withdrawn; do not restore it as part of
validation. Native sanitizers are optional when supported; distinguish unrun
or unsupported checks from passed ones. Linux evidence is not a Windows pass.

## Context and non-negotiable behavior

- Work on the user-selected branch, otherwise the current branch and verified
  upstream. Do not merge, delete branches, force-push, or overwrite unrelated
  user changes as part of validation. The old experiment is already merged.
- Read the repository's applicable `AGENTS.md` instructions if any, `CODEX.md`,
  `validation/windows/README.md`, `review/audits/compiler-m8-adoption.md`, and
  the follow-up milestones in `review/audits/task-scheduler-performance.md`.
- Nerd's normal source-to-binary pipeline invokes LLVM tooling directly:
  `opt`, `llc`, and platform linker/archive tools. **Never add a Clang fallback
  to Nerd.** Clang may build Nerd itself or externally compile compatibility C.
- Keep `--jobs 1` as the default. Explicit jobs exercise parsing, semantic
  closures, HIR and LLVM rendering. Shared imported semantic state remains
  exclusively owned; do not remove that protection to make a test faster.
- Imported generic specialization selection was fixed in `590105cd`; widened
  runtime regressions and portable harness paths are in `c0c9fd8a`. Preserve
  explicit/inferred/function-value coverage with values exceeding 32 bits.
- Preserve Windows headers compatibility, correct target triples through both
  opt and llc, deterministic output/diagnostics, debug information, runtime
  behavior, and all output modes. Do not weaken assertions or broadly skip
  Windows checks to obtain a green result.
- Use freshly built `_bin/nerd-debug.exe` and `_bin/nerd.exe`, not PATH's Nerd.
  Do not replace the globally installed compiler or editor extensions as part
  of this task. Significant compiler fixes must update `docs/overviews/INTERNALS.md`.

## Execute and fix

1. Inspect branch, working tree, HEAD, upstream and available remotes. Confirm
   the intended branch; record the starting commit and unrelated changes.
   Fetch the intended upstream and fast-forward if appropriate. Preserve user
   changes and remote work. The remote was named `agent` on Linux, but use the
   working configured/authenticated remote here; do not assume its SSH alias
   exists on this machine. Never publish to a different repository by guessing.
2. Read the tools' prerequisites. Inspect the installed LLVM version, Windows
   SDK/CRT, Python, Git, just and debugger. Use the configured development shell
   or available SDK environment. Resolve missing development dependencies
   within available permissions, following this machine's instructions. Do not
   silently replace unrelated toolchains, change system policies, or dump the
   entire environment into logs. Record dependency problems and resolutions.
3. Run `python validation/windows/test_runner.py`, then
   `python validation/windows/run.py`. Use `py -3` or the PowerShell launcher
   if that is how Python is installed. Inspect per-stage logs while it runs.
   Each invocation writes a unique results folder and incremental summaries.
   Read failed logs even if subsequent independent tests pass. A failed build
   blocks its dependent tests so that stale binaries cannot look successful.
4. Fix reproducible failures at their cause. Use focused reproductions and
   reruns (`run.py --only <stage>` includes the necessary compiler build).
   Extend meaningful regressions for compiler/runtime bugs. If the harness
   assumes Linux behavior, make the check genuinely portable without discarding
   the behavior it was intended to verify. Preserve baseline failure evidence
   and document each changed expectation or justified platform-specific skip.
5. Within the authorized commit/push scope, publish each coherent tested fix
   to the selected branch as you go. Stage
   only intended changes; include relevant results and update a human-readable
   `validation/windows/results/HANDOFF.md`. Review generated artifacts before
   staging. Do not commit binaries, objects, PDBs, SDKs, core dumps, or ignored
   scratch folders. Retain useful text logs, JSON, minimal reproductions and
   necessary screenshots. Inspect for credentials or unrelated workplace data;
   redact those narrowly while retaining diagnostics and outcomes.
6. Commit code fixes before the final full run so its recorded HEAD identifies
   the tested implementation; commit the resulting evidence afterwards. Run the
   complete runner again. It covers the existing fixture
   suite using the debug compiler and auxiliary checks with debug AND release
   compilers; these include debug/release generated targets and output modes.
   The final run must not be just a filtered subset. Record pass/fail/skip
   counts from individual suites, not merely their process exit codes.
7. Complete the run's `MANUAL.md`: Pixels, Dungeon, Triangle, generated-C
   compatibility, and the normal Windows debugger/editor experience. Use the
   checks and mode/job matrix in the template. Automate interaction only where
   available. Ask the user for actual desktop observations when necessary; if
   unavailable, mark the check BLOCKED and explain. Do not claim a window draws
   correctly merely because its process stays alive. Graphical regressions
   merit fixes and reruns just like automated failures.
8. Run benchmarks only after correctness and with no concurrent builds/tests.
   The full runner includes five-sample jobs 1/2/4/8/16/auto measurements and LLVM hash
   checks. Record hardware and power mode; if practical also exercise the
   physical-core count (identify it explicitly, do not confuse it with logical
   CPU count). The `benchmark-accounting` stage validates Windows process
   CPU/peak-working-set counters. Investigate API failures or zero peaks. These
   counters cover Nerd only, excluding LLVM/linker children; wall time covers
   the full build. Preserve `accounting_scope` and do not equate these counters
   with Linux waited-child accounting or a whole-process-tree memory gate.
   Keep raw results in this folder. No performance claim from a single sample.

## Required return package

Maintain `validation/windows/results/HANDOFF.md` containing:

- Date, Windows edition/build and architecture, CPU/core counts, LLVM/compiler
  versions, SDK/CRT/debugger details, start commit and tested final code commit.
- Links to original failed runs and final full run, exact reproduction/rerun
  commands, fixes and their commit hashes, and any environment changes needed.
- Automated suite counts, skips with reasons, manual outcomes, benchmark
  interpretation, and explicit untested/blocked work. Preserve failed runs.
- The next action for the Linux Codex session, including Linux regressions to
  rerun after Windows fixes. Do not mark M8 fully validated while native checks
  or memory evidence are still missing; macOS remains a separate gate.

Update applicable audit milestones only for gates this run actually covers
and link to the handoff; do not reopen completed historical milestones solely
because this procedure references them. Put the new outcome and tested commit at
the top of `results/HANDOFF.md`, keeping earlier runs as dated history. Include
the two expression-depth outcomes, scope-query regressions, accounting sanity
check, worker sweep and any desktop/editor blockers in the return summary.
Do not rewrite earlier Linux measurements as Windows results. Within the
authorized scope, commit and push intended fixes, harness updates and evidence.
If upstream advanced, fetch and reconcile without overwriting others or forcing
history; rerun checks affected by that reconciliation before the final push.
Verify HEAD is pushed and report the branch, final pushed commit, working-tree
state, remaining blockers and handoff path to the user. If authentication or a
required external resource prevents completion, commit the available evidence
locally, clearly report what was not pushed/tested, and give the concrete next
step. Never label blocked or unrun checks as passed.

The user's next action is to return to Linux and ask its Codex instance to pull.
Make this folder sufficient for that instance to resume without access to this
Windows conversation.
