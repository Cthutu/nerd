# Integer literal range checks — 2026-10-02

Branch `std-library`, starting revision `dd942510`. Fresh Clang debug/release
compiler builds. Results cover the implementation committed with this report.

- [Final compiler suite](compiler-suite.log): **1193 passed, zero failures,
  15 platform skips**.
- [LLVM/C differential suite](cgen.log): **300 fixtures passed at O0/O2,
  two platform skips**.
- [Release diagnostic tests](release-errors.log): fixture 141 passes all
  **23 isolated source/diagnostic pairs**. This covers plain/atomic storage,
  signed/unsigned boundaries, named constants, inferred values, default local/
  global storage, fields, arrays, tuples, assignments, arguments and returns.
- [Release atomic tests](release-atomic.log): 14 fixture passes, including
  expanded existing fixture 349 for valid limits, negative named constants,
  complete u64 magnitudes, and explicit narrowing.
- [Capability/layout probes](capabilities.log): native LLVM debug/optimized pass.

Validation found and repaired two related issues: default-typed storage could
skip range checking, and explicit casts of large literals could truncate through
i32 in LLVM before conversion. The existing integer-wrapping differential case
now uses an explicit cast for an intentional signed bit pattern and passes both
backends. Windows GetStdHandle constants in the text-adventure example similarly
use explicit unsigned conversions for -10/-11/-12.

Historical failures are retained: [initial example rejection](initial-suite.log),
[large-cast backend discrepancy](initial-cgen.log), and an intermediate
[aggregate validation regression](aggregate-regression.log). Final aggregate
checking inspects already-inferred element types without reinference or HIR
changes, preserving destructuring and the existing HIR snapshots.

No test harness changes or test consolidation were made. The independent
`test-suite-audit` branch/worktree remains untouched. Error cases are grouped in
one fixture and execution coverage extends existing fixtures. No full `just do`,
WSL/Linux, desktop, benchmark or global-install run was performed.
