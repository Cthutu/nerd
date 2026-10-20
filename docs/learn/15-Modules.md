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

## The `core` module

`core` is the standard-library module that supplies the common helpers used
throughout this guide. Its source file is `core.n`. Nerd loads it automatically
and makes its public declarations available in every module, without an
explicit `use`. This is why the first hello-world program could call `prn`
before imports had been introduced.

Examples of what `core.n` provides include:

| Declarations | Purpose |
| --- | --- |
| `pr`, `prn` | Print text to standard output, without or with a final newline. |
| `epr`, `eprn` | Print text to standard error, without or with a final newline. |
| `temp_arena` | Access the calling thread's temporary arena. |
| Arena methods such as `.alloc`, `.mark`, `.reset`, and `.done` | Allocate and manage arena storage. |
| `Display`, `Eq`, `Order`, `Default`, `Iterator[Item]` | The standard traits used by the language features from Chapter 13. |

The file contains Nerd declarations, just like your own module files. Some
operations delegate to the runtime; `core.n` supplies their Nerd-facing helpers
and methods. Primitive types such as `i32` and `bool`, the `arena` type, and
the optional and result type forms are built into the language. They are not
ordinary type declarations supplied by `core.n`.

`core` names the module; `core.n` names its source file. In a normal installation,
that file is at `mods/core.n` beside the compiler executable. If you replace
the library search paths with `NERD_LIB_PATH`, make sure `core.n` remains
available through the search locations in the table below. That table explains
where the compiler looks for the file.

Local declarations can shadow implicitly available core names. A new local
trait called `Display`, for example, is a different declaration from the
standard trait. Implement the existing core traits when integrating with
language features, rather than redeclaring their names.

The implicit import covers `core`'s public declarations, not every other
standard-library module. Modules such as `std.text` still need a `use`
declaration, as other imports do.

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
    pub init :: fn (value: i32) -> Self {
        return { value }
    }

    pub get :: fn (self: Self) => self.value
}
```

Create `main.n`:

```nerd
counters :: use counters

main :: fn () -> i32 {
    counter: counters.Counter = counters.Counter.init(42)
    return counter.get()
}
```

This returns `42`. Exporting the type does not by itself export every inherent
method. `pub` on `init` and `get` makes those calls available outside their
module. The associated function is called through the qualified type,
`counters.Counter`, while the receiver method is called through `counter`.

Use `pub impl Counter { ... }` when every method in a block should be public,
instead of marking each member separately.

## Qualified generics

Public generic types and functions support both qualified access and bare
imports. Type arguments and inference follow Chapter 12's rules.

In a new project, create `cells.n`:

```nerd
pub Cell :: plex [T] { value T }

pub identity :: fn [T] (value: T) -> T {
    return value
}
```

Create `main.n`:

```nerd
cells :: use cells

main :: fn () -> i32 {
    cell: cells.Cell[i32] = { value: cells.identity(40) }
    wide := cells.identity[i64](2)
    return cell.value + wide.as(i32)
}
```

This returns `42`. `cells.Cell[i32]` names the exported generic type with
`i32` as its argument. The first function call infers `i32`; the second
explicitly chooses `i64`. Qualified generic types can also be used in aliases,
nested type arguments, and typed literals, such as
`cells.Cell[i32] { value: 42 }`.

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

Ordinary module lookup, including lookup of `core.n`, uses this order:

| Order | `NERD_LIB_PATH` unset | `NERD_LIB_PATH` defined |
| --- | --- | --- |
| 1 | The `mods` directory beside the compiler executable. | Each listed directory, from left to right. An empty value skips this step. |
| 2 | The current working directory of the compiler invocation. | The current working directory of the compiler invocation. |

Separate library directories with `;` on Windows or `:` on Unix. Defining
`NERD_LIB_PATH` replaces the bundled-library search completely: missing modules
do not fall back to `mods` beside the compiler or another installed library.
Your selected search locations must still provide `core.n` for its helpers
to remain available.

The working directory is where you invoked Nerd, not necessarily the directory
containing the source file. This is why the examples run from their project
root. Qualified imports do not automatically search beside that source file.

There is one relative-import exception: an explicit bare single-name import
from a file outside the invocation directory can first resolve an adjacent
sibling module, before the table's search. Imports from the invocation directory
retain library-first precedence. Automatic lookup of `core.n` always follows
the table.

Compile-time selection of platform-specific imports is covered in Chapter 17.
