# Formatter Edge-Case Audit

Status: started

## Baseline

Date: 2026-05-07

Branch: `architecture-review`

Commands:

```sh
just build nerd
python3 build/test.py --filter tests/format
```

Result:

```text
format: 105 passed, 0 failed, 0 skipped
```

Interpretation:

- The existing snapshot suite is currently green.
- The audit is not starting from known failing tests.
- The main risk is architecture and maintainability: the formatter has broad
  coverage but many construct-specific paths in one large file.

## Cases

Record each formatter edge case with input, current output, expected output,
idempotence result, classification, and follow-up.

### Case 1: Idempotence baseline

Date: 2026-05-08

Experiment:

- Temporarily ran every formatter snapshot output through `nerd format` a
  second time.

Result:

- 101 of 105 formatter snapshots were idempotent at baseline.
- 4 snapshots changed on the second pass:
  - `tests/format/024-aligned-assignment.f`
  - `tests/format/078-string-reflow.f`
  - `tests/format/081-inherent-impl-methods.f`
  - `tests/format/097-enum-and-union-trailing-comments.f`

Classification:

- aligned declaration/string reflow stability
- blank-line stability inside `impl`
- trailing-comment continuation alignment stability

Follow-up:

- Keep global idempotence enabled in `build/test.py`.
- Add new formatter snapshots for any future formatting instability before
  changing the formatter.

Update:

- `tests/format/081-inherent-impl-methods.f` was fixed by avoiding duplicate
  blank-line insertion between adjacent function bindings in blocks.
- `tests/format/024-aligned-assignment.f` and
  `tests/format/078-string-reflow.f` were fixed by allowing already-wrapped
  string literal payloads to stay in aligned declaration groups.
- `tests/format/097-enum-and-union-trailing-comments.f` was fixed by treating
  trailing-comment continuation lines as part of the preceding enum variant
  when computing alignment groups.
- Remaining known non-idempotent snapshots: 0.

## Existing Coverage Groups

The current suite already covers these important families:

- comment wrapping and blank-line collapse
- expression spacing and grouping preservation
- local binding and assignment alignment
- function signatures, defaults, generics, and methods
- blocks, expression blocks, loops, `on`, `defer`, and `assert`
- plexes, unions, enums, payloads, patterns, and literal alignment
- modules, grouped `use`, public `use`, and module parts
- dynamic arrays, slices, nil pointers, casts, and field assignments
- trailing comments, continuation comments, standalone comments, and comment
  gaps in structured declarations and literals
- source tests, long signatures, string reflow, return-for expressions, and
  `break on`

## Architectural Risk Classification

These are not failing cases yet; they are categories likely to produce edge
cases as syntax grows.

### Token/Trivia Ownership

Current formatter flow splits comment paragraphs, blank lines, code chunks, CST
parsing, and CST emission across multiple paths. Edge cases are likely when a
comment is both syntactically attached to a node and layout-sensitive as trivia.

Current migration status:

- `FormatTrivia` records newline counts, leading comment ranges, and trailing
  comment indices by token.
- Trailing-comment checks and emission use trivia when the offset maps to a
  token boundary, with scan fallback.
- Block statement leading comments now use trivia comments attached to the
  statement's first token, with scan fallback.
- Structured multiline field and variant leading comments in plex literals,
  enum declarations, and plex declarations now emit from token-attached trivia,
  with scan fallback.

Follow-up:

- Continue migrating one comment/newline consumer at a time.
- Use the existing comment tests as regression coverage.

### Region Planning Versus Token Emission

Alignment requires knowledge of a whole region before emitting any one token.
Examples include local declarations, plex literal fields, enum discriminants,
and trailing comments.

Follow-up:

- Treat alignment as a prepass over token/node ranges.
- Avoid burying alignment decisions inside recursive node emission.

### Tolerant Formatting

The current formatter relies on CST parsing for code blocks. A token/trivia
formatter could preserve or lightly normalise more source even when parsing is
incomplete.

Follow-up:

- Define which formatter features should work with tokens only.
- Add failing/incomplete syntax cases only after deciding intended behaviour.

### Sema Coupling

Formatting should not require semantic success. Some syntax ambiguity may be
helped by semantic facts, but sema should remain optional.

Follow-up:

- List any current or proposed formatting rule that truly needs resolved types.
- Prefer syntax-context tables over sema where possible.

## Next Audit Tasks

- Classify the remaining offset-driven comment consumers as either cursor
  advancement, source-range layout queries, or trivia-table emission.
- Decide whether blank-line decisions between statements should stay as
  source-range queries or move to token newline counts.
- Keep adding focused snapshots for any formatter instability found during
  trivia migration.
