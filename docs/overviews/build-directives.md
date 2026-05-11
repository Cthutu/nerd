# Build Directives

The build system uses a small directive format to declare module dependencies
and compile-time defines.

## Source File Directives

Add directives as line comments in C source files:

```c
//> use: greet core
//> def: FEATURE_X GREETING=42
//> run: clang -c data/nrt.c -o _obj/runtime/nrt.o
//> run(windows): clang -c data/nrt.c -o _obj/runtime/nrt.o
```

- `use` lists module names
- `def` lists preprocessor symbols, optionally with `NAME=VALUE`
- `run` declares generated build inputs that must exist before C compilation

`run` directives may include a platform filter, for example `run(windows)`,
`run(linux)`, `run(macos)`, `run(bsd)`, or `run(posix)`. Multiple platform names
can be comma-separated.

Unknown directives are treated as errors by the build tooling.

## Module `.build` Files

Each module directory may include a `.build` file with directive lines:

```text
use: util net
def: LOGGING LOG_LEVEL=2
```

Blank lines are ignored. Non-empty lines must use `command: params`.

## Module Layout

- A module maps to a top-level directory in `src/`
- The canonical module header is `src/<module>/<module>.h`
- Additional headers under the module tree are allowed as internal headers
- Any header change inside a module tree triggers a rebuild for that module

Top-level headers directly under `src/` are not part of the module-header
tracking scheme.

## Root `.build`

`src/.build` applies to all projects. Touching it forces a full rebuild.
