# Internals

## Purpose

This document is a high-level guide to the current architecture of the codebase.
It is intended to help a new reader understand how the project is organized,
which subsystems exist, how they relate to one another, and where to start
reading depending on the task.

It is not meant to duplicate source code or record small historical changes. If
something is obvious from a local implementation, the source should remain the
authoritative detail.

## Overall Shape

The project is a small compiler toolchain implemented in C, with:

- a command-line executable in `src/nerd.c`
- shared infrastructure in `src/core`
- compiler stages in `src/compiler`
- a schema-driven CLI layer in `src/cli`
- terminal presentation helpers in `src/table`
- JSON/object support in `src/object`
- a test runner in `src/testing`
- an LSP server in `src/lsp`
- tests and planning documents in `tests`

The build is driven by [`build/build.py`](/home/matt/nerd/build/build.py),
which discovers source files automatically and resolves module dependencies from
`//> use:` directives.

## Reading Order

For most tasks, the most useful reading order is:

1. [`src/nerd.c`](/home/matt/nerd/src/nerd.c)
2. [`src/compiler/compiler.h`](/home/matt/nerd/src/compiler/compiler.h)
3. [`src/compiler/build/build.h`](/home/matt/nerd/src/compiler/build/build.h)
4. [`src/compiler/build/front/front.c`](/home/matt/nerd/src/compiler/build/front/front.c)
5. [`src/compiler/build/back/back.c`](/home/matt/nerd/src/compiler/build/back/back.c)
6. the specific subsystem you are changing

If the task is about tests, read:

1. [`tests/README.md`](/home/matt/nerd/tests/README.md)
2. [`src/testing/testing.c`](/home/matt/nerd/src/testing/testing.c)
3. [`src/testing/diff.c`](/home/matt/nerd/src/testing/diff.c)

## Top-Level Execution Flow

The process entry point is [`src/core/main.c`](/home/matt/nerd/src/core/main.c),
which calls `run(argc, argv)`.

For the main compiler executable:

- [`src/nerd.c`](/home/matt/nerd/src/nerd.c) builds the CLI schema
- the generic CLI parser parses `argc/argv` into JSON-like command data
- `src/nerd.c` converts parsed values into command-specific config structs
- dispatch then calls one of the command handlers from the compiler or LSP layer

The `nerd` executable is therefore a thin orchestration layer. Most real work is
done by subsystems beneath it.

## Dependency Shape

At a high level, the dependencies look like this:

- `core`
  Foundation layer. Intended to be broadly reusable.
- `object`, `table`
  Built on `core`.
- `cli`
  Built on `core`, `table`, and `object`.
- `compiler`
  Built on `core` and compiler-specific submodules.
- `testing`
  Built on `core` and `compiler`.
- `lsp`
  Built on `core` and `object`.
- `nerd.c`
  Top-level composition layer depending on `cli`, `compiler`, `lsp`, `table`,
  `object`, and `testing`.

In practice, `core` is the bottom of the project and `src/nerd.c` is the top.

## Core Layer

`src/core` is the utility layer the rest of the project depends on.

It contains:

- memory and dynamic array support
- strings and string builders
- output and terminal printing
- time and timing helpers
- random numbers
- hash/map utilities
- file mapping
- shell/process helpers
- path manipulation
- directory iteration

When adding reusable low-level functionality, `core` is usually the right home.
Compiler-specific logic should stay out of this layer.

## CLI Layer

The CLI system lives in `src/cli` and is intentionally schema-driven.

The important design idea is:

- the CLI parser understands syntax and structure
- command code understands semantics

The CLI layer parses a JSON schema describing commands, flags, and parameters,
then returns a parsed result object. `src/nerd.c` uses that parsed structure to
build typed config structs for each command.

This keeps command parsing centralized and makes it easier to evolve the CLI
without scattering option handling across the codebase.

## Compiler Layer

The compiler is split into clear stages and command handlers.

### Public compiler surface

[`src/compiler/compiler.h`](/home/matt/nerd/src/compiler/compiler.h) is the
main public interface for compiler command/config types. It defines:

- command config structs
- artifact configuration
- `NerdSource`, which carries both source text and its path when available

[`src/compiler/build/build.h`](/home/matt/nerd/src/compiler/build/build.h) is
the shared pipeline header. It defines:

- `FrontEndState`
- `BackEndState`
- compiler dump helpers

### Front-end

The front-end lives primarily in
[`src/compiler/build/front/front.c`](/home/matt/nerd/src/compiler/build/front/front.c).

Its stages are:

1. lexing
2. parsing
3. IR generation

The front-end returns a `FrontEndState` containing:

- `Lexer`
- `Ast`
- `Ir`

Related subdirectories:

- `src/compiler/lexer`
- `src/compiler/ast`
- `src/compiler/ir`

### Back-end

The back-end lives primarily in
[`src/compiler/build/back/back.c`](/home/matt/nerd/src/compiler/build/back/back.c).

Its stages are:

1. C generation
2. optional save of generated C
3. optional native compilation of generated C

The back-end consumes `FrontEndState` and produces `BackEndState`, which
currently contains generated C state.

The back-end is driven by explicit artifact paths:

- binary output path
- IR output path
- C output path

This lets the same backend machinery support both normal builds and test runs.

### Rendering and dumping

The compiler has two kinds of output helpers:

- Renderers
  Stable string output intended for comparison or file emission.
- Dumpers
  Terminal-oriented diagnostic output intended for human inspection.

Examples:

- `ir_render(...)` vs `ir_dump(...)`
- `cgen_render(...)` vs `cgen_dump(...)`

