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
// Look up the source span for a collected declaration binding.

internal ErrorSpan sema_decl_span(const Lexer*    lexer,
                                  const Ast*      ast,
                                  const SemaDecl* decl)
{
    return sema_node_span(lexer, &ast->nodes[decl->bind_node_index]);
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
// Record a dependency edge if it is not already present.

internal void sema_add_dep(Sema* sema, u32 from_decl_index, u32 to_decl_index)
{
    for (u32 i = 0; i < array_count(sema->deps); ++i) {
        const SemaDeclDep* dep = &sema->deps[i];
        if (dep->from_decl_index == from_decl_index &&
            dep->to_decl_index == to_decl_index) {
            return;
        }
    }

    array_push(sema->deps,
               (SemaDeclDep){
                   .from_decl_index = from_decl_index,
                   .to_decl_index   = to_decl_index,
               });
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
                sema_node_span(lexer,
                               &ast->nodes[existing_decl->bind_node_index]));
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
// Walk an expression subtree and record declaration dependencies.

internal void sema_collect_node_deps(const Ast*  ast,
                                     const Sema* sema,
                                     u32         owner_decl_index,
                                     u32         node_index,
                                     Sema*       out_sema)
{
    const AstNode* node = &ast->nodes[node_index];

    switch (node->kind) {
    case AK_IntegerLiteral:
        return;
    case AK_SymbolRef:
        {
            u32 decl_index = sema->node_decl_indices[node_index];
            ASSERT(decl_index != sema_no_decl(),
                   "Expected resolved symbol reference");
            sema_add_dep(out_sema, owner_decl_index, decl_index);
            return;
        }
    case AK_IntegerNegate:
    case AK_Expression:
        sema_collect_node_deps(ast, sema, owner_decl_index, node->a, out_sema);
        return;
    case AK_IntegerPlus:
    case AK_IntegerMinus:
    case AK_IntegerMultiply:
    case AK_IntegerDivide:
    case AK_IntegerModulo:
        sema_collect_node_deps(ast, sema, owner_decl_index, node->a, out_sema);
        sema_collect_node_deps(ast, sema, owner_decl_index, node->b, out_sema);
        return;
    case AK_FnDef:
        {
            const AstNode* fn_start = &ast->nodes[node->a];
            ASSERT(fn_start->kind == AK_FnStart,
                   "Expected function start node");
            ASSERT(fn_start->b > node->a, "Expected function body expression");
            sema_collect_node_deps(
                ast, sema, owner_decl_index, fn_start->b - 1, out_sema);
            return;
        }
    default:
        return;
    }
}

//------------------------------------------------------------------------------
// Collect dependency edges for all top-level declarations.

internal void
sema_collect_deps(const Ast* ast, const Sema* sema, Sema* out_sema)
{
    for (u32 i = 0; i < array_count(sema->decls); ++i) {
        sema_collect_node_deps(
            ast, sema, i, sema->decls[i].value_node_index, out_sema);
    }
}

//------------------------------------------------------------------------------
// Depth-first visit states used while ordering declarations.

typedef enum : u8 {
    SEMA_ORDER_UNSEEN,
    SEMA_ORDER_VISITING,
    SEMA_ORDER_DONE,
} SemaOrderState;

//------------------------------------------------------------------------------
// Visit one declaration and append it after its dependencies.

internal bool sema_order_decl(const Lexer* lexer,
                              const Ast*   ast,
                              const Sema*  sema,
                              u32          decl_index,
                              Array(u8) visit_states,
                              Array(u32) * out_order)
{
    switch ((SemaOrderState)visit_states[decl_index]) {
    case SEMA_ORDER_DONE:
        return true;
    case SEMA_ORDER_VISITING:
        return true;
    case SEMA_ORDER_UNSEEN:
        break;
    }

    visit_states[decl_index] = SEMA_ORDER_VISITING;

    for (u32 i = 0; i < array_count(sema->deps); ++i) {
        const SemaDeclDep* dep = &sema->deps[i];
        if (dep->from_decl_index != decl_index) {
            continue;
        }

        if (visit_states[dep->to_decl_index] == SEMA_ORDER_VISITING) {
            const SemaDecl* decl       = &sema->decls[decl_index];
            const SemaDecl* dependency = &sema->decls[dep->to_decl_index];
            return error_0302_dependency_cycle(
                lexer->source,
                sema_decl_span(lexer, ast, decl),
                lex_symbol(lexer, decl->symbol_handle),
                sema_decl_span(lexer, ast, dependency),
                lex_symbol(lexer, dependency->symbol_handle));
        }

        if (!sema_order_decl(lexer,
                             ast,
                             sema,
                             dep->to_decl_index,
                             visit_states,
                             out_order)) {
            return false;
        }
    }

    visit_states[decl_index] = SEMA_ORDER_DONE;
    array_push(*out_order, decl_index);
    return true;
}

//------------------------------------------------------------------------------
// Build a dependency-safe declaration order.

internal bool sema_order_decls(const Lexer* lexer, const Ast* ast, Sema* sema)
{
    Array(u8) visit_states = NULL;
    for (u32 i = 0; i < array_count(sema->decls); ++i) {
        array_push(visit_states, SEMA_ORDER_UNSEEN);
    }

    bool ok = true;
    for (u32 i = 0; i < array_count(sema->decls); ++i) {
        if (!sema_order_decl(lexer,
                             ast,
                             &*sema,
                             i,
                             visit_states,
                             &sema->ordered_decl_indices)) {
            ok = false;
            break;
        }
    }

    array_free(visit_states);
    return ok;
}

//------------------------------------------------------------------------------
// Resolve AST symbol references to top-level declarations.

internal bool
sema_resolve_symbol_refs(const Lexer* lexer, const Ast* ast, Sema* sema)
{
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind != AK_SymbolRef) {
            continue;
        }

        u32 decl_index = sema_find_decl(sema, node->a);
        if (decl_index == sema_no_decl()) {
            return error_0300_unknown_symbol(lexer->source,
                                             sema_node_span(lexer, node),
                                             lex_symbol(lexer, node->a));
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
    sema_collect_deps(ast, &sema, &sema);
    if (!sema_order_decls(lexer, ast, &sema)) {
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
    array_free(sema->deps);
    array_free(sema->ordered_decl_indices);
    array_free(sema->node_decl_indices);
    *sema = (Sema){0};
}

//------------------------------------------------------------------------------
