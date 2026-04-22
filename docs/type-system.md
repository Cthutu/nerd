# Type System

This document describes the current implementation of type information in the
semantic layer for Milestone 5.

The main code is in
[src/compiler/sema/sema.h](/home/matt/nerd/src/compiler/sema/sema.h) and
[src/compiler/sema/sema.c](/home/matt/nerd/src/compiler/sema/sema.c).

## Design Goal

The current design keeps the AST compact and stores type facts in semantic side
tables instead of growing AST nodes.

That means:

- the parser recognises syntax
- semantic analysis decides what the syntax means
- type rows live in `Sema.types`
- AST nodes are linked to inferred or declared types through side tables

## Type Representation

Each semantic type is stored as a compact `SemaType` row:

- `kind`
- `param_count`
- `a`
- `b`

The current `SemaTypeKind` set includes:

- `void`
- `untyped integer`
- `string`
- `bool`
- integer primitives such as `i32`, `u16`, `isize`, and `usize`
- floating-point primitives `f32` and `f64`
- function types

Function types currently use the payload fields to encode parameter and return
type indices. The representation is intentionally table-oriented rather than
pointer-heavy.

## Canonicalisation

`sema_add_type(...)` interns type rows by value. If the same type row already
exists, the semantic pass reuses its index.

This keeps type equality cheap:

- equal rows share one canonical index
- side tables can compare type indices directly in many cases

## Declaration And Node Tables

`Sema` stores several related tables:

- `decls`
  Top-level declarations.
- `locals`
  Function-local declarations.
- `types`
  Canonical semantic types.
- `node_type_indices`
  Per-AST-node inferred or declared type.
- `node_is_type_expr`
  Marks AST subtrees that should be interpreted as type syntax during resolution.

This is the main mechanism that keeps type facts out of the AST itself.

## Built-In Types

Built-in primitive types are materialised through helper functions such as
`sema_builtin_type(...)`.

They are semantic facts, not parser keywords with hard-coded AST payloads. The
parser mostly sees symbols and type syntax; semantic analysis resolves them.

## Untyped Integers

Integer literals begin life as `untyped integer`. This lets the semantic pass
delay commitment until surrounding context is known.

When a concrete runtime type is required, `sema_materialise_type(...)` currently
maps `untyped integer` to `i32`.

## Type Aliases

Type aliases are normal top-level bindings whose right-hand side resolves to a
type:

- `Price :: u16`
- `MainFn :: fn () -> i32`

Alias classification is a semantic pass, not a parser special case. The parser
still records a normal binding shape and semantic analysis decides whether the
binding names a runtime value or a type alias.

Alias cycles are diagnosed during type classification before normal dependency
ordering. They now have a dedicated semantic error so alias cycles are kept
distinct from value dependency cycles.

## Variables And Storage

Variables are represented semantically rather than through wider AST nodes.
Current storage-eligible types are:

- concrete integer types
- `bool`
- `string`
- `f32`
- `f64`

Zero-initialised declarations such as `count: i32` and `name: string` are
checked in sema and then lowered using type-aware zero values.

## Casts

Explicit casts use postfix syntax:

- `value.cast(u8)`
- `128.cast(u8)`

Cast validity is semantic and table-driven. The parser only records a cast node
with a source expression and a target type expression.

The current milestone allows explicit casts across compatible primitive
numeric/bool types and rejects unsupported casts such as `string.cast(u8)` with
structured semantic diagnostics.

## Interpolated Strings

Interpolated strings are typed as `string`, but each `{expr}` segment is
checked independently in sema.

Today sema accepts interpolation of:

- `string`
- `bool`
- concrete integer types
- `f32`
- `f64`
- `untyped integer`, which materialises to `i32` for runtime conversion

Unsupported segment types, such as function values, produce a dedicated
semantic error.

The current runtime model also treats interpolated strings as temporary
statement-local values. Sema rejects uses that would let them escape, such as
returning or storing them.

## Type Names

`sema_type_name(...)` renders source-facing type text. It is used by:

- semantic error messages
- LSP hover text
- other tooling that needs human-readable type names

That function should stay aligned with the actual semantic representation.

## Current Scope

Today the semantic type system is focused on:

- built-in primitive types
- function signatures
- alias resolution
- variable storage eligibility
- untyped integer materialisation
- exact-match type checks for the implemented arithmetic surface
- explicit cast validation

One implementation caveat remains: the IR is not yet fully self-describing and
some backend stages still consult semantic tables. A future cleanup should move
that remaining type knowledge into IR so VM-style execution becomes practical.
