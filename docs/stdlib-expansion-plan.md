# Standard-library expansion plan

Status: proposed, 2026-10-01. Working branch: `std-library`, based on Nerd
`00e226c2`. This is a plan, not an implementation or a claim of completed ports.

## Objective

Bring Matt Davies's Raptor scheduler/queues, Kerberos graph/node data graphs,
and Nexus networking into idiomatic Nerd standard-library modules. Provide a
standalone cross-platform BSD-sockets-style `std.network` underneath the
message-oriented `std.nexus`. Initial supported platforms are native Windows
and Linux, with a separate WSL validation run. macOS is a later explicit port.

Port behaviour and tests into Nerd; keep native calls behind `os` modules.
Production users should not need to build the original C/C++ libraries. Retain
small original-language reference programs where useful for comparative tests.
No compiler scheduler replacement is included in this project.

## Source review and known starting point

| Source | Evidence | Review status |
| --- | --- | --- |
| [Raptor](https://github.com/cthutu/raptor) | User identifies work-stealing scheduler and queues | Repository inaccessible with current credentials; exact algorithms, API, dependencies and tests must be inventoried in M0 |
| [Kerberos](https://github.com/cthutu/kerberos) | User identifies graph/node data graphs | Repository inaccessible with current credentials; graph semantics and execution model must be inventoried in M0 |
| [Nexus](https://github.com/cthutu/dev/tree/9ae32dd295241bd3889204537dfa476f222e753c/src/nexus) | `dev` revision `9ae32dd295241bd3889204537dfa476f222e753c` | Public header, implementation structure, README, PLAN, and tests inspected; original tests not executed during planning |

Nexus already separates OS access (`lowlevel.c`), TCP/UDP transports, pipes,
message framing, request/reply and Telnet protocols. It has reusable message
buffers, reply routes, multiple clients, non-blocking operations and timeout/
connect-retry options. Its documented URL parser handles IPv4 literals only;
DNS and IPv6 URLs are additions, not existing parity requirements. Its header
sets a 1 MiB message limit. Preserve or explicitly version these contracts.

Reference tests are in
[`tests/nexus`](https://github.com/cthutu/dev/tree/9ae32dd295241bd3889204537dfa476f222e753c/tests/nexus):
`message.c`, `framing.c`, `reqrep.c`, `telnet.c`, and `test_net.h`.
They cover framing and empty messages, oversized sends, primitive encoding,
TCP/UDP reply routing, multiple clients, protocol state, timing/non-blocking
behaviour and Telnet. Map every source test to a Nerd test or a documented
intentional behaviour change before declaring parity.

Nerd already has `std.atomics`, memory/time utilities, and platform FFI modules.
Its compiler has private C thread/mutex/task implementations, but these are not
public Nerd threading or scheduling modules. There are no existing public queue,
graph or socket modules to extend. Follow [NSL standards](../mods/CODING-STANDARDS.md)
and the existing `core` / `std` / `os` separation.

## Proposed module boundaries

Names other than the requested `std.network` are working names; confirm them
with the source inventory before publishing APIs.

| Module | Responsibility |
| --- | --- |
| `std.thread` | Thread creation, joining and identity; explicit lifetime and callback context contracts |
| `std.sync` | Mutexes, condition variables and any semaphore/event primitive actually needed by the ports |
| `std.queue` | Reusable queues from the Raptor inventory; concurrency roles explicit in each type |
| `std.task` | Raptor-derived work-stealing scheduler, task completion and worker lifecycle |
| `std.graph` | Kerberos-derived node/edge storage, data and graph operations, usable without workers |
| `std.network` | Socket lifecycle, addressing, TCP/UDP byte I/O, readiness and portable errors |
| `std.nexus` | Message buffers, framing, routes and basic/request/reply/Telnet protocol behaviour |
| Platform bindings under `os` | Winsock/native Windows calls and Linux sockets/thread bindings |

Graph execution can depend on `std.task` through a separate adapter or child
module; graph storage must not start threads. Nexus uses `std.network` directly
and does not require a scheduler for synchronous or non-blocking use. Scheduler
integration must not turn blocking socket waits into worker-pool starvation.

Use ordinary source `build` settings/platform guards for fixed native linkage
and paths. Keep optional experiment/test defines on the command line. Do not
add consumer-specific command-line flags just to import these modules.

## Contracts to settle before implementation

- **Ownership:** identify owners of queue elements, task contexts, graph data,
  sockets and message buffers. Define transfer on both success and failure,
  borrowing lifetimes and exactly-once destruction. Do not assume Nerd currently
  prevents every unsafe copy or cross-thread capture; prove support in M0/M1.
- **Concurrency:** document owner-only versus multi-producer operations, memory
  orders, atomic width/alignment, publication and reclamation. A queue is not
  labelled lock-free without its progress and allocation/reclamation conditions.
- **Shutdown:** define drain versus cancel, accepted/rejected submissions,
  waiter wake-up, joining, and which operations may race with close. No hidden
  detached worker threads or callbacks after their owning context is freed.
- **Graphs:** distinguish graph storage from dataflow evaluation. Do not assume
  Kerberos is DAG-only. Establish cycle policy, node/edge identity, invalidation,
  mutation rules, ordering and data ownership from its actual implementation.
- **Networking:** socket handles are opaque and wide enough for Windows SOCKET;
  a POSIX `int` is not the portable representation. Separate EOF from would-block,
  timeout and failure; retain native error detail alongside portable categories.
- **Timing:** monotonic deadlines, explicit immediate/infinite waits, partial
  progress and cancellation behaviour. Decide thread safety per object rather
  than claiming all public operations are concurrent.
- **Messaging:** framing, byte order, maximum size, buffer ownership, reply-route
  lifetime, malformed input, partial I/O, reconnect and bounded buffering are
  public contracts. A timed-out request must not cause a late reply to be mistaken
  for a later request. Nanomsg inspiration does not imply wire compatibility.

## Milestones and exit gates

Each milestone produces a small usable API, tests, an example and documentation.
Record completion evidence against its commit; the checkboxes start unchecked.

### M0 — Source inventory and language capability checks

- [ ] Obtain Raptor/Kerberos source, record exact commits, API/test inventories,
  dependencies and required attribution for all imported material.
- [ ] Build/run original reference suites where supported; record gaps rather
  than treating an unavailable upstream test as passing.
- [ ] Map every upstream component to port, internal detail, or explicit deferral.
- [ ] Compile small Nerd probes for atomic compare/exchange and pointer-sized
  state, alignment, generic payload ownership, callback context lifetime, thread
  entry ABI, TLS requirements and platform struct layouts. Avoid aggregate-by-value
  FFI where the current backend does not support it.
- [ ] List any compiler/runtime prerequisites as separate regression-backed work.

Exit: reviewed API/behaviour matrix and an executable capability baseline. Raptor
and Kerberos implementation milestones cannot close before their inventory exists.
Network design and the already-accessible Nexus review can proceed independently.

### M1 — Portable thread and synchronisation foundations

Depends on M0's relevant capability checks.

- [ ] Implement minimal thread/join, mutex and condition-variable APIs; add other
  primitives only where source ports require them.
- [ ] Define explicit context lifetimes, thread-local allocator behaviour,
  initialisation failures and partial-start cleanup.
- [ ] Test predicate waits, missed/spurious wake-ups, startup failure, orderly
  shutdown and resource release on Windows, Linux and WSL.

Exit: race-safe producer/consumer example and passing native foundation tests.

### M2 — Raptor queue primitives

Depends on M1 and Raptor inventory.

- [ ] Port the inventoried queue types in increasing complexity. Explicitly
  distinguish SPSC/MPSC/MPMC roles where present; do not promise all variants
  merely because the scheduler needs one.
- [ ] Implement the owner/thief deque contract needed by work stealing. Preserve
  the reviewed algorithm and its memory-order reasoning, including the last-item
  race, wraparound, growth and reclamation if applicable.
- [ ] Provide bounded model/reference tests and stress tests with unique item IDs:
  no loss, duplication, premature reads or double destruction; exercise empty/full
  transitions and shutdown. Add latency/throughput measurements separately.

Exit: standalone queues with documented ownership, capacity and progress guarantees.

### M3 — Work-stealing scheduler

Depends on M2.

- [ ] Port worker-local scheduling, external submission and stealing from Raptor;
  include explicit worker counts and a single-worker mode.
- [ ] Define task completion, nested submission/waits, errors, drain/cancel and
  self-wait handling. Introduce dependencies only where the source/API requires them.
- [ ] Test exactly-once execution/completion, nested tasks, contention, idle wake-up,
  shutdown during submission and partial worker startup failure.
- [ ] Compare serial, one-worker and multi-worker results and performance using
  reproducible workloads, with task granularity and allocations reported.

Exit: usable `std.task` example, clean lifecycle tests and evidence for scheduler
behaviour. No change to Nerd compiler concurrency defaults or internals is implied.

### M4 — Kerberos graph/node data model and serial evaluation

Depends on Kerberos inventory and relevant M0 capabilities; M3 is not required
for graph storage or a serial evaluator.

- [ ] Port the inventoried node, edge and data model with explicit ownership and
  stable identifiers; detect stale identifiers if storage reuses slots.
- [ ] Implement the actual source traversal/evaluation/invalidation semantics.
  Define cycle handling and mutation-during-evaluation behaviour before adding
  a parallel evaluator; do not silently convert general graphs into DAGs.
- [ ] Test empty/disconnected graphs, fan-in/fan-out, diamonds, cycles, removal,
  stale handles, repeated evaluation and source-defined data propagation.

Exit: source-parity examples and deterministic serial behaviour that can serve
as the reference for later parallel execution.

### M5 — Low-level `std.network`

Depends on relevant M0/M1 foundations; independent of M2–M4.

- [ ] Add platform bindings and native layout checks, with owned sockets and
  Winsock startup/cleanup managed safely. First deliver IPv4 TCP/UDP, then IPv6
  and name resolution as explicit substeps of this milestone.
- [ ] Provide BSD-style create/bind/listen/accept/connect, send/receive,
  send-to/receive-from, shutdown/close, local/peer address and selected options.
  Preserve partial byte counts and distinguish zero-length datagrams from TCP EOF.
- [ ] Add non-blocking connect completion, readiness polling and portable timeout
  handling. Validate native error mapping, interrupted operations, integer limits,
  broken-pipe behaviour and platform differences in address-reuse options.
- [ ] Document that synchronous resolution may block; define resolver errors and
  ownership separately from socket I/O. Keep platform-only options explicit.

Exit: TCP echo and UDP examples with IPv4/IPv6 loopback, local name resolution,
partial I/O, refused connection, timeout, half-close and cleanup tests. IPv6
unavailability must be an explicit capability result, not an unexplained pass.

### M6 — Nexus message transport

Depends on M5's IPv4 TCP/UDP/readiness subset; full M5 closure may proceed alongside.

- [ ] Port reusable message buffers, encoding helpers, TCP framing, UDP message
  boundaries, client/server pipes, multiple clients and reply-route metadata.
- [ ] Set bounded buffering/backpressure rules and preserve parser state across
  fragmented headers/payloads, coalesced messages, would-block and deadlines.
- [ ] Port framing/message tests; add malformed/oversized frames, close mid-frame,
  stale reply routes and payload lifetime tests.
- [ ] Run C-to-Nerd and Nerd-to-C TCP/UDP interoperability checks against the
  pinned source. Specify any intentional wire-format differences before release.

Exit: message echo and multi-client reply examples, standalone from `std.task`.

### M7 — Nexus protocol parity

Depends on M6.

- [ ] Port request/reply state machines, timing/retry/non-blocking options and
  sender inspection, including recovery rules after timeout/disconnection.
- [ ] Port Telnet line/character mode, negotiation filtering and NAWS behaviour
  in a separate protocol component; it must not complicate raw sockets.
- [ ] Map all source network tests to passing Nerd equivalents. Add adversarial
  fragmentation, fairness and reconnect tests with bounded resource usage.
- [ ] Add DNS and bracketed IPv6 Nexus URLs using M5, clearly marked as extensions
  beyond the inspected source; no implicit ZeroMQ/nanomsg interoperability claim.

Exit: complete agreed Nexus parity matrix and runnable request/reply and Telnet
examples. Additional patterns such as pub/sub or push/pull need a later proposal.

### M8 — Graph/scheduler and network integration

Depends on M3/M4 for graphs and M5–M7 for networking.

- [ ] Add a graph execution adapter with explicit dependency readiness,
  completion/error/cancellation and mutation rules. Compare with serial evaluation.
- [ ] Demonstrate processing network messages with scheduled work, keeping blocking
  I/O outside CPU workers or explicitly using a supported readiness driver.
- [ ] Test bounded queues, overload/backpressure, cancellation and shutdown across
  the combined layers. Keep the standalone APIs usable without this integration.

Exit: one data-graph example and one bounded network-service example with clean,
repeatable shutdown and matching serial/parallel results.

### M9 — Cross-platform adoption

Depends on the preceding agreed feature gates.

- [ ] Run `just test`, release-compiler fixtures and `just do` on native Windows,
  Linux and WSL with native tools per host. Record versions, skips and installation
  outcomes separately; WSL is not evidence for native Windows socket behaviour.
- [ ] Add bounded deterministic tests to the normal gate; keep longer stress runs
  and benchmarks as named opt-in commands. Run sanitizers on reference/native code
  where supported, without claiming they prove Nerd lock-free code correct.
- [ ] Verify LLVM and C output for the affected language/runtime features, module
  installation, documentation, examples, formatter and LSP support.
- [ ] Publish ownership/thread-safety/error contracts and measured limitations;
  no unsupported performance or cross-platform guarantees.

Exit: complete platform matrix, updated standard-library reference, and reviewed
merge into `main`. Remove the working branch only after the work is merged.

## Test discipline

Use loopback by default and bind port zero on the actual listener; communicate
its chosen port after it is ready. Do not probe a port, close it and later rebind,
or depend on fixed startup sleeps. This deliberately improves the upstream
Nexus helper behaviour while preserving what each test asserts.

Tests need bounded deadlines and process-tree cleanup on failure. No external
Internet service is needed by the normal gate. Use explicit barriers for
concurrency scenarios and repeated stress separately; a stress pass is not a
proof of memory-order correctness. Native Windows, Linux and WSL results are
recorded independently. Hardware throughput thresholds do not belong in generic CI.

## Implementation order and outstanding decisions

Start with M0 and M1, then pursue two independent paths: M2/M3 plus M4 for
concurrency/graphs, and M5/M6/M7 for networking. M8 joins them; M9 closes adoption.
Do not assign calendar estimates until the Raptor/Kerberos inventories establish
scope and the language capability probes expose prerequisites.

Outstanding: Raptor/Kerberos access and exact revisions; final public module names;
source-derived graph semantics; queue reclamation strategy; scheduler cancellation
contract; and the amount of deliberate API compatibility versus Nerd-specific
simplification. Proposed defaults above are design recommendations, not settled
facts about inaccessible source repositories.
