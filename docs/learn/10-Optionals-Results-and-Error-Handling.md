# Chapter 10: Optionals, Results and Error Handling

[Previous: Branching and Pattern Matching](09-Branching-and-Pattern-Matching.md)

[Next: Loops with `for`](11-Loops.md)

Some operations cannot always produce a useful value. An item may be absent,
or an operation may fail with information explaining why. Nerd represents
these outcomes in return types, so callers can handle them explicitly.

| Type | Outcomes |
| --- | --- |
| `?T` | A value of type `T`, or absence. |
| `T\E` | A successful value of type `T`, or an error of type `E`. |

These are ordinary values, rather than exceptions thrown through the call
stack. A caller chooses whether to inspect an outcome, supply a default, or
return the failure to its own caller. As before, examples either print strings
or return integer exit statuses.

## Optional values

Write `?T` when absence is a normal possibility. Where an optional is expected,
a compatible `T` value constructs a present value, and `nil` constructs absence:

```nerd
find_answer :: fn (available: bool) -> ?i32 {
    on available => return 42
    return nil
}

main :: fn () {
    on find_answer(no) => prn("found") else prn("missing")
}
```

This prints `missing`. The return type is an optional signed 32-bit integer.
Returning `42` constructs presence; returning `nil` means there is no integer.
The condition uses the optional's **truthiness**: a present optional is true,
and an absent optional is false. There is no need to compare it with `nil`
before choosing a branch.

In a declaration, write the optional type explicitly when it is needed:
`answer: ?i32 = 42` holds a present integer. `answer := 42` instead declares
an ordinary integer variable. An optional variable declared without an
initializer starts absent, as in `answer: ?i32`.

## Extracting an optional with `on`

Use `on optional => [name] ... else ...` to handle presence and absence.
The bracketed name binds the contained value in the present branch:

```nerd
main :: fn () -> i32 {
    answer: ?i32 = 42
    return on answer => [number] number else 0
}
```

This returns `42`. `answer` has type `?i32`, while `number` has type `i32`.
The absent branch has no payload to bind. The subject is evaluated once,
and only the selected branch runs. Braced statement bodies work here too.

Presence is independent of the contained value. Zero, an empty string, and
`no` can all be present values:

```nerd
main :: fn () {
    setting: ?bool = no
    on setting => [enabled] {
        on enabled => prn("enabled") else prn("present but disabled")
    } else {
        prn("missing")
    }
}
```

This prints `present but disabled`. The outer `on` tests whether `setting`
contains a value. The inner `on` tests the extracted boolean itself.

## Results and error values

Write `T\E` when failure should carry an error of type `E`. An expression
compatible with `T` constructs success when that result type is expected.
Postfix `!` constructs the error outcome:

```nerd
MathError :: enum { ZeroDivisor NegativeDivisor }

divide :: fn (numerator: i32, divisor: i32) -> i32\MathError {
    return on divisor {
        0 => MathError.ZeroDivisor!
        < 0 => MathError.NegativeDivisor!
        else => numerator / divisor
    }
}

main :: fn () -> i32 {
    return on divide(84, 2) => [number] number else 0
}
```

This returns `42`. A successful call contains an `i32`; a failed call contains
a `MathError`. The `!` after an error value selects the error outcome, even
when the success and error types could otherwise accept the same value.
Results also have truthiness: success is true and failure is false. The `on`
in `main` tests that outcome, then binds the successful payload as `number`.

The error type is your choice: it can be an enum, string, integer, or a plex
carrying details. There is no required base error type. Here the enum makes
the possible errors explicit, using types already covered in Chapter 5.

An error injection needs a result type from context. For example,
`failed: i32\MathError = MathError.ZeroDivisor!` supplies that context.
Do not expect `failed := MathError.ZeroDivisor!` to infer a success type.

## Extracting success and error values

A result has a payload in either outcome. Bind the success after `=>` and
the error after `else`:

```nerd
main :: fn () {
    outcome: string\string = "not found"!
    on outcome => [text] {
        prn(text)
    } else [message] {
        prn(message)
    }
}
```

