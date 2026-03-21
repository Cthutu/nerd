//------------------------------------------------------------------------------
// Build command
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/cmd_internal.h>

//------------------------------------------------------------------------------

int compiler_cmd_build(const NerdBuildConfig* config)
{
    compiler_cmd_print_source_overview(config->source);

    NerdArtifactConfig artifacts = compiler_cmd_default_artifacts();
    artifacts.emit_ir_file       = config->emit_ir;
    artifacts.emit_c_file        = config->emit_c;

    Timing timing = {0};
    timing_init(&timing);
    compiler_cmd_run_pipeline_once(config->source, &artifacts, true, &timing);
    timing_dump(&timing);
    timing_done(&timing);

    return 0;
}

//------------------------------------------------------------------------------
