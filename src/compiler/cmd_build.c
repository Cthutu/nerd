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

    artifacts.binary_path = output_root;
    artifacts.hir_path = compiler_cmd_sidecar_path(arena, output_root, ".hir");
    artifacts.ir_path  = compiler_cmd_sidecar_path(arena, output_root, ".ir");
    artifacts.llvm_path = compiler_cmd_sidecar_path(arena, output_root, ".ll");
    artifacts.c_path = compiler_cmd_sidecar_path(arena, output_root, ".gen.c");
    artifacts.emit_hir_file = config->emit_hir;
    artifacts.emit_ir_file  = config->emit_ir;
    artifacts.emit_llvm_file = config->emit_llvm;
    artifacts.emit_c_file   = config->emit_c;
    artifacts.release       = config->release;
    artifacts.keywords      = config->keywords;

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
