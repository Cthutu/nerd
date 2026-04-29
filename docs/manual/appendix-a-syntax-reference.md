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
Name :: Type
Name :: plex { field Type }
Name :: union { field Type }
Name :: enum { Variant Payload(Type) }
pub name :: value
pub Name :: Type
ffi "lib" foreign_name (...) -> Type
local_name :: ffi "lib" foreign_name (...) -> Type
alias :: mod module.path
use module.path
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
fn (a: Type) => expr
fn (Type, Type) -> ReturnType
```

## Control Flow

```nerd
return
return expr
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
plex #c { field Type }
plex #packed { field Type }
union { field Type }
enum { Variant Payload(Type) }
```

## Casts

```nerd
value.as(Type)
pointer.as([]T, count)
```

## Literals

| Literal form | Type |
| --- | --- |
| `123` | `untyped integer`; materialises to `i32` without other context |
| `1.5` | `untyped float`; materialises to `f64` without other context |
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
