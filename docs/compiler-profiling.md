# Compiler profiling and benchmarks

Set `NERD_PROFILE=1` to emit machine-readable profiling records on stderr:

```sh
NERD_PROFILE=1 nerd build program.n 2>profile.log
python3 build/benchmark_compiler.py --cpu 2 --samples 5 --output /tmp/nerd-benchmark.json
```

The compiler's normal output and `--timing` display are unchanged. Each record
starts with `nerd-profile` and a tab, followed by one JSON object. Other stderr
lines remain ordinary diagnostics. Profiling works without `--timing`, including
`check`, `--cgen` and `--copts`; command-substitution stdout remains clean.

## Records

Phase records contain:

- `stage`, `phase`, and `module`: source path for per-module lexing, parsing,
  semantic analysis, HIR lowering and LLVM rendering. Whole-program phases and
  external tools have an empty module name, as do inline root sources.
- `start_ns` and `wall_ns`: monotonic start timestamp and elapsed wall time in
  nanoseconds. Start timestamps are process observations, not calendar dates or
  scheduler-ready timestamps.
- `cpu_ns`: current-thread CPU time, excluding external tool CPU usage. It is
  null if the host cannot provide a thread CPU clock; it never substitutes wall
  time while calling it CPU time. Windows uses `GetThreadTimes`.
- `success`: whether the measured operation succeeded. Failed front-end phases
  and tools still emit their record when they return normally.
- `output_bytes`: emitted module or combined LLVM text size where available;
  zero means not measured for that phase, not necessarily empty output.
- Allocation deltas: heap allocations/reallocations, arena requested/committed
  bytes and array growths. `heap_live_bytes` and `heap_peak_bytes` are cumulative
  tracked heap snapshots, not per-phase RSS. Arena commitments are cumulative
  new commitments during the phase, not current live arena memory.
  Activity deltas use counters on the executing thread, so another thread's
  allocations are excluded. A probe must begin and finish on one thread without
  running unrelated tasks between them. Nested probes are inclusive. Frees and
  reallocations count toward the executing task, even for handed-off blocks.
  Live/peak snapshots remain synchronized and process-wide; they do not measure
  a task's retained footprint.

LLVM render tasks finish value-only timing records before coordinator diagnostic
replay. The coordinator emits these records in module order, including alternate
sidecar renders. JSON formatting, diagnostic replay and later file writes are
outside the render measurement. Labels and paths are supplied at emission;
finished records retain no source pointers. Other serial callers retain the
combined finish-and-emit API. Dependency records and the legacy
`NERD_MEMORY_PROFILE` stream still require coordinator/serial use; the latter's
deltas remain process-wide.

Dependency records contain `module` and `dependency` source paths, including
implicit core imports. The loader emits them after successful semantic analysis.
They support an estimate of dependency-constrained work. The LLVM render batch
has no dependency scheduling. Its dispatch-delay record includes startup and
waiting behind earlier module tasks; it is not a dependency-ready timestamp.
Lex/parse includes local conditional-block selection.
LLVM sidecar re-rendering has its own phase when requested. Tool phases distinguish
`opt`, `llc`, the linker and the archiver. The existing human-readable backend
summary still groups these under its output-operation phase.

The stream is emitted serially by the coordinator; concurrent render tasks
record values in their own result slots. Instrumentation avoids compiler arena allocations,
so it cannot invalidate active string builders. Record writing occurs after a
phase's clocks and memory counters are sampled, but still adds total process
overhead. Use separate unprofiled runs to measure build latency.

## Repeatable runner

Build the release compiler first with `just build-release nerd --skip-mod-sync`.
`build/benchmark_compiler.py` defaults to that executable; use `--nerd` to choose
another. Compiler build configuration and target debug/release configuration are
independent. The runner tests both target configurations by default.

The matrix includes check, LLVM binary generation, and C emission for tiny,
dungeon, pixels, quill, wide imports, deep imports, and a large single module.
Synthetic inputs have 16 modules with 48 helper functions each, or 768 functions
in one module. Generated programs are not run. C timings exclude C compilation.
Temporary outputs do not modify the examples or require a display.

Each matrix cell runs one warm-up and five unprofiled samples by default, then a
separate profiled sample. JSON retains every sample, commands, compiler hash,
source hashes, source commit/dirty status, LLVM versions and affinity. Run options
include `--scenarios`, `--modes`, `--targets`, `--samples` and `--warmups`.

On Linux, `--cpu N` pins the runner, compiler and its children to one available
logical CPU. This does not control frequency, SMT sibling load or other system
activity. Compare repeated distributions on the same configuration, not isolated
minimum timings. Omit affinity for future multicore scaling measurements.

On hosts with `wait4`, per-invocation user/system CPU and peak RSS are collected.
The RSS field is the maximum process high-water mark reported for the compiler
and waited-for descendants, not the simultaneous sum across a process tree.
Unsupported hosts report null. Artifact sizes are recorded separately from LLVM
text sizes. Compiler allocation counters do not include LLVM subprocess memory.

