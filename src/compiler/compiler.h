//------------------------------------------------------------------------------
// Compiler module
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------
//> use: lexer timing

#pragma once

#include <compiler/ast/ast.h>
#include <timing/timing.h>

//------------------------------------------------------------------------------

typedef enum {
    NERD_COMMAND_BUILD,
    NERD_COMMAND_BENCHMARK,
    NERD_COMMAND_MILLION,
} NerdCommand;

typedef struct {
    NerdCommand command;
    string      source;
} NerdConfig;

//------------------------------------------------------------------------------

typedef struct {
    Lexer lexer;
    Ast   ast;
} FrontEndResults;

typedef struct {
} BackEndResults;

FrontEndResults front_end(string source_code, Timing* timing);
void            front_end_benchmark(string  source_code,
                                    u32     warmup_iterations,
                                    u32     timed_iterations,
                                    Timing* out_timing);
void            front_end_results_done(FrontEndResults* results);

BackEndResults back_end(const FrontEndResults* front_end_results,
                        Timing*                timing);
void           back_end_results_done(BackEndResults* results);

void compiler_dump(const FrontEndResults* front_end_results,
                   const BackEndResults*  back_end_results);

int compiler_cmd_build(const NerdConfig* config);
int compiler_cmd_benchmark(const NerdConfig* config);
int compiler_cmd_million(const NerdConfig* config);

//------------------------------------------------------------------------------
