//------------------------------------------------------------------------------
// Compiler command internals
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#pragma once

#include <compiler/internal.h>

//------------------------------------------------------------------------------

enum {
    NERD_BENCHMARK_WARMUP_ITERATIONS = 1000,
    NERD_BENCHMARK_TIMED_ITERATIONS  = 10000,
    NERD_MILLION_LINES_COUNT         = 1000000,
};

void               compiler_cmd_print_source_overview(string source_code);
NerdArtifactConfig compiler_cmd_default_artifacts(void);
void               compiler_cmd_run_pipeline_once(string                    source_code,
                                                  const NerdArtifactConfig* artifacts,
                                                  bool    dump_compiler_state,
                                                  Timing* timing);

//------------------------------------------------------------------------------
