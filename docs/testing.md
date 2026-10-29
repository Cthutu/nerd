# Testing

`just test` runs the unified correctness gate through
[`tests/run.py`](../tests/run.py). The shared catalogue is
[`tests/harness/plan.py`](../tests/harness/plan.py); it also supplies correctness
commands to native Windows validation. Specialised runners retain their existing
assertions and can still be invoked directly. The fixture runner remains
[`build/test.py`](../build/test.py).

## Live dashboard

In an interactive terminal, the runner uses the full screen to show shared,
debug and release progress, gate status, the current fixture, elapsed time and
live output. Successful runs close the dashboard automatically. Failed runs
finish their independent checks, then hold the failure view until Enter or q.
A failed compiler build blocks only its dependents; blocked checks are never
reported as passing.

| Key | Action |
| --- | --- |
| Up/Down or k/j | Select a gate |
| f | Show failed/blocked gates only |
| g | Follow the running gate |
| PageUp/PageDown | Scroll the selected gate's details |
| l | Toggle failure details and the complete gate log |
| End | Return to the log tail |
| q, Escape, Ctrl-C while running | Cancel the run and terminate test subprocesses |
| Enter or q after failure | Close the dashboard and return a failure status |

Resizing is supported. The dashboard uses Python's standard library and ANSI
terminal control, with no additional UI dependencies. Non-terminal output,
`CI` environments and `--plain` use streaming text and **never wait for input**.
`--no-pause` also suppresses failure acknowledgement in a terminal.

## Selecting tests

```sh
just test                         # Shared checks and both compiler profiles
just test-debug                   # Debug compiler only
just test-release                 # Release compiler only
just test-stdlib                  # Standard-library source tests, both profiles
just test --suite language errors # Named suites plus compiler builds
just test --filter maths          # Matching fixtures, both compiler profiles
just test --only cgen-debug       # One gate and its prerequisites
just test --list                  # Commands without executing anything
just test --list-suites
just test --plain --no-pause       # Explicit unattended mode
```

Filters select fixture paths and imply fixture-only execution unless a suite or
gate selection is also supplied. An empty/unknown selection fails rather than
reporting success with zero tests. Build gates normally run first; `--skip-build`
explicitly reuses existing binaries and fails if a required binary is absent.
`--timeout SECONDS` changes the default 1,200-second deadline per gate.

The default runs cleanup/allocator/thread and harness checks, builds each
compiler, then runs language, diagnostic, semantic, HIR/LLVM, formatter, LSP,
command, stdlib, example and teaching fixtures, plus native capability/frame,
library lifecycle, profiling/scheduling, toolchain, installation, C-output,
build-settings and stdio gates. Both compiler profiles get the same correctness
gates. Gates run serially to avoid sharing fixture artefacts or interfering with
profiling/scheduling checks. Specialised suites may retain their own bounded
internal parallel work.

Network and Nexus remain opt-in, with LLVM/C and debug/release target programs:

```sh
just test --network --nexus
just test --suite network         # Focus the network matrix only
```

The existing `just test-network` and `just test-nexus` commands retain their
focused runner flags. Installed editor/Neovim and CodeLLDB checks are separately
selected with `--editors` and `--debugger` and require their normal native tools
and editor dependencies. Performance benchmarks and interactive native desktop
validation remain separate from correctness tests.

## Logs, results and cancellation

Each run saves readable gate logs and an atomically updated `summary.json` under
`_test-results/<run>/`. The summary records the starting Git revision/dirty flag,
commands, durations and outcomes. `--output PATH` selects a new results directory. Failed
fixture artefacts continue to stay beside their fixtures. Generated evidence is
ignored and remains until `just clean` removes it.

Stage exit codes decide success; fixture events provide exact live progress and
failure details. Negative tests which reject invalid input are positive results;
Raptor's expected assertion rejections are labelled as passing checks.

The harness returns 0 on success, 1 for failed/blocked tests, 2 for invalid
selections/options, and 130 on cancellation. Cancellation and gate deadlines
terminate process trees and restore the terminal. Linux also reaps orphaned
children that started new sessions; Windows uses native process-tree termination.
Native Windows terminal behaviour requires validation on that host.

`just do` runs `clean`, the unified `test` gate once, then `install`. It does not
repeat release tests. Use native Clang/LLVM and SDK/CRT tools for each host; see
[toolchain setup](toolchain.md). WSL uses Linux tools, not Windows executables.

The underlying fixture runner remains useful for focused tooling work:

```sh
python3 build/test.py --filter maths --nerd _bin/nerd-debug
python3 build/test.py --kind format
```

Its `--events` mode emits structured case progress for the dashboard. Python
subprocess output is decoded as UTF-8 on every host, including Windows. The
Windows installation smoke check requires `llvm-dwarfdump` on PATH; keep LLVM
tools outside the checkout so cleaning does not remove them.

`nerd test <root-filename>` runs tests declared in Nerd source, rather than the
repository harness. For example, `just run nerd test mods/std/maths/mod.n` runs
only the maths source tests.

After installation, editor/compiler smoke checks remain available:

```sh
python3 build/test_install.py --nerd _bin/nerd-debug
python3 build/check_editor_integrations.py --nerd _bin/nerd-debug
```

## Test Families

The regression families are indexed by [feature and editor operation](test-features.md):

