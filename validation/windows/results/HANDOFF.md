# Windows return handoff

Native Windows correctness validation completed on 2026-09-22. Work is on
`experiment/task-scheduler-performance`, remote `hub` at
`git@github.com:Cthutu/nerd.git`. Starting commit was
`6bbc17e8e14facc95caee55abdca35db216bbb05`, with a clean working tree.
The final tested implementation is **`828d4b56ec08ea7f2001334e33cf660da9d82bbb`**.
Subsequent commits contain validation evidence and documentation only.

Read the [final full run](20260922T084515Z-f15c8c4e/SUMMARY.md),
[suite counts](20260922T084515Z-f15c8c4e/suite-counts.json),
[desktop/editor observations](20260922T084515Z-f15c8c4e/MANUAL.md) and
[benchmark report](20260922T084515Z-f15c8c4e/BENCHMARKS.md).
Keep `--jobs 1` as the default. The broader M8 adoption gate remains incomplete:
native macOS and Windows CPU/RSS measurements are outstanding.

## Host and reproduction

Windows 11 Pro 10.0.26200 x64; AMD Ryzen Threadripper PRO 5955WX,
**16 physical cores / 32 logical processors**; Balanced power plan.
LLVM/Clang 22.1.8, Python 3.14.7, Git 2.51.1.windows.1,
Visual Studio Professional 2022/MSVC 14.44.35207 and Windows SDK 10.0.26100.0.
CodeLLDB 1.12.2 supplies LLDB 22.1.4-codelldb; Node 26.8.1/npm 11.10.0.
Exact compiler hashes and hardware are in
[environment.json](20260922T084515Z-f15c8c4e/environment.json); tool probes are
in that run's `tools.log`.

