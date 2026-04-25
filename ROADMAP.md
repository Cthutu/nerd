# Roadmap

This document records the current state of the compiler and the step-by-step
plan to reach the first milestone:

- pass all current tests
- do the work in small, reviewable increments

## Current State

From the current codebase and test suite:

- The pipeline is `lexer -> AST parser -> sema -> IR generator -> C generator -> clang`.
- The parser recognises top-level bindings and `fn () => <expr>`.
- The current function surface also includes block functions `fn () { ... }`
  with explicit `return <expr>` statements.
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
- Neovim/LazyVim support is installed from repo-owned runtime files, including
  filetype detection, syntax highlighting, LSP launch, and Conform
  format-on-save integration.
- The `on` branching surface supports short boolean branches, block-form value
  branches, condition chains, integer ranges, explicit comparison patterns,
  `else`, and branch-local pattern binders.
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
- Use British spelling in git commit messages as well.
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
- Prefer dense language regression tests that exercise several related runtime
  cases in one file. Import `pr` and `prn` from `std.print` to make those
  cases visible in the expected program output, rather than adding one tiny
  test per happy path.
- Deliver new language features horizontally across the toolchain.
  - A feature is not complete until the compiler, formatter, LSP, and testing
    surfaces all support it to the agreed milestone depth.
  - Do not move on to the next language feature while one of those surfaces is
    still knowingly behind.
- Keep compiler error-code ranges phase-specific:
  - `0100`-`0199` lexer
  - `0200`-`0299` parser / AST construction
  - `0300`-`0399` semantic analysis
- Every language-compilation failure point must produce either:
  - a categorised compiler diagnostic with source spans, useful references, and
    appropriate notes or help text, or
  - an ICE when the failure represents a compiler bug or violated internal
    invariant rather than invalid user source.
- Use help messages for contextual fix guidance. Use note messages for more
  static explanatory context that is not tailored to one specific fix.
- Test artefact clean-up must stay developer-friendly:
  - remove generated intermediate files for passing tests
  - keep generated intermediate files for failing tests, so failures can be
    analysed locally
- When a feature changes CLI-visible compile-time behaviour, add at least one
  regression that exercises the real `nerd` command path rather than only
  internal compiler entry points.

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
  - Current formatter coverage includes primitive operators, float literal
    spelling, local declaration alignment, and split const/variable alignment
    sub-paragraphs.

- [ ] 22. Extend the LSP as new language features land.
  - Keep editor-facing behaviour aligned with the compiler's supported syntax
    and semantics.
  - Add or extend LSP tests when the server gains enough test surface.
  - Keep LSP work horizontally synchronised with compiler, formatter, and test
    support for each feature.
  - Treat VS Code and Neovim/LazyVim as paired editor surfaces. When a change
    affects file detection, syntax highlighting, LSP launch/configuration,
    formatting, or install packaging, update both editor integrations in the
    same commit unless one is explicitly out of scope.

- [X] 22a. Add first-class Neovim/LazyVim support.
  - Add `nerd format --stdout <file>` so editor integrations can format through
    stdout without mutating source files directly.
  - Keep Neovim runtime files in `syntax/nerd-nvim` and install them by copying,
    not symlinking, into the local Linux Neovim configuration.
  - Install LazyVim plugin configuration under `~/.config/nvim/lua/plugins`.
  - Install filetype and syntax files under the matching Neovim runtime
    directories.
  - Use the existing LazyVim `conform.nvim` setup for format-on-save.
  - Configure the Nerd LSP for `.n` files through `nvim-lspconfig`.
  - Keep `syntax/nerd-vscode` and `syntax/nerd-nvim` behaviour synchronised for
    editor-facing changes.
  - Defer Tree-sitter highlighting and indentation support to a later editor
    milestone, after the language grammar has settled further.
  - Current implementation status:
    - repo-owned Neovim runtime files are copied by `just install-nvim`
    - LazyVim launches `nerd lsp` through `nvim-lspconfig`
    - Conform formats Nerd buffers via a temporary file and `nerd format`
    - syntax and filetype runtime files are installed alongside the plugin
      configuration

- [X] 23. Keep `just test` as the single full-project test entry point.
  - Language, error, and formatter tests should all pass through it.
  - LSP transcript tests should also pass through it.
  - Expand the test runner rather than creating disconnected test commands.
  - Keep the pass-cleans / fail-keeps artefact behaviour consistent across test
    categories.

## Milestone 3: Constant Folding

- [X] 24. Add constant folding for recognised constant expressions.
  - Fold pure built-in operator expressions on literals.
  - Fold references to any symbol recognised as a constant value, including
    top-level constant bindings.
  - Any pure operation whose inputs are compile-time constants should fold to a
    constant result, whether that is a whole expression tree or only a
    constant subtree.
  - Do not fold operations with side effects or operations whose compile-time
    failure behaviour has not been defined yet.
  - Keep this table-driven and compatible with the AST's RPN layout.
  - Keep the first implementation in semantic side tables keyed by AST node
    index rather than introducing a second AST representation.
  - Let IR lowering consume folded immediates directly so the compiler can
    reduce trivial IR such as `answer = 42` and fully constant returns without
    waiting for a later clean-up pass.

- [X] 25. Prototype constant folding as an AST-local VM-style pass.
  - Prefer a simple stack-based pass over AST node indices.
  - Use light-weight AST-local fold flags in node padding while keeping folded
    values in semantic side tables.
  - Explore in-place AST compaction later if profiling or compile-time
    behaviour shows it is worth the extra index-rewrite complexity.
  - If nodes are compacted or replaced, track index adjustment carefully so
    `a` and `b` links remain valid.
  - Keep actual folded values in semantic side tables first, even if AST
    padding is reused for light-weight fold flags.

