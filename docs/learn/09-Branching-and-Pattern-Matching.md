# Chapter 9: Branching and Pattern Matching with `on`

[Previous: Arenas](08-Arenas.md)

Nerd uses `on` to choose which code to run. It covers the jobs of C's `if`,
conditional operator, and `switch`, and extends them with patterns that can
inspect values and extract their parts.

This chapter covers three runtime forms:

| Form | What it selects |
| --- | --- |
| `on condition => expression else expression` | A branch based on one boolean condition. |
| `on { condition => expression ... }` | The first branch whose condition is true. |
| `on value { pattern => expression ... }` | The first branch whose pattern and optional guard match the value. |

An `on` can run statements or produce a value. As in earlier chapters, the
examples either print strings or return integer exit statuses.

## A boolean condition

The shortest form runs an expression when a condition is true:

```nerd
main :: fn () {
    temperature := 25
    on temperature > 20 => prn("warm")
}
```

This prints `warm`. If the condition is false, the statement does nothing.
The condition must have type `bool`; an integer is not implicitly a boolean.

Add `else` to choose between two expressions:

```nerd
describe :: fn (value: i32) -> string {
    return on value > 0 => "positive" else "not positive"
}

main :: fn () {
    prn(describe(42))
}
```

This prints `positive`. Only the selected expression is evaluated. Here the
`on` supplies the string returned by `describe`, much as a conditional
expression does in C. Both branches must produce compatible types.

## Branch bodies and blocks

Use braces when a branch needs several statements:

```nerd
main :: fn () -> i32 {
    enabled := yes
    answer := 0
    on enabled => {
        answer = 40
        answer += 2
    } else {
        answer = 1
    }
    return answer
}
```

This returns `42`. Each branch body has its own local scope. A name declared
inside one body is unavailable in the other body or after the `on`.

A braced statement body does not implicitly yield its last expression. Use
the expression blocks from Chapter 4 when a branch needs several statements
to calculate a value:

```nerd
main :: fn () -> i32 {
    enabled := yes
    answer := on enabled => ${
        base := 40
        break base + 2
    } else 0
    return answer
}
```

This returns `42`. `break` supplies the expression block's value. A `return`
inside a branch instead exits the enclosing function:

```nerd
answer :: fn (enabled: bool) -> i32 {
    on !enabled => return 0
    return 42
}

main :: fn () -> i32 {
    return answer(yes)
}
```

This returns `42`. There is no fallthrough from one selected branch into the
next, and no `break` is needed to end an `on` branch.

## A chain of conditions

When each case has a different condition, leave out the subject and put the
conditions inside braces:

```nerd
describe :: fn (value: i32) -> string {
    return on {
        value < 0 => "negative"
        value == 0 => "zero"
        value < 10 => "small"
        else => "large"
    }
}

main :: fn () {
    prn(describe(7))
}
```

This prints `small`. Conditions are tested in source order. The first true
condition selects its branch, and later conditions are not evaluated. `else`
handles the case where none of the conditions is true.

Use this form where you would use an `if`/`else if` chain in C. A condition
chain that produces a value needs `else` so there is a result even when every
condition is false.

## Matching one value

Put an expression after `on` to match its value against a set of patterns:

```nerd
score :: fn (value: i32) -> i32 {
    return on value {
        < 0 => -1
        0 => 0
        1, 2 => 10
        >= 10 => 42
        else => 20
    }
}

main :: fn () -> i32 {
    return score(12)
}
```

This returns `42`. The expression after `on`, called the **subject** or
**scrutinee**, is evaluated once. Branches are tested in source order, and
only the first matching branch runs. `1, 2` means either pattern selects the
same body. Use `else` for the fallback and place it last.

A value comparison pattern can start with an operator such as `<`, `>=`, or
`==`. The operator is optional: when omitted, `==` is assumed. For example,
`0 => 0` means the same as `== 0 => 0`, and `1, 2` means `== 1, == 2`.

Write each branch on its own line, especially when its pattern begins with a
comparison operator. This separates the next branch's pattern from the
previous branch's expression.

Literal patterns also work with booleans and strings. For example,
`"quit" => prn("Goodbye")` matches the string's contents.

