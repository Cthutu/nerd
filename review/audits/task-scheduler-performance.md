# Compiler task scheduling and single-core performance

Status: M1–M3 complete on Linux; M4 bounded LLVM scheduler implemented, with
measurement and adoption gates in progress. Default compilation remains serial.
Audit date: 2026-09-19. Source baseline: `a15e965dcfd4c554cc7691a82786142f74031eab`.
Branch: `experiment/task-scheduler-performance`.

## Recommendation and scope

Follow-up: [M1 measurements and revised priorities](../measurements/compiler-m1.md).
The initial timing tables below remain historical; use M1 for the direct LLVM baseline.

Architectural requirement clarified by Matt on 2026-09-19: Nerd must never
invoke Clang. Normal compilation produces binaries via LLVM tooling; optional
C emission exists for compatibility (for example, PS5 development) and stops
at the C file. This performance experiment prioritises source through LLVM-IR
generation. Dedicated parallel C emission is deferred.

Implementation update: the backend now invokes direct LLVM tools, and
`nerd doctor` checks the toolchain. The Clang-based timings below remain historical
measurements of the audit baseline, not measurements of the new backend. M1 must
establish a fresh baseline before performance conclusions are carried forward.

Improve avoidable serial work first, then introduce a small task scheduler and
use module LLVM rendering as its first compiler workload. Keep a serial path
through the same task functions. Parallel front-end work follows explicit
ownership and dependency changes, rather than wrapping existing loops in threads.

This report concerns the compiler's throughput and build latency. It does not
propose language-level tasks, asynchronous I/O, or automatic parallel execution
of compiled Nerd programs. Those need a separate runtime and language design.
Generated-program performance should be measured alongside later backend changes,
especially if compilation is split into separate objects.

## Current pipeline and evidence

The normal whole-program entry path is in
[`program.c`](../../src/compiler/build/front/program.c), not just the standalone
`front_end()` pipeline. It parses a module, recursively discovers and checks its
dependencies, finishes its semantic analysis, and collects exports. After all
modules are checked, `program_front_end_generate_hir` lowers them sequentially.

[`back_end_render_llvm_modules`](../../src/compiler/build/back/back.c) renders
each module into one shared arena. The backend then combines LLVM text and runs
one Clang invocation. The phase named `link executable` includes Clang's LLVM
parsing, compilation and linking; it is not a measurement of the linker alone.

[`CGen`](../../src/compiler/cgen/cgen.c) has a program-wide type map, exports,
output builder, and mutable current-module/function state. C output remains a
single translation unit, even if emission eventually uses multiple tasks.

### Initial measurements

Release compiler, default debug target builds, Linux x86-64, Ryzen 9 7950X
(16 cores / 32 logical CPUs), Clang 22.1.8. One warm-up followed by five measured
processes per command, serially, using checkout modules. No CPU affinity or
frequency controls; these are exploratory warm-cache measurements, not a
single-core pinned benchmark or a statistical performance gate. Measurements
finished before the test suite started.

Median end-to-end elapsed milliseconds:

| Input | Check | LLVM executable build | C generation only |
| --- | ---: | ---: | ---: |
| Tiny return-zero program | 1.49 | 32.81 | 2.41 |
| dungeon | 243.22 | 2177.91 | 332.75 |
| pixels | 279.94 | 853.97 | 363.65 |
| text-adventure/quill | 22.83 | 142.78 | 35.06 |

C generation timings exclude compiling the resulting C. They are not an
end-to-end backend comparison. Compiler release configuration and target
`--release` configuration are independent; the latter was not used here.

Selected phases from the final LLVM sample (milliseconds, not medians):

| Input | Front end | LLVM rendering | LLVM combining | External Clang |
| --- | ---: | ---: | ---: | ---: |
| dungeon | 310.19 | 118.85 | 154.61 | 1591 |
| pixels | 340.94 | 244.03 | 159.16 | 100.94 |
| quill | 24.38 | 17.50 | 47.52 | 51.26 |

The existing report mixes front-end thread CPU time on Linux with backend wall
time. It also lacks module labels. Its totals must not become parallel elapsed
time estimates. See [reproduction and raw data](../measurements/task-scheduler-baseline.md).

Even eliminating LLVM rendering entirely would bound pixels' speedup at roughly
1.40x and dungeon's at 1.06x on these samples. Real module parallelism will save
less because of imbalance and overhead. This makes the renderer a useful first
scheduler experiment, but insufficient as the whole performance strategy.

## Opportunities for task scheduling

