# Part 8: Compound Data

[Manual Index](README.md) | Previous: [Loops](part07-loops.md) | Next: [Dynamic Arrays And Manual Memory](part09-dynamic-arrays-and-manual-memory.md)

Compound data types let values carry structure: positions, names, fields,
variants, and views into memory.

This part introduces several forms. The table gives the shape before the details:

| Form | Use |
| --- | --- |
| tuple | fixed positional group |
| fixed array | fixed-length indexed group |
| slice | borrowed view into contiguous storage |
| pointer | address of another value |
| plex | named-field value |
| union | overlapping low-level storage |
| enum | one active case from a set |

Compound type declarations can appear at the top level or inside a block. A
local declaration is scoped like other local bindings:

```nerd
main :: fn () -> i32 {
    Point :: plex { x i32 y i32 }
    point: Point = Point { x: 20, y: 22 }
    return point.x + point.y
}
```

## Tuples

Tuples group values by position:

```nerd
main :: fn () -> i32 {
    pair := (20, 22)       -- tuple with two values
    return pair.0 + pair.1 -- read tuple fields by position
}
```

Use tuples for small, positional groupings. Use plexes when field names matter.

## Fixed Arrays

Fixed arrays carry their length in the type. Indexing uses square brackets:

```nerd
main :: fn () -> i32 {
    values := [1, 2, 3]  -- fixed array literal
    return values[0]     -- indexes start at zero
}
```

An explicit fixed array type is written `[N]T`:

```nerd
values: [3]i32 = [1, 2, 3]  -- array length is part of the type
```

## Slices

A slice is a view into contiguous storage. Slice types are written `[]T`.
Slicing with `[..]` creates a view over the whole array or slice:

```nerd
use std.io

main :: fn () {
    values := [10, 20, 30]
    view := values[..]       -- slice over the whole array
    prn($"{view.count}")     -- slices carry a count
}
```

Slices have `.data` and `.count`. They do not own storage.

## Pointer-To-Slice Casts

When you have a pointer and a count, build a slice view with:

```nerd
view := pointer.as([]u8, count)  -- pointer plus count becomes a slice
```

The cast requires the element type and count. The resulting slice borrows the
memory behind the pointer.

Untyped integer constants can be written as raw pointer address constants when a
pointer type is explicit:

```nerd
null: ^u8 = 0
device := 0x1000.as(^void)
```

Use this for FFI and platform APIs that deal in addresses. Ordinary concrete
integer values are not pointer-compatible.

## Strings And String Slices

`string` is distinct from `[]u8`, but it uses the same data/count shape.

```nerd
main :: fn () -> string {
    text := "north"
    return text[0..2]  -- string slice from index 0 up to 2
}
```

String slices produce strings.

## Pointers

Pointer types are written `^T`.

```nerd
main :: fn () -> i32 {
    value := 41
    ptr := ^value  -- take the address of value
    ptr^ += 1      -- assign through the pointer
    return value
}
```

Use prefix `^` to take an address and postfix `^` to dereference.

## Nil

Pointers, slices, and dynamic arrays can be `nil` where the type supports it:

```nerd
main :: fn () -> i32 {
    ptr: ^i32 = nil
    on ptr == nil => return 0  -- check before dereferencing
    return ptr^
}
```

Check nil before dereferencing.

## Plexes

A plex is a named-field product type:

```nerd
Point :: plex {
    x i32  -- named field
    y i32
}

main :: fn () -> i32 {
    p := Point { x: 20, y: 22 }  -- construct by field name
    return p.x + p.y             -- read fields with dot syntax
}
```

Fields are read with dot syntax and can be assigned when the value is mutable.
These are partial snippets, not complete programs:

```nerd
p.x = 10  -- assign a field
p.y += 5  -- compound-assign a field
```

Plexes may refer to themselves through pointers. This is useful for linked
structures and object graphs because the field has pointer size:

```nerd
Node :: plex {
    value i32
    next  ^Node
}
```

