# Thread/sync foundation — 2026-10-01

Integration update: the C-output discrepancy recorded below is fixed in the
combined `std-library` branch. Its alias reproducer is now a passing regression
in both common test recipes. See the [final combined evidence](../20261001-stdlib-integration/README.md);
the original branch-local observations below are retained as history.

Scope: first M1 foundation on `std-thread-sync`, based on `std-runtime-threads`
`d7bfbf40`; no installation, scheduler or merge into the integration branch.

Native Windows x64, fresh Clang debug and release compilers:

- `python build/build.py nerd --skip-mod-sync`: passed.
- `python build/test_std_thread_sync.py`: native header ABI assertions and both
  lifecycle and pipeline executables passed with debug and optimised LLVM output.
- The same runner with `--nerd _bin/nerd.exe` passed both modes.
- The optional `--cgen` gate passed both fixtures at O0/O2 using native Clang.
- `python build/test.py --filter 117-os-module-completion`: passed after adding
  `os.thread` to the completion expectation. Later integration must union this
  entry with the networking branch's `os.socket` entry.
- Worker allocation tests verify separate current-thread arenas, concurrent heap
  and interpolation use, and no leak diagnostics after normal thread exits.

Native header-only layout assertions also passed under WSL Ubuntu (x86-64,
glibc 2.39, Clang 18.1.3) and WSL Arch Linux (x86-64, glibc 2.43, Clang 22.1.6).
Ubuntu's existing Clang 18 cannot build Nerd's C23 `#embed` directives; Arch has
Clang 22 but no `opt`/`llc` on PATH. A fresh isolated Arch Clang 22 compiler build
passed Linux semantic checks and the explicit `--cgen` compatibility runner:
both lifecycle and pipeline executions passed at O0/O2, including the runtime
thread hooks. This is generated-C compatibility evidence, **not a direct LLVM
execution pass**. No tool installations were performed. Full native Linux and
WSL LLVM executable gates remain.

## Compiler discrepancy found during validation

`tests/stdlib-thread-sync/temp-arena-alias-reproducer.n` is a diagnostic outside
the passing portable runner. It compares the legacy imported `temp_arena` alias
with the current-thread accessor in a worker. Windows LLVM output prints
`current`; generated C prints `cached` on both Windows and WSL Arch. LLVM IR
contains two current-thread lookups:

```llvm
%t0 = call ptr @$current_temp_arena()
%t1 = call ptr @nrt_temp_arena()
```

The generated C instead reads the initialised global for the alias:

```c
void* ncg_v58 = ncg_m3_fn15(); /* current_temp_arena */
void* ncg_v59 = ncg_m3_g0;    /* imported temp_arena binding */
```

The portable tests use the explicit accessor and check that simultaneously live
workers have distinct arenas from each other and from the main thread. The
imported-constant CGen discrepancy is tracked separately for compiler repair;
this branch does not silently remove or claim to fix it.

M1 is not complete: deterministic native startup fault injection, additional
spurious-wake scheduling, broader stress and the common cross-platform test gates
remain. The API intentionally lacks detach, cancellation and timed waits.
The combined full `just test` / `just do` gate remains pending integration;
this branch ran bounded foundation and affected completion checks.
