# Chapter 2: First Programs and Bindings

[Previous: Overview](01-Overview.md)

[Next: Primitive Types and Expressions](03-Primitive-Types-and-Expressions.md)

This chapter introduces a complete Nerd program, how bindings give names to
values and types, and the two forms of function definition. It assumes you
already know how to write and call functions in C.

## Hello world

Save the following program as `hello.n`:

```nerd
main :: fn () {
    prn("Hello, world!")
}
```

With the Nerd compiler installed and available on your command path, run it with:

```sh
nerd run hello.n
```

The compiler builds the program and runs it. You should see:

```text
Hello, world!
```

The program's entry point is the function bound to `main`. Here, `fn ()` defines
a function with no parameters, and the braces contain its statements. Calling
`prn` prints the supplied string followed by a newline. It comes from `core`,
which every Nerd module imports implicitly, so this program needs no explicit
import.

Statements do not need terminating semicolons. Nerd line comments begin with
`--`:

```nerd
main :: fn () {
    prn("Hello, world!")  -- Print a line, then finish.
}
```

## Names come from bindings

In Nerd, a function definition does not contain its own name. The expression
`fn () { ... }` defines the function, and `main ::` binds a name to it. The same
principle applies to types and other values: bindings provide the names by
which you refer to them.

The type `i32` is a signed 32-bit integer type: it represents whole numbers,
including negative values. These declarations name an integer value, a type,
and a function:

```nerd
answer :: 42
Count :: i32
greet :: fn () {
    prn("Hello, world!")
}
```

`answer` names the untyped integer value `42`. This binding does not choose a
concrete integer type; a use that requires one can supply it. `Count` is an
alias for `i32`; it does not create a distinct type. `greet` names the function
defined on its right. The capitalisation of
`Count` is a convention for a type name.

You can also bind another name to an existing function:

```nerd
main :: fn () {
    say_hello()
}

say_hello :: greet

greet :: fn () {
    prn("Hello, world!")
}
```

`say_hello` is an alias for `greet`. It introduces another name for the same
function. Calling either name runs the same code.

The order of top-level definitions does not matter. Here, `main` uses
`say_hello` before its definition, and `say_hello` refers to `greet` before its
definition. Nerd resolves these names without requiring forward declarations
or function prototypes. You can arrange top-level functions, type aliases, and
constant bindings in the order that makes the source easiest to read. Statements
inside a function still execute in the order you write them.

## The binding syntax

The general form of an alias binding is:

```text
<binding> : <optional-type> : <type-or-expression>
```

The first part supplies the name. The optional type specifies the type expected
for the expression on the right. If you omit it, the two colons appear together
as `::`, and the compiler determines the type from what you bind.

```nerd
answer :: 42
typed_answer : i32 : 42
Count :: i32
```

The first declaration binds an untyped integer. The second explicitly chooses
`i32`, so `typed_answer` has a concrete integer type. The third binds a type,
giving `i32` the alias `Count`. Omitting the optional type preserves the type of
the value on the right, which may itself be untyped.

These bindings cannot be reassigned. They give names to values, functions, or
types; mutable variables are introduced in a later chapter.

## Block functions

A block function contains statements inside braces. `-> Type` specifies its
return type. Add this second function to `hello.n`:

```nerd
get_answer :: fn () -> i32 {
    return 42
}
```

This resembles a C function with no parameters and an integer return value.
`get_answer` is the binding, `fn ()` defines a function with no parameters, and
`-> i32` declares that it returns a signed 32-bit integer. The `return 42`
statement ends the function and passes `42` back to its caller.

Writing `get_answer()` calls the function and produces the value `42`. Simply
defining the function does not run it: the original `main` still prints the
greeting and finishes without returning a value. Later examples will use
function results in larger expressions.

For a block function that returns a value, write its return type explicitly.
For one that returns no value, you can omit the return type, as in `greet`.
Writing `-> void` explicitly has the same meaning.

## Expression functions

When a function's body is a single expression, use `=>`:

```nerd
get_answer :: fn () => 42
```

The expression after `=>` supplies the function's result. There are no body
braces and no `return` statement; the compiler infers the return type from the
expression. The literal `42` is an untyped integer, but an inferred function
return type is made concrete: here it defaults to `i32`. This differs from
`answer :: 42`, which preserves the untyped integer. Both forms of `get_answer`
therefore return an `i32` value of `42`. Replace the block definition with this
expression definition to try it; they are two alternatives for the same binding.

An expression function can also perform an operation that returns no value:

```nerd
greet :: fn () => prn("Hello, world!")

main :: fn () {
    greet()
}
```

Since `prn` returns no value, this `greet` also returns no value. Choose a block
body when you need several statements and an expression body when one expression
captures the operation clearly.

## Returning from a function

In a block function, `return expression` returns a value. A bare `return` exits
a function that returns no value:

```nerd
greet :: fn () {
    prn("Hello, world!")
    return
}

main :: fn () {
    greet()
}
```

The bare `return` is optional here because reaching the end of a function with
no return value also finishes the call. It is useful when you need to leave a
function before reaching the end of its body.

For `main`, you can choose between returning no value and returning an `i32`
exit status. The original hello world returns no value and finishes with status
`0`. To make the status explicit, write:

```nerd
main :: fn () -> i32 {
    prn("Hello, world!")
    return 0
}
```

As in C, status `0` indicates success, and a nonzero status indicates failure.
The status is passed to the operating system; it is separate from the text
printed by the program.

## Function parameters

A function declares its inputs inside the parentheses after `fn`. Each
parameter uses `name: Type`:

```nerd
echo :: fn (value: i32) -> i32 {
    return value
}

main :: fn () -> i32 {
    return echo(42)
}
```

This returns `42`. `value` is a parameter of type `i32`. Calling `echo(42)`
supplies `42` as its argument, and the function returns that value. Separate
multiple parameters and their corresponding arguments with commas.

The parameter type supplies context for the argument: here the untyped literal
`42` becomes an `i32`. The compiler checks each argument against its parameter's
type. Parameters receive arguments by value. The parameter name is available
inside the function body and cannot be reassigned there.
