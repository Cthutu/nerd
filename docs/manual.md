# Nerd Manual Authoring Map

This document is the authoring map for the learner-facing Nerd language manual.
It is not the manual itself and it is not an implementation guide for the
compiler. The goal of the manual is to teach the language from first principles,
using examples that can be compiled and run with the current toolchain.

The manual should be written as a sequence of logical sections. Each section
introduces one cluster of concepts, explains the rules, then shows small
programs that exercise those rules. Longer examples should build toward the
style used in [examples/text-adventure/adv.n](/home/matt/nerd/examples/text-adventure/adv.n).

The manual lives in separate part files under `docs/manual/`. This file is
retained as a maintenance checklist for future manual revisions.

Current layout:

- `docs/manual/README.md`
  Manual index, audience, setup instructions, and reading order.
- `docs/manual/part01-first-programs.md`
- `docs/manual/part02-values-bindings-and-assignment.md`
- `docs/manual/part03-primitive-types-and-expressions.md`
- `docs/manual/part04-functions.md`
- `docs/manual/part05-blocks-scope-and-cleanup.md`
- `docs/manual/part06-branching-with-on.md`
- `docs/manual/part07-loops.md`
- `docs/manual/part08-compound-data.md`
- `docs/manual/part09-dynamic-arrays-and-manual-memory.md`
- `docs/manual/part10-modules.md`
- `docs/manual/part11-interoperability-with-c.md`
- `docs/manual/part12-building-a-small-program.md`
- `docs/manual/part13-diagnostics-and-debugging.md`
- `docs/manual/appendix-a-syntax-reference.md`
- `docs/manual/appendix-b-language-reference.md`

Each part file should be readable on its own, but should start with previous
and next links so the full manual has a clear linear path.

## Audience And Style

- Assume the reader can program, but has never used Nerd.
- Prefer executable examples over abstract grammar.
- Introduce syntax only when the reader needs it.
- Keep implementation details out of the main text unless they affect source
  behaviour.
- Use the standard library sparingly for simple examples, but do not document
  the standard library API in the manual.
- Use language tests only as an internal authoring checklist. Do not link to
  tests from generated user-facing manual parts.

## Example Rules

Each example in the manual should be one of:

- a complete file that can run with `nerd run file.n`
- a focused snippet explicitly marked as partial
- a transcript showing source and expected output

Complete examples should prefer:

```nerd
use std.io

main :: fn () {
    prn("hello")
}
```

When a feature depends on module qualification, use:

```nerd
io :: use std.io

main :: fn () {
    io.prn("hello")
}
```

## Part 1: First Programs

Purpose: get the reader from an empty file to a running program.

Concepts:

- source files and `nerd run`
- `main` as the entry point
- comments with `--`
- expression-bodied and block-bodied functions
- `void` return behaviour
- importing `std.io`
- printing with `pr` and `prn`

Rules to teach:

- `main` is the program entry point.
- A block function uses `{ ... }` and explicit `return` when it returns a value.
- A function returning `void` can end naturally.
- Top-level names are introduced with `::` for constant/function bindings.

Authoring coverage:

- [010-hello-world.t](/home/matt/nerd/tests/language/010-hello-world.t)
- [012-block-return.t](/home/matt/nerd/tests/language/012-block-return.t)
- [022-void-main.t](/home/matt/nerd/tests/language/022-void-main.t)

## Part 2: Values, Bindings, And Assignment

Purpose: teach how values are named, typed, and updated.

Concepts:

- top-level constant bindings with `::`
- local inferred bindings with `:=`
- typed local bindings with `name: Type = value`
- mutable variables with `name: Type`
- intentionally uninitialised values with `undefined`
- assignment with `=`
- compound assignment with `+=`, `-=`, `*=`, `/=`, and `%=` where supported
- forward references between top-level bindings

Rules to teach:

- `::` creates a constant binding.
- `:=` creates a new local binding with inferred type.
- `=` assigns to an existing mutable target.
- Annotated locals use `:` and can be left initialised by the type's zero value
  when the language permits that form.
- `undefined` can initialise a value whose contents will be assigned before
  meaningful use. It is an escape hatch, not a normal default.
- Top-level declarations can refer to later top-level declarations.

Authoring coverage:

