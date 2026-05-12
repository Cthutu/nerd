# Memory Phase Profile

Date: 2026-05-12

Purpose: baseline phase-level allocation counters for MS7.

Instrumentation:

- Set `NERD_MEMORY_PROFILE=1` to emit one tab-separated `memory` row per
  measured phase on stderr.
- Heap counters cover `mem_alloc`, `mem_realloc`, and `mem_free`.
- Arena counters cover arena init/done, committed bytes, allocation count, and
  requested arena bytes.
- Dynamic-array counters cover `array_maybe_grow` events and resulting capacity
  bytes.

Representative commands:

```sh
NERD_MEMORY_PROFILE=1 _bin/nerd-debug build --llvm \
  tests/language/153-break-on.t --output /tmp/nerd-ms7-break-on

awk 'BEGIN{p=1} /^¬$/{exit} {print}' \
  tests/format/107-token-trivia-partial-edge-cases.f \
  > /tmp/nerd-ms7-format.n
NERD_MEMORY_PROFILE=1 _bin/nerd-debug format --stdout /tmp/nerd-ms7-format.n

NERD_MEMORY_PROFILE=1 _bin/nerd-debug lsp
```

The LSP input was the framed request sequence from
`tests/lsp/101-incremental-definition-after-partial-edit.lsp`.

| scenario | stage | phase | heap allocs | heap reallocs | heap frees | heap bytes | arena allocs | arena bytes | arena commits | array growths | array bytes |
| --- | --- | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| build | front-end | tokenise source text | 10 | 12 | 4 | 1,840 | 15 | 313 | 1 | 22 | 4,448 |
| build | front-end | parse tokens into AST | 18 | 11 | 4 | 1,152 | 0 | 0 | 0 | 29 | 9,616 |
| build | front-end | analyse AST semantics | 197 | 103 | 174 | 6,812 | 6 | 61 | 1 | 300 | 27,596 |
| build | front-end | generate HIR from sema | 37 | 17 | 13 | 2,696 | 5 | 273 | 1 | 54 | 29,984 |
| build | back-end | render module LLVM | 17 | 7 | 15 | 1,640 | 533 | 7,847 | 2 | 24 | 2,784 |
| build | back-end | combine LLVM text | 2 | 1 | 2 | 144 | 434 | 6,654 | 0 | 3 | 256 |
| build | back-end | write runtime object | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |
| build | back-end | link executable | 0 | 0 | 0 | 0 | 5 | 252 | 0 | 0 | 0 |
| format | formatter | format source | 53 | 32 | 53 | 5,444 | 143 | 992 | 10 | 85 | 8,604 |
| lsp | lsp | analyse document, didOpen | 61 | 67 | 26 | 7,300 | 22 | 310 | 5 | 128 | 14,236 |
| lsp | lsp | analyse document, didChange | 25 | 9 | 25 | 6,984 | 441 | 1,955 | 4 | 34 | 8,000 |

Notes:

- Arena reserved virtual address space is not counted as allocated memory;
  committed pages are counted.
- Linker work happens in external `clang`, so in-process counters only show
  command construction.
- The formatter and LSP paths create multiple short-lived arenas even for small
  inputs.

## After Initial Churn Reduction

Changes:

- Formatter block formatting now passes source slices directly instead of
  allocating a temporary arena and copied block text for each code chunk.
- Backend LLVM module arrays are pre-sized from the known module count.

Formatter result on the same partial edge fixture:

| scenario | stage | phase | arena inits | arena allocs | arena bytes | arena commits | arena commit bytes |
| --- | --- | --- | ---: | ---: | ---: | ---: | ---: |
| before | formatter | format source | 10 | 143 | 992 | 10 | 655,360 |
| after | formatter | format source | 9 | 142 | 872 | 9 | 589,824 |

The formatter change removes one short-lived arena and one copied block slice
for this small fixture. Larger files with more code chunks should save one arena
init/commit per chunk that previously needed the copy.
