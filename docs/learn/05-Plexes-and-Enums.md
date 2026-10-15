# Chapter 5: Plexes and Enums

[Previous: Variables and Scope](04-Variables-and-Scope.md)

[Next: Arrays, Slices and Dynamic Arrays](06-Arrays-Slices-and-Dynamic-Arrays.md)

Primitive types describe individual values. Plexes group named fields into a
single value, much like C structs. Enums describe a choice between named
variants, optionally carrying data specific to each variant.

As in earlier chapters, programs returning an integer use it as an exit status.
Examples using `prn` show their text output separately.

## Defining and constructing a plex

The `plex` type expression defines the fields, and a binding gives the type a
name:

```nerd
Point :: plex {
    x i32
    y i32
}

main :: fn () -> i32 {
    point := Point { x: 20, y: 22 }
    return point.x + point.y
}
```

This returns `42`. Fields in a type declaration use `name Type`, without a
colon. Fields in a value literal use `name: expression`. Access uses the familiar
dot syntax, such as `point.x`.

`point` is inferred as a concrete `Point`. Its fields provide type context:
the untyped literals `20` and `22` become `i32` values. You can also declare
the variable explicitly as `point: Point = Point { x: 20, y: 22 }`.

## Default fields and shorthand

A plex literal normally supplies every field. End the literal with `...` to
initialise omitted fields with their defaults:

```nerd
Point :: plex { x i32 y i32 }

main :: fn () -> i32 {
    point := Point { x: 20, ... }
    return point.x + point.y
}
```

This returns `20`, because the omitted integer field `y` defaults to zero.
Simply leaving out `y` without `...` is an error. A typed variable declared
without an initialiser, such as `point: Point`, also starts with default values
for these integer fields.

When a local name matches a field name, a literal can use shorthand:

```nerd
Point :: plex { x i32 y i32 }

main :: fn () -> i32 {
    x := 20
    y := 22
    point := Point { x, y }
    return point.x + point.y
}
```

This returns `42`. `Point { x, y }` means `Point { x: x, y: y }`.

## Mutation and copying

Fields of a mutable plex variable can be assigned:

```nerd
Point :: plex { x i32 y i32 }

main :: fn () -> i32 {
    original := Point { x: 20, y: 0 }
    copy := original
    copy.x += 2
    return original.x + copy.x
}
```

This returns `42`: `original.x` remains `20`, while `copy.x` becomes `22`.
Assignment copies the plex value. For this type, whose fields are integers,
the two variables have independent field storage.

Use `with` to produce a modified copy without updating the original:

```nerd
Point :: plex { x i32 y i32 }

main :: fn () -> i32 {
    original := Point { x: 20, y: 22 }
    changed := original with { x: 10 }
    return original.x + changed.x + changed.y
}
```

This returns `52`. `changed` keeps the original `y` and replaces `x` with `10`.
The original remains unchanged. `with` can also use field shorthand.

## Nested plexes and embedding

A field can have another plex as its type:

```nerd
Point :: plex { x i32 y i32 }
Rect :: plex {
    origin Point
    width i32
    height i32
}

main :: fn () -> i32 {
    rect := Rect {
        origin: Point { x: 20, y: 22 }
        width: 80
        height: 25
    }
    return rect.origin.x + rect.origin.y
}
```

This returns `42`. The field `origin` provides a named level of nesting.

Alternatively, `use Point` embeds a `Point` and exposes its fields directly:

```nerd
Point :: plex { x i32 y i32 }
Rect :: plex {
    use Point
    width i32
    height i32
}

main :: fn () -> i32 {
    rect := Rect { x: 20, y: 22, width: 80, height: 25 }
    point: Point = rect
    point.x += 1
    return rect.x + point.x
}
```

This returns `41`. `rect.x` accesses the embedded field. Where a `Point` is
expected, Nerd can copy the embedded `Point` from the `Rect`; changing that
copy leaves the rectangle unchanged. Matching field names alone do not permit
this conversion: the `use Point` relationship is required. Field names must
not conflict with other fields exposed by the containing plex.

## Methods

An `impl Type` block groups functions with a type. The first parameter acts as
the receiver, conventionally named `self`:

```nerd
Point :: plex { x i32 y i32 }

impl Point {
    sum :: fn (self: Point) -> i32 {
        return self.x + self.y
    }
}

main :: fn () -> i32 {
    point := Point { x: 20, y: 22 }
    return point.sum()
}
```

