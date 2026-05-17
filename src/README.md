# Source Map

This directory contains the compiler and language-server implementation.

Read the project and documentation indexes first:

- [../README.md](../README.md)
- [../docs/README.md](../docs/README.md)
- [../docs/compiler-pipeline.md](../docs/compiler-pipeline.md)

## Compiler Entry Points

- `compiler/build/front/front.c`
  Front-end orchestration: lex, parse, semantic analysis, and HIR generation.
- `compiler/sema/sema.c`
  Semantic analysis, names, scopes, types, control-flow validation, constants,
  and side tables.
- `compiler/hir/hir.h`, `compiler/hir/gen.c`, `compiler/hir/render.c`
  HIR data model, lowering from semantic products, and stable textual HIR.
- `compiler/llvm/llvm.c`
  HIR to LLVM lowering. Nerd-visible bindings keep `$` names as LLVM aliases;
  generated implementation names are compiler internals.
- `compiler/build/back/back.c` and `compiler/build/back/llvm_text.c`
  Backend orchestration, runtime glue, combined LLVM input generation, and
  clang invocation.
- `compiler/format/format.c`
  Formatter implementation.

## Language Server Entry Points

- `lsp/*.c`
  LSP document state, partial front-end analysis, completion, hover, rename,
  and jump-to-definition behaviour.

For language-surface changes, read [../docs/manual/README.md](../docs/manual/README.md)
and [../docs/spec/README.md](../docs/spec/README.md) before editing source.
