# Part 6: Branching With `on`

[Manual Index](README.md) | Previous: [Blocks, Scope, And Cleanup](part05-blocks-scope-and-cleanup.md) | Next: [Loops](part07-loops.md)

`on` is Nerd's main branching construct. Branching means choosing which
expression or statement to run based on a condition or value. `on` covers simple
boolean branches, condition chains, and pattern matching.

## Short Boolean Form

Use `on condition => expr else expr` for a compact conditional expression:

```nerd
classify :: fn (value: i32) -> string {
    return on value > 0 => "positive" else "not positive"  -- choose by bool
}
```

As a statement, a short `on` can run an action when a condition is true:

```nerd
use std.io

main :: fn () {
    value := 10
    on value > 5 => prn("large")  -- run only when the condition is true
}
```

## Condition Chains

Use `on { ... }` when each branch has its own boolean condition:

```nerd
describe :: fn (value: i32) -> string {
    return on {  -- each branch has its own condition
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

Use `on value { ... }` to match one value, called the scrutinee, against
patterns:

```nerd
score :: fn (value: i32) -> i32 {
    return on value {  -- match value against the branch patterns
        < 0 => -1
        == 0 => 0
        1, 2 => 10
        >= 10 => 100
        else => 20
    }
}
```

Patterns can include:

| Pattern form | Meaning |
| --- | --- |
| `1` | literal value |
| `1, 2` | alternatives |
| `0..10` | range, excluding the end |
| `0..=10` | range, including the end |
| `< 10` | explicit comparison |
| enum variants | covered in Part 8 |
| structural patterns | covered in Part 8 |

## Ranges

Integer ranges use `..` or `..=`:

```nerd
bucket :: fn (value: i32) -> string {
    return on value {
        0..10 => "small"     -- 0 up to but not including 10
        10..=20 => "medium"  -- 10 through 20 inclusive
        else => "large"
    }
}
```

`..` excludes the end. `..=` includes the end.

Pattern values may come from variables that are already in scope:

```nerd
name := "matt"
details := (42, "matt")

on details {
    (42, name) => prn($"Hello {name}!")  -- compares with the outer name value
    (42, as matched) => prn(matched)     -- binds the tuple field
}
```

## Pattern Binders

Patterns can bind values for use in a branch:

```nerd
score :: fn (value: u32) -> u32 {
    return on value {
        0, 1 as matched => matched + 10      -- bind the matched value
        2..=4 as ranged => ranged + 20       -- bind the ranged value
        else as other => other + 30          -- bind the fallback value
    }
}
```

Each binder exists only in the selected branch.

## Guards

Use a guard when a pattern needs an extra condition:

```nerd
describe :: fn (value: i32) -> string {
    return on value {
        0..=10 as small on small % 2 == 0 => "small even"  -- guarded branch
        0..=10 => "small"
        else => "large"
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
    "look" => prn("You see nothing.")  -- run if command is "look"
    "quit" => prn("Goodbye.")          -- run if command is "quit"
}
```

If no branch matches, nothing happens.
