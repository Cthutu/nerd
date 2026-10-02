# Standard-library foundations: additions and recommendations

Date: 2026-10-01; design clarification: 2026-10-02. Review branch: `std-library`.

This tranche adds usable threading, synchronisation and IPv4 socket foundations,
three runnable examples, compiler/runtime repairs and source inventories for the
later ports. It does **not** implement the Raptor scheduler, Kerberos graphs or
Nexus protocols. The complete Windows gates pass; native Linux adoption remains
the next validation step. The recommendations below are proposals, not additional
features already implemented or approved language syntax.

Nerd's goal is a C replacement with clearer syntax, generics, traits and slices.
It does not require a Rust-style ownership system, borrow checker, move-only
types, pinning rules or cross-thread transfer traits. Allocation, aliasing,
cleanup and synchronisation remain programmer responsibilities. Where earlier
notes used “ownership”, read that as an API's allocation/cleanup responsibility,
not a proposed language feature. The recommendations below have been revised
accordingly.

Kerberos's intended direction is a lightweight Grand Central Dispatch-style
system. ASIO is an implementation choice in the inspected source, not a goal or
dependency to preserve. Existing task/queue and dataflow examples inform the
design; reproducing the old executor's internals is not the acceptance criterion.

## What was added

| Area | Delivered behaviour | Main reference |
| --- | --- | --- |
| `std.thread` | Explicit callback/context lifetime, joinable workers, reuse and self-join rejection; Windows CRT and Linux pthread entry wrappers | [Thread/sync contracts](stdlib-thread-sync.md) |
| `std.sync` | Non-recursive mutexes and condition variables, with explicit initialisation/destruction and predicate-loop waits | [Thread/sync contracts](stdlib-thread-sync.md) |
| `std.network` | IPv4 TCP/UDP creation, binding, listening/accepting, connection, partial byte I/O, EOF, datagram sender/truncation handling, nonblocking I/O, send-side shutdown and native error detail | [Network contracts](std-network-foundation.md) |
| Native bindings | Independent `os.thread` and `os.socket` modules; fixed linkage stays in source build settings | [Module index](stdlib.md) |
| Runtime | Worker-local temporary arena/string-builder cleanup, synchronised debug allocation bookkeeping and explicit `current_temp_arena()` | [Compiler/runtime internals](overviews/INTERNALS.md) |
| Capability evidence | Atomic operations, pointer publication, callback context, payload-copy behaviour and graph-storage building blocks | [Capability audit](stdlib-capability-audit.md) |
| Planning | Pinned upstream revisions, API/test/provenance inventory, milestones and integration records | [Source inventory](stdlib-source-inventory.md), [plan](stdlib-expansion-plan.md) |

The initial native bindings cover Windows x64 and Linux x86-64 glibc. Native
header layout assertions validate the supported ABI; other architectures/libcs
are not claimed. As with C handles and native mutex objects, callers must avoid
double cleanup, keep callback data alive and keep active thread/sync structures
at their required addresses. These are API usage rules, not language restrictions
on copying ordinary values.

The examples are bounded executable tests, not merely syntax demonstrations:

| Example | Demonstrates | Run |
| --- | --- | --- |
| `thread-pipeline` | Producer/consumer transfer through a mutex/condition-protected slot; 100 values total 5050 | `just run-example thread-pipeline` |
| `network-echo` | Local TCP client/server byte exchange | `just run-example network-echo` |
| `network-datagram` | Local UDP request and reply to the sender | `just run-example network-datagram` |

Both common test recipes execute the library contracts and examples using fresh
checkout compilers/modules. The new tests cover native layouts, worker lifecycle,
temporary-storage isolation/cleanup, predicate waits and socket ownership/I/O
semantics. Loopback tests bind actual ephemeral ports and use bounded process
execution rather than fixed ports or startup sleeps.

## Compiler repairs found during implementation

