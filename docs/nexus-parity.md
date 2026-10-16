# Nexus source-test parity

The inventory is pinned to `cthutu/dev` revision
`9ae32dd295241bd3889204537dfa476f222e753c`. All 33 message/framing/request-reply
and Telnet scenarios have equivalent Nerd coverage.
Covered means the observable contract is checked, rather than a literal copy of
the original C test. Native Windows automated coverage now passes under both
compiler builds, LLVM/C backends and debug/release targets; see the
[16 October results](../validation/windows/results/20261016T083710Z-8b918d2c/README.md). Human desktop/Ctrl+C
observations and long-duration resource measurements remain separate gates.

The machine-readable [mapping](../tests/nexus/parity-map.json) is checked by
`build/test_nexus.py`: referenced files/functions must exist, entries must be
unique, and each status must agree with its coverage. Passing `--upstream-dir`
also compares the complete inventory against `TEST_CASE` declarations in the
clean pinned source. This mapping records equivalents; it does not claim that
the original C test suite was executed.

| Original test | Status | Nerd coverage |
| --- | --- | --- |
| `framing.c::tcp_message_framing_round_trip` | Covered | [main.n::stream_contracts](../tests/nexus/main.n), [interop.n::exchange](../tests/nexus/interop.n) |
| `framing.c::default_socket_constructor_works_for_message_sockets` | Covered | [socket-api.n::tcp_routes](../tests/nexus/socket-api.n) |
| `framing.c::zero_length_messages_are_valid` | Covered | [main.n::stream_contracts](../tests/nexus/main.n), [socket-api.n::udp_routes](../tests/nexus/socket-api.n) |
| `framing.c::send_rejects_messages_larger_than_maximum` | Covered | [main.n::buffer_contracts](../tests/nexus/main.n), [main.n::stream_contracts](../tests/nexus/main.n) |
| `message.c::message_append_and_read_primitives` | Covered | [main.n::buffer_contracts](../tests/nexus/main.n) |
| `message.c::recv_msg_and_send_msg_round_trip` | Covered | [interop.n::exchange](../tests/nexus/interop.n), [parity.n::metadata](../tests/nexus/parity.n) |
| `message.c::udp_recv_msg_preserves_reply_route_for_send_msg` | Covered | [socket-api.n::udp_routes](../tests/nexus/socket-api.n), [parity.n::metadata](../tests/nexus/parity.n) |
| `message.c::tcp_server_can_reply_to_multiple_clients_via_message_pipe` | Covered | [socket-api.n::tcp_routes](../tests/nexus/socket-api.n) |
| `message.c::socket_options_can_be_set_and_read_back` | Covered | [parity.n::options](../tests/nexus/parity.n) |
| `message.c::connect_can_retry_until_server_binds` | Covered | [test_nexus.py::delayed_connect](../build/test_nexus.py), [timing-driver.n::retry_connect](../tests/nexus/timing-driver.n) |
| `message.c::connect_times_out_when_server_never_binds` | Covered | [socket-api.n::connect_failures](../tests/nexus/socket-api.n) |
| `message.c::recv_times_out_when_no_client_arrives` | Covered | [parity.n::idle_receive](../tests/nexus/parity.n) |
| `message.c::recv_times_out_when_client_connects_but_sends_nothing` | Covered | [parity.n::idle_receive](../tests/nexus/parity.n) |
| `message.c::recv_succeeds_when_message_arrives_before_timeout` | Covered | [test_nexus.py::delayed_receive](../build/test_nexus.py), [timing-driver.n::arrival](../tests/nexus/timing-driver.n) |
| `message.c::nonblocking_connect_returns_would_block_when_server_is_absent` | Covered | [socket-api.n::connect_failures](../tests/nexus/socket-api.n) |
| `message.c::nonblocking_recv_returns_would_block_when_no_client_arrives` | Covered | [parity.n::idle_receive](../tests/nexus/parity.n) |
| `message.c::nonblocking_recv_returns_would_block_when_client_sends_nothing` | Covered | [parity.n::idle_receive](../tests/nexus/parity.n) |
| `message.c::nonblocking_udp_recv_returns_would_block_without_datagram` | Covered | [parity.n::idle_receive](../tests/nexus/parity.n) |
| `reqrep.c::request_reply_round_trip_over_tcp` | Covered | [socket-api.n::ordering_and_timeouts](../tests/nexus/socket-api.n) |
| `reqrep.c::request_socket_cannot_receive_before_sending` | Covered | [socket-api.n::ordering_and_timeouts](../tests/nexus/socket-api.n) |
| `reqrep.c::request_socket_cannot_send_twice_without_reply` | Covered | [socket-api.n::ordering_and_timeouts](../tests/nexus/socket-api.n) |
| `reqrep.c::reply_socket_cannot_send_before_receiving` | Covered | [socket-api.n::ordering_and_timeouts](../tests/nexus/socket-api.n) |
| `telnet.c::telnet_socket_round_trip_over_tcp` | Covered | [telnet.n::round_trip](../tests/nexus/telnet.n) |
| `telnet.c::telnet_socket_round_trip_survives_immediate_close_repeatedly` | Covered | [telnet.n::round_trip](../tests/nexus/telnet.n) |
| `telnet.c::telnet_socket_reports_closed_after_final_reply` | Covered | [telnet.n::round_trip](../tests/nexus/telnet.n) |
| `telnet.c::telnet_socket_sends_crlf_terminated_lines` | Covered | [test_nexus.py::telnet_wire](../build/test_nexus.py) |
| `telnet.c::telnet_character_mode_receives_one_character_per_message` | Covered | [test_nexus.py::telnet_wire](../build/test_nexus.py) |
| `telnet.c::telnet_character_mode_sends_raw_bytes_without_crlf` | Covered | [test_nexus.py::telnet_wire](../build/test_nexus.py) |
| `telnet.c::telnet_socket_ignores_telnet_negotiation_bytes` | Covered | [test_nexus.py::telnet_wire](../build/test_nexus.py) |
| `telnet.c::telnet_socket_reports_negotiated_bounds_from_naws` | Covered | [test_nexus.py::telnet_wire](../build/test_nexus.py) |
| `telnet.c::telnet_socket_rejects_udp_urls` | Covered | [telnet.n::options](../tests/nexus/telnet.n) |
| `telnet.c::telnet_bounds_are_unavailable_on_non_telnet_sockets` | Covered | [telnet.n::options](../tests/nexus/telnet.n) |
| `telnet.c::telnet_mode_option_round_trips_and_rejects_non_telnet_sockets` | Covered | [telnet.n::options](../tests/nexus/telnet.n) |

