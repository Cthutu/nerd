# nexus-multi-client

Run `just run-example nexus-multi-client` from the repository root.

Two clients send numbers to one server. Reusing each received message sends its
reply to the originating client without a connection lookup.

```text
Nexus multi-client: replies 11 and 21
```

Both endpoints run in one thread on an OS-selected loopback port, with bounded
waits and no external service. See the [tutorial](../../docs/tutorials/nexus.md)
and [API contracts](../../docs/stdlib-nexus.md).
