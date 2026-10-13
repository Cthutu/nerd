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

- [0011: Direct LLVM Toolchain](0011-direct-llvm-toolchain.md)
  - Status: accepted; supersedes 0009's toolchain choice.
  - Direct LLVM tools and host linking configuration; `nerd doctor` validates
    dependencies. Earlier closeout statements below describe the original review.
- [0001: HIR And Backend Boundary](0001-hir-and-backend-boundary.md)
  - Status: accepted.
  - Closeout: implemented. HIR is the middle layer; LLVM is the native binary
    backend and HIR-to-C is available for external compilation.
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
  - Status: superseded by 0011. The installed compiler uses direct LLVM tools;
    its original Clang invocation contract no longer applies.
- [0010: Target Layout Support](0010-target-layout-support.md)
  - Status: accepted.
  - Closeout: the 64-bit host layout contract remains; 0011 supersedes the
    Clang invocation. Cross-target and aggregate FFI support require explicit
    layout work.

## Closeout Notes

No proposed decision records remain open at the end of the architecture review
closeout. Future architectural changes should add new numbered records rather
than editing these closeout statements into a new decision.
