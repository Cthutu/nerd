# Statements

Statement parsing lives in `ast_parse_block_statement` in
`src/compiler/ast/parse.c`. Semantic checks for assignments, loop control,
definite assignment, and unused locals are in `src/compiler/sema/sema.c`.

## Block Statements

```bnf
statement ::= pragma
            | ffi-declaration
            | use-declaration
            | 'defer' statement
            | assert-statement
            | 'break' [ label ] [ expression ]
            | 'break' [ label ] 'on' expression '=>' expression
            | 'again' [ label ]
            | 'return' [ expression ]
            | destructure-statement
            | block
            | for-statement
            | binding
            | variable
            | expression
```

Statements do not require semicolons. Statement boundaries are inferred from the
token stream and enclosing parser context. Newlines are not general separators,
but they can act as boundary hints where an expression would otherwise consume
the start of the next statement or `on` branch.

## Bindings And Variables

```bnf
binding  ::= IDENT '::' bind-value
           | IDENT ':' type ':' bind-value

variable ::= IDENT ':=' expression
           | IDENT ':' type [ '=' expression-or-undefined ]
```

Bindings introduce constants, functions, FFI functions, module references, or
type aliases. Variables introduce mutable storage. A typed variable without an
initializer is zero-initialised.

`undefined` is only valid where the parser builds an explicitly typed variable
initializer. Semantic definite-assignment checks require it to be assigned before
read.

## Unused And Discard Names

Function-local runtime values are expected to be read. This applies to local
variables, parameters, local bindings, and pattern binders.

Names beginning with `_` mark a local as deliberately unused:

```nerd
helper :: fn (_unused: i32) -> i32 {
    _scratch := 10
    return 7
}
```

The bare name `_` is the discard binding. It can be used more than once in the
same scope and is lowered as a fresh internal discard name each time:

```nerd
_ := compute()
_ := compute_again()
```

Assignments do not count as reads. Reading a leading-underscore local is an
error; rename it without the leading `_` before using it as an ordinary value.

## Destructuring

Destructuring statements support constant binding, mutable binding, typed
binding, and assignment:

```nerd
(x, y) :: pair
(x, y) := pair
(x, y): Pair : pair
(x, y) = pair
{ name, age } := person
```

Tuple and plex destructuring shape is checked against the source expression
type. `_` ignores a value.

## Defer And Assert

`defer` stores a statement to run when leaving the current scope. Deferred
statements still undergo the same semantic validity checks as if they appeared
normally.

```bnf
assert-statement ::= 'assert' expression [ ',' expression ]
```

The first expression is the assertion condition and must type as `bool`. The
optional expression after the comma is the assertion message and must type as
`string`.

```nerd
assert count > 0
assert count > 0, "count must be positive"
assert count > 0, $"count was {count}"
```

An assertion statement has type `void`. If the condition is false at runtime,
the runtime assertion path reports the source location and message. When no
message is supplied, the compiler uses the default assertion message.
