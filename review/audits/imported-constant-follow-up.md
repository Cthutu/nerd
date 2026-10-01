# Follow-ups discovered while repairing imported C constants

Baseline: `f5d551da`; native Windows, freshly built Clang debug compiler.
These are separate pre-existing compiler defects, not fixed by the imported
constant C-generation repair itself. The formatter defect was subsequently
repaired in `8eb7b5e3`, now integrated into `std-library`; its regression covers
standalone and guarded public variables. The LLVM defect is being repaired on
the separate `std-llvm-imported-globals` branch. Reproductions below preserve the
original failure descriptions.

## Formatter removes visibility from typed mutable globals

Formatting this source removes `pub` from the first declaration:

```nerd
pub count: i32 = 0
main :: fn () {}
```

Run `nerd format <file>` and inspect the declaration. Importing the formatted
module then reports that `count` is private. The constant regression uses a
private counter plus a public accessor so it does not depend on this defect.

## LLVM silently stops at an imported mutable-global reference

Module `state.n`:

```nerd
pub once := 1
```

Consumer (with the module on `NERD_LIB_PATH`):

```nerd
use state
main :: fn () {
    assert once == 1
    prn("reached")
}
```

Both this minimal example and the fuller `pub once := next()` case reproduce
the defect. Generated LLVM ends the function before the direct `once` comparison,
emits no diagnostic, and the executable exits successfully without reaching the
print. Generated C evaluates the comparison normally.
Inspect the emitted LLVM as well as stdout: missing lowering must not be
reported as successful compilation. A public getter in the defining module
avoids the issue and is used by the C-generation regression.

No claim is made here that imported runtime `::` bindings are globally cached.
The existing LLVM path and C backend's local-constant path both expand these
expressions on use; the repaired C import path follows that behaviour. Code
requiring a per-thread value should prefer an explicit accessor, whose intent
is clear independently of constant-expression expansion.

## Verification of the separate C-generation repair

On native Windows x64, 2026-10-01, a fresh Clang debug compiler passed
`python build/test_cgen.py --nerd _bin/nerd-debug.exe --jobs 4`: 296 differential
fixtures against LLVM at C `-O0` and `-O2`, plus CLI conflicts, output-file locking,
external-library/export modes and Pixels compilation checks. Two platform
fixtures were skipped; Dungeon's Linux PTY check was also explicitly skipped.

The new imported-constant fixture failed against the baseline compiler because
C eagerly executed unused constant initialisers. The initial fix then exposed
and reproduced a caller-local collision with an imported enum payload binder;
the final regression passes with isolated expansion locals. The source files
were formatted, and `git diff --check` passed. No Linux/WSL result or combined
runtime/thread-branch integration result is claimed by this branch.
