# Modules

Module path parsing is in `src/compiler/ast/parse.c`; resolution and module
loading are in `src/compiler/modules/modules.c` and
`src/compiler/build/front/program.c`.

## Import Syntax

```bnf
use-declaration ::= 'use' module-path
                  | 'use' module-path '{' grouped-use-entry+ '}'
                  | 'use' expression

module-binding  ::= IDENT '::' 'use' module-path
grouped-use-entry
                ::= module-path
                  | module-path '{' grouped-use-entry+ '}'
module-path     ::= IDENT { '.' IDENT }
```

Bare `use module.path` imports public exports into the current scope. A binding
such as `str :: use std.string` stores the module value so exports are accessed
as fields, for example `str.split`.

`pub use module.path` re-exports the imported module's public declarations.

Every module implicitly imports `core`. Local declarations with the same name
as a core export take precedence over the implicit import. An explicit
`use core` is still allowed and follows normal duplicate-binding checks.

## Resolution Order

`module_resolve_path` currently searches:

1. The current source file's directory.
2. The root source file's directory.
3. Each directory in `NERD_LIB_PATH` (`:` separated on Unix, `;` on Windows).
4. The executable directory.
5. The executable directory's `mods` child.

For `a.b`, the resolver first tries `a/b.n`; if absent, it tries `a/b/mod.n`.
When a `mod.n` file is found on a path prefix, it forms a package boundary for
external imports. For example, `a/b/mod.n` makes `a.b` importable, but external
code cannot import `a.b.c` directly from `a/b/c.n`. The package root can still
import its own child files and decide what to re-export.

## Folder Modules And Parts

A folder module uses `mod.n`. The front end can expand module part files from
the same directory. Part files contribute to the same module analysis rather
than becoming separate importable modules. If `mod.n` explicitly imports a
sibling child file, that file is treated as a private child module instead of an
implicit part.

## Platform Guards

Top-level `on` blocks conditionally include declarations and imports:

```bnf
top-level-on ::= 'on' [ '!' ] STRING '{' { top-level-item } '}'
```

The string is a platform key. Built-in keys include operating systems such as
`"linux"`, `"windows"`, `"macos"`, `"bsd"`, and `"posix"`, build-mode keys
`"debug"` and `"release"`, and the `"x64"` architecture key.

Use `assert on` to require a whole file to compile only for a matching platform:

```bnf
top-level-assert-on ::= 'assert' 'on' [ '!' ] STRING
```

```nerd
assert on "linux"

pub open :: fn (path: string) -> i32 {
    ...
}
```

If the assertion fails, compilation stops with a platform assertion diagnostic.
Unlike a block `on`, `assert on` does not introduce a nested declaration region.

## Visibility

Only `pub` top-level declarations are exported. `program_collect_module_exports`
also expands public `use` re-exports so downstream modules can import the
re-exported symbols.
## Compound Function Visibility

A public compound exports its callable signature set. Its concrete private
members remain private names, but may be selected through the public compound,
as with a public wrapper. Qualified exported compounds do not merge with local
or other-module compounds.
