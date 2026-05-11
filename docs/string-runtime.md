# String Runtime

This document describes the first interpolated-string runtime. Source-level
string rules belong in the manual; this note is for lowering and runtime
implementation details.

## Scope

The current implementation is intentionally simple:

- interpolated strings with only compile-time parts lower to string literals
- runtime interpolated strings are temporary values only
- lowering is explicit in HIR and LLVM
- the C runtime uses a global append-only arena-backed builder
- conversion support is limited to built-in primitive types and `string`

This is a first runtime model, not the final VM-oriented design.

## HIR And LLVM Model

Interpolated strings lower through explicit HIR nodes in
[src/compiler/hir/hir.h](/home/matt/nerd/src/compiler/hir/hir.h) and
[src/compiler/hir/gen.c](/home/matt/nerd/src/compiler/hir/gen.c), then to
runtime helper calls in
[src/compiler/llvm/llvm.c](/home/matt/nerd/src/compiler/llvm/llvm.c):

- `string.reset`
- `string.start`
- `string.append`
- `string.finish`

That keeps interpolation visible in the middle layer rather than hiding it
inside backend string concatenation code.

## Runtime Helpers

The current helpers live in [data/nrt.c](/home/matt/nerd/data/nrt.c).

The runtime provides:

- the thread-local global string arena
- `string_builder_reset()`
- `string_builder_mark()`
- `string_builder_append_string(...)`
- `string_builder_finish(...)`
- `to_string$<type>(...)` helpers for built-in primitive types

The `to_string$<type>` helpers currently use straightforward C formatting and a
shared scratch buffer, then `string_builder_append_string(...)` copies the
result into the arena.

The runtime is compiled to an object file by the build system and embedded in
the compiler binary. During executable builds the backend writes that object
beside the temporary `.link.ll` file and links both with clang. Runtime helpers
use pointer/scalar parameters for Nerd strings rather than passing or returning
string structs by value, so the same generated LLVM works with both MSVC-style
and Unix-style C ABIs.

## Lifetime Model

For this first implementation, interpolation results are temporary values that
live only for the surrounding statement. The runtime resets the thread-local
builder after a statement that uses interpolation completes.

That keeps the runtime simple and avoids exposing manual reset operations in the
language, but it also means interpolated strings may not escape statement
scope. Returning them, assigning them to variables, or storing them in other
bindings is rejected in sema for now unless the interpolation is fully
compile-time and lowers to an ordinary string literal.

Top-level interpolated bindings are allowed when all interpolation parts are
compile-time values. Top-level interpolations that need runtime string building
are still rejected.

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
