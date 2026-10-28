# Chapter 4: Variables and Scope

[Previous: Primitive Types and Expressions](03-Primitive-Types-and-Expressions.md)

[Next: Plexes, Unions, Enums and Tuples](05-Plexes-Unions-Enums-and-Tuples.md)

A variable provides storage whose value can change. Its type is fixed by the
compiler, and each assignment must supply a compatible value. Scope determines
where a name can be used. This chapter introduces variable declarations,
assignment, nested scopes, and blocks that produce values.

The complete programs below return integer exit statuses so you can check their
results with `nerd run`. They do not print those numbers.

## Declaring a variable

Use `:=` to declare a variable whose type is inferred:

```nerd
main :: fn () -> i32 {
    answer := 40
    answer = answer + 2
    return answer
}
```

The literal `40` begins as an untyped integer. Declaring a variable requires a
concrete storage type: in this program, `answer` is inferred as `i32`, and the
initial value becomes an `i32` value of `40`. The assignment replaces that
stored value with `42`, which the function returns.

Compare this with `answer :: 40` from chapter 2. That form preserves the
untyped integer and cannot be reassigned. `answer := 40` introduces mutable
storage with a concrete type. The difference includes both mutability and
whether the integer has been given a concrete type.

Write `name: Type = expression` to choose the type explicitly:

```nerd
main :: fn () -> i32 {
    answer: i32 = 40
    answer = 42
    return answer
}
```

Compare the variable declarations with the `::` binding form:

| Form | Meaning |
| --- | --- |
| `name :: expression` | Immutable binding, with its type inferred. |
| `name := expression` | Variable, with its type inferred. |
| `name: Type = expression` | Variable with an explicit type and initial value. |
| `name: Type` | Variable initialised using its type's default. |

## Variables have concrete types

An untyped number can remain untyped in an immutable binding, but variable
storage needs a concrete type:

```nerd
answer :: 42

main :: fn () -> i32 {
    current := answer
    current += 1
    return current
}
```

`answer` remains an untyped integer. `current` is an `i32` variable, and the
program returns `43`. Initialising a variable from a binding does not change
the original binding's type or value.

Type inference uses the initialiser and the variable's usage. When there is no
other type constraint, an untyped integer becomes `i32` and an untyped float
becomes `f64`. A concrete context can choose another type:

```nerd
get_answer :: fn () -> u64 {
    answer := 42
    return answer
}

main :: fn () -> i32 {
    return get_answer().as(i32)
}
```

The declared return type of `get_answer` supplies context, so its local
`answer` is inferred as `u64`. The cast in `main` converts the returned value
to `i32` for the exit status.

Inference happens while compiling. A variable does not change type as the
program runs. Once its type is determined, assigning an incompatible concrete
type is an error; use `.as(Type)` when you intend a numeric conversion.

## Assignment and compound assignment

Use `=` to update an existing variable. Use `:=` to declare a new one. The
compiler rejects declaring the same name twice in the same scope.

Compound assignments combine an operation with an update:

```nerd
main :: fn () -> i32 {
    total := 10
    total += 11
    total *= 2
    return total
}
```

This returns `42`. For a simple variable, `total += 11` has the same effect as
`total = total + 11`.

| Operators | Update |
| --- | --- |
| `+=`, `-=`, `*=`, `/=`, `%=` | Arithmetic |
| `&=`, `\|=`, `~=` | Bitwise AND, OR, and XOR |
| `<<=`, `>>=` | Bit shifts |
| `&&=`, `\|\|=` | Boolean AND and OR |

Assignment is also an expression, and produces the assigned value:

```nerd
main :: fn () -> i32 {
    first := 0
    second := (first = 21)
    return first + second
}
```

Both variables contain `21`, so this returns `42`. The parentheses make the
assignment inside the initialiser explicit.

Initialising one primitive variable from another copies its value. It does not
make the two names refer to the same mutable storage:

```nerd
main :: fn () -> i32 {
    original := 40
    copy := original
    original += 2
    return original + copy
}
```

This returns `82`: `original` contains `42`, and `copy` still contains `40`.

## Default and undefined initialisation

A declaration with an explicit type can omit the initialiser:

```nerd
main :: fn () -> i32 {
    total: i32
    return total
}
```

This returns `0`. For the primitive types covered so far, numeric variables
default to zero, booleans to `no`, and strings to the empty string. Unlike an
uninitialised C local, `total` has a defined value immediately.

You can explicitly skip initialisation with `undefined`:

```nerd
main :: fn () -> i32 {
    answer: i32 = undefined
    answer = 42
    return answer
}
```

This returns `42`. You must assign a value before reading it. Removing the
assignment makes this program a compile-time error. An operation such as
`answer += 1` also reads the previous value, so it cannot serve as the first
assignment to undefined storage. Prefer ordinary initialisation unless there
is a reason to skip it.

## Local scope

A function body is a scope. Braces introduce nested scopes. A local variable
can be used after its declaration within its scope and within nested scopes:

```nerd
main :: fn () -> i32 {
    total := 40
    {
        extra := 2
        total += extra
    }
    return total
}
```

