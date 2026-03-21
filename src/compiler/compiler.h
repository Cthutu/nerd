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
    string source_path;
    string output_path;
    bool   emit_ir;
    bool   emit_c;
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

typedef struct {
    cstr binary_path;
    cstr ir_path;
    cstr c_path;
    bool emit_ir_file;
    bool emit_c_file;
    bool compile_binary;
} NerdArtifactConfig;

//------------------------------------------------------------------------------

typedef struct {
    Lexer lexer;
    Ast   ast;
    Ir    ir;
} FrontEndState;

typedef struct {
    CGen cgen;
} BackEndState;

bool front_end(string source_code, Timing* timing, FrontEndState* out_results);
void front_end_benchmark(string  source_code,
                         u32     warmup_iterations,
                         u32     timed_iterations,
                         Timing* out_timing);
void front_end_results_done(FrontEndState* results);

bool back_end(const FrontEndState*      front_end_results,
              const NerdArtifactConfig* artifacts,
              Timing*                   timing,
              BackEndState*             out_results);
void back_end_results_done(BackEndState* results);
void back_end_benchmark(const FrontEndState*      front_end_results,
                        const NerdArtifactConfig* artifacts,
                        u32                       warmup_iterations,
                        u32                       timed_iterations,
                        Timing*                   out_timing);

void compiler_dump(const FrontEndState* front_end_results,
                   const BackEndState*  back_end_results);

int compiler_cmd_build(const NerdBuildConfig* config);
int compiler_cmd_benchmark(const NerdBenchmarkConfig* config);
int compiler_cmd_million(const NerdMillionConfig* config);
int compiler_cmd_test(const NerdTestConfig* config);

//------------------------------------------------------------------------------
