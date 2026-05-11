# LLVM Runtime Bridge Experiment

Status: spike

This experiment checks whether an LLVM backend can reuse the existing C
runtime bridge before rewriting the prelude/postlude in handwritten LLVM IR.

The experiment:

- compiles `data/prelude.c` to LLVM IR with clang
- writes the tiny `@main` epilogue wrapper directly as LLVM IR
- links both with a hand-written LLVM module that defines `@"$main"` and
  `@init`; the script copies clang's target triple/data layout into this module
  before linking
- builds and runs the resulting executable

Run:

```sh
python3 review/experiments/llvm-runtime-bridge/run.py
```

Generated files are written to `tmp/llvm-runtime-bridge/`.

Notes:

- The epilogue wrapper only adds prototypes for `init` and `$main`, calls
  `@init`, then returns the result of `@"$main"`.
- Nerd-visible symbols keep the `$` prefix. The hand-written test program
  defines `@"$main"`.
- Clang currently accepts `$` in C identifiers as an extension. The experiment
  passes `-Wno-dollar-in-identifier-extension` so that the bridge is explicit.
