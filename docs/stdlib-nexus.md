# Nexus messages and TCP framing

`std.nexus` provides the first M6 layer: reusable message buffers, typed encoding
and one connected TCP stream. It uses `std.network` directly and starts no
threads. Run `just run-example nexus-message` for a local structured echo.
See the [tutorial](tutorials/nexus.md) for ownership and retry patterns.

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

## TCP stream ownership and progress

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
| `Invalid` | Nil/unsuitable socket, disposed or failed stream, nil message, or queue already occupied |
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
`9ae32dd295241bd3889204537dfa476f222e753c`, still the fetched upstream main head
on 11 October 2026: a big-endian u32 TCP payload length, followed by the payload;
unsigned integers and length-prefixed strings use the encoding described above.
There are no wire-format changes and no nanomsg/ZeroMQ interoperability claim.
Intentional API differences are independent message ownership, cursor reads,
atomic failed string reads, borrowed strings lasting through other reads, explicit
single-slot queuing and resumable partial framing.

Linux LLVM and C output pass debug/release contracts, golden-wire C exchange,
and exchange against the unmodified pinned C implementation in both directions,
including typed values and empty frames. Deterministic short send/receive and
would-block injection checks retry without duplication or loss. Native Windows
validation of this slice remains pending.

Run `python3 build/test_nexus.py` (also part of `just test`). Add `--release`
and/or `--cgen` for the other configurations. To repeat the original-source
check, pass `--upstream-dir /path/to/dev`; the runner requires clean core/Nexus
sources at the pinned revision and compiles them directly. All loopback peers
use ephemeral ports, readiness handshakes and process timeouts.

## Source test mapping for this slice

| Pinned source scenario | Nerd coverage |
| --- | --- |
| `message.c::message_append_and_read_primitives` | `tests/nexus/main.n::buffer_contracts`, golden bytes and bidirectional C peer |
| `framing.c::tcp_message_framing_round_trip` | `stream_contracts`, C peer and `nexus-message` example |
| `framing.c::zero_length_messages_are_valid` | Coalesced empty frame and bidirectional empty-frame C exchange |
| `framing.c::send_rejects_messages_larger_than_maximum` | Bounded append rejection and oversized peer-prefix rejection before allocation |
| Connected peer sends nothing / timeout / later arrival | Immediate false, absolute deadline expiry and fragmented-header retry in `stream_contracts`; listener/URL option parity remains pending |

Additional regressions cover partial payload retry, EOF in prefix/body, snapshot
ownership, atomic malformed string reads, borrowed self-append through growth,
maximum-size frame round-trip and deterministic short I/O. The socket constructors,
server routing and option scenarios require the later pipe/protocol layer.

Remaining M6 work: UDP message boundaries, pipes, multiple clients, fair server
selection and safe reply-route metadata. M7 request/reply, retry options, URL
parsing and Telnet follow those foundations. This layer does not complete M6.
