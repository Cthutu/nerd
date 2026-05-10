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
    NerdSource      source;
    FrontEndOptions options;
    FrontEndState   results;
} FrontEndContext;

internal bool front_end_lex(FrontEndContext* ctx)
{
    return lex(ctx->source, &ctx->results.lexer);
}

internal bool front_end_parse(FrontEndContext* ctx)
{
    ctx->results.ast = ast_parse(&ctx->results.lexer);

    if (array_count(ctx->results.lexer.tokens) > 0 &&
        array_count(ctx->results.ast.nodes) == 0) {
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------
// Run semantic analysis over the parsed AST.

internal bool front_end_sema(FrontEndContext* ctx)
{
    return sema_analyse(&ctx->results.lexer,
                        &ctx->results.ast,
                        &ctx->options,
                        &ctx->results.sema);
}

//------------------------------------------------------------------------------
// Lower the analysed front-end state to HIR.

internal bool front_end_hir_gen(FrontEndContext* ctx)
{
    ctx->results.hir = hir_generate(
        &ctx->results.lexer, &ctx->results.ast, &ctx->results.sema);
    return true;
}

//------------------------------------------------------------------------------
// Lower the analysed front-end state to IR.

internal bool front_end_ir_gen(FrontEndContext* ctx)
{
    ctx->results.ir =
        ir_generate(&ctx->results.lexer, &ctx->results.ast, &ctx->results.sema);
    return true;
}

bool front_end(NerdSource             source,
               const FrontEndOptions* options,
               Timing*                timing,
               FrontEndState*         out_results)
{
    FrontEndContext ctx = {
        .source  = source,
        .options = options ? *options : (FrontEndOptions){0},
        .results = {0},
    };
    bool result = true;

    if (timing != NULL) {
        ThreadTimePoint start = thread_time_now();
        result                = front_end_lex(&ctx);
        timing_add(timing,
                   COMPILER_STAGE_FRONT_END,
                   COMPILER_PHASE_LEX,
                   thread_time_elapsed(start, thread_time_now()));
    } else {
        result = front_end_lex(&ctx);
    }
    if (result && ctx.options.verbose) {
        lex_dump(&ctx.results.lexer);
    }

    if (result) {
        if (timing != NULL) {
            ThreadTimePoint start = thread_time_now();
            result                = front_end_parse(&ctx);
            timing_add(timing,
                       COMPILER_STAGE_FRONT_END,
                       COMPILER_PHASE_PARSE,
                       thread_time_elapsed(start, thread_time_now()));
        } else {
            result = front_end_parse(&ctx);
        }
        if (result && ctx.options.verbose) {
            ast_dump(&ctx.results.ast, &ctx.results.lexer);
        }
    }

    if (result) {
        if (timing != NULL) {
            ThreadTimePoint start = thread_time_now();
            result                = front_end_sema(&ctx);
            timing_add(timing,
                       COMPILER_STAGE_FRONT_END,
                       COMPILER_PHASE_SEMA,
                       thread_time_elapsed(start, thread_time_now()));
        } else {
            result = front_end_sema(&ctx);
        }
    }

    if (result && !ctx.options.skip_ir_generation) {
        if (timing != NULL) {
            ThreadTimePoint start = thread_time_now();
            result                = front_end_hir_gen(&ctx);
            timing_add(timing,
                       COMPILER_STAGE_FRONT_END,
                       COMPILER_PHASE_HIR_GEN,
                       thread_time_elapsed(start, thread_time_now()));
        } else {
            result = front_end_hir_gen(&ctx);
        }
        if (result && ctx.options.verbose) {
            hir_dump(&ctx.results.hir, &ctx.results.lexer, &ctx.results.sema);
        }
    }

    if (result && !ctx.options.skip_ir_generation &&
        !ctx.options.skip_legacy_ir_generation) {
        if (timing != NULL) {
            ThreadTimePoint start = thread_time_now();
            result                = front_end_ir_gen(&ctx);
            timing_add(timing,
                       COMPILER_STAGE_FRONT_END,
                       COMPILER_PHASE_IR_GEN,
                       thread_time_elapsed(start, thread_time_now()));
        } else {
            result = front_end_ir_gen(&ctx);
        }
        if (result && ctx.options.verbose) {
            ir_dump(&ctx.results.ir, &ctx.results.lexer);
        }
    }

    if (out_results != NULL) {
        *out_results = ctx.results;
    } else {
        front_end_results_done(&ctx.results);
    }
    return result;
}

void front_end_results_done(FrontEndState* results)
{
    ir_done(&results->ir);
    results->ir = (Ir){0};

    hir_done(&results->hir);
    results->hir = (Hir){0};

    sema_done(&results->sema);
    results->sema = (Sema){0};

    ast_done(&results->ast);
    results->ast = (Ast){0};

    lex_done(&results->lexer);
    results->lexer = (Lexer){0};

    *results       = (FrontEndState){0};
}

//------------------------------------------------------------------------------
