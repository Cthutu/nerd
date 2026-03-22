//------------------------------------------------------------------------------
// Parsing implementation
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/ast/parse_internal.h>

Ast ast_parse(Lexer* lexer)
{
    AstParseState state = {
         .lexer         = lexer,
         .token_index   = 0,
         .integer_index = 0,
         .token =
            (AstToken){
                 .kind   = TK_EOF,
                 .source = lexer->source,
                 .offset = 0,
            },
         .nodes                 = 0,
         .expr_start_node_index = 0,
         .expr_start_token_index = 0,
    };

    if (array_count(lexer->tokens) == 0) {
        return (Ast){0};
    }

    state.expr_start_node_index  = (u32)array_count(state.nodes);
    state.expr_start_token_index = state.token_index;

    u32 expr_index;
    if (!ast_parse_expr(&state, &expr_index)) {
        ast_done(&(Ast){.nodes = state.nodes});
        return (Ast){0};
    }

    if (state.token_index < array_count(lexer->tokens)) {
        AstToken token;
        ast_peek_token(&state, &token);
        ast_done(&(Ast){.nodes = state.nodes});
        error_0204_unexpected_token(
            token.source, ast_token_span(&state, &token), token.kind);
        return (Ast){0};
    }

    return (Ast){.nodes = state.nodes};
}

//------------------------------------------------------------------------------

void ast_done(Ast* ast) { array_free(ast->nodes); }

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
