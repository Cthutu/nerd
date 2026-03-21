# Test Plan

This document records the intended design for the `nerd test` command, so the
implementation can proceed in staged changes without losing context.

## Goals

- Add a `test` sub-command that runs the compiler tests in the `tests` folder.
- Implement language tests first.
- Reuse the normal compiler pipeline rather than creating a separate test-only
  code path.
- Keep generated artifacts available when a test fails, but clean them up when
  tests pass.

## Test Types

There are two planned test categories:

- `tests/language/*.t`
  Language tests that should compile and run successfully.
- `tests/errors/*.e`
  Error tests that should produce compiler errors in JSON form.

The first implementation phase will only cover `tests/language/*.t`.

## Language Test Format

Each `.t` file contains a single test case split into five sections by the `¬`
character:

1. Source code to compile and run.
2. Expected return value.
3. Expected program output.
4. Expected IR output in human-readable form.
5. Expected generated C code.

Notes:

- The compiler naturally stops compiling at the `¬` character, so the first
  section can be passed through the lexer as normal source text.
- Empty IR or C sections are valid and are used as an authoring aid. In that
  case the runner should print the generated output so it can be copied into the
  test file later.

## High-Level Architecture

The test runner should be built on top of the existing compiler pipeline:

- Parse source with `front_end()`.
- Generate code with `back_end()`.
- Avoid invoking `nerd build` as a subprocess.
- Share the same core code paths used by the normal `build` command.

This requires the back-end to be driven by configuration rather than hard-coded
output names.

## Planned Refactors

### 1. Backend output configuration

The backend should be configuration-driven, so callers can provide explicit
artifact/output paths instead of relying on fixed names.

Planned direction:

- Introduce a backend/build configuration structure that controls:
  - output stem or output filename
  - whether generated C is written to disk
  - whether IR is written to disk
  - whether generated C is compiled to an executable
- Use the same mechanism for both:
  - `nerd build`
  - `nerd test`

### 2. Build flags

The `build` command should later support:

- `--ir`
  Write a `.ir` file next to the source stem.
- `--cgen`
  Write a `.c` file next to the source stem.

These should be thin CLI options over the same artifact-generation path used by
the test runner.

### 3. Stable text rendering

IR and generated C should have stable string renderers, separate from pretty
terminal dump functions.

Planned helpers:

- `ir_render(...)`
- `cgen_render(...)`

These helpers should return strings that can be:

- compared against expected test output
- written to `.ir` / `.c` files
- shown directly when a test leaves those sections blank

The existing dump functions can remain for interactive debugging.

### 4. Shell process capture

The shell API should be extended, so tests can execute generated programs and
capture:

- exit code
- standard output
- standard error

This will be used first by language tests and later by error tests.

### 5. Diff output

A dedicated diff module should be added to compare expected and actual text and
show mismatches in a clear colourful terminal view.

This will be used for:

- program output diffs
- IR diffs
- C diffs
- later, error JSON diffs

The first implementation can be simple as long as it is readable.

## Language Test Runner Behaviour

For each file in `tests/language/*.t`:

1. Load the file.
2. Split it into exactly five sections on `¬`.
3. Parse those sections into a `LanguageTest` structure.
4. Run the compiler front end on section 1.
5. Run the compiler back end using configurable output paths.
6. Execute the generated program and capture return code, stdout, and stderr.
7. Compare:
   - return code
   - stdout
   - rendered IR
   - rendered C
8. Report pass/fail clearly.

Important comparison rules:

- Stdout comparison should be exact, including trailing newline handling.
- Empty expected IR or C sections should not be treated as a normal pass. The
  generated output should be shown, so the test can be completed.

## Data Structures

The language runner should parse tests into a dedicated structure, for example:

```c
typedef struct {
    string path;
    string source;
    string expected_return_value;
    string expected_stdout;
    string expected_ir;
    string expected_c;
} LanguageTest;
```

Exact field names may change, but the structure should retain one field per test
section plus the source path for reporting and artifact naming.

## Artifact Naming

Generated artifact names should come from the original source file stem.

Examples:

- `foo.n` -> `foo.ir`
- `foo.n` -> `foo.c`

For test files, artifacts should use the `.t` file stem:

- `tests/language/001-number.t` -> `tests/language/001-number.ir`
- `tests/language/001-number.t` -> `tests/language/001-number.c`

The executable should use the same stem without an extension where practical.

## Clean-up Policy

Before running the test suite:

- Remove stale generated `.ir` files under `tests/`
- Remove stale generated `.c` files under `tests/`
- Remove stale test executables and temporary outputs under `tests/`

After each test:

- If the test passes, remove:
  - generated `.ir`
  - generated `.c`
  - executable
  - any intermediate artifacts
- If the test fails, keep those files, so the failure can be inspected locally

This gives the following workflow:

- failing tests leave useful artifacts behind
- fixing the issue and rerunning tests causes those artifacts to be cleaned up
  naturally once the test passes

## Git Hygiene

Generated files in the `tests` folder should not be committed accidentally.

Planned repo hygiene changes:

- add ignore rules for generated test artifacts under `tests/` only
- do not add broad ignore rules that hide real source files

## Staged Implementation Plan

### Phase 1

- Refactor backend output handling, so file names are configuration-driven
- Add stable IR and C render helpers
- Extend shell API to capture stdout/stderr/exit code
- Add a diff module for readable text mismatches
- Implement language test discovery, parsing, execution, comparison, and clean-up

### Phase 2

- Add `build --ir`
- Add `build --cgen`
- Reuse the same artifact machinery as the test runner

### Phase 3

- Implement `tests/errors/*.e`
- Reuse the same comparison and reporting framework

## Open Notes

- `tests/README.md` currently refers to `tests/error`, while the actual folder is
  `tests/errors`. This should be made consistent during implementation.
- The first phase should prioritize correctness and clarity of failure output
  over advanced optimisation.