- `tests/language`
  End-to-end compiler tests with expected runtime result, stdout, HIR, and LLVM
  IR.
- `tests/sema`
  Named positive semantic programs, checked without linking external fixtures.
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

Most ordinary scenarios share a feature program with labelled `prn` output and
result assertions. Each label and its results occur in the exact stdout oracle,
so a missing or skipped scenario fails the test.

Programs that require independent namespaces, entry points, imports, locations,
or trait implementations are separated by a line `=== case: scenario-name ===`.
Each block has the same five sections and may add a sixth section for stdin.
The runner compiles each block independently, preserving its declarations and
`main`; failures include the scenario name. Do not rename fields, methods or
canonical traits simply to combine programs. A complete source comment line
`-- test-release` preserves cases that specifically require target optimisation.

The C differential runner discovers every language block, including all migrated
command cases. Legacy backend failures discovered during expansion are tracked
and fixed with regressions; they are not silently excluded from the gate.

Language tests should stay behavior-first. Include only enough HIR or LLVM text
to assert the compiler fact that matters for the case; the runner treats those
sections as ordered subsequences rather than requiring a full-file snapshot.

## Positive semantic tests

`.check` files under `tests/sema` contain `=== case: scenario-name ===` source
blocks. Each must pass `nerd check` without diagnostics. These retain ordinary
positive type-checking coverage moved from `.cmd`, including foreign libraries
which intentionally do not exist for linking. Negative semantic programs belong
in feature-grouped `.e` files with located JSON diagnostics.

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
For `run` command tests, extra CLI arguments after `--` are passed through to
the generated program.
The `build-artifact` run mode checks that a `build` command produced the
expected executable, object, static library, or shared library artifact.

The Python runner writes the source into a generated `.input.n` file next to the test
case and invokes the relevant compiler command against that file. This covers
behaviour outside the pure compiler pipeline, such as `run` resolving generated
binaries correctly when the input path is relative to the current directory.
Use command tests for public CLI behaviour such as `nerd check` exiting
successfully without generating an executable.

## Standard Library Source Tests

The runner treats source files under `mods/std/` that declare source tests as
standard-library test roots. Each root is run with `nerd test`, so standard
library implementation coverage should live next to the code in `test "name" {
... }` sections and test-only `test { ... }` declaration blocks.

Use command tests for the public `nerd test` command itself, not for ordinary
standard-library unit coverage.

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

Test-only declarations can be placed in a bare `test` block. They are ignored
by normal builds and are compiled only while running source tests:

```nerd
test {
    make_answer :: fn () -> i32 {
        return 42
    }
}

test "answer" {
    assert make_answer() == 42
}
```

`test` declarations and declarations inside `test { ... }` cannot be marked
`pub`.

Tests may be placed inside compile-time define blocks. The test command
discovers tests only from enabled blocks, using the host platform and any
`-Dname` keys supplied on the command line:

```nerd
on "linux" {
    test "Linux behaviour" {
        assert yes
    }
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

The test command collects source tests from the root file and from imported
modules. Tests in imported modules run in that module's scope, so they can
exercise private helpers. Imported test names are prefixed with the qualified
module name, for example `test.math: edge cases`.

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

## Variadic C Interoperability

`tests/commands/297-build-variadic-host.cmd` builds a Nerd shared library and
links `tests/ffi/variadic_host.c` against it. The harness exercises C promotions,
register and stack arguments, borrowed/copy cursors, early returns, formatting,
plain export aliases, re-exports, and names formerly used by runtime helpers.
It also checks the defined dynamic symbol table for unexpected exports.
`build-variadic-host` is the command-test run mode for this fixture.

Additional command, diagnostic, HIR, LLVM, formatter, and LSP fixtures cover
variadic function pointers, C `va_list` forwarding, cleanup, forbidden cursor
escapes, and nested callback signatures. End-to-end host-DLL coverage currently
runs on Linux; Windows export alias IR can be compiled with clang's COFF target,
but that is not a substitute for executing the tests on a Windows host.


## Library capabilities and opt-in integrations

`just test` runs `tests/stdlib-capabilities/check_capabilities.py` against the checkout's
modules. It checks single-thread atomic operations and layout, explicit callback
context, payload aliasing and graph-storage building blocks. Native C layout
comparison and debug/release execution are part of the probe; `--cgen` selects
the C path. These probes do not prove concurrent queue correctness, ownership
transfer, reclamation or graph evaluation. Use the Raptor, thread/sync, network
and Nexus runners for their respective runtime contracts.

Network and Nexus example integrations are opt-in and are excluded from default
example discovery and both common test recipes:

```sh
just test-network
just test-network --cgen --release
just test-nexus
just test-nexus --cgen --release
```

The recipes build the debug compiler. To select the release compiler explicitly,
use `python3 build/test_network.py --compiler _bin/nerd --release` or the
corresponding `build/test_nexus.py` command. Default gates retain stdlib source
unit tests, Raptor, native thread/sync contracts and compiler/toolchain checks.

`build/test_harness.py` checks grouped-program isolation, stdin, later formatter
and LSP failures, truncated groups and release metadata. Both common gates run
it. The [feature inventory](test-features.md) records taxonomy, teaching-doc gaps
and the snapshot review. Historical audits and raw validation outputs are
available through immutable archive links, not regenerated tracked reports.
