# Chapter 13: String Interpolation

[Previous: Methods and Traits](12-Methods-and-Traits.md)

[Next: Modules](14-Modules.md)

String interpolation builds text by inserting expression values into a string.
It combines the strings from Chapter 3, the arenas from Chapter 8, and the
`Display` trait from Chapter 12. Runtime interpolation uses the built-in
temporary arena, so understanding the resulting string's lifetime is essential.

## Inserting expressions

Prefix a string literal with `$` and enclose each expression in braces:

```nerd
main :: fn () {
    answer := 42
    enabled := yes
    prn($"answer={answer}, enabled={enabled}")
}
```

This prints `answer=42, enabled=yes`. The result of interpolation is an
ordinary `string`. Interpolation does not print it; `prn` does the printing.
Integer and floating-point values, booleans, and strings have built-in text
formatting. This syntax does not use C's `%d` or `%s` format placeholders.

Braces contain expressions, rather than only variable names:

```nerd
double :: fn (value: i32) => value * 2

main :: fn () {
    value := 21
    prn($"{value} doubled is {double(value)}")
}
```

This prints `21 doubled is 42`. Calls, field access, arithmetic, and the other
expressions already covered can supply the values. Each inserted expression
is evaluated when the string is built.

## Literal braces and continuations

Escape literal braces with `\{` and `\}`:

```nerd
main :: fn () {
    value := 42
    prn($"literal \{value\}, actual {value}")
}
```

This prints `literal {value}, actual 42`. The usual string escapes, including
`\n`, still work.

A `+"..."` continuation after an interpolated literal remains part of that
interpolated string:

```nerd
main :: fn () {
    first := 20
    second := 22
    text := $"first={first}, "
            +"second={second}, total={first + second}"
    prn(text)
}
```

This prints `first=20, second=22, total=42`. The continuation also recognizes
embedded expressions. As with ordinary strings, bare adjacent literals do
not join automatically.

## Formatting your own types with `Display`

For a custom type, implement the existing `Display` trait. Interpolation calls
its `show` method and inserts the returned string:

```nerd
Point :: plex { x i32 y i32 }

impl Display for Point {
    show :: fn (self: Self) => $"({self.x}, {self.y})"
}

main :: fn () {
    point := Point { x: 3, y: 4 }
    prn($"point={point}")
}
```

This prints `point=(3, 4)`. The `show` method can itself use interpolation;
the outer interpolation incorporates that text into its own result.
There is no need to print inside `show` or manually call it at the insertion.

Built-in formatting and a generic trait constraint are distinct in the current
compiler, as discussed in Chapter 12. For example, formatting an integer here
works even though the compiler currently rejects an integer argument to a
generic function constrained by `Display`.

## Compile-time interpolation

When the inserted expressions are compile-time values, interpolation can be
evaluated during compilation:

```nerd
name :: "world"
answer :: 42
message :: $"hello {name}: {answer}"

main :: fn () {
    prn(message)
    temp_arena.reset()
    prn(message)
}
```

This prints `hello world: 42` twice. This particular string is a compile-time
constant, so it does not borrow its bytes from the runtime temporary arena.
A top-level interpolated binding must use expressions the compiler can
evaluate at compile time; module initialization does not build runtime strings.

## Runtime strings outlive the function that builds them

Runtime interpolation places its bytes in the calling thread's temporary
arena, available through `temp_arena`:

```nerd
label :: fn (value: i32) -> string {
    return $"value={value}"
}

main :: fn () {
    first := label(20)
    second := label(22)
    prn(first)
    prn(second)
    temp_arena.reset()
}
```

This prints `value=20` followed by `value=22`. Returning from `label` does
not invalidate its result, and building the second string does not overwrite
the first. Both remain valid until their arena storage is reclaimed.

The lifetime belongs to the arena, rather than the local variable or function
that first held the string. Resetting `temp_arena` invalidates its previous
runtime strings, including strings returned from other functions. It also
invalidates other allocations made in that temporary arena.

Choose a reset boundary where all earlier temporary allocations are finished:
for example, after completing a request or a frame. Until then, arena storage
accumulates as more runtime strings are built. A short-lived program can leave
final reclamation to runtime shutdown.

## Keeping text beyond a temporary-arena reset

Copying a `string` value copies its view of the bytes, not the bytes themselves.
To preserve text across a reset, copy its bytes into storage with a suitable
lifetime. An arena's `.pr` method from Chapter 8 can do this:

```nerd
label :: fn (value: i32) -> string {
    return $"value={value}"
}

main :: fn () {
    storage := arena(4096)
    defer storage.done()
    temporary := label(42)
    view := temporary
    kept := storage.pr(temporary)
    prn(view)
    temp_arena.reset()
    prn(kept)
}
```

This prints `value=42` twice. `temporary` and `view` borrow the same temporary
bytes and must not be read after the reset. `kept` borrows its own copy from
`storage`, so it survives that reset and remains valid until `storage` is
reset, restored past the allocation, or released. `.pr` copies text into the
arena; it does not print to the console.

## Reclaiming only recent temporary strings

Use a mark and restore when earlier temporary allocations must survive:

```nerd
label :: fn (value: i32) -> string {
    return $"value={value}"
}

main :: fn () {
    earlier := label(7)
    mark := temp_arena.mark()
    recent := label(42)
    prn(recent)
    temp_arena.restore(mark)
    prn(earlier)
    temp_arena.reset()
}
```

This prints `value=42` followed by `value=7`. Restoring the mark invalidates
`recent` but preserves `earlier`, which was allocated before the mark. The
final reset invalidates `earlier` too.

The same lifetime rules apply to an interpolated string stored in a plex,
array, or returned error value. Moving or copying the string view does not
move its bytes. Keep its arena alive until every reader has finished.
