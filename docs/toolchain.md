# LLVM toolchain and `nerd doctor`

Nerd generates LLVM IR and invokes LLVM tools directly to produce native output.
It does not invoke Clang. Building Nerd itself from C still requires Clang;
compiling optional C output is a separate action performed by its consumer.

Run:

```sh
nerd doctor
```

The command reports each unavailable tool, checks host SDK configuration, and
then optimises, compiles, links and executes a tiny program in a temporary
directory. This tests compatibility with Nerd's embedded runtime as well as
tool availability. It returns zero on success and nonzero on failure, and cleans
its probe files even if the probe fails. It does not install dependencies or
check project-specific external libraries.

## Required tools

Put these tools from a matching LLVM distribution on PATH:

| Purpose | Linux | Windows | macOS |
| --- | --- | --- | --- |
| Release IR optimisation | `opt` | `opt` | `opt` |
| Native object generation | `llc` | `llc` | `llc` |
| Executable/shared-library linking | `ld.lld` | `lld-link` | `ld64.lld` |
| Static archives | `llvm-ar` | `llvm-lib` | `llvm-ar` |

The initial Linux validation uses LLVM 22.1.8. `doctor` validates the executable
pipeline rather than assuming that a version number guarantees compatibility.
Some LLVM packages ship Clang without `opt` or `llc`; install the LLVM tools and
LLD packages as well. Debug builds do not run `opt`; `doctor` checks the complete
toolchain, including release and archive tool availability.

`llc -filetype=obj -relocation-model=pic` emits native objects. Debug uses `-O0`;
release first runs `opt -passes=default<O2>` and then uses `llc -O2`.
These are separate optimisation stages; `llc -O2` alone does not replace the IR
optimisation pipeline. See the official [opt](https://llvm.org/docs/CommandGuide/opt.html)
and [llc](https://llvm.org/docs/CommandGuide/llc.html) references.

## Host runtime requirements

Linux requires the native libc development/startup files (`Scrt1.o`, `crti.o`,
`crtn.o`, libc and libm), the dynamic loader, and `libgcc_s.so.1` for compiler
runtime helpers. The latter is a runtime library; Nerd invokes no GCC tools.
Common x86-64 and AArch64 library layouts are searched directly. Nonstandard
layouts can set `NERD_CRT_DIR` and `NERD_DYNAMIC_LINKER` to the CRT library
directory and dynamic loader path. Executables are linked as PIE; executable and shared-library links use the PIC
runtime object. Additional FFI libraries must be installed
in the native linker search paths.

Windows requires Visual Studio C++ runtime libraries and the Windows SDK. Run
Nerd in a developer environment with `LIB` configured for the native target.
`lld-link` uses those paths; Nerd explicitly selects the static CRT libraries
(`libcmt`, `libvcruntime`, `libucrt`, `oldnames`) and `kernel32`. Console/windowed
selection remains controlled by the Nerd program. Library exports still produce
the platform import library through LLD.

macOS requires an Apple SDK discoverable through
`xcrun --sdk macosx --show-sdk-path`, plus LLVM's `ld64.lld`. The linker is
configured for the host x86-64 or arm64 architecture and macOS 11.0 minimum.
Apple's SDK supplies `libSystem`.

Native Windows and macOS validation is still required; the initial integration
tests were run on Linux. Other hosts currently report an explicit unsupported
linking configuration. Cross-compilation is not provided by these environment
settings.

## Output and diagnostics

`--obj` stops after object generation. `--lib` archives the generated object and
Nerd runtime. `--dll` invokes the platform LLVM linker in shared-library mode.
On successful builds intermediate `.obj.o` and `.opt.bc` files are removed;
failed builds retain intermediates for diagnosis. Tool failures include the
command and captured output, including when a tool is missing from PATH.

`nerd check`, `--cgen`, and `--copts` require no binary toolchain. C emission is
a compatibility path, for example for platform-specific development toolchains.
`--copts` continues to print Clang-compatible options for consumers that choose
that C compiler; Nerd does not execute it.
