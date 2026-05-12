# 0008: Syntax Product Relationship

Status: accepted
Date: 2026-05-12

## Context

Nerd currently has two syntax products:

- `Ast`, the compact compiler tree used by semantic analysis and HIR lowering
- `Cst`, the concrete source-oriented tree used by formatter and editor tooling

The products intentionally serve different users, but they duplicate grammar
knowledge. That duplication creates drift risk when syntax changes, and it can
make formatter/LSP recovery weaker than the compiler path or vice versa.

## Decision

Keep `Ast` and `Cst` as separate products for now, but treat the split as an
interface contract rather than two unrelated parsers.

The near-term rule is:

- AST remains compact, syntax-only, and compiler-facing.
- CST remains source-preserving, trivia-aware, and tooling-facing.
- Shared syntax classification, token ranges, and construct predicates must
  move into common helpers instead of being reimplemented in formatter and LSP
  code.
- LSP and formatter paths should start from source/token/CST products where
  possible and treat semantic facts as optional enrichments.

Do not derive AST from CST yet. A single parser core or AST-from-CST design
would be a larger parser rewrite, and the current evidence says the immediate
pain is duplicated helper logic and weak tolerant paths rather than the mere
existence of two products.

## Consequences

Syntax changes still need updates in both parsers, so review discipline remains
important. The accepted mitigation is to centralise shared queries and add
focused recovery fixtures for each high-risk syntax family.

Formatter and LSP features should not require successful semantic analysis for
basic source-shaped answers. Document symbols are the first path moved in that
direction: they now start from CST bindings and add semantic detail when
declaration facts are available.

This decision keeps the compiler AST small and avoids forcing source trivia
into the compilation data model. It also avoids taking on a broad parser rewrite
before the tooling contracts are clearer.

## Follow-up

Revisit this decision if any of these become true:

- syntax changes repeatedly land in one parser but not the other
- formatter/LSP recovery requires large token-level fallbacks because CST cannot
  represent partial source usefully
- AST and CST range semantics diverge enough to make shared helpers fragile
- a parser-core extraction becomes cheaper than maintaining duplicated grammar
  families

