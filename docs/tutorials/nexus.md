# Nexus: reusable messages over TCP

Run `just run-example nexus-message`. It creates a loopback listener on an
OS-selected port, connects a client, accepts a server socket and enables
nonblocking I/O on both ends. A single thread exchanges a string and number:

```text
Nexus message: hello nexus, reply 43
```

The complete runnable code is in
[examples/nexus-message/nexus-message.n](../../examples/nexus-message/nexus-message.n).
Connection setup uses the [raw socket readiness API](../std-network-foundation.md).
The message exchange imports `std.nexus` and `std.time`:

```nerd
client_stream := TcpStream.new(client)?
defer client_stream.done()
server_stream := TcpStream.new(server)?
defer server_stream.done()
request : Message
defer request.done()
reply : Message
defer reply.done()
request.append_string("hello nexus")?
request.append_u32(42)?
client_stream.queue(^request)?
request.clear()
assert client_stream.flush(deadline = deadline)?
assert server_stream.receive(^request, deadline = deadline)?
assert request.read_string()? == "hello nexus"
number := request.read_u32()?
```

Here `client` and `server` are pointers to stable, connected, nonblocking
`std.network.Socket` values, and `deadline` is a monotonic `Instant` made with
`now().add(from_secs(2))`. `queue` snapshots the unread payload, so clearing
`request` immediately is safe. `flush` sends its length prefix and payload,
resuming any partial writes. `receive` waits for a whole frame before replacing
`request`. The typed reads consume the string and number through a cursor.

The example clears the same request buffer, appends `"reply"` and `number + 1`,
queues that message on `server_stream`, flushes it, and receives it into `reply`
on the client. Clearing retains storage for reuse. The expected reply is 43.
The example's assertions require completion before its two-second deadline;
production code should handle a false result as deadline expiry.

For immediate polling, use `try_flush()` or `try_receive(^reply)`. A false result
keeps partial progress, allowing another call after socket readiness. For a
bounded wait, pass `deadline = limit`; reuse that absolute deadline across
retries rather than starting a fresh timeout on each would-block result.
After expiry you may deliberately choose a later deadline and resume the same
frame. Keep the stream alive while a frame is incomplete.

Messages own storage and need `done()`. Streams own two bounded buffers and
need `done()` as well; they borrow sockets, which need a separate `close()`.
The example disposes of streams before closing their sockets. Read strings
borrow message storage and remain valid through other reads, but expire when
the message is appended, cleared, successfully received into or disposed of.
Keep each owner in stable storage and do not copy allocated values.

The [API contracts](../stdlib-nexus.md) describe limits, failures and tested C
wire compatibility. Multiple clients, UDP routing, request/reply and Telnet
will have separate walkthroughs when those layers are implemented.
