# Standard-library capability audit

Date: 2026-10-01. Baseline: `f5d551da`. This is the M0 language/runtime
slice, not completion of the Raptor or Kerberos source inventory.

## Executable baseline

Run from this checkout with a freshly built compiler:

```powershell
python build/build.py nerd --skip-mod-sync
python validation/stdlib/check_capabilities.py --compiler _bin/nerd-debug.exe
```

On Linux use `_bin/nerd-debug`. The harness selects this checkout's `mods`
through `NERD_LIB_PATH`, checks the source, builds ordinary and optimised LLVM
executables, and checks their output and exit status. Generated binaries live
in a temporary directory. Each process has a 60-second timeout; these probes
create no threads, children or sockets. No installed modules are changed.

Native Windows x64 result: both modes passed with a freshly built Clang debug
compiler. Clang reports 23.1.0; direct `opt`/`llc` tooling performs Nerd code
generation. Linux, WSL, release compiler and C generation are not validated by
this result. The source is formatted and passes `nerd check`.

| Capability | Executable evidence | Limit |
| --- | --- | --- |
| Pointer-sized atomic state | `atomic[usize]` strong CAS success and observed-value failure, fetch-add and acquire load | Initial literal uses explicit `7.as(usize)`; bare `7` is rejected by current type checking |
| Pointer publication | `atomic[^i32]` release CAS and acquire load preserve the pointer | Single-thread operation only; does not prove a queue algorithm or reclamation scheme |
| Atomic layout | Pointer-sized atomic storage size and address divisibility by `usize.size` | Checks this stack allocation; no cache-line alignment guarantee, packed structs or foreign ABI assertion |
| Explicit callback context | A `fn (^void)` field calls a callback with a caller-owned `i32` pointer | Context remains alive for synchronous call; thread ABI/join lifetime belongs to M1 |
| Generic owning payload | Copying `Box[[..]i32]` aliases the same dynamic-array payload; mutation through the copy reaches the original | There is no transfer/invalidation enforced by this operation; probe frees once and never touches either alias afterwards |
| Graph storage building blocks | Dynamic array of generic nodes, nested adjacency arrays, explicit slot/generation identifiers, manual stale-ID rejection | No Kerberos algorithm, automatic generation handling, deep destruction, cycle policy or evaluation semantics implied |

The tests intentionally do not race data or manufacture a use-after-free to
prove ownership limitations. A successful compile is insufficient evidence of
safe ownership transfer. Queue APIs should accept explicit pointers/handles or
use documented consuming operations that clear the source and return ownership
on rejection. That convention still requires review because callers can copy a
handle. Graph storage growth may invalidate pointers; expose stable IDs and
resolve them after growth instead of retaining pointers into dynamic arrays.

Existing boundary evidence: `tests/commands/245-check-atomic-by-value.cmd`
rejects atomic parameters by value, and `250-check-atomic-ffi-type.cmd` rejects
atomic values in FFI signatures. `mods/std/atomics.n` provides strong/weak CAS,
load/store/exchange and integer read-modify-write operations. Its comment says
the backend guarantees *at least* the requested ordering. This audit does not
promise weaker lowering, lock-free progress, double-width CAS or ABA protection.

## Runtime prerequisite before allocating worker callbacks

At the audited baseline, `data/nrt.c` has thread-local temporary arena and
string-builder state (`g_temp_arena`, `g_string_builder_*`). Lazy initialisation
allows per-thread use, but there is no separate worker shutdown hook:
`nrt_core_done` frees the caller's TLS resources and then prints process-wide
leaks. Calling it from every worker is not a sound lifecycle design.

Debug allocation accounting uses process-global `g_nrt_heap_head`,
`g_nrt_arena_head` and allocation indices. Heap link/unlink and arena
track/find/update/release paths lack synchronisation. Even interpolation enters
heap tracking before `nrt_mem_leak` excludes its reusable buffer. A worker that
only formats a string is therefore not exempt. The page-size helper queries
the OS directly and has no shared lazy cache in the inspected code.

Required separate runtime slice:

1. Add a per-thread cleanup hook for temporary arena and string-builder storage,
   with no process-wide leak reporting; invoke after the user callback and before
   a successful join can return. Keep process finalisation after all workers join.
2. Protect all shared debug allocation lists, indices and traversals, including
   cross-thread frees and leak suppression. Document that raw debug-list views
   require quiescence; locking a getter does not protect traversal afterwards.
3. Test concurrent allocation/reallocation/free and interpolation, repeated
   thread start/join, cross-thread ownership handoff, worker TLS cleanup and
   process leak reporting with active allocations deliberately controlled.
4. Do not treat release-only passes as sufficient: disabling debug bookkeeping
   would hide the shared-state defect. TLS cleanup still matters in release.

This report records the baseline. The runtime/thread repairs are now integrated
into `std-library`; see the latest [handoff](../validation/windows/results/HANDOFF.md)
for combined regression evidence replacing these baseline blockers.
Thread entry ABI, mutex/condition-variable native layout and platform lifecycle
are owned by the M1 track; socket layouts and networking ABI by the M5 track.

## Source inventory boundary and next slices

Raptor and Kerberos were inaccessible during this baseline audit. Access has now
been resolved through the existing owner login; the separate
[source inventory](stdlib-source-inventory.md) records revisions, actual APIs,
test mappings and provenance. No queue or graph implementation is copied in
this tranche, and no upstream execution results are claimed.

Nexus source is available in the review checkout of `cthutu/dev` pinned by the
[expansion plan](stdlib-expansion-plan.md). Its source/test inventory belongs to
the independent networking track; this audit adds no parity claims.

M0 must finish the component disposition and reuse decisions, upstream execution,
and algorithm-specific memory-order/reclamation and graph mutation contracts.
After the runtime/M1 prerequisite, queue work can start with the simplest
inventoried queue and explicit ownership tests. Graph storage/serial evaluation
can proceed independently once Kerberos semantics are known. Native Linux and
WSL runs of this harness remain explicit adoption gates.
