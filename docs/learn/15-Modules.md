# Chapter 15: Modules

[Previous: String Interpolation](14-String-Interpolation.md)

[Next: Special Function Declarations](16-Special-Function-Declarations.md)

Modules organize declarations across source files and control which names
other code can use. Nerd does not require C-style headers or textual inclusion:
`use` loads a module and makes its public declarations available.

For each project below, create the files at the paths shown and run
`nerd run main.n` from that project's directory.

## A module and its public declarations

Create `arithmetic.n`:

```nerd
base :: 40
pub answer :: fn () => base + 2
```

Create `main.n` beside it:

```nerd
math :: use arithmetic

main :: fn () -> i32 {
    return math.answer()
}
```

This returns `42`. `math :: use arithmetic` binds the module to `math`, so its
public declaration is accessed as `math.answer`. The module binding's name is
your choice. `base` has no `pub`, so it is private to `arithmetic`; importing
code cannot access it through `math.base`.

`pub` marks a declaration as public. Names without it are private to their
module, including implementation helpers called by public functions. Public
types must also expose suitable public field types; a public plex or union
cannot expose a private type through a field declaration.

Every module already has access to public core declarations such as `prn`,
`arena`, and `Display`. An explicit import of `core` is unnecessary. Local
declarations can shadow those implicit names, so avoid reusing a core name
when you intend to use its language-supported behavior.

## Importing names directly

Keep the same `arithmetic.n`, but replace `main.n` with:

```nerd
use arithmetic

main :: fn () -> i32 {
    return answer()
}
```

This also returns `42`. Bare `use arithmetic` brings the module's public names
into the current scope. A module binding keeps them qualified instead:

| Form | Access |
| --- | --- |
| `use arithmetic` | `answer()` |
| `math :: use arithmetic` | `math.answer()` |

Qualified access makes the source of a name clear and helps avoid clashes in
larger modules. Qualified module names work in type positions too: an exported
`Point` can be named `shapes.Point`, including in `^shapes.Point`.

## Public types and methods

In a new project, create `counters.n`:

```nerd
pub Counter :: plex { value i32 }

impl Counter {
    pub get :: fn (self: Self) => self.value
}
```

Create `main.n`:

```nerd
counters :: use counters

main :: fn () -> i32 {
    counter: counters.Counter = { value: 42 }
    return counter.get()
}
```

This returns `42`. Exporting the type does not by itself export every inherent
method. `pub` on `get` makes that call available outside its module.
Public generic declarations can be imported too. With a bare `use`, type
arguments and inference work as in Chapter 12.

Use `pub impl Counter { ... }` when every method in a block should be public,
instead of marking each member separately.

Compiler note: a direct associated call such as `module.Type.init(...)`
is currently rejected even when the type and function are public.
Import with `use module` and call
`Type.init(...)` instead. The public receiver call above works with a
qualified type.

Qualified generic type applications also currently fail: import an exported
`Cell` with bare `use` and write `Cell[i32]`, rather than `module.Cell[i32]`.
The same limitation affects generic function calls such as
`module.identity(42)`; use `identity(42)` after a bare import instead.
Qualified ordinary type names, as used for `counters.Counter` above, work.

## Dotted paths and grouped imports

A dotted path maps to directories. For example, `math.add` looks for
`math/add.n`, then `math/add/mod.n` if the first file is absent.

In a new project, create `math/add.n`:

```nerd
pub increment :: fn (value: i32) => value + 1
```

Create `math/multiply.n`:

```nerd
pub double :: fn (value: i32) => value * 2
```

Create `main.n`:

```nerd
use math { add multiply }

main :: fn () -> i32 {
    return double(increment(20))
}
```

This returns `42`. The grouped import loads the two module paths
`math.add` and `math.multiply` and imports their public declarations. It is
equivalent to writing two separate `use` declarations. This example has no
`math/mod.n`; its directory simply groups those paths.

## Folder modules and shared parts

A module can use `mod.n` as its root and share one scope across sibling source
files. In a new project, create `tools/mod.n`:

```nerd
pub answer :: fn () => base + 2
```

Create `tools/values.n`:

```nerd
base :: 40
```

Create `main.n`:

```nerd
tools :: use tools

main :: fn () -> i32 {
    return tools.answer()
}
```

This returns `42`. `values.n` is a **module part**, rather than a separate
imported module. Its declarations share the root's top-level scope, so
`answer` can use `base` directly. Private declarations remain visible across
parts of the same module.

If `tools.n` also existed, it would take precedence over `tools/mod.n` during
resolution. Use one form deliberately to avoid ambiguity about which module
is being imported.

## Re-exporting a child module

A folder root can instead explicitly import a sibling file as a child module.
`pub use` exposes that child's public declarations through the root.

In a new project, create `api/mod.n`:

```nerd
pub use api.internal
```

Create `api/internal.n`:

```nerd
pub answer :: fn () => 42
```

Create `main.n`:

```nerd
api :: use api

main :: fn () -> i32 {
    return api.answer()
}
```

This returns `42`. Explicitly importing `internal.n` makes it a child module,
rather than a shared part. Its public `answer` is re-exported by `api`.
External code imports `api`; the `mod.n` boundary prevents importing
`api.internal` directly from outside that package.

A public module binding, such as `pub child :: use api.internal`, is another
way to re-export: callers then keep the additional qualification, as in
`api.child.answer()`, instead of receiving `answer` directly through `api`.

## Where modules are found

Nerd searches the selected library roots, then the directory from which the
compiler was invoked. This is why the examples run from their project root;
a qualified import does not automatically search beside an arbitrary source
file passed from elsewhere.

If `NERD_LIB_PATH` is unset, the library root is `mods` beside the compiler.
Setting it replaces that bundled-library search with the listed directories:
separate them with `;` on Windows or `:` on Unix. An empty value disables
library lookup. The same selection applies to implicit core declarations, so
an override must still make `core` available. Missing modules do not fall back
to a different installed copy of the library.

Bare sibling imports inside an imported module have a separate relative
lookup rule, allowing a file to import an adjacent helper by its short name.
For a project's public paths, use the directory structure and invoke Nerd
from the root containing those paths.

Compile-time selection of platform-specific imports is covered in Chapter 17.
