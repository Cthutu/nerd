# std-library integration into main — 20 October 2026

Local `main` was fast-forwarded to `agent/main` (`c070b322`), then `std-library`
(`4676f1f8`) was merged without textual conflicts as `fa03b60c`.

The first combined debug gate exposed one integration failure: the applied
generic constructor regression returned incorrect field values after main's
ordinary plex storage reordering. See [initial-test.log](initial-test.log).
LLVM's associated-function lookup selected the first same-named `init` instead
of requiring the concrete signature selected during semantic analysis. Prior
layout happened to leave the checked integer field in a compatible position,
masking the wrong call. The fix matches both name and selected function type.
The regression now also checks string payloads for local and qualified pairs.
Implementation documentation records the interaction.

The [native matrix](native.log) passed all 36 executions of the nine compiler
construct regressions through LLVM and generated C in debug/release modes. The
C executions include ASan, UBSan and leak detection. Reproduce from the repository
root with:

```sh
python3 validation/linux/results/20261020-compiler-constructs/native-probe.py
```

The final `just test` gate passed **1,222 fixtures, zero failures and nine
existing Linux skips**, plus all applicable auxiliary gates. These include
**317 C differential fixtures at O0/O2**, with zero platform skips. See
[just-test.log](just-test.log).

The final `just test-release` gate also passed **1,222 fixtures, zero failures
and nine existing Linux skips**, plus all applicable release auxiliary gates.
See [just-test-release.log](just-test-release.log).
[sources.sha256](sources.sha256) records the integration fix, strengthened
regression and updated documentation. Native Windows remains unvalidated here.
