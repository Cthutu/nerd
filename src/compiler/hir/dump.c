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

internal string hir_type_name(const Lexer* lexer,
                              const Sema*  sema,
                              Arena*       arena,
                              u32          type_index)
{
    if (!sema || type_index == sema_no_type() ||
        type_index >= array_count(sema->types)) {
        return s("<unknown>");
    }
    return sema_type_name(lexer, sema, arena, type_index);
}

internal string hir_function_return_type_name(const HirFunction* function,
                                              const Lexer*       lexer,
                                              const Sema*        sema,
                                              Arena*             arena)
{
    if (!sema || function->type_index == sema_no_type() ||
        function->type_index >= array_count(sema->types) ||
        sema->types[function->type_index].kind != STK_Function) {
        return s("<unknown>");
    }
    return hir_type_name(
        lexer, sema, arena, sema->types[function->type_index].return_type);
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

internal void hir_render_expr(StringBuilder* sb,
                              const Hir*     hir,
                              const Lexer*   lexer,
                              const Sema*    sema,
                              Arena*         arena,
                              u32            expr_index);

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
    sb_append_string(sb, hir_type_name(lexer, sema, arena, expr->type_index));
    sb_append_char(sb, ' ');
    switch (expr->kind) {
    case HIR_EXPR_IntegerLiteral:
        sb_format(sb, "%lld", (long long)expr->integer);
        break;
    case HIR_EXPR_BoolLiteral:
        sb_append_cstr(sb, expr->boolean ? "yes" : "no");
        break;
    case HIR_EXPR_LocalRef:
        if (expr->symbol_handle != U32_MAX) {
            sb_append_string(sb, lex_symbol(lexer, expr->symbol_handle));
        } else {
            sb_append_cstr(sb, "<local>");
        }
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
        for (u32 i = 0; i < expr->arg_count; ++i) {
            if (i > 0) {
                sb_append_cstr(sb, ", ");
            }
            u32 arg_index = expr->first_arg + i;
            if (arg_index >= array_count(hir->call_args)) {
                sb_append_cstr(sb, "<missing>");
                continue;
            }
            hir_render_expr(sb,
                            hir,
                            lexer,
                            sema,
                            arena,
                            hir->call_args[arg_index].expr_index);
        }
        sb_append_char(sb, ')');
        break;
    case HIR_EXPR_Unsupported:
    default:
        sb_append_cstr(sb, "<unsupported>");
        break;
    }
}

internal void hir_render_stmt(StringBuilder* sb,
                              const Hir*     hir,
                              const Lexer*   lexer,
                              const Sema*    sema,
                              Arena*         arena,
                              const HirStmt* stmt)
{
    switch (stmt->kind) {
    case HIR_STMT_Return:
        sb_append_cstr(sb, "  return ");
        hir_render_expr(sb, hir, lexer, sema, arena, stmt->expr_index);
        sb_append_char(sb, '\n');
        break;
    case HIR_STMT_Let:
        sb_append_cstr(sb, "  let ");
        if (stmt->symbol_handle != U32_MAX) {
            sb_append_string(sb, lex_symbol(lexer, stmt->symbol_handle));
        } else {
            sb_append_cstr(sb, "<local>");
        }
        sb_append_cstr(sb, ": ");
        sb_append_string(sb,
                         hir_type_name(lexer, sema, arena, stmt->type_index));
        sb_append_cstr(sb, " = ");
        hir_render_expr(sb, hir, lexer, sema, arena, stmt->expr_index);
        sb_append_char(sb, '\n');
        break;
    case HIR_STMT_Expr:
    default:
        sb_append_cstr(sb, "  expr ");
        hir_render_expr(sb, hir, lexer, sema, arena, stmt->expr_index);
        sb_append_char(sb, '\n');
        break;
    }
}

internal void hir_render_block(StringBuilder* sb,
                               const Hir*     hir,
                               const Lexer*   lexer,
                               const Sema*    sema,
                               Arena*         arena,
                               u32            block_index)
{
    if (block_index == U32_MAX || block_index >= array_count(hir->blocks)) {
        return;
    }

    const HirBlock* block = &hir->blocks[block_index];
    for (u32 i = 0; i < block->stmt_count; ++i) {
        u32 stmt_index = block->first_stmt + i;
        if (stmt_index >= array_count(hir->stmts)) {
            break;
        }
        hir_render_stmt(sb, hir, lexer, sema, arena, &hir->stmts[stmt_index]);
    }
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
            sb_append_string(
                &sb, hir_type_name(lexer, sema, arena, param->type_index));
        }
        sb_append_cstr(&sb, ") -> ");
        sb_append_string(
            &sb, hir_function_return_type_name(function, lexer, sema, arena));
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
