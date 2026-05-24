//------------------------------------------------------------------------------
// HIR generation
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/build/build.h>
#include <compiler/error/error.h>
#include <compiler/hir/hir.h>

//------------------------------------------------------------------------------

internal u32 hir_no_index(void) { return U32_MAX; }

internal u32 hir_node_type(const Sema* sema, u32 node_index)
{
    return node_index < array_count(sema->node_type_indices)
               ? sema->node_type_indices[node_index]
               : sema_no_type();
}

internal u32 hir_node_local(const Sema* sema, u32 node_index)
{
    return node_index < array_count(sema->node_local_indices)
               ? sema->node_local_indices[node_index]
               : sema_no_local();
}

internal u32 hir_node_decl(const Sema* sema, u32 node_index)
{
    return node_index < array_count(sema->node_decl_indices)
               ? sema->node_decl_indices[node_index]
               : sema_no_decl();
}

internal u32 hir_node_scope(const Sema* sema, u32 node_index)
{
    return node_index < array_count(sema->node_scope_indices)
               ? sema->node_scope_indices[node_index]
               : hir_no_index();
}

internal u32 hir_node_line(const Lexer* lexer, const Ast* ast, u32 node_index)
{
    if (lexer == NULL || ast == NULL || node_index >= array_count(ast->nodes)) {
        return 0;
    }
    u32 token_index = ast->nodes[node_index].token_index;
    if (token_index >= array_count(lexer->tokens)) {
        return 0;
    }
    u32 line = 0;
    u32 col  = 0;
    if (!lex_offset_to_line_col(
            lexer->source, lexer->tokens[token_index].offset, &line, &col)) {
        return 0;
    }
    return line + 1;
}

internal void hir_node_source_location(const Lexer* lexer,
                                       const Ast*   ast,
                                       u32          node_index,
                                       string*      out_source_path,
                                       u32*         out_line)
{
    *out_source_path = lexer != NULL ? lexer->source.source_path : (string){0};
    *out_line        = hir_node_line(lexer, ast, node_index);
    if (lexer == NULL || ast == NULL || node_index >= array_count(ast->nodes)) {
        return;
    }

    u32 token_index = ast->nodes[node_index].token_index;
    if (token_index >= array_count(lexer->tokens)) {
        return;
    }

    usize      offset        = lexer->tokens[token_index].offset;
    NerdSource mapped        = lexer->source;
    usize      mapped_offset = offset;
    for (u32 i = 0; i < array_count(lexer->source.fragments); ++i) {
        NerdSourceFragment fragment = lexer->source.fragments[i];
        if (offset < fragment.start || offset >= fragment.end) {
            continue;
        }

        usize source_prefix_start = fragment.start - fragment.source_start;
        usize source_count        = fragment.end - source_prefix_start;
        mapped                    = (NerdSource){
            .source = string_from(
                lexer->source.source.data + source_prefix_start, source_count),
            .source_path = fragment.source_path,
        };
        mapped_offset = offset - fragment.start + fragment.source_start;
        break;
    }

    u32 line = 0;
    u32 col  = 0;
    if (!lex_offset_to_line_col(mapped, mapped_offset, &line, &col)) {
        return;
    }
    *out_line        = line + 1;
    *out_source_path = mapped.source_path;
}

internal u32 hir_local_type(const Sema* sema, u32 local_index)
{
    return local_index < array_count(sema->locals)
               ? sema->locals[local_index].type_index
               : sema_no_type();
}

internal u32 hir_local_dynamic_array_min_capacity(const Ast*  ast,
                                                  const Sema* sema,
                                                  u32         local_index)
{
    if (local_index >= array_count(sema->locals)) {
        return 0;
    }
    u32 type_node_index = sema->locals[local_index].type_node_index;
    if (type_node_index >= array_count(ast->nodes)) {
        return 0;
    }
    const AstNode* type_node = &ast->nodes[type_node_index];
    while (type_node->kind == AK_Expression &&
           type_node->a < array_count(ast->nodes)) {
        type_node_index = type_node->a;
        type_node       = &ast->nodes[type_node_index];
    }
    if (type_node->kind != AK_TypeDynamicArray ||
        type_node->a >= array_count(sema->node_const_known) ||
        !sema->node_const_known[type_node->a]) {
        return 0;
    }
    i64 value = sema->node_const_values[type_node->a];
    return value > 0 ? (u32)value : 0;
}

internal u32 hir_local_dynamic_array_min_capacity_node(const Ast*  ast,
                                                       const Sema* sema,
                                                       u32         local_index)
{
    if (local_index >= array_count(sema->locals)) {
        return hir_no_index();
    }
    u32 type_node_index = sema->locals[local_index].type_node_index;
    if (type_node_index >= array_count(ast->nodes)) {
        return hir_no_index();
    }
    const AstNode* type_node = &ast->nodes[type_node_index];
    while (type_node->kind == AK_Expression &&
           type_node->a < array_count(ast->nodes)) {
        type_node_index = type_node->a;
        type_node       = &ast->nodes[type_node_index];
    }
    if (type_node->kind != AK_TypeDynamicArray ||
        type_node->a >= array_count(ast->nodes)) {
        return hir_no_index();
    }
    return type_node->a;
}

internal bool hir_node_is_const_known(const Sema* sema, u32 node_index)
{
    return node_index < array_count(sema->node_const_known) &&
           sema->node_const_known[node_index];
}

internal u32 hir_decl_type(const Sema* sema, u32 decl_index)
{
    return decl_index < array_count(sema->decls)
               ? sema->decls[decl_index].type_index
               : sema_no_type();
}

internal u32 hir_decl_binding(const Hir* hir, u32 decl_index)
{
    return decl_index < array_count(hir->decl_binding_indices)
               ? hir->decl_binding_indices[decl_index]
               : hir_no_index();
}

internal void hir_set_decl_binding(Hir* hir, u32 decl_index, u32 binding_index)
{
    if (decl_index < array_count(hir->decl_binding_indices)) {
        hir->decl_binding_indices[decl_index] = binding_index;
    }
}

internal u32 hir_find_current_module_index(const Sema* sema)
{
    if (sema == NULL || sema->program == NULL) {
        return hir_no_index();
    }

    for (u32 i = 0; i < array_count(sema->program->modules); ++i) {
        if (&sema->program->modules[i].front_end.sema == sema) {
            return i;
        }
    }
    return hir_no_index();
}

internal u32 hir_find_scope_local(const Sema* sema,
                                  u32         scope_index,
                                  u32         symbol_handle)
{
    if (scope_index == U32_MAX || scope_index >= array_count(sema->scopes) ||
        symbol_handle == U32_MAX) {
        return sema_no_local();
    }

    const SemaScope* scope = &sema->scopes[scope_index];
    for (u32 i = 0; i < scope->local_count; ++i) {
        u32 local_index = scope->first_local + i;
        if (local_index >= array_count(sema->locals)) {
            break;
        }
        if (sema->locals[local_index].symbol_handle == symbol_handle) {
            return local_index;
        }
    }
    return sema_no_local();
}

internal u32 hir_unwrap_node(const Ast* ast, u32 node_index)
{
    while (node_index < array_count(ast->nodes) &&
           (ast->nodes[node_index].kind == AK_Expression ||
            ast->nodes[node_index].kind == AK_Statement)) {
        node_index = ast->nodes[node_index].a;
    }
    return node_index;
}

internal u32 hir_find_function_scope(const Ast* ast, u32 fn_node_index)
{
    (void)ast;
    (void)fn_node_index;
    return U32_MAX;
}

internal u32 hir_lower_block_node(Hir*         hir,
                                  const Lexer* lexer,
                                  const Ast*   ast,
                                  const Sema*  sema,
                                  u32          block_node_index);

internal u32 hir_lower_single_stmt_block(Hir*         hir,
                                         const Lexer* lexer,
                                         const Ast*   ast,
                                         const Sema*  sema,
                                         u32          node_index);

internal u32 hir_lower_on_branch_block(Hir*         hir,
                                       const Lexer* lexer,
                                       const Ast*   ast,
                                       const Sema*  sema,
                                       u32          expr_node_index);

internal u32 hir_lower_pattern(Hir*         hir,
                               const Lexer* lexer,
                               const Ast*   ast,
                               const Sema*  sema,
                               u32          pattern_index);

internal u32 hir_lower_for(Hir*         hir,
                           const Lexer* lexer,
                           const Ast*   ast,
                           const Sema*  sema,
                           u32          node_index);

internal u32 hir_lower_expr(Hir*         hir,
                            const Lexer* lexer,
                            const Ast*   ast,
                            const Sema*  sema,
                            u32          node_index);

internal void hir_add_function(Hir*            hir,
                               const Lexer*    lexer,
                               const Ast*      ast,
                               const Sema*     sema,
                               HirFunctionKind kind,
                               u32             binding_symbol_handle,
                               u32             decl_index,
                               u32             fn_node_index,
                               u32             root_scope_index,
                               u32             type_index,
                               u32*            out_function_index);

internal u32 hir_add_expr(Hir* hir, HirExpr expr)
{
    array_push(hir->exprs, expr);
    return (u32)array_count(hir->exprs) - 1;
}

internal u32 hir_node_source_line(const Lexer* lexer,
                                  const Ast*   ast,
                                  u32          node_index)
{
    if (node_index >= array_count(ast->nodes)) {
        return 0;
    }
    const AstNode* node = &ast->nodes[node_index];
    if (node->token_index >= array_count(lexer->tokens)) {
        return 0;
    }
    u32 line = 0;
    u32 col  = 0;
    if (!lex_offset_to_line_col(lexer->source,
                                lexer->tokens[node->token_index].offset,
                                &line,
                                &col)) {
        return 0;
    }
    UNUSED(col);
    return line + 1;
}

internal u32 hir_token_source_line(const Lexer* lexer, u32 token_index)
{
    if (lexer == NULL || token_index >= array_count(lexer->tokens)) {
        return 0;
    }

    u32 line = 0;
    u32 col  = 0;
    if (!lex_offset_to_line_col(
            lexer->source, lexer->tokens[token_index].offset, &line, &col)) {
        return 0;
    }
    UNUSED(col);
    return line + 1;
}

internal u32 hir_add_unsupported_expr(Hir*        hir,
                                      const Sema* sema,
                                      u32         node_index)
{
    return hir_add_expr(hir,
                        (HirExpr){
                            .kind          = HIR_EXPR_Unsupported,
                            .type_index    = hir_node_type(sema, node_index),
                            .symbol_handle = U32_MAX,
                            .local_index   = sema_no_local(),
                        });
}

internal u32 hir_add_default_value_expr(Hir*        hir,
                                        const Sema* sema,
                                        u32         node_index,
                                        u32         type_index)
{
    if (type_index == sema_no_type()) {
        type_index = hir_node_type(sema, node_index);
    }
    return hir_add_expr(hir,
                        (HirExpr){
                            .kind          = HIR_EXPR_DefaultValue,
                            .type_index    = type_index,
                            .symbol_handle = U32_MAX,
                            .local_index   = sema_no_local(),
                        });
}

internal bool hir_unary_op_from_ast_kind(AstKind kind, HirUnaryOp* out)
{
    switch (kind) {
    case AK_LogicalNot:
        *out = HIR_UNARY_LogicalNot;
        return true;
    case AK_IntegerNegate:
        *out = HIR_UNARY_Negate;
        return true;
    case AK_BitwiseNot:
        *out = HIR_UNARY_BitwiseNot;
        return true;
    case AK_AddressOf:
        *out = HIR_UNARY_AddressOf;
        return true;
    case AK_Deref:
        *out = HIR_UNARY_Deref;
        return true;
    default:
        return false;
    }
}

internal bool hir_binary_op_from_ast_kind(AstKind kind, HirBinaryOp* out)
{
    switch (kind) {
    case AK_IntegerPlus:
        *out = HIR_BINARY_Add;
        return true;
    case AK_IntegerMinus:
        *out = HIR_BINARY_Subtract;
        return true;
    case AK_IntegerMultiply:
        *out = HIR_BINARY_Multiply;
        return true;
    case AK_IntegerDivide:
        *out = HIR_BINARY_Divide;
        return true;
    case AK_IntegerModulo:
        *out = HIR_BINARY_Modulo;
        return true;
    case AK_BitwiseAnd:
        *out = HIR_BINARY_BitwiseAnd;
        return true;
    case AK_BitwiseXor:
        *out = HIR_BINARY_BitwiseXor;
        return true;
    case AK_BitwiseOr:
        *out = HIR_BINARY_BitwiseOr;
        return true;
    case AK_ShiftLeft:
        *out = HIR_BINARY_ShiftLeft;
        return true;
    case AK_ShiftRight:
        *out = HIR_BINARY_ShiftRight;
        return true;
    case AK_Equal:
        *out = HIR_BINARY_Equal;
        return true;
    case AK_NotEqual:
        *out = HIR_BINARY_NotEqual;
        return true;
    case AK_Less:
        *out = HIR_BINARY_Less;
        return true;
    case AK_LessEqual:
        *out = HIR_BINARY_LessEqual;
        return true;
    case AK_Greater:
        *out = HIR_BINARY_Greater;
        return true;
    case AK_GreaterEqual:
        *out = HIR_BINARY_GreaterEqual;
        return true;
    case AK_LogicalAnd:
        *out = HIR_BINARY_LogicalAnd;
        return true;
    case AK_LogicalOr:
        *out = HIR_BINARY_LogicalOr;
        return true;
    default:
        return false;
    }
}

internal bool hir_ast_kind_is_expression_child(AstKind kind)
{
    HirUnaryOp ignored_unary;
    if (hir_unary_op_from_ast_kind(kind, &ignored_unary)) {
        return true;
    }

    HirBinaryOp ignored;
    if (hir_binary_op_from_ast_kind(kind, &ignored)) {
        return true;
    }

    switch (kind) {
    case AK_IntegerLiteral:
    case AK_FloatLiteral:
    case AK_StringLiteral:
    case AK_StringConcat:
    case AK_InterpolatedString:
    case AK_InterpPartExpr:
    case AK_BoolLiteral:
    case AK_NilLiteral:
    case AK_SymbolRef:
    case AK_EnumVariant:
    case AK_TypeFn:
    case AK_TypeApply:
    case AK_TypeTuple:
    case AK_TypeArray:
    case AK_TypeSlice:
    case AK_TypeDynamicArray:
    case AK_TypePointer:
    case AK_TypePlex:
    case AK_TypeEnum:
    case AK_AnnotatedValue:
    case AK_Assign:
    case AK_Call:
    case AK_Cast:
    case AK_Index:
    case AK_Tuple:
    case AK_TupleField:
    case AK_Array:
    case AK_Field:
    case AK_Plex:
    case AK_PlexUpdate:
    case AK_Slice:
    case AK_RangeExclusive:
    case AK_RangeInclusive:
    case AK_ExprBlock:
    case AK_On:
    case AK_FnDef:
    case AK_Expression:
        return true;
    default:
        return false;
    }
}

internal u32 hir_function_param_type(const Sema* sema,
                                     u32         function_type,
                                     u32         param_index)
{
    if (function_type == sema_no_type() ||
        function_type >= array_count(sema->types) ||
        sema->types[function_type].kind != STK_Function ||
        param_index >= sema->types[function_type].param_count) {
        return sema_no_type();
    }
    return sema->type_param_types[sema->types[function_type].first_param_type +
                                  param_index];
}

