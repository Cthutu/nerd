# Runtime Model

This document records language-visible runtime behaviour derived from semantic
analysis and LLVM lowering. Source anchors include `src/compiler/sema/sema.c`,
`src/compiler/llvm/llvm.c`, `src/compiler/hir/gen.c`, and runtime helper notes
in `docs/string-runtime.md`.

## Values And Storage

Bindings are immutable values. Variables allocate mutable storage. Local typed
variables without an initializer use a concrete canonical `core.Default`
implementation when one exists for the variable type; otherwise they are
zero-initialised. Typed variables initialised with `undefined` deliberately skip
both forms of initialisation and are protected by definite-assignment checks.

## Zero Initialisation

Zero initialisation follows the backend representation:

| Type family                | Zero value                                   |
| -------------------------- | -------------------------------------------- |
| Numeric                    | `0` or `0.0`                                 |
| `bool`                     | `no`                                         |
| Pointer/function           | nil pointer                                  |
| Slice/string/dynamic array | zeroed pointer/count/capacity representation |
| Tuple/plex/union/enum      | zeroed aggregate representation              |

## Nil

`nil` materialises only when a destination type admits a nil value, currently
pointers, function values, slices, and dynamic arrays. Nil slices and dynamic
arrays compare against `nil` by their runtime representation.

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

`arena` is an opaque built-in type. The runtime reserves one 4 GiB virtual
address range for each arena and commits pages inside that range as needed, so
element addresses returned by arena allocation remain stable while the arena
grows. Arena cursors and marks are 32-bit offsets within the reserved range.

`arena(num_bytes)` and `arena(num_bytes, increment)` are built-in construction
syntax, not calls to a source-level function. The first form creates an arena
with at least that initial committed capacity, rounded up to the platform page
size. The second form also sets the page-rounded growth increment used when
more pages must be committed. Requests that would move the arena cursor beyond
the reserved range terminate at runtime.

`arena.alloc[T]()` allocates one `T` aligned for `T`;
`arena.alloc_array[T](count)` allocates contiguous storage for `count` values.
Arena values are opaque handles. Copying an `arena` value copies the handle to
the same underlying allocation state; it does not duplicate the reserved range
or committed pages.
`mark()` returns the current cursor. `restore(mark)` invalidates allocations
made after that mark. `reset()` invalidates all allocations from the arena
without releasing its reserved address range. `done()` releases the reserved
range.

## Tracked Memory

The executable runtime exposes `nrt_mem_*` helpers for standard-library heap
allocation wrappers. These helpers reserve a small header before each user
allocation, record the source file and line supplied by the caller, and maintain
process-wide heap statistics. Live, non-leaked allocations can be counted,
totalled, and printed for leak diagnostics. Marking an allocation as leaked
removes it from leak reports while preserving the header so a later free remains
valid.

## Defer

Deferred statements run when leaving the current scope, including exits through
`return`, `break`, and `again`. Invalid deferred control flow is still rejected
by loop-control validation.
