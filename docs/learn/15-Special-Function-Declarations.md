# Chapter 15: Special Function Declarations

[Previous: Modules](14-Modules.md)

[Next: Defines](16-Defines.md)

This chapter covers multi-functions, which select among a set of functions,
and foreign function declarations, which call code through the C ABI. Both
extend ordinary function declarations without changing the binding rules.

## Multi-functions

A multi-function, also called a **compound function**, groups a closed set
of functions under one binding:

```nerd
describe :: fn {
    describe_number
    describe_text
}

describe_number :: fn (value: i32) => $"number {value}"
describe_text :: fn (value: string) => $"text {value}"

main :: fn () {
    prn(describe(42))
    prn(describe("hello"))
}
```

This prints `number 42` followed by `text hello`. At each call, the compiler
selects the one member whose parameters accept the arguments. The member
functions remain directly callable, and forward definitions work normally.

The set is explicit. Declaring unrelated functions with the same name does
not automatically assemble an overload set. The compiler does not try members
in order: no compatible member is an error, and several compatible members
are ambiguous. Return types do not select a member. An untyped number may
therefore need a cast when several numeric signatures could accept it.

Members may name other multi-functions, which are flattened into one set;
cycles are rejected. Members must currently be non-generic free functions,
rather than receiver methods or associated functions. A public multi-function
can select private member functions without exporting their individual names.

## Selecting a function value

A function type can select a concrete member without calling it immediately:

```nerd
convert :: fn { increment text_length }
increment :: fn (value: i32) => value + 1
text_length :: fn (value: string) => value.count.as(i32)

main :: fn () -> i32 {
    selected: fn (value: i32) -> i32 = convert
    return selected(41)
}
```

This returns `42`. `fn (value: i32) -> i32` describes a function value taking
an `i32` and returning an `i32`. Parameter names are required in function
types, but do not affect type compatibility. Here the type selects `increment`.

Without a selecting function type, a multi-function cannot be stored as an
ordinary function value or taken by address. Selection happens at compile
time and produces the concrete function; there is no runtime dispatcher.

## Default parameters and callable forms

An ordinary function may supply defaults for trailing parameters:

```nerd
add :: fn (value: i32, extra: i32 = 1) => value + extra

main :: fn () -> i32 {
    return add(40, extra = 2)
}
```

This returns `42`; `add(40)` would return `41`. Supply a defaulted parameter
by name, using `extra = 2`. A positional `add(40, 2)` is rejected. All
parameters after the first defaulted parameter must also have defaults.
Named arguments follow declaration order and cannot skip earlier parameters.

Defaults are evaluated at the call site and can refer to earlier parameters.
They belong to the declaration rather than the function type. Each default
also creates a shorter callable form: a multi-function rejects members whose
parameter lists overlap after defaults are considered.

## Declaring a foreign function

The foreign function interface (**FFI**) lets Nerd call C functions. Declare
the library, symbol, and exact signature with `ffi`:

```nerd
ffi "c" abs (value: i32) -> i32

main :: fn () -> i32 {
    return abs(-42)
}
```

This returns `42`. `"c"` selects the platform C runtime, including on Windows;
`abs` is both the Nerd binding and the foreign symbol. The declaration has
no Nerd function body. Fixed parameters still need names, and omitting a
return type means `void`.

The compiler checks calls against the declaration you provide. It does not
read a C header and verify the signature for you. Match the library's ABI,
including integer widths, pointer types, return type, and variadic arguments.
FFI parameters cannot have defaults; use an ordinary Nerd wrapper when you
want to offer default arguments.

## Renaming and grouping foreign declarations

A binding can give a foreign symbol a different Nerd name:
`magnitude :: ffi "c" abs (value: i32) -> i32`. Source calls `magnitude`,
while the linker resolves `abs`.

Group declarations from one library in an FFI block:

```nerd
ffi "c" {
    magnitude :: abs (value: i32) -> i32
    length :: strlen (text: ^i8) -> usize
}

main :: fn () -> i32 {
    return magnitude(-40) + length(c"hi").as(i32)
}
```

