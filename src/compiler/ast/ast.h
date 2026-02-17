//------------------------------------------------------------------------------
// AST parsing module
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#pragma once

#include <compiler/lexer/lexer.h>

//------------------------------------------------------------------------------

typedef enum {
    AK_IntegerLiteral,
} AstKind;

typedef struct {
    AstKind kind : 8;
    u8      _pad;
    i16     ref;
    u32     token_index;
    u32     a, b;
} AstNode;

typedef struct {
    Array(AstNode) nodes;
} Ast;

Ast  ast_parse(Lexer* lexer);
void ast_done(Ast* ast);
void ast_dump(const Ast* ast, const Lexer* lexer);
