# Tutorial: typed tasks and serial queues with Raptor

Use `std.raptor` to run finite calculations on an explicitly owned worker pool.
This is Nerd's own work-stealing implementation of the Raptor queue/task model.
You own the task and argument storage, so its addresses must remain stable until
the workers finish. [API contracts](../stdlib-raptor.md) cover admission, waits,
result ownership and shutdown in detail.

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
square :: fn (context: ^void) -> i32 {
    value := context.as(^i32)^
    return value * value
}
```

The owner creates four workers and eight stable task/context slots:

```nerd
scheduler : Scheduler
on !scheduler.init(4) => return 1
values : [8]i32 = [1, 2, 3, 4, 5, 6, 7, 8]
tasks : [8]Task[i32]
for i in [0 .. values.count] {
    assert tasks[i].async(scheduler.get_default_queue(), square, (^values[i]).as(^void))
}
assert when_all[i32](tasks[..])
```

`async` reports whether submission succeeded. It may return before or after the
callback starts. The example has enough capacity for every submission and uses
an assertion; an application can retry a rejected task after completing earlier
work. Task slots and `values` stay alive in `main` throughout execution.

`when_all` waits for every accepted task. Each `get` then returns an optional
result; the example extracts it and adds it to the total. Calling `get` directly
also waits, so you can collect tasks individually. Concurrent callbacks can finish
in any order; collecting their results by index is still deterministic. Finally,
`close` drains accepted work, joins the workers and releases the pool's storage.

## Serial work and void completion

Create a serial queue in storage owned by the same scope as the scheduler:

```nerd
serial : Queue
assert scheduler.create_queue(^serial, QueueType.Serial)
task : Task[void]
assert task.async(^serial, update_state, state_context)
assert task.get() != nil
```

Here `update_state` is a `fn (context: ^void)` callback and `state_context` points
to state kept alive by the owner. Callbacks submitted to this queue execute one at
a time in admission order. They may safely update shared state without an extra
mutex **only if all accesses use that queue**, or the owner waits before reading.
Another queue can still run concurrently. `Task[void]` carries completion without
a result payload.

The [executable contract example](../../tests/stdlib-raptor/contracts.n) submits
64 ordered updates, checks their sequence and non-overlap, and exercises void
completion. Run it and the calculation through both backends with:

```sh
python3 build/test_raptor.py --nerd _bin/nerd-debug
python3 build/test_raptor.py --nerd _bin/nerd-debug --cgen
```

## Nested work and bounds

A callback can submit a child to a concurrent queue and call its `get`. The
waiting worker executes ready jobs itself, allowing this to work even with one
worker. Keep the child task/context alive until its wait completes. Submission
still consumes capacity; reserve room for children or handle rejection.

Waiting on yourself, an active ancestor or unfinished work in your own serial
queue returns failure. A serial callback must return before its queued successor
can run. Other dependency cycles are also invalid and are not generally detected.
Avoid blocking I/O in this worker pool; Nexus transport will have its own readiness
and lifetime rules.
