# Part 1: First Programs

[Manual Index](README.md) | Previous: none | Next: [Values, Bindings, And Assignment](part02-values-bindings-and-assignment.md)

Nerd source files are plain text files. By convention they use the `.n`
extension and can be run with:

```sh
nerd run hello.n
```

Every program starts by running the function bound to the name `main`. A
function is a piece of code that can be run. Functions do not have to be named
by themselves; like other values in Nerd, they get names through bindings.

The first example binds `main` to a simple function:

```nerd
main :: fn () {
    prn("hello, Nerd")  -- print a line of text
}
```

This program declares a function named `main` and prints one line of text.
`pr` and `prn` come from `core`, which every file imports implicitly. `pr`
prints text without adding a newline. `prn` prints text and then moves to the
next line.

| Form            | Meaning                              |
| --------------- | ------------------------------------ |
| `main :: ...`   | bind the top-level name `main`       |
| `fn () { ... }` | define a function with no parameters |
| `prn("text")`   | print text followed by a newline     |

`main` can also receive command-line arguments:

```nerd
main :: fn (args: []string) {
    prn(args[0])
}
```

Run arguments are passed after `--`, as in `nerd run hello.n -- one two`.
`args[0]` is the executable path supplied by the operating system; user
arguments start at `args[1]`.

## Comments

Line comments start with `--`. Comments are for readers of the source file:
they can explain intent, leave notes, or temporarily remove code from a
program. The compiler ignores them. The formatter may reflow ordinary comment
paragraphs; use `--|` for comment lines whose spacing and line breaks should be
preserved.

```nerd
-- This is ignored by the compiler.
--| This comment line is preserved exactly by the formatter.
main :: fn () {
}
```

## Top-Level Bindings

Names are introduced with binding forms. At the top level, the first binding
form to learn is `::`. It creates a constant binding: the name on the left is
bound to the value on the right.

```nerd
answer :: 42  -- bind the name answer to the value 42

main :: fn () {
}
```

Here `answer` names the value `42`. The same binding form can name functions,
types, modules, and other top-level declarations. Variable bindings are covered
in the next part.

## Block Functions

Functions group code so it can be run later. A function can receive input
values, called parameters, and it can return an output value. This part only
uses simple functions; later parts explain parameters and return types in
detail.

There are two common source forms:

| Form                       | Best for           |
| -------------------------- | ------------------ |
| block function             | several statements |
| expression-bodied function | one expression     |

A block function uses braces and contains statements:

To run a function from another part of the program, write its name followed by
parentheses. For a function with no inputs, the parentheses are empty:

```nerd
use std.io

show :: fn () {
    prn("inside show")
}

main :: fn () {
    show()  -- run the function named show
}
```

Functions that do not return a value can end naturally.

You may also write an explicit bare `return`:

```nerd
main :: fn () {
    return  -- leave the function explicitly
}
```

Functions that return values and functions with parameters are covered later,
after values and types have been introduced.

## Expression-Bodied Functions

An expression-bodied function uses `=>` when the whole function is one
expression:

```nerd
answer :: fn () => 42  -- the expression after => is the function body

main :: fn () => answer()  -- run answer and return its result
```

Expression-bodied functions infer their return type from the expression body.

## Printing

Most examples in this manual use `core` printing:

```nerd
main :: fn () {
    pr("same line")       -- print without a newline
    prn(" then newline")  -- print, then add a newline
}
```

`pr` prints text without a newline. `prn` prints text followed by a newline.
They come from `core`, which is imported implicitly so small examples can show
program behaviour without an explicit module import.
