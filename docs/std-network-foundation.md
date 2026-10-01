# IPv4 socket foundation

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
  retrying or losing progress. Nonblocking connect is deliberately rejected
  until a completion/readiness contract exists. Blocking calls have no implicit
  deadline or cancellation in this slice.
- Linux close consumes the owner even on EINTR; retrying could close a reused
  descriptor. Windows close retains ownership on failure so the caller can
  inspect or retry it. No nondefault linger option is exposed.

## Examples and tests

The self-contained `network-echo` and `network-datagram` examples use tiny local
payloads, dynamically assigned ports and explicit cleanup. They require no
external service, startup sleep, worker or fixed port. Run with:

```sh
just run-example network-echo
just run-example network-datagram
```

The bounded runner checks native SDK/header layouts, byte-order/address round
trips, partial receive counts, TCP EOF and half-close, accepted-socket startup
ownership after all other TCP owners close, would-block versus empty UDP,
truncation consumption, reply routing, refused connection and invalid ownership.
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
not a fallback for missing native LLVM tools. Integration should add this runner
to the common test recipe when the independent branches merge. The existing
example family checks syntax but does not replace these execution tests.

## Remaining M5 work

IPv6, DNS/resolver ownership, readiness with monotonic deadlines, nonblocking
connect completion, receive/both-direction shutdown, socket options (including
reuse and close-on-exec/inheritance policy), cancellation and stress testing
remain followups. There is no Nexus framing, wire-format change, protocol or
scheduler dependency here. Do not declare M5 or cross-platform adoption complete.

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
retain the initial failure and the successful follow-up. The independent thread
branch also changes the OS completion fixture; retain both `socket` and `thread`
when merging the branches later.

Native sources consulted: [Winsock startup](https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-wsastartup),
[WSADATA](https://learn.microsoft.com/en-us/windows/win32/api/winsock/ns-winsock-wsadata),
[Linux receive](https://www.man7.org/linux/man-pages/man2/recvfrom.2.html), and
[Linux send](https://man7.org/linux/man-pages/man2/sendmsg.2.html). ABI assertions
also compile against the installed host headers rather than relying only on
remembered layouts.
