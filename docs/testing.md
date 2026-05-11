# Testing

The repository regression test runner lives in
[build/test.py](/home/matt/nerd/build/test.py).

`just test` is the main regression gate.

The `nerd test <root-filename>` command is reserved for unit tests declared in
Nerd source code. It is not the repository regression harness.

## Test Families

The repository currently has five main test families:

- `tests/language`
  End-to-end compiler tests with expected runtime result, stdout, HIR, and LLVM
  IR.
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
4. expected HIR
5. expected generated LLVM IR

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
3. expected stdout

They may also include optional sections for run mode, extra CLI arguments, and
command name. The parser accepts three to six sections.

The Python runner writes the source into a generated `.input.n` file next to the test
case and invokes the relevant compiler command against that file. This covers
behaviour outside the pure compiler pipeline, such as `run` resolving generated
binaries correctly when the input path is relative to the current directory.

## Platform-Specific Tests

Tests that require a specific host platform can declare it in the source section:

```nerd
-- test-platform: linux
```

The runner skips the case when the current host platform is not listed. Use this
only for tests that exercise host APIs or toolchain behaviour that cannot be made
portable, such as POSIX-only FFI calls.

## Artefact Behaviour

For language tests, generated `.ir`, `.c`, and `.out` artefacts are removed on
success and retained on failure. That policy is deliberate: failures should be
easy to inspect locally.

Command tests also generate temporary `.input.n` files and command artefacts
beside the test case; successful runs remove them.

## Source Tests

Source-level unit tests live inside ordinary Nerd files:

```nerd
test "name" {
    assert yes
}
```

Run them with:

```sh
nerd test root.n
nerd test root.n --list
nerd test root.n --filter name
nerd test root.n -v
```

Source tests use the normal compiler and runtime assertion path. A failing
assertion exits with status 127, so the current runner stops at the first
failing selected test.

With `-v` or `--verbose`, the test command prints one `[PASS] name` or
`[FAIL] name` result line as each selected test finishes.

## Working Style

When adding a language feature, update tests horizontally:

- language behaviour
- semantic or parser errors
- formatter snapshots if syntax changes
- LSP behaviour when names, types, or diagnostics are affected
- VS Code and Neovim/LazyVim integration when filetypes, syntax highlighting,
  LSP launch/configuration, formatting, or install behaviour changes
