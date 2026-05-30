# Tests

The test harness and source-level test syntax are documented in
`docs/testing.md` and implemented by the compiler command/test runner. Parser
support for source tests is in `ast_parse_test_decl` in
`src/compiler/ast/parse.c`.

## Source Tests

```bnf
source-test ::= 'test' STRING block
test-decls  ::= 'test' block
```

Source tests and test-only declaration blocks are top-level private
declarations. They cannot be marked `pub`, and declarations inside `test { ...
}` cannot be marked `pub`. The parser recognises `test "name" { ... }` and
`test { ... }` specially only when `test` is not followed by `:`, so ordinary
bindings named `test` remain possible.

Source test bodies and test-only declaration blocks are token-balanced by the
parser and are used by the test command rather than emitted as normal top-level
bindings. In normal builds, `test { ... }` declarations are ignored. During
`nerd test`, the wrapper is removed and the contained declarations are compiled
in the module where they appear.

## Check Command

`nerd check file.n` runs the whole-program front end through lexing, parsing,
module loading, and semantic analysis. It must not generate HIR, LLVM IR, or an
executable. It uses the same import resolution, platform keys, `-Dname`
keywords, release/debug selection, and entry-point validation as normal program
compilation.

## Harness File Types

| Family              | Purpose                                                               |
| ------------------- | --------------------------------------------------------------------- |
| `.t` language tests | Source, expected output, HIR/LLVM expectations depending on sections. |
| `.hir` tests        | HIR-specific expectations.                                            |
| `.ll` tests         | LLVM-specific expectations.                                           |
| `.e` tests          | Expected diagnostics.                                                 |
| `.f` tests          | Formatter input/output checks.                                        |
| `.lsp` tests        | Transcript-style LSP request/response checks.                         |
| `.cmd` tests        | Public command-level regression tests.                                |

Use the closest family to the behaviour being specified. Language syntax and
semantic guarantees usually belong in `.t` or `.e`; editor behaviour belongs in
`.lsp`; formatting belongs in `.f`.
