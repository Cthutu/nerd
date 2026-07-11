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

### Milestone 1: Compound Functions

Add explicit compound functions: one source-level function name mapped to a
closed list of concrete free functions and resolved from the call signature.
Keep `fn` as the single keyword for the function concept; do not introduce an
`overload` or `compound` keyword.

```nerd
write :: fn {
    write_string
    write_i64
    write_writer
}

write_string :: fn (value: string) {
}

write_i64 :: fn (value: i64) {
}

write_writer :: fn (writer: ^Writer, bytes: []u8) {
}
```

#### Initial Scope And Declaration Rules

- [ ] Parse `fn { ... }` as an explicit compound function declaration whose
  entries name member functions. Preserve ordinary `fn (...)` declarations.
- [ ] Initially allow compound declarations only at top level.
- [ ] Require at least one member and diagnose an empty compound.
- [ ] Allow forward references and qualified imported function names in the
  member list.
- [ ] Require every member to resolve to a concrete free function or another
  compound function.
- [ ] Do not initially allow methods, associated functions, or generic
  functions as direct or transitively nested members.
- [ ] Flatten nested compounds during semantic analysis, preserve useful source
  provenance for diagnostics and navigation, and reject dependency cycles.
- [ ] Keep the member list closed and explicit. Functions do not join a
  compound merely by sharing its name, and compounds from separate modules do
  not merge implicitly.
- [ ] Allow members to remain directly callable by their concrete names.
- [ ] Permit a public compound to contain private functions. Treat the public
  compound as exporting callable signatures without exporting the private
  implementation names, conceptually like a public wrapper around private
  calls.

#### Effective Signatures And Declaration Validation

- [ ] Define a member's effective callable signatures from its declared
  parameters and trailing default parameters. For example,
  `fn(i64, radix: u32 = 10)` exposes effective `fn(i64)` and `fn(i64, u32)`
  call surfaces without generating a second function.
- [ ] Ignore return types when comparing, resolving, and selecting compound
  function signatures.
- [ ] Reject duplicate or indistinguishable concrete signatures when the
  compound is analysed, even if it is never called.
- [ ] Reject overlaps introduced by default parameters at declaration time. A
  compound containing both `fn(i32)` and `fn(i32, radix: u32 = 10)` is invalid
  because both expose an effective `fn(i32)` surface.
- [ ] Report the compound, conflicting members, and any default parameter that
  created an effective-signature overlap.
- [ ] Treat structurally different parameter types as different declaration
  signatures. Do not attempt to prove every possible overlap introduced by
  literals or implicit conversions during declaration validation.

#### Call Resolution

- [ ] Resolve a compound call entirely during semantic analysis.
- [ ] Collect the members callable with the supplied arguments using Nerd's
  existing argument-count, default-argument, and type-compatibility rules.
- [ ] Require exactly one compatible candidate. Do not rank candidates or
  prefer exact matches, fewer conversions, fewer defaults, declaration order,
  public visibility, or any other hidden priority.
- [ ] Diagnose zero candidates with the supplied argument types and list the
  relevant available signatures.
- [ ] Diagnose two or more candidates as ambiguous and list every compatible
  member with useful source references.
- [ ] Keep call-site ambiguity possible for structurally distinct signatures
  that both accept a particular literal or implicit conversion. Require the
  programmer to call a concrete member or otherwise disambiguate the argument.
- [ ] Do not use expected return type to select a member. A call remains
  ambiguous even when its result context would accept only one return type.
- [ ] Apply default arguments only after a unique concrete member has been
  selected; defaults never make one candidate preferable to another.

#### Function Values And Addresses

- [ ] Allow a compound function to become a function value or address only
  when the expected function type selects exactly one member under the same
  compatibility rules used for a call.
- [ ] Diagnose a compound used as a value without a sufficient expected
  function type.
- [ ] Diagnose zero or multiple members compatible with the expected function
  type; do not rank candidates.
- [ ] Produce the selected concrete function value or address. Do not generate
  a dispatcher, wrapper, table, or standalone compound symbol.

#### Semantic, HIR, And Backend Boundaries

- [ ] Represent the compound as a source declaration with visibility, member
  dependency edges, flattened candidate facts, effective signatures, and
  source provenance in semantic side tables.
- [ ] Keep compound resolution out of the parser and AST beyond the syntax and
  member-list structure needed to represent the declaration.
- [ ] Resolve every compound call or value to a concrete function declaration
  before HIR generation.
- [ ] Do not add a compound-function entity, call, type, symbol, dispatcher, or
  other representation to HIR or LLVM. From HIR onwards only the selected
  concrete function exists.
- [ ] Preserve concrete member names for direct calls, debugging, profiling,
  emitted symbols, and diagnostics.

#### Diagnostics And Editor Intelligence

