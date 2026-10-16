# Native Windows manual checks — 16 October 2026

Status: **PARTIAL — 24 desktop observations PASS; no observed failures**.

The user completed the debug-compiler batch for Pixels, Dungeon and Triangle:
eight cases per example, covering LLVM/generated C, debug/release targets and
jobs 1/4. All 24 were explicitly marked PASS, with exit code zero. Individual
notes were empty; the observations and timestamps are retained in
[desktop.json](desktop.json). Obsolete blocked-reason fields were removed only
from observed PASS entries.

All 48 builds succeeded. The user stopped after the Triangle examples because
the full manual matrix was too repetitive. The remaining 24 release-compiler
cases are unobserved, not failed or implicitly passed. No further duplicate
desktop launches are requested for this pass. Both compilers already pass the
full automated matrix; manual coverage below retains its exact compiler scope.

| Check | Status | Evidence / notes |
| --- | --- | --- |
| Pixels drawing/input/normal exit | PASS, debug compiler | All 8 target/jobs/backend cases explicitly PASS; exit 0 |
| Dungeon drawing/input/normal exit | PASS, debug compiler | All 8 target/jobs/backend cases explicitly PASS; exit 0 |
| Triangle drawing/input/normal exit | PASS, debug compiler | All 8 target/jobs/backend cases explicitly PASS; exit 0 |
| Generated C display/input/exit parity | PASS, debug compiler | All 12 C cases explicitly PASS alongside 12 LLVM cases |
| Release-compiler desktop batch | UNOBSERVED | All 24 builds pass; user stopped repetitive manual matrix |
| VS Code/CodeLLDB breakpoints, stepping, locals, call stack | BLOCKED | Automated probes pass; interactive editor observation not supplied |
| Interactive Telnet Ctrl+C shutdown | BLOCKED | Automated CTRL_BREAK_EVENT checks pass; interactive Ctrl+C observation not supplied |

The remaining distinct interactive checks are VS Code debugging and Telnet
Ctrl+C; their prepared instructions are in ignored `scratch/manual/START-HERE.md`.
These have not been inferred from desktop PASS answers or automated probes.
No global compiler or extension was installed by this validation run.
