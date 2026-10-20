# Roadmap

This document records current project guidance and active work. Completed
milestone plans should live in git history, tests, and documentation rather than
continuing to grow this file.

## Active Standard-Library Work

Thread/synchronisation, IPv4 TCP/UDP, Raptor queues/tasks, and Nexus
message/request-reply/Telnet APIs are implemented. Supported behaviour lives in
[module contracts](docs/stdlib.md) and [API tutorials](docs/tutorials/README.md).
The retired expansion plan lives in Git history; the open work below replaces
its milestone checklists. Implementation does not imply that every platform or
stress-validation gate has passed.

### Kerberos and integration

- Design typed named ports, FIFO links, graph/node ownership and serial
  data-availability evaluation using the
  [pinned source inventory](docs/stdlib-source-inventory.md). Do not assume graphs
  are DAGs or substitute topological traversal for the source semantics. Define
  cycles, mutation during evaluation, removal/stale handles and ordering before
  implementing parallel evaluation. Stable identifiers are an intentional Nerd
  improvement, not a source-parity requirement. Public module names remain open.
- Keep graph storage independent of workers. Use our original Raptor-compatible
  work-stealing scheduler for a later execution adapter; ASIO is not a dependency
  or algorithm to copy. Define dependency readiness, completion/error/cancellation
  and mutation rules, then compare serial and parallel results.
- Add a runnable Kerberos tutorial: two generators feed a sum and sink checking
  0,2,…,18. Cover typed ports, fan-out, ownership, errors and shutdown; add the
  loader walkthrough after implementing its API.
- Add queue-pipeline, graph-pipeline and bounded task/network-service examples.
  Keep blocking network I/O outside CPU workers unless a readiness driver is
  explicitly supported. Test overload, backpressure, cancellation and shutdown
  across the combined layers without coupling the standalone APIs.

### Networking extensions

- Add IPv6 and DNS resolution to `std.network`, with resolver errors, ownership
  and blocking behaviour explicit. Extend Nexus URLs with DNS names and bracketed
  IPv6; these are additions beyond the inspected upstream contract.
- Define receive/both-direction shutdown, remaining socket options, address reuse,
  close-on-exec/inheritance and cancellation. Retain partial-I/O counts, portable
  error categories and native details. IPv6 unavailability must be an explicit
  capability result.
- Preserve the [Nexus source parity matrix](docs/nexus-parity.md), bounded buffering,
  reply-route lifetime and request/reply timeout recovery. Do not claim
  ZeroMQ/nanomsg interoperability. Further messaging patterns need a fresh design.

### Validation and delivery gates

- Complete the upstream component disposition matrix and build/run original
  reference suites where supported. Record unsupported checks explicitly. Resolve
  attribution/reuse terms before copying future source material; source access
  does not grant reuse rights. The Nerd scheduler is an original implementation.
- Validate relevant atomic width/alignment, publication, payload and callback
  lifetimes, thread-entry ABI, allocator/TLS behaviour and native layouts with
  focused probes. Do not label queues lock-free without progress and reclamation
  guarantees. Compiler prerequisites need separate regression-backed fixes.
- Retain exactly-once queue/task execution and destruction, wraparound, empty/full
  transitions, simultaneous producers/consumers, nested waits, idle wake-up,
  shutdown during submission, partial worker-start failure, predicate waits and
  resource-release coverage. Audit existing tests before adding duplicates.
- Compare serial, single-worker and multi-worker results and performance using
  reproducible workloads, with task granularity and allocations reported. This
  does not authorise a change to compiler concurrency defaults.