| Work unit | Opportunity | Prerequisites / limitations | Priority |
| --- | --- | --- | --- |
| LLVM module rendering | Independent output buffers after HIR completion | Per-task arenas; immutable input verification; deterministic assembly | First compiler experiment |
| HIR per module | Existing explicit loop after semantic checking | Audit cross-module reads and hidden mutation; freeze symbols or precompute copied symbols | Next candidate |
| File read + lex + parse per discovered module | Overlap sibling work | Stable module storage; coordinator-owned discovery; task diagnostics | After loader separation |
| Semantic analysis per ready module | Run independent branches of import graph | Published immutable dependency exports/types; no recursive worker waits | Later, higher risk |
| C function/module body emission | Independent body buffers after type planning | Split shared CGen state; freeze type/export names; merge in source order | After LLVM success |
| LLVM tooling per module/object | Evaluate after the source-to-IR experiment | No Clang invocation; ABI/debug/type compatibility and init ordering | Separate scope |
| Function-level sema | Could help one dominant module | Shared type interning, generics, declaration dependencies and side tables need redesign | Defer |
| Tests / independent builds | Coarse process-level throughput | Isolated paths, CPU budget, deterministic reports | Separate tooling improvement |

`build/test_cgen.py` already uses a four-worker process-driving thread pool.
Do not confuse test-suite throughput with speeding up one compiler invocation.
Limit total concurrency when a test worker launches a multithreaded compiler.

### Thread-safety blockers found in source

These are the original audit findings. M3 progress below records the subsequent
diagnostic ownership and allocator bookkeeping fixes; task metrics and other
shared compiler state still need preparation before enabling workers.

* **Allocation tracking:** [`memory.c`](../../src/core/memory.c) updates global
  `g_memory_stats`; debug builds also mutate a linked allocation list and index.
  Separate arenas alone do not fix these races. Protect debug bookkeeping;
  design statistics for cross-thread frees and accurate live/peak bytes. Naively
  subtracting frees from the freeing thread's counters is incorrect.
* **Diagnostics:** [`error.c`](../../src/compiler/error/error.c) owns global
  modes, two arenas and a last-rendered message. Workers need explicit sinks
  with owned diagnostic payloads; the coordinator renders them in a stable order.
  Thread-local print buffers do not make diagnostic ordering deterministic.
* **Module lifetime:** `program_load_module_by_path` grows `program->modules`
  and copies source into the shared program arena. Existing code reacquires
  pointers after recursive loading. Worker pointers cannot survive concurrent
  reallocation. Use stable records and coordinator publication, or freeze the
  registry before worker tasks. Preserve source buffers through all consumers.
* **Graph state:** `MODULE_Loading` currently detects recursive import cycles.
  With concurrency, an in-flight shared dependency is not a cycle. Separate
  discovery/checking states and perform explicit graph cycle detection, including
  implicit core imports, conditional dependencies and re-exports.
* **Interning and HIR:** the lexer interner is per Lexer, which helps isolation.
  However, `hir_ffi_foreign_symbol_handle_from` and related code in
  [`gen.c`](../../src/compiler/hir/gen.c) cast away lexer constness to copy
  symbols. Prove ownership of every destination and eliminate concurrent
  mutation/read hazards before parallel HIR; const signatures are not proof.
* **Timing and outputs:** `timing_add` appends to shared arrays. Workers should
  return local metrics. Keep side-file registration, final file writes, module
  numbering, debug metadata assembly and global initialization ordering under
  deterministic coordinator control.
* **Other paths:** formatter globals in `format.c` prevent concurrent formatting
  today. Windows `time_frequency()` lazily writes a static cache; initialise it
  before starting workers or use a once mechanism. Audit core random state if
  any worker path uses it. LSP source-loader callbacks require snapshots, not
  unrestricted calls from worker threads.

This is a source audit, not a completed race-freedom proof. Backend helper call
graphs and error paths still need a focused review during implementation.

## Scheduler design proposal

Use a fixed, bounded worker pool with a mutex/condition-variable ready queue.
The existing core abstraction supplies mutexes; add portable worker lifecycle
and wakeup primitives for Windows and POSIX. Avoid work stealing or fibres until
queue measurements justify them. Allocate task records in batches.

Proposed `--jobs N` counts total execution slots, including the calling thread;
`--jobs 1` runs inline without starting workers. Keep the experimental default
at one until benchmarks justify automatic sizing. Cap workers to ready work and
a memory budget; logical CPU count alone is not a suitable default for every
machine. The first M4 slice implements this flag and the ready-work cap; a memory-budget
cap remains future work.

Each task owns scratch storage, result storage, diagnostics and metrics. Inputs
are immutable published products; results occupy preassigned stable slots.
Completion publishes results before releasing dependent tasks. The coordinator
owns graph mutation and ordered output. Result arenas live until consumers
finish; resetting worker scratch must never invalidate a published result.

