# Roadmap

This document records current project guidance and active work. Completed
milestone plans should live in git history, tests, and documentation rather than
continuing to grow this file.

## Current State

- The compiler pipeline is `lexer -> CST/AST parser -> sema -> HIR -> LLVM IR -> clang`.
- HIR is the stable checked middle layer and LLVM IR is the executable backend
  output. The legacy custom IR/C backend is gone.
- The executable backend currently supports the host 64-bit clang target. Do
  not claim cross-target or aggregate FFI ABI support without explicit layout
  work and tests.
- Semantic analysis is a distinct front-end phase and owns name resolution,
  declaration classification, typing, diagnostics, and dependency ordering.
- The AST should remain compact and syntax-oriented. Semantic facts belong in
  arena-backed side tables keyed by indices.
- The CST owns source-preserving tooling such as formatting and editor-facing
  token structure.
- `just test` is the full-project regression gate. It runs language, error,
  HIR, LLVM, formatter, LSP, and command tests.
- Installed-compiler release smoke is covered by `build/test_install.py`.
- Editor wiring smoke is covered by `build/check_editor_integrations.py`.
- Modules load through the program-level front end. Standard modules live under
  `mods/` and should be treated like normal source modules.
- Module paths resolve to `path.n` first, then `path/mod.n` for folder modules.
- Folder modules implicitly include immediate sibling `.n` files in the same
  module scope.
- Public module exports are explicit with `pub`. Non-exported module members
  are private.
- `box[T]` is the built-in owning single-value heap allocation type. It
  supports nil, allocation, pointer adoption, borrowing as `^T`, implicit
  boolean conversion, member access, `.free()`, move semantics, and automatic
  scope-exit cleanup.
- `std.term` includes a terminal framebuffer with views, clipping, dirty-cell
  presentation, box drawing, 24-bit colours, and width-aware UTF-8 text output.
- The CLI supports program arguments plus executable, object, static-library,
  and shared-library output for the host toolchain.
- Source tests can be declared in imported modules, and `test { ... }` blocks
  provide test-only top-level declarations.

## Working Rules

- Do the work in small, reviewable increments.
- Prefer stable intermediate representations over ad hoc cross-stage logic.
- Keep new compiler data structures arena-based, table-based, and index-based.
  Prefer dense parallel tables and side tables over pointer-heavy object graphs.
- Avoid expanding `AstNode` unless there is a strong measured reason.
- Compute spans from token and node boundaries rather than storing large span
  payloads in AST nodes.
- Preserve the AST's RPN-friendly structure so it remains suitable for
  stack/VM-style passes, constant folding, and later compile-time execution.
- Keep parser responsibilities syntactic. Put symbol meaning, declaration
  meaning, type resolution, and language rules in semantic analysis.
- Use British spelling throughout code, comments, tests, documentation, and git
  commit messages where practical.
- In generated LLVM IR, emit Nerd-visible symbols with the `$` prefix and keep
  compiler/runtime helper names generated and internal unless explicitly
  exported.
- Allow forward references between declarations where the language already
  supports them. Track dependencies explicitly and order declarations from that
  information.
- Keep top-level constant bindings simple: lower them as globals/constants and
  let LLVM handle optimisation unless a specific compiler optimisation has been
  designed.
- When touching a compiler file, improve nearby comments and spelling where it
  is useful, but do not turn local fixes into unrelated rewrites.

## Horizontal Feature Policy

New language features should land across the toolchain together:

- compiler parsing and semantic analysis
- HIR and LLVM lowering
- formatter support
- LSP/editor support where relevant
- language tests
- error tests for invalid source
- format/LSP/command tests where the feature affects those surfaces
- user-facing docs when the language surface changes, including the manual and
  syntax/language-reference appendices where relevant

Prefer dense language regression tests that exercise related runtime cases in
one file. Use `std.io` for visible runtime output rather than adding many tiny
happy-path tests.

Do not move on from a language feature while one of the agreed compiler,
formatter, LSP, testing, or manual/documentation surfaces is knowingly behind.
If a documentation update is intentionally deferred, record that explicitly in
the roadmap before committing the implementation.

## Diagnostic Policy

