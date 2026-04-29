# Part 10: Modules

[Manual Index](README.md) | Previous: [Dynamic Arrays And Manual Memory](part09-dynamic-arrays-and-manual-memory.md) | Next: [Interoperability With C](part11-interoperability-with-c.md)

Modules organise code across files and control which names are visible outside a
file.

## `use`

`use` imports public names from a module into the current scope:

```nerd
use std.io

main :: fn () {
    prn("hello")
}
```

Grouped `use` imports are available when a module exposes multiple names:

```nerd
use std.io.{pr, prn}
```

Use imports sparingly in larger files so readers can tell where names come from.

## Module Bindings

Bind a module to a local top-level name with `mod`:

```nerd
io :: mod std.io

main :: fn () {
    io.prn("hello")
}
```

This keeps access qualified and avoids importing public names directly into the
current scope.

## Public Exports

Use `pub` to make a declaration visible outside its module:

```nerd
pub answer :: fn () -> i32 {
    return 42
}
```

Declarations without `pub` are private to the module.

## Re-Exports

A module can re-export public names from another module. Use this when building
a small public surface over several internal files.

## Choosing `use` Or `mod`

Use `use` for simple examples and small programs:

```nerd
use std.io
```

Use `mod` when clarity matters:

```nerd
io :: mod std.io
```

Qualified names such as `io.prn` make dependencies explicit.

## Standard Library

The standard library is still developing. This manual uses it for small
examples, but does not try to document its full API. Treat standard library
details as separate from the core language rules.
