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
`[]u8`, and from byte collections to `string`, when the cast is permitted by
semantic analysis. Byte collections are fixed arrays, slices, and dynamic
arrays whose element type is `i8` or `u8`. The conversion is a view cast: it
does not copy, trim null terminators, or validate UTF-8.

String slicing returns `string`; byte-slice slicing returns `[]u8`.

Slices are borrowing views. A slice value contains a pointer and a count, but it
does not own, free, or extend the lifetime of the storage it views. Slicing an
array, slicing another slice, converting a pointer and count with
`ptr.as([]T, count)`, or receiving a `[]T` parameter all preserve this borrowed
ownership model.

## Boxes

`box[T]` is an owning heap pointer with transparent allocation metadata.
`box[T]()` allocates one default-initialised `T`; `box[T](count)` allocates
contiguous storage for `count` values of `T`; `box[T](ptr)` adopts an existing
runtime-heap-compatible `^T` allocation.

The box value itself is pointer-shaped. The allocation header/runtime metadata
stores the byte size, and the language exposes `box.count` as the allocation
byte size divided by `T.size`. `box.data` exposes the owned pointer as `^T` for
borrowing views such as `box.data.as([]T, box.count)`.

For array-like values, `data` is the pointer to the payload, `count` is the
number of live elements, and `bytes` is the payload byte size. Fixed arrays,
slices, and strings expose `value.bytes`. Fixed arrays also expose `value.data`
as a pointer to element zero. For `[N]T`, `value.bytes` is the fixed array
storage size and matches `value.size`. For `[]T`, this is the live element count
multiplied by `T.size`. For `string`, this is the same value as `value.count`,
because strings are byte strings. Dynamic arrays do not expose `bytes`; code
that needs the byte size of live dynamic-array elements should take a slice view
first.

Box ownership moves when a `box[T]` is assigned to another `box[T]`, passed to a
parameter of type `box[T]`, or returned from a function. The source box is set
to nil by the move. Passing a box where `^T` is expected borrows the pointee and
does not transfer ownership.

`box.free()` releases the owned allocation and resets the box to nil. Local and
parameter boxes that still own an allocation are freed automatically when they
leave scope, but owned fields inside the boxed value remain the program's
responsibility.

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

## Runtime Memory Foundation

Language-generated allocations use the executable runtime's low-level memory
foundation rather than the C allocator. The foundation talks directly to the
operating system:

- Linux reserves and releases memory with `mmap`, `mprotect`, and `munmap`.
- Windows reserves and releases memory with `VirtualAlloc` and `VirtualFree`.

The runtime exposes heap routines for generated code and standard-library
wrappers:

- `nrt_mem_alloc(size, alignment, file, line)`
- `nrt_mem_realloc(memory, size, alignment, file, line)`
- `nrt_mem_free(memory)`
- `nrt_mem_size(memory)`
- `nrt_mem_leak(memory)`
- `nrt_mem_break_on_alloc(index)`
- `nrt_mem_live_head()`
- `nrt_arena_live_head()`

Every heap allocation stores a release header immediately before the returned
pointer. The final word before the returned pointer is the requested size, so
the runtime can expose allocation sizing even in release-oriented builds. Extra
debug metadata, such as linked-list pointers and allocation index, lives before
that release header when debug tracking is enabled.

Debug tracking maintains a linked list of live heap allocations. Marking an
allocation as leaked removes it from the live list; it does not require a
persistent leaked flag. This makes process-lifetime allocations invisible to
leak reporting while keeping later `nrt_mem_free` valid. Debug heap records
also store the source file and line supplied to `nrt_mem_alloc` or
`nrt_mem_realloc`.

`nrt_mem_break_on_alloc(index)` records a debug allocation index that should
emit a diagnostic when that allocation is reached. It uses the runtime's heap
allocation index, so standard-library wrappers and generated dynamic-array
growth observe the same sequence.

Dynamic arrays allocate their header and backing storage through `nrt_mem_*`.
The public `.free()` method releases both pieces of storage through the same
foundation.

Arenas remain runtime-owned language facilities. They reserve one stable virtual
address range and commit pages on demand through the same operating-system
memory layer. Arena allocations do not have per-allocation heap headers because
they are invalidated in bulk by `restore`, `reset`, or `done`. Debug tracking for
arena ownership is represented by separate runtime tracking nodes keyed by the
arena reservation base address rather than by hidden headers before arena
results. `nrt_arena_live_head()` exposes the debug list head to leak reporting
code outside the runtime. Arena debug nodes store the source file and line
supplied to `nrt_arena_init`.

`std.mem` is the public standard-library facade over this runtime foundation.
It owns the source-level API, statistics presentation, and leak-reporting
commands, while compiler-generated dynamic-array and arena operations use the
same low-level allocation substrate. The runtime exposes its debug allocation
list to `std.mem`; it does not format or print leak reports itself.

## Defer

Deferred statements run when leaving the current scope, including exits through
`return`, `break`, and `again`. Invalid deferred control flow is still rejected
by loop-control validation.
