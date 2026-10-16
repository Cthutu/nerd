# IPv4 sockets, readiness and deadlines

`std.network` is the first M5 slice. Importing it needs no consumer linker flags:
`os.socket` selects Winsock (`ws2_32`) on Windows and libc on Linux. This slice
supports the repository's Windows x64 and Linux x64 targets.

## Public contract

- `socket(SocketKind.Tcp)` / `socket(SocketKind.Udp)` creates one blocking IPv4
  `Socket`. Call `close()` exactly once, normally with `defer _ := value.close()`.
  Handles are pointer-sized and opaque. Never copy a live Socket; the language
  does not enforce unique ownership. Passing `^Socket` borrows it for a call.
- There are no hidden threads or mutable initialisation counters. Each Windows
  owner, including an accepted socket, acquires one Winsock startup reference.
  Closing the listener does not invalidate accepted connections. Failed creation
  releases its startup reference. Do not race operations or close on one owner;
  callers must coordinate lifetimes, including across threads.
- `ipv4(a, b, c, d, port)` and `loopback(port)` use host-order fields. Bind port
  zero, then use `local_address()` to obtain the OS-selected port without a
  probe/close/rebind race. `peer_address()` returns the connected peer.
- `bind`, `listen`, `accept`, `connect`, `shutdown_send` and `close` provide the
  initial lifecycle. Accept always returns a blocking owner. Half-close preserves
  reverse traffic. Repeated close or operations on a closed owner return `Invalid`.
- `send` performs one TCP send and reports its actual count. It does not promise
  to send an entire slice. `receive` reports `{count, eof}`; receiving into an
  empty slice is a no-op with `eof: no`, not an EOF probe. Linux sends suppress
  SIGPIPE without changing the application's signal handlers.
- `send_to` / `receive_from` preserve UDP message boundaries and sender addresses.
  Empty datagrams are messages, not EOF. A truncated datagram is consumed and
  returns `MessageTooLarge`; the partial output must be discarded. Winsock returns
  native code 10040. Linux reports the full length using MSG_TRUNC: when that
  exceeds the buffer, the library returns code **0**, explicitly indicating a
  library-detected condition rather than a captured errno.
- Byte buffers are borrowed only for the call. TCP transfers over INT_MAX bytes
  and IPv4 UDP sends over 65507 bytes return `Invalid` before invoking the OS.
- `nonblocking(yes)` makes empty accept/receive and blocked send calls report
  `WouldBlock`; errors retain native codes. `Interrupted` is returned without
  retrying or losing progress. `connect` remains blocking-only; nonblocking TCP
  uses `start_connect` and `finish_connect`. Ordinary I/O calls have no implicit
  deadline or cancellation.
- Linux close consumes the owner even on EINTR; retrying could close a reused
  descriptor. Windows close retains ownership on failure so the caller can
  inspect or retry it. No nondefault linger option is exposed.

## Readiness and deadlines

`PollEntry` borrows a `^Socket`, with `read` and `write` interests. Initialise
unused fields with `...`. `poll(entries[..], deadline = limit)` updates each
entry's `readable`, `writable` and `exceptional` flags and returns the number of
ready **entries**, counting an entry once even when multiple flags are set.
Outputs are cleared on every call, including a failed call. It never accepts or
transfers bytes on the caller's behalf.

The deadline is an optional absolute `std.time.Instant`, normally created with
`now().add(from_ms(2000))`. Omit it to wait indefinitely. Zero ready entries means
the deadline expired; an already expired deadline performs one immediate poll
and can still report readiness. Interrupted native waits and early timeout
returns retry against the same deadline, with fresh descriptor sets and timeout
storage. Nanoseconds are rounded up to native microseconds; OS scheduling can
make a wait overrun its deadline. Native waits longer than one day are split into
bounded waits without changing the deadline.

