//------------------------------------------------------------------------------
// Concrete syntax tree module
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#pragma once

#include <compiler/lexer/lexer.h>

//------------------------------------------------------------------------------
// Table of CST Nodes:
//
// | Name               | a                     | b                     |
// |--------------------|-----------------------|-----------------------|
// | CK_IntegerLiteral  | Integer index         | 0                     |
// | CK_StringLiteral   | Lexer string index    | 0                     |
// | CK_BoolLiteral     | 0 false, 1 true       | 0                     |
// | CK_StringConcat    | Left node index       | Right node index      |
// | CK_InterpPartExpr  | Expr node index       | 0                     |
// | CK_InterpolatedString | First part index    | End-exclusive index   |
// | CK_SymbolRef       | Symbol handle         | 0                     |
// | CK_Group           | Inner node index      | 0                     |
// | CK_IntegerNegate   | Operand node index    | 0                     |
// | CK_IntegerPlus     | Left node index       | Right node index      |
// | CK_IntegerMinus    | Left node index       | Right node index      |
// | CK_IntegerMultiply | Left node index       | Right node index      |
// | CK_IntegerDivide   | Left node index       | Right node index      |
// | CK_IntegerModulo   | Left node index       | Right node index      |
// | CK_Call            | Callee node index     | Call-info index       |
// | CK_Cast            | Value node index      | Target type node      |
// | CK_On              | Scrutinee node index  | On-info index         |
// | CK_TypeFn          | Fn-signature index    | 0                     |
// | CK_FnExpr          | Body node index       | 0                     |
// | CK_FnBlock         | First stmt index      | End-exclusive index   |
// | CK_Statement       | Expr node index       | 0                     |
// | CK_Return          | Expr node index       | 0                     |
// | CK_Block           | First stmt index      | End-exclusive index   |
// | CK_AnnotatedValue  | Type node index       | Value node index      |
// | CK_ZeroInit        | Type node index       | 0                     |
// | CK_Bind            | Symbol handle         | Value node index      |
// | CK_Variable        | Symbol handle         | Value node index      |
// | CK_Assign          | Symbol handle         | Value node index      |

typedef enum {
    CK_IntegerLiteral,
    CK_StringLiteral,
    CK_BoolLiteral,
    CK_StringConcat,
    CK_InterpPartExpr,
    CK_InterpolatedString,
    CK_SymbolRef,
    CK_Group,
    CK_IntegerNegate,
    CK_IntegerPlus,
    CK_IntegerMinus,
    CK_IntegerMultiply,
    CK_IntegerDivide,
    CK_IntegerModulo,
    CK_Call,
    CK_Cast,
    CK_On,
    CK_TypeFn,
    CK_FnExpr,
    CK_FnBlock,
    CK_Statement,
    CK_Return,
    CK_Block,
    CK_AnnotatedValue,
    CK_ZeroInit,
    CK_Bind,
    CK_Variable,
    CK_Assign,
} CstKind;

typedef struct {
    CstKind kind : 8;
    u8      _pad;
    u16     _pad2;
    u32     token_index;
    u32     a, b;
} CstNode;

typedef struct {
    u32 symbol_handle;
    u32 type_node_index;
} CstParam;

typedef struct {
    u32 first_param;
    u32 param_count;
    u32 return_type_node_index;
} CstFnSignature;

typedef struct {
    u32 first_arg;
    u32 arg_count;
} CstCallInfo;

typedef struct {
    u32 pattern_node_index;
    u32 expr_node_index;
    u32 flags;
} CstOnBranch;

typedef enum : u32 {
    COK_Bool,
    COK_Value,
} CstOnKind;

typedef enum : u32 {
    COBF_None = 0,
    COBF_Else = 1 << 0,
} CstOnBranchFlag;

typedef struct {
    CstOnKind kind;
    u32       first_branch;
    u32       branch_count;
} CstOnInfo;

typedef struct {
    Array(CstNode) nodes;
    Array(u64) integers;
    Array(u32) bindings;
    Array(CstParam) params;
    Array(CstFnSignature) fn_signatures;
    Array(u32) call_args;
    Array(CstCallInfo) calls;
    Array(CstOnBranch) on_branches;
    Array(CstOnInfo) ons;
} Cst;

bool cst_parse(const Lexer* lexer, Cst* out_cst);
void cst_done(Cst* cst);

u64 cst_get_integer(const Cst* cst, const CstNode* node);
u32 cst_get_symbol(const CstNode* node);

//------------------------------------------------------------------------------
