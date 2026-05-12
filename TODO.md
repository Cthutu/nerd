# Architecture Review Handoff

This file is the working handoff for the architecture review. It has two jobs:

1. help a fresh Codex session catch up quickly
2. make the remaining work visible as milestone-sized commit slices

## 1. Catch-Up Instructions For Codex

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

5. `docs/compiler-pipeline.md`
   - Current compiler pipeline and stage responsibilities.
   - This is the fastest way to understand the active HIR and LLVM backend
     shape.

6. `ARCHITECTURE_REVIEW.md`
   - Top-level architecture review synthesis.
   - Read this before proposing boundary changes.

7. `review/README.md`
   - Map of the review workspace.
   - Use this to find audits, decision records, measurements, and prototypes.

8. `review/decisions/*.md`
   - Accepted architecture decisions.
   - Pay particular attention to:
     - HIR/backend boundary
     - `for in` pointer items
     - anonymous entities plus explicit bindings
     - module bindings
     - LLVM sidecar generation from HIR

9. `review/audits/*.md`
   - Evidence gathered during the review.
   - These explain why the formatter, LSP, memory, and HIR/backend boundaries
     are being changed.

10. `docs/testing.md` and `tests/README.md`
    - Test harness expectations, fixture conventions, and cleanup rules.
    - Run `just test` before finalising meaningful compiler changes.

After reading, inspect the current implementation in this order:

1. `src/compiler/build/front/front.c`
   - Front-end orchestration: lex, parse, sema, HIR.

2. `src/compiler/sema/sema.c`
   - Semantic analysis and side tables.
   - This is still a large boundary and should be changed carefully.

3. `src/compiler/hir/hir.h`, `src/compiler/hir/gen.c`,
   `src/compiler/hir/render.c`
   - HIR data model, lowering from sema, and stable textual representation.

4. `src/compiler/llvm/llvm.c`
   - HIR to LLVM lowering.
   - Remember: Nerd-visible bindings keep `$` names as LLVM aliases;
     generated implementation names are compiler internals.

5. `src/compiler/build/back/back.c` and `src/compiler/build/back/llvm_text.c`
   - Backend orchestration, runtime glue, combined LLVM input generation, and
     clang invocation.

6. `src/compiler/format/format.c`
   - Current formatter implementation and remaining edge-case pressure.

7. `src/lsp/*.c`
   - LSP document state, partial front-end analysis, completion, hover, and
     rename/jump-to-definition behaviour.

Working rules for this review:

- Keep commits small and describe the migration slice in the commit message.
- Use British spelling in this file and in architecture review notes unless
  quoting code, command output, filenames, or external text.
- Add regression tests when fixing discovered behaviour.
- Do not commit unrelated local changes such as editor task changes.
- Prefer `just test` as the full gate; use focussed `python3 build/test.py
  --filter ...` runs while iterating.
- Update review docs or decision records when an architectural agreement
  changes.
- After each task, state the commit hash, verification run, and recommended
  next step.

## 2. Next Engineering Roadmap

The initial architecture review is complete. This section now tracks the next
implementation milestones that build on the HIR/LLVM/LSP/formatter work.

### Milestone 1: Split Semantic Facts From Semantic Success

- [x] Commit: audit `Sema` products and consumers
  - Classify declaration collection, scope construction, binding/reference
    resolution, type facts, dependency ordering, diagnostics, and lowering-only
    facts.
  - Record the audit under `review/audits/`.

- [x] Commit: introduce explicit declaration and binding fact views
  - Add narrow APIs for declarations, lexical scopes, source spans, imports,
    exports, locals, and references.
  - Keep the initial implementation backed by existing `Sema` tables.

- [x] Commit: define partial semantic readiness levels
  - Distinguish declaration/binding facts from checked type facts.
  - Update LSP readiness and front-end product states to expose those levels.

- [x] Commit: migrate LSP features to the fact views
  - Completion, hover, definition, rename, signature help, and code actions
    should request named views instead of reading broad sema internals.
  - Add regression tests for incomplete source and failed imports.

- [x] Commit: document the new sema boundary
  - Update `docs/compiler-pipeline.md`, `ARCHITECTURE_REVIEW.md`, and decision
    records if the boundary becomes a settled architecture rule.

### Milestone 2: Make Syntax Tooling More Tolerant

- [x] Commit: audit AST/CST divergence
  - List duplicated grammar cases, missing CST recovery cases, and syntax
    families where formatter/LSP and compiler parsing can drift.

