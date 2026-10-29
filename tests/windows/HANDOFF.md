# Windows validation handoff

## Shared catalogue and terminal dashboard — 29 October 2026

The `test-dashboard` branch replaces the normal gate with `tests/run.py` and
shares correctness commands with this native runner. Portable runner checks and
Linux pseudo-terminal success/failure/cancellation tests are included in the
normal gate. Native Windows terminal validation remains pending: check arrow
navigation, resize, failure acknowledgement, Ctrl-C/process-tree cleanup, UTF-8
and redirected/no-pause operation in a native console. Use `--ui-probe success`
and `--ui-probe failure` with `tests/harness/check_runner.py` for deliberate UI
probes (the latter should hold until acknowledgement and exit 1). Do not count
that expected probe failure as a compiler failure. Native results still belong
under `tests/windows/results/`; retain the existing benchmark/manual procedure.

## Main language feature integration — 23 October 2026

Validate `main` after pulling its configured upstream. The integration brings in
explicit plex-pattern field coverage, optional/result entry points, addresses of
intermediate values, optional/result loop conditions, nested labelled loop
results and qualified associated/generic calls. Arena-sharing guidance remains
pointer-based; arenas are still noncopyable. New runtime checks are grouped in
the existing feature files, with entry-point process contracts under commands.
Pay particular attention to stderr/exit/cleanup behaviour for result-returning
`main`, native Windows stdio checks, loop temporaries and imported generic
constructors. Native Windows execution of this integration remains pending;
Linux and portable harness results do not close that gate.

## Test infrastructure relocation — 21 October 2026

The native runner and this handoff now live under `tests/windows/`; standard-library
capability/layout probes live under `tests/stdlib-capabilities/`. Use
`python tests/windows/test_runner.py` and `python tests/windows/run.py --list`
to check the relocated harness. Test scope and native Windows requirements are
unchanged. New raw results stay local under ignored `tests/windows/results/`.

## clean-up branch — 20 October 2026

Validate `clean-up` after pulling its configured upstream. This branch reorganises
compiler fixtures and their harness. Expanded C coverage also fixes global box
source clearing and untyped nested continuation literals in the C backend;
regressions are in `boxes.t` and `strings.t`. No runtime implementation changes.
Run the native Windows debug/release gates, formatter, install and editor checks.
Pay particular attention to grouped fixture boundaries, independent program
namespaces, stdin, release-mode cases and Windows-only error/LSP groups. Network
and Nexus integrations are now opt-in (`just test-network`, `just test-nexus`);
record their results separately when exercised.

The latest recorded native results before cleanup are in the
[archived handoff](https://github.com/Cthutu/nerd/blob/fac0feae825d73a478466410956c1ca7749a2a8a/validation/windows/results/HANDOFF.md).
They validate earlier revisions, not the grouped suite. Keep a concise current
status here; generated reports under `results/` are local and ignored. Publish
only conclusions, reproducible commands and unresolved failures needed by the
next maintainer. Do not silently add generated evidence back to the repository.
