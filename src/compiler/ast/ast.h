//------------------------------------------------------------------------------
// AST parsing module
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------
//> use: compiler/error

#pragma once

#include <compiler/lexer/lexer.h>

// clang-format off
//------------------------------------------------------------------------------
// Table of AST Nodes:
//
// | Name               | a                 | b                               |
// |--------------------|-------------------|---------------------------------|
// | AK_IntegerLiteral  | Integer index     | 0                               |
// | AK_StringLiteral   | String index      | 0                               |
// | AK_StringConcat    | Ast index of lhs  | Ast index of rhs                |
// | AK_SymbolRef       | Symbol handle     | 0                               |
// | AK_IntegerNegate   | Ast index of rhs  | 0                               |
// | AK_IntegerPlus     | Ast index of left | Ast index of right              |
// | AK_IntegerMinus    | Ast index of left | Ast index of right              |
// | AK_IntegerMultiply | Ast index of left | Ast index of right              |
// | AK_IntegerDivide   | Ast index of left | Ast index of right              |
// | AK_IntegerModulo   | Ast index of left | Ast index of right              |
// | AK_Call            | Ast index callee  | Ast index of arg                |
// | AK_Expression      | Ast index of root | 0                               |
// | AK_Statement       | Ast index of expr | 0                               |
// | AK_Bind            | Symbol            | Ast index of type or expression |
// | AK_FnDef           | Body start index  | Fn syntax kind                  |
// | AK_FnStart         | AK_FnDef index    | AK_FnEnd index                  |
// | AK_FnEnd           | AK_FnDef index    | AK_FnStart index                |
//
// clang-format on

typedef enum {
    AK_IntegerLiteral,
    AK_StringLiteral,
    AK_StringConcat,
    AK_SymbolRef,
    AK_IntegerNegate,
    AK_IntegerPlus,
    AK_IntegerMinus,
    AK_IntegerMultiply,
    AK_IntegerDivide,
    AK_IntegerModulo,
    AK_Call,
    AK_Expression,
    AK_Statement,
    AK_Bind,
    AK_FnDef,
    AK_FnStart,
    AK_FnEnd,
} AstKind;

typedef enum : u32 {
    AFK_Expr,
    AFK_Block,
} AstFnKind;

typedef enum : u8 {
    ANF_None       = 0,
    ANF_ConstKnown = 1 << 0,
    ANF_ConstBusy  = 1 << 1,
} AstNodeFlag;

typedef struct {
    AstKind kind : 8;
    u8      flags;
    u16     _pad2;
    u32     token_index;
    u32     a, b; // Meaning depends on the AstKind
} AstNode;

typedef struct {
    Array(AstNode) nodes;
} Ast;

Ast  ast_parse(Lexer* lexer);
void ast_done(Ast* ast);
void ast_dump(const Ast* ast, const Lexer* lexer);

//------------------------------------------------------------------------------
// Extraction API

u64    ast_get_integer(const Lexer* lexer, const AstNode* node);
string ast_get_string(const Lexer* lexer, const AstNode* node);
u32    ast_get_symbol(const AstNode* node);

//------------------------------------------------------------------------------
// AST node flag helpers

static inline bool ast_has_flag(const AstNode* node, AstNodeFlag flag)
{
    return (node->flags & flag) != 0;
}

static inline void ast_set_flag(AstNode* node, AstNodeFlag flag)
{
    node->flags |= flag;
}

static inline void ast_clear_flag(AstNode* node, AstNodeFlag flag)
{
    node->flags &= (u8)~flag;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
