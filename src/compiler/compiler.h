//------------------------------------------------------------------------------
// Compiler module
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------
//> use: timing

#pragma once

#include <compiler/cgen/cgen.h>
#include <timing/timing.h>

//------------------------------------------------------------------------------

typedef struct {
    string source;
} NerdBuildConfig;

typedef struct {
    string source;
} NerdBenchmarkConfig;

typedef struct {
    bool reserved;
} NerdMillionConfig;

typedef struct {
    bool reserved;
} NerdTestConfig;

//------------------------------------------------------------------------------

typedef struct {
    Lexer lexer;
    Ast   ast;
    Ir    ir;
} FrontEndState;

typedef struct {
    CGen cgen;
} BackEndState;

FrontEndState front_end(string source_code, Timing* timing);
void          front_end_benchmark(string  source_code,
                                  u32     warmup_iterations,
                                  u32     timed_iterations,
                                  Timing* out_timing);
void          front_end_results_done(FrontEndState* results);

BackEndState back_end(const FrontEndState* front_end_results, Timing* timing);
void         back_end_results_done(BackEndState* results);
void         back_end_benchmark(const FrontEndState* front_end_results,
                                u32                  warmup_iterations,
                                u32                  timed_iterations,
                                Timing*              out_timing);

void compiler_dump(const FrontEndState* front_end_results,
                   const BackEndState*  back_end_results);

int compiler_cmd_build(const NerdBuildConfig* config);
int compiler_cmd_benchmark(const NerdBenchmarkConfig* config);
int compiler_cmd_million(const NerdMillionConfig* config);
int compiler_cmd_test(const NerdTestConfig* config);

//------------------------------------------------------------------------------
