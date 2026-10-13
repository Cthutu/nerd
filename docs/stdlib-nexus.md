# Nexus messages, TCP framing and Telnet

`std.nexus` follows the original socket-associated message API. Run
`just run-example nexus-message`, `nexus-multi-client`, `nexus-request-reply`, `nexus-telnet` or
`nexus-telnet-server`. The last is a real echo server on loopback port 2323 with Ctrl+C shutdown.
See the [tutorial](tutorials/nexus.md) for simple client/server programs.

## Socket and message API

| Source Nexus | Nerd |
| --- | --- |
| `net_socket()` | `net_socket()` |
| `net_request_socket()` / `net_reply_socket()` | Same constructors |
| `net_connect(&socket, url)` | `socket.connect(url)?` |
| `net_bind(&socket, url)` | `socket.bind(url)?` |
| `net_message_create(&socket)` | `Message.create(^socket)` |
| `net_send(&message)` / `net_recv(&message)` | `message.send()?` / `message.recv()?` |
| `net_message_clear/done` | `message.clear()` / `message.done()` |
| `net_message_read_*` / `net_message_append_*` | `message.read_*()?` / `message.append_*()?` |
| `net_message_id/url` | `message.id()` / `message.url()` |
| `net_set_option/get_option` | `socket.set_option(...)?` / `socket.get_option(...)?` |
| `net_telnet_socket()` | `net_telnet_socket()` |
| `net_telnet_bounds` | `message.telnet_bounds()` returns optional `TelnetBounds` |
| `net_close(&socket)` | `socket.close()` |

`NetSocket` initially owns no native resources. `connect` and `bind` accept
numeric IPv4 `tcp://a.b.c.d:port` and `udp://a.b.c.d:port` URLs. Bind includes TCP
listen; receive accepts connections and selects complete messages fairly across
ready clients. A partial sender cannot block other clients. `local_url()` and
`local_address()` expose the actual port after binding port zero.

Sockets own their TCP peers and framing buffers. Keep owners stable, do not copy
live values, mutate implementation fields or race operations. `close()` is
idempotent, releases all resources and invalidates associated messages using a
session generation. Keep the socket owner storage alive through message disposal.
A reopened owner needs newly created messages. Peer identities are monotonic;
a stale received message cannot accidentally reply to a replacement TCP client.
`clear()` preserves association and reply metadata; `done()` removes both.
Standalone zero-initialised messages still work as payload buffers.

A received message carries the sender ID and address. Its `send()` replies to
that peer; a connected client's send uses its configured server. Successful
receive replaces payload and route atomically. Failed receives leave both
unchanged. `id`, `address` and `url` return nil without a live route. Sender URL
views borrow the message's fixed storage until its next URL query or disposal.
The socket's local URL view similarly lasts until its next local URL query or
close. Normal payload borrowing rules remain separate.

TCP supports at most 63 simultaneous clients per server, leaving one of the 64
readiness slots for accepting. Further connections remain in the OS backlog
until a slot opens. Each peer retains at most two 1 MiB framing buffers. UDP
retains at most 63 distinct sender identities per socket session and one reusable
receive allocation. Additional UDP senders report `TooManyPeers` after consuming
their datagram, leaving output unchanged. UDP payloads are capped at 65,507 bytes;
the limit is checked before sending. Empty datagrams are valid. UDP reply routing
uses the originating address; it cannot detect a different process reusing that
address. No unbounded application queue or hidden worker threads are created.

Request sockets start in the send phase; reply sockets start in the receive
phase. A complete successful operation changes the turn. `WrongState` detects
incorrect ordering before I/O. Errors and timeouts do not change the turn.
Basic and Telnet sockets permit either direction independently.

Options are `Nonblocking`, `ConnectTimeoutMs`, `ReconnectIntervalMs`,
`SendTimeoutMs`, `RecvTimeoutMs` and `TelnetMode`. Set/get return results;
`TelnetMode` rejects ordinary sockets and values other than the two mode constants. Send/receive default to `net_wait_infinite`;
connect defaults to 1000 ms, with a 25 ms reconnect interval. A zero reconnect
interval chooses the source's 50 ms fallback. TCP retries refused/reset/timed-out
or interrupted connects, using fresh native sockets within one absolute budget;
other failures retain `NetworkError` detail. A nonblocking connect attempts once.
Nonblocking send/receive report `WouldBlock`; bounded waits report `TimedOut`.
Each waiting operation computes one absolute deadline, preserving it across
partial I/O and reconnect attempts. Very large timing values are saturated
before nanosecond conversion. Retry sleeps are capped at one second.

A TCP send snapshots its unread payload internally. After timeout/would-block,
retry on the same stable message to continue the snapshot. Sending a different
message while that peer's frame is pending reports `Invalid`. Do not recreate or
dispose of a pending send message before completion; close its socket to cancel.
Fatal framing/I/O errors remove the offending TCP peer. Servers retain other
clients; disconnected client sockets need `close()` before reconnecting.
Available I/O is attempted before waiting, so deadlines do not guarantee strict
wall-clock pre-emption of immediately available operations.

### Connection notifications

