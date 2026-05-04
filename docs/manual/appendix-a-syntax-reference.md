# Appendix A: Syntax Reference

[Manual Index](README.md) | Previous: [Diagnostics And Debugging](part13-diagnostics-and-debugging.md) | Next: [Language Reference](appendix-b-language-reference.md)

This appendix is a compact source-level reference. Earlier parts explain the
ideas in more detail.

## Comments

```nerd
-- line comment
```

## Declarations

```nerd
name :: value
name :: fn (...) -> Type { ... }
name :: fn (...) => expr
name :: fn [T, U] (...) -> Type { ... }
name :: fn [T, U] (...) => expr
Name :: Type
Name :: plex { field Type }
Name :: plex [T, U] { field Type }
Name :: union { field Type }
Name :: union [T, U] { field Type }
Name :: enum { Variant Payload(Type) }
Name :: enum [T, U] { Variant Payload(Type) }
pub name :: value
pub Name :: Type
impl Type {
    name :: fn (self: Type, ...) -> ReturnType { ... }
    pub name :: fn (self: ^Type, ...) { ... }
}
impl Name[T] {
    name :: fn (self: ^Name[T], value: T) { ... }
}
ffi "lib" foreign_name (...) -> Type
ffi "lib" {
    foreign_name (...) -> Type
}
local_name :: ffi "lib" foreign_name (...) -> Type
name :: use module.path
use module.path
pub use module.path
test "name" { statements }
```

## Mutable Bindings

```nerd
name := expr
name: Type = expr
name: Type
name: Type = undefined
```

## Assignment

```nerd
target = expr
target += expr
target -= expr
target *= expr
target /= expr
target %= expr
target &= expr
target ^= expr
target |= expr
target &&= expr
target ||= expr
(left, right) = expr
{ field, other: name } = expr
```

## Functions

```nerd
fn (a: Type, b: Type) -> ReturnType { ... }
fn [T, U] (a: T, b: U) -> T { ... }
fn (a: Type, b: Type = expr) -> ReturnType { ... }
fn (a: Type) => expr
fn [T] (a: T) => expr
fn (Type, Type) -> ReturnType
```

## Control Flow

```nerd
return
return expr
assert expr
assert expr, "message"
break
break expr
break $label
break $label expr
continue
continue $label
defer statement
defer { statements }
```

## Branching

```nerd
on condition => expr else expr

on value {
    pattern => expr
    else => expr
}

on {
    condition => expr
    else => expr
}
```

## Loops

```nerd
for { ... }
for condition { ... }
for init; condition; update { ... }
for item in collection { ... }
for ^item in collection { ... }
for index, item in collection { ... }
for index, ^item in collection { ... }
```

## Types

```nerd
void
bool
string
i8 i16 i32 i64 isize
u8 u16 u32 u64 usize
f32 f64
^T
[]T
[N]T
[..]T
[N..]T
(T1, T2)
fn (T1, T2) -> R
plex { field Type }
plex [T] { field T }
plex #c { field Type }
plex #packed { field Type }
union { field Type }
union [T] { field T }
enum { Variant Payload(Type) }
enum [T] { Variant(T) }
Name[T]
Name[T, U]
```

## Casts

```nerd
value.as(Type)
pointer.as([]T, count)
value.size
Type.size
```

## Literals

| Literal form | Type |
| --- | --- |
| `123`, `1_000_000` | `untyped integer`; materialises to `i32` without other context |
| `0xff`, `0xff_ff` | `untyped integer`; hexadecimal literal |
| `0b1010`, `0b1010_0101` | `untyped integer`; binary literal |
| `0o755`, `0o7_55` | `untyped integer`; octal literal |
| `1.5`, `1_000.5`, `1.5_25` | `untyped float`; materialises to `f64` without other context |
| `yes`, `no` | `bool` |
| `ptr: ^i32 = nil` | `^i32` |
| `value: i32 = undefined` | `i32` |
| `"string"` | `string` |
| `c"c string"` | `^u8` to null-terminated bytes |
| `$"interpolated {value}"` | `string` |
| `[1, 2, 3]` | `[3]i32` |
| `(1, "two")` | `(i32, string)` |
| `Point { x: 1, y: 2 }` | `Point` |
| `Colour.Red` | `Colour` |
| `Maybe.Some(42)` | `Maybe` |

## Patterns

```nerd
literal
_
name
Variant
Variant(name)
left, right
start..end
start..=end
<conditional-op> value
pattern as name
pattern on condition
pattern as name on condition
```