## Comparisons and ranges

Patterns can express comparisons and integer ranges:

| Pattern | Matches |
| --- | --- |
| `42` or `== 42` | A value equal to `42`. |
| `!= 42` | A value other than `42`. |
| `< 10`, `<= 10`, `> 10`, `>= 10` | The corresponding comparison with `10`. |
| `0..10` | Integers from `0` up to, but excluding, `10`. |
| `0..=10` | Integers from `0` through `10`, including both ends. |
| `in [0..10]` | The same exclusive range as `0..10`. |
| `in [0..=10]` | The same inclusive range as `0..=10`. |

Character literals are integers, so they can be range boundaries too:

```nerd
classify :: fn (character: u8) -> string {
    return on character {
        in ['0'..='9'] => "digit"
        'a'..='z' => "lowercase letter"
        else => "other"
    }
}

main :: fn () {
    prn(classify('7'))
}
```

This prints `digit`. These are matching patterns, rather than array literals
or slice operations. The inclusive `..=` spelling includes its upper bound.

## Binding the subject and adding a guard

Write `as name` after a pattern list to bind the subject in that branch.
A second `on` introduces a **guard**: an additional boolean condition that
must be true before the branch is selected.

```nerd
score :: fn (value: i32) -> i32 {
    return on value {
        0, 1 as small => small + 10
        2..=10 as small on small % 2 == 0 => small + 34
        2..=10 as small => small
        else as other => other
    }
}

main :: fn () -> i32 {
    return score(8)
}
```

This returns `42`. The range matches `8`, and the guard confirms that it is
even. If a pattern matches but its guard is false, matching continues with
the next branch. The guard is evaluated only after its pattern matches.

The binder is available in the guard and body of its own branch. The two
names spelled `small` above belong to separate scopes. `else as other`
binds the subject in the fallback branch.

A bare name can also bind the subject directly: a branch such as
`captured on captured > 0 => captured` binds the subject, then tests it with
a guard. It does not compare against an outer name spelled `captured`.

## Tuple patterns and ignored values

A tuple pattern matches its elements by position. A name in an element
position binds that element; `_` ignores it:

```nerd
sum :: fn (pair: (i32, i32)) -> i32 {
    return on pair {
        (0, _) => 0
        (left, right) => left + right
        else => 0
    }
}

main :: fn () -> i32 {
    return sum((20, 22))
}
```

This returns `42`. The tuple pattern must have the same number of elements
as the tuple it matches. Nested patterns can test some elements and bind
others in the same branch.

Use `else` for a whole-match fallback. The standalone branch `_ => ...` is
rejected; `_` is useful inside structural patterns, as in `(0, _)` above.
The usual unused-name rules apply to pattern binders too: ignore an unwanted
element with `_`, or prefix a deliberately unused name with `_`.

## Comparing against an existing value

A bare name in a binding position introduces a new branch-local name; it
does not compare against an outer variable with that spelling. Write
`for expression` to compare against an existing value instead:

```nerd
main :: fn () -> i32 {
    wanted := "nerd"
    details := (20, "nerd")
    return on details {
        (number, for wanted) => number + 22
        else => 0
    }
}
```

This returns `42`. The second tuple element must equal the outer `wanted`.
Writing `(number, wanted)` would instead bind that element to a new name.
Here `for` introduces a value pattern; it does not repeat any code.

## Plex patterns

A plex pattern must mention every field exactly once, or end with `...` to
explicitly ignore the remaining fields. Use `field: _` to ignore one named
field:

```nerd
Point :: plex {
    x i32
    y i32
    label string
}

score :: fn (point: Point) -> i32 {
    return on point {
        { x: 0, y, label: _ } => y
        { x, y, ... } => x + y
        else => 0
    }
}

main :: fn () -> i32 {
    point := Point { x: 20, y: 22, label: "answer" }
    return score(point)
}
```

This returns `42`. The first pattern requires `x` to be zero, binds `y`, and
explicitly ignores `label`. It mentions all three fields. The second pattern
binds `x` and `y`, then uses `...` to ignore `label` and any fields added later.
Without `label: _` or `...`, these patterns would be rejected for missing a field.