- [006-global-vars.t](/home/matt/nerd/tests/language/006-global-vars.t)
- [007-forward-refs.t](/home/matt/nerd/tests/language/007-forward-refs.t)
- [013-typed-binding.t](/home/matt/nerd/tests/language/013-typed-binding.t)
- [015-variables.t](/home/matt/nerd/tests/language/015-variables.t)
- [040-compound-assignments.t](/home/matt/nerd/tests/language/040-compound-assignments.t)
- [085-undefined.t](/home/matt/nerd/tests/language/085-undefined.t)

## Part 3: Primitive Types And Expressions

Purpose: explain the core expression language.

Concepts:

- integer, float, bool, string, C string, and `void`
- `yes` and `no`
- unary operators
- arithmetic, comparison, logical, and bitwise operators
- precedence and grouping
- casts with `.as(Type)`
- pointer-to-slice casts with `.as([]T, count)`
- packed integer literals
- digit separators in integer and float literals
- string interpolation with `$"..."`
- continuation string literals with `+"..."`

Rules to teach:

- Untyped numeric literals are resolved by context where possible.
- Boolean values are `yes` and `no`.
- Casts are explicit.
- Pointer-to-slice casts require an explicit element type and count:
  `pointer.as([]T, count)`.
- Interpolated strings evaluate embedded expressions and produce `string`.
- C strings are distinct from Nerd `string` values and are mainly for FFI.

Authoring coverage:

- [003-precedence.t](/home/matt/nerd/tests/language/003-precedence.t)
- [004-unary-and-grouping.t](/home/matt/nerd/tests/language/004-unary-and-grouping.t)
- [018-primitive-variables.t](/home/matt/nerd/tests/language/018-primitive-variables.t)
- [019-interpolated-strings.t](/home/matt/nerd/tests/language/019-interpolated-strings.t)
- [031-primitive-operators.t](/home/matt/nerd/tests/language/031-primitive-operators.t)
- [075-packed-integer-literals.t](/home/matt/nerd/tests/language/075-packed-integer-literals.t)
- [086-c-strings-and-escapes.t](/home/matt/nerd/tests/language/086-c-strings-and-escapes.t)
- [093-slice-casts-and-nil.t](/home/matt/nerd/tests/language/093-slice-casts-and-nil.t)
- [095-void-pointer-compat.t](/home/matt/nerd/tests/language/095-void-pointer-compat.t)

## Part 4: Functions

Purpose: teach functions as declarations and as values.

Concepts:

- function parameters
- explicit return types
- block functions and `return`
- expression-bodied functions with `=>`
- nested functions
- function values
- function type annotations
- local function forward references

Rules to teach:

- Function types are written `fn (ParamTypes...) -> ReturnType`.
- Block functions use explicit `return` to produce a value.
- Expression-bodied functions infer from the body expression.
- Nested functions can capture only according to the currently implemented local
  binding rules.

Authoring coverage:

- [014-annotated-function-type.t](/home/matt/nerd/tests/language/014-annotated-function-type.t)
- [017-function-type-alias.t](/home/matt/nerd/tests/language/017-function-type-alias.t)
- [021-function-parameters.t](/home/matt/nerd/tests/language/021-function-parameters.t)
- [023-function-values.t](/home/matt/nerd/tests/language/023-function-values.t)
- [024-nested-functions.t](/home/matt/nerd/tests/language/024-nested-functions.t)
- [026-local-function-forward-ref.t](/home/matt/nerd/tests/language/026-local-function-forward-ref.t)
- [084-nested-local-fn-in-block-body.t](/home/matt/nerd/tests/language/084-nested-local-fn-in-block-body.t)

## Part 5: Blocks, Scope, And Cleanup

Purpose: explain lifetime-shaped control flow before loops become complex.

Concepts:

- nested blocks
- statement blocks versus expression blocks
- scope of locals
- `defer <statement>`
- `defer { ... }`
- cleanup on natural exit, `return`, `break`, and `again`

Rules to teach:

- A normal block groups statements and creates a local scope.
- An expression block can produce a value with `break <expr>`.
- Deferred statements run when the current scope exits.
- Defers run in last-in, first-out order.
- A deferred statement must be valid where it appears semantically; invalid
  deferred `break` or `again` is still rejected.

Authoring coverage:

