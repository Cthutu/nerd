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
    if (lexer->tokens[node->token_index].kind != TK_String &&
        lexer->tokens[node->token_index].kind != TK_CString) {
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

bool ast_node_is_block_statement(const AstNode* node)
{
    return node->kind == AK_Block || node->kind == AK_For ||
           node->kind == AK_Break || node->kind == AK_Continue ||
           node->kind == AK_Return || node->kind == AK_Defer ||
           node->kind == AK_Assert || node->kind == AK_Statement ||
           node->kind == AK_Bind || node->kind == AK_Variable ||
           node->kind == AK_DestructureBind ||
           node->kind == AK_DestructureVariable ||
           node->kind == AK_DestructureAssign || node->kind == AK_Use ||
           node->kind == AK_Part || node->kind == AK_FfiDef ||
           node->kind == AK_TopOn;
}

u32 ast_block_statement_end_exclusive(const Ast* ast, u32 node_index)
{
    const AstNode* node = &ast->nodes[node_index];
    if (node->kind == AK_Block) {
        return node->b;
    }
    if (node->kind == AK_For) {
        const AstForInfo* for_info = &ast->fors[node->a];
        return for_info->else_block_index == U32_MAX
                   ? ast->nodes[node->b].b
                   : ast->nodes[for_info->else_block_index].b;
    }
    if (node->kind == AK_TopOn) {
        return ast->nodes[ast->top_ons[node->a].body_node_index].b;
    }
    if (node->kind == AK_Defer) {
        u32 end = ast_block_statement_end_exclusive(ast, node->a);
        return end > node_index + 1 ? end : node_index + 1;
    }
    if (node->kind == AK_Bind || node->kind == AK_Variable ||
        node->kind == AK_DestructureBind ||
        node->kind == AK_DestructureVariable ||
        node->kind == AK_DestructureAssign || node->kind == AK_Statement ||
        node->kind == AK_Use || node->kind == AK_Part) {
        u32 child_index = node->kind == AK_Statement ? node->a : node->b;
        if (node->kind == AK_Use) {
            child_index = node->a;
        }
        if (node->kind == AK_Part) {
            return node_index + 1;
        }
        if (child_index >= array_count(ast->nodes)) {
            return node_index + 1;
        }
        const AstNode* child = &ast->nodes[child_index];
        if (child->kind == AK_For || child->kind == AK_Block) {
            u32 end = ast_block_statement_end_exclusive(ast, child_index);
            return end > node_index + 1 ? end : node_index + 1;
        }
        if (child->kind == AK_Expression) {
            const AstNode* root = &ast->nodes[child->a];
            if (root->kind == AK_For || root->kind == AK_Block) {
                u32 end = ast_block_statement_end_exclusive(ast, child->a);
                return end > node_index + 1 ? end : node_index + 1;
            }
        }
    }
    return node_index + 1;
}
