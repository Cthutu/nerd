//------------------------------------------------------------------------------
// Compiler module
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------
//> use: timing

#pragma once

#include <timing/timing.h>

//------------------------------------------------------------------------------

typedef struct ProgramInfo ProgramInfo;
typedef struct ModuleInfo  ModuleInfo;

typedef struct {
    string source;
    string source_path;
} NerdSource;

typedef struct {
    bool               verbose;
    bool               release;
    bool               require_entry_point;
    bool               skip_ir_generation;
    const ProgramInfo* program;
    u32                current_module_index;
    Array(string) keywords;
} FrontEndOptions;

typedef struct {
    NerdSource source;
    string     output_path;
    bool       emit_ir;
    bool       emit_c;
    bool       release;
    bool       verbose;
    Array(string) keywords;
} NerdBuildConfig;

typedef struct {
    bool reserved;
} NerdTestConfig;

typedef struct {
    string input_path;
    string output_path;
    bool   write_stdout;
} NerdFormatConfig;

typedef struct {
    NerdSource source;
    string     output_path;
    bool       emit_ir;
    bool       emit_c;
    bool       keep_binary;
    bool       release;
    bool       verbose;
    Array(string) keywords;
} NerdRunConfig;

typedef struct {
    cstr binary_path;
    cstr ir_path;
    cstr c_path;
    bool emit_ir_file;
    bool emit_c_file;
    bool compile_binary;
    bool release;
    Array(string) keywords;
} NerdArtifactConfig;

//------------------------------------------------------------------------------

int compiler_cmd_build(const NerdBuildConfig* config);
int compiler_cmd_test(const NerdTestConfig* config);
int compiler_cmd_format(const NerdFormatConfig* config);
int compiler_cmd_run(const NerdRunConfig* config);

//------------------------------------------------------------------------------
