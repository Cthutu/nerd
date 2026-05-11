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

LLVM is also the executable backend used by normal builds and runs:

```text
nerd run source.n
```

This path writes generated LLVM IR and compiles it with build-generated LLVM
runtime bridge artifacts. The LLVM backend is now the only executable backend.

The first emitter writes textual `.ll` from root-module HIR and covers:

- Generated names for all function entities, such as `@fn.N`.
- Nerd-visible bindings with the `$` prefix as aliases, such as `@$main`.
- Imported function declarations from HIR import bindings.
- FFI extern records, including the source library name needed for backend link
  flags.
- LLVM linkage decisions from HIR export records.

Function lowering must preserve the HIR entity/binding split. A function
entity is always emitted with a generated name. A Nerd binding never changes
the entity's symbol; it emits a target-level alias or linkage record that
points at the generated entity:

```llvm
define i32 @fn.0() {
  ret i32 42
}

@$main = alias i32 (), ptr @fn.0
```

Calls produced inside lowered function bodies should target generated entity
names, not Nerd binding aliases, when the callee is a known local HIR function.

The legacy IR/C executable path is no longer exposed through the command-line
build/run interface. The earlier `--llvm-backend` compatibility flag has been
removed because LLVM is no longer an alternate backend.

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
program build, the backend renders module LLVM IR in memory, writes inspection
sidecars only when `--llvm` is requested, concatenates the embedded prelude,
generated modules, generated init wrapper, and tiny LLVM `main` wrapper into one
temporary combined LLVM file, and invokes clang on that single input. The
backend removes the combined temporary file after a successful link.

HIR now owns enough FFI metadata for the LLVM backend to derive external link
flags without consulting the old IR tables. The legacy IR and C generator have
been removed from the build graph, and command tests now exercise the default
LLVM executable path directly.

HIR export records now drive LLVM linkage instead of comments. Non-exported
Nerd-visible function aliases and globals are emitted with internal linkage;
exported aliases/globals keep external linkage. Exported FFI bindings whose
Nerd binding differs from the foreign C symbol emit a small external Nerd
wrapper that calls the C declaration.

Generated function entities default to internal LLVM linkage. The generated
name is an implementation detail of the HIR entity, while the Nerd binding is
represented by an alias or wrapper. A generated function body remains externally
visible only when another module must reference that generated name directly,
such as same-symbol module conflict resolution or imported generic
instantiations.

## Follow-up

1. Consider changing the embedded runtime bridge artifact from textual LLVM IR
   to bitcode once we have timing data.
2. Continue replacing historical IR/C wording in design notes when those notes
   are edited for current work.
