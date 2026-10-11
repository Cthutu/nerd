# Main-thread tasks

Run `just run-example task-main-thread` from the repository root.

The scheduler owns four workers, but these three tasks run serially on the thread
that initialised it, only when `run_main_tasks()` explicitly pumps the queue.
The output is `Main thread: 10`, then `20`, then `30` on separate lines.
The deferred pump completes accepted work before shutdown even if submission fails.

See the [Raptor tutorial](../../docs/tutorials/raptor.md#explicit-main-thread-work)
and [queue contracts](../../docs/stdlib-raptor.md).
