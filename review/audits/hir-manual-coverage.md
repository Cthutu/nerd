# HIR Manual Coverage Audit

Status: current
Date: 2026-05-11

## Purpose

Milestone 4 asks that every documented source construct has one of:

- HIR lowering
- an explicit unsupported diagnostic
- a tracked follow-up

This audit maps the manual surface to the current HIR and LLVM backend tests.
It is a coverage guide, not a replacement for the focused `tests/hir` and
`tests/llvm` snapshots.

## Summary

The executable LLVM path covers the documented language well enough for the
current regression suite: `just test` passes. HIR coverage is broad, but the
textual HIR still contains `<unsupported>` placeholders for a few accepted
constructs that LLVM handles through default-value or special-case lowering.

The important gaps are:

- uninitialised locals and zero/default initialisation
- module-valued local bindings from `local :: use module`
- local type declarations
- generic template type records and some instantiated generic local values
- dynamic-array default literals and zero initialisation

Those are not user-facing compile failures today, but they violate the MS4 goal
that HIR is a complete checked language rather than a mix of HIR plus backend
escape hatches.

## Manual Coverage Matrix

| Manual area | Source constructs | HIR status | Primary coverage |
|---|---|---|---|
| Part 1: first programs | comments, top-level bindings, block/expression functions, printing through `std.io` | Covered. Comments do not lower to HIR by design. | `tests/hir/001-functions.hir`, `021-expression-bodied-functions.hir`, language `010`, `068` |
| Part 2: values/bindings | `::`, `:=`, typed locals, assignment, compound assignment, `undefined`, unused locals | Mostly covered. Uninitialised/default locals currently render as `<unsupported>` even though LLVM emits default storage. | `tests/hir/004-locals.hir`, `009-assignments.hir`, language `015`, `085`, `109` |
| Part 3: primitives | numeric ops, boolean ops, `.size`, casts, pointer-to-slice casts, strings, C strings, untyped literals | Covered in HIR/LLVM snapshots. `.size` is represented as field access on type-like values. | `tests/hir/005`-`008`, language `108`, `125`, command `077` |
| Part 4: functions | parameters, default parameters, return types, function types, generic functions, nested functions, forward references | Mostly covered. Function entities and local function literals are represented; generic templates still include `<unknown>`/placeholder details in some HIR snapshots. | `tests/hir/002`, `019`, `021`, language `126`, `128`, `129` |
| Part 5: blocks/scope/cleanup | nested blocks, expression blocks, labels, `assert`, `defer`, scope exit | Covered for HIR and LLVM execution. | `tests/hir/014`, `015`, language `045`, `048`, `103`, command `059` |
| Part 6: `on` | short boolean form, condition chains, scrutinee matching, ranges, binders, guards, exhaustiveness | Covered, including leading comparison patterns and structural/enum patterns. | `tests/hir/016`, `017-nested-call-args`, LLVM `016`, `021`-`024`, language `151`, `153` |
| Part 7: loops | infinite/condition/C-style loops, `continue`, `for in`, loop values, labels | Covered. `for in` HIR shows item locals as pointers. | `tests/hir/017-for-forms.hir`, LLVM `017`-`020`, command `079`, `082` |
| Part 8: compound data | tuples, fixed arrays, slices, strings, pointers, nil, plexes, generic compound types, methods, destructuring, raw unions, enums | Broadly covered. Gaps remain for local type declarations and some generic template HIR text. | `tests/hir/010`-`013`, `018`, language `056`-`065`, `127`, `130`, `142`, `144`, `151` |
| Part 9: dynamic arrays | type syntax, push/pop/reserve/clear/free, slicing, ownership with `defer`, returning arrays | Executable lowering covered. HIR still renders default dynamic-array locals as `<unsupported>` in several fixtures. | language `099`, `101`, commands `068`-`076`, `081` |
| Part 10: modules | module parts, `use`, platform-gated imports, module bindings, public exports, re-exports | Covered for imports/exports and module bindings. Local module binding currently renders as `<unsupported>`. | `tests/hir/020-module-bindings.hir`, language `071`-`073`, `087`, `131`, `132`, `134`, `135`, `150` |
| Part 11: C interop | FFI declarations, bound FFI, library operands, return types, varargs, C strings, pointer/slice boundaries, wrappers, ABI-safe types | Covered, including extern records and link flags. | `tests/hir/003`, language `067`, `069`, `070`, `088`, `094`, `113`, `149` |
| Part 13: diagnostics | lexer/parser/sema diagnostics, source tests, read-before-assignment, control-flow errors | Diagnostics are not HIR products. Source tests intentionally skip HIR generation in the command path. | `tests/errors/*`, commands `009`-`014` |

Part 12 is an integrated tutorial rather than a distinct language surface.

## Current `<unsupported>` HIR Cases

The following accepted constructs currently produce `<unsupported>` in HIR
snapshots:

- uninitialised locals and default initialisation:
  - `tests/language/015-variables.t`
  - `tests/language/085-undefined.t`
  - `tests/language/109-definite-assignment.t`
- module-valued local bindings:
  - `tests/language/068-std-print-module.t`
  - `tests/language/071-use-modules.t`
- dynamic-array default/zero initialisation:
  - `tests/language/099-dynamic-arrays.t`
  - `tests/language/100-on-short-block-expr.t`
  - `tests/language/101-dynarray-typed-locals.t`
  - `tests/language/119-slice-element-pointers.t`
  - `tests/language/143-dynarray-runtime-capacity-and-resize.t`
- generic and method-heavy local values:
  - `tests/language/127-generic-types.t`
  - `tests/language/128-generic-functions.t`
  - `tests/language/129-cross-module-generics.t`
  - `tests/language/130-inherent-impl-methods.t`
  - `tests/language/138-named-call-arguments.t`
  - `tests/language/141-method-regressions.t`
  - `tests/language/142-lvalue-and-plex-regressions.t`
  - `tests/language/147-qualified-module-types.t`
- local type declarations:
  - `tests/language/144-local-type-declarations.t`
- pointer/nil and expression-valued control-flow edge cases:
  - `tests/language/144-pointer-equality-and-nil-on.t`
  - `tests/language/152-on-break-for-expression.t`
  - `tests/language/153-break-on.t`

Many of these placeholders are generated for syntactic wrapper nodes, type
declaration nodes, or zero/default values that LLVM already handles with
special-case default lowering. MS4 should remove those placeholders from normal
accepted HIR output or record explicit "not a value" HIR nodes where the source
construct is a declaration-only form.

## Recommended MS4 Follow-Up

1. Add focused HIR snapshots for anonymous entity/binding cases:
   functions, types, globals, imports, exports, local function values, and
   module bindings.
2. Replace accepted `<unsupported>` default values with explicit HIR, probably
   `default <type>` or a zero-init expression.
3. Represent module values and local type declarations explicitly enough that
   they do not appear as unsupported expressions in accepted code.
4. Add focused HIR/LLVM tests for expression-valued `on`/loop control cases
   that currently rely on language fixtures only.
5. Add focused HIR tests for dynamic-array default values, pointer auto-deref,
   and generic aggregate values.