- [020-nested-blocks.t](/home/matt/nerd/tests/language/020-nested-blocks.t)
- [045-expression-block.t](/home/matt/nerd/tests/language/045-expression-block.t)
- [047-expression-block-bindings.t](/home/matt/nerd/tests/language/047-expression-block-bindings.t)
- [048-labelled-expression-block.t](/home/matt/nerd/tests/language/048-labelled-expression-block.t)
- [103-defer.t](/home/matt/nerd/tests/language/103-defer.t)

## Part 6: Branching With `on`

Purpose: teach Nerd's main conditional and pattern-matching construct.

Concepts:

- short-form boolean `on condition => expr else expr`
- statement-form `on`
- block-form value `on`
- scrutinee-less condition chains
- `else`
- comma alternatives
- string and integer patterns
- ranges with `..` and `..=`
- comparison patterns
- pattern binders
- pattern guards
- structural patterns
- enum patterns

Rules to teach:

- Short-form `on` can be an expression or statement depending on context.
- Value-producing `on` forms must be exhaustive.
- Scrutinee-less `on { ... }` branches use boolean conditions.
- Pattern binders introduce locals scoped to the selected branch.
- Guards refine a pattern but do not by themselves make an enum match
  exhaustive.

Authoring coverage:

- [027-on-bool.t](/home/matt/nerd/tests/language/027-on-bool.t)
- [028-on-value-branches.t](/home/matt/nerd/tests/language/028-on-value-branches.t)
- [029-on-comma-patterns.t](/home/matt/nerd/tests/language/029-on-comma-patterns.t)
- [030-on-ranges.t](/home/matt/nerd/tests/language/030-on-ranges.t)
- [035-on-pattern-binders.t](/home/matt/nerd/tests/language/035-on-pattern-binders.t)
- [060-on-pattern-guards.t](/home/matt/nerd/tests/language/060-on-pattern-guards.t)
- [061-on-structural-patterns.t](/home/matt/nerd/tests/language/061-on-structural-patterns.t)
- [066-generalised-on.t](/home/matt/nerd/tests/language/066-generalised-on.t)
- [098-on-branch-loop-control.t](/home/matt/nerd/tests/language/098-on-branch-loop-control.t)

## Part 7: Loops

Purpose: teach iteration and non-local loop control.

Concepts:

- infinite `for`
- condition loops
- C-style `for init; condition; update`
- `for in`
- indexed `for index, item in collection`
- item pointer iteration
- `break`
- `again`
- labelled expression blocks and loops
- value-producing loops
- loop `else`

Rules to teach:

- `break` exits a loop, or an expression block when targeting one.
- `again` resumes a loop.
- A value-producing finite loop needs an `else` branch for normal exhaustion.
- Labels disambiguate nested loop and expression-block targets.
- Indexed `for in` binds the index as `usize`.

Authoring coverage:

- [038-for-infinite.t](/home/matt/nerd/tests/language/038-for-infinite.t)
- [041-for-while.t](/home/matt/nerd/tests/language/041-for-while.t) for condition loops
- [042-for-c-style.t](/home/matt/nerd/tests/language/042-for-c-style.t)
- [044-for-break-again.t](/home/matt/nerd/tests/language/044-for-break-again.t)
- [049-labelled-loop-expressions.t](/home/matt/nerd/tests/language/049-labelled-loop-expressions.t)
- [050-for-else.t](/home/matt/nerd/tests/language/050-for-else.t)
- [076-for-in-and-deref.t](/home/matt/nerd/tests/language/076-for-in-and-deref.t)

## Part 8: Compound Data

Purpose: introduce the data model from simple products to tagged variants.

Concepts:

- tuples
- fixed arrays
- array literals
- pointers with `^`
- dereference with postfix `^`
- slices
- casting pointers to slices with an explicit count
- string slices
- nil pointers and nil slices
- plexes
- raw unions
- enums with unit and payload variants
- field access
- field assignment and compound field assignment
- destructuring bindings and assignments

Rules to teach:

- Tuples are positional; plexes are named-field product types.
- Fixed arrays carry length in their type.
- Slices are data/count views.
- `p.as([]T, count)` creates a slice view from a pointer and explicit element
  count. The slice does not own the pointed-to storage.
- `string` behaves like a UTF-8 slice-like type but is distinct from `[]u8`.
- Pointers and nil must be handled explicitly.
- Raw unions are low-level storage without automatic active-field tracking.
- Enums are tagged values and should be preferred when the active case matters.

Authoring coverage:

