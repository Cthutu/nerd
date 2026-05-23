# Linux Debugger Locals

Date: 2026-05-23

Commit context: local implementation for `DEBUGGER.md` Commit 5.

Host tools:

- CodeLLDB extension: `vadimcn.vscode-lldb-1.12.2`
- Bundled LLDB: `lldb version 22.1.4-codelldb`

## Local Values

Command:

```sh
./_bin/nerd-debug build tests/debug/debug_smoke.n \
  -o _tmp/debugger-locals/debug_smoke \
  --llvm
~/.vscode/extensions/vadimcn.vscode-lldb-1.12.2/lldb/bin/lldb \
  -b \
  -o 'target create _tmp/debugger-locals/debug_smoke' \
  -o 'breakpoint set --file debug_smoke.n --line 5' \
  -o 'run' \
  -o 'frame variable' \
  -o 'expr total' \
  _tmp/debugger-locals/debug_smoke
```

Relevant output:

```text
(int) base = 40
(int) bonus = 2
(int) total = 42
(int) $0 = 42
```

## Parameters And Native Watch Expressions

Fixture:

```nerd
add :: fn (left: i32, right: i32) -> i32 {
    total := left + right
    return total
}

main :: fn () -> i32 {
    return add(20, 22)
}
```

Relevant LLDB output at the `total := left + right` line:

```text
(int) left = 20
(int) right = 22
(int) total = <variable not available>
(int) $0 = 42
```

Interpretation:

- primitive parameters are visible by name
- primitive SSA locals become visible after their defining instruction
- native debugger watch expressions can evaluate ordinary visible values
- richer Nerd types, lexical scopes, and source-accurate local declaration lines
  still need follow-up work
