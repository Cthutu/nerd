//------------------------------------------------------------------------------
// HIR dumping
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/error/error.h>
#include <compiler/hir/hir.h>
#include <stdio.h>

//------------------------------------------------------------------------------

internal cstr hir_function_prefix(HirFunctionKind kind)
{
    switch (kind) {
    case HIR_FUNCTION_Normal:
        return "func";
    case HIR_FUNCTION_Ffi:
        return "extern func";
    case HIR_FUNCTION_GenericInstantiation:
        return "inst func";
    default:
        return "func";
    }
}

internal string hir_function_name(const HirFunction* function,
                                  const Lexer*       lexer)
{
    if (function->symbol_handle == U32_MAX) {
        return s("<anonymous>");
    }
    return lex_symbol(lexer, function->symbol_handle);
}

internal void hir_append_type_name(StringBuilder* sb,
                                   const Lexer*   lexer,
                                   const Sema*    sema,
                                   u32            type_index)
{
    if (!sema || type_index == sema_no_type() ||
        type_index >= array_count(sema->types)) {
        sb_append_cstr(sb, "<unknown>");
        return;
    }

    Arena temp = {0};
    arena_init(&temp);
    string name = sema_type_name(lexer, sema, &temp, type_index);
    sb_append_string(sb, name);
    arena_done(&temp);
}

internal void hir_append_function_return_type_name(StringBuilder*     sb,
                                                   const HirFunction* function,
                                                   const Lexer*       lexer,
                                                   const Sema*        sema)
{
    if (!sema || function->type_index == sema_no_type() ||
        function->type_index >= array_count(sema->types) ||
        sema->types[function->type_index].kind != STK_Function) {
        sb_append_cstr(sb, "<unknown>");
        return;
    }
    hir_append_type_name(
        sb, lexer, sema, sema->types[function->type_index].return_type);
}

internal cstr hir_binary_op_name(HirBinaryOp op)
{
    switch (op) {
    case HIR_BINARY_Add:
        return "add";
    case HIR_BINARY_Subtract:
        return "subtract";
    case HIR_BINARY_Multiply:
        return "multiply";
    case HIR_BINARY_Divide:
        return "divide";
    case HIR_BINARY_Modulo:
        return "modulo";
    case HIR_BINARY_BitwiseAnd:
        return "bitwise_and";
    case HIR_BINARY_BitwiseXor:
        return "bitwise_xor";
    case HIR_BINARY_BitwiseOr:
        return "bitwise_or";
    case HIR_BINARY_ShiftLeft:
        return "shift_left";
    case HIR_BINARY_ShiftRight:
        return "shift_right";
    case HIR_BINARY_Equal:
        return "equal";
    case HIR_BINARY_NotEqual:
        return "not_equal";
    case HIR_BINARY_Less:
        return "less";
    case HIR_BINARY_LessEqual:
        return "less_equal";
    case HIR_BINARY_Greater:
        return "greater";
    case HIR_BINARY_GreaterEqual:
        return "greater_equal";
    case HIR_BINARY_LogicalAnd:
        return "logical_and";
    case HIR_BINARY_LogicalOr:
        return "logical_or";
    default:
        return "binary";
    }
}

internal cstr hir_unary_op_name(HirUnaryOp op)
{
    switch (op) {
    case HIR_UNARY_LogicalNot:
        return "logical_not";
    case HIR_UNARY_Negate:
        return "negate";
    case HIR_UNARY_AddressOf:
        return "address_of";
    case HIR_UNARY_Deref:
        return "deref";
    default:
        return "unary";
    }
}

internal void hir_render_expr(StringBuilder* sb,
                              const Hir*     hir,
                              const Lexer*   lexer,
                              const Sema*    sema,
                              Arena*         arena,
                              u32            expr_index);

internal void hir_render_block_at_indent(StringBuilder* sb,
                                         const Hir*     hir,
                                         const Lexer*   lexer,
                                         const Sema*    sema,
                                         Arena*         arena,
                                         u32            block_index,
                                         u32            indent);

