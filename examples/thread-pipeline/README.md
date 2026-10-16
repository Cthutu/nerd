# A one-slot thread pipeline

```sh
just run-example thread-pipeline
```

The main thread sends integers 1 through 100 to a consumer through a one-slot
buffer. Both use one mutex and a condition variable. Predicate loops handle
empty/full states; broadcasting after changes wakes either side. The producer
publishes a finished flag, wakes the consumer, then joins it before reading the
sum or destroying the synchronisation objects.

Expected output:

```text
Consumed 100 values; total = 5050
```

The callback borrows the stack-allocated pipeline. The pipeline and its thread
owner stay at fixed addresses until join succeeds. Failure to start leaves all
ownership in the main thread and releases the initialised synchronisation
objects through `defer`.

This is a new M1 teaching example and foundation test, not a Raptor queue port
or a claimed mapping to an inaccessible upstream test. A Raptor-derived queue
pipeline is a separate M2 milestone. See [the API contract](../../docs/stdlib-thread-sync.md)
and [validation status](../../validation/windows/results/20261001-thread-sync/README.md).
