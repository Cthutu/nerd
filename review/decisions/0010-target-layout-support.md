# 0010: Target Layout Support

Status: accepted
Date: 2026-05-12

## Context

The LLVM backend now owns the executable path. It emits textual LLVM IR and
lets clang select the final target triple and data layout. During MS5 we found
that the backend already had a coherent target model, but it was implicit:
64-bit pointers, `isize`/`usize` as `i64`, two-word strings and slices,
three-word dynamic-array headers, `i64` enum tags, and runtime helper
declarations that assume the same widths.

The immediate goal is to keep the current compiler installable and predictable,
not to claim cross-target ABI support before the layout work exists.

## Decision

Nerd currently supports the host 64-bit clang target used to build and run the
compiler. The backend target contract is:

- opaque LLVM pointers
- pointer-sized integer values lower as `i64`
- `string` and slice values lower as `{ ptr, i64 }`
- dynamic-array handles are nullable pointers to `{ ptr, i64, i64 }`
- enum tags lower as `i64`
- enum payload storage is rounded to a 64-bit boundary
- runtime string helpers use pointer/scalar ABI, not by-value C structs
- libc allocation declarations use `i64` byte counts
- clang owns the final target triple and data layout

Cross-target builds and 32-bit targets are not supported yet. Aggregate FFI
passing and returning is not a supported ABI contract unless a test and a
documented lowering rule explicitly cover the shape.

## Consequences

The new layout context should be treated as the single place to grow target
support. Future target work should change that context first, then update LLVM
type spelling, storage sizing, dynamic-array layout, runtime helper
declarations, and tests together.

The compiler may still generate aggregate values for Nerd-to-Nerd calls inside
generated LLVM. That is not the same as promising those aggregate values match a
C ABI when crossing an FFI boundary.

## Follow-Up

- Add a target-selection mechanism only when there is a real second target to
  test.
- Add target datalayout/triple emission before claiming cross-target
  reproducibility.
- Move memory-operation alignment policy through the layout context.
- Add explicit diagnostics for unsupported aggregate FFI shapes if users hit
  them.
