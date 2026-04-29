# Roadmap

This document records current project guidance and active work. Completed
milestone plans should live in git history, tests, and documentation rather than
continuing to grow this file.

## Current State

- The compiler pipeline is `lexer -> CST/AST parser -> sema -> IR -> C -> clang`.
- Semantic analysis is a distinct front-end phase and owns name resolution,
  declaration classification, typing, diagnostics, and dependency ordering.
- The AST should remain compact and syntax-oriented. Semantic facts belong in
  arena-backed side tables keyed by indices.
- The CST owns source-preserving tooling such as formatting and editor-facing
  token structure.
- `just test` is the full-project regression gate. It runs language, error,
  formatter, LSP, and command tests.
- Modules load through the program-level front end. Standard modules live under
  `mods/` and should be treated like normal source modules.
- Public module exports are explicit with `pub`. Non-exported module members
  are private.

## Working Rules

- Do the work in small, reviewable increments.
- Prefer stable intermediate representations over ad hoc cross-stage logic.
- Keep new compiler data structures arena-based, table-based, and index-based.
  Prefer dense parallel tables and side tables over pointer-heavy object graphs.
- Avoid expanding `AstNode` unless there is a strong measured reason.
- Compute spans from token and node boundaries rather than storing large span
  payloads in AST nodes.
- Preserve the AST's RPN-friendly structure so it remains suitable for
  stack/VM-style passes, constant folding, and later compile-time execution.
- Keep parser responsibilities syntactic. Put symbol meaning, declaration
  meaning, type resolution, and language rules in semantic analysis.
- Use British spelling throughout code, comments, tests, documentation, and git
  commit messages where practical.
- In generated C, emit a leading `$` prefix only for C symbols that correspond
  directly to Nerd-language names. Keep hidden runtime/compiler helper names
  such as `init` unprefixed.
- Allow forward references between declarations where the language already
  supports them. Track dependencies explicitly and order declarations from that
  information.
- Keep top-level constant bindings simple: lower them as globals/constants and
  let the C compiler handle optimisation unless a specific compiler optimisation
  has been designed.
- When touching a compiler file, improve nearby comments and spelling where it
  is useful, but do not turn local fixes into unrelated rewrites.

## Horizontal Feature Policy

New language features should land across the toolchain together:

- compiler parsing and semantic analysis
- IR and C generation
- formatter support
- LSP/editor support where relevant
- language tests
- error tests for invalid source
- format/LSP/command tests where the feature affects those surfaces
- user-facing docs when the language surface changes

Prefer dense language regression tests that exercise related runtime cases in
one file. Use `std.io` for visible runtime output rather than adding many tiny
happy-path tests.

Do not move on from a language feature while one of the agreed compiler,
formatter, LSP, or testing surfaces is knowingly behind.

## Diagnostic Policy

- Keep compiler error-code ranges phase-specific:
  - `0100`-`0199` lexer
  - `0200`-`0299` parser / AST construction
  - `0300`-`0399` semantic analysis
- Keep the fuller diagnostic design policy in
  [docs/error-system.md](docs/error-system.md), and update that document when
  error-shaping rules change.
- Every language-compilation failure point must produce either:
  - a categorised compiler diagnostic with source spans, useful references, and
    appropriate notes or help text, or
  - an ICE when the failure represents a compiler bug or violated internal
    invariant rather than invalid user source.
- Use notes for explanatory context about why an error happened or which rule
  applies.
- Use help for actionable fix guidance.
- Keep `nerd explain <code>` category-level rather than case-level, so error
  codes remain broad and stable.
- Treat OS, filesystem, shell, and toolchain failures as runtime errors, not
  user-source diagnostics or ICEs.
- Treat IR generation and C generation invariant failures as ICEs.

## Test Artefact Policy

- Passing tests should remove generated intermediate files.
- Failing tests should keep generated intermediate files so failures can be
  analysed locally.
- When a feature changes CLI-visible compile-time behaviour, add at least one
  regression that exercises the real `nerd` command path rather than only
  internal compiler entry points.

## Parser And Sema Direction

