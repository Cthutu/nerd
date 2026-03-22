//------------------------------------------------------------------------------
// Front-end orchestration
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/build/front/front.h>
#include <compiler/error/error.h>
#include <compiler/internal.h>

//------------------------------------------------------------------------------

typedef struct {
    NerdSource    source;
    FrontEndState results;
} FrontEndContext;

internal bool phase_lex_run(void* raw_ctx)
{
    FrontEndContext* ctx = (FrontEndContext*)raw_ctx;
    return lex(ctx->source, &ctx->results.lexer);
}

internal bool phase_lex_reset(void* raw_ctx)
{
    FrontEndContext* ctx = (FrontEndContext*)raw_ctx;
    lex_done(&ctx->results.lexer);
    ctx->results.lexer = (Lexer){0};
    return true;
}

internal bool phase_parse_run(void* raw_ctx)
{
    FrontEndContext* ctx = (FrontEndContext*)raw_ctx;
    ctx->results.ast     = ast_parse(&ctx->results.lexer);

    if (array_count(ctx->results.lexer.tokens) > 0 &&
        array_count(ctx->results.ast.nodes) == 0) {
        return false;
    }
    return true;
}

internal bool phase_parse_reset(void* raw_ctx)
{
    FrontEndContext* ctx = (FrontEndContext*)raw_ctx;
    ast_done(&ctx->results.ast);
    ctx->results.ast = (Ast){0};
    return true;
}

internal bool phase_ir_gen_run(void* raw_ctx)
{
    FrontEndContext* ctx = (FrontEndContext*)raw_ctx;
    ctx->results.ir      = ir_generate(&ctx->results.lexer, &ctx->results.ast);
    return true;
}

internal bool phase_ir_gen_reset(void* raw_ctx)
{
    FrontEndContext* ctx = (FrontEndContext*)raw_ctx;
    ir_done(&ctx->results.ir);
    ctx->results.ir = (Ir){0};
    return true;
}

internal const PhaseSpec g_front_end_phases[] = {
    {.stage = COMPILER_STAGE_FRONT_END,
     .phase = COMPILER_PHASE_LEX,
     .run   = phase_lex_run,
     .reset = phase_lex_reset},
    {.stage = COMPILER_STAGE_FRONT_END,
     .phase = COMPILER_PHASE_PARSE,
     .run   = phase_parse_run,
     .reset = phase_parse_reset},
    {.stage = COMPILER_STAGE_FRONT_END,
     .phase = COMPILER_PHASE_IR_GEN,
     .run   = phase_ir_gen_run,
     .reset = phase_ir_gen_reset},
};

#define FRONT_END_PHASE_COUNT                                                  \
    (sizeof(g_front_end_phases) / sizeof(g_front_end_phases[0]))

bool front_end(NerdSource source, Timing* timing, FrontEndState* out_results)
{
    FrontEndContext ctx    = {.source = source, .results = {0}};
    bool            result = compiler_phase_run(
        g_front_end_phases, FRONT_END_PHASE_COUNT, &ctx, timing);
    if (out_results != NULL) {
        *out_results = ctx.results;
    }
    return result;
}

void front_end_benchmark(NerdSource source,
                         u32        warmup_iterations,
                         u32        timed_iterations,
                         Timing*    out_timing)
{
    timing_init(out_timing);
    if (timed_iterations == 0) {
        return;
    }

    for (usize i = 0; i < FRONT_END_PHASE_COUNT; i++) {
        FrontEndContext  ctx   = {.source = source, .results = {0}};
        const PhaseSpec* phase = &g_front_end_phases[i];
        TimeDuration     avg =
            compiler_phase_benchmark_single(g_front_end_phases,
                                            FRONT_END_PHASE_COUNT,
                                            i,
                                            &ctx,
                                            warmup_iterations,
                                            timed_iterations);
        timing_add(out_timing, phase->stage, phase->phase, avg);
    }
}

void front_end_results_done(FrontEndState* results)
{
    NerdSource fake_source = (NerdSource){
        .source      = s(""),
        .source_path = s(""),
    };
    FrontEndContext ctx = {.source = fake_source, .results = *results};
    compiler_phase_reset_reverse(
        g_front_end_phases, FRONT_END_PHASE_COUNT, &ctx);
    *results = (FrontEndState){0};
}

//------------------------------------------------------------------------------
