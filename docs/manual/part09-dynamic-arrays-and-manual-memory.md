# Part 9: Dynamic Arrays And Manual Memory

[Manual Index](README.md) | Previous: [Compound Data](part08-compound-data.md) | Next: [Modules](part10-modules.md)

Dynamic arrays are growable arrays. They own their storage and can change size
while the program runs, which makes them useful when the number of items is not
known at compile time.

## Types

An initially empty dynamic array type is written:

```nerd
[..]string  -- growable array of string values
```

A dynamic array with inline capacity is written:

```nerd
[4..]string  -- starts with room for four strings inline
```

The inline capacity gives the array room before it needs heap allocation.
The capacity expression may also be a runtime integer in a local declaration:

```nerd
make :: fn (initial_capacity: usize) {
    values: [initial_capacity..]i32
    values.free()
}
```

## Basic Operations

```nerd
use std.io

main :: fn () {
    names: [..]string
    defer names.free()  -- release owned storage on scope exit

    names.push("north")  -- add one item
    names.push("south")

    prn($"{names.count}")  -- number of live items
}
```

Common operations:

| Operation | Meaning |
| --- | --- |
| `.push(value)` | add one item |
| `.pop()` | remove and return the last item |
| `.delete(index)` | remove one item and preserve order |
| `.swap_delete(index)` | remove one item by replacing it with the last item |
| `.append(slice)` | add many items from a slice |
| `.reserve_to(capacity)` | ensure storage for at least this absolute capacity |
| `.reserve_extra(additional)` | ensure room for this many more live items |
| `.resize_to(count)` | set the count and default-initialise new items |
| `.resize_undefined_to(count)` | set the count without initialising new items |
| `.extend(count)` | add this many default-initialised items |
| `.extend_undefined(count)` | add this many uninitialised items |
| `.clear()` | set the count to zero but keep storage |
| `.free()` | release owned storage |

Dynamic arrays expose:

| Field | Meaning |
| --- | --- |
| `.count` | number of live items |
| `.capacity` | number of items that fit before growing |

## Slicing

A dynamic array can be viewed as a slice:

```nerd
view := names[..]  -- borrow the dynamic array as a slice
```

The slice borrows the array storage. It does not own the contents.

## Clearing And Freeing

`.clear()` resets the count but keeps capacity:

```nerd
names.clear()  -- remove items but keep allocated storage
```

`.pop()` removes the last item and returns it:

```nerd
last := names.pop()  -- count decreases by one
```

Popping an empty dynamic array is a runtime error.

`.delete(index)` removes the item at `index`, shifts later items left, and
preserves order:

```nerd
names.delete(1)
```

`.swap_delete(index)` removes the item at `index` by moving the last item into
that slot. It does not preserve order, but it avoids shifting the rest of the
array:

```nerd
names.swap_delete(1)
```

Deleting with an out-of-bounds index is a runtime error.

`.free()` releases owned storage and resets the array to nil:

```nerd
names.free()  -- release owned storage
```

Call `.free()` when a dynamic array owns heap storage and you are done with it.

## Cleanup With `defer`

Prefer writing cleanup next to ownership:

```nerd
main :: fn () {
    parts: [..]string
    defer parts.free()  -- cleanup stays next to ownership

    parts.push("look")
    parts.push("north")
}
```

If the cleanup has multiple statements, use a deferred block:

```nerd
defer {
    parts.free()  -- deferred block can contain several cleanup steps
}
```

This is especially helpful in functions with several returns or loops with
`break` and `again`.

## Returning Dynamic Arrays

A function can return a dynamic array:

```nerd
make_words :: fn () -> [..]string {
    words: [..]string
    words.push("look")
    words.push("north")
    return words  -- ownership moves to the caller
}
```

The caller owns the returned array and should free it:

```nerd
main :: fn () {
    words := make_words()
    defer words.free()  -- caller releases the returned array
}
```

## Arenas

The implicitly imported `core` module provides a pointer-stable arena API for
allocations that can be released together:

```nerd
main :: fn () {
    scratch: arena
    scratch = arena(4096, 4096)
    defer scratch.done()

    value := scratch.alloc[i32]()
    value^ = 42

    bytes := scratch.alloc_array[u8](128)
    bytes[0] = 1

    mark := scratch.mark()
    _more := scratch.alloc_bytes(256)
    scratch.restore(mark)

    scratch.reset()  -- invalidates earlier arena allocations
}
```

`arena(num_bytes)` and `arena(num_bytes, increment)` are built-in construction
syntax, not calls to a source-level function. The first form creates an arena
with at least that many bytes of initial capacity. The second form also sets the
growth increment used when the arena runs out of room. Both sizes are rounded up
by the runtime to the platform page size. The runtime reserves one stable
address range for each arena and commits pages on demand as it grows. Previously
returned element addresses do not move while the arena grows.

An arena can grow up to its reserved 4 GiB address range. Arena marks and
offsets are 32-bit values, and allocation fails at runtime if a request would
move the cursor beyond that range.

Use `arena.alloc[T]()` for one value and `arena.alloc_array[T](count)` for a
slice. The arena representation is opaque; use methods rather than fields.
Arena values are handle-like: assigning or passing an `arena` value copies the
handle to the same underlying allocation state, not the owned storage.

`mark()` returns the current arena cursor as a `u32`. `restore(mark)` moves the
cursor back to a previous mark, invalidating allocations made after that mark
while leaving earlier allocations valid.

`reset()` makes previous arena pointers and slices invalid but keeps the arena
ready for reuse. `done()` releases the arena's owned storage. Call `done()` for
arenas that own runtime memory.

Runtime interpolated strings use a separate thread-local temporary arena. They
can be returned from functions and stored as ordinary `string` values until that
temporary arena is reset:

```nerd
label :: fn (value: i32) -> string {
    return $"value={value}"
}

main :: fn () {
    for {
        text := label(42)
        -- use text during this frame or request

        temp_arena.reset()
    }
}
```

Programs without a frame or request loop do not need to reset the temporary
arena. Programs with a loop should call `temp_arena.reset()` at a clear boundary
after temporary strings from the previous iteration are no longer needed.

## Ownership Rule Of Thumb

- Dynamic arrays own storage.
- Slices borrow storage.
- `free()` releases owned storage.
- Arena allocations are valid until the arena is restored before them, reset, or
  released.
- Runtime interpolated strings are valid until `temp_arena.reset()`.
- `defer` is the normal way to keep cleanup attached to a scope.
