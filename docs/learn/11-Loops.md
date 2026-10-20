# Chapter 11: Loops with `for`

[Previous: Optionals, Results and Error Handling](10-Optionals-Results-and-Error-Handling.md)

[Next: Generics](12-Generics.md)

Nerd uses `for` for every loop. The same keyword covers the jobs of C's
`while` and `for`, as well as iteration over collections. Loops can also
produce values, using the `break` keyword already seen in expression blocks.

| Form | Meaning |
| --- | --- |
| `for condition { ... }` | Repeat while a condition is true. |
| `for { ... }` | Repeat until control explicitly leaves the loop. |
| `for initialization; condition; update { ... }` | A C-style loop. |
| `for item in collection { ... }` | Visit successive items. |

The examples use types and expressions from earlier chapters. Custom iteration
behavior is introduced later, after generics and traits.

## Repeating while a condition is true

The condition is tested before each iteration:

```nerd
main :: fn () -> i32 {
    count := 0
    for count < 3 {
        count += 1
    }
    return count
}
```

This returns `3`. If the first test is false, the body never runs. The
condition is checked again after each completed iteration.

The current compiler requires a `bool` condition here. As discussed in
Chapter 10, use `!!outcome` when you want an optional's presence or a result's
success as a boolean condition; direct truthiness is not yet accepted in
every conditional context.

## Unconditional loops and `break`

Omit the condition for an unconditional loop:

```nerd
main :: fn () -> i32 {
    count := 0
    for {
        count += 1
        on count == 3 => break
    }
    return count
}
```

This returns `3`. A bare `break` exits the surrounding loop and execution
continues after it. `return` would leave the whole function instead.
The `on` branch does not introduce a new target for loop control.

## C-style loops

Separate initialization, condition, and update with semicolons:

```nerd
main :: fn () -> i32 {
    total := 0
    for i := 0; i < 7; i += 1 {
        total += i
    }
    return total * 2
}
```

This returns `42`: the sum of `0` through `6` is `21`.

| Part | When it runs |
| --- | --- |
| `i := 0` | Once, before the first condition test. |
| `i < 7` | Before each iteration. |
| `i += 1` | After each completed iteration, before the next test. |

The variable `i` belongs to the loop's scope and is unavailable after the loop.
The body has its own scope for each iteration.

## Skipping to the next iteration with `again`

`again` is Nerd's counterpart to C's `continue`:

```nerd
main :: fn () -> i32 {
    total := 0
    for i := 0; i < 5; i += 1 {
        on i == 2 => again
        total += i
    }
    return total
}
```

This returns `8`, the sum of `0`, `1`, `3`, and `4`. `again` skips the rest of
the current body. In a C-style loop, the update still runs before the next
condition test. In a condition loop, control returns to the condition test.
In collection iteration, it advances to the next item.

## Iterating over arrays and slices

Built-in collection iteration binds a **pointer to each element**:

```nerd
main :: fn () -> i32 {
    values := [10, 20, 6]
    for item in values {
        item^ += 2
    }

    view: []i32 = values
    total := 0
    for item in view {
        total += item^
    }
    return total
}
```

This returns `42`. The first loop changes the array to `[12, 22, 8]`.
Its `item` has type `^i32`; `item^` accesses the actual array element.
The second loop visits the same elements through a slice and reads their values.

The pointer binding is not a separate copy of the element. Its validity follows
the collection's backing storage, just like other borrowed pointers from
Chapters 6 and 7. Keep that storage valid while using those pointers.

## Getting the index too

Use two bindings when the index is needed:

```nerd
main :: fn () -> i32 {
    values := [10, 20, 9]
    total := 0
    for index, item in values {
        total += index.as(i32) + item^
    }
    return total
}
```

This returns `42`: the terms are `10`, `21`, and `11`. `index` is a zero-based
`usize`, while `item` is still a pointer. The cast converts each index to the
integer type used by `total`.

## Dynamic arrays and strings

Dynamic arrays use the same pointer-item rule:

```nerd
main :: fn () -> i32 {
    values: [..]i32
    defer values.free()
    values.push(20)
    values.push(22)

    total := 0
    for item in values {
        total += item^
    }
    return total
}
```

This returns `42`. Iteration does not transfer ownership or free the array;
the registered `defer` releases it when `main` exits.

String iteration visits bytes, rather than Unicode characters:

```nerd
main :: fn () -> i32 {
    matches := 0
    for byte in "banana" {
        on byte^ == 'a' => matches += 1
    }
    return matches
}
```

