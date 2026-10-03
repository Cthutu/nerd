# Parallel standard-library workstreams

See the [review report](stdlib-review-report.md) for delivered features, compiler
repairs, validation limits and prioritised language/library recommendations.

Started 2026-10-01 from `f5d551da` on `std-library`. Each stream has its own
Git worktree and branch. The first slices are now merged into `std-library` for
a draft PR. `main` is unchanged; these are not completed M0/M1/M5 milestones.

| Branch | Owner/task | First deliverable | Dependency |
| --- | --- | --- | --- |
| `std-library-audit` | Capability audit agent | Executable atomic/callback/ownership/graph-storage probes and evidence | Planning base |
| `std-runtime-threads` | Coordinator | Runtime worker cleanup, concurrent diagnostic bookkeeping, current-thread arena accessor | Planning base |
| `std-thread-sync` | Threading agent | Owned start/join, mutex/conditions, producer/consumer example and native tests | Runtime branch |
| `std-network` | Networking agent | IPv4 TCP/UDP socket foundation, echo/datagram examples and native contract tests | Planning base; no scheduler dependency |
| `std-cgen-imported-constants` | Capability audit agent, follow-up | Correct imported constant evaluation in compatibility C output | Planning base; discovered by worker checks |
| `std-llvm-imported-globals` | Capability audit agent, follow-up | Correct direct imported mutable-global access through LLVM | C-output repair branch |
| `std-format-public-globals` | Threading agent, follow-up | Preserve standalone public variable visibility during formatting | C-output repair branch |
| `std-llvm-lowering-regressions` | Threading agent with C-output repair assistance | Packed-field pointer assignment and optional-void side effects exposed by integration tests | Integrated compiler repairs |
| `std-library` | Coordinator | Integrated changes, common test recipes, draft PR and Linux handoff | Collects reviewed branches |

The first three agent tasks are deliberately independent. Source access was later
resolved through the existing owner login; the [inventory](stdlib-source-inventory.md)
records actual algorithms, test mappings and provenance. No work-stealing
implementation was found in the available Raptor branch heads.
Nexus messaging will follow the socket foundation rather than being developed
against an unstable placeholder API.

## Shared decisions

- Keep generated binaries and compiler objects inside each worktree. Always use
  that worktree's `mods` explicitly in `NERD_LIB_PATH`; do not test against installed
  modules or write into another agent's checkout.
- New native bindings live in independent `os.thread` and `os.socket` folder
  modules. The existing `os.windows` and `os.linux` roots prevent arbitrary external
  descent into child modules; extending those shared roots is avoided in this batch.
- Public owned handles are non-copyable by documented contract; current Nerd
  capabilities do not enforce all resource moves. Callback context must remain
  alive and stable until join. Atomic support alone does not prove a queue correct.
- Worker wrappers pair `nrt_thread_init`/`nrt_thread_done`. Both `temp_arena`
  (evaluated at each use) and `current_temp_arena()` borrow caller TLS. Results
  transferred between threads need storage that survives worker cleanup.
  The integrated C-output repair restores per-use imported constants; both
  backends now run the worker alias regression in the common test recipe.
- Diagnostic allocation locks protect bookkeeping, not concurrent access to an
  application's allocation or arena. Join workers before raw leak-list inspection.
- Socket ownership, native handle width, error capture, partial I/O and EOF/datagram
  semantics must be tested before building Nexus on top. IPv6, DNS, readiness and
  non-blocking connect completion remain later network slices.
- No global compiler/editor installs or benchmark comparisons during parallel
  work. Record native Windows, individual WSL distributions and Linux separately.

## Review and integration sequence

1. Each owner commits and pushes a bounded tested slice, with commands, platform
   evidence, limitations and unresolved compiler/runtime blockers in its handoff.
2. Review source, tests, public contracts and examples. Distinguish focused checks
   from a complete repository gate; do not claim whole milestones from one slice.
3. Integrate the runtime prerequisite before the thread branch. The thread branch
   is stacked on that prerequisite to make ordinary checkout/build work. Audit and
   network work can be reviewed independently. Include the imported-constant
   compiler fix before claiming backend parity for worker temporary storage.