- [ ] Add focused diagnostics for empty compounds, non-function members,
  unsupported method or generic members, nested cycles, inaccessible imported
  members, duplicate signatures, default-expanded overlaps, no matching member,
  ambiguous calls, and unresolved function values.
- [ ] Make ambiguity diagnostics show all compatible signatures and their
  declaration locations without suggesting an arbitrary preferred member.
- [ ] Show the flattened callable signature set in compound hover and signature
  help. On a resolved call, show the selected signature and note the compound
  through which it was selected.
- [ ] Make go-to-definition on a resolved call reach the selected concrete
  function. Where the client distinguishes declaration navigation, retain a
  route to the compound declaration.
- [ ] Treat syntactic uses of the compound name as references to the compound.
  Also count calls resolved through a compound as references to the selected
  concrete member.
- [ ] Renaming a compound changes its declaration and syntactic uses only.
  Renaming a concrete member changes its declaration, direct uses, and member
  entries in compounds.
- [ ] Present public compound signatures in completion and hover without
  pretending that private implementation names are exported.

#### Formatter, Tests, And Documentation

- [ ] Define and test stable formatting for empty/error-recovery cases,
  single-member compounds, multiline member lists, qualified members, comments,
  and nested compound references.
- [ ] Add dense language regressions for direct members, forward references,
  imported members, private implementations, nested flattening, defaults,
  implicit conversions, function values, and direct concrete calls.
- [ ] Add error regressions for every declaration and resolution failure,
  including eager default-expanded overlap and return-context non-resolution.
- [ ] Add HIR and LLVM regressions proving calls and function values name only
  the selected concrete function and that no compound symbol is emitted.
- [ ] Add formatter, LSP, and real command-path regressions for the new surface.
- [ ] Update the learner-facing manual with declaration syntax, calls, defaults,
  private implementations, ambiguity examples, nesting, and function-value
  conversion.
- [ ] Update the implementation-derived specs with normative grammar,
  effective-signature construction, flattening, cycle handling, visibility,
  compatibility, ambiguity, address selection, and the sema/HIR boundary.
- [ ] Update the syntax and language-reference appendices with concise forms
  and restrictions.
- [ ] Update compiler internals with declaration facts, dependencies, semantic
  resolution, source provenance, and the guarantee that HIR receives only
  concrete functions.
- [ ] Update LSP and formatter documentation where their compound-function
  presentation and layout rules are described.
- [ ] Update the diagnostic design documentation if implementation introduces
  new compound-specific diagnostic shaping rules.

### Integrated Optional And Result Types

Add optional and result types as built-in language facilities while retaining
general declaration-level generics. Generic functions, methods, compound
types, traits, implementations, constraints, and inference remain supported.
Parametrised modules and the broader generics redirection described in
[GENERICS.md](GENERICS.md) are held as design exploration rather than active
replacement work.

The intended language boundary for this programme is:

- `?T` is the canonical optional type and replaces the generic `Option[T]`
  enum
- `T\E` is the canonical result type and replaces the generic `Result[T, E]`
  enum
- contextual construction, postfix propagation, and both boolean extraction
  and full payload matching are language features
- general user-defined generics remain available and are not migrated to
  parametrised modules by this programme
- arbitrary type unions are not part of Nerd's direction; explicit C-style
  `union` storage remains a separate low-level feature

Every milestone must cover the applicable parser/CST, formatter, sema, HIR,
LLVM, diagnostics, LSP, tests, manual, specs, appendices, and compiler-internals
surfaces. Do not let transitional compatibility become the undocumented final
design.

#### Milestone 2: Optional And Result Normative Design

- [x] Inventory every `Option[T]`, `Result[T, E]`, constructor, pattern,
  propagation helper, standard-library dependency, test, and manual example.
- [x] Finalise `?T` and `T\E` grammar, precedence, contextual construction,
  movement, cleanup, equality, layout, FFI, pattern, and control-flow rules.
- [x] Specify boolean extraction with `=>`, full `on ... else ...` payload
  matching, branch-local bindings, guards, expression-valued forms, and
  independent exhaustiveness rules.
- [x] Specify postfix `?` propagation and postfix `!` error injection,
  including expected-type and enclosing-return-type requirements.
- [x] Keep existing nullable `^T` pointer semantics during this programme;
  making pointers non-null is a separate future migration.
- [x] Remove no existing `Option` or `Result` facility during this milestone.

#### Milestone 3: Optional Types

- [x] Add prefix `?T`, contextual presence construction, and `nil` absence.
- [x] Add boolean extraction with `on optional => [value] { ... } else { ... }`.
- [x] Add full payload matching with
  `on optional { present patterns } else { ... }`, where `else` represents
  absence and has no payload.
