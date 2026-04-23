-- functions

With constant bindings, a symbol is an alias of a function:

```
foo :: fn(a: i32, b: i32) -> i32 { return a + b }
foo(1, 2) // 3
```

However, with variable bindings, the variable contains a pointer to the function:

```
pfoo := fn(a: i32, b: i32) -> i32 { return a + b }
```

you can also do:

```
pfoo := foo
```

-- `on` keyword

This replaces all conditional keywords.  The general syntax is:

```
on <condition> {
    <value>[.. <value>] => <expr>,
    ...
    else => <expr>,
}
```

The `on` condition should have 100% coverage of all possible values, otherwise
the compiler will error.  The `else` case is optional if the condition is
already exhaustive.  Also note, ranges are supported with `..<` syntax, and the
right-hand value is exclusive.  Also `..=` is supported for inclusive ranges.

For simple `if` statements, the syntax is:

```
on <condition> => <expr> [else <expr>]

-- e.g.
on x > 0 => "positive"
else => "non-positive"
```

An `on` statement is an expression and can be assigned to a lvalue.  This means all branches must return the same type, or not type, otherwise the compiler will error.

For example:

```
description := on size {
    0 => "empty",
    1..=10 => "small",
    11..=100 => "medium",
    else => "large",
}
``` 

-- `for` loops

Every type of loop is described by the `for` keyword.  The syntax is:

```
-- Infinite loops
for {
    ...
}

-- While loops
for <condition> {
    ...
}

-- C-style loops
for <init>; <condition>; <update> {
    ...
}

-- Iteration loops (e.g. over arrays, ranges, maps, etc.)
for <var> in <iterable> {
    ...
}
```

The `break` and `continue` keywords are supported in all loop types.  The `for`
loop is an expression, so it can be assigned to a variable.  In this case, the
loop must have a `break <expr>` statement in all branches, and all branches must
return the same type, or not type, otherwise the compiler will error.

Loops can have labels for breaking out of nested loops.  A label is defined by
prefixing a loop with `@<label>`, and then you can `break @<label> <expr>` or
`continue @<label>` to break or continue that specific loop.  For example:

```
answer := for o in 0..<10 @outer {
    for i in 0..<10 @inner {
        if o == i {
            break @outer o
        }
    }
}
else {
    42
}
```

Note that all labels must be unique within the function and prefixed with a `@`,
otherwise the compiler will error.

Also note, in the previous example, the `else` branch on a `for` is called if
the loop completes without hitting a `break` statement.  This is useful for
searching loops, where you want to return a default value if the search fails.
In this case, any variables defined in the loop are accessible in the `else`
branch, and the `else` branch is required to return the same type as the `break`
statements in the loop, otherwise the compiler will error.

