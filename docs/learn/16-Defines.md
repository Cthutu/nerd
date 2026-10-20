# Chapter 16: Defines

[Previous: Special Function Declarations](15-Special-Function-Declarations.md)

Defines select code during compilation. They let a program include a platform
API or optional feature without making that selection a runtime decision.
Nerd uses a string-literal form of `on` to test whether a named define is enabled.

## Compile-time `on`

A string literal directly after `on` selects a block when that name is enabled.
Prefix it with `!` to select the block when the name is absent:

```nerd
main :: fn () {
    on "tracing" {
        prn("tracing enabled")
    }
    on !"tracing" {
        prn("tracing disabled")
    }
}
```

Save this as `main.n`. `nerd run main.n` prints `tracing disabled`.
`nerd run -Dtracing main.n` prints `tracing enabled` instead. The `-D` option
adds a define for that compiler invocation; it does not declare a runtime
variable named `tracing`.

This is different from Chapter 9's runtime `on condition => ...`. A string
literal followed by a block has special compile-time meaning. The body must
have valid syntax, but inactive bodies are excluded before name resolution
and type checking. Unknown define names are simply disabled unless enabled.

Defines are presence flags, rather than C-style textual substitutions.
Use ordinary bindings for values such as buffer sizes or version numbers.

## Built-in defines

The compiler supplies defines describing its host platform and build mode:

| Define | Enabled for |
| --- | --- |
| `"windows"`, `"linux"`, `"macos"`, `"bsd"` | The corresponding host operating system. |
| `"posix"` | A POSIX host. |
| `"x64"` | An x86-64 host. |
| `"debug"` | A normal debug build. |
| `"release"` | A release build selected with `--release` or `-r`. |

For example:

```nerd
main :: fn () {
    on "debug" {
        prn("debug build")
    }
    on "release" {
        prn("release build")
    }
}
```

`nerd run main.n` prints `debug build`; `nerd run --release main.n` prints
`release build`. The build-mode define describes the program being built,
rather than whether the compiler executable itself was a release build.

These platform defines describe the supported host toolchain; enabling a
custom name does not turn the compiler into a cross-compiler.

## Selecting declarations

Compile-time `on` also works at module scope:

```nerd
on "windows" {
    platform_name :: "Windows"
}

on !"windows" {
    platform_name :: "another platform"
}

main :: fn () {
    prn(platform_name)
}
```

This prints `Windows` on Windows and `another platform` elsewhere. The enabled
declaration is visible to the rest of its module. The inactive declaration
does not conflict with it.

Use complementary blocks when both configurations need a declaration.
Do not leave a name defined only in an optional block and then use it
unconditionally in configurations where that block is disabled.

## Excluding unavailable code

Inactive code does not need to resolve platform-specific names:

```nerd
main :: fn () {
    on "tutorial_special_api" {
        unavailable_platform_call()
    }
    prn("ordinary program")
}
```

With that define absent, this prints `ordinary program`. Enabling
`-Dtutorial_special_api` makes the compiler check the call and report its
missing declaration. This example deliberately demonstrates the boundary:
disabled code is excluded, while enabled code must be valid Nerd code.

The same rule lets module-scope blocks guard imports and FFI declarations.
An inactive import does not load the module, so it may name an API that is
unavailable on the current platform. An enabled import contributes names to
the enclosing module just like an ordinary import.

## Module-local defines in source

Use a top-level `build` block to add a define for the current module:

```nerd
build {
    define: tracing
}

main :: fn () {
    on "tracing" {
        prn("local tracing")
    }
}
```

This prints `local tracing` without a command-line `-D`. Here `tracing` after
`define:` is an identifier naming the flag, rather than a string expression.
The define applies to ordinary code throughout this module, including its
parts. It does not propagate into imported modules or out to importers.
Command-line defines apply across the compilation instead.

Build entries are processed in source order: a define affects later guards
inside build blocks, but does not retroactively enable an earlier build guard.
Ordinary code sees the module's collected defines. A build block may also
declare native-library search paths, as described below.

## Requiring a define

Use `assert on` at module scope when the entire source file requires a define:

```nerd
assert on "tutorial_feature"

main :: fn () {
    prn("feature required")
}
```

`nerd run -Dtutorial_feature main.n` prints `feature required`. Without that
define, compilation fails with a platform assertion diagnostic. The same
form can require `"windows"`, for example. `assert on !"name"` requires the
name to be disabled.

This is a compile-time requirement, distinct from a runtime assertion.

## Keeping editor and command-line defines aligned

Project configuration can supply invocation-wide defines. In the directory
from which you run Nerd, create `nerd.json`:

```json
{
    "define": ["tracing"]
}
```

The compiler and language server read this configuration, so they can agree
about enabled code. Configuration defines add to command-line defines.
The default configuration is optional; an explicit
`nerd --config path/to/nerd.json run main.n` selects another file and requires
that file to exist.

Use source build settings for a module's stable requirements, project
configuration for shared invocation settings, and `-Dname` for a particular
build. None performs C preprocessor-style token replacement.

## Combining defines with native libraries

Chapter 15's FFI declarations may need a platform-specific library search
path. A source `build` block can select it using the same define syntax:

```nerd
build {
    on "tutorial_sdk" {
        library_path: "vendor/lib"
    }
}

main :: fn () {
    prn("library configuration")
}
```

This prints `library configuration`. The relative path is interpreted from
the declaring source file's directory and contributes to native linking when
the guard is enabled. It does not add a Nerd module search root; module lookup
still uses the rules from Chapter 14.
