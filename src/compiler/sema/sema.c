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
// Return whether one node already has a known folded constant result.

internal bool sema_is_known_constant(const Sema* sema, u32 node_index)
{
    return node_index < array_count(sema->node_const_known) &&
           sema->node_const_known[node_index];
}

//------------------------------------------------------------------------------
// Store one folded constant result for an AST node.

internal void sema_set_constant(Sema* sema, u32 node_index, i64 value)
{
    sema->node_const_known[node_index]  = true;
    sema->node_const_values[node_index] = value;
}

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
// Visit states used while folding constant expressions.

typedef enum : u8 {
    SEMA_CONST_UNSEEN,
    SEMA_CONST_VISITING,
    SEMA_CONST_DONE,
} SemaConstState;

//------------------------------------------------------------------------------
// Fold one AST node to a signed integer when possible.

internal bool sema_fold_node(const Lexer* lex,
                             const Ast*   ast,
                             const Sema*  sema,
                             u32          node_index,
                             Array(u8) visit_states,
                             Sema* out_sema,
                             i64*  out_value)
{
    if (sema_is_known_constant(out_sema, node_index)) {
        *out_value = out_sema->node_const_values[node_index];
        return true;
    }

    if (visit_states[node_index] == SEMA_CONST_VISITING) {
        return false;
    }
    if (visit_states[node_index] == SEMA_CONST_DONE) {
        return sema_is_known_constant(out_sema, node_index);
    }

    visit_states[node_index] = SEMA_CONST_VISITING;

    const AstNode* node      = &ast->nodes[node_index];
    i64            value     = 0;
    bool           ok        = false;

    switch (node->kind) {
    case AK_IntegerLiteral:
        value = (i64)ast_get_integer(lex, node);
        ok    = true;
        break;

    case AK_SymbolRef:
        {
            u32 decl_index = sema->node_decl_indices[node_index];
            ASSERT(decl_index != sema_no_decl(),
                   "Expected resolved symbol reference");
            const SemaDecl* decl = &sema->decls[decl_index];
            if (decl->kind == SK_Constant) {
                ok = sema_fold_node(lex,
                                    ast,
                                    sema,
                                    decl->value_node_index,
                                    visit_states,
                                    out_sema,
                                    &value);
            }
        }
        break;

    case AK_IntegerNegate:
    case AK_Expression:
        ok = sema_fold_node(
            lex, ast, sema, node->a, visit_states, out_sema, &value);
        if (ok && node->kind == AK_IntegerNegate) {
            value = -value;
        }
        break;

    case AK_IntegerPlus:
    case AK_IntegerMinus:
    case AK_IntegerMultiply:
    case AK_IntegerDivide:
    case AK_IntegerModulo:
        {
            i64 lhs = 0;
            i64 rhs = 0;
            ok      = sema_fold_node(
                     lex, ast, sema, node->a, visit_states, out_sema, &lhs) &&
                 sema_fold_node(
                     lex, ast, sema, node->b, visit_states, out_sema, &rhs);
            if (!ok) {
                break;
            }

            switch (node->kind) {
            case AK_IntegerPlus:
                value = lhs + rhs;
                break;
            case AK_IntegerMinus:
                value = lhs - rhs;
                break;
            case AK_IntegerMultiply:
                value = lhs * rhs;
                break;
            case AK_IntegerDivide:
                if (rhs == 0) {
                    ok = false;
                    break;
                }
                value = lhs / rhs;
                break;
            case AK_IntegerModulo:
                if (rhs == 0) {
                    ok = false;
                    break;
                }
                value = lhs % rhs;
                break;
            default:
                ok = false;
                break;
            }
        }
        break;

    default:
        ok = false;
        break;
    }

    visit_states[node_index] = SEMA_CONST_DONE;

    if (!ok) {
        return false;
    }

    sema_set_constant(out_sema, node_index, value);
    *out_value = value;
    return true;
}

//------------------------------------------------------------------------------
// Fold all constant-capable AST nodes into semantic side tables.

internal void sema_fold_constants(const Lexer* lex, const Ast* ast, Sema* sema)
{
    Array(u8) visit_states = NULL;
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        array_push(visit_states, SEMA_CONST_UNSEEN);
    }

    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        i64 ignored = 0;
        sema_fold_node(lex, ast, sema, i, visit_states, sema, &ignored);
    }

    array_free(visit_states);
}

//------------------------------------------------------------------------------
// Analyse the AST into compact declaration and resolution tables.

bool sema_analyse(const Lexer* lexer, const Ast* ast, Sema* out_sema)
{
    Sema sema = {0};

    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        array_push(sema.node_decl_indices, sema_no_decl());
        array_push(sema.node_const_known, false);
        array_push(sema.node_const_values, 0);
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

    sema_fold_constants(lexer, ast, &sema);

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
    array_free(sema->node_const_known);
    array_free(sema->node_const_values);
    *sema = (Sema){0};
}

//------------------------------------------------------------------------------