internal bool hir_imported_decl_source(const Sema*      sema,
                                       const SemaDecl*  decl,
                                       const Lexer**    out_lexer,
                                       const Ast**      out_ast,
                                       const Sema**     out_sema,
                                       const SemaDecl** out_decl)
{
    if (decl->import_module_index == sema_no_decl() ||
        decl->import_decl_index == sema_no_decl() || sema->program == NULL ||
        decl->import_module_index >= array_count(sema->program->modules)) {
        return false;
    }

    const ModuleInfo* module =
        &sema->program->modules[decl->import_module_index];
    if (decl->import_decl_index >= array_count(module->front_end.sema.decls)) {
        return false;
    }

    const SemaDecl* source_decl =
        &module->front_end.sema.decls[decl->import_decl_index];
    *out_lexer = &module->front_end.lexer;
    *out_ast   = &module->front_end.ast;
    *out_sema  = &module->front_end.sema;
    *out_decl  = source_decl;
    return true;
}

internal u32 hir_unwrap_expr_node(const Ast* ast, u32 node_index)
{
    while (node_index < array_count(ast->nodes)) {
        const AstNode* node = &ast->nodes[node_index];
        if (node->kind == AK_Expression || node->kind == AK_Statement ||
            node->kind == AK_InterpPartExpr) {
            node_index = node->a;
            continue;
        }
        if (node->kind == AK_AnnotatedValue) {
            node_index = node->b;
            continue;
        }
        break;
    }
    return node_index;
}

internal u32 hir_ffi_foreign_symbol_handle_from(const Lexer*    root_lexer,
                                                const Lexer*    lexer,
                                                const Ast*      ast,
                                                const Sema*     sema,
                                                const SemaDecl* decl,
                                                u32             depth);

internal bool hir_decl_alias_target(const Lexer*     root_lexer,
                                    const Lexer*     lexer,
                                    const Ast*       ast,
                                    const Sema*      sema,
                                    const SemaDecl*  decl,
                                    const Lexer**    out_lexer,
                                    const Ast**      out_ast,
                                    const Sema**     out_sema,
                                    const SemaDecl** out_decl,
                                    u32              depth)
{
    if (decl == NULL || decl->value_node_index == sema_no_decl()) {
        return false;
    }

    u32 value_node_index = hir_unwrap_expr_node(ast, decl->value_node_index);
    if (value_node_index >= array_count(ast->nodes) ||
        ast->nodes[value_node_index].kind != AK_SymbolRef ||
        value_node_index >= array_count(sema->node_decl_indices)) {
        return false;
    }

    u32 target_decl_index = sema->node_decl_indices[value_node_index];
    if (target_decl_index == sema_no_decl() ||
        target_decl_index >= array_count(sema->decls)) {
        return false;
    }

    const SemaDecl* target_decl = &sema->decls[target_decl_index];
    if (hir_ffi_foreign_symbol_handle_from(
            root_lexer, lexer, ast, sema, target_decl, depth + 1) == U32_MAX) {
        return false;
    }

    *out_lexer = lexer;
    *out_ast   = ast;
    *out_sema  = sema;
    *out_decl  = target_decl;
    return true;
}

internal u32 hir_ffi_foreign_symbol_handle_from(const Lexer*    root_lexer,
                                                const Lexer*    lexer,
                                                const Ast*      ast,
                                                const Sema*     sema,
                                                const SemaDecl* decl,
                                                u32             depth)
{
    if (decl == NULL || depth > 32) {
        return U32_MAX;
    }

    if (decl->import_module_index != sema_no_decl()) {
        const Lexer*    source_lexer = NULL;
        const Ast*      source_ast   = NULL;
        const Sema*     source_sema  = NULL;
        const SemaDecl* source_decl  = NULL;
        if (!hir_imported_decl_source(sema,
                                      decl,
                                      &source_lexer,
                                      &source_ast,
                                      &source_sema,
                                      &source_decl)) {
            return U32_MAX;
        }
        return hir_ffi_foreign_symbol_handle_from(root_lexer,
                                                  source_lexer,
                                                  source_ast,
                                                  source_sema,
                                                  source_decl,
                                                  depth + 1);
    }

    if (decl->kind == SK_Constant || decl->kind == SK_Variable) {
        const Lexer*    target_lexer = NULL;
        const Ast*      target_ast   = NULL;
        const Sema*     target_sema  = NULL;
        const SemaDecl* target_decl  = NULL;
        if (!hir_decl_alias_target(root_lexer,
                                   lexer,
                                   ast,
                                   sema,
                                   decl,
                                   &target_lexer,
                                   &target_ast,
                                   &target_sema,
                                   &target_decl,
                                   depth)) {
            return U32_MAX;
        }
        return hir_ffi_foreign_symbol_handle_from(root_lexer,
                                                  target_lexer,
                                                  target_ast,
                                                  target_sema,
                                                  target_decl,
                                                  depth + 1);
    }

    if (decl->kind != SK_FfiFunction ||
        decl->value_node_index == sema_no_decl() ||
        decl->value_node_index >= array_count(ast->nodes)) {
        return U32_MAX;
    }

    const AstNode* ffi_node = &ast->nodes[decl->value_node_index];
    if (ffi_node->kind != AK_FfiDef ||
        ffi_node->a >= array_count(ast->ffi_infos)) {
        return U32_MAX;
    }

    const AstFfiInfo* ffi_info = &ast->ffi_infos[ffi_node->a];
    return sema_import_symbol_handle(
        (Lexer*)root_lexer, lexer, ffi_info->foreign_symbol_handle);
}

internal u32 hir_ffi_foreign_symbol_handle(const Lexer*    lexer,
                                           const Ast*      ast,
                                           const Sema*     sema,
                                           const SemaDecl* decl)
{
    return hir_ffi_foreign_symbol_handle_from(lexer, lexer, ast, sema, decl, 0);
}

internal bool hir_decl_ffi_info(const Lexer*       lexer,
                                const Ast*         ast,
                                const Sema*        sema,
                                const SemaDecl*    decl,
                                const Lexer**      out_lexer,
                                const Ast**        out_ast,
                                const Sema**       out_sema,
                                const AstFfiInfo** out_info)
{
    const Lexer*    source_lexer = lexer;
    const Ast*      source_ast   = ast;
    const Sema*     source_sema  = sema;
    const SemaDecl* source_decl  = decl;

    for (u32 depth = 0; depth < 32; ++depth) {
        if (source_decl == NULL) {
            return false;
        }
        if (source_decl->import_module_index != sema_no_decl()) {
            if (!hir_imported_decl_source(source_sema,
                                          source_decl,
                                          &source_lexer,
                                          &source_ast,
                                          &source_sema,
                                          &source_decl)) {
                return false;
            }
            continue;
        }
        if (source_decl->kind == SK_FfiFunction) {
            break;
        }
        if (source_decl->kind != SK_Constant &&
            source_decl->kind != SK_Variable) {
            return false;
        }
        const Lexer*    target_lexer = NULL;
        const Ast*      target_ast   = NULL;
        const Sema*     target_sema  = NULL;
        const SemaDecl* target_decl  = NULL;
        if (!hir_decl_alias_target(lexer,
                                   source_lexer,
                                   source_ast,
                                   source_sema,
                                   source_decl,
                                   &target_lexer,
                                   &target_ast,
                                   &target_sema,
                                   &target_decl,
                                   depth)) {
            return false;
        }
        source_lexer = target_lexer;
        source_ast   = target_ast;
        source_sema  = target_sema;
        source_decl  = target_decl;
    }

    if (source_decl == NULL || source_decl->kind != SK_FfiFunction ||
        source_decl->value_node_index == sema_no_decl() ||
        source_decl->value_node_index >= array_count(source_ast->nodes)) {
        return false;
    }

    const AstNode* ffi_node = &source_ast->nodes[source_decl->value_node_index];
    if (ffi_node->kind != AK_FfiDef ||
        ffi_node->a >= array_count(source_ast->ffi_infos)) {
        return false;
    }

    *out_lexer = source_lexer;
    *out_ast   = source_ast;
    *out_sema  = source_sema;
    *out_info  = &source_ast->ffi_infos[ffi_node->a];
    return true;
}

internal bool hir_eval_string_constant(const Lexer* lexer,
                                       const Ast*   ast,
                                       const Sema*  sema,
                                       Hir*         hir,
                                       u32          node_index,
                                       string*      out)
{
    if (node_index >= array_count(ast->nodes)) {
        return false;
    }

    const AstNode* node = &ast->nodes[node_index];
    switch (node->kind) {
    case AK_StringLiteral:
        *out = ast_get_string(lexer, node);
        return true;
    case AK_StringConcat:
        {
            string lhs = {0};
            string rhs = {0};
            if (!hir_eval_string_constant(
                    lexer, ast, sema, hir, node->a, &lhs) ||
                !hir_eval_string_constant(
                    lexer, ast, sema, hir, node->b, &rhs)) {
                return false;
            }
            *out = string_format(
                &hir->arena, STRINGP STRINGP, STRINGV(lhs), STRINGV(rhs));
            return true;
        }
    case AK_Expression:
    case AK_Statement:
    case AK_InterpPartExpr:
        return hir_eval_string_constant(lexer, ast, sema, hir, node->a, out);
    case AK_SymbolRef:
        if (node_index < array_count(sema->node_local_indices)) {
            u32 local_index = sema->node_local_indices[node_index];
            if (local_index != sema_no_local() &&
                sema->locals[local_index].kind == SLK_Constant &&
                sema->locals[local_index].value_node_index != sema_no_decl()) {
                return hir_eval_string_constant(
                    lexer,
                    ast,
                    sema,
                    hir,
                    sema->locals[local_index].value_node_index,
                    out);
            }
        }
        if (node_index < array_count(sema->node_decl_indices)) {
            u32 decl_index = sema->node_decl_indices[node_index];
            if (decl_index != sema_no_decl() &&
                sema->decls[decl_index].kind == SK_Constant &&
                sema->decls[decl_index].value_node_index != sema_no_decl()) {
                return hir_eval_string_constant(
                    lexer,
                    ast,
                    sema,
                    hir,
                    sema->decls[decl_index].value_node_index,
                    out);
            }
        }
        return false;
    case AK_AnnotatedValue:
        return hir_eval_string_constant(lexer, ast, sema, hir, node->b, out);
    default:
        return false;
    }
}

internal void hir_add_extern(Hir*            hir,
                             const Lexer*    lexer,
                             const Ast*      ast,
                             const Sema*     sema,
                             const SemaDecl* decl)
{
    if (decl == NULL) {
        return;
    }

    const Lexer*      ffi_lexer = lexer;
    const Ast*        ffi_ast   = ast;
    const Sema*       ffi_sema  = sema;
    const AstFfiInfo* ffi_info  = NULL;
    if (!hir_decl_ffi_info(lexer,
                           ast,
                           sema,
                           decl,
                           &ffi_lexer,
                           &ffi_ast,
                           &ffi_sema,
                           &ffi_info)) {
        return;
    }

    string library = {0};
    if (ffi_info->library_node_index != U32_MAX &&
        !hir_eval_string_constant(ffi_lexer,
                                  ffi_ast,
                                  ffi_sema,
                                  hir,
                                  ffi_info->library_node_index,
                                  &library)) {
        error_ice("Could not evaluate FFI library string");
    }

    u32 symbol_handle = sema_import_symbol_handle(
        (Lexer*)lexer, ffi_lexer, ffi_info->foreign_symbol_handle);
    for (u32 i = 0; i < array_count(hir->externs); ++i) {
        if (hir->externs[i].symbol_handle == symbol_handle) {
            return;
        }
    }

    array_push(hir->externs,
               (HirExtern){
                   .symbol_handle = symbol_handle,
                   .type_index    = decl->type_index,
                   .library       = library,
               });
}

internal u32 hir_call_arg_value_node(const Ast* ast,
                                     u32        arg_node_index,
                                     u32*       out_symbol)
{
    *out_symbol = U32_MAX;
    if (arg_node_index < array_count(ast->nodes) &&
        ast->nodes[arg_node_index].kind == AK_Assign) {
        const AstNode* assign = &ast->nodes[arg_node_index];
        if (assign->a < array_count(ast->nodes) &&
            ast->nodes[assign->a].kind == AK_SymbolRef) {
            *out_symbol    = ast->nodes[assign->a].a;
            arg_node_index = assign->b;
        }
    }
    return arg_node_index;
}

internal u32 hir_enum_variant_index(const Sema* sema,
                                    u32         enum_type,
                                    u32         symbol_handle)
{
    if (enum_type == sema_no_type() || enum_type >= array_count(sema->types) ||
        sema->types[enum_type].kind != STK_Enum) {
        return U32_MAX;
    }

    const SemaType* type = &sema->types[enum_type];
    for (u32 i = 0; i < type->param_count; ++i) {
        if (sema->type_param_symbols[type->first_param_type + i] ==
            symbol_handle) {
            return i;
        }
    }
    return U32_MAX;
}

internal bool
hir_call_callee_symbol(const Ast* ast, u32 call_node_index, u32* out_symbol)
{
    call_node_index = hir_unwrap_node(ast, call_node_index);
    if (call_node_index >= array_count(ast->nodes) ||
        ast->nodes[call_node_index].kind != AK_Call) {
        return false;
    }

    u32 callee_index = hir_unwrap_node(ast, ast->nodes[call_node_index].a);
    if (callee_index >= array_count(ast->nodes)) {
        return false;
    }

    const AstNode* callee = &ast->nodes[callee_index];
    if (callee->kind == AK_SymbolRef || callee->kind == AK_EnumVariant) {
        *out_symbol = callee->a;
        return true;
    }
    if (callee->kind == AK_Field) {
        *out_symbol = callee->b;
        return true;
    }
    return false;
}

internal u32 hir_lower_expr_with_expected(Hir*         hir,
                                          const Lexer* lexer,
                                          const Ast*   ast,
                                          const Sema*  sema,
                                          u32          node_index,
                                          u32          expected_type)
{
    u32 symbol_handle = U32_MAX;
    if (expected_type != sema_no_type() &&
        expected_type < array_count(sema->types) &&
        sema->types[expected_type].kind == STK_Enum &&
        hir_call_callee_symbol(ast, node_index, &symbol_handle) &&
        hir_enum_variant_index(sema, expected_type, symbol_handle) != U32_MAX) {
        u32                call_node_index = hir_unwrap_node(ast, node_index);
        const AstNode*     call_node       = &ast->nodes[call_node_index];
        const AstCallInfo* call            = &ast->calls[call_node->b];

        u32 callee_expr_index =
            hir_lower_expr(hir, lexer, ast, sema, call_node->a);
        Array(HirCallArg) lowered_args = NULL;
        for (u32 i = 0; i < call->arg_count; ++i) {
            u32 arg_node_index = ast->call_args[call->first_arg + i];
            u32 arg_symbol     = U32_MAX;
            arg_node_index =
                hir_call_arg_value_node(ast, arg_node_index, &arg_symbol);
            array_push(lowered_args,
                       (HirCallArg){
                           .expr_index = hir_lower_expr(
                               hir, lexer, ast, sema, arg_node_index),
                           .symbol_handle = arg_symbol,
                       });
        }
        u32 first_arg = (u32)array_count(hir->call_args);
        for (u32 i = 0; i < array_count(lowered_args); ++i) {
            array_push(hir->call_args, lowered_args[i]);
        }
        array_free(lowered_args);

        return hir_add_expr(
            hir,
            (HirExpr){
                .kind              = HIR_EXPR_Call,
                .type_index        = expected_type,
                .symbol_handle     = U32_MAX,
                .local_index       = sema_no_local(),
                .callee_expr_index = callee_expr_index,
                .first_arg         = first_arg,
                .arg_count         = call->arg_count,
                .source_line = hir_node_source_line(lexer, ast, node_index),
                .source_path = lexer->source.source_path,
            });
    }

    return hir_lower_expr(hir, lexer, ast, sema, node_index);
}

