# Internals

This document is a living set of notes about the internal structure of the
codebase. It is intended to preserve implementation context as the project
evolves.

## Project Shape

The repo is centered around a small compiler toolchain implemented in C under
`src/`, with tests under `tests/` and a custom Python build script under
`build/build.py`.

Key top-level areas:

- `src/`
  Main codebase.
- `tests/`
  Language and error test fixtures plus planning/docs.
- `build/build.py`
  Custom build orchestration. It discovers sources from `src/` and understands
  module dependencies via `//> use:` and `//> def:` directives.
- `nerd-src/`
  Sample/source material for the Nerd language.

## Entry Points

- [`src/core/main.c`](/home/matt/nerd/src/core/main.c)
  Generic process entry point calling `run(argc, argv)`.
- [`src/nerd.c`](/home/matt/nerd/src/nerd.c)
  CLI schema, CLI parsing, config extraction, and top-level dispatch for the
  `nerd` executable.

The CLI layer is data-driven through the JSON-based parser in
[`src/cli/cli.c`](/home/matt/nerd/src/cli/cli.c).

## Module Layout

`src/` is organized by subsystem rather than by strict library/application
split.

Important modules:

- `core`
  Shared cross-platform utilities: memory, arrays, strings, output, time,
  shell, file mapping, path handling, and directory iteration.
- `cli`
  Schema-driven command-line parser.
- `compiler`
  Front-end, IR, back-end, benchmarks, command helpers, and code generation.
- `table`
  Terminal table rendering used for dumps and reporting.
- `object`
  JSON/object helpers used heavily by the CLI parser.
- `testing`
  Test discovery, parsing, execution, and diff reporting.
- `lsp`
  Language server support.

## Build System Notes

The build is driven by [`build/build.py`](/home/matt/nerd/build/build.py).

Important properties:

- Top-level executables are inferred from `src/*.c`.
- Module dependencies are declared with `//> use: ...`.
- Preprocessor definitions are declared with `//> def: ...`.
- The build script expands those dependencies and compiles the required source
  set automatically.

This means new modules usually only need correct `//> use:` wiring; no manual
project file needs updating.

## CLI Architecture

The CLI schema for `nerd` is built in
[`src/nerd.c`](/home/matt/nerd/src/nerd.c).

Current commands:

- `build`
- `benchmark`
- `million`
- `test`
- `lsp`

The parser returns a JSON result with:

- `command.name`
- `command.flags`
- `command.params`
- `global_flags`
- `global_params`

`src/nerd.c` then converts that JSON into command-specific config structs.

## Compiler Pipeline

The compiler is split into explicit front-end and back-end stages.

### Front-end

Implemented in [`src/compiler/front.c`](/home/matt/nerd/src/compiler/front.c).

Pipeline:

1. lex
2. parse
3. IR generation

Public surface:

- `front_end(...)`
- `front_end_benchmark(...)`
- `front_end_results_done(...)`

The front-end returns a `FrontEndState` containing:

- `Lexer`
- `Ast`
- `Ir`

### Back-end

Implemented in [`src/compiler/back.c`](/home/matt/nerd/src/compiler/back.c).

Pipeline:

1. C generation
2. save generated C
3. compile generated C

The back-end is now configuration-driven through `NerdArtifactConfig` in
[`src/compiler/compiler.h`](/home/matt/nerd/src/compiler/compiler.h).

Current artifact controls:

- `output_stem`
- `emit_ir_file`
- `emit_c_file`
- `compile_binary`

This is used by both `build` and `test`.

## Rendering vs Dumping

IR and generated C now have two different roles:

- stable rendered text for tests and file emission
- terminal dump output for interactive inspection

Current rendering helpers:

- `ir_render(...)`
- `cgen_render(...)`

Related save/dump functions:

- `ir_save(...)`
- `ir_dump(...)`
- `cgen_save(...)`
- `cgen_dump(...)`

This distinction matters because tests compare stable strings, while human
inspection can remain formatted for terminal output.

## Command Helpers

Shared command helpers live in
[`src/compiler/cmd_common.c`](/home/matt/nerd/src/compiler/cmd_common.c) and
[`src/compiler/cmd_internal.h`](/home/matt/nerd/src/compiler/cmd_internal.h).

Important helper:

- `compiler_cmd_run_pipeline_once(...)`

This is the common path used by command implementations. The goal is to keep
command handlers thin and push shared pipeline behavior into this layer.

## Test Runner

The first language-test pass is implemented under `src/testing/`.

Important files:

- [`src/testing/testing.c`](/home/matt/nerd/src/testing/testing.c)
- [`src/testing/testing.h`](/home/matt/nerd/src/testing/testing.h)
- [`src/testing/diff.c`](/home/matt/nerd/src/testing/diff.c)
- [`src/testing/diff.h`](/home/matt/nerd/src/testing/diff.h)

Current behavior:

- scans `tests/language` recursively for `.t` files
- parses each `.t` file into one `LanguageTest`
- runs the compiler via `front_end()` and `back_end()`
- runs the generated executable and captures stdout/stderr/exit code
- compares:
  - return code
  - stdout
  - rendered IR
  - rendered C
- prints a colored line-based diff on mismatch
- removes generated artifacts on pass
- keeps generated artifacts on failure

The `test` command currently implements language tests only. Error tests are
planned but not wired yet.

## Test Fixture Format

Language tests in `tests/language/*.t` are split by the `¬` character into five
sections:

1. source
2. expected return value
3. expected stdout
4. expected IR
5. expected C

The compiler naturally stops at `¬`, so the first section can be lexed directly.

Empty expected IR/C sections are treated as incomplete fixtures: the runner
prints generated output so the fixture can be filled in later.

## Artifact Policy

Generated artifacts use a shared stem.

Examples:

- normal build default stem: `_output`
- test fixture stem: the `.t` file path without extension

For tests:

- `.ir`, `.c`, and executable artifacts are cleaned before a run
- passing tests remove their artifacts afterwards
- failing tests keep their artifacts for inspection

Generated test `.ir` and `.c` files are ignored in
[`.gitignore`](/home/matt/nerd/.gitignore).

## Core Utilities Added Recently

The `core` module now includes:

- path helpers in [`src/core/path.c`](/home/matt/nerd/src/core/path.c)
- directory iteration in [`src/core/dir.c`](/home/matt/nerd/src/core/dir.c)
- shell capture in [`src/core/shell.c`](/home/matt/nerd/src/core/shell.c)

These were added primarily to support the test framework and to avoid scattering
filesystem/process handling details across compiler code.

## Current Known Mismatch

[`tests/README.md`](/home/matt/nerd/tests/README.md) refers to `tests/error`,
but the actual directory is `tests/errors`.

This should be normalized when error tests are implemented.

## Maintenance Rule

When major internal structure changes, this file should be updated alongside the
code so architectural context remains local to the repository rather than living
only in chat history.