This prints `not found`. Both payload types happen to be `string`; postfix
`!` distinguishes an error from a successful string. Omitting an unwanted
binder is allowed. Optional absence, by contrast, cannot have an error binder
because it carries no error value.

If the subject is a bare identifier, omitting its bracketed binder reuses
that identifier inside the selected branch:

```nerd
main :: fn () -> i32 {
    outcome: i32\string = 42
    answer := on outcome => outcome else 0
    return answer
}
```

This returns `42`. In the success branch, `outcome` names the contained
`i32`; in an error branch it would name the error string. After the `on`,
the outer `outcome` still has its original result type. The same shorthand
works for optionals, but their absent branch has no replacement payload.
Use explicit bracketed names when that makes the changing types clearer.

## Truthiness and checking for failure

Booleans, optionals, and results have truthiness:

| Value | True when | False when |
| --- | --- | --- |
| `bool` | `yes` | `no` |
| `?T` | A value is present | The value is absent (`nil`) |
| `T\E` | The outcome is success | The outcome is an error |

For optionals and results, truthiness tests the wrapper's outcome, rather than
the payload. A present `?bool` containing `no` is true, as is a successful
`bool\E` containing `no`. The same applies to a contained zero or empty string.
Use `on outcome => ... else ...` to branch directly on this truthiness; extract
the payload first when you want to test the contained value instead.

Prefix `!` tests absence or failure. It is true for an absent optional or a
failed result, and false for a present optional or successful result:

```nerd
main :: fn () -> i32 {
    missing: ?i32
    present: ?i32 = 0
    failed: i32\string = "failed"!
    succeeded: i32\string = 0
    return ((!missing && !failed) && !(!present) && !(!succeeded)).as(i32)
}
```

This returns `1`. The two zero values are present or successful; they do not
count as failure. For optionals, `value == nil` and `value != nil` also test
absence and presence.

Placement matters: prefix `!value` checks failure, while postfix `error!`
constructs an error. Prefix `!` on an ordinary boolean remains boolean
negation, as in earlier chapters.

The current compiler does not yet accept optionals and results everywhere a
`bool` is required. Direct short-form `on` and prefix `!` accept them, but
`&&`, `||`, condition chains, guards, and boolean parameters or assignments
require actual `bool` values. `!!outcome` converts the truthiness test into a
`bool`: the first `!` tests absence or failure, and the second negates that
test. For example, combine two outcomes with `!!first && !!second`.

## Supplying a default with `\`

The expression operator `\` extracts a present or successful value, or
evaluates its right operand as a fallback on absence or failure:

```nerd
choose :: fn (available: bool) -> ?i32 {
    on available => return 20
    return nil
}

fallback :: fn () -> i32 {
    prn("using default")
    return 22
}

main :: fn () -> i32 {
    first := choose(yes) \ fallback()
    second := choose(no) \ fallback()
    return first + second
}
```

This returns `42` and prints `using default` once. The left operand is
evaluated once, and the fallback runs only when needed. With a result, the
error is discarded; use `on` when you need to inspect it.

When recovery needs several statements, use an expression block as the
fallback. `break` supplies the replacement value:

```nerd
read_number :: fn () -> i32\string {
    return "number unavailable"!
}

main :: fn () -> i32 {
    number := read_number() \ ${
        prn("using default")
        break 40
    }
    return number + 2
}
```

This prints `using default` and returns `42`. The block runs only on failure;
on success, `number` receives the successful payload directly. `break 40`
exits the expression block, then execution continues after the declaration.
It does not return from `main`. The same form works for optional absence.

The extracted value and fallback must have compatible success types. You can
chain choices, such as `first_choice() \ second_choice() \ 42`. Chains group
from the right and stop at the first present or successful value.

Use parentheses for arithmetic inside a fallback: `maybe \ (2 * 3)`.
Without them, `maybe \ 2 * 3` means `(maybe \ 2) * 3`, because `\` binds
more tightly than multiplication. In a type, the same character separates
success and error types; in an expression it supplies a fallback.

## Pattern matching optional values

Extract the present value first, then match it using the patterns from
Chapter 9. The outer `else` handles absence:

```nerd
describe :: fn (value: ?i32) -> string {
    return on value => [number] {
        return on number {
            0 => "zero"
            else => "present"
        }
    } else {
        return "missing"
    }
}

