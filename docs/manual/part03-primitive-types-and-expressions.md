# Part 3: Primitive Types And Expressions

[Manual Index](README.md) | Previous: [Values, Bindings, And Assignment](part02-values-bindings-and-assignment.md) | Next: [Functions](part04-functions.md)

Expressions compute values. Nerd keeps common arithmetic and logical expression
forms familiar, while requiring explicit casts when the type changes.

## Basic Types

Common primitive types include:

| Type family       | Types                                      | Notes                                      |
| ----------------- | ------------------------------------------ | ------------------------------------------ |
| no value          | `void`                                     | used when no value is produced             |
| truth values      | `bool`                                     | exactly two values: `yes` and `no`         |
| text              | `string`                                   | immutable text value                       |
| signed integers   | `i8`, `i16`, `i32`, `i64`, `isize`         | whole numbers, including negative numbers  |
| unsigned integers | `u8`, `u16`, `u32`, `u64`, `usize`         | whole numbers, zero or positive            |
| floats            | `f32`, `f64`                               | fractional numbers                         |

The number in an integer or float type is its size in bits. For example, `i32`
is a signed 32-bit integer, `u8` is an unsigned 8-bit integer, and `f64` is a
64-bit floating-point value. The `isize` and `usize` types are pointer-sized
integer types.

Use `usize` for counts, indexes, byte sizes, and capacities: values that must be
large enough to describe memory on the target platform. Use `isize` when you
need a signed integer with the same platform-sized range, such as an offset that
may be negative.

Boolean literals are:

```nerd
yes
no
true   -- alias for yes
false  -- alias for no
```

Use `bool` for answers to yes/no questions: comparisons, loop conditions,
branch conditions, and flags.

## Numeric Expressions

```nerd
main :: fn () -> i32 {
    value := 2 + 3 * 4  -- multiplication happens before addition
    return value
}
```

Normal precedence rules apply: multiplication, division, and modulo bind more
tightly than addition and subtraction. Parentheses make grouping explicit:

```nerd
main :: fn () -> i32 {
    return (2 + 3) * 4  -- parentheses force addition first
}
```

Integer literals can be written in decimal, hexadecimal, binary, or octal:

| Form          | Base        | Example | Value |
| ------------- | ----------- | ------- | ----- |
| no prefix     | decimal     | `1_000_000` | 1000000 |
| `0x` prefix   | hexadecimal | `0x2a`  | 42    |
| `0b` prefix   | binary      | `0b101010` | 42 |
| `0o` prefix   | octal       | `0o52`  | 42    |

The prefix is part of the literal's spelling. It changes how the digits are
read, not the eventual type. Underscores may be used between digits in integer
and float literals to make large values easier to read; they do not change the
value:

```nerd
mask: u64 = 0xff_ff      -- hexadecimal literal becomes a u64 from context
bits := 0b1010_0101      -- underscores are ignored in integer literals
mode := 0o755            -- octal literal materialises as i32 without context
million := 1_000_000     -- decimal literal with digit separators
seconds := 86_400.0      -- float literal with digit separators
```

## Operators

Arithmetic operators work with matching numeric operands:

| Operator  | Meaning  |
| --------- | -------- |
| `+`       | add      |
| `-`       | subtract |
| `*`       | multiply |
| `/`       | divide   |
| `%`       | modulo   |
| unary `-` | negate   |

Comparison operators produce `bool`:

| Operator | Meaning               |
| -------- | --------------------- |
| `==`     | equal                 |
| `!=`     | not equal             |
| `<`      | less than             |
| `<=`     | less than or equal    |
| `>`      | greater than          |
| `>=`     | greater than or equal |

For non-built-in values, `==` and `!=` use the canonical `core.Eq`
implementation when one exists for the value type.

Logical operators work with `bool`:

| Operator                  | Meaning |
| ------------------------- | ------- |
| `&&`                      | and     |
| <code>&#124;&#124;</code> | or      |
| `!`                       | not     |

Bitwise operators work with matching integer operands:

| Operator            | Meaning             |
| ------------------- | ------------------- |
| `&`                 | bitwise and         |
| `^`                 | bitwise xor         |
| <code>&#124;</code> | bitwise or          |
| `<<`                | shift left          |
| `>>`                | shift right         |

Shift operators require integer operands. Use an explicit cast if the shift
count or shifted value comes from another numeric type.

## Value Size

Use `.size` to ask for the runtime size of a type or value in bytes. The result
has type `usize`:

```nerd
i32_bytes := i32.size    -- size of an i32 value
ptr_bytes := nil.size    -- nil itself has size 0
text_bytes := "hi".size  -- size of a string value, not its character count
```

For fixed arrays, `.size` is the size of the whole array value and `.count` is
the fixed element count. Fixed arrays also expose `.bytes`, which is the number
of bytes occupied by their elements.

For strings, slices, and dynamic arrays, `.size` is the size of the value
header. Use `.count` when you want the number of live elements. Slices also
expose `.bytes`, which is `slice.count * T.size` for a `[]T` slice. Strings and
dynamic arrays do not expose `.bytes`.

Untyped integer literals use the default materialised integer type for `.size`,
so `128.size` is the same as `i32.size` unless context gives the literal a
different concrete type.

## Casts

A cast is an explicit request to treat a value as another type. Use `.as(Type)`
when the conversion is part of the program's meaning.

```nerd
main :: fn () -> i32 {
    count: usize = 10
    return count.as(i32)  -- convert a usize value to an i32 value
}
```

Numeric values can be cast between concrete numeric types, such as `usize` to
`i32` or `f64` to `f32`. Pointer and slice casts are more restricted and are
introduced below, then covered again in Part 8.

Use `nil` for null pointers. Integer address constants must be written with an
explicit pointer cast:

```nerd
base: ^void = nil
same := 0x1000.as(^void)
```

Concrete integer variables do not silently become pointers. Pointer-sized
integers can be cast explicitly when an API represents a pointer-sized address
or resource ID as `usize` or `isize`:

```nerd
resource: usize = 32512
cursor_name := resource.as(^u8)
```

Keep these casts at FFI or platform boundaries where raw addresses are part of
the interface.

Casts between integers and floats are explicit because they can change the
value. Integer-to-float casts produce the nearest representable floating-point
value. Float-to-integer casts discard the fractional part by truncating toward
zero:

```nerd
whole: i32 = 3.9.as(i32)   -- becomes 3
below: i32 = -3.9.as(i32)  -- becomes -3
```

Nerd does not silently insert broad implicit casts between concrete types. If a
type conversion matters, write it down. Untyped integer and float literals are
the main exception: they can become a concrete type from context, such as a
binding annotation or function parameter type. Untyped literals are explained at
the end of this part.

## Pointer-To-Slice Casts

A pointer is a value that stores the address of another value. A plain pointer
does not carry a length.

A slice is a view over a run of elements in memory. It is represented as a
pointer plus a count, so it knows where the elements start and how many elements
the view contains. This kind of two-part view is often called a fat pointer.

Pointers and slices are covered properly in Part 8. The cast form is shown here
because it uses the same `.as(...)` syntax as primitive casts.

A pointer can be converted to a slice when you provide the element type and the
element count:

```nerd
view := pointer.as([]u8, size)  -- make a []u8 slice from pointer and count
```

The result is a slice view. It does not own the pointed-to storage. The program
must still know where the pointer came from and how long it remains valid.

This form is common at foreign function interface (FFI) boundaries, where C APIs
often return a pointer and a separate length. FFI is the part of Nerd that lets
code declare and run functions from another language.

## Strings

String literals produce `string` values. A Nerd string is an immutable sequence
of Unicode scalar values. It is not null-terminated.

At runtime, a `string` has the same broad shape as a slice: data plus count.
This makes it a fat pointer rather than a single address. A `string` is still a
distinct type, not just `[]u8`.

```nerd
message := "hello"  -- string literal produces a string value
```

Long string literals can be split with continuation string literals. A
continuation starts with `+"` and is joined to the previous string value at
compile time:

