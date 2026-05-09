# HIR Backend Readiness Audit

Status: started

## Current Shape

HIR is now a typed, derived program view emitted after semantic analysis. It is
useful for review because it renders a readable language of functions, blocks,
statements, typed expressions, loops, and `on` forms.

Current HIR owns:

- normal, FFI, and generic-instantiated function records
- function parameters and body blocks
- local `let`, assignment, return, assert, defer, break, continue, expression,
  and nested block statements
- integer, float, string, bool, nil, local-ref, unary, binary, call, cast,
  index, tuple, tuple-field, array, field, plex, plex-update, slice, range,
  block, `on`, and `for` expressions
- `on` branches, guards, binders, structural patterns, enum variant patterns,
  and comparison patterns
- condition, C-style, and `for in` loops, including expression-valued loops and
  `else` blocks
- stable text snapshots under `tests/hir/`

This is enough to discuss HIR as a language in its own right, but it is not yet
enough to replace the current IR/C path.

## Backend Blockers

### Whole-Program Records

HIR currently starts from ordered semantic declarations but only materialises
functions. A backend needs first-class records for:

- modules and source units
- type declarations and aliases
- global constants and mutable globals
- global initialisation code
- imported/exported symbols
- runtime/prelude requirements

Without these, HIR cannot own dependency-order decisions or emit a complete
program.

### Type And ABI Model

HIR nodes carry `Sema` type indices. That is sufficient for snapshots, but a
backend needs a target-facing type layer with explicit layout decisions:

- integer and float widths
- pointer, slice, dynamic-array, tuple, plex, enum, raw-union, and function
  layouts
- aggregate passing and returning rules
- varargs and C ABI calls
- string literal storage
- nil representation
- alignment and data layout sourced from the backend target

The LLVM backend should not infer ABI rules ad hoc while emitting instructions.
It needs a dedicated HIR-backend type/layout context.

### Runtime Operations

Several language features are still represented as high-level expressions or
lowered later by the current IR/C path. Direct LLVM lowering needs explicit HIR
or backend operations for:

- dynamic-array reserve, resize, push, append, delete, swap-delete, pop, clear,
  and free
- string interpolation construction
- `assert` failure paths
- `defer` execution ordering
- source-test/runtime entry plumbing
- module init and global init
- prelude/epilogue symbols used by generated code

These can stay as structured HIR nodes if we want HIR to remain high-level, but
the lowering contract must say where each runtime operation becomes calls and
control flow.

### Control-Flow Lowering

HIR preserves structured control flow. LLVM needs blocks, branches, phis or
alloca/store/load slots for expression-valued constructs. The backend contract
must define lowering for:

- expression blocks
- expression-valued `for`
- `break`/`continue` with labels and values
- `on` expressions and pattern binders
- short `on` statements
- `defer` in the presence of return, break, and continue

Structured HIR is still the right shape, but the LLVM backend needs a local CFG
builder rather than trying to map statements one-by-one.

### Names And Linkage

The LLVM decision is:

- Nerd-visible symbols keep the `$` prefix, for example `@"$main"`.
- Compiler-created internals use generated names such as `@fn.N`,
  `@global.N`, and `%type.N`.

The missing piece is a whole-program symbol table that decides:

- exported names
- internal names
- imported FFI names
- aliases, if we decide to bind public Nerd names to generated internals
- platform-specific linkage and visibility

## Textual HIR Gaps

The textual representation is intentionally derived, not a constraint on the
data layout. It still needs coverage for backend-relevant top-level products:

- type declarations and resolved layouts
- globals and init blocks
- module/import/export records
- runtime operation nodes
- generated temporaries, if and only if they become part of HIR
- optional source spans or declaration IDs for debug builds

The default snapshot should stay readable. Low-level IDs should be reserved for
an explicit debug dump mode.

## Proposed Migration Sequence

1. Add whole-program HIR records for types, globals, externs, modules, and init
   blocks without changing the default backend.
2. Extend HIR snapshots to cover those records with small tests for globals,
   aliases, enums, dynamic arrays, modules, and FFI blocks.
3. Introduce a backend layout context that maps HIR/Sema types to target types
   and records ABI decisions.
4. Add an experimental HIR-to-LLVM emitter for the smallest executable subset:
   functions, integers, locals, calls, returns, and `@"$main"`.
5. Link generated LLVM with the existing clang-produced prelude/epilogue bridge.
6. Grow LLVM lowering by feature families: aggregates, pointers/slices,
   structured control flow, `on`, dynamic arrays/strings, modules/globals, then
   generics/runtime polish.
7. Retire current IR only after HIR owns every program product currently needed
   by C generation and the LLVM path can run the meaningful language snapshots.

## Immediate Next Checks

- Add HIR snapshots for global variables and type declarations to make the
  current omission visible.
- Decide whether HIR should contain explicit `global`, `type`, and `init`
  records before adding LLVM emission.
- Keep C generation on the current IR until those products exist.
