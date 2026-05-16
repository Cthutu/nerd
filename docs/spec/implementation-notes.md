# Implementation Notes

These notes are for contributors extending the language specs. They describe
where source-of-truth behaviour currently lives.

## Front End Boundaries

| Layer             | Main source                                        | Responsibility                                                      |
| ----------------- | -------------------------------------------------- | ------------------------------------------------------------------- |
| Lexer             | `src/compiler/lexer/lexer.c`, `lexer.h`            | Tokenisation, comments, literal decoding, interpolation token mode. |
| CST/formatter     | `src/compiler/cst`, `src/compiler/format`          | Concrete syntax and formatting preservation.                        |
| AST parser        | `src/compiler/ast/parse.c`, `expr.c`, `ast.h`      | Parser-recognised language forms and AST nodes.                     |
| Semantic analysis | `src/compiler/sema/sema.c`, `sema.h`               | Names, scopes, types, control-flow validation, constants.           |
| Modules           | `src/compiler/modules`, `src/compiler/build/front` | Module resolution, part expansion, imports, exports.                |
| HIR/LLVM          | `src/compiler/hir`, `src/compiler/llvm`            | Lowering and runtime representation.                                |

## Documentation Rule

For specs in this directory, prefer the implementation over the manual when they
disagree. Record the disagreement in the relevant spec or change summary, then
update the manual only when the user asks or the fix is clearly within scope.

## Parser Notes

The AST parser is not a pure context-free grammar:

- Expressions are Pratt parsed.
- Some tokens are only statement boundaries in specific parser modes.
- `::` and `:=` are parsed as adjacent tokens, not lexer-level combined tokens.
- Some syntactically accepted shapes are rejected later by semantic analysis.

Specs should distinguish parser acceptance from semantic validity when that
distinction matters.
