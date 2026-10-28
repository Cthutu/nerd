# Chapter 20: Compile-Time Parameters and Built-In Macros

[Previous: Assertions and Testing](19-Assertions-and-Testing.md)

[Next: Atomic Values](21-Atomic-Values.md)

Generics select types. Compile-time parameters select values known while the
compiler checks a call. Built-in macros supply source locations or embedded
file contents without reading those files at runtime.

## Declaring a compile-time parameter

Use `::` between a parameter name and its type:

```nerd
choose :: fn (enabled :: bool) -> i32 {
    return on enabled => 42 else 0
}

main :: fn () -> i32 {
    return choose(yes)
}
```

This returns `42`. Every argument for `enabled` must be a compile-time constant.
Boolean constants, integer constants, and payload-free enum constants are
supported. A runtime variable cannot supply the argument merely because it
happens to hold the same value.

The `::` belongs to the parameter declaration. Calls use ordinary argument
syntax. Compare `enabled: bool`, which accepts a runtime value, with
`enabled :: bool`, which requires a constant.

## Specialisation and runtime parameters

Different compile-time arguments identify specialised functions. The compiler
substitutes those values and removes them from the runtime parameter list.
Ordinary parameters still carry runtime values:

```nerd
adjust :: fn (value: i32, double_it :: bool) -> i32 {
    return on double_it => value * 2 else value
}

main :: fn () -> i32 {
    value := 21
    return adjust(value, yes)
}
```

This returns `42`. Calls with `yes` share one specialisation; calls with `no`
select another. A compile-time parameter is not a general facility for executing
arbitrary functions during compilation. It also does not replace Chapter 17's
string-literal `on` guards, which exclude declarations before name resolution.

## Compile-time defaults

Compile-time parameters can have defaults, which must also be constants:

```nerd
choose :: fn (enabled :: bool = yes) -> i32 {
    return on enabled => 42 else 0
}

main :: fn () -> i32 {
    return choose() + choose(enabled = no)
}
```

This returns `42`. As with other defaulted parameters, an explicit override
uses its name. `choose(no)` is not the way to override this default.
The atomic-order parameters in the next chapter use this mechanism to ensure
that memory ordering is known during compilation.

## Source filename and line number

`@file` produces a `string` containing the source filename. `@line` produces
an untyped integer containing the current one-based source line:

```nerd
main :: fn () {
    prn(@file)
    prn($"line {@line}")
}
```

The filename depends on the source path used by the compiler. Do not rely on
it being an absolute path or a stable installation path.

A macro in a default argument is evaluated at the call site where that default
is inserted:

```nerd
report :: fn (message: string, file: string = @file, line: usize = @line) {
    prn($"{file}:{line}: {message}")
}

main :: fn () {
    report("ready")
}
```

This reports the location of `report("ready")`, rather than the declaration
of `report`. The caller can supply named defaults explicitly when forwarding
a location from another wrapper. Default overrides must remain in declaration
order.

## Embedding a file

`@embed("path")` reads a file during compilation and produces a `[]u8` slice
backed by static binary data in the program:

```nerd
asset :: @embed("message.txt")

main :: fn () {
    prn(asset.as(string))
}
```

Place `message.txt` beside this source file with the contents `Hello from an
asset!` and no trailing newline. Running the program prints that text. The
file is needed to compile the program, but not to run the resulting executable.

Relative embed paths are resolved from the source file containing the macro,
including when that source belongs to an imported module. They are not resolved
from the shell's working directory. Missing files, unreadable files, and paths
that name directories are compile-time errors.

Embedded data is binary: zero bytes are preserved, and there is no implicit C
string terminator. The string cast in this example creates a counted view;
it does not validate UTF-8 or copy bytes. Treat the static backing data as
read-only even though the expression has slice type. Copy it to owned mutable
storage before changing it.

## Choosing the mechanism

| Need | Mechanism |
| --- | --- |
| Reuse an algorithm for different types | Generic type parameters. |
| Specialise a function for constant values | Compile-time `::` parameters. |
| Include or exclude platform or feature code | Compile-time define guards. |
| Report a source location | `@file` and `@line`. |
| Ship bytes inside the executable | `@embed("path")`. |

These are built-in facilities. They do not introduce C-style text substitution
or a user-defined macro language.
