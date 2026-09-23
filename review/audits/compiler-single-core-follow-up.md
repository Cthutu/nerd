# Single-core completion plan

2026-09-22. Active follow-up to the withdrawn dependency-scheduler experiment.
G3–G6 remain deferred. Normal binaries use LLVM tools, never a Clang subprocess.

| Milestone | Work | Gate | Status |
| --- | --- | --- | --- |
| S1 | Hash symbol membership during LLVM combination | Exact output parity and paired timings | Complete (`39c7d26e`) |
| S2 | Resolve the remaining 6,000-term arithmetic depth failure in semantic analysis and subsequent stages | Debug/release and sanitizer compilation, runtime/order checks, jobs 1/4; unchanged existing diagnostics | Complete on Linux and Windows; macOS pending |
| S3 | Profile semantic cost and remove measured avoidable work; investigate the single-module regression | Paired pre-change comparison, output/diagnostic parity, representative corpus | Complete on Linux |
| S4 | Run full correctness, sanitizer and native-runner preparation gates | Fixture/integration suites; explicit native-platform limitations | Linux and Windows automated correctness complete; Windows editor observation and native sanitizers outstanding, macOS pending |
| S5 | Final original/current and turn-start/current benchmarks; worker sweep and scaled temporary projects | Alternating repeated samples; LLVM identity where comparable; runtime checks, CPU/RSS, full raw results and report | Complete on Linux |

Preserve the original installed compiler. Generated large-project inputs remain
in a temporary folder. Commit/push each validated implementation milestone and
the final evidence. Do not change the worker default without its existing
performance/adoption gates. Windows/macOS execution requires those native hosts;
prepare and document their checks without claiming Linux results as native ones.

## Completion evidence

All S1–S5 work available on this Linux host is complete. The
[final benchmark report](../measurements/compiler-single-core-final.md) contains
original/current and turn-start/current comparisons, the full worker sweep,
all twelve scaled cases, CPU/RSS measurements and archived raw evidence.
Implementation: `6dd4b36e`. The original measurements remain explicitly
Linux-only; see the dated native Windows addendum below. macOS remains pending. No worker
default change or deferred G3–G6 implementation is included.

## Native Windows follow-up — 2026-09-23

Tested `c32eb92dfccbfc64e337b84440d9f906c33fc675`, incorporating `6dd4b36e`.
See the [Windows handoff](../../validation/windows/results/HANDOFF.md) and
[full run](../../validation/windows/results/20260923T084624Z-7d394bf0/SUMMARY.md).
S2 passes with both Clang-built debug/release compilers at the unchanged 6,000
terms and jobs 1/4: exact LLVM equality, runtime values, operand counts and order.
No stack enlargement, test reduction or compiler repair was needed.

S4 native automated correctness passes: 1,116 fixtures, zero failures, 15 declared
platform skips, complete frontend/worker tests with both compilers, 278 C parity
fixtures per compiler at O0/O2, debugger/editor probes and 48 desktop cases.
Scope-query coverage includes platform conditionals and negation, nested
functions/blocks, traits, generics and ordered diagnostics. A fresh user-observed
VS Code workflow remains outstanding; earlier dated observations are not reused.
Native Windows sanitizers were not run (optional this round); Linux sanitizer
results do not substitute for them. Native macOS remains pending.

S5 gains a current-revision Windows jobs 1/2/4/8/16/auto sweep, including the
16-physical-core count, five samples per cell and exact LLVM hash parity.
See [measurements](../../validation/windows/results/20260923T084624Z-7d394bf0/BENCHMARKS.md).
Windows CPU and peak working set are compiler-process-only, excluding LLVM/linker
children. The accounting sanity probe passed; this does not close the whole-tree
memory gate or provide a paired before/after Windows serial speedup measurement.
The one-worker default and deferred G3–G6 scope remain unchanged.
