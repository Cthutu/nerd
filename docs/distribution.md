# Distribution packaging

Run `just dist` to build `_dist/nerd.zip` from this checkout. The archive has
the release compiler (`nerd.exe` on Windows, `nerd` elsewhere), the VS Code
extension, `llvm-tools.json`, `mods/`, `examples/` and `docs/` at its root.
The root README, debugger guide and roadmap accompany the documentation.
Only tracked module, example and documentation files are included; generated
outputs and local verification evidence are excluded.

The command uses the normal release compiler and extension builds. It needs
the compiler build dependencies, Node.js and npm. `just package` continues to
build only the VS Code extension.

`python3 build/dist.py --skip-build` packages existing release outputs.
`--output path/to/nerd.zip` selects an alternative archive. A failed build or
packaging operation leaves the previous archive intact.

Extract the archive before use. Install the `.vsix` through VS Code's
**Extensions → Install from VSIX…** command. On Windows with Scoop installed,
`scoop install ./llvm-tools.json` installs the LLVM tools described by the
included manifest. Host SDK and runtime requirements still apply; see
[toolchain setup](toolchain.md). The archive targets its build host and does
not provide cross-platform binaries.
