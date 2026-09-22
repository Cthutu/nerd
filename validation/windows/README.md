# Windows validation handoff

On Windows, pull `experiment/task-scheduler-performance`, then tell Codex:

> Read `validation/windows/PROMPT.md` and execute it.

Codex should run the validation, fix failures, preserve results here, and push
fixes and evidence to the same branch. Back on Linux, ask Codex to pull that
branch and read `validation/windows/results/HANDOFF.md`.

The compiler must be built with a recent Clang supporting the repository's C23
features and `#embed` (Linux evidence used LLVM 22.1.8). Native binary generation
uses `opt`, `llc`, `lld-link`, `llvm-lib` and the Windows SDK/CRT. Use a configured
Windows development shell; `nerd doctor` diagnoses compiler runtime dependencies.
Python 3.10+, Git and `just` are needed by the existing checks; some tests also
use external Clang for C-output validation and LLDB/CodeLLDB for debugging.
Editor checks require Node/npm and installed local dependencies under
`syntax/nerd-vscode/node_modules` (`npm install` in that directory if absent).
Missing dependencies must be recorded and resolved, not treated as passing tests.

From the repository root in PowerShell:

```powershell
python validation/windows/run.py --list
python validation/windows/run.py
# Equivalent launcher, if your script execution policy permits it:
.\validation\windows\run.ps1
# Focus a rerun; required builds are included automatically:
python validation/windows/run.py --only front-threads-debug jobs-release
```

Each invocation creates `results/<UTC>-<unique-id>/` with incremental summaries,
separate command logs, tool versions, and a manual checklist. Tests run serially,
continue after independent failures, and block dependents of failed builds.
The full run covers both compiler configurations, the fixture suite (which is
hardwired to the debug compiler), all auxiliary `just test` checks, debugger
probes, and finally a jobs 1/2/4/8 timing sweep if correctness checks pass.
The runner does not install Nerd globally, modify Git, or run `just clean` in
this checkout. The clean test uses an isolated temporary copy.

`--timeout` sets a per-stage limit (default one hour); timeouts fail the stage
and terminate its process tree. Commands stream output directly to their log;
Codex can inspect these logs while a stage runs. A targeted run is labelled with
its selection and never substitutes for a final full run. A full automated pass
still requires completing or explicitly blocking the manual checklist.

The benchmark runner measures wall time on Windows; CPU/RSS fields currently
remain null there. Do not interpret missing memory metrics as zero or claim the
memory adoption gate passed. Native memory observations and physical-core-count
runs can be collected separately, recording the method and hardware. Do not run
benchmarks alongside builds/tests. Keep the one-job default pending evidence.

For repeatable desktop evidence, prepare the matrix after building both compilers
and before benchmarks, then observe it from an interactive Windows terminal:

```powershell
python validation/windows/manual.py validation/windows/results/<run> --prepare
python validation/windows/manual.py validation/windows/results/<run> --observe
```

The helper covers both compilers, debug/release targets, jobs 1/4 and LLVM/C
backends for Pixels, Dungeon and Triangle. Each launch requires a human
PASS/FAIL/BLOCKED observation; evidence is saved incrementally to `desktop.json`.
Use `--compiler release` or `--example pixels` to observe smaller groups.
VS Code breakpoints, stepping, locals and call stacks still require the separate
editor check in `MANUAL.md`. Windows installation smoke additionally requires
`llvm-dwarfdump` from the full LLVM distribution to inspect embedded DWARF.

To verify this harness itself (does not validate Windows compiler behavior):

```powershell
python validation/windows/test_runner.py
```

## Adaptive scheduling follow-up (M9–M11)

The benchmark now includes jobs `1 2 4 8 16 auto`. Automatic mode chooses a
half-logical-CPU ceiling and reduces dispatch for small/dominated batches;
numeric overrides retain their previous behavior. Keep omitted jobs at one
until the adoption gates in `review/audits/task-scheduler-performance.md` pass.

`benchmark-accounting` verifies a known CPU/memory workload before the benchmark.
On CPython/Windows the harness reads process times and peak working set through
the retained process handle. API failures or zero peaks fail visibly. Native
validation of this new path is required. Windows CPU/RSS counters describe the
compiler process only, excluding LLVM/linker descendants; elapsed time includes
the entire build. POSIX wait4 CPU includes waited-for descendants, and RSS is a
maximum process peak rather than simultaneous process-tree memory. The JSON
`accounting_scope` field identifies this distinction. Do not compare these CPU
figures across operating systems as equivalent totals. Whole-tree Windows CPU
and peak-memory accounting remains an additional adoption measurement.

Review `scheduler-policy` records and the source-to-IR/front-end elapsed spans
alongside whole-build latency. Policy profiles are separate intrusive runs, not
latency samples. Report repeated noisy results and failures of the acceptance
gate; do not enable automatic defaults merely because correctness tests pass.