The oversized-send source case mutates private message fields. Nerd callers
must not do that: checked appends reject oversized payloads atomically, and the
receive parser rejects an oversized peer prefix before allocating its body.
Those intentional API differences are covered by buffer/framing contracts.

Additional regressions cover 63-client saturation and queued admission after a
disconnect; stale routes and owner-session generations; reconnecting basic and
request/reply clients; malformed peers alongside healthy clients; replies
followed immediately by EOF; UDP sender capacity; atomic failed reads; and
forced short/would-block I/O. Timing injection verifies refused connects are
retried and TCP/UDP deadlines expire even when readiness stays asserted while
every I/O attempt would block. See the [contracts](stdlib-nexus.md) for limits.

Telnet regressions also cover fragmented IAC/SB commands, escaped IAC in text
and NAWS, coalesced lines, empty lines and character CRLF normalisation.
Additional tests check per-peer bounds/routes, partial-line timeout recovery,
1 MiB input limits, and readable final input after failed control writes.
Escaped IAC immediately completes a character message, correcting the source
parser's delayed delivery. Original-C interoperability checks both endpoint roles.
The [bounded stress runner](../tests/nexus/README.md) adds repeated FIN/reset
churn, concurrent TCP/Telnet streams, real slow-reader backpressure, exact send
recovery and shutdown with partial input or pending output.
Remaining validation: interactive Windows observations, long-duration soak and
resource-growth measurements. Native Windows automated contracts and bounded
stress pass; Linux-only fault injection remains explicitly skipped there.
DNS and bracketed IPv6 URLs are planned extensions beyond the inspected source.
The focused [timeout/nonblocking walkthrough](tutorials/nexus-timeouts.md)
and `nexus-timeouts` example cover bounded receive, immediate would-block,
and recovery; the native runner checks their output.
