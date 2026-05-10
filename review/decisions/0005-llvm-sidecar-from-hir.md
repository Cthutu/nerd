# 0005: LLVM Sidecar From HIR

Status: accepted
Date: 2026-05-10

## Context

The review direction is to make HIR the target-agnostic program form and lower
it directly to LLVM IR, eventually replacing the current custom IR and C
generation path.

Jumping straight to executable LLVM would mix several decisions at once:
runtime/prelude lowering, concrete aggregate layout, expression lowering,
module linking, and CLI/backend orchestration.

## Decision

Start with a non-invasive LLVM sidecar artifact generated from HIR:

```text
nerd build --llvm source.n
```

The first emitter writes textual `.ll` from root-module HIR and covers:

- Nerd-visible function names with the `$` prefix, such as `@$main`.
- Generated names for anonymous/unbound functions, such as `@fn.N`.
- Imported function declarations from HIR import bindings.
- Export metadata comments from HIR export records.

The existing executable path still goes through IR and C generation.

## Consequences

This gives us a stable place to grow LLVM lowering under tests without
destabilising normal builds. It also validates that HIR now contains enough
binding/module information to drive target symbol decisions.

The current LLVM function bodies are scaffolds and return zero/default values.
They are not intended to preserve program semantics yet.

## Follow-up

1. Lower simple expressions and returns into real LLVM instructions.
2. Add LLVM type/layout lowering for pointers, arrays, slices, strings, tuples,
   plexes, unions, and enums.
3. Replace export comments with concrete LLVM linkage/alias decisions.
4. Compile the generated `.ll` through clang or llc once runtime/prelude
   dependencies are represented.