- Keep the fuller diagnostic design policy in
  [docs/error-system.md](docs/error-system.md), and update that document when
  error-shaping rules change.
- Every language-compilation failure point must produce either:
  - a categorised compiler diagnostic with source spans, useful references, and
    appropriate notes or help text, or
  - an ICE when the failure represents a compiler bug or violated internal
    invariant rather than invalid user source.
- Use notes for explanatory context about why an error happened or which rule
  applies.
- Use help for actionable fix guidance.
- Treat OS, filesystem, shell, and toolchain failures as runtime errors, not
  user-source diagnostics or ICEs.
- Treat HIR generation and LLVM lowering invariant failures as ICEs.

## Test Artefact Policy

- Passing tests should remove generated intermediate files.
- Failing tests should keep generated intermediate files so failures can be
  analysed locally.
- When a feature changes CLI-visible compile-time behaviour, add at least one
  regression that exercises the real `nerd` command path rather than only
  internal compiler entry points.

## Parser And Sema Direction

- Replace scattered token classification logic with small shared tables or
  macros when doing so reduces local complexity.
- Use compact AST/sema emit helpers where they remove repetitive parser or
  semantic boilerplate.
- Keep semantic analysis VM-friendly and table-driven:
  - process AST/CST data through linear or index-based passes where practical
  - store semantic data in side tables keyed by AST node, symbol, declaration,
    local, and scope indices
  - keep dependency data explicit
- Standardise parser helper contracts:
  - each helper should clearly either require the first token to have already
    been consumed, or consume it itself
  - comments and function names should make the contract clear where ambiguity
    is likely
- Standardise function comments as files are edited:
  - one short purpose comment when it helps
  - separator comments before major internal functions where the file already
    uses that style

## Active Work

### Standard Library Expansion

- Continue expanding the standard library only as the module/export model needs
  real validation.
- Keep the standard library surface disciplined while the core language is still
  moving.
- Prefer simple modules that exercise existing language features before adding
  library-driven language changes.
- Keep `docs/stdlib.md` as a separate standard-library document; the language
  manual should reference the standard library only for small examples.
- [ ] Keep parsing traits such as `Parse` at standard-library level rather than
  making them language-known traits.

### Graphics Pixel Buffer Milestone

Add `std.gfx` as the first window graphics layer system. Start with a
frame-presented pixel buffer layer attached to `std.frame` windows, while
keeping the public API backend-neutral enough for later OpenGL, Vulkan,
tile-map, and ASCII layers.

- [x] Add `mods/std/gfx/mod.n` with a small orchestrating `GfxSystem`.
- [x] Add `PixelLayerMode`:
  - [x] `FitToWindow { pixel_scale u16 }`
  - [x] `FixedSizeAutoScale { width u16, height u16 }`
- [ ] Define crisp-pixel presentation rules:
  - [x] `FitToWindow` uses the fixed integer `pixel_scale`, resizes the backing
    pixel buffer as the frame changes, and keeps the presented frame dimensions
    at the top-left with no intentional letterboxing.
  - [x] `FixedSizeAutoScale` keeps a fixed virtual buffer size, uses the largest
    integer scale that fits the frame, and letterboxes unused space.
- [ ] Add typed pixel-buffer operations:
  - [x] create/destroy a pixel layer for a frame
  - [x] expose width, height, and contiguous `[]u32` pixels
  - [x] `clear`, `put`, and `fill` helpers
  - [x] `paint(area, painter)` where the painter receives a contiguous pixel
    slice, area width/height, and stride
- [ ] Render through 3D graphics hardware where practical:
  - [x] Linux X11 backend using OpenGL/GLX first
  - [ ] Windows backend using OpenGL/WGL
  - [x] upload the pixel buffer to a texture and draw a nearest-filtered quad
  - [x] render through a shader, vertex buffer, and vertex array rather than
    fixed-function OpenGL calls
  - [x] keep whole-texture upload acceptable for the first version; add dirty
    rectangles later only after the API shape is proven
- [x] Add `examples/pixels/pixels.n` showing a frame, pixel layer, clear/fill,
  simple animation, resize handling, and render loop.
- [x] Add `examples/pixels_fit/pixels_fit.n` showing `FitToWindow`.
- [ ] Add standard-library documentation for `std.gfx` and the pixel-buffer
  sizing modes.
