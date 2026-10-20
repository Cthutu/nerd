# Chapter 7: Pointers

[Previous: Arrays, Slices and Dynamic Arrays](06-Arrays-Slices-and-Dynamic-Arrays.md)

[Next: Arenas](08-Arenas.md)

A pointer holds the address of a value. As in C, copying a pointer copies the
address, and accessing the pointed-to value requires valid storage. Nerd uses
`^` for pointer types, taking addresses, and dereferencing; its position tells
you which operation is intended.

The programs below return integer exit statuses so you can check their results
with `nerd run`.

## Pointer types, addresses and dereferencing

Write `^T` for a pointer to a value of type `T`. Prefix `^` takes an address;
postfix `^` accesses the value at that address:

```nerd
main :: fn () -> i32 {
    value := 41
    pointer: ^i32 = ^value
    pointer^ += 1
    return value
}
```

This returns `42`. `pointer` refers to the storage of `value`, so writing
through `pointer^` changes `value` itself. You could also write
`pointer := ^value` and let the compiler infer `^i32`.

| Nerd | Meaning | C equivalent |
| --- | --- | --- |
| `^i32` | Pointer type. | `int32_t *` |
| `^value` | Address of a value. | `&value` |
| `pointer^` | Value at a pointer. | `*pointer` |

Taking the address of a variable, array element, or plex field borrows its
existing storage. When the operand is an intermediate expression or a named
constant, the compiler instead creates anonymous mutable stack storage for
its value. The sections below explain that temporary storage's lifetime.

The `^` character is reserved for pointer operations. Infix bitwise XOR uses
`~`, as in `flags ~ mask`.

## Copying and reassigning pointers

Assigning a pointer variable changes which address it holds. Assigning through
the pointer changes the pointed-to value:

```nerd
main :: fn () -> i32 {
    left := 0
    right := 22
    pointer := ^left
    alias := pointer
    pointer = ^right
    alias^ = 20
    return alias^ + pointer^
}
```

This returns `42`. `alias` still points to `left` after `pointer` is redirected
to `right`. Copying the address did not copy either integer or link the two
pointer variables so that they must change together.

The pointed-to value can itself be a pointer. `^^i32` is a pointer to a
`^i32`, and two postfix dereferences reach the integer:

```nerd
main :: fn () -> i32 {
    answer := 42
    pointer := ^answer
    outer: ^^i32 = ^pointer
    return (outer^)^
}
```

This returns `42`. The first dereference, `outer^`, accesses the pointer
variable; the second accesses `answer`.

## Pointer parameters

Use a pointer parameter when a function needs to modify the caller's value:

```nerd
increment :: fn (value: ^i32) {
    value^ += 1
}

main :: fn () -> i32 {
    answer := 41
    increment(^answer)
    return answer
}
```

This returns `42`. The address is passed by value. The parameter `value`
cannot be reassigned inside `increment`, but the integer it points to can be
changed through `value^`.

## Null pointers

For pointers, `nil` means no address. A pointer variable declared without an
initialiser defaults to `nil`:

```nerd
main :: fn () -> i32 {
    pointer: ^i32
    return (pointer == nil).as(i32)
}
```

This returns `1`. You can also initialise it explicitly with
`pointer: ^i32 = nil`. Use `== nil` or `!= nil` to test a pointer. A non-nil
address alone does not establish that its storage is still valid or has the
right type. Do not dereference a nil pointer. Branching to handle a missing
pointer will be covered in a later chapter.

## Pointers to plexes and tuples

Dot access automatically dereferences pointers to plexes and tuples:

```nerd
Point :: plex { x i32 y i32 }

main :: fn () -> i32 {
    point := Point { x: 20, y: 0 }
    pointer := ^point
    pointer.y = 22
    return pointer.x + pointer.y
}
```

This returns `42`. `pointer.x` accesses the same field as `pointer^.x`.
For a pointer to a tuple, `pointer.0` similarly accesses its first field.

The pointer receiver mentioned in chapter 5 uses the same rule:

```nerd
Point :: plex { x i32 y i32 }

impl Point {
    add_x :: fn (self: ^Point, amount: i32) {
        self.x += amount
    }
}

main :: fn () -> i32 {
    point := Point { x: 18, y: 22 }
    pointer := ^point
    pointer.add_x(2)
    return point.x + point.y
}
```

This returns `42`. The receiver supplies the address of `point`, allowing
`add_x` to update its `x` field.

## Pointers into arrays

Taking the address of an array element produces a pointer to that element's
type. Pointer indexing then accesses elements relative to that address:

```nerd
main :: fn () -> i32 {
    values := [10, 20, 12]
    first := ^values[0]
    first[1] = 30
    return first[1] + first[2]
}
```

This returns `42`. `first` has type `^i32`, and `first[0]` refers to
`values[0]`. The collection's `.data` member also supplies a pointer to its
first element. This does not imply that an empty collection has an element
you can access.

A raw pointer carries no element count. Pointer indexing does not perform
bounds checks; the program must keep each access within valid storage.

Taking the address of the whole array gives a different type:

```nerd
main :: fn () -> i32 {
    values: [3]i32 = [10, 20, 12]
    array_pointer: ^[3]i32 = ^values
    return array_pointer^[0] + array_pointer^[1] + array_pointer^[2]
}
```

This returns `42`. `array_pointer^` is the whole `[3]i32` array, and the
following `[index]` selects an element. `array_pointer^[1]` means
`(array_pointer^)[1]`. In contrast, `array_pointer[0]` accesses the first whole
array at that address; it does not select its first integer.

## Addresses of literals and intermediate values

You can take the address of a plex or array literal directly:

```nerd
Point :: plex { x i32 y i32 }

main :: fn () -> i32 {
    point := ^Point { x: 20, y: 22 }
    values := ^[20, 22]
    return point.x + values^[1]
}
```

