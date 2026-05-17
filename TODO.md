# Project Catch-Up

This file is the working handoff for a fresh Codex session. Its purpose is to
identify the current source of truth quickly, then make the normal task workflow
explicit.

## Catch-Up Instructions For Codex

Start by reading these files in order:

1. `README.md`
   - Project overview, build/test entry points, and user-facing compiler shape.

2. `CODEX.md`
   - Repo-specific working rules for Codex sessions.
   - Treat these as local instructions in addition to the active prompt.

3. `ROADMAP.md`
   - Current project direction and active priorities.
   - Use this instead of older historical notes when project direction differs.

4. `docs/manual/README.md`
   - Manual index and source-language documentation map.
   - Follow into the specific manual chapters when touching syntax, semantics,
     diagnostics, or tests for a language feature.

5. `docs/spec/README.md` and the documents under `docs/spec/`
   - Implementation-derived language specs for lexer, syntax, types,
     expressions, statements, control flow, patterns, modules, FFI, runtime
     model, diagnostics, tests, and implementation notes.
   - Read these before changing language behaviour, tests, diagnostics,
     formatter/LSP syntax handling, or user-facing documentation.
   - When the manual and specs disagree, preserve the code-derived fact in the
     spec and flag or fix the manual inconsistency as part of the task.

6. `docs/compiler-pipeline.md`
   - Current compiler pipeline and stage responsibilities.
   - This is the fastest way to understand the active HIR and LLVM backend
     shape.

7. `docs/testing.md` and `tests/README.md`
   - Test harness expectations, fixture conventions, and cleanup rules.
   - Run `just test` before finalising meaningful compiler changes.

Use the architecture review documents as historical context only:

- `ARCHITECTURE_REVIEW.md`
- `review/README.md`
- `review/decisions/*.md`
- `review/audits/*.md`
- `review/measurements/*.md`

Read them when a task touches an architectural boundary or when current docs
refer to a settled review decision. Do not treat the completed review roadmap as
active project work.

After reading the project docs, inspect the current implementation relevant to
the task. These are the main entry points:

1. `src/compiler/build/front/front.c`
   - Front-end orchestration: lex, parse, sema, HIR.

2. `src/compiler/sema/sema.c`
   - Semantic analysis and side tables.

3. `src/compiler/hir/hir.h`, `src/compiler/hir/gen.c`,
   `src/compiler/hir/render.c`
   - HIR data model, lowering from sema, and stable textual representation.

4. `src/compiler/llvm/llvm.c`
   - HIR to LLVM lowering.
   - Nerd-visible bindings keep `$` names as LLVM aliases; generated
     implementation names are compiler internals.

5. `src/compiler/build/back/back.c` and `src/compiler/build/back/llvm_text.c`
   - Backend orchestration, runtime glue, combined LLVM input generation, and
     clang invocation.

6. `src/compiler/format/format.c`
   - Formatter implementation.

7. `src/lsp/*.c`
   - LSP document state, partial front-end analysis, completion, hover,
     rename, and jump-to-definition behaviour.

## Task Workflow

- Make the smallest coherent change for the task.
- Update or add tests for changed behaviour, including regression tests for
  discovered bugs.
- Update affected documentation in the same change, including manual pages and
  `docs/spec/` language specs when syntax, semantics, diagnostics, runtime
  behaviour, tests, workflow, architecture, or user-facing compiler details
  change.
- Prefer `just test` as the full gate; use focussed `python3 build/test.py
  --filter ...` runs while iterating.
- Run `just test` after the task before finalising.
- Commit the work. Keep commits small and describe the task slice in the commit
  message.
- Install the current verified compiler with `just install` so the installed
  `nerd` and standard modules match the source tree.
- Do not commit unrelated local changes.
- After each task, state the commit hash, verification run, and recommended
  next step.
