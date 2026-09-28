# Focused Windows Vulkan repair — 2026-09-28

Starting revision: `06c17eec610b227f41ae26239904a264aab096c2`, branch `vulkan`.
Clean working tree at start. Windows 11 x64; Vulkan SDK from
`C:/Users/matt/scoop/apps/vulkan/current`, with `vulkan-1.lib` and Khronos
validation-layer manifests under `Lib` and `Bin`. Runtime GPU: NVIDIA GeForce
RTX 4070 SUPER. Native compiler verification used LLVM/Clang 22.1.8 and the
existing process-local SDK/CRT environment from the September 23 validation.

## Failure and repair

The original F7 task invokes `just run-example vktriangle`; the linker asks for
`vulkan.lib`, which the Windows SDK does not provide. The loader import library
is `vulkan-1.lib`. Platform-selected FFI library constants fix the shared and
platform-specific bindings without duplicating declarations or changing Linux's
`vulkan` name. C output options also select `-lvulkan-1`.

An ordinary editor shell also needs SDK/CRT paths. Setting LIB to just the
Vulkan directory masks default system-library discovery. The new Windows task
wrapper imports x64 Visual Studio development headers/libraries via vswhere and
VsDevCmd, preserving LLVM on PATH, then adds the Vulkan SDK Lib directory. It
selects Clang explicitly to build Nerd. All settings are process-local.

After linking, the example failed with VK_ERROR_LAYER_NOT_PRESENT (-6). The
installed SDK has the validation layer, but the loader was not discovering it.
The wrapper adds its Bin manifests through VK_ADD_LAYER_PATH, preserving
existing additions. See [LunarG layer configuration](https://vulkan.lunarg.com/doc/view/1.4.321.1/windows/layer_configuration.html).
No validation layer was disabled, and no registry/system setting was changed.

## Verification

- `powershell.exe -NoProfile -File build/run-example-windows.ps1 -Example vktriangle -BuildOnly`: PASS from a normal shell without developer LIB settings.
- Same wrapper without BuildOnly (the Windows F7 command): PASS. Native window
  found; output confirms instance creation and RTX 4070 SUPER selection.
  Synthetic Q to the Vulkan window exits the complete task with code 0.
  This is lifecycle/device evidence, not a triangle rendering claim: the current
  example does not yet implement triangle rendering. Human F7 confirmation was
  requested separately and was not assumed.
- `python build/test.py --filter 324-vulkan-surface-invalid-frame`: 1 pass, 0 failures.
- Release `nerd build --copts examples/vktriangle/vktriangle.n`: correct Windows `-lvulkan-1` plus existing system libraries.
- `just test`: **1,160 passed, 2 failed, 15 declared platform skips** in the fixture
  stage. Earlier clean/memory/thread checks and the fresh Clang debug build passed.
  The failed fixture stage stops Just before subsequent auxiliary suites; those
  are not claimed as run. No assertions or skip rules were changed.
- `just format`: PASS; unrelated formatter-only edits were reverted.
- No compiler internals changed. No global installation or extension replacement
  was performed for this focused validation.

The two full-suite failures do not import std.vulkan and persist independently
of this repair:

1. `tests/errors/137-runtime-fixed-arrays.e`, case 7, expects a literal Linux
   absolute-path/target LLVM error for returning a runtime-sized array. Windows
   reports the same i32-versus-i64 invalid-IR error with Windows paths/triple.
   This is an existing compiler/diagnostic regression, not a valid portable error
   expectation; fix the compiler/semantic handling rather than blessing another
   platform-specific LLVM error snapshot.
2. `tests/commands/274-run-std-files-windows.cmd` calls
   `open(output_path, FileMode.Append)` despite the named-default-argument rule.
   The Linux counterpart already uses `mode = FileMode.Append`.

Original linker and layer failures, successful task output, focused regression,
full-suite log and formatter log are retained here. Generated test inputs and
binaries remain outside the commit. Linux follow-up: run the Vulkan surface
regression with its SDK and resolve the runtime-array error test; Windows also
needs its file fixture updated for the current argument syntax. This is not a
new full native validation or benchmark result.
