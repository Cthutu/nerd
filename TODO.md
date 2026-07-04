# TODO

## Native `std.image`

Port image loading into Nerd as a standard-library module rather than wrapping a
C implementation. Use `stb_image.h` as the behavioural reference, but write the
library in idiomatic Nerd with methods, default parameters, source tests, and
pattern matching.

### Module Shape

- Create `mods/std/image/mod.n` as the public entry point.
- Split implementation across module parts so each decoder stays readable:
  - `image.n`: public `Image`, `ImageError`, loading API, channel conversion.
  - `reader.n`: byte reader/cursor helpers over `[]u8`.
  - `png.n`: PNG container, zlib/deflate, filters, colour conversion.
  - `jpeg.n`: JPEG markers, entropy decode, IDCT, colour conversion.
  - `bmp.n`, `tga.n`: simpler uncompressed/RLE formats after PNG.
  - `tests.n`: source-level unit tests and tiny embedded fixtures.
- Keep the public import as `use std.image`; implementation files should remain
  private unless a helper type is intentionally public.

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

Open question: decide whether image pixel storage is temp-arena-owned,
heap-owned, or caller-arena-owned. The API should make ownership explicit before
the first decoder lands.

### Nerd-Native Style

- Use `Result[Image, ImageError]` for errors rather than global failure state.
- Use enums and pattern matching for formats, chunks, markers, and decode
  states.
- Use slices, `.data`, `.count`, and `.bytes` instead of raw pointer arithmetic
  when the representation allows it.
- Use default parameters for desired channel count and decoder options.
- Prefer methods on reader/decoder structs over free helper functions where it
  improves locality.
- Keep source-compatible behaviour close to `stb_image.h`, but do not preserve C
  preprocessor structure or macro-heavy implementation details.

### Test Plan

- Use Nerd source tests inside the `std.image` module parts so private decoder
  helpers can be tested directly:

```nerd
test "png signature" {
    assert is_png(@embed("fixtures/tiny.png"))
}
```

- Add command regressions that run `nerd test` for the module, so source tests
  are included in `just test`.
- Use `@embed` for tiny fixtures:
  - 1x1 PNG RGBA.
  - 2x2 PNG RGB/RGBA with known pixels.
  - Indexed PNG if palette support is included.
  - Corrupt/truncated PNG cases.
  - JPEG/BMP/TGA fixtures as each decoder lands.
- Test channel conversion for desired channels `0`, `1`, `2`, `3`, and `4`.
- Test source-relative loading with a real file path, not only `@embed`.
- Add invalid-data tests that assert stable `ImageError` variants.

### Milestones

1. Land module skeleton, public API, reader helpers, and source-test command.
2. Implement PNG signature, chunk iteration, CRC validation policy, and IHDR
   parsing.
3. Implement zlib/deflate required for PNG.
4. Implement PNG filters and non-interlaced 8-bit RGB/RGBA decode.
5. Add grayscale, alpha, palette, transparency, and channel conversion.
6. Add file-path loading on top of `std.io`.
7. Add JPEG baseline decode.
8. Add BMP/TGA.
9. Decide whether to support optional stb formats such as HDR, GIF, PSD, PIC,
   and PNM.

### Integration Notes

- `std.image` should work cleanly with OpenGL texture upload:
  `image.data.data`, `image.width`, `image.height`, and channel count should map
  directly to `glTexImage2D`.
- Consider `flip_vertically := no` as a load option or method because OpenGL
  tutorials often need vertically flipped textures.
- Keep decoder allocations visible and deterministic; avoid hidden permanent
  allocations for transient decode buffers.
