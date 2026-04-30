# Historical Notes

This file is older design material. It is not the current source of truth for
project direction or language behaviour. Use [ROADMAP.md](ROADMAP.md) for active
work and [docs/manual/README.md](docs/manual/README.md) for source-level
language rules.

The roadmap deliberately keeps retiring this file as deferred work, so leave it
visible until its useful ideas have been moved into active roadmap items or
discarded.

# Compound data types

## Tuples

Tuples are ordered collections of values, where each value can be of a different
type. They are immutable, meaning that once a tuple is created, its contents
cannot be changed. Tuples are often used to group related data together.

A tuple's type is denoted by parentheses `(T1, T2, ..., Tn)`, where `T1`, `T2`,
..., `Tn` are the types of the elements in the tuple. For example, a tuple
containing an integer and a string would have the type `(i32, string)`.

A tuple literal is created by enclosing a comma-separated list of values in
parentheses. For example, `(42, "Hello")` is a tuple literal of type `(int,
string)`.  If a tuple has only one element, it must be followed by a comma to
distinguish it from a parenthesized expression. For example, `(42,)` is a tuple
literal of type `(i32)`, while `(42)` is just the integer `42` enclosed in
parentheses.

Tuples can be destructured to extract their individual elements. For example, if
we have a tuple `t` of type `(i32, string)`, we can write `(x, y) := t` to
assign the first element of the tuple to `x` and the second element to `y`.
This is known as tuple unpacking or destructuring assignment.  Use the `_`
symbol to ignore elements that you don't care about. For example, if we only
want the first element of the tuple, we can write `(x, _) := t`.

Tuples can also be used in pattern matching. For example, we can match a tuple
against a pattern like `(x, "Hello")` to check if the second element is the
string "Hello" and extract the first element into `x`.  Again use the `_` symbol
to ignore elements in patterns. For example, we can match a tuple against a
pattern like `(_, "Hello")` to check if the second element is the string "Hello"
without caring about the first element.

Indexing into a tuple is done using the dot notation. For example, if we have a
tuple `t` of type `(i32, string)`, we can access the first element with `t.0`
and the second element with `t.1`. The indices are zero-based, so `t.0` refers
to the first element, `t.1` to the second element, and so on.  Note that tuple
indexing is not supported in patterns, so you cannot use `t.0` or `t.1` in a
pattern match. Instead, you must use tuple destructuring to access the elements
in a pattern.

## Slices

Slices are dynamically sized views into a contiguous sequence of elements. They
are often used to represent a portion of an array or a vector. A slice is
denoted by square brackets `[]T`, where `T` is the type of the elements in the
slice. For example, a slice of integers would have the type `[]i32`.

Slices can be used to reference an array literal. For example, `[1, 2, 3]` is an
array literal of type `[3]i32`, and we can create a slice that references this
array with `s := ^[1, 2, 3]`, which would have the type `[]i32`. The `^` symbol
is used to indicate that we want to create a slice that references the array
literal.  A type of `[n]T` cannot be coerced to `[]T` implicitly.

Slices are "fat pointers" that consist of a pointer to the data and a length.

The pointer of a slice can be obtained by `^p[0]`. For example, if we have a
slice `s` of type `[]i32`, we can get a pointer to the first element of the
slice with `p := ^s[0]`. This pointer can be used to access the elements of the
slice using pointer arithmetic. For example, `p[0]` would give us the first
element of the slice, `p[1]` would give us the second element, and so on.
Slices cannot be used in patterns, so you cannot match against a slice directly.

Also, the pointer to the data of a slice can also be obtained by `s.data`. For
example, if we have a slice `s` of type `[]i32`, we can get a pointer to the
first element of the slice with `p := s.data`. This is an alternative way to
access the pointer of the slice, and it can be used in the same way as `^s[0]`
to access the elements of the slice.

You can get the length of a slice with the `.count` property. For example, if we
have a slice `s` of type `[]i32`, we can get the number of elements in the slice
with `len := s.count`. This will give us the length of the slice, which is the
number of elements it contains.  Note that the length of a slice is not the same
as the length of the underlying array. The length of a slice is determined by
the number of elements it references, while the length of an array is fixed at
compile time.

A string is an alias of `[]u8`, so string literals like `"Hello"` are of type
`[]u8`. This means that strings can be treated as slices of bytes, and you can
use slice operations on them. For example, you can get a substring of a string
with slicing syntax like `s[0..5]` to get the first five characters of the
string.  However, since strings are UTF-8 encoded, slicing a string may not
always produce valid UTF-8 if you slice in the middle of a multi-byte character.
Therefore, it's important to be careful when slicing strings to ensure that you
don't end up with invalid UTF-8.  The `..` is exclusive, so `s[0..5]` will give
you the characters at indices 0, 1, 2, 3, and 4, but not the character at index
5.

