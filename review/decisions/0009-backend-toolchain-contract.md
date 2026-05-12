# 0009: Backend Toolchain Contract

Status: accepted
Date: 2026-05-12

## Context

Nerd now lowers HIR to textual LLVM IR and invokes clang with one combined
`.link.ll` input plus the embedded runtime object. Earlier review discussion
raised whether direct LLVM tools such as `llvm-as`, `opt`, and `llc` would avoid
clang startup cost or simplify dependency ordering further.

Measurements in `review/measurements/backend-phase-timing.md` show that small
builds spend microseconds in LLVM text rendering/combining/runtime-object
writing, while the external executable tool step is roughly 27-28 ms on the
current machine.

Measurements in `review/measurements/llvm-cli-tool-comparison.md` show no
meaningful object-generation advantage for `llvm-as` plus `llc` on the current
small fixtures, and that path adds another process before final system linking.

## Decision

Keep the current executable backend contract:

- render textual LLVM IR from HIR
- build one combined `.link.ll` input
- write the embedded runtime object beside it
- invoke clang once for native executable generation

Do not switch to `llvm-as`, `opt`, `llc`, bitcode, or a direct object pipeline
without a larger measurement showing a clear win for complete executable builds.

## Consequences

The compiler keeps a smaller install surface: a working clang is enough for the
LLVM backend. Textual LLVM remains easy to inspect when lowering fails, and the
single combined input avoids the old C/backend dependency-ordering problem.

Backend failure diagnostics must preserve failed `.link.ll` inputs and report
the clang command and generated paths, because textual LLVM inspection is part
of the chosen workflow.

## Follow-up

Revisit this decision with larger programs and release-mode measurements if:

- clang startup dominates interactive compile times enough to matter
- generated LLVM text becomes large enough that parsing cost is visible
- cross-target support requires tighter control over LLVM target triples,
  data layouts, or object emission
- a future in-process LLVM binding becomes preferable to a CLI toolchain

