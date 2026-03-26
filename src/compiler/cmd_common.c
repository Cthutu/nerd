//------------------------------------------------------------------------------
// Shared helpers for compiler commands
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/build/back/back.h>
#include <compiler/build/front/front.h>
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

bool compiler_cmd_run_pipeline_once(NerdSource                source,
                                    const NerdArtifactConfig* artifacts,
                                    bool    dump_compiler_state,
                                    Timing* timing)
{
    NerdArtifactConfig default_artifacts = compiler_cmd_default_artifacts();
    if (!artifacts) {
        artifacts = &default_artifacts;
    }

    FrontEndState front_results = {0};
    if (!front_end(source, dump_compiler_state, timing, &front_results)) {
        front_end_results_done(&front_results);
        return false;
    }

    BackEndState back_results = {0};
    if (!back_end(&front_results,
                  artifacts,
                  dump_compiler_state,
                  timing,
                  &back_results)) {
        back_end_results_done(&back_results);
        front_end_results_done(&front_results);
        return false;
    }

    back_end_results_done(&back_results);
    front_end_results_done(&front_results);
    return true;
}

//------------------------------------------------------------------------------
