//------------------------------------------------------------------------------
// AST Querying
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/ast/ast.h>

//------------------------------------------------------------------------------

u64 ast_get_integer(const Lexer* lexer, const AstNode* node)
{
    ASSERT(node->kind == AK_IntegerLiteral, "Node is not an integer literal");
    usize integer_index = node->a;
    ASSERT(integer_index < array_count(lexer->integers),
           "Integer index out of bounds");
    ASSERT(lexer->tokens[node->token_index].kind == TK_Integer,
           "Token is not an integer");
    return lexer->integers[integer_index];
}
