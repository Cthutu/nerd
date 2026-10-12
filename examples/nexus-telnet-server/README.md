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
clients can connect; each gets its own replies. An unfinished line from one
client does not prevent another client receiving an echo. Telnet negotiation is
handled by Nexus; a literal IAC byte is doubled on outgoing echoes.

Press Ctrl+C **in the server terminal** to close the listener and all clients,
release message buffers, restore the shutdown watcher, and exit successfully:

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
Ctrl+C while idle/connected, and cancellation of a stalled echo send.

The [commented source](nexus-telnet-server.n) explains stable ownership, reply
routes, IAC escaping and the distinction between receiving and retrying a pending
send. Both network waits use 100 ms deadlines so the main thread can regularly
poll `std.signal.watch_shutdown().requested()`. A pending send retains its original
message and snapshot until completion or socket closure. Closing the socket
before disposing of that message safely cancels it during shutdown.

Decoded and escaped messages are bounded at 1 MiB. A line whose doubled IAC bytes
would exceed that limit causes a reported error and cleanup. Fatal receive/setup
errors also exit with failure; ordinary client EOF is handled by Nexus.

See the [Telnet tutorial](../../docs/tutorials/nexus-telnet.md) and
[shutdown API notes](../../docs/stdlib.md#shutdown-notifications). Native Windows
validation of this example remains pending; the runner uses Ctrl+Break for its
targeted Windows process-group shutdown check.
