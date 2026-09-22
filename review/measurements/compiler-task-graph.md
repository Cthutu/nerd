# Dependency scheduler: G1/G2 validation

2026-09-22, Linux x86-64. Plan: [dependency-driven compiler tasks](../audits/compiler-task-graph.md).

- `f45a0116`: plan and contracts.
- `55883666`: persistent dependency scheduler and per-worker work stealing.
- `598a2d1a`: file-stage dependencies, front-end pool reuse and reduced callback budgets.

G1/G2 were subsequently withdrawn for lack of consistent measured benefit. This
report records historical validation; it does not describe the current compiler.
See the plan for the withdrawal decision.

## Historical implemented behavior

Eligible sibling files use a lex node followed by a dependent parse node. The
coordinator retains stable DFS adoption, source capture and error publication.
Folder expansion can re-lex/re-parse combined source inside the parse node.
The front-end pool persists across parsing, exclusive semantic-closure batches
and HIR. It grows only at drained boundaries, capped by ready work and requested
jobs; smaller batches reuse it with fewer participating callbacks. Singleton and
one-job batches stay inline. LLVM rendering remains on the existing batch runner.

This is a frozen-graph implementation with discovery boundaries. Dynamic import
admission (G3), the signature/body split (G4/G5), and adoption measurements (G6)
are not implemented by these commits. No new speedup or default-adoption claim
is made. Omitted jobs remains one. The earlier whole-module headroom model does
not bound future finer-grained semantic tasks.

## Validation

The scheduler suite runs debug/release configurations, native-start failure
injection, and ASan/TSan. It checks:

- Multi-layer dependency graphs, duplicate edges and exactly-once callbacks.
- Invalid indices, missing callbacks and cycles, including a cycle disconnected
  from a ready task: validation executes no callbacks on any invalid graph.
- Repeated graphs and changing callback budgets on one persistent pool.
- Forced stealing: one root releases two callbacks onto its worker's deque;
  both must overlap to pass a barrier. Their failed join must never execute.
- Failure draining and subsequent reuse, empty graphs and partial worker startup.
- Pool reuse creates no new threads, verified with the native-start failure shim.

The final release compiler passes production jobs and front-end suites. Full
`just test` passes 1,122 fixtures with zero failures and nine platform skips,
plus 280 C differential fixtures at both optimization levels. Dungeon renders
and quits correctly in the C/LLVM runtime comparison. Front-end tests additionally
verify each parse starts after its matching lex stage, including expanded folder
modules, while preserving HIR/LLVM/C bytes, runtime results and diagnostic order.
Compiler-level front-end suites also pass both AddressSanitizer and
ThreadSanitizer with the integrated pool and file-stage dependencies. Logs are retained in
[compiler-task-graph](compiler-task-graph/).

The updated Windows runner includes `task-graph` as a prerequisite of performance
measurements, plus the expanded production front-end checks. The runner's own
unit tests pass on Linux; this is not native Windows validation. Native
Windows/macOS checks remain outstanding.

Scheduler bookkeeping uses checked calloc/free, so OS process memory includes
it but Nerd's tracked arena/heap counters do not. Each graph retains O(nodes +
edges) metadata and validation scratch. Parked pool workers persist until the
front end is finished; G6 must measure memory and wakeup/queue costs, not assume
that retaining threads guarantees faster builds.
