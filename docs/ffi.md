# FFI

Nerd keeps the FFI surface explicit and ABI-focused.

## Syntax

Use `ffi` with an explicit library operand, foreign symbol name, and
signature:

```nerd
ffi "c" abs (i32) -> i32
```

This declares `abs` as both:

- the Nerd-visible name used in source
- the foreign symbol name emitted for the C linker

You can also bind the foreign symbol to a different Nerd-visible name:

```nerd
seed_rng :: ffi "c" srand (u32)
```

The local binding name is `seed_rng`; the foreign symbol name is `srand`. Source
code calls `seed_rng(...)`, while generated code links against `srand`.

Use the bound form when:

- the C symbol name is unclear or does not match Nerd naming style
- you want to wrap or reserve the original foreign name
- several libraries expose similar names and local clarity matters

## Library Operand

The library operand is any compile-time expression whose value is a `string`.

These are valid:

```nerd
ffi "c" abs (i32) -> i32

libc :: "c"
puts_line :: ffi libc puts (^u8) -> i32

sqrt_fn :: ffi ("m") sqrt (f64) -> f64
```

Runtime string values are not valid library operands.

## Return Types

Omitting `-> <type>` means the foreign function returns `void`:

```nerd
seed_rng :: ffi "c" srand (u32)
```

## Varargs

`...` is only valid in FFI signatures:

```nerd
ffi "c" fcntl (i32, i32, ...) -> i32
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