internal u32 hir_lower_eq_trait_expr(Hir*         hir,
                                     const Lexer* lexer,
                                     const Ast*   ast,
                                     const Sema*  sema,
                                     u32          node_index,
                                     HirBinaryOp  binary_op)
{
    if (node_index >= array_count(ast->nodes) ||
        node_index >= array_count(sema->node_method_call_decl_indices)) {
        return hir_no_index();
    }

    u32 decl_index = sema->node_method_call_decl_indices[node_index];
    if (decl_index == sema_no_decl() ||
        decl_index >= array_count(sema->decls)) {
        return hir_no_index();
    }

    const AstNode* node        = &ast->nodes[node_index];
    u32            callee_type = sema->decls[decl_index].type_index;
    u32            binding     = hir_decl_binding(hir, decl_index);
    u32            callee      = hir_add_expr(
        hir,
        (HirExpr){
            .kind          = HIR_EXPR_LocalRef,
            .type_index    = callee_type,
            .symbol_handle = sema->decls[decl_index].symbol_handle,
            .local_index   = sema_no_local(),
            .ref_kind =
                binding != hir_no_index() ? HIR_REF_Binding : HIR_REF_Decl,
            .ref_index = binding != hir_no_index() ? binding : decl_index,
        });

    u32 lhs = hir_lower_expr_with_expected(
        hir,
        lexer,
        ast,
        sema,
        node->a,
        hir_function_param_type(sema, callee_type, 0));
    u32 rhs = hir_lower_expr_with_expected(
        hir,
        lexer,
        ast,
        sema,
        node->b,
        hir_function_param_type(sema, callee_type, 1));

    u32 first_arg = (u32)array_count(hir->call_args);
    array_push(hir->call_args,
               (HirCallArg){.expr_index = lhs, .symbol_handle = U32_MAX});
    array_push(hir->call_args,
               (HirCallArg){.expr_index = rhs, .symbol_handle = U32_MAX});

    u32 call = hir_add_expr(hir,
                            (HirExpr){
                                .kind       = HIR_EXPR_Call,
                                .type_index = hir_node_type(sema, node_index),
                                .symbol_handle     = U32_MAX,
                                .local_index       = sema_no_local(),
                                .callee_expr_index = callee,
                                .first_arg         = first_arg,
                                .arg_count         = 2,
                            });
    if (binary_op == HIR_BINARY_NotEqual) {
        return hir_add_expr(hir,
                            (HirExpr){
                                .kind       = HIR_EXPR_Unary,
                                .type_index = hir_node_type(sema, node_index),
                                .symbol_handle      = U32_MAX,
                                .local_index        = sema_no_local(),
                                .operand_expr_index = call,
                                .unary_op           = HIR_UNARY_LogicalNot,
                            });
    }

    return call;
}

internal u32 hir_lower_order_trait_expr(Hir*         hir,
                                        const Lexer* lexer,
                                        const Ast*   ast,
                                        const Sema*  sema,
                                        u32          node_index,
                                        HirBinaryOp  binary_op)
{
    if (node_index >= array_count(ast->nodes) ||
        node_index >= array_count(sema->node_method_call_decl_indices)) {
        return hir_no_index();
    }

    u32 decl_index = sema->node_method_call_decl_indices[node_index];
    if (decl_index == sema_no_decl() ||
        decl_index >= array_count(sema->decls)) {
        return hir_no_index();
    }

    const AstNode* node        = &ast->nodes[node_index];
    u32            callee_type = sema->decls[decl_index].type_index;
    u32            binding     = hir_decl_binding(hir, decl_index);
    u32            callee      = hir_add_expr(
        hir,
        (HirExpr){
            .kind          = HIR_EXPR_LocalRef,
            .type_index    = callee_type,
            .symbol_handle = sema->decls[decl_index].symbol_handle,
            .local_index   = sema_no_local(),
            .ref_kind =
                binding != hir_no_index() ? HIR_REF_Binding : HIR_REF_Decl,
            .ref_index = binding != hir_no_index() ? binding : decl_index,
        });

    u32 lhs = hir_lower_expr_with_expected(
        hir,
        lexer,
        ast,
        sema,
        node->a,
        hir_function_param_type(sema, callee_type, 0));
    u32 rhs = hir_lower_expr_with_expected(
        hir,
        lexer,
        ast,
        sema,
        node->b,
        hir_function_param_type(sema, callee_type, 1));

    u32 first_arg = (u32)array_count(hir->call_args);
    array_push(hir->call_args,
               (HirCallArg){.expr_index = lhs, .symbol_handle = U32_MAX});
    array_push(hir->call_args,
               (HirCallArg){.expr_index = rhs, .symbol_handle = U32_MAX});

    u32 compare_type = sema->types[callee_type].return_type;
    u32 compare      = hir_add_expr(hir,
                                    (HirExpr){
                                        .kind              = HIR_EXPR_Call,
                                        .type_index        = compare_type,
                                        .symbol_handle     = U32_MAX,
                                        .local_index       = sema_no_local(),
                                        .callee_expr_index = callee,
                                        .first_arg         = first_arg,
                                        .arg_count         = 2,
                                    });
    u32 zero         = hir_add_expr(hir,
                                    (HirExpr){
                                        .kind          = HIR_EXPR_IntegerLiteral,
                                        .type_index    = compare_type,
                                        .symbol_handle = U32_MAX,
                                        .local_index   = sema_no_local(),
                                        .integer       = 0,
                                    });

    return hir_add_expr(hir,
                        (HirExpr){
                            .kind           = HIR_EXPR_Binary,
                            .type_index     = hir_node_type(sema, node_index),
                            .symbol_handle  = U32_MAX,
                            .local_index    = sema_no_local(),
                            .lhs_expr_index = compare,
                            .rhs_expr_index = zero,
                            .binary_op      = binary_op,
                        });
}

internal u32 hir_lower_default_trait_expr(Hir*        hir,
                                          const Sema* sema,
                                          u32         node_index)
{
    if (node_index >= array_count(sema->node_method_call_decl_indices)) {
        return hir_no_index();
    }

    u32 decl_index = sema->node_method_call_decl_indices[node_index];
    if (decl_index == sema_no_decl() ||
        decl_index >= array_count(sema->decls)) {
        return hir_no_index();
    }

    u32 callee_type = sema->decls[decl_index].type_index;
    u32 binding     = hir_decl_binding(hir, decl_index);
    u32 callee      = hir_add_expr(
        hir,
        (HirExpr){
            .kind          = HIR_EXPR_LocalRef,
            .type_index    = callee_type,
            .symbol_handle = sema->decls[decl_index].symbol_handle,
            .local_index   = sema_no_local(),
            .ref_kind =
                binding != hir_no_index() ? HIR_REF_Binding : HIR_REF_Decl,
            .ref_index = binding != hir_no_index() ? binding : decl_index,
        });

    return hir_add_expr(hir,
                        (HirExpr){
                            .kind          = HIR_EXPR_Call,
                            .type_index    = hir_node_type(sema, node_index),
                            .symbol_handle = U32_MAX,
                            .local_index   = sema_no_local(),
                            .callee_expr_index = callee,
                            .first_arg = (u32)array_count(hir->call_args),
                            .arg_count = 0,
                        });
}

