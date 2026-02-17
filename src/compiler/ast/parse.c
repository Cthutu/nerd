//------------------------------------------------------------------------------
// Parsing implementation
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/ast/ast.h>

//------------------------------------------------------------------------------

Ast ast_parse(Lexer* lexer)
{
    Array(AstNode) nodes = NULL;
    u32 integer_index    = 0;

    for (usize i = 0; i < array_count(lexer->tokens); i++) {
        Token token = lexer->tokens[i];
        switch (token.kind) {
        case TK_Integer:
            {
                AstNode node = {
                    .kind  = AK_IntegerLiteral,
                    .token = token,
                    .a     = integer_index++,
                };
                array_push(nodes, node);
            }
            break;
        }
    }

    return (Ast){.nodes = nodes};
}

//------------------------------------------------------------------------------

void ast_done(Ast* ast) { array_free(ast->nodes); }

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
