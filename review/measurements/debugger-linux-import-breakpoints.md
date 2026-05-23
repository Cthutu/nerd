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

Current decoded line-table paths include `tests/mods/test/parts/mod.n`, but the
line rows for code originating in `tests/mods/test/parts/body.n` are also
reported under `mod.n`.

Interpretation:

- folder-module part debug paths are not source-accurate yet
- MS4 should preserve part-file source paths through module part expansion before
  claiming breakpoints in part files