internal u32 hir_lower_expr(Hir*         hir,
                            const Lexer* lexer,
                            const Ast*   ast,
                            const Sema*  sema,
                            u32          node_index)
{
    node_index = hir_unwrap_node(ast, node_index);
    if (node_index >= array_count(ast->nodes)) {
        return hir_add_unsupported_expr(hir, sema, node_index);
    }

    const AstNode* node = &ast->nodes[node_index];
    HirUnaryOp     unary_op;
    if (node->kind == AK_ZeroInit) {
        u32 default_expr = hir_lower_default_trait_expr(hir, sema, node_index);
        if (default_expr != hir_no_index()) {
            return default_expr;
        }
    }
    if (node->kind == AK_ZeroInit || node->kind == AK_Undefined) {
        return hir_add_default_value_expr(
            hir, sema, node_index, sema_no_type());
    }

    if (hir_unary_op_from_ast_kind(node->kind, &unary_op)) {
        return hir_add_expr(hir,
                            (HirExpr){
                                .kind       = HIR_EXPR_Unary,
                                .type_index = hir_node_type(sema, node_index),
                                .symbol_handle      = U32_MAX,
                                .local_index        = sema_no_local(),
                                .operand_expr_index = hir_lower_expr(
                                    hir, lexer, ast, sema, node->a),
                                .unary_op = unary_op,
                            });
    }

    HirBinaryOp binary_op;
    if (hir_binary_op_from_ast_kind(node->kind, &binary_op)) {
        if ((binary_op == HIR_BINARY_Equal ||
             binary_op == HIR_BINARY_NotEqual) &&
            node_index < array_count(sema->node_method_call_decl_indices) &&
            sema->node_method_call_decl_indices[node_index] != sema_no_decl()) {
            u32 eq_expr = hir_lower_eq_trait_expr(
                hir, lexer, ast, sema, node_index, binary_op);
            if (eq_expr != hir_no_index()) {
                return eq_expr;
            }
        }
        if ((binary_op == HIR_BINARY_Less ||
             binary_op == HIR_BINARY_LessEqual ||
             binary_op == HIR_BINARY_Greater ||
             binary_op == HIR_BINARY_GreaterEqual) &&
            node_index < array_count(sema->node_method_call_decl_indices) &&
            sema->node_method_call_decl_indices[node_index] != sema_no_decl()) {
            u32 order_expr = hir_lower_order_trait_expr(
                hir, lexer, ast, sema, node_index, binary_op);
            if (order_expr != hir_no_index()) {
                return order_expr;
            }
        }

        u32 lhs_expr_index = hir_lower_expr(hir, lexer, ast, sema, node->a);
        u32 rhs_expr_index = hir_lower_expr(hir, lexer, ast, sema, node->b);
        return hir_add_expr(hir,
                            (HirExpr){
                                .kind       = HIR_EXPR_Binary,
                                .type_index = hir_node_type(sema, node_index),
                                .symbol_handle  = U32_MAX,
                                .local_index    = sema_no_local(),
                                .lhs_expr_index = lhs_expr_index,
                                .rhs_expr_index = rhs_expr_index,
                                .binary_op      = binary_op,
                            });
    }

    switch (node->kind) {
    case AK_IntegerLiteral:
        return hir_add_expr(hir,
                            (HirExpr){
                                .kind       = HIR_EXPR_IntegerLiteral,
                                .type_index = hir_node_type(sema, node_index),
                                .symbol_handle = U32_MAX,
                                .local_index   = sema_no_local(),
                                .integer       = (i64)lexer->integers[node->a],
                            });
    case AK_FloatLiteral:
        return hir_add_expr(hir,
                            (HirExpr){
                                .kind       = HIR_EXPR_FloatLiteral,
                                .type_index = hir_node_type(sema, node_index),
                                .symbol_handle = U32_MAX,
                                .local_index   = sema_no_local(),
                                .floating      = lexer->floats[node->a],
                            });
    case AK_StringLiteral:
        {
            TokenKind token_kind =
                node->token_index < array_count(lexer->tokens)
                    ? lexer->tokens[node->token_index].kind
                    : TK_String;
            return hir_add_expr(
                hir,
                (HirExpr){
                    .kind              = HIR_EXPR_StringLiteral,
                    .type_index        = hir_node_type(sema, node_index),
                    .symbol_handle     = U32_MAX,
                    .local_index       = sema_no_local(),
                    .string_index      = node->a,
                    .string_is_cstring = token_kind == TK_CString,
                });
        }
    case AK_BuiltinMacro:
        {
            u32 line = 0;
            u32 col  = 0;
            if (node->token_index < array_count(lexer->tokens)) {
                const Token* token = &lexer->tokens[node->token_index];
                (void)lex_offset_to_line_col(
                    lexer->source, token->offset, &line, &col);
            }
            UNUSED(col);
            return hir_add_expr(
                hir,
                (HirExpr){
                    .kind          = HIR_EXPR_BuiltinMacro,
                    .type_index    = hir_node_type(sema, node_index),
                    .symbol_handle = node->a,
                    .local_index   = sema_no_local(),
                    .source_line   = line + 1,
                    .source_path   = lexer->source.source_path,
                });
        }
    case AK_StringConcat:
        return hir_add_expr(hir,
                            (HirExpr){
                                .kind       = HIR_EXPR_StringConcat,
                                .type_index = hir_node_type(sema, node_index),
                                .symbol_handle  = U32_MAX,
                                .local_index    = sema_no_local(),
                                .lhs_expr_index = hir_lower_expr(
                                    hir, lexer, ast, sema, node->a),
                                .rhs_expr_index = hir_lower_expr(
                                    hir, lexer, ast, sema, node->b),
                            });
    case AK_InterpPartExpr:
        return hir_lower_expr(hir, lexer, ast, sema, node->a);
    case AK_InterpolatedString:
        {
            Array(HirCallArg) lowered_parts = NULL;
            for (u32 i = node->a; i < node->b; ++i) {
                if (i >= array_count(ast->nodes) ||
                    (ast->nodes[i].kind != AK_StringLiteral &&
                     ast->nodes[i].kind != AK_InterpPartExpr)) {
                    continue;
                }
                array_push(
                    lowered_parts,
                    (HirCallArg){
                        .expr_index = hir_lower_expr(hir, lexer, ast, sema, i),
                        .symbol_handle = U32_MAX,
                    });
            }
            u32 first_arg = (u32)array_count(hir->call_args);
            for (u32 i = 0; i < array_count(lowered_parts); ++i) {
                array_push(hir->call_args, lowered_parts[i]);
            }
            u32 arg_count = (u32)array_count(lowered_parts);
            array_free(lowered_parts);
            return hir_add_expr(
                hir,
                (HirExpr){
                    .kind          = HIR_EXPR_InterpolatedString,
                    .type_index    = hir_node_type(sema, node_index),
                    .symbol_handle = U32_MAX,
                    .local_index   = sema_no_local(),
                    .first_arg     = first_arg,
                    .arg_count     = arg_count,
                });
        }
    case AK_BoolLiteral:
        return hir_add_expr(hir,
                            (HirExpr){
                                .kind       = HIR_EXPR_BoolLiteral,
                                .type_index = hir_node_type(sema, node_index),
                                .symbol_handle = U32_MAX,
                                .local_index   = sema_no_local(),
                                .boolean       = node->a != 0,
                            });
    case AK_NilLiteral:
        return hir_add_expr(hir,
                            (HirExpr){
                                .kind       = HIR_EXPR_NilLiteral,
                                .type_index = hir_node_type(sema, node_index),
                                .symbol_handle = U32_MAX,
                                .local_index   = sema_no_local(),
                            });
    case AK_Assign:
        return hir_add_expr(hir,
                            (HirExpr){
                                .kind       = HIR_EXPR_Assign,
                                .type_index = hir_node_type(sema, node_index),
                                .symbol_handle  = U32_MAX,
                                .local_index    = sema_no_local(),
                                .lhs_expr_index = hir_lower_expr(
                                    hir, lexer, ast, sema, node->a),
                                .rhs_expr_index = hir_lower_expr(
                                    hir, lexer, ast, sema, node->b),
                            });
    case AK_FnDef:
        {
            u32 function_index = hir_no_index();
            hir_add_function(hir,
                             lexer,
                             ast,
                             sema,
                             HIR_FUNCTION_Normal,
                             U32_MAX,
                             hir_node_decl(sema, node_index),
                             node_index,
                             hir_node_scope(sema, node_index),
                             hir_node_type(sema, node_index),
                             &function_index);
            return hir_add_expr(
                hir,
                (HirExpr){
                    .kind          = HIR_EXPR_FunctionRef,
                    .type_index    = hir_node_type(sema, node_index),
                    .symbol_handle = U32_MAX,
                    .local_index   = sema_no_local(),
                    .ref_kind      = HIR_REF_None,
                    .ref_index     = function_index,
                });
        }
    case AK_SymbolRef:
        {
            u32 local_index = hir_node_local(sema, node_index);
            u32 decl_index  = hir_node_decl(sema, node_index);
            u32 type_index  = hir_node_type(sema, node_index);
            if (type_index == sema_no_type()) {
                type_index = hir_local_type(sema, local_index);
            }
            if (type_index == sema_no_type()) {
                type_index = hir_decl_type(sema, decl_index);
            }
            if (hir_enum_variant_index(sema, type_index, node->a) != U32_MAX) {
                return hir_add_expr(hir,
                                    (HirExpr){
                                        .kind          = HIR_EXPR_LocalRef,
                                        .type_index    = type_index,
                                        .symbol_handle = node->a,
                                        .local_index   = sema_no_local(),
                                        .ref_kind      = HIR_REF_None,
                                        .ref_index     = hir_no_index(),
                                    });
            }
            HirRefKind ref_kind  = HIR_REF_None;
            u32        ref_index = hir_no_index();
            if (local_index != sema_no_local()) {
                ref_kind  = HIR_REF_Local;
                ref_index = local_index;
            } else if (decl_index != sema_no_decl()) {
                u32 binding_index = hir_decl_binding(hir, decl_index);
                if (binding_index != hir_no_index()) {
                    ref_kind  = HIR_REF_Binding;
                    ref_index = binding_index;
                } else {
                    ref_kind  = HIR_REF_Decl;
                    ref_index = decl_index;
                }
            }
            return hir_add_expr(hir,
                                (HirExpr){
                                    .kind          = HIR_EXPR_LocalRef,
                                    .type_index    = type_index,
                                    .symbol_handle = node->a,
                                    .local_index   = local_index,
                                    .ref_kind      = ref_kind,
                                    .ref_index     = ref_index,
                                });
        }
    case AK_EnumVariant:
        return hir_add_expr(hir,
                            (HirExpr){
                                .kind       = HIR_EXPR_LocalRef,
                                .type_index = hir_node_type(sema, node_index),
                                .symbol_handle = node->a,
                                .local_index   = sema_no_local(),
                                .ref_kind      = HIR_REF_None,
                                .ref_index     = hir_no_index(),
                            });
    case AK_Call:
        {
            if (node->b >= array_count(ast->calls)) {
                return hir_add_unsupported_expr(hir, sema, node_index);
            }

            if (node_index < array_count(sema->node_method_call_decl_indices) &&
                sema->node_method_call_decl_indices[node_index] !=
                    sema_no_decl() &&
                node->a < array_count(ast->nodes)) {
                u32 method_field_node_index = node->a;
                if (ast->nodes[node->a].kind == AK_Index) {
                    u32 target_node_index = ast->nodes[node->a].a;
                    if (target_node_index < array_count(ast->nodes) &&
                        ast->nodes[target_node_index].kind == AK_Field) {
                        method_field_node_index = target_node_index;
                    }
                }
                if (ast->nodes[method_field_node_index].kind != AK_Field) {
                    return hir_add_unsupported_expr(hir, sema, node_index);
                }
                const AstNode* callee_node =
                    &ast->nodes[method_field_node_index];
                const AstCallInfo* call = &ast->calls[node->b];
                u32                decl_index =
                    sema->node_method_call_decl_indices[node_index];
                u32 callee_type = hir_node_type(sema, node->a);
                u32 lowered_symbol =
                    node->a < array_count(sema->node_lowered_symbol_handles)
                        ? sema->node_lowered_symbol_handles[node->a]
                        : U32_MAX;
                u32 binding_index     = hir_decl_binding(hir, decl_index);
                u32 callee_expr_index = hir_add_expr(
                    hir,
                    (HirExpr){
                        .kind          = HIR_EXPR_LocalRef,
                        .type_index    = callee_type,
                        .symbol_handle = lowered_symbol,
                        .local_index   = sema_no_local(),
                        .ref_kind      = binding_index != hir_no_index()
                                             ? HIR_REF_Binding
                                             : HIR_REF_Decl,
                        .ref_index     = binding_index != hir_no_index()
                                             ? binding_index
                                             : decl_index,
                    });

                Array(HirCallArg) lowered_args = NULL;
                bool explicit_trait_call =
                    node_index <
                        array_count(sema->node_method_call_explicit_traits) &&
                    sema->node_method_call_explicit_traits[node_index];
                u32 receiver_node_index =
                    explicit_trait_call && call->arg_count > 0
                        ? ast->call_args[call->first_arg]
                        : callee_node->a;
                u32 receiver_expr_index =
                    hir_lower_expr(hir, lexer, ast, sema, receiver_node_index);
                u32 receiver_type =
                    hir_function_param_type(sema, callee_type, 0);
                if (receiver_type == sema_no_type()) {
                    receiver_type = hir_node_type(sema, receiver_node_index);
                }
                if (sema->node_method_call_receiver_refs[node_index]) {
                    receiver_expr_index = hir_add_expr(
                        hir,
                        (HirExpr){
                            .kind               = HIR_EXPR_Unary,
                            .type_index         = receiver_type,
                            .symbol_handle      = U32_MAX,
                            .local_index        = sema_no_local(),
                            .operand_expr_index = receiver_expr_index,
                            .unary_op           = HIR_UNARY_AddressOf,
                        });
                } else if (sema->node_method_call_receiver_derefs[node_index]) {
                    receiver_expr_index = hir_add_expr(
                        hir,
                        (HirExpr){
                            .kind               = HIR_EXPR_Unary,
                            .type_index         = receiver_type,
                            .symbol_handle      = U32_MAX,
                            .local_index        = sema_no_local(),
                            .operand_expr_index = receiver_expr_index,
                            .unary_op           = HIR_UNARY_Deref,
                        });
                }
                array_push(lowered_args,
                           (HirCallArg){
                               .expr_index    = receiver_expr_index,
                               .symbol_handle = U32_MAX,
                           });

                u32 first_call_arg = explicit_trait_call ? 1 : 0;
                for (u32 i = first_call_arg; i < call->arg_count; ++i) {
                    u32 arg_node_index = ast->call_args[call->first_arg + i];
                    u32 arg_symbol     = U32_MAX;
                    arg_node_index     = hir_call_arg_value_node(
                        ast, arg_node_index, &arg_symbol);
                    u32 expected_arg_type = hir_function_param_type(
                        sema, callee_type, i + 1 - first_call_arg);
                    array_push(lowered_args,
                               (HirCallArg){
                                   .expr_index = hir_lower_expr_with_expected(
                                       hir,
                                       lexer,
                                       ast,
                                       sema,
                                       arg_node_index,
                                       expected_arg_type),
                                   .symbol_handle = arg_symbol,
                               });
                }

                u32 first_arg = (u32)array_count(hir->call_args);
                for (u32 i = 0; i < array_count(lowered_args); ++i) {
                    array_push(hir->call_args, lowered_args[i]);
                }
                u32 arg_count = (u32)array_count(lowered_args);
                array_free(lowered_args);

                return hir_add_expr(
                    hir,
                    (HirExpr){
                        .kind              = HIR_EXPR_Call,
                        .type_index        = hir_node_type(sema, node_index),
                        .symbol_handle     = U32_MAX,
                        .local_index       = sema_no_local(),
                        .callee_expr_index = callee_expr_index,
                        .first_arg         = first_arg,
                        .arg_count         = arg_count,
                        .source_line =
                            hir_node_source_line(lexer, ast, node_index),
                        .source_path = lexer->source.source_path,
                    });
            }

            u32 callee_expr_index =
                hir_lower_expr(hir, lexer, ast, sema, node->a);
            const AstCallInfo* call        = &ast->calls[node->b];
            u32                callee_type = hir_node_type(sema, node->a);
            if (callee_type == sema_no_type() &&
                callee_expr_index < array_count(hir->exprs)) {
                callee_type = hir->exprs[callee_expr_index].type_index;
            }
            Array(HirCallArg) lowered_args = NULL;
            for (u32 i = 0; i < call->arg_count; ++i) {
                u32 arg_node_index = ast->call_args[call->first_arg + i];
                u32 arg_symbol     = U32_MAX;
                arg_node_index =
                    hir_call_arg_value_node(ast, arg_node_index, &arg_symbol);
                u32 expected_arg_type =
                    hir_function_param_type(sema, callee_type, i);
                array_push(lowered_args,
                           (HirCallArg){
                               .expr_index = hir_lower_expr_with_expected(
                                   hir,
                                   lexer,
                                   ast,
                                   sema,
                                   arg_node_index,
                                   expected_arg_type),
                               .symbol_handle = arg_symbol,
                           });
            }
            u32 first_arg = (u32)array_count(hir->call_args);
            for (u32 i = 0; i < array_count(lowered_args); ++i) {
                array_push(hir->call_args, lowered_args[i]);
            }
            array_free(lowered_args);

            return hir_add_expr(
                hir,
                (HirExpr){
                    .kind              = HIR_EXPR_Call,
                    .type_index        = hir_node_type(sema, node_index),
                    .symbol_handle     = U32_MAX,
                    .local_index       = sema_no_local(),
                    .callee_expr_index = callee_expr_index,
                    .first_arg         = first_arg,
                    .arg_count         = call->arg_count,
                    .source_line = hir_node_source_line(lexer, ast, node_index),
                    .source_path = lexer->source.source_path,
                });
        }
    case AK_Cast:
        {
            if (node->b >= array_count(ast->casts)) {
                return hir_add_unsupported_expr(hir, sema, node_index);
            }

            const AstCastInfo* cast = &ast->casts[node->b];
            return hir_add_expr(
                hir,
                (HirExpr){
                    .kind          = HIR_EXPR_Cast,
                    .type_index    = hir_node_type(sema, node_index),
                    .symbol_handle = U32_MAX,
                    .local_index   = sema_no_local(),
                    .operand_expr_index =
                        hir_lower_expr(hir, lexer, ast, sema, node->a),
                    .extra_expr_index =
                        cast->extra_node_index != U32_MAX
                            ? hir_lower_expr(
                                  hir, lexer, ast, sema, cast->extra_node_index)
                            : hir_no_index(),
                });
        }
    case AK_Index:
        return hir_add_expr(hir,
                            (HirExpr){
                                .kind       = HIR_EXPR_Index,
                                .type_index = hir_node_type(sema, node_index),
                                .symbol_handle      = U32_MAX,
                                .local_index        = sema_no_local(),
                                .operand_expr_index = hir_lower_expr(
                                    hir, lexer, ast, sema, node->a),
                                .extra_expr_index = hir_lower_expr(
                                    hir, lexer, ast, sema, node->b),
                            });
    case AK_Tuple:
    case AK_Array:
        {
            Array(HirCallArg) lowered_args = NULL;
            for (u32 i = 0; i < node->b; ++i) {
                u32 item_node_index = ast->tuple_items[node->a + i];
                array_push(lowered_args,
                           (HirCallArg){
                               .expr_index = hir_lower_expr(
                                   hir, lexer, ast, sema, item_node_index),
                               .symbol_handle = U32_MAX,
                           });
            }
            u32 first_arg = (u32)array_count(hir->call_args);
            for (u32 i = 0; i < array_count(lowered_args); ++i) {
                array_push(hir->call_args, lowered_args[i]);
            }
            array_free(lowered_args);

            return hir_add_expr(
                hir,
                (HirExpr){
                    .kind             = node->kind == AK_Tuple ? HIR_EXPR_Tuple
                                                               : HIR_EXPR_Array,
                    .type_index       = hir_node_type(sema, node_index),
                    .symbol_handle    = U32_MAX,
                    .local_index      = sema_no_local(),
                    .first_arg        = first_arg,
                    .arg_count        = node->b,
                    .extra_expr_index = hir_no_index(),
                });
        }
    case AK_TupleField:
        return hir_add_expr(hir,
                            (HirExpr){
                                .kind       = HIR_EXPR_TupleField,
                                .type_index = hir_node_type(sema, node_index),
                                .symbol_handle      = U32_MAX,
                                .local_index        = sema_no_local(),
                                .integer            = node->b,
                                .operand_expr_index = hir_lower_expr(
                                    hir, lexer, ast, sema, node->a),
                            });
    case AK_Field:
        return hir_add_expr(hir,
                            (HirExpr){
                                .kind       = HIR_EXPR_Field,
                                .type_index = hir_node_type(sema, node_index),
                                .symbol_handle      = node->b,
                                .local_index        = sema_no_local(),
                                .operand_expr_index = hir_lower_expr(
                                    hir, lexer, ast, sema, node->a),
                            });
    case AK_Plex:
    case AK_PlexUpdate:
        {
            if (node->a >= array_count(ast->plex_literals)) {
                return hir_add_unsupported_expr(hir, sema, node_index);
            }

            const AstPlexLiteralInfo* literal = &ast->plex_literals[node->a];
            Array(HirCallArg) lowered_args    = NULL;
            for (u32 i = 0; i < literal->field_count; ++i) {
                const AstPlexLiteralField* field =
                    &ast->plex_literal_fields[literal->first_field + i];
                array_push(
                    lowered_args,
                    (HirCallArg){
                        .expr_index = hir_lower_expr(
                            hir, lexer, ast, sema, field->value_node_index),
                        .symbol_handle = field->symbol_handle,
                    });
            }
            u32 first_arg = (u32)array_count(hir->call_args);
            for (u32 i = 0; i < array_count(lowered_args); ++i) {
                array_push(hir->call_args, lowered_args[i]);
            }
            array_free(lowered_args);

            return hir_add_expr(
                hir,
                (HirExpr){
                    .kind       = node->kind == AK_Plex ? HIR_EXPR_Plex
                                                        : HIR_EXPR_PlexUpdate,
                    .type_index = hir_node_type(sema, node_index),
                    .symbol_handle = U32_MAX,
                    .local_index   = sema_no_local(),
                    .operand_expr_index =
                        node->kind == AK_PlexUpdate
                            ? hir_lower_expr(hir,
                                             lexer,
                                             ast,
                                             sema,
                                             literal->target_node_index)
                            : hir_no_index(),
                    .first_arg = first_arg,
                    .arg_count = literal->field_count,
                    .default_missing =
                        (literal->flags & APLF_DefaultMissing) != 0,
                });
        }
    case AK_Slice:
        {
            if (node->a >= array_count(ast->slices)) {
                return hir_add_unsupported_expr(hir, sema, node_index);
            }

            const AstSliceInfo* slice = &ast->slices[node->a];
            return hir_add_expr(
                hir,
                (HirExpr){
                    .kind               = HIR_EXPR_Slice,
                    .type_index         = hir_node_type(sema, node_index),
                    .symbol_handle      = U32_MAX,
                    .local_index        = sema_no_local(),
                    .operand_expr_index = hir_lower_expr(
                        hir, lexer, ast, sema, slice->target_node_index),
                    .lhs_expr_index =
                        slice->start_node_index != U32_MAX
                            ? hir_lower_expr(hir,
                                             lexer,
                                             ast,
                                             sema,
                                             slice->start_node_index)
                            : hir_no_index(),
                    .rhs_expr_index =
                        slice->end_node_index != U32_MAX
                            ? hir_lower_expr(
                                  hir, lexer, ast, sema, slice->end_node_index)
                            : hir_no_index(),
                });
        }
    case AK_RangeExclusive:
    case AK_RangeInclusive:
        return hir_add_expr(hir,
                            (HirExpr){
                                .kind       = node->kind == AK_RangeExclusive
                                                  ? HIR_EXPR_RangeExclusive
                                                  : HIR_EXPR_RangeInclusive,
                                .type_index = hir_node_type(sema, node_index),
                                .symbol_handle  = U32_MAX,
                                .local_index    = sema_no_local(),
                                .lhs_expr_index = hir_lower_expr(
                                    hir, lexer, ast, sema, node->a),
                                .rhs_expr_index = hir_lower_expr(
                                    hir, lexer, ast, sema, node->b),
                            });
    case AK_ExprBlock:
        return hir_add_expr(hir,
                            (HirExpr){
                                .kind       = HIR_EXPR_Block,
                                .type_index = hir_node_type(sema, node_index),
                                .symbol_handle    = node->b,
                                .local_index      = sema_no_local(),
                                .body_block_index = hir_lower_block_node(
                                    hir, lexer, ast, sema, node->a),
                            });
    case AK_On:
        {
            if (node->b >= array_count(ast->ons)) {
                return hir_add_unsupported_expr(hir, sema, node_index);
            }

            const AstOnInfo* on                 = &ast->ons[node->b];
            Array(HirOnBranch) lowered_branches = NULL;
            for (u32 i = 0; i < on->branch_count; ++i) {
                const AstOnBranch* branch =
                    &ast->on_branches[on->first_branch + i];
                u32 first_pattern = (u32)array_count(hir->on_branch_patterns);
                for (u32 pattern = 0; pattern < branch->pattern_count;
                     ++pattern) {
                    u32 ast_pattern_index =
                        ast->pattern_items[branch->pattern_index + pattern];
                    array_push(hir->on_branch_patterns,
                               hir_lower_pattern(
                                   hir, lexer, ast, sema, ast_pattern_index));
                }

                HirOnBranch hir_branch = {
                    .is_else = (branch->flags & AOBF_Else) != 0,
                    .source_line =
                        hir_token_source_line(lexer, branch->token_index),
                    .source_path   = lexer->source.source_path,
                    .first_pattern = first_pattern,
                    .pattern_count = branch->pattern_count,
                    .guard_expr_index =
                        branch->guard_node_index != U32_MAX
                            ? hir_lower_expr(hir,
                                             lexer,
                                             ast,
                                             sema,
                                             branch->guard_node_index)
                            : hir_no_index(),
                    .body_block_index = hir_lower_on_branch_block(
                        hir, lexer, ast, sema, branch->expr_node_index),
                    .binder_symbol_handle = branch->binder_symbol_handle,
                };
                array_push(lowered_branches, hir_branch);
            }
            u32 first_branch = (u32)array_count(hir->on_branches);
            for (u32 i = 0; i < array_count(lowered_branches); ++i) {
                array_push(hir->on_branches, lowered_branches[i]);
            }
            array_free(lowered_branches);

            HirOnKind on_kind = HIR_ON_Condition;
            if (on->kind == AOK_Bool) {
                on_kind = HIR_ON_Bool;
            } else if (on->kind == AOK_Value) {
                on_kind = HIR_ON_Value;
            }

            return hir_add_expr(
                hir,
                (HirExpr){
                    .kind          = HIR_EXPR_On,
                    .type_index    = hir_node_type(sema, node_index),
                    .symbol_handle = U32_MAX,
                    .local_index   = sema_no_local(),
                    .operand_expr_index =
                        node->a != U32_MAX
                            ? hir_lower_expr(hir, lexer, ast, sema, node->a)
                            : hir_no_index(),
                    .first_branch = first_branch,
                    .branch_count = on->branch_count,
                    .on_kind      = on_kind,
                });
        }
    case AK_For:
        return hir_lower_for(hir, lexer, ast, sema, node_index);
    default:
        return hir_add_unsupported_expr(hir, sema, node_index);
    }
}

