# Chapter 8: Arenas

[Previous: Pointers](07-Pointers.md)

An arena manages storage for a group of values that can be released together.
Allocate from it as needed, then reclaim a group of allocations at once.
This suits data that shares a lifetime, such as the working data for one task
or request. An arena's allocations keep their addresses as its storage grows.

The `arena` type and the operations used here are available through the
implicitly imported `core` module. No explicit import is needed. As in earlier
chapters, integer results are program exit statuses; printed output is shown
separately.

## Creating an arena and allocating a value

`arena(4096)` creates an arena with at least 4096 bytes of initial capacity.
To allocate one `i32`, write `storage.alloc[i32]()`.

The square brackets supply a **type argument**: they choose the type of value
to allocate. The parentheses supply ordinary value arguments, if any.
`alloc[i32]()` returns `^i32`; `alloc[Point]()` would return `^Point`.
Here `Point` would be a plex type defined by the program.

```nerd
main :: fn () -> i32 {
    storage := arena(4096)
    value := storage.alloc[i32]()
    value^ = 42

    result := value^
    storage.done()
    return result
}
```

This returns `42`. `storage` has type `arena`, and `value` points into its
storage. `.done()` releases the arena's storage, so the example saves its
integer result before that call. It does not free `value` separately.

Arena allocation provides memory without initializing its contents. Write a
value before reading it. This differs from the default initialization of the
array declarations in chapter 6. Reusing arena storage does not clear its old
contents either.

For a plex, assign a complete value through the returned pointer:

```nerd
Point :: plex { x i32 y i32 }

main :: fn () -> i32 {
    storage := arena(4096)
    point := storage.alloc[Point]()
    point^ = Point { x: 20, y: 22 }

    result := point.x + point.y
    storage.done()
    return result
}
```

This returns `42`. The allocation uses `Point` as its type argument and
returns `^Point`. The field access follows the pointer rules from chapter 7.

## Allocating arrays and bytes

`alloc_array[T](count)` allocates room for `count` elements of type `T` and
returns a slice, `[]T`:

```nerd
main :: fn () -> i32 {
    storage := arena(4096)
    values := storage.alloc_array[i32](2)
    values[0] = 20
    values[1] = 22

    result := values[0] + values[1]
    storage.done()
    return result
}
```

This returns `42`. Both elements are written before being read. The slice
borrows the arena's storage; it does not own a separate allocation and has no
`.free()` responsibility. Its count is fixed by this allocation, and it has
no dynamic-array growth operations.

Use `alloc_bytes(count)` when the amount is specified in bytes:

```nerd
main :: fn () -> i32 {
    storage := arena(4096)
    bytes := storage.alloc_bytes(2)
    bytes[0] = 20
    bytes[1] = 22

    result := bytes[0].as(i32) + bytes[1].as(i32)
    storage.done()
    return result
}
```

This returns `42`. `bytes` has type `[]u8`; its elements also require
initialization before use.

| Method | Result | Count means |
| --- | --- | --- |
| `.alloc[T]()` | `^T` | One value of `T`. |
| `.alloc_array[T](count)` | `[]T` | Number of elements. |
| `.alloc_bytes(count)` | `[]u8` | Number of bytes. |

## Capacity, growth and default construction

Initial capacity is not a limit on the total amount you can allocate.
`arena(initial_bytes, increment)` also chooses a growth increment, in bytes.
Both sizes are rounded up to the operating system's page size. With one
argument, the growth increment defaults to the rounded initial capacity.

```nerd
main :: fn () -> i32 {
    storage := arena(4096, 4096)
    first := storage.alloc[i32]()
    first^ = 20

    bytes := storage.alloc_bytes(9000)
    bytes[8999] = 22

    result := first^ + bytes[8999].as(i32)
    storage.done()
    return result
}
```

This returns `42`. Allocating more storage leaves `first` valid, including
when the arena needs to grow. This differs from dynamic-array growth, which
can move the array's elements.

More generally, any valid pointer into a live arena allocation continues to
refer to the same storage as more allocations are made. This includes pointers
to fields and array elements within an allocation. Arena-allocated values can
therefore hold pointers to one another without growth invalidating those
pointers, provided the allocations they reference remain live.

The current runtime reserves a 4 GiB virtual address range per arena and
commits memory as needed. Reserving this range does not immediately consume
4 GiB of physical memory. Requests that exceed the supported range fail at
runtime. Use the arena's methods rather than relying on its internal fields.

An explicitly typed declaration can also leave construction until the first
allocation:

```nerd
main :: fn () -> i32 {
    storage: arena
    value := storage.alloc[i32]()
    value^ = 42

    result := value^
    storage.done()
    return result
}
```

This returns `42`. `storage: arena` starts without allocated storage; its
first allocation initializes it with runtime defaults. It still needs
`.done()` once it has acquired storage. `arena()` with no arguments is not
the default-construction syntax.

## Marks and restoring allocations

`.mark()` returns the current allocation position as a `u32`.
Save it before a group of allocations, then call `.restore(mark)` when that
group is no longer needed:

```nerd
main :: fn () -> i32 {
    storage := arena(4096)
    kept := storage.alloc[i32]()
    kept^ = 20

    mark := storage.mark()
    temporary := storage.alloc[i32]()
    temporary^ = 100
    storage.restore(mark)

    replacement := storage.alloc[i32]()
    replacement^ = 22
    result := kept^ + replacement^
    storage.done()
    return result
}
```

