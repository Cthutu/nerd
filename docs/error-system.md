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

## Failure Policy

Every failure point while compiling user language source must be one of two
things:

- a categorised compiler diagnostic, when the source program is invalid or uses
  unsupported language features
- an ICE, when an internal compiler invariant is violated and the compiler
  itself is buggy

Use diagnostics for anything a user can reasonably fix in their source. Use
`error_ice(...)` only for states that should be impossible after earlier
compiler phases have done their job.

Failures outside the source program are runtime errors. Use
`error_runtime(...)` for OS, filesystem, shell, and toolchain failures such as
being unable to write generated files or compile the generated LLVM IR. These
are not language diagnostics, and they are not ICEs unless they expose a
compiler invariant violation.

HIR generation and LLVM generation should not invent user-facing language
errors. If those phases see an invalid AST, invalid HIR operation, invalid HIR
value, or unsupported internal type state, report an ICE. Earlier phases are
responsible for rejecting invalid source programs.

`help` and `note` messages have different roles:

- `help` is contextual fix guidance. It should point at what the user can change
  for this specific diagnostic.
- `note` is static explanatory context. It should explain a rule, limitation, or
  background fact that is not tailored to one specific fix.

## Diagnostic Shape Policy

The project treats diagnostic helpers as broad compiler-error categories, not as
one-off leaf cases.

That has a few consequences:

- the rendered main message carries the concrete details for the current source
  case
- notes explain why the diagnostic applies
- help messages explain what the user can do next

For example, a parser diagnostic helper should describe a general parser failure
class such as:

- missing value
- missing operator
- expected token
- unexpected operator

It should not name one tiny syntax corner-case in the function/API itself. The
current source-specific wording belongs in the rendered message, note, and help
text instead.

### Main Message

The primary diagnostic message should be:

- category-stable
- concrete about the current source case
- templated with specific values

Examples:

- `Expected \`]` but found \`)``
- `Unexpected \`<\` after \`..\``

The helper stays broad; the rendered message stays precise.

### Notes And Help

Keep the distinction strict:

- `note`
  - extra explanatory context
  - language rule or parser/semantic fact
  - why the compiler rejected the construct

- `help`
  - actionable fix guidance
  - what the user can change in the source
  - likely next step

Good example:

- error: `Unexpected \`<\` after \`..\``
- note: `Exclusive end ranges and slices use \`..\`; inclusive end forms use \`..=\``
- help: `Remove the \`<\` and keep \`..\` for an exclusive end range or slice`

### Primary Spans

Parser diagnostics should prefer the earliest token where the parser can tell
which source rule was broken. A later token may be where parsing finally fails,
but the primary span should move back to the syntactic fork when the parser has
a reliable recovery clue. For example, a binding value shaped like
`name :: (...) { ... }`, `name :: symbol (...) { ... }`, or
`name := (...) { ... }` is diagnosed at the apparent function introducer,
because function values after binding operators must start with `fn`.

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
Diagnostics for expanded folder modules carry source-fragment information so
terminal and LSP renderers can report the original part file and line rather
than the synthetic concatenated module source.

## Coverage Policy

Every reachable public language diagnostic should have structured coverage under
`tests/errors/*.e`. Add separate tests for reused categories when the source
span, related references, notes, or help text differ in a meaningful way.

Some diagnostics describe hard implementation limits rather than ordinary source
mistakes. Those should still remain categorised and structured, but they do not
need source-file tests that require enormous inputs or fragile machine-resource
setup. Cover those through a synthetic diagnostic harness when one exists.

The current deferred hard-limit categories are:

- `0102`
- `0105`
- `0200`

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

- `error: message`
- file, line, and column
- a source snippet
- primary `^` markers
- secondary `~` markers
- optional note and help blocks

Source locations come from lexer offset helpers, not stored spans inside AST
nodes.

## Adding Or Reusing An Error Category

The usual pattern is:

1. first check whether an existing broad diagnostic category already fits
2. if an existing category fits, reuse it and pass source-specific message,
   note, and help context
3. only add a new helper in
   [error.h](/home/matt/nerd/src/compiler/error/error.h) when the category is
   broad enough to reuse across source cases
4. implement the helper in the relevant phase-specific file
5. attach the right primary and secondary references
6. add contextual help text for likely fixes
7. add notes for static rule explanations when useful
8. add or update `tests/errors/*.e`
9. verify the LSP diagnostics rendering still makes sense

New feature work is not complete until the error path is covered as well.
