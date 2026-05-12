# Nerd VS Code Support

This directory contains the repo-owned VS Code extension for Nerd.

The extension contributes:

- `.n` filetype registration
- TextMate grammar wiring
- `nerd.restartLanguageServer`
- language-server launch through `nerd lsp`
- settings for overriding the server path and arguments

The default server lookup order is:

1. workspace `_bin/nerd-debug` or `_bin/nerd`
2. user install under `~/.local/bin/nerd`
3. `nerd` from `PATH`

The `install` recipe packages and installs the extension after installing the
compiler and standard modules. `build/check_editor_integrations.py` verifies the
extension metadata and LSP startup contract.
