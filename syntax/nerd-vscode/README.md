# Nerd VS Code Support

This directory contains the repo-owned VS Code extension for Nerd.

The extension contributes:

- `.n` filetype registration
- TextMate grammar wiring
- `nerd.restartLanguageServer`
- `nerd.debugActiveFileWithCodeLLDB`
- language-server launch through `nerd lsp`
- settings for overriding the server path and arguments

The default server lookup order is:

1. workspace `_bin/nerd-debug` or `_bin/nerd`
2. user install under `~/.local/bin/nerd`
3. `nerd` from `PATH`

The `install` recipe packages and installs the extension after installing the
compiler and standard modules. `build/check_editor_integrations.py` verifies the
extension metadata and LSP startup contract.

## Debugging

On Linux, install the CodeLLDB VS Code extension and run `Nerd: Debug Active
File with CodeLLDB` from a `.n` file. The command saves the active file, builds
it with the detected Nerd executable, writes the executable under
`.nerd/debug/`, and launches a CodeLLDB `type: "lldb"` session.

After installing or updating the extension, run `Developer: Reload Window` in
open VS Code windows so newly contributed commands and breakpoint support are
loaded.

The build command uses the same Nerd executable lookup as formatting:

1. `nerd.languageServer.path`
2. workspace `_bin/nerd-debug` or `_bin/nerd`
3. user install under `~/.local/bin/nerd`
4. `nerd` from `PATH`

For a checked-in `.vscode/launch.json` workflow, use CodeLLDB directly and let
the Nerd command build the active file:

```json
{
    "type": "lldb",
    "request": "launch",
    "name": "Debug Nerd Active File",
    "program": "${command:nerd.buildActiveFileForDebug}",
    "args": [],
    "cwd": "${workspaceFolder}",
    "sourceLanguages": ["c"]
}
```

This is the current Linux-first bridge. A future Nerd-owned `type: "nerd"`
debug configuration still needs a real adapter or a proven delegation strategy.