- [051-tuples.t](/home/matt/nerd/tests/language/051-tuples.t)
- [052-fixed-arrays.t](/home/matt/nerd/tests/language/052-fixed-arrays.t)
- [053-pointers.t](/home/matt/nerd/tests/language/053-pointers.t)
- [054-slices.t](/home/matt/nerd/tests/language/054-slices.t)
- [055-string-slices.t](/home/matt/nerd/tests/language/055-string-slices.t)
- [056-plexes.t](/home/matt/nerd/tests/language/056-plexes.t)
- [059-destructuring-bindings.t](/home/matt/nerd/tests/language/059-destructuring-bindings.t)
- [062-raw-unions.t](/home/matt/nerd/tests/language/062-raw-unions.t)
- [063-enum-unit-variants.t](/home/matt/nerd/tests/language/063-enum-unit-variants.t)
- [065-enum-payloads.t](/home/matt/nerd/tests/language/065-enum-payloads.t)
- [089-field-lvalues.t](/home/matt/nerd/tests/language/089-field-lvalues.t)
- [090-nil-pointers.t](/home/matt/nerd/tests/language/090-nil-pointers.t)
- [093-slice-casts-and-nil.t](/home/matt/nerd/tests/language/093-slice-casts-and-nil.t)

## Part 9: Dynamic Arrays And Manual Memory

Purpose: teach the growable array surface and the cleanup discipline it implies.

Concepts:

- dynamic array type syntax `[N..]T` and `[..]T`
- `.count` and `.capacity`
- `.push`
- `.delete` and `.swap_delete`
- `.append`
- `.reserve`
- `.clear`
- `.free`
- slicing dynamic arrays
- returning dynamic arrays from functions
- using `defer` for cleanup

Rules to teach:

- Dynamic arrays own heap storage when allocated.
- `.free()` releases that storage and resets the array to nil.
- `.clear()` keeps capacity but resets count.
- Slices view array contents; they do not own the storage.
- Use `defer array.free()` or `defer { array.free() }` when a scope owns the
  array and has multiple exits.

Authoring coverage:

- [080-dynamic-slice-bounds.t](/home/matt/nerd/tests/language/080-dynamic-slice-bounds.t)
- [081-nested-array-literals.t](/home/matt/nerd/tests/language/081-nested-array-literals.t)
- [099-dynamic-arrays.t](/home/matt/nerd/tests/language/099-dynamic-arrays.t)
- [101-dynarray-typed-locals.t](/home/matt/nerd/tests/language/101-dynarray-typed-locals.t)
- [103-defer.t](/home/matt/nerd/tests/language/103-defer.t)

## Part 10: Modules

Purpose: teach multi-file code and module boundaries. Standard library modules
can be used for small examples, but the manual should not try to be the
standard library reference.

Concepts:

- `use std.io`
- grouped `use`
- module bindings with `name :: use module.path`
- public exports with `pub`
- qualified access with `module.name`
- re-exporting

Rules to teach:

- `use` imports public names into the current scope.
- `name :: use module.path` binds the module value, and public members are
  accessed through fields.
- Only `pub` declarations are visible outside a module.
- Qualified module access is useful when avoiding local namespace pollution.
- The standard library is still in development and should be documented
  separately from the core language manual.

Authoring coverage:

- [068-std-print-module.t](/home/matt/nerd/tests/language/068-std-print-module.t)
- [071-use-modules.t](/home/matt/nerd/tests/language/071-use-modules.t)
- [073-module-pub-reexport.t](/home/matt/nerd/tests/language/073-module-pub-reexport.t)
- [087-grouped-use.t](/home/matt/nerd/tests/language/087-grouped-use.t)
- [091-imported-plex-field-interpolation.t](/home/matt/nerd/tests/language/091-imported-plex-field-interpolation.t)
- [102-io-input-lines.t](/home/matt/nerd/tests/language/102-io-input-lines.t)

## Part 11: Interoperability With C

Purpose: explain the low-level boundary clearly and conservatively.

Concepts:

- `ffi "c" name (...) -> Type`
- `local_name :: ffi "c" foreign_name (...) -> Type`
- variadic FFI calls
- C strings
- pointer-compatible values
- pointer-to-slice casts with `p.as([]T, count)`
- linking external libraries
- wrapper functions around FFI
- ABI-safe type choices

Rules to teach:

- A bare `ffi "lib" name (...)` declares a foreign function whose Nerd-visible
  name is the same as the foreign symbol.
