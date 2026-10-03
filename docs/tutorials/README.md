# Nerd API tutorials

The requested tutorial set covers all three remade APIs. Each tutorial must use
Nerd code that builds and runs in the repository, explain ownership and cleanup,
and have its observable result checked by the native test recipes. This is a
completion requirement for the remakes, selected on 3 October 2026.

| API | Tutorial status | Required walkthroughs |
| --- | --- | --- |
| Raptor | [First tutorial available](raptor.md) | Queue-owned typed submission, task handle cleanup, serial/concurrent queues, nested waits and drain; async aggregation when implemented |
| Kerberos | Pending graph API (M4) | Two generators → sum → sink checking 0,2,…,18; typed ports, connections, fan-out, graph/node ownership, errors and shutdown; loader after implementation |
| Nexus | Pending message API (M6/M7) | Framed message echo, multiple clients and reply routes, request/reply with deadlines and reconnect, UDP boundaries and separate Telnet tutorial |

[Raw TCP/UDP examples](../std-network-foundation.md) are available as foundations;
they do not establish Nexus message/protocol compatibility. Kerberos's scheduler
usage will build on `std.raptor`; graph dataflow needs its own API and contracts.
The [implementation plan](../stdlib-expansion-plan.md) tracks the remaining work.
