# Code Review Checklist

Track subsystem review progress here. Check a file only after it has been read
for correctness, architecture, maintainability, diagnostics, tests, and obvious
dead code.

## Root Program

- [x] `src/nerd.c`

## CLI

- [x] `src/cli/cli.c`
- [x] `src/cli/cli.h`

## Compiler

- [x] `src/compiler/compiler.h`
- [x] `src/compiler/internal.h`
- [x] `src/compiler/cmd_build.c`
- [x] `src/compiler/cmd_common.c`
- [x] `src/compiler/cmd_format.c`
- [x] `src/compiler/cmd_internal.h`
- [x] `src/compiler/cmd_run.c`
- [x] `src/compiler/cmd_test.c`

## Compiler AST

- [x] `src/compiler/ast/ast.h`
- [x] `src/compiler/ast/dump.c`
- [x] `src/compiler/ast/expr.c`
- [x] `src/compiler/ast/parse.c`
- [x] `src/compiler/ast/parse_internal.h`
- [x] `src/compiler/ast/query.c`
- [x] `src/compiler/ast/utils.c`

## Compiler Build

- [x] `src/compiler/build/build.h`
- [x] `src/compiler/build/back/back.c`
- [x] `src/compiler/build/back/back.h`
- [x] `src/compiler/build/front/front.c`
- [x] `src/compiler/build/front/front.h`

## Compiler C Generation

- [x] `src/compiler/cgen/cgen.c`
- [x] `src/compiler/cgen/cgen.h`
- [x] `src/compiler/cgen/dump.c`
- [x] `src/compiler/cgen/save.c`

## Compiler CST

- [x] `src/compiler/cst/cst.c`
- [x] `src/compiler/cst/cst.h`

## Compiler Errors

- [x] `src/compiler/error/ast_errors.c`
- [x] `src/compiler/error/error.c`
- [x] `src/compiler/error/error.h`
- [x] `src/compiler/error/lex_errors.c`
- [x] `src/compiler/error/render.c`
- [x] `src/compiler/error/sema_errors.c`

## Compiler Formatting

- [x] `src/compiler/format/format.c`
- [x] `src/compiler/format/format.h`

## Compiler IR

- [x] `src/compiler/ir/dump.c`
- [x] `src/compiler/ir/gen.c`
- [x] `src/compiler/ir/ir.h`

## Compiler Lexer

- [x] `src/compiler/lexer/dump.c`
- [x] `src/compiler/lexer/interner.c`
- [x] `src/compiler/lexer/lexer.c`
- [x] `src/compiler/lexer/lexer.h`

## Compiler Semantic Analysis

- [x] `src/compiler/sema/sema.c`
- [x] `src/compiler/sema/sema.h`

## Core

- [x] `src/core/arena.c`
- [x] `src/core/array.c`
- [x] `src/core/core.h`
- [x] `src/core/dir.c`
- [x] `src/core/filemap.c`
- [x] `src/core/hash.c`
- [x] `src/core/main.c`
- [x] `src/core/map.c`
- [x] `src/core/memory.c`
- [x] `src/core/mutex.c`
- [x] `src/core/output.c`
- [x] `src/core/path.c`
- [x] `src/core/random.c`
- [x] `src/core/shell.c`
- [x] `src/core/string.c`
- [x] `src/core/time.c`

## ELF

- [x] `src/elf/elf.h`
- [x] `src/elf/load.c`

## Interning

- [x] `src/intern/intern.c`
- [x] `src/intern/intern.h`

## LSP

- [x] `src/lsp/document.c`
- [x] `src/lsp/hover.c`
- [x] `src/lsp/lsp.c`
- [x] `src/lsp/lsp.h`
- [x] `src/lsp/semantic_tokens.c`
- [x] `src/lsp/utils.c`

## Object

- [x] `src/object/object.c`
- [x] `src/object/object.h`

## Table

- [x] `src/table/info.c`
- [x] `src/table/table.c`
- [x] `src/table/table.h`

## Testing

- [x] `src/testing/diff.c`
- [x] `src/testing/diff.h`
- [x] `src/testing/testing.c`
- [x] `src/testing/testing.h`

## Timing

- [x] `src/timing/timing.c`
- [x] `src/timing/timing.h`

## Unicode

- [x] `src/unicode/unicode.c`
- [x] `src/unicode/unicode.h`