internal HirPatternKind hir_pattern_kind_from_ast(AstPatternKind kind)
{
    switch (kind) {
    case APK_Value:
    case APK_ForValue:
        return HIR_PATTERN_Value;
    case APK_Ignore:
        return HIR_PATTERN_Ignore;
    case APK_Bind:
        return HIR_PATTERN_Bind;
    case APK_Equal:
        return HIR_PATTERN_Equal;
    case APK_NotEqual:
        return HIR_PATTERN_NotEqual;
    case APK_Less:
        return HIR_PATTERN_Less;
    case APK_LessEqual:
        return HIR_PATTERN_LessEqual;
    case APK_Greater:
        return HIR_PATTERN_Greater;
    case APK_GreaterEqual:
        return HIR_PATTERN_GreaterEqual;
    case APK_RangeExclusive:
        return HIR_PATTERN_RangeExclusive;
    case APK_RangeInclusive:
        return HIR_PATTERN_RangeInclusive;
    case APK_Tuple:
        return HIR_PATTERN_Tuple;
    case APK_Plex:
        return HIR_PATTERN_Plex;
    case APK_EnumVariant:
        return HIR_PATTERN_EnumVariant;
    default:
        return HIR_PATTERN_Value;
    }
}

internal u32 hir_add_pattern(Hir* hir, HirPattern pattern)
{
    array_push(hir->patterns, pattern);
    return (u32)array_count(hir->patterns) - 1;
}

internal u32 hir_lower_pattern(Hir*         hir,
                               const Lexer* lexer,
                               const Ast*   ast,
                               const Sema*  sema,
                               u32          pattern_index)
{
    if (pattern_index >= array_count(ast->patterns)) {
        return hir_add_pattern(hir, (HirPattern){.kind = HIR_PATTERN_Ignore});
    }

    const AstPattern* pattern = &ast->patterns[pattern_index];
    switch (pattern->kind) {
    case APK_Value:
    case APK_ForValue:
    case APK_Equal:
    case APK_NotEqual:
    case APK_Less:
    case APK_LessEqual:
    case APK_Greater:
    case APK_GreaterEqual:
        return hir_add_pattern(
            hir,
            (HirPattern){
                .kind          = hir_pattern_kind_from_ast(pattern->kind),
                .symbol_handle = U32_MAX,
                .expr_index = hir_lower_expr(hir, lexer, ast, sema, pattern->a),
                .extra_expr_index = hir_no_index(),
            });
    case APK_RangeExclusive:
    case APK_RangeInclusive:
        return hir_add_pattern(
            hir,
            (HirPattern){
                .kind          = hir_pattern_kind_from_ast(pattern->kind),
                .symbol_handle = U32_MAX,
                .expr_index = hir_lower_expr(hir, lexer, ast, sema, pattern->a),
                .extra_expr_index =
                    hir_lower_expr(hir, lexer, ast, sema, pattern->b),
            });
    case APK_Bind:
        return hir_add_pattern(hir,
                               (HirPattern){
                                   .kind          = HIR_PATTERN_Bind,
                                   .symbol_handle = pattern->a,
                                   .expr_index    = hir_no_index(),
                               });
    case APK_Ignore:
        return hir_add_pattern(hir,
                               (HirPattern){
                                   .kind          = HIR_PATTERN_Ignore,
                                   .symbol_handle = U32_MAX,
                                   .expr_index    = hir_no_index(),
                               });
    case APK_Tuple:
        {
            u32 first_child = (u32)array_count(hir->pattern_children);
            for (u32 i = 0; i < pattern->b; ++i) {
                u32 child = ast->pattern_items[pattern->a + i];
                array_push(hir->pattern_children,
                           (HirPatternChild){
                               .symbol_handle = U32_MAX,
                               .pattern_index = hir_lower_pattern(
                                   hir, lexer, ast, sema, child),
                           });
            }
            return hir_add_pattern(hir,
                                   (HirPattern){
                                       .kind          = HIR_PATTERN_Tuple,
                                       .symbol_handle = U32_MAX,
                                       .first_child   = first_child,
                                       .child_count   = pattern->b,
                                   });
        }
    case APK_Plex:
        {
            u32 first_child = (u32)array_count(hir->pattern_children);
            for (u32 i = 0; i < pattern->b; ++i) {
                const AstPlexPatternField* field =
                    &ast->pattern_fields[pattern->a + i];
                array_push(hir->pattern_children,
                           (HirPatternChild){
                               .symbol_handle = field->symbol_handle,
                               .pattern_index = hir_lower_pattern(
                                   hir, lexer, ast, sema, field->pattern_index),
                           });
            }
            return hir_add_pattern(hir,
                                   (HirPattern){
                                       .kind          = HIR_PATTERN_Plex,
                                       .symbol_handle = U32_MAX,
                                       .first_child   = first_child,
                                       .child_count   = pattern->b,
                                   });
        }
    case APK_EnumVariant:
        {
            if (pattern->a >= array_count(ast->enum_patterns)) {
                return hir_add_pattern(
                    hir, (HirPattern){.kind = HIR_PATTERN_Ignore});
            }

            const AstEnumPattern* enum_pattern =
                &ast->enum_patterns[pattern->a];
            u32 first_child = (u32)array_count(hir->pattern_children);
            for (u32 i = 0; i < enum_pattern->pattern_count; ++i) {
                u32 child = ast->pattern_items[enum_pattern->first_pattern + i];
                array_push(hir->pattern_children,
                           (HirPatternChild){
                               .symbol_handle = U32_MAX,
                               .pattern_index = hir_lower_pattern(
                                   hir, lexer, ast, sema, child),
                           });
            }
            return hir_add_pattern(
                hir,
                (HirPattern){
                    .kind          = HIR_PATTERN_EnumVariant,
                    .symbol_handle = enum_pattern->symbol_handle,
                    .expr_index =
                        enum_pattern->qualifier_node_index != U32_MAX
                            ? hir_lower_expr(hir,
                                             lexer,
                                             ast,
                                             sema,
                                             enum_pattern->qualifier_node_index)
                            : hir_no_index(),
                    .first_child = first_child,
                    .child_count = enum_pattern->pattern_count,
                });
        }
    default:
        return hir_add_pattern(hir, (HirPattern){.kind = HIR_PATTERN_Ignore});
    }
}

internal u32 hir_add_stmt(Hir* hir, HirStmt stmt)
{
    array_push(hir->stmts, stmt);
    return (u32)array_count(hir->stmts) - 1;
}

internal bool hir_append_destructure_tuple_items(Hir*        hir,
                                                 const Ast*  ast,
                                                 const Sema* sema,
                                                 u32         pattern_index,
                                                 u32*        out_count)
{
    if (pattern_index >= array_count(ast->patterns)) {
        return false;
    }

    const AstPattern* pattern = &ast->patterns[pattern_index];
    if (pattern->kind != APK_Tuple) {
        return false;
    }

    for (u32 i = 0; i < pattern->b; ++i) {
        u32 child_index = ast->pattern_items[pattern->a + i];
        if (child_index >= array_count(ast->patterns)) {
            return false;
        }

        const AstPattern* child = &ast->patterns[child_index];
        if (child->kind == APK_Ignore) {
            continue;
        }

        u32 local_index = child_index < array_count(sema->pattern_local_indices)
                              ? sema->pattern_local_indices[child_index]
                              : sema_no_local();
        if (local_index == sema_no_local()) {
            return false;
        }

        array_push(hir->destructure_items,
                   (HirDestructureItem){
                       .local_index = local_index,
                       .type_index  = hir_local_type(sema, local_index),
                       .field_index = i,
                   });
        (*out_count)++;
    }
    return true;
}

internal u32 hir_lower_stmt(Hir*         hir,
                            const Lexer* lexer,
                            const Ast*   ast,
                            const Sema*  sema,
                            u32          node_index);

internal u32 hir_lower_single_stmt_block(Hir*         hir,
                                         const Lexer* lexer,
                                         const Ast*   ast,
                                         const Sema*  sema,
                                         u32          node_index)
{
    u32 block_index = (u32)array_count(hir->blocks);
    array_push(hir->blocks,
               (HirBlock){
                   .first_stmt = 0,
                   .stmt_count = 0,
                   .scope_index = hir_node_scope(sema, node_index),
               });

    u32 stmt_index = hir_lower_stmt(hir, lexer, ast, sema, node_index);
    if (stmt_index != hir_no_index()) {
        array_push(hir->blocks[block_index].stmt_indices, stmt_index);
        hir->blocks[block_index].stmt_count++;
    }
    return block_index;
}

internal u32 hir_lower_on_branch_block(Hir*         hir,
                                       const Lexer* lexer,
                                       const Ast*   ast,
                                       const Sema*  sema,
                                       u32          expr_node_index)
{
    u32 root_index = hir_unwrap_node(ast, expr_node_index);
    if (root_index < array_count(ast->nodes)) {
        const AstNode* root = &ast->nodes[root_index];
        if (root->kind == AK_ExprBlock && root->b == U32_MAX) {
            return hir_lower_block_node(hir, lexer, ast, sema, root->a);
        }
    }
    return hir_lower_single_stmt_block(hir, lexer, ast, sema, expr_node_index);
}

