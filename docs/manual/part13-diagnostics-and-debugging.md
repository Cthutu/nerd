# Part 13: Diagnostics And Debugging

[Manual Index](README.md) | Previous: [Building A Small Program](part12-building-a-small-program.md) | Next: [Syntax Reference](appendix-a-syntax-reference.md)

Compiler diagnostics are messages produced when the compiler cannot accept a
program. They are meant to point at the source rule that was broken. Read them
from top to bottom: code, message, primary location, notes, and help.

## Diagnostic Ranges

Nerd diagnostic codes are grouped by compiler phase:

- `0100`-`0199`: lexer diagnostics
- `0200`-`0299`: parser and AST construction diagnostics
- `0300`-`0399`: semantic analysis diagnostics

This tells you roughly how far the compiler got.

## Lexer Errors

Lexer errors happen before parsing. They usually mean the source text contains
characters or literals the compiler cannot tokenise.

Common causes:

- unknown characters
- unterminated strings
- invalid string escapes

## Parser Errors

Parser errors happen when tokens do not form valid syntax.

Common causes:

- missing delimiters
- missing expressions
- misplaced operators
- invalid declaration forms

Fix parser errors before reasoning about types. The compiler may not understand
the program shape yet.

## Semantic Errors

Semantic errors happen after parsing, when the program shape is known but a
language rule is broken.

Common causes:

- unknown symbols
- type mismatches
- invalid assignment targets
- reads of `undefined` bindings before assignment
- unused local variables, parameters, or pattern binders
- invalid `break` or `continue`
- non-exhaustive value-producing `on`
- private module members
- invalid FFI signatures

## Reading A Diagnostic

A diagnostic has:

| Part | Meaning |
| --- | --- |
| code | stable category, such as `0304` |
| main message | short description of what failed |
| primary source location | where the compiler found the problem |
| notes | extra context about the rule |
| help | suggested source change |

The primary message says what failed. Notes explain the rule. Help suggests a
change.

## Unknown Symbols

If you see an unknown symbol diagnostic, check:

- spelling
- whether the name is in scope
- whether a module member is public
- whether you meant to qualify it, such as `io.prn`

## Type Mismatches

Type mismatches usually mean an expression produced one type where another was
expected.

Common fixes:

- add an explicit annotation
- use `.as(Type)` when a cast is intended
- update one branch of an expression so all paths produce the same type
- use `else` when a value-producing branch must be exhaustive

## Read Before Assignment

`undefined` creates storage without a default value. If you read that binding
before assigning to it, the compiler reports a semantic error.

```nerd
main :: fn () -> i32 {
    value: i32 = undefined  -- no meaningful i32 value yet
    value = 42              -- assignment makes value readable
    return value
}
```

When control flow branches, every reachable path to the read must assign the
binding first. Add an initial value, assign in all branches, or avoid
`undefined` when the default storage value is acceptable.

## Unused Locals

Unused local diagnostics mean a local variable, parameter, or pattern binder is
never read.

```nerd
main :: fn () -> i32 {
    _future := 1  -- leading underscore marks deliberate non-use
    return 0
}
```

Remove the binding if it is not needed. Use a leading underscore only when the
binding is intentionally present for documentation, future work, or shape
matching.

## Control Flow Errors

`break` and `continue` must target valid control-flow constructs.

- `break` can leave loops and expression blocks.
- `continue` can only continue loops.
- Labels can make nested targets explicit.

Deferred statements follow the same rules: a `break` or `continue` inside a
deferred statement still needs a valid target where it appears.

## Source Tests

Nerd source files can contain top-level tests:

```nerd
test "adds two numbers" {
    assert 1 + 1 == 2  -- fail the test if the condition is false
}
```

Run tests from the command line:

```sh
nerd test path/to/file.n
```

Use `--list` to show discovered test names without running them:

```sh
nerd test path/to/file.n --list
```

Use `--filter text` to run only tests whose names contain that text:

```sh
nerd test path/to/file.n --filter adds
```

Use `-v` or `--verbose` to print a result line as each selected test finishes:

```sh
nerd test path/to/file.n -v
```

Test declarations are not normal module API declarations. The test runner
turns each selected test into a hidden zero-parameter function and calls those
functions from a generated test entry point.

The current test runner uses fail-fast assertion behaviour. If an `assert`
inside a test fails, the program exits with status 127 and later selected tests
do not run.

## Debugging Small

When a program fails to compile:

1. Fix the first diagnostic first.
2. Reduce the failing expression to a smaller one.
3. Add type annotations where inference is not obvious.
4. Split complex `on` expressions into named helper functions.
5. Prefer explicit module qualification while debugging imports.