The installed Scoop Clang 23.1.0 lacked opt/llc. We extracted the official full
[LLVM 22.1.8 Windows x64 archive](https://github.com/llvm/llvm-project/releases/download/llvmorg-22.1.8/clang%2Bllvm-22.1.8-x86_64-pc-windows-msvc.tar.xz)
under ignored `_tmp/llvm-tools`. Its SHA-256 matches the published release digest:
`d96c2cc1736f4eb7fa43cb9bbdf56d93551a9ae0a9aadb9c99c3c3b2b712a234`.
Old Clang 23 caches were moved to `_tmp/clang23-objects` before rebuilding.
No globally installed compiler, extension, toolchain or machine policy changed.

From the repository root in PowerShell:

```powershell
. ./validation/windows/results/20260922T084515Z-f15c8c4e/environment.ps1
python validation/windows/test_runner.py
python validation/windows/run.py
python validation/windows/manual.py validation/windows/results/<new-run> --prepare
```

The checked-in environment script prepends the local full LLVM bin directory,
sets NERD_LIB_PATH and selects the installed MSVC x64, UCRT x64 and UM x64 LIB
directories, plus explicit INCLUDE, VCToolsInstallDir and Windows SDK variables.
The [preceding run](20260922T083831Z-654d9483/SUMMARY.md) passed all 1,116 fixtures
and the desktop matrix, but external Clang later failed to discover stdio.h
despite the installed header being present. The explicit process-local SDK
configuration passed a standalone C probe and the final full run. The reason
automatic discovery stopped working was not established; no system settings
were changed. Use the generated
`desktop.json` commands or `manual.py --observe` for interactive reproduction;
the final run's `capture-desktop.ps1` records native window/input checks for the
prepared matrix. Generated C is compiled externally from `--copts` arguments.
Nerd itself continues to call opt/llc and the native linker/archive tools directly.

## Repairs and retained failures

- `66ad592e`: enum/u32 signedness cast and CRT include ordering. Original failed
  builds: [initial full run](20260922T072040Z-c854dc52/SUMMARY.md) and
  [second build](20260922T072126Z-6c5bc397/SUMMARY.md).
- `c760a1bf`: correct main return width/sign conversion into the Windows exit
  status, direct `vprintf` linking with Microsoft's `legacy_stdio_definitions`,
  and C output suffix handling. Expanded missing-tool/SDK/no-Clang, entry-point,
  DWARF and portable harness checks. Original failures:
  [fresh LLVM 22 run](20260922T072410Z-d0e26691/SUMMARY.md); focused repairs:
  [073444](20260922T073444Z-2a8ab0b9/SUMMARY.md) and
  [073919](20260922T073919Z-3ae286a6/SUMMARY.md).
- `abee4399`: resolve Windows npm.cmd for editor adapter validation and include
  editor checks in the full runner. Earlier editor logs and isolated VS Code
  launcher: [074219](20260922T074219Z-d55d7186/MANUAL.md).
- `a618456b`: retry only native sharing/lock violations when opening generated
  C, bounded to 500 ms; persistent locks still fail. WinError 32 recurred in
  [074219](20260922T074219Z-d55d7186/SUMMARY.md); controlled native-handle
  reproduction and successful focused checks:
  [075157](20260922T075157Z-7bec6ae1/SUMMARY.md). The external lock owner was
  not established; other I/O errors are not retried.
- `828d4b56`: pad dynamic-array headers to 32 bytes in LLVM and compatibility C
  to preserve 16-byte element alignment. The preceding
  [full automated pass](20260922T075856Z-e3a49814/SUMMARY.md) was insufficient:
  native optimized Dungeon input faulted at an aligned SIMD store. Its crash
  records, failed captures and pre-repair benchmarks remain in that folder.
  The new alignment regression checks allocation/growth/reserve and payload
  values in debug/release targets at jobs 1/4 and external C O0/O2. Existing
  LLVM snapshots and one leak-size expectation changed by the eight-byte header
  padding. [Focused repair evidence](20260922T083052Z-6bf97fd5/SUMMARY.md)
  includes successful reruns after an initial regression-source syntax mistake.

Significant compiler changes are documented in `docs/overviews/INTERNALS.md`.
The original imported-generic wide-value cases and semantic ownership protection
remain intact; no Clang fallback, weakened assertions or new broad skips.

## Final correctness results

- Harness self-tests: **4 passed**.
- Full fixtures using the fresh debug compiler: **1,116 passed, 0 failed,
  15 skipped**. Language 202/0/2; errors 117; HIR 28; LLVM 48; format 198;
  LSP 198; commands 312/0/13; stdlib root 1; examples 12.
- Fresh debug AND release compiler auxiliary checks all passed: allocator and
  thread lifecycle, doctor, profile contracts, render ownership, jobs,
  front-end graphs/diagnostics, toolchain/output modes, install, C generation,
  LLDB stepping, editor integrations and adapter transformations.
- Each render suite prints 11 workload passes; each jobs suite prints five
  synthetic passes plus CLI/output contracts. Each front-end suite covers 15
  valid source shapes, five diagnostic cases and six randomized independent
  closure iterations (one aggregate printed group). These are workload counts,
  not the number of assertions.
- Toolchain suites each cover 36 Windows integer-main cases plus four alignment
  cases, console/windowed paths, output modes and isolated missing-tool/SDK
  checks with no Clang on PATH.
- C differential: **278 fixtures per compiler**, each externally compiled and
  executed at **O0 and O2**. C option/output/library/graphics checks and the
  controlled sharing-violation test passed too.
- Native desktop: **48 cases passed** across Pixels/Dungeon/Triangle, both
  compilers, debug/release targets, jobs 1/4 and LLVM/C. Actual client captures
  were inspected, Pixels animation and graphics resize were checked, Dungeon
  drew before input and regenerated, and every automated Q exit returned zero.
  The user also reported that the separate isolated VS Code/CodeLLDB procedure
  seemed to work; detailed automated line/locals transcripts passed for both
  final compilers. See MANUAL.md for the scope of the human observation.

The 15 fixture skips follow existing platform annotations: two Linux language
fixtures (fcntl varargs and terminal implementation) and 13 Linux-only command
contracts. Exact names are in suite-counts.json. The C harness honours the same
two language annotations; its separate Linux PTY Dungeon probe is skipped on
Windows and supplemented by the full native console matrix, not counted as a
PTY pass. Embedded DWARF source/line tables replace an obsolete PDB expectation.
Full-path diagnostic assertions use a fixed wide terminal width.

## Performance and remaining work

Both final-code benchmark sweeps ran after correctness with no concurrent
builds, tests or desktop examples. Each has one warmup and five unprofiled
samples per cell, plus separate profiles and combined LLVM hash checks.
Jobs 1/2/4/8 and the separate jobs 1/16 physical-core sweep retain their own
one-worker baselines. The best representative improvements are 3.2% for Pixels
debug at eight workers and 4.8% at 16 in the separate sweep, below the 15% gate.
Read BENCHMARKS.md for the actual medians and interpretation;
do not mix baselines or claim speedups from the pre-alignment run as final-code
measurements. Default parallel adoption remains deferred.

Windows benchmark CPU time and peak RSS are **null**, not zero. The extra
eight-byte array prefix is a known allocation cost, not a measured peak-RSS
result. No Windows main/M5 paired comparison was performed. macOS remains
unrun, and its SDK/linker, runtime, editor and memory/performance gates are
separate. These limitations prevent marking M8 fully validated.

## Next Linux action

Fetch and pull `experiment/task-scheduler-performance` from the repository's
working remote, then read this handoff and the linked audit. Review the Windows
fixes and run the full Linux `just test` sequence with freshly built compilers.
In particular rerun main-width/output-mode toolchain checks, the new dynamic
enum alignment regression in optimized LLVM and C, array pointer conversions,
allocator/leak diagnostics, profile/jobs/front-end ownership and wide imported
generic regressions. Rerun front-end AddressSanitizer/ThreadSanitizer checks and
native Linux PTY Dungeon interaction. The alignment change affects Linux too;
the Windows-only controlled sharing test is intentionally OS guarded.
Review adoption only with the missing native/memory evidence; retain jobs 1.

## Linux return completed — 2026-09-22

Pulled through `da00ffae`, reviewed the returned fixes and rebuilt both compilers.
The [Linux return validation](linux-return-20260922/SUMMARY.md) passed: 1,122
fixtures, all auxiliary checks, ASan/TSan front-end suites, release toolchain
checks, 280 C differential fixtures per compiler at O0/O2, Linux PTY Dungeon
interaction and debugger stepping with both compilers. No additional Linux fix
was needed. Logs and compiler hashes are retained alongside that summary.
Native macOS and Windows CPU/RSS evidence remain outstanding; jobs 1 remains
the default. The next step is to decide on merging the validated fixes or arrange
the remaining platform/measurement work; no merge into main was performed here.

## Subsequent Linux work: M9 auto ceiling

The adaptive-default plan now adds opt-in `--jobs auto`; omitted jobs still uses
one. See [M9 implementation and validation](../../../review/measurements/compiler-m9-auto-ceiling.md).
The new core discovery and production jobs tests include CPU-affinity restriction,
half-CPU selection, explicit overrides and safe discovery-failure fallback.
They pass on Linux; the earlier Windows evidence does not cover this new code.
The next native run should repeat the full Windows runner, which automatically
includes these tests with both compiler configurations. Primary processor-group
sizing and CPU Sets/job-object budget limitations are documented in INTERNALS.

## Next native run: adaptive dispatch (M10/M11)

Linux implemented work-aware `--jobs auto` with ordered policy profiling and
LLVM/C/error parity checks. Numeric overrides and omitted jobs=1 are preserved.
Run the updated full `validation/windows/run.py`; its benchmark includes
1/2/4/8/16/auto and a new known-workload accounting sanity check. Investigate API
or zero-memory failures rather than accepting missing counters. Windows counters
cover the compiler only; whole-tree CPU/memory remains unmeasured. Review the
new README accounting section and `review/audits/compiler-m12-semantic-ownership.md`.
M12's shared-core exclusion must remain until its ownership redesign is proven.
