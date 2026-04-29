# Part 3: Primitive Types And Expressions

[Manual Index](README.md) | Previous: [Values, Bindings, And Assignment](part02-values-bindings-and-assignment.md) | Next: [Functions](part04-functions.md)

Expressions compute values. Nerd keeps common arithmetic and logical expression
forms familiar, while requiring explicit casts when the type changes.

## Basic Types

Common primitive types include:

| Type family | Types |
| --- | --- |
| no value | `void` |
| truth values | `bool` |
| text | `string` |
| signed integers | `i8`, `i16`, `i32`, `i64`, `isize` |
| unsigned integers | `u8`, `u16`, `u32`, `u64`, `usize` |
| floats | `f32`, `f64` |

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

Arithmetic operators work with matching numeric operands:

| Operator | Meaning |
| --- | --- |
| `+` | add |
| `-` | subtract |
| `*` | multiply |
| `/` | divide |
| `%` | modulo |
| unary `-` | negate |

Comparison operators produce `bool`:

| Operator | Meaning |
| --- | --- |
| `==` | equal |
| `!=` | not equal |
| `<` | less than |
| `<=` | less than or equal |
| `>` | greater than |
| `>=` | greater than or equal |

Logical operators work with `bool`:

| Operator | Meaning |
| --- | --- |
| `&&` | and |
| `||` | or |
| `!` | not |

Bitwise operators work with matching integer operands:

| Operator | Meaning |
| --- | --- |
| `&` | bitwise and |
| `^` | bitwise xor |
| `|` | bitwise or |

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

Pointers and slices are compound data concepts explained in Part 8. The cast
form is shown here because it uses the same `.as(...)` syntax as primitive
casts.

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

They are null-terminated byte strings, mainly for FFI. A C string is not the
same thing as a Nerd `string`. FFI is covered in Part 11.

## Untyped Literals

Integer and float literals can begin as untyped values. Context decides their
concrete type when possible:

```nerd
value: i32 = 10
```

Here `10` becomes an `i32` because the annotation gives it context.
