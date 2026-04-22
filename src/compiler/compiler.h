//------------------------------------------------------------------------------
// Compiler module
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------
//> use: timing

#pragma once

#include <timing/timing.h>

//------------------------------------------------------------------------------

typedef struct {
    string source;
    string source_path;
} NerdSource;

typedef struct {
    NerdSource source;
    string     output_path;
    bool       emit_ir;
    bool       emit_c;
    bool       verbose;
} NerdBuildConfig;

typedef struct {
    bool reserved;
} NerdTestConfig;

typedef struct {
    string input_path;
} NerdFormatConfig;

typedef struct {
    NerdSource source;
    string     output_path;
    bool       emit_ir;
    bool       emit_c;
    bool       verbose;
} NerdRunConfig;

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
int compiler_cmd_test(const NerdTestConfig* config);
int compiler_cmd_format(const NerdFormatConfig* config);
int compiler_cmd_run(const NerdRunConfig* config);

//------------------------------------------------------------------------------
