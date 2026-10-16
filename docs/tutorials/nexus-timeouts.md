# Nexus: receive timeouts and retry

This tutorial shows three outcomes of `Message.recv()`: `TimedOut`,
`WouldBlock`, and success after retry. It assumes the socket/message setup
from the [basic Nexus tutorial](nexus.md).

Run the [commented example](../../examples/nexus-timeouts/nexus-timeouts.n):

```sh
just run-example nexus-timeouts
```

Expected output:

```text
Receive timed out
Receive would block
Received: hello after retry
```

The example runs both endpoints in one thread using an OS-selected loopback
port. It needs no external server or sleeps to coordinate the endpoints.

## Wait for a limited time

With a bound server and an associated `incoming` message:

```nerd
server.set_option(NetOption.RecvTimeoutMs, 50)?
on incoming.recv() {
    NexusError.TimedOut! => prn("Receive timed out")
    error! => return error!
    else => { assert no }
}
```

No client has been created yet, so the receive exhausts its 50 ms budget.
`TimedOut` means this call could not receive a complete message within that
budget. The socket remains usable. The example handles this expected error
and propagates any other error from its `void\NexusError` function.

## Try without waiting

```nerd
server.set_option(NetOption.Nonblocking, 1)?
on incoming.recv() {
    NexusError.WouldBlock! => prn("Receive would block")
    error! => return error!
    else => { assert no }
}
```

Nonblocking mode overrides the configured wait: the call attempts to receive
without waiting for data. `WouldBlock` means no complete message is available
yet. In a program doing other work, return to that work before retrying; a tight
retry loop would waste CPU time.

## Retry when data can arrive

The example next connects a client and sends `"hello after retry"`. It then
restores waiting and retries the same receive:

```nerd
server.set_option(NetOption.Nonblocking, 0)?
server.set_option(NetOption.RecvTimeoutMs, 2000)?
incoming.recv()?
text := incoming.read_string()?
prn($"Received: {text}")
```

The two earlier failures did not require recreating the socket or message.
Nexus retains partial TCP receive progress across timeouts and would-block
results; retrying lets it finish the message. This example sends before retrying
and permits a bounded wait for delivery, so it does not depend on an immediate
receive succeeding.

Keep each socket and message at a stable address. The example uses `defer` to
call each message's `done()` before its socket's `close()`. The string returned
by `read_string()` borrows message storage and is printed before that storage
is reused or released.
