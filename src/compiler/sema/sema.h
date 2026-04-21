//------------------------------------------------------------------------------
// Semantic analysis module
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#pragma once

#include <compiler/ast/ast.h>

//------------------------------------------------------------------------------

typedef enum : u8 {
    SK_Constant,
    SK_Function,
} SemaDeclKind;

//------------------------------------------------------------------------------
// A top-level declaration collected from an AK_Bind node.

typedef struct {
    SemaDeclKind kind;
    u32          symbol_handle;
    u32          bind_node_index;
    u32          value_node_index;
} SemaDecl;

//------------------------------------------------------------------------------
// A dependency edge between two top-level declarations.

typedef struct {
    u32 from_decl_index;
    u32 to_decl_index;
} SemaDeclDep;

//------------------------------------------------------------------------------
// Compact semantic side tables keyed by declaration and AST node index.

typedef struct {
    Array(SemaDecl) decls;
    Array(SemaDeclDep) deps;
    Array(u32) ordered_decl_indices;
    Array(u32) node_decl_indices;
    Array(bool) node_const_known;
    Array(i64) node_const_values;
} Sema;

//------------------------------------------------------------------------------

bool sema_analyse(const Lexer* lexer, Ast* ast, Sema* out_sema);
void sema_done(Sema* sema);

//------------------------------------------------------------------------------
