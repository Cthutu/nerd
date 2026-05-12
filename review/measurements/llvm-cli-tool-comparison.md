# LLVM CLI Tool Comparison

Date: 2026-05-12
Context: local `main` after adding `--timing` and backend wall-clock timings.

## Question

Should Nerd replace the current clang-on-text LLVM path with lower-level LLVM
CLI tools such as `llvm-as` and `llc`?

## Available Tools

```text
clang:   22.1.3
llvm-as: 22.1.3
llc:     22.1.3
opt:     22.1.3
```

## Method

The current executable path was measured with:

```sh
_bin/nerd-debug --timing build -o /tmp/nerd-ms4-small \
    'main :: fn () -> i32 { return 0 }'
```

For direct LLVM CLI checks, the generated side of existing LLVM fixtures was
extracted and compiled to object files:

```sh
awk 'BEGIN{RS="¬"} NR==2{print}' tests/llvm/003-expressions.ll \
    > /tmp/nerd-ms4-expressions.ll
time -p clang -Wno-override-module -c /tmp/nerd-ms4-expressions.ll \
    -o /tmp/nerd-ms4-clang-text.o

time -p llvm-as /tmp/nerd-ms4-expressions.ll \
    -o /tmp/nerd-ms4-expressions.bc
time -p llc -filetype=obj /tmp/nerd-ms4-expressions.bc \
    -o /tmp/nerd-ms4-llc.o
```

The same object-only comparison was repeated with `tests/llvm/024-enums.ll`.
These are coarse shell timings, useful only for direction; they do not include
runtime object linking or external library flags.

## Results

Current executable path from `--timing`:

```text
small inline:
  link executable: 27.013 ms

dynarray fixture:
  link executable: 27.982 ms

module fixture:
  link executable: 27.423 ms
```

Object-only comparison:

```text
003-expressions:
  clang -c text .ll:      real 0.01s
  llvm-as text -> bc:     real 0.00s
  llc bc -> object:       real 0.01s

024-enums:
  clang -c text .ll:      real 0.01s
  llvm-as text -> bc:     real 0.00s
  llc bc -> object:       real 0.01s
```

## Interpretation

The current build-time bottleneck is still the external executable tool step.
For object generation alone, `llvm-as` plus `llc` does not show a meaningful
advantage over `clang -c` at this fixture size; it also adds another process.

The current install contract should remain textual LLVM IR plus clang. A switch
to `llvm-as`/`llc` needs a stronger result on larger programs and a full
executable-link measurement that includes the runtime object and external link
flags.

## Follow-up

If clang startup remains a concern, the next comparison should use a generated
large module and measure complete executable builds:

- current single clang invocation on `.link.ll` plus `nrt.o`
- `llvm-as` to bitcode, `llc` to object, then clang for final system link
- clang consuming bitcode directly