internal u32 hir_lower_stmt(Hir*         hir,
                            const Lexer* lexer,
                            const Ast*   ast,
                            const Sema*  sema,
                            u32          node_index)
{
    node_index = hir_unwrap_node(ast, node_index);
    if (node_index >= array_count(ast->nodes)) {
        return hir_no_index();
    }

    const AstNode* node             = &ast->nodes[node_index];
    string         stmt_source_path = {0};
    u32            stmt_source_line = 0;
    hir_node_source_location(
        lexer, ast, node_index, &stmt_source_path, &stmt_source_line);
    switch (node->kind) {
    case AK_Pragma:
        return hir_no_index();
    case AK_Return:
    case AK_ReturnExpr:
        return hir_add_stmt(
            hir,
            (HirStmt){
                .kind = HIR_STMT_Return,
                .expr_index =
                    node->a != U32_MAX
                        ? hir_lower_expr(hir, lexer, ast, sema, node->a)
                        : hir_no_index(),
                .symbol_handle    = U32_MAX,
                .local_index      = sema_no_local(),
                .type_index       = hir_node_type(sema, node_index),
                .body_block_index = hir_no_index(),
                .source_line      = stmt_source_line,
                .source_path      = stmt_source_path,
            });
    case AK_Bind:
    case AK_Variable:
        {
            u32 value_node_index = node->b;
            if (value_node_index < array_count(ast->nodes) &&
                ast->nodes[value_node_index].kind == AK_AnnotatedValue) {
                value_node_index = ast->nodes[value_node_index].b;
            }
            u32 expr_index =
                value_node_index < array_count(ast->nodes)
                    ? hir_lower_expr(hir, lexer, ast, sema, value_node_index)
                    : hir_no_index();
            u32 local_index = hir_node_local(sema, node_index);
            u32 local_type  = hir_local_type(sema, local_index);
            if (expr_index == hir_no_index() && local_type != sema_no_type()) {
                expr_index =
                    hir_lower_default_trait_expr(hir, sema, node_index);
                if (expr_index == hir_no_index()) {
                    expr_index = hir_add_default_value_expr(
                        hir, sema, node_index, local_type);
                }
            }
            if (local_type < array_count(sema->types) &&
                sema->types[local_type].kind == STK_DynamicArray) {
                u32 min_capacity_node =
                    hir_local_dynamic_array_min_capacity_node(
                        ast, sema, local_index);
                u32 min_capacity = hir_local_dynamic_array_min_capacity(
                    ast, sema, local_index);
                bool min_capacity_is_runtime =
                    min_capacity_node != hir_no_index() &&
                    !hir_node_is_const_known(sema, min_capacity_node);
                bool should_make_array_init =
                    min_capacity_node != hir_no_index() &&
                    expr_index >= array_count(hir->exprs);
                if (expr_index < array_count(hir->exprs) &&
                    (hir->exprs[expr_index].kind == HIR_EXPR_Unsupported ||
                     hir->exprs[expr_index].kind == HIR_EXPR_DefaultValue)) {
                    u32 init_node = value_node_index;
                    while (init_node < array_count(ast->nodes) &&
                           ast->nodes[init_node].kind == AK_Expression) {
                        init_node = ast->nodes[init_node].a;
                    }
                    should_make_array_init =
                        min_capacity_node != hir_no_index() &&
                        init_node < array_count(ast->nodes) &&
                        (ast->nodes[init_node].kind == AK_ZeroInit ||
                         ast->nodes[init_node].kind == AK_Undefined);
                }
                if (should_make_array_init) {
                    u32 min_capacity_expr_index =
                        min_capacity_is_runtime
                            ? hir_lower_expr(
                                  hir, lexer, ast, sema, min_capacity_node)
                            : hir_no_index();
                    expr_index = hir_add_expr(
                        hir,
                        (HirExpr){
                            .kind             = HIR_EXPR_Array,
                            .type_index       = local_type,
                            .symbol_handle    = U32_MAX,
                            .local_index      = sema_no_local(),
                            .extra_expr_index = min_capacity_expr_index,
                        });
                }
                if (expr_index < array_count(hir->exprs) &&
                    hir->exprs[expr_index].kind == HIR_EXPR_Array) {
                    hir->exprs[expr_index].integer = min_capacity;
                    if (hir->exprs[expr_index].extra_expr_index ==
                            hir_no_index() &&
                        min_capacity_is_runtime) {
                        u32 min_capacity_expr_index = hir_lower_expr(
                            hir, lexer, ast, sema, min_capacity_node);
                        hir->exprs[expr_index].extra_expr_index =
                            min_capacity_expr_index;
                    }
                }
            }
            return hir_add_stmt(hir,
                                (HirStmt){
                                    .kind             = HIR_STMT_Let,
                                    .expr_index       = expr_index,
                                    .symbol_handle    = node->a,
                                    .local_index      = local_index,
                                    .type_index       = local_type,
                                    .body_block_index = hir_no_index(),
                                    .source_line      = stmt_source_line,
                                    .source_path      = stmt_source_path,
                                });
        }
    case AK_Assign:
        return hir_add_stmt(
            hir,
            (HirStmt){
                .kind       = HIR_STMT_Assign,
                .expr_index = hir_lower_expr(hir, lexer, ast, sema, node->b),
                .target_expr_index =
                    hir_lower_expr(hir, lexer, ast, sema, node->a),
                .symbol_handle    = U32_MAX,
                .local_index      = hir_node_local(sema, node_index),
                .type_index       = hir_node_type(sema, node_index),
                .body_block_index = hir_no_index(),
                .source_line      = stmt_source_line,
                .source_path      = stmt_source_path,
            });
    case AK_DestructureBind:
    case AK_DestructureVariable:
    case AK_DestructureAssign:
        {
            u32 value_node_index = node->b;
            if (value_node_index < array_count(ast->nodes) &&
                ast->nodes[value_node_index].kind == AK_AnnotatedValue) {
                value_node_index = ast->nodes[value_node_index].b;
            }
            u32 first_item = (u32)array_count(hir->destructure_items);
            u32 item_count = 0;
            if (!hir_append_destructure_tuple_items(
                    hir, ast, sema, node->a, &item_count)) {
                return hir_add_stmt(
                    hir,
                    (HirStmt){
                        .kind = HIR_STMT_Expr,
                        .expr_index =
                            hir_add_unsupported_expr(hir, sema, node_index),
                        .symbol_handle    = U32_MAX,
                        .local_index      = sema_no_local(),
                        .type_index       = hir_node_type(sema, node_index),
                        .body_block_index = hir_no_index(),
                    });
            }

            return hir_add_stmt(
                hir,
                (HirStmt){
                    .kind = node->kind == AK_DestructureAssign
                                ? HIR_STMT_DestructureAssign
                                : HIR_STMT_DestructureLet,
                    .expr_index =
                        value_node_index < array_count(ast->nodes)
                            ? hir_lower_expr(
                                  hir, lexer, ast, sema, value_node_index)
                            : hir_no_index(),
                    .target_expr_index = first_item,
                    .symbol_handle     = U32_MAX,
                    .local_index       = sema_no_local(),
                    .type_index        = hir_node_type(sema, node_index),
                    .body_block_index  = item_count,
                    .source_line       = stmt_source_line,
                    .source_path       = stmt_source_path,
                });
        }
    case AK_Assert:
        {
            return hir_add_stmt(
                hir,
                (HirStmt){
                    .kind = HIR_STMT_Assert,
                    .expr_index =
                        hir_lower_expr(hir, lexer, ast, sema, node->a),
                    .target_expr_index =
                        node->b != U32_MAX
                            ? hir_lower_expr(hir, lexer, ast, sema, node->b)
                            : hir_no_index(),
                    .symbol_handle    = U32_MAX,
                    .local_index      = sema_no_local(),
                    .type_index       = hir_node_type(sema, node_index),
                    .body_block_index = hir_no_index(),
                    .source_line      = stmt_source_line,
                    .source_path      = stmt_source_path,
                });
        }
    case AK_Defer:
        return hir_add_stmt(hir,
                            (HirStmt){
                                .kind              = HIR_STMT_Defer,
                                .expr_index        = hir_no_index(),
                                .target_expr_index = hir_no_index(),
                                .symbol_handle     = U32_MAX,
                                .local_index       = sema_no_local(),
                                .type_index = hir_node_type(sema, node_index),
                                .body_block_index = hir_lower_single_stmt_block(
                                    hir, lexer, ast, sema, node->a),
                                .source_line = stmt_source_line,
                                .source_path = stmt_source_path,
                            });
    case AK_Break:
    case AK_BreakExpr:
        return hir_add_stmt(
            hir,
            (HirStmt){
                .kind = HIR_STMT_Break,
                .expr_index =
                    node->a != U32_MAX
                        ? hir_lower_expr(hir, lexer, ast, sema, node->a)
                        : hir_no_index(),
                .symbol_handle    = node->b,
                .local_index      = sema_no_local(),
                .type_index       = hir_node_type(sema, node_index),
                .body_block_index = hir_no_index(),
                .source_line      = stmt_source_line,
                .source_path      = stmt_source_path,
            });
    case AK_Continue:
    case AK_ContinueExpr:
        return hir_add_stmt(hir,
                            (HirStmt){
                                .kind          = HIR_STMT_Continue,
                                .expr_index    = hir_no_index(),
                                .symbol_handle = node->b,
                                .local_index   = sema_no_local(),
                                .type_index = hir_node_type(sema, node_index),
                                .body_block_index = hir_no_index(),
                                .source_line      = stmt_source_line,
                                .source_path      = stmt_source_path,
                            });
    case AK_Block:
        return hir_add_stmt(hir,
                            (HirStmt){
                                .kind          = HIR_STMT_Block,
                                .expr_index    = hir_no_index(),
                                .symbol_handle = U32_MAX,
                                .local_index   = sema_no_local(),
                                .type_index = hir_node_type(sema, node_index),
                                .body_block_index = hir_lower_block_node(
                                    hir, lexer, ast, sema, node_index),
                                .source_line = stmt_source_line,
                                .source_path = stmt_source_path,
                            });
    default:
        return hir_add_stmt(
            hir,
            (HirStmt){
                .kind       = HIR_STMT_Expr,
                .expr_index = hir_lower_expr(hir, lexer, ast, sema, node_index),
                .symbol_handle    = U32_MAX,
                .local_index      = sema_no_local(),
                .type_index       = hir_node_type(sema, node_index),
                .body_block_index = hir_no_index(),
                .source_line      = stmt_source_line,
                .source_path      = stmt_source_path,
            });
    }
}

internal HirForKind hir_for_kind_from_ast(u32 mode)
{
    switch (mode) {
    case AFM_CStyle:
        return HIR_FOR_CStyle;
    case AFM_In:
        return HIR_FOR_In;
    case AFM_Condition:
    default:
        return HIR_FOR_Condition;
    }
}

internal u32 hir_lower_for_stmt_items(Hir*         hir,
                                      const Lexer* lexer,
                                      const Ast*   ast,
                                      const Sema*  sema,
                                      Array(u32) * out_items,
                                      u32 first_item,
                                      u32 item_count)
{
    u32 first = (u32)array_count(*out_items);
    if (first_item == U32_MAX) {
        return first;
    }

    for (u32 i = 0; i < item_count; ++i) {
        u32 ast_item_index = ast->for_items[first_item + i];
        u32 stmt_index = hir_lower_stmt(hir, lexer, ast, sema, ast_item_index);
        if (stmt_index != hir_no_index()) {
            array_push(*out_items, stmt_index);
        }
    }
    return first;
}

internal u32 hir_lower_for(Hir*         hir,
                           const Lexer* lexer,
                           const Ast*   ast,
                           const Sema*  sema,
                           u32          node_index)
{
    if (node_index >= array_count(ast->nodes)) {
        return hir_add_unsupported_expr(hir, sema, node_index);
    }

    const AstNode* node = &ast->nodes[node_index];
    if (node->a >= array_count(ast->fors)) {
        return hir_add_unsupported_expr(hir, sema, node_index);
    }

    const AstForInfo* for_info = &ast->fors[node->a];
    u32 first_init             = hir_lower_for_stmt_items(hir,
                                                          lexer,
                                                          ast,
                                                          sema,
                                                          &hir->for_init_stmts,
                                                          for_info->first_init,
                                                          for_info->init_count);
    u32 first_update = hir_lower_for_stmt_items(hir,
                                                lexer,
                                                ast,
                                                sema,
                                                &hir->for_update_stmts,
                                                for_info->first_update,
                                                for_info->update_count);
    u32 for_scope    = node_index < array_count(sema->node_scope_indices)
                           ? sema->node_scope_indices[node_index]
                           : U32_MAX;
    u32 for_index    = (u32)array_count(hir->fors);
    array_push(hir->fors, (HirFor){0});

    u32 condition_expr_index =
        for_info->condition_node_index != U32_MAX
            ? hir_lower_expr(
                  hir, lexer, ast, sema, for_info->condition_node_index)
            : hir_no_index();
    u32 iterable_expr_index =
        for_info->iterable_node_index != U32_MAX
            ? hir_lower_expr(
                  hir, lexer, ast, sema, for_info->iterable_node_index)
            : hir_no_index();
    u32 body_block_index = hir_lower_block_node(hir, lexer, ast, sema, node->b);
    u32 else_block_index =
        for_info->else_block_index != U32_MAX
            ? hir_lower_block_node(
                  hir, lexer, ast, sema, for_info->else_block_index)
            : hir_no_index();

    hir->fors[for_index] = (HirFor){
        .kind                 = hir_for_kind_from_ast(for_info->mode),
        .label_symbol         = for_info->label_symbol,
        .condition_expr_index = condition_expr_index,
        .iterable_expr_index  = iterable_expr_index,
        .body_block_index     = body_block_index,
        .else_block_index     = else_block_index,
        .first_init_stmt      = first_init,
        .init_stmt_count      = for_info->init_count,
        .first_update_stmt    = first_update,
        .update_stmt_count    = for_info->update_count,
        .index_symbol         = for_info->index_symbol,
        .index_local_index =
            hir_find_scope_local(sema, for_scope, for_info->index_symbol),
        .item_symbol = for_info->item_symbol,
        .item_local_index =
            hir_find_scope_local(sema, for_scope, for_info->item_symbol),
        .iterator_next_decl_index =
            node_index < array_count(sema->node_method_call_decl_indices)
                ? sema->node_method_call_decl_indices[node_index]
                : sema_no_decl(),
    };

    return hir_add_expr(hir,
                        (HirExpr){
                            .kind          = HIR_EXPR_For,
                            .type_index    = hir_node_type(sema, node_index),
                            .symbol_handle = U32_MAX,
                            .local_index   = sema_no_local(),
                            .for_index     = for_index,
                        });
}

internal void hir_mark_owned_ast_subtree(
    const Ast* ast, bool* owned_nodes, u32 first, u32 end, u32 node_index)
{
    if (node_index >= end) {
        return;
    }

    u32 root_index = hir_unwrap_node(ast, node_index);
    if (node_index >= first) {
        owned_nodes[node_index] = true;
    }
    if (root_index < end && root_index >= first) {
        owned_nodes[root_index] = true;
    }

    if (root_index < end && ast->nodes[root_index].kind == AK_Block) {
        u32 child_first = ast->nodes[root_index].a;
        u32 child_end   = ast->nodes[root_index].b;
        if (child_end > end) {
            child_end = end;
        }
        for (u32 i = child_first; i < child_end; ++i) {
            owned_nodes[i] = true;
        }
    }

    for (u32 i = first; i < end; ++i) {
        if ((ast->nodes[i].kind == AK_Statement ||
             ast->nodes[i].kind == AK_Expression) &&
            (hir_unwrap_node(ast, i) == node_index ||
             hir_unwrap_node(ast, i) == root_index)) {
            owned_nodes[i] = true;
        }
    }
}

internal void hir_mark_owned_function_body(
    const Ast* ast, bool* owned_nodes, u32 first, u32 end, u32 fn_node_index)
{
    if (fn_node_index >= end || ast->nodes[fn_node_index].kind != AK_FnDef) {
        return;
    }

    const AstNode* fn_def         = &ast->nodes[fn_node_index];
    u32            fn_start_index = fn_def->a;
    if (fn_start_index >= end ||
        ast->nodes[fn_start_index].kind != AK_FnStart) {
        return;
    }

    u32 fn_end = ast->nodes[fn_start_index].b;
    if (fn_end > end) {
        fn_end = end;
    }
    if (fn_node_index >= first) {
        owned_nodes[fn_node_index] = true;
    }
    if (fn_start_index >= first) {
        owned_nodes[fn_start_index] = true;
    }
    for (u32 i = fn_start_index + 1; i < fn_end; ++i) {
        owned_nodes[i] = true;
    }
}

