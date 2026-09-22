# M9: opt-in automatic worker ceiling

Date: 2026-09-22. Implementation follows plan commit `77070043` on
`experiment/task-scheduler-performance`. This milestone adds CPU discovery and
CLI selection; it makes no new performance claim and does not change the default.

`nerd build --jobs auto source.n` selects half the available logical CPUs,
rounded down, with a minimum of one and maximum of 256. The caller is included.
Explicit numeric jobs remain overrides; omitted jobs still means one. Existing
batch readiness/ownership limits continue to cap actual concurrency.

Linux uses a growing `sched_getaffinity` mask, so restricted affinity and sparse
CPU IDs affect the ceiling. Discovery/allocation failure falls back to one.
Windows counts its primary-group process affinity mask; other POSIX platforms
use online CPUs where supported. Native Windows/macOS validation of this new
selection path is pending. CPU quotas and Windows CPU Sets/job-object budgets
are not yet represented; the ceiling is not a memory budget. The earlier Windows
validation covers the preceding compiler, not this new CPU selection code.

## Linux validation

- Core lifecycle tests pass in debug/release, including injected CPU discovery
  failure, zero/one/odd CPU counts, 32 CPUs -> 16 jobs, and saturation at 256.
- Core lifecycle tests also pass under AddressSanitizer and ThreadSanitizer.
- Production jobs suites pass with freshly rebuilt debug and release compilers.
  They restrict inherited affinity to 1/2/3/4/8 CPUs (as available), use the
  highest available IDs, verify selected worker counts and numeric overrides,
  execute the binaries, compare exact LLVM/C output and confirm the unchanged
  serial default. Invalid auto spellings remain errors.
- `just test`: 1,122 fixtures passed, zero failed, nine existing skips; all
  auxiliary suites passed, including 280 C differential fixtures at O0/O2.
  Raw full-suite and debug/release jobs logs are retained in
  [compiler-m9-auto-ceiling/](compiler-m9-auto-ceiling/).

Commands:

```sh
python3 build/test_threads.py
python3 build/test_threads.py --sanitize address
python3 build/test_threads.py --sanitize thread
just test
just build-release nerd --skip-mod-sync
python3 build/test_jobs.py --nerd _bin/nerd
```

The Windows validation runner already invokes both the lifecycle and jobs
suites, so its next full run includes the new CPU-affinity coverage. No global
compiler installation changed. Core semantic ownership, task cancellation and
LLVM tooling selection are unchanged.

## Next: M10

Carry automatic-vs-explicit mode into the scheduling policy, estimate useful
work before dispatch, and measure inline cutoffs and worker reuse. Compare
source-to-IR intervals, repeated unprofiled whole-command time, total CPU work
and memory. Use post-Windows-alignment binaries for new baselines. M11 can then
evaluate switching omitted jobs to automatic; M12 semantic ownership redesign
remains separate. A CPU-derived ceiling alone is not an adaptive default.
