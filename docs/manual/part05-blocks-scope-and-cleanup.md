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
    {
        value := 2
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
    value := ${
        break 42
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
            break $outer 42
        }
        break $outer 0
    }
    return value
}
```

Use labels when a `break` target would otherwise be unclear.

## `defer`

`defer` schedules a statement to run when the current scope exits.

```nerd
use std.io

main :: fn () {
    defer prn("leaving main")
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
defer statement
```

or:

```nerd
defer {
    statement_one()
    statement_two()
}
```

## Defer Order

Defers run in last-in, first-out order:

```nerd
use std.io

main :: fn () {
    defer prn("first")
    defer prn("second")
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
        defer prn("leaving block")
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
    defer names.free()

    names.push("north")
    names.push("south")
    prn($"{names[0]} {names[1]}")
}
```

This example uses a dynamic array, which is introduced in Part 9. The important
point here is the cleanup shape: write the cleanup next to the acquisition.
Later `return`, `break`, or `continue` paths still run it.