| Problem | Resulting behaviour and regression |
| --- | --- |
| Generated C cached imported constants and could execute unused initialisers | Imported/qualified constants now evaluate per use, matching LLVM/local constants; mutable globals still initialise once |
| Imported constant binders could overwrite caller locals | Expansions isolate local names and bookkeeping; retained-caller-local regression covers the collision |
| LLVM mis-lowered imported mutable globals and could silently stop a function | Imports resolve to defining storage, including re-exports; same-named globals stay distinct and root public symbol names remain stable |
| Failed LLVM lowering was treated like normal fallthrough | A failed render now produces a diagnostic and rejects output instead of generating a success-looking partial function |
| Formatting removed standalone/guarded mutable variables' `pub` modifier | Visibility is preserved for typed, inferred, defaulted and guarded declarations |
| Packed bit-field writes through pointers failed lowering | The containing storage is resolved once, with adjacent-field and owner-call-count checks |
| Optional-void construction mishandled a returned void expression | LLVM omits nonexistent payload storage while retaining evaluation; generated C also preserves the call's side effects exactly once |

The stricter failure reporting exposed previously hidden bugs. Strengthened
fixtures assert completion output and observable effects, so returning early can
no longer masquerade as a passing executable. Diagnostic snapshots were updated
only where the new core declaration shifted source coordinates.

## Validation and its limits

Implementation revision `2b540cd9` passed:

- Native Windows `just test` and `just test-release`: **1181 fixtures passed,
  zero failures, 15 platform skips each**, followed by their library gates.
- All debug auxiliary checks, including **298 LLVM/C differential fixtures at
  O0/O2** (two platform skips), 6000-term depth, concurrency, toolchain/temporary
  installation, formatter workflow, source build settings and Windows stdio.
- A fresh Arch WSL build and bounded generated-C O0/O2 runtime/thread/network
  checks, including the strengthened bit-field and optional-void regressions.
- `just format`; subsequent compiler formatting changes were whitespace-only.

See the [combined evidence](../validation/windows/results/20261001-stdlib-integration/README.md)
for exact scope, logs and preserved failures. WSL lacks `opt`, `llc`, `ld.lld` and
`llvm-ar`, so its C-output checks are not a native LLVM pass. Standalone Linux,
full `just do`, desktop/editor and benchmark adoption gates remain open. No global
compiler/editor installation changed. Original upstream suites were not executed.

## Language and compiler recommendations

The main recommendations are ordinary C-replacement capabilities: typed native
callbacks, correct alignment/atomics and consistent type inference. They should
be separate, regression-backed changes. Examples labelled “proposed” describe
desired behaviour, not syntax or support already implemented.

### 1. Typed callbacks at the FFI boundary

The Windows wrapper already defines a function type:

```nerd
NativeThreadEntry :: fn (context: ^void) -> u32
```

Previously the native declaration used `entry: ^void`, and the wrapper passed
`thread_entry.as(^void)`. The practical problem is that converting to a raw pointer
discards the callback signature at the call boundary. For example, a callback
with the wrong return type can be explicitly cast to the same raw pointer type:

```nerd
wrong_entry :: fn (context: ^void) -> u64 { return 0 }
-- wrong_entry.as(^void) no longer carries the expected u32 callback signature.
```

