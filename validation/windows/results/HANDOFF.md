# Windows handoff status

**Native Windows validation in progress, 2026-09-22.**

Starting commit: `6bbc17e8e14facc95caee55abdca35db216bbb05`; clean working tree,
up to date with `hub/experiment/task-scheduler-performance`. Authenticated remote
`hub` points to `git@github.com:Cthutu/nerd.git`.

Host: Windows 11 Pro 10.0.26200 x64, AMD Ryzen Threadripper PRO 5955WX,
16 physical cores / 32 logical processors, Balanced power plan.
Installed Clang 23.1.0 lacks opt/llc. Official LLVM 22.1.8 Windows x64 archive
was extracted under ignored `_tmp/llvm-tools` without replacing installed tools.
Validation uses its bin directory first on process PATH and these LIB directories:

```text
C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Tools\MSVC\14.44.35207\lib\x64
C:\Program Files (x86)\Windows Kits\10\Lib\10.0.26100.0\ucrt\x64
C:\Program Files (x86)\Windows Kits\10\Lib\10.0.26100.0\um\x64
```

Local `_tmp/windows-env.ps1` sets that process-only environment and NERD_LIB_PATH.
No global compiler, extension, policy, or toolchain was replaced. CodeLLDB 1.12.2
is installed (1.11.3 also present). Compiler caches from Clang 23 were moved to
ignored `_tmp/clang23-objects` before a fresh LLVM 22.1.8 build.

- Harness self-tests: 4 passed.
- [Initial full run](20260922T072040Z-c854dc52/SUMMARY.md): both compiler builds
  failed on enum/u32 signedness; clean, allocator and thread suites passed;
  dependent suites correctly blocked.
- [Second build run](20260922T072126Z-6c5bc397/SUMMARY.md): exposed CRT getenv
  deprecation because timing.c included system headers before core definitions.
- [Build repair verification](20260922T072250Z-0648ef15/SUMMARY.md): both builds
  passed with Clang 23. Explicitly cast the test mode back to ErrorRenderMode;
  include timing.h before CRT headers to apply the existing portability policy.
- [Fresh LLVM 22 full run](20260922T072410Z-d0e26691/SUMMARY.md): in progress.

Reproduce: `. ./_tmp/windows-env.ps1; python validation/windows/run.py`.
`nerd-debug.exe doctor` passed with LLVM 22 and the SDK/CRT paths above.
Manual checks are pending; the user is available for desktop observations.
Windows CPU/RSS benchmark accounting is unavailable; macOS remains untested.
Keep jobs 1 as default and do not mark the M8 cross-platform gate complete.

## Original Linux preparation record

Branch: `experiment/task-scheduler-performance`.
Compiler baseline when this package was prepared: `bb2c441b` (M8 Linux report).

On Windows, read and execute [the handoff prompt](../PROMPT.md). Replace/extend
this status with the actual Windows run, fix commits, evidence and remaining
blockers. Preserve earlier run folders.

Preparation on Linux:

- `python3 validation/windows/test_runner.py`: four harness tests passed.
  Covers dependency selection, failed-build blocking with independent-stage
  continuation, pass/failure/missing-command/timeout recording, and summaries.
- `python3 validation/windows/run.py --list --only front-threads-release jobs-debug`:
  correctly included debug/release build prerequisites and explicit compiler paths.
- `python3 validation/windows/run.py --allow-non-windows --only threads`:
  passed the native Linux worker lifecycle suite through the runner. See its
  [summary](20260921T192954Z-b94fb83e/SUMMARY.md) and logs. This is a harness
  smoke test only, not Windows compiler validation.
- PowerShell is unavailable on the preparation machine; `run.ps1` has been
  reviewed but not executed. Python is the primary runner and can be invoked
  directly by Windows Codex.

Return to Linux: fetch/pull this branch, read this file and the linked Windows
runs, inspect all fixes and remaining failures, then run Linux regression checks
appropriate to the Windows changes. The one-job default remains in place;
macOS validation is still a separate outstanding gate.
