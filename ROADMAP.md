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

### Boxed Single Allocation Milestone

Add `box[T]` as the built-in owning single-value heap allocation type, backed by
the runtime heap allocator. The feature exists to replace unsafe byte-slice
allocation casts when a program wants one heap-allocated plex or value.

- [x] Add `box[T]` type resolution and source-facing type names.
- [x] Add `box[T] = nil` and nil comparison support.
- [x] Add `box[T]()` allocation through `nrt_mem_alloc` with call-site file
  and line information for leak diagnostics.
- [x] Add `box[T](ptr)` adoption from `^T` without allocating.
- [x] Add implicit `box[T]` to `^T` borrowing for function calls.
- [x] Add implicit `box[T]` to `bool` conversion where nil is `no` and non-nil
  is `yes`.
- [x] Add field access through boxes so `boxed.field` behaves like pointer
  member access.
- [x] Add `.free()` lowering through `nrt_mem_free`, resetting the box to nil.
- [x] Add formatter coverage for `box[T]` type syntax through existing generic
  type formatting.
- [x] Add basic LSP member completion and hover support for `.free()` and boxed
  pointee fields.
- [x] Add debugger representation support as pointer-shaped storage with the
  `box[...]` source type name in debug metadata.
- [x] Add executable command coverage for allocation, borrowing, field access,
  nil, and free.
- [x] Add move assignment/call/return semantics that nil the source box.
- [x] Add automatic scope-exit cleanup for local and parameter boxes that are
  not returned or moved.
- [ ] Add dedicated diagnostics for invalid box adoption and double-free-prone
  patterns once broader ownership analysis exists.

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

### Terminal Framebuffer Milestone

Add a terminal framebuffer backing store to `std.term` for single-screen
terminal applications. The first version should integrate with the existing
terminal loop, keep the public API small, and favour correctness over terminal
output micro-optimisation.

- [x] Add framebuffer storage:
  - [x] store cells in row-major order in a dynamic array
  - [x] store each cell's UTF-8 codepoint as `u32`
  - [x] store 24-bit foreground colour (`ink`) and background colour (`paper`)
  - [x] track dirty cells so presentation can emit changed row intervals rather
    than redrawing the whole screen every time
  - [x] track wide-character head/tail state so rendering can skip continuation
    cells safely
- [x] Add framebuffer lifecycle integration:
  - [x] initialise the framebuffer on the first `term_loop` iteration
  - [x] size the framebuffer to the current terminal dimensions
  - [x] clear the initial framebuffer with default colours and spaces
  - [x] reset the active view to the full framebuffer on each `term_loop`
    iteration
  - [x] call framebuffer presentation from inside `term_loop`
  - [x] release framebuffer storage when `term_loop` exits after `term_done`
- [x] Add resize behaviour:
  - [x] grow capacity when the terminal grows
  - [x] copy existing rows into the new row width
  - [x] fill newly exposed cells with spaces and default colours
  - [x] truncate characters at row ends when shrinking width
  - [x] drop whole rows when shrinking height
  - [x] mark affected cells dirty after resize
- [x] Add framebuffer drawing API:
  - [x] `term_fb_clear()` to clear the full active view
  - [x] `term_fb_put(x, y, ch, ink, paper)` for one codepoint
  - [x] `term_fb_text(x, y, text, ink, paper)` for UTF-8 text
  - [x] `term_fb_fill_rect(x, y, w, h, ch, ink, paper)` to write a character
    and optionally colours over a rectangle
  - [x] `term_fb_paint_rect(x, y, w, h, ink, paper)` to alter only colours over
    a rectangle
  - [x] `term_fb_box(x, y, w, h, style, ink, paper)` for 9-sliced box drawing
  - [x] support transparent colour values that leave the existing ink and/or
    paper unchanged
- [x] Add box styles:
  - [x] built-in styles such as single, double, rounded, and heavy
  - [x] `BoxStyle.Custom(string)` where the string supplies nine box characters
    in row-major 9-slice order
  - [x] validate or gracefully handle custom strings with the wrong number of
    characters
