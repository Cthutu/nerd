# Windows handoff status

**Ready for native Windows execution. Windows checks have not been run.**

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
