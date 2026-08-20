# Project Configuration

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
