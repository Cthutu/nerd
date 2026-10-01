# Parallel standard-library workstreams

Started 2026-10-01 from `f5d551da` on `std-library`. Each stream has its own
Git worktree and branch. `main` is unchanged; these are review branches for
later integration, not completed M0/M1/M5 milestones.

| Branch | Owner/task | First deliverable | Dependency |
| --- | --- | --- | --- |
| `std-library-audit` | Capability audit agent | Executable atomic/callback/ownership/graph-storage probes and evidence | Planning base |
| `std-runtime-threads` | Coordinator | Runtime worker cleanup, concurrent diagnostic bookkeeping, current-thread arena accessor | Planning base |
| `std-thread-sync` | Threading agent | Owned start/join, mutex/conditions, producer/consumer example and native tests | Runtime branch |
| `std-network` | Networking agent | IPv4 TCP/UDP socket foundation, echo/datagram examples and native contract tests | Planning base; no scheduler dependency |
| `std-cgen-imported-constants` | Capability audit agent, follow-up | Correct imported constant evaluation in compatibility C output | Planning base; discovered by worker checks |
| `std-library` | Coordinator | Scope, cross-stream decisions, review and integration records | Collects reviewed branches later |

The first three agent tasks are deliberately independent. Raptor queue algorithms
and Kerberos semantics cannot be ported faithfully until source access is resolved.
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
  Compatibility C output currently caches imported constants incorrectly; use
  the explicit function until the compiler fix is integrated.
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
- Imported-constant compiler repair: validation in progress. Review reproduced
  a local-variable collision inside imported constant binders, which must also
  be fixed before the compiler branch is ready. The two completed library
  slices use explicit function calls and do not depend on this unfinished fix.

See [the milestone plan](stdlib-expansion-plan.md) for feature and example exit gates.
