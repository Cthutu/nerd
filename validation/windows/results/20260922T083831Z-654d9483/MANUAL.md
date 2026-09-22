# Native Windows manual checks

Status: **PASS: all 48 desktop cases**, using compiler implementation
`828d4b56ec08ea7f2001334e33cf660da9d82bbb`. This run's automated suite encountered
external Clang SDK-header discovery failures; desktop binaries were separately
prepared successfully and checked. A later full run uses explicit SDK paths.
Compiler hashes, exact commands and per-case results are in
[desktop.json](desktop.json) and [desktop-automated.json](desktop-automated.json).

The matrix crosses both compilers, debug/release targets, jobs 1/4 and LLVM/C.
Codex inspected every before/after cell in the contact sheets, with original
client images retained under `desktop-captures`. Pixels animation captures
changed; graphic clients resized to 960x720; native Space input regenerated
Dungeon, and every automated Q exit returned zero. The helper injects console
scan codes with a 100 ms key hold. These checks ran before benchmarks.
This establishes visible drawing/response, not just process survival.

- [Pixels before](pixels-before-contact.png) / [resized](pixels-resized-contact.png)
- [Dungeon before input](dungeon-before-contact.png) / [regenerated](dungeon-regenerated-contact.png)
- [Triangle before](triangle-before-contact.png) / [resized](triangle-resized-contact.png)

The separate editor launcher is
[launch-editor.ps1](../20260922T074219Z-d55d7186/launch-editor.ps1). It opens an
isolated extension-development window with separate user data, the local Nerd
extension/compiler and installed CodeLLDB. The user was asked to break at
main.n:8, step over lines 9–11, step into mix at line 12, and check left=11,
right=3, total=14 and the call stack. Their reply, “It seemed to work”, is a
broad smoke observation; detailed automated LLDB transcripts passed for both
compilers. No global compiler or extension was replaced. Other debugger
features/drivers, macOS and Windows peak RSS are not claimed.

Record compiler commit, exact commands, generated target mode, worker count,
observations, and PASS / FAIL / BLOCKED for each row. Leave unavailable checks
BLOCKED with a concrete reason; do not infer visual success from a live process.

| Check | Status | Evidence / notes |
| --- | --- | --- |
| Pixels visibly draws and responds; normal exit | PASS | 16 cases: colourful framebuffer, animation/resize, Q exit zero |
| Dungeon displays before input, accepts input, Q exits | PASS | 16 cases: rooms/corridors before input, different map after Space, Q exit zero |
| Triangle draws and responds; normal exit | PASS | 16 cases: coloured triangle on dark background, resize, Q exit zero |
| VS Code/CodeLLDB breakpoints, source stepping, locals, call stack | PASS (smoke) | User observation plus detailed automated stepping transcripts |
| Generated C versions: display/input/exit parity | PASS | All three examples behave equivalently through LLVM and external C |

Use `_bin/nerd-debug.exe` and `_bin/nerd.exe` directly. For each graphical example,
exercise debug/release **target modes** at jobs 1 and 4. Keep generated files in
this run's ignored `scratch/` directory. Compile compatibility C externally using
the argument list printed by `nerd build --copts`; Nerd must not invoke Clang.
PowerShell does not use Bash's `$(...)` word splitting: use an argument array with
proper handling of quoted paths, or transcribe the printed arguments explicitly.

The automated debugger probe is useful but does not replace checking the normal
Windows editor workflow. Record any human assistance needed to access a desktop.
