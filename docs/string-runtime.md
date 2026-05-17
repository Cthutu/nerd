# String Runtime

This document describes the first interpolated-string runtime. Source-level
string rules belong in the manual; this note is for lowering and runtime
implementation details.

## Scope

The current implementation is intentionally simple:

- interpolated strings with only compile-time parts lower to string literals
- runtime interpolated strings are backed by the global temporary string arena
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

Runtime interpolation results are allocated from a thread-local temporary string
arena. They may be returned, assigned to variables, and passed through ordinary
`string` values. The storage remains valid until the temporary arena is reset.

The `core.temp_arena.reset()` method resets this storage explicitly. Programs
with request or frame loops should call it at a clear boundary after temporary
strings from the previous iteration are no longer needed. Programs that never
reset the temporary arena keep these strings for the process lifetime.

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

Top-level runtime interpolated bindings still produce a dedicated error because
there is no runtime statement context in which to build them during module
initialisation.
