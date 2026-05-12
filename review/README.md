# Architecture Review Workspace

This folder holds detailed architecture review material.

- `decisions/`: accepted, rejected, or superseded decision records.
- `audits/`: evidence gathered before decisions.
  - `audits/lsp-boundaries.md`: current LSP product/readiness boundaries.
  - `audits/hir-backend-readiness.md`: current HIR surface and LLVM backend
    blockers.
  - `audits/hir-manual-coverage.md`: manual-to-HIR coverage matrix and
    remaining accepted `<unsupported>` HIR cases.
  - `audits/formatter-token-trivia-prototype.md`: historical audit trail for
    the formatter token/trivia prototype.
  - `audits/formatter-layout-classification.md`: current formatter layout
    policy groups and recommended MS3 extraction order.
  - `audits/llvm-abi-layout-assumptions.md`: current LLVM target-layout and
    runtime ABI assumptions for MS5.
- `measurements/`: profiling, benchmark, and allocation results.

Keep `../ARCHITECTURE_REVIEW.md` as the top-level synthesis.

The initial architecture review is closed. Keep useful audits and measurements
in place as evidence, but add new architectural agreements as numbered decision
records instead of creating a fresh prototype workspace.
