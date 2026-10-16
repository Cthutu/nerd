# std.network foundation, native Windows, 2026-10-01

Branch `std-network`, based on `f5d551da`. Isolated worktree; fresh Clang-built
compiler and this checkout's NERD_LIB_PATH. No global installation or editor
extension changed.

- [Initial full gate](just-test.log): 1175 passes, one stale OS module-completion
  expectation, 15 platform skips. The new `os.socket` entry is the only failure.
- [Focused repair and auxiliary gate](just-test-followup.log): corrected fixture
  passes; all remaining `just test` auxiliary checks pass. Exit status zero.
- Separate socket runner: native C ABI assertions and three Nerd programs pass
  using direct LLVM and explicit C output at O0/O2. Fresh release-compiler
  optimised execution also passes. Both common `just run-example` commands pass.
- Arch WSL generated-C O0/O2 execution also passes; this is separately identified
  compatibility evidence. Missing opt/llc prevents a direct LLVM WSL claim.

Contracts, examples, commands and remaining M5 work are in
[the foundation document](../../../../docs/std-network-foundation.md).
