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

typedef enum : u8 {
    HIR_STMT_Expr,
    HIR_STMT_Return,
    HIR_STMT_Let,
} HirStmtKind;

typedef enum : u8 {
    HIR_EXPR_Unsupported,
    HIR_EXPR_IntegerLiteral,
    HIR_EXPR_FloatLiteral,
    HIR_EXPR_StringLiteral,
    HIR_EXPR_BoolLiteral,
    HIR_EXPR_NilLiteral,
    HIR_EXPR_LocalRef,
    HIR_EXPR_Unary,
    HIR_EXPR_Binary,
    HIR_EXPR_Call,
    HIR_EXPR_Cast,
} HirExprKind;

typedef enum : u8 {
    HIR_UNARY_LogicalNot,
    HIR_UNARY_Negate,
    HIR_UNARY_AddressOf,
    HIR_UNARY_Deref,
} HirUnaryOp;

typedef enum : u8 {
    HIR_BINARY_Add,
    HIR_BINARY_Subtract,
    HIR_BINARY_Multiply,
    HIR_BINARY_Divide,
    HIR_BINARY_Modulo,
    HIR_BINARY_BitwiseAnd,
    HIR_BINARY_BitwiseXor,
    HIR_BINARY_BitwiseOr,
    HIR_BINARY_ShiftLeft,
    HIR_BINARY_ShiftRight,
    HIR_BINARY_Equal,
    HIR_BINARY_NotEqual,
    HIR_BINARY_Less,
    HIR_BINARY_LessEqual,
    HIR_BINARY_Greater,
    HIR_BINARY_GreaterEqual,
    HIR_BINARY_LogicalAnd,
    HIR_BINARY_LogicalOr,
} HirBinaryOp;

typedef struct {
    HirFunctionKind kind;
    u32             symbol_handle;
    u32             decl_index;
    u32             fn_node_index;
    u32             root_scope_index;
    u32             type_index;
    u32             first_param;
    u32             param_count;
    u32             body_block_index;
} HirFunction;

typedef struct {
    u32 symbol_handle;
    u32 local_index;
    u32 type_index;
} HirParam;

typedef struct {
    u32 first_stmt;
    u32 stmt_count;
} HirBlock;

typedef struct {
    HirStmtKind kind;
    u32         expr_index;
    u32         symbol_handle;
    u32         local_index;
    u32         type_index;
} HirStmt;

typedef struct {
    HirExprKind kind;
    u32         type_index;
    u32         symbol_handle;
    u32         local_index;
    i64         integer;
    f64         floating;
    u32         string_index;
    bool        boolean;
    bool        string_is_cstring;
    u32         operand_expr_index;
    u32         extra_expr_index;
    u32         lhs_expr_index;
    u32         rhs_expr_index;
    u32         callee_expr_index;
    u32         first_arg;
    u32         arg_count;
    HirUnaryOp  unary_op;
    HirBinaryOp binary_op;
} HirExpr;

typedef struct {
    u32 expr_index;
} HirCallArg;

typedef struct {
    Array(HirFunction) functions;
    Array(HirParam) params;
    Array(HirBlock) blocks;
    Array(HirStmt) stmts;
    Array(HirExpr) exprs;
    Array(HirCallArg) call_args;
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