internal void hir_render_pattern(StringBuilder* sb,
                                 const Hir*     hir,
                                 const Lexer*   lexer,
                                 const Sema*    sema,
                                 Arena*         arena,
                                 u32            pattern_index);

internal void hir_render_expr_arg_list(StringBuilder* sb,
                                       const Hir*     hir,
                                       const Lexer*   lexer,
                                       const Sema*    sema,
                                       Arena*         arena,
                                       const HirExpr* expr,
                                       bool           named,
                                       bool           zero_missing)
{
    for (u32 i = 0; i < expr->arg_count; ++i) {
        if (i > 0) {
            sb_append_cstr(sb, ", ");
        }
        u32 arg_index = expr->first_arg + i;
        if (arg_index >= array_count(hir->call_args)) {
            sb_append_cstr(sb, "<missing>");
            continue;
        }
        const HirCallArg* arg = &hir->call_args[arg_index];
        if (named && arg->symbol_handle != U32_MAX) {
            sb_append_string(sb, lex_symbol(lexer, arg->symbol_handle));
            sb_append_cstr(sb, ": ");
        }
        hir_render_expr(sb, hir, lexer, sema, arena, arg->expr_index);
    }
    if (zero_missing) {
        if (expr->arg_count > 0) {
            sb_append_cstr(sb, ", ");
        }
        sb_append_cstr(sb, "...");
    }
}

internal void hir_render_call_callee(StringBuilder* sb,
                                     const Hir*     hir,
                                     const Lexer*   lexer,
                                     const Sema*    sema,
                                     Arena*         arena,
                                     u32            expr_index)
{
    if (expr_index == U32_MAX || expr_index >= array_count(hir->exprs)) {
        sb_append_cstr(sb, "<none>");
        return;
    }

    const HirExpr* expr = &hir->exprs[expr_index];
    if (expr->kind == HIR_EXPR_LocalRef && expr->symbol_handle != U32_MAX) {
        sb_append_string(sb, lex_symbol(lexer, expr->symbol_handle));
        return;
    }

    hir_render_expr(sb, hir, lexer, sema, arena, expr_index);
}

internal cstr hir_pattern_name(HirPatternKind kind)
{
    switch (kind) {
    case HIR_PATTERN_Value:
        return "value";
    case HIR_PATTERN_Equal:
        return "equal";
    case HIR_PATTERN_NotEqual:
        return "not_equal";
    case HIR_PATTERN_Less:
        return "less";
    case HIR_PATTERN_LessEqual:
        return "less_equal";
    case HIR_PATTERN_Greater:
        return "greater";
    case HIR_PATTERN_GreaterEqual:
        return "greater_equal";
    case HIR_PATTERN_RangeExclusive:
        return "range_exclusive";
    case HIR_PATTERN_RangeInclusive:
        return "range_inclusive";
    default:
        return "pattern";
    }
}

