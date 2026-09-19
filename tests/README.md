# Test Suite

This directory contains the repository regression tests. The full gate is:

```sh
just test
```

The implementation notes for the runner live in
[../docs/testing.md](/home/matt/nerd/docs/testing.md). This file is the quick
index for the test inputs.

`nerd test <root-filename>` is for source-level unit tests written in Nerd
source files. Repository regression tests are run by the Python harness via
`just test`.

`just test` also runs `build/test_clean.py`, which exercises the cleanup recipe
in a temporary workspace. Cleanup removes generated `.host.c` and `.input.c`
files while preserving C source fixtures, including `tests/ffi/variadic_host.c`.

## Test Families

- `language`
  End-to-end source tests. `.t` files contain source, expected process return
  value, expected stdout, expected HIR, and expected LLVM IR, separated by
  `¬`.
- `errors`
  Structured compiler diagnostic tests. `.e` files contain source and expected
  JSON diagnostic output, separated by `¬`.
- `hir`
  Focused HIR snapshot tests. `.hir` files contain source and expected HIR,
  separated by `¬`.
- `llvm`
  Focused LLVM IR snapshot tests. `.ll` files contain source and expected LLVM
  IR, separated by `¬`.
- `format`
  Formatter snapshot tests. `.f` files contain input and expected formatted
  output.
- `lsp`
  Language-server transcript tests.
- `commands`
  Public command regressions that exercise the real command path.
- `stdlib`
  Source-level unit tests declared with `test` blocks inside `mods/std`.
- `examples`
  Real programs under `examples/` that declare `main`; the runner checks them
  with `nerd check`, which also checks their imported support modules.
- `install`
  Source fixtures used by `build/test_install.py` for installed-compiler smoke
  checks outside the repository tree.
- `mods`
  Source modules used by module-related tests.

Generated artefacts are removed when tests pass and retained when tests fail, so
failed cases can be inspected locally.

Use `-- test-platform: linux` in the source section for cases that intentionally
depend on host APIs unavailable on other platforms.

Debugger coverage is split between LLVM metadata snapshots, command tests with
`debug-info`/`no-debug-info`, and Linux CodeLLDB smoke scripts:

```sh
python3 build/check_debugger_adapter_transforms.py
python3 build/check_debugger_smoke.py --nerd _bin/nerd-debug
python3 build/check_debugger_stepping.py --nerd _bin/nerd-debug
```

## C backend differential tests

`python3 build/test_cgen.py --nerd _bin/nerd-debug` runs every language fixture,
selected command runtime regressions, and `tests/cgen/*.n`. Each program is
built with LLVM, then generated as C and compiled by Clang at `-O0` and `-O2`.
The runner compares exit status, stdout and stderr, forwarding fixture stdin.
It also checks output names, paths with spaces, inline source, argument forwarding,
invalid input, and generation without Clang on `PATH`. The pixels examples also
exercise `--cgen --copts` with default source discovery and standalone compilation
without requiring a display server. `just test` runs this suite.
Installation smoke tests move generated C out of its source directory and compile
it independently for debug and release runtime configurations.

Generated C is compiled with `-Werror` in the differential suite. On Linux, a
PTY regression also builds the actual dungeon example with a fixed seed, waits
for a complete frame before sending any input, compares that frame across LLVM
and C at both optimisation levels, and checks that Q exits successfully. Waiting
for the frame first distinguishes normal presentation from the shutdown flush.

C option tests execute `--copts` with no Clang on PATH, verify that existing
artifacts are preserved, and compile with its returned arguments. C hosts call
exports from generated objects, archives and shared libraries, covering imported
functions, variadic definitions and global initialisation.

`build/test_toolchain.py` checks direct LLVM binary/object/library output and
`nerd doctor`. On POSIX it restricts PATH to LLVM tools, traps any Clang use,
and checks each missing tool plus C generation without a binary toolchain.
It runs as part of `just test`.