Top-level pointer aliases can point into a top-level collection and be used by
that collection's initializer. The alias is treated as an address constant, not
as an ordinary value dependency:

```nerd
nodes: []Node = [
    { value: 1, next: second },
    { value: 2, next: first },
]

first  :: ^nodes[0]
second :: ^nodes[1]
```

A plex may not contain itself directly by value, because that would require an
infinite amount of storage:

```nerd
BadNode :: plex {
    next BadNode  -- invalid
}
```

## Generic Compound Types

Plexes, raw unions, and enums can take type parameters. The parameters are
written after the type form:

```nerd
Box :: plex [T] {
    value T  -- T is supplied when Box is used
}

IntBox :: Box[i32]  -- concrete alias for Box with T as i32
```

Generic type arguments are written in square brackets. All type parameters must
be supplied:

```nerd
Pair :: plex [A, B] {
    first  A
    second B
}

pair: Pair[i32, string]  -- A is i32, B is string
```

Enums can be generic too:

```nerd
Maybe :: enum [T] {
    None
    Some(T)  -- payload type is chosen by Maybe[T]
}
```

Generic types are templates. A use such as `Box[i32]` creates a concrete type
that can be stored, passed, and returned like any other type.

## Inherent Methods

An `impl Type { ... }` block groups functions with a compound type. These are
inherent methods: they belong to the type directly, not to a trait.

The first parameter is the receiver. It is usually called `self` by convention:

```nerd
Counter :: plex {
    value i32
}

impl Counter {
    inc :: fn (self: ^Counter, amount: i32) {  -- mutating receiver
        self.value += amount
    }

    get :: fn (self: Counter) -> i32 {  -- value receiver
        return self.value
    }
}
```

Call a method with dot syntax. The value before the dot is passed as the first
parameter:

```nerd
main :: fn () -> i32 {
    counter: Counter
    counter.inc(7)        -- same receiver role as ^counter
    return counter.get()  -- returns the current value
}
```

Use a pointer receiver, such as `^Counter`, when the method mutates the value.
The call site still writes `counter.inc(7)`; the compiler passes the receiver
as a pointer when the receiver parameter requires it.

Generic compound types can have generic impl blocks:

```nerd
Stack :: plex [T] {
    data [..]T
}

impl Stack[T] {
    push :: fn (self: ^Stack[T], item: T) {
        self.data.push(item)
    }
}
```

The impl generic parameters are also in scope inside method bodies, so type
operations such as `T.size` work there.

An impl block may also contain associated functions. These are called through
the type rather than through a value. Associated functions are intended for
constructors and factories, so they return `Self` or `^Self`:

```nerd
impl Stack[T] {
    init :: fn () -> Self {
        return { data: nil }
    }
}

main :: fn () {
    stack := Stack[i32].init()
    stack.data.free()
}
```

Methods can be public inside a module:

```nerd
impl Stack[T] {
    pub push :: fn (self: ^Stack[T], item: T) {
        self.data.push(item)
    }
}
```

Public methods are available when another module imports the type.

## Destructuring

Destructuring binds parts of compound values:

```nerd
main :: fn () -> i32 {
    (left, right) := (20, 22)  -- bind tuple parts by position
    return left + right
}
```

Use `_` to ignore a value:

```nerd
(first, _) := (1, 2)  -- ignore the second value
```

## Raw Unions

Raw unions overlap storage:

```nerd
Value :: union {
    i i32  -- shares storage with f
    f f32
}
```

Raw unions are low-level. The program must know which field is valid.

## Enums

Enums are tagged values:

```nerd
Maybe :: enum {
    None,      -- case with no payload
    Some(i32)  -- case carrying an i32
}

value_or_zero :: fn (value: Maybe) -> i32 {
    return on value {
        Some(as x) => x  -- bind the payload
        None => 0
    }
}
```

Enum variants may be separated with commas. A trailing comma is allowed. Variant
names must be unique within one enum. Explicit `= <value>` discriminants must be
unique after implicit values are filled in.

Use enums when a value has one active case and the program should track that
case safely.
