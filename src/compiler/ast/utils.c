//------------------------------------------------------------------------------
// AST parser utilities
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/ast/parse_internal.h>
#include <limits.h>

//------------------------------------------------------------------------------

ErrorSpan ast_token_span(const AstParseState* state, const AstToken* token)
{
    usize end = token->offset + 1;

    if (token->kind == TK_EOF) {
        end = token->source.source.count;
    } else if (token->token_index < array_count(state->lexer->tokens)) {
        const Token* lex_token = &state->lexer->tokens[token->token_index];
        end                    = lex_token_end_offset(state->lexer, lex_token);
    }

    return (ErrorSpan){.start = token->offset, .end = end};
}

internal bool ast_expression_ref(AstParseState* state,
                                 u32            node_index,
                                 AstToken       token,
                                 i16*           out_ref)
{
    isize delta = (isize)state->expr_start_node_index - (isize)node_index;
    if (delta < INT16_MIN || delta > INT16_MAX) {
        return error_0200_code_too_complex(token.source,
                                           ast_token_span(state, &token));
    }

    *out_ref = (i16)delta;
    return true;
}

bool ast_emit_node(AstParseState* state,
                   AstNode        node,
                   AstToken       token,
                   u32*           out_index)
{
    u32 node_index = (u32)array_count(state->nodes);
    if (!ast_expression_ref(state, node_index, token, &node.ref)) {
        return false;
    }

    array_push(state->nodes, node);
    *out_index = node_index;
    return true;
}

bool ast_expect_token(AstParseState* state, TokenKind expected_kind)
{
    AstToken token;
    if (!ast_next_token(state, &token)) {
        AstToken eof = {
            .kind        = TK_EOF,
            .source      = state->lexer->source,
            .offset      = state->lexer->source.source.count,
            .token_index = (u32)array_count(state->lexer->tokens),
        };
        return error_0203_expected_token(
            eof.source, ast_token_span(state, &eof), expected_kind, eof.kind);
    }

    if (token.kind != expected_kind) {
        return error_0203_expected_token(token.source,
                                         ast_token_span(state, &token),
                                         expected_kind,
                                         token.kind);
    }

    return true;
}

bool ast_peek_token(AstParseState* state, AstToken* out_token)
{
    if (state->token_index >= array_count(state->lexer->tokens)) {
        *out_token = (AstToken){
            .kind        = TK_EOF,
            .source      = state->lexer->source,
            .offset      = state->lexer->source.source.count,
            .token_index = (u32)array_count(state->lexer->tokens),
        };
        return false;
    }

    Token      token  = state->lexer->tokens[state->token_index];
    NerdSource source = state->lexer->source;

    switch (token.kind) {
    case TK_Integer:
        *out_token = (AstToken){
            .kind                = TK_Integer,
            .source              = source,
            .offset              = token.offset,
            .token_index         = state->token_index,
            .value.integer_index = state->integer_index,
        };
        return true;

    default:
        *out_token = (AstToken){
            .kind        = token.kind,
            .source      = source,
            .offset      = token.offset,
            .token_index = state->token_index,
        };
        return true;
    }
}

bool ast_next_token(AstParseState* state, AstToken* out_token)
{
    if (!ast_peek_token(state, out_token)) {
        return false;
    }

    state->token_index++;
    if (out_token->kind == TK_Integer) {
        state->integer_index++;
    }

    return true;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
