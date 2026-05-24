# Testing

The repository regression test runner lives in
[build/test.py](/home/matt/nerd/build/test.py).

`just test` is the main regression gate.

The `nerd test <root-filename>` command is reserved for unit tests declared in
Nerd source code. It is not the repository regression harness.

Release smoke checks live beside the main harness:

```sh
python3 build/test_install.py --nerd _bin/nerd-debug
python3 build/check_editor_integrations.py --nerd _bin/nerd-debug
```

After `just install`, run the same scripts with the installed `nerd` binary by
omitting `--nerd` or by setting `NERD_BIN`.

## Test Families

The repository currently has seven main test families:

- `tests/language`
  End-to-end compiler tests with expected runtime result, stdout, HIR, and LLVM
  IR.
- `tests/errors`
  Structured error-rendering tests in JSON form.
- `tests/hir`
  Focused HIR snapshot tests. These are the middle-layer textual comparison
  target that replaced the historical custom IR expectations.
- `tests/llvm`
  Focused LLVM IR snapshot tests. These are the backend textual comparison
  target that replaced historical generated C expectations.
- `tests/format`
  Formatter snapshot tests.
- `tests/lsp`
  LSP transcript-style tests.
- `tests/commands`
  Command-level regressions for public compiler subcommands.
- `tests/install`
  Source fixtures copied outside the repository by `build/test_install.py` for
  installed-compiler smoke checks.

## Language Tests

Language tests use `.t` files with five sections separated by `¬`:

1. source code
2. expected return value
3. expected stdout
4. expected HIR
5. expected generated LLVM IR

The first section is also valid source input by itself, which makes local
debugging straightforward.

Language tests should stay behavior-first. Include only enough HIR or LLVM text
to assert the compiler fact that matters for the case; the runner treats those
sections as ordered subsequences rather than requiring a full-file snapshot.

## HIR Tests

HIR tests use `.hir` files with two sections:

1. source code
2. expected generated HIR

Use these when the intent is the semantically checked middle layer rather than
runtime behavior. HIR is the current comparison target for middle-layer
expectations; do not add new tests against the retired custom IR.

## LLVM Tests

LLVM tests use `.ll` files with two sections:

1. source code
2. expected generated LLVM IR

Use these when the intent is backend text. LLVM IR is the current comparison
target for backend expectations; do not add new generated-C snapshots.

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

They may also include optional sections for run mode, extra CLI arguments,
command name, and expected stderr. The parser accepts three to seven sections.

The Python runner writes the source into a generated `.input.n` file next to the test
case and invokes the relevant compiler command against that file. This covers
behaviour outside the pure compiler pipeline, such as `run` resolving generated
binaries correctly when the input path is relative to the current directory.
Use command tests for public CLI behaviour such as `nerd check` exiting
successfully without generating an executable.

## Example Checks

The runner also treats source files under `examples/` that declare `main` as
example roots. Each root is checked with `nerd check`, so support modules without
their own entry point are covered through the roots that import them.

## Platform-Specific Tests

Tests that require a specific host platform can declare it in the source section:

```nerd
-- test-platform: linux
```

The runner skips the case when the current host platform is not listed. Use this
only for tests that exercise host APIs or toolchain behaviour that cannot be made
portable, such as POSIX-only FFI calls.

Linux command tests may use the `debug-info` and `no-debug-info` run modes to
inspect the kept executable with `readelf --debug-dump=decodedline`. These modes
cover the debug-build contract that normal builds embed Nerd line tables and the
release-build contract that the current release path omits them.

Linux debugger smoke scripts cover the VS Code/CodeLLDB-facing debugger
contract:

```sh
python3 build/check_debugger_adapter_transforms.py
python3 build/check_debugger_smoke.py --nerd _bin/nerd-debug
python3 build/check_debugger_stepping.py --nerd _bin/nerd-debug
```

Use the adapter transform check for TypeScript value rendering and watch
translation changes. Use the smoke and stepping checks for source breakpoints,
locals, parameters, globals, watches, and source stepping against CodeLLDB's
bundled LLDB.

## Artefact Behaviour

Language, HIR, LLVM, and command tests write temporary `.input.n`, `.hir`,
`.ll`, `.link.ll`, executable, and related sidecar artefacts beside the test
case. Successful runs remove them. Failed runs retain generated files where
possible so the failure can be inspected locally.

`build/test_install.py` also checks successful external `run` and `test`
commands for leftover executables, link inputs, runtime object copies, and
unrequested LLVM sidecars.

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