- [X] 26. Extend tests for folding behaviour.
  - Add language tests showing folded constant expressions still produce the
    same observable result.
  - Add snapshot expectations where folding should reduce emitted IR or C.
  - Add error coverage if folding introduces new semantic constraints later.

- [X] 27. Eliminate dead top-level constant declarations after folding.
  - If folding proves that a top-level constant binding is no longer required
    by any remaining runtime code, do not emit its global declaration or init
    assignment.
  - Keep `main` and any declarations still reachable from non-folded runtime
    code.
  - Allow fully folded programs to omit unnecessary globals and, when
    possible, avoid emitting an `init` body that does no useful work.

## Milestone 4: Strings And Output Built-ins

- [X] 28. Add UTF-8 string literals as first-class values.
  - Strings are not C strings in the language model.
  - Represent them as fat pointers: address plus byte length.
  - Lower them in generated C via a struct-based representation.
  - Add a helper macro such as `DEF_SLICE` in the C prologue if that keeps the
    representation tidy and reusable.

- [X] 29. Add built-in output functions `pr` and `prn`.
  - Treat them as compiler-known built-ins, not user-defined bindings.
  - Keep their runtime names unprefixed in IR and generated C because they are
    built-ins rather than Nerd-defined symbols.
  - Predefine the corresponding symbols during semantic analysis as external
    built-in functions.
  - Do not allow user shadowing in the initial implementation.

- [X] 30. Restrict `pr` and `prn` to strings in their first implementation.
  - Do not add scalar printing shortcuts.
  - Leave primitive-to-string conversion to interpolated strings and later
    helper routines.

- [X] 31. Extend tests, formatter support, and LSP support for strings.
  - Add language tests for string literals and built-in output.
  - Add error tests for invalid built-in usage.
  - Extend the formatter and LSP at the same time the syntax lands.

- [X] 32. Add explicit `return <expr>` statements in block functions.
  - Treat `return` as a block-only statement, not a general expression form.
  - Lower explicit block returns directly into IR and generated C.
  - Keep compiler, formatter, LSP, and tests in sync as the syntax lands.

## Milestone 5: Primitive Types, Variables, And Casts

- [X] 33. Introduce primitive built-in types.
  - Add signed integers such as `i8`, `i16`, `i32`, and `i64`.
  - Add unsigned integers such as `u8`, `u16`, `u32`, and `u64`.
  - Add floating-point types `f32` and `f64`.
  - Add `bool`, `isize`, and `usize`.
  - Add compact semantic type tables rather than storing type information ad
    hoc in the AST or IR.

- [X] 34. Add explicit type annotations and variable bindings while preserving inference.
  - Place explicit annotations between the colons in bindings.
  - `hello :: "Hello"` should remain equivalent to `hello: string: "Hello"`.
  - Add variable bindings using `:` and `=`.
  - Support the syntax `<var-name> : <optional-type> = <value>`.
  - Support the shorthand `<var-name> := <value>` as sugar for inferred
    variable bindings.
  - Allow variable bindings both at top level and within functions.
  - Treat the annotation on a binding as the type of the bound value itself.
    This applies equally to values, variables, and functions.
  - Example:
    - `main: fn () -> i32: fn () => 1`
    - Here the annotation describes the type of `main`, not the type of
      calling `main()`.
  - Keep inferred types visible to the LSP so editor tooling can surface them.
  - Treat semantic type resolution as a multi-pass analysis problem rather than
    a single-pass walk.
  - Allow later passes to refine earlier placeholder or unresolved inferred
    types as more declaration and usage information becomes available.

- [X] 35. Define integer literal typing rules.
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

- [X] 36. Require exact type matches for arithmetic and add explicit casts.
  - Do not introduce implicit conversions.
  - Extend the primitive operator surface to include:
    - comparison operators `<`, `<=`, `>`, `>=`, `==`, and `!=`
    - bitwise operators `&`, `^`, and `|`
    - logical operators `!`, `&&`, and `||`
    - integer `%`
  - Restrict each operator family semantically:
    - arithmetic and comparisons use matching numeric operands
    - bitwise operators use matching integer operands
    - logical operators use `bool` operands
  - Add explicit casts through a `.as(<type>)` form.
  - Support casts on both named values and literals, for example:
    - `my_byte := my_word.as(u8)`
    - `my_other_byte := 128.as(u8)`
  - If casting from one type to another is not defined by the current language
    rules, report a compiler error.
  - Keep cast validity semantic and table-driven rather than encoding it as
    parser special cases.
  - Keep this no-implicit-casts rule as a language principle.

- [X] 37. Extend tests, formatter support, and LSP support for primitive types, variables, and casts.
  - Add language tests for type annotations, inference, variable bindings,
    exact-match arithmetic, primitive operators, float literals, and casts.
  - Add error tests for mismatched primitive operations, invalid casts, and
    invalid variable/type combinations.
  - Ensure new feature work is covered through the structured error system as
    well as unit tests under `tests/errors`.
  - Extend tooling surfaces at the same time as the compiler support.

## Milestone 6: Interpolated Strings

- [X] 38. Add `$"...{expr}..."` interpolated strings.
  - Keep interpolation distinct from normal string literals.
  - Only strings prefixed with `$` may contain interpolation.
  - Support left-to-right evaluation and append behaviour.

- [X] 39. Keep the first interpolation implementation function-local.
  - Initially allow interpolated strings only inside functions.
  - Defer top-level interpolated bindings until a later milestone if they
    require broader init-time support.

- [X] 40. Lower interpolation through prologue helper routines.
  - Add `to_string$<type>` helpers in the prologue for all primitive types.
  - Restrict conversion support to built-in types in the initial design.
  - Leave user-defined conversion mechanisms for a later trait system.
  - Keep the first lowering explicit in IR rather than hiding it in C codegen.

