# LLVM Single Input Link Time

Date: 2026-05-11
Context:

- previous multi-input pipeline: `1c9e8fe3`
- current combined-input pipeline: `01528171`

Environment:

- Linux alpha 7.0.3-1-cachyos x86_64
- clang 22.1.3

## Question

Does concatenating the embedded prelude, generated module LLVM IR, epilogue, and
init wrapper into one temporary `.link.ll` file improve small-program build
time compared with invoking clang on separate LLVM IR inputs?

## Method

Built a release compiler for both commits. The previous compiler was built in a
temporary detached worktree at `/tmp/nerd-measure-prev`; the current compiler
used this checkout.

Each command had two warmup runs and 25 measured runs. Timings cover the full
`nerd build` command, including front end, HIR lowering, LLVM rendering, clang
invocation, and output executable write.

Small file case:

```sh
nerd build -o /tmp/nerd-measure-... /home/matt/tmp/test.n
```

Dynarray case:

```sh
nerd build -o /tmp/nerd-measure-... "$(cat /tmp/nerd-measure-dynarray.n)"
```

The dynarray source was extracted from
`tests/language/101-dynarray-typed-locals.t`.

Multi-module case:

```sh
NERD_LIB_PATH=/home/matt/nerd/tests/mods:/home/matt/nerd/mods \
    nerd build -o /tmp/nerd-measure-... \
    tests/commands/021-run-llvm-module.input.n
```

The source was the command test body from
`tests/commands/021-run-llvm-module.cmd`.

## Results

```text
previous_multi_input small_file:     avg=61.7ms median=61.2ms min=57.7ms max=75.9ms
previous_multi_input dynarray_inline: avg=54.6ms median=54.5ms min=50.7ms max=61.6ms
current_combined_input small_file:    avg=29.6ms median=29.5ms min=27.9ms max=31.6ms
current_combined_input dynarray_inline: avg=29.8ms median=29.7ms min=28.7ms max=31.6ms

previous_multi_input module_file:     avg=73.8ms median=74.2ms min=69.0ms max=79.8ms
current_combined_input module_file:    avg=31.1ms median=30.9ms min=29.3ms max=34.3ms
```

## Interpretation

The combined-input pipeline roughly halves these small-build timings. This is
large enough to keep the single `.link.ll` path as the default LLVM build path.

The module-import check shows the same direction: the current path avoids
passing several separate LLVM inputs to clang, and that matters even for a tiny
two-module program.

The next useful measurement is a larger real application. Until that says
otherwise, the next backend simplification should focus on removing remaining
legacy IR/C-facing assumptions rather than replacing clang with lower-level LLVM
CLI tools immediately.
