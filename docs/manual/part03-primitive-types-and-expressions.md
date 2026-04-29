# Part 3: Primitive Types And Expressions

[Manual Index](README.md) | Previous: [Values, Bindings, And Assignment](part02-values-bindings-and-assignment.md) | Next: [Functions](part04-functions.md)

Expressions compute values. Nerd keeps common arithmetic and logical expression
forms familiar, while requiring explicit casts when the type changes.

## Basic Types

Common primitive types include:

- `void`
- `bool`
- `string`
- signed integers such as `i8`, `i16`, `i32`, `i64`, and `isize`
- unsigned integers such as `u8`, `u16`, `u32`, `u64`, and `usize`
- floats `f32` and `f64`

Boolean literals are:

```nerd
yes
no
```

## Numeric Expressions

```nerd
main :: fn () -> i32 {
    value := 2 + 3 * 4
    return value
}
```

Normal precedence rules apply: multiplication, division, and modulo bind more
tightly than addition and subtraction. Parentheses make grouping explicit:

```nerd
main :: fn () -> i32 {
    return (2 + 3) * 4
}
```

## Operators

Arithmetic:

```nerd
a + b
a - b
a * b
a / b
a % b
```

Comparison:

```nerd
a == b
a != b
a < b
a <= b
a > b
a >= b
```

Logical operators work with `bool`:

```nerd
ready && allowed
ready || allowed
!ready
```

Bitwise operators work with integer values:

```nerd
mask & flag
mask | flag
mask ^ flag
```

## Casts

Use `.as(Type)` for explicit casts.

```nerd
main :: fn () -> i32 {
    count: usize = 10
    return count.as(i32)
}
```

Nerd does not silently insert broad implicit casts. If a type conversion matters,
write it down.

## Pointer-To-Slice Casts

A pointer can be converted to a slice when you provide the element type and the
element count:

```nerd
view := pointer.as([]u8, size)
```

The result is a slice view. It does not own the pointed-to storage. The program
must still know where the pointer came from and how long it remains valid.

This form is common at FFI boundaries, where C APIs often return a pointer and a
separate length.

## Strings

String literals produce `string` values:

```nerd
message := "hello"
```

Adjacent string literals are combined:

```nerd
message := "hello, "
           "world"
```

Interpolated strings start with `$`:

```nerd
use std.io

main :: fn () {
    value := 42
    prn($"value={value}")
}
```

Expressions inside `{...}` are evaluated and appended to the produced string.

## C Strings

C strings use the `c"..."` prefix:

```nerd
c"%.*s"
```

They are mainly for FFI. A C string is not the same thing as a Nerd `string`.

## Untyped Literals

Integer and float literals can begin as untyped values. Context decides their
concrete type when possible:

```nerd
value: i32 = 10
```

Here `10` becomes an `i32` because the annotation gives it context.
