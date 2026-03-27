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
        .symbol_index  = 0,
        .token =
            (AstToken){
                .kind   = TK_EOF,
                .source = lexer->source,
                .offset = 0,
            },
        .nodes             = 0,
        .start_node_index  = 0,
        .start_token_index = 0,
    };

    while (state.token_index < array_count(lexer->tokens)) {
        state.start_token_index = state.token_index;
        state.start_node_index  = array_count(state.nodes);
        Token token             = lexer->tokens[state.token_index++];

        // <program> := <binding>*

        switch (token.kind) {
        case TK_Symbol:
            // Parse binding
            if (!ast_parse_bind(&state)) {
                ast_done(&(Ast){.nodes = state.nodes});
                return (Ast){0};
            }
            break;

        default:
            error_0204_unexpected_token(
                lexer->source,
                ast_token_span(&state,
                               &(AstToken){
                                   .kind        = token.kind,
                                   .source      = lexer->source,
                                   .offset      = token.offset,
                                   .token_index = state.token_index - 1,
                               }),
                token.kind,
                "Expected a symbol to start a binding");
            ast_done(&(Ast){.nodes = state.nodes});
            return (Ast){0};
        }
    }

    return (Ast){.nodes = state.nodes};
}

//------------------------------------------------------------------------------

void ast_done(Ast* ast) { array_free(ast->nodes); }

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
