//------------------------------------------------------------------------------
// Shared helpers for compiler commands
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/cmd_internal.h>

//------------------------------------------------------------------------------

void compiler_cmd_print_source_overview(string source_code)
{
    if (source_code.count <= 128) {
        prn("Source code: " STRINGP, STRINGV(source_code));
    } else {
        prn("Source code size: %zu bytes", source_code.count);
    }
}

void compiler_cmd_run_pipeline_once(string source_code,
                                    bool   dump_compiler_state,
                                    Timing* timing)
{
    FrontEndResults front_results = front_end(source_code, timing);
    BackEndResults  back_results  = back_end(&front_results, timing);

    if (dump_compiler_state) {
        compiler_dump(&front_results, &back_results);
    }

    back_end_results_done(&back_results);
    front_end_results_done(&front_results);
}

//------------------------------------------------------------------------------