internal void hir_render_pattern(StringBuilder* sb,
                                 const Hir*     hir,
                                 const Lexer*   lexer,
                                 const Sema*    sema,
                                 Arena*         arena,
                                 u32            pattern_index)
{
    if (pattern_index == U32_MAX ||
        pattern_index >= array_count(hir->patterns)) {
        sb_append_cstr(sb, "<pattern>");
        return;
    }

    const HirPattern* pattern = &hir->patterns[pattern_index];
    switch (pattern->kind) {
    case HIR_PATTERN_Ignore:
        sb_append_cstr(sb, "_");
        break;
    case HIR_PATTERN_Bind:
        sb_append_cstr(sb, "as ");
        sb_append_string(sb, lex_symbol(lexer, pattern->symbol_handle));
        break;
    case HIR_PATTERN_Value:
    case HIR_PATTERN_Equal:
    case HIR_PATTERN_NotEqual:
    case HIR_PATTERN_Less:
    case HIR_PATTERN_LessEqual:
    case HIR_PATTERN_Greater:
    case HIR_PATTERN_GreaterEqual:
        sb_append_cstr(sb, hir_pattern_name(pattern->kind));
        sb_append_char(sb, '(');
        hir_render_expr(sb, hir, lexer, sema, arena, pattern->expr_index);
        sb_append_char(sb, ')');
        break;
    case HIR_PATTERN_RangeExclusive:
    case HIR_PATTERN_RangeInclusive:
        sb_append_cstr(sb, hir_pattern_name(pattern->kind));
        sb_append_char(sb, '(');
        hir_render_expr(sb, hir, lexer, sema, arena, pattern->expr_index);
        sb_append_cstr(sb, ", ");
        hir_render_expr(sb, hir, lexer, sema, arena, pattern->extra_expr_index);
        sb_append_char(sb, ')');
        break;
    case HIR_PATTERN_Tuple:
    case HIR_PATTERN_Plex:
        sb_append_cstr(sb,
                       pattern->kind == HIR_PATTERN_Tuple ? "tuple(" : "plex(");
        for (u32 i = 0; i < pattern->child_count; ++i) {
            if (i > 0) {
                sb_append_cstr(sb, ", ");
            }
            u32 child_index = pattern->first_child + i;
            if (child_index >= array_count(hir->pattern_children)) {
                sb_append_cstr(sb, "<pattern>");
                continue;
            }
            const HirPatternChild* child = &hir->pattern_children[child_index];
            if (pattern->kind == HIR_PATTERN_Plex &&
                child->symbol_handle != U32_MAX) {
                sb_append_string(sb, lex_symbol(lexer, child->symbol_handle));
                sb_append_cstr(sb, ": ");
            }
            hir_render_pattern(
                sb, hir, lexer, sema, arena, child->pattern_index);
        }
        sb_append_char(sb, ')');
        break;
    case HIR_PATTERN_EnumVariant:
        sb_append_cstr(sb, "enum_variant(");
        if (pattern->expr_index != U32_MAX) {
            hir_render_expr(sb, hir, lexer, sema, arena, pattern->expr_index);
            sb_append_char(sb, '.');
        }
        if (pattern->symbol_handle != U32_MAX) {
            sb_append_string(sb, lex_symbol(lexer, pattern->symbol_handle));
        } else {
            sb_append_cstr(sb, "<variant>");
        }
        for (u32 i = 0; i < pattern->child_count; ++i) {
            sb_append_cstr(sb, ", ");
            u32 child_index = pattern->first_child + i;
            if (child_index >= array_count(hir->pattern_children)) {
                sb_append_cstr(sb, "<pattern>");
                continue;
            }
            hir_render_pattern(
                sb,
                hir,
                lexer,
                sema,
                arena,
                hir->pattern_children[child_index].pattern_index);
        }
        sb_append_char(sb, ')');
        break;
    default:
        sb_append_cstr(sb, "<pattern>");
        break;
    }
}

