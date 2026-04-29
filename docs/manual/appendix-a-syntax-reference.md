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
Name :: plex { field Type }
Name :: union { field Type }
Name :: enum { Variant Payload(Type) }
pub name :: value
ffi "lib" foreign_name (...) -> Type
local_name :: ffi "lib" foreign_name (...) -> Type
alias :: mod module.path
use module.path
```

## Local Bindings

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
break label expr
continue
continue label
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
label: for { ... }
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
```

## Casts

```nerd
value.as(Type)
pointer.as([]T, count)
```

## Literals

```nerd
123
1.5
yes
no
nil
undefined
"string"
c"c string"
$"interpolated {value}"
[1, 2, 3]
(1, "two")
Type { field: value }
Enum.Variant
Enum.Payload(value)
```

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
== value
!= value
< value
<= value
> value
>= value
pattern as name
pattern on condition
pattern as name on condition
```