- [X] 41. Use a simple arena-backed runtime string builder first.
  - A global arena plus helper functions in the prologue is acceptable for the
    first implementation.
  - Build the string, return a fat pointer to it, and reset the arena as
    appropriate for the chosen runtime model.
  - Leave constant-expression optimisation and smarter storage for later work.
  - First IR shape:
    - explicit reset/mark/append/finish operations
    - no hidden sema-side interpolation lowering
    - future VM work should be able to execute the same IR model

- [X] 42. Extend tests, formatter support, and LSP support for interpolated strings.
  - Add language tests for mixed literal and interpolated segments.
  - Add error tests for invalid interpolation forms and unsupported types.
  - Extend tooling surfaces at the same time as the syntax lands.

## Milestone 7: Scopes, Blocks, And Coverage Hardening (Completed)

- [X] 43. Audit and complete compiler error coverage.
  - Keep every compiler error as a structured `ErrorInfo` with a phase-specific
    code.
  - Audit all compile-language failure points and classify each as either a
    user-facing diagnostic or an ICE-only compiler invariant failure.
  - Add at least one unit test for every reachable public error category.
  - Cover meaningful distinct occurrences of reused categories when the source
    span, related information, or help text can differ.
  - Reachable coverage added for `0101`, `0104`, and `0205`.
  - Known hard-limit diagnostics `0102`, `0105`, and `0200` are intentionally
    deferred until there is a synthetic diagnostic harness; exercising them
    through normal source inputs would require intentionally enormous files or
    resource-limit setup.
  - Extend LSP diagnostic transcript coverage for semantic errors that editor
    users are likely to hit, rather than only relying on JSON error tests.
  - Keep error tests under `tests/errors` as the primary structured diagnostic
    coverage.
  - Treat OS, filesystem, shell, and toolchain failures as `runtime-error`
    reports, not user-source diagnostics or ICEs.
  - Treat IR generation and C generation invariant failures as ICEs.

- [X] 44. Introduce lexical variable scopes in semantic analysis.
  - [X] Replace the current function-wide local lookup with explicit scope rows.
  - Keep scope data in semantic side tables; do not enlarge AST nodes for scope
    ownership.
  - [X] Track parent scope, declaration order, and local ownership so lookups
    are lexical and declaration-order aware.
  - [X] Decide and document whether inner scopes may shadow outer locals.
    Inner scopes may shadow outer locals once arbitrary blocks are added; the
    same scope still rejects duplicate locals.
  - [X] Add error tests for duplicate locals in the same scope and references
    before declaration, including self-reference in an initializer.
  - [X] Add invalid-reference-outside-scope tests with arbitrary blocks.
  - [X] Add LSP tests for hover/definition when the same name exists in
    different scopes.

- [X] 45. Add arbitrary block statements.
  - [X] Support standalone `{ ... }` blocks inside function bodies.
  - [X] Treat each block as a lexical scope boundary.
  - [X] Allow statements already supported in function bodies inside nested
    blocks: variable declarations, assignments, expression statements, and
    returns.
  - [X] Define return behaviour for nested blocks: a nested `return` returns
    from the enclosing function; unreachable diagnostics remain future work.
  - [X] Extend IR generation so block scopes do not require CGen or future VM
    back ends to ask semantic side tables for local lifetime information.

- [X] 46. Keep formatter and LSP work synchronised with scoped blocks.
  - [X] Add formatter snapshots for nested blocks, indentation, locals,
    assignments, and returns.
  - [X] Add LSP hover/definition and diagnostic tests for scoped locals.
  - [X] Update documentation in `docs/compiler-pipeline.md` and
    `docs/type-system.md` as the implementation lands.

## Milestone 8: Functions (Completed)

- [X] 47. Add function parameters and call arguments.
  - Support typed parameters in both top-level and nested functions.
  - Extend call parsing, semantic analysis, IR, C generation, formatter, LSP,
    and tests together.
  - Keep argument count and argument type checking exact and explicit.

- [X] 48. Support both expression-bodied and block-bodied function forms.
  - Support inferred-return expression bodies such as:
    - `add :: fn(a: i32, b: i32) => a + b`
  - Support explicit-return block bodies such as:
    - `add :: fn(a: i32, b: i32) -> i32 { return a + b }`
  - Reject mixed explicit return annotations with fat-arrow bodies such as:
    - `add :: fn(a: i32, b: i32) -> i32 => a + b`
  - Keep the rule simple: fat arrow means inferred return, thin arrow means an
    explicit return type in the function type.

- [X] 49. Add nested non-closure functions.
  - Allow nested functions inside function scopes.
  - Nested functions may reference globals and their own parameters and locals.
  - Nested functions may not capture parameters or locals from enclosing
    function scopes.
  - Any outer function value needed by a nested function must be passed
    explicitly as a parameter.
  - Capturing attempts must be semantic errors with contextual help text.
  - Lower nested functions into generated C by flattening lexical names, for
    example `bar` -> `$bar` and nested `foo` inside `bar` -> `$bar$foo`.

- [X] 50. Add function values and function-pointer-compatible assignment.
  - Constant bindings remain the normal way to name functions.
  - Variable bindings may hold unnamed function values or references to named
    functions.
  - Support examples such as:
    - `pfoo := fn(a: i32, b: i32) => a + b`
    - `pfoo := foo`
    - `pfoo : fn(i32, i32) -> i32 = foo`
  - Treat named function values as runtime-callable function values with the
    same semantic signature they expose through hover and diagnostics.
  - Calls automatically use the function-value lowering emitted by the back
    end.
  - Allow reassignment only when the full function signature matches exactly.
  - Keep this fully type-safe; no implicit signature conversions.
  - Generated C lowers these through function-pointer-compatible storage.
  - Defer explicit `^fn(...) -> ...` source syntax until the general
    pointer-type milestone.

