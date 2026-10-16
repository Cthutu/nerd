# Plex field layout: native Windows verification, 2026-10-16

Feature branch: `plex-field-layout`, starting from `d5702e16` on `main`.
These are focused compiler-feature checks with fresh checkout debug/release
compilers on the same Windows/LLVM toolchain as the earlier 16 October full run.
This is not a repeat of the 65-stage platform validation or its manual checks.

Ordinary plex storage units now sort stably by descending alignment. C-layout
and packed plexes retain declaration order. The runtime regression proves that
`u8, u32, u8` occupies 8 bytes normally and 12 bytes with `#c`, and checks field
addresses, initializer effects, constants, globals, copies, arrays, generics,
bit blocks and direct/transitive embedding conversions. A dedicated LLVM
snapshot verifies compact, C-layout and embedded debugger member offsets.

## Results

The initial full fixture run found eight expected LLVM snapshot mismatches from
field reordering, with 1,193 other passes and 15 platform skips. After updating
those snapshots and correcting typed bit-block alignment, the second full run
passed 1,200 fixtures with 15 skips and one remaining snapshot mismatch: an
array-element GEP index had been changed unnecessarily in the expected output.
Correcting that expectation and rerunning the complete LLVM family passed all
49 cases, including the newly added debugger fixture. No compiler changes
followed the second full run. The final per-family coverage is:

| Family | Passed | Skipped |
| --- | ---: | ---: |
| Language | 215 | 2 |
| Diagnostics | 136 | 0 |
| HIR | 28 | 0 |
| LLVM (final focused rerun) | 49 | 0 |
| Formatter | 223 | 0 |
| LSP | 218 | 0 |
| Commands | 305 | 13 |
| Standard library | 1 | 0 |
| Examples | 27 | 0 |
| Total | 1,202 | 15 |

The release compiler independently passed 215 language fixtures, with two
platform skips. LLVM/C differential testing passed all 305 applicable programs
at `-O0` and `-O2`, including the new runtime regression, plus CLI/output-mode
checks. Two platform fixtures and the Linux-only Dungeon PTY check were skipped.

Capabilities and native C layout checks passed for debug/release targets.
Thread-sync contracts passed through LLVM and C for debug/release targets.
Network and Nexus contracts passed through LLVM (debug targets), and Raptor
contracts passed through LLVM for debug/release targets. Existing Linux-only
injection skips remain explicit in the logs. Concurrent LLVM rendering matched
serial output exactly, including repeated-input reuse and cleanup checks.

Reproduction (PowerShell or Git Bash, from repository root):

```sh
python build/build.py nerd --skip-mod-sync
python build/build.py -r nerd --skip-mod-sync
python build/test.py
python build/test.py --nerd _bin/nerd.exe --filter language
python build/test_cgen.py --nerd _bin/nerd-debug.exe
python validation/stdlib/check_capabilities.py --compiler _bin/nerd-debug.exe
python build/test_std_thread_sync.py --nerd _bin/nerd-debug.exe
python build/test_std_thread_sync.py --nerd _bin/nerd-debug.exe --cgen
python build/test_network.py --compiler _bin/nerd-debug.exe
python build/test_raptor.py --nerd _bin/nerd-debug.exe
python build/test_nexus.py --compiler _bin/nerd-debug.exe
python build/test_render_threads.py --nerd _bin/nerd-debug.exe
```

Linux/macOS execution, the complete native validation matrix, benchmarks and
new human desktop/debugger observations were not run for this feature.
