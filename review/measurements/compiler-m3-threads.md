# M3: portable worker primitives and render-input review

2026-09-20, Linux x86-64. Baseline `43c38e8d` plus this worker-primitives slice.
The compiler remains serial; this change does not install a scheduler or add
jobs to the build command.

## Lifecycle contract

`src/core/thread.c` provides joinable threads and condition variables. POSIX
uses pthreads. Windows uses `_beginthreadex` for CRT-compatible thread lifecycle,
wait/close handles, and native condition variables with the existing critical
section mutex wrapper. No new runtime tool dependencies are introduced.

The coordinator zero-initializes each Thread and keeps it and callback arguments
at stable addresses through join. A started Thread cannot be copied or moved.
Only one coordinator manipulates start/join state. Creation failure leaves an
empty slot; duplicate starts fail. Successful join closes/releases the native
resource and resets the slot. Joining empty or already joined slots succeeds.
Failed join retains ownership; callers must not destroy inputs/results or
synchronization objects if workers remain live. Self-join is rejected; workers
must not manipulate their own Thread.

Condition waits require the associated mutex held and a predicate loop. Stop
requests are predicate changes followed by wakeups; shutdown joins workers before
freeing their storage. No detached workers or asynchronous cancellation are
provided. Condition initialization and the new checked mutex initializer report native
failure. Existing callers retain the original mutex initializer. A pool must
unwind whichever synchronization objects and workers have started if any later
startup step fails.

## Tests

`build/test_threads.py` runs three repetitions of debug and release harnesses,
both normally and with native thread creation replaced by a test wrapper that
fails after two successful starts. That wrapper includes the production thread
implementation; it adds no failure hooks to the shipped implementation.

The lifecycle harness exercises 50 start/join/reuse cycles, duplicate-start
rejection, empty/repeated joins, one and four waiting workers over 100 rounds,
extra notifications, stop requests with work published, and partial-start
cleanup. Work/results are guarded by predicates or published through join. Test
arguments and Thread slots have stable addresses until all workers are joined.
The injected native failure verifies the actual creation-error/reset path,
including cleanup of workers already started.

The allocator harness now uses the production Thread API instead of direct OS
thread calls. Its cross-thread alloc/realloc/free, debug allocation bookkeeping and
thread-local activity checks therefore exercise the new join/publication path.
Both harnesses also run with ThreadSanitizer and AddressSanitizer. This validates
core primitives on Linux, not concurrent compiler rendering or native Windows/
macOS behavior. Native validation remains pending.

## Initial LLVM shared-input review

The first source pass found these boundaries:

| Area | Current evidence | Requirement before scheduling |
| --- | --- | --- |
| Module registry, HIR, lexer and semantic side tables | `llvm_render_hir` accepts const views; imported declarations/functions traverse `sema->program->modules` | Finish loading/lowering and freeze registry/index storage before dispatch; retain all inputs until every job joins |
| Mutable type data | `llvm_prepare_render_sema` copies types and three parameter arrays; builtin/pointer additions and cleanup operate on those copies | Keep copies private to each render; finish reviewing all helper calls and aliases |
| LLVM contexts and debug metadata | Builders, local slots and metadata arrays are local render/function contexts; file-static data in llvm.c is constant | Keep one result arena/context per task; do not share builders |
| Symbol lookup | `lex_symbol` reads the interner; `sema_type_name` uses the supplied arena or local nested arenas | No interner mutation while rendering; review reachable helpers before enabling workers |
| Name-conflict index | `map_find` reads the index; backend installs an emission-scoped map pointer on ProgramInfo, then clears/frees it | Dispatch only after construction; join all jobs, including failure cleanup, before clearing/freeing the map or returning from emission |
| Source-line index | Debug lookup reads the program-owned index | No rebuilding or destruction while workers borrow it |
| Diagnostics and profiling | Task-owned captures/results exist; no direct temp_arena reference found in llvm.c | Keep rendering/output replay and human/legacy timing aggregation on the coordinator; fatal internal errors still exit the process |
| Allocation | Shared accounting is locked; result/scratch arenas belong to tasks | Caller-owned blocks still need exclusive ownership; measure lock contention with actual render workers |

This is a source review, not a thread-safety proof. The remaining gate is a
complete reachable-helper/alias review and a concurrent-render harness exercising
real multi-module inputs, comparing output against serial renders under race
checking where supported. A bounded queue, worker count selection, cancellation
policy and deterministic result merge belong to M4 after that gate. M3 remains
in progress.

No compilation speedup or scaling is claimed. Normal compiler paths do not call
these primitives yet, so this slice uses lifecycle/correctness checks rather
than another serial timing comparison.

Linux validation: `just test` passed with 1,119 compiler tests and nine skips,
the worker lifecycle and allocator harnesses, profiling/output parity,
direct LLVM/no-Clang and doctor checks, installation smoke tests, and 279 C
compatibility fixtures at both optimization levels. CodeLLDB stepping passed.
Both ThreadSanitizer and AddressSanitizer passed the lifecycle harness in debug
and release, including native creation-failure injection, and the allocator
harness using the new production thread primitives.
