# Editor Syntax And Integration

This directory contains repo-owned editor support for Nerd.

- [nerd-vscode/README.md](nerd-vscode/README.md)
  VS Code extension metadata, TextMate grammar wiring, command contributions,
  and language-server launch contract.
- [nerd-nvim/README.md](nerd-nvim/README.md)
  Neovim runtime files, LazyVim plugin config, formatter integration, and LSP
  command contract.

The full install workflow packages and installs editor support through:

```sh
just install
```

Use [../docs/editor-support.md](../docs/editor-support.md) for the verification
checklist and ownership notes.
