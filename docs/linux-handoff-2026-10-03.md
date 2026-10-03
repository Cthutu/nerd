# Standard-library pickup — 3 October 2026

Continue on **`std-library`**. The test audit and first consolidation are now
integrated, including enum optional/result materialisation fix `58818307`,
Windows evidence through `87e4567a`, Linux repairs `209e8857` and Linux evidence
`f0983f96`. Integration preserves the published task histories; `main` is
unchanged. Merged task branches were subsequently removed locally and remotely
after ancestry review; detached review worktrees were retained.

## Branch review

Git ancestry confirms the earlier capability audit, runtime threads,
thread/sync, networking, imported-constant C fix, public-global formatter fix,
imported-global LLVM fix and lowering regressions were already integrated.
The diagnostics/LSP branch is also integrated. `test-suite-consolidation`
contains `test-suite-audit`, so its merge incorporates both remaining histories.
Linux uses upstream **`agent/std-library`**; remote names differ by host.

Consolidation removes 33 exact command/language runtime duplicates and batches
14 additional `on` scenarios into one labelled language fixture. Dedicated
CLI/cleanup contracts, existing HIR/LLVM assertions and diagnostic cases are
preserved. See the [report](test-suite-consolidation.md) and
[migration map](test-suite-consolidation-map.json).

## Linux validation and repairs

Fresh Clang 22.1.8 compilers at `209e8857` pass full **`just test` and
`just test-release`: 1,153 fixtures, zero failures, nine existing Linux skips
each**. All debug auxiliary gates pass, including unchanged 6,000-term checks,
291 LLVM/C differential fixtures at C O0/O2 with zero skips, and Linux PTY
Dungeon interaction. The full frontend AddressSanitizer suite, explicit C
capability/layout probes and three `just run-example` commands also pass.
See [complete evidence and limits](../validation/linux/results/20261003-test-suite-consolidation/README.md).

The first full run exposed a compiler use-after-free: function-body inference
imported overload proxies, growing the declaration array while the callee type
output still pointed into its former allocation. The repair keeps inference's
output on the stack and reacquires the declaration by index. Existing command
223 and its differential entry remain, plus a new frontend sanitizer case.
LSP fixture URI expansion also needed a single-pass replacement to avoid
doubling this worktree's directory suffix. Original failures and the ASan
diagnosis are retained alongside passing results.

Changed-work timings on Linux at `87e4567a`: old 47 command fixtures **1.485 s**
median versus **0.0619 s** for the replacement batch, using the same compiler,
one excluded warmup and three sequential samples. This is a measurement of
replaced work, not a whole-suite speedup. Migration maps and omission/wrong-result
mutations were verified again after the repair.

## Next work

1. Continue the audit with structured expected diagnostics for the negative
   `check` cases that currently assert only failure. Preserve CLI-specific
   contracts and separate lexer overflow from destination-range errors.
2. Expand compatible feature batching, then investigate sharing same-run LLVM
   golden/differential baselines. Measure compiler/toolchain launches and wall
   time while retaining backend, release, IR, ABI and platform checks.
3. Native Windows must validate the new inference lifetime repair. Earlier
   Windows results cover `caaef691` and do not establish success for `209e8857`.
4. Full `just do` remains unrun here; it cleans local artifacts and installs the
   global compiler/modules/editor integration. The temporary-install gate passed.
5. The scheduler direction is resolved: our own Nerd work stealing around the
   Raptor API, without an ASIO copy. The first `std.queue`/`std.raptor` slice is
   implemented; see [contracts](stdlib-raptor.md),
   [tutorial](tutorials/raptor.md) and
   [Linux evidence](../validation/linux/results/20261003-raptor/README.md).
6. Next: native Windows Raptor/compiler checks, async aggregation design and
   performance measurements, then Kerberos graph dataflow (M4) and Nexus message
   framing/readiness (M6/M7). Complete runnable tutorials for all three remade
   APIs; [tutorial requirements](tutorials/README.md). Compiler follow-ups include
   imported recursive record type identity and binding optional void payloads.

The [October 2 handoff](linux-handoff-2026-10-02.md) retains the broader design
decisions. Nerd remains a C replacement with explicit allocation, aliasing,
cleanup and synchronization responsibilities. Fix compiler bugs as discovered,
retain regression coverage, and commit/push verified work.

## Raptor slice added after consolidation

The original scheduler has fixed workers, bounded local LIFO/victim FIFO deques,
serial/concurrent queues, typed/void task results, cooperative nested waits,
capacity rejection, one-shot admission and draining close. Mutexes establish the
first correctness reference. Its initial API required stable caller task storage;
the returned-handle API revision below replaces that requirement. Borrowed callback
arguments still stay alive until completion; close requires external
producers/waiters to have stopped.

Compiler prerequisites repair generic function/optional/result-field inference,
function-field calls under optional context, generic optional return wrapping,
LLVM constructor address tracking and callback/declaration name collisions.
The full Linux gates pass 1,155 fixtures each; debug also passes 292 differential
fixtures at O0/O2, all auxiliaries and frontend ASan. The bounded Raptor runners
exercise both backends in debug/release, including Linux partial-start injection.
Windows results from earlier revisions do not establish success for this slice.

## Raptor API and method signature revision

`Queue.async[A, T](callback, argument)` now infers typed arguments/results and
returns `?Task[T]`, preserving upstream queue-owned submission. `Scheduler.init`
returns `?void` for propagation. User callbacks take pointers such as `^i32` or
plain values; no erased-pointer cast is needed. Each accepted invocation has
stable allocated storage, so the returned handle may move while work runs.
`take()` transfers a handle; `done()` waits and releases its storage. All readers
must finish before cleanup. Payloads are shallow and require caller-managed
lifetimes; owning boxes/records containing them are not supported as payloads.
The revised tutorial registers cleanup before submission to cover early failure.

Compiler inference now binds generic callback signatures before checking them.
A sanitizer probe also exposed erased generic function values selecting the
first specialization with a matching signature; LLVM and C generation now honor
the explicit specialization, with a pointer/integer/floating-point regression.
The formatter supports `defer assert` and `undo assert`. LSP signature help hides
implicit receivers and highlights visible arguments correctly for local,
imported, generic and incomplete method calls, while preserving builtin help.

Full Linux debug/release gates pass 1,160 fixtures each, with zero failures and
nine existing skips. Debug also passes 294 C differential fixtures at O0/O2.
Frontend ASan and native task ASan/UBSan/leak checks pass. Evidence is recorded in
[the API revision results](../validation/linux/results/20261003-raptor-api/README.md).
Native Windows validation remains pending for this revision. Nexus and Kerberos
tutorials remain tied to the message/graph APIs still to be implemented.
