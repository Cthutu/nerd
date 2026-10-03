# Tutorial: typed tasks and serial queues with Raptor

Use `std.raptor` to run finite calculations on an explicitly owned worker pool.
This is Nerd's own work-stealing implementation of the Raptor queue/task model.
Queues submit typed callbacks and return owned task handles. Borrowed callback
arguments must remain alive until completion. [API contracts](../stdlib-raptor.md)
cover admission, waits, result ownership and shutdown in detail.

## Run a calculation

From the repository root:

```sh
just build nerd --skip-mod-sync
just run-example task-parallel
```

The [complete program](../../examples/task-parallel/task-parallel.n) prints:

```text
Raptor tasks: sum of squares = 204
```

Its callback reads one borrowed integer and returns its square:

```nerd
square :: fn (value: ^i32) -> i32 {
    number := value^
    return number * number
}
```

Inside `main :: fn () -> ?void`, start four workers and submit eight calculations:

```nerd
scheduler : Scheduler
scheduler.init(4)?
defer assert scheduler.close()
values : [8]i32 = [1, 2, 3, 4, 5, 6, 7, 8]
tasks : [8]Task[i32]
defer {
    for i in [0 .. tasks.count] { assert tasks[i].done() }
}
for i in [0 .. values.count] {
    tasks[i] = scheduler.get_default_queue().async(square, ^values[i])?
}
assert when_all[i32](tasks[..])
```

`Queue.async` infers the argument and result types, returning `?Task[i32]` here.
No `^void` cast is necessary. A plain value callback such as
`fn (value: i32) -> i32` can also be submitted with an integer argument.
`init` returns `?void`; both `?` expressions propagate failure from `main`.
Submission may return before or after the callback starts. Values stay alive in
`main` throughout execution.

Cleanup is registered **before submission**: if a later submission fails, the
deferred loop waits for and releases previously accepted tasks. Empty slots
clean up successfully. Defers execute in reverse registration order, so handles
are released before the scheduler closes. `done()` frees each task's invocation;
`close()` drains accepted work and joins workers but does not free task handles.

`when_all` waits for every accepted task. Each `get()` returns an optional result;
the complete example extracts it and adds it to the total. Calling `get` directly
also waits, so tasks can be collected individually. Callbacks finish in any order;
collecting results by index remains deterministic. The example closes the pool
and checks that the sum is 204 before deferred handle cleanup.

## Handle ownership

A returned handle can move while its callback is running because the invocation
has a stable allocation. Transfer an existing handle with `task.take()`:

```nerd
task := scheduler.get_default_queue().async(square, ^values[0])?
moved := task.take()
defer assert moved.done()
assert moved.get() != nil
```

The source is now idle. Do not copy live handles or overwrite them without
cleanup. Arguments and results are shallow values; do not pass owning boxes or
records containing them as payloads. Pass borrowed pointers to owner-held
objects, and keep them alive until the task finishes. Release any result payload
storage separately after all readers finish.

## Serial work and void completion

Create a serial queue in storage owned by the same scope as the scheduler:

```nerd
serial : Queue
assert scheduler.create_queue(^serial, QueueType.Serial)
task := serial.async(update_state, ^state)?
defer assert task.done()
assert task.get() != nil
```

Here `update_state` is a `fn (state: ^State)` callback and `state` is a caller-owned
`State`. Callbacks submitted to this queue execute one at a time in admission
order. They may update shared state without an extra mutex **only if all accesses
use that queue**, or the owner waits before reading. Another queue can run
concurrently. `Task[void]` carries completion without a result payload.

The [executable contracts](../../tests/stdlib-raptor/contracts.n) submit 64 ordered
updates, check their sequence and non-overlap, and exercise void completion,
handle movement and cleanup. Run them and the calculation through both backends:

```sh
python3 build/test_raptor.py --nerd _bin/nerd-debug
python3 build/test_raptor.py --nerd _bin/nerd-debug --cgen
```

## Nested work and bounds

A callback can submit a child to a concurrent queue and call its `get`. The
waiting worker executes ready jobs itself, allowing this even with one worker.
Keep borrowed child arguments alive until its wait completes and release the
child handle with `done`. Submission still consumes capacity; reserve room for
children or handle rejection. Completed handles retain memory until cleanup,
even though they no longer consume admission capacity.

Waiting on yourself, an active ancestor or unfinished work in your own serial
queue returns failure, including when attempted by `done`. A serial callback
must return before its queued successor can run. Other dependency cycles are
also invalid and are not generally detected. Avoid blocking I/O in this worker
pool; Nexus transport will have its own readiness and lifetime rules.
