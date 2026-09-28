# Source build settings — native Windows, 2026-09-28

Branch: `vulkan`, starting at `5a583d6e`. Compiler bootstrap: Clang 22.1.8,
using the existing `20260923T084624Z-7d394bf0/environment.ps1` SDK/CRT setup.
Nerd native compilation still invokes LLVM tools directly. No global compiler
or editor extension was replaced. The user's existing Vulkan example edits
were preserved separately.

Implemented contextual build blocks, module-local source defines, guarded
entries, importer-first native library paths, and overriding windowed settings.
`std.frame` now declares its windowed setting in a build block. Documentation,
configuration audit, syntax highlighting and a runnable example are updated.

Validation:

- Fresh debug and release builds pass.
- `settings-debug.log`, `settings-release.log`: integration tests pass, including
  jobs 1/4, conditional imports, define isolation, diamond path ordering,
  duplicate paths, scalar conflicts/overrides, missing environment variables,
  object/archive output, module parts, and real native library precedence.
- `format-final.log`, `format-release.log`: all 210 formatter fixtures pass,
  including comments, environment paths and recovery around malformed code.
- `lsp-final.log`: 211 existing editor fixtures pass after updating four source
  location snapshots for the two lines added to `std.frame`. The new
  `212-build-settings.lsp` also passes: source defines work in editor analysis
  without installing a referenced link SDK.
- `toolchain-release.log`: direct LLVM outputs, entry wrappers/subsystems and
  doctor checks pass. The toolchain test now exercises `build { windowed: yes }`.
- `cgen-release.log`: 294 differential fixtures pass at two optimization levels;
  two platform skips. C options, output modes, external libraries and graphics
  C compilation checks pass.
- `debug-suite.log` preserves the initial complete run, including the four
  expected LSP snapshot shifts and two pre-existing failures. The final complete
  run in `debug-suite-final.log`: **1,170 passed, 2 baseline failures, 15 skips**.
  This includes all 212 LSP fixtures and 14 examples.

Known baseline failures remain outside this feature:

1. `tests/errors/137-runtime-fixed-arrays.e`, case 7: invalid LLVM array-length
   type (`i32` passed as `i64`) with a Linux-specific diagnostic snapshot.
2. `tests/commands/274-run-std-files-windows.cmd`: stale positional `FileMode.Append`
   argument where the API now requires `mode = FileMode.Append`.

Linux execution is still required for ELF linker-path quoting/order and the
Linux-only C/terminal checks. The new integration test is wired into `just test`.

The full fixture suite used the existing Vulkan `LIB` workaround before the
separate `std.vulkan` migration. The generic library integration test supplies
its own source paths and proves precedence with two different native archives.