```nerd
use std.network
use std.time
-- stream is a live, nonblocking Socket.
entries := [PollEntry { socket: ^stream read: yes ... }]
limit := now().add(from_ms(2000))
on poll(entries[..], deadline = limit) {
    count => {
        on count == 0 => prn("Deadline expired")
        -- When entries[0].readable, attempt receive and handle WouldBlock.
    }
    error! => {
        _ := error -- Handle the socket failure here.
    }
}
```

Readiness is a hint. Use nonblocking sockets and handle `WouldBlock` on the
subsequent operation. Readability can mean TCP EOF or reset, and UDP readability
includes empty datagrams. Writability does not promise room for an entire slice.
`exceptional` covers native exceptional conditions, including urgent data and
Windows connect failure; it does not itself mean EOF or supply an error code.
Socket operations and connection completion retrieve the actual errors.

One call accepts 1–64 distinct live sockets, with at least one read/write interest
per entry. Empty lists, nil/closed owners, duplicate native handles, missing
interests and oversized lists return `NetworkError.Invalid`. Linux additionally
rejects descriptors numbered 1024 or higher because this first implementation
uses native `select` descriptor sets; Windows has no equivalent numeric handle
restriction. The bound is exposed as `max_poll_sockets`. There is no per-call
allocation or hidden worker. All borrowed sockets, entries and their native
handles must remain valid and unchanged through the call; do not concurrently
close or mutate them. Waiting on a slice does not transfer ownership.

A higher-level operation must check its absolute deadline between partial
transfers even when sockets remain continuously ready. Reuse that deadline on
every retry; creating a fresh deadline after each byte would allow an operation
to run indefinitely. `NetworkError.TimedOut { code: 0 }` represents a
caller-detected deadline expiry; native timeout failures retain their OS code.
The [bounded TCP example](../examples/network-readiness/network-readiness.n)
shows these loops.

## Nonblocking TCP connection establishment

For a fresh TCP socket, enable `nonblocking(yes)`, then call
`start_connect(address) -> bool\NetworkError`. True means immediate success;
false means a pending attempt. `finish_connect(deadline = limit)` waits for
writability or exceptional readiness and reads `SO_ERROR` to determine whether
the attempt succeeded or failed. True means connected; false means deadline
expiry, leaving the attempt pending for a later call. Omit the deadline to wait
indefinitely. Polling a connecting socket yourself never completes its library
state: call `finish_connect`, optionally with an immediate deadline after polling.

`Socket.connection` records `Idle`, `Pending`, `Connected` or `Failed`; callers
must not mutate it. Repeated `start_connect`, completion on an idle/failed/closed
socket, and disabling nonblocking mode while pending return `Invalid`.
Completion on a connected socket returns true repeatedly without consuming
`SO_ERROR` again. Accepted TCP sockets start connected and blocking, as before.
Both failed attempts and abandoned pending attempts require explicit close;
create a new owner to try another connection. Timeout does not cancel or close
the native socket. Ownership and Winsock references remain with the caller.

## Examples and tests

The self-contained `network-echo` and `network-datagram` examples use tiny local
payloads, dynamically assigned ports and explicit cleanup. They require no
external service, startup sleep, worker or fixed port. Run with:

```sh
just run-example network-echo
just run-example network-datagram
just run-example network-readiness
```

The bounded runner checks native SDK/header layouts, byte-order/address round
trips, partial receive counts, TCP EOF and half-close, accepted-socket startup
ownership after all other TCP owners close, would-block versus empty UDP,
truncation consumption, reply routing, refused connection and invalid ownership.
The readiness contracts additionally check multiple ready sockets, stale-output
clearing, immediate/finite deadlines, empty datagrams, partial stream reads, EOF,
connection completion/refusal, invalid entries and native descriptor-set bounds.
Linux injects interrupted waits and early native timeout returns to verify that
remaining time decreases, and a connect timeout to verify pending-state retry.
The runner repeats readiness contracts and the bounded example in every mode.
Every executable has a 20-second process deadline; compilation has 60 seconds.
A timeout terminates the process tree. It always overrides NERD_LIB_PATH with
this checkout's modules and never silently uses an installed standard library.

