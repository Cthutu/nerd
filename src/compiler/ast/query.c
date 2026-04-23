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

f64 ast_get_float(const Lexer* lexer, const AstNode* node)
{
    if (node->kind != AK_FloatLiteral) {
        error_ice("Node is not a float literal");
    }
    usize float_index = node->a;
    if (float_index >= array_count(lexer->floats)) {
        error_ice("Float index out of bounds");
    }
    if (lexer->tokens[node->token_index].kind != TK_Float) {
        error_ice("Token is not a float");
    }
    return lexer->floats[float_index];
}

//------------------------------------------------------------------------------
// Extract the decoded string literal contents from an AK_StringLiteral node.

string ast_get_string(const Lexer* lexer, const AstNode* node)
{
    if (node->kind != AK_StringLiteral) {
        error_ice("Node is not a string literal");
    }
    usize string_index = node->a;
    if (string_index >= array_count(lexer->strings)) {
        error_ice("String index out of bounds");
    }
    if (lexer->tokens[node->token_index].kind != TK_String) {
        error_ice("Token is not a string");
    }
    return lexer->strings[string_index];
}

//------------------------------------------------------------------------------
// Extract the bound symbol handle from a symbol-bearing declaration node.

u32 ast_get_symbol(const AstNode* node)
{
    if (node->kind != AK_Bind && node->kind != AK_Variable) {
        error_ice("Node is not a binding");
    }
    return node->a;
}
