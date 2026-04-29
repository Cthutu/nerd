# Part 2: Values, Bindings, And Assignment

[Manual Index](README.md) | Previous: [First Programs](part01-first-programs.md) | Next: [Primitive Types And Expressions](part03-primitive-types-and-expressions.md)

Nerd has a small set of binding forms. The operator you use tells the reader
whether you are declaring a constant, creating a mutable variable, choosing an
explicit type, or assigning to an existing value.

Some examples in this part use `i32` and `-> i32` so they can return visible
integer results. Read `i32` as "an integer type" for now; primitive types are
explained in the next part, and function signatures are explained in Part 4.

## Binding Forms

A binding gives a name to a value. The main choices are whether the name is
constant or mutable, and whether the type is inferred or written explicitly.

| Form                 | Meaning                                      |
| -------------------- | -------------------------------------------- |
| `name :: value`      | constant binding with inferred type          |
| `name: Type: value`  | constant binding with explicit type          |
| `name := value`      | mutable binding with inferred type           |
| `name: Type = value` | mutable binding with explicit type and value |
| `name: Type`         | mutable binding with default storage value   |

These forms are about the binding itself, not whether the binding appears at
the top level or inside a function. Scope rules are covered in Part 5; for now,
read "scope" as the region of code where a name can be used.

## Constant Bindings

Use `::` when a constant's type can be inferred from its value.

```nerd
limit :: 10

main :: fn () -> i32 {
    return limit
}
```

Constants can refer to other top-level declarations, even declarations that
appear later in the file:

```nerd
answer :: base + 2
base :: 40
```

Use `name: Type: value` when a constant should have an explicit type.

```nerd
answer: i32: 42
```

## Mutable Bindings

Use `:=` to introduce a mutable binding whose type is inferred from its
initialiser.

```nerd
main :: fn () -> i32 {
    value := 21
    return value * 2
}
```

Use `name: Type = value` when a mutable binding should have an explicit type.

```nerd
main :: fn () -> i32 {
    value: i32 = 42
    return value
}
```

Some mutable bindings can omit the initial value. In that form, Nerd creates
storage for the value and fills it with the type's default storage value.

```nerd
main :: fn () -> i32 {
    total: i32
    total = 5
    return total
}
```

A default storage value is the value Nerd uses for a typed storage location
when no initial value is provided:

| Type kind                  | Default storage value                  |
| -------------------------- | -------------------------------------- |
| integer and floating types | zero                                   |
| `bool`                     | `no`                                   |
| `string`                   | empty string value                     |
| pointer types              | `nil`                                  |
| slice and dynamic array    | nil-backed empty value                 |
| fixed array, tuple, plex   | each element or field gets its default |

## Assignment

Use `=` to assign to an existing mutable target.

```nerd
main :: fn () -> i32 {
    total := 0
    total = total + 10
    return total
}
```

The left side must be something assignable. For now, that means a local variable.
Later parts add more assignable targets, such as fields, indexes, and
dereferences.

The assigned value must match the target's type. Nerd is statically typed, so
type mismatches are compile-time errors rather than surprises at runtime.

A mutable binding must also have a meaningful value before it is read. The
compiler's read-before-assignment diagnostics are future work; until then,
prefer giving a binding an initial value, assigning it before first use, or
using `undefined` only when you are deliberately choosing to skip default
initialisation.

Assignments are expressions. The expression's value is the value assigned to the
target:

```nerd
main :: fn () -> i32 {
    value: i32 = 1
    copy: i32 = value = 9
    return copy
}
```

The inner expression `value = 9` stores `9` in `value`, then evaluates to `9`.
That result is assigned to `copy`, so both `value` and `copy` become `9`.

## Compound Assignment

Compound assignments update an existing value in place:

```nerd
main :: fn () -> i32 {
    value := 10
    value += 5
    value *= 2
    return value
}
```

Compound assignment reads the current value of the target, applies an operator
with the right side, and stores the result back into the same target. For
example, `value += 5` means the same kind of update as `value = value + 5`.

Assignment operators are:

| Operator | Meaning                  |
| -------- | ------------------------ |
| `=`      | assign                   |
| `+=`     | add, then assign         |
| `-=`     | subtract, then assign    |
| `*=`     | multiply, then assign    |
| `/=`     | divide, then assign      |
| `%=`     | modulo, then assign      |
| `&=`     | bitwise and, then assign |
| `^=`     | bitwise xor, then assign |
| `|=`     | bitwise or, then assign  |
| `&&=`    | logical and, then assign |
| `||=`    | logical or, then assign  |

The arithmetic, bitwise, and logical operators used by these assignment forms
are explained in the next part.

## `undefined`

`undefined` creates intentionally uninitialised storage for a typed value. It
means "reserve storage for this value, but do not fill it with the type's
default storage value".

```nerd
main :: fn () -> i32 {
    value: i32 = undefined
    value = 42
    return value
}
```

Use `undefined` sparingly. It is useful for low-level code and for values that
are definitely assigned before meaningful use.

Reading an uninitialised value is not meaningful program behaviour. The stored
bits are whatever happened to be in that storage location, so the result should
not be treated as a real value of the type. Assign to the binding before any
read.

## Choosing A Form

| Need                                  | Form                       |
| ------------------------------------- | -------------------------- |
| constant with inferred type           | `name :: value`            |
| constant with explicit type           | `name: Type: value`        |
| mutable binding with inferred type    | `name := value`            |
| mutable binding with explicit value   | `name: Type = value`       |
| mutable binding with default storage  | `name: Type`               |
| change an existing mutable target     | `target = value`           |
| explicit uninitialised typed storage  | `name: Type = undefined`   |
