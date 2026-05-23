# Linux Debugger Imported Source Paths

Date: 2026-05-23

Commit context: breakpoint-quality validation after the initial CodeLLDB bridge.

Host tools:

- Host: `Linux alpha 7.0.9-1-cachyos x86_64`
- `readelf`: GNU Binutils 2.46.0
- CodeLLDB extension: `vadimcn.vscode-lldb-1.12.2`
- Bundled LLDB: `lldb version 22.1.4-codelldb`

## Imported Module

Fixture:

```nerd
use test.folder_mod

main :: fn () -> i32 {
    return answer()
}
```

Relevant decoded line-table paths after `nerd build`:

```text
149-build-imported-debug-info-linux.input.n
tests/mods/test/folder_mod/mod.n
```

Interpretation:

- imported modules produce separate source-file entries in the Linux executable
- source breakpoints in imported module root files should have enough DWARF path
  information for LLDB/CodeLLDB to bind

CodeLLDB bundled LLDB proof:

```text
breakpoint set --file /home/matt/nerd/tests/mods/test/folder_mod/mod.n --line 1
Breakpoint 1: where = imports`@m1.fn.0 + 5 at mod.n:1
```

Running the program stops at `mod.n:1`.

## Folder Module Parts

Probe:

```nerd
use test.parts

main :: fn () -> i32 {
    return part_answer()
}
```

Decoded line-table paths now include both:

```text
tests/mods/test/parts/mod.n
tests/mods/test/parts/body.n
```

Rows for executable code originating in `body.n` are reported under `body.n`.
The function subprogram metadata still starts from the combined folder-module
scope, so fully polished function-entry metadata remains a follow-up.

Interpretation:

- folder-module part line-table paths are source-accurate for executable rows
- full paths should be used for debugger proofs when multiple modules contain a
  `mod.n` basename

CodeLLDB bundled LLDB proof:

```text
breakpoint set --file /home/matt/nerd/tests/mods/test/parts/body.n --line 6
Breakpoint 2: where = imports`@m2.fn.1 + 1 at body.n:6:1
```

Running the program stops at `body.n:6`, the `thing := make_thing(42)` line.
