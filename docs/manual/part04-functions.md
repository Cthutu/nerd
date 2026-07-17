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

Parameter bindings are immutable. A function cannot assign a new value to a
parameter or assign to storage that belongs directly to a by-value parameter.
Use a local variable when the function needs a scratch copy:

```nerd
normalise :: fn (seed: u64) -> u64 {
    value := seed
    on value == 0 => value = 7
    return value
}
```

Use a pointer parameter when the function needs to mutate caller-owned state.
The pointer binding cannot be reassigned, but the value it points at can be
updated:

```nerd
move :: fn (state: ^State) {
    state.player_loc = 1
}
```

## Default Parameters

### Compile-time parameters

Write `::` after a parameter name when every call must supply a value known
during semantic analysis:

```nerd
choose :: fn (enabled :: bool = yes) -> i32 {
    return on enabled => 1 else 0
}
```

Boolean, integer, and payload-free enum constants are supported. Defaults must
also be compile-time constants. This facility is used by `std.atomics` so an
ordering never reaches the backend as an unresolved runtime value.

Each distinct set of compile-time arguments identifies a canonical specialised
function. The compiler substitutes those values while producing HIR and removes
them from the specialised function's runtime parameter list. Runtime parameters
remain shared normally. The same value-identity model is reserved for future
compile-time module parameters.

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

`void` functions can omit both `-> void` and a final return:

```nerd
use std.io

show :: fn (value: i32) {
    prn($"value={value}")  -- no return value is needed
}
```

A block-bodied function without `-> Type` returns no value, so `return <expr>`
inside it is an error. The entry point `main` is the compatibility exception:
an unannotated block-bodied `main` may still return an integer process status.

Use `-> !` for a function that never returns to its caller:

```nerd
fail :: fn (message: string) -> ! {
    abort(message)
}
```

The standard library `abort` function returns `!`. A `!` expression can appear
where another value type is expected because evaluation never reaches the point
where that value would be used.

## Expression-Bodied Functions

Use `=>` when the whole function is one expression:

```nerd
triple :: fn (value: i32) => value * 3  -- expression body
```

Expression-bodied functions infer their return type from the expression. They
are useful for small helpers.

## Compound Functions

A compound function gives one name to a closed, explicit set of ordinary
functions. It is useful when an operation has several distinct parameter
signatures:

```nerd
write :: fn {
    write_string
    write_i64
}

write_string :: fn (value: string) { }
write_i64 :: fn (value: i64) { }
```

At each call, the compiler selects the only member compatible with the
arguments. There is no preference order: no match is an error and two matches
are ambiguous. Return types never influence selection. Trailing defaults add
callable forms, so members whose parameter lists overlap after defaults are
rejected when the compound is declared.

Compound functions are top-level declarations. Members may be forward
references, qualified imported names, or other compounds; nesting is flattened
and cycles are rejected. Members must currently be non-generic free functions,
not methods or associated functions. A public compound may use private members
without exporting their implementation names.

An expected function type can select a compound member:

```nerd
write_number: fn(i64) = write
```

Without a selecting function type, a compound cannot be stored as a value or
taken by address. Selection produces the concrete function directly; Nerd does
not generate a runtime dispatcher. Concrete members remain directly callable.

## Function Types

Function types are written with parameter types and an optional return type:

```nerd
operation: fn (i32, i32) -> i32 = add  -- store a function value
handler: fn (i32) = log_value           -- no return value
```

Parameter names are allowed in function types for readability, but they are not
part of the type:

```nerd
handler: fn (value: i32) = log_value
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
