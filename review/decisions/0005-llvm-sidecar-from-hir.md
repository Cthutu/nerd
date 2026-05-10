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

- Generated names for all function entities, such as `@fn.N`.
- Nerd-visible bindings with the `$` prefix as aliases, such as `@$main`.
- Imported function declarations from HIR import bindings.
- Export metadata comments from HIR export records.

Function lowering must preserve the HIR entity/binding split. A function
entity is always emitted with a generated name. A Nerd binding never changes
the entity's symbol; it emits a target-level alias, export, or metadata record
that points at the generated entity:

```llvm
define i32 @fn.0() {
  ret i32 42
}

@$main = alias i32 (), ptr @fn.0
```

Calls produced inside lowered function bodies should target generated entity
names, not Nerd binding aliases, when the callee is a known local HIR function.

The existing executable path still goes through IR and C generation.

## Consequences

This gives us a stable place to grow LLVM lowering under tests without
destabilising normal builds. It also validates that HIR now contains enough
binding/module information to drive target symbol decisions.

The current LLVM lowering covers the core scalar and structured slices needed
for early backend replacement work: generated function aliases, imports,
locals, assignments, pointer/index/deref operations, aggregate values, slices,
structured control flow, `for` forms, `on` forms with value/plex/enum patterns,
and enum construction, including contextual enum constructors nested inside
call arguments.

## Follow-up

1. Replace export comments with concrete LLVM linkage/alias decisions.
2. Compile the generated `.ll` through clang or llc once runtime/prelude
   dependencies are represented.