- [ ] Add source, command, and example tests where the behaviour can be
  verified without relying on an interactive desktop session.

### OpenGL Standard Library Milestone

Add `std.opengl` as a frame-compatible OpenGL 3 binding layer. This should be a
thin, explicit API first; higher-level graphics layers can build on it later.

- [x] Add initial `mods/std/opengl/mod.n` and Linux GLX/OpenGL declarations.
- [ ] Expand `std.opengl` with portable public OpenGL type aliases, constants,
  and command declarations for the basic OpenGL 3 workflow.
- [ ] Add platform loader support:
  - [x] Linux GLX context creation for frame pixel presentation.
  - [ ] Linux GLX command lookup for frames.
  - [ ] Windows WGL command lookup for frames.
- [ ] Add `gl_init(^FrameSystem, ^Frame) -> bool` to create or attach an OpenGL
  context for a frame and load OpenGL 3 command pointers into module-level
  function pointer variables.
- [ ] Add `gl_done(^FrameSystem, ^Frame)` to release frame-attached OpenGL
  context resources and clear loaded command pointers when appropriate.
- [ ] Add basic commands needed by the pixel-buffer renderer:
  - [x] buffer, texture, shader, and vertex-array creation/destruction
  - [ ] shader compile/link/status logging
  - [x] viewport, clear, draw, texture upload, and swap buffers
- [x] Move `std.gfx` pixel presentation from XImage fallback to a nearest
  filtered texture quad on OpenGL-capable frames.
- [x] Move frame pixel presentation off fixed-function OpenGL and onto a
  shader/VBO/VAO path.
- [x] Keep an XImage/software fallback while OpenGL support is incomplete or
  unavailable.
- [ ] Add examples and tests that can check loading and symbol availability
  without requiring an interactive desktop where possible.

### Source Testing Follow-up Milestone

- [x] Collect and run tests declared in imported modules.
- [x] Add `test { ... }` blocks for test-only top-level declarations.
- [ ] Add non-fail-fast test assertions if continuing after failure becomes
  useful.

### Editor Intelligence Milestone

- [ ] Keep extending formatter support as syntax lands.
- [ ] Keep extending LSP support as syntax and semantics land.
- [ ] Treat VS Code and Neovim/LazyVim as paired editor surfaces. When a change
  affects file detection, syntax highlighting, LSP launch/configuration,
  formatting, or install packaging, update both editor integrations in the same
  commit unless one is explicitly out of scope.
- [ ] Prioritise semantic LSP intelligence before Tree-sitter. The compiler
  front end already has semantic information that should drive completion,
  signature help, definitions, references, rename, and code actions.
- [x] Add references and rename:
  - [x] same-file references
  - [x] same-file rename
  - [x] cross-module references
  - [x] cross-module rename
- [x] Add code actions for diagnostics where the compiler already has actionable
  help, such as `:` versus `::` and unused local fixes.
  - [x] `:` versus `::` function-definition fix
  - [x] unused local fixes
- [x] Add document links for module imports.
- [ ] Add workspace symbol search for top-level declarations.
- [ ] Add Tree-sitter support after the LSP intelligence work is useful:
  - [ ] grammar for Nerd source
  - [ ] editor-native highlighting
  - [ ] folding
  - [ ] indentation
  - [ ] incremental selection
  - [ ] structural editing support, especially for Neovim

## Deferred Work

These items are worth keeping visible, but they should not be treated as
near-term tasks without a fresh design pass.

- In-place AST compaction for constant folding.
- Calling-convention annotations for FFI, including platform-specific behaviour.
- Named arguments, including their interaction with default parameters and
  call-site argument ordering.
- Numeric generic parameters, such as `[T, N: usize]`, including canonical
  value identity and constant-evaluation rules.
- Targeted diagnostics for value arguments in generic type-argument positions
  and type arguments in value-index positions.
- Truncating readable generic C symbol stems while keeping the hash suffix for
  collision resistance.
- Interpolation formatting specifiers such as `{expr; format}`.
- Operator traits for arithmetic, bitwise, comparison, and assignment
  operators.
- A synthetic diagnostic harness for hard-limit error categories that are
  impractical to trigger through normal source files.