To get the first n elements of a slice, you can use the slicing syntax `s[..n]`.
For example, if we have a slice `s` of type `[]i32`, we can get the first three
elements of the slice with `first_three := s[..3]`. This will give us a new
slice that references the first three elements of the original slice `s`.
Similarly, to get the last n elements of a slice, you can use the slicing syntax
`s[len-n..]`. For example, if we have a slice `s` of type `[]i32` and we want to
get the last two elements of the slice, we can write `last_two :=
s[s.count-2..]`. This will give us a new slice that references the last two
elements of the original slice `s`.  If you use `s[..]`, it will give you a
slice that references all the elements of the original slice `s`.  The slicing
syntax also works with strings and arrays, which will return a slice.

## Structs (Plexes)

A struct type, also known as a plex, is a user-defined compound data type that
groups together related values. Each value in a struct is called a field, and
each field has a name and a type. Structs are used to represent complex data
structures that have multiple related values.

The syntax for a plex is:

```
plex {
    field1 Type1
    field2 Type2
    ...
    fieldn Typen
}
```

You can bind a symbol to a plex as normal.  To create an instance of a plex, you can use the following syntax:

```
instance := PlexName {
    field1: value1
    field2: value2
    ...
    fieldn: valuen
}
```

You can of course put a full plex definition in place of `PlexName` if you want
to create an anonymous plex instance.  The field values must match the types
specified in the plex definition.

You can create another instance of plex based on an existing instance using the update syntax:

```
new_instance := existing_instance with {
    field1: new_value1
    field2: new_value2
    ...
    fieldn: new_valuen
}
```

The compiler is free to order the fields in memory in any way it sees fit, so
you should not rely on the order of fields in a plex.  However, the fields of a
plex are always stored contiguously in memory, which allows for efficient access
to the fields.  The compiler will order the fields to minimise padding and
ensure that the fields are properly aligned according to their types.  The order
of fields in a plex definition does not affect the semantics of the program, so
you can define the fields in any order you like.

You can stop the reordering by marking the plex as `#c`, which will give it a
C-compatible layout.  This means that the fields will be laid out in memory in
the order they are defined in the plex, without any reordering, but there still
may be padding between fields to ensure proper alignment.  This is useful when
you need to interface with C code that expects a specific memory layout for a
struct.  Additionally, you can mark a plex as `#packed` to indicate that the
compiler should not add any padding between fields, which can be useful for
certain low-level programming tasks where you need to control the exact memory
layout of the struct.  However, using `#packed` can lead to unaligned memory
accesses, which may be less efficient on some architectures, so it should be
used with caution.  Packed implies `#c` layout, so the fields will be laid out
in memory in the order they are defined in the plex, without any reordering or
padding.

Here is an example of a C-based plex definition:

```
Point :: plex #c {
    x f32
    y f32
}
```

Accessing the fields of a plex is done using the dot notation. For example, if
we have a plex `p` of type `Point`, we can access the `x` field with `p.x` and
the `y` field with `p.y`.  You can also use the dot notation to access fields in
patterns.  Any pointers to a plex are automatically dereferenced when accessing
fields, so if we have a pointer `pp` of type `^Point`, we can access the `x`
field with `pp.x` and the `y` field with `pp.y`, and the compiler will
automatically dereference the pointer to access the fields of the plex.

Struct can be destructured in assignments and patterns. For example, if we have
a plex `p` of type `Point`, we can write `{ x: x, y: y } := p` to assign the `x`
field of the plex to `x` and the `y` field to `y`. This is known as struct
unpacking or destructuring assignment.  Use the `_` symbol to ignore fields that
you don't care about. For example, if we only want the `x` field of the plex, we
can write `{ x: x, _ } := p`.  If the field name is the same as the variable
name you want to assign to, you can use a shorthand syntax. For example, if we
have a plex `p` of type `Point`, we can write `{ x, y } := p` to assign the `x`
field of the plex to a variable named `x` and the `y` field to a variable named
`y`.

This works in `on` patterns too:

```
on p {
    { x, y } on x > 2 => ...
}
```

Notice the new `on <condition>` syntax for pattern guards, which allows you to
add another condition to a pattern match. In this example, the pattern `{
x, y }` will only match if the `x` field of the plex `p` is greater than 2. This
allows for more fine-grained control over pattern matching, as you can specify
additional conditions that must be met for a pattern to match.  The `on
<condition>` syntax can be used with any pattern, not just struct patterns, so
you can use it to add guards to tuple patterns, enum patterns, and so on.

Also, underscores can be used in patterns to ignore fields that you don't care
about.

## Unions (Unions)

A union type is a user-defined compound data type that can hold one of
several different types of values. Each value in a union is called a variant, and
each variant has a name and a type. Unions are used to represent data that can
take on different forms, where only one form is valid at a time.

The syntax for a union is exactly the same as a plex, but with the `union` keyword instead of `plex`:

```
union {
    field1 Type1
    field2 Type2
    ...
    fieldn Typen
}
```

