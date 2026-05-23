# Linux Debugger Imported Source Paths

Date: 2026-05-23

Commit context: breakpoint-quality validation after the initial CodeLLDB bridge.

Host tools:

- Host: `Linux alpha 7.0.9-1-cachyos x86_64`
- `readelf`: GNU Binutils 2.46.0

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
- MS4 still needs manual CodeLLDB breakpoint validation in part files before the
  VS Code workflow is considered covered
