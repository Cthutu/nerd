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
type aliases. Variables introduce mutable storage. A local typed variable
without an initializer calls the canonical `core.Default` implementation for
its type when a concrete implementation exists; otherwise it is
zero-initialised.

`undefined` is only valid where the parser builds an explicitly typed variable
initializer. It opts out of both `Default` and zero-initialisation. Semantic
definite-assignment checks require it to be assigned before read.

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

## Return Statements

`return` exits the enclosing function. `return expression` is valid only when
the enclosing block-bodied function has an explicit `-> Type`, except that an
unannotated block-bodied `main` may infer an integer process status for
entry-point compatibility. Other block-bodied functions without `-> Type`
return `void`, and a value-bearing `return` is rejected.

Executable entry points may be either `main :: fn ()` or
`main :: fn (args: []string)`. In the argument form, `args[0]` is the
operating-system executable path and additional user arguments start at
`args[1]`.

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

## Assignment Targets

Assignment and compound assignment require mutable storage. Valid targets
include mutable locals, mutable module variables, fields or elements reached
through mutable storage, indexes into slices and dynamic arrays, and explicit
pointer dereferences.

Function parameters are immutable bindings. Assigning to a parameter name is
rejected, and assigning to fields or fixed-array elements stored directly inside
a by-value parameter is also rejected. Passing a pointer parameter is the normal
way to mutate caller-owned state:

```nerd
move :: fn (state: ^State) {
    state.player_loc = 1  -- writes through the pointer
}
```

The parameter binding itself remains immutable, so assigning a new pointer value
to `state` is still rejected.

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
