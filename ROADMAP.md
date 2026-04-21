# Roadmap

This document records the current state of the compiler and the step-by-step
plan to reach the first milestone:

- pass all current tests
- do the work in small, reviewable increments

## Current State

From the current codebase and test suite:

- The pipeline is `lexer -> AST parser -> sema -> IR generator -> C generator -> clang`.
- The parser recognises top-level bindings and `fn () => <expr>`.
- Semantic analysis exists as a separate front-end phase.
- Top-level bindings are resolved through compact semantic tables.
- Dependency tracking and ordering exist for forward-referenced top-level
  bindings.
- IR and C generation model top-level declarations, hidden runtime init code,
  and generated functions such as `$main`.
- The formatter has a CST-backed path, a `format` sub-command, and snapshot
  tests under `tests/format`.
- The LSP supports hover, definition, document symbols, semantic tokens, and
  diagnostics, with transcript tests under `tests/lsp`.
- `just test` runs language, error, formatter, and LSP suites.

## Guiding Rules

- Keep the first milestone narrowly focused on existing tests and
  `just test`.
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
- In generated C, emit a leading `$` prefix only for C symbols that correspond
  directly to Nerd-language names.
- Keep hidden runtime/compiler helper names such as `init` unprefixed.
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
- Deliver new language features horizontally across the toolchain.
  - A feature is not complete until the compiler, formatter, LSP, and testing
    surfaces all support it to the agreed milestone depth.
  - Do not move on to the next language feature while one of those surfaces is
    still knowingly behind.
- Keep compiler error-code ranges phase-specific:
  - `0100`-`0199` lexer
  - `0200`-`0299` parser / AST construction
  - `0300`-`0399` semantic analysis
- Test artefact clean-up must stay developer-friendly:
  - remove generated intermediate files for passing tests
  - keep generated intermediate files for failing tests, so failures can be
    analysed locally

## Immediate Architectural Direction

- Introduce a semantic layer between AST and IR generation.
- Keep parsing responsible for syntax only.
- Make parser token-consumption rules consistent across all parse helpers.
- Move name resolution, declaration classification, constant/function binding,
  and “what does this symbol mean?” into semantic analysis.
- Lower IR from semantic output, not directly from raw AST.
- Represent semantic results as arena-backed index tables keyed by AST node,
  symbol, and declaration indices rather than pointer-linked trees.
- Keep the AST compact and fixed-size. Preserve the current `kind`,
  `token_index`, `a`, and `b` shape if at all possible.
- Do not extend `AstNode` unless there is a strong-measured reason.
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
- Standardise parser entry/exit contracts.
  - Each parse helper should clearly either:
    - require the first token to have already been consumed on entry, or
    - consume the first token itself on entry
  - Use one convention consistently across the parser rather than mixing both.
  - Document the convention in parser helper comments and function naming if
    needed.
- Standardise function comments as code is edited:
  - one short purpose comment
  - a surrounding separator comment such as `//------------------------------------------------------------------------------`
    before each function definition

## Milestone 1: Pass Tests Via `just test` (Completed)

- [X] 1. Record and preserve the failing baseline.
  - Keep `just test` as the main regression gate.
  - Treat the tests under `tests/` as the acceptance set for this milestone.

- [X] 2. Standardise parser token-consumption architecture.
  - Choose one parser contract and apply it consistently across top-level,
    declaration, binding, and expression parsing helpers.
  - Remove mixed assumptions about whether the current token has already been
    consumed.
  - Make helper comments explicit about parser state on entry and exit.

- [X] 3. Restore function-body IR/C generation for the already-parsed `fn`.
  - Make IR generation recognise `AK_FnDef`, `AK_FnStart`, and `AK_FnEnd`.
  - Emit function-scoped IR with explicit `fn ... end` structure so the current
    IR snapshots can pass.
  - Update C generation to emit returns from function bodies rather than only
    from a top-level trailing expression.
  - Goal: pass `tests/language/001` through `005`.

