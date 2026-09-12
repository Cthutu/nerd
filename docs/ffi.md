# FFI

Nerd keeps the FFI surface explicit and ABI-focused.

## Syntax

Use `ffi` with an explicit library operand, foreign symbol name, and
signature:

```nerd
ffi "c" abs (value: i32) -> i32
```

This declares `abs` as both:

- the Nerd-visible name used in source
- the foreign symbol name emitted for the C linker

Every fixed parameter uses `name: Type`. The name is compulsory so diagnostics,
hover, and signature help can describe call arguments clearly; it does not
change the foreign ABI.

Several declarations that use the same library can be grouped in an FFI block:

```nerd
ffi "c" {
    abs (value: i32) -> i32
    strlen (text: ^i8) -> usize
    length :: strlen (text: ^i8) -> usize
    pub seed_rng :: srand (seed: u32)
}
```

Entries in a block can use `local :: foreign (...)` when the Nerd-visible name
should differ from the foreign symbol. Prefix an entry with `pub` to export that
entry without making the whole block public.

You can also bind the foreign symbol to a different Nerd-visible name:

```nerd
seed_rng :: ffi "c" srand (seed: u32)
```

The local binding name is `seed_rng`; the foreign symbol name is `srand`. Source
code calls `seed_rng(...)`, while generated code links against `srand`. The same
rename can be written inside an FFI block as `seed_rng :: srand (seed: u32)`.

Use the bound form when:

- the C symbol name is unclear or does not match Nerd naming style
- you want to wrap or reserve the original foreign name
- several libraries expose similar names and local clarity matters

## Library Operand

The library operand is any compile-time expression whose value is a `string`.

These are valid:

```nerd
ffi "c" abs (value: i32) -> i32

libc :: "c"
puts_line :: ffi libc puts (text: ^i8) -> i32

sqrt_fn :: ffi ("m") sqrt (value: f64) -> f64
```

Runtime string values are not valid library operands.

## Return Types

Omitting `-> <type>` means the foreign function returns `void`:

```nerd
seed_rng :: ffi "c" srand (seed: u32)
```

## Varargs

Imported C functions use an unnamed `...`:

```nerd
ffi "c" fcntl (fd: i32, command: i32, ...) -> i32
```

## ABI

The current FFI surface assumes the default C ABI only.

Calling-convention syntax is intentionally deferred until it is specified and
tested on platforms that need it.

## Supported FFI-Safe Types

Nerd currently accepts these types in FFI signatures:

- `void`
- `bool`
- signed integers: `i8`, `i16`, `i32`, `i64`, `isize`
- unsigned integers: `u8`, `u16`, `u32`, `u64`, `usize`
- floats: `f32`, `f64`
- pointers: `^T`
- raw `union`
- `plex #c`
- `plex #packed`

## Rejected Higher-Level Types

These are rejected in FFI signatures because their layout or calling convention
is not explicit enough yet:

- `string`
- slices like `[]T`
- tuples
- fixed arrays like `[N]T`
- ordinary `plex` without `#c` or `#packed`
- enums without an explicit ABI contract

## Diagnostics

FFI diagnostics should stay concrete about the ABI boundary:

- the primary message identifies the invalid part of the signature
- notes explain the rule being enforced
- help points at the fix, such as choosing an ABI-safe type or making the
  library operand a compile-time string

## Receiving Variadic Calls And Export Names

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
The corresponding callback type is `fn (count: i32, args: ...) -> i32`. Imported FFI
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

For a printf-style logger, `std.text.format_c(format, args)` accepts a Nerd
`string` or a null-terminated C string (`^i8`) and returns a string in the
temporary arena without advancing `args`. The Nerd-string form copies the
format into terminated temporary storage, so slices need no trailing NUL.
It uses separate C argument list copies for sizing and rendering; a C formatting
failure returns an empty string. The result lives until the temporary arena is
reset or restored. `args.format(format)` is the underlying C-pointer operation.

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

Root public function exports use plain names; `pub external_name :: implementation`
selects a different C name without a forwarding wrapper. Runtime names use the
reserved `nrt_` prefix and runtime helpers are hidden in shared libraries.
The executable entry alias and internal Nerd linkage names remain compiler-owned.
C-export signature validation rejects unsupported aggregates and unresolved generic
or compound functions. This changes the binary interface: rebuild clients that
previously referenced `$`-prefixed public functions.

Imported variadic functions cannot be re-exported by forwarding their unnamed
arguments. A receiving Nerd wrapper needs a corresponding C `va_list` API.
Exports that collide with an imported foreign symbol are diagnosed; choose a
distinct public binding name.