```nerd
message := "hello, "
           +"world"  -- continuation literal is joined to the previous one
```

This is equivalent to:

```nerd
message := "hello, world"
```

Bare adjacent string literals are not joined. Use `+"..."` for an intentional
continuation so adjacent string syntax stays unambiguous in other contexts.

Multi-line string literals use triple quotes. If the first character after the
opening delimiter is a newline, that newline is not part of the value. When the
closing delimiter is on its own indented line, that indentation is trimmed from
each body line:

```nerd
message := """
    alpha
      beta
    """
```

This is equivalent to:

```nerd
message := "alpha\n  beta\n"
```

An interpolated string is a string built from literal text and embedded
expressions. Interpolated strings start with `$`:

```nerd
use std.io

main :: fn () {
    value := 42
    prn($"value={value}")  -- insert value's text at {value}
}
```

Each expression inside `{...}` is evaluated, converted to text, and inserted at
that position in the produced string. In the example, the result is the string
`"value=42"`.

Use `\{` and `\}` for literal braces in interpolated string text:

```nerd
prn($"literal \{value\}, actual {value}")  -- literal {value}, actual 42
```

Primitive values and built-in aggregate values can be interpolated directly.
Other values can be interpolated when their type implements the canonical
`core.Display` trait; interpolation calls `Display.show(value)` and inserts the
returned string.

Non-built-in values can use `==` and `!=` when their type implements
`core.Eq`. They can use `<`, `<=`, `>`, and `>=` when their type implements
`core.Order`; the operators call `Order.compare(lhs, rhs)` and compare the
returned `i32` with zero.

Continuation literals after an interpolated string are part of the same
interpolated string:

```nerd
message := $"hello {name}, "
           +"again {name}"
```

When every expression inside the interpolation is a compile-time value, the
compiler can produce the string at compile time:

```nerd
name :: "world"
message :: $"hello {name}"  -- compile-time interpolation
```

This is useful for top-level bindings. Runtime interpolated strings use the
temporary string arena and may be returned or stored as ordinary `string`
values. A top-level interpolated binding must still use only compile-time values
inside `{...}` because top-level runtime string building is not available during
module initialisation.

## C Strings

A C string is a pointer to bytes ending with a zero byte. C functions use that
zero byte, called the null terminator, to find the end of the text.

C string literals use the `c"..."` prefix:

```nerd
c"hello"  -- null-terminated C string literal
```

They are mainly for FFI. A C string is not the same thing as a Nerd `string`:
it is null-terminated, does not carry a count, and is passed as a pointer. FFI
is covered in Part 11.

## Untyped Literals

Integer and float literals can begin as untyped values. An untyped literal is a
number that has not yet been fixed to a concrete storage type. Context decides
its concrete type when possible:

```nerd
value: i32 = 10     -- untyped integer literal becomes i32
whole_pi: f64 = 3   -- untyped integer literal becomes f64
```

Here `10` becomes an `i32` because the annotation gives it context. Likewise,
`3` becomes an `f64` because the annotation's type is `f64`. The same untyped
literal can become different types in different contexts:

```nerd
base_value :: 5                  -- base_value is an untyped integer
int_version: i32 = base_value    -- base_value becomes i32 here
float_version: f64 = base_value  -- base_value becomes f64 here
```

The same rule applies to floats:

```nerd
pi: f32 = 3.14        -- untyped float literal becomes f32
whole_pi: i64 = 3.14  -- untyped float literal becomes i64 (truncated to 3)
```

Here `3.14` becomes either `3.14` or `3` depending on the context.

Without other context, an integer literal materialises as `i32`, and a float
literal materialises as `f64`.

```nerd
default_int   := 42    -- default_int is an i32
default_float := 3.14  -- default_float is an f64
```

An explicit function return type can also provide context to a returned local
whose initializer is still untyped:

```nerd
Result :: isize

callback :: fn () -> Result {
    result := 0  -- result is inferred as Result, not defaulted to i32
    return result
}
```
