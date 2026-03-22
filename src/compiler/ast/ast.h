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
// | Name               | ref                   | a                 | b                  |
// |--------------------|-----------------------|-------------------|--------------------|
// | AK_IntegerLiteral  | 0                     | Integer index     | 0                  |
// | AK_IntegerNegate   | Offset to first node  | Ast index of rhs  | 0                  |
// | AK_IntegerPlus     | Offset to first node  | Ast index of left | Ast index of right |
// | AK_IntegerMinus    | Offset to first node  | Ast index of left | Ast index of right |
// | AK_IntegerMultiply | Offset to first node  | Ast index of left | Ast index of right |
// | AK_IntegerDivide   | Offset to first node  | Ast index of left | Ast index of right |
// | AK_IntegerModulo   | Offset to first node  | Ast index of left | Ast index of right |
// | AK_Expression      | Offset to first node  | Ast index of root | 0                  |
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
} AstKind;

typedef struct {
    AstKind kind : 8;
    u8      _pad;
    i16     ref; // Usually igned offset to AstNode index for start
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
