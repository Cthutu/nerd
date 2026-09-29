# Windowed Windows standard streams — 2026-09-29

Branch `vulkan`, starting at `26c3982c`. The user's local
`frame_system.close(^main_window)` edit remains separate. Clang 22.1.8 builds
fresh debug/release compilers with the existing Windows SDK/CRT environment.
Nerd continues to generate binaries through LLVM tools directly.

The native and generated-C GUI entry points now call `nrt_windows_init_stdio`
before initialization and main. It preserves inherited redirections, attaches
to an existing parent console when needed, repairs missing C standard streams
with owned handle duplicates, and makes connected console output unbuffered.
It never allocates a console. No terminal wrapper, source setting, or new flag
is needed; `windowed: yes` remains enabled.

Evidence:

- `baseline.log`: the new test against the previous compiler fails because a
  windowed child produces neither stdout nor stderr in its parent's console.
- `debug-build.log`, `release-build.log`: fresh Clang builds pass.
- `stdio-debug.log`, `stdio-release.log`: the integration test passes using both
  compiler binaries. Each exercises LLVM and generated-C programs at debug and
  release settings, including live output before process exit, explicit console
  handle inheritance, mixed file/console stdout and stderr, redirected stdin and
  output pipes, separate output files, and detached launches with no console.
  The console-owning test parent is hidden. Detached children verify through
  `GetConsoleCP` that startup has not allocated a console.
- `pty-vulkan.log`: the unchanged `just run-example vktriangle` command was run
  in a real Windows pseudoterminal. Instance/device/queue-family messages appeared
  while the native window stayed open. An Escape event sent only to that window
  closed it and the whole Just command exited 0.
- `toolchain-release.log`: direct LLVM outputs, entry-point integer widths,
  signedness/arguments/target modes, dynamic enum alignment and no-Clang/doctor
  checks pass.
- `fixtures.log`: complete existing fixture run, including formatter/LSP and
  examples: 1,169 passed, three failures, 15 platform skips. One failure was a
  stale LSP location snapshot from the earlier six-line `std.vulkan` build block;
  that snapshot is corrected, and `lsp-final.log` shows all 212 LSP tests passing.
  The remaining two failures are unchanged baselines:
  `137-runtime-fixed-arrays.e` case 7 (invalid LLVM array-length type and a
  Linux-specific diagnostic snapshot), and `274-run-std-files-windows.cmd`
  (stale positional `FileMode.Append` argument). No fully green suite is claimed.

The user confirmed the earlier F7 launch opened the frame and Escape closed it,
but reported no terminal text. A repeat F7 observation with this change was
requested; automated pseudoterminal evidence is distinct from that manual check.
No globally installed compiler or editor extension was replaced.

Linux execution is not claimed. The helper and headers are Windows-only, and
normal console entry points and library initialization are unchanged. The new
test is part of `just test` and reports a platform skip elsewhere.
