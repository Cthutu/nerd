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
- [ ] Limit active format work to PNG and JPEG until those decoders are solid.
- [ ] Defer file/path loading until Nerd has a stable standard filesystem
      module.

### Target Module Shape

- [x] Create `mods/std/image/mod.n` as the public entry point.
- [ ] Split implementation across module parts:
  - [x] `image.n`: public `Image`, `ImageError`, loading API, and channel
        conversion.
  - [x] `reader.n`: byte reader/cursor helpers over `[]u8`.
  - [x] `png.n`: PNG container, zlib/deflate, filters, and colour conversion.
  - [ ] `jpeg.n`: JPEG markers, entropy decode, IDCT, and colour conversion.
  - [ ] `tests.n`: source-level unit tests and tiny embedded fixtures.
- [ ] Keep the public import as `use std.image`.
- [ ] Keep implementation details private unless a helper type is intentionally
      part of the public API.

### Companion `std.compress` Module

- [x] Create `mods/std/compress/mod.n` before PNG zlib/deflate work starts.
- [x] Keep compression algorithms separate from image decoding so PNG can use
      the same public decompression APIs as other callers.
- [x] Design a small Nerd-native API using `Result`, slices, default
      parameters, and method-style helpers where useful.
- [x] Start with zlib-wrapped deflate because PNG requires it.
- [x] Keep raw deflate and gzip as explicit future extensions, not accidental
      behaviours.
- [x] Put compression implementation tests in `test { ... }` sections inside
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
- [x] Add an initial source test proving the stdlib source-test suite discovers
      `std.image`.

### Milestone 2: Reader Helpers

- [x] Add a private `ImageReader` over `[]u8`.
- [x] Implement `remaining()`, `position()`, and `done()`.
- [x] Implement `read_u8()`.
- [x] Implement big-endian reads for PNG: `read_be_u16()` and `read_be_u32()`.
- [x] Implement little-endian reads for future binary containers:
      `read_le_u16()` and `read_le_u32()`.
- [x] Implement `take(count)` returning a slice or `UnexpectedEnd`.
- [x] Add source tests for empty input, exact reads, short reads, and cursor
      advancement.

### Milestone 3: Format Detection

- [x] Add `ImageFormat` enum.
- [x] Implement PNG signature detection.
- [x] Implement JPEG SOI marker detection.
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

- [x] Create the `std.compress` folder module.
- [x] Add `CompressionError`.
- [x] Add placeholder public zlib/deflate API.
- [x] Parse zlib header and checksum fields.
- [x] Implement stored blocks.
- [x] Implement fixed Huffman blocks.
- [x] Implement dynamic Huffman blocks.
- [x] Implement length/distance copy logic.
- [x] Validate end-of-block handling.
- [x] Validate truncated stream failures.
- [x] Add `std.compress` source tests for small compressed streams independent
      of PNG.
- [x] Add `std.image` tests against embedded PNG `IDAT` payloads once PNG chunk
      parsing exists.

### Milestone 6: PNG Filters And RGBA Decode

- [x] Implement filter type 0: None.
- [x] Implement filter type 1: Sub.
- [x] Implement filter type 2: Up.
- [x] Implement filter type 3: Average.
- [x] Implement filter type 4: Paeth.
- [x] Decode non-interlaced 8-bit RGB.
- [x] Decode non-interlaced 8-bit RGBA.
- [x] Convert RGB to desired channels `0`, `3`, and `4`.
- [x] Add source tests for every filter using tiny handcrafted scanlines.
- [x] Add tests for embedded 1x1 and 2x2 PNG fixtures with known pixels.

### Milestone 7: More PNG Colour Modes

- [x] Decode greyscale 8-bit.
- [x] Decode greyscale with alpha.
- [x] Decode indexed colour with `PLTE`.
- [x] Apply `tRNS` transparency where supported.
- [x] Add channel conversion for desired channels `1` and `2`.
- [x] Decide whether to support 16-bit PNG now or explicitly reject it with a
      stable `UnsupportedFormat` reason.
- [x] Decide whether to support Adam7 interlace now or explicitly reject it with
      a stable `UnsupportedFormat` reason.
- [x] Add fixtures for greyscale, greyscale-alpha, indexed, and transparent
      PNGs.

### Milestone 8: JPEG Baseline

- [x] Parse JPEG markers.
- [x] Parse SOF0 frame headers.
- [x] Parse DQT quantisation tables.
- [x] Parse DHT Huffman tables.
- [x] Parse SOS scan headers.
- [ ] Implement entropy decoding for baseline sequential JPEG.
- [ ] Implement dequantisation and IDCT.
- [ ] Implement YCbCr to RGB conversion.
- [ ] Add desired-channel conversion.
- [ ] Add tests with embedded baseline JPEG fixtures.
- [x] Explicitly reject progressive JPEG until supported.

### Milestone 9: Image Viewer Integration

- [x] Replace the generated `DemoImage` path in
      `examples/image_viewer/image_viewer.n` with an embedded asset decoded via
      `std.image.Image.load_bytes(...)`.
- [x] Keep a default asset in `examples/image_viewer/assets/`.
- [x] Add nearest-neighbour fit-to-window presentation for loaded images.
- [x] Add optional checkerboard background for transparent PNGs.
- [x] Add `flip_vertically := no` as either a load option or a viewer option.

### Deferred: File Loading And Extra Formats

- [ ] Add `Image.load(path, desired_channels := 0)` after a stable `std.fs` or
      equivalent file API exists.
- [ ] Add source-relative fixture tests once file loading exists.
- [ ] Add command regressions for missing files and corrupted file contents once
      file loading exists.
- [ ] Revisit BMP and TGA only after PNG and JPEG are complete.
- [ ] Revisit other `stb_image` formats such as HDR, GIF, PSD, PIC, and PNM only
      after PNG and JPEG are complete.

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
- [ ] Should CRC checking be mandatory for PNG?
- [ ] Should the first implementation support 16-bit PNG or reject it cleanly?
