# Local Scoop manifests

## LLVM tools (Windows x64)

`llvm-tools.json` installs the official LLVM 20.1.8 full archive and exposes
`opt` (LLVM optimizer), `llc` (LLVM static compiler), and `lld-link` (Windows
linker) through Scoop shims.
The download is approximately 896 MB and needs additional space when extracted.

This supplies tools omitted from the standard LLVM Windows installer package.
This manifest exposes only these three tools, rather than adding the entire
archive's bin directory to PATH. An existing `scoop install llvm` may also
provide `lld-link`; use `Get-Command lld-link` to check which installation runs.

### Install

With Scoop already installed, run PowerShell from this directory (`etc`):

```powershell
scoop install .\llvm-tools.json
```

### Verify

```powershell
opt --version
llc --version
lld-link --version
Get-Command opt, llc, lld-link
```

All three tools should report version 20.1.8. If the commands are unavailable, open a
new PowerShell session and try again. `Get-Command` shows which commands are
being resolved if another installation is taking precedence.

### Version and maintenance

This is a pinned local manifest without automatic update rules. To use another
release, update `version`, the archive URL, its SHA-256 hash, and `extract_dir`
together. Use the LLVM version required by your project; the separately
installed Scoop `llvm` package may use a different version.

- [Official LLVM 20.1.8 release](https://github.com/llvm/llvm-project/releases/tag/llvmorg-20.1.8)
- [Release assets and checksums](https://github.com/llvm/llvm-project/releases/expanded_assets/llvmorg-20.1.8)

### Uninstall

```powershell
scoop uninstall llvm-tools
```

This removes the Scoop installation; the local manifest remains in this folder.
