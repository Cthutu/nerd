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
`_tmp/debug/`, and launches a Nerd `type: "nerd"` session that delegates to
CodeLLDB.

After installing or updating the extension, run `Developer: Reload Window` in
open VS Code windows so newly contributed commands and breakpoint support are
loaded.

The build command uses the same Nerd executable lookup as formatting:

1. `nerd.languageServer.path`
2. workspace `_bin/nerd-debug` or `_bin/nerd`
3. user install under `~/.local/bin/nerd`
4. `nerd` from `PATH`

For a checked-in `.vscode/launch.json` workflow, use the Nerd debug type and let
the Nerd command build the active file:

```json
{
    "type": "nerd",
    "request": "launch",
    "name": "Debug Nerd Active File",
    "program": "${command:nerd.buildActiveFileForDebug}",
    "args": [],
    "cwd": "${workspaceFolder}"
}
```

This is the current Linux-first bridge. The Nerd debug adapter is a small VS
Code shim that delegates process control to CodeLLDB while keeping Nerd-specific
debugger presentation decisions in this extension.

Current Linux debugger support includes source breakpoints, source stepping,
locals, parameters, globals, strings, slices, dynamic arrays, tuples, plexes,
tagged enums, raw unions, and raw pointers. Watch expressions are native
CodeLLDB expressions with Nerd-specific help for known runtime layouts such as
dynamic-array `count`, `capacity`, and `data`; unsupported Nerd-only forms are
reported explicitly instead of being passed through as confusing LLDB errors.