You can't use unions in patterns.

## Enums (Enumerations)

An enum type is a user-defined compound data type that consists of a set of
named variants. Each variant can optionally have associated data, which can be
of different types. Enums are used to represent data that can take on a fixed
set of values, where each value is represented by a named variant.

Under the hood, enums are defined as a union of types and a integer tag that
indicates which variant is currently being used. The syntax for an enum is as
follows:

```
enum {
    Variant1 (Type1a, Type1b, ...)
    Variant2 (Type2a, Type2b, ...)
    ...
    Variantn (TypeNa, TypeNb, ...)
}
```

The tuple types following each variant name are optional.  Under the hood, the
enum looks like this:

```
plex {
    tag TagType
    union {
        (Type1a, Type1b, ...)
        (Type2a, Type2b, ...)
        ...
        (TypeNa, TypeNb, ...)
    }
}
```

Where `TagType` is an integer type that is large enough to hold the number of
variants in the enum.  The tag type can be `u8`, `u16`, `u32`, or `u64`,
depending on the number of variants in the enum.  The union part of enum is
aligned to the max alignment of the variant types, and the tag is stored in a
way that allows for efficient access to both the tag and the union data.

If there are no tuple types following a variant name, then that variant is just a
unit variant that does not have any associated data.  For example, the following enum has three unit variants:

```
enum {
    Red
    Green
    Blue
}
```

And the tag values will be 0 for `Red`, 1 for `Green`, and 2 for `Blue`.  If a
variant has tuple types following its name, then that variant is a tuple variant
that has associated data. For example, the following enum has three tuple
variants:

```
enum {
    Point (f32, f32)
    Circle (f32)
    Rectangle (f32, f32)
}
```

Values in the associated data of an enum variant can be accessed using pattern
matching using the `on` expression. For example, if we have an enum value
`shape` of the above enum type, we can match it against the variants like this:

```
on shape {
    Point (x, y) => ...
    Circle (r) => ...
    Rectangle (w, h) => ...
}
```

Variants are referenced using dot notation. For example, if we have an enum
value `shape` of the above enum type, we can check if it is a `Point` variant
with `shape.Point`, and if it is, we can access the associated data with
`shape.Point.0` for the first element and `shape.Point.1` for the second
element.  However, it's more common to access the associated data of an enum
variant using pattern matching with the `on` expression, as shown in the
previous example, which is also safer.

If the enum type is known at compile time, you can also construct enum values
using the variant names directly using an initial dot.

```
test_colour :: fn (colour: Colour) {
    on colour {
        .Red => ...
        .Green => ...
        .Blue => ...
    }
}
```

We know the `on` expression is matching against a `Colour` enum, so we can use
the initial dot syntax to refer to the variants directly without needing to
specify the enum type.  This is a convenient syntax for matching against enum
variants when the enum type is known, and it can make the code more concise and
easier to read.

## New `on` expression syntax

The `on` expression is a powerful control flow construct that allows you to match
a value against multiple patterns and execute different code based on which pattern matches. The syntax for an `on` expression is as follows:

```
on value {
    pattern1 => expr1
    pattern2 => expr2
    ...
    patternn => exprn
}
```

In this syntax, `value` is the value that you want to match against the
patterns. Each `pattern` is a pattern that you want to match against the value,
and each `expr` is an expression that will be executed if the corresponding
pattern matches. The patterns are evaluated in order, so the first pattern that
matches will determine which expression is executed.

Patterns can have guards using the `on <condition>` syntax, which allows you to
add an additional condition that must be satisfied for the pattern to match. For
example:

```
on value {
    pattern1 on condition1 => expr1
    pattern2 on condition2 => expr2
    ...
    patternn on conditionn => exprn
}
```

There is one more `on` expression:

```
on {
    condition1 => expr1
    condition2 => expr2
    ...
    conditionn => exprn
}
```

In this syntax, there is no value to match against. Instead, each `condition` is
a boolean expression that is evaluated in order, and the first condition that
evaluates to `yes` will determine which expression is executed. This allows you
to write a series of conditional checks without needing to match against a
specific value.

Finally, in the normal `on` expression, the patterns are normally values, but
they have an implicit `==` operator, so the actual format is:

```
on value {
    [<op>] pattern1 => expr1
    [<op>] pattern2 => expr2
    [<op>] pattern3 => expr3
    ...
    [<op>] patternn => exprn
}
```

This means that these two `on` expressions are equivalent:

```
on x {
    1 => ...
    2 => ...
    3 => ...
}

on x {
    == 1 => ...
    == 2 => ...
    == 3 => ...
}
```

However, you can use different operators for different patterns. For example:

```
on x {
    == 1 => ...
    > 2 => ...
    < 3 => ...
}
```


# Future ideas

- FFI syntax for external C functions
- Modules and import/export
- Traits and generics
- CLI arguments - main :: fn (args: []string) -> int
- 
