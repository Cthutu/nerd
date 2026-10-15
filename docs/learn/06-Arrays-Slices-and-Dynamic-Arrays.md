# Chapter 6: Arrays, Slices and Dynamic Arrays

[Previous: Plexes and Enums](05-Plexes-and-Enums.md)

Arrays store sequences of elements of one type. Slices borrow a view of existing
elements. Dynamic arrays own storage that can grow. Choosing between them means
choosing both how the length is managed and who owns the storage.

| Type | Role |
| --- | --- |
| `[N]T` | An array of `N` elements of type `T`. |
| `[]T` | A borrowed view carrying a pointer and an element count. |
| `[..]T` | A dynamic array with owned, growable storage. |

The programs below return their results as integer exit statuses unless they
explicitly print text.

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
Copying one copies its elements:

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

## Counts and storage sizes

Collection members distinguish element counts from byte sizes:

| Member | Fixed array | Slice | Dynamic array |
| --- | --- | --- | --- |
| `.count` | Number of elements. | Number of viewed elements. | Number of live elements. |
| `.bytes` | Byte size of all elements. | Byte size of viewed elements. | Obtain a slice first. |
| `.size` | Size of the array itself. | Size of the view itself. | Size of the array handle itself. |
| `.capacity` | Not applicable. | Not applicable. | Number of elements that fit before growth is needed. |
| `.data` | Pointer to the first element. | Borrowed pointer to the first element. | Pointer to the current element storage. |

Counts, capacities, and byte sizes have type `usize`. For `[3]i32`, `.count`
is `3` and `.bytes` is `3 * i32.size`. Its `.size` equals `.bytes` because
the elements are the array's storage. A slice's `.size` describes the view,
not the storage it borrows.

Use `.count` for indexing and iteration. A byte size is not an element count.
An empty collection has no valid element index, and indexing at `.count` is
always outside the live elements.

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

Nerd allocates this runtime-sized local array's storage on the heap and releases
it automatically when its scope exits. It has no resize or `.free()` operation.
Borrowed slices must remain within its lifetime. Runtime lengths are supported
for direct local declarations, while ordinary fixed-array fields and function
signatures require constant lengths. Use a slice parameter for varying lengths.

## Dynamic arrays and cleanup

Use `[..]T` when the number of live elements needs to change:

```nerd
main :: fn () -> i32 {
    values: [..]i32
    defer values.free()

    values.push(20)
    values.push(22)
    return values[0] + values[1]
}
```

This returns `42`. A default dynamic array starts empty. `.push(value)` appends
an element, growing storage as necessary. Indexing accesses live elements, just
as with a fixed array.

The dynamic array owns its allocated storage and requires cleanup. `.free()`
releases it and resets the array to `nil`. `defer` schedules that call for the
end of the current scope, including when a `return` exits the scope. Here, the
return value is computed before the deferred cleanup runs. Keeping `defer`
beside the declaration makes the cleanup responsibility visible.

You can request initial capacity in a declaration such as `values: [4..]i32`.
Capacity provides room for elements; it does not make those elements live.
The array still starts with `.count` equal to zero.

## Capacity and count

Reserving storage and adding elements are different operations:

```nerd
main :: fn () -> i32 {
    values: [..]i32
    defer values.free()

    values.reserve_to(8)
    values.resize_to(3)
    values[1] = 42
    return values[0] + values[1] + values[2]
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
    defer values.free()

    values.append(source[..])
    last := values.pop()
    return values[0] + values[1] + last
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
    defer values.free()

    values.append(source[..])
    values.delete(1)
    values.swap_delete(0)
    return values[0] + values[1]
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
    defer values.free()
    values.push(20)
    values.push(0)

    view := values[..]
    view[1] = 22
    return values[0] + values[1]
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
    defer values.free()
    return values[0] + values[1]
}
```

This returns `42`. The function deliberately does not defer `values.free()`:
the caller needs the storage after the function returns. Explicitly discarding
an owning result is not a substitute for freeing its storage.

## Iterating over elements

Use `for item in collection` to visit array, slice, or dynamic-array elements.
For these collections, the item binding is a pointer to the element. Postfix
`^` dereferences it, serving the role of prefix `*` in C:

```nerd
main :: fn () -> i32 {
    values := [10, 20, 12]
    total := 0
    for item in values {
        total += item^
    }
    return total
}
```

This returns `42`. Each iteration reads the next element through `item^`.
Writing `item^ = expression` updates that element. The same iteration form works
with a slice or dynamic array. Keep the collection's storage stable during
iteration; avoid operations that grow, free, or rearrange it while traversing
its elements.
