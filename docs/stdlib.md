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
- `std.files`
  Portable file handles, whole-file operations, filesystem queries, and lexical
  path processing.
- `std.math`
  Mathematical constants, scalar functions, and small geometry helper types.
- `std.memory`
  Low-level allocation wrappers.
- `std.process`
  Portable child-process execution and waiting.
- `std.text`
  String methods, Unicode scalar operations, and UTF-8 conversion.
- `std.slice`
  Borrowing slice membership with an `Eq` constraint.
- `std.time`
  Monotonic timestamps and duration construction and conversion.
- `std.traits`
  Compatibility module for common trait declarations. Language-required traits
  are canonical in `core`.
- `os.linux`
  Low-level Linux syscall-adjacent bindings, including process creation,
  executable replacement, child waiting, and process exit.
- `os.windows`
  Windows operating-system bindings re-exported from narrower modules such as
  `os.windows.kernel32`, including Win32 process creation, waiting, exit-code
  inspection, and handle management.

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

### `std.files`

- `FileError`
- `FileMode`
- `FileKind`
- `FileInfo`
- `File`
- `open(path: string, mode: FileMode = FileMode.Read) -> File\FileError`
- `File.read(output: []u8) -> usize\FileError`
- `File.write(input: []u8) -> usize\FileError`
- `File.write_all(input: []u8) -> void\FileError`
- `File.flush() -> void\FileError`
- `File.close() -> void\FileError`
- `read_bytes(path, output_arena = temp_arena) -> []u8\FileError`
- `read_text(path, output_arena = temp_arena) -> string\FileError`
- `write_bytes(path, input) -> void\FileError`
- `write_text(path, text) -> void\FileError`
- `remove(path) -> void\FileError`
- `file_info(path) -> FileInfo\FileError`
- `exists(path) -> bool`
- `is_file(path) -> bool`
- `is_directory(path) -> bool`
- `is_symlink(path) -> bool`
- `file_name(path) -> string`
- `parent(path) -> string`
- `extension(path) -> string`
- `stem(path) -> string`
- `replace_extension(path, extension, output_arena = temp_arena) -> string`
- `is_absolute(path) -> bool`
- `join(left, right, output_arena = temp_arena) -> string`

`File` owns its platform handle and should be closed once. Whole-file reads and
constructive path functions allocate into the supplied arena, which defaults to
`temp_arena`; their returned views are invalidated when that arena is restored,
reset, or released. Path component functions return borrowed views. Filesystem
failures use `FileError`; recognised conditions have portable enum variants and
`System { code }` preserves an otherwise unmapped native error code.

### `std.memory`

Runtime-backed `alloc`, `realloc`, `free`, and `alloc_size` operate on whole
owned byte-slice allocations. Free each allocation exactly once. `realloc`
replaces the old view and preserves bytes up to the smaller size. Zero-sized
allocations still require freeing. Allocation failure terminates the process.

`kb`, `mb`, and `gb` convert binary units to bytes. `align_up` requires a nonzero
alignment and a result that fits in `usize`. `leak` excludes an allocation from
reports without freeing it. `print_leaks(show_message_on_no_leaks = no)` reports
live allocations in debug builds and is a no-op with the same API in release.

### `std.process`

- `run_string(command: string) -> i32`
- `run_array(arguments: []string) -> i32`
- `run :: fn { run_string run_array }`

`run_string` starts a platform shell, waits for the command to finish, and
returns its exit status. Linux commands run through `/bin/sh -c`; Windows
commands run through `cmd.exe /C`. `run_array` starts the executable named by
element zero and passes every later slice element as one process argument. The
`run` compound function selects either operation from the argument type. A
negative return value reports that the process could not be created or waited
for. On Linux, termination by a signal returns `128` plus the signal number.
Windows array arguments are quoted when empty or containing spaces, tabs, or
double quotes; plain arguments such as `/C` are passed without quotes.

### `std.math`

- `pi`
  Untyped floating-point literal for the circle constant.
- `sin(value: f64) -> f64`
- `cos(value: f64) -> f64`
- `tan(value: f64) -> f64`
- `Point[T]`
- `Rect[U]`
- `PointI32`
- `RectI32`
- `Rect.right() -> U`
- `Rect.bottom() -> U`
- `Rect.is_empty() -> bool`
- `Rect.intersection(other: Self) -> Self`

The trigonometric functions take radians and return `f64` values.

### `core`

- `arena`
- `arena(num_bytes)` and `arena(num_bytes, increment)` built-in construction
  syntax
