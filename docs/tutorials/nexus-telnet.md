# Telnet with Nexus

Telnet uses the same socket/message API as the [Nexus tutorial](nexus.md).
Create a `net_telnet_socket()`, bind or connect a TCP URL, then send and receive
associated messages. Nexus accepts clients, filters Telnet commands and retains
reply routes. It creates no worker threads.

Run `just run-example nexus-telnet` for a local exchange:

```text
Nexus Telnet: echo: hello terminal
```

The [commented example](../../examples/nexus-telnet/nexus-telnet.n) runs both
endpoints in one thread on an OS-selected loopback port. Real applications
usually run the terminal client and server in separate processes.

## Running a real echo server

The [server example](../../examples/nexus-telnet-server/README.md) stays running
for real terminal clients and echoes each complete line to its original sender:

```sh
just build-example nexus-telnet-server
./_bin/examples/nexus-telnet-server
```

From another terminal, run `telnet 127.0.0.1 2323`, then type a line and press
Enter. Empty lines work too. Several clients can connect, and a partial line
or stalled echo from one does not stall replies to the others. The server prints
`[Client 1] connected`, logs decoded lines as `[Client 1] hello`, and reports
`[Client 1] disconnected` when that connection closes. Numbers increase for each
new connection; echoes contain only the text, without the console prefix.
Control/non-ASCII bytes are escaped as `\xHH` in the log. Echoes contain raw text,
with outgoing literal IAC bytes doubled and CRLF supplied by line mode.

Press Ctrl+C in the server terminal to stop. The example opens a
`std.signal.watch_shutdown()` watcher on its main thread and polls `requested()`
between nonblocking network operations. Fixed message slots keep unfinished
replies stable while other clients make progress; a short sleep avoids busy
waiting. It retries each unfinished send with the same Message. Shutdown closes
the socket before disposing of those messages,
then closes the watcher; all cleanup runs in ordinary code. Running the built
binary directly avoids also interrupting the `just` recipe runner.

The example has a fixed listening address and no command-line options or
self-test mode. Edit its `url` value to change the port. Automated checks use
separate clients to verify connection/disconnection logs, numbering across
reconnects, reset recovery and progress alongside a stalled echo. Shutdown is
checked while idle, with partial input and with a pending echo.

Use `127.0.0.1` when connecting to this IPv4 listener. Some clients try IPv6
`::1` first for `localhost`, reporting a refused attempt before trying IPv4.

## Receiving and replying to a line

Line mode is the default. `recv()` returns one complete line without its newline;
`send()` appends CRLF. An empty message sends an empty line. Append raw text with
`append`, since `append_string` adds a binary length prefix intended for ordinary
Nexus messages.

```nerd
use std.nexus

serve :: fn () -> void\NexusError {
    server := net_telnet_socket()
    defer server.close()
    server.bind("tcp://127.0.0.1:2323")?
    message := Message.create(^server)
    defer message.done()

    for {
        message.recv()?
        prn(message.payload().as(string))
        message.clear()
        message.append("received".as([]u8))?
        message.send()?
    }
}
```

`clear()` keeps the sender's route, so `send()` replies to that client. A slow
partial line from one client does not block a complete line from another.
Keep the socket and message at stable addresses; dispose of messages with
`done()` and sockets with `close()` after their last use. Both examples and tests
use bounded timeouts; the server above intentionally waits indefinitely.

When inspecting a result needs an error message before returning, extraction
shorthand keeps the successful path small:

```nerd
bound := on server.local_url() else? [error] {
    eprn($"Failed to get local URL (code {error.as(i32)})")
}
prn($"Listening on {bound}")
```

Inside a function returning `void\NexusError`, this reports a failure and
forwards the original `NexusError`. On success, `bound` receives the URL. If the
handler should return an application exit code instead, use plain `else` and
an explicit `return 1` inside a function returning `i32`.

## Character mode

Set the mode before binding or connecting:

```nerd
server := net_telnet_socket()
server.set_option(NetOption.TelnetMode, net_telnet_character_mode)?
server.bind("tcp://127.0.0.1:2323")?
```

Each receive returns one decoded byte. CR becomes LF, and the following LF in
CRLF is suppressed. This is a byte API, so a multibyte UTF-8 character takes
several receives. Character sends write the entire payload as raw bytes, without
CRLF; an empty send writes nothing. Use `net_telnet_line_mode` to select line mode.
The receive mode is captured when each connection is created, as in the source
library; a changed socket mode affects subsequent sends and new connections.
Use a fresh session when changing the application's input mode.

## Negotiation and terminal sizes

A Telnet server requests NAWS (Negotiate About Window Size) when it accepts a
client. Character mode also requests ECHO and Suppress Go Ahead. Expected replies
are filtered; unsupported DO/WILL requests receive WONT/DONT. Fragmented commands
and subnegotiations are retained between reads. Escaped IAC (`255, 255`) becomes
one literal byte and completes a character-mode message.

After a client advertises its size, inspect the received message:

```nerd
on message.telnet_bounds() {
    bounds => prn($"terminal: {bounds.width} x {bounds.height}")
    else => prn("terminal size unavailable")
}
```

Bounds are optional and belong to the live sender. They survive `clear()` and
update when that client sends another NAWS report; different clients retain
separate sizes. Ordinary sockets, unreported sizes and expired routes return
nil. Connected clients can also inspect received NAWS after a partial receive;
no complete text message is required. Nexus does not invent a size or advertise one for its own client sockets.

## Completion, limits and errors

A complete final line can be received after the peer closes. EOF discards an
unterminated line and reports `Closed`. A failed receive preserves the caller's
payload and route. Lines are bounded by `max_message_size` (1 MiB); oversized
input reports `TooLarge` and removes that peer. TCP client capacity and timing
options are shared with ordinary Nexus sockets. Timeouts preserve partial input
and sends; retry a pending send with the same stable message.

Telnet rejects UDP URLs with `ProtocolNotSupported`. Setting or querying
`TelnetMode` on an ordinary socket returns that error; invalid mode values return
`Invalid`. All option operations return results, so use `?` or handle the error.

Outgoing bytes are transmitted unchanged, apart from line-mode CRLF, matching
the original implementation. Applications emitting a literal IAC to a terminal
must double it themselves. A failed negotiation write does not discard readable
final input; further sends on that peer fail. See the [API contracts](../stdlib-nexus.md)
and [source parity tests](../nexus-parity.md) for ownership and validation details.
