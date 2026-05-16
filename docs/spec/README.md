# Nerd Language Specs

This directory contains implementation-derived technical documentation for the
Nerd language. These documents describe what the compiler currently accepts and
checks, with source anchors into the lexer, parser, semantic analyser, module
loader, formatter, and test harness where relevant.

## Documents

| Document                                             | Scope                                                                                      |
| ---------------------------------------------------- | ------------------------------------------------------------------------------------------ |
| [`lexer.md`](lexer.md)                               | Token kinds, keyword spellings, literals, and interpolated string tokenisation.            |
| [`syntax.md`](syntax.md)                             | BNF-style grammar for parser-recognised source forms.                                      |
| [`types.md`](types.md)                               | Built-in types, type forms, aggregate types, and type-system rules.                         |
| [`expressions.md`](expressions.md)                   | Expression forms, precedence, calls, casts, and aggregate expressions.                     |
| [`statements.md`](statements.md)                     | Block statements, bindings, variables, destructuring, and control statements.              |
| [`control-flow.md`](control-flow.md)                 | All loops via `for`, all branches via `on`, labels, loop values, and exhaustiveness rules. |
| [`patterns.md`](patterns.md)                         | Pattern syntax and semantic matching rules.                                                |
| [`modules.md`](modules.md)                           | `use`, module resolution, exports, re-exports, and module parts.                           |
| [`ffi.md`](ffi.md)                                   | Foreign function declarations, supported signatures, and semantic restrictions.            |
| [`runtime-model.md`](runtime-model.md)               | Runtime representation, nil, zero initialisation, undefined, and dynamic arrays.           |
| [`diagnostics.md`](diagnostics.md)                   | Diagnostic phases and selected language error guarantees.                                  |
| [`tests.md`](tests.md)                               | Test formats and source-level `test` declarations.                                         |
| [`implementation-notes.md`](implementation-notes.md) | Contributor notes about implementation boundaries.                                         |

When code and manual disagree, preserve the code-derived fact in these specs and
flag the manual inconsistency in the relevant document or change summary.