`--cache cold-inputs` requests source-page eviction with POSIX
`POSIX_FADV_DONTNEED` before each run, including library sources. It is best-effort:
dirty or mapped pages may remain cached, and compiler/tool binaries and filesystem
metadata are not flushed. Report it as source eviction advice, not a fully cold
machine. Fully cold tests need a controlled reboot or dedicated cache-management
procedure outside this runner; it never drops system-wide caches.

For Linux call-stack evidence, record a separate unprofiled run with `perf`, for
example `perf record -F 999 --call-graph dwarf -- nerd check program.n`. Do not
run sampling or test suites concurrently with the latency benchmark. Sampled
percentages identify candidates; validate optimisations with unprofiled timings.


## Comparing a serial optimization

Preserve the baseline release compiler before rebuilding, then run:

```sh
python3 build/benchmark_compare.py --before /tmp/nerd-before --cpu 2 \
  --output /tmp/nerd-comparison.json
```

This runner alternates baseline and changed compilers on the same input and
output paths, using checkout modules for both. It tests debug and release LLVM
builds, with one warm-up, five unprofiled samples and a separate profiled sample
per compiler. `--after`, `--samples` and `--scenarios` override defaults.
For a paired parallel comparison, `--jobs N` passes the same worker count to
both compilers and `--cpus 0 1 ...` selects a shared multi-core affinity set
instead of `--cpu`. Omitting `--jobs` preserves each compiler's default. Both
compilers must support `NERD_PROFILE`. It retains combined LLVM and rejects any
byte difference, including in profiled runs. Use it for changes that should
preserve exact LLVM output. JSON records compiler hashes, commands, samples,
phase data, output hashes and affinity. Outputs live in a temporary directory;
programs are compiled but not run. The same affinity and RSS limitations above
apply. Keep a separate correctness test run after timing completes.


## Comparing LLVM worker counts

`nerd build --jobs N` (or `-j N`) enables concurrent LLVM module rendering;
1 is the default and runs inline. The front end, merge and external LLVM tools
remain serial. Each module's profile is emitted in program order after workers
join, irrespective of completion order. Per-task wall times overlap; their sum
is work duration, not elapsed render time. Heap live/peak observations remain
process-wide. Batch dispatch/drain timings are recorded separately. Optional
lock-acquisition timings require `NERD_PROFILE_LOCKS=1` as described below.

```sh
python3 build/benchmark_jobs.py --cpus 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 \
  --output /tmp/nerd-jobs.json
```

Choose an affinity set appropriate to the host (the example assumes 16 distinct
physical cores). This runner rotates jobs 1/2/4/8/16, with one warmup and five
unprofiled samples each, plus one separately profiled run. It checks identical
combined LLVM bytes at fixed paths for debug and release builds. The profiled
render envelope spans earliest task start to latest task finish; it excludes
thread startup, final joins and ordered merge. Whole-command timings include
those costs and all front-end/tool work. Profile envelopes are single-sample
diagnostic evidence, not repeated latency measurements. `--jobs`, `--samples`
and `--scenarios` override defaults. Do not run competing builds/tests while
measuring. Generated programs are not executed by the benchmark.


## Allocator and scheduler diagnosis

Set both `NERD_PROFILE=1` and `NERD_PROFILE_LOCKS=1` to add
`memory_lock_acquisitions` and `memory_lock_acquire_ns` to phase records.
Counters are cumulative per OS thread, sampled around each probe. Nested probes
are inclusive; records retain values and the coordinator emits them later.
Turning on the lock flag alone produces no output and does not enable timing.
Scopes restore the preceding thread-local setting. The final process heap
snapshot is outside the probe's own lock measurements.

Acquisition duration runs from immediately before the native bookkeeping lock
to immediately after acquisition. It includes uncontended lock cost, clock
cost and descheduling, so it is not a pure blocked-time measurement. Every
instrumented acquisition takes two clock readings, one while holding the lock;
conversion and counter updates occur after unlocking. This perturbs contention.
Use a separate diagnostic run, never these timings as an unprofiled speedup.
When disabled, there are no extra clock reads; the allocator still tests the
thread-local enable flag.

With more than one requested job, a `kind: "scheduler"` record describes the
LLVM render batch. It is emitted after joining workers and before ordered
module records. `jobs` is requested slots; `slots` is capped at `modules`.
`completed` and `success` record batch completion, including startup failure.
`wall_ns` spans the task-run call, including startup and joins.
`first_dispatch_ns` is the interval before the first callback starts, and
`drain_ns` spans the last callback's completion to return from the task batch.
These are elapsed boundaries, not isolated native thread-create/join costs.
`dispatch_delay_ns_sum` sums each task's delay from batch submission to callback
entry, including startup and waiting for earlier tasks. Do not add it to elapsed
wall time. A failed startup reports zero completed tasks and zero task intervals.
The inline jobs=1 path has no batch record, since it interleaves rendering with
coordinator output. Per-module phase profiles remain available in both paths.

`build/benchmark_jobs.py --lock-profile` adds a separate lock-instrumented run
per cell alongside its ordinary profile and unprofiled samples. It checks LLVM
identity in every mode and retains `lock_profile` and `lock_sample` separately.
The runner clears inherited lock instrumentation for ordinary measurements.