This returns `42`. The receiver before the dot supplies the first argument.
Here `self: Point` receives a value, and its fields cannot be reassigned through
that parameter. Methods that modify the caller's plex need a pointer receiver;
pointer types and mutation through pointers are separate topics.

## Enums without payloads

An enum defines a set of variants:

```nerd
Colour :: enum {
    Red
    Green
    Blue
}

describe :: fn (colour: Colour) -> string {
    return on colour {
        Red => "red"
        Green => "green"
        Blue => "blue"
    }
}

main :: fn () {
    colour := Colour.Green
    prn(describe(colour))
}
```

Output:

```text
green
```

`Colour.Green` constructs the variant named `Green`. Its type is `Colour`.
Inside `on colour`, the subject supplies the enum type, so the branch patterns
can use the unqualified names `Red`, `Green`, and `Blue`.

This is a value-producing `on` expression. Each `pattern => expression` branch
supplies a result, and the enclosing `return` returns the selected string.
All variants are covered. A value-producing match must be exhaustive; you can
also use `else => expression` to handle the remaining cases. Branching and
patterns will be explored in more detail separately.

## Enums with payloads

A variant can carry data. The enum records which variant is active, so the
data and its interpretation stay together:

```nerd
Reading :: enum {
    Missing
    Value(i32)
}

value_or_zero :: fn (reading: Reading) -> i32 {
    return on reading {
        Missing => 0
        Value(number) => number
    }
}

main :: fn () -> i32 {
    reading := Reading.Value(42)
    return value_or_zero(reading)
}
```

This returns `42`. `Reading.Value(42)` constructs a `Value` variant carrying an
integer. The pattern `Value(number)` binds that payload to `number` within its
branch. `Reading.Missing` carries no data.

In C, this commonly needs an enum tag, a union for payload storage, and rules
kept in sync by the programmer. Nerd's enum represents the variant and its
payload as one typed value. Use matching to access the active payload.

Variants can also carry named fields:

```nerd
Event :: enum {
    Closed
    Resized {
        width i32
        height i32
    }
}

dimensions_sum :: fn (event: Event) -> i32 {
    return on event {
        Closed => 0
        Resized { width, height } => width + height
    }
}

main :: fn () -> i32 {
    event := Event.Resized { width: 20, height: 22 }
    return dimensions_sum(event)
}
```

This returns `42`. Construction and matching use the same braced shape.
The names `width` and `height` are available only within the matching branch.

## Discriminants and equality

Variants have integer discriminants. By default they start at zero and increase
by one. An explicit `= value` chooses a discriminant, and following implicit
values continue from it:

```nerd
Status :: enum {
    Unknown = 0
    Ready = 4
    Running
}

main :: fn () -> i32 {
    return Status.Running.as(i32)
}
```

This returns `5`. Explicit discriminants must be compile-time integer values,
and variant names and discriminants must be unique within the enum. Enum values
remain typed as their enum; `.as(i32)` explicitly obtains an integer here.

Plexes and enums containing the primitive values used in this chapter support
`==` and `!=`. Plex equality compares fields; enum equality compares the active
variant and its payload. Two `Reading.Value(42)` values compare equal, while
`Reading.Missing` and `Reading.Value(42)` do not.

## Layout and bit fields

Write `plex #c { ... }` when you need C-compatible field layout.
`plex #packed { ... }` requests packed C-compatible storage. Ordinary plex
syntax describes the fields; layout annotations make an external layout
requirement explicit.

A plex can pack named bit fields into an unsigned integer storage block:

```nerd
Kind :: enum { Request Response }
Header :: plex {
    u8 {
        kind Kind : 2
        length : 6
    }
}

main :: fn () -> i32 {
    header := Header { kind: Kind.Response, length: 42 }
    return header.length.as(i32)
}
```

This returns `42`. The two fields share one `u8`, using two and six bits
respectively. Fields are allocated from the least significant bit upwards.
The widths must be positive compile-time integers and fit the backing type.
`_ : N` reserves unnamed padding bits.

An unannotated bit field such as `length` uses the backing integer's type when
read. A field annotated with a payload-free enum, such as `kind`, uses that
enum as its logical type. Its discriminants must fit the allotted width.
Constant writes that do not fit are rejected; runtime writes retain the low
bits. Bit fields cannot have their address taken independently.
