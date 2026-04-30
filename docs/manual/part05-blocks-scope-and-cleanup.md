# Part 5: Blocks, Scope, And Cleanup

[Manual Index](README.md) | Previous: [Functions](part04-functions.md) | Next: [Branching With `on`](part06-branching-with-on.md)

Blocks group statements. A statement is a piece of code that performs an action,
such as creating a local binding, assigning a value, or returning from a
function. Blocks also introduce scope and define where deferred cleanup runs.

## Nested Blocks

Use braces to create a nested scope:

```nerd
main :: fn () -> i32 {
    value := 1
    {           -- nested block starts a new scope
        value := 2  -- different local from the outer value
    }
    return value
}
```

The inner `value` is a different local. It exists only inside the inner block.

## Expression Blocks

An ordinary block groups statements. An expression block starts with `$` and can
produce a value by using `break <expr>`:

```nerd
main :: fn () -> i32 {
    value := ${  -- expression block produces a value
        break 42 -- leave the block with value 42
    }
    return value
}
```

This is useful when computing a value needs several statements.

## Labels

Labels disambiguate nested expression blocks. A labelled expression block starts
with `$name`, and labelled breaks name the target with the same `$name`:

```nerd
main :: fn () -> i32 {
    value := $outer {
        $inner {
            break $outer 42  -- leave the outer expression block
        }
        break $outer 0
    }
    return value
}
```

Use labels when a `break` target would otherwise be unclear.

## `assert`

`assert` checks that a condition is true at runtime:

```nerd
main :: fn () {
    value := 42
    assert value > 0  -- fail if value is not positive
}
```

The condition must have type `bool`. If the condition is false, the program
prints an assertion failure with the source location and stops.

An assertion can include a string literal message:

```nerd
assert value < 100, "value must stay below 100"  -- print message on failure
```

Use assertions for rules that should always hold if the program is correct.
They are not a substitute for normal input validation or recoverable error
handling.

## `defer`

`defer` schedules a statement to run when the current scope exits.

```nerd
use std.io

main :: fn () {
    defer prn("leaving main")  -- run when main exits
    prn("inside main")
}
```

Output:

```text
inside main
leaving main
```

The syntax is either:

```nerd
defer statement  -- run statement when this scope exits
```

or:

```nerd
defer {
    statement_one()  -- first deferred statement
    statement_two()  -- second deferred statement
}
```

## Defer Order

Defers run in last-in, first-out order:

```nerd
use std.io

main :: fn () {
    defer prn("first")   -- runs second
    defer prn("second")  -- runs first
    prn("body")
}
```

Output:

```text
body
second
first
```

## Scope Exit

Deferred statements run when the current scope exits naturally, by `return`, by
`break`, or by `continue`.

```nerd
use std.io

main :: fn () {
    {
        defer prn("leaving block")  -- runs at the closing brace
        prn("inside block")
    }
    prn("after block")
}
```

Output:

```text
inside block
leaving block
after block
```

## Cleanup Pattern

Use `defer` when a scope owns a resource:

```nerd
use std.io

main :: fn () {
    names: [..]string
    defer names.free()  -- release owned storage on scope exit

    names.push("north")
    names.push("south")
    prn($"{names[0]} {names[1]}")
}
```

This example uses a dynamic array, which is introduced in Part 9. The important
point here is the cleanup shape: write the cleanup next to the acquisition.
Later `return`, `break`, or `continue` paths still run it.
