# TODO

## Native `std.image`

Port image loading into Nerd as a standard-library module rather than wrapping a
C implementation. Use `stb_image.h` as the behavioural reference, but write the
library in idiomatic Nerd with methods, default parameters, source tests, and
pattern matching.

### Project Principles

- [ ] Keep `std.image` implemented in Nerd source, not as an FFI wrapper around
      `stb_image.h`.
- [ ] Use `stb_image.h` as the compatibility reference for accepted formats,
      channel conversion behaviour, and edge-case expectations.
- [ ] Prefer clear Nerd code over preserving C preprocessor structure,
      macro-heavy helpers, or pointer-arithmetic-heavy implementation details.
- [ ] Keep decoder allocations explicit and deterministic.
- [ ] Make image ownership explicit before landing the first decoder.
- [ ] Use source-level unit tests in module parts so private helpers can be
      tested directly.

### Target Module Shape

- [x] Create `mods/std/image/mod.n` as the public entry point.
- [ ] Split implementation across module parts:
  - [x] `image.n`: public `Image`, `ImageError`, loading API, and channel
        conversion.
  - [x] `reader.n`: byte reader/cursor helpers over `[]u8`.
  - [ ] `png.n`: PNG container, zlib/deflate, filters, and colour conversion.
  - [ ] `jpeg.n`: JPEG markers, entropy decode, IDCT, and colour conversion.
  - [ ] `bmp.n`: BMP container and pixel formats.
  - [ ] `tga.n`: TGA uncompressed/RLE formats.
  - [ ] `tests.n`: source-level unit tests and tiny embedded fixtures.
- [ ] Keep the public import as `use std.image`.
- [ ] Keep implementation details private unless a helper type is intentionally
      part of the public API.

### Companion `std.compress` Module

- [ ] Create `mods/std/compress/mod.n` before PNG zlib/deflate work starts.
- [ ] Keep compression algorithms separate from image decoding so PNG can use
      the same public decompression APIs as other callers.
- [ ] Design a small Nerd-native API using `Result`, slices, default
      parameters, and method-style helpers where useful.
- [ ] Start with zlib-wrapped deflate because PNG requires it.
- [ ] Keep raw deflate and gzip as explicit future extensions, not accidental
      behaviours.
- [ ] Put compression implementation tests in `test { ... }` sections inside
      `std.compress` module parts.

API sketch:

```nerd
use std.compress

bytes := compressed.deflate_zlib().expect("decompression failed")
```

Possible public shape:

```nerd
CompressionError :: enum {
    UnsupportedFormat
    InvalidData { message string }
    UnexpectedEnd
    OutputLimitExceeded
}

impl []u8 {
    pub deflate_zlib :: fn (bytes: Self,
                            max_output_bytes: usize = 0) -> Result[[..]u8, CompressionError]
}
```

### Public API Sketch

```nerd
Image :: plex {
    width    i32
    height   i32
    channels i32
    data     []u8
}

ImageError :: enum {
    UnsupportedFormat
    InvalidData { message string }
    UnexpectedEnd
    OutOfMemory
}

impl Image {
    pub load :: fn (path: string, desired_channels := 0) -> Result[Self, ImageError]

    pub load_bytes :: fn (bytes: []u8,
                          desired_channels := 0) -> Result[Self, ImageError]

    pub free :: fn (self: ^Self) {
    }
}
```

### Testing Pattern

Use source tests inside the `std.image` module parts:

```nerd
test "png signature" {
    assert is_png(@embed("fixtures/tiny.png"))
}
```

`just test` discovers source tests in `mods/std`, so standard-library coverage
should live in module-local `test` sections rather than `.cmd` wrappers.

### Milestone 1: Module Skeleton

- [x] Create `mods/std/image/mod.n`.
- [x] Add a minimal `Image` plex.
- [x] Add `ImageError`.
- [x] Decide whether `Image.data` is heap-owned, temp-arena-owned, or
      caller-arena-owned.
- [x] Add `Image.free()` only if the chosen ownership model requires it.
- [x] Add placeholder `Image.load_bytes()` returning `UnsupportedFormat`.
- [x] Add placeholder `Image.load()` returning `UnsupportedFormat`.
- [x] Add an initial source test proving the stdlib source-test suite discovers
      `std.image`.

### Milestone 2: Reader Helpers

- [x] Add a private `ImageReader` over `[]u8`.
- [x] Implement `remaining()`, `position()`, and `done()`.
- [x] Implement `read_u8()`.
- [x] Implement big-endian reads for PNG: `read_be_u16()` and `read_be_u32()`.
- [x] Implement little-endian reads for BMP/TGA: `read_le_u16()` and
      `read_le_u32()`.
- [x] Implement `take(count)` returning a slice or `UnexpectedEnd`.
- [x] Add source tests for empty input, exact reads, short reads, and cursor
      advancement.

### Milestone 3: Format Detection

- [x] Add `ImageFormat` enum.
- [x] Implement PNG signature detection.
- [x] Implement JPEG SOI marker detection.
- [x] Implement BMP signature detection.
- [ ] Implement TGA heuristic detection only once the TGA decoder starts.
- [x] Route `Image.load_bytes()` through format detection.
- [x] Return `UnsupportedFormat` for unknown data.
- [x] Add tests using embedded tiny signatures and invalid byte sequences.

### Milestone 4: PNG Container

- [x] Parse PNG signature.
- [x] Parse chunk headers: length, chunk type, data, CRC.
- [x] Parse `IHDR`.
- [x] Collect one or more `IDAT` chunks.
- [x] Stop at `IEND`.
- [x] Validate mandatory chunk order enough to reject malformed inputs.
- [x] Decide CRC policy: strict by default, optional relaxed mode later if
      useful.