main :: fn () {
    prn(describe(0))
    prn(describe(nil))
}
```

This prints `zero` followed by `missing`. The inner `else => "present"`
handles other integers, while the outer `else` handles the optional's absence.
The absent branch uses `return` to exit `describe`; it is a statement body.

## Matching success and error details

For a result, extract each payload and match its details in a nested `on`:

```nerd
Failure :: enum { Missing Invalid { code i32 } }

describe :: fn (outcome: i32\Failure) -> string {
    on outcome => [number] {
        return on number {
            0 => "zero"
            > 0 => "positive"
            else => "negative"
        }
    } else [problem] {
        return on problem {
            Missing => "missing"
            Invalid { code: > 0 } => "positive error code"
            Invalid { ... } => "other invalid code"
        }
    }
}

main :: fn () {
    prn(describe(42))
    prn(describe(Failure.Invalid { code: 7 }!))
}
```

This prints `positive` followed by `positive error code`. Each table uses
the pattern rules from Chapter 9. The error table already matches a
`Failure`, so its patterns do not need an error-channel marker.

## Error patterns in one table

Alternatively, suffix a pattern with `!` to match the error outcome in a
single table. Patterns without that suffix match successful payloads:

```nerd
Failure :: enum { Missing Invalid { code i32 } }

score :: fn (outcome: i32\Failure) -> i32 {
    on outcome {
        Failure.Missing! => return 0
        Failure.Invalid { code }! => return code
        number => return number
    }
    return 0
}

main :: fn () -> i32 {
    return score(Failure.Invalid { code: 42 }!)
}
```

This returns `42`. `Failure.Invalid { code }!` checks the error outcome,
matches the error variant, and binds its `code`. `_!` can ignore any error
payload. This suffix selects an error outcome only when the subject is a
result; a plain `Failure` value is matched without `!`.

These wrappers use dedicated optional and result syntax. They do not expose
ordinary enum constructors such as `Some`, `None`, `Ok`, or `Err`.

## Propagating a failure with `?`

Postfix `?` extracts presence or success. On absence or failure it immediately
returns that outcome from the enclosing function:

```nerd
Failure :: enum { Missing }

read_number :: fn (available: bool) -> i32\Failure {
    on !available => return Failure.Missing!
    return 20
}

calculate :: fn (available: bool) -> i32\Failure {
    number := read_number(available)?
    return number * 2 + 2
}

main :: fn () -> i32 {
    on calculate(no) => [_number] {
        return 1
    } else {
        prn("failure forwarded")
    }
    return on calculate(yes) => [answer] answer else 0
}
```

This prints `failure forwarded` and returns `42`. In `calculate`, `number`
is an `i32`. On a failed read, its initializer returns the error immediately;
the arithmetic after it does not run. The enclosing function must have a
compatible error return type. An ordinary `i32` return type cannot propagate
this error.

The same operator forwards optional absence:

```nerd
find_number :: fn (available: bool) -> ?i32 {
    on available => return 40
    return nil
}

calculate :: fn (available: bool) -> ?i32 {
    number := find_number(available)?
    return number + 2
}

main :: fn () -> i32 {
    return calculate(no) \ 42
}
```

This returns `42` from the fallback. `calculate(no)` forwards `nil` before
reaching its arithmetic. Propagation forwards the failure; it does not log
it, turn it into a default, or resume at the failed expression.

## Running a handler before propagation

Use `expression else [error] { ... }` to run code on failure while extracting
the successful payload:

```nerd
Failure :: enum { Missing }

read_number :: fn () -> i32\Failure {
    return Failure.Missing!
}

calculate :: fn () -> i32\Failure {
    number := read_number() else [problem] {
        on problem {
            Missing => prn("missing number")
        }
    }
    return number + 2
}

