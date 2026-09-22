# Semantic scope-query optimisation

2026-09-22. Linux follow-up to `ebab002f`.

Sampling the turn-start release compiler on the temporary 200-unit heavy project
attributed 30.6% of user-cycle samples to usage-context seeding, 11.2% to declaration
collection, and 8.1% to top-level import collection. Inspection found repeated
whole-AST membership searches inside these passes.

Conditional, trait and generic-implementation membership now queries the parser's
completed side tables. Missing implementations are rejected immediately. Local
function-body queries walk backward to containing function starts rather than
scanning every earlier function definition. No shared mutable cache or scheduler
change is introduced. Conditional keywords, negation and disabled flags retain
their existing checks; the full corpus covers generics, nested scopes, imports,
platform conditions and ordered diagnostics.

Preliminary five-sample, one-worker paired results against `990cfb5a` preserve
exact LLVM: single-module debug 107.33 → 64.66 ms, release 83.26 → 39.88 ms;
Pixels debug 204.75 → 167.75 ms, release 314.01 → 277.46 ms. These results include
the preceding frontend-depth fix. Final acceptance uses the completion report's
fresh full matrix, not these preliminary numbers.

The full Linux test suite and ASan frontend/depth checks pass. Final debug,
release, TSan, native-runner preparation and benchmark evidence are collected
under `compiler-single-core-final/`. Native Windows/macOS execution remains an
external gate. The historical 4–6% single-module regression is superseded by
measured semantic-cost reductions, not by a speculative alignment change.
