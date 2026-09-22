# Native Windows manual checks

Status: **NOT RUN**. Automated success does not complete this checklist.

Record compiler commit, exact commands, generated target mode, worker count,
observations, and PASS / FAIL / BLOCKED for each row. Leave unavailable checks
BLOCKED with a concrete reason; do not infer visual success from a live process.

| Check | Status | Evidence / notes |
| --- | --- | --- |
| Pixels visibly draws and responds; normal exit | NOT RUN | |
| Dungeon displays before input, accepts input, Q exits | NOT RUN | |
| Triangle draws and responds; normal exit | NOT RUN | |
| VS Code/CodeLLDB breakpoints, source stepping, locals, call stack | NOT RUN | |
| Generated C versions: display/input/exit parity | NOT RUN | |

Use `_bin/nerd-debug.exe` and `_bin/nerd.exe` directly. For each graphical example,
exercise debug/release **target modes** at jobs 1 and 4. Keep generated files in
this run's ignored `scratch/` directory. Compile compatibility C externally using
the argument list printed by `nerd build --copts`; Nerd must not invoke Clang.
PowerShell does not use Bash's `$(...)` word splitting: use an argument array with
proper handling of quoted paths, or transcribe the printed arguments explicitly.

The automated debugger probe is useful but does not replace checking the normal
Windows editor workflow. Record any human assistance needed to access a desktop.
