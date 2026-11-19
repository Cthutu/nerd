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
directory and dynamic loader path. Executables are linked as PIE. Linux
executable links use a dedicated PIC runtime with the experimental metadata
preinit hook; shared-library links use the hosted PIC runtime. Additional FFI
libraries must be installed in the native linker search paths. Experimental
Linux executable builds currently require GNU libc's preinit calling convention;
musl executable startup is deliberately unsupported until independent startup
replaces that bridge.

Windows requires Visual Studio C++ runtime libraries and the Windows SDK.
`lld-link` can discover installed SDK/CRT libraries when `LIB` is unset. For a
custom installation, run in a developer environment with `LIB` configured for
the native target. `nerd doctor` validates either setup with its actual link/run
probe rather than rejecting an unset `LIB`. Nerd explicitly selects the static
CRT libraries
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

## Experimental Linux libc removal

Linux output is **not yet libc-free**. The runtime owns its byte copying,
overlap-safe moving, comparison, C-string length/equality and decimal integer
conversion. Variadic cursors use the Nerd allocator rather than `calloc`/`free`.
On Linux x86-64 and AArch64, the runtime's virtual-memory operations use raw
`mmap`, `mprotect` and `munmap` syscalls. Standard output/error and line input use
raw `write`/`read`, terminal detection uses `ioctl(TCGETS)`, and status-127 fatal
failures use `exit_group`. Internal signal aborts unblock and deliver SIGABRT to
the calling thread, retrying with its default disposition if a handler returns
or the signal was ignored. If the signal syscalls cannot terminate the process,
the runtime falls back to status 127 rather than returning to invalid state.
Runtime diagnostics have a private string/unsigned integer/width formatter;
user `VaList.format` still uses libc. This private
bridge handles kernel negative error results directly; it does not change the
public `os.linux` libc-style error convention or write libc's `errno`.

Linux runtime output is unbuffered and retries interrupted and short writes.
Other write errors stop the current output operation; the void print API does
not report errors. The kernel's normal SIGPIPE policy remains in effect. Line
input serialises runtime readers, handles LF/CRLF/CR and retains at most one
lookahead byte for CR. Foreign `FILE` streams are separate: callers mixing Nerd
I/O with libc stdio must flush/synchronise the foreign streams explicitly.
Fatal runtime exits skip foreign `atexit` handlers and libc buffer flushing,
including when triggered by a worker. Normal return still follows hosted CRT
shutdown. Windows/macOS retain their existing hosted I/O and exit paths.

The Linux runtime supplies a hidden `__stack_chk_fail` compiler-ABI hook that
reports stack corruption and takes the signal-abort path. Stack protection is
not disabled: canary storage/initialisation still depends on the hosted runtime
(TLS or an ABI-specific guard) and must be owned before startup is freestanding.

Linux executables capture `AT_PAGESZ`, `NO_COLOR` and `TERM` in an ELF preinit
hook, before shared-library and executable constructors can mutate the initial
environment vector. Page size must be present, nonzero and a power of two; there
is no `/proc` dependency or fallback guess. Colour policy uses copied startup
flags, so later foreign `setenv`/`unsetenv` calls do not change it. Terminal
capability is still checked when reporting. The hook uses GNU libc's three-argument
preinit ABI: this is a hosted transition, not independent kernel startup.

On validated Linux x86-64, the executable runtime's remaining direct imports are
`snprintf`, `vsnprintf` and (for PIC TLS) `__tls_get_addr`. Object/static/shared
library runtimes retain `getenv` and `sysconf`: they belong to a foreign host and
must not install an executable preinit hook or assume an initial stack layout.
Other target ABIs can additionally require a canary guard symbol. Executable
startup remains CRT-owned. Linux linking still supplies the runtime libraries
listed above.

For C output, executable `--copts` includes `-DNRT_LINUX_EXECUTABLE=1` to select
the same preinit path. Object/library options omit it. Manually compiling C
without the reported options retains hosted metadata lookup for compatibility. pthreads,
socket/errno bindings, `execvp` and libm also need replacement before the standard
library is independent of libc. External FFI libraries can retain their own
transitive libc dependencies.

`just test --suite runtime` checks the runtime foundation against a hosted test
oracle at `-O0` and `-O2`, debug and release. Linux probes include binary and
redirected I/O, line boundaries/truncation, terminal colour policy, interrupted
reads/writes, full/broken pipes, fatal worker exits, blocked/ignored/handled
SIGABRT and the stack-corruption hook. Stack-protected probe builds verify that
the hook stays defined and hidden in normal/PIC runtime objects, with compiler
stack-check relocations still emitted. Undefined-symbol checks guard against
compiler optimisation reintroducing removed calls. Separate executable-runtime
checks also reject `getenv`/`sysconf` and require a preinit section, while hosted
objects must not contain one. Synthetic auxv tests cover page sizes and invalid
records; a DSO constructor that changes/removes environment entries verifies
capture ordering. LLVM and recommended C invocations inspect actual binaries.
This is a partial boundary,
not an assertion that the whole executable is freestanding. Final acceptance
must inspect executable `PT_INTERP`, `DT_NEEDED` and undefined symbols as well as
run the normal behavioural gates. AArch64's syscall bridge still needs native
validation; this does not broaden the standard library's supported layouts.

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
