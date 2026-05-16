# Runtime Model

This document records language-visible runtime behaviour derived from semantic
analysis and LLVM lowering. Source anchors include `src/compiler/sema/sema.c`,
`src/compiler/llvm/llvm.c`, `src/compiler/hir/gen.c`, and runtime helper notes
in `docs/string-runtime.md`.

## Values And Storage

Bindings are immutable values. Variables allocate mutable storage. Typed
variables without an initializer are zero-initialised. Typed variables
initialised with `undefined` deliberately skip initialisation and are protected
by definite-assignment checks.

## Zero Initialisation

Zero initialisation follows the backend representation:

| Type family                | Zero value                                   |
| -------------------------- | -------------------------------------------- |
| Numeric                    | `0` or `0.0`                                 |
| `bool`                     | `no`                                         |
| Pointer                    | nil pointer                                  |
| Slice/string/dynamic array | zeroed pointer/count/capacity representation |
| Tuple/plex/union/enum      | zeroed aggregate representation              |

## Nil

`nil` materialises only when a destination type admits a nil value, currently
pointers, slices, and dynamic arrays. Nil slices and dynamic arrays compare
against `nil` by their runtime representation.

## Strings And Slices

Runtime `string` and `[]u8` share a pointer/count representation, but are
distinct semantic types. Explicit casts support conversion between `string` and
`[]u8` when the cast is permitted by semantic analysis.

String slicing returns `string`; byte-slice slicing returns `[]u8`.

## Dynamic Arrays

Dynamic arrays own storage and expose language-recognised operations:

| Operation | Behaviour |
| --- | --- |
| `push(value)` | Append one item. |
| `append(values)` | Append a matching slice or dynamic array. |
| `reserve(count)` | Ensure storage for at least `count` items. |
| `resize(count)` | Set the live count and default-initialise new items. |
| `resize_undefined(count)` | Set the live count without initialising new items. |
| `delete(index)` | Remove one item and preserve order by shifting later items. |
| `swap_delete(index)` | Remove one item by replacing it with the last item. |
| `pop()` | Remove and return the last item. |
| `clear()` | Set the live count to zero while keeping storage. |
| `free()` | Release storage and reset to the nil representation. |

## Defer

Deferred statements run when leaving the current scope, including exits through
`return`, `break`, and `again`. Invalid deferred control flow is still rejected
by loop-control validation.
