# Nerd

Nerd is a small compiler toolchain and language server implemented in C.

The codebase is split into a few major areas:

- `src/compiler` contains the compiler pipeline
- `src/lsp` contains the language server
- `build/test.py` contains the repository regression test runner
- `build/test_install.py` contains installed-compiler smoke checks
- `build/check_editor_integrations.py` checks VS Code/Neovim/LSP wiring
- `tests` contains language, error, formatter, and LSP tests
- `docs` contains developer-facing documentation

Start here if you are new to the repository:

- [docs/README.md](/home/matt/nerd/docs/README.md)
- [docs/overviews/INTERNALS.md](/home/matt/nerd/docs/overviews/INTERNALS.md)
- [ROADMAP.md](/home/matt/nerd/ROADMAP.md)
- [tests/README.md](/home/matt/nerd/tests/README.md)

Useful commands:

```sh
just test
python3 build/test_install.py --nerd _bin/nerd-debug
python3 build/check_editor_integrations.py --nerd _bin/nerd-debug
python3 build/build.py
./_bin/nerd --help
```

LLVM backend status:

- `nerd build --llvm source.n` writes the HIR-derived LLVM IR sidecar.
- `nerd run source.n` and `nerd build source.n` compile executables with the
  LLVM backend.
- The previous IR/C backend has been removed; HIR is the compiler middle layer
  and LLVM IR is the executable backend output.
- The executable backend targets the host 64-bit clang toolchain. Cross-target
  and aggregate FFI ABI support are future layout-context work.
