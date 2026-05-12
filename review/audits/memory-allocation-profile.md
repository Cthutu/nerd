# Memory Allocation Profile

Status: started

## Baselines

Date: 2026-05-12

Branch: `main`

Instrumentation:

- `NERD_MEMORY_PROFILE=1` enables phase-level memory counters on stderr.
- Counters are process-local and cumulative snapshots are diffed around phase
  boundaries.
- Heap counters track `mem_alloc`, `mem_realloc`, and `mem_free`.
- Arena counters track arena init/done, committed bytes, allocation count, and
  requested arena bytes.
- Dynamic-array counters track `array_maybe_grow` events and resulting array
  capacity bytes.

Important interpretation notes:

- Arena reserved virtual address space is intentionally not counted as
  allocated memory. Committed pages are counted.
- `heap_net_bytes` is a non-negative current-live delta for the phase. If a
  phase frees memory allocated earlier, use `heap_freed_bytes` for that churn.
- `heap_peak_bytes` is process peak at the end of the phase, not phase-local
  peak.
- Linker work is mostly external `clang`, so in-process counters only show
  command construction.

### Build: Language Fixture

Command:

```sh
NERD_MEMORY_PROFILE=1 _bin/nerd-debug build --llvm \
  tests/language/153-break-on.t --output /tmp/nerd-ms7-break-on
```

Result:

| stage | phase | heap allocs | heap reallocs | heap frees | heap bytes | arena allocs | arena bytes | arena commits | array growths | array bytes |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| front-end | tokenise source text | 10 | 12 | 4 | 1,840 | 15 | 313 | 1 | 22 | 4,448 |
| front-end | parse tokens into AST | 18 | 11 | 4 | 1,152 | 0 | 0 | 0 | 29 | 9,616 |
| front-end | analyse AST semantics | 197 | 103 | 174 | 6,812 | 6 | 61 | 1 | 300 | 27,596 |
| front-end | generate HIR from sema | 37 | 17 | 13 | 2,696 | 5 | 273 | 1 | 54 | 29,984 |
| back-end | render module LLVM | 17 | 7 | 15 | 1,640 | 533 | 7,847 | 2 | 24 | 2,784 |
| back-end | combine LLVM text | 2 | 1 | 2 | 144 | 434 | 6,654 | 0 | 3 | 256 |
| back-end | write runtime object | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |
| back-end | link executable | 0 | 0 | 0 | 0 | 5 | 252 | 0 | 0 | 0 |

Initial read:

- Sema has the largest dynamic-array churn in this small build.
- HIR generation has modest heap count but large array capacity growth relative
  to source size.
- LLVM rendering uses arenas heavily, mostly through string construction.
- Combined LLVM text construction is arena-heavy but heap-light.

### Formatter: Partial Edge Fixture

Command:

```sh
awk 'BEGIN{p=1} /^¬$/{exit} {print}' \
  tests/format/107-token-trivia-partial-edge-cases.f \
  > /tmp/nerd-ms7-format.n
NERD_MEMORY_PROFILE=1 _bin/nerd-debug format --stdout /tmp/nerd-ms7-format.n
```

Result:

| stage | phase | heap allocs | heap reallocs | heap frees | heap bytes | arena allocs | arena bytes | arena commits | array growths | array bytes |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| formatter | format source | 53 | 32 | 53 | 5,444 | 143 | 992 | 10 | 85 | 8,604 |

Initial read:

- The formatter uses many short-lived arenas, which commit at least one page
  each. This is a good target for scratch arena reuse.
- Dynamic-array growth is visible even on a tiny partial source fixture.

### LSP: Incremental Partial Edit Fixture

Command:

```sh
NERD_MEMORY_PROFILE=1 _bin/nerd-debug lsp
```

Input was the framed request sequence from
`tests/lsp/101-incremental-definition-after-partial-edit.lsp`.

Result:

| stage | phase | heap allocs | heap reallocs | heap frees | heap bytes | arena allocs | arena bytes | arena commits | array growths | array bytes |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| lsp | analyse document, didOpen | 61 | 67 | 26 | 7,300 | 22 | 310 | 5 | 128 | 14,236 |
| lsp | analyse document, didChange | 25 | 9 | 25 | 6,984 | 441 | 1,955 | 4 | 34 | 8,000 |

Initial read:

- LSP reanalysis frees old runtime state and rebuilds front-end products.
- The didChange path is heap-neutral in this fixture but still creates several
  arenas and many tiny arena allocations.
- Partial parsing during the edit accounts for most arena allocations in the
  second LSP analysis.

## Follow-Up Targets

- Reuse scratch arenas in formatter block formatting and token fallback.
- Pre-size common sema/HIR dynamic arrays where source counts are already known.
- Keep backend LLVM rendering arena based, but avoid unnecessary helper arrays
  in small modules.
- Keep LSP document lifetime arenas, but reduce temporary per-request arenas
  during incremental reanalysis.

## Ownership Classification

Accepted decision: `review/decisions/0006-memory-ownership-strategy.md`.

Summary:

- Arenas own phase-lifetime products, request-lifetime response data, and
  scratch text.
- Dynamic arrays own growing compiler tables.
- Direct heap allocations should be either dynamic-array backing stores,
  explicit map/interner storage, or ownership that genuinely escapes a phase.

The current measurements match that model overall. The main pressure points are
not unexpected direct heap ownership; they are dynamic-array growth in sema/HIR
tables and short-lived arena creation in formatter/LSP paths.