- `arena.alloc[T]() -> ^T`
- `arena.alloc_array[T](count: usize) -> []T`
- `arena.alloc_bytes(count: usize) -> []u8`
- `arena.pr(text: string = "") -> string`
- `arena.prn(text: string = "") -> string`
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
  Requires `show :: fn (self: Self) -> string`.
- `Eq`
  Requires `eq :: fn (self: Self, other: Self) -> bool`.
- `Order`
  Requires `compare :: fn (self: Self, other: Self) -> i32`.
- `Default`
  Requires `default :: fn () -> Self`.
- `Iterator[Item]`
  Requires `next :: fn (self: ^Self) -> ?Item`.

Optional `?T` and result `T\E` are language types rather than declarations in
`core`.

Arena sizes are rounded up to the platform page size by the runtime. Arena
construction reserves one 4 GiB virtual address range and commits pages on
demand, so earlier allocation pointers do not move when the arena grows. Marks
and offsets are 32-bit values within that range. `restore(mark)` invalidates
allocations made after the mark, `reset()` invalidates all previous allocations
and reuses storage, and `done()` releases the reserved arena range.

`pr` places text in the receiving arena and returns that arena-backed string.
`prn` does the same while appending a newline. Their results remain valid until
the receiving arena is restored past the allocation, reset, or released.

### `std.text`

- `Rune.is_ascii()`, `Rune.is_valid()`, and `Rune.utf8_length()`
- `Rune.utf8_encode(output: []u8) -> usize`
- `Rune.display_width() -> i32`
- `utf8_decode(bytes: []u8) -> (Rune, usize)`
- `string.utf8_validate() -> void\Utf8Error`
- `string.is_empty()`, `string.starts_with(prefix)`, and `string.ends_with(suffix)`
- `string.split(separator: string) -> [..]string`
- `string.trim()`, `string.trim_start()`, and `string.trim_end()`
- `string.trim_whitespace()` and `string.trim_null_terminated()`
- `string.c_string() -> ^i8` and `string.display_width() -> i32`

`split` returns an owned array of borrowed string views; free the array when
finished, keeping the original string storage alive while using its parts.
Trimming returns borrowed views and removes NUL bytes and ASCII whitespace.
`trim_null_terminated` stops at the first NUL. `c_string` copies into the temporary
arena and appends a NUL; the pointer expires when that arena is restored or reset.

`utf8_decode` returns U+FFFD and a zero byte count for invalid or empty input.
It rejects overlong forms, surrogates, invalid continuation bytes, and values
outside Unicode. A valid encoded U+FFFD is accepted. `utf8_encode` replaces
invalid scalar values with U+FFFD and returns zero without writing when the
output is too small. Display width uses the currently supported wide-character
ranges, rather than a complete Unicode grapheme-width implementation.

The old `std.string` and `std.utf8` modules have been absorbed into `std.text`.

### `std.slice`

`slice.contains(value)` borrows its receiver and requires `T: Eq`. Matching uses
value equality, including the content equality of nested slices and strings.

### `std.time`

`Instant` and `Duration` are nanosecond counts. Use `instant.add(duration)`,
`duration.to_secs()`, `to_ms()`, `to_us()`, `to_ns()`, and `secs()` for the
method-based API. `secs()` returns fractional seconds; the other conversions
return whole units. Constructors remain `from_secs`, `from_ms`, `from_us`, and
`from_ns`. `elapsed(start, finish)`, `now()`, and `sleep_ms()` remain free functions.

### `std.term`

