# Native Windows manual checks — 16 October 2026

Status: **BLOCKED — awaiting human observations**.

All 48 desktop builds succeeded at implementation `05c6e023`: Pixels, Dungeon,
and Triangle, each with debug/release compilers, debug/release targets, jobs 1/4,
and LLVM/generated-C backends. See [the matrix](desktop.json) and
[build output](desktop-prepare.log). Successful compilation establishes build
coverage; drawing, input, resize and normal exit have not been observed here.
Human observations were requested during this session; none were supplied.

| Check | Status | Evidence / notes |
| --- | --- | --- |
| Pixels visibly draws/responds; normal exit | BLOCKED | 16 builds pass; human visual/input observations pending |
| Dungeon draws before input, regenerates, Q exits | BLOCKED | 16 builds pass; human terminal observations pending |
| Triangle draws/responds; normal exit | BLOCKED | 16 builds pass; human visual/input observations pending |
| VS Code/CodeLLDB breakpoints, stepping, locals, call stack | BLOCKED | Both automated debugger/editor probes pass; interactive editor observation pending |
| Generated C display/input/exit parity | BLOCKED | All 24 C builds pass; human comparison with LLVM pending |
| Interactive Telnet Ctrl+C shutdown | BLOCKED | All automated CTRL_BREAK_EVENT shutdown checks pass; interactive Ctrl+C observation pending |

To complete the prepared desktop matrix from an interactive PowerShell:

```powershell
python validation/windows/manual.py validation/windows/results/20261016T083710Z-8b918d2c --observe
```

Use `--compiler debug` / `--compiler release` and `--example pixels`, `dungeon`
or `triangle` to work through smaller groups. The helper records an explicit
PASS/FAIL/BLOCKED and notes per case. If ignored scratch artifacts have been
removed, run it with `--prepare` first. Check debugger behaviour with the freshly
built repository compiler/artifacts, following [DEBUGGER.md](../../../../DEBUGGER.md).
Do not infer interactive editor success from the automated probes.

Separately run the Telnet server from an interactive console, connect a client,
exchange text, then press Ctrl+C. Record its command, target/compiler, shutdown
output and exit status. Keep this observation distinct from automated
CTRL_BREAK_EVENT delivery. Update this checklist and the handoff with actual
observations when available. No global compiler or extension was installed by
this validation run.
