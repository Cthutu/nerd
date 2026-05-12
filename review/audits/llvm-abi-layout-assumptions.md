# LLVM ABI And Layout Assumptions

This audit records the current target-layout and runtime ABI assumptions in the
LLVM back end. It is evidence for the next MS5 slices: a layout context,
focussed ABI tests, and a single source for runtime helper declarations.

## Current Target Model

The LLVM generator currently assumes a 64-bit target without naming that target
explicitly. There is no generated `target triple` or `target datalayout`; clang
chooses those when compiling the combined `.link.ll` file.

The compiler-side sizing helpers in `src/compiler/llvm/llvm.c` encode these
assumptions directly:

- pointers, function values, and dynamic-array handles are 64 bits
- `isize` and `usize` lower to `i64`
- strings and slices are two-word records, `{ ptr, i64 }`
- dynamic-array headers are three-word records, `{ ptr, i64, i64 }`
- enum tags are `i64`
- enum payload storage is rounded up to a 64-bit boundary
- unions are represented as an integer storage cell large enough for the
  largest payload

This is acceptable for the current Linux/Windows 64-bit development target, but
it is not yet a portable target-layout contract.

## Primitive And Pointer Types

Primitive type spelling is centralised in `llvm_append_type`, but the policy is
still implicit:

| Nerd type family | LLVM spelling |
| --- | --- |
| `bool` | `i1` |
| `i8`/`u8` | `i8` |
| `i16`/`u16` | `i16` |
| `i32`/`u32`/untyped integer | `i32` |
| `i64`/`u64` | `i64` |
| `isize`/`usize` | `i64` |
| `f32` | `float` |
| `f64`/untyped float | `double` |
| pointers, function pointers, dynamic arrays | `ptr` |

The back end uses opaque LLVM pointers, which keeps pointer spelling simple.
The missing piece is a target layout object that can say how large pointer-sized
integers are and what alignment should be used when storing or loading them.

## Records, Aggregates, And Enums

Tuples and plexes lower to literal LLVM struct types such as `{ i32, i32 }`.
Fixed arrays lower to `[N x T]`. Strings and slices lower to `{ ptr, i64 }`.

The current manual storage-size helper sums field bit widths for tuples and
plexes. That ignores target-specific padding and natural alignment. It is safe
only while those sizes are used for internal storage estimates where the
generated LLVM type is also a simple packed-looking literal, and it becomes
risky for ABI decisions, allocation sizes, and future FFI aggregate passing.

Enums lower as `{ i64, payload }`, where the payload is an integer storage cell
large enough for the largest variant payload and rounded to 64 bits. Typed plex
patterns already exercise larger enum payloads, for example `{ i64, i192 }`.

Unions lower to an integer storage cell. The payload conversion code bitcasts,
extends, or truncates values to that storage width. This is target-independent
only for scalar-like payloads; aggregate payload layout must be treated
carefully if unions grow beyond the current tested shapes.

## Runtime ABI

`data/nrt.c` is compiled to an object and embedded into the compiler. The LLVM
backend emits handwritten declarations for the runtime functions it calls.

Runtime helpers that exchange Nerd strings use a pointer/scalar ABI:

- generated string values are `{ ptr, i64 }`
- generated LLVM stores string values into temporary stack slots when calling
  helpers
- helpers accept `NerdString*`/`const NerdString*`, emitted as `ptr`

This avoids relying on C aggregate-by-value calling conventions for strings.
The same idea should be kept for all runtime-facing aggregate helpers.

The current handwritten runtime declarations include:

- `string_eq(ptr, ptr) -> i1`
- string builder helpers with `i64` marks
- `to_string$...` helpers
- `nerd_assert(i1, ptr, i32, ptr)`
- libc allocation helpers: `malloc(i64)`, `realloc(ptr, i64)`, `free(ptr)`

The declarations match the current `nrt.c` on the assumed 64-bit target, but
the duplication is fragile. A future slice should generate these declarations
from one table that also documents the runtime C signature.

## Dynamic Arrays

Dynamic arrays are represented as nullable pointers to a header:

```llvm
{ ptr, i64, i64 } ; data, count, capacity
```

The generator allocates the header with a hardcoded byte count of `24`, then
allocates element storage with `item_count * llvm_type_storage_bytes(item)`.
Count, capacity, `reserve`, `push`, `pop`, `clear`, and `free` all operate on
`i64` values.

This matches the current 64-bit runtime model. It should move behind the layout
context so the header type, header byte size, count type, and allocation size
type stay in one place.

## Function ABI And Varargs

Nerd functions currently pass and return lowered LLVM values directly. That
means aggregate parameters can appear as `{ ... }` by value in generated LLVM.
This is fine for Nerd-to-Nerd calls in one generated LLVM module, but it should
not be assumed to match a C ABI for external functions.

FFI functions are declared from their semantic function type, including varargs
when the function type carries `STF_FunctionVarargs`. Scalar FFI is covered by
existing tests. Aggregate FFI and vararg promotion policy need explicit tests
and a documented support limit.

## Alignment

The generator sometimes emits fixed `align 4` on stack slots, loads, and stores.
Other loads and stores omit alignment entirely and let LLVM infer conservative
behaviour.

Fixed `align 4` is not a general layout policy. It is too weak for 64-bit values
and too strong for some byte-oriented values. A layout context should provide
alignment decisions for stack slots and memory operations, or the generator
should deliberately omit alignment where there is no known target-specific
answer yet.

## Open Risks

- The compiler currently assumes 64-bit pointer-sized integers.
- Manual size computation does not model padding.
- Dynamic-array header byte size is duplicated as structure spelling plus
  literal `24`.
- Runtime declarations are duplicated by hand in LLVM generation.
- Aggregate FFI passing and returning is not yet a documented contract.
- Vararg lowering relies on clang/LLVM accepting the emitted scalar types, but
  default C promotions are not represented as an explicit policy.
- No target triple or data layout is emitted, so clang owns the final target
  choice.

## Recommended MS5 Order

1. Introduce a backend layout context with the current 64-bit defaults.
2. Move primitive spelling, pointer-sized widths, string/slice shapes,
   dynamic-array header shape, and common alignment decisions through that
   context.
3. Add ABI snapshot and run tests before changing behaviour.
4. Generate runtime helper declarations from a single table.
5. Document the supported target contract and unsupported FFI aggregate cases.

## Follow-Up: Layout Context

The first implementation slice introduced `LlvmLayout` in the LLVM generator
and threaded it into function emission. It currently preserves the existing
64-bit target contract while centralising:

- pointer and pointer-sized integer bit widths
- pointer, size, and enum-tag type spelling
- string/slice aggregate spelling
- dynamic-array header spelling and byte size
- enum payload alignment width

The remaining layout work is to move memory-operation alignment and any
runtime-helper declaration spelling through the same source of truth.

## Follow-Up: ABI Regression Coverage

`tests/llvm/029-abi-layout-runtime.ll` now pins a focussed cross-section of the
current ABI contract:

- string/slice spelling as `{ ptr, i64 }`
- C FFI declarations for scalar and vararg calls
- runtime string-builder and `to_string$...` declarations
- `string_eq` pointer ABI
- dynamic-array header allocation as `{ ptr, i64, i64 }` and 24 bytes
- dynamic-array element allocation sizing for aggregate elements
- enum tags as `i64` with a widened integer payload cell
- aggregate fields in plex values and enum payloads
- pointer-to-slice casts and C string pointers
