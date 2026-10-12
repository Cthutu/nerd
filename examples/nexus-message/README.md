# nexus-message

Run `just run-example nexus-message` from the repository root.

A client sends a string and number. The server clears and reuses its received
message to reply to that sender. TCP setup, framing and reply routing are handled
by Nexus.

```text
Nexus message: hello nexus, reply 43
```

Both endpoints run in one thread on an OS-selected loopback port, with bounded
waits and no external service. See the [tutorial](../../docs/tutorials/nexus.md)
and [API contracts](../../docs/stdlib-nexus.md).
