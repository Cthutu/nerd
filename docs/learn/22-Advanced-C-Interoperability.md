# Chapter 22: Advanced C Interoperability

[Previous: Atomic Values](21-Atomic-Values.md)

[Next: Building and Configuring Programs](23-Building-and-Configuring-Programs.md)

Chapter 16 introduces foreign declarations, C strings, callbacks, and calls to
variadic C functions. This chapter covers the other direction: receiving a C
argument stream and exposing concrete Nerd functions to C callers.

## Receiving variadic arguments

A variadic definition names its final argument stream with `args: ...`:

```nerd
sum :: fn (count: i32, args: ...) -> i32 {
    total := 0
    for _ in [0..count] {
        total += args.next[i32]()
    }
    return total
}

main :: fn () -> i32 {
    return sum(3, 10, 12, 20)
}
```

This returns `42`. The fixed `count` argument tells the receiver how many
arguments to read. The stream itself contains no count or type metadata.
`next[T]()` consumes one argument; reading past the supplied arguments or
reading the wrong type has undefined behaviour.

A receiving definition requires at least one fixed runtime parameter. Generic,
compile-time, and defaulted parameters are not supported on these definitions.
Imported FFI declarations keep Chapter 16's unnamed `...` marker. A corresponding
callback type may name the marker, such as
`fn (count: i32, args: ...) -> i32`.

## Reading promoted C types

C's variadic promotions apply at the boundary. Small integers are promoted;
a single-precision float arrives as a double. Read the promoted type:

| Argument representation | Cursor read |
| --- | --- |
| Signed promoted integer | `args.next[i32]()` |
| Unsigned 32-bit integer | `args.next[u32]()` |
| Wide or pointer-sized integer | `next[i64]()`, `next[u64]()`, `next[isize]()`, or `next[usize]()`, matching the caller. |
| Floating-point value | `args.next[f64]()` |
| C string pointer | `args.next[^i8]()` |

`next[i8]()` and `next[f32]()` are rejected. A count, format string, or sentinel
is part of your API contract and must agree with the arguments supplied.

## Borrowing and copying a cursor

The argument stream has the opaque type `VaList`. A `VaList` parameter borrows
the current cursor, so helper reads advance that same cursor:

```nerd
read_pair :: fn (args: VaList) -> i32 {
    left := args.next[i32]()
    right := args.next[i32]()
    return left + right
}

sum_twice :: fn (_count: i32, args: ...) -> i32 {
    saved := args.copy()
    first := read_pair(args)
    second := read_pair(saved)
    return first + second
}

main :: fn () -> i32 {
    return sum_twice(2, 10, 11)
}
```

This returns `42`. `copy()` starts independent traversal at the current
position. Ordinary assignment is not a cursor copy. Cursors cannot be returned,
addressed, stored in containers or globals, or passed as a variadic tail value.

The original receiving invocation owns all cursor copies. They are released
together on its return, after user `defer` actions. Repeated copies inside a
loop therefore remain allocated until that function returns. A helper must not
retain a borrowed cursor beyond the receiving invocation.

## Formatting an argument stream

`std.text.format_c` accepts a format and a cursor:

```nerd
use std.text

log :: fn (format: string, args: ...) {
    message := format_c(format, args)
    prn(message)
}

main :: fn () {
    log("answer: %d", 42)
}
```

This prints `answer: 42`. `format_c` accepts Nerd strings or C string pointers.
It copies Nerd formats into terminated temporary storage and uses independent
C argument-list copies for sizing and rendering, leaving the original cursor
unadvanced. The result is a string in the temporary arena. A C formatting
failure produces an empty string.

The lower-level `args.format(format)` operation takes a `^i8` C format pointer.
Both forms require the format to describe the actual argument types. Percent
formatting follows the C library's rules, rather than Nerd interpolation rules.

## Forwarding to C `va_list` functions

On x86-64 hosts, a fixed `VaList` parameter in an FFI declaration represents
C's adjusted `va_list` parameter:

```nerd
ffi "c" vprintf (format: ^i8, args: VaList) -> i32

print_c :: fn (format: ^i8, args: ...) {
    forwarded := args.copy()
    _written := vprintf(format, forwarded)
}

main :: fn () {
    print_c(c"answer: %d\n", 42)
}
```

This prints `answer: 42`. C may consume or invalidate the list according to
the foreign function's contract, so use a copy when the original must remain
usable. The C function must not retain it. This forwards a `va_list`; it does
not expand a cursor into a call to an arbitrary `...` function. Fixed `VaList`
FFI forwarding on other architectures is currently rejected.

## Exporting a C-callable function

Root public functions in object, static-library, and shared-library output
receive plain C export names:

```nerd
add_impl :: fn (left: i32, right: i32) -> i32 {
    return left + right
}

pub tutorial_add :: add_impl
```

Save this as `arithmetic.n`. Build a shared library with:

```sh
nerd build --dll arithmetic.n
```

The exported name is `tutorial_add`. A C caller can declare:

```c
#include <stdint.h>
int32_t tutorial_add(int32_t left, int32_t right);
```

C++ callers use `extern "C"`. A library source does not need an executable
`main`. `--obj` selects relocatable object output; `--lib` selects a static
library. Output suffixes and the C link command depend on the host toolchain.
Root public re-exports also participate in the export surface.

Keep exports concrete and use scalar or pointer signatures, including
compatible function pointers. Aggregate signatures, borrowed `VaList`
parameters, and unresolved generic or compound functions require concrete
wrappers. The `nrt_` symbol prefix is reserved for the runtime.

## Intrinsic declarations

An intrinsic binding names a compiler/runtime intrinsic without a native
library operand:

```nerd
syscall :: intrinsic "syscall" (number: u64,
                                a0: u64, a1: u64, a2: u64,
                                a3: u64, a4: u64, a5: u64) -> i64
```

This is a declaration fragment for the supported platform-specific syscall
binding. Fixed parameters still require `name: Type`. The string names the
lowered intrinsic; it is not a general facility for calling arbitrary LLVM
instructions. Only use an intrinsic with a known compiler/runtime contract
and the matching platform and signature. Prefer existing standard-library
wrappers for application code. Pragmas used to identify compiler-supported
operations are described in [Appendix A](Appendix-A-Syntax-and-Pragmas.md).
