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
    usize  start;
    usize  end;
    usize  source_start;
    string source_path;
} NerdSourceFragment;

typedef struct {
    string source;
    string source_path;
    Array(NerdSourceFragment) fragments;
} NerdSource;

typedef struct {
    bool               verbose;
    bool               release;
    bool               require_entry_point;
    bool               skip_hir_generation;
    bool               keep_partial_results;
    const ProgramInfo* program;
    u32                current_module_index;
    Array(string) keywords;
} FrontEndOptions;

typedef struct {
    NerdSource source;
    bool       release;
    bool       verbose;
    bool       timing;
    Array(string) keywords;
} NerdCheckConfig;

typedef struct {
    NerdSource source;
    string     output_path;
    bool       emit_hir;
    bool       emit_llvm;
    bool       release;
    bool       verbose;
    bool       timing;
    Array(string) keywords;
} NerdBuildConfig;

typedef struct {
    string input_path;
    string filter;
    bool   list;
    bool   list_results;
    bool   verbose;
    Array(string) keywords;
} NerdTestConfig;

typedef struct {
    string input_path;
    string output_path;
    bool   write_stdout;
} NerdFormatConfig;

typedef struct {
    NerdSource source;
    string     output_path;
    bool       emit_hir;
    bool       emit_llvm;
    bool       keep_binary;
    bool       release;
    bool       verbose;
    bool       timing;
    Array(string) keywords;
} NerdRunConfig;

typedef struct {
    cstr binary_path;
    cstr hir_path;
    cstr llvm_path;
    bool emit_hir_file;
    bool emit_llvm_file;
    bool emit_executable;
    bool release;
    Array(string) keywords;
} NerdArtifactConfig;

//------------------------------------------------------------------------------

int compiler_cmd_build(const NerdBuildConfig* config);
int compiler_cmd_check(const NerdCheckConfig* config);
int compiler_cmd_test(const NerdTestConfig* config);
int compiler_cmd_format(const NerdFormatConfig* config);
int compiler_cmd_run(const NerdRunConfig* config);

//------------------------------------------------------------------------------