- [X] 4. Fix AST/expression error behaviour to match the existing error tests.
  - Preserve expression-local errors instead of allowing top-level parsing to
    collapse them into `0204 Expected a symbol to start a binding`.
  - Verify missing value, missing operator, and delimiter cases produce the
    expected codes and source spans.
  - Goal: pass `tests/errors/002` and `003` without weakening current parser
    structure.

- [X] 5. Add a semantic analysis phase to the front end.
  - Extend the front-end pipeline to `lex -> parse -> sema -> ir`.
  - Reserve semantic diagnostics for the `0300`-`0399` error-code range.
  - Define a semantic output structure for:
    - top-level constant bindings
    - top-level function bindings
    - symbol references inside expressions
  - Store semantic facts in compact tables and side tables, not ad hoc heap
    nodes.
  - Keep the AST syntax-only; do not encode semantic meaning by expanding AST
    node shape unless there is no cheaper alternative.
  - Keep this minimal at first: enough for the current tests.

- [X] 6. Introduce a top-level symbol table.
  - Map binding names to semantic declarations.
  - Distinguish at least:
    - constant/value bindings
    - function bindings
  - Reject duplicate bindings and unresolved symbols once semantic diagnostics
    are added.

- [X] 7. Resolve symbol references in expressions.
  - Allow `main :: fn () => answer / magic_number` to resolve both names.
  - Support referencing earlier top-level constant bindings from function bodies.
  - Support forward references between top-level bindings.

- [X] 8. Lower semantic bindings into IR.
  - Emit IR for top-level constants and functions from semantic output.
  - Keep the first implementation simple:
    - constants lower to named/global declarations
    - functions lower to separate IR function bodies
  - Ensure `main` is explicitly recognised as the binary entry point.

- [X] 9. Build dependency tracking and ordering for top-level bindings.
  - Record top-level binding dependencies during semantic analysis.
  - Derive an ordered declaration/definition sequence from those dependencies.
  - Report useful cycle diagnostics when a binding dependency loop is found.

- [X] 10. Extend C generation to match the new IR model.
  - Generate top-level declarations in dependency-safe order.
  - Generate one C function per Nerd function.
  - Preserve the `$` prefix for C symbols that correspond to Nerd-visible names,
    including `$main`.
  - Keep hidden runtime helpers such as `init` unprefixed because they are not
    part of the Nerd-language naming surface.
  - Emit top-level constants as globals/constants rather than trying to inline
    or fold them in the compiler.
  - Keep generated output stable enough for snapshot-style tests.

- [X] 11. Add and update tests as each increment lands.
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

## First Execution Order

This is the intended first coding sequence for milestone 1:

1. Standardise parser token consumption and comments in the parser files.
2. Restore function-aware IR/C generation, so the arithmetic language tests can
   pass again.
3. Fix the current parser error behaviour to match the existing error tests.
4. Add semantic tables, symbol resolution, and dependency tracking for
   top-level bindings.
5. Extend IR and C generation for global constants and multiple functions.
6. Add and refresh tests as each capability lands.

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

## Milestone 2: Tooling Extensions (Completed)

These are important, but should follow milestone 1 unless a small piece is
needed earlier.

- [X] 17. Extend LSP support for bindings, symbols, and semantic diagnostics.
  - Hover for bound symbols
  - diagnostics from semantic analysis
  - document updates for the new front-end phases
  - definition, document symbols, and semantic tokens
  - LSP transcript regression tests under `tests/lsp`

- [X] 18. Introduce a CST for formatting and source-preserving tooling.
  - Keep AST focused on semantics-oriented structure.
  - Use CST for formatting, precise token ownership, and future refactors.
  - Treat CST as the planned formatter architecture rather than relying on a
    lexer/state-machine formatter as the long-term design.
  - Introduce it before richer syntax such as strings, primitive types, and
    interpolated strings make formatter ownership too implicit or brittle.

