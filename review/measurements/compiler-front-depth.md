# Frontend arithmetic depth

2026-09-22. Follow-up to the LLVM-only depth fix (`990cfb5a`).

Semantic arithmetic inference and HIR lowering now use explicit traversal frames.
Expected types still flow left-to-right; the existing binary-result checks and
final validation are shared with the recursive paths. No cross-context inference
cache is introduced. Simple operators avoid frame allocation. HIR expression
indices retain the original postorder, preserving exact output.

The default source regression now uses 6,000 operands in all configurations.
Debug and AddressSanitizer pass both jobs=1/4, including ordinal side-effect
checks and non-associative/right-nested subtraction. The full frontend parity
suite passes under ASan. Before/after exact LLVM comparisons pass all seven
benchmark projects, both target modes, jobs=4. These comparisons overlapped
validation and are correctness evidence, not timing claims.

The previous `sema_infer_node_type` and subsequent `hir_lower_expr` stack failures
on this source are resolved. Other recursive constructs are not claimed to
support arbitrary depth. Native Windows/macOS execution remains pending; the
Windows runner now runs the same 6,000-term source for both compiler builds.

Logs are preserved in `compiler-single-core-final/s2-*.log.gz`.

Full `just test` passed: 1,122 fixtures, zero failures, nine skips; all integration suites and 280 C differential cases at two optimisation levels.
