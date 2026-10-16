# Tutorial: typed tasks, serial output and main-thread queues with Raptor

Use `std.raptor` to run finite calculations on an explicitly owned worker pool.
This is Nerd's own work-stealing implementation of the Raptor queue/task model.
Queues submit typed callbacks into caller-owned task storage. Borrowed callback
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
defer _ := when_all(tasks[..])
for i in [0 .. values.count] {
    tasks[i].create(square, ^values[i])
    scheduler.get_default_queue().async(^tasks[i])?
}
assert when_all(tasks[..])
```

`Task.create` accepts a callback `fn (value: ^U) -> T` and a borrowed `^U`.
`T` comes from `Task[i32]`, and `U` is inferred as `i32`. Preparation stores the
callback and pointer without allocating or executing work. `Queue.async` borrows
the prepared task and returns `?void` for admission success or failure; a callback
may start before submission returns. Both `?` expressions propagate failure from
`main`. `when_all(tasks[..])` also infers `i32`; no explicit type argument is
needed. The values and task array stay alive throughout execution.

The deferred wait is registered **before submission**: if a later submission
fails, it still waits for every previously accepted task before `main` leaves
scope. `when_all` attempts every entry, returning false for never-submitted slots;
we deliberately ignore that return during the deferred wait. Defers execute in
reverse registration order, so the array's wait happens before scheduler close.
Tasks and borrowed arguments must remain alive until completion on every exit.

Callbacks and results live directly in the caller's task, with no per-task
allocation. A completed task needs no `done()` and can be discarded safely without reading its result.

`when_all` waits for every accepted task. Each `get()` returns an optional result;
the complete example extracts it and adds it to the total. Calling `get` directly
also waits, so tasks can be collected individually. Callbacks finish in any order;
collecting results by index remains deterministic. The example closes the pool
and checks that the sum is 204 before leaving scope.

## Stable task storage

Declare task storage before submission and keep it at the same address until
completion:

```nerd
task : Task[i32]
task.create(square, ^values[0])
scheduler.get_default_queue().async(^task)?
assert task.wait()
assert task.get() != nil
```

A prepared task can move before submission; `async` borrows its final address.
Do not copy, move, overwrite, reprepare or discard a pending task. An array in the current
scope provides stable storage; a growable container must not relocate its tasks
while they are pending. A callback can start before submission returns.

After completion, the task is an ordinary value with no execution allocation.
You may copy or move it, leave scope, or call `create` again for another submission.
Repreparing clears the old result; wait for all previous readers to finish first.
Calling `create` on pending work asserts. Each preparation can be submitted once;
submitting an idle or already-submitted task returns `nil`. Rejected admission
leaves the preparation intact, so it can be retried after capacity becomes free.

Arguments are borrowed pointers. For several inputs, pass a pointer to a context
record containing them. The callback may borrow owning data in that record, but
must not let it leave scope before completion. Results are shallow values; owning
boxes or records containing them are unsupported result payloads. Release any
caller-owned result payload storage separately after all readers finish.

## Serial work and void completion

Create a serial queue in storage owned by the same scope as the scheduler:

```nerd
serial : Queue
assert scheduler.create_queue(^serial, QueueType.Serial)
task : Task[void]
task.create(update_state, ^state)
serial.async(^task)?
assert task.get() != nil
```

Here `update_state` is a `fn (state: ^State)` callback and `state` is a caller-owned
`State`. Callbacks submitted to this queue execute one at a time in admission
order. They may update shared state without an extra mutex **only if all accesses
use that queue**, or the owner waits before reading. Another queue can run
concurrently. `Task[void]` carries completion without a result payload.

### Concurrent calculations feeding serial output

Run the [complete program](../../examples/task-serial-output/task-serial-output.n):

```sh
just run-example task-serial-output
```

Eight callbacks on the default concurrent queue square their inputs. Each owns a
separate output task inside its stable `Work` context:

```nerd
Work :: plex {
    number       i32
    result       i32
    output_queue ^Queue
    output_task  Task[void]
    summary      ^Summary
}

