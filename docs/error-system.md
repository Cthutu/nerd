# Error System

The compiler error system is structured around `ErrorInfo`, not around ad hoc
printed strings.

The core interfaces are in
[src/compiler/error/error.h](/home/matt/nerd/src/compiler/error/error.h),
[src/compiler/error/error.c](/home/matt/nerd/src/compiler/error/error.c), and
[src/compiler/error/render.c](/home/matt/nerd/src/compiler/error/render.c).

## Core Model

Each reported diagnostic is assembled as an `ErrorInfo` value containing:

- an `ErrorKind`
- a four-digit code
- the main message
- source file data
- a primary span
- references
- notes
- help messages

Subsystem-specific helpers such as `error_0300_unknown_symbol(...)` construct
that structured payload for one diagnostic category.

All public error helpers return `bool` so callers can write:

```c
return error_0300_unknown_symbol(...);
```

## Error-Code Ranges

The project keeps phase-specific ranges:

- `0100` to `0199` for lexer errors
- `0200` to `0299` for parser and AST errors
- `0300` to `0399` for semantic-analysis errors

That convention matters for readability, tests, and future coverage.

## Render Modes

The same `ErrorInfo` can be rendered in three modes:

- `ERROR_RENDER_NORMAL`
  Human-facing terminal diagnostics with snippets and coloured markers.
- `ERROR_RENDER_TEST`
  Stable JSON used by `tests/errors/*.e`.
- `ERROR_RENDER_DIAGNOSTICS`
  JSON diagnostics consumed by the LSP path.

This is the key architectural choice: the structured error is the source of
truth, and each consumer gets a different projection of it.

## Global Error State

The current implementation uses a small global error subsystem to manage:

- the active render mode
- whether output should be emitted immediately
- an arena for building error messages
- the last rendered output string

The last-rendered buffer is especially important for tests and LSP. The LSP
path temporarily switches the system into diagnostics mode, runs analysis, then
parses the rendered JSON diagnostics back into LSP responses.

## Normal Rendering

Normal rendering builds compiler-style diagnostics with:

- `error[CODE]: message`
- file, line, and column
- a source snippet
- primary `^` markers
- secondary `~` markers
- optional note and help blocks

Source locations come from lexer offset helpers, not stored spans inside AST
nodes.

## Adding A New Error

The usual pattern is:

1. add the function declaration in [error.h](/home/matt/nerd/src/compiler/error/error.h)
2. implement the helper in the relevant phase-specific file
3. attach the right primary and secondary references
4. add notes or help text if they improve the fix path
5. add or update `tests/errors/*.e`
6. verify the LSP diagnostics rendering still makes sense

New feature work is not complete until the error path is covered as well.
