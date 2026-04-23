# Testing

The repository test runner lives in
[src/testing/testing.c](/home/matt/nerd/src/testing/testing.c).

`just test` is the main regression gate.

## Test Families

The repository currently has four main test families:

- `tests/language`
  End-to-end compiler tests with expected runtime result, stdout, IR, and C.
- `tests/errors`
  Structured error-rendering tests in JSON form.
- `tests/format`
  Formatter snapshot tests.
- `tests/lsp`
  LSP transcript-style tests.
- `tests/commands`
  Command-level regressions for public compiler subcommands.

## Language Tests

Language tests use `.t` files with five sections separated by `¬`:

1. source code
2. expected return value
3. expected stdout
4. expected IR
5. expected generated C

The first section is also valid source input by itself, which makes local
debugging straightforward.

## Error Tests

Error tests use `.e` files. Each case has two sections:

1. source code
2. expected JSON diagnostic output

These tests validate the structured error path, not only human terminal text.

## Formatter Tests

Formatter tests use `.f` files with:

1. input text
2. expected formatted output

This keeps formatter behaviour deterministic and snapshot-friendly.

## LSP Tests

LSP tests use transcript-like inputs that drive requests against the language
server and compare the resulting JSON output.

These tests matter because several editor features depend on compiler analysis
and structured diagnostics.

## Command Tests

Command tests use `.cmd` files with three sections:

1. source code
2. expected process return value
3. currently reserved stdout section

The runner writes the source into a generated `.input.n` file next to the test
case and invokes the relevant compiler command against that file. This covers
behaviour outside the pure compiler pipeline, such as `run` resolving generated
binaries correctly when the input path is relative to the current directory.

## Artefact Behaviour

For language tests, generated `.ir`, `.c`, and `.out` artefacts are removed on
success and retained on failure. That policy is deliberate: failures should be
easy to inspect locally.

Command tests also generate temporary `.input.n` files and command artefacts
beside the test case; successful runs remove them.

## Working Style

When adding a language feature, update tests horizontally:

- language behaviour
- semantic or parser errors
- formatter snapshots if syntax changes
- LSP behaviour when names, types, or diagnostics are affected