- Replace scattered token classification logic with small shared tables or
  macros when doing so reduces local complexity.
- Use compact AST/sema emit helpers where they remove repetitive parser or
  semantic boilerplate.
- Keep semantic analysis VM-friendly and table-driven:
  - process AST/CST data through linear or index-based passes where practical
  - store semantic data in side tables keyed by AST node, symbol, declaration,
    local, and scope indices
  - keep dependency data explicit
- Standardise parser helper contracts:
  - each helper should clearly either require the first token to have already
    been consumed, or consume it itself
  - comments and function names should make the contract clear where ambiguity
    is likely
- Standardise function comments as files are edited:
  - one short purpose comment when it helps
  - separator comments before major internal functions where the file already
    uses that style

## Active Work

### Standard Library Expansion

- Continue expanding the standard library only as the module/export model needs
  real validation.
- Keep the standard library surface disciplined while the core language is still
  moving.
- Prefer simple modules that exercise existing language features before adding
  library-driven language changes.
- Keep `docs/stdlib.md` as a separate standard-library document; the language
  manual should reference the standard library only for small examples.

### `.size` Operator

- Add a `.size` postfix operator for value-size queries.
- Define `.size` as Nerd's own `sizeof`-style operation, based on Nerd type and
  layout rules rather than current C backend behaviour.
- Allow `.size` on any expression whose type is known.
- Return type should be `usize`.
- Treat `void.size` as `0`.
- Treat untyped integers as materialised runtime integers for `.size`, so
  `128.size` follows the size of the default materialised integer type.
- For arrays, return the full aggregate size in bytes, not element count.
- For slices and strings, return the size of the slice/string header, not
  `count * element_size`.
- For dynamic arrays `[..]T`, keep `.count` as the live element count and
  `.capacity` as the reserved element capacity.
- For dynamic arrays `[..]T`, make `.size` return the size of the dynamic-array
  value/header itself, not `count * element_size`, not `capacity * element_size`,
  and not header-plus-owned-storage bytes.
- If total owned storage size is needed later for dynamic arrays, add a distinct
  property or method rather than overloading `.size`.
- For `nil`, return `0` while it remains `nil`-typed; once coerced to a pointer
  or slice, `.size` should follow the target runtime representation.
- Function values should report pointer size.
- Keep module values invalid for `.size` unless a concrete runtime
  representation is later introduced and specified.

### String Surface Follow-through

- Support top-level interpolated string bindings once the runtime init model is
  robust enough.
- Optimise interpolated strings for constant expressions once the runtime-based
  implementation has settled.
- Extend the formatter to reflow long string literals into consecutive adjacent
  string literals, and to merge adjacent short literals when that improves
  readability.
- Prefer breaking strings at spaces so normal prose does not split words where
  practical.
- Keep emitted literals semantically equivalent to the original source.

### Formatter And LSP Follow-through

- Keep extending formatter support as syntax lands.
- Keep extending LSP support as syntax and semantics land.
- Treat VS Code and Neovim/LazyVim as paired editor surfaces. When a change
  affects file detection, syntax highlighting, LSP launch/configuration,
  formatting, or install packaging, update both editor integrations in the same
  commit unless one is explicitly out of scope.

### Definite Assignment Diagnostics

- Add semantic read-before-assignment checks for mutable storage that can be
  declared without an initial value or with `undefined`.
- Keep the language rule simple for users: a value must be assigned before it
  is read, and assignments must match the variable's type.
- Cover the feature with error tests for invalid reads, plus language tests for
  valid assignment-before-read paths.

## Deferred Work

These items are worth keeping visible, but they should not be treated as
near-term tasks without a fresh design pass.

- Trait-based conversion and formatting support for user-defined types.
- In-place AST compaction for constant folding.
- Tree-sitter highlighting and indentation support.
- Calling-convention annotations for FFI, including platform-specific behaviour.
- A synthetic diagnostic harness for hard-limit error categories that are
  impractical to trigger through normal source files, including `0102`, `0105`,
  and `0200`.
- Retiring `TODO.md` once its ideas have been integrated into active roadmap
  work or deliberately discarded.
