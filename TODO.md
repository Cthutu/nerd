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
    - Run `just test` before finalizing meaningful compiler changes.

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
     rename/jump-to-definition behavior.

Working rules for this review:

- Keep commits small and describe the migration slice in the commit message.
- Add regression tests when fixing discovered behavior.
- Do not commit unrelated local changes such as editor task changes.
- Prefer `just test` as the full gate; use focused `python3 build/test.py
  --filter ...` runs while iterating.
- Update review docs or decision records when an architectural agreement
  changes.
- After each task, state the commit hash, verification run, and recommended
  next step.

## 2. Architecture Review Roadmap

### Milestone 1: Stabilize The LLVM Backend As The Default Path

- [x] Commit: add direct tests for LLVM text combination
  - Cover duplicate declarations, declarations satisfied by definitions,
    aliases, quoted `$` symbols, generated globals, and unresolved libc
    declarations.
  - Prefer focused command fixtures or a small internal test entry point that
    exercises `src/compiler/build/back/llvm_text.c`.

- [x] Commit: split runtime/init LLVM rendering out of backend orchestration
  - Move generated epilogue wrapper and init wrapper helpers out of
    `back.c`.
  - Keep `back.c` responsible for artifact policy and process orchestration.

- [x] Commit: document the executable backend contract
  - Update `docs/compiler-pipeline.md` and review notes with the single
    combined LLVM input model, temp cleanup rules, and clang/tool assumptions.

- [x] Commit: run installed compiler smoke tests
  - Verify `just install`.
  - Verify `nerd run` on small standalone programs outside the repo.
  - Add a regression fixture for any language failure found during smoke tests.

### Milestone 2: Finish Replacing Old IR/C Test Expectations

- [x] Commit: make HIR the old IR textual comparison target
  - Rename or redirect fixtures so compiler middle-layer tests compare HIR.
  - Remove assumptions that the old linear IR is the expected intermediate
    output.

- [x] Commit: make LLVM IR the old C output comparison target
  - Replace C-generation expectations with LLVM-generation expectations where
    the test intent is backend text.
  - Keep language tests focused on behavior rather than incidental backend
    spelling.

- [x] Commit: remove stale generated-file cleanup gaps
  - Ensure passing tests remove temporary `.ll`, `.link.ll`, `.hir`, and input
    sidecars unless the test intentionally preserves them.

### Milestone 3: Remove The Old IR And C Backend

- [x] Commit: remove old IR generation from the build path
  - Delete or isolate old IR code after tests no longer depend on it.
  - Keep any still-useful concepts only if they are explicitly moved into HIR
    or LLVM lowering.

- [x] Commit: remove C generation and C-specific dependency ordering
  - Delete the C backend path once LLVM can compile all language fixtures.
  - Remove topological ordering logic that only existed to satisfy C emission.

- [x] Commit: simplify backend artifact configuration
  - Make emitted artifacts match the LLVM pipeline:
    - HIR dump
    - module LLVM sidecars
    - combined link LLVM only when requested or debugging
    - executable output

- [x] Commit: update docs after removal
  - Remove C backend references from current docs.
  - Preserve historical notes only where useful for context.

### Milestone 4: Complete HIR Coverage For Language Lowering

- [x] Commit: audit HIR coverage against the manual
  - Walk manual chapters and ensure every source construct has either HIR
    lowering, an explicit unsupported diagnostic, or a tracked TODO.

- [x] Commit: strengthen anonymous entity and binding tests
  - Functions, types, globals, imports, exports, and module-visible bindings
    should reflect the Nerd binding model:
    entities are nameless until explicitly bound.

- [ ] Commit: finish expression-valued control flow lowering
  - Ensure `on`, `if`-like forms, loops, branches, early returns, and void
    expressions lower consistently.
  - LLVM should own phi construction where structured HIR branches produce a
    value.

- [ ] Commit: cover aggregate, enum, slice, dynamic-array, and pointer cases
  - Include member auto-deref behavior.
  - Include `for item in collection` where `item` is always a pointer.

### Milestone 5: Harden The LSP Boundary

- [ ] Commit: define front-end product readiness states
  - Make it explicit which of lexer, AST, CST, sema, and HIR are valid after
    partial or failed analysis.

- [ ] Commit: centralize LSP access to compiler facts
  - Reduce direct feature-level poking into `doc->front_end`.
  - Add helper APIs for symbol lookup, scope lookup, type lookup, and source
    spans that are safe under partial analysis.

- [ ] Commit: add crash regression fixtures
  - Convert known LSP crash cases from `review/audits/lsp-crashes.md` into
    automated tests.

- [ ] Commit: improve completion on incomplete code
  - Use tolerant syntax and partial semantic facts where available.
  - Avoid requiring a successful full build for basic completions.

- [ ] Commit: improve hover, rename, and jump-to-definition
  - Base each feature on the shared partial-facts interface.
  - Add tests for incomplete source and cross-module references.

### Milestone 6: Rework Formatter Architecture

- [ ] Commit: prototype token/trivia-driven formatting on a narrow construct set
  - Use lexer token stream with comments and newlines preserved.
  - Track indentation and scope with an explicit formatter state machine.

- [ ] Commit: connect formatter decisions to syntax nodes without requiring sema
  - Formatting should remain source/syntax based.
  - Sema tables can inform optional future tooling, but should not be required
    for stable formatting.

- [ ] Commit: migrate edge-case fixtures to the new formatter path
  - Prioritize comments, blank lines, nested blocks, `on` forms, aggregate
    literals, and partially typed code.

- [ ] Commit: remove duplicated layout cases from the old formatter path
  - Keep the public formatter API stable while replacing internals.

### Milestone 7: Review Memory Strategy

- [ ] Commit: measure allocations by phase
  - Lexer/parser/sema/HIR/LLVM/backend/LSP/formatter.
  - Record results under `review/measurements/`.

- [ ] Commit: classify allocation ownership
  - Arenas for phase-lifetime products.
  - Dynamic arrays for growing compiler tables.
  - Heap allocations only where ownership escapes or lifetime is not phase
    bound.

- [ ] Commit: reduce hot-path allocation churn
  - Focus on LSP reanalysis, formatter passes, HIR generation, and LLVM text
    construction.

### Milestone 8: Final Architecture Review Closeout

- [ ] Commit: update decision records
  - Mark settled choices as accepted, rejected, or superseded.

- [ ] Commit: update `ARCHITECTURE_REVIEW.md`
  - Summarize final pipeline, boundaries, and remaining known risks.

- [ ] Commit: remove obsolete review scaffolding
  - Keep useful audits and measurements.
  - Delete prototypes or notes that no longer describe the chosen direction.

- [ ] Commit: final full verification
  - `just test`
  - `just install`
  - installed compiler smoke tests
  - LSP smoke test if the local editor/server workflow is available