- [X] 51. Extend scoped declarations and tooling for local function bindings.
  - Allow forward references for scoped constant declarations of the form
    `<name> :: <value-or-type>` so declaration order does not matter in a
    scope.
  - Keep local variable lookup rules distinct from local declaration lookup
    rules where needed.
  - Add language tests, error tests, formatter snapshots, and LSP coverage for
    nested functions, function values, and invalid captures.

## Milestone 9: `on` Branching And Pattern Matching Foundations

- [X] 52. Add `on` as the branching construct.
  - `on` replaces ad hoc `if`-style branching for the language surface.
  - Support the short boolean form:
    - `on (x > 0) => "positive" else "non-positive"`
  - Current implementation status:
    - short-form boolean `on <bool-expr> => <expr> else <expr>` is supported
    - source-level boolean literals `yes` and `no` are supported
    - block-form `on value { ... }` with simple constant-value branches and
      `else => ...` is supported
    - block-form currently accepts `bool` and concrete integer scrutinees only
    - untyped integer scrutinees materialise to `i32`
    - branch patterns currently must be compile-time constants
    - statement-position block-form `on` may omit `else`; missing cases are a
      no-op
  - Support the block form:
    - `on size { ... }`
  - Prefer syntax that does not require branch separators when the parser can
    unambiguously detect the start of the next branch.

- [x] 53. Add block-form branches, `else`, and simple value/range matching.
  - Support branch bodies as general expressions, including blocks.
  - Support `else => ...` in block form for consistency with other branches.
  - Current implementation status:
    - simple constant value branches are supported
    - comma-separated value alternatives in one branch are supported
    - exclusive and inclusive integer range branches are supported
  - Support comma-separated alternative values in one branch.
  - Support exclusive and inclusive integer ranges through `..` and `..=`.
  - Keep exact type matching throughout; do not add implicit casts.

- [X] 54. Add branch-local pattern binders.
  - Use `<pattern> as <name>` to bind the matched value for one branch.
  - Allow binders on `else` branches as well, for example:
    - `else as other => ...`
  - Binder scope is limited to that branch expression or block.
  - Reusing the same binder name in different branches is valid because branch
    scopes are separate.
  - Keep binders immutable unless a later milestone explicitly adds mutable
    pattern bindings.
  - Current implementation status:
    - value, range, and `else` branches can bind the matched scrutinee through
      `<pattern> as <name>` or `else as <name>`
    - compiler, formatter, LSP, VS Code syntax, Neovim syntax, and regression
      tests are updated together

- [X] 55. Define `on` typing and exhaustiveness rules.
  - Treat `on` as an expression form.
  - When used in a statement position, `on` has type `void`.
  - Non-void `on` expressions must be exhaustive.
  - For broad domains such as integers, require `else` unless the compiler can
    trivially prove exhaustiveness.
  - Void `on` expressions may omit branches because missing cases are a no-op.
  - All value-producing branches must converge to exactly the same type.
  - Current implementation status:
    - statement-position block-form `on` is typed as `void` and may omit `else`
    - value-producing block-form `on` requires `else`
    - all value-producing branches must converge to one semantic type
    - missing `else` on value-producing block-form `on` reports diagnostic 0327

- [X] 56. Add IR merge support for value-producing branches.
  - Introduce phi nodes or equivalent explicit typed merge instructions in IR.
  - Keep the IR self-contained so a future VM can execute the same control-flow
    model without semantic side tables.
  - Current implementation status:
    - short-form and block-form value-producing `on` lower to explicit branch
      labels, jumps, and a typed temporary merge slot in IR
    - statement-position block-form `on` lowers without allocating a merge slot
  - Extend C generation, formatter, LSP, and tests together as `on` lands.

## Milestone 10: Basic `for` Loops

- [X] 57. Add statement-oriented loop forms.
  - [x] Support infinite loops:
    - `for { ... }`
  - [x] Support while-style loops:
    - `for condition { ... }`
  - [x] Support C-style loops:
    - `for init; condition; update { ... }`
  - Defer iterable/range iteration syntax if it materially slows the first loop
    milestone.

- [X] 58. Add loop control flow.
  - [x] Support `break` and `continue`.
  - [x] `continue` in C-style loops must still run the update expression before the
    next condition check.
  - [x] Add semantic validation for invalid `break` and `continue` usage.
  - [x] Treat statement loops as `void` in the initial implementation.

- [X] 59. Keep the first loop milestone horizontal.
  - [x] Add language tests, error tests, formatter snapshots, and LSP support for
    the loop forms that land.
  - [x] Keep generated IR explicit about loop structure rather than hiding control
    flow in C generation.
  - [x] Defer value-producing loop expressions and labelled blocks to the next
    milestone.

## Milestone 11: Labelled Blocks, Expression Blocks, And Loop Expressions

- [X] 60. Add labelled block syntax.
  - Use `$` for labels rather than `@`.
  - Labelled blocks are expression blocks, written `$label { ... }`.
  - Plain `{ ... }` remains statement/block-body syntax.
  - This label model should apply cleanly to loop bodies once loops become
    value-producing expressions.

- [X] 61. Add expression blocks.
  - Add expression-block syntax as:
    - `${ ... }` for an unlabelled expression block
    - `$label { ... }` for a labelled expression block
  - Non-void expression blocks must return a value on every reachable path via
    `break <expr>`.
  - All value-producing `break` statements targeting the same expression block
    must produce the same exact type.
  - Void expression blocks may fall through and may use `break` without a value.
  - Expression blocks allow structured value flow without overloading ordinary
    statement blocks.
  - Keep the semantics compatible with future VM execution and explicit IR.
  - Current implementation status:
    - unlabelled and labelled expression blocks are parsed, formatted, typed,
      lowered through IR/C, and covered by language, error, formatter, and LSP
      tests
    - expression-block results can be assigned to constants and variables
    - labelled `break $label <expr>` can target an outer expression block,
      including across nested expression blocks
    - expression-block path analysis is intentionally shallow for now; nested
      exits to an outer block still need an explicit fallback `break` in the
      target block to prove the result type

