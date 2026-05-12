# Formatter Layout Classification

Status: active
Date: 2026-05-12

## Purpose

`src/compiler/format/format.c` is still a large formatter implementation with
several layout policies embedded inside recursive emission. This note classifies
the remaining work so future commits can move one policy at a time without
changing unrelated formatting behaviour.

## Current Layout Policy Groups

### Token Spacing

Main code:

- `format_expr_precedence`
- `format_emit_expr`
- `format_assignment_operator`
- `format_token_kind_is_binary_operator`
- `format_token_needs_space_between`
- `format_emit_token_stream_block`

Role:

- operator spacing
- parenthesis decisions
- fallback token-stream spacing for incomplete source
- punctuation spacing for calls, fields, indices, slices, casts, and ranges

Risk:

- the parsed CST path and token fallback path can drift because they use
  different local spacing rules
- binary operator classification exists in the token fallback while precedence
  lives in the CST expression renderer

Next extraction target:

- a small syntax/token spacing helper that can answer "space before this token"
  without knowing whether the caller is the CST path or fallback path

### Comment Attachment

Main code:

- `FormatTrivia`
- `format_emit_block_comments_before_offset`
- `format_emit_block_comments_before_token`
- `format_skip_block_comments_before_offset`
- `format_node_has_trailing_comment`
- `format_emit_trailing_comment_after_offset`
- `format_emit_trailing_comment_after_token`
- `format_emit_trailing_comment_for_node`
- `format_emit_token_comments_before`

Role:

- leading comment emission
- trailing comment detection and emission
- comment-only line rendering
- continuation comments after aligned regions

Risk:

- offset-driven and token-driven paths still coexist
- some structured emitters still ask whether an offset has a trailing comment
  instead of asking for the token-attached trivia slot

Next extraction target:

- convert one remaining offset-driven trailing-comment consumer to an explicit
  token/index query and leave the scan path as a validation fallback only

Migration note:

- Multiline plex literal field alignment and multiline plex type field
  alignment now ask `FormatTrivia` directly for trailing comments after the
  computed value/type end token. The older offset scan remains only for callers
  that run without trivia.

### Blank-Line Policy

Main code:

- `format_has_blank_line_between_offsets`
- `format_syntax_has_blank_line_between_nodes`
- `format_ffi_infos_have_blank_line_between`
- blank-line checks inside `format_emit_block_contents`
- blank-line checks inside top-level emission
- line-oriented paragraph handling in `format_source`

Role:

- preserve intentional blank lines between statement groups
- avoid introducing duplicate blank lines around adjacent bindings
- split FFI and aligned regions on blank lines
- separate top-level comment paragraphs from code blocks

Risk:

- most parsed-code decisions still count source lines between offsets, while
  the token fallback already uses `FormatTrivia.newlines_before_token`
- the same "is there a blank line between these syntax items?" question is
  answered by several call sites with slightly different inputs

Next extraction target:

- add a token-range blank-line query to `FormatSyntaxContext` and migrate one
  block/top-level grouping decision to it

Migration note:

- `format_trivia_has_blank_line_between_tokens` now answers token-range
  blank-line questions from `FormatTrivia`.
- `format_syntax_has_blank_line_between_nodes` uses that token/trivia query
  before falling back to offset line counting.
- FFI block grouping also checks the token/trivia query before using the older
  offset path.

### Indentation

Main code:

- `format_emit_indent`
- `format_emit_block_contents`
- `format_emit_expr_with_indent`
- `format_emit_value_with_indent`
- `format_emit_on_block_multiline`
- `format_emit_plex_literal_multiline`
- `format_emit_array_multiline`
- `format_emit_token_stream_block`

Role:

- block body indentation
- expression-valued block indentation
- multiline aggregate indentation
- `on` branch indentation
- fallback brace indentation

Risk:

- indentation is passed through many recursive calls as a raw `u32`
- multiline expressions sometimes set global formatter state
  (`g_format_expr_indent_level`, `g_format_value_indent_level`) before
  rendering nested constructs

Next extraction target:

- keep raw indentation for now, but avoid adding more global indentation state;
  new multiline helpers should take indentation explicitly

### Alignment Regions

Main code:

- `FormatAlignedStatement`
- `format_collect_aligned_statement`
- `format_aligned_statements_same_family`
- `format_emit_aligned_statement_group`
- field alignment in `format_emit_plex_literal_multiline`
- variant/payload alignment in type plex/enum emitters
- FFI entry width planning

Role:

- align local declarations and assignments
- align plex literal fields and type fields
- align enum variants and trailing comment continuations
- align grouped FFI entries

Risk:

- planning is repeated per construct family
- some regions decide alignment while emitting, which makes comment/blank-line
  boundaries harder to reason about
- trailing comments can disable or alter alignment behaviour in top-level and
  block paths

Next extraction target:

- introduce a region planner abstraction only after one more comment/trivia
  migration; otherwise the planner will inherit the current offset decisions

Migration note:

- `format_plan_trailing_comment_columns` now plans trailing-comment alignment
  columns for a syntax region from item starts, ends, code widths, and
  token-attached comment indices.
- Multiline plex literals and multiline plex type fields use that shared
  planner instead of each carrying a local copy of the grouping loop.

### Construct-Specific Rendering

Main code:

- `format_emit_fn_signature`
- `format_emit_ffi_def`
- `format_emit_ffi_block`
- `format_emit_mod_ref`
- `format_emit_top_on`
- `format_emit_impl`
- `format_emit_test`
- `format_emit_pattern`
- `format_emit_value`
- `format_emit_variable_payload`

Role:

- syntax-specific spelling and local multiline choices

Risk:

- these functions are the right place for language spelling, but not for shared
  comment attachment, blank-line, or alignment policy
- formatter changes can become too broad when a construct-specific renderer also
  owns layout-region planning

Next extraction target:

- when changing one construct renderer, first check whether the desired change
  is really a shared layout rule

## Recommended MS3 Order

1. Move one blank-line decision from offset line counting to token/trivia syntax
   helpers.
2. Move one remaining comment consumer from offset scans to `FormatTrivia`.
3. Only then extract an alignment-region planner, starting with a narrow family
   such as local statements or FFI entries.
4. Grow fallback snapshots for each partial-source case before changing token
   fallback behaviour.

This order keeps the formatter contract syntax/trivia-based and avoids
stabilising a region planner around the old offset-driven comment model.
