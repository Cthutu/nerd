# Thread and synchronisation foundation

This is the first M1 slice, not a scheduler or a complete M1 sign-off.
It provides `std.thread` and `std.sync`, backed by `os.thread` without adding
application-specific compiler flags. It depends on the runtime thread lifecycle
and allocation tracking fixes on `std-runtime-threads`.

```nerd
use std.thread

calculate :: fn (context: ^void) {
    value := context.as(^i32)
    value^ = 42
}

main :: fn () {
    value : i32
    worker : Thread
    assert worker.start(calculate, (^value).as(^void))
    assert worker.join()
    assert value == 42
}
```

## Thread ownership

`Thread` starts zero-initialised. `start(callback, context)` borrows its
context; it never copies, frees or transfers ownership of that context. Keep
both the context and the `Thread` object alive at fixed addresses until a
successful `join()`. Do not copy an active thread owner. Nerd currently does
not enforce these rules through its type system.

The callback can start before `start` returns. It must not inspect the owner
until the owner has published completion of `start` through synchronisation.
Only one owner may perform lifecycle operations; concurrent starts or joins on
the same owner are forbidden. Shared application data needs its own mutex or
atomics. Successful `join` makes the worker's completed writes visible and
releases its native resources. A second join of an idle object succeeds, and
the object can start another thread. Self-join is rejected.

Starting an active owner or passing a null callback returns `no`. Native start
failure resets an idle owner; the caller still owns the context. Join failure
retains ownership and must be handled before freeing the context. There is no
detach, cancellation, forced termination, timeout or implicit destructor. Do
not exit the process with unjoined workers. Default stack sizing comes from the
native threading API.

The trampolines call `nrt_thread_init` before the callback and `nrt_thread_done`
after normal callback return. Worker heap allocations remain explicitly owned;
only thread-local interpolation/temporary arena storage is released by the exit
hook. Borrow temporary storage only on its owning thread and do not return a
temporary pointer to a caller that outlives the worker. Use
`current_temp_arena()` for an explicit current-thread lookup. These hooks do not
make shared objects or every standard-library helper thread-safe: inspect each
helper's ownership, mutation and global-state requirements. On this base the
legacy `temp_arena` alias resolves per use in LLVM output but is cached at module
initialisation in generated C. Avoid that alias and helpers depending on it in
workers until the separately tracked compiler discrepancy is fixed.

## Mutexes and conditions

`Mutex` and `Condition` require `init()` and `done()`. Repeated initialisation
returns `no`; destruction of an idle object succeeds. Keep initialised objects
at stable addresses without copying them. Initialise before publishing them to
workers; destroy only after all users have joined. Destroying a locked mutex,
destroying a condition with waiters, unlocking from a non-owner or recursively
locking a mutex violates the contract. A `bool` return does not validate such
misuse; the native API may deadlock or have undefined behaviour.

`Mutex.lock()` and `unlock()` provide exclusive, non-recursive ownership.
`Condition.wait(^mutex)` requires that the caller holds that mutex. It
atomically releases it while waiting and reacquires it on wake. **Always wait in
a loop over a predicate protected by the same mutex.** Spurious wakes and
notifications arriving before a wait are normal; a condition is not a queued
event. All concurrent waiters on a condition must use the same mutex.

Publish the predicate while holding its mutex, then call `signal()` to wake at
least one waiter or `broadcast()` to wake all current waiters. Shutdown is an
application predicate followed by broadcast and joins; there is no hidden
cancellation policy. Waits are indefinite in this slice.

## Platforms and verification

Windows x64 uses CRT `_beginthreadex` with SRW exclusive locks and condition
variables. Returning from the callback lets the CRT perform its native thread
cleanup; the joining owner closes the handle. See Microsoft's
[`_beginthreadex` contract](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/beginthread-beginthreadex?view=msvc-170).

Linux bindings currently target **x86-64 glibc** and use pthreads. The opaque
mutex and condition storage matches
[glibc's x86 ABI declarations](https://github.com/bminor/glibc/blob/master/sysdeps/x86/nptl/bits/pthreadtypes-arch.h).
Do not assume these layouts on musl, another libc or another architecture.
`tests/stdlib-thread-sync/native-layout.c` validates sizes and alignments against
the actual platform headers before the runner executes Nerd tests.

Run the bounded tests with a freshly built compiler:

```sh
python build/build.py nerd --skip-mod-sync
python build/test_std_thread_sync.py
```

For the separate optional C-output compatibility gate, add `--cgen`. That
explicitly compiles generated C with native Clang; it is never an automatic
fallback for unavailable LLVM tools.

The runner uses this checkout's modules, native header layout assertions, debug
and optimised LLVM executables, expected results and 15-second execution
deadlines with process-tree termination on timeout. It covers startup rejection,
active-start rejection, repeated start/join, self-join rejection, two predicate
waiters, signal/broadcast, notification before waiting, cleanup/reinitialisation,
concurrent heap/interpolation allocations and worker TLS cleanup. The example
checks a 100-element one-slot producer/consumer pipeline with total 5050.

Native OS resource-exhaustion fault injection, forced spurious wake scheduling,
timed waits, cancellation, thread identity and broader stress testing remain
future work. Native Windows evidence and Linux/WSL gaps are recorded in
[the validation note](../validation/windows/results/20261001-thread-sync/README.md).
Both `just test` and `just test-release` execute this runner through LLVM and
generated C, including the imported temporary-arena alias regression. This does
not replace the later complete `just do` platform gates.