main :: fn () -> i32 {
    return calculate() \ 42
}
```

This prints `missing number` and returns `42`. If the handler reaches its
end, the original error is returned from `calculate`, following the same
return-type rules as `?`. The handler does not fall through to `number + 2`.
The subject is evaluated once, and propagation uses its captured failure.

For an optional, write `expression else { ... }` without an error binder;
falling through forwards `nil`. With a bare identifier as subject, an
omitted result error binder reuses that identifier for the error inside the
handler, just as in `on` extraction.

An explicit `return` in a handler exits the enclosing function instead of
forwarding the original error. It can return a successful value or translate
the error to another type:

```nerd
ReadError :: enum { Missing }
LoadError :: enum { Unavailable }

read_number :: fn () -> i32\ReadError {
    return ReadError.Missing!
}

load_number :: fn () -> i32\LoadError {
    number := read_number() else {
        return LoadError.Unavailable!
    }
    return number
}

main :: fn () -> i32 {
    return on load_number() => [number] number else [problem]
        on problem { Unavailable => 42 }
}
```

This returns `42`. The handler maps a `ReadError` to a `LoadError`. Writing
`return 42` there would instead return success from `load_number`. To supply
a replacement and continue in that function, use `\` or an `on` expression.

Handler braces are statement scopes, so a `break` inside one keeps the
expression-block target it would have outside the handler. Parenthesise a
bare handler used as an unbraced branch of a short `on`, so its `else` is
distinct from the `on` branch's own `else`.

## Successful operations with no payload

Use `?void` or `void\E` when success carries no data. A bare `return`, or
reaching the end of the function, means success:

```nerd
Failure :: enum { Unavailable }

check :: fn (ready: bool) -> void\Failure {
    on !ready => return Failure.Unavailable!
    return
}

prepare :: fn () -> void\Failure {
    check(yes)?
    prn("ready")
}

main :: fn () {
    on prepare() => prn("success") else prn("failure")
}
```

This prints `ready` followed by `success`. `check(yes)?` either completes
successfully with no value to bind, or returns its error from `prepare`.
For `?void`, `return nil` represents absence. A void fallback can also handle
failure and continue, as in `check(no) \ prn("not ready")`.

## Cleanup on every scope exit with `defer`

`defer` registers a statement to run when its current scope exits, including
through `return`, an expression block's `break`, or failure propagation.
Actions run in reverse registration order:

```nerd
main :: fn () {
    defer prn("first registered")
    defer prn("second registered")
    prn("body")
}
```

The output is:

```text
body
second registered
first registered
```

Only actions whose registration was reached can run. Write `defer { ... }`
for several cleanup statements. Deferred code reads local values when cleanup
runs, rather than saving a copy of those values at registration.

Register resource cleanup just after acquiring the resource:

```nerd
Failure :: enum { Unavailable }

read_value :: fn (available: bool) -> i32\Failure {
    storage := arena(4096)
    defer storage.done()
    on !available => return Failure.Unavailable!
    number := storage.alloc[i32]()
    number^ = 42
    return number^
}

main :: fn () -> i32 {
    return (read_value(no) \ 0) + (read_value(yes) \ 0)
}
```

This returns `42`. Each call releases its arena on exit, including the call
that returns an error. The successful integer is read before cleanup runs.
A pointer into that arena would become invalid when the function exits;
`defer` does not extend an allocation's lifetime.

## Cleanup only on failure with `undo`

Use `undo` while building a resource that a successful call transfers to its
caller. It registers cleanup for a return carrying absence or an error,
including one caused by `?`:

```nerd
Failure :: enum { Unavailable }

create_values :: fn (ready: bool) -> [..]i32\Failure {
    values: [..]i32
    undo {
        prn("discarding partial array")
        values.free()
    }
    values.push(20)
    on !ready => return Failure.Unavailable!
    values.push(22)
    return values
}

