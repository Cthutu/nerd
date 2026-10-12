# Nexus: simple sockets and messages

Nexus follows the original `cthutu/dev` API: create a socket, connect or bind a
URL, create a message associated with that socket, then send or receive it.
Nexus manages TCP connections, message framing and server reply routing.

## A basic client

With a server listening on port 8080:

```nerd
use std.nexus

send_greeting :: fn () -> void\NexusError {
    socket := net_socket()
    defer socket.close()
    socket.connect("tcp://127.0.0.1:8080")?

    message := Message.create(^socket)
    defer message.done()
    message.append_string("hello")?
    message.append_u32(42)?
    message.send()?
}
```

`send()` sends one complete message. For TCP, Nexus adds its length prefix and
handles partial writes internally. For UDP, one message becomes one datagram.
The `?` propagates failures to the caller; the defers release resources on both
success and failure. Keep socket and message owners stable and do not copy live
allocated values.

## A basic server and reply

```nerd
use std.nexus

serve_greeting :: fn () -> void\NexusError {
    socket := net_socket()
    defer socket.close()
    socket.bind("tcp://127.0.0.1:8080")?

    message := Message.create(^socket)
    defer message.done()
    message.recv()?
    text := message.read_string()?
    number := message.read_u32()?
    prn($"Received {text} and {number}")

    message.clear()
    message.append_string("reply")?
    message.append_u32(number + 1)?
    message.send()?
}
```

`bind()` includes TCP listening. `recv()` accepts clients internally and returns
one complete message. A server can receive from several clients, and each
received message remembers its originating peer. `clear()` removes the payload
while preserving that reply route and reusable storage. Sending the same
message replies to that peer, even if other clients have since sent messages.
Use `message.id()`, `message.address()` or `message.url()` to inspect the sender.
They return optional values when no live route is available.

Typed reads consume bytes through a cursor. Read strings borrow message storage;
use or copy them before clearing, appending, receiving again or disposing of the
message. The socket must remain in stable storage while associated messages
exist. `close()` invalidates their association, including after reopening the
same socket owner; create fresh messages for the new session.

## Request/reply and UDP

Use `net_request_socket()` for a client that must send then receive, and
`net_reply_socket()` for a server that must receive then send. The message API
stays the same. Breaking the order returns `NexusError.WrongState`; failed or
timed-out operations leave the turn unchanged. Basic `net_socket()` imposes no
turn-taking rule.

Changing a URL to `udp://127.0.0.1:8080` selects datagrams. The same message
methods and automatic server replies work, including empty datagrams. UDP
payloads are limited to 65,507 bytes; delivery and ordering are not guaranteed.
TCP and UDP use identical typed field encodings.

## Timeouts and immediate operations

Normal send and receive calls wait indefinitely. Connect defaults to a
one-second total budget, retrying a refused/reset connection while waiting for
a server. Configure options once on the socket:

```nerd
socket.set_option(NetOption.ConnectTimeoutMs, 2000)
socket.set_option(NetOption.ReconnectIntervalMs, 25)
socket.set_option(NetOption.SendTimeoutMs, 2000)
socket.set_option(NetOption.RecvTimeoutMs, 500)
```

A bounded operation reports `NexusError.TimedOut`. Set
`NetOption.Nonblocking` to `1` for immediate operations, which report
`NexusError.WouldBlock` when they cannot finish immediately; set it to `0` to
restore waiting. `net_wait_immediate` and `net_wait_infinite` are accepted for
timeout options. A zero reconnect interval uses the source's 50 ms fallback.

A TCP timeout or would-block result retains partial framing. Retry `send()` on
the same stable message to resume the original snapshot, or retry `recv()` to
finish receiving. A failed receive leaves its previous payload and route intact.
Do not replace or dispose of a pending send message before completion; close the
socket to cancel it. A fatal connection/frame error requires closing that
connection; a server can continue servicing its other clients.

## Runnable examples

All examples use an OS-selected loopback port and run both endpoints in one
thread. Per-operation timeouts bound their waits, and no external service is
required:

- `just run-example nexus-message`: exchange a string and number; prints
  `Nexus message: hello nexus, reply 43`.
- `just run-example nexus-multi-client`: reply to two clients through the received
  messages; prints `Nexus multi-client: replies 11 and 21`.
- `just run-example nexus-request-reply`: repeat an exchange and demonstrate a
  wrong-state error; prints `Nexus request/reply: two exchanges passed`.

The [API contracts](../stdlib-nexus.md) cover ownership, limits and source
compatibility. `TcpStream` remains available for direct framing/event-loop use;
the basic socket/message API manages that state for ordinary programs. Telnet
and DNS/IPv6 URLs remain follow-on work.