- [x] 62. Add labelled `break`/`continue` and loop expressions.
  - [x] Support `break $label <expr>` for expression-block labels.
  - [x] Support labelled loops.
  - [x] Support `continue $label`.
  - [x] Plain `break <expr>` targets the nearest surrounding expression block or
    value-producing loop.
  - [x] Labelled `break $label <expr>` targets the referenced expression block or
    labelled loop.
  - [x] Labelled `continue $label` targets labelled loops only; continuing to a
    non-loop expression block is invalid.
  - [x] Add value-producing loop expressions with `break <expr>`.
  - [x] Keep loop-expression typing exact and explicit.
  - Covered by language, error, formatter, and LSP regression tests.

- [x] 63. Extend typed control-flow merging beyond `on`.
  - [x] Reuse or extend the branch-merge IR model for expression blocks and loop
    expressions.
  - [x] Keep control-flow value merges explicit in IR rather than hidden in later
    code generation.
  - [x] Add `else` branches for loops only when the associated loop has reachable
    value-producing `break` paths.
  - [x] Extend tests, formatter support, LSP coverage, and documentation together.
  - Finite loop expressions now require `else { break <expr> }` so normal
    loop exhaustion has an explicit result value.

## Milestone 12: Tuple Values

- [x] 64. Add tuple syntax and types.
  - Support tuple type syntax `(T1, T2, ...)`.
  - Support one-element tuple type syntax `(T,)`.
  - Keep `(T)` as ordinary grouped type syntax rather than a tuple.

- [x] 65. Add tuple literals and tuple field access.
  - Support tuple literals `(a, b, ...)`.
  - Support one-element tuple literals `(a,)`.
  - Keep `(a)` as ordinary grouped expression syntax.
  - Support tuple field access with zero-based dot indices such as `value.0`.
  - Lower tuple values through explicit IR/C storage.

- [x] 66. Cover tuple values horizontally.
  - Add language, error, formatter, and LSP tests.
  - Add documentation for tuple literals, tuple types, and tuple field access.
  - Support interpolating tuple values as `(x1, x2, ...)` strings when every
    element type is interpolatable.
  - Defer tuple destructuring and tuple patterns to the pattern milestones.

## Milestone 13: Fixed Arrays And Indexing

- [X] 67. Add fixed-size array types and literals.
  - Support fixed array type syntax `[N]T`.
  - Support array literals such as `[1, 2, 3]`.
  - Infer array literal element type from the literal contents or expected type.
  - Keep array length as part of the type.
  - Covered in language, error, formatter, LSP, and type-system docs.

- [X] 68. Add array indexing.
  - Support `array[index]` for fixed arrays.
  - Debug builds should include bounds checks.
  - Release builds may omit bounds checks.
  - Add tests for valid indexing, invalid index types, and debug bounds
    diagnostics or traps.
  - Debug C generation emits fixed-length bounds traps before indexing.

## Milestone 14: Pointer Basics

- [X] 69. Add pointer types and address-of.
  - Support pointer type syntax `^T`.
  - Support address-of for addressable values such as `^x` and `^array[0]`.
  - Keep `^[1, 2, 3]` as pointer-to-array-literal construction, returning a
    single pointer rather than a slice.
  - Covered in language, error, formatter, LSP, and type-system docs.

- [X] 70. Add pointer indexing.
  - Support `pointer[index]` where the pointer element type is known.
  - Apply the same debug/release bounds policy only where a bound is known.
  - Document that raw pointer indexing is inherently lower-level than slices.
  - Pointer indexing requires an integer index and intentionally has no bounds
    check when no bound is known.

## Milestone 15: Slices

- [X] 71. Add slice types and representation.
  - Support slice type syntax `[]T`.
  - Represent slices as fat pointers containing `.data` and `.count`.
  - Support `.data` and `.count` field-style access.
  - Covered in language, error, formatter, LSP, and type-system docs.

- [X] 72. Add explicit slice construction.
  - Support slicing syntax that constructs slices, including `s[a..b]`,
    `s[..b]`, `s[a..]`, and `s[..]`.
  - Support slicing fixed arrays and existing slices.
  - Keep fixed array to slice conversion explicit; `[N]T` should not implicitly
    coerce to `[]T`.
  - Support `[1, 2, 3][..]` as array-literal-to-slice construction.
  - Covered by runtime output and formatter snapshots for each slice form.

- [X] 73. Add slice indexing and bounds behaviour.
  - Support `slice[index]`.
  - Debug builds should include bounds checks.
  - Release builds may omit bounds checks.
  - Cover slice creation, indexing, slicing, `.data`, and `.count` in language,
    error, formatter, and LSP tests.
  - Slice interpolation renders visible contents as `[x1, x2, x3, ...]`.

## Milestone 16: String And Byte Slice Interop

- [X] 74. Keep `string` distinct from `[]u8`.
  - `string` should be representation-compatible with `[]u8`, but not an alias.
  - Preserve the invariant that `string` contains valid UTF-8.
  - Share implementation paths for `.data`, `.count`, comparison, and slicing
    where practical.
  - String `.data` returns `^u8`, `.count` returns `usize`, and string slicing
    returns `string` after runtime UTF-8 boundary validation.

- [X] 75. Add explicit string/slice operations.
  - Decide and document whether string slicing returns `string` only after
    UTF-8 boundary validation, or returns `[]u8`.
  - Keep byte-oriented access available through explicit `[]u8` operations.
  - Extend string comparison and pattern support as needed after the slice
    representation lands.
  - Covered in language, error, formatter, LSP, and type-system docs.