See the learner-facing [`std.term` manual](manual/std-term.md) for lifecycle,
input, framebuffer, and capability examples.

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
FrameContext\FrameError` is the public way to resolve platform handles;
detached, closed, and invalid frames report explicit `FrameError` values.

`Frame.resizable = no` asks the window manager to prevent user resizing.
On Windows, fixed frames remove the resize/maximise styles and report equal
minimum and maximum tracking sizes through `WM_GETMINMAXINFO`. These limits
use the current outer window dimensions, including its non-client border.
Programmatic size changes through `apply`, fullscreen transitions, and restoring
a minimised window bypass the tracking limits; normal windowed operation
restores the constraint.

`std.gfx` owns frame-attached pixel layers and pixel presentation. It may use an
OpenGL texture path internally when a frame has a usable context, with software
platform presentation as the fallback.

Create a graphics system with `GfxSystem.init()` and release it with
`GfxSystem.done()`. A `PixelLayer` belongs to the graphics system that created
it. `GfxSystem.create_pixel_layer(^frame, mode)` returns a stable
`GfxLayerHandle` whose ID increases from one and is never reused.
`GfxSystem.get_pixel_layer(handle) -> ^PixelLayer\GfxError` borrows the typed
layer; it reports `LayerNotFound` for an unknown or deleted ID and
`WrongLayerType` when the handle names another kind of layer.
`GfxSystem.destroy_layer(handle) -> bool\GfxError` removes the layer and
releases its storage.

Internally, `GfxSystem` keeps one ordered metadata array for every layer. Each
entry stores the stable ID, `GfxLayerType`, and an index into that type's side
array. Pixel data therefore remains in a homogeneous pixel-layer array while
the metadata array defines render order. Destruction shifts the relevant side
array and decrements later indexes; stable IDs and handles do not change. This
layout leaves room for OpenGL, Vulkan, and other layer side arrays without
making their APIs part of `PixelLayer`. Borrowed layer pointers can be
invalidated by later layer creation or destruction, so retain the handle and
resolve it again when needed.

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

`PixelLayer.pixel_width()` and `PixelLayer.pixel_height()` return the current
virtual dimensions as `u32` values.

`GfxSystem.render(^frame)` resizes `FitToWindow` layers as needed and presents
the layers for that frame. Multiple pixel layers are composited in layer order
into a temporary frame-sized buffer before presentation.

`std.opengl` owns portable OpenGL aliases, constants, raw command wrappers,
command address loading, and current/swap helpers. Call `gl_init(^Frame)` after
the frame has an OpenGL context and before using any loaded command. Call
`gl_done(^Frame)` when finished with that frame's GL surface.

The supported surface is a deliberately practical OpenGL 3.0 subset rather
than a generated binding for every core command. It includes shader creation,
compilation, linking, logs, and uniforms; buffer objects and mapped ranges;
vertex-array objects and attributes; 2D and 3D textures and mipmap generation;
framebuffers, renderbuffers, multisample storage, blitting, texture layers, and
multiple draw buffers; separate colour/alpha blending; separate front/back
stencil state; indexed extension strings; and indexed and non-indexed drawing.
The module retains its small OpenGL 1.1 compatibility surface for existing
programs. Uniform uploads cover scalar/vector `glUniform*f` and `glUniform*i`,
pointer-array `glUniform*fv` and `glUniform*iv`, and float matrix
`glUniformMatrix*fv` wrappers.

Here, portable means that user code sees the same OpenGL names and ABI shapes
on Windows and Linux. Aliases such as `GLenum`, `GLuint`, `GLsizei`,
`GLintptr`, and `GLsizeiptr` fix the width and signedness expected by OpenGL;
constants such as `GL_ARRAY_BUFFER` use the Khronos-defined numeric values.
They do not represent Nerd-specific types or translated platform constants.

Command loading remains platform-specific: Windows obtains addresses through
WGL and Linux through GLX after a context exists. Loading is all-or-nothing for
the documented subset: `gl_init` returns `no` if any required address is
unavailable, and no partially loaded command table is published. Before a
successful load, and again after `gl_done` or `gl_reset_commands`, wrappers use
fail-fast stubs instead of calling a missing or stale address. A constant being
present in the module still does not guarantee that the current driver provides
the associated feature.

`std.frame` owns windows and OpenGL context lifecycle. `std.opengl` owns the raw
portable command API and current/swap conveniences over that context.
`std.gfx` owns higher-level packed-pixel presentation; it is not an alternate
home for raw OpenGL commands.

### `std.traits`

- `Display`
  Requires `show :: fn (self: Self) -> string`.
- `Eq`
  Requires `eq :: fn (self: Self, other: Self) -> bool`.
- `Order`
  Requires `compare :: fn (self: Self, other: Self) -> i32`.
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
# `std.atomics`

`std.atomics` exports `AtomicLoadOrder`, `AtomicStoreOrder`, `AtomicOrder`, and
`AtomicCompareExchangeResult[T]`, plus explicit atomic operations and inherent
methods on `atomic[T]`. Ordinary syntax and omitted order arguments are
sequentially consistent. Weak compare-exchange is intended for retry loops and
may report `NotExchanged(observed)` spuriously. Atomic pointer operations do not
manage pointee reclamation. Compare-exchange takes separate compile-time
success and failure orders; the failure order may not be stronger than the
success order. The compiler lowers these methods to dedicated atomic HIR and
LLVM operations, while the public types and signatures remain Nerd source.
