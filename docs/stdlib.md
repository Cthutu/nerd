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
- `std.utf8`
  UTF-8 conversion and display-width utilities.
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

- `from_null_terminated(text: string) -> string`
- `split(s: string, sep: string) -> [..]string`
- `string.trim() -> string`
- `string.trim_start() -> string`
- `string.trim_end() -> string`
- `string.trim_null_terminated() -> string`
- `string.trim_whitespace() -> string`

`from_null_terminated` and `trim_null_terminated` return a borrowed view ending
before the first zero byte. They are useful after casting fixed C-style byte
buffers to `string`, for example `str.from_null_terminated(buffer.as(string))`.
The `trim` family returns borrowed views with leading and/or trailing zero bytes
and ASCII whitespace removed. `trim_whitespace` is kept as an alias for `trim`.
`split` returns a dynamic array and the caller is responsible for freeing that
array when it is no longer needed.

### `std.utf8`

- `decode_at(text: string, index: usize) -> (u32, usize)`
- `decode_bytes_at(bytes: []u8, index: usize) -> (u32, usize)`
- `decode_first_byte_slice(bytes: []u8) -> u32`
- `encoded_count(codepoint: u32) -> usize`
- `encode_to(codepoint: u32, output: []u8) -> usize`
- `codepoint_display_width(codepoint: u32) -> i32`
- `display_width(text: string) -> i32`

Decode functions return the decoded UTF-32 codepoint and the number of bytes
consumed. A byte count of zero means the requested offset was at or beyond the
end of the input. Malformed input decodes to `REPLACEMENT_CODEPOINT` and
consumes one byte. `encode_to` returns zero without writing when the output
slice is too small.

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

### `std.frame`, `std.gfx`, and `std.opengl`

`std.frame` owns native windows, input/events, frame lifecycle, fullscreen
state, and platform context creation. `Frame.context() ->
Result[FrameContext, FrameError]` is the public way to resolve platform handles;
detached, closed, and invalid frames report explicit `FrameError` values.

`std.gfx` owns frame-attached pixel layers and pixel presentation. It may use an
OpenGL texture path internally when a frame has a usable context, with software
platform presentation as the fallback.

Create a graphics system with `GfxSystem.init()` and release it with
`GfxSystem.done()`. A `PixelLayer` belongs to the graphics system that created
it; callers should keep the returned `PixelLayerHandle` and use
`GfxSystem.pixel(handle)` to borrow the layer. `GfxSystem.destroy(handle)`
removes the layer and releases its pixel storage. Layer pointers are borrowed
from the graphics system and can be invalidated by layer creation or
destruction.

Pixel layers use two sizing modes:

- `PixelLayerMode.FitToWindow { pixel_scale }`
  The layer buffer follows the frame size divided by `pixel_scale`. A zero
  scale is treated as one. Presentation starts at the top-left of the frame and
  does not intentionally letterbox.
- `PixelLayerMode.FixedSizeAutoScale { width, height }`
  The layer keeps a fixed virtual size. Presentation uses the largest integer
  scale that fits inside the frame, with unused frame space letterboxed around
  the centred layer. Zero width or height is treated as one.

`PixelLayer.pixels()` returns a borrowed contiguous `[]u32` view of the whole
virtual buffer. Pixels are `0xAARRGGBB`; alpha is used when multiple layers are
composited. `clear`, `put`, and `fill` mutate the buffer in virtual pixel
coordinates. `fill` clips rectangles to the layer bounds. `paint(area, painter,
user = nil)` clips `area`, then calls `painter` with a borrowed slice starting
at the first clipped pixel, the clipped width and height, the full layer stride,
and the optional opaque user pointer.

`GfxSystem.render(^frame)` resizes `FitToWindow` layers as needed and presents
the layers for that frame. Multiple pixel layers are composited in layer order
into a temporary frame-sized buffer before presentation.

`std.opengl` owns portable OpenGL aliases, constants, command wrappers, command
address loading, and current/swap helpers. Call `gl_init(^Frame)` before using
loaded OpenGL commands and `gl_done(^Frame)` when finished with that frame's GL
surface.

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
