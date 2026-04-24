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

Tree-sitter highlighting and indentation support are intentionally deferred
until a later editor milestone.

## Verification

For language or LSP behaviour changes, run:

```sh
just test
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
