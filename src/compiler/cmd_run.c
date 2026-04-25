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

    cstr output_root             = NULL;
    if (config->output_path.count > 0) {
        output_root = compiler_cmd_copy_path(arena, config->output_path);
    } else if (config->source.source_path.count > 0) {
        cstr source_path =
            compiler_cmd_copy_path(arena, config->source.source_path);
        output_root = path_replace_extension(arena, source_path, "");
    } else {
        output_root = artifacts.binary_path;
    }

    artifacts.binary_path  = output_root;
    artifacts.ir_path      = path_replace_extension(arena, output_root, ".ir");
    artifacts.c_path       = path_replace_extension(arena, output_root, ".c");
    artifacts.emit_ir_file = config->emit_ir;
    artifacts.emit_c_file  = config->emit_c;
    artifacts.release      = config->release;
    artifacts.keywords     = config->keywords;

    return artifacts;
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

    timing_done(&timing);
    if (!ok) {
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

    if (!config->keep_binary && !path_remove(artifacts.binary_path)) {
        error_runtime("Failed to remove generated executable: %s",
                      artifacts.binary_path);
        arena_done(&arena);
        return 1;
    }

    arena_done(&arena);
    return result;
}

//------------------------------------------------------------------------------