- [x] Implement movement, cleanup, equality, layout, definite-assignment, cast,
  and FFI behaviour.
- [x] Cover parser/CST, formatter, sema, HIR, LLVM, diagnostics, LSP, tests,
  manual, specs, appendices, and compiler internals.
- [x] Retain `Option[T]` only until repository migration validates `?T`.

#### Milestone 4: Result Types And Integrated Control Flow

- [x] Add the dedicated `T\E` result type. Reserve `\` for this type syntax;
  it is not an expression operator or general type union.
- [x] Add contextual success construction and postfix `error!` injection, with
  no standalone error-only type.
- [x] Add postfix `?` propagation for optionals and results.
- [x] Add boolean extraction with
  `on value => [success] { ... } else [error] { ... }`.
- [x] Add full payload matching with
  `on value { success patterns } else { error patterns }`; for optionals,
  `else` represents absence.
- [x] Define branch-local binding, guards, comma patterns, expression-valued
  forms, exhaustiveness, cleanup, and propagated-error compatibility.
- [x] Migrate representative decoder, I/O, frame, and command code while old
  `Option` and `Result` forms remain temporarily available for comparison.
- [x] Stop for an ergonomics review before repository-wide migration.

#### Held Generics Redirection

Parametrised modules, arena type operands, generic-trait replacement, and
removal of general generics are not active milestones. Revisit them only after
a separate design decision; they are not prerequisites for optional or result
types. `Display`, iteration, and generic arena helpers retain their current
generic designs during this programme.

#### Milestone 9: Optional And Result Repository Migration

- [x] Convert every `Option[T]` use to `?T` and every `Result[T, E]` use to
  `T\E` across core, standard modules, examples, and tests.
- [x] Convert `Some`, `None`, `Ok`, and `Err` construction and patterns to
  contextual construction and the two integrated `on ... else ...` forms.
- [x] Migrate representative real programs and rewrite the manual examples.
- [x] Keep migrations in small reviewable slices with real command-path
  regressions.
- [x] Do not migrate unrelated generic functions, types, traits,
  implementations, constraints, inference, iteration, or arena operations.

#### Milestone 10: Remove Generic Option And Result Enums

Begin this milestone only after the repository migration and its design review
are complete.

- [x] Remove the generic `Option[T]` and `Result[T, E]` declarations from
  `core`.
- [x] Remove `Some`, `None`, `Ok`, and `Err` compatibility paths, diagnostics,
  tests, and documentation.
- [x] Preserve all general generic language facilities and their compiler,
  tooling, and documentation support.
- [x] Delete obsolete compatibility machinery coherently rather than retaining
  two optional/result models.

#### Milestone 11: Optional And Result Consolidation And Measurement

- [x] Complete the manual, normative specs, references, compiler internals,
  LSP, formatter, diagnostics, and migration documentation.
- [x] Run installed-compiler, editor-integration, FFI, platform, and release
  smoke coverage.
- [x] Measure compile time, memory use, diagnostic quality, runtime layout, and
  generated code for representative optional and result programs.
- [x] Record the final optional/result boundary and move completed planning
  detail out of the active roadmap while retaining `GENERICS.md` as historical
  design context for the held generics redirection.

### Milestone 12: Atomics

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
- [ ] Treat compile-time arguments as part of canonical function
  specialisation identity without depending on general type-generic functions.
- [ ] Substitute the known value before or during HIR generation so the backend
  never receives an unresolved runtime ordering for an atomic instruction.
- [ ] Require default values for `::` parameters to be compile-time-known.
- [ ] Keep compile-time parameters immutable within the function body.
- [ ] Preserve `::` in formatting, hover, signature help, and rendered function
  types. Offer appropriate constants and enum variants in completion.
- [ ] Initially limit compile-time arguments to values with stable canonical
  identity, including booleans, integers, and payload-free enums. Extend the
  set only with explicit constant-evaluation and specialisation tests.
- [ ] Reuse this mechanism for later compile-time module parameters rather than
  designing a separate value-specialisation model.

#### Tuple Enum Payloads

Add tuple-style enum payloads as a small general language feature before the
atomic compare-exchange API depends on them.

- [ ] Accept variants such as `NotExchanged(T)` alongside unit and named-field
  variants.
- [ ] Support construction, type checking, destructuring patterns, generic
  substitution, HIR generation, and LLVM layout for tuple payloads.
- [ ] Add formatter, diagnostics, hover, completion, signature help, manual,
  syntax-reference, and positive and negative regression coverage.

#### Atomic Types And Operators

- [ ] Add the built-in type constructor `atomic[T]`.
- [ ] Initially permit `T` to be:
  - `bool`
  - `i8`, `i16`, `i32`, `i64`, and `isize`
  - `u8`, `u16`, `u32`, `u64`, and `usize`
  - a thin non-owning pointer `^U`, including object, `void`, opaque FFI, and
    atomic pointee types
- [ ] Reject owning, dynamically sized, or fat values, including `box[T]`,
  strings, slices, dynamic arrays, aggregates, and pointer-like values carrying
  metadata. Defer function pointers until their representation is explicitly
  included in the thin-pointer contract.
- [ ] Represent `atomic[bool]` with suitable addressable atomic storage rather
  than relying on language-level boolean ABI assumptions.
- [ ] Make atomic storage non-copyable and reject passing or returning an
  atomic by value. Functions that operate on the same storage must use
  `^atomic[T]`.
- [ ] Allow construction of independent atomic storage from a compatible
  compile-time or runtime `T` value.
- [ ] Allow assignment and initialisation from another compatible atomic by
  performing a sequentially consistent load of the source followed by a store
  to, or construction of, the destination. This transfers `T`, not atomic
  storage identity, and is not one indivisible operation across both objects.
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
- [ ] Support boolean load, store, exchange, compare-exchange, `&=`, `|=`, and
  `^=`.
- [ ] Support pointer load, store, exchange, compare-exchange, equality, and
  `nil`. Do not initially support atomic pointer arithmetic.
- [ ] Require an atomic pointer to be loaded into an ordinary thin pointer
  before dereference; do not make direct atomic-pointer dereference perform an
  invisible load.
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
- [ ] Define the built-in parametric compare-exchange result, exposed through
  `std.atomics`, as:

  ```nerd
  AtomicCompareExchangeResult :: enum[T] {
      Exchanged
      NotExchanged(T)
  }
  ```

- [ ] Expose built-in atomic methods through `std.atomics` for `load`, `store`,
  `exchange`, integer fetch operations, strong `compare_exchange`, and weak
  `compare_exchange_weak`. Do not depend on the general generic impl facility
  being removed by the generics simplification programme.
- [ ] Default all method order parameters to sequential consistency.
- [ ] Give compare-exchange separate success and failure ordering parameters.
  Both are `::` parameters with sequentially consistent defaults. The failure
  parameter uses `AtomicLoadOrder`; reject a failure ordering that is stronger
  than the success ordering using the LLVM/C++ ordering constraints.
- [ ] Return `Exchanged` after a successful comparison and
  `NotExchanged(observed)` after a mismatch.
- [ ] Document that weak compare-exchange may return
  `NotExchanged(observed)` spuriously and is intended for retry loops.
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
- [ ] Apply the existing Nerd integer overflow rules to signed and unsigned
  atomic read-modify-write operations.
- [ ] Guarantee atomic semantics but do not claim portable lock-free
  implementation. Document which properties come from the language memory
  model and which remain target-dependent; add a lock-free query later only if
  a concrete caller needs one.
- [ ] Do not initially promise C ABI layout compatibility for `atomic[T]`.
  Reject direct atomic FFI values until interoperability is deliberately
  designed and tested.
- [ ] Keep atomic intrinsic identity internal; do not resolve compiler
  behaviour by matching user-visible source names.
- [ ] Specify conflicting unsynchronised access, mixed atomic and non-atomic
  access to the same storage, and invalid pointee lifetime as invalid program
  behaviour rather than presenting them as supported operations with undefined
  results.
- [ ] Prevent invalid behaviour structurally or diagnose it wherever static
  analysis can do so. Do not build extra optimisation assumptions around
  undetected races in Nerd middle layers; document cases that necessarily
  remain the programmer's responsibility and consider future checked tooling.

#### Diagnostics, Tooling, Tests, And Documentation

- [ ] Add focused diagnostics for unsupported element types, copying or
  passing atomic storage identity by value, invalid operators, direct atomic
  pointer dereference, pointer arithmetic, invalid FFI use, and invalid
  compare-exchange ordering combinations.
- [ ] Diagnose non-constant arguments to `::` parameters with actionable help
  showing how to branch on a runtime value and use constants in each branch.
- [ ] Explain in pointer-related diagnostics that atomicity of the pointer does
  not protect pointee lifetime or fields.
- [ ] Add formatter coverage for `::` parameters and atomic type/method syntax.
- [ ] Add LSP completion, hover, signature help, definition, references, and
  semantic-token coverage for compile-time parameters, `atomic[T]`, ordering
  enums, and substituted method signatures.
- [ ] Add dense language and LLVM regressions covering integer, boolean, and
  pointer operations at every supported ordering, including atomic-to-atomic
  load/store assignment and strong and weak compare-exchange.
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
- Interpolation formatting specifiers such as `{expr; format}`.
- Operator traits for arithmetic, bitwise, comparison, and assignment
  operators.
- A synthetic diagnostic harness for hard-limit error categories that are
  impractical to trigger through normal source files.
