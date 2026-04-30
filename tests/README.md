# Test Suite

This directory contains the repository regression tests. The full gate is:

```sh
just test
```

The implementation notes for the runner live in
[../docs/testing.md](/home/matt/nerd/docs/testing.md). This file is the quick
index for the test inputs.

## Test Families

- `language`
  End-to-end source tests. `.t` files contain source, expected process return
  value, expected stdout, expected IR, and expected generated C, separated by
  `¬`.
- `errors`
  Structured compiler diagnostic tests. `.e` files contain source and expected
  JSON diagnostic output, separated by `¬`.
- `format`
  Formatter snapshot tests. `.f` files contain input and expected formatted
  output.
- `lsp`
  Language-server transcript tests.
- `commands`
  Public command regressions that exercise the real command path.
- `mods`
  Source modules used by module-related tests.

Generated artefacts are removed when tests pass and retained when tests fail, so
failed cases can be inspected locally.
