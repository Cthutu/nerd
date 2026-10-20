# Part 7: Loops

[Manual Index](README.md) | Previous: [Branching With `on`](part06-branching-with-on.md) | Next: [Compound Data](part08-compound-data.md)

Nerd uses `for` for every loop. The same keyword covers infinite loops,
condition loops, C-style loops, and iteration over collections.

## Infinite Loops

```nerd
main :: fn () -> i32 {
    value := 0
    for {  -- loop until a break runs
        value += 1
        on value == 3 => break  -- leave the loop
    }
    return value
}
```

Use `break` to leave a loop.

## Condition Loops

```nerd
main :: fn () -> i32 {
    value := 0
    for value < 3 {  -- repeat while this condition is true
        value += 1
    }
    return value
}
```

The loop continues while the condition is truthy. Conditions accept booleans,
boxes, optionals, and results. A box is truthy when allocated, an optional when
present, and a result when successful. The payload is not tested: a present
`?bool` containing `no`, or a successful result containing zero, is still truthy.

```nerd
for try_step() {  -- repeat while an optional is present or a result succeeds
    iterations += 1
}
```

The condition is evaluated once before every iteration, including the final
check that ends the loop. Absence or error ends the loop normally; it does not
propagate the failure or bind the payload. Use the existing extraction syntax
inside the body when the payload is needed.

## C-Style Loops

```nerd
main :: fn () -> i32 {
    total := 0
    for i := 0; i < 5; i += 1 {  -- init; condition; update
        total += i
    }
    return total
}
```

The three parts are:

| Part | Role |
| --- | --- |
| initialisation | runs once before the loop starts |
| condition | checked before each iteration |
| update | runs after each iteration |

The condition uses the same truth rules as a condition loop, including optional
presence and result success.

## `again`

`again` skips to the next loop iteration:

```nerd
main :: fn () -> i32 {
    total := 0
    for i := 0; i < 5; i += 1 {
        on i == 2 => again  -- skip the rest of this iteration
        total += i
    }
    return total
}
```

## `for in`

Use `for item in collection` to iterate arrays, slices, strings, dynamic
arrays, integer ranges, and values that implement `core.Iterator[Item]`.

Arrays and slices are introduced in Part 8, and dynamic arrays in Part 9. For
now, read `words` as a fixed list of strings.

```nerd
use std.io

main :: fn () {
    words :: ["north", "south", "east"]
    for word in words {  -- bind each item to word in turn
        prn(word^)
    }
}
```

When both the collection index and item are needed, bind both names:

```nerd
use std.io

main :: fn () {
    words :: ["north", "south", "east"]
    for i, word in words {
        prn($"{i}: {word^}")
    }
}
```

The index binding has type `usize`. Built-in collection items are pointers to
elements; use `item^` when you want the pointed-to value.

Integer ranges use bracketed `start..end` or `start..=end` forms:

```nerd
main :: fn () -> i32 {
    total := 0
    for i in [0..10] {  -- 0 up to, but not including, 10
        total += i
    }
    for i in [10..=12] {  -- 10 through 12
        total += i
    }
    return total
}
```

Range loop items are integer values, not pointers. As with collection
iteration, an optional leading index binding has type `usize`:

```nerd
for index, value in [3..6] {
    prn($"{index}: {value}")
}
```

A bare `for name in expression` always means iteration. Parenthesise a boolean
membership condition to distinguish it from that form:

```nerd
for (value in [0..10]) {
    value += 1
}
```

User-defined iterators implement `Iterator[Item]` and return `?Item`
from `next`. `nil` ends the loop, while a present value binds to the
loop item:

```nerd
Counter :: plex { current i32 end i32 }

impl Iterator[i32] for Counter {
    next :: fn (self: ^Self) -> ?i32 {
        result: ?i32 = nil
        on self.current < self.end {
            yes => {
                value := self.current
                self.current += 1
                result = value
            }
            else => {}
        }
        return result
    }
}

sum := 0
counter := Counter { current: 0, end: 4 }
for value in counter {
    sum += value
}
```

## Loop Values

Loops can produce values through `break <expr>`:

```nerd
main :: fn () -> i32 {
    found := for i := 0; i < 10; i += 1 {  -- loop produces a value
        on i == 4 => break i               -- value for found
    } else {
        break -1                           -- fallback value
    }

    return found
}
```

A finite value-producing loop needs `else`, because the loop might finish
without hitting a value-producing `break`.

An `on` branch does not capture loop control. A `break` or `again` inside an
`on` branch targets the surrounding loop, so this is a compact way to return a
loop value when a condition is met:

```nerd
find :: fn (values: [..]i32, needle: i32) -> i32 {
    return for value in values {
        on value == needle => break value
    } else {
        break -1
    }
}
```

Expression blocks are still value targets, so `break $label <expr>` can target a
labelled expression block when that is the nearest matching label.

The same conditional break can be written with `break on <condition> => <expr>`.
When needed, place a loop label after `break`:

```nerd
find :: fn (values: [..]i32, needle: i32) -> i32 {
    return for value in values {
        break on value == needle => value
    } else {
        break -1
    }
}
```

## Labels

Loops can also use labels when nested control flow needs an explicit target.
Loop labels use the same `$name` spelling as expression-block labels:

```nerd
main :: fn () -> i32 {
    count := 0
    for $outer {  -- labelled loop
        for {
            count += 1
            break $outer  -- leave the labelled loop
        }
    }
    return count
}
```

A labelled break can also supply the named loop's result from inside a nested
loop:

```nerd
answer := for $outer {
    for {
        break $outer 42
    }
}
```

Here, `answer` is `42`. Only breaks targeting `$outer` determine its result
type; the inner loop's own breaks belong to the inner loop. All values supplied
to the same loop must have compatible types. A finite outer loop still needs
an `else` value for normal exhaustion.

Use labels only when the target would otherwise be ambiguous.
