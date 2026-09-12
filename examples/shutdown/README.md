# Graceful shutdown

From the repository root, run:

```sh
just run-example shutdown
```

Press Ctrl+C when the example says it is ready. It prints `Shutdown requested.`
and `Cleanup complete.`, then exits successfully.

The example uses `std.signal.watch_shutdown()` to own shutdown notification,
`wait()` to sleep until a request arrives, and `requested()` to read the sticky
request flag. Deferred cleanup runs on the main thread, including `close()`.
No application signal handler or C library declarations are needed.

Create the watcher on the main thread before starting worker threads. Use it
only on that thread, and stop and join workers before closing it. Only one
watcher can be active in a process. See [the API notes](../../docs/stdlib.md#shutdown-notifications)
for supported events and platform limits.
