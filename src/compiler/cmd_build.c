//------------------------------------------------------------------------------
// Build command
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/cmd_internal.h>

//------------------------------------------------------------------------------

internal NerdArtifactConfig
compiler_cmd_build_artifacts(Arena* arena, const NerdBuildConfig* config)
{
    NerdArtifactConfig artifacts = compiler_cmd_default_artifacts();

    cstr output_root =
        compiler_cmd_output_root(arena, config->output_path, config->source);

    switch (config->output_kind) {
    case NERD_BUILD_OUTPUT_Executable:
        artifacts.binary_path =
            compiler_cmd_build_binary_path(arena, output_root);
        break;
    case NERD_BUILD_OUTPUT_Object:
        if (config->output_path.count > 0) {
            artifacts.binary_path = output_root;
        } else {
#if OS_WINDOWS
            artifacts.binary_path = compiler_cmd_output_path_with_extension(
                arena, output_root, ".obj");
#else
            artifacts.binary_path = compiler_cmd_output_path_with_extension(
                arena, output_root, ".o");
#endif
        }
        break;
    case NERD_BUILD_OUTPUT_StaticLibrary:
        if (config->output_path.count > 0) {
            artifacts.binary_path = output_root;
        } else {
#if OS_WINDOWS
            artifacts.binary_path = compiler_cmd_output_path_with_extension(
                arena, output_root, ".lib");
#else
            artifacts.binary_path = compiler_cmd_output_path_with_extension(
                arena, output_root, ".a");
#endif
        }
        break;
    case NERD_BUILD_OUTPUT_SharedLibrary:
        if (config->output_path.count > 0) {
            artifacts.binary_path = output_root;
        } else {
#if OS_WINDOWS
            artifacts.binary_path = compiler_cmd_output_path_with_extension(
                arena, output_root, ".dll");
#elif OS_MACOS
            artifacts.binary_path = compiler_cmd_output_path_with_extension(
                arena, output_root, ".dylib");
#else
            artifacts.binary_path = compiler_cmd_output_path_with_extension(
                arena, output_root, ".so");
#endif
        }
        break;
    }
    artifacts.hir_path  = compiler_cmd_sidecar_path(arena, output_root, ".hir");
    artifacts.llvm_path = compiler_cmd_sidecar_path(arena, output_root, ".ll");
    artifacts.emit_hir_file  = config->emit_hir;
    artifacts.emit_llvm_file = config->emit_llvm;
    artifacts.output_kind    = config->output_kind;
    artifacts.require_entry_point =
        config->output_kind == NERD_BUILD_OUTPUT_Executable;
    artifacts.release  = config->release;
    artifacts.keywords = config->keywords;

    return artifacts;
}

//------------------------------------------------------------------------------

internal bool compiler_cmd_build_string_ends_with_cstr(string text, cstr suffix)
{
    usize suffix_len = strlen(suffix);
    return text.count >= suffix_len &&
           memcmp(text.data + text.count - suffix_len, suffix, suffix_len) == 0;
}

internal bool compiler_cmd_build_is_module_llvm(string filename,
                                                string base_stem)
{
    if (filename.count <= base_stem.count + 4 ||
        memcmp(filename.data, base_stem.data, base_stem.count) != 0 ||
        filename.data[base_stem.count] != '.' ||
        filename.data[base_stem.count + 1] != 'm' ||
        !compiler_cmd_build_string_ends_with_cstr(filename, ".ll")) {
        return false;
    }

    for (usize i = base_stem.count + 2; i + 3 < filename.count; ++i) {
        if (filename.data[i] < '0' || filename.data[i] > '9') {
            return false;
        }
    }
    return true;
}

internal void compiler_cmd_build_cleanup_stale_sidecars(Arena* arena,
                                                        cstr   binary_path)
{
    cstr dir_path = path_dirname(arena, binary_path);
    if (dir_path == NULL || dir_path[0] == '\0') {
        dir_path = ".";
    }
    string  base_stem = path_stem(path_filename(s(binary_path)));
    void*   mark      = arena_store(arena);
    DirIter iter      = {0};
    if (!dir_iter_init(&iter, dir_path)) {
        return;
    }

    cstr path         = NULL;
    bool is_directory = false;
    while (dir_iter_next(&iter, arena, &path, &is_directory)) {
        if (!is_directory && compiler_cmd_build_is_module_llvm(
                                 path_filename(s(path)), base_stem)) {
            path_remove(path);
        }
        arena_restore(arena, mark);
    }
    dir_iter_done(&iter);
}

//------------------------------------------------------------------------------

int compiler_cmd_build(const NerdBuildConfig* config)
{
    Arena arena = {0};
    arena_init(&arena);
    NerdArtifactConfig artifacts = compiler_cmd_build_artifacts(&arena, config);
    compiler_cmd_build_cleanup_stale_sidecars(&arena, artifacts.binary_path);

    Timing timing = {0};
    timing_init(&timing);
    bool ok = compile(config->source, &artifacts, config->verbose, &timing);
    if (config->timing) {
        timing_dump(&timing);
    }
    timing_done(&timing);
    arena_done(&arena);

    return ok ? 0 : 1;
}

//------------------------------------------------------------------------------
