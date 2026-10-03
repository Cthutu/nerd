# Raptor queues and work stealing in Nerd

`std.raptor` implements our own scheduler around Raptor's concurrent/serial
queues and typed task API. The implementation was written for Nerd; it does not
embed ASIO or copy the upstream scheduler/deque implementation. The user selected
this direction on 3 October 2026. [Source inventory](stdlib-source-inventory.md)
records the API reference revision; [tutorial](tutorials/raptor.md) starts with an
executable example.

## Scheduling and progress

A scheduler starts an explicit, fixed number of `std.thread` workers. Each owns
one bounded `std.queue.WorkDeque[^void]`. A worker removes its newest local job
(LIFO), then visits other workers and steals their oldest job (FIFO). External
submitters rotate their preferred worker; callback submissions prefer their
current worker. Concurrent queues promise independent execution, without a FIFO
ordering guarantee. Serial queues admit in mutex acquisition order and execute
one callback at a time in that order, across whichever workers claim their jobs.

This first version is a correctness reference using mutexes. A scheduler mutex
serialises admission, ready-work claims, serial succession and completion
bookkeeping. Callbacks execute outside that mutex. Each standalone deque also
has a mutex, making its final-item owner/thief race a single serialised removal.
The scheduler always acquires its mutex before a deque mutex. There is no spin
loop, lock-free claim, queue growth, reclamation or performance claim. Fairness
and starvation freedom are not guaranteed under continuous local submissions.

`init(workers, capacity = 256) -> ?void` allocates all worker/deque storage
before starting threads. At most `workers * capacity` tasks may remain incomplete, including
executing tasks and serial backlogs. `async` rejects excess admission immediately;
it does not wait for capacity. Each accepted task allocates one stable typed
invocation; serial backlogs link these records. Completed but unreleased task
handles no longer consume admission capacity, but still retain their allocations.
All native-start failures join previously started workers and free storage;
allocation uses the existing allocator's fatal out-of-memory policy. Linux failure
injection verifies partial-start cleanup and reinitialisation;
native Windows execution remains validation work.

## API mapping and deliberate differences

| Raptor API | Nerd API | Behaviour |
| --- | --- | --- |
| `Scheduler(n)` | `scheduler.init(n)` / `close()` | Explicit owner lifetime; drain accepted work |
| `createQueue(kind)` | `create_queue(^queue, kind)` | Caller supplies stable queue storage |
| `getDefaultQueue()` | `get_default_queue()` | Borrowed concurrent queue |
| `Queue::async(callback)` | `queue.async(callback, argument)` | Infers argument/result types; returns an owned `?Task[T]` |
| `Task<T>` / `Task<void>` | `Task[T]` / `Task[void]` | One-shot typed completion, including void |
| `hasFinished()` / `wait()` / `get()` | `has_finished()` / `wait()` / `get()` | Completion query; wait success; optional result |
| `when_all` | `when_all[T](tasks[..])` | Attempts every wait, returning overall success; no aggregate task yet |
| `Result<T>` | `Task[T].get()` | Result publication belongs to the task; no separately mutable result object |

Callbacks use `fn (argument: A) -> T`; `Queue.async[A, T]` infers both types
from the callback and its argument. A callback can take `^i32`, another borrowed
pointer, or a plain value such as `i32`. No erased-pointer cast is needed in user
code; Nerd also allows implicit conversion of any pointer to `^void` when a
callback explicitly accepts it. Void callbacks return `Task[void]`;
`get() != nil` means successful completion. A task's result may itself be an
optional or result type. `init` uses `?void`, so callers can propagate failure
with `scheduler.init(4)?`, just as they can propagate rejected submission with
`queue.async(callback, argument)?`.

There is no exception transport, cancellation, continuation, timer queue, ASIO
service accessor, or graph API in this slice. `when_all` is a synchronous boolean
wait over handles, rather than a returned aggregate task. These remain deliberate
scope limits rather than requirements of the work-stealing algorithm.

External waiters sleep on a condition predicate. A waiting worker cooperatively
runs eligible ready jobs, so a single worker can submit and wait for children.
`wait` rejects idle tasks, self/ancestor waits and dependencies on its own active
serial queue. Arbitrary cross-worker dependency cycles remain invalid; a blocked
callback also occupies a worker. Use these workers for finite CPU work rather
than blocking socket operations. Completion is published atomically, currently
with sequentially consistent operations (stronger than release/acquire).

## Ownership and shutdown

Zero-initialise owned objects. Keep schedulers, queues and borrowed callback
argument targets at stable addresses until their accepted work completes.
Initialise and finish standalone deques before/after their users. Do not copy
active schedulers, queues or deques.

`async` returns a handle whose invocation and completion state have stable heap
addresses. The returned handle may move before completion. Transfer an existing
handle with `task.take()`, which leaves the source idle; do not duplicate ownership
by ordinary assignment, and do not overwrite a live handle. Rejected submission
returns `nil` without transferring any argument payload ownership. Public internal
record fields exist to satisfy Nerd's public member-type rules; callers must not
mutate task state, invocation records or scheduler bookkeeping.

Every accepted handle must eventually call `done()`. It waits for completion,
releases the invocation allocation and resets the handle. Idle/released cleanup
succeeds, making deferred cleanup safe for partially filled arrays. Invalid
self/ancestor/serial waits cause `done` to return failure and retain ownership.
Register cleanup before submitting tasks so propagated admission failure also
releases earlier accepted handles. Stop all readers/waiters before releasing a
handle; concurrent cleanup or cleanup while another thread observes it is invalid.

`get` and `wait` may be called repeatedly before cleanup. Arguments and results
are shallow values: pointer/slice targets remain caller-owned. Owning boxes, or
records containing them, are not supported as argument/result payloads because
this storage has no payload destruction hooks; pass borrowed pointers to
caller-owned objects instead. Never return a pointer or slice into callback local
storage or a worker temporary arena. Release any caller-owned result payloads
separately, after all readers finish; `done` only frees invocation storage.

Only the owning external thread initialises/closes the scheduler. Stop external
submitters and waiters before `close`; callbacks must terminate and their borrowed
storage must remain alive. Close rejects callback attempts to close their own
scheduler, stops admission, drains accepted jobs, joins workers and frees storage.
It is idempotent. Completed task handles remain readable after close until their
own `done()`; closing the scheduler does not release handle allocations. Queues expire at
close: discard/reinitialise them before a later scheduler lifetime, and never
reuse a stale queue after reinitialising its scheduler.

## Validation

`build/test_raptor.py` runs native LLVM and generated C, in debug and release
modes, with process-tree timeouts. Repeated contracts check deque wrap/full/empty,
competing owner/thief removal of one item, forced cross-worker stealing, serial
FIFO/non-overlap, void/result publication, nested single-worker waits, self and
serial-dependency rejection, external producers, capacity/retry, typed pointer/value
callbacks, handle moves, explicit/idempotent cleanup, post-close results,
partial-start cleanup/reinitialisation and draining close. Both common test recipes
include it. The executable tutorial's result is checked against 204. Native Windows and performance measurements remain pending.
