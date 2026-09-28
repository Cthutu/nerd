# Vulkan source-owned SDK path — Windows, 2026-09-28

Follows build-settings implementation `7c1cd3e9` on `vulkan`.
`mods/std/vulkan/mod.n` now contributes:

```nerd
build {
    on "windows" {
        library_path: $VULKAN_SDK/Lib
    }
}
```

The unchanged `just run-example vktriangle` command built and linked with
neither `LIB` nor `LIBPATH` set. Its first launch then failed with Vulkan error
`-6` because the requested validation layer was not registered. See
`first-launch.log`.

Ran the installed Scoop SDK's `install-vk-layers.ps1`, which registers its
explicit layer manifests under `HKCU/SOFTWARE/Khronos/Vulkan/ExplicitLayers`.
This is a per-user SDK installation change; no compiler or example wrapper,
`LIB`, `PATH`, or `VK_ADD_LAYER_PATH` workaround was added. Registry evidence is
in `layer-registration.log`. README now describes this runtime prerequisite.

Retried the same command successfully (`registered-launch.log`): Vulkan instance
creation, physical-device discovery and selection of NVIDIA GeForce RTX 4070
SUPER. The example's native window was visible and responsive. Automated
`WM_KEYDOWN Q` sent only to that launched window closed it; the complete Just
command exited **0**. See `window-check.log`. A manual desktop observation was
requested but has not been received; the automated result is not a manual F7
confirmation. This example currently initializes Vulkan and does not draw a
triangle.

Additional checks:

- Both focused Vulkan command/LSP fixtures pass (`vulkan-regressions.log`).
- `--copts` includes the inherited SDK path (`copts.log`).
- With `VULKAN_SDK` removed only from a test child environment, `check` succeeds
  and `--copts` reports the missing variable and declaring module, as intended
  (`environment-checks.log`).

The user's existing changes to `examples/vktriangle/vktriangle.n` were used for
the launch but remain uncommitted and unchanged by this work. Generic compiler
validation and the two known baseline failures are recorded in
[the preceding evidence](../20260928-build-settings/README.md). Linux runtime
validation remains for the Linux handoff. No global Nerd/editor installation
was replaced.
