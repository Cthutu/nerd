# Token Trivia Formatter Prototype

Status: first slice implemented

## Goal

Prototype a formatter path driven by lexer tokens, explicit trivia tables, and
syntax context, with sema as optional context only.

## Non-Goals

- Do not replace the existing formatter in one change.
- Do not require semantic analysis for ordinary formatting.
- Do not change formatting output in the first trivia-table step.
- Do not add a separate Python script until there is a repeatable workflow that
  the existing test runner cannot cover.

## Proposed First Slice

Build a `FormatTrivia` product from the existing format-mode lexer/source:

```c
typedef struct {
    Array(u16) newlines_before_token;
    Array(u32) first_comment_before_token;
    Array(u16) comment_count_before_token;
    Array(u32) trailing_comment_index_by_token;
} FormatTrivia;
```

The first implementation can be internal to `src/compiler/format/format.c`.
If it proves useful, move it behind a small formatter-internal API.

Implemented:

- internal `FormatTrivia` struct in `src/compiler/format/format.c`
- `format_trivia_build(...)`
- `format_trivia_done(...)`
- `format_trivia_trailing_comment_after_token(...)`
- newline counts keyed by token index plus an EOF slot
- leading comment ranges keyed by token index plus an EOF slot
- trailing comment index keyed by the preceding token
- debug validation that compares trivia trailing-comment classification with the
  previous offset-scanning implementation
- trivia-backed `format_node_has_trailing_comment(...)`
- trivia-backed node trailing-comment emission through
  `format_emit_trailing_comment_after_token(...)`
- trivia-backed offset trailing-comment lookup when the offset matches a token
  boundary, with the previous offset scan retained as fallback

The first slice is mostly passive: it constructs trivia tables on the current
formatter paths, validates trailing-comment classification in debug builds, and
uses the trivia table for token-indexed trailing-comment queries and node
trailing-comment emission. Offset-based trailing-comment consumers now use the
same trivia table where possible. It does not change emitted output.

## Acceptance Target

The first slice should preserve current output for:

- `tests/format/001-comment-wrap.f`
- `tests/format/002-blank-lines.f`
- `tests/format/085-comments-inside-blocks.f`
- `tests/format/093-trailing-comments.f`
- `tests/format/094-trailing-comment-continuations.f`
- `tests/format/096-comments-break-groups.f`

Then run the full formatter subset:

```sh
python3 build/test.py --filter tests/format
```

Verification on 2026-05-07:

```text
just build nerd
python3 build/test.py --filter tests/format

format: 105 passed, 0 failed, 0 skipped
```

Second verification after the first trivia consumer:

```text
python3 build/format.py
just build nerd
python3 build/test.py --filter tests/format

format: 105 passed, 0 failed, 0 skipped
```

Third verification after trivia-backed node trailing-comment emission:

```text
python3 build/format.py
just build nerd
python3 build/test.py --filter tests/format

format: 105 passed, 0 failed, 0 skipped
```

Fourth verification after trivia-backed offset trailing-comment lookup:

```text
python3 build/format.py
just build nerd
python3 build/test.py --filter tests/format

format: 105 passed, 0 failed, 0 skipped
```

## Design Notes

- `LEXER_MODE_FORMAT` already records `LexerComment` rows with offset,
  end-offset, token index, and text.
- Newline detection already exists in CST as
  `cst_token_has_newline_before(...)`, but it is local to CST parsing.
- A trivia product should make newline/comment attachment explicit and reusable
  by both current formatter paths and a future token-state formatter.
- Comments need paragraph logic in addition to token attachment. The trivia
  table should classify placement; comment reflow can remain a separate
  subsystem.

## Open Questions

- Should trivia construction happen in the lexer, formatter, or future tolerant
  syntax layer?
- Should trailing comments be keyed by the token before the comment or by the
  next token with a `trailing` classification?
- How should comments between two tokens on separate lines be represented when
  both syntactic attachment and original spacing matter?
- Should newline counts saturate at `u16`, or should the formatter only care
  about `0`, `1`, and `2+`?

## Next Step

Move one leading-comment or newline-sensitive path onto `FormatTrivia`, ideally
behind a narrow helper so the recursive formatter keeps shrinking toward a
token/trivia state machine.
