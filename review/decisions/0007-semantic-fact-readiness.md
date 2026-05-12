# 0007: Semantic Fact Readiness

Status: accepted
Date: 2026-05-12

## Context

`Sema` is still the compiler's main semantic product, but it contains several
different kinds of facts:

- declaration facts
- binding/reference facts
- type facts
- dependency and ordering facts
- constant and definite-assignment facts
- lowering-only facts

Batch compilation needs the checked whole, but editor tooling often needs only
the first two or three products. Treating "sema succeeded" as the only semantic
boundary made the LSP either too fragile or too dependent on broad fallback
paths.

## Decision

Semantic readiness is split into explicit sub-products:

- declarations
- bindings
- type facts

The first implementation keeps these backed by existing `Sema` tables. The
important boundary is the API contract:

- declaration views expose top-level declarations, imports, exports, and source
  declaration spans
- binding views expose source bindings, references, lexical scopes, locals, and
  source navigation facts
- type-fact views expose best-effort or checked type rows, method facts, and
  type-derived completion/hover/signature information

`FrontEndReadiness` now records semantic sub-product states. `LspDocument`
derives `decls_ready`, `bindings_ready`, `type_facts_partial`, and
`type_facts_complete` from those states. The older `sema_partial` and
`sema_complete` flags remain as compatibility while call sites finish migrating.

## Consequences

LSP feature handlers can now request the weakest product they need:

- semantic tokens and document symbols use declaration facts
- hover and definition use binding facts for request context
- completion uses binding facts for symbols and type facts for receiver members
- signature help and code actions use type facts

This does not yet split `sema.c` internally. It creates the contract needed to
do so safely later. Future sema work should preserve useful declaration and
binding facts even when type checking fails.

## Follow-Up

1. Continue replacing direct `Sema` table reads in LSP feature code with
   declaration, binding, and type-fact accessors.
2. Split declaration collection and binding/reference indexing inside
   `sema.c` when there is enough test coverage around failed type checking.
3. Remove the compatibility `sema_partial` flag once all editor features use
   the narrower readiness products.
