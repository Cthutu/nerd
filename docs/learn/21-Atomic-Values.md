# Chapter 21: Atomic Values

[Previous: Compile-Time Parameters and Built-In Macros](20-Compile-Time-Parameters-and-Macros.md)

[Next: Advanced C Interoperability](22-Advanced-C-Interoperability.md)

Atomic storage lets threads access one shared scalar without tearing a read
or write into separate observable pieces. This chapter assumes you understand
threads, shared memory, and the need to synchronise access. It teaches the
language's storage and operations; the examples use one thread so their
results are repeatable.

## Declaring atomic storage

`atomic[T]` is a built-in storage type:

```nerd
main :: fn () -> i32 {
    count: atomic[i32] = 40
    count += 2
    result: i32 = count
    return result
}
```

This returns `42`. Reading `count` loads an `i32`; assigning it stores an
`i32`. Supported compound assignments perform atomic read-modify-write
operations. Ordinary atomic accesses use sequentially consistent ordering.

Atomic elements may be `bool`, signed or unsigned integer types, or thin
pointers such as `^i32`. Floats, plexes, strings, slices, dynamic arrays, boxes,
and function values are not supported element types.

## One operation versus several operations

An atomic compound addition is one read-modify-write operation. An expression
that loads, calculates, and stores separately is several operations:

```nerd
main :: fn () -> i32 {
    count: atomic[i32] = 40
    snapshot: i32 = count
    count = snapshot + 1
    count += 1
    return count
}
```

This returns `42` in this single-threaded example. With concurrent writers,
the load followed by `count = snapshot + 1` can overwrite another thread's
update. `count += 1` performs the increment atomically. Atomic storage does
not make a sequence of statements indivisible.

## Borrowing atomic storage

Atomic storage has identity. Pass its address rather than passing or returning
an `atomic[T]` by value:

```nerd
increment :: fn (count: ^atomic[i32]) {
    count^ += 1
}

main :: fn () -> i32 {
    count: atomic[i32] = 41
    increment(^count)
    return count
}
```

This returns `42`. An explicit atomic-to-atomic initialisation or assignment
loads the source and stores into distinct destination storage. That pair is
not one indivisible transfer. It does not make both names refer to one atomic.

Keep the storage alive for every thread that borrows it, and complete those
threads before releasing it. An atomic pointer protects access to the pointer
value; it does not automatically protect or retain the object it points to.

## Explicit operations

Import `std.atomics` to make the explicit operations available:

```nerd
use std.atomics

main :: fn () -> i32 {
    count: atomic[i32] = 10
    previous := count.fetch_add(5)
    assert previous == 10
    assert count.load() == 15
    old := count.exchange(42)
    assert old == 15
    return count.load()
}
```

This returns `42`. `fetch_add` returns the value before the update, as do the
other fetch operations. `exchange` replaces a value and returns the old value.

| Operation | Effect |
| --- | --- |
| `load()` | Return the current value. |
| `store(value)` | Replace the value without returning the previous one. |
| `exchange(value)` | Replace the value and return the previous one. |
| `fetch_add(value)`, `fetch_sub(value)` | Arithmetic update, returning the previous value. |
| `fetch_and(value)`, `fetch_or(value)`, `fetch_xor(value)` | Bitwise update, returning the previous value. |
| `compare_exchange(expected, desired)` | Replace only if the current value equals `expected`. |
| `compare_exchange_weak(expected, desired)` | Like compare-exchange, but may fail spuriously. |

Use operations appropriate to the element type; an atomic pointer is not an
integer counter.

## Comparing and exchanging

Compare-exchange returns an enum describing the outcome:

```nerd
use std.atomics

main :: fn () -> i32 {
    state: atomic[i32] = 0
    on state.compare_exchange(0, 42) {
        Exchanged => return state.load()
        NotExchanged(observed) => return observed
    }
}
```

This returns `42`. `NotExchanged` carries the observed value, which may help a
caller recompute and retry. A weak operation may report failure even when the
values match, so use it only in an algorithm that permits retrying. A successful
exchange alone does not establish the correctness of a larger concurrent
algorithm.

## Memory ordering

Each explicit method defaults to `SequentiallyConsistent`. Override a default
with a named compile-time argument:

```nerd
use std.atomics

main :: fn () -> i32 {
    count: atomic[i32] = 40
    _previous := count.fetch_add(2, order = Relaxed)
    return count.load(order = Relaxed)
}
```

This returns `42`. The enum value must be known at compile time, as described
in Chapter 20. The operation categories use different enums:

| Parameter type | Available orders |
| --- | --- |
| `AtomicLoadOrder` | `Relaxed`, `Acquire`, `SequentiallyConsistent`. |
| `AtomicStoreOrder` | `Relaxed`, `Release`, `SequentiallyConsistent`. |
| `AtomicOrder` | `Relaxed`, `Acquire`, `Release`, `AcquireRelease`, `SequentiallyConsistent`. |

Relaxed accesses provide atomicity without publishing unrelated memory.
Release and acquire can publish and observe other writes when the acquire
observes the corresponding release through a correctly designed protocol.
`AcquireRelease` combines those roles for a read-modify-write operation.
Sequential consistency adds a shared ordering for sequentially consistent
atomic operations. Start with the default while establishing correctness.

Compare-exchange has separate `success_order` and `failure_order` defaults.
Failure performs a load rather than a store, so its order uses
`AtomicLoadOrder`; choose a failure order compatible with the success order.
For example, `success_order = AcquireRelease, failure_order = Acquire` is a
valid combination.

The backend guarantees at least the requested ordering and may use stronger
ordering. Do not depend on that extra strength. Nerd guarantees atomic
semantics, not that every supported atomic is lock-free on every target.
Use a lock or another synchronisation abstraction when an invariant spans
several values or operations.
