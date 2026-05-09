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
    HIR_TYPE_Alias,
    HIR_TYPE_GenericAlias,
} HirTypeDeclKind;

typedef enum : u8 {
    HIR_GLOBAL_Constant,
    HIR_GLOBAL_Variable,
} HirGlobalKind;

typedef enum : u8 {
    HIR_STMT_Expr,
    HIR_STMT_Return,
    HIR_STMT_Let,
    HIR_STMT_Assign,
    HIR_STMT_Assert,
    HIR_STMT_Defer,
    HIR_STMT_Break,
    HIR_STMT_Continue,
    HIR_STMT_Block,
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
    HIR_EXPR_Index,
    HIR_EXPR_Tuple,
    HIR_EXPR_TupleField,
    HIR_EXPR_Array,
    HIR_EXPR_Field,
    HIR_EXPR_Plex,
    HIR_EXPR_PlexUpdate,
    HIR_EXPR_Slice,
    HIR_EXPR_RangeExclusive,
    HIR_EXPR_RangeInclusive,
    HIR_EXPR_Block,
    HIR_EXPR_On,
    HIR_EXPR_For,
} HirExprKind;

typedef enum : u8 {
    HIR_ON_Bool,
    HIR_ON_Value,
    HIR_ON_Condition,
} HirOnKind;

typedef enum : u8 {
    HIR_FOR_Condition,
    HIR_FOR_CStyle,
    HIR_FOR_In,
} HirForKind;

typedef enum : u8 {
    HIR_PATTERN_Value,
    HIR_PATTERN_Ignore,
    HIR_PATTERN_Bind,
    HIR_PATTERN_Equal,
    HIR_PATTERN_NotEqual,
    HIR_PATTERN_Less,
    HIR_PATTERN_LessEqual,
    HIR_PATTERN_Greater,
    HIR_PATTERN_GreaterEqual,
    HIR_PATTERN_RangeExclusive,
    HIR_PATTERN_RangeInclusive,
    HIR_PATTERN_Tuple,
    HIR_PATTERN_Plex,
    HIR_PATTERN_EnumVariant,
} HirPatternKind;

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
    HirTypeDeclKind kind;
    u32             symbol_handle;
    u32             decl_index;
    u32             type_index;
} HirTypeDecl;

typedef struct {
    HirGlobalKind kind;
    u32           symbol_handle;
    u32           decl_index;
    u32           type_index;
    u32           value_expr_index;
} HirGlobal;

typedef struct {
    u32 symbol_handle;
    u32 local_index;
    u32 type_index;
} HirParam;

typedef struct {
    u32 first_stmt;
    u32 stmt_count;
    Array(u32) stmt_indices;
} HirBlock;

typedef struct {
    HirStmtKind kind;
    u32         expr_index;
    u32         target_expr_index;
    u32         symbol_handle;
    u32         local_index;
    u32         type_index;
    u32         body_block_index;
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
    u32         body_block_index;
    u32         first_branch;
    u32         branch_count;
    u32         for_index;
    HirOnKind   on_kind;
    HirUnaryOp  unary_op;
    HirBinaryOp binary_op;
    bool        zero_missing;
} HirExpr;

typedef struct {
    u32 expr_index;
    u32 symbol_handle;
} HirCallArg;

typedef struct {
    bool is_else;
    u32  first_pattern;
    u32  pattern_count;
    u32  guard_expr_index;
    u32  body_block_index;
    u32  binder_symbol_handle;
} HirOnBranch;

typedef struct {
    HirPatternKind kind;
    u32            symbol_handle;
    u32            expr_index;
    u32            extra_expr_index;
    u32            first_child;
    u32            child_count;
} HirPattern;

typedef struct {
    u32 symbol_handle;
    u32 pattern_index;
} HirPatternChild;

typedef struct {
    HirForKind kind;
    u32        label_symbol;
    u32        condition_expr_index;
    u32        iterable_expr_index;
    u32        body_block_index;
    u32        else_block_index;
    u32        first_init_stmt;
    u32        init_stmt_count;
    u32        first_update_stmt;
    u32        update_stmt_count;
    u32        index_symbol;
    u32        index_local_index;
    u32        item_symbol;
    u32        item_local_index;
} HirFor;

typedef struct {
    Array(HirTypeDecl) type_decls;
    Array(HirGlobal) globals;
    Array(HirFunction) functions;
    Array(HirParam) params;
    Array(HirBlock) blocks;
    Array(HirStmt) stmts;
    Array(HirExpr) exprs;
    Array(HirCallArg) call_args;
    Array(HirOnBranch) on_branches;
    Array(u32) on_branch_patterns;
    Array(HirPattern) patterns;
    Array(HirPatternChild) pattern_children;
    Array(HirFor) fors;
    Array(u32) for_init_stmts;
    Array(u32) for_update_stmts;
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
