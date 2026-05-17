# Lexer Tokens

This document is generated from the current lexer implementation, not from the
manual. It records every `TokenKind` defined in
`src/compiler/lexer/lexer.h`, with spellings cross-checked against
`src/compiler/lexer/lexer.c` and `src/compiler/lexer/dump.c`.

## Notes

- `TK_EOF` is not emitted by `lex()`. It exists for parser/AST use.
- Whitespace is skipped.
- Line comments start with `--` and run to the next newline. They are skipped in
  normal lexing. In formatter mode they are stored as `LexerComment` records,
  not as tokens.
- The test-section separator `¬` stops lexing by truncating
  `lexer->source.source`; it is not emitted as a token.
- Identifiers start with an ASCII letter or `_`, then may contain ASCII
  letters, decimal digits, or `_`. If the identifier text matches a keyword, the
  keyword token is emitted instead of `TK_Symbol`.
- Numeric literal separators use `_`. Separators are accepted inside digits but
  not at the start, not doubled, and not at the end of the digit sequence.
- Single-quoted packed integer literals emit `TK_Integer`, not a distinct
  character-literal token.

## Interpolated Strings

Interpolated strings start with `$"` and emit `TK_InterpolatedStringStart`.
String continuations start with `+"` and emit `TK_StringContinuationStart`.
Both forms then use the same text/expression scanning path.

Text between interpolation expressions is decoded into ordinary `TK_String`
tokens and stored in `lexer->strings`. Empty text chunks are not emitted. Escape
sequences in text chunks use the same decoder as string and packed integer
literals: `\\`, `\n`, `\r`, `\t`, `\0`, `\a`, `\b`, `\f`, `\v`, and `\xNN`
are recognised, and `\"` produces a quote.

An unescaped `{` inside interpolated text ends the current text chunk, emits
`TK_LBrace`, and switches back to normal expression lexing. The expression body
uses ordinary Nerd tokens. Nested `{` and `}` tokens are counted so braces inside
the interpolation expression do not close the interpolation early.

When the brace depth returns to zero, the closing `}` is emitted as `TK_RBrace`
and the lexer resumes interpolated text scanning. A closing `"` in text mode
emits `TK_InterpolatedStringEnd`. If the source ends before that closing quote,
the lexer reports an unterminated string literal.

For example, `$"Hello {name}"` emits this token shape:

| Source fragment | Token kind |
| --- | --- |
| `$"` | `TK_InterpolatedStringStart` |
| `Hello ` | `TK_String` |
| `{` | `TK_LBrace` |
| `name` | `TK_Symbol` |
| `}` | `TK_RBrace` |
| `"` | `TK_InterpolatedStringEnd` |

## Literal And Identifier Tokens

| Token kind                   | Source spelling                                                                    | Payload                 | Notes                                                                                     |
| ---------------------------- | ---------------------------------------------------------------------------------- | ----------------------- | ----------------------------------------------------------------------------------------- |
| `TK_EOF`                     | none                                                                               | none                    | Parser sentinel; not emitted by `lex()`.                                                  |
| `TK_Integer`                 | Decimal digits, `0x` hex, `0b` binary, `0o` octal, or single-quoted packed integer | `lexer->integers`       | Decimal, prefixed-base, and packed integer literals all share this token.                 |
| `TK_Float`                   | Decimal digits followed by `.` and at least one decimal digit or `_`               | `lexer->floats`         | Only base-10 floats are recognised. No exponent form is currently lexed.                  |
| `TK_String`                  | `"..."` or an interpolated text chunk                                              | `lexer->strings`        | Escapes are decoded by the lexer. Interpolated strings emit string chunks as `TK_String`. |
| `TK_CString`                 | `c"..."`                                                                           | `lexer->strings`        | C string literal. Escapes are decoded by the lexer.                                       |
| `TK_InterpolatedStringStart` | `$"`                                                                               | none                    | Starts an interpolated string.                                                            |
| `TK_StringContinuationStart` | `+"`                                                                               | none                    | Starts an interpolated string continuation.                                               |
| `TK_InterpolatedStringEnd`   | `"`                                                                                | none                    | Ends an interpolated string or continuation.                                              |
| `TK_Symbol`                  | Identifier text                                                                    | `lexer->symbol_handles` | Emitted only when the identifier is not a keyword.                                        |

## Operator And Punctuation Tokens

