# Part 8: Compound Data

[Manual Index](README.md) | Previous: [Loops](part07-loops.md) | Next: [Dynamic Arrays And Manual Memory](part09-dynamic-arrays-and-manual-memory.md)

Compound data types let values carry structure: positions, names, fields,
variants, and views into memory.

## Tuples

Tuples group values by position:

```nerd
main :: fn () -> i32 {
    pair := (20, 22)
    return pair.0 + pair.1
}
```

Use tuples for small, positional groupings. Use plexes when field names matter.

## Fixed Arrays

Fixed arrays carry their length in the type:

```nerd
main :: fn () -> i32 {
    values := [1, 2, 3]
    return values[0]
}
```

An explicit fixed array type is written `[N]T`:

```nerd
values: [3]i32 = [1, 2, 3]
```

## Slices

A slice is a view into contiguous storage. Slice types are written `[]T`.

```nerd
use std.io

main :: fn () {
    values := [10, 20, 30]
    view := values[..]
    prn($"{view.count}")
}
```

Slices have `.data` and `.count`. They do not own storage.

## Pointer-To-Slice Casts

When you have a pointer and a count, build a slice view with:

```nerd
view := pointer.as([]u8, count)
```

The cast requires the element type and count. The resulting slice borrows the
memory behind the pointer.

## Strings And String Slices

`string` is distinct from `[]u8`, but it uses the same data/count shape.

```nerd
main :: fn () -> string {
    text := "north"
    return text[0..2]
}
```

String slices produce strings.

## Pointers

Pointer types are written `^T`.

```nerd
main :: fn () -> i32 {
    value := 41
    ptr := ^value
    ptr^ += 1
    return value
}
```

Use prefix `^` to take an address and postfix `^` to dereference.

## Nil

Pointers, slices, and dynamic arrays can be `nil` where the type supports it:

```nerd
main :: fn () -> i32 {
    ptr: ^i32 = nil
    on ptr == nil => return 0
    return ptr^
}
```

Check nil before dereferencing.

## Plexes

A plex is a named-field product type:

```nerd
Point :: plex {
    x i32
    y i32
}

main :: fn () -> i32 {
    p := Point { x: 20, y: 22 }
    return p.x + p.y
}
```

Fields are read with dot syntax and can be assigned when the value is mutable:

```nerd
p.x = 10
p.y += 5
```

## Destructuring

Destructuring binds parts of compound values:

```nerd
main :: fn () -> i32 {
    (left, right) := (20, 22)
    return left + right
}
```

Use `_` to ignore a value:

```nerd
(first, _) := (1, 2)
```

## Raw Unions

Raw unions overlap storage:

```nerd
Value :: union {
    i i32
    f f32
}
```

Raw unions are low-level. The program must know which field is valid.

## Enums

Enums are tagged values:

```nerd
Maybe :: enum {
    None
    Some(i32)
}

value_or_zero :: fn (value: Maybe) -> i32 {
    return on value {
        Some(as x) => x
        None => 0
    }
}
```

Use enums when a value has one active case and the program should track that
case safely.
