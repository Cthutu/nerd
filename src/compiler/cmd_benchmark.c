//------------------------------------------------------------------------------
// Benchmark command
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/cmd_internal.h>

//------------------------------------------------------------------------------

int compiler_cmd_benchmark(const NerdBenchmarkConfig* config)
{
    compiler_cmd_print_source_overview(config->source);

    FrontEndState front_results = front_end(config->source, NULL);

    Timing benchmark_timing = {0};
    Timing back_end_timing  = {0};
    front_end_benchmark(config->source,
                        NERD_BENCHMARK_WARMUP_ITERATIONS,
                        NERD_BENCHMARK_TIMED_ITERATIONS,
                        &benchmark_timing);
    back_end_benchmark(&front_results,
                       NERD_BENCHMARK_WARMUP_ITERATIONS,
                       NERD_BENCHMARK_TIMED_ITERATIONS,
                       &back_end_timing);

    for (usize i = 0; i < array_count(back_end_timing.timings); i++) {
        timing_add(&benchmark_timing,
                   back_end_timing.timings[i].stage,
                   back_end_timing.timings[i].phase,
                   back_end_timing.timings[i].time);
    }

    timing_dump(&benchmark_timing);
    timing_done(&back_end_timing);
    timing_done(&benchmark_timing);
    front_end_results_done(&front_results);

    // Run one regular build for state inspection after benchmark timings.
    compiler_cmd_run_pipeline_once(config->source, true, NULL);

    return 0;
}

//------------------------------------------------------------------------------
