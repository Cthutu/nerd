# Nerd

Nerd is a small compiler toolchain and language server implemented in C.

The codebase is split into a few major areas:

- `src/compiler` contains the compiler pipeline
- `src/lsp` contains the language server
- `src/testing` contains the test runner
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
