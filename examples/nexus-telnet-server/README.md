# nexus-telnet-server

A real line-oriented Telnet echo server. It listens on loopback port 2323,
accepts clients through Nexus, and sends each complete line back to its sender.

From the repository root:

```sh
just build-example nexus-telnet-server
./_bin/examples/nexus-telnet-server
```

In another terminal:

```sh
telnet 127.0.0.1 2323
```

Type a line and press Enter. The server echoes it, including empty lines. Several
clients can connect (up to 63); each gets its own replies. An unfinished line or
blocked echo from one client does not prevent another client receiving an echo.
The server assigns increasing client numbers, logs connections/disconnections,
and logs each decoded line with its client prefix:

```text
[Client 1] connected
[Client 2] connected
[Client 1] hello
[Client 2] goodbye
[Client 1] disconnected
```

Only the echoed text is sent back; the client prefix is console output. Control
and non-ASCII bytes appear as `\xHH` in logs and retain their original values in
echoes. Client numbers are not reused when a connection slot becomes available.
A reset client is removed while the listener continues accepting connections.
Telnet negotiation is handled by Nexus; a literal IAC byte is doubled on outgoing echoes.

Press Ctrl+C **in the server terminal** to close the listener and all clients,
release message buffers, restore the shutdown watcher, and exit successfully
(with disconnect logs for any remaining clients):

```text
Shutdown requested.
Telnet server stopped.
```

`just run-example nexus-telnet-server` also starts the server. Running the built
binary directly keeps Ctrl+C from also interrupting the `just` recipe runner.
Ctrl+C in a remote terminal client is terminal input, not the server's process
shutdown request; disconnect from the client using its own escape/quit commands.

A different numeric IPv4 TCP URL can be passed to the built binary:

```sh
./_bin/examples/nexus-telnet-server tcp://127.0.0.1:2324
```

For a bounded local demonstration without an installed Telnet client:

```sh
./_bin/examples/nexus-telnet-server --smoke
```

This checks a text line and an empty line on an OS-selected port, prints
`Nexus Telnet echo smoke passed`, and exits. The automated native runner also
checks independent terminal wire bytes, multiple clients, partial input,
client numbering and logs, reset/reconnect recovery, first-attempt connection,
Ctrl+C while idle/connected, and progress alongside a stalled echo send.

The [commented source](nexus-telnet-server.n) explains stable ownership, reply
routes, IAC escaping and the distinction between receiving and retrying a pending
send. Nonblocking operations visit every client's pending reply before receiving
more input, then sleep for 1 ms to avoid spinning while idle. Each iteration also
polls `std.signal.watch_shutdown().requested()`. Fixed message slots give pending
echoes stable addresses. A pending send retains its original message and snapshot
until completion or socket closure. Closing the socket before disposing of its
messages safely cancels pending sends during shutdown.

Decoded and escaped messages are bounded at 1 MiB. A line whose doubled IAC bytes
would exceed that limit causes a reported error and cleanup. Other receive/setup
errors also exit with failure; ordinary client EOF is handled by Nexus. Use the numeric IPv4 address shown above: `localhost` may first try
IPv6 (`::1`), which this IPv4 listener does not serve.

See the [Telnet tutorial](../../docs/tutorials/nexus-telnet.md) and
[shutdown API notes](../../docs/stdlib.md#shutdown-notifications). Native Windows
validation of this example remains pending; the runner uses Ctrl+Break for its
targeted Windows process-group shutdown check.
