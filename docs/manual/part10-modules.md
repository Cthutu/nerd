# Part 10: Modules

[Manual Index](README.md) | Previous: [Dynamic Arrays And Manual Memory](part09-dynamic-arrays-and-manual-memory.md) | Next: [Interoperability With C](part11-interoperability-with-c.md)

Modules organise code across files and control which names are visible outside a
file.

A module path such as `std.io` names a module by its dotted path. The compiler
resolves that path in the configured module roots. A path can resolve to a
single file or to a folder module:

| Import path | First location tried | Folder-module fallback |
|-------------|----------------------|------------------------|
| `std.io`    | `std/io.n`           | `std/io/mod.n`         |
| `std.term`  | `std/term.n`         | `std/term/mod.n`       |

If both forms exist, the single `.n` file wins. Use `mod.n` when a module needs
to own a directory of helper files while still presenting one public module.

## Module Parts

A folder module owns its immediate sibling `.n` files as one module scope. The
root file is `mod.n`; every other `.n` file in the same directory is a module
part:

```text
std/term/mod.n   -- module root
std/term/term.n  -- part of std.term
std/term/input.n -- part of std.term
```

A part file is not a separate imported module. It shares the same top-level
scope as `mod.n`, so declarations in `mod.n` and declarations in parts can
refer to each other directly. For example, `mod.n` can define `Term`:

```nerd
pub Term :: plex {
    running bool
}
```

Then `term.n` can use it without an import:

```nerd
pub term_init :: fn (term: ^Term) {  -- Term is declared in mod.n
    term.running = yes
}
```

Use sibling `.n` files for implementation files that belong to the same public
module. Use a nested folder module, such as `std/term/input/mod.n`, when another
file should remain a separate module with its own public/private boundary.

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

## Platform-Gated Imports

Top-level `on` blocks can contain declarations and `use` statements. The
condition is a string feature symbol. Built-in symbols include the host platform
names such as `"windows"` and `"linux"`, and extra symbols can be enabled from
the CLI with `-Dname`. The compiler only loads imports from an enabled platform
branch:

```nerd
on "linux" {
    use std.linux  -- imported only when the "linux" symbol is enabled
}
```

Names imported inside an enabled top-level `on` block become visible to the rest
of the module, just like other top-level imports. Imports inside a disabled
branch are ignored.

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

Qualified module names work in type positions as well as expression positions:

```nerd
shapes :: use app.shapes

Item :: plex {
    bounds shapes.Rect
    owner  ^shapes.Layer
}

main :: fn () {
    layer : shapes.Layer
}
```

## Public Exports

Use `pub` to make a declaration visible outside its module:

```nerd
pub answer :: fn () -> i32 {  -- exported from this module
    return 42
}
```

Declarations without `pub` are private to the module.

Public plex and union declarations cannot expose private type declarations in
their field types. Make the field type public, or keep that implementation
detail outside the public field list.

Public generic functions and generic type aliases can be imported like other
public declarations. The concrete versions are still created only when another
module uses them:

```nerd
use app.collections

stack: Stack[i32]       -- use an imported generic type alias
stack_push(^stack, 42)  -- infer the imported generic function as stack_push[i32]
```

## Re-Exports

A module can re-export public names from another module. Use this when building
a small public surface over several internal files.

```nerd
pub io :: use std.io  -- re-export std.io through this module
```

Code that imports this module can then access the public `io` module binding.

Use `pub use` to import another module's public names into the current module
and re-export them through this module:

```nerd
pub use child  -- re-export public names from a sibling module
```

Inside a folder module, this imports public names from `child.n` and makes them
visible to code that imports the folder module.

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
