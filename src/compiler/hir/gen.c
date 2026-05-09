//------------------------------------------------------------------------------
// HIR generation
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

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

internal u32 hir_local_type(const Sema* sema, u32 local_index)
{
    return local_index < array_count(sema->locals)
               ? sema->locals[local_index].type_index
               : sema_no_type();
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

internal u32 hir_lower_pattern(Hir*         hir,
                               const Lexer* lexer,
                               const Ast*   ast,
                               const Sema*  sema,
                               u32          pattern_index);

internal u32 hir_add_expr(Hir* hir, HirExpr expr)
{
    array_push(hir->exprs, expr);
    return (u32)array_count(hir->exprs) - 1;
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

internal bool hir_unary_op_from_ast_kind(AstKind kind, HirUnaryOp* out)
{
    switch (kind) {
    case AK_LogicalNot:
        *out = HIR_UNARY_LogicalNot;
        return true;
    case AK_IntegerNegate:
        *out = HIR_UNARY_Negate;
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
    case AK_BoolLiteral:
    case AK_NilLiteral:
    case AK_SymbolRef:
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
    case AK_Expression:
        return true;
    default:
        return false;
    }
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
    case AK_SymbolRef:
        {
            u32 local_index = hir_node_local(sema, node_index);
            u32 type_index  = hir_node_type(sema, node_index);
            if (type_index == sema_no_type()) {
                type_index = hir_local_type(sema, local_index);
            }
            return hir_add_expr(hir,
                                (HirExpr){
                                    .kind          = HIR_EXPR_LocalRef,
                                    .type_index    = type_index,
                                    .symbol_handle = node->a,
                                    .local_index   = local_index,
                                });
        }
    case AK_Call:
        {
            if (node->b >= array_count(ast->calls)) {
                return hir_add_unsupported_expr(hir, sema, node_index);
            }

            u32 callee_expr_index =
                hir_lower_expr(hir, lexer, ast, sema, node->a);
            const AstCallInfo* call      = &ast->calls[node->b];
            u32                first_arg = (u32)array_count(hir->call_args);
            for (u32 i = 0; i < call->arg_count; ++i) {
                u32 arg_node_index = ast->call_args[call->first_arg + i];
                array_push(hir->call_args,
                           (HirCallArg){
                               .expr_index = hir_lower_expr(
                                   hir, lexer, ast, sema, arg_node_index),
                               .symbol_handle = U32_MAX,
                           });
            }

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
            u32 first_arg = (u32)array_count(hir->call_args);
            for (u32 i = 0; i < node->b; ++i) {
                u32 item_node_index = ast->tuple_items[node->a + i];
                array_push(hir->call_args,
                           (HirCallArg){
                               .expr_index = hir_lower_expr(
                                   hir, lexer, ast, sema, item_node_index),
                               .symbol_handle = U32_MAX,
                           });
            }

            return hir_add_expr(
                hir,
                (HirExpr){
                    .kind          = node->kind == AK_Tuple ? HIR_EXPR_Tuple
                                                            : HIR_EXPR_Array,
                    .type_index    = hir_node_type(sema, node_index),
                    .symbol_handle = U32_MAX,
                    .local_index   = sema_no_local(),
                    .first_arg     = first_arg,
                    .arg_count     = node->b,
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
            u32 first_arg = (u32)array_count(hir->call_args);
            for (u32 i = 0; i < literal->field_count; ++i) {
                const AstPlexLiteralField* field =
                    &ast->plex_literal_fields[literal->first_field + i];
                array_push(
                    hir->call_args,
                    (HirCallArg){
                        .expr_index = hir_lower_expr(
                            hir, lexer, ast, sema, field->value_node_index),
                        .symbol_handle = field->symbol_handle,
                    });
            }

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
                    .first_arg    = first_arg,
                    .arg_count    = literal->field_count,
                    .zero_missing = (literal->flags & APLF_ZeroMissing) != 0,
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

            const AstOnInfo* on           = &ast->ons[node->b];
            u32              first_branch = (u32)array_count(hir->on_branches);
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
                    .is_else       = (branch->flags & AOBF_Else) != 0,
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
                    .body_block_index = hir_lower_single_stmt_block(
                        hir, lexer, ast, sema, branch->expr_node_index),
                    .binder_symbol_handle = branch->binder_symbol_handle,
                };
                array_push(hir->on_branches, hir_branch);
            }

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
    default:
        return hir_add_unsupported_expr(hir, sema, node_index);
    }
}

internal HirPatternKind hir_pattern_kind_from_ast(AstPatternKind kind)
{
    switch (kind) {
    case APK_Value:
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
               });

    u32 stmt_index = hir_lower_stmt(hir, lexer, ast, sema, node_index);
    if (stmt_index != hir_no_index()) {
        array_push(hir->blocks[block_index].stmt_indices, stmt_index);
        hir->blocks[block_index].stmt_count++;
    }
    return block_index;
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

    const AstNode* node = &ast->nodes[node_index];
    switch (node->kind) {
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
            });
    case AK_Bind:
    case AK_Variable:
        {
            u32 value_node_index = node->b;
            if (value_node_index < array_count(ast->nodes) &&
                ast->nodes[value_node_index].kind == AK_AnnotatedValue) {
                value_node_index = ast->nodes[value_node_index].b;
            }
            return hir_add_stmt(
                hir,
                (HirStmt){
                    .kind = HIR_STMT_Let,
                    .expr_index =
                        value_node_index < array_count(ast->nodes)
                            ? hir_lower_expr(
                                  hir, lexer, ast, sema, value_node_index)
                            : hir_no_index(),
                    .symbol_handle = node->a,
                    .local_index   = hir_node_local(sema, node_index),
                    .type_index =
                        hir_local_type(sema, hir_node_local(sema, node_index)),
                    .body_block_index = hir_no_index(),
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
            });
    case AK_Assert:
        return hir_add_stmt(
            hir,
            (HirStmt){
                .kind       = HIR_STMT_Assert,
                .expr_index = hir_lower_expr(hir, lexer, ast, sema, node->a),
                .target_expr_index =
                    node->b != U32_MAX
                        ? hir_lower_expr(hir, lexer, ast, sema, node->b)
                        : hir_no_index(),
                .symbol_handle    = U32_MAX,
                .local_index      = sema_no_local(),
                .type_index       = hir_node_type(sema, node_index),
                .body_block_index = hir_no_index(),
            });
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
            });
    }
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

        if (ast->nodes[i].kind == AK_On) {
            hir_mark_owned_on_branch_bodies(ast, owned_nodes, first, end, i);
        }

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
               });

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

        if (ast->nodes[i].kind == AK_On) {
            hir_mark_owned_on_branch_bodies(
                ast, owned_nodes, fn_start_index + 1, fn_end, i);
        }

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

internal void hir_add_function_params(Hir*        hir,
                                      const Ast*  ast,
                                      const Sema* sema,
                                      u32         function_index,
                                      u32         fn_node_index,
                                      u32         root_scope_index)
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
                });
            function = &hir->functions[function_index];
            function->param_count++;
        }
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
                               const Lexer*    lexer,
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

    u32 body_block_index =
        kind == HIR_FUNCTION_Ffi
            ? hir_no_index()
            : hir_lower_function_body(hir, lexer, ast, sema, fn_node_index);

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
                   .body_block_index = body_block_index,
               });
    hir_add_function_params(
        hir, ast, sema, function_index, fn_node_index, root_scope_index);
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
                         lexer,
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
                         lexer,
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
    for (u32 i = 0; i < array_count(hir->blocks); ++i) {
        array_free(hir->blocks[i].stmt_indices);
    }
    array_free(hir->blocks);
    array_free(hir->stmts);
    array_free(hir->exprs);
    array_free(hir->call_args);
    array_free(hir->on_branches);
    array_free(hir->on_branch_patterns);
    array_free(hir->patterns);
    array_free(hir->pattern_children);
    arena_done(&hir->arena);
    *hir = (Hir){0};
}

//------------------------------------------------------------------------------
