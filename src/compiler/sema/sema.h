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
// Compact semantic side tables keyed by declaration and AST node index.

typedef struct {
    Array(SemaDecl) decls;
    Array(u32) node_decl_indices;
} Sema;

//------------------------------------------------------------------------------

bool sema_analyse(const Lexer* lexer, const Ast* ast, Sema* out_sema);
void sema_done(Sema* sema);

//------------------------------------------------------------------------------