main :: fn () -> i32 {
    on create_values(no) => [_values] {
        return 1
    } else {
        prn("creation failed")
    }
    values := create_values(yes) else {
        return 0
    }
    defer values.free()
    return values[0] + values[1]
}
```

This prints `discarding partial array` followed by `creation failed`, and
returns `42`. Failure releases the partially built array. Success skips the
undo action, so the caller receives the array and takes responsibility for
freeing it.

`undo` accepts a statement or block, requires an enclosing optional- or
result-returning function, and reads local values when cleanup runs. Active
undo and defer actions run together in reverse registration order on failure.
On a successful return, only ordinary defers run. Leaving an inner scope
normally discards that scope's undo actions; a later failure does not revive
them. Register cleanup only after an acquisition has succeeded.

An undo statement may follow another statement on the same line. For example,
`acquire()? undo release()` registers `release()` only if `acquire()` succeeds;
if the `?` returns a failure, execution never reaches the registration.

Undo cleanup cannot return or propagate another failure, or register further
undo actions. Process termination and runtime panics are not failure returns
and do not trigger undo as error handling.

## Return types for the `main` function

The function bound to `main` is the program's entry point. Its return value
determines the process exit status. Nerd supports these return types for
`main`:

| Return type | Exit status |
| --- | --- |
| No return value (`void`) | `0` when the function finishes. |
| `i32` | The returned integer. |
| `?void` | `0` on success; `1` on absence. |
| `?i32` | The present integer; `1` on absence. |
| `void\Error` | `0` on success; `1` on error, with a message on stderr. |
| `i32\Error` | The successful integer; `1` on error, with a message on stderr. |

Plain integer return types of other widths are also accepted. When `main`
returns an optional or result, its present or successful payload type must be
`void` or `i32`.

The `main` function can return `?void` when initialization may fail:

```nerd
initialize :: fn () -> ?void {
    return
}

main :: fn () -> ?void {
    initialize()?
    prn("ready")
}
```

This prints `ready` and exits with status `0`. Reaching the end of this
`main`, or using bare `return`, succeeds. Returning `nil` or propagating
absence exits with status `1`, after registered cleanup runs.

### Optional integer exit statuses

When `main` returns `?i32`, presence supplies an explicit exit status:
`return 0` exits with `0`, and `return 42` exits with `42`. Absence supplies
the fallback status `1`:

```nerd
main :: fn () -> ?i32 {
    return nil
}
```

This exits with status `1` without printing an error message. A useful
convention is to use `0` for success, `1` for a generic or unknown error, and
other nonzero statuses for specific failures defined by your program. On
that convention, `nil` means failure without a more specific exit status.
Returning a present `1` produces the same process exit status as returning
`nil`; the operating system does not receive the optional's presence tag.

A present integer is used as the status even though the optional itself is
truthy. Presence of `42` therefore exits with `42`, rather than indicating
successful process completion. Propagating optional absence with `?` also
uses the fallback status `1`.

### Results with error messages

In `T\Error`, `Error` stands for an error **type**, rather than a particular
error value. It can be an enum, string, integer, or another suitable value
type; an error outcome carries a value of that type. For a result-returning
`main`, the error type must implement the `Display` trait, which supplies its
textual representation. This is required even if the program only returns
success. [Chapter 13](13-Methods-and-Traits.md) covers traits and how to
implement them.
The `string` type already implements `Display`, so it can be used directly:

```nerd
main :: fn () -> i32\string {
    return "could not read configuration"!
}
```

This exits with status `1` and writes `Error: could not read configuration`
followed by a newline to standard error. A successful `return 42` would instead
exit with `42` and produce no automatic error message. For `void\Error`, a
bare `return` or reaching the end succeeds with status `0`.

Explicit error returns and errors propagated with `?` run applicable `undo`
and `defer` cleanup before the error returned from `main` is displayed. A result
error always uses status `1`; an integer error payload is displayed as an
error message, rather than used as the exit status. Use a successful `i32`
payload when you want to choose the exit status yourself.

## Choosing how to handle an outcome

| Need | Form |
| --- | --- |
| Inspect absence or failure without extracting | `!outcome`; optionals also support comparison with `nil`. |
| Extract and handle both outcomes | `on outcome => [value] ... else ...`. |
| Match values or error details | Extract with `on`, then match the payload; or use result error patterns ending in `!`. |
| Use a fallback and continue | `outcome \ fallback`. |
| Forward absence or error immediately | `outcome?`. |
| Run a handler, then forward failure | `outcome else [error] { ... }`; omit the binder for optional absence. |
| Clean up on every scope exit | `defer statement`. |
| Clean up only on a failure return | `undo statement`. |

Function results still follow Chapter 4's use-or-discard rule. Discarding an
optional or result explicitly does not report its failure or propagate it.
