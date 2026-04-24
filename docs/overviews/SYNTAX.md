# Nerd Syntax

This is a short reference for the currently implemented syntax surface, not the
full intended language.

## Comments

```nerd
-- This is a single-line comment
```

## Top-Level Bindings

```nerd
answer :: 42
main :: fn () => answer
typed_value: i32: 42
```

`::` binds a name to a value or type-level entity. `name: type: value` is an
annotated binding.

## Functions

Expression-bodied functions:

```nerd
main :: fn () => 1 + 2
```

Block-bodied functions:

```nerd
main :: fn () {
    return 1 + 2
}
```

## Current Core Expressions

- integer literals
- floating-point literals
- boolean literals
- string literals
- interpolated strings with `$"...{expr}..."`
- symbol references
- unary `-` and `!`
- arithmetic operators `+`, `-`, `*`, `/`, `%`
- comparison operators `<`, `<=`, `>`, `>=`, `==`, `!=`
- bitwise operators `&`, `^`, `|`
- logical operators `&&`, `||`
- block-form `on` branch binders with `<name> @ <pattern>` and
  `<name> @ else`
- calls with zero or more arguments

## Current Grammar Sketch

```text
<program> ::= <declaration>*

<declaration> ::= <binding>
                | <annotated-binding>

<binding> ::= <symbol> "::" <expression-or-type>
<annotated-binding> ::= <symbol> ":" <type> ":" <expression>

<expression-or-type> ::= <expression> | <type>
<type> ::= "fn" "(" ")" "->" <type>
<expression> ::= <integer>
               | <string>
               | <interpolated-string>
               | <symbol>
               | "fn" "(" ")" "=>" <expression>
               | "fn" "(" ")" "{" <statement>* "}"
```

This file should stay small. Detailed language design notes live elsewhere.