- [X] 75.1. Align exclusive range syntax with slice syntax.
  - Replace exclusive `on` range spelling with `..`.
  - Keep inclusive ranges spelled `..=`.
  - Update lexer token naming, parser/CST handling, formatter snapshots,
    language/error/LSP tests, and documentation together.
  - Treat this as a source-breaking cleanup before the language grows more
    range-like syntax.

## Milestone 17: Plex Types

- [X] 76. Add basic plex definitions.
  - Support `plex { field Type ... }` type syntax.
  - Support named plex aliases.
  - Initially use source-order C-compatible layout for predictable lowering and
    debugging.
  - Keep compiler-reordered layout as a later extension.

- [X] 77. Add plex construction and field access.
  - Support instance literals such as `Point { x: 1, y: 2 }`.
  - Support field access such as `point.x`.
  - Lower plex values through explicit IR/C storage.
  - Add language, error, formatter, LSP, and documentation coverage.
  - MS17 coverage includes named aliases, named literals, direct field access,
    parser disambiguation from block-form `on`, formatter snapshots, and editor
    keyword highlighting for VS Code and Neovim.

## Milestone 18: Plex Ergonomics And Layout Controls

- [X] 78. Add plex update syntax.
  - Support `existing with { field: value }`.
  - Require updated fields to exist and values to match their declared types.

- [X] 79. Add pointer field ergonomics.
  - Support automatic dereference for field access on `^Plex`.
  - Keep the generated IR explicit about the dereference.

- [X] 80. Add plex layout annotations.
  - Support `#c` for explicit source-order C-compatible layout.
  - Support `#packed` for packed layout; packed implies `#c`.
  - Defer compiler-reordered layout until there is a concrete optimisation need.
  - MS18 coverage includes update expressions, pointer field access, `#c` and
    `#packed` formatting, generated packed C structs, semantic diagnostics,
    editor keyword highlighting, LSP tokens, and documentation.

## Milestone 19: Shared Pattern Infrastructure

- [X] 81. Generalise pattern representation.
  - Introduce a shared pattern AST/CST/sema model used by `on` and destructuring.
  - Support `_` ignore patterns.
  - Support binder patterns consistently.
  - Keep exhaustiveness and binder scoping explicit in semantic analysis.
  - Shared AST/CST pattern tables now back `on` branches and destructuring,
    including `_`, binders, tuple/plex pattern storage, semantic binder
    indices, formatter output, and IR lowering hooks.

- [X] 82. Add destructuring bindings.
  - Support tuple destructuring in `::`, `:`, and `:=` forms.
  - Support plex destructuring with field names and shorthand field binders.
  - Support `_` in destructuring assignments.
  - Add formatter, LSP, and error coverage before extending `on`.
  - Completed coverage includes tuple `::`, `:=`, typed `: Type =`,
    assignment `=`, `_` ignores in assignment, plex shorthand destructuring,
    formatter, error diagnostics, and LSP hover/definition coverage.

- [X] 83. Add pattern guards.
  - Support `pattern on condition => expr`.
  - Ensure guard binders are in scope for the guard condition.
  - A guarded branch should not count as exhaustive unless the guard is known
    to be unconditional.
  - Guards are parsed, formatted, type-checked as `bool`, lowered before branch
    bodies, and covered by language, error, format, and LSP tests.

## Milestone 20: Tuple And Plex Patterns In `on`

- [X] 84. Add tuple patterns.
  - Support tuple value patterns in `on`.
  - Support tuple binders and `_` ignores.
  - Keep tuple dot indexing out of patterns; use destructuring instead.
  - Completed across AST/CST parsing, semantic binder scoping, IR lowering,
    formatter, error tests, LSP binder hover/definition, and runtime coverage.

- [X] 85. Add plex patterns.
  - Support plex field patterns in `on`.
  - Support shorthand field binders such as `{ x, y }`.
  - Support ignored fields and partial field matching if the semantics are made
    explicit.
  - Extend exhaustiveness checks where feasible.
  - Completed for explicit field value patterns, shorthand binders, ignored
    fields, partial field matching with explicit `else` where exhaustiveness
    cannot be proven, formatter output, error tests, LSP coverage, and runtime
    coverage.

## Milestone 21: Raw Unions For FFI

- [X] 86. Add raw union types.
  - Support `union { field Type ... }`.
  - Keep raw unions out of pattern matching.
  - Define construction and field access rules with FFI use in mind.
  - Document the safety rule: the programmer is responsible for knowing which
    field is valid.

- [X] 87. Prepare FFI-facing layout guarantees.
  - Align raw union layout with C ABI expectations.
  - Ensure `plex #c`, `plex #packed`, and `union` can be used together for later
    FFI declarations.

## Milestone 22: Enum Unit Variants

- [X] 88. Add enum type declarations.
  - Support `enum { Variant ... }` for unit variants.
  - Lower enum values as a tag plus storage compatible with future payloads.
  - Choose the smallest practical unsigned tag type.

- [X] 89. Add enum construction and matching.
  - Support constructing variants with contextual shorthand such as `Red` when
    the expected enum type is known.
  - Support explicit qualified variants such as `Colour.Red` when context is
    absent or ambiguous.
  - Support matching bare unit variants in enum `on` branches.
  - Add exhaustiveness checks over enum variants.

## Milestone 23: Enum Payloads

- [X] 90. Add tuple-like enum payloads.
  - Support variants such as `Point(f32, f32)`.
  - Lower payload storage through the enum's internal union representation.
  - Support payload access through pattern matching.

- [X] 91. Extend enum pattern matching.
  - Support enum variant binders in `on`.
  - Support `_` ignores inside enum payload patterns.
  - Extend formatter, LSP, error, and documentation coverage.

## Milestone 24: Generalised `on` Syntax

