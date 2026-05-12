# Nerd Neovim Support

This directory contains the repo-owned Neovim runtime files for Nerd.

The `install-nvim` recipe copies them into the platform Neovim configuration
directory. On Linux/macOS this is `~/.config/nvim`; on Windows this is
`%LOCALAPPDATA%/nvim`.

- `lua/plugins/nerd.lua` -> `~/.config/nvim/lua/plugins/nerd.lua`
- `ftdetect/nerd.vim` -> `~/.config/nvim/ftdetect/nerd.vim`
- `syntax/nerd.vim` -> `~/.config/nvim/syntax/nerd.vim`

The LazyVim plugin config:

- registers the `nerd` LSP for `.n` files
- configures `conform.nvim` to run `nerd format` against Conform's temporary
  file

Tree-sitter support is intentionally deferred until a later editor milestone.

`build/check_editor_integrations.py` verifies these paths and checks that the
configured LSP command remains `nerd lsp`.
