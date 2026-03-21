//------------------------------------------------------------------------------
// Shared helpers for compiler commands
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/cmd_internal.h>

//------------------------------------------------------------------------------

NerdArtifactConfig compiler_cmd_default_artifacts(void)
{
    return (NerdArtifactConfig){
        .output_stem    = "_output",
        .emit_ir_file   = false,
        .emit_c_file    = false,
        .compile_binary = true,
    };
}

void compiler_cmd_print_source_overview(string source_code)
{
    if (source_code.count <= 128) {
        prn("Source code: " STRINGP, STRINGV(source_code));
    } else {
        prn("Source code size: %zu bytes", source_code.count);
    }
}

void compiler_cmd_run_pipeline_once(string                 source_code,
                                    const NerdArtifactConfig* artifacts,
                                    bool                   dump_compiler_state,
                                    Timing*                timing)
{
    NerdArtifactConfig default_artifacts = compiler_cmd_default_artifacts();
    if (!artifacts) {
        artifacts = &default_artifacts;
    }

    FrontEndState front_results = front_end(source_code, timing);
    BackEndState  back_results  = back_end(&front_results, artifacts, timing);

    if (dump_compiler_state) {
        compiler_dump(&front_results, &back_results);
    }

    back_end_results_done(&back_results);
    front_end_results_done(&front_results);
}

//------------------------------------------------------------------------------
