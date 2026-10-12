# nexus-request-reply

Run `just run-example nexus-request-reply` from the repository root.

Request and reply sockets use the same message API while enforcing turn-taking.
Two exchanges succeed, followed by a deliberate wrong-state check.

```text
Nexus request/reply: two exchanges passed
```

Both endpoints run in one thread on an OS-selected loopback port, with bounded
waits and no external service. See the [tutorial](../../docs/tutorials/nexus.md)
and [API contracts](../../docs/stdlib-nexus.md).
