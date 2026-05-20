# Nerd Standard Library

This repository is the working area for the Nerd Standard Library (NSL).
It depends on the Nerd compiler, tools, runtime, and language documentation in
the sibling Nerd repository:

- `~/nerd/README.md`

Read that file first when joining the project. It points to the language
manual, compiler internals, testing documentation, and repository workflow used
when bugs need to be fixed in the compiler, language server, formatter, or
runtime.

## Dependency

The installed `nerd` command must come from the local Nerd project. When a tool
bug is found, fix it in `~/nerd` using that project's workflow:

1. Add or update focused regression tests.
2. Run the relevant test command while iterating.
3. Run the full gate requested by the Nerd project before finalising.
4. Commit the compiler/tooling change.
5. Install the verified compiler with `just install` from `~/nerd`.

After installation, return to this repository and continue standard-library
work with the updated toolchain.

## Workflow

The normal NSL development workflow is:

1. Read `~/nerd/README.md` and the relevant Nerd language or standard
   library documentation before using unfamiliar syntax or APIs.
2. Define the smallest useful API surface for the standard-library behaviour
   being added.
3. Add source-level tests with the Nerd `test` keyword where behaviour can be
   checked in Nerd source.
4. Implement or update the library code.
5. Format each changed Nerd source file with `nerd format`.
6. Run `just check` to compile `main.n`.
7. Run `just test` when tests are present or when behaviour changes.
8. Use `just run` for local manual checks from `main.n`.
9. Commit coherent NSL changes after checks pass.

The `Justfile` currently treats `main.n` as the root source file.

When updating the NSL to the main Nerd repository, the workflow is:

1. From this repository (`~/std`), run `just install` to copy the NSL modules
   into `~/nerd/mods`.
2. Change to the Nerd repository with `cd ~/nerd`.
3. From the Nerd repository (`~/nerd`), run `just do` to run the debug and
   release test suites and install the verified Nerd toolchain.
4. Commit the resulting `~/nerd` changes with a clear commit message.

## Library Direction

The NSL is independent from the standard modules currently shipped by the Nerd
compiler repository. Existing modules may be copied into this repository when
they are brought under the NSL coding standards.

The long-term goal is for Nerd programs to avoid depending on the C library.
Temporary modules may use C FFI while the library is being bootstrapped, but
portable APIs should move towards platform modules such as `os.linux` and
`os.windows` backed by native operating-system calls.

Memory allocation APIs should accept call-site source metadata through default
parameters using `@file` and `@line`. Callers should normally use the short form,
such as `alloc(size)`, while the implementation receives the source filename and
line number for future diagnostics.

## Coding Standards

Coding rules for this repository live in
[`CODING_STANDARDS.md`](CODING_STANDARDS.md). Follow them for every NSL source
file unless a task explicitly updates the standards, including the requirement
to use British English spelling.
