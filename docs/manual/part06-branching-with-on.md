# Part 6: Branching With `on`

[Manual Index](README.md) | Previous: [Blocks, Scope, And Cleanup](part05-blocks-scope-and-cleanup.md) | Next: [Loops](part07-loops.md)

`on` is Nerd's main branching construct. It covers simple boolean branches,
condition chains, and pattern matching.

## Short Boolean Form

Use `on condition => expr else expr` for a compact conditional expression:

```nerd
classify :: fn (value: i32) -> string {
    return on value > 0 => "positive" else "not positive"
}
```

As a statement, a short `on` can run an action when a condition is true:

```nerd
use std.io

main :: fn () {
    value := 10
    on value > 5 => prn("large")
}
```

## Condition Chains

Use `on { ... }` when each branch has its own boolean condition:

```nerd
describe :: fn (value: i32) -> string {
    return on {
        value < 0 => "negative"
        value == 0 => "zero"
        value < 10 => "small"
        else => "large"
    }
}
```

Value-producing condition chains need an `else`, because arbitrary conditions
cannot be proven exhaustive.

## Matching A Scrutinee

Use `on value { ... }` to match one value against patterns:

```nerd
score :: fn (value: i32) -> i32 {
    return on value {
        < 0 => -1
        == 0 => 0
        1, 2 => 10
        >= 10 => 100
        else => 20
    }
}
```

Patterns can include:

- literal values
- comma-separated alternatives
- ranges
- explicit comparisons
- enum variants
- structural patterns

## Ranges

Integer ranges use `..` or `..=`:

```nerd
bucket :: fn (value: i32) -> string {
    return on value {
        0..10 => "small"
        10..=20 => "medium"
        else => "large"
    }
}
```

`..` excludes the end. `..=` includes the end.

## Pattern Binders

Patterns can bind values for use in a branch:

```nerd
Maybe :: enum {
    None
    Some(i32)
}

value_or_zero :: fn (value: Maybe) -> i32 {
    return on value {
        Some(as x) => x
        None => 0
    }
}
```

The binder `x` exists only in the selected branch.

## Guards

Use a guard when a pattern needs an extra condition:

```nerd
Maybe :: enum {
    None
    Some(i32)
}

describe :: fn (value: Maybe) -> string {
    return on value {
        Some(as x) on x > 10 => "large"
        Some(_) => "some"
        None => "none"
    }
}
```

Guards refine a branch. They do not by themselves make a match exhaustive.

## Exhaustiveness

If an `on` expression produces a value, every possible path must produce a
value. Use `else` when the compiler cannot prove all cases are covered.

Statement-form `on` can be partial:

```nerd
on command {
    "look" => prn("You see nothing.")
    "quit" => prn("Goodbye.")
}
```

If no branch matches, nothing happens.
