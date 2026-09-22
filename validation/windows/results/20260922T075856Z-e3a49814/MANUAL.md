# Native Windows manual checks

Status: **FAILED in optimized Dungeon; superseded by alignment repair.**

Compiler commit `a618456b5d0013917b22a50933f9408cedfd5dbb`; exact commands,
compiler/program hashes, modes/jobs and input/capture results are recorded in
[desktop.json](desktop.json) and [desktop-automated.json](desktop-automated.json).
The full 48-cell build matrix succeeded. Pixels and Triangle each passed their
16 drawing/resize/Q-exit cases; contact sheets and original client captures were
visually reviewed. Pixels' successive frame hashes changed. All successful
automated shutdowns returned zero.

Optimized LLVM Dungeon faulted while appending console events; native Windows
[crash records](dungeon-crash-events.json) identify `0xc0000005` at offset
`0x1e9c6`, an aligned SIMD store into misaligned dynamic-array storage. The user
reported that an individually launched case seemed to regenerate and quit, but
that broad smoke observation does not override the repeatable crash evidence.
The helper's initial console-host/window-selection and attachment failures are
preserved in the `desktop-capture-*.json` attempt records. Scan codes and a
100 ms key hold did not resolve the optimized crash. The 32-byte header repair
and successful native rerun are in [the focused run](../20260922T083052Z-6bf97fd5/SUMMARY.md).

Record compiler commit, exact commands, generated target mode, worker count,
observations, and PASS / FAIL / BLOCKED for each row. Leave unavailable checks
BLOCKED with a concrete reason; do not infer visual success from a live process.

| Check | Status | Evidence / notes |
| --- | --- | --- |
| Pixels visibly draws and responds; normal exit | PASS | 16 cases, animation/resize/client captures/Q exit zero |
| Dungeon displays before input, accepts input, Q exits | FAIL | Optimized LLVM event-append alignment crash |
| Triangle draws and responds; normal exit | PASS | 16 cases, coloured triangle before/after resize/Q exit zero |
| VS Code/CodeLLDB breakpoints, source stepping, locals, call stack | PASS (smoke) | User: “It seemed to work”; exact automated stepping transcripts passed both compilers |
| Generated C versions: display/input/exit parity | FAIL overall | C versions work; optimized LLVM Dungeon crash breaks parity |

Use `_bin/nerd-debug.exe` and `_bin/nerd.exe` directly. For each graphical example,
exercise debug/release **target modes** at jobs 1 and 4. Keep generated files in
this run's ignored `scratch/` directory. Compile compatibility C externally using
the argument list printed by `nerd build --copts`; Nerd must not invoke Clang.
PowerShell does not use Bash's `$(...)` word splitting: use an argument array with
proper handling of quoted paths, or transcribe the printed arguments explicitly.

The automated debugger probe is useful but does not replace checking the normal
Windows editor workflow. Record any human assistance needed to access a desktop.
