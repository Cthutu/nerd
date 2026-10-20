# Chapter 23: Building and Configuring Programs

[Previous: Advanced C Interoperability](22-Advanced-C-Interoperability.md)

[Next: Appendix A: Syntax and Pragmas](Appendix-A-Syntax-and-Pragmas.md)

A program's source can describe stable native dependencies, while invocation
options select a particular build. This chapter brings modules, defines, source
build settings, and command-line arguments together in a small program.

## A small multi-module program

Create two files in the same directory. In `greeting.n`, write:

```nerd
pub greet :: fn (name: string) {
    prn($"Hello, {name}!")
}
```

In `main.n`, write:

```nerd
greeting :: use greeting

main :: fn (args: []string) {
    on args.count > 1 => {
        greeting.greet(args[1])
    } else {
        greeting.greet("world")
    }
}
```

Check and build the executable from that directory:

```sh
nerd check main.n
nerd build main.n
```

On Linux or macOS, run `./main Ada`; on Windows, run `.\main.exe Ada`.
This prints `Hello, Ada!`. With no argument it prints `Hello, world!`.
`args[0]` is the executable path supplied by the operating system. User
arguments begin at `args[1]`; the count includes the executable path.

## Selecting a build

Common invocation choices are:

```sh
nerd run main.n
nerd build --release main.n
nerd build -o greeting-app main.n
nerd build --jobs 4 main.n
```

`run` builds and executes the ordinary entry point. `--release` selects the
release profile and its `"release"` define. `-o` chooses an output path;
choose an appropriate executable suffix on Windows. `--jobs` selects compiler
task slots, not application threads. The default is one; `auto` is also accepted.

Use `nerd doctor` to check the LLVM tools and host linking dependencies when
setting up a machine. `nerd build --cgen main.n` produces C instead of an
executable, and `--copts` prints the Clang arguments for that generated C.
`--obj`, `--lib`, and `--dll` select the library output forms introduced in
Chapter 22.

## Source build settings

A top-level `build` block records stable settings with literal entries:

```nerd
build {
    define: tracing
    windowed: no
    library_path: "vendor/lib"
}

main :: fn () {
    on "tracing" {
        prn("tracing enabled")
    }
}
```

This prints `tracing enabled`. The path is a native linker search directory;
this example does not itself require a library in that directory.

| Key | Value | Meaning |
| --- | --- | --- |
| `define` | Identifier | Enable a flag in this module and its parts. |
| `library_path` | Quoted path or `$ENV/suffix` | Add a native library search directory. |
| `windowed` | `yes` or `no` | Select the Windows executable subsystem. |

Blocks may appear between declarations, including in module parts. Putting them
near the beginning makes requirements easy to find. Keys and blocks can repeat;
arrays and executable expressions are not accepted as their values. Unknown
keys and wrong value types are errors even in inactive build branches.

`windowed: yes` selects a Windows GUI-style executable without opening a new
console. Inherited standard streams are preserved, and missing streams can
attach to an existing parent console. Other platforms ignore the setting's
executable effect. Prefer `build` settings to the legacy `pragma windowed`.

## Guards and evaluation order

Build entries use Chapter 17's define guards:

```nerd
build {
    define: tracing
    on "tracing" {
        library_path: "vendor/trace"
    }
    on "windows" {
        windowed: no
    }
}

main :: fn () {
    on "tracing" { prn("trace configuration") }
}
```

Entries are processed from top to bottom. A `define` enables later build
guards, but does not retroactively activate earlier guards. Ordinary code sees
the module's collected defines. Source defines do not propagate into imports
or out to importers; command-line and project-configuration defines apply
across the invocation.

## Resolving paths and native libraries

Quoted relative paths are resolved from the declaring source file, including
when it is a module part. Environment paths expand one variable and append a
literal suffix:

```nerd
build {
    on "windows" {
        library_path: $VULKAN_SDK/Lib
    }
}

main :: fn () {}
```

This example is an SDK configuration: native linking on Windows requires
`VULKAN_SDK` to be set. Use an absolute SDK directory and forward slashes in
the suffix. Spaces in the variable's value remain part of one path. There is
no shell execution, tilde expansion, or recursive variable expansion.

Missing or empty variables are diagnosed when paths are required for native
linking or executable/shared-library `--copts`. Checking, formatting, editor
analysis, `--cgen` alone, and object/archive output do not require the SDK.
On Windows, resolved paths containing quotes, newlines, `%`, or `!` are rejected.

A search path locates a library; an FFI declaration's library operand requests
it. For example, a platform-specific `ffi "tutorial"` binding may be paired
with a path containing that SDK's native library. The path does not locate
Nerd modules and does not configure the runtime loader. Installing a shared
library where the operating system can load it is a separate deployment step.

## Settings from imported modules

Imported modules contribute their build settings automatically. Native library
paths are searched with importers before dependencies and deduplicated without
sorting. Independent modules retain discovery order. This lets a root program
supply a preferred location before a dependency's fallback location.

Within a module, the last active `windowed` entry wins. An importing module can
override a dependency's setting. If sibling imports disagree, specify the
intended value in their common importer; `windowed: no` can override an imported
windowed requirement.

## Project configuration and module lookup

Create `nerd.json` in the directory from which you invoke Nerd to share settings
with the compiler and language server:

```json
{
    "env": {
        "NERD_LIB_PATH": "mods"
    },
    "define": ["tracing"]
}
```

The `env` object sets environment variables before the command runs; relative
values remain relative to the invocation directory. `define` adds invocation-wide
flags, alongside `-Dname`. These settings are also used by editor analysis.

`NERD_LIB_PATH` replaces the bundled Nerd library search path. The example
therefore assumes `mods` contains the library modules the program needs,
including `core`; it does not add `mods` to the bundled roots. If unset, Nerd
uses `mods` beside its executable. If explicitly empty, library lookup is
skipped. The invocation working directory is searched after the selected
library roots, and sibling imports retain their source-relative behaviour.

The default configuration file is optional. Select a required alternative with:

```sh
nerd --config config/development.json check main.n
```

Keep module lookup (`NERD_LIB_PATH`), native linking (`library_path` and FFI
libraries), and runtime shared-library loading distinct when diagnosing a
missing dependency. Use source settings for stable module requirements and
invocation options for a particular build or machine.