- [X] 92. Add condition-chain `on`.
  - Support `on { condition => expr ... }`.
  - Treat it as ordered boolean branching with no scrutinee.
  - Keep exhaustiveness rules explicit, requiring an `else` or a provably
    exhaustive final branch.
  - Completed across AST/CST parsing, semantic typing, IR lowering, formatter,
    LSP, language tests, and error tests. Value-producing condition chains
    require `else`; statement-form condition chains may omit it.

- [X] 93. Add explicit pattern operators.
  - Support operators before value patterns, such as `== 1`, `> 2`, and `< 3`.
  - Preserve the current implicit `==` behaviour for plain value patterns.
  - Define which operators participate in exhaustiveness analysis.
  - Completed for `==`, `!=`, `<`, `<=`, `>`, and `>=` patterns. Explicit
    comparison values must be compile-time constants; relational operators are
    numeric-only and do not contribute to exhaustiveness analysis.

## Milestone 25: FFI Function Declarations

- [X] 94. Add FFI function declarations and calls.
  - Support declarations such as `ffi "c" fcntl (i32, i32, ...) -> i32`
    and renamed bindings such as
    `set_flags :: ffi "c" fcntl (i32, i32, ...) -> i32`.
  - Allow an omitted return type to mean `void`.
  - Link `ffi "name"` using the platform linker. On POSIX this emits
    `-lname`, so the linker can resolve `libname.a` or `libname.so`; `ffi "c"`
    is treated as the ambient C runtime and does not add an explicit link flag.
  - Reserve `#` annotations for calling conventions such as `#stdcall`; this is
    deferred until Windows FFI is exercised directly.
  - Start with ABI-safe primitive, pointer, `plex #c`, `plex #packed`, and
    `union` types; reject higher-level Nerd-only types unless explicitly
    lowered by a wrapper.
  - Fixed-arity and varargs FFI functions are implemented and covered by
    language, formatter, LSP, generated C, library-linking, and ABI rejection
    tests. Calling convention annotations and Windows-specific link naming
    remain deferred.
  - The `...` token is only valid inside FFI signatures.
  - Keep ABI rules documented and covered by generated C, language, formatter,
    LSP, and error tests.

## Milestone 26: Standard Module Layout

- [X] 95. Add the standard module layout and installer support.
  - Store repo-owned standard modules under `mods/`.
  - Extend `just install` to copy `mods/` next to the installed `nerd`
    executable.
  - Repo/install layout exists: `mods/` is copied beside the installed
    executable.
  - Compiler module-root discovery is wired into semantic analysis for module
    paths: `NERD_LIB_PATH` first, then installed `mods`, then the repo/dev
    `mods` fallback used by `_bin/nerd-debug`.
  - Resolve standard modules from `NERD_LIB_PATH` entries first, then from the
    `mods/` folder next to the running compiler executable.
  - Do not support source-relative module lookup; project-local modules should
    be supplied through `NERD_LIB_PATH`.

## Milestone 27: Named Modules

- [X] 96. Add named module imports.
  - Support `<name> :: mod path.to.module`.
  - Support `use <module>` to bring module exports into the current scope.
  - `use` can appear at top level or inside functions; imports are scoped to
    the containing scope.
  - Allow `mod` bindings wherever normal bindings are allowed, including inside
    functions; the module name follows the usual binding scope rules.
  - Parser, CST, formatter, sema, IR, CLI tests, and LSP support exist for
    named module bindings and `use <module>`, including local function-scope
    imports.
  - Map dotted module paths to rooted files such as `std/print.n`.
  - Treat the right-hand side as a module value whose exported declarations are
    accessed through the chosen binding name.
  - Detect missing modules, malformed module paths, duplicate import names, and
    import cycles with useful diagnostics.
  - Decide the first privacy model before implementation; exported-by-default
    is acceptable for the initial standard library if kept explicit in docs.

## Milestone 28: `std.print`

- [X] 97. Move printing into `std.print`.
  - Define the first standard module as `std.print`.
  - Provide `pr` and `prn` wrappers in Nerd source.
  - Implement those wrappers via FFI to C `printf`.
  - Define the C-string/varargs boundary explicitly: Nerd `string` is a
    counted value, so wrappers must lower to a safe `printf` form such as
    `%.*s` rather than passing Nerd strings as C strings.
  - `mods/std/print.n` is now the repo-owned standard module surface.
  - `pr` and `prn` are no longer available as global built-ins; user code gets
    them through `std.print`.
  - Dense tests cover imported `pr`/`prn`, interpolation, newlines,
    formatter/LSP integration, module-qualified calls, and invalid calls.
  - The remaining follow-up is architectural rather than user-facing: replace
    the current bootstrap resolution path for `std.print` with general module
    loading so the compiler does not special-case that module internally.

## Later Milestones

- [ ] Retire `TODO.md` once its ideas have been integrated into the numbered
  roadmap or deliberately deferred.

## Milestone 29: General Module Loading

- [X] 98. Replace bootstrap module resolution with general module loading.
  - This is the first of the next design-work items and is the one whose
    direction has already been settled.
  - Completed:
    - rooted module-path resolution now lives in shared compiler helpers
    - the old repo-root fallback and `mod.n` folder-module fallback are gone
    - a program-level front-end entry point plus module record/cache scaffold
      now drives recursive loading
    - ordinary build/run/compile paths now go through the program-level loader
    - `pub` is parsed on top-level bindings and `use std.print` is the
      canonical import spelling
    - legacy `use mod ...` has been removed
    - bootstrap resolution for `std.print` has been removed from semantic
      analysis
    - whole-program back-end merging now emits imported module functions into
      the final IR/C/binary
    - debug and release suites both pass after the loader transition
  - Remove compiler-side special-casing for `std.print`.
  - Load module source from configured module roots through the same front-end
    pipeline as normal source files.
  - Build module exports from analysed module declarations rather than
    synthesising them in semantic analysis.
  - Keep module loading deterministic and cache analysed modules per build so
    repeated imports do not reparse or reanalyse the same file.
  - Resolve dotted module paths such as `std.print` to rooted files such as
    `std/print.n`.
  - Support only rooted module lookup; do not add relative module imports.
  - Search module roots in this order:
    - the directory of the root `.n` file being built
    - entries from `NERD_LIB_PATH`, left to right
    - the installed `mods/` directory beside the `nerd` executable
  - Deduplicate loaded modules by canonical resolved file path while preserving
    the dotted module path for diagnostics and lowered-name qualification.
  - Detect and report module load failures and module import cycles with useful
    diagnostics.
  - Keep `data/prelude.c` and `data/epilogue.c` as backend/runtime support for
    generated C, but remove bootstrap modules entirely from the language/module
    system.
  - Completion criterion: no compiler-known bootstrap module resolution remains;
    `std.print` is loaded and exported exactly like any other Nerd module.

