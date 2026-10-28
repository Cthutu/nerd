---
name: nerd-start-task
description: Start or resume a Nerd compiler, language, standard-library or tooling task by reading current repository guidance and establishing the development workflow. Use when beginning work in a fresh Nerd conversation or when asked to catch up before a task.
---

# Start a Nerd task

Establish the current repository context before making changes. Read maintained
guidance from the checkout each time; earlier conversation summaries and this
skill's workflow summary may be older than the repository.

## Read the current guidance

Locate the Nerd checkout using the working directory and `git rev-parse
--show-toplevel`. On Matt's Windows machine the usual checkout is
`C:/Users/matt/nerd`; use the selected checkout or worktree when different.

Read the root `README.md` in full, including Task Workflow and Contribution
Rules. Read applicable `AGENTS.md` files and any additional instruction files
the checkout identifies. Do not assume historical files such as `CODEX.md`
still exist.

Follow the README's current Catching Up reading order. It currently points to:

1. `docs/README.md`
2. `src/README.md`
3. `tests/README.md`
4. `ROADMAP.md`
5. `DEBUGGER.md`

Read `docs/compiler-pipeline.md` for architectural contracts. For a concrete
task, follow the relevant subsystem links and local instructions before editing.
Language changes need the affected learning guide and implementation-derived
specs; compiler changes need the relevant internals. Avoid loading every learning
chapter or historical validation record for an unrelated task.

## Establish the working state

Inspect the current branch, working-tree changes, latest commit, upstream and
configured remotes. Preserve existing work and report changes relevant to the
task. Use the selected branch; do not infer that a new conversation means
switching to `main`, pulling, merging, cleaning or resetting the checkout.
Update branches when the user requests it or the actual task requires it.

Matt uses Git Bash. Give shell examples in Git Bash syntax when appropriate;
execute commands using the environment's actual configured shell.

For GitHub CLI operations, use the `Cthutu` account with a command-scoped
`GH_TOKEN`. Never switch the shared active GitHub account or expose the token.
Load the stored token for that account with inherited `GH_TOKEN` and
`GITHUB_TOKEN` cleared, then scope it to the required command. SSH Git remotes
authenticate separately; use the active branch's configured remote.

## Carry the workflow into the task

Apply the current README workflow and the user's explicit scope. The current
completion sequence is:

1. Add or update meaningful regressions and affected documentation.
2. Run `just test`; repair failures and rerun it.
3. Run `just format`; review changes and repair any failures.
4. Run `just install`; repair failures and rerun it.
5. Commit only the task's changes and push to the active branch's configured remote.
6. Report the commit, verification, installation and push outcomes, plus a useful
   next step when one remains.

Use focused `python3 build/test.py --filter ...` checks while iterating. Do not
replace the normal gate with a new validation/reporting system. `just do`
includes cleaning and additional release checks; use it when requested rather
than silently substituting it for the README sequence. Full Windows desktop
validation, benchmarks and network/Nexus integration runs have their own scope;
read their instructions when those checks are requested or relevant.

Respect explicit changes to this workflow, including discussion-only work,
deferred tests, no commit/push, or installation requests. State what was actually
verified. Creating or invoking this skill alone does not request implementation,
builds, installation, commits or pushes.

When invoked without a concrete task, finish with a brief readiness note naming
the branch, relevant local changes and the workflow you will follow. When a task
is supplied alongside the invocation, briefly acknowledge the context and
continue with that task.
