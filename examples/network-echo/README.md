# Raw TCP echo

```sh
just run-example network-echo
```

Expected output: `TCP echo: hello sockets`.

This self-contained demonstration starts both endpoints in one process on IPv4
loopback. The OS chooses the server port with bind(0); there is no fixed port,
external service or startup sleep. The tiny payload fits in the socket buffers,
allowing a single thread to show both roles. It completes and closes every owned
socket. A production concurrent service needs the future readiness API or
explicit I/O threads; these examples are not general event loops.

Inspired by the transport scenario in `cthutu/dev` revision
`9ae32dd295241bd3889204537dfa476f222e753c`, `tests/nexus/framing.c::tcp_message_framing_round_trip`.
This example uses raw socket bytes and does not implement Nexus framing.

Sockets are unique owners by convention: do not copy them; pass pointers to
helpers, borrow byte buffers for each call and close on every exit. The TCP
example loops on partial counts and distinguishes EOF. The UDP example replies
to the sender address returned with its message.

`python build/test_network.py` executes this example with a process deadline.
See [the foundation contract](../../docs/std-network-foundation.md) for limits,
validation and followups.
