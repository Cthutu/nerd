# Chapter 3: Primitive Types and Expressions

[Previous: First Programs and Bindings](02-First-Programs.md)

[Next: Variables and Scope](04-Variables-and-Scope.md)

Expressions produce values. A literal, a reference to a binding, an arithmetic
operation, and a function call can all be expressions. This chapter covers
primitive values and the operators used to combine them, using the immutable
bindings introduced in chapter 2.

Most examples show declarations you can add to a source file. Comments give
their resulting values; they are not printed output. Add `main :: fn () {}`
to examples without an entry point when checking them with `nerd check`.

## Primitive types

Nerd provides these types for numbers, truth values, and text:

| Types | Meaning |
| --- | --- |
| `i8`, `i16`, `i32`, `i64` | Signed integers, including negative values. |
| `u8`, `u16`, `u32`, `u64` | Unsigned integers, representing zero and positive values. |
| `isize`, `usize` | Signed and unsigned integers with the target's pointer width. |
| `f32`, `f64` | Floating-point numbers. |
| `bool` | A truth value. |
| `string` | Text stored as a counted sequence of bytes. |

The number in a type name is its width in bits. For example, `i32` is a signed
32-bit integer, and `u8` is an unsigned 8-bit integer. `u8` can represent values
from `0` to `255`; `i8` can represent values from `-128` to `127`. Wider integer
types provide larger ranges. Floating-point widths affect both range and
precision; many decimal fractions cannot be represented exactly.

Use `usize` for counts and sizes, and `isize` for offsets that may be negative.
Unlike C's `int` and `long`, the numbered types state their width directly.

You have already seen `void`, which describes a function with no return value.
There is no `void` literal.

## Integer literals

An integer literal writes a whole-number value directly in the source:

| Spelling | Base | Value |
| --- | --- | --- |
| `42` | Decimal | 42 |
| `0x2a` | Hexadecimal | 42 |
| `0b101010` | Binary | 42 |
| `0o52` | Octal | 42 |

Use underscores between digits to improve readability, such as `1_000_000` or
`0xff_ff`. They do not change the value. Octal uses the explicit `0o` prefix;
the base determines how digits are read, not the integer's type.

The minus sign is a unary operator applied to a literal, as in `-42`.

Single quotes can express an integer codepoint:

```nerd
letter :: 'A'    -- 65
newline :: '\n'  -- 10
```

A single Unicode codepoint in quotes produces its integer value. There is no
separate character type: `'A'` is an untyped integer, while `"A"` is a string.

## Untyped and typed integers

An ordinary integer literal begins as an **untyped integer**. It has a value,
but has not yet been given a concrete integer width or signedness. Binding a
name to it preserves that distinction:

```nerd
answer :: 42
another_answer :: answer
```

Both names refer to untyped integers. Neither binding chooses `i32`.
Expressions made entirely from untyped integer operands can also remain
untyped, so `total :: 40 + 2` does not choose a concrete integer type either.

A type annotation supplies a concrete type:

```nerd
answer :: 42
small_answer : u8 : answer
wide_answer : i64 : answer
float_answer : f32 : answer
```

`small_answer` has type `u8`, `wide_answer` has type `i64`, and `float_answer`
has type `f32`. An untyped integer can supply either an integer or a
floating-point value. Each use supplies its own context; the original `answer`
remains untyped. The compiler checks
that an integer literal fits the concrete integer type required at that use.
For example, `256` cannot be used as a `u8` literal.

A function's declared return type also supplies context:

```nerd
answer :: 42

get_small_answer :: fn () -> u8 {
    return answer
}

get_wide_answer :: fn () -> i64 {
    return answer
}
```

Here, the same binding supplies a `u8` result in one function and an `i64`
result in the other. An expression function needs a concrete return type even
when its body is untyped. As in chapter 2, `get_answer :: fn () => 42`
therefore infers `i32` as its return type.

Typed integers have already committed to a representation. A binding such as
`small_answer : u8 : 42` stays `u8` when used elsewhere. It does not acquire a
different type merely because a surrounding expression expects one. Use an
explicit cast when converting between concrete numeric types.

