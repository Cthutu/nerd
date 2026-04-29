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
| `.append(slice)` | add many items from a slice |
| `.reserve(capacity)` | ensure storage for at least this many items |
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
`break` and `continue`.

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

## Ownership Rule Of Thumb

- Dynamic arrays own storage.
- Slices borrow storage.
- `free()` releases owned storage.
- `defer` is the normal way to keep cleanup attached to a scope.