**Implemented:** the compiler now accepts existing function types in FFI
signatures, recursively checking callback parameters and results for native ABI
compatibility. Windows and Linux thread bindings use `entry: NativeThreadEntry`
and pass `thread_entry` without the cast. Incompatible signatures are diagnosed.
Callbacks support scalars, pointers, compatible function types, and void results;
aggregates passed by value remain unsupported. See [typed callbacks](ffi.md#typed-callbacks).
This is C-style function-pointer type checking, not lifetime checking or
restrictions on what a callback may access. The native `qsort` regression and
thread lifecycle tests exercise calls from native code into Nerd.

### 2. Atomic literal inference bug — fixed

Atomic initialisers now infer untyped literals from the declared element type:

```nerd
use std.atomics

main :: fn () {
    count : atomic[usize] = 7
    assert count.load() == 7
}
```

This was a compiler bug: the atomic wrapper hid `usize` from inference, causing
the untyped literal to default to `i32` prematurely. Inference now uses the
element type as its value context, including expressions and assignments.
Literals exceeding the compiler's integer-literal capacity and incompatible
explicitly typed values remain rejected. A separate existing issue remains:
`u8 = 256` is accepted for both plain and atomic storage; this change does not
introduce element-range validation.
The capability probe no longer needs the explicit cast; atomic operations and
memory ordering are unchanged.

### 3. Layout/alignment evidence before queue optimisation

Ordinary plex layout is compiler-controlled. The compiler may reorder or pad
fields to reduce wasted space and improve cache behavior; declaration order is
not a physical-layout contract. `#c` preserves the target C ABI; `#packed`
already implies `#c` with packed storage. Both backends currently retain source
order for ordinary plexes, but library code must not depend on that choice.

The [layout probe](../validation/stdlib/layout.n) checks natural alignment and
atomic field access in stack and nested plexes, fixed arrays, growing dynamic
arrays, arena arrays (following an odd-sized allocation), and heap arrays. It
does not assert field order, fixed offsets or total size for ordinary plexes.
For example:

```nerd
state : atomic[usize] = 0
assert (^state).as(usize) % usize.size == 0
```

That says nothing about separating frequently written counters onto different
cache lines. Consider this proposed queue layout, using ordinary existing fields:

```nerd
QueueCounters :: plex {
    read_index  atomic[usize]
    write_index atomic[usize]
}
```

The declaration alone does not request cache-line separation, and an ordinary
padding field would not guarantee separation once fields can be reordered.
The probe separately compares a `#c` record's size and offsets against native
Clang C output. LLVM and generated C pass at O0/O2 on Windows x64. Run
`python validation/stdlib/check_capabilities.py --compiler <compiler>` and repeat
with `--cgen` for the compatibility backend. Other targets require their own run.

No new annotation is proposed at this stage. Existing arena allocation chooses
alignment from size, capped at eight bytes; `std.memory.alloc` promises 16-byte
alignment. Neither is a cache-line alignment contract. If queue benchmarks show
false sharing, evaluate a compiler layout policy or an explicit alignment/
separation facility with consistent stack, array and allocator support. Such a
policy must also keep named field access, initialization, `.size`, debug offsets
and both code generators consistent. Cache-line separation is a performance
choice, not a queue correctness requirement; alignment alone does not establish
lock-free progress or correct publication ordering.

### 4. Keep compiler behaviour consistent across backends

This is an already-fixed example of a compiler problem, not a new language proposal:

```nerd
noop :: fn (calls: ^i32) { calls^ += 1 }
success :: fn (calls: ^i32) -> ?void { return noop(calls) }

main :: fn () {
    calls : i32 = 0
    on success(^calls) => {} else { assert no }
    assert calls == 1
}
```

`?void` has no payload bits for the call result, but `noop` must still execute
exactly once. The earlier C backend dropped that side effect. Keep differential
tests with observable results like this, plus diagnostics for failed lowering;
an executable that silently returns early must not count as successful compilation.

## Resource management: C-style API examples

Shallow copying a dynamic array aliases its allocation. That is behaviour to
document and use deliberately, not evidence that Nerd needs an ownership system:

```nerd
main :: fn () {
    values : [..]i32
    values.push(17)
    alias := values
    alias[0] = 23
    assert values[0] == 23
    values.free()
    -- Do not use either view now, or call alias.free(): the allocation is gone.
}
```

Likewise, keeping a thread's context alive is the caller's responsibility:

```nerd
use std.thread

increment :: fn (context: ^void) { context.as(^i32)^ += 1 }

main :: fn () {
    value : i32 = 41
    worker : Thread
    assert worker.start(increment, (^value).as(^void))
    assert worker.join()
    assert value == 42
}
```

Joining before the stack variables leave scope makes this use valid. Returning
while the worker still uses them would be a caller error, just as with C thread
APIs. Production code must handle start/join failures according to the API contract;
the assertions keep this example short. No borrow checker or pinning feature is
proposed. Library-level convenience wrappers can reduce casts and boilerplate
using existing generics/traits, without changing these responsibilities.

Example verification (2026-10-02): the four complete programs above were built
and executed with a freshly built native Windows debug compiler. Typed FFI
callbacks and bare atomic literals were subsequently implemented and tested.
The expanded layout probe passes both backends at O0/O2 on Windows x64; the queue
declaration remains an illustrative fragment, not an implemented queue.

## Library and workflow recommendations

1. **Finish native Linux validation before merging into `main`.** Follow the
   [handoff](../validation/windows/results/HANDOFF.md): run both test recipes and
   the examples with the required LLVM tools, then exercise `just do` when ready
   to update that PC's installation. Automate the supported Windows/Linux matrix
   once these workflows are established; keep WSL evidence labelled separately.
2. **Complete lifecycle/error coverage before a scheduler.** Add deterministic
   native startup/initialisation failure injection, partial-worker-pool cleanup,
   contention stress and timed waits using monotonic deadlines. Review whether
   thread/sync boolean failures should expose native error detail, as networking
   already does. Preserve the distinction between locking allocator bookkeeping
   and making application allocations/arenas safe for shared mutation.
3. **Choose and prove the queue contract before optimising it.** Start with a
   bounded reference implementation and explicit producer/consumer roles. Test
   wraparound, publication, concurrent consumers, rejection ownership, shutdown
   and explicit payload cleanup before claiming lock-free or work-stealing behaviour.
   Benchmark granularity, allocation and contention only after correctness passes.
4. **Add readiness/deadlines before building Nexus atop blocking workers.** Next
   socket slices should cover nonblocking connect completion, readiness, timeout
   semantics, DNS/IPv6 and socket options/inheritance policy. Nexus must handle
   partial I/O, bounded framing/buffering, reply-route lifetime and late replies;
   do not let blocking socket waits exhaust a future worker pool.
5. **Design Kerberos around lightweight dispatch.** Define serial/concurrent
   queues, task submission, completion, waiting and shutdown around the intended
   GCD-style experience. ASIO is optional, not a required public API, architecture
   or dependency. Work stealing is one possible scheduling choice, not implied
   by adopting a dispatch API. Specify these contracts before selecting internals.
6. **Retain a small dataflow example as a behavioural reference.** Kerberos's `Graph.GenKernel`
   gives a concrete two-generators → sum → sink reference. Specify fan-out payload
   copying/cleanup rules: C++ value copying cannot be assumed equivalent to Nerd dynamic-array
   copying. Avoid serial deadlock on a full FIFO. Stable IDs, deterministic node
   order, invalidation and parallel evaluation are deliberate additions, not
   existing upstream guarantees.

## Source decisions still needed

Repository access is resolved, but the inspected Raptor branch heads use ASIO
scheduling, not a custom work-stealing scheduler. Confirm the intended newer
revision/repository or explicitly choose a new algorithm. The inspected Kerberos
graph executor is serial FIFO dataflow and leaves its scheduler argument unused;
that is a source observation, not a limit on its intended lightweight GCD-style
design. ASIO need not be retained. The [inventory](stdlib-source-inventory.md)
records exact revisions/tests and concrete lifetime/publication/type-validation
hazards that should be repaired through contracts and tests, not copied unchanged.

The repositories lack standalone licence grants, and their headers name company
copyright holders. Resolve the intended reuse/attribution terms before copying
implementation material. Source access and passing Nerd foundation tests do not
establish those terms or upstream parity.

Recommended sequence: Linux adoption checks and API review, source/algorithm
decisions, bounded queues and serial dataflow in separate branches, then scheduler
and socket-readiness work, followed by Nexus framing/protocols. Keep each milestone
reviewable with an executable example and an explicit evidence boundary.
