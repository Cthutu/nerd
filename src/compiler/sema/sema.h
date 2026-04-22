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
    SK_BuiltinFunction,
} SemaDeclKind;

typedef enum : u8 {
    STK_Void,
    STK_UntypedInteger,
    STK_String,
    STK_Bool,
    STK_I8,
    STK_I16,
    STK_I32,
    STK_I64,
    STK_U8,
    STK_U16,
    STK_U32,
    STK_U64,
    STK_F32,
    STK_F64,
    STK_Isize,
    STK_Usize,
    STK_Function,
} SemaTypeKind;

//------------------------------------------------------------------------------
// One semantic type row stored outside the AST.

typedef struct {
    SemaTypeKind kind;
    u8           param_count;
    u16          _pad2;
    u32          a;
    u32          b;
} SemaType;

//------------------------------------------------------------------------------
// A top-level declaration collected from an AK_Bind node.

typedef struct {
    SemaDeclKind kind;
    u32          symbol_handle;
    u32          bind_node_index;
    u32          type_node_index;
    u32          value_node_index;
    u32          type_index;
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
    Array(SemaType) types;
    Array(SemaDecl) decls;
    Array(SemaDeclDep) deps;
    Array(u32) ordered_decl_indices;
    Array(u32) node_decl_indices;
    Array(u32) node_type_indices;
    Array(bool) node_is_type_expr;
    Array(bool) node_const_known;
    Array(i64) node_const_values;
} Sema;

//------------------------------------------------------------------------------

bool sema_analyse(const Lexer* lexer, Ast* ast, Sema* out_sema);
void sema_done(Sema* sema);

u32    sema_no_decl(void);
u32    sema_no_type(void);
u32    sema_materialise_type(const Sema* sema, u32 type_index);
bool   sema_type_is_integer(const Sema* sema, u32 type_index);
bool   sema_type_is_concrete_integer(const Sema* sema, u32 type_index);
string sema_type_name(const Sema* sema, Arena* arena, u32 type_index);

//------------------------------------------------------------------------------
