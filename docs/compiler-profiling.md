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

Dependency records contain `module` and `dependency` source paths, including
implicit core imports. The loader emits them after successful semantic analysis.
They support an estimate of dependency-constrained work; there is no scheduler
or measured queue wait yet. Lex/parse includes local conditional-block selection.
LLVM sidecar re-rendering has its own phase when requested. Tool phases distinguish
`opt`, `llc`, the linker and the archiver. The existing human-readable backend
summary still groups these under its output-operation phase.

The stream is currently serial. Per-task sinks and safe allocation accounting
must precede concurrent use. Instrumentation avoids compiler arena allocations,
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
per compiler. `--after`, `--samples` and `--scenarios` override defaults. Both
compilers must support `NERD_PROFILE`. It retains combined LLVM and rejects any
byte difference, including in profiled runs. Use it for changes that should
preserve exact LLVM output. JSON records compiler hashes, commands, samples,
phase data, output hashes and affinity. Outputs live in a temporary directory;
programs are compiled but not run. The same affinity and RSS limitations above
apply. Keep a separate correctness test run after timing completes.
