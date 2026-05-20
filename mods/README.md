# Nerd Standard Library

The `mods/` directory contains the Nerd Standard Library (NSL) modules shipped
with this repository. These modules are loaded by the compiler like ordinary
Nerd source modules and are used by tests, examples, and installed Nerd
toolchains.

Read the repository root [`README.md`](../README.md) first when joining the
project. It points to the language manual, compiler internals, testing
documentation, and repository workflow used when standard-library changes need
matching compiler, language-server, formatter, or runtime work.

## Repository Context

Standard-library work may expose bugs or missing support in the compiler,
language server, formatter, or runtime. Fix those in this repository using the
normal Nerd workflow:

1. Add or update focused regression tests.
2. Run the relevant test command while iterating.
3. Run the full gate requested by the repository before finalising.
4. Commit coherent compiler, tooling, runtime, or standard-library changes.
5. Install the verified toolchain with `just install` when the installed
   `nerd` command must be updated.

## Workflow

The normal NSL development workflow is:

1. Read the root [`README.md`](../README.md) and the relevant Nerd language or
   standard-library documentation before using unfamiliar syntax or APIs.
2. Define the smallest useful API surface for the standard-library behaviour
   being added.
3. Add source-level tests with the Nerd `test` keyword where behaviour can be
   checked in Nerd source.
4. Implement or update the library code.
5. Format each changed Nerd source file with `nerd format`.
6. Run focused `nerd check`, `nerd run`, or `nerd test` commands against the
   affected examples, tests, or small temporary roots while iterating.
7. Run the repository regression gate with `just test` before finalising
   behaviour changes.
8. Commit coherent NSL changes after checks pass.

## Library Direction

The NSL is the source of truth for standard modules shipped by Nerd. Existing
modules should be brought under the NSL coding standards as they are expanded
or reorganised.

The library is organised into three layers:

- `core`
  Language-support declarations that every module can rely on. `core` is
  implicitly imported by all Nerd modules and owns language-adjacent APIs such
  as canonical traits, core result types, arenas, and temporary allocation.
- `std`
  Platform-agnostic standard-library modules. `std` APIs should provide
  portable behaviour even when their implementation delegates to platform
  modules internally.
- `os`
  Platform-specific modules such as `os.linux` and `os.windows`.
  Platform FFI declarations, syscall wrappers, constants, and operating-system
  details belong here rather than in `std`.

The long-term goal is for Nerd programs to avoid depending on the C library.
Temporary modules may use C FFI while the library is being bootstrapped, but
portable APIs should move towards platform modules such as `os.linux` and
`os.windows` backed by native operating-system calls.

## Coding Standards

Coding rules for this repository live in
[`CODING-STANDARDS.md`](CODING-STANDARDS.md). Follow them for every NSL source
file unless a task explicitly updates the standards, including the requirement
to use British English spelling.
