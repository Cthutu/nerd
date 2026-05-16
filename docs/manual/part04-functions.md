# Part 4: Functions

[Manual Index](README.md) | Previous: [Primitive Types And Expressions](part03-primitive-types-and-expressions.md) | Next: [Blocks, Scope, And Cleanup](part05-blocks-scope-and-cleanup.md)

Functions are introduced with `fn`. Bindings give functions names:

```nerd
name :: fn () {  -- bind name to a function with no parameters
}
```

## Parameters

Parameters are written inside parentheses:

```nerd
add :: fn (left: i32, right: i32) -> i32 {
    return left + right  -- return a value matching -> i32
}
```

Run a function by writing its name followed by parentheses. Values passed inside
the parentheses become the function's arguments:

```nerd
main :: fn () -> i32 {
    return add(20, 22)  -- pass 20 as left and 22 as right
}
```

Here `20` becomes `left`, and `22` becomes `right`.

## Default Parameters

Trailing parameters can have default values:

```nerd
add :: fn (left: i32, right: i32 = 1) -> i32 {
    return left + right  -- right is 1 when the call omits it
}

main :: fn () -> i32 {
    return add(20)  -- same as add(20, 1)
}
```

All parameters after the first defaulted parameter must also have defaults. A
default expression is checked against the parameter type, and it can use
parameters that appear earlier in the signature:

```nerd
grow :: fn (base: i32, amount: i32 = base + 1) => base + amount
```

Defaults are evaluated at the call site. They belong to the function
declaration, not to the function type. A direct function-value alias keeps the
definition's defaults when the compiler can still trace it back to that
definition:

```nerd
add_one := add
return add_one(20)  -- same as add(20)
```

Arbitrary function-typed values still use the full function type when no
specific defaulted definition is known.

FFI declarations cannot have default parameters. Wrap an FFI function in a
normal Nerd function when a default is useful.

## Return Types

Block functions that return values use `-> Type`:

```nerd
is_large :: fn (value: i32) -> bool {
    return value > 100  -- return a bool value
}
```

`void` functions can omit a final return:

```nerd
use std.io

show :: fn (value: i32) -> void {
    prn($"value={value}")  -- no return value is needed
}
```

## Expression-Bodied Functions

Use `=>` when the whole function is one expression:

```nerd
triple :: fn (value: i32) => value * 3  -- expression body
```

Expression-bodied functions infer their return type from the expression. They
are useful for small helpers.

## Function Types

Function types are written with parameter types and a return type:

```nerd
operation: fn (i32, i32) -> i32 = add  -- store a function value
```

A function type describes a function value. This means a function can receive
another function as an argument:

```nerd
apply :: fn (f: fn (i32) -> i32, value: i32) -> i32 {
    return f(value)  -- run the function passed as f
}

double :: fn (value: i32) => value * 2  -- function value passed to apply

main :: fn () -> i32 {
    return apply(double, 21)
}
```

## Generic Functions

A generic function has type parameters after `fn`. Type parameters are names
for types that are chosen when the function is used:

```nerd
id :: fn [T] (value: T) -> T {  -- T is chosen for each concrete use
    return value                -- return the same type that came in
}
```

Most calls infer the type parameters from the arguments:

```nerd
main :: fn () -> i32 {
    return id(42)  -- infers T as i32
}
```

Inference can look through pointers, slices, arrays, dynamic arrays, and generic
type aliases used in parameter types. For example, a parameter of type
`^Stack[T]` can infer `T` from an argument of type `^Stack[i32]`.

You can also provide every type argument explicitly:

```nerd
main :: fn () -> i32 {
    return id[i32](42)  -- explicitly use the i32 version of id
}
```

Explicit generic arguments are all-or-nothing. If a function has `[A, B]`, a
call must either let both types be inferred or provide both types explicitly.

A concrete instantiation can be stored as a function value:

```nerd
int_id := id[i32]  -- choose the i32 version as a function value
return int_id(42)  -- function values are called normally
```

Generics accept type parameters only. Constraints are not part of the current
generic syntax, so a generic body must be valid for each concrete use that the
program asks the compiler to build.

## Nested Functions

Functions can be declared inside block bodies:

```nerd
main :: fn () -> i32 {
    helper :: fn () -> i32 {  -- nested function local to main
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
    return answer()  -- answer is declared later
}

answer :: fn () -> i32 {
    return 42
}
```

This keeps source order flexible. Use it deliberately; readers still benefit
from nearby definitions when functions are tightly related.
