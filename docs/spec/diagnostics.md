# Diagnostics

Nerd diagnostics are structured compiler reports for invalid user source. The
error system itself is documented in [`../error-system.md`](../error-system.md);
this spec records the language-facing diagnostic contract.

Source anchors:

- `src/compiler/error/error.h`
- `src/compiler/error/*_errors.c`
- `src/compiler/lexer/lexer.c`
- `src/compiler/ast/parse.c`
- `src/compiler/sema/sema.c`

## Diagnostic Payload

Each language diagnostic is built as an `ErrorInfo` record with:

| Field | Purpose |
| --- | --- |
| `kind` | Error, warning, internal error, or runtime error category. |
| `error_message` | Main human-readable failure message. |
| `source` | Source file content and path. |
| `span` | Primary byte span. |
| `references` | Primary and secondary labelled source references. |
| `notes` | Explanatory context. |
| `help_messages` | Actionable fix guidance. |

Diagnostics should identify the source construct that caused the failure and
include enough context for terminal rendering, JSON error tests, and LSP
diagnostic publishing to agree.

## Phase Responsibilities

| Phase | Diagnostic responsibility |
| --- | --- |
| Lexer | Reject malformed tokens and literals, including unexpected characters, invalid numbers, over-large literals, and unterminated strings. |
| Parser / AST | Reject malformed syntax, missing values/operators, unexpected tokens, invalid binding targets, and invalid type grammar. |
| Semantic analysis | Reject invalid names, duplicate declarations, type mismatches, invalid casts, invalid `on` forms, invalid loop control, default-parameter errors, FFI restrictions, privacy violations, and definite-assignment failures. |
| Module loading | Report missing modules, import cycles, and failed module part loads through runtime/module-loading errors when the source itself is not the failing construct. |
| Tooling | Reuse compiler diagnostics for LSP publication instead of inventing separate editor-only language errors. |

HIR and LLVM generation should not introduce new user-facing language
diagnostics. If those phases receive invalid compiler IR, that is an internal
compiler error unless an earlier phase deliberately deferred the check.

## Rendering Modes

The same structured diagnostic can be rendered in three modes:

| Mode | Consumer |
| --- | --- |
| `ERROR_RENDER_NORMAL` | Human-facing terminal output. |
| `ERROR_RENDER_TEST` | Stable JSON for `tests/errors/*.e`. |
| `ERROR_RENDER_DIAGNOSTICS` | JSON diagnostics consumed by the LSP path. |

The structured `ErrorInfo` is the source of truth. Renderers should project the
same message, spans, notes, and help into their output format.

## Authoring Rules

- Use a categorised diagnostic when the user can fix the source program.
- Use `error_runtime(...)` for filesystem, shell, toolchain, and environment
  failures.
- Use `error_ice(...)` only for violated compiler invariants.
- Keep diagnostic helpers broad enough to reuse across related source cases.
- Put source-specific detail in the rendered message, notes, references, and
  help text.
- Prefer primary spans at the earliest token where the compiler can identify
  the broken source rule.
- Add `tests/errors/*.e` coverage for new reachable public diagnostics.
