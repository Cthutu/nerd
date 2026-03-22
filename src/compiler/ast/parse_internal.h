//------------------------------------------------------------------------------
// Internal AST parser declarations
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#pragma once

#include <compiler/ast/ast.h>
#include <compiler/compiler.h>
#include <compiler/error/error.h>

//------------------------------------------------------------------------------

#define AST_BP_ADDITIVE 10
#define AST_BP_MULTIPLICATIVE 20
#define AST_BP_PREFIX 30

typedef struct {
    TokenKind  kind;
    NerdSource source;
    u32        offset;
    u32        token_index;

    union {
        u32 integer_index;
    } value;
} AstToken;

typedef struct {
    Lexer* lexer;
    u32    token_index;
    u32    integer_index;

    // Current token (from last peek or next)
    AstToken token;

    // Output
    Array(AstNode) nodes;

    // Current top-level expression start
    u32 expr_start_node_index;
    u32 expr_start_token_index;
} AstParseState;

// Token stream access over the sequential lexer arrays.
bool      ast_peek_token(AstParseState* state, AstToken* out_token);
bool      ast_next_token(AstParseState* state, AstToken* out_token);

// Shared parsing utilities for spans, node emission, and operator metadata.
ErrorSpan ast_token_span(const AstParseState* state, const AstToken* token);
bool      ast_emit_node(AstParseState* state,
                        AstNode        node,
                        AstToken       token,
                        u32*           out_index);
bool      ast_token_starts_expression(TokenKind kind);
bool ast_infix_binding_power(
    TokenKind kind, u8* out_left_bp, u8* out_right_bp);
bool ast_expect_token(AstParseState* state, TokenKind expected_kind);

// Expression parsing entry points.
bool ast_parse_expr(AstParseState* state, u32* out_expr_node);
bool ast_parse_expr_bp(AstParseState* state, u8 min_bp, u32* out_node);

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
