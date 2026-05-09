//------------------------------------------------------------------------------
// HIR generation
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/hir/hir.h>

//------------------------------------------------------------------------------

internal u32 hir_find_function_scope(const Ast* ast, u32 fn_node_index)
{
    (void)ast;
    (void)fn_node_index;
    return U32_MAX;
}

internal void hir_add_function_params(Hir*        hir,
                                      const Sema* sema,
                                      u32         function_index,
                                      u32         root_scope_index)
{
    if (root_scope_index == U32_MAX ||
        root_scope_index >= array_count(sema->scopes)) {
        return;
    }

    HirFunction*     function = &hir->functions[function_index];
    const SemaScope* scope    = &sema->scopes[root_scope_index];
    function->first_param     = (u32)array_count(hir->params);

    for (u32 i = 0; i < scope->local_count; ++i) {
        u32 local_index = scope->first_local + i;
        if (local_index >= array_count(sema->locals)) {
            break;
        }

        const SemaLocal* local = &sema->locals[local_index];
        if (local->kind != SLK_Param) {
            continue;
        }

        array_push(hir->params,
                   (HirParam){
                       .symbol_handle = local->symbol_handle,
                       .local_index   = local_index,
                       .type_index    = local->type_index,
                   });
        function = &hir->functions[function_index];
        function->param_count++;
    }
}

internal void hir_add_function(Hir*            hir,
                               const Ast*      ast,
                               const Sema*     sema,
                               HirFunctionKind kind,
                               u32             symbol_handle,
                               u32             decl_index,
                               u32             fn_node_index,
                               u32             root_scope_index,
                               u32             type_index)
{
    if (root_scope_index == U32_MAX) {
        root_scope_index = hir_find_function_scope(ast, fn_node_index);
    }

    u32 function_index = (u32)array_count(hir->functions);
    array_push(hir->functions,
               (HirFunction){
                   .kind             = kind,
                   .symbol_handle    = symbol_handle,
                   .decl_index       = decl_index,
                   .fn_node_index    = fn_node_index,
                   .root_scope_index = root_scope_index,
                   .type_index       = type_index,
                   .first_param      = (u32)array_count(hir->params),
                   .param_count      = 0,
               });
    hir_add_function_params(hir, sema, function_index, root_scope_index);
}

internal u32 hir_decl_fn_node(const Ast* ast, const SemaDecl* decl)
{
    u32 value_node_index = decl->value_node_index;
    if (value_node_index == sema_no_decl() ||
        value_node_index >= array_count(ast->nodes)) {
        return sema_no_decl();
    }

    const AstNode* value_node = &ast->nodes[value_node_index];
    if (value_node->kind == AK_Expression &&
        value_node->a < array_count(ast->nodes) &&
        ast->nodes[value_node->a].kind == AK_FnDef) {
        return value_node->a;
    }
    return value_node->kind == AK_FnDef ? value_node_index : sema_no_decl();
}

Hir hir_generate(const Lexer* lexer, const Ast* ast, const Sema* sema)
{
    (void)lexer;

    Hir hir = {0};
    arena_init(&hir.arena);

    for (u32 i = 0; i < array_count(sema->ordered_decl_indices); ++i) {
        u32 decl_index = sema->ordered_decl_indices[i];
        if (decl_index >= array_count(sema->decls)) {
            continue;
        }

        const SemaDecl* decl = &sema->decls[decl_index];
        if (decl->kind != SK_Function && decl->kind != SK_FfiFunction) {
            continue;
        }

        u32 fn_node_index = hir_decl_fn_node(ast, decl);
        u32 root_scope_index =
            fn_node_index != sema_no_decl() &&
                    fn_node_index < array_count(sema->node_scope_indices)
                ? sema->node_scope_indices[fn_node_index]
                : U32_MAX;
        hir_add_function(&hir,
                         ast,
                         sema,
                         decl->kind == SK_FfiFunction ? HIR_FUNCTION_Ffi
                                                      : HIR_FUNCTION_Normal,
                         decl->symbol_handle,
                         decl_index,
                         fn_node_index,
                         root_scope_index,
                         decl->type_index);
    }

    for (u32 i = 0; i < array_count(sema->generic_fn_instantiations); ++i) {
        const SemaGenericFnInstantiation* inst =
            &sema->generic_fn_instantiations[i];
        Sema inst_sema               = *sema;
        inst_sema.node_scope_indices = inst->node_scope_indices;
        inst_sema.locals             = sema->locals;
        inst_sema.scopes             = sema->scopes;
        hir_add_function(&hir,
                         ast,
                         &inst_sema,
                         HIR_FUNCTION_GenericInstantiation,
                         inst->symbol_handle,
                         inst->template_decl_index,
                         inst->fn_node_index,
                         inst->root_scope_index,
                         inst->type_index);
    }

    return hir;
}

void hir_done(Hir* hir)
{
    array_free(hir->functions);
    array_free(hir->params);
    arena_done(&hir->arena);
    *hir = (Hir){0};
}

//------------------------------------------------------------------------------
