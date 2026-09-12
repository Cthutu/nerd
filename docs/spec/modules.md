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
such as `str :: use std.text` stores the module value so exports are accessed
as fields, for example `str.utf8_decode`.

`pub use module.path` re-exports the imported module's public declarations.

Every module implicitly imports all public `core` bindings. Local declarations
with the same name as a core export take precedence over the implicit import;
an explicit import of `core` is neither required nor used.

## Resolution Order

Explicit module imports and implicit `core` lookup use these search roots:

1. Each directory in `NERD_LIB_PATH`, when the variable is defined (`:` separated
   on Unix, `;` on Windows). Otherwise, the executable directory's `mods` child.
2. The current working directory of the compiler invocation.

A defined `NERD_LIB_PATH` completely replaces the bundled library, even when it
is empty or a requested module is missing. An empty value disables library
lookup. `NERD_INSTALL_LIB_PATH` and the executable directory itself are not
search roots. Qualified imports do not search the root or current source file's
directory. The same library selection applies to language-server lookup.

As a separate relative-import rule, a bare single-name import can first resolve
an adjacent sibling when the importing file is outside the invocation directory.
This keeps imports such as `use kernel32` inside `os.windows` relative to their
module. Imports from the invocation directory retain library-first precedence.
Implicit `core` lookup always uses the ordered roots above.

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
