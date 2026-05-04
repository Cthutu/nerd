# Internals

This document is the high-level map of the current codebase. It points at the
main subsystems and tells you where to read next.

For deeper implementation notes, use these companion documents:

- [../compiler-pipeline.md](/home/matt/nerd/docs/compiler-pipeline.md)
- [../error-system.md](/home/matt/nerd/docs/error-system.md)
- [../type-system.md](/home/matt/nerd/docs/type-system.md)
- [../lsp.md](/home/matt/nerd/docs/lsp.md)
- [../testing.md](/home/matt/nerd/docs/testing.md)

## Overall Shape

The repository contains:

- `src/core`
  Low-level utilities shared by almost everything else.
- `src/object` and `src/table`
  Shared data helpers built on `core`.
- `src/cli`
  Schema-driven command-line parsing.
- `src/compiler`
  The compiler front end, IR, C generation, error system, and commands.
- `src/lsp`
  The language server built on the compiler front end and CST.
- `build/test.py`
  The repository regression test runner.
- `tests`
  Language, error, formatter, and LSP test inputs.

The main executable entry point is [src/nerd.c](/home/matt/nerd/src/nerd.c).

## Reading Order

For most compiler work:

1. [src/nerd.c](/home/matt/nerd/src/nerd.c)
2. [src/compiler/compiler.h](/home/matt/nerd/src/compiler/compiler.h)
3. [src/compiler/build/front/front.c](/home/matt/nerd/src/compiler/build/front/front.c)
4. [src/compiler/build/back/back.c](/home/matt/nerd/src/compiler/build/back/back.c)
5. the subsystem you are changing

For editor tooling work:

1. [src/lsp/lsp.c](/home/matt/nerd/src/lsp/lsp.c)
2. [src/lsp/document.c](/home/matt/nerd/src/lsp/document.c)
3. [src/lsp/hover.c](/home/matt/nerd/src/lsp/hover.c)
4. [src/compiler/cst/cst.c](/home/matt/nerd/src/compiler/cst/cst.c)

For tests:

1. [tests/README.md](/home/matt/nerd/tests/README.md)
2. [docs/testing.md](/home/matt/nerd/docs/testing.md)
3. [build/test.py](/home/matt/nerd/build/test.py)

## Execution Flow

The `nerd` executable is mostly orchestration:

1. parse CLI input
2. build a command-specific config struct
3. run compiler, LSP, or test commands

The compiler front end currently runs:

1. lexing
2. AST parsing
3. semantic analysis
4. IR generation

The back end currently runs:

1. C generation
2. optional save of generated C
3. optional native compilation of generated C

## Important Architectural Rules

- Keep the AST syntax-only.
- Prefer semantic side tables over larger AST nodes.
- Keep compiler stages explicit rather than smearing semantic logic into the parser.
- Keep renderers stable for tests and keep dumpers human-oriented.
- Treat `ErrorInfo` as the source of truth for diagnostics rather than terminal text.

## Key Data Products

- `Lexer`
  Tokens, symbol handles, and source mapping.
- `Ast`
  Compact syntax tree nodes.
- `Sema`
  Semantic declarations, type rows, dependency edges, and AST-indexed side tables.
- `Ir`
  Lowered compiler IR used by C generation.
- `Cgen`
  Generated C text and related state.
- `Cst`
  Concrete syntax tree used mainly for formatter and LSP tooling.

## File Families

- `src/compiler/lexer`
  Tokenisation and source-position helpers.
- `src/compiler/ast`
  Syntax parsing, AST utilities, and AST dumping.
- `src/compiler/sema`
  Name resolution, dependency ordering, constant folding, and type analysis.
- `src/compiler/ir`
  IR data model, lowering, and rendering.
- `src/compiler/cgen`
  C emission.
- `src/compiler/error`
  Structured diagnostics and renderers.
- `src/compiler/format`
  Formatting rules and formatter output.
- `src/lsp`
  LSP message handling and editor-facing features.

## Related Documents

- [FORMAT.md](/home/matt/nerd/docs/overviews/FORMAT.md) for formatter rules
- [../manual/appendix-a-syntax-reference.md](/home/matt/nerd/docs/manual/appendix-a-syntax-reference.md)
  for the source-level syntax reference
- [build-directives.md](/home/matt/nerd/docs/overviews/build-directives.md) for build-system metadata
