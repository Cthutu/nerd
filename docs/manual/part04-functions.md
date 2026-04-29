# Part 4: Functions

[Manual Index](README.md) | Previous: [Primitive Types And Expressions](part03-primitive-types-and-expressions.md) | Next: [Blocks, Scope, And Cleanup](part05-blocks-scope-and-cleanup.md)

Functions are introduced with `fn`. Bindings give functions names:

```nerd
name :: fn () {
}
```

## Parameters

Parameters are written inside parentheses:

```nerd
add :: fn (left: i32, right: i32) -> i32 {
    return left + right
}
```

Run a function by writing its name followed by parentheses. Values passed inside
the parentheses become the function's arguments:

```nerd
main :: fn () -> i32 {
    return add(20, 22)
}
```

Here `20` becomes `left`, and `22` becomes `right`.

## Return Types

Block functions that return values use `-> Type`:

```nerd
is_large :: fn (value: i32) -> bool {
    return value > 100
}
```

`void` functions can omit a final return:

```nerd
use std.io

show :: fn (value: i32) -> void {
    prn($"value={value}")
}
```

## Expression-Bodied Functions

Use `=>` when the whole function is one expression:

```nerd
triple :: fn (value: i32) => value * 3
```

Expression-bodied functions infer their return type from the expression. They
are useful for small helpers.

## Function Types

Function types are written with parameter types and a return type:

```nerd
operation: fn (i32, i32) -> i32 = add
```

A function type describes a function value. This means a function can receive
another function as an argument:

```nerd
apply :: fn (f: fn (i32) -> i32, value: i32) -> i32 {
    return f(value)
}

double :: fn (value: i32) => value * 2

main :: fn () -> i32 {
    return apply(double, 21)
}
```

## Nested Functions

Functions can be declared inside block bodies:

```nerd
main :: fn () -> i32 {
    helper :: fn () -> i32 {
        return 42
    }

    return helper()
}
```

Nested functions are useful for local helpers that do not belong at the top
level.

## Forward References

Top-level functions can refer to functions declared later:

```nerd
main :: fn () -> i32 {
    return answer()
}

answer :: fn () -> i32 {
    return 42
}
```

This keeps source order flexible. Use it deliberately; readers still benefit
from nearby definitions when functions are tightly related.
