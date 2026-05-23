# Linux Debugger Baseline

Date: 2026-05-23

Commit context: `c8330356 Add debugger roadmap` plus local baseline fixture.

Host tools:

- `readelf`: GNU Binutils 2.46.0
- `clang`: 22.1.5, target `x86_64-pc-linux-gnu`
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
rm -rf _tmp/debugger-baseline
mkdir -p _tmp/debugger-baseline
./_bin/nerd-debug build tests/debug/debug_smoke.n \
  -o _tmp/debugger-baseline/debug_smoke \
  --llvm
```

Result:

- build succeeded
- executable path: `_tmp/debugger-baseline/debug_smoke`
- LLVM sidecar path: `_tmp/debugger-baseline/_debug_smoke.ll`
- running the executable exits with status `42`

The generated LLVM is:

```llvm
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [26 x i8] c"tests/debug/debug_smoke.n\00"

define internal i32 @fn.0() {
  %t0 = add i32 40, 2
  ret i32 %t0
}

@$main = alias i32 (), ptr @fn.0
```

## Executable Sections

Command:

```sh
readelf -S _tmp/debugger-baseline/debug_smoke | rg 'debug|symtab|strtab'
```

Output:

```text
[29] .symtab           SYMTAB
[30] .strtab           STRTAB
[31] .shstrtab         STRTAB
```

`readelf --debug-dump=decodedline _tmp/debugger-baseline/debug_smoke` produced
no decoded line table output.

Interpretation:

- the executable is not stripped
- symbol tables are present
- no DWARF `.debug_*` sections are present
- `-g -O0` on clang is not enough because generated LLVM does not yet carry
  Nerd source-level debug metadata

## Symbols

Command:

```sh
nm -an _tmp/debugger-baseline/debug_smoke | rg 'fn\.0|\$main|main'
```

Output:

```text
                 U __libc_start_main@GLIBC_2.34
0000000000001400 T $main
0000000000001400 t fn.0
0000000000001410 T main
```

Interpretation:

- generated and entry wrapper symbols exist
- there is no source-line information connecting those symbols to
  `tests/debug/debug_smoke.n`

## LLDB Breakpoint Attempt

Command:

```sh
~/.vscode/extensions/vadimcn.vscode-lldb-1.12.2/lldb/bin/lldb \
  -b \
  -o 'target create _tmp/debugger-baseline/debug_smoke' \
  -o 'breakpoint set --file tests/debug/debug_smoke.n --line 4' \
  -o 'breakpoint list' \
  -o 'run' \
  _tmp/debugger-baseline/debug_smoke
```

Relevant output:

```text
Breakpoint 1: no locations (pending).
WARNING:  Unable to resolve breakpoint to any actual locations.
1: file = 'tests/debug/debug_smoke.n', line = 4, exact_match = 0, locations = 0 (pending)
Process ... exited with status = 42 (0x0000002a)
```

Interpretation:

- CodeLLDB's bundled LLDB can load and run the executable
- a breakpoint in the Nerd source file remains pending
- MS1 needs LLVM `!dbg` metadata for compile units, files, functions, and line
  locations before CodeLLDB can bind `.n` source breakpoints

