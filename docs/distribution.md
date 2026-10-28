# Distribution packaging

Run `just dist` to build `_dist/nerd.zip` from this checkout. It contains:

- The release compiler (`nerd.exe` on Windows, `nerd` elsewhere).
- `docs/learn/`: the learning guide and its supplements.
- `mods/`: the standard library.
- `llvm-tools.json`: the LLVM Scoop install manifest.
- `examples/`: example programs and their tracked assets.

Developer documentation, root repository documents and the VS Code extension
are excluded. Links from the packaged learning guide to omitted reference
material point to the corresponding GitHub revision; links between included
chapters remain local. Source Markdown and code fences are preserved.

The command uses the normal release compiler build and needs its build
dependencies. `just package` builds the VS Code extension separately, requiring
Node.js and npm. Only tracked module, example and learning files are included;
generated outputs and local verification evidence are excluded.

`python3 build/dist.py --skip-build` packages existing release outputs.
`--output path/to/nerd.zip` selects an alternative archive. A failed build or
packaging operation leaves the previous archive intact.

Extract the archive before use. On Windows with Scoop installed,
`scoop install ./llvm-tools.json` installs the LLVM tools described by the
included manifest. Host SDK and runtime requirements still apply; see
[toolchain setup](toolchain.md). The archive targets its build host and does
not provide cross-platform binaries.