calculate :: fn (work: ^Work) {
    work.result = work.number * work.number
    work.output_task.create(print_result, work)
    assert work.output_queue.async(^work.output_task) != nil
}
```

`Summary` holds a count and total. `print_result` adds the result to that summary
and prints one line. Only the serial output queue updates the summary. The pool
uses four workers with capacity 64 per worker, enough to admit all eight
calculation tasks and eight output tasks; this finite example therefore asserts
output admission. A larger producer must handle capacity rejection.

The owner first waits for every calculation, ensuring all output tasks have been
prepared and submitted, then waits for the output tasks before reading the
summary. The deferred cleanup uses that same order on an early return. An output
callback may complete before its calculation callback returns, so both task sets
must finish before discarding their shared contexts.

Individual lines appear in output admission order, which depends on when the
concurrent calculations finish. Serial execution prevents overlap; it does not
sort by input index. The program checks that eight outputs ran and prints
`Serial output: total = 204` last. Serial callbacks can run on different workers;
use a main-thread queue when thread affinity matters.

The [executable contracts](../../tests/stdlib-raptor/contracts.n) submit 64 ordered
updates, check their sequence and non-overlap, and exercise void completion,
stable completion storage, preparation, reuse and inferred aggregation. Run them and
the calculation through both backends:

```sh
python3 build/test_raptor.py --nerd _bin/nerd-debug
python3 build/test_raptor.py --nerd _bin/nerd-debug --cgen
```

## Explicit main-thread work

Every main-thread queue is serial. The thread that calls `scheduler.init` owns
execution, so initialise the scheduler on the main thread. Its workers keep
running concurrently, but cannot execute main-thread tasks.

Run the [complete program](../../examples/task-main-thread/task-main-thread.n):

```sh
just run-example task-main-thread
```

It prepares three `Task[void]` entries with a `print_number` callback, then uses
the main queue instead of the default worker queue:

```nerd
queue := scheduler.get_main_queue()
-- These callbacks enqueue no additional work, so one deferred batch covers
-- accepted submissions if a later submission fails.
defer assert scheduler.run_main_tasks() != nil
for i in [0 .. values.count] {
    tasks[i].create(print_number, ^values[i])
    queue.async(^tasks[i])?
}
on scheduler.run_main_tasks() {
    count => {
        assert count == 3
    }
    else => {
        assert no
    }
}
assert when_all(tasks[..])
```

The output is always:

```text
Main thread: 10
Main thread: 20
Main thread: 30
```

`run_main_tasks` executes the FIFO batch queued when the call begins and returns
its size as `?usize`. Zero means an empty queue; `nil` means the scheduler is
inactive, the caller is not its owner, or a main callback tried to pump recursively.
Tasks submitted during a batch wait for the next pump. An application usually
calls this method from its main loop.

Owner-thread `wait` and `get` fail for unfinished main tasks: explicitly pump
before collecting them. Other threads can submit and wait for main-thread work,
but depend on the owner pumping it. Keep pumping while such producers run rather
than blocking the owner waiting for them. Stop producers before final cleanup.
If callbacks enqueue further main work, drain with repeated batches before
leaving scope. Scheduler close rejects queued main work without stopping the
scheduler; it never pumps it for you. The example registers its deferred pump
after the deferred close, so pumping runs first on every exit.

You can also create caller-owned queues with `QueueType.MainThread`. All such
queues share the scheduler's owner and FIFO; they do not create additional threads.

## Nested work and bounds

A callback can submit a child to a concurrent queue and call its `get`. The
waiting worker executes ready jobs itself, allowing this even with one worker.
Keep borrowed child arguments alive until its wait completes, and wait before
the child task storage leaves scope. Submission still consumes capacity; reserve
room for children or handle rejection. Completed tasks retain only their
caller-owned completion state and result, with no execution allocation to release.

Waiting on yourself, an active ancestor or unfinished work in your own serial
queue returns failure. A failed wait does not make pending storage safe to
discard. A serial callback must return before its queued successor can run. Other
dependency cycles are also invalid and are not generally detected. Avoid blocking I/O in this worker
pool; Nexus transport will have its own readiness and lifetime rules.
