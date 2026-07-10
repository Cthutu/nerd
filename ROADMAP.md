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

Completed milestone plans belong in git history, tests, and subsystem
documentation. The active list below contains only work that is ready to guide
the next implementation slices.

### Atomics Milestone

Add first-class atomic values with sequentially consistent operator defaults
and explicit memory ordering through `std.atomics`. Atomic storage protects the
stored scalar value only; it does not provide ownership, lifetime management,
or implicit synchronisation for pointee data.

#### Compile-Time Parameters

Add compile-time value parameters as the small general language feature needed
to express atomic ordering without making ordering enums special types.

- [ ] Accept `::` in function parameter declarations to require a value known
  during semantic analysis:

  ```nerd
  load :: fn (
      self  : ^atomic[T],
      order :: AtomicLoadOrder = SequentiallyConsistent,
  ) -> T
  ```

- [ ] Permit literals, constants, and constant expressions supported by the
  existing constant evaluator as arguments to `::` parameters.
- [ ] Reject runtime values with a diagnostic that identifies the argument and
  the compile-time parameter declaration.
- [ ] Treat compile-time arguments as part of function specialisation identity
  alongside generic type arguments.
- [ ] Substitute the known value before or during HIR generation so the backend
  never receives an unresolved runtime ordering for an atomic instruction.
- [ ] Require default values for `::` parameters to be compile-time-known.
- [ ] Keep compile-time parameters immutable within the function body.
- [ ] Preserve `::` in formatting, hover, signature help, and rendered function
  types. Offer appropriate constants and enum variants in completion.
- [ ] Initially limit compile-time arguments to values with stable canonical
  identity, including booleans, integers, and payload-free enums. Extend the
  set only with explicit constant-evaluation and specialisation tests.
- [ ] Use this mechanism for later numeric generic parameters rather than
  designing a separate value-specialisation model.

#### Atomic Types And Operators

- [ ] Add the built-in type constructor `atomic[T]`.
- [ ] Initially permit `T` to be:
  - `bool`
  - `i8`, `i16`, `i32`, `i64`, and `isize`
  - `u8`, `u16`, `u32`, `u64`, and `usize`
  - a thin non-owning pointer `^U`
- [ ] Reject owning, dynamically sized, or fat values, including `box[T]`,
  strings, slices, dynamic arrays, and aggregates.
- [ ] Represent `atomic[bool]` with suitable addressable atomic storage rather
  than relying on language-level boolean ABI assumptions.
- [ ] Make atomic storage non-copyable and reject passing or returning an
  atomic by value. Functions that operate on the same storage must use
  `^atomic[T]`.
- [ ] Allow explicit construction of independent atomic storage from a
  compatible compile-time or runtime `T` value.
- [ ] Give ordinary syntax sequentially consistent semantics:
  - reading an `atomic[T]` as `T` performs one atomic load
  - assigning `T` with `=` performs one atomic store
  - integer `+=`, `-=`, `&=`, `|=`, and `^=` perform indivisible atomic
    read-modify-write operations
  - ordinary arithmetic, bitwise, boolean, and comparison expressions first
    perform an atomic load and then operate on the resulting non-atomic `T`
- [ ] Document and test that `value = value + 1` is an atomic load followed by
  an atomic store, not an indivisible increment; `value += 1` is the atomic
  read-modify-write form.
- [ ] Support boolean load, store, exchange, and compare-exchange. Consider
  boolean `&=`, `|=`, and `^=` only if their semantics remain clear and useful.
- [ ] Support pointer load, store, exchange, compare-exchange, equality, and
  `nil`. Do not initially support atomic pointer arithmetic.
- [ ] Treat an atomic pointer as non-owning. Loading a pointer neither keeps the
  pointee alive nor makes its fields atomic; reclamation schemes such as hazard
  pointers and epochs remain library responsibilities.

#### `std.atomics` API

- [ ] Add `std.atomics` with separate public ordering enums so invalid load and
  store orderings are not representable:

  ```nerd
  AtomicLoadOrder :: enum {
      Relaxed
      Acquire
      SequentiallyConsistent
  }

  AtomicStoreOrder :: enum {
      Relaxed
      Release
      SequentiallyConsistent
  }

  AtomicOrder :: enum {
      Relaxed
      Acquire
      Release
      AcquireRelease
      SequentiallyConsistent
  }
  ```

- [ ] Keep these as ordinary enums that can be stored, passed, returned, and
  inspected with `on`. Atomic methods require compile-time values through `::`;
  callers with a runtime order can branch and call the method with an explicit
  constant in each branch.
- [ ] Define the generic compare-exchange result as:

  ```nerd
  AtomicCompareExchangeResult :: enum[T] {
      Exchanged
      NotExchanged(T)
  }
  ```

  If positional enum payloads are not yet supported, use an equivalent named
  `observed T` payload without changing the result semantics.
