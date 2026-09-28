# Project Configuration

## Source build settings

Stable dependency and executable requirements belong in source:

```nerd
build {
    define: validation
    windowed: yes
    on "windows" {
        library_path: $VULKAN_SDK/Lib
    }
    library_path: "vendor/lib"
}
```

`build` is contextual: ordinary bindings named `build` remain legal. Blocks
may appear between top-level declarations, including module parts. Prefer the
preamble for visibility. Nested `on "name"` and `on !"name"` guards accept the
same platform, profile and user defines as ordinary conditional code. Unknown
keys and incorrect value types are errors, including in inactive branches.

| Key | Value | Combination rule |
| --- | --- | --- |
| `library_path` | A quoted literal path or `$ENV/suffix` | Append in source order; search importers before dependencies |
| `windowed` | `yes` or `no` | Last active local entry wins; importer overrides dependencies |
| `define` | An identifier | Add to this module’s defines |

Keys and blocks may repeat; arrays and executable expressions are not accepted.
`define` supplements command-line/configuration defines without changing the
process environment. Entries are evaluated top to bottom: a define affects
later build guards and all ordinary code in its module, but cannot activate
an earlier build guard retroactively. Source defines do not propagate into
imports or out to importing modules. Command-line defines still apply globally.

Imported modules contribute build settings automatically. The selected root
has highest precedence; shared dependencies are visited after all their
importers, with discovery order breaking ties between independent modules.
Identical resolved library paths are deduplicated without sorting (case
insensitive on Windows). Conflicting sibling `windowed` values require an
explicit setting in a common importer; `windowed: no` can override an imported
windowed requirement. On Windows this controls the subsystem and generated
entry wrapper. Other platforms ignore its executable effect. Legacy
`pragma windowed` remains supported as a local `windowed: yes` declaration.

Quoted paths are literal and relative to the declaring source file, including
module parts. `$ENV/suffix` expands a single environment variable followed by
a literal suffix; spaces in its value remain part of one path. Use forward
slashes in unquoted suffixes, and absolute SDK environment values for portable
invocation from any directory. There is no shell, tilde or recursive expansion.
The compiler does not change `LIB`, `PATH`, or the launched program’s environment.
On Windows, quotes, newlines, `%` and `!` in resolved paths are rejected to avoid
command-shell expansion.

Paths are resolved for native linking and executable/shared-library `--copts`,
which emits equivalent Clang `-L` arguments. Missing or empty environment
variables produce a diagnostic naming the variable and declaring file.
`check`, `format`, editor analysis, `--cgen` alone, and object/archive output
do not require the SDK. Native linking emits `/libpath:` on Windows and `-L`
on Unix before default library search paths. Runtime loader configuration,
such as Vulkan validation layers, remains a separate environment concern.

Invocation choices—release/debug, jobs, output names/modes, diagnostic options,
and temporary feature defines—remain command-line options. Module search is
still configured with `NERD_LIB_PATH`; `library_path` only finds native libraries.

## Invocation configuration

Nerd reads project configuration from `nerd.json` in the process's current
directory. Pass `--config <path>` as a global option to select another file:

```sh
nerd --config path/to/nerd.json check main.n
nerd lsp --config path/to/nerd.json
```

The default file is optional. A path supplied with `--config` must exist and
contain valid JSON.

The currently supported fields are `env` and `define`:

```json
{
    "env": {
        "NERD_LIB_PATH": "mods"
    },
    "define": [
        "sqlite",
        "tracing"
    ]
}
```

- `env` must be an object whose names and values are strings. Nerd sets these
  variables before executing the selected command. Relative path values remain
  relative to Nerd's current working directory.
- `define` must be an array of non-empty strings. These names behave like
  command-line `-Dname` defines and are additive with them.

Configuration applies to normal compiler commands and the LSP server. This
allows command-line builds and editor analysis to share module paths and
compile-time `on "name"` branches.

## Library search

`NERD_LIB_PATH` replaces the compiler's bundled library search path. If it is
unset, Nerd uses `mods` beside its executable; if defined but empty, Nerd skips
library lookup. After the selected library roots, Nerd searches the invocation
working directory. Missing modules never fall back to another installed library.
`NERD_INSTALL_LIB_PATH` is no longer used. Bare sibling imports inside modules
remain relative to their importing file.
