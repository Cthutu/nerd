# Standard-library integration — 2026-10-01

Review branch: `std-library`, targeting `main` through a draft PR. This combines
the first capability, runtime, threading and IPv4 socket slices, with compiler
repairs found during their validation. It does not implement the Raptor scheduler,
Kerberos graphs or Nexus protocols, nor declare M0/M1/M5 complete.

## Changes and review map

- [Workstreams](../../../../docs/stdlib-workstreams.md) preserves branch ownership
  and the integration sequence; published commits are retained by merges.
- [Thread/sync contracts](../../../../docs/stdlib-thread-sync.md) describe callback
  context lifetime, join ownership and predicate-based waits.
- [Network contracts](../../../../docs/std-network-foundation.md) describe socket
  ownership, partial I/O, EOF, nonblocking operations and datagram truncation.
- `thread-pipeline`, `network-echo` and `network-datagram` are bounded runnable
  examples included in the execution tests. Examples use native resources rather
  than sleeps or probed-then-released ports.
- The C-output constant repair preserves per-use evaluation and isolates imported
  binders. Runtime worker cleanup and debug allocation bookkeeping are integrated.
- Both `just test` and `just test-release` run the new library contracts; the OS
  completion fixture includes both `socket` and `thread`.

## Validation scope

Fresh native Windows Clang builds and the combined debug/release recipes are the
integration gate. Exact final results are recorded below when complete. These
checks use checkout modules and compilers. The common recipe includes isolated
clean and installation smoke tests; it does not run the global compiler/editor
installation performed by `just do`. Desktop/editor observations, benchmarks and
global installation are outside this review pass.

Arch WSL snapshot `2ceff08d55d2f2f73d705b903a9f89521af8bba5` passed a fresh Clang
22.1.6 native compiler build, runtime C worker stress (debug/release, three runs
each), thread/sync C output at O0/O2 including the temporary-arena alias regression,
and socket layouts/contracts/examples at O0/O2. See [the WSL log](wsl-arch.log).
This snapshot predates the separate public-variable formatting and LLVM-global
repairs. `nerd doctor` reports missing `opt`, `llc`, `ld.lld` and `llvm-ar` in that
WSL environment. No tool installation was performed. This is generated-C WSL
evidence, not native LLVM or standalone Linux adoption.

## Linux pickup

Use the remote name configured on the Linux PC (`git remote -v`), fetch and check
out `std-library`, then fast-forward it from that remote. Review any existing
local edits before switching or pulling. No integration into `main` is required
to review this draft.

Ensure recent Clang with C23 `#embed`, `opt`, `llc`, LLD, LLVM archive/debug tools,
Python, `uv` and `just` are available. The initial pthread and socket bindings are
for **Linux x86-64 glibc**; their native header assertions reject incompatible
layouts rather than treating them as tested platforms.

```sh
just test
just test-release
just run-example thread-pipeline
just run-example network-echo
just run-example network-datagram
```

Those commands build this checkout's compiler and use its modules. The two test
recipes include capabilities, runtime worker stress (debug recipe), thread/sync,
networking and the imported-arena regression. The debug recipe additionally runs
the full C-output differential suite. Run `just do` when ready to exercise the
clean/build/test/global-install workflow on that PC; it intentionally updates the
installed compiler/modules and editor support. Native Linux/full WSL `just do`
remain adoption gates and are not implied by the Windows results.

Source access was resolved during integration. The
[inventory](../../../../docs/stdlib-source-inventory.md) pins Raptor/Kerberos
revisions, tests and provenance; upstream execution and reuse decisions remain.
No work-stealing implementation was found in the available Raptor heads, so
confirm the intended revision or explicitly choose an algorithm. Continue the
[milestone plan](../../../../docs/stdlib-expansion-plan.md)
after reviewing this foundation: queues/scheduler, graph semantics, broader socket
support and Nexus framing/protocols remain separate work.
