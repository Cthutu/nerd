# TODO

## Native `std.image`

`std.image` now provides Nerd-native PNG and baseline JPEG decoding, channel
conversion, explicit owned pixel storage, module-local source tests, and image
viewer integration. The completed implementation plan lives in git history,
tests, and the module source. This file tracks only decisions and extensions
that remain open.

### Settled Direction

- Keep the implementation in idiomatic Nerd rather than wrapping
  `stb_image.h`; use stb only as a behavioural compatibility reference.
- Keep decoder allocation explicit and deterministic.
- `Image.data` is an owned `[..]u8` allocation released by `Image.free()`.
- Keep implementation helpers private and test them in module-local `test`
  sections.
- Keep decoded images byte-channel based. Callers such as `std.gfx` should own
  any packed-pixel conversion unless repeated use demonstrates that a shared
  image helper is warranted.
- PNG CRC checking is mandatory.
- Unsupported PNG variants, including 16-bit samples and Adam7 interlace, must
  fail cleanly until deliberately implemented.

### Outstanding Extensions

- [ ] Add `Image.load(path, desired_channels := 0)` after a stable `std.fs` or
      equivalent file API exists.
- [ ] Add source-relative fixture tests once file loading exists.
- [ ] Add command regressions for missing files and corrupted file contents
      once file loading exists.
- [ ] Consider progressive JPEG, 16-bit PNG, and Adam7 interlace only in
      response to a concrete use case.
- [ ] Consider BMP and TGA after the existing PNG and JPEG decoders have had
      broader real-world validation.
- [ ] Consider further stb-compatible formats such as HDR, GIF, PSD, PIC, and
      PNM only after the narrower format surface proves insufficient.
- [ ] Replace placeholder viewer assets with more representative images when
      suitable licensed, reasonably sized assets are available.

### Companion Compression Work

`std.compress` currently provides the zlib-wrapped deflate support required by
PNG. Raw deflate and gzip should remain explicit future additions rather than
accidental behaviours of the zlib API.

- [ ] Add raw deflate only when a caller requires it.
- [ ] Add gzip parsing and checksum validation only when a caller requires it.
