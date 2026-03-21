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
    const NerdArtifactConfig* artifacts;
    BackEndState              results;
} BackEndContext;

internal NerdArtifactConfig compiler_default_artifacts(void)
{
    return (NerdArtifactConfig){
        .output_stem    = "_output",
        .emit_ir_file   = false,
        .emit_c_file    = false,
        .compile_binary = true,
    };
}

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
    if (!ctx->artifacts->emit_c_file && !ctx->artifacts->compile_binary) {
        return;
    }

    cstr output_path = path_replace_extension(
        &temp_arena, ctx->artifacts->output_stem, ".c");
    cgen_save(&ctx->results.cgen, output_path);
}

internal void phase_noop_reset(void* raw_ctx)
{
    BackEndContext* ctx = (BackEndContext*)raw_ctx;
    UNUSED(ctx);
}

internal void phase_compile_run(void* raw_ctx)
{
    BackEndContext* ctx = (BackEndContext*)raw_ctx;
    if (!ctx->artifacts->compile_binary) {
        return;
    }

    Arena arena = {0};
    arena_init(&arena);

    cstr c_path = path_replace_extension(
        &arena, ctx->artifacts->output_stem, ".c");
    cstr exe_path = ctx->artifacts->output_stem;
#if OS_POSIX
    string command = string_format(
        &arena, "clang -o \"%s\" \"%s\"", exe_path, c_path);
    int compile_result = shell((cstr)command.data);
    if (compile_result != 0) {
        arena_done(&arena);
        kill("Failed to compile generated C file (exit code %d)",
             compile_result);
    }
    if (chmod(exe_path, 0755) != 0) {
        arena_done(&arena);
        kill("Failed to make %s executable", exe_path);
    }
#elif OS_WINDOWS
    cstr output_path = path_replace_extension(&arena, exe_path, ".exe");
    string command =
        string_format(&arena, "clang -o \"%s\" \"%s\"", output_path, c_path);
    int compile_result = shell((cstr)command.data);
    if (compile_result != 0) {
        arena_done(&arena);
        kill("Failed to compile generated C file (exit code %d)",
             compile_result);
    }
#endif

    if (!ctx->artifacts->emit_c_file) {
        path_remove(c_path);
    }

    arena_done(&arena);
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

BackEndState back_end(const FrontEndState*     front_end_results,
                      const NerdArtifactConfig* artifacts,
                      Timing*                  timing)
{
    NerdArtifactConfig default_artifacts = compiler_default_artifacts();
    if (!artifacts) {
        artifacts = &default_artifacts;
    }

    if (artifacts->emit_ir_file) {
        cstr ir_path =
            path_replace_extension(&temp_arena, artifacts->output_stem, ".ir");
        ir_save(&front_end_results->ir, ir_path);
    }

    BackEndContext ctx = {.front_end_results = front_end_results,
                          .artifacts         = artifacts,
                          .results           = {0}};
    compiler_phase_run(g_back_end_phases, BACK_END_PHASE_COUNT, &ctx, timing);

    return ctx.results;
}

void back_end_benchmark(const FrontEndState*     front_end_results,
                        const NerdArtifactConfig* artifacts,
                        u32                     warmup_iterations,
                        u32                     timed_iterations,
                        Timing*                 out_timing)
{
    NerdArtifactConfig default_artifacts = compiler_default_artifacts();
    if (!artifacts) {
        artifacts = &default_artifacts;
    }

    timing_init(out_timing);
    if (timed_iterations == 0) {
        return;
    }

    for (usize i = 0; i < BACK_END_PHASE_COUNT; i++) {
        BackEndContext   ctx   = {.front_end_results = front_end_results,
                                  .artifacts         = artifacts,
                                  .results           = {0}};
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
    BackEndContext ctx = {.front_end_results = NULL,
                          .artifacts         = NULL,
                          .results           = *results};
    compiler_phase_reset_reverse(g_back_end_phases, BACK_END_PHASE_COUNT, &ctx);
    *results = (BackEndState){0};
}

//------------------------------------------------------------------------------
