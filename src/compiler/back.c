//------------------------------------------------------------------------------
// Back-end orchestration
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/internal.h>
#if OS_POSIX
#    include <sys/stat.h>
#endif

//------------------------------------------------------------------------------

typedef struct {
    const FrontEndState* front_end_results;
    BackEndState         results;
    cstr                 output_path;
} BackEndContext;

internal void phase_cgen_run(void* raw_ctx)
{
    BackEndContext* ctx = (BackEndContext*)raw_ctx;
    ctx->results.cgen   = cgen_init(&ctx->front_end_results->ir);
}

internal void phase_cgen_reset(void* raw_ctx)
{
    BackEndContext* ctx = (BackEndContext*)raw_ctx;
    cgen_done(&ctx->results.cgen);
    ctx->results.cgen = (CGen){0};
}

internal void phase_save_run(void* raw_ctx)
{
    BackEndContext* ctx = (BackEndContext*)raw_ctx;
    cgen_save(&ctx->results.cgen, ctx->output_path);
}

internal void phase_noop_reset(void* raw_ctx)
{
    BackEndContext* ctx = (BackEndContext*)raw_ctx;
    UNUSED(ctx);
}

internal void phase_compile_run(void* raw_ctx)
{
    BackEndContext* ctx = (BackEndContext*)raw_ctx;
    UNUSED(ctx);
#if OS_POSIX
    int compile_result = shell("clang -o _output _output.c");
    if (compile_result != 0) {
        kill("Failed to compile generated C file (exit code %d)",
             compile_result);
    }
    if (chmod("_output", 0755) != 0) {
        kill("Failed to make _output executable");
    }
#elif OS_WINDOWS
    int compile_result = shell("clang -o _output.exe _output.c");
    if (compile_result != 0) {
        kill("Failed to compile generated C file (exit code %d)",
             compile_result);
    }
#endif
}

internal const PhaseSpec g_back_end_phases[] = {
    {.stage = COMPILER_STAGE_BACK_END,
     .phase = COMPILER_PHASE_C_GEN,
     .run   = phase_cgen_run,
     .reset = phase_cgen_reset},
    {.stage = COMPILER_STAGE_BACK_END,
     .phase = COMPILER_PHASE_C_SAVE,
     .run   = phase_save_run,
     .reset = phase_noop_reset},
    {.stage = COMPILER_STAGE_BACK_END,
     .phase = COMPILER_PHASE_C_COMPILE,
     .run   = phase_compile_run,
     .reset = phase_noop_reset},
};

#define BACK_END_PHASE_COUNT                                                   \
    (sizeof(g_back_end_phases) / sizeof(g_back_end_phases[0]))

BackEndState back_end(const FrontEndState* front_end_results, Timing* timing)
{
    BackEndContext ctx = {.front_end_results = front_end_results,
                          .results           = {0},
                          .output_path       = "_output.c"};
    compiler_phase_run(g_back_end_phases, BACK_END_PHASE_COUNT, &ctx, timing);

    return ctx.results;
}

void back_end_benchmark(const FrontEndState* front_end_results,
                        u32                  warmup_iterations,
                        u32                  timed_iterations,
                        Timing*              out_timing)
{
    timing_init(out_timing);
    if (timed_iterations == 0) {
        return;
    }

    for (usize i = 0; i < BACK_END_PHASE_COUNT; i++) {
        BackEndContext   ctx   = {.front_end_results = front_end_results,
                                  .results           = {0},
                                  .output_path       = "_output.c"};
        const PhaseSpec* phase = &g_back_end_phases[i];
        u32              phase_warmup_iterations = warmup_iterations;
        u32              phase_timed_iterations  = timed_iterations;
        if (strcmp(phase->phase, COMPILER_PHASE_C_SAVE) == 0 ||
            strcmp(phase->phase, COMPILER_PHASE_C_COMPILE) == 0) {
            // Avoid running filesystem/compiler work thousands of times in
            // benchmark mode.
            phase_warmup_iterations = 0;
            phase_timed_iterations  = 1;
        }
        TimeDuration     avg   = compiler_phase_benchmark_single(
            g_back_end_phases,
            BACK_END_PHASE_COUNT,
            i,
            &ctx,
            phase_warmup_iterations,
            phase_timed_iterations);
        timing_add(out_timing, phase->stage, phase->phase, avg);
    }
}

void back_end_results_done(BackEndState* results)
{
    BackEndContext ctx = {.front_end_results = NULL, .results = *results};
    compiler_phase_reset_reverse(g_back_end_phases, BACK_END_PHASE_COUNT, &ctx);
    *results = (BackEndState){0};
}

//------------------------------------------------------------------------------
