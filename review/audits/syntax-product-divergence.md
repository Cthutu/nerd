# Syntax Product Divergence Audit

Status: active
Date: 2026-05-12

## Purpose

Nerd currently has two syntax products:

- `Ast`, the compact compiler syntax tree consumed by sema and HIR generation
- `Cst`, the concrete syntax tree consumed by formatter and some editor tooling

The split has been useful: the AST stays compact, while CST preserves grouping
and source-oriented constructs. The risk is grammar drift. A syntax change must
usually be added to both parsers, and editor tooling can lose useful partial
structure when CST recovery is weaker than the token stream.

## Current Shape

Both products cover the main language families:

- literals and interpolated strings
- unary, binary, call, cast, field, index, slice, tuple, array, plex, and range
  expressions
- `on` expressions and patterns
- function, FFI, use, impl, top-level `on`, and test declarations
- blocks, `for`, `return`, `break`, `continue`, `defer`, and `assert`
- type syntax for functions, tuples, arrays, slices, dynamic arrays, pointers,
  plexes, enums, and type application

Important intentional differences:

- CST has `CK_Group`; AST generally removes grouping after precedence is known.
- CST has `CK_FnExpr` and `CK_FnBlock`; AST uses `AK_FnDef`,
  `AK_FnStart`, and `AK_FnEnd` to support compiler declaration/body handling.
- CST has `CK_FfiBlock`; AST lowers FFI block entries to `AK_FfiDef` rows.
- CST has `CK_Test`; AST routes source tests through top-level parsing for
  compiler/test execution.
- CST has `CK_BreakOn`; AST represents this through ordinary break/on
  expression structure.
- CST node ranges are more important to formatter/LSP; AST nodes are more
  important to sema side-table indexing.

## Duplicated Grammar Families

These families exist in both parsers and are likely to drift when syntax grows:

- function signatures, default parameters, generics, and varargs
- FFI signatures and FFI blocks
- grouped `use` forms, public use, module paths, and module parts
- `on` branches, typed patterns, structural patterns, enum variants, guards,
  and leading comparison operators
- `for` headers: infinite, condition, C-style, `for item in collection`, and
  indexed `for index, item in collection`
- expression-valued blocks, `break`, `continue`, and `return`
- aggregate literals and plex updates
- type declarations and type-level applications

## Current Recovery Surface

The formatter already has a token/trivia fallback for chunks that lex but do not
parse as CST. Current coverage includes:

- partial nested blocks
- multiline bracket scopes
- comments inside partial literals
- `on` branches
- partial aggregate literals

The LSP keeps partial compiler products when the front end fails after lexing
and parsing. It also has feature-specific source fallbacks, especially in
completion and rename.

Missing or weak recovery areas:

- unterminated calls and argument lists
- unterminated grouped `use` lists
- partially typed function signatures
- partially typed `for` headers
- partially typed `on` branch patterns
- partially typed aggregate fields after a colon or comma
- module-path edits where one segment is missing or incomplete
- formatter/LSP behaviour when AST parses but CST does not
- formatter/LSP behaviour when CST parses but sema fails

## Drift Risks

### Syntax Feature Drift

Any new syntax feature must update:

- lexer token handling, if new tokens are introduced
- AST parser
- CST parser
- formatter rendering
- LSP syntax walks or fallbacks
- sema and HIR lowering, if the feature affects semantics

The highest-risk areas are `on`, `for`, grouped `use`, function signatures, and
aggregate/type syntax because each has several related forms.

### Range And Token Drift

The formatter and LSP often need token ranges, while sema needs AST node ids.
If helper logic for "start token", "end token", "binding symbol", or
"construct kind" is duplicated per feature, editor behaviour can diverge from
compiler parsing even when both parsers accept the same source.

### Recovery Drift

The AST parser is intentionally strict for compilation. The CST parser is
source-oriented but still mostly succeeds or fails as a whole. If CST recovery
does not become more tolerant, formatter and LSP will keep needing
feature-specific token fallbacks.

## Recommended Shared Helpers

The next implementation slices should centralise syntax classification helpers
that are currently reimplemented in formatter and LSP code:

- node start/end token for AST and CST nodes
- token range to source range
- whether a CST/AST node is a top-level binding-like construct
- whether a node is statement-like
- whether a token belongs to a declaration name, local binding, field name, or
  module path segment
- syntax context around a cursor offset: call, member access, module use,
  aggregate field, `on` branch, or block

These helpers should not require semantic success. They may take semantic facts
optionally when a feature wants a richer answer.

## Fixture Targets

Add regression fixtures before changing parser internals:

- formatter snapshot for an unterminated call with comments
- formatter snapshot for an unterminated grouped `use`
- formatter snapshot for partial aggregate fields and nested delimiters
- LSP completion after an incomplete call argument
- LSP document symbols after a partially typed function body
- LSP module completion after an incomplete module path
- LSP hover/definition surviving CST failure when AST and sema facts exist

## Open Design Question

The long-term syntax relationship is still open:

- derive compact AST from a tolerant CST
- make AST and CST views from one parser core
- keep separate parsers but move all shared classification/range logic into
  common syntax utilities

The conservative next step is the third option: shared utilities and fixtures
first. Deriving AST from CST is a larger rewrite and should wait until the
recovery and range contracts are clearer.
