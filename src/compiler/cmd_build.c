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
        output_root =
            (cstr)string_format(arena, STRINGP, STRINGV(config->output_path))
                .data;
    } else if (config->source_path.count > 0) {
        output_root =
            path_replace_extension(arena, (cstr)config->source_path.data, "");
    } else {
        output_root = artifacts.binary_path;
    }

    artifacts.binary_path  = output_root;
    artifacts.ir_path      = path_replace_extension(arena, output_root, ".ir");
    artifacts.c_path       = path_replace_extension(arena, output_root, ".c");
    artifacts.emit_ir_file = config->emit_ir;
    artifacts.emit_c_file  = config->emit_c;

    return artifacts;
}

int compiler_cmd_build(const NerdBuildConfig* config)
{
    compiler_cmd_print_source_overview(config->source);

    Arena arena = {0};
    arena_init(&arena);
    NerdArtifactConfig artifacts = compiler_cmd_build_artifacts(&arena, config);

    Timing timing                = {0};
    timing_init(&timing);
    bool ok = compiler_cmd_run_pipeline_once(
        config->source, &artifacts, true, &timing);
    timing_dump(&timing);
    timing_done(&timing);
    arena_done(&arena);

    return ok ? 0 : 1;
}

//------------------------------------------------------------------------------
