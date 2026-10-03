# Raptor and Kerberos source inventory

Inspected 2026-10-01 using the existing repository-owner GitHub login. Both
repositories are private; earlier access failures were credential scope, not
missing repositories. This is source inspection, **not upstream execution**.
Cached archives are ignored local review inputs under `_tmp/std-library-review`;
they are not bundled into Nerd. No implementation is copied in this tranche.

User design clarification (2026-10-02): Kerberos is intended as a lightweight
Grand Central Dispatch-style system. ASIO is an implementation detail and need
not be retained. The serial graph behaviour below describes the inspected source,
not a restriction on the intended dispatch design.

## Pinned revisions

| Repository/branch | Commit | Observed scope |
| --- | --- | --- |
| Raptor `master` | `3ae8c14b75d29e751a51d3046bde51c201aa5245` | ASIO workers, task results, concurrent/serial queues, `when_all` |
| Raptor `graph` | `f82fbb17f11be6a08c26bb35d7bf8afec00ba0e0` | Adds atomic ring FIFO, ports, nodes and synchronous graph execution |
| Raptor `continuations` | `c6e4dcda38e154c23ac87b207358837f10edc381` | Continuation sketches/tests; unfinished implementation |
| Raptor `v2` | `f72c2454314ee2a17985cf55d14e898d493a8766` | Result/task/combiners redesign; several declared methods unimplemented |
| Kerberos `master` | `09cae65e37bac03a06ce19862f95546793a723c4` | FIFO dataflow, loader and bundled ASIO scheduler |

No Raptor tags were found. None of these inspected branch heads implements a
custom work-stealing deque/scheduler: scheduling delegates to ASIO. Work stealing
remains the user's requested design goal; confirm the intended source revision
or deliberately select a new algorithm before calling it a port.

## Raptor APIs and examples

Primary sources are `src/raptor.h` and `src/raptor.cc`; build configuration is
`make/premake5.lua` (Windows x64/C++17). Concurrent queues post to ASIO's
`io_service`; serial queues use an ASIO strand. Tasks expose results and waiting;
`when_all` combines completion. The graph branch's `DataQueue<T>` is a bounded
ring FIFO, not a work-stealing deque. `Graph::exe` ignores its scheduler argument
and executes nodes synchronously.

| Source | Inspected tests | Nerd mapping |
| --- | --- | --- |
| Master `src/test_main.cc` | `Wait`, `VoidTasks`, `Serial` | M3 task results, void completion and serial dependency example |
| Graph `src/test_task.cc`, `src/test_graph.cc` | Task checks plus generator → sum → print/fan-out graph scenario | M4 serial dataflow example; compare with Kerberos's more developed tests |
| Continuations/v2 branches | Incomplete continuation/result redesign checks | Defer until API intent and completion are resolved |

Inspection identifies hazards that need targeted tests and redesigned contracts:

- Queued callbacks retain raw task pointers while returned handles use
  `shared_ptr`; dropping a handle can invalidate queued work.
- `when_all` borrows container iterators. Container and task lifetimes must be
  defined independently of submission/completion timing.
- Shutdown stops ASIO then joins workers; this is not a guarantee that all
  accepted work drains.
- Graph FIFO publication CAS and consumer slot-release ordering need independent
  correctness work. The consumer advances its read index before copying payload,
  allowing potential slot reuse. The publication CAS mutates the expected index
  after failure. Do not treat this as a validated MPMC algorithm.
- `push(const T&)` followed by `std::move(value)` generally copies a const payload;
  it is not evidence of ownership transfer.

These are source-review findings, not executed race detections. Nerd queue tests
must cover concurrent consumers/producers, publication, wraparound, full/empty
transitions, payload destruction and shutdown, not reproduce the hazards.

## Kerberos dataflow semantics

Core types include `Fifo`, `DataQueue<T>`, `Port`, `Ports`, `InputPorts`,
`OutputPorts`, `Node`, `NodePair` and `Graph`.

- Input/output ports have names and intended payload types. Each input connects
  to one FIFO; outputs can fan out to several.
- A single destination receives a moved value; fan-out copies to destinations.
- A node is ready when every input has data. Sources return `Incomplete` or
  `Complete`; the serial evaluator repeatedly runs incomplete sources and ready
  internal nodes. It ignores internal nodes' returned completion state.