Start with a barrier after module rendering. Later graph tasks carry dependency
counts; only ready tasks enter the queue. A worker never blocks waiting for a
child queued behind itself. The coordinator detects cycles before waiting for
completion. For failures, stop scheduling dependent work, join running tasks,
then release storage. Buffer diagnostics and select/report failures using a
defined serial-compatible order, not whichever worker finishes first.

Cancellation is cooperative at task boundaries. Ensure shutdown wakes sleepers,
joins workers and leaves artifact cleanup to one owner. Keep subprocess limits
separate from in-process task limits. Preserve module IDs, symbol names, init
order and `--copts`' single-line stdout contract for every worker count.

## Single-core improvements to investigate

1. **LLVM symbol membership indexing.** `back_end_symbol_array_contains` in
   [`llvm_text.c`](../../src/compiler/build/back/llvm_text.c) scans arrays during
   definition collection and declaration filtering. Replace repeated membership
   scans with hash indexes while retaining ordered emission arrays. Combining
   costs 48–159 ms on the examples above; a CPU profile must establish how much
   belongs to these searches before claiming a gain.
2. **Semantic type canonicalization.** `sema_add_type` scans all type rows;
   function/tuple type paths also scan and compare members. Profile these paths,
   then index immutable structural keys. Preserve nominal identity, recursive
   types and reserved rows that are completed later; a raw-byte hash over mutable
   or padded structs is not a sufficient design.
3. **Lookup indexes.** Module-path lookup in both program loading and sema is
   linear. Backend external-library deduplication is nested scanning. Add
   per-build indexes if profiles show material costs, retaining deterministic
   iteration and current path semantics. Measure hits and misses separately.
4. **Avoid repeated emission and text work.** Debug `--llvm` sidecars can render
   each module a second time without debug metadata. Profile that mode separately;
   share a deliberate emission product rather than stripping arbitrary LLVM
   text. C `cgen_format` formats twice and stores temporary strings; profile
   allocation/formatting costs before replacing it. Return an explicit
   has-initializer flag instead of scanning rendered LLVM if worthwhile.
5. **Allocation and copying.** Extend existing memory counters with ownership
   accounting, reserve known table sizes, and reuse bounded scratch arenas.
   Source loading currently maps, copies and unmaps imported files. Compare
   retained mappings against copied snapshots, including LSP edits and Windows
   file locking. Avoid trading small time savings for excessive retained memory.
6. **Reuse across builds.** Consider dependency-keyed checked-module or object
   caches only after serial hotspots are measured. Keys need source/dependency
   contents, compiler/runtime version, target, flags and conditional definitions.
   Test invalidation before relying on cached results. Existing `--copts` still
   does front-end work; reuse may help repeated C generation/options queries.

Keep each optimization independently measurable at one worker. Old May 2026
allocation and timing notes are useful hypotheses, not current baselines.

## Milestones and acceptance gates

| Milestone | Deliverable | Completion gate |
| --- | --- | --- |
| M0 — this audit | Experiment branch, source audit, exploratory measurements, proposed plan | Report checked against current source; no scheduler changes |
| M1 — reproducible baseline (complete on Linux) | Benchmark runner; per-module wall/CPU timings; dependency graph; memory and output-size data | Debug/release targets, LLVM/C/check, tiny/real/wide/deep/single-large-module inputs; warm and cold runs; raw results retained |
| M2 — serial improvements (complete on Linux) | Combiner scratch reuse; function-name and source-line indexes; usage-context and declaration filtering | Same outputs/diagnostics; measured one-core improvement beyond noise; retained-memory comparison |
| M3 — ownership preparation (complete on Linux) | Task diagnostic sink; safe allocator bookkeeping; result lifetimes; task metrics; portable worker primitives | Serial tests unchanged; allocation/free across threads and failure cleanup stress tests; race checking where supported |
| M4 — scheduler + LLVM modules (in progress) | Bounded queue, inline one-worker mode, ordered render-result merge | Jobs 1/2/4/8/physical-core count; byte-stable C/HIR/LLVM where applicable; debug behavior and runtime parity; no deadlock on failure |
| M5 — module front end | Split discovery from checking; stable registry; parallel parse, then HIR and dependency-ready sema in separate changes | Diamond/duplicate/cyclic/missing/conditional imports; shared FFI symbols; identical diagnostics; randomized completion stress |
| M6 — C emission (deferred) | Reconsider only if compatibility workloads justify it | One C file; preserve compatibility and initialization behavior |
| M7 — LLVM tooling (implemented separately) | Direct LLVM tooling and linking, plus `nerd doctor` | No Clang invocation by Nerd; preserve output modes, debugging, FFI and runtime behavior |
| M8 — adoption decision | Worker default and task-size thresholds backed by data | Cross-platform validation and documented regressions/tradeoffs; accept or stop individual experiments |

