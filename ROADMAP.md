# Roadmap

This document records the current state of the compiler and the step-by-step
plan to reach the first milestone:

- pass all current tests
- compile [`nerd-src/main.n`](/home/matt/nerd/nerd-src/main.n)
- do the work in small, reviewable increments

## Current State

From the current codebase and test suite:

- The pipeline is `lexer -> AST parser -> IR generator -> C generator -> clang`.
- The parser now recognises top-level bindings and `fn () => <expr>`.
- The AST contains binding and function nodes, but IR generation still only
  lowers plain expression trees and only returns when the final AST node is
  `AK_Expression`.
- The C generator emits a single hard-coded `int $main() { ... }` body and does
  not yet model top-level declarations.
- The test suite currently fails at the language and AST-error layers:
  - `0/6` language tests passing
  - `2/7` error tests passing
- `tests/language/006-global-vars.t` already expects global binding resolution,
  but there is no symbol table or semantic analysis phase yet.

## Guiding Rules

- Keep the first milestone narrowly focused on existing tests and
  `nerd-src/main.n`.
- Add new infrastructure only when it directly simplifies that path.
- Prefer stable intermediate representations over ad hoc cross-stage logic.
- Refactor toward explicit front-end phases instead of growing parser-side
  special cases.
- Keep new compiler data structures arena-based, table-based, and index-based.
  Prefer dense parallel tables and side tables over pointer-heavy object graphs.
- Improve code comments as code is touched.
- Use British spelling throughout code, comments, tests, and documentation.
  This applies to identifiers and filenames where practical, so prefer names
  such as `analyse`, `optimise`, `behaviour`, and `initialise`.
- In generated C, emit all compiler-generated function names with a leading
  `$` prefix to distinguish them from types and helper functions supplied by the
  C prologue and epilogue.
- Allow forward references between top-level bindings. Track dependencies
  explicitly and order declarations/definitions from that dependency
  information.
- Keep top-level constant bindings simple in milestone 1: lower them as
  globals/constants and let the C compiler handle optimisation.
- Treat `main` as the only entry point for now. The only supported function form
  in milestone 1 is the expression-bodied function expression
  `fn () => <expression>`.
- When touching a file, prefer broader comment and spelling clean-up in that
  file rather than the minimum local edit.
- As new language features are added, update all relevant surfaces together:
  language tests, error tests, LSP support, formatter support, and `just test`
  coverage.
- Test artefact clean-up must stay developer-friendly:
  - remove generated intermediate files for passing tests
  - keep generated intermediate files for failing tests so failures can be
    analysed locally

## Immediate Architectural Direction

- Introduce a semantic layer between AST and IR generation.
- Keep parsing responsible for syntax only.
- Move name resolution, declaration classification, constant/function binding,
  and “what does this symbol mean?” into semantic analysis.
- Lower IR from semantic output, not directly from raw AST.
- Represent semantic results as arena-backed index tables keyed by AST node,
  symbol, and declaration indices rather than pointer-linked trees.
- Keep the AST compact and fixed-size. Preserve the current `kind`,
  `token_index`, `a`, and `b` shape if at all possible.
- Do not extend `AstNode` unless there is a strong measured reason.
- Preserve the AST's RPN-friendly structure so it remains suitable for
  stack/VM-style passes, constant folding, and later compile-time execution.
- Compute spans from token and node boundaries rather than storing large span
  payloads in AST nodes. For multi-token spans, continue using begin/end node
  conventions plus token-end calculation helpers.
- Prefer semantic side tables over repurposing AST storage. Padding fields are
  available if needed later, but should not be the first choice.
- Add helper macros/tables where they reduce repetitive parser and semantic
  boilerplate, especially for:
  - token-to-AST operator mapping
  - token classification (`starts expr`, `starts decl`, infix precedence)
  - semantic node iteration / emit patterns
- Standardise function comments as code is edited:
  - one short purpose comment
  - a surrounding separator comment such as `//------------------------------------------------------------------------------`
    before each function definition

