# Part 11: Interoperability With C

[Manual Index](README.md) | Previous: [Modules](part10-modules.md) | Next: [Building A Small Program](part12-building-a-small-program.md)

FFI means foreign function interface: the part of Nerd that lets source code
declare and run C functions. Nerd's FFI surface is explicit. You declare the
library, the foreign symbol, and the ABI-safe signature.

## Direct FFI Declarations

The simplest form declares a foreign function with the same Nerd-visible name
as the C symbol:

```nerd
ffi "c" abs (value: i32) -> i32  -- declare the C function abs

main :: fn () -> i32 {
    return abs(-7)  -- run the foreign function
}
```

Here `abs` is both the Nerd name and the foreign symbol name.
FFI parameters must be named with `name: Type`. The names appear in editor hover
and signature help but do not affect linking or the C ABI.

When several functions come from the same library, group them in an FFI block:

```nerd
ffi "c" {
    abs (value: i32) -> i32      -- declares the C function abs
    strlen (text: ^i8) -> usize  -- declares the C function strlen
    length :: strlen (text: ^i8) -> usize -- Nerd name differs from C symbol
}
```

Each entry inside the block has the same form as the part after the library in a
direct declaration. Use `local :: foreign` inside the block when the
Nerd-visible name should differ from the foreign symbol.

FFI block entries are module declarations. Code earlier in the same module may
call them, just like other top-level declarations.

Use `pub ffi` when a module should export the declared foreign functions:

```nerd
pub ffi "c" {
    ioctl (fd: i32, request: i32, ...) -> i32  -- exported from this module
}
```

`pub` applies to every entry in the block. For a single foreign function,
`pub ffi "c" name (...) -> Type` exports that one declaration. Inside a block,
`pub local :: foreign (...) -> Type` exports only that entry.

## Bound FFI Declarations

Use a binding when the Nerd name should differ from the foreign symbol:

```nerd
seed_rng :: ffi "c" srand (seed: u32)  -- Nerd name differs from C symbol
```

Source code runs `seed_rng(...)`. Generated code links to `srand`.

Inside an FFI block, omit the repeated library operand:

```nerd
ffi "c" {
    seed_rng :: srand (seed: u32)
}
```

Use this form when the C name is unclear, conflicts with a better wrapper name,
or does not fit your source style.

## Intrinsic Bindings

`intrinsic` binds a named compiler/runtime intrinsic without a library operand:

```nerd
syscall :: intrinsic "syscall" (number: u64,
                                a0: u64,
                                a1: u64,
                                a2: u64,
                                a3: u64,
                                a4: u64,
                                a5: u64) -> i64
```

The string gives the lowered symbol name. Use this for platform-specific
bindings that are not modelled as ordinary `ffi "lib"` declarations.

## Library Operands

The library operand is a compile-time string. A compile-time string is a string
value the compiler can know while building the program:

```nerd
ffi "c" puts (text: ^i8) -> i32  -- "c" names the C library
```

It can also come from a compile-time binding:

```nerd
libc :: "c"
write_line :: ffi libc puts (text: ^i8) -> i32  -- use a compile-time binding
```

Parenthesised compile-time expressions are allowed:

```nerd
sqrt_fn :: ffi ("m") sqrt (value: f64) -> f64  -- parenthesised library expression
```

## Return Types

If the return type is omitted, the foreign function returns `void`:

```nerd
seed_rng :: ffi "c" srand (seed: u32)  -- no -> Type means void
```

Write `-> Type` when the function returns a value.

## Variadic Functions

Use `...` for C variadic functions:

```nerd
ffi "c" printf (format: ^i8, ...) -> i32  -- accepts C variadic arguments
```

Variadic definitions name the C argument stream with a final `args: ...`
parameter:

```nerd
pub sum :: fn (count: i32, args: ...) -> i32 {
    total := 0
    for _ in [0..count] { total += args.next[i32]() }
    return total
}
```