- [x] Add source tests for valid `IHDR`, missing `IEND`, truncated chunks, and
      unexpected chunk order.

### Milestone 5: `std.compress` Zlib And Deflate

- [ ] Create the `std.compress` folder module.
- [ ] Add `CompressionError`.
- [ ] Add placeholder public zlib/deflate API.
- [ ] Parse zlib header and checksum fields.
- [ ] Implement stored blocks.
- [ ] Implement fixed Huffman blocks.
- [ ] Implement dynamic Huffman blocks.
- [ ] Implement length/distance copy logic.
- [ ] Validate end-of-block handling.
- [ ] Validate truncated stream failures.
- [ ] Add `std.compress` source tests for small compressed streams independent
      of PNG.
- [ ] Add `std.image` tests against embedded PNG `IDAT` payloads once PNG chunk
      parsing exists.

### Milestone 6: PNG Filters And RGBA Decode

- [ ] Implement filter type 0: None.
- [ ] Implement filter type 1: Sub.
- [ ] Implement filter type 2: Up.
- [ ] Implement filter type 3: Average.
- [ ] Implement filter type 4: Paeth.
- [ ] Decode non-interlaced 8-bit RGB.
- [ ] Decode non-interlaced 8-bit RGBA.
- [ ] Convert RGB to desired channels `0`, `3`, and `4`.
- [ ] Add source tests for every filter using tiny handcrafted scanlines.
- [ ] Add tests for embedded 1x1 and 2x2 PNG fixtures with known pixels.

### Milestone 7: More PNG Colour Modes

- [ ] Decode grayscale 8-bit.
- [ ] Decode grayscale with alpha.
- [ ] Decode indexed colour with `PLTE`.
- [ ] Apply `tRNS` transparency where supported.
- [ ] Add channel conversion for desired channels `1` and `2`.
- [ ] Decide whether to support 16-bit PNG now or explicitly reject it with a
      stable `UnsupportedFormat` reason.
- [ ] Decide whether to support Adam7 interlace now or explicitly reject it with
      a stable `UnsupportedFormat` reason.
- [ ] Add fixtures for grayscale, grayscale-alpha, indexed, and transparent
      PNGs.

### Milestone 8: File Loading

- [ ] Implement `Image.load(path, desired_channels := 0)` on top of `std.io`.
- [ ] Ensure path loading reports file errors distinctly from decode errors if
      the standard error model supports it.
- [ ] Add source-relative fixture tests using real files.
- [ ] Add command regression for missing files.
- [ ] Add command regression for corrupted file contents.

### Milestone 9: Image Viewer Integration

- [ ] Replace the generated `DemoImage` path in
      `examples/image_viewer/image_viewer.n` with `std.image.Image.load(...)`
      once file loading works.
- [ ] Add a command-line argument for the image path when `main(args)` is
      suitable for examples.
- [ ] Keep a default asset in `examples/image_viewer/assets/`.
- [ ] Add nearest-neighbour fit-to-window presentation for loaded images.
- [ ] Add optional checkerboard background for transparent PNGs.
- [ ] Add `flip_vertically := no` as either a load option or a viewer option.

### Milestone 10: JPEG Baseline

- [ ] Parse JPEG markers.
- [ ] Parse SOF0 frame headers.
- [ ] Parse DQT quantisation tables.
- [ ] Parse DHT Huffman tables.
- [ ] Parse SOS scan headers.
- [ ] Implement entropy decoding for baseline sequential JPEG.
- [ ] Implement dequantisation and IDCT.
- [ ] Implement YCbCr to RGB conversion.
- [ ] Add desired-channel conversion.
- [ ] Add tests with embedded baseline JPEG fixtures.
- [ ] Explicitly reject progressive JPEG until supported.

### Milestone 11: BMP

- [ ] Parse BMP file header.
- [ ] Parse DIB headers needed for common Windows BMPs.
- [ ] Decode 24-bit BGR.
- [ ] Decode 32-bit BGRA/BGRX.
- [ ] Handle row padding.
- [ ] Handle top-down and bottom-up images.
- [ ] Add fixtures for 24-bit, 32-bit, and padded rows.

### Milestone 12: TGA

- [ ] Parse TGA header.
- [ ] Decode uncompressed true-colour images.
- [ ] Decode RLE true-colour images.
- [ ] Handle origin flags.
- [ ] Convert BGR/BGRA to RGB/RGBA.
- [ ] Add fixtures for uncompressed and RLE TGA.

### Milestone 13: Optional `stb_image` Formats

- [ ] Decide whether to support HDR.
- [ ] Decide whether to support GIF.
- [ ] Decide whether to support PSD.
- [ ] Decide whether to support PIC.
- [ ] Decide whether to support PNM.
- [ ] For each unsupported format, add a stable `UnsupportedFormat` test.

### Demo Assets

- [x] Add a PNG demo asset for `examples/image_viewer`.
- [x] Add a JPEG demo asset for `examples/image_viewer`.
- [x] Record asset sources in `examples/image_viewer/assets/SOURCES.md`.
- [ ] Replace placeholder assets with more representative images if licensing
      and size are appropriate.

### Open Questions

- [ ] Should decoded pixel data be `[]u8`, `[..]u8`, or an owned allocation type
      once ownership APIs settle?
- [ ] Should `std.image` expose packed `[]u32` helpers for `std.gfx`, or should
      it stay byte-channel based and let callers convert?
- [ ] Should `Image.load()` default to source-relative paths, cwd-relative
      paths, or explicit caller paths only?
- [ ] Should CRC checking be mandatory for PNG?
- [ ] Should the first implementation support 16-bit PNG or reject it cleanly?
