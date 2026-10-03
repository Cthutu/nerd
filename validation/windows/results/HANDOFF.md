# Windows return handoff

## Linux integration completed — 2026-10-03

The audit/consolidation history and optional/result fix are now integrated into
`std-library`, along with Linux repair `209e8857` for a declaration-inference
use-after-free and LSP fixture path expansion. Native Linux `just test` and
`just test-release` passed: 1,153 fixtures, zero failures, nine platform skips
per compiler; 291 C differential fixtures passed at O0/O2 with zero skips.
The full frontend AddressSanitizer suite and three thread/network examples pass.
See [current Linux handoff](../../../docs/linux-handoff-2026-10-03.md) and
[retained evidence](../../linux/results/20261003-test-suite-consolidation/README.md).

Windows results below cover their recorded revisions; the new Linux inference
repair has not been validated on native Windows. Statements about separate or
unmerged branches in the October 2 sections describe that day's state.

## Complete Linux pickup — 2026-10-02

Start with [today's consolidated Linux handoff](../../../docs/linux-handoff-2026-10-02.md).
It covers every review decision and implementation change on `std-library`
through `6bc00639`, validation boundaries, Linux commands, branch backups, and
the independent audit/consolidation work. The consolidation branch also has an
optional/result enum-materialization fix not yet on `std-library`; fetch its
latest evidence before integration. Repository `main` has not been changed.

## First test consolidation — 2026-10-02

Separate branch **`test-suite-consolidation`**, tested implementation **`caaef691`**;
not merged into `std-library` or `main`. Removed 33 exact command/language runtime
duplicates and replaced 14 additional `on` command regressions with one labelled
language batch. Dedicated CLI/cleanup contracts, existing HIR/LLVM assertions,
all diagnostic cases and inherited integer destination-range fixes remain intact.

Batching exposed a compiler bug: materialising an enum with an equivalent earlier
plex payload lost optional/result flags. **`58818307`** fixes it and covers both
wrapper forms in the batch; this prerequisite must accompany the consolidation.

Native Windows **`just test` and `just test-release` passed**: 1,147 fixture passes,
zero failures, 15 platform skips each. Debug common gate: 289 LLVM/C differential
fixtures at O0/O2 pass, two platform skips, Linux PTY skipped; all auxiliary gates
including unchanged 6,000-term depth, concurrency, direct toolchain, temporary
install, library contracts and four Windows stdio modes pass.

The old changed workload (47 command fixtures) measured 10.273 s median versus
0.317 s for its replacement batch with the same fixed compiler, excluding
unchanged language alternatives. This is about ten seconds less repeated work
per debug fixture pass, **not** a full-gate percentage speedup.

See the [report and exact mapping](../../../docs/test-suite-consolidation.md) and
[native evidence](20261002-test-suite-consolidation/README.md). Linux/WSL gates
remain for pickup; no global compiler/editor installation or desktop checks were
performed. Review/merge this branch independently of ongoing standard-library work.

## Integer literal destination-range checks — 2026-10-02

`std-library` rejects out-of-range integer literals during semantic analysis,
including atomic storage and inferred defaults. Signed minima and full u64
magnitudes remain valid in the appropriate types; explicit conversions preserve
large literals before casting. Intentional Windows unsigned handle constants
now use explicit casts. Native Windows: 1193 fixtures pass, zero failures,
15 skips; all 300 LLVM/C differential fixtures pass at O0/O2. Release compiler
range diagnostics, atomic tests and capability/layout probes pass. See
[evidence and limits](20261002-integer-literal-range/README.md).

## Atomic literal inference fixed — 2026-10-02

`atomic[usize] = 7` now receives the element type as its inference context;
the capability probe no longer needs a cast. Native Windows compiler suite:
1192 passed, zero failures, 15 skips; all 300 C differential fixtures passed.
Final atomic tests pass with debug/release compilers. See
[results and the separate existing element-range issue](20261002-atomic-literal-inference/README.md).

## Typed FFI callbacks — 2026-10-02

`std-library` now accepts ABI-compatible function types in FFI signatures.
Windows/Linux thread bindings use typed entry callbacks without raw-pointer casts.
Native Windows compiler suite: 1187 passed, 0 failed, 15 skips; final eight focused
callback tests pass with debug/release compilers. All 299 C differential fixtures
pass, as do Windows LLVM/C and Arch WSL C-output thread tests at O0/O2.
See [evidence and limits](20261002-typed-ffi-callbacks/README.md). WSL native LLVM
remains unavailable because `opt`/`llc` are missing. No global installation changed.

## Standard-library foundations integrated for draft review — 2026-10-01

Branch **`std-library`**, [draft PR #1](https://github.com/Cthutu/nerd/pull/1),
targets `main`; all parallel work and repair branches are merged without
rewriting their published history. `main` is unchanged.

The [review report](../../../docs/stdlib-review-report.md) summarises additions
and prioritises language/compiler, library and workflow recommendations.

Final implementation `2b540cd9` passes native Windows **`just test` and
`just test-release`: 1181 fixture passes, 0 failures, 15 platform skips each**.
All debug auxiliary stages passed, including **298 C differential fixtures at
O0/O2** (2 platform skips), toolchain/temporary-install smoke, 6000-term depth,
concurrency, build settings and Windows stdio. Both recipes execute the new
thread/network contracts and examples. `just format` passed; later formatting
changes are whitespace-only in touched C files. Unrelated pre-existing module
formatting drift was excluded.

Includes `std.thread`, `std.sync`, IPv4 `std.network`, runtime worker cleanup,
allocation bookkeeping protection, capability/source inventories, and runnable
`thread-pipeline`, `network-echo`, `network-datagram` examples. Compiler repairs
preserve imported constant/global semantics, public variable formatting, packed
bit-field writes and void-call effects; failed LLVM lowering now reports errors.
These are first foundations, not completed Raptor/Kerberos/Nexus ports.

Fresh Arch WSL build and bounded generated-C checks also pass on `2b540cd9`,
including runtime/thread/network contracts and the strengthened compiler tests.
Native LLVM there remains blocked by missing `opt`, `llc`, `ld.lld`, `llvm-ar`.
No global compiler/editor install, desktop validation or benchmark run occurred.
Full `just do` and native Linux adoption remain for the receiving PC.

See [exact evidence, failure history and Linux commands](20261001-stdlib-integration/README.md).
On Linux, fetch/check out `std-library`, fast-forward from the configured remote,
then run `just test`, `just test-release` and the three `just run-example` commands.
Run `just do` when ready to test its global installation step.

Raptor/Kerberos source access is now resolved; see the
[pinned inventory](../../../docs/stdlib-source-inventory.md). The inspected Raptor
heads use ASIO rather than a custom work-stealing implementation; the intended
revision/algorithm needs clarification. Kerberos is serial FIFO dataflow.
Upstream execution, reuse/attribution decisions and later milestones remain open.

## Windows `just test` repaired — 2026-09-30

On `main`, starting from `b542a13d`, the normal **`just test` recipe passes** in
this Windows shell without the historical validation environment script:
**1,174 fixture passes, 0 failures, 15 platform skips**, followed by all auxiliary
stages, including 6,000-term depth checks, scheduler/front-end tests, direct LLVM
toolchain, temporary installation smoke, **295 C differential fixtures** at O0/O2
(2 platform skips), build settings, and all four Windows stdio configurations.
`just format` also passes and retains JPEG's explicit result type. A focused
`just test-release` run selects the release compiler correctly; release `doctor`
also passes with linker SDK discovery.

Fixed Windows UTF-8 subprocess/fixture handling, the files fixture's named default
argument, runtime-array return validation and i32 length lowering, formatter
return-type preservation, and doctor's premature rejection of unset `LIB`.
Installed only the already-downloaded `llvm-dwarfdump` into `~/.local/bin` for the
debug-info smoke test. No global Nerd compiler/editor installation changed.

See [failure history, environment and final logs](20260930-just-test/README.md).
This is the requested native Windows test-recipe scope, not a full desktop or
benchmark validation. **Next: validate `just test` / `just do` on WSL and Linux,
and the full `just do` install workflow on Windows.** Full release fixtures were
not rerun in this scoped pass.

## Windowed terminal output — 2026-09-29

On `vulkan`, starting from `26c3982c`, the generated Windows GUI entry point
connects standard streams before initialization/main. Existing pipes/files stay
redirected; missing streams can attach to the parent's console. No console is
allocated, and connected console output appears immediately. Both LLVM and C
backends use the helper. `windowed: yes` and the Just/F7 command remain unchanged.

Fresh Clang debug/release builds and native stdio integration tests pass,
including hidden parent consoles, explicit inherited handles, mixed redirection,
stdin/pipes/files, and detached launches. The previous compiler fails the new
console regression. The Vulkan example prints live instance/device/queue-family
messages in a Windows pseudoterminal and exits 0 after Escape. Direct LLVM
toolchain checks pass. Full fixtures: 1,169 pass, 3 failures, 15 skips; one stale
Vulkan LSP snapshot is repaired and all 212 LSP tests then pass. The same two
pre-existing fixture failures remain. See [evidence](20260929-windowed-stdio/README.md).

The user's existing example edit is preserved and excluded from this commit.
No global compiler/editor installation changed. A repeat manual VS Code F7
observation was requested but has not been received; Linux execution is not
claimed. The new native test is wired into `just test` with a non-Windows skip.

## Vulkan module SDK path — 2026-09-28

After `7c1cd3e9`, `std.vulkan` now contributes `$VULKAN_SDK/Lib` on Windows.
The unchanged `just run-example vktriangle` builds, opens its native window,
initializes Vulkan, selects the RTX 4070 SUPER and exits 0 after an automated
Q key event. Neither `LIB` nor `VK_ADD_LAYER_PATH` was set. Scoop’s supplied
per-user layer-registration script repaired the separate missing validation
layer installation. No command wrapper or global Nerd/editor replacement.
See [launch evidence and environment change](20260928-vulkan-build-settings/README.md).
The user’s Vulkan example edits remain uncommitted. Manual F7 observation and
Linux execution have not been claimed.

## Source build settings — 2026-09-28

Implemented contextual `build` blocks with repeated `library_path`, `windowed`
and module-local `define` settings. Importers precede dependencies for library
lookup and override scalar settings. `std.frame`, documentation, highlighting,
a runnable example and regression coverage are migrated. Legacy pragmas remain
compatible. The user’s existing Vulkan example edits remain uncommitted.

Native Windows: fresh Clang debug/release builds; integration tests, 210
formatter tests on both binaries, 212 LSP tests, direct LLVM toolchain checks
and 294 C differential fixtures pass. Final complete fixture run: **1,170 pass,
2 pre-existing failures, 15 skips**. See [evidence and Linux follow-up](20260928-build-settings/README.md). No global installation was changed.


## Formatter error isolation — 2026-09-28

On `vulkan`, a syntax error in one top-level declaration no longer forces valid
neighbouring functions through token-based formatting. Healthy functions retain
parameter alignment. Four regression fixtures cover surrounding errors,
continuation boundaries, comments and idempotence. The user's example edits
remain separate. See [implementation and checks](20260928-formatter-recovery/README.md).

Final formatter suites: **206 passed each with debug and release**; final LSP:
**211 passed**. The broader run had 1,163 passes, the same two baseline failures,
and 15 platform skips; the final continuation refinement was checked with the
complete formatter/LSP suites. No green full-suite result is claimed.

## Vulkan FFI approach correction — 2026-09-28

The user selected ordinary platform conditionals and a compile-time FFI library
constant. `std.vulkan` retains `VULKAN_LIBRARY` (`vulkan-1` on Windows, `vulkan`
on Linux) and shared declarations. The Windows task override and
`build/run-example-windows.ps1` have been removed: F7 again runs exactly
`just run-example vktriangle` on both platforms. SDK/CRT, Vulkan library and
validation-layer discovery remain development-environment prerequisites; see
README.md. No compiler syntax or machine environment was changed.

The user reported that the earlier wrapper failed in VS Code and rejected that
launch approach. Its automated launch evidence below is historical and does not
establish successful user F7 execution. The two unrelated full-suite failures
remain outstanding. This correction changes task wiring/documentation only;
the platform-specific binding was already implemented and verified.

## Focused Vulkan F7 repair — 2026-09-28

Branch `vulkan`, starting at `06c17eec610b227f41ae26239904a264aab096c2`.
Windows F7 now uses the correct `vulkan-1` loader import library, initializes
SDK/CRT search paths and discovers the SDK validation layer. The actual task
creates a Vulkan instance, selects the RTX 4070 SUPER and exits 0 after Q.
Linux retains `vulkan` linkage. No compiler internals or global settings changed.

See [repair, reproduction and evidence](20260928-vktriangle/README.md).
Focused Vulkan regression passes. Full `just test` stops with **1,160 passes,
2 unrelated failures, 15 platform skips**: runtime-array error case 7 embeds a
Linux-only invalid-IR diagnostic, and the Windows files fixture uses obsolete
positional syntax for a defaulted argument. These remain explicit follow-up
work; this repair does not claim a green full suite. Earlier results below
remain dated history.

## Native single-core follow-up — 2026-09-23

Tested implementation: **`c32eb92dfccbfc64e337b84440d9f906c33fc675`** on
`experiment/task-scheduler-performance`. Started clean at `da00ffae6ed845d860ce6029be1326cac1a742f0`,
fetched `hub` (`git@github.com:Cthutu/nerd.git`) and fast-forwarded 21 commits.
This covers the single-core implementation `6dd4b36e`, Linux evidence `9db6822a`
and the updated native runner. **No compiler, runtime or harness fix was needed.**
This return commit contains evidence/documentation only.

Read the [full run](20260923T084624Z-7d394bf0/SUMMARY.md),
[suite counts and individual skips](20260923T084624Z-7d394bf0/suite-counts.json),
[desktop/editor observations](20260923T084624Z-7d394bf0/MANUAL.md),
[worker measurements](20260923T084624Z-7d394bf0/BENCHMARKS.md) and
[raw benchmark](20260923T084624Z-7d394bf0/benchmark.json).

### Host and reproduction

Windows 11 Pro 10.0.26200 x64; Threadripper PRO 5955WX, **16 physical cores /
32 logical processors**, Balanced power plan. LLVM/Clang 22.1.8 builds Nerd;
MSVC 14.44.35207 supplies headers/CRT and Windows SDK 10.0.26100.0 supplies SDK
headers/libraries. Nerd's binary pipeline uses opt/llc/linker directly.
Python 3.14.7, Git 2.51.1.windows.1, Node 26.8.1/npm 11.10.0;
CodeLLDB 1.12.2 / LLDB 22.1.4-codelldb. See
[environment](20260923T084624Z-7d394bf0/environment.json) for compiler hashes and
[tool probes](20260923T084624Z-7d394bf0/tools.log).

Reused the prior official full LLVM archive under ignored `_tmp/llvm-tools`;
no installation, local Scoop manifest, global compiler/extension replacement or
machine-policy change. The process-local environment sets explicit LLVM PATH,
MSVC/SDK INCLUDE and LIB, avoiding prior Clang header autodiscovery trouble.
From the repository root in PowerShell:

```powershell
. ./validation/windows/results/20260923T084624Z-7d394bf0/environment.ps1
python validation/windows/test_runner.py
python validation/windows/run.py
python validation/windows/manual.py validation/windows/results/<new-run> --prepare
```

The archived [desktop capture helper](20260923T084624Z-7d394bf0/capture-desktop.ps1)
and [isolated editor launcher](20260923T084624Z-7d394bf0/launch-editor.ps1) are
reproducible alongside the recorded commands. Generated executables, objects,
PDBs and editor scratch state remain ignored. This round has no failed run;
previous failures and repairs remain in the dated history below.

### Correctness and observations

- Runner self-tests: **4 passed**. Full fixture suite: **1,116 passed, 0 failed,
  15 declared platform skips** (202 language, 117 errors, 28 HIR, 48 LLVM,
  198 formatter, 198 LSP, 312 command, 1 stdlib and 12 example passes).
- **expression-depth-debug PASS; expression-depth-release PASS**, each at the
  unchanged **6,000 terms**, jobs 1 and 4, exact LLVM comparison, runtime values,
  operand counts and evaluation order. No increased stack, lowered count or skip.
- Both compilers pass all auxiliary checks: doctor, profile, render workers,
  jobs/auto, frontend, toolchain/output modes, install, C, debugger and editor.
  Each frontend suite prints 21 passing check groups, render 11, jobs 6 and
  toolchain 2; these are printed groups, not underlying assertion counts.
  Scope-query regressions include Windows platform conditionals/negation,
  nested declarations, traits, explicit/inferred/function-value generics,
  ordered diagnostics and deterministic LLVM. Clean/memory/threads also pass.
- C parity: **278 fixtures per compiler at both O0/O2**, plus CLI/output-mode
  groups. Each log has two Linux-only fixture skips (varargs and terminal API)
  and a separate Linux-PTY Dungeon skip. Native Dungeon was checked below.
  The 15 main-suite skips are fixture-declared platform exclusions, including
  Linux-specific process/files/ELF/debug-info/terminal/FFI cases; names are retained
  in suite-counts.json. No new exclusions were added.
- **48/48 desktop cases PASS**: Pixels, Dungeon and Triangle, both compilers,
  both target modes, jobs 1/4 and LLVM/C. Agent-reviewed native captures confirm
  rendering, Pixels animation, graphic resizing and Dungeon regeneration; all
  exit 0, including Dungeon Q. Exact commands/hashes and six contact sheets retained.
- Automated LLDB stepping and editor probes pass for both compilers, as do
  adapter transforms and TypeScript extension compilation. **Fresh human VS Code
  workflow observation is BLOCKED (no fresh user result received)**; the isolated
  launcher was supplied.
  Earlier dated user feedback is not reused for this revision.

### Performance, limits and Linux return

The accounting sanity check passed: 0.125 CPU seconds and 45,002,752 bytes peak
working set for a known 0.1-second CPU workload with a 32 MiB allocation.
Worker sweep: jobs **1/2/4/8/16/auto**, one warmup and five rotating unprofiled
samples per cell, seven scenarios and both target modes; separate profiles and
identical combined LLVM hashes. Jobs 16 is the physical-core count; auto's
half-logical-CPU ceiling is also 16, with workload-dependent phase budgets.
Best measured gain is 6.3% for Pixels debug at four workers; auto ranges from
5.9% faster to 4.1% slower. No row reaches the 15% adoption improvement threshold.
See the measurement report for medians and interpretation.

**Accounting scope is compiler-process-only**: CPU/peak working set exclude
LLVM/linker children, while wall time covers the full build. This does not close
the whole-process-tree memory gate, equal Linux waited-child accounting or
establish a paired before/after Windows serial speedup. Native Windows sanitizers
were not run (optional); macOS remains a separate native gate. M8 is not fully
validated and **jobs=1 remains the default**. G1/G2 remain withdrawn and G3–G6
deferred.

Linux next action: pull this branch and read this section plus the linked run.
There are no new implementation changes requiring Linux repair regressions;
retain the existing Linux results without re-labelling them as Windows evidence.
The remaining work is fresh Windows editor observation, native macOS validation
and any whole-tree memory/paired-baseline adoption work. Continue only the agreed
follow-up scope; do not restore the withdrawn scheduler or change the default.

## Previous native result — 2026-09-22

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

Linux M10 completion: the full suite and ASan/TSan passed with adaptive dispatch.
The retained policy uses parse/HIR/render grains 16384/2048/1024. Nine-sample
unrestricted Linux results give Pixels debug about 5% improvement, far below the
15% adoption target; omitted jobs remains one. M12's optimistic dependency-graph
model finds only about 1% module-level whole-build headroom on Pixels and below
0.1% on Dungeon. Its module-level ownership implementation was not adopted;
see the feasibility decision in the M12 design. Do not report these native
validation gates as passed until the updated runner actually runs on Windows.

## Dependency scheduler G1/G2 withdrawn (2026-09-22)

G1/G2 were removed after the paired Linux benchmarks failed to establish a
consistent benefit over the previous batch implementation. G3–G6 are deferred.
The historical plan and validation remain under `review/audits/compiler-task-graph.md`
and its linked measurements. Do not restore the dependency pool or run its removed
`task-graph` suite as part of native follow-up.

The full Windows runner again uses the established batch scheduler and its core
thread tests. Earlier adaptive-dispatch validation remains applicable and pending
on native Windows as described above. Run the current full runner and retain
native results; Linux validation does not satisfy that gate.

## Single-core lookup and arithmetic-depth follow-up (2026-09-22)

LLVM text symbol membership now uses hash maps; input order still determines
emission. The arithmetic emitter uses explicit frames for nested eager binary
operators. The runner adds expression-depth checks to both compiler builds:
512 terms for debug, 6,000 for release, with jobs=1/4 and runtime/order/IR parity.
Linux release handles the original 24,000-function stress project; native
Windows/macOS results are still pending.

There is a separate known semantic-inference stack limit: the Linux debug and
ASan compilers overflow in `sema_infer_node_type` with the 6,000-term source,
before LLVM emission. The 512-term test passes there. Do not interpret this LLVM
fix as general elimination of compiler recursion, or hide any native depth
failures by claiming the large case passed. Preserve failures for follow-up.

## Frontend depth fix supersedes the preceding limit (2026-09-22)

Semantic arithmetic inference and HIR lowering now use explicit traversal frames.
The default expression-depth test is 6,000 terms for both debug and release; ASan
and debug pass that source on Linux with jobs=1/4. The earlier semantic stack
failure above is historical for this source. Native Windows/macOS must still
run these checks; do not infer native success from Linux results.

## Semantic scope queries (2026-09-22)

Scope membership now uses completed parser side tables for conditionals, traits
and generic implementations; function-body queries search backwards. Run the
full current runner to cover platform keyword/negation, nested declarations,
generics and diagnostics. Linux output parity and tests pass; native evidence
for this change is still required. Scheduling defaults are unchanged.

## Linux single-core completion and next native gate (2026-09-22)

S1–S5 are complete on Linux at implementation `6dd4b36e`; see the
[final report](../../../review/measurements/compiler-single-core-final.md).
Full fixtures/integrations, ASan and TSan pass, including the 6,000-term source
with jobs=1/4. All twelve scaled cases match LLVM and runtime results.
Run the current Windows validation prompt/runner after pulling, retain results
here and commit/push fixes and evidence as its instructions specify. Native
Windows/macOS success is not implied by the Linux report. Keep omitted jobs=1;
the worker sweep does not meet the automatic-default adoption gate.
