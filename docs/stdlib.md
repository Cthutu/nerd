# Standard Library Notes

This document tracks the current standard library surface separately from the
language manual.

The standard library is still in development, so this file should be treated as
a working reference rather than a stable contract. The language manual may use
standard library functions in small examples, but it should not duplicate this
API reference.

## Current Modules

The current standard modules live under [mods](/home/matt/nerd/mods). The
library is moving toward three layers:

- `core`
  Language-adjacent helpers with minimal dependencies. This contains the arena
  API, the temporary arena, language-required traits, and core result types.
  Every module implicitly imports `core`.
- `std`
  Portable higher-level utilities.
- `sys`
  Platform and OS bindings such as `sys.linux`. Future platform modules belong
  here rather than under `std`.

- `core`
  Pointer-stable arena construction, allocation, reset, and release helpers.
- `std.io`
  Basic input and output helpers.
- `std.mem`
  Low-level allocation wrappers.
- `std.arena`
  Arena allocation helpers.
- `std.string`
  String utilities.
- `std.traits`
  Compatibility module for common trait declarations. Language-required traits
  are canonical in `core`.
- `sys.linux`
  Low-level Linux C and syscall-adjacent bindings.

The repository also contains early `std.random` source work. Treat it as
experimental until its dependencies and syntax surface are covered by the
roadmap's horizontal feature policy.

## Documentation Policy

- Keep core language rules in [manual.md](/home/matt/nerd/docs/manual.md).
- Keep implementation notes in the existing compiler documentation.
- Keep public standard library signatures, ownership notes, and examples here.
- Mark unstable or experimental APIs clearly when this document becomes a fuller
  reference.

## Known Ownership-Sensitive APIs

Dynamic arrays and memory helpers should document ownership explicitly.

Examples:

- whether the caller must call `.free()`
- whether a returned slice borrows existing storage
- whether a function may reallocate a dynamic array
- whether a pointer-to-slice cast such as `p.as([]T, count)` creates a borrowed
  view rather than owned storage

## Initial API Inventory

This inventory is intentionally brief until the standard library settles.

### `std.io`

- `pr(text: string) -> void`
- `prn(text: string) -> void`
- `input(prompt: string) -> string`

### `std.mem`

Low-level allocation helpers backed by C allocation functions. These APIs should
be documented with exact ownership and lifetime rules before being presented as
stable user-facing library functions.

### `std.arena`

Arena helpers built on top of `std.mem`.

This module remains as a compatibility/reference implementation. New code
should prefer the built-in `arena` type exposed through `core`.

### `core`

- `arena`
- `arena(num_bytes)` and `arena(num_bytes, increment)` built-in construction
  syntax
- `arena.alloc[T]() -> ^T`
- `arena.alloc_array[T](count: usize) -> []T`
- `arena.alloc_bytes(count: usize) -> []u8`
- `arena.reset()`
- `arena.mark() -> u32`
- `arena.restore(mark: u32)`
- `arena.done()`
- `temp_arena.reset()`
- `Display`
  Requires `show :: fn (Self) -> string`.
- `Eq`
  Requires `eq :: fn (Self, Self) -> bool`.
- `Order`
  Requires `compare :: fn (Self, Self) -> i32`.
- `Default`
  Requires `default :: fn () -> Self`.
- `Option[T]`
  Tagged enum with `None` and `Some(T)`.
- `Result[T, E]`
  Tagged enum with `Ok(T)` and `Err(E)`.

Arena sizes are rounded up to the platform page size by the runtime. Arena
construction reserves one 4 GiB virtual address range and commits pages on
demand, so earlier allocation pointers do not move when the arena grows. Marks
and offsets are 32-bit values within that range. `restore(mark)` invalidates
allocations made after the mark, `reset()` invalidates all previous allocations
and reuses storage, and `done()` releases the reserved arena range.

### `std.string`

- `split(s: string, sep: string) -> [..]string`

`split` returns a dynamic array and the caller is responsible for freeing that
array when it is no longer needed.

### `std.traits`

- `Display`
  Requires `show :: fn (Self) -> string`.
- `Eq`
  Requires `eq :: fn (Self, Self) -> bool`.
- `Order`
  Requires `compare :: fn (Self, Self) -> i32`.
- `Default`
  Requires `default :: fn () -> Self`.

This module remains as a compatibility location while existing code moves to
`core`. Language features that recognise built-in traits use the canonical
declarations in `core`, not duplicate same-named declarations elsewhere.

### Experimental `std.random`

The tracked `mods/std/random.n` file is not part of the stable standard-library
inventory yet. Before documenting it as supported, make sure the module builds
through the normal module pipeline and that its required language features,
supporting modules, tests, and manual notes have landed together.