This returns `42`. The inner block can read and update `total`, which belongs
to the outer scope. Its own variable `extra` is available only inside that
block. Referring to `extra` after the closing brace is an error.

Local variables must be declared before use. The freedom to reorder top-level
definitions described in chapter 2 does not make local variables available
before their declarations. Statements still execute in source order, and a
local declaration initialises its variable when execution reaches it.

## Shadowing

A nested scope can introduce a new variable with the same name as an outer
variable. The inner name hides, or *shadows*, the outer name within that scope:

```nerd
main :: fn () -> i32 {
    value := 40
    result := 0
    {
        value := 2
        result = value
    }
    return value + result
}
```

This returns `42`. Inside the block, `value` names the inner variable containing
`2`. Outside it, `value` once again names the outer variable containing `40`.
The assignment to `result` updates the outer variable because the block does
not declare another `result`.

The distinction between declaration and assignment matters here: `value := 2`
creates an inner variable, while `value = 2` would update the visible outer
variable. Use shadowing when the distinction is clear to readers.

## Top-level variables

A variable declared outside a function is available to functions in its module.
Its storage persists between calls:

```nerd
counter: i32 = 0

increment :: fn () {
    counter += 1
}

main :: fn () -> i32 {
    increment()
    increment()
    return counter
}
```

This returns `2`. A local variable belongs to an individual function call;
the top-level `counter` is shared by these calls to `increment`. Prefer local
variables when the state is needed only within one call.

## Expression blocks

An ordinary block groups statements. An expression block lets several
statements work together to produce a value. It starts with `${` and uses
`break expression` to supply the result:

```nerd
main :: fn () -> i32 {
    answer := ${
        left := 20
        right := 22
        break left + right
    }
    return answer
}
```

This returns `42`. The expression block introduces a scope for `left` and
`right`. `break left + right` exits that block and makes its value `42`, which
initialises `answer`. The local names remain inside the block; the resulting
value is available outside it through `answer`.

The last expression is not implicitly the block's result. Write `break` to
produce the value. `return` has a different target: it exits the enclosing
function, even when written inside an expression block.

Expression blocks can appear wherever their resulting value is needed. Here,
one supplies the function's return value directly:

```nerd
main :: fn () -> i32 {
    return ${
        answer := 40
        answer += 2
        break answer
    }
}
```

## Naming an expression block

For nested expression blocks, a label makes the target explicit. Write
`$name { ... }` to label a block, and `break $name value` to leave it:

```nerd
main :: fn () -> i32 {
    answer := $outer {
        $inner {
            break $outer 42
        }
        break 0
    }
    return answer
}
```

This returns `42`. The labelled `break` leaves `outer` directly, including its
nested `inner` block. The later `break 0` is not reached. Without a label,
`break` targets the nearest enclosing expression block in examples like these.
Labels use a separate `$name` spelling to distinguish them from ordinary value
names.

## Functions that never return

The type `!` means evaluation never produces a value. A function returning `!`
terminates or otherwise never returns to its caller. This differs from `void`,
whose functions can finish normally without returning a value.

The built-in `abort` function returns `!`:

```nerd
require_positive :: fn (value: i32) -> i32 {
    on value <= 0 => abort("expected a positive value")
    return value
}

main :: fn () -> i32 {
    return require_positive(42)
}
```

This returns `42`. Passing zero would terminate the program instead. The
compiler knows that a call returning `!` cannot fall through, so it can also
appear where a value is otherwise required:

```nerd
main :: fn () -> i32 {
    allowed := yes
    answer := on allowed => 42 else abort("not allowed")
    return answer
}
```

This returns `42`. The abort branch need not manufacture an integer. Do not
expect normal scope cleanup from process termination. The prefix `!condition`
operator still means logical negation; `!` in a type position is the never type.

## Unused local names

Nerd enforces unused local names as a compile-time rule. A local variable or
binding that is never read must have a name beginning with `_`; leaving off
the prefix is a compiler error. Conversely, reading a name beginning with `_`
is also a compiler error. Assigning a new value does not count as reading the
variable.

Prefix a deliberately unused name with `_`, or use the bare `_` to discard a
value without giving it a usable name:

```nerd
main :: fn () {
    _scratch := 10
    _ := 20 + 22
}
```

Here, `_scratch` must remain unread. Rename it to `scratch` when you start
using its value. The discard name `_` can be used repeatedly within the same
scope.

## Using or discarding function results

A function's return value must be used or explicitly discarded. Calling a
function that returns a value as a standalone statement is a compiler error.
You can use the result in an expression, return it to the caller, assign it to
a variable, or explicitly discard it with `_`:

```nerd
get_answer :: fn () -> i32 {
    return 42
}

main :: fn () -> i32 {
    answer := get_answer()
    _ := get_answer()
    return answer
}
```

This returns `42`. The first call supplies a value that is read by the
`return` statement. The second call explicitly discards its result. Replacing
`_ := get_answer()` with just `get_answer()` is an error.

Assigning a result to an ordinary local does not bypass the unused-name rule:
that local must subsequently be read. Use an underscore-prefixed name or `_`
when you intend to leave the result unused.

Functions that return no value, such as `prn`, can be called as standalone
statements without a discard binding.
