# Chapter 5: Plexes, Unions, Enums and Tuples

[Previous: Variables and Scope](04-Variables-and-Scope.md)

[Next: Arrays, Slices and Dynamic Arrays](06-Arrays-Slices-and-Dynamic-Arrays.md)

Primitive types describe individual values. Plexes group named fields into a
single value, much like C structs. Unions provide named fields that overlap in
storage. Enums describe a choice between named variants, optionally carrying
data specific to each variant. Tuples group values by position rather than by
field name.

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

When the surrounding context supplies the plex type, you can omit the type
name from the literal. This works with an explicitly typed variable, a
function parameter, or a function's declared return type:

```nerd
Point :: plex { x i32 y i32 }

make_point :: fn () -> Point {
    return { x: 20, y: 22 }
}

sum :: fn (point: Point) => point.x + point.y

main :: fn () -> i32 {
    point: Point = { x: 20, y: 22 }
    other := make_point()
    return point.x + other.y + sum({ x: 0, y: 0 })
}
```

This returns `42`. Each unnamed literal gets its `Point` type from its
context. Writing `Point { ... }` explicitly is still valid in these places.

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

A plex value introduced with `::` does not allow direct field assignment.
For example, `original :: Point { x: 20, y: 0 }` makes `original.x += 2` a
compile-time error. This also applies to nested plex fields stored by value.
Use `:=` or an explicitly typed variable when you intend to modify a plex,
or initialise a mutable copy from the binding.

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

## Raw unions

A `union` defines named alternatives that share storage, much like a C union.
Field declarations use the same `name Type` syntax as plexes:

```nerd
Value :: union {
    integer i32
    real f32
}

main :: fn () -> i32 {
    value := Value { integer: 42 }
    return value.integer
}
```

This returns `42`. A union literal initialises exactly one field. Unlike a
plex, it cannot hold independent values in all its fields simultaneously.
Fields stored by value follow the same binding rule as plexes: use a variable
when you intend to assign a union field, rather than a value bound with `::`.

Assigning another field overwrites the shared storage:

```nerd
Value :: union {
    integer i32
    real f32
}

main :: fn () -> i32 {
    value := Value { integer: 42 }
    value.real = 3.5
    return value.real.as(i32)
}
```

This returns `3`. Writing `real` stores a floating-point value in the shared
storage; it does not numerically convert the previous integer value. After the
write, the example reads `real` rather than treating `integer` as a separately
preserved value.

A raw union has no tag recording which field the program should read. The
compiler does not verify that the field you read matches the field last
written. Use a union when overlapping storage is required and the program
manages that interpretation. Use an enum when the value should track its
active alternative.

## Enums without payloads

An enum defines a set of variants:

```nerd
Colour :: enum {
    Red
    Green
    Blue
}

main :: fn () -> i32 {
    colour := Colour.Green
    return (colour == Colour.Green).as(i32)
}
```

This returns `1`. `Colour.Green` constructs the variant named `Green`. Its
type is `Colour`. The comparison checks which variant the value holds, and
`.as(i32)` converts the boolean result to `1` for `yes` or `0` for `no`.

When the expected enum type is known, you can omit the type name and write
`Green` instead of `Colour.Green`:

```nerd
Colour :: enum { Red Green Blue }

get_colour :: fn () -> Colour {
    return Green
}

is_green :: fn (colour: Colour) => colour == Green

main :: fn () -> i32 {
    colour: Colour = Green
    return (is_green(Green) && colour == Green && get_colour() == Green).as(i32)
}
```

This returns `1`. The variable annotation, function return type, and function
parameter each supply `Colour` as the expected type. In comparisons such as
`colour == Green`, the left operand supplies the type for the right operand.

Use the qualified form when there is no enum type supplied by context:
`colour := Colour.Green` lets the initialiser determine the variable's type.
A bare `Green` does not select an enum just because one is defined nearby.

## Enums with payloads

A variant can carry data. The enum records which variant is active, so the
data and its interpretation stay together:

```nerd
Reading :: enum {
    Missing
    Value(i32)
}

main :: fn () -> i32 {
    reading := Reading.Value(42)
    return (reading == Reading.Value(42)).as(i32)
}
```

This returns `1`. `Reading.Value(42)` constructs a `Value` variant carrying an integer.
`Reading.Missing` carries no data. Equality compares both the variant and
its payload, when present.