- [x] Add views and clipping:
  - [x] `term_view(x, y, w, h)` sets the active view, clipped to the terminal
    size
  - [x] `term_view_reset()` restores the full-terminal view
  - [x] treat drawing coordinates as relative to the active view
  - [x] clip every drawing rectangle against the active view and terminal bounds
  - [x] compute source offsets during clipping, such as `x_offset = -x` when a
    rectangle begins left of the view
  - [x] use those offsets to skip clipped text content correctly
  - [x] keep UTF-8 clipping width-aware so wide characters are emitted only when
    their full display width fits
- [x] Add minimal geometry support in `std.math`:
  - [x] generic `Point[T]` and `Rect[T]` types
  - [x] aliases such as `PointI32` and `RectI32`
  - [x] only add operations needed by the terminal framebuffer initially, such
    as `right`, `bottom`, `is_empty`, and `intersection`
  - [x] defer broader `union`, `diff`, `contains`, and numeric-trait-driven
    generic operations until a concrete caller needs them
- [x] Add presentation:
  - [x] build one output string per presentation using the terminal arena
  - [x] emit cursor movement, foreground colour, background colour, and text
    using ANSI escape sequences
  - [x] emit row dirty intervals by scanning dirty flags
  - [x] avoid redundant colour escape sequences when consecutive cells share
    colours
  - [x] clear dirty flags after successful presentation
- [x] Tests and examples:
  - [x] add source tests for geometry helpers
  - [x] add source tests for framebuffer resize and clipping helpers where they
    can be tested without terminal interaction
  - [x] update `examples/dungeon/dungeon.n` to draw through the framebuffer
  - [x] keep platform-specific terminal behaviour behind `std.term` and
    `os.*`

### CLI And Binary Output Polish Milestone

This milestone is for command-line and build-output polish after the current
language/runtime layers are stable. Keep the first version host-toolchain only;
do not imply cross-target packaging or platform ABI guarantees beyond what the
current LLVM/clang backend can verify.

- [x] Add program argument support:
  - [x] allow the entry point to be either `main :: fn ()` or
    `main :: fn (args: []string)`
  - [x] lower the runtime entry wrapper so command-line arguments are exposed as
    Nerd `string` slices
  - [x] define whether `args[0]` is the executable path or the first user
    argument, and document the choice
  - [x] reject unsupported `main` signatures with a targeted diagnostic and
    help text
  - [x] add command tests for zero arguments, multiple arguments, and strings
    containing spaces
- [x] Add library/object output:
  - [x] add a CLI mode or flag for producing relocatable object output instead
    of an executable
  - [x] support static library output through `--lib`
  - [x] support conventional host artefact names such as `.o` on POSIX and
    `.obj`/`.lib` on Windows where applicable
  - [x] define how the root module's public surface becomes exported/linkable
    symbols
  - [x] keep temporary LLVM/runtime artefact cleanup consistent with existing
    `run` and `build` cleanup rules
  - [x] add command tests for object/library generation and stale artefact
    cleanup
- [x] Add DLL/shared-library support:
  - [x] add a CLI mode or flag for producing a host shared library
  - [x] support `.dll` on Windows and document any deferred POSIX `.so`/macOS
    `.dylib` behaviour if not implemented together
  - [x] define explicit export rules for root-public functions and data
  - [x] decide how, or whether, the Nerd runtime object is linked into shared
    libraries
  - [x] add command tests that build a shared library and link/load it from a
    small host program where practical
- [x] Update documentation:
  - [x] CLI help and command reference for argument handling and output modes
  - [x] manual/language reference for allowed `main` signatures
  - [x] compiler pipeline notes for object and shared-library output paths

### Trait Polish Milestone

These are open trait-system follow-ups that were previously recorded as
"Later" notes inside the completed traits milestone.

- [x] Add stricter non-lazy generic body checks once constraints exist:
  - [x] reject unresolved names that do not depend on concrete type arguments
    before a generic function or impl method is instantiated
  - [x] check constraint-sensitive operations against trait bounds where
    possible before monomorphisation
  - [x] keep concrete instantiation diagnostics for cases that genuinely depend
    on substituted types
- [x] Improve diagnostics for non-inferable trait generic parameters:
  - [x] produce useful help text when trait generic parameters are required but
    cannot be inferred
  - [x] add error tests for non-inferable trait generic parameters
- [x] Stabilise generated backend names for trait implementation functions.

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
- [ ] Add code actions for diagnostics where the compiler already has actionable
  help, such as `:` versus `::` and unused local fixes.
  - [ ] `:` versus `::` function-definition fix
  - [ ] unused local fixes
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
