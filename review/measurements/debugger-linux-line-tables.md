# Linux Debugger Line Tables

Date: 2026-05-23

Commit context: local implementation for `DEBUGGER.md` Commit 3.

Host tools:

- Host: `Linux alpha 7.0.9-1-cachyos x86_64`
- `readelf`: GNU Binutils 2.46.0
- CodeLLDB extension: `vadimcn.vscode-lldb-1.12.2`
- Bundled LLDB: `lldb version 22.1.4-codelldb`

## Fixture

Fixture path:

```text
tests/debug/debug_smoke.n
```

Source:

```nerd
main :: fn () -> i32 {
    base := 40
    bonus := 2
    total := base + bonus
    return total
}
```

## Build

Command:

```sh
rm -rf _tmp/debugger-line-tables
mkdir -p _tmp/debugger-line-tables
./_bin/nerd-debug build tests/debug/debug_smoke.n \
  -o _tmp/debugger-line-tables/debug_smoke \
  --llvm
```

Result:

- build succeeded
- executable path: `_tmp/debugger-line-tables/debug_smoke`
- LLVM sidecar path: `_tmp/debugger-line-tables/_debug_smoke.ll`

## Decoded Line Table

Command:

```sh
readelf --debug-dump=decodedline _tmp/debugger-line-tables/debug_smoke
```

Relevant output:

```text
Contents of the .debug_line section:

debug_smoke.n:
File name                        Line number    Starting address    View    Stmt

/home/matt/nerd/tests/debug/debug_smoke.n:
debug_smoke.n                              4              0x1400               x
debug_smoke.n                              5              0x1408               x
debug_smoke.n                              -              0x1409
```

Interpretation:

- the Linux executable now contains DWARF line-table data
- the first emitted source rows map to Nerd source lines 4 and 5
- lines 2 and 3 currently fold into constants and do not produce executable
  instructions

## CodeLLDB LLDB Breakpoint

Command:

```sh
~/.vscode/extensions/vadimcn.vscode-lldb-1.12.2/lldb/bin/lldb \
  -b \
  -o 'target create _tmp/debugger-line-tables/debug_smoke' \
  -o 'breakpoint set --file debug_smoke.n --line 4' \
  -o 'breakpoint list' \
  -o 'run' \
  _tmp/debugger-line-tables/debug_smoke
```

Relevant output:

```text
Breakpoint 1: where = debug_smoke`@fn.0 at debug_smoke.n:4:1, address = 0x0000000000001400
Process ... stopped
* thread #1, name = 'debug_smoke', stop reason = breakpoint 1.1
    frame #0: ... debug_smoke`@fn.0 at debug_smoke.n:4:1
-> 4        total := base + bonus
```

Interpretation:

- CodeLLDB's bundled LLDB can bind a breakpoint in a `.n` file
- execution stops on the expected Nerd source line
- function display still exposes the generated linkage name `@fn.0`; improving
  user-facing function names belongs with the next debug metadata slices
