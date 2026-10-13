# nexus-timeouts

Run `just run-example nexus-timeouts` from the repository root.

This example shows a receive timing out, a nonblocking receive returning
`WouldBlock`, and a successful retry after a client sends data.

```text
Receive timed out
Receive would block
Received: hello after retry
```

Both endpoints run in one thread on an OS-selected loopback port. No second
process is required. See the [focused tutorial](../../docs/tutorials/nexus-timeouts.md).
