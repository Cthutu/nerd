# Structured Nexus message echo

Run `just run-example nexus-message` from the repository root.

A nonblocking loopback TCP client sends `"hello nexus"` and 42. The server
reuses the message buffer to reply with `"reply"` and 43. Both streams retain
partial framing state and share an absolute two-second deadline. Output:

```text
Nexus message: hello nexus, reply 43
```

The exchange runs in one thread with an ephemeral port and needs no external
service. See the [tutorial](../../docs/tutorials/nexus.md) and
[ownership and wire contracts](../../docs/stdlib-nexus.md).