This returns `42`. `point` has type `^Point`, and `values` has type `^[2]i32`.
The compiler places these temporary values in anonymous local storage; they
do not allocate heap storage. Their addresses must stay within the lifetime
of the enclosing scope.

The pointed-to type can also come from context. For example,
`point: ^Point = ^{ x: 20, y: 22 }` supplies `Point` as the literal's type.

The same rule applies to scalar literals, computed expressions, returned
values, and tuple literals:

```nerd
make_answer :: fn () -> i32 {
    return 42
}

main :: fn () -> i32 {
    literal := ^42
    computed := ^(20 + 22)
    returned := ^make_answer()
    pair := ^(20, 22)
    return (literal^ + computed^ + returned^ + pair.0 + pair.1) / 4
}
```

This returns `42`. Each operand is evaluated once and stored in an anonymous
mutable stack variable. Its storage lasts until the enclosing scope exits,
rather than disappearing at the end of the expression. No heap allocation
is needed, and the pointer must not escape that scope.

An expected pointer type supplies context for literals: `wide: ^i64 = ^42`
stores an `i64`. Without that context, ordinary runtime defaults apply, so
`literal` above has type `^i32`.

## Taking the address of a named constant

Taking a constant's address creates a mutable copy, rather than exposing
writable storage for the constant:

```nerd
answer :: 42

main :: fn () -> i32 {
    copy := ^answer
    copy^ = 0
    return answer + copy^
}
```

This returns `42`. Changing `copy^` does not change `answer`. This differs from
`^variable`, which points at the variable's existing storage. Normal copy
restrictions still apply when creating a temporary from a constant.

## Pointer arithmetic and comparison

For a sized pointee type, adding or subtracting an integer moves by elements,
as in C:

```nerd
main :: fn () -> i32 {
    values := [10, 20, 12]
    first := ^values[0]
    third := first + 2
    second := third - 1
    distance := third - first
    return second^ + third^ + distance.as(i32) + 8
}
```

This returns `42`. Adding `2` to `^i32` advances by two integers, rather than
two bytes. Subtracting compatible pointers gives an `isize` distance measured
in elements. Use subtraction for pointers into the same allocation.

Compatible pointers support `==` and `!=` to compare addresses. The ordering
operators `<`, `<=`, `>`, and `>=` compare addresses too; their ordering is
useful when the pointers refer into the same allocation. Comparing addresses
does not compare the contents of the pointed-to values.

## Casting to and from void pointers

`^void` holds an address without specifying a pointee type, like C's `void *`.
A typed pointer converts implicitly to `^void` when that type is expected.
Converting back to a typed pointer requires an explicit `.as(^T)` cast:

```nerd
main :: fn () -> i32 {
    answer := 42
    pointer := ^answer
    address: ^void = pointer
    restored := address.as(^i32)
    return restored^
}
```

This returns `42`. Assigning `pointer` to `address` implicitly converts `^i32`
to `^void`; `.as(^void)` is not required. You can also write
`address := pointer.as(^void)` when you want to choose that type explicitly
without an annotation.

Both conversions preserve the address. They do not allocate or copy the
pointed-to value, convert its contents, or extend its lifetime. Here the
original storage holds an `i32`, so restoring `^i32` is appropriate.

A typed pointer can also be supplied directly where `^void` is expected:

```nerd
read_number :: fn (address: ^void) -> i32 {
    pointer := address.as(^i32)
    return pointer^
}

main :: fn () -> i32 {
    answer := 42
    return read_number(^answer)
}
```

This returns `42`. The parameter accepts `^answer` without an explicit cast
to `^void`. Converting back requires `.as(^i32)`; writing
`pointer: ^i32 = address` would be a compile-time error.

The contract of `read_number` requires an address of a live, correctly aligned
`i32`. `^void` does not record or verify that information. A cast between
pointer types tells the compiler how you intend to access the existing memory;
it does not make incorrectly typed or misaligned storage suitable for that
access.

You cannot use a `^void` pointer to read a value or perform element arithmetic:
`void` has no element size. Cast to the appropriate pointer type first.

Explicit `.as(^T)` casts can also change one typed pointer into another.
The same storage, alignment, and lifetime requirements apply. Integer addresses
require an explicit cast too, such as `0x1000.as(^void)`. Such a cast only
forms an address; it does not allocate or establish valid storage there.

## Building a slice from a pointer

When a pointer and an element count are available separately, the two-argument
form of `.as` builds a slice:

```nerd
main :: fn () -> i32 {
    values := [20, 22, 100]
    pointer := ^values[0]
    view := pointer.as([]i32, 2)
    return view[0] + view[1]
}
```

This returns `42`. The count `2` is a count of `i32` elements, not bytes.
The cast creates a borrowed view; it does not allocate or copy elements.
The pointer must provide suitably aligned storage for the element type and
enough live elements for the supplied count. The cast does not establish that
the count fits the original allocation. The slice must stay within the
lifetime of that storage.

## Storage lifetime and ownership

Taking the address of existing storage borrows it; addressing an intermediate
value creates anonymous stack storage. Neither transfers ownership or creates
a heap allocation.
Do not call `.free()` merely because you obtained a pointer to a local value
or an array element. The storage still belongs to the original value or
allocation.

A pointer to a local variable becomes invalid when that variable's storage
ceases to exist. Returning its address does not keep it alive. Pointers into
dynamic arrays follow the rules from chapter 6: freeing the array invalidates
them, and growth can move the elements and invalidate existing pointers.

Copying or casting a pointer does not change these rules. Nerd does not track
raw pointer lifetimes for you; ensure that the pointed-to storage remains
valid whenever you access it.