## Floating-point and boolean literals

Floating-point literals use a decimal point, with digits on both sides:

```nerd
fraction :: 0.5
single_fraction : f32 : fraction
double_fraction : f64 : fraction
```

Like integer literals, floating-point literals can remain untyped in bindings
and acquire a concrete type from context. When an inferred function return
type must be made concrete without other context, an untyped float defaults
to `f64`.

Write `2.0` for a floating-point literal and `2` for an integer literal.
`2.0` can be used where `f32` or `f64` is required, but not where an integer
is required, even though its value has no fractional part. Use `.as(i32)`
when an explicit conversion to an integer is intended. The reverse is allowed
without a cast: an untyped integer such as `2` can be used where an integer or
a float is required. This also applies through bindings, as in `float_answer`
above; a concrete typed integer still requires an explicit cast to a float.

Digit separators are allowed, as in `86_400.0`. Nerd currently recognises decimal
floating-point literals; C-style exponent notation such as `1e3` is not
supported.

Boolean literals are `yes` and `no`, both with the concrete type `bool`.
The C spellings `true` and `false` are not predefined boolean values:

```nerd
enabled :: yes
disabled : bool : no
```

Among these primitive types, only `bool` has truthiness: `yes` is true and `no`
is false. Integers do not implicitly act as truth values. Use a comparison such
as `count != 0` to produce a boolean. Optional and result types also have
truthiness; Chapter 10 covers them and where the compiler accepts them.

## Arithmetic expressions

The familiar arithmetic operators are available:

| Operator | Meaning |
| --- | --- |
| `+` | Addition |
| `-` | Subtraction |
| `*` | Multiplication |
| `/` | Division |
| `%` | Integer remainder |
| Prefix `-` | Negation |

```nerd
total :: 2 + 3 * 4       -- 14
grouped :: (2 + 3) * 4   -- 20
quotient :: 7 / 2        -- 3
remainder :: 7 % 2       -- 1
```

Multiplication, division, and remainder bind more tightly than addition and
subtraction. Parentheses choose a different grouping. Integer division
discards the fractional part, truncating toward zero; floating-point division
can produce a fractional result.

Concrete numeric operands need matching types. An untyped literal can adopt
the type of a concrete operand:

```nerd
base : u64 : 40
total :: base + 2
```

Here, `2` takes the type `u64`, and `total` is a `u64`. Combining a concrete
`u64` with a concrete `i32` requires an explicit conversion; Nerd does not use
C's usual arithmetic conversions to silently choose a common type.

## Comparison and logical operators

Comparisons produce `bool`:

| Operator | Meaning |
| --- | --- |
| `==` | Equal |
| `!=` | Not equal |
| `<` | Less than |
| `<=` | Less than or equal |
| `>` | Greater than |
| `>=` | Greater than or equal |

For example, `42 == 42` produces `yes`, and `3 > 10` produces `no`.
Ordering comparisons apply to numeric values. String equality is covered below.

Logical operators combine boolean values:

| Operator | Meaning |
| --- | --- |
| `&&` | Both operands are true |
| `\|\|` | At least one operand is true |
| Prefix `!` | Logical negation |

```nerd
within_limits :: 42 >= 0 && 42 < 100
outside_limits :: !within_limits
```

`&&` and `||` short-circuit. The right operand of `&&` is evaluated only if
the left operand is true; the right operand of `||` is evaluated only if the
left operand is false. For the primitive types covered here, `!` operates on
`bool`, so use `!(count == 0)` or `count != 0` when testing a number.

## Bitwise operators

Bitwise operations work on integers:

| Operator | Meaning |
| --- | --- |
| `&` | Bitwise AND |
| `\|` | Bitwise OR |
| `~` | Bitwise XOR |
| Prefix `~` | Bitwise complement |
| `<<` | Shift left |
| `>>` | Shift right |

```nerd
bits : u32 : 0b1010
low_bits :: bits & 0b0011  -- 2
combined :: bits | 0b0101  -- 15
toggled :: bits ~ 0b1111   -- 5
shifted :: bits << 1      -- 20
```