4. Rebase remaining work against the latest `std-library` only when its owner has
   finished local edits; coordinate any published-history rewrite. Resolve shared
   documentation and test-recipe wiring deliberately. Preserve independent commits.
   Both OS modules extend the completion fixture `117-os-module-completion.lsp`;
   retain both `socket` and `thread` entries when resolving that overlap.
5. On the combined branch, run the capability probes, runtime worker test, threading
   and networking harnesses, examples and full debug/release regression gates.
   Validate clean build/install workflows on Windows, Linux and WSL. Concurrent
   per-branch passes do not substitute for this combined validation.
6. Merge reviewed work into `main` only after integration gates pass. Delete
   temporary branches/worktrees after merge, preserving any unrelated worktrees.

## Initial evidence

- Audit `bc35692d`: native Windows debug/optimised LLVM probes passed, with explicit
  ownership limitations and blocked Raptor/Kerberos source inventories.
- Runtime `d7bfbf40`: native C worker stress passed in debug/release, including
  cross-thread heap freeing and repeated TLS cleanup/reinitialisation. Fresh
  compiler build, 11 arena fixture passes (1 platform skip), 2 memory fixture
  passes, and current-thread arena LLVM/C-output checks passed. Independent code
  review found no blocking defect under the documented ownership contracts.
- Threading `3920d4b3`: fresh Windows debug/release builds, LLVM debug/release,
  C-output O0/O2 and affected LSP completion checks passed. WSL Arch passed a
  fresh compiler build, semantic checks and C-output O0/O2 execution; Ubuntu and
  Arch passed native pthread ABI assertions. Direct LLVM WSL execution remains
  blocked by missing toolchain components. See `docs/stdlib-thread-sync.md` and
  `validation/windows/results/20261001-thread-sync/README.md` on that branch.
- Networking `b3dc70c8`: Windows LLVM/C-output O0/O2 contract checks, fresh release
  compiler checks and both `just run-example` commands passed. Full fixture run
  produced 1175 passes, one stale OS-module completion expectation and 15 skips;
  corrected completion fixture and all remaining `just test` checks then passed,
  including 295 C-output differential fixtures at both optimisation levels.
  Arch WSL C-output O0/O2 passed; direct LLVM execution lacks `opt`/`llc` there.
  See `docs/std-network-foundation.md` and
  `validation/windows/results/20261001-std-network/` on that branch.
- Imported-constant repair `b0d46a57`: integrated, including isolated imported
  binder locals. Its independent Windows gate passed 296 differential fixtures
  at C O0/O2; the combined thread alias regression also passes LLVM/C output.
- Public-variable formatter repair `8eb7b5e3`: integrated; 212 formatting and
  three related command fixtures passed independently, including public typed,
  inferred, defaulted and guarded variables.
- Imported mutable-global repair `53fba4cf`: integrated; LLVM/C O0/O2 regressions
  cover shared storage, mutation, pointers, aggregates, atomics and duplicate
  module names while preserving the root public ABI. Failed LLVM lowering now
  reports an error instead of silently producing an incomplete executable.
- Lowering follow-up `3e32a7e0`: integrated; fixes packed-field pointer writes and
  void success constructors in LLVM/C output. Strengthened completion and
  side-effect regressions pass in both backends, including final Arch WSL C output.

## Combined review branch

All original workstream commits are preserved by merges without rewriting their
published histories. The shared OS completion fixture retains both new modules.
The standard test recipes execute capability, thread/sync and socket contracts,
including all three runnable examples. Full combined results and Linux pickup
commands are recorded in the latest
[Windows return handoff](../validation/windows/results/HANDOFF.md).

See [the milestone plan](stdlib-expansion-plan.md) for feature and example exit gates.

## Subsequent scheduler work — 3 October 2026

The user selected an original work-stealing implementation around Raptor's API
semantics, without an ASIO copy. `std.raptor` and `std.queue` provide the first
bounded correctness reference; see [contracts](stdlib-raptor.md). Tutorials for
all three remade APIs are required; [status](tutorials/README.md). The task branches
above are historical and have since been removed after ancestry review.

The Raptor API revision places generic `async` on `Queue`, returns owned
`Task[T]` handles, and makes scheduler `init` return `?void`. Typed callbacks
need no erased-pointer casts. Handles use explicit `take`/`done`; method signature
help hides implicit receivers. The tutorial and native contracts cover this API.
