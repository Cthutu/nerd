# Native Windows desktop checks — 2026-09-23

Tested commit: `c32eb92dfccbfc64e337b84440d9f906c33fc675`.
Fresh Clang-built debug/release compilers; both generated target modes,
jobs 1/4 and LLVM/external-Clang C backends: **48/48 desktop cases PASS**.
Exact commands and compiler hashes: [desktop.json](desktop.json).
Native capture/input/resize/exit records: [desktop-automated.json](desktop-automated.json).

| Check | Status | Observation |
| --- | --- | --- |
| Pixels | PASS (16 cases) | Colourful animated framebuffer and white bar; frame hashes change; redraw at 960x720; normal exit 0. |
| Dungeon | PASS (16 cases) | Rooms/corridors displayed before input; Space regenerates visibly different maps; Q exits 0. |
| Triangle | PASS (16 cases) | Interpolated RGB triangle on dark background; redraw at 960x720; normal exit 0. |
| Generated C parity | PASS (24 cases included above) | Display/input/resize/exit match LLVM behaviour. |
| VS Code/CodeLLDB workflow | BLOCKED | Isolated launcher supplied to user; no fresh human breakpoint/step/locals/stack observation received for this commit. Automated probes passed for both compilers. |

The agent inspected all six labelled contact sheets and their native client
captures; process survival alone was not used as visual evidence. Input was
sent to the owned console/window; capture helper and logs are retained.
Screenshots contain only the test applications. Scratch binaries are ignored.
The automated debugger/editor probes are separate from the user workflow.
No global compiler or extension was replaced.

Reproduce from the repository root:

```powershell
. ./validation/windows/results/20260923T084624Z-7d394bf0/environment.ps1
python validation/windows/manual.py validation/windows/results/20260923T084624Z-7d394bf0 --prepare
./validation/windows/results/20260923T084624Z-7d394bf0/capture-desktop.ps1
./validation/windows/results/20260923T084624Z-7d394bf0/launch-editor.ps1
```

In the isolated editor, set a breakpoint at main.n line 8, press F5, step
through lines 9–11 and into mix at line 12; inspect left=11, right=3,
total=14 after evaluation, and the call stack. Earlier dated user observations
do not establish this revision's editor result.