`socket.set_connection_handler(handler)` registers an optional synchronous
`fn (id: u64, connected: bool)` callback. TCP connection creation reports `yes`;
removal (EOF, fatal peer error or socket close) reports `no`. IDs match
`message.id()` and are never reused by the socket owner. UDP has no connection
notifications. Register before bind/connect to observe every connection;
registering later does not replay existing peers. Pass a nil function pointer
to disable the handler. Registration survives `close()` and reopening.

Callbacks run on the thread performing the socket operation, including during
`recv()` and `close()`. They must not re-enter, close or modify the socket.
No background thread or event queue is created: idle connection/disconnection
notifications require the application to keep receiving. The Telnet server
example uses this callback for console logging.

## Message ownership and encoding

A zero-initialised `Message` is ready to use. It owns a geometrically grown byte
allocation, capped at `max_message_size` (1,048,576 bytes). Do not copy an allocated
message. `clear()` resets payload and cursor while retaining capacity; `done()`
frees storage and resets the value. Empty and already-disposed values accept
`done()`. Heap allocation failure follows `std.memory` and terminates the process.
Fields are implementation state; callers must not modify them.

`append([]u8)` copies bytes. `append_u8/u16/u32/u64` write unsigned integers in
big-endian order. `append_string(string)` writes a big-endian u32 byte count,
then the bytes, with no encoding validation. All appends check the total stored
length, including consumed bytes, before mutation. Call `clear()` to reuse a
consumed buffer. `TooLarge` leaves the previous payload unchanged. Appending a
slice or string borrowed from this message is supported, including growth.

`read(count)` and `read_u8/u16/u32/u64/read_string` advance a cursor without
moving bytes. A short read reports `IncompletePayload` and leaves the cursor
unchanged; this includes an incomplete string body after a valid length prefix.
`payload()` returns unread bytes, matching the source API's logically destructive
reads. Raw reads and strings borrow message storage. Views survive other reads
but expire on append, clear, successful receive or done. Copy a view before any
of those operations if it must outlive them.

## Telnet sockets

`net_telnet_socket()` uses raw TCP terminal text and shares the socket/message
ownership, routes, bounded peers and timeout model above. See the
[Telnet tutorial](tutorials/nexus-telnet.md) and commented `nexus-telnet` example.
`TelnetMode` defaults to `net_telnet_line_mode`; set it before bind/connect.
`net_telnet_character_mode` selects one decoded byte per receive and raw sends.
Input mode is captured per connection; later changes affect sends and new peers.
A server receive skips clients with an unfinished send, including pending
negotiation before the text write. Retry those sends with their original stable
messages while continuing to receive/reply for other clients. Nonblocking
operations make this possible in one thread; the echo server demonstrates
bounded caller-owned message slots.

Line sends append CRLF, and line receives discard CR and finish on LF. Empty
lines are valid. Character receives map CR to LF and suppress the following LF.
IAC commands are filtered, doubled IAC yields a literal byte, and NAWS retains
big-endian width/height per peer. Accepted peers receive DO NAWS; character mode
also requests WILL ECHO, WILL SGA and DO SGA. Expected options are ignored;
unsupported DO/WILL receive WONT/DONT. DONT/WONT require no response. Client
connect does not initiate session negotiation. Subnegotiation storage is fixed
at eight bytes, and unread wire input at 1024 bytes per peer; decoded lines and
outgoing snapshots are independently bounded at 1 MiB. Control output drains
between input chunks and never interrupts a pending application send. If a
pending application send prevents draining, queued control replies are also
capped at 1 MiB.

`message.telnet_bounds()` returns `?TelnetBounds` for the live sender after NAWS,
nil otherwise, including on non-Telnet sockets. Connected clients can inspect
negotiated bounds even before a complete text message arrives. Clear preserves the route and
size lookup. Receive timeouts retain partial lines/commands and leave output
unchanged. Complete messages precede EOF; incomplete final lines report `Closed`.
Oversized decoded input removes the offending peer. A failed control write
prevents further sends but leaves final readable input available. These writes
use the enclosing send/receive deadline and do not create a hidden worker.

Use `append(text.as([]u8))`, not binary `append_string`, for terminal text.
Outgoing IAC is not automatically escaped, matching the source implementation.
Applications must double literal IAC themselves. Unlike the source parser,
escaped IAC immediately completes a character-mode message; bounds and partial
commands remain fully incremental. Telnet accepts TCP URLs only.

## Advanced TCP stream ownership and progress

`TcpStream.new(^socket)` requires a live, connected, nonblocking TCP socket.
Keep that socket at a stable address and open until `stream.done()`, then close
it separately. Do not copy the stream, mutate its fields, race operations on it,
or interleave raw socket I/O with framing. Disposing of a stream releases its
buffers and discards partial frames; it does not close the socket. After
cancelling a partial frame, close the socket rather than creating fresh framing
state on its existing byte stream.

`queue(^message)` copies the unread payload into a single outgoing slot. The
caller may immediately clear, change or dispose of the original message.
Queuing while a frame is pending reports `Invalid`; no replacement or unbounded
queue is allowed. `try_flush()` makes immediate progress and returns false on
would-block or interruption, retaining the exact prefix/payload position.
True means the entire queued frame has reached the OS; it does not acknowledge
remote receipt. Flushing an empty outgoing slot returns true.