internal void hir_mark_owned_function_start(
    const Ast* ast, bool* owned_nodes, u32 first, u32 end, u32 fn_start_index)
{
    if (fn_start_index >= end ||
        ast->nodes[fn_start_index].kind != AK_FnStart) {
        return;
    }

    u32 fn_end = ast->nodes[fn_start_index].b;
    if (fn_end > end) {
        fn_end = end;
    }
    for (u32 i = fn_start_index; i <= fn_end && i < end; ++i) {
        if (i >= first) {
            owned_nodes[i] = true;
        }
    }
    u32 fn_def_index = fn_end + 1;
    if (fn_def_index < end && ast->nodes[fn_def_index].kind == AK_FnDef &&
        ast->nodes[fn_def_index].a == fn_start_index) {
        owned_nodes[fn_def_index] = true;
    }
}

internal void hir_mark_owned_on_branch_bodies(
    const Ast* ast, bool* owned_nodes, u32 first, u32 end, u32 on_node_index)
{
    if (on_node_index >= end || ast->nodes[on_node_index].kind != AK_On) {
        return;
    }

    const AstNode* on_node = &ast->nodes[on_node_index];
    if (on_node->b >= array_count(ast->ons)) {
        return;
    }

    const AstOnInfo* on = &ast->ons[on_node->b];
    for (u32 i = 0; i < on->branch_count; ++i) {
        const AstOnBranch* branch = &ast->on_branches[on->first_branch + i];
        hir_mark_owned_ast_subtree(
            ast, owned_nodes, first, end, branch->expr_node_index);
    }
}

internal void hir_mark_owned_for_parts(
    const Ast* ast, bool* owned_nodes, u32 first, u32 end, u32 for_node_index)
{
    if (for_node_index >= end || ast->nodes[for_node_index].kind != AK_For) {
        return;
    }

    const AstNode* for_node = &ast->nodes[for_node_index];
    if (for_node->a >= array_count(ast->fors)) {
        return;
    }

    const AstForInfo* for_info = &ast->fors[for_node->a];
    for (u32 i = 0; i < for_info->init_count; ++i) {
        hir_mark_owned_ast_subtree(ast,
                                   owned_nodes,
                                   first,
                                   end,
                                   ast->for_items[for_info->first_init + i]);
    }
    for (u32 i = 0; i < for_info->update_count; ++i) {
        hir_mark_owned_ast_subtree(ast,
                                   owned_nodes,
                                   first,
                                   end,
                                   ast->for_items[for_info->first_update + i]);
    }
    hir_mark_owned_ast_subtree(ast, owned_nodes, first, end, for_node->b);
    if (for_info->else_block_index != U32_MAX) {
        hir_mark_owned_ast_subtree(
            ast, owned_nodes, first, end, for_info->else_block_index);
    }
}

internal void hir_mark_owned_embedded_for_expr(
    const Ast* ast, bool* owned_nodes, u32 first, u32 end, u32 expr_node_index)
{
    u32 root_index = hir_unwrap_node(ast, expr_node_index);
    if (root_index >= end || ast->nodes[root_index].kind != AK_For) {
        return;
    }

    if (root_index >= first) {
        owned_nodes[root_index] = true;
    }
    hir_mark_owned_for_parts(ast, owned_nodes, first, end, root_index);
}

internal void hir_mark_owned_statement_exprs(
    const Ast* ast, bool* owned_nodes, u32 first, u32 end, u32 node_index)
{
    if (node_index >= end) {
        return;
    }

    const AstNode* node = &ast->nodes[node_index];
    switch (node->kind) {
    case AK_Return:
    case AK_ReturnExpr:
        if (node->a != U32_MAX) {
            hir_mark_owned_embedded_for_expr(
                ast, owned_nodes, first, end, node->a);
        }
        break;
    case AK_Bind:
    case AK_Variable:
        {
            u32 value_node_index = node->b;
            if (value_node_index < end &&
                ast->nodes[value_node_index].kind == AK_AnnotatedValue) {
                value_node_index = ast->nodes[value_node_index].b;
            }
            u32 value_root = hir_unwrap_node(ast, value_node_index);
            if (value_root < end && ast->nodes[value_root].kind == AK_FnDef) {
                hir_mark_owned_function_body(
                    ast, owned_nodes, first, end, value_root);
            }
            hir_mark_owned_embedded_for_expr(
                ast, owned_nodes, first, end, value_node_index);
            break;
        }
    case AK_Assign:
        hir_mark_owned_embedded_for_expr(ast, owned_nodes, first, end, node->a);
        hir_mark_owned_embedded_for_expr(ast, owned_nodes, first, end, node->b);
        break;
    case AK_Assert:
        hir_mark_owned_embedded_for_expr(ast, owned_nodes, first, end, node->a);
        if (node->b != U32_MAX) {
            hir_mark_owned_embedded_for_expr(
                ast, owned_nodes, first, end, node->b);
        }
        break;
    default:
        break;
    }
}

internal u32 hir_lower_block_node(Hir*         hir,
                                  const Lexer* lexer,
                                  const Ast*   ast,
                                  const Sema*  sema,
                                  u32          block_node_index)
{
    block_node_index = hir_unwrap_node(ast, block_node_index);
    if (block_node_index >= array_count(ast->nodes) ||
        ast->nodes[block_node_index].kind != AK_Block) {
        return hir_lower_single_stmt_block(
            hir, lexer, ast, sema, block_node_index);
    }

    const AstNode* block_node = &ast->nodes[block_node_index];
    u32            first      = block_node->a;
    u32            end        = block_node->b;
    if (end > array_count(ast->nodes)) {
        end = (u32)array_count(ast->nodes);
    }

    u32 block_index = (u32)array_count(hir->blocks);
    array_push(hir->blocks,
               (HirBlock){
                   .first_stmt = 0,
                   .stmt_count = 0,
                   .scope_index = hir_node_scope(sema, block_node_index),
               });

    bool* owned_nodes = arena_alloc(&hir->arena, sizeof(bool) * end);
    memset(owned_nodes, 0, sizeof(bool) * end);
    for (u32 i = first; i < end; ++i) {
        if (ast->nodes[i].kind == AK_ExprBlock) {
            u32 child_block_index = ast->nodes[i].a;
            if (child_block_index < end &&
                ast->nodes[child_block_index].kind == AK_Block) {
                owned_nodes[child_block_index] = true;
                u32 child_first = ast->nodes[child_block_index].a;
                u32 child_end   = ast->nodes[child_block_index].b;
                if (child_end > end) {
                    child_end = end;
                }
                for (u32 j = child_first; j < child_end; ++j) {
                    owned_nodes[j] = true;
                }
            }
        }

        u32 owned_root = hir_unwrap_node(ast, i);
        if (owned_root < array_count(ast->nodes) &&
            ast->nodes[owned_root].kind == AK_Block) {
            u32 child_first = ast->nodes[owned_root].a;
            u32 child_end   = ast->nodes[owned_root].b;
            if (child_end > end) {
                child_end = end;
            }
            for (u32 j = child_first; j < child_end; ++j) {
                owned_nodes[j] = true;
            }
        }

        if (ast->nodes[i].kind == AK_On) {
            hir_mark_owned_on_branch_bodies(ast, owned_nodes, first, end, i);
        }

        if (ast->nodes[i].kind == AK_For) {
            hir_mark_owned_for_parts(ast, owned_nodes, first, end, i);
        }

        if (ast->nodes[i].kind == AK_FnDef) {
            hir_mark_owned_function_body(ast, owned_nodes, first, end, i);
        }

        if (ast->nodes[i].kind == AK_FnStart) {
            hir_mark_owned_function_start(ast, owned_nodes, first, end, i);
        }

        hir_mark_owned_statement_exprs(ast, owned_nodes, first, end, i);

        if (ast->nodes[i].kind != AK_Defer) {
            continue;
        }

        u32 deferred_node = ast->nodes[i].a;
        u32 deferred_root = hir_unwrap_node(ast, deferred_node);
        if (deferred_node < end) {
            owned_nodes[deferred_node] = true;
        }
        if (deferred_root < end) {
            owned_nodes[deferred_root] = true;
        }
        for (u32 j = first; j < end; ++j) {
            if ((ast->nodes[j].kind == AK_Statement ||
                 ast->nodes[j].kind == AK_Expression) &&
                (hir_unwrap_node(ast, j) == deferred_node ||
                 hir_unwrap_node(ast, j) == deferred_root)) {
                owned_nodes[j] = true;
            }
        }
    }

    for (u32 i = first; i < end; ++i) {
        AstKind kind = ast->nodes[i].kind;
        if (owned_nodes[i] || hir_ast_kind_is_expression_child(kind)) {
            continue;
        }
        u32 stmt_index = hir_lower_stmt(hir, lexer, ast, sema, i);
        if (stmt_index != hir_no_index()) {
            array_push(hir->blocks[block_index].stmt_indices, stmt_index);
            hir->blocks[block_index].stmt_count++;
        }
    }

    return block_index;
}

internal u32 hir_lower_function_body(Hir*         hir,
                                     const Lexer* lexer,
                                     const Ast*   ast,
                                     const Sema*  sema,
                                     u32          fn_node_index)
{
    if (fn_node_index >= array_count(ast->nodes) ||
        ast->nodes[fn_node_index].kind != AK_FnDef) {
        return hir_no_index();
    }

    const AstNode* fn_node        = &ast->nodes[fn_node_index];
    u32            fn_start_index = fn_node->a;
    if (fn_start_index >= array_count(ast->nodes) ||
        ast->nodes[fn_start_index].kind != AK_FnStart) {
        return hir_no_index();
    }

    const AstNode* fn_start = &ast->nodes[fn_start_index];
    u32            fn_end   = fn_start->b;
    if (fn_end > array_count(ast->nodes)) {
        fn_end = (u32)array_count(ast->nodes);
    }

    u32 block_index = (u32)array_count(hir->blocks);
    array_push(hir->blocks,
               (HirBlock){
                   .first_stmt = 0,
                   .stmt_count = 0,
                   .scope_index = hir_node_scope(sema, fn_node_index),
               });

    if (fn_node->b == AFK_Expr) {
        u32 expr_node_index = fn_end > 0 ? fn_end - 1 : hir_no_index();
        if (expr_node_index < array_count(ast->nodes)) {
            u32 stmt_index = hir_add_stmt(
                hir,
                (HirStmt){
                    .kind = HIR_STMT_Return,
                    .expr_index =
                        hir_lower_expr(hir, lexer, ast, sema, expr_node_index),
                    .symbol_handle    = U32_MAX,
                    .local_index      = sema_no_local(),
                    .type_index       = hir_node_type(sema, expr_node_index),
                    .body_block_index = hir_no_index(),
                });
            array_push(hir->blocks[block_index].stmt_indices, stmt_index);
            hir->blocks[block_index].stmt_count++;
        }
        return block_index;
    }

    bool* owned_nodes = arena_alloc(&hir->arena, sizeof(bool) * fn_end);
    memset(owned_nodes, 0, sizeof(bool) * fn_end);
    for (u32 i = fn_start_index + 1; i < fn_end; ++i) {
        if (ast->nodes[i].kind == AK_ExprBlock) {
            u32 child_block_index = ast->nodes[i].a;
            if (child_block_index < fn_end &&
                ast->nodes[child_block_index].kind == AK_Block) {
                owned_nodes[child_block_index] = true;
                u32 child_first = ast->nodes[child_block_index].a;
                u32 child_end   = ast->nodes[child_block_index].b;
                if (child_end > fn_end) {
                    child_end = fn_end;
                }
                for (u32 j = child_first; j < child_end; ++j) {
                    owned_nodes[j] = true;
                }
            }
        }

        u32 owned_root = hir_unwrap_node(ast, i);
        if (owned_root < array_count(ast->nodes) &&
            ast->nodes[owned_root].kind == AK_Block) {
            u32 child_first = ast->nodes[owned_root].a;
            u32 child_end   = ast->nodes[owned_root].b;
            if (child_end > fn_end) {
                child_end = fn_end;
            }
            for (u32 j = child_first; j < child_end; ++j) {
                owned_nodes[j] = true;
            }
        }

        if (ast->nodes[i].kind == AK_On) {
            hir_mark_owned_on_branch_bodies(
                ast, owned_nodes, fn_start_index + 1, fn_end, i);
        }

        if (ast->nodes[i].kind == AK_For) {
            hir_mark_owned_for_parts(
                ast, owned_nodes, fn_start_index + 1, fn_end, i);
        }

        if (ast->nodes[i].kind == AK_FnDef) {
            hir_mark_owned_function_body(
                ast, owned_nodes, fn_start_index + 1, fn_end, i);
        }

        if (ast->nodes[i].kind == AK_FnStart) {
            hir_mark_owned_function_start(
                ast, owned_nodes, fn_start_index + 1, fn_end, i);
        }

        hir_mark_owned_statement_exprs(
            ast, owned_nodes, fn_start_index + 1, fn_end, i);

        if (ast->nodes[i].kind != AK_Defer) {
            continue;
        }

        u32 deferred_node = ast->nodes[i].a;
        u32 deferred_root = hir_unwrap_node(ast, deferred_node);
        if (deferred_node < fn_end) {
            owned_nodes[deferred_node] = true;
        }
        if (deferred_root < fn_end) {
            owned_nodes[deferred_root] = true;
        }
        for (u32 j = fn_start_index + 1; j < fn_end; ++j) {
            if ((ast->nodes[j].kind == AK_Statement ||
                 ast->nodes[j].kind == AK_Expression) &&
                (hir_unwrap_node(ast, j) == deferred_node ||
                 hir_unwrap_node(ast, j) == deferred_root)) {
                owned_nodes[j] = true;
            }
        }
    }

    for (u32 i = fn_start_index + 1; i < fn_end; ++i) {
        AstKind kind = ast->nodes[i].kind;
        if (owned_nodes[i]) {
            continue;
        }
        if (hir_ast_kind_is_expression_child(kind)) {
            continue;
        }
        u32 stmt_index = hir_lower_stmt(hir, lexer, ast, sema, i);
        if (stmt_index != hir_no_index()) {
            array_push(hir->blocks[block_index].stmt_indices, stmt_index);
            hir->blocks[block_index].stmt_count++;
        }
    }

    return block_index;
}