Keep bitwise `&` and `|` distinct from boolean `&&` and `||`. A right shift
of an unsigned integer shifts in zero bits; a right shift of a signed integer
preserves the sign. Prefix `~` flips every bit in an integer; its concrete type
determines how many bits are flipped.

## Operator precedence

For the operators introduced here, the following groups run from highest to
lowest precedence:

| Operators | Purpose |
| --- | --- |
| Calls, `.as(Type)`, member access | Operations on a value |
| Prefix `-`, `!`, `~` | Unary operations |
| `*`, `/`, `%` | Multiplicative arithmetic |
| `+`, `-` | Additive arithmetic |
| `<<`, `>>` | Shifts |
| `<`, `<=`, `>`, `>=` | Ordering |
| `==`, `!=` | Equality |
| `&` | Bitwise AND |
| `~` | Bitwise XOR |
| `\|` | Bitwise OR |
| `&&` | Logical AND |
| `\|\|` | Logical OR |

These precedence relationships are familiar from C. Use parentheses when
combining operators to make the intended grouping explicit. For a bit test,
write `(bits & 0b0010) != 0`: equality binds more tightly than bitwise AND,
so those parentheses matter.

## Casting with as

Use the postfix `.as(Type)` operator to convert a numeric value:

```nerd
small : u8 : 42
wide :: small.as(i64)     -- 42, with type i64
fraction : f64 : 3.9
whole :: fraction.as(i32) -- 3, with type i32
```

`wide` has type `i64`, and `whole` has type `i32`. A cast produces a new value;
it does not change the source binding's type. Unlike a C cast, the target type
follows the value being converted.

A float-to-integer cast truncates toward zero. An integer-to-float cast may
round to the nearest representable value. Choosing a narrower numeric type
can lose information, so choose the destination type with the value's range
and precision in mind.

Because `.as` binds more tightly than arithmetic, use parentheses to cast an
entire calculation: `(2 + 3).as(f64)`. Writing `2 + 3.as(f64)` casts only `3`.

## Strings

Double-quoted literals produce `string` values:

```nerd
message :: "Hello, world!"

main :: fn () {
    prn(message)
}
```

Nerd strings carry their byte count and do not depend on a terminating zero
byte. `.count` reports the number of bytes, which can differ from the number
of Unicode characters. `==` and `!=` compare string contents.

Common escape sequences include:

| Escape | Meaning |
| --- | --- |
| `\n` | Newline |
| `\r` | Carriage return |
| `\t` | Tab |
| `\\` | Backslash |
| `\"` | Double quote |
| `\0` | Zero byte |
| `\xNN` | A byte specified by two hexadecimal digits |

For example, `"first\nsecond"` contains a newline between the two words.
A zero byte is part of a Nerd string's contents; it does not end the string.

Long literals can continue on another source line using `+"`:

```nerd
message :: "Hello, "
           +"world!"
```

This forms the single string `"Hello, world!"` at compile time. Bare adjacent
string literals are not joined. The continuation syntax is for extending
literals; it is not a general string concatenation operator.

Triple quotes delimit a multiline string:

```nerd
message :: """
    first
      second
    """
```

The initial newline after the opening delimiter is omitted. The indentation of
the closing delimiter is trimmed from the body lines. This example contains
the same text as `"first\n  second\n"`, including the final newline.

## Storage size with `.size`

`.size` is a general property of types and values, not just arrays. Write a
type followed by `.size` to obtain the storage size of a value of that type
in bytes. On a typed value, `.size` gives the size of that value's representation.
The result has type `usize` in both cases:

```nerd
number : i32 : 42

main :: fn () -> i32 {
    return (i32.size + number.size).as(i32)
}
```

This returns `8`: `i32.size` and `number.size` are each `4`. The cast converts
the `usize` sum into the `i32` exit status.

The property works for every concrete type, including the compound types
introduced in later chapters. A size describes the value's own storage,
including any layout padding. It does not include separately referenced data.

For a string, `.size` measures the string value's representation, not the
number of text bytes. `"hello".count` is `5`, while `"hello".size` equals
`string.size`, regardless of the text's length. The same distinction between
a value's own size and the data it refers to matters for collections later.
