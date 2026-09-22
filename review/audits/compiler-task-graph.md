# Dependency-driven compiler tasks

2026-09-22, branch `experiment/task-scheduler-performance`.

Historical proposal: implement a persistent work-stealing task scheduler and
expose dependencies between compiler operations. The previous module-level headroom analysis does
not bound this architecture: splitting signatures from bodies changes the graph.
Normal binary production continues through LLVM tooling; Nerd never invokes Clang.

## Decision: experiment withdrawn (2026-09-22)

G1/G2 were implemented and validated, then removed after the paired benchmarks
failed to establish a consistent gain over the simpler pre-graph implementation.
The user approved withdrawing this complexity. G3–G6 are deferred; the proposal
and historical validation below are retained as evidence, not active work.

The compiler again uses the established finite-batch `task_run` implementation.
Earlier parallel parsing, exclusive semantic-closure batches, HIR and LLVM
rendering, adaptive opt-in worker selection, and single-core optimisations remain.
Omitted jobs remains one. Benchmark tools and measurements are retained.
[Withdrawal validation](../measurements/compiler-task-graph-withdrawal.md)
records the restored implementation and passing checks.

The temporary large-project experiment found roughly 7–8% speedups with four
workers on 1,000-module synthetic projects, but compared worker counts within G2,
not G2 against the earlier scheduler. Background CPU load also limited confidence.
Those results do not establish a benefit from work stealing.

Next priorities are the linear symbol-list searches during LLVM text combination,
semantic-analysis costs, and the recursive LLVM expression-emission crash exposed
by the concentrated synthetic workload. These need separate fixes and measurement;
this withdrawal does not claim to resolve them. The temporary experiment remains
in `/tmp/nerd-scale-bench-Xo1BUG` on the originating machine and is not portable
or durable repository evidence.

## Milestones

| Milestone | Implementation | Gate |
| --- | --- | --- |
| G1: scheduler foundation | Persistent worker pool, per-worker ready deques, stealing, explicit dependency edges, inline caller participation; validate frozen graphs before execution | Chain/diamond ordering, exactly-once execution, failure drain, repeated graphs, partial startup failure, ASan/TSan |
| G2: file pipeline | Reuse one pool for front-end work; represent lex → parse for discovered sibling files, keep private source/results and stable DFS adoption | LLVM/HIR/C/runtime and ordered-error parity; folder modules, conditional imports, cancellation and sanitizers |
| G3: dynamic graph | Coordinator discovery can publish new nodes while other independent nodes run; stable task identities, bounded admission, completion notifications and cancellation | Dynamic diamonds, duplicate imports, cycles, one-worker progress and bounded peak memory; no worker waits for another queued task |
| G4: semantic preparation prototype | Separate declaration/signature availability from function-body checking on a restricted, explicitly supported subset; immutable shared signatures and owned results | Demonstrate body overlap across a dependency edge; serial equivalence; unsupported features use established path |
| G5: semantic dependencies | Explicit generic/trait/compile-time requests, canonical identities, deterministic publication and diagnostic order | Shared/nested/recursive requests, inferred signatures, failure cleanup, randomized scheduling, ASan/TSan |
| G6: integration and adoption | Connect HIR/LLVM tasks where their inputs are immutable; tune grain size and admission, measure source-to-IR and whole builds | Linux and Windows correctness/performance/memory; native macOS status explicit; enable defaults only on evidence |

G1 deliberately begins with frozen graphs to prove lifecycle and dependency
semantics. G2 initially has discovery boundaries. G3 removes those boundaries;
G1/G2 alone must not be described as the complete asynchronous compiler pipeline.
A CST is not introduced just to create a task: current parsing produces AST.

## Scheduler contracts

- A task owns mutable scratch and outputs until completion. Dependency completion
  publishes those outputs before a dependent callback starts.
- Ready tasks enter per-worker deques. Owners take recent tasks, idle workers
  steal older tasks. Begin with a mutex protecting queue/dependency bookkeeping;
  callbacks run outside it. Measure contention before adding lock-free machinery.
- Pool lifetime is independent of graph lifetime. Only a coordinator submits or
  destroys graphs/pools. Callbacks must not synchronously run a graph on the same
  pool or block waiting for queued work. Dependencies represent those waits.
- Invalid graphs execute no callbacks. Failure stops new dispatch and drains all
  in-flight callbacks before caller-owned inputs/results can be freed. Worker
  creation is transactional; a partially started pool executes no callbacks.
- Completion order does not determine module identities, symbols, emitted output,
  or the selected diagnostic. Existing serial error replay remains until an
  equivalent dependency-aware publication scheme is proven.
- The calling thread counts toward the worker ceiling. Numeric overrides and
  auto's half-CPU ceiling remain; small tasks may be fused or executed inline.
  CPU count is not a memory budget. Measure retained intermediate representations.

## Measurement

Record graph preparation, runnable/blocked tasks, queue/steal activity, critical
path, callback CPU, source-to-IR elapsed time, whole-build elapsed time and peak
memory. Compare jobs=1 through the same graph with the established serial path.
Use real examples and tiny/wide/deep/single-large-module cases. Attribute changes
to the graph/ownership changes separately from scheduler policy. Keep old graphs
and native handoff evidence so future decisions are reproducible.

## Historical progress before withdrawal

- G1 implemented: persistent pool, dependency validation and ready-queue stealing.
  Linux debug/release, injected partial startup failure, ASan and TSan pass.
  A forced fan-out test requires stealing to make progress. Native checks pending.
- G2 implemented: dependent file lex/parse nodes and a reused front-end pool,
  with capacity growth at drained boundaries and per-graph callback budgets.
  Final release output/runtime/diagnostic parity, full suite, ASan and TSan
  pass on Linux. [Validation evidence](../measurements/compiler-task-graph.md)
  records the exact scope. Native checks were pending; G3–G6 were not implemented.


## G2 performance checkpoint

[The paired benchmark](../measurements/compiler-g2-benchmark.md) compares G2
against original main and the exact pre-graph revision. Cumulative single-core
gains remain, but G2 has not established a consistent general scheduler speedup.
The initial four-worker Pixels improvement reverses in a longer broad-affinity
repeat; restricting four workers to four physical cores gives about 1% gain.
Single-worker changes are small. This checkpoint led to withdrawing G1/G2 and
deferring G3–G6; correctness and infrastructure completion did not establish
performance gains.

## Single-core follow-up

LLVM combination now uses string hash membership while preserving input order.
[Paired measurements](../measurements/compiler-symbol-lookup.md) show 16.4% less
whole-build time on the 1,000-module synthetic case with byte-identical LLVM.
Semantic-analysis costs and expression-depth handling remain separate follow-ups.
