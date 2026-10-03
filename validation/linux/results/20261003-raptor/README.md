# Original Raptor scheduler — Linux validation

Branch: `std-library`, based on integration `19bd1165`; compiler prerequisites
are committed as `3a7d6555`. This change introduces
`std.queue.WorkDeque` and `std.raptor`, an original bounded work-stealing scheduler
with Raptor queue/task semantics. It does not copy ASIO or the upstream scheduler.
See [contracts](../../../../docs/stdlib-raptor.md) and
[tutorial](../../../../docs/tutorials/raptor.md).

Logs: [debug gate](just-test.log), [release gate](just-test-release.log),
[frontend ASan](frontend-asan.log), [tutorial run](tutorial.log).
[Source hashes](source-sha256.txt) bind this evidence to the code and fixtures.

## Native checks

- Full debug and release gates: 1,155 fixtures passed, zero failed, nine existing
  Linux skips each. Debug includes 292 LLVM/C differential fixtures at C O0/O2,
  zero platform skips, plus all auxiliary contract gates.
- Raptor runner: native LLVM and generated C, debug/release, with 16 repeated
  contract executions per configuration and deadlines that kill the process
  tree. Each execution includes 32 competing final-item owner/thief removals,
  deque wrap/full/empty, forced stealing, single-worker nested waits,
  self/ancestor/same-serial rejection, serial FIFO/non-overlap, typed/void results,
  admission/retry, mixed accepted/idle aggregation, four external producers and
  draining close. Linux injects failure into the third pthread creation and
  counts native creates/joins, then checks successful reinitialisation.
- Full scheduled frontend AddressSanitizer suite passed using an isolated
  instrumented compiler.
- `just run-example task-parallel` produced the exact expected total, 204.

## Compiler prerequisites repaired

`tests/language/236-generic-task-shapes.t` covers generic inference through
function, optional and result fields; runtime callback calls under an expected
optional result; pointer payload injection in generic returns; address publication
inside constructors; and stored callbacks shadowing same-named functions.
The full differential suite includes it automatically.

The serial scheduler check initially failed intermittently in release LLVM.
Generated IR showed initialisation of the shared sequence local inside the
submission loop, because address tracking ignored constructor arguments. Tracking
addresses in aggregates moves materialisation to the declaration. This was a
compiler defect, not a reason to weaken the serial queue contract.

## Limits and follow-up

Native Windows validation is pending for this revision, including compiler
repairs and all Raptor configurations. This is a mutex-based correctness
reference, with scheduler bookkeeping serialised by a shared mutex; no lock-free,
throughput, latency, fairness or starvation-freedom claim is made. Native resource
failure injection is Linux-only. The complete upstream suites were not executed.
Async aggregate tasks, cancellation, timers, graph execution and Nexus messaging
are outside this first scheduler slice. Tutorials for Kerberos and Nexus are
required when those APIs are implemented.

While exploring the implementation, imported recursive records caused a compiler
stack overflow, and binding a `?void` payload emitted an invalid LLVM `alloca void`.
These remain compiler follow-ups. Internal scheduler links use explicit opaque
pointers, and the documented void completion check uses `get() != nil`; neither
path requires those unsupported forms. A speculative recursive import memoisation
change was removed rather than shipping an unvalidated type identity change.
