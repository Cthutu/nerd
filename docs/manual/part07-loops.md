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

The loop continues while the condition is true.

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

User-defined iterators implement `Iterator[Item]` and return `Option[Item]`
from `next`. `None` ends the loop, while `Some(value)` binds `value` to the
loop item:

```nerd
Counter :: plex { current i32 end i32 }

impl Iterator[i32] for Counter {
    next :: fn (self: ^Self) -> Option[i32] {
        result: Option[i32] = None
        on self.current < self.end {
            yes => {
                value := self.current
                self.current += 1
                result = Some(value)
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

Use labels only when the target would otherwise be ambiguous.
