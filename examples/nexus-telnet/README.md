# nexus-telnet

Run `just run-example nexus-telnet` from the repository root.

A Telnet client sends a raw text line. The server receives the decoded line,
clears and reuses its message to reply, then closes. The client receives the
complete buffered reply. Both endpoints run locally with bounded waits.

```text
Nexus Telnet: echo: hello terminal
```

See the [Telnet tutorial](../../docs/tutorials/nexus-telnet.md) for character
mode, negotiation, terminal sizes and ownership concepts. The automated native
runner also uses scripted raw peers, so no installed Telnet client is needed.
