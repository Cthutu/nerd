# Single-core completion plan

2026-09-22. Active follow-up to the withdrawn dependency-scheduler experiment.
G3–G6 remain deferred. Normal binaries use LLVM tools, never a Clang subprocess.

| Milestone | Work | Gate | Status |
| --- | --- | --- | --- |
| S1 | Hash symbol membership during LLVM combination | Exact output parity and paired timings | Complete (`39c7d26e`) |
| S2 | Resolve the remaining 6,000-term arithmetic depth failure in semantic analysis and subsequent stages | Debug/release and sanitizer compilation, runtime/order checks, jobs 1/4; unchanged existing diagnostics | In progress |
| S3 | Profile semantic cost and remove measured avoidable work; investigate the single-module regression | Paired pre-change comparison, output/diagnostic parity, representative corpus | Pending |
| S4 | Run full correctness, sanitizer and native-runner preparation gates | Fixture/integration suites; explicit native-platform limitations | Pending |
| S5 | Final original/current and turn-start/current benchmarks; worker sweep and scaled temporary projects | Alternating repeated samples; LLVM identity where comparable; runtime checks, CPU/RSS, full raw results and report | Pending |

Preserve the original installed compiler. Generated large-project inputs remain
in a temporary folder. Commit/push each validated implementation milestone and
the final evidence. Do not change the worker default without its existing
performance/adoption gates. Windows/macOS execution requires those native hosts;
prepare and document their checks without claiming Linux results as native ones.