## Milestone 30: Module Exports And Privacy

- [ ] 99. Define the first stable module visibility model.
  - This is the second of the next design-work items and its design direction
    has already been settled. Implementation follows general module loading.
  - Use `pub` as the explicit export marker.
  - Allow `pub` only on top-level bindings.
  - Allow public module-valued bindings such as `pub p :: mod std.print` to
    re-export submodules through ordinary namespacing.
  - Define what `use` imports from a module and how non-exported names behave.
  - Keep the initial model simple, documented, and enforced consistently in
    sema, formatter, LSP, and tests.
  - Add regressions for duplicate export names, private-name access, and
    shadowing through `use`.

## Milestone 31: Pattern And Enum Consolidation

- [ ] 100. Consolidate the long-term enum and pattern model.
  - This is the third of the next design-work items and its design direction
    has now been settled.
  - Keep `on` pattern rules explicit and teachable as enums, payloads, and
    structural patterns grow.
  - Allow implicit enum variant scoping only where the expected value is
    unambiguously of the enum type.
  - Keep explicit qualified enum names valid everywhere, for example
    `Colour.RED`.
  - Keep `else` as the wildcard for branching. Reserve `_` for destructuring.
  - Keep binders post-pattern with `as`.
  - Keep guards trailing with `on`.
  - Treat tuple patterns as tuple-shaped value patterns, but require explicit
    binders such as `(2, 3, as x)` so bare names always remain value/name
    matches rather than introducing new symbols implicitly.
  - Treat plex patterns similarly, with explicit field binders such as
    `{ x: 20, y: _, z: as z }`.
  - Do not add array/slice pattern matching yet.
  - Keep formatter and diagnostics aligned with the intended reading order of
    patterns and binders.
  - Add focused tests for ambiguous cases so future syntax changes do not
    erode clarity.

## Milestone 32: FFI Consolidation

- [ ] 101. Keep the FFI surface explicit and ABI-focused.
  - This is the fourth of the next design-work items and its design direction
    has now been settled.
  - Keep the foreign symbol name explicit in the surface syntax, with an
    optional local rename binding.
  - Allow the FFI library operand to be any expression that evaluates to a
    string.
  - Keep omitted return types meaning `void`.
  - Allow `...` only in FFI declarations.
  - Start with the default C ABI only; defer calling-convention syntax until it
    is needed and specified.
  - Restrict FFI signatures to ABI-obvious Nerd types and reject higher-level
    types whose layout or calling convention is not explicit.
  - Preserve the rule that convenience must not hide ABI-sensitive behaviour.
  - Document the supported ABI-safe Nerd types and the rejected higher-level
    types.
  - Add calling-convention support only when the behaviour is specified and
    tested across platforms that need it.
  - Keep FFI diagnostics concrete about why one type/signature is unsafe.

## Milestone 33: Standard Library Expansion

- [ ] 102. Expand the standard library only after module loading and exports are
  stable.
  - This is the fifth of the next design-work items and should stay sequenced
    after module loading and export/privacy work.
  - Treat `std.print` as the first real module and use it to validate the
    module/export design.
  - Add the next standard modules only after the module loader, export rules,
    and import ergonomics are settled.
  - Keep standard library growth disciplined so the core language semantics do
    not drift under a large library surface.

## Milestone 34: Grouped `use` Syntax

- [ ] 103. Add grouped `use` as parser-level syntax sugar.
  - Support grouped forms rooted at one module path, for example:
    `use std.collections { array list.single list.double }`.
  - Treat grouped `use` as pure desugaring into multiple ordinary `use`
    statements in the parser.
  - Keep sema, module loading, export rules, and IR generation unaware of the
    grouped syntax.
  - Prefer direct parser expansion into multiple ordinary `AK_Use` nodes rather
    than introducing a separate grouped-use semantic form.
  - Add formatter and LSP coverage so grouped imports remain stable and
    readable.

## Future Ideas

These items are worth keeping visible, but they are not assigned to a numbered
milestone yet.

- [ ] Support top-level interpolated string bindings once the runtime init model
  is robust enough.
- [ ] Add a generated initialisation path for more than top-level constants when
  later features require runtime setup before `$main`.
- [ ] Optimise interpolated strings for constant expressions once the first
  runtime-based version works.
- [ ] Extend the formatter to reflow long string literals into consecutive
  adjacent string literals, and to merge adjacent short literals when that
  improves readability.
  - Prefer breaking at spaces so normal prose does not split words where
    practical.
  - Keep the emitted literals semantically equivalent to the original source.
- [ ] Add trait-based conversion and formatting support for user-defined types.
- [ ] Revisit whether in-place AST compaction is the best home for constant
  folding after a first implementation exists and can be measured.
- [ ] Add a synthetic diagnostic harness for hard-limit error categories that
  are impractical to trigger through normal source files, including `0102`,
  `0105`, and `0200`.

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
