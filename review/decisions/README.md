# Decision Records

Use numbered decision records for settled architecture choices.

Suggested filename:

```text
0001-short-title.md
```

Suggested sections:

```text
# 0001: Title

Status: proposed | accepted | rejected | superseded
Date: YYYY-MM-DD

## Context
## Decision
## Consequences
## Follow-up
```

## Records

- [0001: HIR And Backend Boundary](0001-hir-and-backend-boundary.md)
  - Status: accepted.
  - Closeout: implemented. HIR is the middle layer and LLVM is the only
    executable backend.
- [0002: For-In Pointer Items](0002-for-in-pointer-items.md)
  - Status: accepted.
  - Closeout: implemented. `for item in collection` binds `item` as a pointer.
- [0003: HIR Entities And Bindings](0003-hir-entities-and-bindings.md)
  - Status: accepted.
  - Closeout: implemented for top-level HIR entities and bindings.
- [0004: HIR Module Bindings](0004-hir-module-bindings.md)
  - Status: accepted.
  - Closeout: implemented. HIR records module imports, aliases, and exports.
- [0005: LLVM Sidecar From HIR](0005-llvm-sidecar-from-hir.md)
  - Status: accepted.
  - Closeout: implemented. LLVM is the default executable backend; `--llvm`
    keeps module sidecars for inspection.
- [0006: Memory Ownership Strategy](0006-memory-ownership-strategy.md)
  - Status: accepted.
  - Closeout: active policy for future performance work.
- [0007: Semantic Fact Readiness](0007-semantic-fact-readiness.md)
  - Status: accepted.
  - Closeout: active policy for LSP/front-end boundary work.
- [0008: Syntax Product Relationship](0008-syntax-product-relationship.md)
  - Status: accepted.
  - Closeout: active policy for AST/CST boundary work.
- [0009: Backend Toolchain Contract](0009-backend-toolchain-contract.md)
  - Status: accepted.
  - Closeout: keep textual LLVM plus clang until measurements justify another
    executable toolchain.

## Closeout Notes

No proposed decision records remain open at the end of the architecture review
closeout. Future architectural changes should add new numbered records rather
than editing these closeout statements into a new decision.
