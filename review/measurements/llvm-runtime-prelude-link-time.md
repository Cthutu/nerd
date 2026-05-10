# LLVM Runtime Prelude Link Time

Date: 2026-05-10
Context: `ffc2a462 Generate LLVM epilogue wrapper in backend` plus local
working-tree changes.

## Question

Should the LLVM backend embed the runtime prelude as textual LLVM IR
(`prelude.ll`) or bitcode (`prelude.bc`)?

## Method

Generated a bitcode prelude locally:

```sh
clang -emit-llvm -c -O0 -Xclang -disable-O0-optnone -std=gnu23 \
    data/prelude.c -o /tmp/nerd-link-measure/prelude.bc
```

Compared repeated clang link invocations using:

- textual prelude: `_obj/llvm/prelude.ll`
- bitcode prelude: `/tmp/nerd-link-measure/prelude.bc`

Both runs linked the same generated Nerd LLVM module, generated `init.ll`, and
generated `main` wrapper.

## Results

Small program: `/home/matt/tmp/test.n`

```text
prelude.ll average: 64 ms
prelude.bc average: 63 ms
```

Larger generated module: source extracted from
`tests/language/101-dynarray-typed-locals.t`

```text
prelude.ll average: 55 ms
prelude.bc average: 55 ms
```

## Decision

Keep embedding textual `prelude.ll` for now. Bitcode does not show a meaningful
link-time improvement in these checks, and textual LLVM IR is easier to inspect
while the backend is still changing quickly.

Revisit bitcode if the prelude grows substantially, if release-mode timings
change the result, or if the backend moves toward a lower-level LLVM toolchain
that consumes bitcode directly.
