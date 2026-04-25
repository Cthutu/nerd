//------------------------------------------------------------------------------
// Back-end orchestration
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/internal.h>
#if OS_POSIX
#    include <sys/stat.h>
#endif

#include <compiler/build/back/back.h>
#include <compiler/build/front/front.h>
#include <compiler/error/error.h>

//------------------------------------------------------------------------------

#define GENERATED_C_WARNINGS                                                   \
    "-Wall -Wextra -Werror -Wno-unused-function -Wno-unused-label "            \
    "-Wno-unused-variable"

typedef struct {
    const FrontEndState*      front_end_results;
    const NerdArtifactConfig* artifacts;
    bool                      verbose;
    BackEndState              results;
} BackEndContext;

internal NerdArtifactConfig compiler_default_artifacts(void)
{
    return (NerdArtifactConfig){
        .binary_path    = "a.out",
        .ir_path        = "a.ir",
        .c_path         = "a.c",
        .emit_ir_file   = false,
        .emit_c_file    = false,
        .compile_binary = true,
        .release        = false,
    };
}

internal bool back_end_timing_add(Timing* timing,
                                  cstr    phase,
                                  bool (*run)(BackEndContext*),
                                  BackEndContext* ctx)
{
    if (timing == NULL) {
        return run(ctx);
    }

    ThreadTimePoint start  = thread_time_now();
    bool            result = run(ctx);
    ThreadTimePoint end    = thread_time_now();
    timing_add(timing,
               COMPILER_STAGE_BACK_END,
               phase,
               thread_time_elapsed(start, end));
    return result;
}

internal bool back_end_cgen(BackEndContext* ctx)
{
    ctx->results.cgen = cgen_init(&ctx->front_end_results->ir,
                                  &ctx->front_end_results->lexer,
                                  &ctx->front_end_results->sema);
    return true;
}

internal bool back_end_save_c(BackEndContext* ctx)
{
    if (!ctx->artifacts->emit_c_file && !ctx->artifacts->compile_binary) {
        return true;
    }

    return cgen_save(&ctx->results.cgen, ctx->artifacts->c_path);
}

internal bool back_end_compile_c(BackEndContext* ctx)
{
    if (!ctx->artifacts->compile_binary) {
        return true;
    }

    Arena arena = {0};
    arena_init(&arena);

    cstr c_path   = ctx->artifacts->c_path;
    cstr exe_path = ctx->artifacts->binary_path;
#if OS_POSIX
    string command =
        ctx->artifacts->release
            ? string_format(&arena,
                            "clang " GENERATED_C_WARNINGS " -O2 -DNDEBUG -o "
                            "\"%s\" \"%s\"",
                            exe_path,
                            c_path)
            : string_format(&arena,
                            "clang " GENERATED_C_WARNINGS " -g -O0 -DDEBUG -o "
                            "\"%s\" \"%s\"",
                            exe_path,
                            c_path);
    int compile_result = shell((cstr)command.data);
    if (compile_result != 0) {
        arena_done(&arena);
        return error_runtime(
            "Failed to compile generated C file (exit code %d)",
            compile_result);
    }
    if (chmod(exe_path, 0755) != 0) {
        arena_done(&arena);
        return error_runtime("Failed to make %s executable", exe_path);
    }
#elif OS_WINDOWS
    string command =
        ctx->artifacts->release
            ? string_format(&arena,
                            "clang " GENERATED_C_WARNINGS " -O2 -DNDEBUG -o "
                            "\"%s\" \"%s\"",
                            exe_path,
                            c_path)
            : string_format(&arena,
                            "clang " GENERATED_C_WARNINGS " -g -O0 -DDEBUG -o "
                            "\"%s\" \"%s\"",
                            exe_path,
                            c_path);
    int compile_result = shell((cstr)command.data);
    if (compile_result != 0) {
        arena_done(&arena);
        return error_runtime(
            "Failed to compile generated C file (exit code %d)",
            compile_result);
    }
#endif

    if (!ctx->artifacts->emit_c_file) {
        path_remove(c_path);
    }

    arena_done(&arena);
    return true;
}

bool back_end(const FrontEndState*      front_end_results,
              const NerdArtifactConfig* artifacts,
              bool                      verbose,
              Timing*                   timing,
              BackEndState*             out_results)
{
    NerdArtifactConfig default_artifacts = compiler_default_artifacts();
    if (!artifacts) {
        artifacts = &default_artifacts;
    }

    if (artifacts->emit_ir_file) {
        if (!ir_save(&front_end_results->ir,
                     &front_end_results->lexer,
                     artifacts->ir_path)) {
            return false;
        }
    }

    BackEndContext ctx = {.front_end_results = front_end_results,
                          .artifacts         = artifacts,
                          .verbose           = verbose,
                          .results           = {0}};
    bool           result =
        back_end_timing_add(timing, COMPILER_PHASE_C_GEN, back_end_cgen, &ctx);

    if (result && verbose) {
        cgen_dump(&ctx.results.cgen);
    }

    if (result) {
        result = back_end_timing_add(
            timing, COMPILER_PHASE_C_SAVE, back_end_save_c, &ctx);
    }

    if (result) {
        result = back_end_timing_add(
            timing, COMPILER_PHASE_C_COMPILE, back_end_compile_c, &ctx);
    }

    if (out_results != NULL) {
        *out_results = ctx.results;
    } else {
        back_end_results_done(&ctx.results);
    }

    return result;
}

void back_end_results_done(BackEndState* results)
{
    cgen_done(&results->cgen);
    results->cgen = (CGen){0};

    *results      = (BackEndState){0};
}

//------------------------------------------------------------------------------
