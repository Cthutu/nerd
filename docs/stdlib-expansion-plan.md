# Standard-library expansion plan

Status: first foundations implemented for draft review, 2026-10-01. Working
branch: `std-library`, based on Nerd `00e226c2`. Later milestones remain planned;
this is not a claim of completed ports.

The first implementation batch uses [parallel workstreams](stdlib-workstreams.md)
with separate branches, ownership boundaries and a later integration gate.

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

Design clarification (2026-10-02): Nerd remains a C replacement with generics,
traits and slices, not a language with enforced ownership/borrowing. Resource
lifetime, copying, cleanup and synchronisation are caller/API responsibilities.
Kerberos's intended direction is lightweight GCD-style dispatch; ASIO is an
implementation detail, not a required dependency or parity goal. The inspected
serial dataflow executor is evidence about that revision, not the design target.

## Source review and known starting point

| Source | Evidence | Review status |
| --- | --- | --- |
| [Raptor](https://github.com/cthutu/raptor) | Master `3ae8c14b75d29e751a51d3046bde51c201aa5245`, plus graph/continuations/v2 heads | Source inspected: ASIO scheduling, task/serial queues and experimental graph FIFO; no work-stealing implementation found in these heads |
| [Kerberos](https://github.com/cthutu/kerberos) | Master `09cae65e37bac03a06ce19862f95546793a723c4` | Source inspected: serial FIFO dataflow, named typed ports and fan-out; scheduler argument unused |
| [Nexus](https://github.com/cthutu/dev/tree/9ae32dd295241bd3889204537dfa476f222e753c/src/nexus) | `dev` revision `9ae32dd295241bd3889204537dfa476f222e753c` | Public header, implementation structure, README, PLAN, and tests inspected; original tests not executed during planning |

The [source inventory](stdlib-source-inventory.md) records exact branch revisions,
API/test mappings, ownership hazards and source copyright notices. Raptor and
Kerberos access was resolved using the existing repository-owner login; original
suites have not been built or executed. The 3 October decision is an original Nerd work-stealing scheduler built around
Raptor API semantics; M3 is not a direct algorithm port.

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
Its compiler has private C thread/mutex/task implementations. This review branch
now adds public `std.thread`, `std.sync` and initial `std.network` modules; public
queue, scheduler and graph modules remain future work. Follow [NSL standards](../mods/CODING-STANDARDS.md)
and the existing `core` / `std` / `os` separation.

## Proposed module boundaries

Names other than the requested `std.network` are working names; confirm them
with the source inventory before publishing APIs.

| Module | Responsibility |
| --- | --- |
| `std.thread` | Thread creation, joining and identity; explicit lifetime and callback context contracts |
| `std.sync` | Mutexes, condition variables and any semaphore/event primitive actually needed by the ports |
| `std.queue` | Reusable queues from the Raptor inventory; concurrency roles explicit in each type |
| `std.raptor` | Our own bounded work-stealing scheduler with Raptor queue/task semantics and explicit Nerd ownership |
| `std.graph` | Kerberos-derived node/edge storage, data and graph operations, usable without workers |
| `std.network` | Socket lifecycle, addressing, TCP/UDP byte I/O, readiness and portable errors |
| `std.nexus` | Message buffers, framing, routes and basic/request/reply/Telnet protocol behaviour |
| Platform bindings under `os` | Winsock/native Windows calls and Linux sockets/thread bindings |

Graph execution can depend on `std.raptor` through a separate adapter or child
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
The example catalogue below is part of the milestone exit gates, not a separate
cleanup task at the end. Record completion evidence against each commit; the
checkboxes start unchecked.

### M0 — Source inventory and language capability checks

- [x] Obtain Raptor/Kerberos source and record exact commits, API/test inventories
  and dependencies in the source inventory.
- [ ] Resolve required attribution/reuse terms before copying source material;
  the inspected headers name company copyright holders and contain no standalone
  repository licence grant.
- [ ] Build/run original reference suites where supported; record gaps rather
  than treating an unavailable upstream test as passing.
- [ ] Map every upstream component to port, internal detail, or explicit deferral.
- [ ] Select representative upstream tests for the example catalogue and record
  their repository revision, file and test name. Initial Raptor/Kerberos mappings
  are recorded below; upstream execution and final API choices remain open.
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

- [x] Implement minimal thread/join, mutex and condition-variable APIs; add other
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
- [x] Implement our bounded owner/thief deque correctness reference: mutex
  publication/removal, local LIFO/victim FIFO, last-item exclusion and wraparound.
  Storage is borrowed, fixed and shallow; no growth or reclamation algorithm is
  claimed. See [contracts](stdlib-raptor.md).
- [ ] Provide bounded model/reference tests and stress tests with unique item IDs:
  no loss, duplication, premature reads or double destruction; exercise empty/full
  transitions and shutdown. Add latency/throughput measurements separately.

Exit: standalone queues with documented ownership, capacity and progress guarantees.

### M3 — Work-stealing scheduler

Depends on M2.

- [x] Select our own Nerd work-stealing implementation around Raptor API semantics:
  first `std.raptor` slice uses bounded mutex deques, local LIFO/victim FIFO,
  external submission and explicit worker counts including single-worker mode.
  See [contracts](stdlib-raptor.md); this is not upstream algorithm parity.
- [x] Define typed/void completion, nested cooperative waits, bounded admission,
  drain and self/ancestor/same-serial rejection. Cancellation and general
  cross-worker cycle detection are not supported; caller lifetimes are explicit.
- [ ] Test exactly-once execution/completion, nested tasks, contention, idle wake-up,
  shutdown during submission and partial worker startup failure.
- [ ] Compare serial, one-worker and multi-worker results and performance using
  reproducible workloads, with task granularity and allocations reported.

Exit: usable `std.raptor` example, clean lifecycle tests and evidence for scheduler
behaviour. No change to Nerd compiler concurrency defaults or internals is implied.

### M4 — Kerberos graph/node data model and serial evaluation

Depends on Kerberos inventory and relevant M0 capabilities; M3 is not required
for graph storage or a serial evaluator.

- [ ] Port named typed ports, FIFO links and dataflow nodes with explicit ownership.
  Stable identifiers and stale-slot detection would be deliberate improvements
  over the source's borrowed node pointers, not source-parity requirements.
- [ ] Implement the source's serial data-availability evaluation semantics.
  There is no source dirty-cache/invalidation engine or topological traversal.
  Define cycle handling and mutation-during-evaluation behaviour before adding
  a parallel evaluator; do not silently convert general graphs into DAGs.
- [ ] Test empty/disconnected graphs, fan-in/fan-out, diamonds, cycles, removal,
  stale handles, repeated evaluation and source-defined data propagation.

Exit: source-parity dataflow examples and an explicit serial ordering contract
for later parallel comparisons. Deterministic node ordering would be a deliberate
Nerd improvement over the source's unordered-set visitation.

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

Exit: message echo and multi-client reply examples, standalone from `std.raptor`.

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

## Examples derived from upstream tests

Examples should turn a representative test scenario into a small, useful program:
show setup, normal operation, error handling and cleanup using the public API.
Retain focused regression tests separately; examples should not reproduce the
upstream test framework or expose private implementation details.

The network examples and M1 `thread-pipeline` are implemented in this review;
the other directories and scenarios remain proposed.
Nexus test names below refer to the pinned `dev` revision in the source inventory.
Raptor and Kerberos rows now reference inspected source tests. They have not yet
been executed upstream or ported into Nerd.

| Example directory | Milestone | User-visible scenario | Upstream test basis |
| --- | --- | --- | --- |
| `thread-pipeline` | M1 | Producer/consumer transfers 100 integers through a mutex/condition-protected slot and verifies total 5050 | New primitive-level prerequisite example; implemented |
| `queue-pipeline` | M2 | Producers submit numbered work items; consumers process every item once and shut down cleanly | New contention/ownership tests informed by graph-branch Raptor `DataQueue`; no upstream standalone queue suite found |
| `task-parallel` | M3 | Split a calculation into tasks, wait for completion and compare with its serial result | Raptor master `src/test_main.cc`: `Wait`, `VoidTasks`, `Serial` |
| `graph-pipeline` | M4, extended in M8 | Two generators feed a sum node and checker; evaluate serially before any parallel extension | Kerberos `src/test_pipeline.cc`: `Graph.GenKernel`, emits 0,2,...,18 |
| `network-echo` | M5 | A raw TCP client/server exchanges bytes and handles partial I/O and peer shutdown | New socket-level tests extracted from the transport scenarios behind `framing.c::tcp_message_framing_round_trip`; no Nexus framing in the raw example |
| `network-datagram` | M5 | Send a UDP datagram and reply to its sender | Socket-level counterpart of `message.c::udp_recv_msg_preserves_reply_route_for_send_msg` |
| `nexus-message` | M6 | Send structured messages with strings and integers, read them back and reuse the message buffer | `message.c::message_append_and_read_primitives`, `framing.c::tcp_message_framing_round_trip`, `framing.c::zero_length_messages_are_valid` |
| `nexus-multi-client` | M6 | Several clients contact one server; each reply goes to the originating client | `message.c::tcp_server_can_reply_to_multiple_clients_via_message_pipe` |
| `nexus-request-reply` | M7 | A client sends a request, receives a reply, and repeats with the required protocol order | `reqrep.c::request_reply_round_trip_over_tcp` and the request/reply wrong-state tests |
| `nexus-timeouts` | M7 | Show a bounded receive timeout, a non-blocking would-block result and recovery when a peer becomes ready | `message.c::recv_times_out_when_client_connects_but_sends_nothing`, `nonblocking_recv_returns_would_block_when_no_client_arrives`, `recv_succeeds_when_message_arrives_before_timeout` |
| `nexus-telnet` | M7 | A small line-oriented server, with character mode and terminal-size reporting as follow-on modes | `telnet.c::telnet_socket_round_trip_over_tcp`, `telnet_character_mode_receives_one_character_per_message`, `telnet_socket_reports_negotiated_bounds_from_naws` |
| `task-network-service` | M8 | Receive messages, dispatch bounded CPU work, return replies and drain cleanly on shutdown | Integration of the selected Raptor task tests and Nexus multi-client/reply tests; new overload/shutdown regressions |

### Packaging and acceptance

- [ ] Use the existing `examples/<name>/<name>.n` layout, with a README explaining
  the concept, the source test references, expected output and ownership rules.
- [ ] Make `just run-example <name>` work with the same command on Windows,
  Linux and WSL. Default demonstrations should finish on their own. For networking,
  a default self-contained run starts a local server and client; optional explicit
  client/server modes can support two-terminal exploration. Document how to pass
  arguments and extend the common recipe only if those modes need it.
- [ ] Give interactive Telnet modes a separate bounded smoke mode using a scripted
  peer, so automated checks never wait for a user or an installed Telnet client.
- [ ] Run examples against this checkout's compiler/modules. Put fixed linkage in
  source build settings, use loopback and dynamically assigned ports, and report
  errors clearly. Normal examples need neither Internet access nor elevated rights.
- [ ] Add execution smoke checks to the test workflow: the existing example family
  only performs `nerd check`, which is insufficient to verify message exchange,
  task completion or shutdown. Check meaningful results and exit status with a
  deadline; do not snapshot nondeterministic worker order or allocated port numbers.
- [ ] Validate debug and release execution on the supported platform matrix and
  retain regression coverage for failure cases omitted from the teaching example.
  No pending capability should be disguised by a stub or a silently skipped demo.

M2–M8 are complete only when their corresponding examples and smoke checks pass.
M9 verifies the common launch commands and documents any genuine platform limits.

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

Outstanding: source reuse terms for any future copied material;
remaining public module names; FIFO publication/reclamation strategy; scheduler
cancellation and draining contracts; and deliberate API improvements versus source
parity. The inspected queue/lifetime hazards must be fixed through explicit
contracts and tests, not carried into Nerd unchanged.

## Required API tutorials — 3 October 2026

Deliver runnable tutorials for every remade Raptor, Kerberos and Nexus API after
implementation. Cover practical usage, ownership, error/backpressure behaviour
and shutdown; run examples through native gates. The first
[Raptor tutorial](tutorials/raptor.md) accompanies `std.raptor`.
[Remaining tutorial requirements](tutorials/README.md) are completion gates for
M4, M6 and M7, rather than documentation of hypothetical APIs.
