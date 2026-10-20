# Chapter 6: Arrays, Slices and Dynamic Arrays

[Previous: Plexes, Unions, Enums and Tuples](05-Plexes-Unions-Enums-and-Tuples.md)

[Next: Pointers](07-Pointers.md)

Arrays store sequences of elements of one type. Slices borrow a view of existing
elements. Dynamic arrays own storage that can grow. Choosing between them means
choosing both how the length is managed and who owns the storage.

| Type | Role |
| --- | --- |
| `[N]T` | An array of `N` elements of type `T`. |
| `[]T` | A slice: a borrowed view carrying a pointer and an element count. |
| `[..]T` | A dynamic array with owned, growable storage. |

The programs below return their results as integer exit statuses unless they
explicitly print text.

## Storage and ownership

The collection type determines where its elements live and who releases their
storage:

| Form | Element storage | Cleanup responsibility |
| --- | --- | --- |
| Constant-length `[N]T` | Inline, as part of the array value. Local variables normally use stack storage; array fields live inside their containing value. | No separate allocation to free. |
| Local `[count]T` with a runtime length | Stack storage, with its length fixed at declaration. | Storage is reclaimed automatically when the declaring scope exits. |
| `[]T` | Existing storage borrowed from another value or allocation. | The owner releases the storage; the slice must stay within its lifetime. |
| `[..]T` | A separate heap allocation that can grow. | The program calls `.free()` after its last use of the storage. |

A local dynamic-array variable is a handle to its allocation. Letting that
variable go out of scope does not free the allocation automatically. Returning
the handle can pass cleanup responsibility to the caller; copying it does not
create independent storage or another allocation to free.

These rules describe the collection's own storage. If an element holds another
dynamic array or another separately owned resource, copying the outer collection
does not duplicate that resource, and releasing the outer storage does not
release it. Manage each owned resource separately.

## Fixed arrays

Square brackets construct an array literal:

```nerd
main :: fn () -> i32 {
    values := [10, 20, 12]
    return values[0] + values[1] + values[2]
}
```

This returns `42`. The inferred type is `[3]i32`: three elements, each an
`i32`. Array indexing starts at zero, as in C. The last valid index is one less
than the element count.

Write the length before the element type to declare it explicitly:

```nerd
main :: fn () -> i32 {
    values: [3]i32 = [10, 20, 0]
    values[2] = 12
    return values[0] + values[1] + values[2]
}
```

This also returns `42`. Individual elements of a mutable array can change,
but its length stays fixed. `[3]i32` and `[4]i32` are different types.
`values: [3]i32` without an initialiser starts with three zeroed integers.

Ordinary fixed arrays contain their elements directly and need no `.free()`.
Copying a fixed array copies its elements:

```nerd
main :: fn () -> i32 {
    original := [20, 22]
    copy := original
    copy[0] = 1
    return original[0] + copy[0]
}
```

This returns `21`, because changing `copy` leaves `original` unchanged.

Arrays can contain other arrays, or the plexes and enums from chapter 5:

```nerd
main :: fn () -> i32 {
    matrix: [2][2]i32 = [[20, 22], [1, 2]]
    return matrix[0][0] + matrix[0][1]
}
```

This returns `42`. The first index selects a row; the second selects an element
of that row.

## Creating a slice

Use `[]T` for a slice type. Slicing an array creates a view of its existing
storage:

```nerd
main :: fn () -> i32 {
    values := [10, 20, 12]
    view := values[1..3]
    view[0] = 30
    return values[1] + view[1]
}
```

This returns `42`. `values[1..3]` selects original indices `1` and `2`.
The slice's own indices begin at zero, so `view[0]` refers to `values[1]`.
Changing it updates the original array; making the slice did not copy elements.

The end of a slice is exclusive. You can omit either bound:

| Expression | Meaning |
| --- | --- |
| `values[..]` | View the entire sequence. |
| `values[start..]` | View from `start` to the end. |
| `values[..end]` | View from the beginning up to `end`. |
| `values[start..end]` | View the selected interval. |

Slice bounds must describe a valid interval within the source. Slicing uses
the exclusive `..` spelling, not `..=`.

## Counts and storage sizes

Collection members distinguish element counts from byte sizes:

| Member | Fixed array | Slice | Dynamic array |
| --- | --- | --- | --- |
| `.count` | Number of elements. | Number of viewed elements. | Number of live elements. |
| `.bytes` | Byte size of all elements. | Byte size of viewed elements. | Obtain a slice first. |
| `.size` | Bytes occupied by the array's elements. | Bytes occupied by the slice value: its pointer and count. | Bytes occupied by the dynamic-array handle, excluding its heap allocation. |
| `.capacity` | Not applicable. | Not applicable. | Number of elements that fit before growth is needed. |
| `.data` | Pointer to the first element. | Borrowed pointer to the first element. | Pointer to the current element storage. |

