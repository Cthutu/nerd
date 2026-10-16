# Native Windows validation — 16 October 2026

**Automation: PASS (65/65 stages). Desktop: 24 observed PASS; interactive checks remain partial.**

Tested implementation: `05c6e0235f622d8b184f1f0e872594d42160f628` on
`std-library`. The checkout was clean at the start and the tested source did
not change during the run. No compiler/runtime/harness fixes were needed;
subsequent commits contain evidence and current-status documentation only.

## Host and tools

Windows 11 Pro x64, build 26200; AMD Ryzen Threadripper PRO 5955WX,
16 physical / 32 logical CPUs; Balanced power scheme. Clang/LLVM/LLDB 23.1.0,
Python 3.14.7, Git 2.51.1.windows.1, just 1.48.0. Visual Studio installation
18.10.12210.168 has MSVC 14.51.36231; installed SDK libraries are 10.0.22621.0
and 10.0.26100.0. No SDK/CRT environment overrides were set; native link/run
probes passed using discovery. See [hardware](hardware.json),
[SDK/CRT inventory](sdk-crt.json), and [tool paths and versions](tools.log).
No dependencies, global compiler, editor extensions or system policies were changed.

## Commands and outcomes

```powershell
python validation/windows/test_runner.py
python validation/windows/run.py
python validation/windows/manual.py validation/windows/results/20261016T083710Z-8b918d2c --prepare
```

The runner self-test passed all six tests; its deliberate failed-build/blocked-
dependent stub is expected harness test output, not a compiler failure.
The full runner passed all 65 stages without a failed or blocked automated stage.
See [the stage report](SUMMARY.md), [machine-readable report](summary.json),
[per-suite counts and skips](suite-counts.json), and
[harness self-test output](harness-self-test.log).

| Suite | Native outcome |
| --- | --- |
| Debug compiler fixtures | 1,200 passed, 0 failed, 15 platform skips |
| Release compiler fixtures | 1,200 passed, 0 failed, 15 platform skips |
| LLVM/C differential fixtures | 304 passed at O0/O2 under each compiler; 2 platform fixture skips each, plus the explicit Linux-PTY Dungeon skip |
| Network | All 8 compiler/backend/target combinations pass |
| Nexus | All 8 combinations pass, including the 33-entry parity inventory, traffic, churn, backpressure, exact recovery and shutdown |
| Thread synchronisation / Raptor | LLVM/C contracts and examples pass under both compilers; Linux-only injection remains skipped |
| Expression depth | Both compilers pass 6,000 terms at jobs 1/4, exact LLVM identity and runtime/operand-order checks |
| Front-end, render and jobs | Both compilers pass deterministic output/diagnostics, generics, scope-query and scheduler regressions |
| Toolchain, temporary install, format workflow, build settings, Windows stdio | Both compilers pass |
| Debugger/editor/adapter | Automated stepping/editor probes pass under both compilers; adapter transforms pass |
| Benchmark | Accounting sanity and all 14 workload/target cells pass, with identical LLVM across jobs 1/2/4/8/16/auto |
| Desktop matrix | 48/48 builds pass; 24 debug-compiler cases observed PASS/exit 0, 24 release-compiler cases unobserved |

Each Nexus configuration checks 96 TCP and 96 Telnet completed connections,
1,152 exact replies per protocol, FIN/reset churn and partial-input shutdown.
Slow-reader checks require observed would-block, healthy-peer progress, exact
recovery and shutdown with output pending. Automation exercises console shutdown
with CTRL_BREAK_EVENT; human Ctrl+C remains a separate check.

The bounded stress runs are not long-duration soak or resource-growth evidence.
Linux-only select/startup/short-I/O injection and Linux PTY checks are retained
as explicit skips; see each log and `suite-counts.json`. Original upstream C
source interoperability was not newly run on Windows: the native runner's
bidirectional golden-wire C peers passed. Earlier Linux source-interoperability
evidence remains separately scoped. Native sanitizers, macOS and new Linux/WSL
runs are not claimed.

## Performance and remaining work

[The benchmark interpretation](BENCHMARK.md) retains five samples per count,
one warm-up and separate profiles, including jobs 16 for physical cores. Auto
has about 1.1% lower geometric-mean wall time here, with mixed cell results;
no cell meets the 15% whole-build gain threshold. Keep the one-job default.
CPU/peak working set describe Nerd only, excluding LLVM/linker children;
wall time includes them. This is not a whole-process-tree memory gate.

[MANUAL.md](MANUAL.md) and [desktop.json](desktop.json) retain the user's 24
explicit desktop PASS observations and zero exit codes, covering all three
examples, both target modes, jobs 1/4 and both backends with the debug compiler.
The user stopped the full matrix because it was repetitive; the remaining 24
release-compiler cases are unobserved. No further duplicate desktop launches
are requested for this pass. Interactive VS Code and Telnet Ctrl+C observations
are still pending. Long-duration Nexus soak/resource growth, DNS/IPv6, graph integration,
and cross-platform adoption remain separate work.

## Linux return

Pull `std-library` and read the newest [handoff](../HANDOFF.md). This Windows run
made no implementation changes, so there is no Windows code fix requiring a
new Linux regression run. Preserve the existing Linux-only injection evidence
and run appropriate Linux regressions when DNS/IPv6 or other implementation
work starts. Earlier native results remain dated history; see
[changes since the prior documented full Windows revision](changes-since-last-full.log).
