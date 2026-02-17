//------------------------------------------------------------------------------
// Benchmark command
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/cmd_internal.h>

//------------------------------------------------------------------------------

int compiler_cmd_benchmark(const NerdConfig* config)
{
    compiler_cmd_print_source_overview(config->source);

    Timing benchmark_timing = {0};
    front_end_benchmark(config->source,
                        NERD_BENCHMARK_WARMUP_ITERATIONS,
                        NERD_BENCHMARK_TIMED_ITERATIONS,
                        &benchmark_timing);
    timing_dump(&benchmark_timing);
    timing_done(&benchmark_timing);

    // Run one regular build for state inspection after benchmark timings.
    compiler_cmd_run_pipeline_once(config->source, true, NULL);

    return 0;
}

//------------------------------------------------------------------------------