internal void hir_render_expr(StringBuilder* sb,
                              const Hir*     hir,
                              const Lexer*   lexer,
                              const Sema*    sema,
                              Arena*         arena,
                              u32            expr_index)
{
    if (expr_index == U32_MAX || expr_index >= array_count(hir->exprs)) {
        sb_append_cstr(sb, "<none>");
        return;
    }

    const HirExpr* expr = &hir->exprs[expr_index];
    hir_append_type_name(sb, lexer, sema, expr->type_index);
    sb_append_char(sb, ' ');
    switch (expr->kind) {
    case HIR_EXPR_IntegerLiteral:
        sb_format(sb, "%lld", (long long)expr->integer);
        break;
    case HIR_EXPR_FloatLiteral:
        sb_format(sb, "%.17g", expr->floating);
        break;
    case HIR_EXPR_StringLiteral:
        if (expr->string_is_cstring) {
            sb_append_char(sb, 'c');
        }
        sb_append_char(sb, '"');
        if (expr->string_index < array_count(lexer->strings)) {
            sb_append_string(sb, lexer->strings[expr->string_index]);
        } else {
            sb_append_cstr(sb, "<missing>");
        }
        sb_append_char(sb, '"');
        break;
    case HIR_EXPR_BoolLiteral:
        sb_append_cstr(sb, expr->boolean ? "yes" : "no");
        break;
    case HIR_EXPR_NilLiteral:
        sb_append_cstr(sb, "nil");
        break;
    case HIR_EXPR_LocalRef:
        if (expr->symbol_handle != U32_MAX) {
            sb_append_string(sb, lex_symbol(lexer, expr->symbol_handle));
        } else {
            sb_append_cstr(sb, "<local>");
        }
        break;
    case HIR_EXPR_Unary:
        sb_append_cstr(sb, hir_unary_op_name(expr->unary_op));
        sb_append_char(sb, '(');
        hir_render_expr(sb, hir, lexer, sema, arena, expr->operand_expr_index);
        sb_append_char(sb, ')');
        break;
    case HIR_EXPR_Binary:
        sb_append_cstr(sb, hir_binary_op_name(expr->binary_op));
        sb_append_char(sb, '(');
        hir_render_expr(sb, hir, lexer, sema, arena, expr->lhs_expr_index);
        sb_append_cstr(sb, ", ");
        hir_render_expr(sb, hir, lexer, sema, arena, expr->rhs_expr_index);
        sb_append_char(sb, ')');
        break;
    case HIR_EXPR_Call:
        sb_append_cstr(sb, "call ");
        hir_render_call_callee(
            sb, hir, lexer, sema, arena, expr->callee_expr_index);
        sb_append_char(sb, '(');
        hir_render_expr_arg_list(
            sb, hir, lexer, sema, arena, expr, false, false);
        sb_append_char(sb, ')');
        break;
    case HIR_EXPR_Cast:
        sb_append_cstr(sb, "cast(");
        hir_render_expr(sb, hir, lexer, sema, arena, expr->operand_expr_index);
        sb_append_cstr(sb, " as ");
        hir_append_type_name(sb, lexer, sema, expr->type_index);
        if (expr->extra_expr_index != U32_MAX) {
            sb_append_cstr(sb, ", ");
            hir_render_expr(
                sb, hir, lexer, sema, arena, expr->extra_expr_index);
        }
        sb_append_char(sb, ')');
        break;
    case HIR_EXPR_Index:
        sb_append_cstr(sb, "index(");
        hir_render_expr(sb, hir, lexer, sema, arena, expr->operand_expr_index);
        sb_append_cstr(sb, ", ");
        hir_render_expr(sb, hir, lexer, sema, arena, expr->extra_expr_index);
        sb_append_char(sb, ')');
        break;
    case HIR_EXPR_Tuple:
    case HIR_EXPR_Array:
        sb_append_cstr(sb, expr->kind == HIR_EXPR_Tuple ? "tuple(" : "array(");
        hir_render_expr_arg_list(
            sb, hir, lexer, sema, arena, expr, false, false);
        sb_append_char(sb, ')');
        break;
    case HIR_EXPR_TupleField:
        sb_append_cstr(sb, "tuple_field(");
        hir_render_expr(sb, hir, lexer, sema, arena, expr->operand_expr_index);
        sb_format(sb, ", %lld)", (long long)expr->integer);
        break;
    case HIR_EXPR_Field:
        sb_append_cstr(sb, "field(");
        hir_render_expr(sb, hir, lexer, sema, arena, expr->operand_expr_index);
        sb_append_cstr(sb, ", ");
        if (expr->symbol_handle != U32_MAX) {
            sb_append_string(sb, lex_symbol(lexer, expr->symbol_handle));
        } else {
            sb_append_cstr(sb, "<field>");
        }
        sb_append_char(sb, ')');
        break;
    case HIR_EXPR_Plex:
        sb_append_cstr(sb, "plex(");
        hir_render_expr_arg_list(
            sb, hir, lexer, sema, arena, expr, true, expr->zero_missing);
        sb_append_char(sb, ')');
        break;
    case HIR_EXPR_PlexUpdate:
        sb_append_cstr(sb, "plex_update(");
        hir_render_expr(sb, hir, lexer, sema, arena, expr->operand_expr_index);
        if (expr->arg_count > 0 || expr->zero_missing) {
            sb_append_cstr(sb, ", ");
        }
        hir_render_expr_arg_list(
            sb, hir, lexer, sema, arena, expr, true, expr->zero_missing);
        sb_append_char(sb, ')');
        break;
    case HIR_EXPR_Slice:
        sb_append_cstr(sb, "slice(");
        hir_render_expr(sb, hir, lexer, sema, arena, expr->operand_expr_index);
        sb_append_cstr(sb, ", ");
        hir_render_expr(sb, hir, lexer, sema, arena, expr->lhs_expr_index);
        sb_append_cstr(sb, ", ");
        hir_render_expr(sb, hir, lexer, sema, arena, expr->rhs_expr_index);
        sb_append_char(sb, ')');
        break;
    case HIR_EXPR_RangeExclusive:
    case HIR_EXPR_RangeInclusive:
        sb_append_cstr(sb,
                       expr->kind == HIR_EXPR_RangeExclusive
                           ? "range_exclusive("
                           : "range_inclusive(");
        hir_render_expr(sb, hir, lexer, sema, arena, expr->lhs_expr_index);
        sb_append_cstr(sb, ", ");
        hir_render_expr(sb, hir, lexer, sema, arena, expr->rhs_expr_index);
        sb_append_char(sb, ')');
        break;
    case HIR_EXPR_Block:
        sb_append_cstr(sb, "block");
        if (expr->symbol_handle != U32_MAX) {
            sb_append_cstr(sb, " $");
            sb_append_string(sb, lex_symbol(lexer, expr->symbol_handle));
        }
        sb_append_cstr(sb, " {\n");
        hir_render_block_at_indent(
            sb, hir, lexer, sema, arena, expr->body_block_index, 2);
        sb_append_cstr(sb, "  }");
        break;
    case HIR_EXPR_On:
        sb_append_cstr(sb, "on ");
        if (expr->on_kind == HIR_ON_Condition) {
            sb_append_cstr(sb, "condition");
        } else {
            hir_render_expr(
                sb, hir, lexer, sema, arena, expr->operand_expr_index);
        }
        sb_append_cstr(sb, " {\n");
        for (u32 i = 0; i < expr->branch_count; ++i) {
            u32 branch_index = expr->first_branch + i;
            if (branch_index >= array_count(hir->on_branches)) {
                continue;
            }
            const HirOnBranch* branch = &hir->on_branches[branch_index];
            sb_append_cstr(sb, "    ");
            if (branch->is_else) {
                sb_append_cstr(sb, "else");
            } else if (expr->on_kind == HIR_ON_Condition) {
                hir_render_expr(
                    sb, hir, lexer, sema, arena, branch->guard_expr_index);
            } else {
                for (u32 pattern = 0; pattern < branch->pattern_count;
                     ++pattern) {
                    if (pattern > 0) {
                        sb_append_cstr(sb, ", ");
                    }
                    u32 pattern_index_index = branch->first_pattern + pattern;
                    if (pattern_index_index >=
                        array_count(hir->on_branch_patterns)) {
                        sb_append_cstr(sb, "<pattern>");
                        continue;
                    }
                    hir_render_pattern(
                        sb,
                        hir,
                        lexer,
                        sema,
                        arena,
                        hir->on_branch_patterns[pattern_index_index]);
                }
            }
            if (branch->binder_symbol_handle != U32_MAX) {
                sb_append_cstr(sb, " as ");
                sb_append_string(
                    sb, lex_symbol(lexer, branch->binder_symbol_handle));
            }
            if (branch->guard_expr_index != U32_MAX &&
                expr->on_kind != HIR_ON_Condition) {
                sb_append_cstr(sb, " when ");
                hir_render_expr(
                    sb, hir, lexer, sema, arena, branch->guard_expr_index);
            }
            sb_append_cstr(sb, " => {\n");
            hir_render_block_at_indent(
                sb, hir, lexer, sema, arena, branch->body_block_index, 3);
            sb_append_cstr(sb, "    }\n");
        }
        sb_append_cstr(sb, "  }");
        break;
    case HIR_EXPR_Unsupported:
    default:
        sb_append_cstr(sb, "<unsupported>");
        break;
    }
}