This returns `42`. `kept` was allocated before the mark and remains valid
after the restore. The allocation referred to by `temporary` is invalidated,
and the program stops using that pointer. Later allocations may reuse the
reclaimed space.

A mark is an allocation position, not a copy of the stored values. Restoring
does not undo writes to allocations before the mark. Use a mark from the
same arena, and do not restore forward beyond its current allocation
position. Discard saved marks when their arena is reset or released.

## Resetting and releasing storage

`.reset()` invalidates all allocations from the arena and makes its storage
available for reuse:

```nerd
main :: fn () -> i32 {
    storage := arena(4096)
    first := storage.alloc[i32]()
    first^ = 20
    saved := first^
    storage.reset()

    second := storage.alloc[i32]()
    second^ = 22
    result := saved + second^
    storage.done()
    return result
}
```

This returns `42`. The program stops using `first` before resetting and
initializes the new allocation before reading it.

| Operation | Effect on allocations | Storage afterward |
| --- | --- | --- |
| `.restore(mark)` | Invalidates allocations after the mark. | Retained for reuse. |
| `.reset()` | Invalidates all allocations. | Retained for reuse. |
| `.done()` | Invalidates all allocations. | Released. |

Pointers into an allocation remain valid until that allocation is invalidated.
This applies equally to a pointer to the whole allocation and a pointer to a
field or element inside it. `.reset()` and `.done()` invalidate all such pointers;
`.restore(mark)` invalidates only pointers into allocations made after the mark.
Pointers into allocations made before the mark remain valid after a restore.

An owned arena does not automatically release its storage when its local
variable goes out of scope. Call `.done()` after the last use of its values.
Reassigning an arena variable is not a substitute for releasing its storage.
Separately owned resources stored inside arena values still need their own
cleanup; these operations do not automatically call their `.free()` methods.

## Sharing an arena with a function

A function can allocate from an arena supplied by its caller. A pointer to
the arena makes that relationship explicit:

```nerd
make_answer :: fn (storage: ^arena) -> ^i32 {
    value := storage.alloc[i32]()
    value^ = 42
    return value
}

main :: fn () -> i32 {
    storage := arena(4096)
    value := make_answer(^storage)
    result := value^
    storage.done()
    return result
}
```

This returns `42`. Returning `value` is valid because its storage belongs to
the caller's arena. It does not belong to a local integer inside
`make_answer`. The caller decides when that storage can be released.

Keep one arena variable responsible for cleanup. Use a pointer to that variable
when several functions need to share its allocation position and storage.
Passing `^arena` also lets a function initialize a caller's default-constructed
arena when its first allocation is made.

Arena values cannot be copied. For example, `copy := storage` is a compile-time
error, as is passing `storage` to a function that takes an `arena` by value.
This prevents two arena values from allocating overlapping memory or releasing
the same storage. The restriction also applies to aggregates containing arenas.
Use `^storage` to share the existing arena through a pointer.

## Storing strings in an arena

The arena's `.pr(text)` method returns a string backed by that arena.
It copies the bytes when they are not already in the arena:

```nerd
main :: fn () {
    storage := arena(4096)
    text := storage.pr("Hello, arena!")
    prn(text)
    storage.done()
}
```

Output:

```text
Hello, arena!
```

`storage.pr(...)` produces a string; it does not print to the console.
The ordinary `prn(text)` call prints that string. `storage.prn(text)` also
returns an arena-backed string, but appends a newline to its contents.
Its returned string must be used or explicitly discarded, as with other
function results.

These strings borrow their bytes from the arena. Copying a `string` value
copies its view, not its bytes, and does not extend the arena allocation's
lifetime.

## The built-in temporary arena

`temp_arena` is a pointer to the calling thread's temporary arena, provided by
the runtime. It supports the same allocation, mark, restore, and reset methods:

```nerd
main :: fn () -> i32 {
    value := temp_arena.alloc[i32]()
    value^ = 42
    result := value^
    temp_arena.reset()
    return result
}
```

This returns `42`. The program borrows this arena; the runtime owns and releases
it. Use `.reset()` to reclaim its allocations when none of them are needed.
Do not call `.done()` on the runtime-owned temporary arena. The function
`current_temp_arena()` also returns a pointer to the calling thread's
temporary arena. Do not share that pointer across threads.

Temporary-arena allocations can outlive the function that creates them. They
remain valid until an arena operation invalidates them, rather than being
released automatically at function return. A reset affects allocations made
by other functions too. A helper that borrows the arena should not reset it
while its caller may still need earlier allocations. A saved mark and restore
can reclaim just the helper's later allocations instead.

To preserve temporary data, copy it into storage whose lifetime you control:

```nerd
make_text :: fn () -> string {
    return temp_arena.pr("keep this text")
}

main :: fn () {
    mark := temp_arena.mark()
    temporary := make_text()
    storage := arena(4096)
    kept := storage.pr(temporary)
    temp_arena.restore(mark)

    prn(kept)
    storage.done()
}
```

Output:

```text
keep this text
```

`kept` borrows from `storage`, so restoring the temporary arena does not
invalidate it. `temporary` must no longer be used after the restore.

An arena allocation's lifetime follows its arena, whether it is accessed
through a pointer, slice, string, or another copied view. Keep that lifetime
separate from the scope of the names used to access it.
