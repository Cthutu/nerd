# Nerd

Nerd is a small compiler toolchain and language server implemented in C.

The codebase is split into a few major areas:

- `src/compiler` contains the compiler pipeline
- `src/lsp` contains the language server
- `build/test.py` contains the repository regression test runner
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
python3 build/build.py
./_bin/nerd --help
```

LLVM backend status:

- `nerd build --llvm source.n` writes the HIR-derived LLVM IR sidecar.
- `nerd run --llvm-backend source.n` runs through the opt-in LLVM backend.
- The default run/build path still uses the current stable backend while the
  LLVM path finishes its runtime and default-backend migration.