M2 and M3 are independent after M1; M4 requires M3. M5 follows evidence
from M4. M6 is deferred. M7 records the architectural correction implemented outside
this source-to-IR experiment. Each implementation milestone should be a small series
of independently reviewable commits with its own before/after measurements.

Suggested adoption gates (targets, not measured promises): no more than 5%
one-worker regression outside noise; at least 15% end-to-end improvement on a
representative multi-module workload before enabling parallelism by default;
no more than 1.5x peak RSS at the selected default without an explicit tradeoff
decision. Tiny inputs should stay inline when scheduling overhead outweighs work.
Report CPU utilization and summed work alongside wall time: doing more total work
can disguise poor scaling. Pin a one-core run for serial comparisons and measure
physical cores separately from SMT.

Use the existing test suite plus targeted scheduler lifecycle tests, repeated
random scheduling, Linux race instrumentation where available, and native Windows
testing. Verify library output modes, C/LLVM parity, source debugging, deterministic
diagnostics and cancellation cleanup. Full compiler tests establish correctness;
they do not establish a performance win.

The first M2 slice, combiner scratch-arena reuse, is complete on Linux; see
[the before/after report](../measurements/compiler-m2-scratch.md).
The second slice, function-name conflict indexing, is also complete on Linux;
see [its comparison](../measurements/compiler-m2-function-names.md).
The third slice, source-line lookup indexing, is complete on Linux; see
[its measurements](../measurements/compiler-m2-lines.md).
Usage-context inference now skips scope scans for AST kinds that cannot
contribute constraints; see [the fourth M2 comparison](../measurements/compiler-m2-usage.md).
Declaration collection now also avoids irrelevant scope scans; see
[the fifth M2 comparison](../measurements/compiler-m2-decls.md). This completes
the planned Linux M2 work. Further semantic indexes remain optional candidates.
M3 has started with per-module result arenas and fixed result slots; see
[the ownership and validation report](../measurements/compiler-m3-results.md).
Result lifetimes and partial cleanup are now explicit. Task-owned diagnostic
contexts and deep-owned deferred queues are also implemented; see
[the diagnostic ownership report](../measurements/compiler-m3-diagnostics.md).
The coordinator replays diagnostics in stable order; rendering still uses its
global temporary arena. Allocator bookkeeping now has synchronized global
counters and a protected debug list, with cross-thread ownership stress tests;
see [the allocator report](../measurements/compiler-m3-memory.md).
Thread-local allocation activity and value-only render timing records now
separate task measurement from coordinator reporting; see
[the metrics report](../measurements/compiler-m3-metrics.md).
Portable joinable threads and condition variables now have lifecycle, native
creation-failure and sanitizer tests; see
[the worker preparation report](../measurements/compiler-m3-threads.md).
The render ownership review and concurrent-render gate are now complete on
Linux; see [the render report](../measurements/compiler-m3-render.md). That audit
found and fixed borrowed type metadata and imported-expression materialization
paths; AddressSanitizer also found a serial semantic type-pointer lifetime bug.
Actual compiler renders now pass serial/concurrent identity, unchanged-input and
reuse checks under ThreadSanitizer and AddressSanitizer on the exercised inputs.
This completes the Linux M3 preparation gate; native Windows/macOS validation
remains pending. M4 is in progress: the bounded finite-index queue, inline
jobs=1 path, ordered
merge and transactional worker startup are implemented. The default stays one.
Production CLI parity and failure recovery pass under both sanitizers on Linux.
Batch dispatch/drain and opt-in allocator-lock acquisition measurements are
implemented. Memory-budget sizing and native Windows/macOS validation remain. See [the scheduler report](../measurements/compiler-m4-scheduler.md).
The [contention report](../measurements/compiler-m4-contention.md) records
corrected-target scaling and separates lock pressure from rising system CPU.
[Batched unchanged LLVM metadata text](../measurements/compiler-m4-metadata.md)
now reduces serial combine work with byte-identical output. Next: investigate
render scratch-arena reuse and reduced bookkeeping contention. Legacy memory-profile output and human timing
aggregation remain coordinator work. LLVM module rendering is opt-in parallel;
the default and the front end remain serial.
The [M1 evidence](../measurements/compiler-m1.md) revises the initial hypotheses:
usage-context inference, name-conflict scans and source-line lookup are measured
hotspots. Batch dispatch delay now includes startup and waiting behind earlier
tasks; isolated queue-mutex waiting is not yet measured.