`try_receive(^output)` reads exactly one frame, leaving later coalesced frames
in the socket. False means would-block or interruption; partial prefix/body
state is retained and output remains unchanged. True replaces output with a
complete payload and resets its cursor. Zero-length frames are valid. The
receive buffer grows only after validating the complete prefix.

`flush(deadline = limit)` and `receive(^output, deadline = limit)` wait using
`std.network.poll`. Their optional deadline is an absolute `std.time.Instant`;
nil waits indefinitely. They first attempt available I/O, even for an expired
deadline, then return false if further progress requires waiting past the
limit. False preserves progress and permits retry with a new deadline. The
same absolute deadline is used across all partial-I/O retries. For event-loop
integration, use the immediate methods and poll for read/write readiness yourself.
No operation guarantees strict wall-clock pre-emption of immediately available
I/O, allocation or scheduling overhead.

Per stream, internal allocation is bounded by two 1 MiB buffers plus fixed
state. Output messages own their own storage. One pending frame per direction
provides explicit backpressure without hidden workers or unlimited buffering.

## Failures and source compatibility

| Failure | Behaviour |
| --- | --- |
| `Invalid` | Missing/stale socket association, missing reply route, unsuitable/failed stream, or occupied outgoing slot |
| `ProtocolNotSupported` | Telnet UDP URL, or Telnet mode option on an ordinary socket |
| `BadUrl` | Unsupported or malformed numeric IPv4 URL; no socket is opened |
| `WouldBlock` / `TimedOut` | Immediate operation / bounded wait cannot complete; partial progress survives |
| `WrongState` | Request/reply operation violates its current turn; no I/O occurs |
| `TooManyPeers` | A UDP sender exceeds the bounded session identity table; output is unchanged |
| `TooLarge` | Local append exceeds the limit, or peer frame length exceeds it |
| `IncompletePayload` | A message read lacks its requested bytes; cursor is unchanged |
| `Closed` | Orderly EOF between frames, or an unsuccessful zero-byte nonempty send |
| `Truncated` | EOF within a prefix or payload |
| `Network { error }` | Original `NetworkError`, retaining its native code |

Oversized peer lengths, EOF and fatal I/O errors make the stream terminal:
subsequent I/O reports `Invalid`. Dispose of it and close its socket. Output
remains unchanged on receive failure. Readiness failures preserve parser state.
This reference treats EOF as terminal for both stream directions.

Wire compatibility is with `cthutu/dev` revision
`9ae32dd295241bd3889204537dfa476f222e753c`, the inspected source revision: a big-endian u32 TCP payload length, followed by the payload;
unsigned integers and length-prefixed strings use the encoding described above.
There are no wire-format changes and no nanomsg/ZeroMQ interoperability claim.
Intentional API differences are independent message ownership, cursor reads,
atomic failed string reads, borrowed strings lasting through other reads, explicit
single-slot resumable framing, checked appends and bounded peer tables. The
simple API retains the original socket/message association and automatic replies;
Nerd uses methods and propagated result errors in place of C output parameters.

Linux LLVM and C output pass debug/release contracts, golden-wire C exchange,
and TCP/UDP/Telnet exchanges against the unmodified pinned C implementation in both directions,
including typed values, empty frames and empty datagrams. Deterministic short send/receive and
would-block injection checks retry without duplication or loss. Native Windows
validation of this slice remains pending.

Run `python3 build/test_nexus.py` (also part of `just test`). Add `--release`
and/or `--cgen` for the other configurations. To repeat the original-source
check, pass `--upstream-dir /path/to/dev`; the runner requires clean core/Nexus
sources at the pinned revision and compiles them directly. All loopback peers
use ephemeral ports, readiness handshakes and process timeouts.

## Source test parity

The [complete parity inventory](nexus-parity.md) maps all 33 pinned scenarios:
all 22 core message/framing/request-reply and 11 Telnet scenarios have equivalent
Nerd coverage. The runner validates every coverage reference
and, with `--upstream-dir`, compares the inventory against the original test files.

Regressions also cover all 63 TCP peer slots and admitting a queued client after
one disconnects; stale reply routes; repeated basic/request-reply reconnects;
metadata surviving reads/clear; incomplete/malformed senders alongside healthy
clients; receiving a final reply before EOF; UDP peer capacity; and delayed
connect/receive success. Deterministic native injection forces refused connect
attempts and persistent would-block despite read/write readiness. TCP/UDP receive
and UDP send check their original deadline after incomplete I/O, preventing
readiness from extending a timeout indefinitely. Immediate calls preserve partial
progress and return WouldBlock after an incomplete attempt.

Remaining feature work: DNS/bracketed IPv6 URLs. Native Windows
and broader stress/soak validation remain pending. The focused
[timeout/nonblocking walkthrough](tutorials/nexus-timeouts.md) demonstrates
receive recovery with an example checked by the native runner. M6/M7 milestone
closure still requires their agreed checks.
