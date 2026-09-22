# Single-core completion plan

2026-09-22. Active follow-up to the withdrawn dependency-scheduler experiment.
G3–G6 remain deferred. Normal binaries use LLVM tools, never a Clang subprocess.

| Milestone | Work | Gate | Status |
| --- | --- | --- | --- |
| S1 | Hash symbol membership during LLVM combination | Exact output parity and paired timings | Complete (`39c7d26e`) |
| S2 | Resolve the remaining 6,000-term arithmetic depth failure in semantic analysis and subsequent stages | Debug/release and sanitizer compilation, runtime/order checks, jobs 1/4; unchanged existing diagnostics | Complete on Linux; native checks pending |
| S3 | Profile semantic cost and remove measured avoidable work; investigate the single-module regression | Paired pre-change comparison, output/diagnostic parity, representative corpus | Complete on Linux |
| S4 | Run full correctness, sanitizer and native-runner preparation gates | Fixture/integration suites; explicit native-platform limitations | Complete on Linux; native execution pending |
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
Implementation: `6dd4b36e`. Native Windows/macOS validation remains pending and
does not block publication of the explicitly Linux-only results. No worker
default change or deferred G3–G6 implementation is included.
