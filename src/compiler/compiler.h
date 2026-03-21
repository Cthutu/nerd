//------------------------------------------------------------------------------
// Compiler module
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------
//> use: timing

#pragma once

#include <compiler/source.h>
#include <timing/timing.h>

//------------------------------------------------------------------------------

typedef struct {
    NerdSource source;
    string     output_path;
    bool       emit_ir;
    bool       emit_c;
} NerdBuildConfig;

typedef struct {
    NerdSource source;
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

int compiler_cmd_build(const NerdBuildConfig* config);
int compiler_cmd_benchmark(const NerdBenchmarkConfig* config);
int compiler_cmd_million(const NerdMillionConfig* config);
int compiler_cmd_test(const NerdTestConfig* config);

//------------------------------------------------------------------------------
