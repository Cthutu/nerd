# Part 10: Modules

[Manual Index](README.md) | Previous: [Dynamic Arrays And Manual Memory](part09-dynamic-arrays-and-manual-memory.md) | Next: [Interoperability With C](part11-interoperability-with-c.md)

Modules organise code across files and control which names are visible outside a
file.

A module path such as `std.io` names a module by its dotted path. The compiler
resolves that path to a source file in the configured module roots.

## `use`

`use` imports public names from a module into the current scope:

```nerd
use std.io  -- import public names from std.io

main :: fn () {
    prn("hello")
}
```

Grouped `use` imports are available when a module exposes multiple names:

```nerd
use std { io }             -- import std.io
use app { commands rooms } -- import app.commands and app.rooms
```

The first line imports `std.io`. The second imports `app.commands` and
`app.rooms`.

Use imports sparingly in larger files so readers can tell where names come from.

## Module Bindings

Bind a module to a local top-level name with `use` as the binding value:

```nerd
io :: use std.io  -- bind the module to the name io

main :: fn () {
    io.prn("hello")  -- qualified access through io
}
```

This keeps access qualified and avoids importing public names directly into the
current scope.

## Public Exports

Use `pub` to make a declaration visible outside its module:

```nerd
pub answer :: fn () -> i32 {  -- exported from this module
    return 42
}
```

Declarations without `pub` are private to the module.

## Re-Exports

A module can re-export public names from another module. Use this when building
a small public surface over several internal files.

```nerd
pub io :: use std.io  -- re-export std.io through this module
```

Code that imports this module can then access the public `io` module binding.

## Choosing A Use Form

Use `use` for simple examples and small programs:

```nerd
use std.io  -- import names directly
```

Use a named module binding when clarity matters:

```nerd
io :: use std.io  -- keep names qualified
```

Qualified names such as `io.prn` make dependencies explicit.

## Standard Library

The standard library is still developing. This manual uses it for small
examples, but does not try to document its full API. Treat standard library
details as separate from the core language rules.
