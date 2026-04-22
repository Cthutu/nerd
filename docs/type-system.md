# Type System

This document describes the current implementation of type information in the
semantic layer.

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
- integer and string typing
- exact-match type checks for the implemented arithmetic surface

The type system is still under active expansion, so future features such as
aliases, casts, and richer inference should continue to follow the same
side-table model rather than expanding AST node size.