In C, this commonly needs an enum tag, a union for payload storage, and rules
kept in sync by the programmer. Nerd's enum represents the variant and its
payload as one typed value. Accessing the active payload is covered in
[Chapter 9's enum patterns](09-Branching-and-Pattern-Matching.md#extracting-enum-payloads).

Variants can also carry named fields:

```nerd
Event :: enum {
    Closed
    Resized {
        width i32
        height i32
    }
}

main :: fn () -> i32 {
    event := Event.Resized { width: 20, height: 22 }
    return (event == Event.Resized { width: 20, height: 22 }).as(i32)
}
```

This returns `1`. The braced literal supplies the fields carried by the
`Resized` variant.

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
    status := Status.Ready
    number: i32 = Status.Ready
    return number + (status == Status.Ready).as(i32)
}
```

This returns `5`: `Ready` has the explicit discriminant `4`, and the comparison
contributes `1`. `status` is inferred as `Status`. Because `Ready = 4` explicitly
defines an integer discriminant, `Status.Ready` can also supply that integer
where a numeric type is required, without a cast. Integer contexts check that
the value fits their type, just as for an integer literal.

Explicit discriminants must be compile-time integer values, and variant names
and discriminants must be unique within the enum. `Running` receives the next
discriminant, `5`, but its implicit numbering does not enable this contextual
numeric conversion. Payload constructors also remain enum values. To obtain
the discriminant of an enum variable, use an explicit cast such as
`status.as(i32)`.

Plexes and enums containing the primitive values used in this chapter support
`==` and `!=`. Plex equality compares fields; enum equality compares the active
variant and its payload. Two `Reading.Value(42)` values compare equal, while
`Reading.Missing` and `Reading.Value(42)` do not.

Raw unions do not have built-in value equality: their shared bytes do not
identify which field should be compared.

## Tuples

A tuple groups a fixed number of values. Its fields can have different types,
and their positions identify them:

```nerd
main :: fn () -> i32 {
    result := (42, "answer")
    prn(result.1)
    return result.0
}
```

This prints `answer` and returns `42`. Tuple literals use parentheses and
commas. Field positions start at zero: `.0` accesses the first field and `.1`
the second. Here `result` has type `(i32, string)`.

A tuple type lists its field types in the same order. This lets a function
return several values as one result:

```nerd
get_pair :: fn () -> (i32, i32) {
    return (20, 22)
}

main :: fn () -> i32 {
    (left, right) := get_pair()
    return left + right
}
```

This returns `42`. The declaration `(left, right) := ...` destructures the
tuple into two variables. Each variable receives the corresponding field.
An explicit tuple type works on a variable too:
`pair: (i32, string) = (42, "answer")`.

Assignment copies a tuple value. You can change individual fields of a tuple
variable or replace its whole value:

```nerd
main :: fn () -> i32 {
    original := (20, 22)
    copy := original
    copy.0 = 10
    return original.0 + copy.0 + copy.1
}
```

This returns `52`. Changing `copy` leaves `original` unchanged. Compound
assignment, such as `copy.0 += 1`, also works. Nested tuple fields follow the
same rule: the storage containing them must be mutable. Fields of a tuple
bound with `::` or passed as a by-value parameter cannot be assigned to.

Use `_` in a destructuring declaration to discard a field you do not need:

```nerd
main :: fn () -> i32 {
    (answer, _) := (42, "unused")
    return answer
}
```

This returns `42`. The string field is explicitly discarded.

A one-element tuple needs a trailing comma in both its value and its type:

```nerd
main :: fn () -> i32 {
    single := (42,)
    typed: (i32,) = (42,)
    return single.0 + typed.0 - 42
}
```

This returns `42`. Both variables have type `(i32,)`, and `.0` accesses their
only field. Without the comma, `(42)` is just a parenthesised integer
expression, and `(i32)` is not a one-element tuple type.

Tuples can also contain other tuples or the compound values introduced in
this chapter.

Use a tuple when positions make the meaning clear, such as a small function
result. Use a plex when field names make the value easier to understand.

## More destructuring forms

Destructuring works with constant bindings, explicitly typed variables, and
assignment to existing variables as well as inferred variable declarations:

```nerd
main :: fn () -> i32 {
    (base, extra) :: (20, 1)
    (left, right): (i32, i32) = (base, extra)
    (left, right) = (right, left)
    return left + right + 21
}
```

This returns `42`. The right-hand side is evaluated before assignment changes
the destinations, so a tuple assignment can swap values. Constant destructuring
uses `::`; typed constant destructuring uses a colon before the source value,
for example `(left, right): (i32, i32) : (20, 22)`.

A plex can be destructured by field name:

```nerd
Point :: plex { x i32 y i32 }

main :: fn () -> i32 {
    point := Point { x: 20, y: 22 }
    { x, y } := point
    return x + y
}
```

This returns `42`. To give the extracted values different names, write
`{ x: left, y: right } := point` and use `left` and `right`. Tuple and plex
destructuring patterns can also nest. The source expression is evaluated once.

Tuple positions and plex field names must match the source shape. `_` discards
an unwanted tuple position. Destructuring a value does not make the new names
aliases for its fields; ordinary value and ownership rules apply to extracted
values.

## Choosing between the four forms

| Form | Storage and interpretation | Access |
| --- | --- | --- |
| `plex` | Holds all its fields in distinct storage. | Read fields by name. |
| `union` | Fields overlap; the program tracks which interpretation is valid. | Read the appropriate field by name. |
| `enum` | Tracks one active variant and its associated payload. | Extract payloads with the [patterns in Chapter 9](09-Branching-and-Pattern-Matching.md#extracting-enum-payloads). |
| tuple | Holds all its fields in distinct storage, identified by position. | Read fields with `.0`, `.1`, and so on. |

## Layout and bit fields

Ordinary plex fields are reordered to reduce padding and make the value smaller.
The compiler places fields in descending alignment order; fields with equal
alignment keep their declaration order. Field names and the order in which
literal expressions are evaluated are unaffected. Embedded plexes and bit-field
storage blocks stay intact when reordered.

Write `plex #c { ... }` when you need C-compatible field layout in declaration
order. `plex #packed { ... }` also preserves declaration order, with packed
C-compatible storage. For example, `plex { a u8 b u32 c u8 }` occupies 8 bytes,
while the same fields in a `plex #c` occupy 12 bytes.

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
