# LSP Crash Audit

Status: started

## Baseline

Date: 2026-05-07

Branch: `architecture-review`

Commands:

```sh
just build nerd
python3 build/test.py --filter tests/lsp
```

Result:

```text
lsp: 95 passed, 0 failed, 0 skipped
```

Interpretation:

- The existing LSP transcript suite is currently green.
- The crash audit needs either real editor reproduction steps or new stress
  transcripts that exercise incomplete source states not covered today.
- Current tests already include many partial/fallback behaviours, so new crash
  cases should be reduced to specific missing readiness/state assumptions.

## Cases

Record each crash with reproduction steps, expected behaviour, actual behaviour,
classification, and follow-up.

### Case 1: Incremental edit to incomplete member access

Source state:

- Document opens valid.
- Incremental edit removes the field name from `frame.handle`, leaving
  `frame.`.

Request:

- `textDocument/completion` immediately after the edit at the dot position.

Reproduction:

```sh
python3 build/test.py --filter 096-incremental-member-completion-after-edit
```

Expected:

- LSP does not crash.
- Diagnostics publish the parse error for incomplete field access.
- Completion still offers `handle` and `system` for `frame`.

Actual:

- Covered by `tests/lsp/096-incremental-member-completion-after-edit.lsp`.

Classification:

- `source_ready`
- `tokens_ready`
- partial syntax after edit
- repaired-source/member-completion fallback

Likely owner:

- `src/lsp/document.c`
- `src/lsp/completion.c`

Regression test:

- `tests/lsp/096-incremental-member-completion-after-edit.lsp`

Follow-up:

- This specific incremental-edit shape is covered and does not reproduce a
  crash.
- Continue with additional real editor crash shapes, especially hover,
  semantic tokens, rename, and module completion after edits.

### Case 2: Hover after incremental edit to incomplete member access

Source state:

- Document opens valid.
- Incremental edit removes the field name from `frame.handle`, leaving
  `frame.`.

Request:

- `textDocument/hover` on the still-valid `frame` reference after the edit.

Reproduction:

```sh
python3 build/test.py --filter 097-incremental-hover-after-partial-edit
```

Expected:

- LSP does not crash.
- Diagnostics publish the parse error for incomplete field access.
- Hover for `frame` still reports local variable information and type `^Frame`.

Actual:

- Covered by `tests/lsp/097-incremental-hover-after-partial-edit.lsp`.

Classification:

- `source_ready`
- `tokens_ready`
- partial syntax after edit
- sema partial state retained for hover

Likely owner:

- `src/lsp/document.c`
- `src/lsp/hover.c`

Regression test:

- `tests/lsp/097-incremental-hover-after-partial-edit.lsp`

Follow-up:

- This specific hover-after-edit shape is covered and does not reproduce a
  crash.
- Continue with semantic tokens, rename, and module completion after edits.

## Structured Stress Baseline

Date: 2026-05-07

Command:

```sh
python3 build/review_lsp_stress.py
```

Result:

```text
[PASS] completion after member field delete
[PASS] hover after member field delete
[PASS] semantic tokens after member field delete
[PASS] document symbols after member field delete
[PASS] rename after member field delete
[PASS] module completion after broken use edit
[PASS] semantic tokens after broken generic edit
[PASS] hover after broken generic edit
```

Interpretation:

- These partial-edit paths do not currently crash when reduced to deterministic
  stdin/stdout LSP sessions.
- This runner is not a substitute for transcript regression tests. It is a
  cheap way to grow crash-shape coverage while deciding which cases deserve
  permanent `.lsp` transcripts.
- A request-level JSON-RPC response is required for every stress request; a
  process exit, malformed frame stream, or missing response is a failure.

## Existing Coverage Groups

The current suite covers:

- hover and definition
- document symbols
- semantic tokens
- diagnostics, including imported/module-part diagnostics
- type aliases, scoped locals, functions, loops, `on`, patterns, modules, and
  dynamic arrays
- completion, including partial member completion and module fallback paths
- signature help and named call arguments
- code actions for plex literals
- rename, including rename with syntax/import errors
- payload/member completion while source has errors
- pointer-field completion during type edits

## Crash Classification Template

Use this for each reproduced crash:

```text
### Case N: short title

Source state:
Request:
Reproduction:
Expected:
Actual:
Classification:
  - source_ready | tokens_ready | syntax_ready | decls_ready
  - sema_partial | sema_complete | module_state | range_mapping
Likely owner:
Regression test:
Follow-up:
```

## Initial Risk Categories

These are areas to target with real editor repros or new transcript tests.

### Partial Semantic Tables

Symptoms:

- feature assumes `node_*` side-table rows exist for an AST node
- feature assumes a declaration/type/local index is valid because full
  compilation would have produced it

Likely features:

- hover
- semantic tokens
- completion
- code actions
- rename

Follow-up:

- Checked accessors now cover LSP semantic side-table reads outside the
  accessor implementation itself.
- Continue by splitting declaration/binding/type readiness if real crash
  transcripts show that `sema_partial` is too coarse.

### Failed Or Partial Module Analysis

Symptoms:

- imported module or folder module failed before exports/types were complete
- feature follows `import_module_index` into missing module state

Likely features:

- module completion
- imported hover/definition
- code actions using imported plex definitions
- diagnostics grouping

Follow-up:

- Define module readiness levels separately from document readiness.

### Range Mapping And Incremental Edits

Symptoms:

- LSP offset/range points outside current document contents
- analysis source differs from editor buffer after incremental changes

Likely features:

- didChange
- semantic tokens
- rename edits
- diagnostics ranges

Follow-up:

- Add transcript tests with chained incremental edits around incomplete syntax.

### Repaired-Source Completion

Symptoms:

- completion creates a temporary repaired buffer and then maps results through
  stale source/token assumptions

Likely features:

- member completion after `value.`
- payload completion inside incomplete `on` branches
- module completion while imports are broken

Follow-up:

- Prefer explicit readiness/products over repaired-source paths where possible.

## Next Audit Tasks

- Reproduce at least one real editor crash and reduce it to an LSP transcript.
- Add the transcript before making architecture changes.
- Identify the exact table/range/module assumption that caused the crash.
- Record whether a readiness-level API would have prevented it.