- [X] 19. Add a `format` sub-command.
  - Accept an input source file.
  - Initially write formatted output to `<input filename>.format`.
  - Keep formatting rules fixed and deterministic.
  - Support comment reflow and word-wrapping as part of the formatter design.

- [X] 20. Add formatter test support under `tests/format`.
  - Define a stable test file format for formatter input/output snapshots.
  - Make `just test` run formatter tests alongside language and error tests.
  - Keep formatter outputs stable enough for snapshot comparison.
  - Apply the same artefact clean-up policy used by language tests.

- [ ] 21. Extend the formatter as new language features land.
  - Every syntax feature added to the compiler should gain formatter support.
  - Add formatter regression tests at the same time as language/error tests.
  - Keep formatter work horizontally synchronised with compiler, LSP, and test
    support for each feature.

- [ ] 22. Extend the LSP as new language features land.
  - Keep editor-facing behaviour aligned with the compiler's supported syntax
    and semantics.
  - Add or extend LSP tests when the server gains enough test surface.
  - Keep LSP work horizontally synchronised with compiler, formatter, and test
    support for each feature.

- [X] 23. Keep `just test` as the single full-project test entry point.
  - Language, error, and formatter tests should all pass through it.
  - LSP transcript tests should also pass through it.
  - Expand the test runner rather than creating disconnected test commands.
  - Keep the pass-cleans / fail-keeps artefact behaviour consistent across test
    categories.

## Milestone 3: Constant Folding

- [ ] 24. Add constant folding for recognised constant expressions.
  - Fold pure built-in operator expressions on literals.
  - Fold references to any symbol recognised as a constant value, including
    top-level constant bindings.
  - Keep this table-driven and compatible with the AST's RPN layout.
  - Keep the first implementation in semantic side tables keyed by AST node
    index rather than introducing a second AST representation.
  - Let IR lowering consume folded immediates directly so the compiler can
    reduce trivial IR such as `answer = 42` and fully constant returns without
    waiting for a later clean-up pass.

- [ ] 25. Prototype constant folding as an AST-local VM-style pass.
  - Prefer a simple stack-based pass over AST node indices.
  - Explore in-place AST compaction later if profiling or compile-time
    behaviour shows it is worth the extra index-rewrite complexity.
  - If nodes are compacted or replaced, track index adjustment carefully so
    `a` and `b` links remain valid.
  - Keep actual folded values in semantic side tables first, even if AST
    padding is later reused for light-weight fold flags.

- [ ] 26. Extend tests for folding behaviour.
  - Add language tests showing folded constant expressions still produce the
    same observable result.
  - Add snapshot expectations where folding should reduce emitted IR or C.
  - Add error coverage if folding introduces new semantic constraints later.

## Milestone 4: Strings And Output Built-ins

- [ ] 27. Add UTF-8 string literals as first-class values.
  - Strings are not C strings in the language model.
  - Represent them as fat pointers: address plus byte length.
  - Lower them in generated C via a struct-based representation.
  - Add a helper macro such as `DEF_SLICE` in the C prologue if that keeps the
    representation tidy and reusable.

- [ ] 28. Add built-in output functions `pr` and `prn`.
  - Treat them as compiler-known built-ins, not user-defined bindings.
  - Rename the prologue implementations to `$pr` and `$prn`.
  - Predefine the corresponding symbols during semantic analysis as external
    functions.
  - Do not allow user shadowing in the initial implementation.

- [ ] 29. Restrict `pr` and `prn` to strings in their first implementation.
  - Do not add scalar printing shortcuts.
  - Leave primitive-to-string conversion to interpolated strings and later
    helper routines.

- [ ] 30. Extend tests, formatter support, and LSP support for strings.
  - Add language tests for string literals and built-in output.
  - Add error tests for invalid built-in usage.
  - Extend the formatter and LSP at the same time the syntax lands.

## Milestone 5: Primitive Types