A receiving function needs at least one fixed runtime parameter. Generic,
compile-time, and defaulted parameters are not supported on these definitions.
The corresponding callback type is `fn (i32, ...) -> i32`. Imported FFI
signatures retain the unnamed `...` spelling.

`next[T]()` consumes one argument. C supplies no count or type metadata: a
count, format string, or sentinel must tell the function what to read. There
is no end-of-stream result. Reading past the arguments or using the wrong type
has undefined behaviour.

Use the promoted C type: `i32`, `u32`, `i64`, `u64`, `isize`, `usize`, `f64`,
or a pointer type. Small integers undergo C integer promotion and `float`
arrives as `double`; `next[i8]()` and `next[f32]()` are rejected. Nerd applies
these promotions when making outgoing variadic calls too.

The cursor has the opaque type `VaList`. Pass it directly to a helper parameter
`args: VaList` to borrow it. Each `next` advances the shared cursor. Use
`other := args.copy()` for independent traversal from the current position.
Ordinary assignment, taking its address, storing it in a container or global,
and returning a cursor are rejected. All copies belong to the original
receiving invocation and are released together when that function returns,
after user `defer` statements. Copies created repeatedly in a loop remain
allocated until that return.

For a printf-style logger, `std.text.format_c(format, args)` returns a string
in the temporary arena without advancing `args`. It uses separate C argument
list copies for sizing and rendering; a C formatting failure returns an empty
string. `args.format(format)` is the underlying cursor operation.

On x86-64 hosts, a fixed `VaList` parameter in an FFI declaration means C's
`va_list` parameter, with target-specific argument adjustment:

```nerd
ffi "c" vprintf (format: ^i8, args: VaList) -> i32

print_c :: fn (format: ^i8, args: ...) {
    forwarded := args.copy()
    _written := vprintf(format, forwarded)
}
```

The foreign function must not retain the borrowed argument list. Its contract
determines whether the list is consumed or becomes indeterminate; use a copy
when the original is needed afterwards. This does not expand a cursor into an
ordinary `...` call. C `va_list` forwarding on other architectures is currently
rejected; scalar cursor reads use the host C runtime implementation.


## C Strings

C string literals use `c"..."`:

```nerd
ffi "c" puts (text: ^i8) -> i32

main :: fn () {
    puts(c"hello")  -- pass a null-terminated C string
}
```

C strings are not Nerd `string` values. Convert deliberately at the boundary.
They are null-terminated so C functions can read them through `^i8`.

When a C API expects a mutable pointer to null-terminated text, use
`string.c_string` to copy a Nerd string into the temporary arena and append
the zero terminator:

```nerd
str :: use std.text

name := "OpenGL".c_string()
```

The returned pointer is valid until the temporary arena is restored or reset.

When a C API writes text into a fixed byte buffer, cast the buffer to `string`
and use `string.trim_null_terminated` to trim at the first zero byte:

```nerd
str :: use std.text

buffer: [512]i8
text := buffer.as(string).trim_null_terminated()
```

The result is a borrowed view over `buffer`; it does not copy or validate the
bytes.

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
c_realloc :: ffi "c" realloc (ptr: ^void, size: usize) -> ^void

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

## Exporting C Functions

Root public functions in `--obj`, `--lib`, and `--dll` output have plain C
export names. `pub add :: fn (...)` exports `add`, without a `$` prefix.
Root public re-exports participate in the same export surface. To select a
different external name, bind the implementation under the desired public name:

```nerd
log_impl :: fn (format: ^i8, args: ...) {
    _message := args.format(format)
}
pub myapp_log :: log_impl
```

C exports currently support scalar and pointer signatures, including compatible
function pointers. Aggregate signatures, borrowed `VaList` parameters, and
unresolved generic or compound exports require concrete wrappers. The `nrt_`
prefix is reserved for runtime symbols. Internal compiler symbols are not a
public naming contract. Existing C clients using `$` exports must update their
bindings and rebuild. C++ declarations use `extern "C"`.

See [the C logger example](../../examples/ffi-logger/README.md) for a complete
DLL and caller.