internal void hir_append_indent(StringBuilder* sb, u32 indent)
{
    for (u32 i = 0; i < indent; ++i) {
        sb_append_cstr(sb, "  ");
    }
}

internal void hir_render_stmt_at_indent(StringBuilder* sb,
                                        const Hir*     hir,
                                        const Lexer*   lexer,
                                        const Sema*    sema,
                                        Arena*         arena,
                                        const HirStmt* stmt,
                                        u32            indent)
{
    switch (stmt->kind) {
    case HIR_STMT_Return:
        hir_append_indent(sb, indent);
        sb_append_cstr(sb, "return ");
        hir_render_expr(sb, hir, lexer, sema, arena, stmt->expr_index);
        sb_append_char(sb, '\n');
        break;
    case HIR_STMT_Let:
        hir_append_indent(sb, indent);
        sb_append_cstr(sb, "let ");
        if (stmt->symbol_handle != U32_MAX) {
            sb_append_string(sb, lex_symbol(lexer, stmt->symbol_handle));
        } else {
            sb_append_cstr(sb, "<local>");
        }
        sb_append_cstr(sb, ": ");
        hir_append_type_name(sb, lexer, sema, stmt->type_index);
        sb_append_cstr(sb, " = ");
        hir_render_expr(sb, hir, lexer, sema, arena, stmt->expr_index);
        sb_append_char(sb, '\n');
        break;
    case HIR_STMT_Assign:
        hir_append_indent(sb, indent);
        sb_append_cstr(sb, "assign ");
        hir_render_expr(sb, hir, lexer, sema, arena, stmt->target_expr_index);
        sb_append_cstr(sb, " = ");
        hir_render_expr(sb, hir, lexer, sema, arena, stmt->expr_index);
        sb_append_char(sb, '\n');
        break;
    case HIR_STMT_Assert:
        hir_append_indent(sb, indent);
        sb_append_cstr(sb, "assert ");
        hir_render_expr(sb, hir, lexer, sema, arena, stmt->expr_index);
        if (stmt->target_expr_index != U32_MAX) {
            sb_append_cstr(sb, ", ");
            hir_render_expr(
                sb, hir, lexer, sema, arena, stmt->target_expr_index);
        }
        sb_append_char(sb, '\n');
        break;
    case HIR_STMT_Defer:
        hir_append_indent(sb, indent);
        sb_append_cstr(sb, "defer {\n");
        hir_render_block_at_indent(
            sb, hir, lexer, sema, arena, stmt->body_block_index, indent + 1);
        hir_append_indent(sb, indent);
        sb_append_cstr(sb, "}\n");
        break;
    case HIR_STMT_Break:
        hir_append_indent(sb, indent);
        sb_append_cstr(sb, "break");
        if (stmt->symbol_handle != U32_MAX) {
            sb_append_cstr(sb, " $");
            sb_append_string(sb, lex_symbol(lexer, stmt->symbol_handle));
        }
        if (stmt->expr_index != U32_MAX) {
            sb_append_char(sb, ' ');
            hir_render_expr(sb, hir, lexer, sema, arena, stmt->expr_index);
        }
        sb_append_char(sb, '\n');
        break;
    case HIR_STMT_Continue:
        hir_append_indent(sb, indent);
        sb_append_cstr(sb, "continue");
        if (stmt->symbol_handle != U32_MAX) {
            sb_append_cstr(sb, " $");
            sb_append_string(sb, lex_symbol(lexer, stmt->symbol_handle));
        }
        sb_append_char(sb, '\n');
        break;
    case HIR_STMT_Block:
        hir_append_indent(sb, indent);
        sb_append_cstr(sb, "{\n");
        hir_render_block_at_indent(
            sb, hir, lexer, sema, arena, stmt->body_block_index, indent + 1);
        hir_append_indent(sb, indent);
        sb_append_cstr(sb, "}\n");
        break;
    case HIR_STMT_Expr:
    default:
        hir_append_indent(sb, indent);
        sb_append_cstr(sb, "expr ");
        hir_render_expr(sb, hir, lexer, sema, arena, stmt->expr_index);
        sb_append_char(sb, '\n');
        break;
    }
}

