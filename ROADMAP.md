# Open work

Confirmed compiler bugs take priority. Track reproductions and fixes in
[compiler issues](docs/compiler-issues.md). Current behaviour belongs in the
[manual](docs/manual/README.md), [specs](docs/spec/README.md) and subsystem docs;
completed plans remain in Git history.

## Active Standard-Library Work

### Kerberos and integration

- Design typed ports, FIFO links, graph/node ownership and serial dataflow from
  the [source inventory](docs/stdlib-source-inventory.md). Define cycle, mutation,
  ordering and stale-handle behaviour; do not assume graphs are DAGs. Module
  names and intentional improvements over upstream remain design decisions.
- Keep graph storage independent of workers. Add a Raptor execution adapter only
  after defining readiness, completion, errors and cancellation; compare its
  results with serial evaluation. Use our own work-stealing implementation,
  retaining Raptor API semantics, without an ASIO dependency.
- Deliver the Kerberos tutorial (two generators → sum → sink checking 0,2,…,18),
  covering ports, fan-out, ownership, errors and shutdown; add loader coverage
  after implementation. Existing [Raptor and Nexus tutorials](docs/tutorials/README.md)
  cover their implemented APIs.
- Add queue/graph pipelines and a bounded task/network-service example. Keep
  blocking I/O outside CPU workers; verify backpressure and coordinated shutdown.

### Networking extensions

- Add IPv6 and DNS resolution, with explicit resolver ownership, errors and
  blocking behaviour. Extend Nexus URLs with names and bracketed IPv6 addresses.
- Define remaining shutdown/socket options, address reuse, descriptor inheritance
  and cancellation. Preserve [network contracts](docs/std-network-foundation.md)
  and [Nexus parity](docs/nexus-parity.md); further messaging patterns need design.

### Image and compression

- Add `Image.load(path, desired_channels = 0)` using `std.files`, with
  source-relative fixtures and missing/corrupt-file regressions.
- Replace placeholder viewer assets with representative, suitably licensed images.
- Consider progressive JPEG, 16-bit/interlaced PNG and additional formats only
  for concrete use cases. Keep the decoder Nerd-native, allocations explicit,
  CRC checks mandatory and unsupported variants rejected cleanly.
- Add raw deflate or gzip only when callers need them; the existing compression
  API provides zlib-wrapped deflate.

### Validation and delivery gates

- Complete the upstream component disposition matrix and supported reference-suite
  runs. Record unavailable checks; resolve reuse/attribution before copying code.
- Audit queue/task/thread coverage for publication, payload/context lifetimes,
  atomic/native layouts, allocator/TLS behaviour, exactly-once completion,
  contention, wake-ups, nested waits, partial startup and shutdown. Add focused
  regressions for gaps; stress passes do not prove memory-order correctness.
- Compare serial, one-worker and multi-worker performance using reproducible
  workloads that report task granularity and allocations. Keep compiler defaults
  at one job until the [adoption gates](docs/compiler-profiling.md#scheduler-adoption-policy) pass.
- Complete sustained resource-growth checks and remaining interactive/platform
  observations. Follow the [Windows handoff](validation/windows/HANDOFF.md);
  validate native Windows, Linux and WSL independently. macOS is a separate port.
- Check runnable tutorials/examples for results, ownership, errors and shutdown
  under LLVM/C and debug/release builds, including installation and editor support.
  Keep network/Nexus execution and long stress/benchmarks opt-in, with bounded
  deadlines, process cleanup, loopback and listener-assigned ports.

## Compiler and tooling

- Improve semantic editor operations and harden incomplete/broken-import analysis;
  reproduce crashes with incremental-edit transcripts. Keep VS Code and Neovim
  integrations aligned. See [LSP](docs/lsp.md) and [editor support](docs/editor-support.md).
- Extract shared formatter spacing/comment/layout policies with partial-source and
  idempotence coverage. Keep AST/CST classification and recovery aligned; parser
  unification needs a separate design.
- Add structured diagnostic expectations where retained command tests only assert
  failure. Preserve genuine CLI/process contracts and [feature coverage](docs/test-features.md).
  Further test consolidation or shared LLVM/C baselines must demonstrate fewer
  compiler/tool launches while retaining IR, ABI, platform and cleanup assertions.
- Continue [debugger follow-up](DEBUGGER.md#remaining-work).

## Deferred ideas

These require a concrete use case or a fresh design before implementation:

- Cross-target layouts, aggregate FFI ABI and calling-convention annotations.
- In-place AST compaction for constant folding.
- Interpolation formatting specifiers and arithmetic/bitwise/assignment traits.
- Tree-sitter after the semantic LSP experience is established.
- Non-fail-fast source-test assertions and synthetic hard-limit diagnostic tests.
- Parametrised modules replacing general generics remain on hold.
