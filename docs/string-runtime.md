# String Runtime

This document describes the first interpolated-string runtime introduced for
Milestone 6.

## Scope

The current implementation is intentionally simple:

- interpolated strings are only allowed inside functions
- interpolated strings are temporary values only
- lowering is explicit in IR
- generated C uses a global append-only arena-backed builder
- conversion support is limited to built-in primitive types and `string`

This is a first runtime model, not the final VM-oriented design.

## IR Model

Interpolated strings lower through explicit IR instructions in
[src/compiler/ir/ir.h](/home/matt/nerd/src/compiler/ir/ir.h) and
[src/compiler/ir/gen.c](/home/matt/nerd/src/compiler/ir/gen.c):

- `string.reset`
- `string.start`
- `string.append`
- `string.finish`

That keeps interpolation visible in IR rather than hiding it inside C code
generation. The current C backend still uses semantic type tables for some type
lookups, but the string-building behaviour itself now exists as IR operations.

## Runtime Helpers

The current helpers live in [data/prelude.c](/home/matt/nerd/data/prelude.c).

The prelude provides:

- the thread-local global string arena
- `string_builder_reset()`
- `string_builder_mark()`
- `string_builder_append_string(...)`
- `string_builder_finish(...)`
- `to_string$<type>(...)` helpers for built-in primitive types

The `to_string$<type>` helpers currently use straightforward C formatting and a
shared scratch buffer, then `string_builder_append_string(...)` copies the
result into the arena.

## Lifetime Model

For this first implementation, interpolation results are temporary values that
live only for the surrounding statement. The runtime resets the thread-local
builder after a statement that uses interpolation completes.

That keeps the runtime simple and avoids exposing manual reset operations in the
language, but it also means interpolated strings may not escape statement
scope. Returning them, assigning them to variables, or storing them in other
bindings is rejected in sema for now.

## Semantic Rules

Inside interpolation braces, sema currently accepts:

- `string`
- `bool`
- integer types
- `f32`
- `f64`

Unsupported values, such as function-typed expressions, produce a dedicated
semantic error.

Interpolated strings also produce a dedicated error when they are used in an
escaping context.
