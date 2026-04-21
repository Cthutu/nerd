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
// | CK_StringConcat    | Left node index       | Right node index      |
// | CK_SymbolRef       | Symbol handle         | 0                     |
// | CK_Group           | Inner node index      | 0                     |
// | CK_IntegerNegate   | Operand node index    | 0                     |
// | CK_IntegerPlus     | Left node index       | Right node index      |
// | CK_IntegerMinus    | Left node index       | Right node index      |
// | CK_IntegerMultiply | Left node index       | Right node index      |
// | CK_IntegerDivide   | Left node index       | Right node index      |
// | CK_IntegerModulo   | Left node index       | Right node index      |
// | CK_Call            | Callee node index     | Arg node index        |
// | CK_FnExpr          | Body node index       | 0                     |
// | CK_FnBlock         | First stmt index      | End-exclusive index   |
// | CK_Statement       | Expr node index       | 0                     |
// | CK_Bind            | Symbol handle         | Value node index      |

typedef enum {
    CK_IntegerLiteral,
    CK_StringLiteral,
    CK_StringConcat,
    CK_SymbolRef,
    CK_Group,
    CK_IntegerNegate,
    CK_IntegerPlus,
    CK_IntegerMinus,
    CK_IntegerMultiply,
    CK_IntegerDivide,
    CK_IntegerModulo,
    CK_Call,
    CK_FnExpr,
    CK_FnBlock,
    CK_Statement,
    CK_Bind,
} CstKind;

typedef struct {
    CstKind kind : 8;
    u8      _pad;
    u16     _pad2;
    u32     token_index;
    u32     a, b;
} CstNode;

typedef struct {
    Array(CstNode) nodes;
    Array(u64) integers;
    Array(u32) bindings;
} Cst;

bool cst_parse(const Lexer* lexer, Cst* out_cst);
void cst_done(Cst* cst);

u64 cst_get_integer(const Cst* cst, const CstNode* node);
u32 cst_get_symbol(const CstNode* node);

//------------------------------------------------------------------------------
