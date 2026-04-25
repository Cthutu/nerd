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

int compiler_cmd_build(const NerdBuildConfig* config)
{
    Arena arena = {0};
    arena_init(&arena);
    NerdArtifactConfig artifacts = compiler_cmd_build_artifacts(&arena, config);

    Timing timing                = {0};
    timing_init(&timing);
    bool ok = compile(config->source, &artifacts, config->verbose, &timing);
    timing_done(&timing);
    arena_done(&arena);

    return ok ? 0 : 1;
}

//------------------------------------------------------------------------------
