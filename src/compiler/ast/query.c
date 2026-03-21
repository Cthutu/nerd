//------------------------------------------------------------------------------
// AST Querying
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/ast/ast.h>
#include <compiler/error/error.h>

//------------------------------------------------------------------------------

u64 ast_get_integer(const Lexer* lexer, const AstNode* node)
{
    if (node->kind != AK_IntegerLiteral) {
        error_ice("Node is not an integer literal");
    }
    usize integer_index = node->a;
    if (integer_index >= array_count(lexer->integers)) {
        error_ice("Integer index out of bounds");
    }
    if (lexer->tokens[node->token_index].kind != TK_Integer) {
        error_ice("Token is not an integer");
    }
    return lexer->integers[integer_index];
}