```sh
python build/build.py nerd --skip-mod-sync
python build/test_network.py
python build/test_network.py --release
python build/test_network.py --cgen
python build/test_network.py --cgen --release
```

`--release` optimises the example/test programs; `--compiler PATH` selects a
separately built compiler. `--cgen` explicitly tests C output with Clang; it is
not a fallback for missing native LLVM tools. Both `just test` and
`just test-release` execute this runner on the integrated branch. The existing
example family checks syntax but does not replace these execution tests.

## Remaining M5 work

IPv6, DNS/resolver ownership, receive/both-direction shutdown, socket options
(including reuse and close-on-exec/inheritance policy), cancellation and stress testing
remain followups. There is no Nexus framing, wire-format change, protocol or
scheduler dependency here. Do not declare M5 or cross-platform adoption complete.

## Native Windows automated validation, 2026-10-16

Implementation `05c6e023` passes all eight network compiler/backend/target
combinations, including native layout, readiness, connection completion,
deadlines and examples. Both compiler fixture suites and C differential gates
also pass. See the [full results](../validation/windows/results/20261016T083710Z-8b918d2c/README.md). Linux-only
interruption injection remains explicitly skipped on Windows. Interactive
Windows observations and wider M5 extensions/adoption gates remain open.

The following October 1 section records the earlier validation boundary.

## Validation, 2026-10-01

Fresh Clang-built Windows compiler: direct LLVM and explicit generated-C checks
passed with both unoptimised and optimised programs. All three programs and the
native C layout assertions passed in each mode.

Arch Linux under WSL: a fresh native compiler built with Clang 22.1.6; Linux
semantic checks and generated-C execution passed at O0/O2, including Linux
MSG_TRUNC and SIGPIPE-suppression declarations. The compiler lives in an isolated
user cache checkout; no global installation changed. Direct LLVM execution is
still unverified there because opt/llc are absent. This is WSL evidence, not a
standalone Linux host adoption gate.

The initial full `just test` fixture run had 1175 passes, 15 platform skips and
one stale LSP completion expectation: `os.socket` was missing from the expected
module list. After updating that fixture, `just test --filter
117-os-module-completion` passed the corrected test and every auxiliary check,
including 295 C-output differential fixtures at two optimisation levels (two
platform skips), build settings and all four Windows stdio modes. The other
1175 fixture results were not redundantly rerun. Both `just run-example`
commands also passed using the freshly built release compiler; the focused
socket suite passed with that compiler and optimised programs.

[Raw Windows gate logs](../validation/windows/results/20261001-std-network/README.md)
retain the initial failure and the successful follow-up. The integrated OS
completion fixture contains both `socket` and `thread`. See the latest
[handoff](../validation/windows/results/HANDOFF.md) for combined results.

Readiness sources: [Winsock select](https://learn.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-select),
[Winsock getsockopt](https://learn.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-getsockopt),
[Linux select](https://man7.org/linux/man-pages/man2/select.2.html) and
[Linux connect](https://man7.org/linux/man-pages/man2/connect.2.html). The new
readiness/connect slice passes native Linux LLVM and C output in debug/release;
its native Windows execution is still pending. The Windows results below describe
the earlier IPv4 socket slice.

Native sources consulted: [Winsock startup](https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-wsastartup),
[WSADATA](https://learn.microsoft.com/en-us/windows/win32/api/winsock/ns-winsock-wsadata),
[Linux receive](https://www.man7.org/linux/man-pages/man2/recvfrom.2.html), and
[Linux send](https://man7.org/linux/man-pages/man2/sendmsg.2.html). ABI assertions
also compile against the installed host headers rather than relying only on
remembered layouts.
