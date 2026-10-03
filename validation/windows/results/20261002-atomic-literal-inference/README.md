# Atomic literal inference — 2026-10-02

Branch `std-library`, starting revision `e6dd9f01`. Fresh Clang debug/release
compiler builds; no global installation changed.

- [Compiler suite](compiler-suite.log): 1192 passes, zero failures, 15 platform skips.
- [LLVM/C differential suite](cgen.log): 300 fixtures passed at O0/O2, two platform skips.
- Final atomic tests: 14 passes with [debug](atomic-debug.log) and
  [release](atomic-release.log) compilers. These reruns include corrected negative
  fixtures: unused variables and an invalid semicolon initially masked the
  intended checks. Verified final diagnostics are `Integer literal is too large`
  and `Type mismatch: expected usize, found i32`.
- Updated capability probe passes in debug and optimized modes without the cast.

Only inference changes: expected atomic storage supplies its element type to
ordinary expression inference. No new implicit conversion of explicitly typed
values, backend changes, or changes to atomic ordering. The regression covers
named constants, expressions, signed/unsigned limits, field initializers and
assignments.

Separate existing issue confirmed: both `value: u8 = 256` and
`value: atomic[u8] = 256` pass semantic checking. This fix preserves plain-value
behavior; it does not add element-range checking. The oversized-literal test uses
a value exceeding 64 bits. No WSL/Linux or full `just do` run for this change.
