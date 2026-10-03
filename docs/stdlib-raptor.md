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

`init(workers, capacity = 256)` allocates all worker/deque storage before starting
threads. At most `workers * capacity` tasks may remain incomplete, including
executing tasks and serial backlogs. `async` rejects excess admission immediately;
it does not block or allocate. Serial backlogs link the borrowed task records.
All native-start failures join previously started workers and free storage;
allocation uses the existing allocator's fatal out-of-memory policy. Linux failure injection verifies partial-start cleanup and reinitialisation;
native Windows execution remains validation work.

## API mapping and deliberate differences

| Raptor API | Nerd API | Behaviour |
| --- | --- | --- |
| `Scheduler(n)` | `scheduler.init(n)` / `close()` | Explicit owner lifetime; drain accepted work |
| `createQueue(kind)` | `create_queue(^queue, kind)` | Caller supplies stable queue storage |
| `getDefaultQueue()` | `get_default_queue()` | Borrowed concurrent queue |
| `Queue::async(callback)` | `task.async(^queue, callback, context)` | Caller supplies typed task storage; returns admission success |
| `Task<T>` / `Task<void>` | `Task[T]` / `Task[void]` | One-shot typed completion, including void |
| `hasFinished()` / `wait()` / `get()` | `has_finished()` / `wait()` / `get()` | Completion query; wait success; optional result |
| `when_all` | `when_all[T](tasks[..])` | Attempts every wait, returning overall success; no aggregate task yet |
| `Result<T>` | `Task[T].get()` | Result publication belongs to the task; no separately mutable result object |

Callbacks use `fn (context: ^void) -> T`. Context provides explicit argument
storage instead of owning a C++ closure. Void callbacks are supported;
`Task[void].get() != nil` means successful completion. A task's result may itself
be an optional or result type. There is no exception transport, cancellation,
continuation, timer queue, ASIO service accessor, or graph API in this slice.
The upstream timer queue is also unimplemented. Asynchronous aggregation remains
an API extension to design with caller-owned storage.

External waiters sleep on a condition predicate. A waiting worker cooperatively
runs eligible ready jobs, so a single worker can submit and wait for children.
`wait` rejects idle tasks, self/ancestor waits and dependencies on its own active
serial queue. Arbitrary cross-worker dependency cycles remain invalid; a blocked
callback also occupies a worker. Use these workers for finite CPU work rather
than blocking socket operations. Completion is published atomically, currently
with sequentially consistent operations (stronger than release/acquire).

## Ownership and shutdown

Zero-initialise owned objects. Keep scheduler, queues, tasks and callback contexts
at stable addresses until all accepted work completes. Initialise and finish
standalone deques before/after their users; deque operations require successful
initialisation. Do not copy any of them
while active. A rejected task remains caller-owned and may be retried; an accepted
task cannot be submitted again. Keep a task's `callback`, `context`, `result` and
`job` fields private by convention after submission. `Job` and `Worker` are exposed
only because public Nerd record fields require public member types.

`get` and `wait` may be called repeatedly after completion. Results are shallow
copies: callers manage payload lifetime and cleanup. Never return a pointer or
slice into a callback's local storage or worker temporary arena. One observer
must decide when all result readers have finished before freeing payloads.

Only the owning external thread initialises/closes the scheduler. Stop external
submitters and waiters before `close`; callbacks must terminate and their borrowed
storage must remain alive. Close rejects callback attempts to close their own
scheduler, stops admission, drains accepted jobs, joins workers and frees storage.
It is idempotent. Completed tasks remain readable after close. Queues expire at
close: discard/reinitialise them before a later scheduler lifetime, and never
reuse a stale queue after reinitialising its scheduler.

## Validation

`build/test_raptor.py` runs native LLVM and generated C, in debug and release
modes, with process-tree timeouts. Repeated contracts check deque wrap/full/empty,
competing owner/thief removal of one item, forced cross-worker stealing, serial
FIFO/non-overlap, void/result publication, nested single-worker waits, self and
serial-dependency rejection, external producers, capacity/retry, partial-start cleanup/reinitialisation and draining
close. Both common test recipes include it. The executable tutorial's result is
checked against 204. Native Windows and performance measurements remain pending.
