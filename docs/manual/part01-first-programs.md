# Part 1: First Programs

[Manual Index](README.md) | Previous: none | Next: [Values, Bindings, And Assignment](part02-values-bindings-and-assignment.md)

Nerd source files are plain text files. By convention they use the `.n`
extension and can be run with:

```sh
nerd run hello.n
```

Every program starts at `main`.

```nerd
use std.io

main :: fn () {
    prn("hello, Nerd")
}
```

This program imports `std.io`, declares a function named `main`, and prints one
line.

## Comments

Line comments start with `--`.

```nerd
-- This is ignored by the compiler.
main :: fn () {
}
```

## Top-Level Bindings

Top-level names are introduced with `::`.

```nerd
answer :: 42

main :: fn () -> i32 {
    return answer
}
```

At the top level, `::` is used for constants, functions, type aliases, modules,
and other declarations. The name on the left becomes available elsewhere in the
program.

## Block Functions

A block function uses braces:

```nerd
add :: fn (a: i32, b: i32) -> i32 {
    return a + b
}
```

When a block function returns a value, use `return <expr>`.

```nerd
main :: fn () -> i32 {
    return add(20, 22)
}
```

Functions returning `void` can end naturally:

```nerd
use std.io

main :: fn () {
    prn("done")
}
```

You may also write an explicit bare `return` in a `void` function:

```nerd
main :: fn () {
    return
}
```

## Expression-Bodied Functions

Short functions can use `=>`:

```nerd
double :: fn (value: i32) => value * 2
```

Expression-bodied functions infer their return type from the expression body.
Block functions use `-> Type` when they return a value.

## Printing

Most examples in this manual use `std.io`:

```nerd
use std.io

main :: fn () {
    pr("same line")
    prn(" then newline")
}
```

`pr` prints text without a newline. `prn` prints text followed by a newline.
The exact standard library API is documented separately; here it is just a
convenient way to see program behaviour.
