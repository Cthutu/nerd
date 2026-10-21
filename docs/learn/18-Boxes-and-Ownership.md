# Chapter 18: Boxes and Ownership

[Previous: Defines](17-Defines.md)

[Next: Assertions and Testing](19-Assertions-and-Testing.md)

A box owns a heap allocation. Use one when a value must survive the function
that creates it, and one owner should be responsible for releasing its storage.
Unlike a slice or a raw pointer, a box carries ownership. Unlike an arena, it
releases one allocation independently of other allocations.

## Allocating a value

`box[T]()` allocates one default-initialised value of type `T`:

```nerd
Point :: plex { x i32 y i32 }

main :: fn () -> i32 {
    point := box[Point]()
    point.x = 20
    point.y = 22
    return point.x + point.y
}
```

This returns `42`. Field access goes through the box to the allocated `Point`.
The local box releases its allocation when the function exits. There is no
matching `defer point.free()` to add for ordinary local ownership.

A typed box variable without an allocation is nil. A box can be tested with
`on owner` or `on !owner`; the latter means it has no allocation. Do not access
its pointee while it is nil.

## Ownership moves

Assigning a box to another box transfers the allocation and sets the source
to nil:

```nerd
main :: fn () -> i32 {
    first := box[i32]()
    first.data^ = 42
    second := first
    on first => return 1
    return second.data^
}
```

This returns `42`. There is one owner after the assignment. `second` releases
the allocation at scope exit; `first` has nothing left to release. Moving is
different from copying the allocated value. To make an independent copy,
allocate another box and copy the payload into it.

Box-typed parameters and return values also move ownership:

```nerd
make_answer :: fn () -> box[i32] {
    answer := box[i32]()
    answer.data^ = 42
    return answer
}

take_answer :: fn (answer: box[i32]) -> i32 {
    return answer.data^
}

main :: fn () -> i32 {
    answer := make_answer()
    result := take_answer(answer)
    on answer => return 1
    return result
}
```

This returns `42`. Returning the box moves the allocation to the caller.
Passing it to `take_answer` moves it again, and that function's parameter
releases it when the function returns.

## Borrowing without moving

Passing a box to a pointer parameter borrows its pointee. The allocation stays
with its owner:

```nerd
set_answer :: fn (value: ^i32) {
    value^ = 42
}

main :: fn () -> i32 {
    answer := box[i32]()
    set_answer(answer)
    return answer.data^
}
```

This returns `42`. `.data` exposes the same borrowed `^T` pointer explicitly.
Neither a pointer nor a slice extends the allocation's lifetime. Do not retain
one after the owner is freed or leaves scope. Nerd does not track these
borrowed lifetimes for you.

## Allocating several elements

`box[T](count)` allocates a contiguous sequence. `.count` gives its element
count; `.data` points to the first element. A box implicitly converts to a
slice when the expected type is `[]T`:

```nerd
main :: fn () -> i32 {
    numbers := box[i32](3)
    view: []i32 = numbers
    view[0] = 10
    view[1] = 12
    view[2] = 20
    return view[0] + view[1] + view[2]
}
```

This returns `42`. The slice borrows the box's allocation without moving
ownership. The same conversion works when passing a box to a `[]T` parameter.
The element type must match exactly. A nil box produces an empty slice with a
null data pointer. You can still construct the view explicitly with
`numbers.data.as([]i32, numbers.count)`.

Borrowing does not extend the allocation's lifetime. Moving the box to another
owner leaves existing views valid until that owner releases the allocation;
returning a view into a local box leaves it dangling after automatic cleanup.
A box has no `push` operation and does not grow; use a dynamic array when the
live element count needs to change.

## Releasing storage early

`owner.free()` releases its allocation and resets the owner to nil:

```nerd
main :: fn () -> i32 {
    answer := box[i32]()
    answer.data^ = 42
    result := answer.data^
    answer.free()
    on answer => return 1
    return result
}
```

This returns `42`. Early release is useful when the owning scope continues for
a long time. Existing pointers and slices into that allocation become invalid.
Local and parameter boxes receive automatic scope cleanup. Do not assume that
this recursively frees owning fields inside their payloads: resources stored
inside a boxed value remain the program's responsibility. Globals likewise
need an explicit ownership and release policy.

## Adopting a pointer

`box[T](ptr)` adopts an existing `^T` allocation. The pointer must come from a
runtime-heap-compatible allocation with the allocation metadata a box expects.
This is an ownership transfer: the previous owner must stop freeing it.

Do not adopt an address of a local, fixed array, literal, arena allocation,
interior element, or an arbitrary foreign allocation. A C API's allocation
must normally be released with that API's matching release function. Adoption
is for storage whose allocator contract explicitly supports Nerd boxes.

## Equality and choosing an owner

Boxes compare contents when their element type supports equality. Two allocated
boxes compare equal when their counts and corresponding elements match; two
nil boxes compare equal. This comparison borrows the operands and does not
move ownership. Compare borrowed pointers when allocation identity is what
you need instead.

| Storage form | Use it for |
| --- | --- |
| `box[T]` | One owned allocation that moves between scopes or functions. |
| `[..]T` | Owned storage with a changing live count and capacity. |
| `arena` | Many allocations released together by reset, restore, or free. |
| `^T`, `[]T` | Borrowing storage whose lifetime is managed elsewhere. |
