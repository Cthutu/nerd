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
// | AK_IntegerNegate   | Ast index of rhs  | 0                               |
// | AK_IntegerPlus     | Ast index of left | Ast index of right              |
// | AK_IntegerMinus    | Ast index of left | Ast index of right              |
// | AK_IntegerMultiply | Ast index of left | Ast index of right              |
// | AK_IntegerDivide   | Ast index of left | Ast index of right              |
// | AK_IntegerModulo   | Ast index of left | Ast index of right              |
// | AK_Expression      | Ast index of root | 0                               |
// | AK_Bind            | Symbol            | Ast index of type or expression |
// | AK_FnDef           | Body index        | 0                               |
// | AK_FnStart         | AK_FnDef index    | AK_FnEnd index                  |
// | AK_FnEnd           | AK_FnDef index    | AK_FnStart index                |
//
// clang-format on

typedef enum {
    AK_IntegerLiteral,
    AK_IntegerNegate,
    AK_IntegerPlus,
    AK_IntegerMinus,
    AK_IntegerMultiply,
    AK_IntegerDivide,
    AK_IntegerModulo,
    AK_Expression,
    AK_Bind,
    AK_FnDef,
    AK_FnStart,
    AK_FnEnd,
} AstKind;

typedef struct {
    AstKind kind : 8;
    u8      _pad;
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

u64 ast_get_integer(const Lexer* lexer, const AstNode* node);

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