A shorthand field such as `y` means `y: y`; write `y: vertical` to give its
binder another name. The subject supplies the plex type. Fields can appear in
any order.

`{ ... }` ignores every field without binding any of them. An empty `{}`
pattern is complete only for an empty plex. The `...` marker must be last,
although a trailing comma is allowed. Unknown or duplicate fields are errors
even when `...` is present. Each nested plex pattern follows the same rules.

Complete patterns make the compiler report newly added fields that need
attention. Use `...` when you deliberately want additional fields to remain
unconstrained. Unlike `...` in a plex literal, pattern `...` does not initialize
omitted fields or compare them with their defaults.

## Matching enum variants

The subject's enum type supplies the context for variant names, so the
qualified spelling is optional:

```nerd
Colour :: enum { Red Green Blue }

name :: fn (colour: Colour) -> string {
    return on colour {
        Red => "red"
        Green => "green"
        Colour.Blue => "blue"
    }
}

main :: fn () {
    prn(name(Colour.Green))
}
```

This prints `green`. All three variants are covered, so this value-producing
match needs no `else`.

## Extracting enum payloads

Use a variant's constructor shape as a pattern to inspect and bind its payload:

```nerd
Reading :: enum {
    Missing
    Value(i32)
    Pair(i32, i32)
}

score :: fn (reading: Reading) -> i32 {
    return on reading {
        Missing => 0
        Value(number) => number
        Pair(left, right) => left + right
    }
}

main :: fn () -> i32 {
    return score(Reading.Pair(20, 22))
}
```

This returns `42`. `Value(number)` binds an `i32`, and `Pair(left, right)`
binds two `i32` values. Each payload is accessed only in the branch that
matches its variant. This is how to extract the payloads introduced in
Chapter 5; an enum value does not expose them through `.0` or `.field_name`.

Named payloads use braced patterns:

```nerd
Event :: enum {
    Closed
    Resized { width i32 height i32 }
}

score :: fn (event: Event) -> i32 {
    return on event {
        Resized { width: horizontal, height } => horizontal + height
        Closed => 0
    }
}

main :: fn () -> i32 {
    return score(Event.Resized { width: 20, height: 22 })
}
```

This returns `42`. `width: horizontal` renames the bound field, while `height`
uses shorthand. Named payload patterns follow the same field-coverage rules
as plex patterns: mention every field, or use trailing `...`. For example,
`Resized { width, ... }` binds `width` and ignores `height`. A branch-level
`as whole` would bind the whole `Event`, rather than one of its payload fields.

## Nested patterns and alternatives

Patterns can inspect compound values inside other compound values. Multiple
patterns can share a body when they introduce no payload binders:

```nerd
Key :: enum { Q Escape Other }
Event :: enum {
    Press { key Key }
    Closed
}

is_exit :: fn (event: Event) -> bool {
    return on event {
        Press { key: Q }, Press { key: Escape } => yes
        Closed => yes
        else => no
    }
}

main :: fn () -> i32 {
    return is_exit(Event.Press { key: Escape }).as(i32)
}
```

This returns `1`. The first branch requires `Press`, then tests the enum
stored in its `key` field. Put alternatives that extract payloads into
separate branches so each body's bound values are unambiguous.

## Complete matches and partial statements

A value-producing `on` must produce a compatible value on every path that
continues after it. A branch that returns from the enclosing function does
not need to produce a value for the `on` itself.

Use `else` when the compiler cannot establish complete coverage. Boolean
matches covering `yes` and `no`, and enum matches covering every variant,
can omit it. Condition chains require `else` when producing a value. For
tuples, plexes, numeric ranges, and other matches, include a fallback rather
than relying on the compiler to prove that your patterns cover every value.

To handle an enum variant completely, leave its payload unrestricted with
binders or `_`. A payload test or guard may fail, so provide another branch
for that variant or an `else`.

A statement-form match may deliberately handle only some cases:

```nerd
main :: fn () {
    command := "look"
    on command {
        "look" => prn("You see a doorway.")
        "quit" => prn("Goodbye.")
    }
}
```

This prints `You see a doorway.` An unmatched command would do nothing.
When an `on` produces a value, use that value or discard it explicitly,
following the same rules as other expressions and function results.
