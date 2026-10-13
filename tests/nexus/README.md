# Nexus validation

Run the native contract, interoperability and bounded stress checks with:

```sh
python3 build/test_nexus.py --compiler _bin/nerd-debug
```

`--release` selects optimised Nerd output; `--cgen` compiles the generated C
with Clang. `just test` runs all four combinations. These runs validate the
host they execute on; Linux results do not establish native Windows results.

## Bounded stress checks

The runner builds `stress.n`, a framed TCP echo driver, and the existing Telnet
server example. Only the example's port is changed in a temporary copy, allowing
independent test runs to use OS-selected ports.

| Scenario | Default workload | Checked outcome |
| --- | --- | --- |
| TCP and Telnet traffic | Each uses 12 rounds of eight concurrent clients, each sending 12 messages | Exact bytes, order and routes for 1,152 replies per protocol; split prefixes and coalesced messages |
| Connection churn | 96 completed connections per protocol; alternate FIN and reset | Peer slots reusable beyond the 63-slot capacity; monotonically assigned IDs; exactly one connect/disconnect callback per client |
| Partial-input shutdown | Four additional live peers per protocol | Shutdown notification exits cleanly, closes peers and delivers disconnect callbacks |
| Slow-reader recovery | Eight distinct 512 KiB TCP frames; 20 healthy-peer probes | An observed `WouldBlock`, healthy-peer progress before the slow peer drains, then exact replay-free recovery and a fresh exchange |
| Pending-output shutdown | A second slow-reader burst | Cancellation after a newly observed blocked send; clean process exit and completed writer cleanup |

Backpressure uses a real socket with a small receive window. The test must
observe the driver's blocked-send notification; it cannot pass merely because
a buffer was configured. The framed driver queues replies in receipt order per
peer because Nexus allows only one pending outgoing snapshot per peer. This is
test orchestration, not an additional public API or example requirement.

Every socket operation and wait is bounded. Each server also has a 60-second
watchdog (extended churn runs allow two seconds per round, with a 60-second
minimum). Console diagnostics retain a bounded tail while lifecycle counters
are bounded by the selected round count. Failure cleanup closes clients, kills
any remaining server and joins reader/writer threads. Unexpected driver output,
including runtime leak diagnostics, fails validation.

For a larger repeatable run:

```sh
python3 build/test_nexus.py --compiler _bin/nerd-debug --stress-rounds 128
```

This performs 1,024 completed connections and 12,288 checked replies for each
protocol. The option accepts 12–1,000 rounds. These are bounded stress checks;
long-duration soak, resource-growth measurements, WAN behaviour and native
platform validation remain separate work.
