# Native Windows Just test workflow — 2026-09-30

Starting revision: `b542a13d` on `main`. Scope: make the normal native Windows
`just test` recipe work, without the historical validation environment script.
WSL/Linux and the complete `just do` installation workflow are not validated here.

## Failures and repairs

- The first invocation stopped in `test_memory.py`: Python decoded UTF-8 allocator
  output with Windows CP1252. Subprocess text decoding now explicitly uses UTF-8.
- Render/front-end/toolchain helpers also decoded fixture files with the local
  code page, corrupting the `¬` delimiter. Their fixture reads now use UTF-8.
- The Windows files fixture used a positional default argument; it now names
  `mode = FileMode.Append`, as the language requires.
- The array-return error fixture encoded an LLVM failure with Linux absolute
  paths. Block return inference bypassed the existing local-array escape check.
  It now rejects the return before LLVM. Runtime-array type resolution also
  interns `usize`, so an i32 length widens correctly in import-free programs.
- The formatter removed explicit return types from simple functions. It now
  preserves these contracts, including result/optional wrapping and widening.
  Six affected snapshots were updated and a dedicated fixture added. The local
  JPEG edit present at the start was restored; `just format` now preserves it.
- `nerd doctor` rejected unset `LIB` despite successful linker SDK discovery.
  The real link/run probe now decides; an invalid `LIB` still fails the test.
- Windows debug-info smoke tests needed `llvm-dwarfdump` on PATH. Copied the
  existing LLVM 22.1.8 executable to `~/.local/bin` alongside opt/llc, outside
  the directories deleted by `just clean`. Missing-tool errors are now explicit.
- `just test-release` previously ran the debug compiler. It now selects the
  release binary via the harness's new `--nerd` option. Missing selected
  binaries fail before fixture execution.

## Environment and evidence

Native Windows x64, Python 3.14.7, Just 1.48.0, Scoop Clang 23.1.0,
opt/llc and llvm-dwarfdump 22.1.8. Native Nerd output continues to use LLVM
tools directly; Clang builds Nerd and validates optional C output.
No global Nerd compiler or editor extension installation was changed.

- `baseline-fixtures.log`: after the initial encoding repair, 1,171 passes,
  one pre-existing array diagnostic failure, 15 skips (the files fixture was
  corrected before that run reached it).
- `first-fixture-pass.log`: 1,174 fixture passes, 15 skips, followed by the
  auxiliary render-helper encoding failure.
- `auxiliary-sdk-failure.log`: render, depth, scheduler and front-end checks
  passed after encoding repairs; doctor then rejected unset LIB.
- `release-selection.log`: focused `just test-release` run against `_bin/nerd.exe`.
- `just-test.log`: complete recipe exited 0: 1,174 fixture passes, 15 skips,
  all auxiliary stages passed, including 295 C differential fixtures (2 skips).
- `format.log`: final `just format` exited 0.
- `release-doctor.log`: release compiler doctor passed with linker SDK discovery.

Focused checks passed for all 211 formatter cases, array diagnostics, the new
runtime array case, and the direct toolchain (including missing tools and invalid
SDK paths). The new runtime case is also covered by the C differential suite.
Installation smoke checks run in temporary directories and do not install Nerd.
