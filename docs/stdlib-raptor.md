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
before starting threads. At most `workers * capacity` tasks may remain incomplete,
including executing tasks and serial backlogs. `async` rejects excess admission
immediately;
it does not wait for capacity. Preparation and submission allocate no per-task
storage: the callback, borrowed argument pointer, dispatch links and result live
in caller-owned `Task[T]`. Serial backlogs link these tasks. Completed tasks
consume no admission capacity and require no cleanup.
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
| `Queue::async(callback)` | `task.create(callback, ^argument)` then `queue.async(^task)` | Prepare borrowed inputs, then schedule; admission returns `?void` |
| `Task<T>` / `Task<void>` | `Task[T]` / `Task[void]` | Caller-owned typed completion, including void; reusable after completion |
| `hasFinished()` / `wait()` / `get()` | `has_finished()` / `wait()` / `get()` | Completion query; wait success; optional result |
| `when_all` | `when_all(tasks[..])` | Attempts every wait, returning overall success; no aggregate task yet |
| `Result<T>` | `Task[T].get()` | Result publication belongs to the task; no separately mutable result object |

Callbacks use `fn (value: ^U) -> T`, where `T` is the task's result type and
`U` is inferred from the callback and argument pointer. `task.create(callback,
^argument)` stores that callback and borrows the argument without allocating or
starting work. The callback's result must match `Task[T]`; mismatched callbacks
and argument pointers are compiler errors. Multiple inputs belong in a
caller-owned context record passed by pointer. Nil arguments are allowed if the
callback handles them; a nil callback is a programming error.

Zero-initialised tasks start idle. `create` prepares an idle, prepared or completed
task and clears any previous result. Calling it on a pending task asserts, and
must not race with submission or readers/waiters. `async(^task)` accepts a
prepared task once; idle, already-submitted, nil and closed-queue submissions
return `nil`. Rejection leaves the task unchanged, so capacity rejection can be
retried without recreating it. Reusing a completed task requires another `create`
after previous readers finish. Serialise preparation and submission for each
task, including submissions to different queues or schedulers.

Void callbacks use `Task[void]`; `get() != nil` means successful completion.
Results may themselves be optional or result types. Both startup and submission
use `?void`, permitting `scheduler.init(4)?` and `queue.async(^task)?`.
`when_all(tasks[..])` infers `T` from the slice and attempts every wait, returning
false for idle/prepared entries or invalid dependencies.

There is no exception transport, cancellation, continuation, timer queue, ASIO
service accessor, or graph API in this slice. `when_all` is a synchronous boolean
wait over tasks, rather than a returned aggregate task. These remain deliberate
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

`async` borrows the prepared task at its final address. Prepared tasks can move
before submission because `create` retains no pointer to the task itself. Keep
each task alive at the same address until `wait()` succeeds, `get()` succeeds, `has_finished()` reports
completion, or its scheduler's `close()` has drained the work. Do not copy, move,
overwrite or discard pending tasks; growable containers must not relocate them.
Submission may begin executing before `async` returns, so the storage must
already be stable when passed to it. Public internal fields exist to satisfy
Nerd's public member-type rules; callers must not mutate scheduler bookkeeping
or task state.

Completion is published only after callback execution and scheduler bookkeeping
finish. Workers never access that task again after publication. The completed task retains its result directly and has no execution
allocation to release: there is no `done()` or `take()`. Completed tasks can be
copied, moved or discarded after previous readers finish, or prepared again with
`create` for a later submission. Copying a result remains shallow.

Before an early return, wait for previously accepted tasks while their storage
and borrowed arguments remain alive. For an array with some rejected or
never-submitted entries, `defer _ := when_all(tasks[..])` attempts every wait;
its false return for idle entries does not prevent waiting for accepted ones.
A successful scheduler close also provides this lifetime boundary. Failed
self/ancestor/serial waits do not permit destroying pending storage.

`get` and `wait` may be called repeatedly until the task is reprepared or
discarded. Arguments are borrowed; their targets may contain caller-owned data,
including owning boxes. Results are shallow values: pointer/slice targets remain
caller-owned. Owning boxes, or records containing them, are not supported as
result payloads because task storage has no payload destruction hooks. Never
return a pointer or slice into callback local storage or a worker temporary
arena. Release caller-owned result payloads separately after all readers finish;
discarding the task does not release them.

Only the owning external thread initialises/closes the scheduler. Stop external
submitters and waiters before `close`; callbacks must terminate and their borrowed
storage must remain alive. Close rejects callback attempts to close their own
scheduler, stops admission, drains accepted jobs, joins workers and frees storage.
It is idempotent. Completed tasks remain readable after close, with their results
in caller storage. Queues expire at close: discard/reinitialise them before a
later scheduler lifetime, and never reuse a stale queue after reinitialising its scheduler.

## Validation

`build/test_raptor.py` runs native LLVM and generated C, in debug and release
modes, with process-tree timeouts. Repeated contracts check deque wrap/full/empty,
competing owner/thief removal of one item, forced cross-worker stealing, serial
FIFO/non-overlap, void/result publication, nested single-worker waits, self and
serial-dependency rejection, external producers, capacity/retry, typed borrowed
callbacks, mismatched callback/argument diagnostics, prepared relocation,
duplicate submission rejection, pending recreation/nil-callback assertion checks,
completed reuse/copies, inferred aggregation, discard without result reads,
post-close results, partial-start cleanup/reinitialisation and draining close. Both common test recipes
include it. The executable tutorial's result is checked against 204. Native
Windows and performance measurements remain pending.
