# Linux Raptor API revision — 3 October 2026

Validated on `std-library`, based on `cd47f38a`, with Clang 22.1.8. The committed
source changes and `sources.sha256` identify the tested implementation. The
worktree also contained pre-existing edits to `mods/std/compress/deflate.n`,
`mods/std/frame/frame.n`, `mods/std/image/jpeg.n` and `mods/std/image/reader.n`.
Those files were tested as present and hashed here, but were neither edited nor
included in this revision's commit.

| Gate | Result | Evidence |
| --- | --- | --- |
| `just test` | 1,160 fixtures passed, 0 failed, 9 existing Linux skips; all applicable auxiliary gates passed | [Debug log](just-test.log) |
| `just test-release` | 1,160 fixtures passed, 0 failed, 9 existing Linux skips; all applicable auxiliary gates passed | [Release log](just-test-release.log) |
| C differential suite (debug recipe) | 294 fixtures at O0/O2, 0 platform skips | Debug log |
| Raptor native runner (both recipes) | LLVM and C, debug/release; 16 contract repetitions per variant; Linux partial startup injection passed | Both full logs |
| Frontend AddressSanitizer | Scheduled graph, diagnostics, generics, FFI and runtime checks passed | [ASan log](front-asan.log) |
| Task memory checks | AddressSanitizer, UndefinedBehaviorSanitizer and leak detection; C debug/release contracts and tutorial, 8 runs per variant, all passed | [Memory log](native-memory.log), [reproducible probe](native-memory-probe.py) |
| `just run-example task-parallel` | Sum of squares = 204 | [Example log](task-parallel.log) |

The five added fixtures cover generic callback method inference, explicit erased
callback specialization, deferred assertions, local method signature help and
imported Raptor method signature help. Native contracts now cover returned
handles, typed pointer/value arguments, movement before completion, explicit and
idempotent cleanup, invalid cleanup dependencies and reading results after close.
The formatter was applied only to changed source files and verified idempotent.

The additional memory probe exposed a wrong generic trampoline: distinct explicit
specializations with identical erased function signatures selected the first
matching signature. Integer callbacks happened to produce the expected result in
normal native runs. Both backends now select the explicit specialization; the
regression also uses a floating-point callback to expose incompatible calling
conventions without relying on a sanitizer. Native memory checks report no
sanitizer errors or leaks after the repair.

To reproduce the additional probes from the repository root:

```sh
python3 build/test_front_threads.py --sanitize address
python3 validation/linux/results/20261003-raptor-api/native-memory-probe.py
just run-example task-parallel
```

Native Windows checks remain pending for this revision. No throughput, lock-free,
fairness or starvation-freedom claims are established. Owning boxes/records
containing them remain unsupported task payloads; callers retain payload lifetime
and cleanup responsibilities. Nexus message and Kerberos graph remakes and their
tutorials are still future work.
