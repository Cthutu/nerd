# Installed Compiler Smoke Fixtures

These fixtures are copied to a temporary directory outside the repository by
`build/test_install.py`. They exercise the installed `nerd` command without
depending on test harness internals.

The smoke runner covers:

- `nerd build --hir --llvm`
- `nerd run`
- `nerd test`
- `nerd format --stdout`
- cleanup of temporary link inputs, runtime object copies, run executables, and
  unrequested LLVM sidecars after successful commands
