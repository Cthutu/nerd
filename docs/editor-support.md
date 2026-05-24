# Editor Support

Editor support is a paired surface. Keep VS Code support in
`syntax/nerd-vscode` and Neovim/LazyVim support in `syntax/nerd-nvim` aligned
when an editor-facing change lands.

Update both integrations when a change affects:

- file extension or filetype detection
- syntax highlighting rules
- LSP startup, command arguments, capabilities, or settings
- formatter integration and format-on-save behaviour
- install or packaging recipes

If only one editor can reasonably be updated in a commit, call that out in the
commit message or accompanying notes and leave a clear follow-up.

## Neovim Runtime

The Neovim runtime is repo-owned and copied into the local Linux configuration
by `just install-nvim`. Do not rely on symlinks for the installed files.

The current LazyVim integration:

- registers `.n` files as Nerd source
- configures `nvim-lspconfig` to launch the Nerd LSP
- configures `conform.nvim` to format a temporary file with `nerd format`
- installs a Nerd indent file for block and continuation indentation

Tree-sitter highlighting support is intentionally deferred until a later editor
milestone.

## VS Code Extension

The VS Code extension is repo-owned and lives in `syntax/nerd-vscode`. It
registers `.n` files, contributes the TextMate grammar, and starts the language
server with `nerd lsp`.

The server lookup order is:

1. workspace `_bin/nerd-debug` or `_bin/nerd`
2. user install under `~/.local/bin/nerd`
3. `nerd` from `PATH`

`just install` installs the compiler, modules, Neovim files, the CodeLLDB VS
Code prerequisite, and the repo-owned VS Code extension.

The VS Code extension also adjusts indentation after Enter for Nerd buffers.
This mirrors the Neovim indent file for cases that VS Code's declarative
language configuration cannot express, such as returning to the declaration
indent after a multi-line FFI signature closes.

On Linux, the VS Code extension contributes `Nerd: Debug Active File with
CodeLLDB`. The command builds the active `.n` file to `_tmp/debug/` with the
detected Nerd executable, then starts a Nerd `type: "nerd"` launch session that
delegates process control to CodeLLDB. The extension canonicalizes source paths
before build and breakpoint requests so VS Code, command-line builds, and the
installed `nerd` agree on the source files named in DWARF.

The debugger shim owns Nerd-specific presentation for values that native LLDB
does not understand by itself. It renders strings, slices, dynamic arrays,
tuples, plexes, enums, raw unions, and pointers in Nerd-facing terms where
possible, and it gives clear messages for known unsupported Nerd-only watch
forms such as declarations, ranges, casts, aggregate literals, and `on`
branches. This is still a Linux-first bridge to a native debugger while the
project decides whether a larger Nerd-owned adapter is needed.

## Verification

For language or LSP behaviour changes, run:

```sh
just test
```

For editor wiring and LSP startup changes, run:

```sh
python3 build/check_editor_integrations.py --nerd _bin/nerd-debug
```

For debugger value or stepping changes on Linux, also run:

```sh
python3 build/check_debugger_adapter_transforms.py
python3 build/check_debugger_smoke.py --nerd _bin/nerd-debug
python3 build/check_debugger_stepping.py --nerd _bin/nerd-debug
```

For install recipe changes, also check:

```sh
just --dry-run install-nvim
```

For Neovim filetype or syntax changes, a quick headless smoke test should show
the `nerd` filetype and syntax for a `.n` file:

```sh
nvim --headless -u NONE \
  --cmd 'set runtimepath^=/home/matt/nerd/syntax/nerd-nvim' \
  +'syntax on' +'filetype on' +'edit _tmp/c4.n' \
  +'lua io.write((vim.bo.filetype or "") .. ":" .. (vim.bo.syntax or ""))' \
  +qa
```

For Neovim indentation changes, run:

```sh
python3 build/check_nvim_indent.py
```