## Milestone 1: Pass Tests And Compile `main.n`

- [ ] 1. Record and preserve the failing baseline.
  - Keep `just test` as the main regression gate.
  - Treat `tests/language/*.t`, `tests/errors/*.e`, and `nerd-src/main.n` as the
    acceptance set for this milestone.

- [ ] 2. Restore function-body IR/C generation for the already-parsed `fn`.
  - Make IR generation recognise `AK_FnDef`, `AK_FnStart`, and `AK_FnEnd`.
  - Emit function-scoped IR with explicit `fn ... end` structure so the current
    IR snapshots can pass.
  - Update C generation to emit returns from function bodies rather than only
    from a top-level trailing expression.
  - Goal: pass `tests/language/001` through `005`.

- [ ] 3. Fix AST/expression error behaviour to match the existing error tests.
  - Preserve expression-local errors instead of allowing top-level parsing to
    collapse them into `0204 Expected a symbol to start a binding`.
  - Verify missing value, missing operator, and delimiter cases produce the
    expected codes and source spans.
  - Goal: pass `tests/errors/002` and `003` without weakening current parser
    structure.

- [ ] 4. Add a semantic analysis phase to the front end.
  - Extend the front-end pipeline to `lex -> parse -> sema -> ir`.
  - Define a semantic output structure for:
    - top-level constant bindings
    - top-level function bindings
    - symbol references inside expressions
  - Store semantic facts in compact tables and side tables, not ad hoc heap
    nodes.
  - Keep the AST syntax-only; do not encode semantic meaning by expanding AST
    node shape unless there is no cheaper alternative.
  - Keep this minimal at first: enough for current tests and `main.n`.

- [ ] 5. Introduce a top-level symbol table.
  - Map binding names to semantic declarations.
  - Distinguish at least:
    - constant/value bindings
    - function bindings
  - Reject duplicate bindings and unresolved symbols once semantic diagnostics
    are added.

- [ ] 6. Resolve symbol references in expressions.
  - Allow `main :: fn () => answer / magic_number` to resolve both names.
  - Support referencing earlier top-level constant bindings from function bodies.
  - Support forward references between top-level bindings.

- [ ] 7. Lower semantic bindings into IR.
  - Emit IR for top-level constants and functions from semantic output.
  - Keep the first implementation simple:
    - constants lower to named/global declarations
    - functions lower to separate IR function bodies
  - Ensure `main` is explicitly recognised as the binary entry point.

- [ ] 8. Build dependency tracking and ordering for top-level bindings.
  - Record top-level binding dependencies during semantic analysis.
  - Derive an ordered declaration/definition sequence from those dependencies.
  - Decide where cycle handling belongs and report useful diagnostics when it is
    not yet supported.

- [ ] 9. Extend C generation to match the new IR model.
  - Generate top-level declarations in dependency-safe order.
  - Generate one C function per Nerd function.
  - Preserve the `$` prefix for every compiler-generated C function name,
    including `$main`, so generated symbols cannot collide with prologue or
    epilogue definitions.
  - Emit top-level constants as globals/constants rather than trying to inline
    or fold them in the compiler.
  - Keep generated output stable enough for snapshot-style tests.

- [ ] 10. Add and update tests as each increment lands.
  - Add focused language tests for:
    - top-level constant lookup
    - symbol use inside function bodies
    - multiple bindings before `main`
  - Add semantic error tests for:
    - unknown symbol
    - duplicate binding
    - invalid use of non-function/non-constant bindings as features expand
  - Preserve the current artefact rule for language tests: `.ir`, `.c`, and
    `.out` files are removed on success and retained on failure.

- [ ] 11. Compile and run `nerd-src/main.n`.
  - Treat this as the final integration check for milestone 1.
  - Confirm it compiles through the normal pipeline, not a test-only path.

## Parser And Sema Simplification Track

These changes should be made when they reduce friction on milestone 1 work, not
as a separate rewrite.

