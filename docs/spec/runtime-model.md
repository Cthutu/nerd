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
| `reserve_to(capacity)` | Ensure absolute storage for at least `capacity` items. |
| `reserve_extra(additional)` | Ensure room for `additional` more items beyond the current live count. |
| `resize_to(count)` | Set the live count to absolute `count` and default-initialise new items. |
| `resize_undefined_to(count)` | Set the live count to absolute `count` without initialising new items. |
| `extend(count)` | Add `count` default-initialised items. |
| `extend_undefined(count)` | Add `count` uninitialised items. |
| `delete(index)` | Remove one item and preserve order by shifting later items. |
| `swap_delete(index)` | Remove one item by replacing it with the last item. |
| `pop()` | Remove and return the last item. |
| `clear()` | Set the live count to zero while keeping storage. |
| `free()` | Release storage and reset to the nil representation. |

## Arenas

`core.Arena` is the source-level arena type. The runtime reserves one 4 GiB
virtual address range for each arena and commits pages inside that range as
needed, so element addresses returned by arena allocation remain stable while
the arena grows. Arena cursors and marks are 32-bit offsets within the reserved
range.

`arena(num_bytes)` creates an arena with at least that initial committed
capacity, rounded up to the platform page size. `arena(num_bytes, increment)`
also sets the page-rounded growth increment used when more pages must be
committed. Requests that would move the arena cursor beyond the reserved range
terminate at runtime.

`alloc[T](^arena)` allocates one `T` aligned for `T`; `alloc_array[T](^arena,
count)` allocates contiguous storage for `count` values. `mark()` returns the
current cursor. `restore(mark)` invalidates allocations made after that mark.
`reset()` invalidates all allocations from the arena without releasing its
reserved address range. `done()` releases the reserved range.

## Defer

Deferred statements run when leaving the current scope, including exits through
`return`, `break`, and `again`. Invalid deferred control flow is still rejected
by loop-control validation.
