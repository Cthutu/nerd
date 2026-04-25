//------------------------------------------------------------------------------
// Build module
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#pragma once

#include <compiler/ast/ast.h>
#include <compiler/cgen/cgen.h>
#include <compiler/compiler.h>
#include <compiler/ir/ir.h>
#include <compiler/lexer/lexer.h>
#include <compiler/sema/sema.h>

//------------------------------------------------------------------------------

typedef struct {
    Lexer lexer;
    Ast   ast;
    Sema  sema;
    Ir    ir;
} FrontEndState;

typedef enum : u8 {
    MODULE_Loading,
    MODULE_Loaded,
    MODULE_Failed,
} ModuleState;

typedef struct {
    string        qualified_name;
    cstr          resolved_path;
    ModuleState   state;
    FrontEndState front_end;
    FileMap       source_map;
    Array(u32) export_decl_indices;
} ModuleInfo;

typedef struct {
    NerdSource root_source;
    Arena      arena;
    Array(ModuleInfo) modules;
    u32 root_module_index;
} ProgramInfo;

typedef struct {
    CGen cgen;
} BackEndState;
