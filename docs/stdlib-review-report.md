# Standard-library foundations: additions and recommendations

Date: 2026-10-01. Review branch: `std-library`.

This tranche adds usable threading, synchronisation and IPv4 socket foundations,
three runnable examples, compiler/runtime repairs and source inventories for the
later ports. It does **not** implement the Raptor scheduler, Kerberos graphs or
Nexus protocols. The complete Windows gates pass; native Linux adoption remains
the next validation step. The recommendations below are proposals, not additional
features already implemented or approved language syntax.

## What was added

| Area | Delivered behaviour | Main reference |
| --- | --- | --- |
| `std.thread` | Explicit callback/context ownership, joinable workers, reuse and self-join rejection; Windows CRT and Linux pthread entry wrappers | [Thread/sync contracts](stdlib-thread-sync.md) |
| `std.sync` | Non-recursive mutexes and condition variables, with explicit initialisation/destruction and predicate-loop waits | [Thread/sync contracts](stdlib-thread-sync.md) |
| `std.network` | IPv4 TCP/UDP creation, binding, listening/accepting, connection, partial byte I/O, EOF, datagram sender/truncation handling, nonblocking I/O, send-side shutdown and native error detail | [Network contracts](std-network-foundation.md) |
| Native bindings | Independent `os.thread` and `os.socket` modules; fixed linkage stays in source build settings | [Module index](stdlib.md) |
| Runtime | Worker-local temporary arena/string-builder cleanup, synchronised debug allocation bookkeeping and explicit `current_temp_arena()` | [Compiler/runtime internals](overviews/INTERNALS.md) |
| Capability evidence | Atomic operations, pointer publication, callback context, payload-copy behaviour and graph-storage building blocks | [Capability audit](stdlib-capability-audit.md) |
| Planning | Pinned upstream revisions, API/test/provenance inventory, milestones and integration records | [Source inventory](stdlib-source-inventory.md), [plan](stdlib-expansion-plan.md) |

The initial native bindings cover Windows x64 and Linux x86-64 glibc. Native
header layout assertions validate the supported ABI; other architectures/libcs
are not claimed. Resource handles remain non-copyable and address-stable by
documented contract, rather than enforced by the language.

The examples are bounded executable tests, not merely syntax demonstrations:

| Example | Demonstrates | Run |
| --- | --- | --- |
| `thread-pipeline` | Producer/consumer transfer through a mutex/condition-protected slot; 100 values total 5050 | `just run-example thread-pipeline` |
| `network-echo` | Local TCP client/server byte exchange | `just run-example network-echo` |
| `network-datagram` | Local UDP request and reply to the sender | `just run-example network-datagram` |

Both common test recipes execute the library contracts and examples using fresh
checkout compilers/modules. The new tests cover native layouts, worker lifecycle,
temporary-storage isolation/cleanup, predicate waits and socket ownership/I/O
semantics. Loopback tests bind actual ephemeral ports and use bounded process
execution rather than fixed ports or startup sleeps.

## Compiler repairs found during implementation

| Problem | Resulting behaviour and regression |
| --- | --- |
| Generated C cached imported constants and could execute unused initialisers | Imported/qualified constants now evaluate per use, matching LLVM/local constants; mutable globals still initialise once |
| Imported constant binders could overwrite caller locals | Expansions isolate local names and bookkeeping; retained-caller-local regression covers the collision |
| LLVM mis-lowered imported mutable globals and could silently stop a function | Imports resolve to defining storage, including re-exports; same-named globals stay distinct and root public symbol names remain stable |
| Failed LLVM lowering was treated like normal fallthrough | A failed render now produces a diagnostic and rejects output instead of generating a success-looking partial function |
| Formatting removed standalone/guarded mutable variables' `pub` modifier | Visibility is preserved for typed, inferred, defaulted and guarded declarations |
| Packed bit-field writes through pointers failed lowering | The containing storage is resolved once, with adjacent-field and owner-call-count checks |
| Optional-void construction mishandled a returned void expression | LLVM omits nonexistent payload storage while retaining evaluation; generated C also preserves the call's side effects exactly once |

The stricter failure reporting exposed previously hidden bugs. Strengthened
fixtures assert completion output and observable effects, so returning early can
no longer masquerade as a passing executable. Diagnostic snapshots were updated
only where the new core declaration shifted source coordinates.

## Validation and its limits

Implementation revision `2b540cd9` passed:

- Native Windows `just test` and `just test-release`: **1181 fixtures passed,
  zero failures, 15 platform skips each**, followed by their library gates.
- All debug auxiliary checks, including **298 LLVM/C differential fixtures at
  O0/O2** (two platform skips), 6000-term depth, concurrency, toolchain/temporary
  installation, formatter workflow, source build settings and Windows stdio.
- A fresh Arch WSL build and bounded generated-C O0/O2 runtime/thread/network
  checks, including the strengthened bit-field and optional-void regressions.
- `just format`; subsequent compiler formatting changes were whitespace-only.