This returns `3`. Each `byte` is a pointer to a `u8`. The optional leading
index has type `usize`, just as it does for arrays and slices. Reading bytes
is different from decoding multi-byte UTF-8 characters.

## Integer ranges

Bracketed ranges produce integer values, rather than pointers:

```nerd
main :: fn () -> i32 {
    total := 0
    for value in [0..3] {
        total += value
    }
    for value in [3..=5] {
        total += value
    }
    return total
}
```

This returns `15`. `[0..3]` produces `0`, `1`, and `2`; `[3..=5]` includes
its final bound and produces `3`, `4`, and `5`. Use `value` directly, without
`^`. An optional leading index is a zero-based `usize`, distinct from the
range value: in `[3..6]`, the first index is `0` and the first value is `3`.

Unused loop bindings follow Chapter 4's underscore rule:

```nerd
main :: fn () -> i32 {
    count := 0
    for _ in [0..3] {
        count += 1
    }
    return count
}
```

This returns `3`. `_` explicitly discards each value. When only an index is
unwanted, a name such as `_index` in `for _index, item in values` marks it
unused while keeping the item available.

## A range membership condition

The expression `value in [start..end]` produces a boolean indicating whether
the value is within the range. Inclusive `..=` works here too. Parenthesize
a membership expression when using it as a loop condition:

```nerd
main :: fn () -> i32 {
    value := 0
    for (value in [0..3]) {
        value += 1
    }
    return value
}
```

This returns `3`. It repeatedly tests the existing variable `value`.
Without parentheses, `for value in [0..3]` instead declares a loop item and
iterates the range. The two forms have different meanings.

## Loops that produce values

Use `break expression` to provide a loop's result. A finite value-producing
loop needs an `else` block for the case where it finishes without such a break:

```nerd
find :: fn (values: []i32, needle: i32) -> i32 {
    return for index, item in values {
        on item^ == needle => break index.as(i32)
    } else {
        break -1
    }
}

main :: fn () -> i32 {
    values := [20, 42, 9]
    found := find(values, 42)
    missing := find(values, 7)
    return found - missing
}
```

This returns `2`: the matching index is `1`, and the missing value produces
`-1`. A value-producing break skips the loop's `else`. Normal exhaustion
enters `else`, whose `break -1` supplies the fallback result.

The conditional break can also be written
`break on item^ == needle => index.as(i32)`. Both forms select a loop result
when the condition is true and continue the body otherwise.

An unconditional loop has no normal exhaustion and needs no fallback:

```nerd
main :: fn () -> i32 {
    answer := for {
        break 42
    }
    return answer
}
```

This returns `42`. Use or explicitly discard a loop's result, following the
same rules as other expressions.

## Labels for nested loops

A label makes the target explicit. Put `$name` after the loop's header:

```nerd
main :: fn () -> i32 {
    count := 0
    for $outer {
        for {
            count += 1
            break $outer
        }
    }
    return count
}
```

This returns `1`. `break $outer` exits the outer loop directly. A bare
`break` there would leave only the inner loop.

The same label syntax works with `again`:

```nerd
main :: fn () -> i32 {
    total := 0
    for row := 0; row < 3; row += 1 $outer {
        for column in [0..3] {
            on column == 1 => again $outer
            total += row
        }
    }
    return total
}
```

This returns `3`. Each row adds its value once, then the inner loop reaches
column `1` and advances the outer loop. The outer update still runs.

Statement blocks and `on` branches do not capture `break` or `again`.
An expression block such as `${ ... }` does provide a value target for
`break`, as in Chapter 4. Use a loop label when a nested expression block
would otherwise receive the break intended for the loop. `again` targets a
loop rather than an expression block.

A label can also accompany a value, as in `break $outer 42`. Compiler note:
that works directly in the labelled loop's body, but using it from inside
another loop currently leaves the outer loop incorrectly typed as `void`.
When a nested loop needs to supply an outer result, assign it to a variable
and use a bare labelled break as a workaround.

## Cleanup on each iteration

The loop body is a scope for each iteration, so deferred cleanup runs when
that iteration ends, including through `again` or `break`:

```nerd
main :: fn () {
    for value in [0..2] {
        defer prn("leaving iteration")
        on value == 0 => again
        prn("body")
    }
}
```

This prints:

```text
leaving iteration
body
leaving iteration
```

Cleanup registered outside the loop belongs to the enclosing scope and runs
when that scope exits. Chapter 10's `undo` rules also remain in effect:
normal iteration completion, `break`, and `again` discard an iteration's
undo actions; a return carrying absence or an error runs the active ones.
