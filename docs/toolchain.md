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

Linux runtime-only executables use Nerd-owned startup/TLS and need no CRT
objects, loader, libc, libm or libgcc at execution time. This path is validated
on native x86-64. The compiler executable itself remains hosted.

Source FFI requirements can select PIE, the hosted
runtime, native libc startup files (`Scrt1.o`, `crti.o`, `crtn.o`), the
dynamic loader and runtime libraries. Common x86-64/AArch64 layouts are searched;
nonstandard installations can set `NERD_CRT_DIR` and `NERD_DYNAMIC_LINKER`.
Doctor reports these as optional hosted dependencies and proves its independent
link/run path even when they are unavailable. FFI library discovery/validation
still happens in the hosted compiler. Building Nerd itself, or compiling its
generated C, still requires the appropriate C development headers.

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

**Runtime-only Linux x86-64 programs are libc-free**, including standard-library
files, threads/synchronisation, maths, sockets and process launching. This means
independent executable code, not a statically linked libc. The compiler remains
hosted; native AArch64 validation and broader ABI/accuracy hardening are still
outstanding. This branch remains experimental.

```sh
nerd build main.n                      # No libc unless source FFI requires it
nerd build --cgen main.n
clang main.c $(nerd build --copts main.n) -o main
```

Every ordinary LLVM executable build first links the independent runtime, using section
garbage collection and `--as-needed` so unused foreign declarations do not
create runtime dependencies. It inspects the result for `DT_NEEDED`. Dynamic
foreign-library or unsupported relocation requirements select a fresh hosted
link/runtime instead; native
and foreign libc TLS are never mixed. A failed native link can take that path
only when HIR references foreign functions outside the runtime-owned ABI. An
accidental libc import from generated code or the runtime fails linking rather
than silently selecting libc. There are no startup-mode command-line switches.

Recommended C options make a conservative HIR callable-reference check: calls
outside the runtime-owned ABI select hosted compilation. Runtime-only options
include `NRT_LINUX_EXECUTABLE=1`, `NRT_FREESTANDING=1`, local-exec TLS,
freestanding compilation, stack protection and `-nostdlib -no-pie`. Foreign
functions in unreachable source bodies can make this query more conservative
than LLVM's final section collection. Source FFI, not a compiler option, controls
whether libc is permitted. Manually compiling generated C without its reported options retains
the hosted path. Object/archive/shared-library outputs also remain hosted.

### Executable substrate

The standalone executable is fixed-address **ET_EXEC**, not PIE. Its objects
can be PIC, but all relocations are resolved at link time; there is no unowned
static-PIE relocation phase. `_start` preserves the initial kernel stack,
aligns the call stack and captures argc/argv/envp and auxv before constructors.
`AT_PAGESZ` must occur exactly once, be nonzero and be a power of two: no guessed
page size and no `/proc` lookup. Copied `NO_COLOR`/`TERM` flags survive
constructor environment mutation; terminal capability is checked when reporting.

Startup reads `PT_TLS` through `AT_PHDR`, copies the initial TLS image into
fresh zero-backed storage, honours TLS alignment (including alignment larger
than a page), installs the thread pointer, and seeds the canary from
`AT_RANDOM`. x86-64 uses local-exec TLS, `arch_prctl(ARCH_SET_FS)` and the
ABI guard at FS:40; AArch64 has its own thread-pointer/global-guard path.
Link-resolved RELRO tables are protected before preinit/init arrays run; fini
arrays run in reverse after a
normal main return. Normal and fatal process exits use kernel `exit_group`.
Startup has no GNU libc constructor-convention dependency. Hosted parser/I/O
oracles supply explicit metadata without a preinit bridge.

The runtime owns byte/string primitives and compiler memory helpers; allocation,
descriptor I/O, terminal detection and fatal/signal termination use raw syscalls.
The hidden stack-check hook remains enabled and reports corruption before
SIGABRT. The embedded executable runtime is built with strong stack protection.
Normal calls cannot run with uninitialised TLS/canaries.

Threads use `clone`, independent initial TLS images, guarded 8 MiB usable stacks,
kernel TID-clear/futex joins and runtime-owned mutex/condition state. Joins wait
for kernel thread exit before unmapping TLS/stack/control storage. Public
`os.linux` calls retain -1/error-code semantics through runtime-owned per-thread
errno; hosted calls continue to use libc errno. Socket bindings use syscalls;
`execvp` owns PATH lookup, permission-error precedence and ENOEXEC shell fallback.
The shell/executable deliberately launched by an application may itself use
libc; that is not linked into the Nerd process.

Linux float and variadic formatting is runtime-owned, locale-independent and
round-to-nearest/ties-to-even. Binary64 decimal conversion uses an exact integer
coefficient before rounding. Integer/pointer/string/character/count and
`f/e/g/a` conversions, flags, widths, precisions and `*` are supported.
Positional, wide-character and long-double formats are not supported; invalid
formats return an empty result through `VaList.format`. See
[string runtime](string-runtime.md). Standalone maths uses hardware square root,
Payne–Hanek full-range reduction and runtime trigonometric/inverse functions;
it is not advertised as a correctly-rounded replacement for every libm/fenv API.

Output is unbuffered, retries EINTR/short writes and retains normal kernel
SIGPIPE policy. Runtime line input handles LF/CRLF/CR with at most one pushback
byte. Foreign FILE buffering must be synchronised explicitly. Fatal exits skip
foreign atexit/buffer flushing. Hosted normal returns retain CRT shutdown.
External X11/GL/Vulkan and other FFI libraries may retain libc dependencies;
Intentional LD_PRELOAD tests use an actual source FFI libc call to request that
foreign ABI; there is no command-line override.
Windows/macOS keep their existing hosted startup.

### Acceptance and remaining validation

`just test --suite runtime` includes `build/test_libc_free.py` for each compiler
profile. It runs LLVM/C debug/release programs and rejects any ELF `PT_INTERP`,
`DT_NEEDED` or undefined symbols. Plain hello-world builds are inspected without
special startup options; direct and imported FFI libc calls are separately checked
for successful hosted linking. A simulated generated libc import with no source
FFI must fail rather than retry against libc. The gate also proves argument/status handling, binary files,
formatting, maths, TLS/errno isolation, joins, PATH errors and script execution.
A no-CRT C bootstrap probe additionally checks constructors/destructors,
over-aligned TLS, actual compiler canary failure, SIGABRT and worker-wide fatal
termination at O0/O2. Failed acceptance artefacts are retained under `_tmp`.

Hosted foundation oracles still check byte/VM/I/O contracts, auxv errors,
interruptions, SIGABRT dispositions and symbol boundaries. New differential
formatting tests compare thousands of binary64 patterns/precisions against the
host formatter; maths checks full-exponent random values and IEEE edge cases
against a separately loaded libm (8e-15 relative comparison, not an exhaustive
accuracy proof). Network/Nexus execution remains opt-in; their normal binaries
use independent startup, while preload fault-injection subjects explicitly use
hosted startup. Native AArch64, alternative C SDKs/libcs, resource-exhaustion
injection and exhaustive numerical/fenv compatibility remain separate gates.

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