internal void hir_render_block_at_indent(StringBuilder* sb,
                                         const Hir*     hir,
                                         const Lexer*   lexer,
                                         const Sema*    sema,
                                         Arena*         arena,
                                         u32            block_index,
                                         u32            indent)
{
    if (block_index == U32_MAX || block_index >= array_count(hir->blocks)) {
        return;
    }

    const HirBlock* block = &hir->blocks[block_index];
    for (u32 i = 0; i < block->stmt_count; ++i) {
        if (i >= array_count(block->stmt_indices)) {
            break;
        }
        u32 stmt_index = block->stmt_indices[i];
        if (stmt_index >= array_count(hir->stmts)) {
            break;
        }
        hir_render_stmt_at_indent(
            sb, hir, lexer, sema, arena, &hir->stmts[stmt_index], indent);
    }
}

internal void hir_render_block(StringBuilder* sb,
                               const Hir*     hir,
                               const Lexer*   lexer,
                               const Sema*    sema,
                               Arena*         arena,
                               u32            block_index)
{
    hir_render_block_at_indent(sb, hir, lexer, sema, arena, block_index, 1);
}

string
hir_render(const Hir* hir, const Lexer* lexer, const Sema* sema, Arena* arena)
{
    StringBuilder sb = {0};
    sb_init(&sb, arena);

    sb_append_cstr(&sb, "hir 0\n");
    for (u32 i = 0; i < array_count(hir->functions); ++i) {
        const HirFunction* function = &hir->functions[i];
        sb_format(&sb, "%s ", hir_function_prefix(function->kind));
        sb_append_string(&sb, hir_function_name(function, lexer));
        sb_append_char(&sb, '(');
        for (u32 p = 0; p < function->param_count; ++p) {
            if (p > 0) {
                sb_append_cstr(&sb, ", ");
            }
            const HirParam* param = &hir->params[function->first_param + p];
            if (param->symbol_handle != U32_MAX) {
                sb_append_string(&sb, lex_symbol(lexer, param->symbol_handle));
                sb_append_cstr(&sb, ": ");
            } else {
                sb_append_cstr(&sb, "");
            }
            hir_append_type_name(&sb, lexer, sema, param->type_index);
        }
        sb_append_cstr(&sb, ") -> ");
        hir_append_function_return_type_name(&sb, function, lexer, sema);
        if (function->body_block_index == U32_MAX) {
            sb_append_char(&sb, '\n');
        } else {
            sb_append_cstr(&sb, " {\n");
            hir_render_block(
                &sb, hir, lexer, sema, arena, function->body_block_index);
            sb_append_cstr(&sb, "}\n");
        }
    }

    return sb_to_string(&sb);
}

void hir_dump(const Hir* hir, const Lexer* lexer, const Sema* sema)
{
    Arena arena = {0};
    arena_init(&arena);
    epr(STRINGP, STRINGV(hir_render(hir, lexer, sema, &arena)));
    arena_done(&arena);
}

bool hir_save(const Hir* hir, const Lexer* lexer, const Sema* sema, cstr path)
{
    Arena arena = {0};
    arena_init(&arena);
    string rendered = hir_render(hir, lexer, sema, &arena);

    FILE* file      = fopen(path, "wb");
    if (!file) {
        arena_done(&arena);
        return error_runtime("Failed to open file for writing: %s", path);
    }

    usize written      = fwrite(rendered.data, 1, rendered.count, file);
    bool  close_failed = fclose(file) != 0;
    arena_done(&arena);

    if (written != rendered.count || close_failed) {
        return error_runtime("Failed to write HIR file: %s", path);
    }
    return true;
}

//------------------------------------------------------------------------------
