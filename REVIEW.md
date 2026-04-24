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

- [ ] `src/compiler/cst/cst.c`
- [ ] `src/compiler/cst/cst.h`

## Compiler Errors

- [ ] `src/compiler/error/ast_errors.c`
- [ ] `src/compiler/error/error.c`
- [ ] `src/compiler/error/error.h`
- [ ] `src/compiler/error/lex_errors.c`
- [ ] `src/compiler/error/render.c`
- [ ] `src/compiler/error/sema_errors.c`

## Compiler Formatting

- [ ] `src/compiler/format/format.c`
- [ ] `src/compiler/format/format.h`

## Compiler IR

- [ ] `src/compiler/ir/dump.c`
- [ ] `src/compiler/ir/gen.c`
- [ ] `src/compiler/ir/ir.h`

## Compiler Lexer

- [ ] `src/compiler/lexer/dump.c`
- [ ] `src/compiler/lexer/interner.c`
- [ ] `src/compiler/lexer/lexer.c`
- [ ] `src/compiler/lexer/lexer.h`

## Compiler Semantic Analysis

- [ ] `src/compiler/sema/sema.c`
- [ ] `src/compiler/sema/sema.h`

## Core

- [ ] `src/core/arena.c`
- [ ] `src/core/array.c`
- [ ] `src/core/core.h`
- [ ] `src/core/dir.c`
- [ ] `src/core/filemap.c`
- [ ] `src/core/hash.c`
- [ ] `src/core/main.c`
- [ ] `src/core/map.c`
- [ ] `src/core/memory.c`
- [ ] `src/core/mutex.c`
- [ ] `src/core/output.c`
- [ ] `src/core/path.c`
- [ ] `src/core/random.c`
- [ ] `src/core/shell.c`
- [ ] `src/core/string.c`
- [ ] `src/core/time.c`

## ELF

- [ ] `src/elf/elf.h`
- [ ] `src/elf/load.c`

## Interning

- [ ] `src/intern/intern.c`
- [ ] `src/intern/intern.h`

## LSP

- [ ] `src/lsp/document.c`
- [ ] `src/lsp/hover.c`
- [ ] `src/lsp/lsp.c`
- [ ] `src/lsp/lsp.h`
- [ ] `src/lsp/semantic_tokens.c`
- [ ] `src/lsp/utils.c`

## Object

- [ ] `src/object/object.c`
- [ ] `src/object/object.h`

## Table

- [ ] `src/table/info.c`
- [ ] `src/table/table.c`
- [ ] `src/table/table.h`

## Testing

- [ ] `src/testing/diff.c`
- [ ] `src/testing/diff.h`
- [ ] `src/testing/testing.c`
- [ ] `src/testing/testing.h`

## Timing

- [ ] `src/timing/timing.c`
- [ ] `src/timing/timing.h`

## Unicode

- [ ] `src/unicode/unicode.c`
- [ ] `src/unicode/unicode.h`