Counts, capacities, and byte sizes have type `usize`. As introduced in
[Chapter 3](03-Primitive-Types-and-Expressions.md), `.size` works on all
concrete types and values, including collections. On a type, `T.size`
gives the byte size of a value of that type; for example, `i32.size` is `4`.
For `[3]i32`, `.count` is `3` and `.bytes` is `3 * i32.size`. Its `.size` equals
`.bytes` because the elements are the array's storage.

A slice value contains a pointer and a `usize` element count. Its `.size`
measures those two fields, including any layout padding. It does not depend
on how many elements the slice views or on their type. On the current 64-bit
target, a slice occupies `16` bytes: an 8-byte pointer and an 8-byte count.
Its `.bytes` instead measures the viewed elements: `.count * T.size` for a
slice of `T`.

For example, given `values: [3]i32 = [10, 20, 12]`:

| Expression | `.count` | `.bytes` | `.size` on the current 64-bit target |
| --- | --- | --- | --- |
| `values` | `3` | `12` | `12` |
| `values[..]` | `3` | `12` | `16` |
| `values[..1]` | `1` | `4` | `16` |

A dynamic-array handle occupies one pointer on the current implementation.
Its `.size` excludes both the heap-allocated bookkeeping and the element
storage, so growing the array does not change the handle's size. Use
`values[..].bytes` for its live element bytes, or
`values.capacity * T.size` for its element capacity in bytes. Neither includes
allocation bookkeeping.

Use `.count` for indexing and iteration. A byte size is not an element count.
An empty collection has no valid element index, and indexing at `.count` is
always outside the live elements.

## Passing slices to functions

In C, a function often receives a pointer and a separate length. A Nerd slice
carries both in one value:

```nerd
first_two_sum :: fn (values: []i32) -> i32 {
    return values[0] + values[1]
}

main :: fn () -> i32 {
    values := [20, 22, 100]
    return first_two_sum(values[..2])
}
```

This returns `42`. The function's contract here requires at least two elements.
The `[]i32` type carries a runtime count, so the function can also inspect
`values.count` when it needs to handle varying lengths.

A fixed array can be passed directly where a matching slice is expected.
For example, `first_two_sum(values)` borrows the whole array without copying it.
Slice parameters can also modify the elements they view:

```nerd
set_first :: fn (values: []i32) {
    values[0] = 42
}

main :: fn () -> i32 {
    values := [0, 1]
    set_first(values)
    return values[0]
}
```

This returns `42`. Although a function parameter cannot itself be reassigned,
a slice parameter does not make its underlying elements read-only.

## Slice lifetime and equality

A slice borrows storage. Copying a slice copies the view, so both views refer
to the same elements. A slice does not free that storage or extend its lifetime.
It must not outlive the array or other allocation it views. In particular,
returning a slice of a local fixed array does not make that local array survive
the function call. Nerd does not track borrowed lifetimes for you.

For the primitive elements used here, fixed arrays of the same type and slices
support content equality:

```nerd
main :: fn () -> i32 {
    left := [20, 22]
    right := [20, 22]
    return (left[..] == right[..]).as(i32)
}
```

This returns `1` because the slices have equal counts and equal elements.
The elements occupy different storage; equality compares contents.

## Strings as counted views

Slicing a `string` produces another `string`, borrowing the selected bytes:

```nerd
main :: fn () {
    text := "north"
    prn(text[..2])
}
```

Output:

```text
no
```

Indices and counts refer to bytes. With UTF-8 text, an arbitrary byte boundary
may fall within a character. Choose boundaries appropriately for the text.

Byte arrays whose element type is `u8` or `i8` can be viewed as strings with
`.as(string)`:

```nerd
main :: fn () {
    bytes: [2]u8 = ['h', 'i']
    prn(bytes.as(string))
}
```

Output:

```text
hi
```

The cast borrows existing bytes. It does not copy them, remove a trailing zero
byte, or validate their encoding. Slice first when only part of a buffer
contains meaningful text.

## Fixed lengths chosen at runtime

A local array declaration can use a runtime integer length:

```nerd
main :: fn () -> i32 {
    count := 3
    values: [count]i32
    count = 1
    return values.count.as(i32) + count
}
```

This returns `4`. The length is evaluated once when the array is declared,
so changing `count` later does not change the array's three-element length.
The elements start zeroed.

Nerd allocates this runtime-sized local array's elements on the stack, just as
it does for constant-length local arrays. Storage is reclaimed automatically
when the declaring scope exits, including when the function returns. It has no
resize or `.free()` operation. The storage must fit the available thread stack;
there is no fallback to heap allocation.

Borrowed slices must remain within the array's lifetime. Returning a slice does
not keep that stack storage alive, and returning the runtime-sized array itself
is rejected by the compiler. Runtime lengths are supported for direct local
declarations, while ordinary fixed-array fields and function signatures require
constant lengths. Use a slice parameter for varying lengths.

## Dynamic arrays and cleanup

Use `[..]T` when the number of live elements needs to change:

