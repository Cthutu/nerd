//------------------------------------------------------------------------------
// High-level Intermediate Representation (HIR)
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#pragma once

#include <compiler/ast/ast.h>
#include <compiler/sema/sema.h>

//------------------------------------------------------------------------------

typedef enum : u8 {
    HIR_FUNCTION_Normal,
    HIR_FUNCTION_Ffi,
    HIR_FUNCTION_GenericInstantiation,
} HirFunctionKind;

typedef struct {
    HirFunctionKind kind;
    u32             symbol_handle;
    u32             decl_index;
    u32             fn_node_index;
    u32             root_scope_index;
    u32             type_index;
    u32             first_param;
    u32             param_count;
} HirFunction;

typedef struct {
    u32 symbol_handle;
    u32 local_index;
    u32 type_index;
} HirParam;

typedef struct {
    Array(HirFunction) functions;
    Array(HirParam) params;
    Arena arena;
} Hir;

//------------------------------------------------------------------------------

Hir  hir_generate(const Lexer* lexer, const Ast* ast, const Sema* sema);
void hir_done(Hir* hir);
string
hir_render(const Hir* hir, const Lexer* lexer, const Sema* sema, Arena* arena);
bool hir_save(const Hir* hir, const Lexer* lexer, const Sema* sema, cstr path);
void hir_dump(const Hir* hir, const Lexer* lexer, const Sema* sema);

//------------------------------------------------------------------------------