- [ ] 31. Introduce primitive built-in types.
  - Add signed integers such as `i8`, `i16`, `i32`, and `i64`.
  - Add unsigned integers such as `u8`, `u16`, `u32`, and `u64`.
  - Add floating-point types `f32` and `f64`.
  - Add `bool`, `isize`, and `usize`.
  - Add compact semantic type tables rather than storing type information ad
    hoc in the AST or IR.

- [ ] 32. Add explicit type annotations while preserving inference.
  - Place explicit annotations between the colons in bindings.
  - `hello :: "Hello"` should remain equivalent to `hello: string: "Hello"`.
  - Keep inferred types visible to the LSP so editor tooling can surface them.
  - Treat semantic type resolution as a multi-pass analysis problem rather than
    a single-pass walk.
  - Allow later passes to refine earlier placeholder or unresolved inferred
    types as more declaration and usage information becomes available.

- [ ] 33. Define integer literal typing rules.
  - Top-level numeric bindings are initially untyped integers until use fixes
    their type.
  - Using an untyped integer in a typed context should adopt that target type
    if valid.
  - If an untyped integer remains unconstrained when materialised as a value,
    treat it as `i32`.
  - Function return positions are also a typing context.
  - If a function body resolves to an integer expression and no narrower type
    is forced by context, infer the return type as `i32`.
  - Example:
    - `value :: 120`
    - `a : u8 : value`
    - `b :: value`
    - Here `value` starts untyped, `a` becomes `u8`, and `b` becomes `i32`.

- [ ] 34. Require exact type matches for arithmetic.
  - Do not introduce implicit conversions.
  - Add explicit casts later through a `.cast(<type>)` form.
  - Keep this no-implicit-casts rule as a language principle.

- [ ] 35. Extend tests, formatter support, and LSP support for primitive types.
  - Add language tests for type annotations, inference, and exact-match
    arithmetic.
  - Add error tests for mismatched primitive operations.
  - Extend tooling surfaces at the same time as the compiler support.

## Milestone 6: Interpolated Strings

- [ ] 36. Add `$"...{expr}..."` interpolated strings.
  - Keep interpolation distinct from normal string literals.
  - Only strings prefixed with `$` may contain interpolation.
  - Support left-to-right evaluation and append behaviour.

- [ ] 37. Keep the first interpolation implementation function-local.
  - Initially allow interpolated strings only inside functions.
  - Defer top-level interpolated bindings until a later milestone if they
    require broader init-time support.

- [ ] 38. Lower interpolation through prologue helper routines.
  - Add `to_string$<type>` helpers in the prologue for all primitive types.
  - Restrict conversion support to built-in types in the initial design.
  - Leave user-defined conversion mechanisms for a later trait system.

- [ ] 39. Use a simple arena-backed runtime string builder first.
  - A global arena plus helper functions in the prologue is acceptable for the
    first implementation.
  - Build the string, return a fat pointer to it, and reset the arena as
    appropriate for the chosen runtime model.
  - Leave constant-expression optimisation and smarter storage for later work.

- [ ] 40. Extend tests, formatter support, and LSP support for interpolated strings.
  - Add language tests for mixed literal and interpolated segments.
  - Add error tests for invalid interpolation forms and unsupported types.
  - Extend tooling surfaces at the same time as the syntax lands.

## Future Ideas

These items are worth keeping visible, but they are not assigned to a numbered
milestone yet.

- [ ] Support top-level interpolated string bindings once the runtime init model
  is robust enough.
- [ ] Add a generated initialisation path for more than top-level constants when
  later features require runtime setup before `$main`.
- [ ] Optimise interpolated strings for constant expressions once the first
  runtime-based version works.
- [ ] Add trait-based conversion and formatting support for user-defined types.
- [ ] Revisit whether in-place AST compaction is the best home for constant
  folding after a first implementation exists and can be measured.

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

- [x] `just test` passes
- [x] binding resolution exists for current top-level constants/functions
- [x] semantic analysis exists as a distinct front-end phase
- [x] dependency ordering exists for forward-referenced top-level bindings
- [x] IR and C generation support functions and top-level declarations
- [x] touched compiler files have improved comments and British spelling