| Token kind           | Source spelling | Notes                                                     |
| -------------------- | --------------- | --------------------------------------------------------- |
| `TK_Plus`            | `+`             |                                                           |
| `TK_PlusEqual`       | `+=`            |                                                           |
| `TK_Minus`           | `-`             |                                                           |
| `TK_MinusEqual`      | `-=`            |                                                           |
| `TK_Star`            | `*`             |                                                           |
| `TK_StarEqual`       | `*=`            |                                                           |
| `TK_Slash`           | `/`             |                                                           |
| `TK_SlashEqual`      | `/=`            |                                                           |
| `TK_Percent`         | `%`             |                                                           |
| `TK_PercentEqual`    | `%=`            |                                                           |
| `TK_LParen`          | `(`             |                                                           |
| `TK_RParen`          | `)`             |                                                           |
| `TK_LBracket`        | `[`             |                                                           |
| `TK_RBracket`        | `]`             |                                                           |
| `TK_Comma`           | `,`             |                                                           |
| `TK_Semicolon`       | `;`             |                                                           |
| `TK_LBrace`          | `{`             |                                                           |
| `TK_RBrace`          | `}`             |                                                           |
| `TK_Dot`             | `.`             |                                                           |
| `TK_At`              | `@`             |                                                           |
| `TK_Dollar`          | `$`             | `$"` is recognised first as `TK_InterpolatedStringStart`. |
| `TK_Hash`            | `#`             |                                                           |
| `TK_Ellipsis`        | `...`           | Recognised before `..` and `.`.                           |
| `TK_Range`           | `..`            |                                                           |
| `TK_RangeInclusive`  | `..=`           | Recognised before `..` and `.`.                           |
| `TK_Colon`           | `:`             | `::` is two `TK_Colon` tokens.                            |
| `TK_Equal`           | `=`             |                                                           |
| `TK_EqualEqual`      | `==`            |                                                           |
| `TK_Bang`            | `!`             |                                                           |
| `TK_BangEqual`       | `!=`            |                                                           |
| `TK_Amp`             | `&`             |                                                           |
| `TK_AmpEqual`        | `&=`            |                                                           |
| `TK_AmpAmp`          | `&&`            |                                                           |
| `TK_AmpAmpEqual`     | `&&=`           | Recognised before `&&` and `&=`.                          |
| `TK_Pipe`            | `|`             |                                                           |
| `TK_PipeEqual`       | `|=`            |                                                           |
| `TK_PipePipe`        | `||`            |                                                           |
| `TK_PipePipeEqual`   | `||=`           | Recognised before `||` and `|=`.                          |
| `TK_Caret`           | `^`             |                                                           |
| `TK_CaretEqual`      | `^=`            |                                                           |
| `TK_Less`            | `<`             |                                                           |
| `TK_LessEqual`       | `<=`            |                                                           |
| `TK_ShiftLeft`       | `<<`            |                                                           |
| `TK_ShiftLeftEqual`  | `<<=`           | Recognised before `<<` and `<=`.                          |
| `TK_Greater`         | `>`             |                                                           |
| `TK_GreaterEqual`    | `>=`            |                                                           |
| `TK_ShiftRight`      | `>>`            |                                                           |
| `TK_ShiftRightEqual` | `>>=`           | Recognised before `>>` and `>=`.                          |
| `TK_FatArrow`        | `=>`            |                                                           |
| `TK_ThinArrow`       | `->`            | `--` starts a comment and is recognised before `-`.       |

## Keyword Tokens

| Token kind     | Source spelling | Notes                          |
| -------------- | --------------- | ------------------------------ |
| `TK_fn`        | `fn`            |                                |
| `TK_for`       | `for`           |                                |
| `TK_on`        | `on`            |                                |
| `TK_else`      | `else`          |                                |
| `TK_defer`     | `defer`         |                                |
| `TK_assert`    | `assert`        |                                |
| `TK_break`     | `break`         |                                |
| `TK_again`    | `again`         |                                |
| `TK_return`    | `return`        |                                |
| `TK_plex`      | `plex`          |                                |
| `TK_union`     | `union`         |                                |
| `TK_enum`      | `enum`          |                                |
| `TK_ffi`       | `ffi`           |                                |
| `TK_intrinsic` | `intrinsic`     |                                |
| `TK_use`       | `use`           |                                |
| `TK_pub`       | `pub`           |                                |
| `TK_impl`      | `impl`          |                                |
| `TK_trait`     | `trait`         |                                |
| `TK_where`     | `where`         |                                |
| `TK_pragma`    | `pragma`        |                                |
| `TK_with`      | `with`          |                                |
| `TK_in`        | `in`            |                                |
| `TK_as`        | `as`            |                                |
| `TK_yes`       | `yes`           | Boolean literal token.         |
| `TK_no`        | `no`            | Boolean literal token.         |
| `TK_nil`       | `nil`           | Nil literal token.             |
| `TK_undefined` | `undefined`     | Undefined-value literal token. |
