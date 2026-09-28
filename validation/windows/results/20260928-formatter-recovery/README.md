# Formatter error isolation — 2026-09-28

Starting commit: `55648026`, branch `vulkan`. The user's local edits to
`examples/vktriangle/vktriangle.n` were preserved and excluded from the commit.

A parse error in one function previously sent an entire comment-delimited code
block through token-stream formatting, losing structured parameter alignment in
valid neighbouring functions. Recovery now identifies balanced, line-leading
top-level declaration boundaries, uses CST formatting for valid runs, and limits
token formatting to damaged regions. Continuation operators and split `pub`
modifiers do not start a new region. Unclosed delimiters remain conservative:
recovery does not guess boundaries inside them. No compiler syntax is changed.

Four regressions cover errors before, between and after healthy callback
functions, plus multiline headers and grouped declaration alignment. Fixtures
also verify idempotence and preservation of comments and malformed tokens.
Three prior recovery snapshots now reflect normal formatting of healthy regions:
plex-field alignment (111), a trailing array comma before its retained comment
(183), and spacing between declarations (195). Their source/error inputs remain
unchanged.

Native Windows, existing LLVM/Clang 22.1.8 SDK/CRT setup:

- Final debug formatter suite: **206 passed, 0 failed**, including idempotence.
- Final release formatter suite: **206 passed, 0 failed**, including idempotence.
- Final LSP suite: **211 passed, 0 failed**.
- `just test` before the final continuation-boundary refinement: **1,163 passed,
  2 failed, 15 platform skips**. Clean/memory/thread prerequisites passed; the
  fixture failures stop later auxiliary stages. Final refinement was rechecked
  with the complete formatter and LSP suites listed above.
- The two full-suite failures match the preceding Vulkan run:
  `137-runtime-fixed-arrays.e` expects a Linux-specific LLVM invalid-IR error;
  `274-run-std-files-windows.cmd` uses obsolete positional default-argument syntax.
  Neither test was weakened or skipped. See the adjacent Vulkan repair report.

Reproduction:

```powershell
. ./validation/windows/results/20260923T084624Z-7d394bf0/environment.ps1
$env:LIB += ";$env:VULKAN_SDK/Lib"
python build/build.py nerd --skip-mod-sync
python build/test.py --filter tests/format
python build/test.py --filter tests/lsp
python build/build.py -r nerd --skip-mod-sync
```

For the release formatter run, the unchanged Python fixture harness was loaded
with `test.NERD = test.ROOT / '_bin/nerd.exe'`, then `test.main()` ran with
`--filter tests/format`. Both fresh workspace binaries include the fix. No global
installation or user example modifications were made. Linux follow-up: run the
formatter/LSP suites and address the two already-documented baseline failures.