See the [combined evidence](../validation/windows/results/20261001-stdlib-integration/README.md)
for exact scope, logs and preserved failures. WSL lacks `opt`, `llc`, `ld.lld` and
`llvm-ar`, so its C-output checks are not a native LLVM pass. Standalone Linux,
full `just do`, desktop/editor and benchmark adoption gates remain open. No global
compiler/editor installation changed. Original upstream suites were not executed.

## Language and compiler recommendations

The current APIs can support further bounded work under their documented
contracts. The following improvements would make them safer or easier to use;
they should be separate, regression-backed changes rather than an unplanned
language redesign inside this library PR.

| Priority | Recommendation | Evidence and practical next step |
| --- | --- | --- |
| High | Distinguish copying, ownership transfer and address stability | The payload probe shows dynamic-array copies alias storage. Define consuming operations that invalidate the source and return ownership on rejection. Explore non-copyable owners, but also a pinning/address-stability rule: moving an active `Thread`, `Mutex` or `Condition` can be invalid even if copying is forbidden |
| High | Strengthen typed callbacks at the FFI boundary | Thread entry points currently cross the native boundary through `^void` casts. Investigate typed function-pointer parameters and explicit ABI checking, with Windows/Linux callback execution regressions. Keep callback context lifetime explicit; a typed pointer alone does not prove it outlives a worker |
| High before queue optimisation | Make layout and atomic guarantees reviewable | Existing probes establish natural pointer-sized alignment, not cache-line separation or lock-free progress. Assess explicit alignment/layout assertions and expose/document supported atomic widths/order guarantees before choosing a queue/reclamation algorithm |
| Medium | Standardise cleanup and move conventions before adding complex owners | Use existing explicit cleanup/defer facilities consistently now. Specify whether future destructor/drop support should handle partially initialised owners and moved values; do not introduce implicit copies or double destruction through generic containers |
| Medium | Improve contextual typing for atomic initialisers | `atomic[usize]` currently needs `7.as(usize)` where a bare literal is rejected. Add a focused contextual-inference regression and assess a compiler fix rather than proliferating casts across queue code |
| Later, after ownership semantics | Consider static restrictions on values crossing threads | A future sharing/transfer contract could reject unsafe captures or payloads. Define its interaction with raw pointers, aliases and synchronisation first; adding marker traits alone would not establish thread safety |

Retain LLVM/C parity checks for imported values, side effects and every new owning
API. Extend negative tests for backend failure propagation whenever another
unsupported lowering case is found; do not restore silent fallback returns.

## Library and workflow recommendations

1. **Finish native Linux validation before merging into `main`.** Follow the
   [handoff](../validation/windows/results/HANDOFF.md): run both test recipes and
   the examples with the required LLVM tools, then exercise `just do` when ready
   to update that PC's installation. Automate the supported Windows/Linux matrix
   once these workflows are established; keep WSL evidence labelled separately.
2. **Complete lifecycle/error coverage before a scheduler.** Add deterministic
   native startup/initialisation failure injection, partial-worker-pool cleanup,
   contention stress and timed waits using monotonic deadlines. Review whether
   thread/sync boolean failures should expose native error detail, as networking
   already does. Preserve the distinction between locking allocator bookkeeping
   and making application allocations/arenas safe for shared mutation.
3. **Choose and prove the queue contract before optimising it.** Start with a
   bounded reference implementation and explicit producer/consumer roles. Test
   wraparound, publication, concurrent consumers, rejection ownership, shutdown
   and payload destruction before claiming lock-free or work-stealing behaviour.
   Benchmark granularity, allocation and contention only after correctness passes.
4. **Add readiness/deadlines before building Nexus atop blocking workers.** Next
   socket slices should cover nonblocking connect completion, readiness, timeout
   semantics, DNS/IPv6 and socket options/inheritance policy. Nexus must handle
   partial I/O, bounded framing/buffering, reply-route lifetime and late replies;
   do not let blocking socket waits exhaust a future worker pool.
5. **Port a small serial graph example first.** Kerberos's `Graph.GenKernel`
   gives a concrete two-generators → sum → sink reference. Specify fan-out payload
   ownership: C++ value copying cannot be assumed equivalent to Nerd dynamic-array
   copying. Avoid serial deadlock on a full FIFO. Stable IDs, deterministic node
   order, invalidation and parallel evaluation are deliberate additions, not
   existing upstream guarantees.

## Source decisions still needed

Repository access is resolved, but the inspected Raptor branch heads use ASIO
scheduling, not a custom work-stealing scheduler. Confirm the intended newer
revision/repository or explicitly choose a new algorithm. Kerberos is serial FIFO
dataflow; its scheduler argument is unused. The [inventory](stdlib-source-inventory.md)
records exact revisions/tests and concrete lifetime/publication/type-validation
hazards that should be repaired through contracts and tests, not copied unchanged.

The repositories lack standalone licence grants, and their headers name company
copyright holders. Resolve the intended reuse/attribution terms before copying
implementation material. Source access and passing Nerd foundation tests do not
establish those terms or upstream parity.

Recommended sequence: Linux adoption checks and API review, source/algorithm
decisions, bounded queues and serial dataflow in separate branches, then scheduler
and socket-readiness work, followed by Nexus framing/protocols. Keep each milestone
reviewable with an executable example and an explicit evidence boundary.
