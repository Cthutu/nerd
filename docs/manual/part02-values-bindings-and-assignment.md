# Part 2: Values, Bindings, And Assignment

[Manual Index](README.md) | Previous: [First Programs](part01-first-programs.md) | Next: [Primitive Types And Expressions](part03-primitive-types-and-expressions.md)

Nerd has a small set of binding forms. The operator you use tells the reader
whether you are declaring a constant, creating a local, or assigning to an
existing value.

## Constant Bindings

Use `::` for constant bindings.

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

## Local Inferred Bindings

Use `:=` to introduce a local binding whose type is inferred.

```nerd
main :: fn () -> i32 {
    value := 21
    return value * 2
}
```

The name is scoped to the block where it appears.

## Typed Local Bindings

Use `name: Type = value` when you want an explicit type.

```nerd
main :: fn () -> i32 {
    value: i32 = 42
    return value
}
```

Some local declarations can omit the initializer and use the type's default
storage value:

```nerd
main :: fn () -> i32 {
    total: i32
    total = 5
    return total
}
```

## Assignment

Use `=` to assign to an existing mutable target.

```nerd
main :: fn () -> i32 {
    total := 0
    total = total + 10
    return total
}
```

The left side must be something assignable, such as a local variable, a field,
an index, or a dereference.

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

Use compound assignment when the left side is the value being updated. It is
equivalent in spirit to writing `value = value <op> rhs`, but keeps the target
clear.

## `undefined`

`undefined` creates intentionally uninitialised storage for a typed value.

```nerd
main :: fn () -> i32 {
    value: i32 = undefined
    value = 42
    return value
}
```

Use `undefined` sparingly. It is useful for low-level code and for values that
are definitely assigned before meaningful use. It is not a normal default and
does not make a value safe to read.

## Choosing A Form

- Use `::` for top-level constants, functions, and aliases.
- Use `:=` for ordinary local values with obvious types.
- Use `name: Type = value` when the type matters.
- Use `=` when changing an existing value.
- Use `undefined` only when you need explicit uninitialised storage.
