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
        .binary_path    = "a.out",
        .ir_path        = "a.ir",
        .c_path         = "a.c",
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

bool compiler_cmd_run_pipeline_once(string                    source_code,
                                    const NerdArtifactConfig* artifacts,
                                    bool    dump_compiler_state,
                                    Timing* timing)
{
    NerdArtifactConfig default_artifacts = compiler_cmd_default_artifacts();
    if (!artifacts) {
        artifacts = &default_artifacts;
    }

    FrontEndState front_results = {0};
    if (!front_end(source_code, timing, &front_results)) {
        front_end_results_done(&front_results);
        return false;
    }

    BackEndState back_results = {0};
    if (!back_end(&front_results, artifacts, timing, &back_results)) {
        back_end_results_done(&back_results);
        front_end_results_done(&front_results);
        return false;
    }

    if (dump_compiler_state) {
        compiler_dump(&front_results, &back_results);
    }

    back_end_results_done(&back_results);
    front_end_results_done(&front_results);
    return true;
}

//------------------------------------------------------------------------------
