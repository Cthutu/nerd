# Windows validation handoff

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
