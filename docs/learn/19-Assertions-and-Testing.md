# Chapter 19: Assertions and Testing

[Previous: Boxes and Ownership](18-Boxes-and-Ownership.md)

[Next: Compile-Time Parameters and Built-In Macros](20-Compile-Time-Parameters-and-Macros.md)

Assertions check assumptions while a program runs. Source tests exercise those
assumptions with repeatable examples. They are useful for the small functions
and data types developed in earlier chapters as well as larger programs.

## Runtime assertions

Write `assert` followed by a boolean condition:

```nerd
main :: fn () {
    count := 3
    assert count > 0
    assert count == 3, "expected three items"
    prn("checks passed")
}
```

This prints `checks passed`. When a condition is false, the runtime reports the
source location and message and terminates the program. Without an explicit
message it uses a default assertion message. The condition must be `bool`;
extract an optional or result, or turn its truthiness into a boolean, before
asserting about it.

The message after the comma must be a `string`. Interpolation can include
values that help explain a failure:

```nerd
main :: fn () {
    count := 3
    assert count > 0, $"expected a positive count, got {count}"
}
```

Use assertions for broken assumptions in your code. Use optionals and results
for expected outcomes such as missing input or a file that cannot be opened.
Assertion termination is not an error return and does not provide the ordinary
scope-exit cleanup guarantees of `return`, `?`, `defer`, and `undo`.

Chapter 17's module-scope `assert on "feature"` is a separate compile-time
requirement. A runtime assertion checks a boolean value during execution.

## A named source test

A top-level `test "name"` block contains statements to execute as a test:

```nerd
add :: fn (left: i32, right: i32) -> i32 {
    return left + right
}

test "add combines positive values" {
    assert add(20, 22) == 42
}

test "add accepts zero" {
    assert add(7, 0) == 7
}

main :: fn () {
    prn($"{add(20, 22)}")
}
```

Save this as `main.n` and run:

```sh
nerd test main.n
```

Both tests pass. `nerd run main.n` runs the ordinary program and prints `42`;
it does not execute the test bodies. A failing assertion makes its test fail.
Give each test a name describing the behaviour it checks, so a failure identifies
an expectation rather than just a function name.

## Test-only declarations

Use a top-level `test { ... }` block for helpers and data needed only by tests:

```nerd
sum :: fn (values: []i32) -> i32 {
    total := 0
    for value in values { total += value^ }
    return total
}

test {
    sample :: fn () -> [3]i32 {
        return [10, 12, 20]
    }
}

test "sum combines every element" {
    values := sample()
    assert sum(values[..]) == 42
}

test "sum of no elements is zero" {
    values: [0]i32
    assert sum(values[..]) == 0
}

main :: fn () {}
```

During `nerd test`, the helper declarations become part of the module where
they appear. Ordinary builds ignore them. Keep production code independent
of these helpers. Source tests, test-only blocks, and declarations inside
those blocks are private; they cannot be marked `pub`.

## Checking an error-handling contract

Match a result to verify both its success and failure cases:

```nerd
positive :: fn (value: i32) -> i32\string {
    on value <= 0 => return "not positive"!
    return value
}

test "positive accepts a positive number" {
    on positive(42) {
        value => { assert value == 42 }
        error! => { assert no, error }
    }
}

test "positive rejects zero" {
    on positive(0) {
        _value => { assert no, "expected an error" }
        error! => { assert error == "not positive" }
    }
}

main :: fn () {}
```

Both tests pass. The success test checks the returned payload; the failure test
checks the error value. This makes the function's contract explicit without
depending on its implementation.

## Checking and running

| Command | Purpose |
| --- | --- |
| `nerd check main.n` | Check the ordinary program without generating an executable. |
| `nerd run main.n` | Build and run the ordinary entry point. |
| `nerd test main.n` | Build and run source tests for the selected module. |

A successful ordinary check does not establish that test-only declarations and
bodies pass. Run the test command after editing them. Keep tests independent
of execution order and release any resources they allocate according to the
same ownership rules as ordinary code.
