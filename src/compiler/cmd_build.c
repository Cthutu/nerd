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

    Timing timing = {0};
    timing_init(&timing);
    compiler_cmd_run_pipeline_once(config->source, true, &timing);
    timing_dump(&timing);
    timing_done(&timing);

    return 0;
}

//------------------------------------------------------------------------------
