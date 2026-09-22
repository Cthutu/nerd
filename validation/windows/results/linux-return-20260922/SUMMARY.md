# Linux return validation — 2026-09-22

Pulled `6bbc17e8..da00ffae` and reviewed the Windows handoff and compiler changes.
Tested HEAD `da00ffae` (compiler implementation `828d4b56`) on native Linux x86-64
with freshly rebuilt debug and release compilers. All commands below exited zero.
No additional compiler or test repairs were needed. Compiler hashes and local
platform/tool information are in [environment.json](environment.json).

| Command | Result | Evidence |
| --- | --- | --- |
| `just test` | 1,122 fixtures passed, zero failed, nine existing skips; all auxiliary suites passed; 280 C differential fixtures at O0/O2, zero C platform skips | [Complete log](just-test.log) |
| `just build-release nerd --skip-mod-sync` | Fresh release compiler built | [Build log](build-release.log) |
| `python3 build/test_front_threads.py --sanitize address` | Graph, generic, diagnostic, randomized concurrency and runtime checks passed | [ASan log](front-address-sanitizer.log) |
| `python3 build/test_front_threads.py --sanitize thread` | Same front-end ownership suite passed | [TSan log](front-thread-sanitizer.log) |
| `python3 build/test_toolchain.py --nerd _bin/nerd` | Direct LLVM, integer entry points, output modes, doctor, missing tools and dynamic enum alignment passed | [Toolchain log](release-toolchain.log) |
| `python3 build/test_cgen.py --nerd _bin/nerd` | 280 differential fixtures at O0/O2; C options/libraries and terminal behavior passed | [Release C log](release-cgen.log) |
| `python3 build/check_debugger_stepping.py --nerd _bin/nerd-debug --jobs 4` | Source debugger stepping passed | [Debug compiler log](debugger-debug.log) |
| `python3 build/check_debugger_stepping.py --nerd _bin/nerd --jobs 4` | Source debugger stepping passed | [Release compiler log](debugger-release.log) |

The full `just test` auxiliary sequence includes allocator/thread lifecycle,
profile contracts, concurrent render ownership, job dispatch/failures, front-end
scheduling, direct LLVM toolchain/doctor, isolated installation and C emission.
Both toolchain runs exercise the new dynamic enum allocation/growth/reserve
alignment regression in debug/release targets at jobs 1/4. Both C suites include
that regression at O0/O2, and native Linux PTY Dungeon rendering-before-input,
LLVM/C behavior parity and Q exit. Array conversions and adjusted leak-size/LLVM
snapshots pass in the full fixture suite. Wide imported generic cases remain
covered by the front-end suites. Normal binary generation still uses LLVM tools;
external Clang is used by the C compatibility tests.

No new graphical desktop capture or benchmark was performed on Linux. Some
correctness suites ran concurrently, so their durations are not performance
measurements. Earlier Linux timing evidence predates the array-header padding
fix. Native macOS and Windows CPU/RSS evidence remain outstanding; keep jobs 1
as the default. The globally installed main comparison compiler was not replaced;
the returned code is built in `_bin/nerd-debug` and `_bin/nerd`.
