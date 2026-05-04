# Part 11: Interoperability With C

[Manual Index](README.md) | Previous: [Modules](part10-modules.md) | Next: [Building A Small Program](part12-building-a-small-program.md)

FFI means foreign function interface: the part of Nerd that lets source code
declare and run C functions. Nerd's FFI surface is explicit. You declare the
library, the foreign symbol, and the ABI-safe signature.

## Direct FFI Declarations

The simplest form declares a foreign function with the same Nerd-visible name
as the C symbol:

```nerd
ffi "c" abs (i32) -> i32  -- declare the C function abs

main :: fn () -> i32 {
    return abs(-7)  -- run the foreign function
}
```

Here `abs` is both the Nerd name and the foreign symbol name.

When several functions come from the same library, group them in an FFI block:

```nerd
ffi "c" {
    abs (i32) -> i32      -- declares the C function abs
    strlen (^u8) -> usize -- declares the C function strlen
}
```

Each entry inside the block has the same form as the part after the library in a
direct declaration. FFI block entries do not have separate Nerd binding names;
the Nerd-visible name is the foreign symbol name.

Use `pub ffi` when a module should export the declared foreign functions:

```nerd
pub ffi "c" {
    ioctl (i32, i32, ...) -> i32  -- exported from this module
}
```

`pub` applies to every entry in the block. For a single foreign function,
`pub ffi "c" name (...) -> Type` exports that one declaration.

## Bound FFI Declarations

Use a binding when the Nerd name should differ from the foreign symbol:

```nerd
seed_rng :: ffi "c" srand (u32)  -- Nerd name differs from C symbol
```

Source code runs `seed_rng(...)`. Generated code links to `srand`.

Use this form when the C name is unclear, conflicts with a better wrapper name,
or does not fit your source style.

## Library Operands

The library operand is a compile-time string. A compile-time string is a string
value the compiler can know while building the program:

```nerd
ffi "c" puts (^u8) -> i32  -- "c" names the C library
```

It can also come from a compile-time binding:

```nerd
libc :: "c"
write_line :: ffi libc puts (^u8) -> i32  -- use a compile-time binding
```

Parenthesised compile-time expressions are allowed:

```nerd
sqrt_fn :: ffi ("m") sqrt (f64) -> f64  -- parenthesised library expression
```

## Return Types

If the return type is omitted, the foreign function returns `void`:

```nerd
seed_rng :: ffi "c" srand (u32)  -- no -> Type means void
```

Write `-> Type` when the function returns a value.

## Variadic Functions

Use `...` for C variadic functions:

```nerd
ffi "c" printf (^u8, ...) -> i32  -- ... accepts C variadic arguments
```

Variadic syntax is only for FFI signatures.

## C Strings

C string literals use `c"..."`:

```nerd
ffi "c" puts (^u8) -> i32

main :: fn () {
    puts(c"hello")  -- pass a null-terminated C string
}
```

C strings are not Nerd `string` values. Convert deliberately at the boundary.
They are null-terminated so C functions can read them through `^u8`.

## Pointer-To-Slice At Boundaries

C APIs often return a pointer plus a size. Convert that pair to a Nerd slice
view with:

```nerd
view := ptr.as([]u8, size)  -- pointer plus size becomes a Nerd slice
```

The slice borrows memory. It does not free or own the pointer.

## Wrapper Functions

Prefer wrapping raw FFI in Nerd functions:

```nerd
c_realloc :: ffi "c" realloc (^void, usize) -> ^void  -- raw C binding

realloc :: fn (ptr: ^void, size: usize) -> ^void {
    return c_realloc(ptr, size)  -- wrapper controls the public name
}
```

Wrappers give the rest of your program clearer names and a place to enforce
ownership rules.

## ABI-Safe Types

FFI signatures should use types with clear C ABI behaviour, such as primitive
integers, floats, pointers, raw unions, and C-compatible plexes. Keep higher
level types behind wrappers unless their ABI contract is explicit.