- Complete sustained resource-growth/soak validation and the remaining interactive
  platform observations. The [live Windows handoff](validation/windows/HANDOFF.md)
  records native follow-up; [archived automated evidence](https://github.com/Cthutu/nerd/blob/fac0feae825d73a478466410956c1ca7749a2a8a/validation/windows/results/HANDOFF.md)
  describes its recorded revision, not subsequent changes. Validate native
  Windows, Linux and WSL independently, including release, LLVM/C output,
  installation, formatter and LSP surfaces. macOS remains a separate port.
- Keep network/Nexus execution opt-in through `just test-network` and
  `just test-nexus`; keep long stress runs and benchmarks opt-in too. Use loopback,
  listener-assigned ports, explicit synchronisation, bounded deadlines and
  process-tree cleanup. A stress pass is not a memory-order proof.
- Deliver runnable tutorials for all three remade APIs. Examples must explain
  ownership, errors/backpressure and shutdown, use ordinary source build settings,
  and have meaningful output/exit checks in the appropriate native runner.
  Preserve cross-platform `just run-example` launches and scripted peers for
  interactive examples; no silent stubs or unexplained skips close these gates.

## Current State

- The compiler pipeline is `lexer -> AST -> sema -> HIR -> LLVM IR -> LLVM tools`.
  The CST provides source-preserving syntax for formatting and editor tooling.
- HIR is the checked middle layer. LLVM tools produce native binaries; the
  optional HIR-to-C backend emits standalone C for external compilation. The
  retired custom IR is not part of either path.
- The executable backend currently supports the configured 64-bit host target. Do
  not claim cross-target or aggregate FFI ABI support without explicit layout
  work and tests.
- Semantic analysis is a distinct front-end phase and owns name resolution,
  declaration classification, typing, diagnostics, and dependency ordering.
- The AST should remain compact and syntax-oriented. Semantic facts belong in
  arena-backed side tables keyed by indices.
- The CST owns source-preserving tooling such as formatting and editor-facing
  token structure.
- `just test` is the full-project regression gate. It runs language, error,
  HIR, LLVM, formatter, LSP, and command tests.
- Installed-compiler release smoke is covered by `build/test_install.py`.
- Editor wiring smoke is covered by `build/check_editor_integrations.py`.
- Modules load through the program-level front end. Standard modules live under
  `mods/` and should be treated like normal source modules.
- Module paths resolve to `path.n` first, then `path/mod.n` for folder modules.
- Folder modules implicitly include immediate sibling `.n` files in the same
  module scope.
- Public module exports are explicit with `pub`. Non-exported module members
  are private.
- `box[T]` is the built-in owning single-value heap allocation type. It
  supports nil, allocation, pointer adoption, borrowing as `^T`, implicit
  boolean conversion, member access, `.free()`, move semantics, and automatic
  scope-exit cleanup.
- `std.term` includes a terminal framebuffer with views, clipping, dirty-cell
  presentation, box drawing, 24-bit colours, and width-aware UTF-8 text output.
- The CLI supports program arguments plus executable, object, static-library,
  and shared-library output for the host toolchain.
- Source tests can be declared in imported modules, and `test { ... }` blocks
  provide test-only top-level declarations.

## Working Rules

- Do the work in small, reviewable increments.
- Prefer stable intermediate representations over ad hoc cross-stage logic.
- Keep new compiler data structures arena-based, table-based, and index-based.
  Prefer dense parallel tables and side tables over pointer-heavy object graphs.
- Avoid expanding `AstNode` unless there is a strong measured reason.
- Compute spans from token and node boundaries rather than storing large span
  payloads in AST nodes.
- Preserve the AST's RPN-friendly structure so it remains suitable for
  stack/VM-style passes, constant folding, and later compile-time execution.
- Keep parser responsibilities syntactic. Put symbol meaning, declaration
  meaning, type resolution, and language rules in semantic analysis.
- Use British spelling throughout code, comments, tests, documentation, and git
  commit messages where practical.
- In generated LLVM IR, emit Nerd-visible symbols with the `$` prefix and keep
  compiler/runtime helper names generated and internal unless explicitly
  exported.
- Allow forward references between declarations where the language already
  supports them. Track dependencies explicitly and order declarations from that
  information.
- Keep top-level constant bindings simple: lower them as globals/constants and
  let LLVM handle optimisation unless a specific compiler optimisation has been
  designed.
- When touching a compiler file, improve nearby comments and spelling where it
  is useful, but do not turn local fixes into unrelated rewrites.

## Horizontal Feature Policy

New language features should land across the toolchain together:

- compiler parsing and semantic analysis
- HIR and LLVM lowering
- formatter support
- LSP/editor support where relevant
- language tests
- error tests for invalid source
- format/LSP/command tests where the feature affects those surfaces
- user-facing docs when the language surface changes, including the manual and
  syntax/language-reference appendices where relevant

Prefer dense language regression tests that exercise related runtime cases in
one file. Use `std.io` for visible runtime output rather than adding many tiny
happy-path tests.

Do not move on from a language feature while one of the agreed compiler,
formatter, LSP, testing, or manual/documentation surfaces is knowingly behind.
If a documentation update is intentionally deferred, record that explicitly in
the roadmap before committing the implementation.

## Diagnostic Policy

- Keep the fuller diagnostic design policy in
  [docs/error-system.md](docs/error-system.md), and update that document when
  error-shaping rules change.
- Every language-compilation failure point must produce either:
  - a categorised compiler diagnostic with source spans, useful references, and
    appropriate notes or help text, or
  - an ICE when the failure represents a compiler bug or violated internal
    invariant rather than invalid user source.
- Use notes for explanatory context about why an error happened or which rule
  applies.
- Use help for actionable fix guidance.
- Treat OS, filesystem, shell, and toolchain failures as runtime errors, not
  user-source diagnostics or ICEs.
- Treat HIR generation and LLVM lowering invariant failures as ICEs.

## Test Artefact Policy

- Passing tests should remove generated intermediate files.
- Failing tests should keep generated intermediate files so failures can be
  analysed locally.
- When a feature changes CLI-visible compile-time behaviour, add at least one
  regression that exercises the real `nerd` command path rather than only
  internal compiler entry points.

## Parser And Sema Direction

- Replace scattered token classification logic with small shared tables or
  macros when doing so reduces local complexity.
- Use compact AST/sema emit helpers where they remove repetitive parser or
  semantic boilerplate.
- Keep semantic analysis VM-friendly and table-driven:
  - process AST/CST data through linear or index-based passes where practical
  - store semantic data in side tables keyed by AST node, symbol, declaration,
    local, and scope indices
  - keep dependency data explicit
- Standardise parser helper contracts:
  - each helper should clearly either require the first token to have already
    been consumed, or consume it itself
  - comments and function names should make the contract clear where ambiguity
    is likely
- Standardise function comments as files are edited:
  - one short purpose comment when it helps
  - separator comments before major internal functions where the file already
    uses that style

## Active Work

Completed milestone plans belong in git history, tests, and subsystem
documentation. The active list below contains only work that is ready to guide
the next implementation slices.

Keep parsing traits such as `Parse` at standard-library level rather than
making them language-known traits. Expand the standard library only where it
validates the module/export model or provides a clear user-facing capability.
Track the remaining `std.image` decisions and extensions in
[TODO.md](TODO.md).

General generic functions, types, traits and constraints remain supported. The
parametrised-module replacement proposal is on hold; integrated optionals and
results are implemented and documented in the manual and specs.

### Editor Intelligence

- Extend formatter and LSP behaviour alongside new syntax and semantics.
- Improve semantic completion, signature help, hover, definitions, references,
  rename, and code actions using the existing front-end facts.
- Treat VS Code and Neovim/LazyVim as paired editor surfaces when file
  detection, highlighting, LSP wiring, formatting, or packaging changes.
- Consider Tree-sitter only after the semantic LSP experience is strong enough
  to justify a second syntax implementation. Its eventual scope may include
  highlighting, folding, indentation, incremental selection, and structural
  editing.

### Compiler Maintenance

- Keep formatter layout rules syntax/trivia-based. Extract shared spacing,
  comment, blank-line and alignment policies in small changes backed by partial
  source and idempotence fixtures.
- Keep AST/CST classification and recovery tests aligned. Parser unification
  needs a separate design; shared syntax helpers are the current direction.
- Harden LSP declaration/binding/type-fact accessors against incomplete tables
  and broken imports; reduce crashes to incremental-edit transcripts first.
- Keep target layout and runtime helper declarations consistent with the C
  runtime. Cross-target and aggregate FFI support need explicit ABI tests.
- Keep compiler scheduling at one job by default. Any adaptive-default change
  needs the [performance adoption gates](docs/compiler-profiling.md#scheduler-adoption-policy).

### Testing Follow-up

- Add structured expected diagnostics to negative command checks where only
  failure is currently asserted; retain separate CLI contracts.
- Consolidate compatible runtime scenarios and investigate sharing LLVM
  baselines with C differential tests. Preserve IR, ABI, platform and cleanup
  coverage, and measure compiler/toolchain launches before adopting changes.
- Add non-fail-fast source-test assertions only if real test suites demonstrate
  that continuing after a failed assertion would be useful.

## Deferred Work

These items are worth keeping visible, but they should not be treated as
near-term tasks without a fresh design pass.

- In-place AST compaction for constant folding.
- Calling-convention annotations for FFI, including platform-specific behaviour.
- Interpolation formatting specifiers such as `{expr; format}`.
- Operator traits for arithmetic, bitwise and assignment operators. Equality
  and ordering already use `core.Eq` and `core.Order`.
- A synthetic diagnostic harness for hard-limit error categories that are
  impractical to trigger through normal source files.
