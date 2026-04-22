//------------------------------------------------------------------------------
// AST parser utilities
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/ast/parse_internal.h>
#include <limits.h>

// clang-format off
//
// Public utility function index:
//   ast_token_span     Return the source span covered by an AST token.
//   ast_emit_node      Append an AST node and compute its relative expression
//                      ref. 
//   ast_expect_token   Consume the next token and require a specific token
//                      kind.
//  ast_peek_token      Read the current token without advancing parser
//                      state.
//  ast_next_token      Read the current token and advance parser state.
//
// clang-format on

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

// Emit the AstNode `node` to the state, and emit the index
bool ast_emit_node(AstParseState* state, AstNode node, u32* out_index)
{
    if (array_count(state->nodes) >= UINT32_MAX) {
        return error_0200_code_too_complex(
            state->lexer->source, ast_token_span(state, &state->token));
    }
    u32 node_index = (u32)array_count(state->nodes);
    array_push(state->nodes, node);
    if (out_index) {
        *out_index = node_index;
    }
    return true;
}

bool ast_expect_token(AstParseState* state, TokenKind expected_kind)
{
    if (!ast_next_token(state)) {
        AstToken eof = {
            .kind        = TK_EOF,
            .source      = state->lexer->source,
            .offset      = state->lexer->source.source.count,
            .token_index = (u32)array_count(state->lexer->tokens),
        };
        return error_0203_expected_token(
            eof.source, ast_token_span(state, &eof), expected_kind, eof.kind);
    }

    if (state->token.kind != expected_kind) {
        return error_0203_expected_token(state->token.source,
                                         ast_token_span(state, &state->token),
                                         expected_kind,
                                         state->token.kind);
    }

    return true;
}

bool ast_peek_token(AstParseState* state)
{
    if (state->token_index >= array_count(state->lexer->tokens)) {
        state->token = (AstToken){
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
        state->token = (AstToken){
            .kind                = TK_Integer,
            .source              = source,
            .offset              = token.offset,
            .token_index         = state->token_index,
            .value.integer_index = state->integer_index,
        };
        return true;

    case TK_String:
    case TK_InterpolatedStringStart:
    case TK_InterpolatedStringEnd:
        state->token = (AstToken){
            .kind               = token.kind,
            .source             = source,
            .offset             = token.offset,
            .token_index        = state->token_index,
        };
        if (token.kind == TK_String) {
            state->token.value.string_index = state->string_index;
        }
        return true;

    case TK_Symbol:
        state->token = (AstToken){
            .kind        = TK_Symbol,
            .source      = source,
            .offset      = token.offset,
            .token_index = state->token_index,
            .value.symbol_handle =
                state->lexer->symbol_handles[state->symbol_index],
        };
        return true;

    default:
        state->token = (AstToken){
            .kind        = token.kind,
            .source      = source,
            .offset      = token.offset,
            .token_index = state->token_index,
        };
        return true;
    }
}

bool ast_next_token(AstParseState* state)
{
    if (!ast_peek_token(state)) {
        return false;
    }

    // Advance any indicies
    state->token_index++;
    switch (state->token.kind) {
    case TK_Integer:
        state->integer_index++;
        break;
    case TK_String:
        state->string_index++;
        break;
    case TK_Symbol:
        state->symbol_index++;
        break;
    default:
        break;
    }

    return true;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