- `Graph::exe(Scheduler&)` is serial and does not use its scheduler parameter.
  Nodes are visited through unordered sets, not a topological order.
- There is no stable-ID/removal/dirty-cache/invalidation API, cycle detection or
  mutation-during-execution contract. Cycles are not explicitly rejected, but
  useful cyclic execution is not established; graphs without sources do not start.
- Graphs own connection FIFOs and borrow nodes. `Loader` owns the nodes it creates.

`Loader` supplies node-type registration, `load`, `addToGraph`, `hasError`, an
error callback and string/integer/float fields. It parses `.g` instance/connection
syntax with `>>` and optional named endpoints. The JSON file in `data` is
illustrative, not the inspected parser's format.

The bundled scheduler has `Result<T>`, `Task<T>`, `Queue::async`, serial/concurrent
queues, queue creation/default access and `when_all`. It uses ASIO workers and
strands. A declared timer queue is not implemented.

| File | Active tests | Nerd mapping |
| --- | --- | --- |
| `src/test_queue.cc` | `DataQueue.Empty`, `PushPop`, `Strings`, `FailPop`, `Full`, `Multithread` | M2 FIFO baseline; extend concurrency and ownership coverage |
| `src/test_pipeline.cc` | `DataQueue.Complex`, `Graph.GenKernel`, `Graph.Loader` | M4 payload, serial dataflow and later loader examples |
| `src/test_scheduler.cc` | `Scheduler.Basic`, `Wait`, `VoidTasks`, `Serial` | M3 task and serial execution contracts |

There are 13 active project tests. `Graph.GenKernel` (line 132) connects two
0–9 generators to a sum node and sink checking 0,2,...,18: use this as the first
`graph-pipeline` example. `Graph.Loader` (line 556) and `data/pipeline.g` provide a
later JPEG-to-PNG fan-out/join example; the direct `Graph.JPG_Convertor` test is
commented out. Queue stress joins concurrent producers before consuming serially;
it does not prove simultaneous MPMC behaviour.

Additional source-review hazards: the port compatibility field remains zero,
so intended type checking is ineffective; FIFOs are deleted through a base
without a virtual destructor; blocking pushes can stall a serial graph before
consumers run; there is no general deadlock/cycle/bounded-execution contract.
Specify intentional improvements and regressions before porting mechanics.

## Dependencies and provenance

Both projects use C++17/ASIO and carry Windows/Premake build assumptions. The ASIO
submodule is pinned to `b0926b61b057ce563241d609cae5768ed3a4e1b1`; downloaded
repository archives do not populate it. GoogleTest is vendored. Kerberos also
vendors `stb_image` 2.19 and `stb_image_write` 1.09, with their own notices.

Neither repository has a standalone licence file or GitHub licence metadata.
Raptor core headers name Bit-7 Technology (2018, all rights reserved); Kerberos
core headers name Gameloft Entertainment (2018, all rights reserved) and Matt
Davies as author. Some tests instead name Matt Davies. Record the intended reuse
and attribution terms before copying implementation material; source access alone
does not resolve those terms. This inventory changes neither project's notices.

M0 still needs upstream builds/execution, a complete component disposition matrix
and reuse decisions. M2/M3 should explicitly resolve queue correctness and the
intended work-stealing implementation. M4 can start with serial FIFO dataflow;
stable handles, cached invalidation and parallel scheduling are additions.

## Selected Nerd scheduler direction — 3 October 2026

The user selected an original Nerd work-stealing implementation retaining the
Raptor queue/task model, with no ASIO copy or dependency. The first implementation
is `std.queue` plus `std.raptor`: bounded mutex deques, local LIFO/victim FIFO,
serial/concurrent dispatch, typed/void completion, cooperative waits and drain.
See [contracts and deliberate API differences](stdlib-raptor.md). The earlier
search for an upstream work-stealing revision is closed by this decision; source
reuse terms still apply to any future copied material. No upstream implementation
was copied for this slice. Runnable tutorials for Raptor, Kerberos and Nexus are
required delivery gates; [status and walkthroughs](tutorials/README.md).