- [x] Commit: add syntax recovery fixtures
  - Cover partially typed blocks, calls, aggregates, `on` branches, module uses,
    and unterminated delimiters.
  - Include formatter and LSP cases where the expected behaviour is useful
    partial output rather than full success.

- [ ] Commit: derive shared syntax classification helpers
  - Centralise token-to-construct and node-range queries currently duplicated
    between formatter, LSP, AST, and CST utilities.

- [ ] Commit: move one formatter or LSP path to tolerant syntax first
  - Prefer a narrow path with clear value, such as document symbols or
    newline/comment decisions around incomplete blocks.

- [ ] Commit: decide the long-term AST/CST relationship
  - Record whether AST should be derived from CST, whether both should come
    from one parser core, or whether the current split remains intentional.

### Milestone 3: Continue Formatter Simplification

- [ ] Commit: classify remaining layout cases in `format.c`
  - Separate token spacing, comment attachment, blank-line policy, indentation,
    alignment regions, and construct-specific rendering.

- [ ] Commit: move blank-line decisions onto trivia/syntax helpers
  - Prefer token newline counts and syntax ranges over ad hoc source scans.
  - Keep snapshots idempotent.

- [ ] Commit: move another comment consumer onto `FormatTrivia`
  - Pick one remaining offset-driven path and replace it with token-attached
    leading or trailing trivia.

- [ ] Commit: extract alignment region planning
  - Make local declarations, assignments, plex fields, enum variants, and
    trailing comments use an explicit region planner where practical.

- [ ] Commit: grow token/trivia fallback coverage
  - Add partial-source snapshots before changing behaviour.
  - Keep sema out of the formatter contract.

### Milestone 4: Measure And Tune The LLVM Backend

- [ ] Commit: measure build/link timings by backend phase
  - Track LLVM text rendering, combined input construction, runtime object
    writing, clang startup, and clang link/compile time.
  - Record results under `review/measurements/`.

- [ ] Commit: compare clang text input with LLVM CLI alternatives
  - Prototype `llvm-as`, `llc`, `opt`, bitcode, or direct object flows only as
    measurements.
  - Keep clang as the install contract unless data justifies a change.

- [ ] Commit: reduce unnecessary LLVM text churn
  - Focus on string builder growth, temporary path construction, duplicate
    declaration filtering, and module sidecar policy.

- [ ] Commit: improve backend diagnostics on LLVM/tool failures
  - Preserve failing `.ll` inputs.
  - Report the exact command, generated file paths, and first useful tool error.

- [ ] Commit: document the measured backend toolchain decision
  - Add or update a decision record if the external tool contract changes.

### Milestone 5: Strengthen Target Layout And Runtime ABI

- [ ] Commit: audit current LLVM type and ABI assumptions
  - Strings, slices, dynamic arrays, tuples, plexes, enums, varargs, pointers,
    integer widths, alignment, and aggregate passing/returning.

- [ ] Commit: introduce a backend layout context
  - Centralise target type spelling, sizes, alignments, and ABI lowering
    choices used by LLVM generation.

- [ ] Commit: add focussed ABI regression tests
  - Include FFI calls, varargs, string helpers, aggregate fields, enum payloads,
    pointer casts, and dynamic-array runtime calls.

- [ ] Commit: make runtime helper declarations generated from one source
  - Avoid duplicated handwritten signatures between runtime C, HIR lowering,
    LLVM emission, and tests.

- [ ] Commit: document target support limits
  - Be explicit about the current host assumptions and what must change for
    cross-target builds.

### Milestone 6: Release And Installation Hardening

- [ ] Commit: add installed compiler smoke fixtures
  - Exercise `nerd build`, `nerd run`, `nerd test`, `nerd format`, `--hir`, and
    `--llvm` outside the repository tree.

- [ ] Commit: check generated artefact cleanup from installed builds
  - Ensure successful external builds remove temporary `.ll`, `.link.ll`,
    runtime object copies, and executables unless requested.

- [ ] Commit: verify editor integrations after compiler changes
  - VS Code extension packaging/install.
  - Neovim syntax/LSP install paths.
  - LSP startup from the installed `nerd` binary.

- [ ] Commit: update public docs for the current compiler shape
  - README, roadmap, manual references, compiler pipeline, testing docs, and
    editor-support docs should agree.

- [ ] Commit: final release gate
  - `just test`
  - `just install`
  - installed compiler smoke tests
  - focussed LSP transcript smoke