- A bound `local_name :: ffi "lib" foreign_name (...)` declares a foreign
  function with a different Nerd-visible name. Calls use `local_name`; generated
  code links to `foreign_name`.
- Inside an `ffi "lib" { ... }` block, `local_name :: foreign_name (...)`
  declares one renamed entry without repeating the library operand.
- Prefer wrapping FFI declarations in Nerd functions that expose safer types.
- C strings and Nerd strings are different.
- Use explicit casts at the FFI boundary.
- When C returns a raw pointer plus a length, convert it to a Nerd slice with
  `p.as([]T, count)` and keep ownership rules explicit.
- Keep ownership rules explicit when C allocates or receives memory.

Authoring coverage:

- [067-ffi-functions.t](/home/matt/nerd/tests/language/067-ffi-functions.t)
- [069-ffi-varargs.t](/home/matt/nerd/tests/language/069-ffi-varargs.t)
- [070-ffi-library-linking.t](/home/matt/nerd/tests/language/070-ffi-library-linking.t)
- [088-ffi-consolidation.t](/home/matt/nerd/tests/language/088-ffi-consolidation.t)
- [094-ffi-wrapper-bindings.t](/home/matt/nerd/tests/language/094-ffi-wrapper-bindings.t)
- [095-void-pointer-compat.t](/home/matt/nerd/tests/language/095-void-pointer-compat.t)

## Part 12: Building A Small Program

Purpose: combine the earlier material into one guided project.

Recommended project:

- a small command-line text adventure
- input through `std.io.input`
- command parsing with `std.string.split`
- command dispatch with `on`
- dynamic arrays for split words
- `defer parts.free()` or `defer { parts.free() }`
- simple world state stored in plexes or enums

This part should be based on
[examples/text-adventure/adv.n](/home/matt/nerd/examples/text-adventure/adv.n),
but each chapter should evolve the program in small steps rather than present
the final file immediately.

## Part 13: Diagnostics And Debugging

Purpose: teach readers how to interpret compiler feedback.

Concepts:

- lexer, parser, and semantic diagnostic ranges
- primary spans
- notes and help
- common mistakes:
  - unknown symbols
  - type mismatches
  - invalid assignment targets
  - missing `else` on value-producing `on`
  - invalid `break` or `again`
  - module privacy errors

Rules to teach:

- `0100`-`0199` diagnostics come from lexing.
- `0200`-`0299` diagnostics come from parsing and AST construction.
- `0300`-`0399` diagnostics come from semantic analysis.
- Treat help text as a suggested source change, and notes as extra context.

Authoring references:

- [error-system.md](/home/matt/nerd/docs/error-system.md)
- [tests/errors](/home/matt/nerd/tests/errors)

## Appendix A: Syntax Reference

This appendix should be compact and complete. It is now the source-level syntax
reference for the project.

Suggested subsections:

- declarations
- expressions
- statements
- types
- patterns
- literals
- operators

## Internal Authoring Coverage Map

Keep a private table mapping manual sections to language tests. This is an
authoring aid for maintainers and should not be copied into generated
user-facing manual parts.

Example:

| Manual Section | Primary Tests |
| --- | --- |
| First Programs | `010`, `012`, `022` |
| Values, Bindings, And Assignment | `006`, `007`, `013`, `015`, `040`, `085` |
| Primitive Types And Expressions | `003`, `004`, `018`, `019`, `031`, `075`, `086`, `093`, `095` |
| Branching With `on` | `027`-`030`, `035`, `060`, `061`, `066` |
| Loops | `038`, `041`-`044`, `049`, `050`, `076` |
| Compound Data | `051`-`056`, `059`, `062`, `063`, `065`, `089`, `090`, `093` |
| Dynamic Arrays And Manual Memory | `080`, `081`, `099`, `101`, `103` |

## Writing Order

Write the manual in this order:

1. First Programs
2. Values, Bindings, And Assignment
3. Primitive Types And Expressions
4. Functions
5. Branching With `on`
6. Loops
7. Compound Data
8. Dynamic Arrays And Manual Memory
9. Blocks, Scope, And Cleanup
10. Modules
11. Interoperability With C
12. Building A Small Program
13. Diagnostics And Debugging
14. Appendices

The teaching order differs slightly from the part numbering: write common
control flow before cleanup, then revise the cleanup section once examples can
use loops and multiple exits naturally.
