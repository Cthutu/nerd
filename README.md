# Build Directives

This project uses a small directive system to declare module dependencies and
compile-time defines.

## Source file directives

Add directives as line comments:

```c
//> use: greet core
//> def: FEATURE_X GREETING=42
```

- `use`: list module names (directories under `src/`)
- `def`: list preprocessor symbols (optionally `NAME=VALUE`)

Unknown directives are errors.

## Module `.build` files

Each module directory may include a `.build` file with directive lines:

```
use: util net
def: LOGGING LOG_LEVEL=2
```

Blank lines are ignored. Non-empty lines must match `command: params`.

## Module headers

- A module maps to a top-level directory in `src/` (for example `src/core`).
- The canonical module header must be named after the directory:
  - `src/<module>/<module>.h` (for example `src/core/core.h`)
- Additional `*.h` files inside the module tree are allowed as internal headers.
- Any header change inside a module tree triggers rebuild for all sources in that
  module, including nested sub-directories.
- Top-level headers directly under `src/` are ignored for header dependency
  rebuild tracking.

## Root `.build`

`src/.build` applies to all projects. Touching it forces a full rebuild.
