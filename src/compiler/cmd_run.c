//------------------------------------------------------------------------------
// Run command
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/cmd_internal.h>
#include <compiler/error/error.h>

//------------------------------------------------------------------------------
// Build one artifact set for the `run` command.

internal NerdArtifactConfig
compiler_cmd_run_artifacts(Arena* arena, const NerdRunConfig* config)
{
    NerdArtifactConfig artifacts = compiler_cmd_default_artifacts();

    cstr output_root =
        compiler_cmd_output_root(arena, config->output_path, config->source);

    artifacts.binary_path =
        config->keep_binary ? output_root
                            : compiler_cmd_temp_binary_path(arena, output_root);
    artifacts.hir_path  = compiler_cmd_sidecar_path(arena, output_root, ".hir");
    artifacts.llvm_path = compiler_cmd_sidecar_path(arena, output_root, ".ll");
    artifacts.emit_hir_file  = config->emit_hir;
    artifacts.emit_llvm_file = config->emit_llvm;
    artifacts.release        = config->release;
    artifacts.keywords       = config->keywords;

    return artifacts;
}

//------------------------------------------------------------------------------
// Remove every temporary file shape that `run` can create.

internal bool compiler_cmd_string_starts_with(string text, string prefix)
{
    return text.count >= prefix.count &&
           memcmp(text.data, prefix.data, prefix.count) == 0;
}

internal void compiler_cmd_run_cleanup_generated(
    Arena* arena, const NerdArtifactConfig* artifacts, bool keep_binary)
{
    if (keep_binary) {
        return;
    }

    path_remove(artifacts->binary_path);
#if OS_WINDOWS
    path_remove(path_replace_extension(arena, artifacts->binary_path, ".pdb"));
#endif

    cstr   dir_path      = path_dirname(arena, artifacts->binary_path);
    string binary_prefix = path_filename(s(artifacts->binary_path));

    void*   mark         = arena_store(arena);
    DirIter iter         = {0};
    if (!dir_iter_init(&iter, dir_path)) {
        return;
    }

    cstr path         = NULL;
    bool is_directory = false;
    while (dir_iter_next(&iter, arena, &path, &is_directory)) {
        if (is_directory) {
            arena_restore(arena, mark);
            continue;
        }

        string filename = path_filename(s(path));
        if (compiler_cmd_string_starts_with(filename, binary_prefix)) {
            path_remove(path);
        }
        arena_restore(arena, mark);
    }
    dir_iter_done(&iter);
}

//------------------------------------------------------------------------------
// Build and then execute one Nerd program.

internal bool compiler_cmd_path_is_absolute(cstr path)
{
#if OS_WINDOWS
    if ((path[0] >= 'A' && path[0] <= 'Z') ||
        (path[0] >= 'a' && path[0] <= 'z')) {
        return path[1] == ':' && (path[2] == '\\' || path[2] == '/');
    }
    return path[0] == '\\' && path[1] == '\\';
#elif OS_POSIX
    return path[0] == '/';
#else
    return false;
#endif
}

internal cstr compiler_cmd_absolute_path(Arena* arena, cstr path)
{
    if (compiler_cmd_path_is_absolute(path)) {
        return path;
    }

#if OS_WINDOWS
    DWORD required = GetFullPathNameA(path, 0, NULL, NULL);
    if (required == 0) {
        error_runtime("Failed to resolve absolute path for `%s`", path);
        return NULL;
    }

    char* buffer  = (char*)arena_alloc(arena, required);
    DWORD written = GetFullPathNameA(path, required, buffer, NULL);
    if (written == 0 || written >= required) {
        error_runtime("Failed to resolve absolute path for `%s`", path);
        return NULL;
    }
    return buffer;
#elif OS_POSIX
    char* cwd = getcwd(NULL, 0);
    if (cwd == NULL) {
        error_runtime("Failed to resolve current working directory");
        return NULL;
    }

    // `run hello.n` emits `hello`; execute it by absolute path so POSIX shells
    // do not search PATH and miss binaries in the current directory.
    cstr result = path_join(arena, cwd, path);
    free(cwd);
    return result;
#else
    return path;
#endif
}

int compiler_cmd_run(const NerdRunConfig* config)
{
    Arena arena = {0};
    arena_init(&arena);

    NerdArtifactConfig artifacts = compiler_cmd_run_artifacts(&arena, config);
    Timing             timing    = {0};
    timing_init(&timing);

    bool ok = compile(config->source, &artifacts, config->verbose, &timing);

    if (config->timing) {
        timing_dump(&timing);
    }
    timing_done(&timing);
    if (!ok) {
        compiler_cmd_run_cleanup_generated(
            &arena, &artifacts, config->keep_binary);
        arena_done(&arena);
        return 1;
    }

#if OS_POSIX
    cstr executable_path =
        compiler_cmd_absolute_path(&arena, artifacts.binary_path);
    if (executable_path == NULL) {
        arena_done(&arena);
        return 1;
    }
    string command = string_format(&arena, "\"%s\"", executable_path);
#elif OS_WINDOWS
    cstr executable_path =
        compiler_cmd_absolute_path(&arena, artifacts.binary_path);
    if (executable_path == NULL) {
        arena_done(&arena);
        return 1;
    }
    string command = string_format(&arena, "\"%s\"", executable_path);
#endif

    int result = shell((cstr)command.data);

    if (!config->keep_binary) {
        compiler_cmd_run_cleanup_generated(&arena, &artifacts, false);
    }

    arena_done(&arena);
    return result;
}

//------------------------------------------------------------------------------
