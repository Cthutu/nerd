//------------------------------------------------------------------------------
// Semantic analysis implementation
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/error/error.h>
#include <compiler/sema/sema.h>

//------------------------------------------------------------------------------
// Return the sentinel declaration index used for unresolved entries.

internal u32 sema_no_decl(void) { return U32_MAX; }

//------------------------------------------------------------------------------
// Compute the source span covered by an AST node's main token.

internal ErrorSpan sema_node_span(const Lexer* lexer, const AstNode* node)
{
    const Token* token = &lexer->tokens[node->token_index];
    return (ErrorSpan){.start = token->offset,
                       .end   = lex_token_end_offset(lexer, token)};
}

//------------------------------------------------------------------------------
// Find a top-level declaration by its bound symbol handle.

internal u32 sema_find_decl(const Sema* sema, u32 symbol_handle)
{
    for (u32 i = 0; i < array_count(sema->decls); ++i) {
        if (sema->decls[i].symbol_handle == symbol_handle) {
            return i;
        }
    }
    return sema_no_decl();
}

//------------------------------------------------------------------------------
// Collect top-level bindings into a compact declaration table.

internal bool sema_collect_decls(const Lexer* lexer, const Ast* ast, Sema* sema)
{
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind != AK_Bind) {
            continue;
        }

        u32 existing_decl_index = sema_find_decl(sema, ast_get_symbol(node));
        if (existing_decl_index != sema_no_decl()) {
            const SemaDecl* existing_decl = &sema->decls[existing_decl_index];
            return error_0301_duplicate_binding(
                lexer->source,
                sema_node_span(lexer, node),
                lex_symbol(lexer, ast_get_symbol(node)),
                sema_node_span(lexer, &ast->nodes[existing_decl->bind_node_index]));
        }

        const AstNode* value = &ast->nodes[node->b];
        SemaDeclKind kind = value->kind == AK_FnDef ? SK_Function : SK_Constant;

        array_push(sema->decls,
                   (SemaDecl){
                       .kind             = kind,
                       .symbol_handle    = ast_get_symbol(node),
                       .bind_node_index  = i,
                       .value_node_index = node->b,
                   });
    }

    return true;
}

//------------------------------------------------------------------------------
// Resolve AST symbol references to top-level declarations.

internal bool sema_resolve_symbol_refs(const Lexer* lexer,
                                       const Ast*   ast,
                                       Sema*        sema)
{
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind != AK_SymbolRef) {
            continue;
        }

        u32 decl_index = sema_find_decl(sema, node->a);
        if (decl_index == sema_no_decl()) {
            return error_0300_unknown_symbol(
                lexer->source, sema_node_span(lexer, node), lex_symbol(lexer, node->a));
        }

        sema->node_decl_indices[i] = decl_index;
    }

    return true;
}

//------------------------------------------------------------------------------
// Analyse the AST into compact declaration and resolution tables.

bool sema_analyse(const Lexer* lexer, const Ast* ast, Sema* out_sema)
{
    Sema sema = {0};

    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        array_push(sema.node_decl_indices, sema_no_decl());
    }

    if (!sema_collect_decls(lexer, ast, &sema)) {
        sema_done(&sema);
        return false;
    }
    if (!sema_resolve_symbol_refs(lexer, ast, &sema)) {
        sema_done(&sema);
        return false;
    }

    *out_sema = sema;
    return true;
}

//------------------------------------------------------------------------------
// Free the semantic analysis tables.

void sema_done(Sema* sema)
{
    array_free(sema->decls);
    array_free(sema->node_decl_indices);
    *sema = (Sema){0};
}

//------------------------------------------------------------------------------
