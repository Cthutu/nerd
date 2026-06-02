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

typedef bool (*NerdModuleSourceLoader)(void*   user_data,
                                       cstr    resolved_path,
                                       string* out_source);

typedef struct {
    bool                   verbose;
    bool                   release;
    bool                   require_entry_point;
    bool                   skip_hir_generation;
    bool                   keep_partial_results;
    string                 module_root_source_path;
    NerdModuleSourceLoader module_source_loader;
    void*                  module_source_loader_data;
    const ProgramInfo*     program;
    u32                    current_module_index;
    Array(string) keywords;
} FrontEndOptions;

typedef struct {
    NerdSource source;
    bool       release;
    bool       verbose;
    bool       timing;
    Array(string) keywords;
} NerdCheckConfig;

typedef enum {
    NERD_BUILD_OUTPUT_Executable,
    NERD_BUILD_OUTPUT_Object,
    NERD_BUILD_OUTPUT_StaticLibrary,
    NERD_BUILD_OUTPUT_SharedLibrary,
} NerdBuildOutputKind;

typedef struct {
    NerdSource          source;
    string              output_path;
    NerdBuildOutputKind output_kind;
    bool                emit_hir;
    bool                emit_llvm;
    bool                release;
    bool                verbose;
    bool                timing;
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
    bool   verbose;
} NerdFormatConfig;

typedef struct {
    NerdSource source;
    string     output_path;
    Array(string) program_args;
    bool emit_hir;
    bool emit_llvm;
    bool keep_binary;
    bool release;
    bool verbose;
    bool timing;
    Array(string) keywords;
} NerdRunConfig;

typedef struct {
    Arena arena;
    Array(cstr) cleanup_paths;
} NerdSideFileRegistry;

void nerd_side_file_registry_init(NerdSideFileRegistry* registry);
void nerd_side_file_registry_done(NerdSideFileRegistry* registry);
void nerd_side_file_register_cleanup(NerdSideFileRegistry* registry, cstr path);
void nerd_side_file_cleanup_registered(NerdSideFileRegistry* registry);

typedef struct {
    cstr                binary_path;
    cstr                hir_path;
    cstr                llvm_path;
    bool                emit_hir_file;
    bool                emit_llvm_file;
    NerdBuildOutputKind output_kind;
    bool                require_entry_point;
    bool                release;
    Array(string) keywords;
    NerdSideFileRegistry* side_files;
} NerdArtifactConfig;

//------------------------------------------------------------------------------

int compiler_cmd_build(const NerdBuildConfig* config);
int compiler_cmd_check(const NerdCheckConfig* config);
int compiler_cmd_test(const NerdTestConfig* config);
int compiler_cmd_format(const NerdFormatConfig* config);
int compiler_cmd_run(const NerdRunConfig* config);

//------------------------------------------------------------------------------
