# Part 6: Branching With `on`

[Manual Index](README.md) | Previous: [Blocks, Scope, And Cleanup](part05-blocks-scope-and-cleanup.md) | Next: [Loops](part07-loops.md)

`on` is Nerd's branching construct. Branching means choosing which expression or
statement to run based on a condition or value. All branch forms use `on`: simple
boolean branches, condition chains, and pattern matching.

## Compile-Time Define Blocks

A string literal directly after `on` selects a block at compile time. This works
inside functions and nested blocks as well as at module scope:

```nerd
on "windows" {
    CopyMemory(destination, source, length)
}

on !"windows" {
    portable_copy(destination, source, length)
}
```

Inactive bodies are removed before names are resolved or types are checked, so
they may safely refer to APIs available only on their selected platform. The
string may name a platform, build mode, architecture, or a key supplied with
`-Dname`.

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
| `in [0..10]` | explicit range membership, excluding the end |
| `in [0..=10]` | explicit range membership, including the end |
| `< 10` | explicit comparison |
| enum variants | covered in Part 8 |
| structural patterns | covered in Part 8 |

Branch bodies can be expressions, blocks, `return`, `break`, or `again`. In
block-form `on`, write each branch on its own line when using comparison
patterns:

```nerd
return on size {
    >= 8 => 8
    >= 4 => 4
    else => 1
}
```

The newline before `>= 4` helps mark it as the next branch head, not as part of
the previous branch result.

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

The bracketed `in` spelling is also available when it reads more clearly:

```nerd
on character {
    in ['0'..='9'] => "digit"
    else => "other"
}
```

Bare range patterns remain supported.

Use `for` when a pattern should compare against a variable that is already in
scope:

```nerd
name := "matt"
details := (42, "matt")

on details {
    (42, for name) => prn($"Hello {name}!")  -- compares with the outer name value
    (42, matched) => prn(matched)            -- binds the tuple field
}
```

## Plex patterns

A plex pattern must mention every field, or explicitly ignore the rest with
trailing `...`. For a plex with fields `x`, `y`, and `name`:

```nerd
on point {
    { x: for wanted_x, y: height, name: _ } => handle(height)
    { x: 0, ... } as matched => handle(matched.y)
    else => fallback()
}
```

`x: for wanted_x` compares with an existing value. `y: height` binds a new
branch-local name; `y` would bind the field as `y`. `name: _` ignores that field.
`...` ignores every field not listed, and `as matched` binds the whole subject.
The rules also apply inside nested plex patterns and named enum payload patterns.
You can name the record type before the braces, such as `Point { x, y, name }`.
This works as a branch head too; the subject type distinguishes it from an enum
variant with a braced payload.

Adding a field makes complete patterns require an update. Patterns ending with
`...` continue to ignore the additional field. Even with `...`, listed fields
must exist and cannot be repeated. An empty `{}` pattern is complete only for
an empty plex; use `{ ... }` to ignore all fields of any plex.

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

## Optional and result extraction

Boolean extraction binds payloads directly:

```nerd
on maybe => [value] {
    use(value)
} else {
    prn("missing")
}

on operation => [value] {
    use(value)
} else [error] {
    report(error)
}
```

When the value after `on` is a bare identifier, omitted binders reuse that name
inside each branch:

```nerd
on operation => {
    use(operation)       -- the successful value
} else {
    report(operation)    -- the error value
}
```

The implicit bindings are branch-local. After the `on`, `operation` again means
the original result. Use `[value]` or `[error]` when a different name is clearer.
For an optional value, the success branch receives the present payload; the
absence branch has no payload and retains the original optional value.

When you only need the successful payload, use a bare failure handler:

```nerd
bound := server.local_url() else [error] {
    report(error)
}
```

The operation runs once, and success produces its value. On failure the handler
runs, then forwards the original error if it falls through, following postfix
`?` return-channel rules. The enclosing function must have a compatible return
channel on paths that propagate. Mutation of the subject does not replace the
captured failure.

Explicit `return`, `break`, `again`, and non-returning calls override implicit
propagation. For example, `server.local_url() else { return 1 }` can appear in
an integer-returning `main`. Handler braces are ordinary statement scopes, so
`break` targets the surrounding loop or expression block. Normal `defer` and
failure-only `undo` cleanup runs on propagation, including handler cleanup.

Optionals use the same form without an error binder: `optional() else { ... }`.
Absence propagates `nil`. Void payloads work too; continuing after a handled
failure requires explicit recovery, such as the lazy `\` operator.

Bare handlers bind below boolean operators and above assignment. Parenthesise
a handler expression used as an unbraced branch of `on`, so its `else` is
distinct from that branch's `else`.

An `on` subject can contain a bare handler without parentheses; `=>` ends
the subject:

```nerd
on shutdown.requested() else [error] {
    eprn($"Shutdown watcher failed: {error}")
    return 1
} => break
```

Here `break` exits the surrounding loop; the handler's `return 1` exits the
enclosing function on failure.

The old `on expression else ...` and `on expression else? ...` shorthand forms
have been removed. Full extraction with `on expression => [value] { ... }
else [error] { ... }` remains available for work in both branches.

Full matching uses the first table for present/success payload patterns.
Optional `else` handles absence; result `else` contains error payload patterns:

```nerd
on operation {
    0    => "zero"
    code => $"code {code}"
} else {
    "timeout" => "try again"
    else      => "failed"
}
```
