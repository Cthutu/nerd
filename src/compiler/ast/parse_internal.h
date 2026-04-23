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
#define AST_BP_POSTFIX 40

typedef struct {
    TokenKind  kind;
    NerdSource source;
    u32        offset;
    u32        token_index;

    union {
        u32 integer_index;
        u32 string_index;
        u32 symbol_handle;
    } value;
} AstToken;

typedef struct {
    Lexer* lexer;
    u32    token_index;
    u32    integer_index;
    u32    string_index;
    u32    symbol_index;
    bool   allow_statement_boundary;

    // Current token (from last peek or next)
    AstToken token;

    // Output
    Array(AstNode) nodes;
    Array(AstParam) params;
    Array(AstFnSignature) fn_signatures;
    Array(u32) call_args;
    Array(AstCallInfo) calls;

    // Current top-level expression start
    // u32 expr_start_node_index;
    // u32 expr_start_token_index;
    u32 start_node_index;
    u32 start_token_index;
} AstParseState;

//------------------------------------------------------------------------------
// Token stream access over the sequential lexer arrays.

bool ast_peek_token(AstParseState* state);
bool ast_next_token(AstParseState* state);
TokenKind ast_peek_kind_at(const AstParseState* state, u32 lookahead);

//------------------------------------------------------------------------------
// Shared parsing utilities for spans, node emission, and operator metadata.

ErrorSpan ast_token_span(const AstParseState* state, const AstToken* token);
bool      ast_emit_node(AstParseState* state, AstNode node, u32* out_index);

bool ast_token_starts_expression(TokenKind kind);
bool ast_infix_binding_power(TokenKind kind, u8* out_left_bp, u8* out_right_bp);
bool ast_expect_token(AstParseState* state, TokenKind expected_kind);
bool ast_token_starts_type_syntax(const AstParseState* state, u32 token_index);

//------------------------------------------------------------------------------
// Expression parsing entry points.
//
// Parser contract:
// - `state->token` is already loaded with the first token of the construct.
// - parse helpers consume the remainder of the construct.

bool ast_parse_expr(AstParseState* state, u32* out_expr_node);
bool ast_parse_expr_bp(AstParseState* state, u8 min_bp, u32* out_node);
bool ast_parse_type(AstParseState* state, u32* out_node);
bool ast_parse_type_signature(AstParseState* state, u32* out_signature_index);
bool ast_parse_fn_signature(AstParseState* state,
                            bool           allow_named_params,
                            bool           require_return_type,
                            u32*           out_signature_index);
bool ast_parse_variable(AstParseState* state, u32* out_node);
bool ast_parse_assignment(AstParseState* state, u32* out_node);

//------------------------------------------------------------------------------
// Parsing queries

typedef enum {
    PQ_Invalid,
    PQ_Expresssion,
    PQ_Declaration,
} ParsingQuery;

ParsingQuery ast_parsing_query_for_token(TokenKind kind);

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
