# Native Windows manual checks

Status: **PASS: 48 native desktop cases and user-reported editor smoke**.

Desktop checks for the exact final compiler implementation `828d4b56` were
completed in the immediately preceding run. Both compiler SHA-256 hashes were
verified unchanged after this full run's build stages. The rerun changes only
process-local Clang SDK discovery. See the complete
[desktop/editor record](../20260922T083831Z-654d9483/MANUAL.md),
[48-case command ledger](../20260922T083831Z-654d9483/desktop.json),
[native captures/input/exit results](../20260922T083831Z-654d9483/desktop-automated.json)
and [compiler hashes](environment.json). All desktop activity ended before
this run's benchmarks; it was not repeated unnecessarily during timing.

Record compiler commit, exact commands, generated target mode, worker count,
observations, and PASS / FAIL / BLOCKED for each row. Leave unavailable checks
BLOCKED with a concrete reason; do not infer visual success from a live process.

| Check | Status | Evidence / notes |
| --- | --- | --- |
| Pixels visibly draws and responds; normal exit | PASS | 16 cases, animation, resize, reviewed captures, Q exit zero |
| Dungeon displays before input, accepts input, Q exits | PASS | 16 cases, rooms before input, Space regeneration, Q exit zero |
| Triangle draws and responds; normal exit | PASS | 16 cases, coloured triangle, resize, Q exit zero |
| VS Code/CodeLLDB breakpoints, source stepping, locals, call stack | PASS (smoke) | User: “It seemed to work”; detailed automated LLDB transcripts supplement that broad observation |
| Generated C versions: display/input/exit parity | PASS | All three examples, both compilers and target modes, jobs 1/4 |

Use `_bin/nerd-debug.exe` and `_bin/nerd.exe` directly. For each graphical example,
exercise debug/release **target modes** at jobs 1 and 4. Keep generated files in
this run's ignored `scratch/` directory. Compile compatibility C externally using
the argument list printed by `nerd build --copts`; Nerd must not invoke Clang.
PowerShell does not use Bash's `$(...)` word splitting: use an argument array with
proper handling of quoted paths, or transcribe the printed arguments explicitly.

The automated debugger probe is useful but does not replace checking the normal
Windows editor workflow. Record any human assistance needed to access a desktop.
