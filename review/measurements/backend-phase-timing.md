# Backend Phase Timing

Date: 2026-05-12
Context: local `main` working tree while adding backend timing entries.

## Question

How much of a small Nerd build is spent in LLVM rendering/combining/runtime
file writes versus the external clang invocation?

## Instrumentation

The compiler now records back-end timing entries when `--timing` is passed:

- `render module LLVM`
- `combine LLVM text`
- `write combined LLVM`
- `write runtime object`
- `link executable`

Back-end timings use wall-clock time so the external clang process is included.
Front-end timings still use the existing per-thread timing.

## Environment

- Linux alpha 7.0.3-1-cachyos x86_64
- clang 22.1.3
- debug compiler: `_bin/nerd-debug`

## Commands

Small inline program:

```sh
_bin/nerd-debug --timing build -o /tmp/nerd-ms4-small \
    'main :: fn () -> i32 { return 0 }'
```

Dynamic-array fixture:

```sh
awk 'BEGIN{RS="¬"} NR==1{print}' \
    tests/language/101-dynarray-typed-locals.t > /tmp/nerd-ms4-dynarray.n
_bin/nerd-debug --timing build -o /tmp/nerd-ms4-dynarray \
    /tmp/nerd-ms4-dynarray.n
```

Module fixture:

```sh
awk 'BEGIN{RS="¬"} NR==1{print}' \
    tests/commands/021-run-llvm-module.cmd > /tmp/nerd-ms4-module.n
NERD_LIB_PATH=/home/matt/nerd/tests/mods:/home/matt/nerd/mods \
    _bin/nerd-debug --timing build -o /tmp/nerd-ms4-module \
    /tmp/nerd-ms4-module.n
```

## Representative Results

```text
small inline:
  render module LLVM:   17.543 us
  combine LLVM text:     3.246 us
  write combined LLVM:  19.627 us
  write runtime object: 11.080 us
  link executable:      27.013 ms
  back-end total:       27.065 ms

dynarray fixture:
  render module LLVM:   100.368 us
  combine LLVM text:     40.185 us
  write combined LLVM:   20.238 us
  write runtime object:   8.425 us
  link executable:       27.982 ms
  back-end total:        28.151 ms

module fixture:
  render module LLVM:    58.410 us
  combine LLVM text:      6.041 us
  write combined LLVM:   13.696 us
  write runtime object:  13.275 us
  link executable:       27.423 ms
  back-end total:        27.514 ms
```

## Interpretation

For these small builds, clang dominates the executable back end. LLVM text
rendering, combined-input construction, and runtime object writing are all in
microseconds, while the external link/compile step is roughly 27-28 ms.

This supports keeping optimisation attention on external tool invocation and
toolchain shape before spending much effort micro-optimising string-builder
growth. The next useful measurement is a direct comparison against LLVM CLI
alternatives (`llvm-as`, `llc`, `opt`, bitcode/object flows) using the same
fixtures.

