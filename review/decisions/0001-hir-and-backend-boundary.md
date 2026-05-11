# 0001: HIR And Backend Boundary

Status: proposed
Date: 2026-05-09

Update: this decision has been carried out by the later LLVM backend work.
HIR is now the compiler middle layer and LLVM is the only executable backend;
the legacy IR/C backend has been removed.

## Context

At the time of this decision, the build pipeline was:

```text
lexer -> AST parser -> sema -> IR -> C -> clang
```

The current IR is useful, but it is not mainly an optimizer IR. It is a
linearized C-generation input with side tables for globals, externs, functions,
locals, call arguments, aggregate construction, strings, dynamic arrays, copied
semantic types, and module merge/remapping.

The architecture review identified two related goals:

- simplify the compiler by introducing a semantically checked lowered program
  product between sema and backend emission
- avoid making C generation the permanent center of the architecture, because
  LLVM IR is a plausible backend target

The LLVM runtime bridge experiment has shown that the existing C prelude and
epilogue can be compiled to LLVM IR and linked with a hand-written module that
defines `@"$main"`.

## Decision

Introduce HIR as the durable checked product after semantic analysis.

The intended long-term build pipeline is:

```text
lexer -> tolerant syntax -> sema/name/type facts -> HIR -> backend
```

The executable backend is now LLVM IR generation from HIR.

HIR must be typed and name-resolved. It should not preserve parser trivia, and
it should not be shaped around C syntax. It should own:

- function, global, extern, and module-init records
- locals, parameters, and compiler temporaries
- typed expression and statement nodes
- explicit calls, casts, field/index operations, slices, aggregates, string
  interpolation, and dynamic-array operations
- enough structured control flow to represent blocks, loops, `on`, `defer`,
  returns, branches, and expression-valued control constructs
- stable references to semantic declarations, locals, types, symbols, and source
  spans needed for diagnostics and debug information

HIR must also have a stable textual representation for tests and review. This
text is a derived rendering of the HIR program, not a constraint on the in-memory
layout. The renderer should prefer readable HIR concepts such as functions,
parameters, blocks, typed values, and control-flow constructs over compiler
implementation details such as AST node indices or sema table indices. Source
and compiler provenance may be available as optional metadata/debug output, but
the default snapshot form should describe the HIR language itself.

LLVM backend rules:

- Nerd-visible bindings keep the `$` prefix. A Nerd `main` binding is emitted
  as `@"$main"`.
- Generated opaque names such as `@fn.N`, `@global.N`, and `%type.N` are
  reserved for compiler-created internals.
- The first LLVM backend may compile `data/prelude.c` to LLVM IR or bitcode
  with clang, generate the tiny epilogue wrapper directly, then link those
  products with generated Nerd LLVM IR.

## Consequences

This keeps semantic dependency analysis separate from backend ordering. The
language still needs dependency and cycle checks for inference, constants,
types, imports, and diagnostics.

This reduced C-specific pressure in the middle of the compiler. C
declaration/prototype ordering, C-safe type spelling, and generated C symbol
constraints were removed from the core representation constraints.

HIR and the LLVM backend have replaced the legacy backend services that were
called out during the migration:

- current IR dumps used for debugging
- module merge/remapping
- copied semantic type rows used by the old C generation path
- global/runtime initialization ordering
- explicit temporary and local ownership
- string/dynamic-array lowering records

LLVM support will still require deliberate ABI and runtime work. The C runtime
bridge avoids rewriting helper functions initially, but does not remove the
need to manage target triples, data layouts, linkage, platform C runtime
linking, varargs, debug metadata, and aggregate ABI choices.

## Follow-up

1. Define the first HIR data model in `src/compiler/hir/`.
2. Add HIR dumping before adding HIR-backed code generation.
3. Lower a very small subset first: function records, blocks, integer
   literals, calls, returns, and `main`.
4. Completed: the existing IR/C backend has been removed after the HIR/LLVM
   path became the default executable path.
5. Completed: the LLVM backend path uses the proven runtime bridge.
6. Completed: module merge and whole-program backend inputs now flow through
   HIR/backend context instead of the legacy IR.
