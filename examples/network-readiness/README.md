# Socket readiness and deadlines

Run `just run-example network-readiness` from the repository root.

A loopback TCP client starts a nonblocking connection, waits for completion,
and exchanges `hello deadlines` with a nonblocking server. The output is:

```text
TCP readiness: hello deadlines
```

One absolute monotonic deadline covers connection setup and every partial send,
receive and readiness retry. The helpers check that deadline before each attempt
and handle `WouldBlock` without restarting the timeout. The tiny exchange runs
in one thread and needs no external service, fixed port, startup sleep or worker.

See the [readiness API and contracts](../../docs/std-network-foundation.md#readiness-and-deadlines).
