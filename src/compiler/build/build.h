//------------------------------------------------------------------------------
// Build module
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#pragma once

#include <compiler/ast/ast.h>
#include <compiler/compiler.h>
#include <compiler/hir/hir.h>
#include <compiler/lexer/lexer.h>
#include <compiler/sema/sema.h>

//------------------------------------------------------------------------------

typedef enum : u8 {
    FRONT_END_PRODUCT_Missing,
    FRONT_END_PRODUCT_Partial,
    FRONT_END_PRODUCT_Complete,
} FrontEndProductState;

typedef struct FrontEndSemanticReadiness {
    FrontEndProductState declarations;
    FrontEndProductState bindings;
    FrontEndProductState type_facts;
} FrontEndSemanticReadiness;

typedef struct FrontEndReadiness {
    FrontEndProductState      lexer;
    FrontEndProductState      ast;
    FrontEndProductState      sema;
    FrontEndSemanticReadiness semantic;
    FrontEndProductState      hir;
} FrontEndReadiness;

typedef struct FrontEndState {
    Lexer             lexer;
    Ast               ast;
    Sema              sema;
    Hir               hir;
    FrontEndReadiness readiness;
} FrontEndState;

typedef enum : u8 {
    MODULE_Loading,
    MODULE_Loaded,
    MODULE_Failed,
} ModuleState;

typedef struct ModuleInfo {
    string        qualified_name;
    cstr          resolved_path;
    ModuleState   state;
    FrontEndState front_end;
    FileMap       source_map;
    Array(u32) imported_module_indices;
    Array(u32) export_decl_indices;
} ModuleInfo;

typedef struct ProgramInfo {
    NerdSource root_source;
    Arena      arena;
    Array(ModuleInfo) modules;
    u32 root_module_index;
} ProgramInfo;