This separation matters because tests need stable textual output, while command
handlers often want richer terminal diagnostics.

### Error system

The compiler error system lives under `src/compiler/error`.

Its central design is that compiler errors are assembled as structured
`ErrorInfo` values and then projected into one of several render modes:

- normal
  Human-readable terminal diagnostics with code snippets and markers.
- test
  Pretty JSON used by compiler error tests.
- diagnostics
  LSP-oriented JSON diagnostics payloads.

This means the compiler does not treat terminal output as the source of truth.
The structured error data is the source of truth, and the CLI, test runner, and
LSP each consume different renderings of the same underlying error.

### Command handlers

Compiler CLI command handlers live in files such as:

- [`cmd_build.c`](/home/matt/nerd/src/compiler/cmd_build.c)
- [`cmd_benchmark.c`](/home/matt/nerd/src/compiler/cmd_benchmark.c)
- [`cmd_million.c`](/home/matt/nerd/src/compiler/cmd_million.c)
- [`cmd_test.c`](/home/matt/nerd/src/compiler/cmd_test.c)

Shared orchestration helpers live in:

- [`cmd_common.c`](/home/matt/nerd/src/compiler/cmd_common.c)
- [`cmd_internal.h`](/home/matt/nerd/src/compiler/cmd_internal.h)

The command handlers should stay thin and delegate to shared compiler pipeline
helpers where possible.

### Internal dependency direction

Within the compiler, the intended dependency direction is:

- `source.h`
  Lowest-level source container shared across compiler subsystems.
- `lexer`
- `ast`
- `ir`
- `cgen`
- `build/front` and `build/back`
- command handlers

This matters because `NerdSource` is intentionally defined below the lexer,
rather than in the higher-level compiler command header, so the core compiler
stages do not depend on command-layer declarations.

## Testing Layer

The test runner lives in `src/testing`.

It currently covers:

- language tests from `tests/language/*.t`
- error tests from `tests/errors/*.e`

Responsibilities:

- discover test files
- parse test fixture sections
- run the compiler pipeline directly through `front_end()` and `back_end()`
- execute compiled output
- compare exit code, stdout, IR, and generated C
- compare structured error JSON for failing compiler cases
- print readable diffs for failures

The test runner is intentionally built on the same compiler pipeline used by the
normal commands, rather than shelling out to `nerd build`.

### Test artifacts

Tests generate artifacts beside the test file when needed:

- `.ir`
  Human-readable intermediate representation generated after parsing and IR
  construction.
- `.c`
  Generated C source emitted by the back-end and used as the input to `clang`.
- `.out`
  Compiled executable produced by test runs.

Passing tests clean those artifacts up automatically. Failing tests keep them
for inspection. `just clean` also removes them.

## LSP Layer

The language server lives in `src/lsp`.

This subsystem is separate from the CLI test/build path, but shares the same
core support code, compiler lexer, and object/JSON utilities.

At the moment, the LSP server keeps an in-memory document table and performs
lexing on `didOpen` and `didChange`. Diagnostics are published immediately after
each re-lex:

- successful lexing publishes an empty diagnostics array to clear prior errors
- failed lexing publishes diagnostics derived from the compiler error system's
  diagnostics render mode
- `didClose` clears diagnostics for that URI

The LSP diagnostics path therefore reuses compiler error construction rather
than duplicating diagnostic assembly inside the LSP layer. If working on editor
integration, this is the area to read after `src/nerd.c`.

## Object and Table Utilities

Two support modules appear frequently across the codebase:

- `src/object`
  JSON-like values and query helpers used heavily by the CLI and LSP layers.
- `src/table`
  Terminal table rendering used for argument dumps, help output, and diagnostic
  presentation.

These are infrastructure modules, not compiler stages, but they are important
because much of the user-facing console output is built on them.

## Build and Formatting Tooling

The main developer scripts live under `build/`.

Current notable scripts:

- [`build/build.py`](/home/matt/nerd/build/build.py)
  Compiles the project.
- [`build/format.py`](/home/matt/nerd/build/format.py)
  Runs `clang-format` across the source tree.

The main developer entry points are exposed through [`Justfile`](/home/matt/nerd/Justfile).

Important recipes:

- `just build`
- `just build-release`
- `just run nerd ...`
- `just test`
- `just format`
- `just clean`
- `just install`

`just clean` is intended to be the broad reset command for generated artifacts.

### Common local workflows

For normal command-line development:

- `just run nerd ...`
  Build the debug executable and run `nerd` with the provided arguments.
- `just run-release nerd ...`
  Build the release executable and run `nerd` with the provided arguments.

For editor integration:

- `just install`
  Builds the compiler, packages the VS Code extension, installs the `nerd`
  binary to the local user bin location, uninstalls any previous extension
  version, and installs the newly packaged VS Code extension.

## Current Testing Layout

The `tests` directory currently contains:

- `tests/language`
  successful compile-and-run tests
- `tests/errors`
  compiler error tests using expected JSON diagnostics

The language test format is documented in
[`tests/README.md`](/home/matt/nerd/tests/README.md).

## Practical Guidance

When making changes:

- start from `src/nerd.c` if the change is user-facing
- start from `src/compiler/build` if the change touches compiler flow
- start from `src/core` if the functionality is reusable infrastructure
- start from `src/testing` if the change affects test discovery, comparison, or
  artifact handling

If a change begins to mix responsibilities, it is usually a sign that logic
should be pushed down into a lower layer rather than kept in `src/nerd.c`.