internal void hir_add_function_params(Hir*         hir,
                                      const Lexer* lexer,
                                      const Ast*   ast,
                                      const Sema*  sema,
                                      u32          function_index,
                                      u32          fn_node_index,
                                      u32          root_scope_index)
{
    if (root_scope_index == U32_MAX ||
        root_scope_index >= array_count(sema->scopes)) {
        HirFunction* function = &hir->functions[function_index];
        if (function->type_index == sema_no_type() ||
            function->type_index >= array_count(sema->types) ||
            sema->types[function->type_index].kind != STK_Function) {
            return;
        }

        const SemaType* function_type = &sema->types[function->type_index];
        function->first_param         = (u32)array_count(hir->params);

        if (fn_node_index >= array_count(ast->nodes) ||
            ast->nodes[fn_node_index].kind != AK_FnDef) {
            for (u32 i = 0; i < function_type->param_count; ++i) {
                array_push(
                    hir->params,
                    (HirParam){
                        .symbol_handle = U32_MAX,
                        .local_index   = sema_no_local(),
                        .type_index = sema->type_param_types
                                          [function_type->first_param_type + i],
                        .default_expr_index = hir_no_index(),
                    });
                function = &hir->functions[function_index];
                function->param_count++;
            }
            return;
        }

        u32 fn_start_index = ast->nodes[fn_node_index].a;
        if (fn_start_index >= array_count(ast->nodes) ||
            ast->nodes[fn_start_index].kind != AK_FnStart) {
            return;
        }

        const AstFnSignature* signature =
            &ast->fn_signatures[ast->nodes[fn_start_index].a];
        u32 count = signature->param_count < function_type->param_count
                        ? signature->param_count
                        : function_type->param_count;
        for (u32 i = 0; i < count; ++i) {
            const AstParam* param = &ast->params[signature->first_param + i];
            array_push(
                hir->params,
                (HirParam){
                    .symbol_handle = param->symbol_handle,
                    .local_index   = sema_no_local(),
                    .type_index =
                        sema->type_param_types[function_type->first_param_type +
                                               i],
                    .default_expr_index =
                        param->default_node_index != U32_MAX
                            ? hir_lower_expr(hir,
                                             lexer,
                                             ast,
                                             sema,
                                             param->default_node_index)
                            : hir_no_index(),
                });
            function = &hir->functions[function_index];
            function->param_count++;
        }
        return;
    }

    HirFunction*     function       = &hir->functions[function_index];
    const SemaScope* scope          = &sema->scopes[root_scope_index];
    function->first_param           = (u32)array_count(hir->params);
    const AstFnSignature* signature = NULL;
    if (fn_node_index < array_count(ast->nodes) &&
        ast->nodes[fn_node_index].kind == AK_FnDef) {
        u32 fn_start_index = ast->nodes[fn_node_index].a;
        if (fn_start_index < array_count(ast->nodes) &&
            ast->nodes[fn_start_index].kind == AK_FnStart &&
            ast->nodes[fn_start_index].a < array_count(ast->fn_signatures)) {
            signature = &ast->fn_signatures[ast->nodes[fn_start_index].a];
        }
    }

    for (u32 i = 0; i < scope->local_count; ++i) {
        u32 local_index = scope->first_local + i;
        if (local_index >= array_count(sema->locals)) {
            break;
        }

        const SemaLocal* local = &sema->locals[local_index];
        if (local->kind != SLK_Param) {
            continue;
        }
        u32 default_node_index = U32_MAX;
        if (signature != NULL) {
            for (u32 j = 0; j < signature->param_count; ++j) {
                const AstParam* param =
                    &ast->params[signature->first_param + j];
                if (param->symbol_handle == local->symbol_handle) {
                    default_node_index = param->default_node_index;
                    break;
                }
            }
        }

        array_push(hir->params,
                   (HirParam){
                       .symbol_handle = local->symbol_handle,
                       .local_index   = local_index,
                       .type_index    = local->type_index,
                       .default_expr_index =
                           default_node_index != U32_MAX
                               ? hir_lower_expr(
                                     hir, lexer, ast, sema, default_node_index)
                               : hir_no_index(),
                   });
        function = &hir->functions[function_index];
        function->param_count++;
    }
}

internal u32 hir_add_binding(Hir*           hir,
                             HirBindingKind kind,
                             u32            symbol_handle,
                             u32            target_index);

internal void hir_add_function(Hir*            hir,
                               const Lexer*    lexer,
                               const Ast*      ast,
                               const Sema*     sema,
                               HirFunctionKind kind,
                               u32             binding_symbol_handle,
                               u32             decl_index,
                               u32             fn_node_index,
                               u32             root_scope_index,
                               u32             type_index,
                               u32*            out_function_index)
{
    if (root_scope_index == U32_MAX) {
        root_scope_index = hir_find_function_scope(ast, fn_node_index);
    }

    u32 ffi_symbol_handle = U32_MAX;
    if (kind == HIR_FUNCTION_Ffi && decl_index < array_count(sema->decls)) {
        ffi_symbol_handle = hir_ffi_foreign_symbol_handle(
            lexer, ast, sema, &sema->decls[decl_index]);
    }

    u32 function_index = (u32)array_count(hir->functions);
    array_push(hir->functions,
               (HirFunction){
                   .kind              = kind,
                   .decl_index        = decl_index,
                   .fn_node_index     = fn_node_index,
                   .root_scope_index  = root_scope_index,
                   .type_index        = type_index,
                   .ffi_symbol_handle = ffi_symbol_handle,
                   .first_param       = (u32)array_count(hir->params),
                   .param_count       = 0,
                   .body_block_index  = hir_no_index(),
               });
    if (out_function_index) {
        *out_function_index = function_index;
    }
    if (binding_symbol_handle != U32_MAX) {
        u32 binding_index = hir_add_binding(
            hir, HIR_BINDING_Function, binding_symbol_handle, function_index);
        hir_set_decl_binding(hir, decl_index, binding_index);
    }

    u32 body_block_index =
        kind == HIR_FUNCTION_Ffi
            ? hir_no_index()
            : hir_lower_function_body(hir, lexer, ast, sema, fn_node_index);
    hir->functions[function_index].body_block_index = body_block_index;
    hir_add_function_params(
        hir, lexer, ast, sema, function_index, fn_node_index, root_scope_index);
}

internal u32 hir_add_type_def(Hir*           hir,
                              HirTypeDefKind kind,
                              u32            decl_index,
                              u32            type_index)
{
    u32 type_def_index = (u32)array_count(hir->type_defs);
    array_push(hir->type_defs,
               (HirTypeDef){
                   .kind       = kind,
                   .decl_index = decl_index,
                   .type_index = type_index,
               });
    return type_def_index;
}

internal u32 hir_add_value(Hir*         hir,
                           const Lexer* lexer,
                           const Ast*   ast,
                           const Sema*  sema,
                           HirValueKind kind,
                           u32          binding_symbol_handle,
                           u32          decl_index,
                           u32          type_index,
                           u32          value_node_index)
{
    u32 value_index = (u32)array_count(hir->values);
    array_push(hir->values,
               (HirValue){
                   .kind             = kind,
                   .decl_index       = decl_index,
                   .type_index       = type_index,
                   .value_expr_index = hir_no_index(),
               });
    if (binding_symbol_handle != U32_MAX) {
        u32 binding_index = hir_add_binding(
            hir, HIR_BINDING_Value, binding_symbol_handle, value_index);
        hir_set_decl_binding(hir, decl_index, binding_index);
    }

    u32 value_expr_index =
        value_node_index != U32_MAX &&
                value_node_index < array_count(ast->nodes)
            ? hir_lower_expr(hir, lexer, ast, sema, value_node_index)
            : hir_no_index();
    HirValue* value         = &hir->values[value_index];
    value->value_expr_index = value_expr_index;
    return value_index;
}

internal u32 hir_add_binding(Hir*           hir,
                             HirBindingKind kind,
                             u32            symbol_handle,
                             u32            target_index)
{
    u32 binding_index = (u32)array_count(hir->bindings);
    array_push(hir->bindings,
               (HirBinding){
                   .kind          = kind,
                   .symbol_handle = symbol_handle,
                   .target_index  = target_index,
               });
    return binding_index;
}

internal u32 hir_add_import(Hir*         hir,
                            const Lexer* lexer,
                            const Ast*   ast,
                            const Sema*  sema,
                            u32          decl_index)
{
    if (decl_index >= array_count(sema->decls)) {
        return hir_no_index();
    }

    const SemaDecl* decl = &sema->decls[decl_index];
    if (decl->import_module_index == sema_no_decl()) {
        return hir_no_index();
    }

    u32 ffi_symbol_handle =
        hir_ffi_foreign_symbol_handle(lexer, ast, sema, decl);

    u32 import_index = (u32)array_count(hir->imports);
    array_push(hir->imports,
               (HirImport){
                   .module_index      = decl->import_module_index,
                   .decl_index        = decl->import_decl_index,
                   .symbol_handle     = decl->symbol_handle,
                   .ffi_symbol_handle = ffi_symbol_handle,
                   .type_index        = decl->type_index,
               });

    u32 binding_index = hir_add_binding(
        hir, HIR_BINDING_Import, decl->symbol_handle, import_index);
    hir_set_decl_binding(hir, decl_index, binding_index);
    if (ffi_symbol_handle != U32_MAX) {
        hir_add_extern(hir, lexer, ast, sema, decl);
    }
    return binding_index;
}

internal void hir_add_import_bindings(Hir*         hir,
                                      const Lexer* lexer,
                                      const Ast*   ast,
                                      const Sema*  sema)
{
    for (u32 i = 0; i < array_count(sema->decls); ++i) {
        if (hir_decl_binding(hir, i) != hir_no_index()) {
            continue;
        }

        const SemaDecl* decl = &sema->decls[i];
        if (decl->import_module_index != sema_no_decl()) {
            hir_add_import(hir, lexer, ast, sema, i);
        }
    }
}

internal void hir_add_module_records(Hir* hir, const Sema* sema)
{
    if (hir->current_module_index == hir_no_index() || sema == NULL ||
        sema->program == NULL ||
        hir->current_module_index >= array_count(sema->program->modules)) {
        return;
    }

    const ModuleInfo* module =
        &sema->program->modules[hir->current_module_index];
    for (u32 i = 0; i < array_count(module->imported_module_indices); ++i) {
        array_push(hir->module_imports,
                   (HirModuleImport){
                       .module_index = module->imported_module_indices[i],
                   });
    }

    for (u32 i = 0; i < array_count(module->export_decl_indices); ++i) {
        u32 decl_index    = module->export_decl_indices[i];
        u32 binding_index = hir_decl_binding(hir, decl_index);
        if (binding_index == hir_no_index()) {
            continue;
        }
        array_push(hir->exports,
                   (HirExport){
                       .decl_index    = decl_index,
                       .binding_index = binding_index,
                   });
    }
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
    Hir hir = {0};
    arena_init(&hir.arena);
    hir.current_module_index = hir_find_current_module_index(sema);

    for (u32 i = 0; i < array_count(sema->decls); ++i) {
        array_push(hir.decl_binding_indices, hir_no_index());
    }

    hir_add_import_bindings(&hir, lexer, ast, sema);

    for (u32 i = 0; i < array_count(sema->ordered_decl_indices); ++i) {
        u32 decl_index = sema->ordered_decl_indices[i];
        if (decl_index >= array_count(sema->decls)) {
            continue;
        }

        const SemaDecl* decl = &sema->decls[decl_index];
        switch (decl->kind) {
        case SK_Trait:
            break;
        case SK_Module:
            {
                u32 module_index = hir_no_index();
                if (decl->type_index < array_count(sema->types) &&
                    sema->types[decl->type_index].kind == STK_Module) {
                    module_index = sema->types[decl->type_index].return_type;
                }
                u32 binding_index = hir_add_binding(&hir,
                                                    HIR_BINDING_Module,
                                                    decl->symbol_handle,
                                                    module_index);
                hir_set_decl_binding(&hir, decl_index, binding_index);
                break;
            }
        case SK_TypeAlias:
        case SK_GenericTypeAlias:
            {
                u32 type_def_index = hir_add_type_def(
                    &hir,
                    decl->kind == SK_GenericTypeAlias ? HIR_TYPE_Generic
                                                      : HIR_TYPE_Normal,
                    decl_index,
                    decl->type_index);
                u32 binding_index = hir_add_binding(&hir,
                                                    HIR_BINDING_Type,
                                                    decl->symbol_handle,
                                                    type_def_index);
                hir_set_decl_binding(&hir, decl_index, binding_index);
                break;
            }
        case SK_Constant:
        case SK_Variable:
            {
                u32 value_index = hir_add_value(&hir,
                                                lexer,
                                                ast,
                                                sema,
                                                decl->kind == SK_Variable
                                                    ? HIR_VALUE_Global
                                                    : HIR_VALUE_Constant,
                                                decl->symbol_handle,
                                                decl_index,
                                                decl->type_index,
                                                decl->value_node_index);
                (void)value_index;
                break;
            }
        case SK_Function:
        case SK_FfiFunction:
            {
                if (decl->kind == SK_FfiFunction) {
                    hir_add_extern(&hir, lexer, ast, sema, decl);
                }

                u32 fn_node_index = hir_decl_fn_node(ast, decl);
                u32 root_scope_index =
                    fn_node_index != sema_no_decl() &&
                            fn_node_index <
                                array_count(sema->node_scope_indices)
                        ? sema->node_scope_indices[fn_node_index]
                        : U32_MAX;
                u32 function_index = hir_no_index();
                hir_add_function(&hir,
                                 lexer,
                                 ast,
                                 sema,
                                 decl->kind == SK_FfiFunction
                                     ? HIR_FUNCTION_Ffi
                                     : HIR_FUNCTION_Normal,
                                 decl->symbol_handle,
                                 decl_index,
                                 fn_node_index,
                                 root_scope_index,
                                 decl->type_index,
                                 &function_index);
                break;
            }
        default:
            break;
        }
    }

    hir_add_module_records(&hir, sema);

    for (u32 i = 0; i < array_count(sema->generic_fn_instantiations); ++i) {
        const SemaGenericFnInstantiation* inst =
            &sema->generic_fn_instantiations[i];
        Sema inst_sema               = *sema;
        inst_sema.node_decl_indices  = inst->node_decl_indices;
        inst_sema.node_local_indices = inst->node_local_indices;
        inst_sema.node_scope_indices = inst->node_scope_indices;
        inst_sema.node_lowered_symbol_handles =
            inst->node_lowered_symbol_handles;
        inst_sema.node_type_indices = inst->node_type_indices;
        inst_sema.node_method_call_decl_indices =
            inst->node_method_call_decl_indices;
        inst_sema.node_method_call_receiver_refs =
            inst->node_method_call_receiver_refs;
        inst_sema.node_method_call_receiver_derefs =
            inst->node_method_call_receiver_derefs;
        inst_sema.node_method_call_explicit_traits =
            inst->node_method_call_explicit_traits;
        inst_sema.locals = sema->locals;
        inst_sema.scopes = sema->scopes;
        hir_add_function(&hir,
                         lexer,
                         ast,
                         &inst_sema,
                         HIR_FUNCTION_GenericInstantiation,
                         U32_MAX,
                         inst->template_decl_index,
                         inst->fn_node_index,
                         inst->root_scope_index,
                         inst->type_index,
                         NULL);
    }

    return hir;
}

void hir_done(Hir* hir)
{
    array_free(hir->module_imports);
    array_free(hir->imports);
    array_free(hir->externs);
    array_free(hir->exports);
    array_free(hir->bindings);
    array_free(hir->type_defs);
    array_free(hir->values);
    array_free(hir->functions);
    array_free(hir->params);
    for (u32 i = 0; i < array_count(hir->blocks); ++i) {
        array_free(hir->blocks[i].stmt_indices);
    }
    array_free(hir->blocks);
    array_free(hir->stmts);
    array_free(hir->destructure_items);
    array_free(hir->exprs);
    array_free(hir->call_args);
    array_free(hir->on_branches);
    array_free(hir->on_branch_patterns);
    array_free(hir->patterns);
    array_free(hir->pattern_children);
    array_free(hir->fors);
    array_free(hir->for_init_stmts);
    array_free(hir->for_update_stmts);
    array_free(hir->decl_binding_indices);
    arena_done(&hir->arena);
    *hir = (Hir){0};
}

//------------------------------------------------------------------------------
