//------------------------------------------------------------------------------
// Build command
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/cmd_internal.h>

//------------------------------------------------------------------------------

int compiler_cmd_build(void)
{
    string source = string_from_cstr("42");
    compiler_cmd_print_source_overview(source);

    Timing timing = {0};
    timing_init(&timing);
    compiler_cmd_run_pipeline_once(source, true, &timing);
    timing_dump(&timing);
    timing_done(&timing);

    return 0;
}

//------------------------------------------------------------------------------