- [ ] 12. Replace scattered token classification logic with small shared tables
  or macros.
  - Examples: “token starts expression”, “token starts declaration”, operator
    precedence, token-to-AST-kind mapping.

- [ ] 13. Introduce compact AST/Sema emit helpers.
  - Keep repetitive node construction out of parser control flow.
  - Use consistent helper/macros for node creation, span capture, and index
    linking.

- [ ] 14. Separate syntax nodes from semantic meaning cleanly.
  - Avoid baking symbol resolution or declaration meaning into parser state.
  - Prefer AST query helpers plus semantic side tables over parser-time hacks.

- [ ] 15. Keep semantic analysis VM-friendly and table-driven.
  - Treat the AST as a compact instruction/data stream that can be processed in
    linear passes.
  - Keep semantic data in arena-backed side tables keyed by AST node index.
  - Track top-level dependencies at binding granularity in milestone 1.

- [ ] 16. Improve function-level comments in touched files.
  - Add a separator comment block before each function definition in files
    touched during this milestone.
  - Use short comments that explain purpose or invariant, not line-by-line
    narration.
  - Normalise British spelling in touched files as part of the clean-up.

## Milestone 2: Tooling Extensions

These are important, but should follow milestone 1 unless a small piece is
needed earlier.

- [ ] 17. Extend LSP support for bindings, symbols, and semantic diagnostics.
  - Hover for bound symbols
  - diagnostics from semantic analysis
  - document updates for the new front-end phases

- [ ] 18. Introduce a CST for formatting and source-preserving tooling.
  - Keep AST focused on semantics-oriented structure.
  - Use CST for formatting, precise token ownership, and future refactors.

- [ ] 19. Add a `format` sub-command.
  - Accept an input source file.
  - Initially write formatted output to `<input filename>.format`.
  - Keep formatting rules fixed and deterministic.
  - Support comment reflow and word-wrapping as part of the formatter design.

- [ ] 20. Add formatter test support under `tests/format`.
  - Define a stable test file format for formatter input/output snapshots.
  - Make `just test` run formatter tests alongside language and error tests.
  - Keep formatter outputs stable enough for snapshot comparison.
  - Apply the same artefact clean-up policy used by language tests.

- [ ] 21. Extend the formatter as new language features land.
  - Every syntax feature added to the compiler should gain formatter support.
  - Add formatter regression tests at the same time as language/error tests.

- [ ] 22. Extend the LSP as new language features land.
  - Keep editor-facing behaviour aligned with the compiler's supported syntax
    and semantics.
  - Add or extend LSP tests when the server gains enough test surface.

- [ ] 23. Keep `just test` as the single full-project test entry point.
  - Language, error, and formatter tests should all pass through it.
  - Expand the test runner rather than creating disconnected test commands.
  - Keep the pass-cleans / fail-keeps artefact behaviour consistent across test
    categories.

## Semantic Layout Sketch

The semantic phase should remain compatible with the performance goals and AST
shape:

- `decls[]`
  One row per top-level binding.
- `symbol_to_decl`
  Map from interned symbol handle to declaration index.
- `node_info[]`
  Side table keyed by AST node index for resolved declaration links and other
  semantic facts needed by milestone 1.
- `deps[]`
  Flat dependency edge table storing `(from_decl, to_decl)` pairs.
- `ordered_decls[]`
  Top-level declaration indices in dependency order.

This keeps semantic analysis arena-backed, table-based, cache-friendly, and
compatible with later VM-style processing.

## Definition Of Done For Milestone 1

- [ ] `just test` passes
- [ ] `nerd-src/main.n` builds successfully through the normal compiler path
- [ ] binding resolution exists for current top-level constants/functions
- [ ] semantic analysis exists as a distinct front-end phase
- [ ] dependency ordering exists for forward-referenced top-level bindings
- [ ] IR and C generation support functions and top-level declarations
- [ ] touched compiler files have improved comments and British spelling
