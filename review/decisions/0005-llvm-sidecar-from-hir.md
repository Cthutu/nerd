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

An opt-in execution backend is available separately:

```text
nerd run --llvm-backend source.n
```

This path writes generated LLVM IR and compiles it with build-generated LLVM
runtime bridge artifacts. The LLVM backend is now the default executable path;
the C backend remains available as an escape hatch.

The first emitter writes textual `.ll` from root-module HIR and covers:

- Generated names for all function entities, such as `@fn.N`.
- Nerd-visible bindings with the `$` prefix as aliases, such as `@$main`.
- Imported function declarations from HIR import bindings.
- FFI extern records, including the source library name needed for backend link
  flags.
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

The legacy IR/C executable path is no longer exposed through the command-line
build/run interface. The `--llvm-backend` flag remains accepted as a
backwards-compatible no-op while LLVM is the only public executable backend.

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

Array, field, and pointer indexing now use addressable lowering when the value
is used as an lvalue or when a later operation needs a stable element address.
For non-global constant bindings, the backend may materialise the aggregate into
a temporary stack slot before issuing `getelementptr` and `load`. That keeps
constant aggregate indexing, nested field access, and element address-taking on
one lowering path, at the cost of less compact LLVM text until a later
optimisation pass recognises pure value-only indexes.

As of this slice, the checked test suite passes with LLVM as the default
executable backend. The installed compiler runs the LLVM backend with
`nerd run source.n` or `nerd build source.n`, and it can still produce textual
LLVM IR with `nerd build --llvm source.n`.

The compiler build now supports `//> run: ...` directives on source files.
`src/nerd.c` uses this to compile `data/prelude.c` into
`_obj/llvm/prelude.ll` before compiling the compiler itself. That generated
runtime `.ll` file is embedded into the compiler with `#embed`. During a Nerd
program build, the backend writes a temporary prelude copy beside the generated
program LLVM IR, emits the tiny LLVM `main` wrapper directly, and invokes clang
on all LLVM inputs together. The backend removes those temporary runtime files
after a successful link.

HIR now owns enough FFI metadata for the LLVM backend to derive external link
flags without consulting the old IR tables. Normal LLVM builds request HIR but
always skip legacy IR generation. This removes the old IR from the default
compiler critical path and from the public artifact interface.

## Follow-up

1. Replace export comments with concrete LLVM linkage/alias decisions.
2. Consider changing the embedded runtime bridge artifact from textual LLVM IR
   to bitcode once we have timing data.
3. Remove the remaining legacy IR/C implementation files.
