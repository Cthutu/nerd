# Standard Library Notes

This document tracks the current standard library surface separately from the
language manual.

The standard library is still in development, so this file should be treated as
a working reference rather than a stable contract. The language manual may use
standard library functions in small examples, but it should not duplicate this
API reference.

## Current Modules

The current standard modules live under [mods](/home/matt/nerd/mods). The
library is organised into three layers:

- `core`
  Language-support declarations that every module can rely on. This contains
  the arena API, the temporary arena, language-required traits, and core result
  types. Every module implicitly imports `core`.
- `std`
  Platform-agnostic standard-library modules. `std` APIs should provide
  portable behaviour even when their implementation delegates to platform
  modules internally.
- `os`
  Platform-specific modules such as `os.linux` and `os.windows`. Platform FFI
  declarations, syscall wrappers, constants, and operating-system details
  belong here rather than under `std`.

- `core`
  Pointer-stable arena construction, allocation, reset, and release helpers.
- `std.io`
  Basic input helpers.
- `std.mem`
  Low-level allocation wrappers.
- `std.string`
  String utilities.
- `std.traits`
  Compatibility module for common trait declarations. Language-required traits
  are canonical in `core`.
- `os.linux`
  Low-level Linux syscall-adjacent bindings.
- `os.windows`
  Windows operating-system bindings re-exported from narrower modules such as
  `os.windows.kernel`.

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

- `input(prompt: string) -> string`

### `std.mem`

Low-level allocation helpers backed by C allocation functions. These APIs should
be documented with exact ownership and lifetime rules before being presented as
stable user-facing library functions.

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
- `pr(text: string = "") -> void`
- `prn(text: string = "") -> void`
- `epr(text: string = "") -> void`
- `eprn(text: string = "") -> void`
- `Display`
  Requires `show :: fn (Self) -> string`.
- `Eq`
  Requires `eq :: fn (Self, Self) -> bool`.
- `Order`
  Requires `compare :: fn (Self, Self) -> i32`.
- `Default`
  Requires `default :: fn () -> Self`.
- `Iterator[Item]`
  Requires `next :: fn (^Self) -> Option[Item]`.
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

### `std.term`

Framebuffer rectangle APIs keep positions signed so callers can draw partially
off-screen, but use unsigned dimensions for sizes:

- `term_view(x: i32, y: i32, w: u32, h: u32) -> void`
- `term_fb_fill_rect(x: i32, y: i32, w: u32, h: u32, ...) -> void`
- `term_fb_paint_rect(x: i32, y: i32, w: u32, h: u32, ...) -> void`
- `term_fb_box(x: i32, y: i32, w: u32, h: u32, ...) -> void`

Terminal hook registration accepts optional opaque user data:

- `term_hook_simulation(interval: Duration, callback: TermSimulateFn, user_data: ^void = nil) -> void`
- `term_hook_presentation(interval: Duration, callback: TermPresentFn, user_data: ^void = nil) -> void`

The registered pointer is relayed through `TermSimulate.user_data` and
`TermPresent.user_data` each time the hook runs. `term_hooks_clear()` resets both
stored pointers to `nil`.

`term_init` accepts a `TermTooSmallPolicy` argument. The default
`PauseSimulation` policy suppresses app simulation while the terminal is below
the configured minimum size and discards key/mouse edge state, while preserving
current key-down state. `ContinueSimulation` keeps simulation hooks running but
still lets `std.term` own presentation until the terminal is large enough again.

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