- [ ] Add built-in-backed `impl atomic[T]` methods in `std.atomics` for `load`,
  `store`, `exchange`, integer fetch operations, and strong
  `compare_exchange`.
- [ ] Default all method order parameters to sequential consistency.
- [ ] Give compare-exchange separate success and failure ordering parameters.
  The failure parameter uses `AtomicLoadOrder`; reject a failure ordering that
  is stronger than the success ordering.
- [ ] Return `Exchanged` after a successful comparison and
  `NotExchanged(observed)` after a mismatch.
- [ ] Consider weak compare-exchange only after the strong operation and its
  retry patterns are established.
- [ ] Keep public signatures, documentation, and convenience logic in Nerd
  source. Keep representation, validation, and primitive atomic lowering in
  the compiler.

#### Compiler And LLVM Work

- [ ] Add semantic type facts, compatibility rules, initialisation rules, and
  operation validation for `atomic[T]`.
- [ ] Lower atomic operations explicitly through HIR rather than encoding them
  as ordinary loads, stores, or arithmetic.
- [ ] Lower each supported operation and ordering to valid LLVM atomic
  instructions for the host 64-bit clang target.
- [ ] Preserve target alignment requirements and reject types or widths the
  current target contract cannot implement correctly.
- [ ] Do not claim portable lock-free guarantees. Document which properties
  come from the language memory model and which remain target-dependent.
- [ ] Keep atomic intrinsic identity internal; do not resolve compiler
  behaviour by matching user-visible source names.

#### Diagnostics, Tooling, Tests, And Documentation

- [ ] Add focused diagnostics for unsupported element types, copying or
  passing atomic storage by value, invalid operators, pointer arithmetic, and
  invalid compare-exchange ordering combinations.
- [ ] Diagnose non-constant arguments to `::` parameters with actionable help
  showing how to branch on a runtime value and use constants in each branch.
- [ ] Explain in pointer-related diagnostics that atomicity of the pointer does
  not protect pointee lifetime or fields.
- [ ] Add formatter coverage for `::` parameters and atomic type/method syntax.
- [ ] Add LSP completion, hover, signature help, definition, references, and
  semantic-token coverage for compile-time parameters, `atomic[T]`, ordering
  enums, and substituted method signatures.
- [ ] Add dense language and LLVM regressions covering integer, boolean, and
  pointer operations at every supported ordering.
- [ ] Add error regressions for every rejected type, copying case, invalid
  operation, non-constant order, and invalid ordering pair.
- [ ] Add command-path regressions that compile and run representative atomic
  programs through the real `nerd` executable.
- [ ] Add standard-library source tests for ordering types and public method
  behaviour where deterministic single-threaded checks are meaningful.
- [ ] Add manual and language-reference documentation for compile-time
  parameters, atomic syntax, default ordering, explicit methods, and pointer
  lifetime hazards. Keep implementation notes and `docs/stdlib.md` aligned.

### Standard Library

- Finish the public OpenGL 3 surface needed by small direct-OpenGL programs:
  portable aliases, constants, commands, and non-interactive loader tests.
- Keep `std.frame` responsible for window and context lifecycle, `std.gfx` for
  pixel presentation, and `std.opengl` for raw portable OpenGL commands.
- Keep parsing traits such as `Parse` at standard-library level rather than
  making them language-known traits.
- Expand the standard library only where it validates the module/export model
  or provides a clear user-facing capability. Keep `docs/stdlib.md` aligned.
- Track the remaining `std.image` decisions and extensions in [TODO.md](TODO.md).

### Editor Intelligence

- Extend formatter and LSP behaviour alongside new syntax and semantics.
- Improve semantic completion, signature help, hover, definitions, references,
  rename, and code actions using the existing front-end facts.
- Treat VS Code and Neovim/LazyVim as paired editor surfaces when file
  detection, highlighting, LSP wiring, formatting, or packaging changes.
- Consider Tree-sitter only after the semantic LSP experience is strong enough
  to justify a second syntax implementation. Its eventual scope may include
  highlighting, folding, indentation, incremental selection, and structural
  editing.

### Testing Follow-up

- Add non-fail-fast source-test assertions only if real test suites demonstrate
  that continuing after a failed assertion would be useful.

## Deferred Work

These items are worth keeping visible, but they should not be treated as
near-term tasks without a fresh design pass.

- In-place AST compaction for constant folding.
- Calling-convention annotations for FFI, including platform-specific behaviour.
- Named arguments, including their interaction with default parameters and
  call-site argument ordering.
- Targeted diagnostics for value arguments in generic type-argument positions
  and type arguments in value-index positions.
- Truncating readable generic C symbol stems while keeping the hash suffix for
  collision resistance.
- Interpolation formatting specifiers such as `{expr; format}`.
- Operator traits for arithmetic, bitwise, comparison, and assignment
  operators.
- A synthetic diagnostic harness for hard-limit error categories that are
  impractical to trigger through normal source files.