```nerd
main :: fn () -> i32 {
    values: [..]i32

    values.push(20)
    values.push(22)
    result := values[0] + values[1]
    values.free()
    return result
}
```

This returns `42`. A default dynamic array starts empty. `.push(value)` appends
an element, growing storage as necessary. Indexing accesses live elements, just
as with a fixed array.

The default `values: [..]i32` starts as `nil`. Here, `nil` is an empty handle
with no allocation: both `.count` and `.capacity` are zero. Adding elements
or reserving capacity allocates heap storage as needed. That storage survives
the declaring scope until it is explicitly freed.

The dynamic array owns its allocated storage and requires cleanup. `.free()`
releases it and resets the array to `nil`. The example saves the calculated
result before freeing the storage, then returns that result. Accessing the
array's elements after `.free()` would be invalid. Make sure every path that
finishes using an owned allocation releases it or passes cleanup responsibility
to another function.

You can request initial capacity in a declaration such as `values: [4..]i32`.
Capacity provides room for elements; it does not make those elements live.
The array still starts with `.count` equal to zero. This capacity uses heap
storage too, and requires the same `.free()` cleanup.

## Capacity and count

Reserving storage and adding elements are different operations:

```nerd
main :: fn () -> i32 {
    values: [..]i32

    values.reserve_to(8)
    values.resize_to(3)
    values[1] = 42
    result := values[0] + values[1] + values[2]
    values.free()
    return result
}
```

This returns `42`. Reserving capacity leaves the count unchanged. Resizing to
three creates three live integers, initially zeroed.

| Operation | Effect |
| --- | --- |
| `.reserve_to(n)` | Ensure capacity for at least `n` elements without changing count. |
| `.reserve_extra(n)` | Ensure room for `n` additional elements beyond the current count. |
| `.resize_to(n)` | Set the count to `n`, default-initialising new elements. |
| `.extend(n)` | Add `n` default-initialised elements to the current count. |
| `.resize_undefined_to(n)` | Set count without initialising newly added elements. |
| `.extend_undefined(n)` | Add `n` elements without initialising them. |
| `.clear()` | Set count to zero while keeping storage for reuse. |

Write newly added uninitialised elements before reading them. Do not assume the
compiler can track initialisation of individual dynamic-array elements in the
same way it tracks an `undefined` local variable.

## Appending and removing elements

`.append` copies elements from a slice. To append another dynamic array's
elements, pass a slice such as `other[..]`. `.pop()` removes and returns the
last element:

```nerd
main :: fn () -> i32 {
    source := [10, 20, 12]
    values: [..]i32

    values.append(source[..])
    last := values.pop()
    result := values[0] + values[1] + last
    values.free()
    return result
}
```

This returns `42`. After the pop, `values.count` is `2`. The returned element
must be used or explicitly discarded, as with other function results. Popping
an empty dynamic array is a runtime error.

There are two indexed removal operations:

```nerd
main :: fn () -> i32 {
    source := [10, 20, 30, 40]
    values: [..]i32

    values.append(source[..])
    values.delete(1)
    values.swap_delete(0)
    result := values[0] + values[1]
    values.free()
    return result
}
```

This returns `70`. `.delete(1)` removes `20` and shifts the later elements,
leaving `[10, 30, 40]`. `.swap_delete(0)` moves the last element into the removed
slot, leaving `[40, 30]`. Use `.delete` when order matters and `.swap_delete`
when moving the last element is acceptable. Both require a valid live index.

## Borrowing dynamic-array storage

Take a slice to borrow a dynamic array's current elements:

```nerd
main :: fn () -> i32 {
    values: [..]i32
    values.push(20)
    values.push(0)

    view := values[..]
    view[1] = 22
    result := values[0] + values[1]
    values.free()
    return result
}
```

This returns `42`. The view does not own the storage, so do not free it.
Its count is a snapshot; it does not follow later changes to `values.count`.
Growing an array can move its element storage and invalidate existing views.
Obtain a fresh slice after growth, and never use a view after its owner is freed.
Use `values[..].bytes` when you need the byte size of the live elements.

Do not apply the fixed-array copying rule to dynamic arrays: a simple assignment
does not duplicate the element allocation. To obtain independent storage, create
another dynamic array and `.append` the source elements into it. Keep one clear
cleanup responsibility for each owned allocation. Freeing array storage also
does not automatically clean up resources owned separately by its elements.

## Returning owned storage

A function can return a dynamic array, leaving cleanup to its caller:

```nerd
make_values :: fn () -> [..]i32 {
    values: [..]i32
    values.push(20)
    values.push(22)
    return values
}

main :: fn () -> i32 {
    values := make_values()
    result := values[0] + values[1]
    values.free()
    return result
}
```

This returns `42`. `make_values` returns the owned storage without freeing it,
so its caller can still use the elements. The caller saves its result and then
calls `.free()`. Explicitly discarding an owning result is not a substitute for
freeing its storage.