This returns `42`. Entries omit the repeated library operand. The
`local :: foreign` form renames a symbol; an unrenamed entry can simply
start with its foreign name, as in `abs (value: i32) -> i32`.

The library operand must be a compile-time string. A constant binding such
as `libc :: "c"` can supply it. Use `pub ffi` to export a declaration or
every entry in a block; `pub` on an individual block entry exports just that
entry. Declarations can be referenced earlier in the module like other
top-level bindings.

## C strings at the boundary

The `c"hi"` literal above is a null-terminated C string, with a pointer type
compatible with `^i8`. A Nerd `string` instead carries a pointer and a length;
it is not interchangeable with that C pointer.

For runtime text, the standard text module provides `.c_string()`:

```nerd
text :: use std.text
ffi "c" strlen (value: ^i8) -> usize

main :: fn () -> i32 {
    number := 42
    message := $"answer={number}"
    terminated := message.c_string()
    return strlen(terminated).as(i32)
}
```

This returns `9`. `.c_string()` copies the text into the temporary arena and
adds a zero terminator. The returned pointer remains valid until that arena
storage is reclaimed, following Chapter 13's rules. A foreign function must
not retain it beyond that lifetime. Taking a string's data pointer alone
does not add the terminator a C string API requires.

Use pointers to pass structured data whose layout the C API specifies. A plex
marked `#c` keeps the C-compatible field layout introduced in Chapter 5.
Nerd strings, slices, dynamic arrays, optionals, and results are language
values; do not assume an arbitrary C signature accepts their representation.

## Typed callbacks

Declare a C callback using a named function type and pass a matching function
directly. There is no need to erase its type to `^void`:

```nerd
Compare :: fn (left: ^void, right: ^void) -> i32
ffi "c" qsort (base: ^void, count: usize, size: usize, compare: Compare)

compare :: fn (left: ^void, right: ^void) -> i32 {
    first := left.as(^i32)^
    second := right.as(^i32)^
    return on {
        first < second => -1
        first > second => 1
        else => 0
    }
}

main :: fn () -> i32 {
    values := [40, 2]
    qsort(^values[0], values.count, i32.size, compare)
    return on values[0] == 2 && values[1] == 40 => 42 else 0
}
```

This returns `42`. The C library calls `compare` with pointers to two array
elements. The function casts them back to the element type and returns the
ordering sign expected by `qsort`.

The callback signature must follow the C ABI. Scalar values, pointers, and
compatible function types are supported; language aggregates passed by value
are not accepted in callback signatures. Pass pointers to such data instead.

## Variadic foreign functions

Use `...` for the variable argument portion of a C declaration:

```nerd
ffi "c" printf (format: ^i8, ...) -> i32

main :: fn () {
    _written := printf(c"answer=%d\n", 42)
}
```

This prints `answer=42`. The return value is explicitly discarded as required
by Chapter 4. C's format string determines the number and types of arguments;
Nerd does not check that a `%d` matches what you supplied.

C argument promotions still apply: small integers are promoted, and an `f32`
argument is passed as `f64`. Pointer arguments must match the foreign API's
expectations.

Nerd can also define a function that receives a C-style argument stream.
Name its final parameter with `args: ...`, then consume the agreed types:

```nerd
sum_three :: fn (first: i32, args: ...) -> i32 {
    second := args.next[i32]()
    third := args.next[i32]()
    return first + second + third
}

main :: fn () -> i32 {
    return sum_three(20, 10, 12)
}
```

This returns `42`. The example's contract requires exactly those two further
`i32` arguments. The stream carries no count or type metadata, so reading too
many values or requesting the wrong type has undefined behavior. Read the
promoted C type, such as `i32` or `f64`, rather than `i8` or `f32`.

A receiving declaration needs at least one fixed runtime parameter. Generic
and defaulted parameters are not supported on these receiving
definitions. Its argument cursor is borrowed for that invocation; it is not
an owning collection that may escape the function.
