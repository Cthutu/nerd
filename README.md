# Nerd

Nerd is a small compiler toolchain and language server implemented in C.

## Repository Map

- [docs/README.md](docs/README.md)
  Developer documentation, manual, implementation-derived language specs, and
  compiler subsystem notes.
- [src/README.md](src/README.md)
  Main compiler and language-server implementation entry points.
- [tests/README.md](tests/README.md)
  Regression test families and fixture conventions.
- [syntax/README.md](syntax/README.md)
  VS Code and Neovim editor integration ownership.
- [review/README.md](review/README.md)
  Historical architecture review evidence and accepted decision records.
- [ROADMAP.md](ROADMAP.md)
  Current project direction and active priorities.
- [CODEX.md](CODEX.md)
  Repo-specific guidance for Codex sessions.

## Catching Up

If you are new to the repository, read these in order:

1. [CODEX.md](CODEX.md)
   Repo-specific working rules for Codex sessions.
2. [ROADMAP.md](ROADMAP.md)
   Current project direction. Prefer this over older historical notes when they
   disagree.
3. [DEBUGGER.md](DEBUGGER.md)
   Source-level debugger roadmap and current VS Code/CodeLLDB implementation
   status.
4. [docs/README.md](docs/README.md)
   Documentation map. Follow its links into the manual, specs, compiler
   internals, diagnostics, standard library, LSP, editor support, and testing
   docs.
5. [src/README.md](src/README.md)
   Current implementation entry points for the compiler and language server.
6. [tests/README.md](tests/README.md)
   Test harness expectations and fixture layout.

Use the architecture review under [review/README.md](review/README.md) as
historical context only. Read it when a task touches an architectural boundary
or when current docs refer to a settled review decision.

## Task Workflow

- Make the smallest coherent change for the task.
- Update or add tests for changed behaviour, including regression tests for
  discovered bugs.
- Update affected documentation in the same change. For language changes, keep
  the learner-facing manual under [docs/manual](docs/manual/README.md) and the
  implementation-derived specs under [docs/spec](docs/spec/README.md) aligned.
- Prefer `just test` as the full gate; use focussed
  `python3 build/test.py --filter ...` runs while iterating.
- Do not commit unrelated local changes.
- Before committing a completed task, always run the final workflow in this
  order:
  1. `just test`; if it fails, fix the failure and run it again.
  2. `just format`; if it changes files or fails, review/fix as needed.
  3. `just install`; if it fails, fix the failure and run it again.
  4. Commit the work, keeping commits small and describing the task slice in the
     commit message.
  5. Push the completed commit with `git push agent main` when the local
     `agent` remote is configured for the Codex SSH identity.
  6. Report back with the commit hash, verification run, install result, push
     result, and useful next step.

## Generated Files

Repo-level generated directories should start with an underscore, such as
`_tmp`, `_bin`, or `_obj`. This keeps local build and debugger output easy to
identify and clean up. Avoid adding new hidden generated directories such as
`.nerd` in the repository root.

Useful commands:

```sh
just test
just install
python3 build/test_install.py --nerd _bin/nerd-debug
python3 build/check_editor_integrations.py --nerd _bin/nerd-debug
python3 build/build.py
./_bin/nerd --help
```

LLVM backend status:

- `nerd build --llvm source.n` writes the HIR-derived LLVM IR sidecar.
- `nerd check source.n` runs lexing, parsing, and semantic analysis without
  HIR generation, LLVM generation, or linking.
- `nerd run source.n` and `nerd build source.n` compile executables with the
  LLVM backend. `nerd build --obj`, `--lib`, and `--dll` produce host object,
  static-library, and shared-library outputs.
- `nerd init <project>` creates a new Nerd project with `main.n`, a `Justfile`,
  VS Code task/launch files, `.gitignore`, and an initial git commit.
- The previous IR/C backend has been removed; HIR is the compiler middle layer
  and LLVM IR is the executable backend output.
- The executable backend targets the host 64-bit clang toolchain. Cross-target
  and aggregate FFI ABI support are future layout-context work.
