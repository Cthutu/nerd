//------------------------------------------------------------------------------
// LLVM IR emission from HIR
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/error/error.h>
#include <compiler/llvm/llvm.h>
#include <stdio.h>

//------------------------------------------------------------------------------

internal bool llvm_type_is_void(const Sema* sema, u32 type_index)
{
    return sema != NULL && type_index < array_count(sema->types) &&
           sema->types[type_index].kind == STK_Void;
}

internal u32 llvm_function_return_type(const Sema* sema, u32 type_index)
{
    if (sema == NULL || type_index >= array_count(sema->types)) {
        return sema_no_type();
    }

    const SemaType* type = &sema->types[type_index];
    return type->kind == STK_Function ? type->return_type : sema_no_type();
}

internal u32 llvm_function_param_count(const Sema* sema, u32 type_index)
{
    if (sema == NULL || type_index >= array_count(sema->types)) {
        return 0;
    }

    const SemaType* type = &sema->types[type_index];
    return type->kind == STK_Function ? type->param_count : 0;
}

internal u32 llvm_function_param_type(const Sema* sema,
                                      u32         type_index,
                                      u32         param_index)
{
    if (sema == NULL || type_index >= array_count(sema->types)) {
        return sema_no_type();
    }

    const SemaType* type = &sema->types[type_index];
    if (type->kind != STK_Function || param_index >= type->param_count) {
        return sema_no_type();
    }
    return sema->type_param_types[type->first_param_type + param_index];
}

internal bool llvm_type_is_function(const Sema* sema, u32 type_index)
{
    return sema != NULL && type_index < array_count(sema->types) &&
           sema->types[type_index].kind == STK_Function;
}

internal void llvm_append_type(StringBuilder* sb,
                               const Sema*    sema,
                               u32            type_index)
{
    if (sema == NULL || type_index == sema_no_type() ||
        type_index >= array_count(sema->types)) {
        sb_append_cstr(sb, "ptr");
        return;
    }

    const SemaType* type = &sema->types[type_index];
    switch (type->kind) {
    case STK_Void:
        sb_append_cstr(sb, "void");
        break;
    case STK_Bool:
        sb_append_cstr(sb, "i1");
        break;
    case STK_I8:
    case STK_U8:
        sb_append_cstr(sb, "i8");
        break;
    case STK_I16:
    case STK_U16:
        sb_append_cstr(sb, "i16");
        break;
    case STK_I32:
    case STK_U32:
    case STK_UntypedInteger:
        sb_append_cstr(sb, "i32");
        break;
    case STK_I64:
    case STK_U64:
        sb_append_cstr(sb, "i64");
        break;
    case STK_Isize:
    case STK_Usize:
        sb_append_cstr(sb, "i64");
        break;
    case STK_F32:
        sb_append_cstr(sb, "float");
        break;
    case STK_F64:
    case STK_UntypedFloat:
        sb_append_cstr(sb, "double");
        break;
    case STK_Function:
    case STK_Pointer:
    case STK_String:
    case STK_Array:
    case STK_Slice:
    case STK_DynamicArray:
    case STK_Plex:
    case STK_Union:
    case STK_Enum:
    case STK_Tuple:
    case STK_Module:
    default:
        sb_append_cstr(sb, "ptr");
        break;
    }
}

internal void llvm_append_symbol_name(StringBuilder* sb, string name)
{
    sb_append_char(sb, '@');
    sb_append_char(sb, '$');
    for (usize i = 0; i < name.count; ++i) {
        u8 ch = name.data[i];
        bool simple = (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
                      (ch >= '0' && ch <= '9') || ch == '_' || ch == '$' ||
                      ch == '.';
        sb_append_char(sb, simple ? (char)ch : '$');
    }
}

internal void llvm_append_generated_function_name(StringBuilder* sb,
                                                 u32            function_index)
{
    sb_format(sb, "@fn.%u", function_index);
}

internal void llvm_append_function_name(StringBuilder* sb,
                                        const Hir*     hir,
                                        const Lexer*   lexer,
                                        u32            function_index)
{
    llvm_append_generated_function_name(sb, function_index);
    (void)hir;
    (void)lexer;
}

internal string llvm_function_name_string(const Hir*   hir,
                                          const Lexer* lexer,
                                          Arena*       arena,
                                          u32          function_index)
{
    StringBuilder sb = {0};
    sb_init(&sb, arena);
    llvm_append_function_name(&sb, hir, lexer, function_index);
    return sb_to_string(&sb);
}

internal string llvm_symbol_name_string(const Lexer* lexer,
                                        Arena*       arena,
                                        u32          symbol_handle)
{
    StringBuilder sb = {0};
    sb_init(&sb, arena);
    llvm_append_symbol_name(&sb, lex_symbol(lexer, symbol_handle));
    return sb_to_string(&sb);
}

internal void llvm_append_function_signature(StringBuilder*   sb,
                                             const Hir*        hir,
                                             const Lexer*      lexer,
                                             const Sema*       sema,
                                             const HirFunction* function,
                                             u32               function_index)
{
    u32 return_type = llvm_function_return_type(sema, function->type_index);
    llvm_append_type(sb, sema, return_type);
    sb_append_char(sb, ' ');
    llvm_append_function_name(sb, hir, lexer, function_index);
    sb_append_char(sb, '(');
    for (u32 i = 0; i < function->param_count; ++i) {
        if (i > 0) {
            sb_append_cstr(sb, ", ");
        }
        const HirParam* param = &hir->params[function->first_param + i];
        llvm_append_type(sb, sema, param->type_index);
        if (param->symbol_handle != U32_MAX) {
            sb_append_cstr(sb, " %");
            sb_append_string(sb, lex_symbol(lexer, param->symbol_handle));
        }
    }
    sb_append_char(sb, ')');
}

internal void llvm_append_function_type(StringBuilder* sb,
                                        const Sema*    sema,
                                        u32            type_index)
{
    u32 return_type = llvm_function_return_type(sema, type_index);
    llvm_append_type(sb, sema, return_type);
    sb_append_cstr(sb, " (");
    u32 param_count = llvm_function_param_count(sema, type_index);
    for (u32 i = 0; i < param_count; ++i) {
        if (i > 0) {
            sb_append_cstr(sb, ", ");
        }
        llvm_append_type(sb, sema, llvm_function_param_type(sema, type_index, i));
    }
    sb_append_char(sb, ')');
}

internal void llvm_append_default_return(StringBuilder* sb,
                                         const Sema*    sema,
                                         u32            return_type)
{
    if (llvm_type_is_void(sema, return_type)) {
        sb_append_cstr(sb, "  ret void\n");
        return;
    }

    sb_append_cstr(sb, "  ret ");
    llvm_append_type(sb, sema, return_type);
    sb_append_char(sb, ' ');
    if (sema == NULL || return_type >= array_count(sema->types)) {
        sb_append_cstr(sb, "null\n");
        return;
    }

    switch (sema->types[return_type].kind) {
    case STK_F32:
    case STK_F64:
    case STK_UntypedFloat:
        sb_append_cstr(sb, "0.000000e+00\n");
        break;
    case STK_Function:
    case STK_Pointer:
    case STK_String:
    case STK_Array:
    case STK_Slice:
    case STK_DynamicArray:
    case STK_Plex:
    case STK_Union:
    case STK_Enum:
    case STK_Tuple:
    case STK_Module:
        sb_append_cstr(sb, "null\n");
        break;
    default:
        sb_append_cstr(sb, "0\n");
        break;
    }
}

typedef struct {
    bool   ok;
    u32    type_index;
    string value;
} LlvmValue;

typedef struct {
    u32       local_index;
    LlvmValue value;
} LlvmLocalValue;

typedef struct {
    u32    local_index;
    u32    type_index;
    string ptr;
} LlvmLocalSlot;

typedef struct {
    StringBuilder*        sb;
    const Hir*            hir;
    const Lexer*          lexer;
    const Sema*           sema;
    Arena*                arena;
    u32                   next_temp;
    Array(LlvmLocalValue) locals;
    Array(LlvmLocalSlot)  slots;
    Array(u32)            assigned_locals;
} LlvmFunctionContext;

internal string llvm_temp(LlvmFunctionContext* ctx)
{
    return string_format(ctx->arena, "%%t%u", ctx->next_temp++);
}

internal string llvm_type_string(LlvmFunctionContext* ctx, u32 type_index)
{
    StringBuilder sb = {0};
    sb_init(&sb, ctx->arena);
    llvm_append_type(&sb, ctx->sema, type_index);
    return sb_to_string(&sb);
}

internal string llvm_param_value(const HirFunction* function,
                                 const Hir*         hir,
                                 const Lexer*       lexer,
                                 Arena*             arena,
                                 u32                local_index)
{
    for (u32 i = 0; i < function->param_count; ++i) {
        const HirParam* param = &hir->params[function->first_param + i];
        if (param->local_index != local_index) {
            continue;
        }
        return string_format(
            arena, "%%%.*s", (int)lex_symbol(lexer, param->symbol_handle).count,
            lex_symbol(lexer, param->symbol_handle).data);
    }
    return (string){0};
}

internal bool llvm_find_local_value(LlvmFunctionContext* ctx,
                                    u32                  local_index,
                                    LlvmValue*           out)
{
    for (u32 i = 0; i < array_count(ctx->locals); ++i) {
        if (ctx->locals[i].local_index == local_index) {
            *out = ctx->locals[i].value;
            return true;
        }
    }
    return false;
}

internal void llvm_set_local_value(LlvmFunctionContext* ctx,
                                   u32                  local_index,
                                   LlvmValue            value)
{
    for (u32 i = 0; i < array_count(ctx->locals); ++i) {
        if (ctx->locals[i].local_index == local_index) {
            ctx->locals[i].value = value;
            return;
        }
    }

    array_push(ctx->locals,
               (LlvmLocalValue){
                   .local_index = local_index,
                   .value       = value,
               });
}

internal bool llvm_local_is_assigned(LlvmFunctionContext* ctx, u32 local_index)
{
    for (u32 i = 0; i < array_count(ctx->assigned_locals); ++i) {
        if (ctx->assigned_locals[i] == local_index) {
            return true;
        }
    }
    return false;
}

internal void llvm_mark_assigned_local(LlvmFunctionContext* ctx, u32 local_index)
{
    if (local_index == U32_MAX || llvm_local_is_assigned(ctx, local_index)) {
        return;
    }
    array_push(ctx->assigned_locals, local_index);
}

internal LlvmLocalSlot* llvm_find_local_slot(LlvmFunctionContext* ctx,
                                             u32                  local_index)
{
    for (u32 i = 0; i < array_count(ctx->slots); ++i) {
        if (ctx->slots[i].local_index == local_index) {
            return &ctx->slots[i];
        }
    }
    return NULL;
}

internal LlvmLocalSlot* llvm_ensure_local_slot(LlvmFunctionContext* ctx,
                                               u32                  local_index,
                                               u32                  type_index)
{
    LlvmLocalSlot* slot = llvm_find_local_slot(ctx, local_index);
    if (slot != NULL) {
        return slot;
    }

    string ptr = string_format(ctx->arena, "%%local.%u", local_index);
    array_push(ctx->slots,
               (LlvmLocalSlot){
                   .local_index = local_index,
                   .type_index  = type_index,
                   .ptr         = ptr,
               });
    string type = llvm_type_string(ctx, type_index);
    sb_format(ctx->sb,
              "  " STRINGP " = alloca " STRINGP "\n",
              STRINGV(ptr),
              STRINGV(type));
    return &ctx->slots[array_count(ctx->slots) - 1];
}

internal void llvm_store_local_slot(LlvmFunctionContext* ctx,
                                    LlvmLocalSlot*       slot,
                                    LlvmValue            value)
{
    string type = llvm_type_string(ctx, slot->type_index);
    sb_format(ctx->sb,
              "  store " STRINGP " " STRINGP ", ptr " STRINGP "\n",
              STRINGV(type),
              STRINGV(value.value),
              STRINGV(slot->ptr));
}

internal LlvmValue llvm_load_local_slot(LlvmFunctionContext* ctx,
                                        LlvmLocalSlot*       slot)
{
    string type = llvm_type_string(ctx, slot->type_index);
    string temp = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = load " STRINGP ", ptr " STRINGP "\n",
              STRINGV(temp),
              STRINGV(type),
              STRINGV(slot->ptr));
    return (LlvmValue){
        .ok         = true,
        .type_index = slot->type_index,
        .value      = temp,
    };
}

internal string llvm_binary_instruction(HirBinaryOp op)
{
    switch (op) {
    case HIR_BINARY_Add:
        return s("add");
    case HIR_BINARY_Subtract:
        return s("sub");
    case HIR_BINARY_Multiply:
        return s("mul");
    case HIR_BINARY_Divide:
        return s("sdiv");
    case HIR_BINARY_Modulo:
        return s("srem");
    case HIR_BINARY_BitwiseAnd:
        return s("and");
    case HIR_BINARY_BitwiseXor:
        return s("xor");
    case HIR_BINARY_BitwiseOr:
        return s("or");
    case HIR_BINARY_ShiftLeft:
        return s("shl");
    case HIR_BINARY_ShiftRight:
        return s("ashr");
    default:
        return (string){0};
    }
}

internal bool llvm_callee_name(LlvmFunctionContext* ctx,
                               u32                  callee_expr_index,
                               string*              out)
{
    if (callee_expr_index >= array_count(ctx->hir->exprs)) {
        return false;
    }

    const HirExpr* callee = &ctx->hir->exprs[callee_expr_index];
    if (callee->kind == HIR_EXPR_FunctionRef &&
        callee->ref_index < array_count(ctx->hir->functions)) {
        *out = llvm_function_name_string(
            ctx->hir, ctx->lexer, ctx->arena, callee->ref_index);
        return true;
    }

    if (callee->kind != HIR_EXPR_LocalRef ||
        callee->ref_kind != HIR_REF_Binding ||
        callee->ref_index >= array_count(ctx->hir->bindings)) {
        return false;
    }

    const HirBinding* binding = &ctx->hir->bindings[callee->ref_index];
    switch (binding->kind) {
    case HIR_BINDING_Function:
        *out = llvm_function_name_string(
            ctx->hir, ctx->lexer, ctx->arena, binding->target_index);
        return true;
    case HIR_BINDING_Import:
        *out = llvm_symbol_name_string(
            ctx->lexer, ctx->arena, binding->symbol_handle);
        return true;
    default:
        return false;
    }
}

internal LlvmValue llvm_emit_expr(LlvmFunctionContext* ctx,
                                  const HirFunction*   function,
                                  u32                  expr_index)
{
    if (expr_index >= array_count(ctx->hir->exprs)) {
        return (LlvmValue){0};
    }

    const HirExpr* expr = &ctx->hir->exprs[expr_index];
    switch (expr->kind) {
    case HIR_EXPR_IntegerLiteral:
        return (LlvmValue){
            .ok         = true,
            .type_index = expr->type_index,
            .value = string_format(ctx->arena, "%lld", (long long)expr->integer),
        };
    case HIR_EXPR_BoolLiteral:
        return (LlvmValue){
            .ok         = true,
            .type_index = expr->type_index,
            .value      = expr->boolean ? s("1") : s("0"),
        };
    case HIR_EXPR_LocalRef:
        if (expr->ref_kind == HIR_REF_Local) {
            LlvmLocalSlot* slot = llvm_find_local_slot(ctx, expr->ref_index);
            if (slot != NULL) {
                return llvm_load_local_slot(ctx, slot);
            }

            LlvmValue local_value = {0};
            if (llvm_find_local_value(ctx, expr->ref_index, &local_value)) {
                return local_value;
            }

            string value = llvm_param_value(function,
                                            ctx->hir,
                                            ctx->lexer,
                                            ctx->arena,
                                            expr->ref_index);
            return (LlvmValue){
                .ok         = value.count > 0,
                .type_index = expr->type_index,
                .value      = value,
            };
        }
        return (LlvmValue){0};
    case HIR_EXPR_Binary:
        {
            string instr = llvm_binary_instruction(expr->binary_op);
            if (instr.count == 0) {
                return (LlvmValue){0};
            }

            LlvmValue lhs =
                llvm_emit_expr(ctx, function, expr->lhs_expr_index);
            LlvmValue rhs =
                llvm_emit_expr(ctx, function, expr->rhs_expr_index);
            if (!lhs.ok || !rhs.ok) {
                return (LlvmValue){0};
            }

            string type = llvm_type_string(ctx, expr->type_index);
            string temp = llvm_temp(ctx);
            sb_format(ctx->sb,
                      "  " STRINGP " = " STRINGP " " STRINGP " " STRINGP
                      ", " STRINGP "\n",
                      STRINGV(temp),
                      STRINGV(instr),
                      STRINGV(type),
                      STRINGV(lhs.value),
                      STRINGV(rhs.value));
            return (LlvmValue){
                .ok         = true,
                .type_index = expr->type_index,
                .value      = temp,
            };
        }
    case HIR_EXPR_Call:
        {
            string callee = {0};
            if (!llvm_callee_name(ctx, expr->callee_expr_index, &callee)) {
                return (LlvmValue){0};
            }

            string temp        = llvm_temp(ctx);
            string return_type = llvm_type_string(ctx, expr->type_index);
            sb_format(ctx->sb,
                      "  " STRINGP " = call " STRINGP " " STRINGP "(",
                      STRINGV(temp),
                      STRINGV(return_type),
                      STRINGV(callee));
            for (u32 i = 0; i < expr->arg_count; ++i) {
                if (i > 0) {
                    sb_append_cstr(ctx->sb, ", ");
                }
                const HirCallArg* arg =
                    &ctx->hir->call_args[expr->first_arg + i];
                LlvmValue value = llvm_emit_expr(ctx, function, arg->expr_index);
                if (!value.ok) {
                    return (LlvmValue){0};
                }
                string type = llvm_type_string(ctx, value.type_index);
                sb_format(ctx->sb,
                          STRINGP " " STRINGP,
                          STRINGV(type),
                          STRINGV(value.value));
            }
            sb_append_cstr(ctx->sb, ")\n");
            return (LlvmValue){
                .ok         = true,
                .type_index = expr->type_index,
                .value      = temp,
            };
        }
    default:
        return (LlvmValue){0};
    }
}

internal bool llvm_emit_let(LlvmFunctionContext* ctx,
                            const HirFunction*   function,
                            const HirStmt*       stmt)
{
    if (stmt->local_index == U32_MAX || stmt->expr_index == U32_MAX) {
        return false;
    }

    LlvmValue value = llvm_emit_expr(ctx, function, stmt->expr_index);
    if (!value.ok) {
        return false;
    }

    if (llvm_local_is_assigned(ctx, stmt->local_index)) {
        LlvmLocalSlot* slot =
            llvm_ensure_local_slot(ctx, stmt->local_index, stmt->type_index);
        llvm_store_local_slot(ctx, slot, value);
        return true;
    }

    value.type_index = stmt->type_index;
    llvm_set_local_value(ctx, stmt->local_index, value);
    return true;
}

internal bool llvm_emit_assign(LlvmFunctionContext* ctx,
                               const HirFunction*   function,
                               const HirStmt*       stmt)
{
    if (stmt->target_expr_index >= array_count(ctx->hir->exprs)) {
        return false;
    }

    const HirExpr* target = &ctx->hir->exprs[stmt->target_expr_index];
    if (target->kind != HIR_EXPR_LocalRef ||
        target->ref_kind != HIR_REF_Local) {
        return false;
    }

    LlvmValue value = llvm_emit_expr(ctx, function, stmt->expr_index);
    if (!value.ok) {
        return false;
    }

    u32 type_index = target->type_index != sema_no_type() ? target->type_index
                                                          : value.type_index;
    LlvmLocalSlot* slot =
        llvm_ensure_local_slot(ctx, target->ref_index, type_index);
    llvm_store_local_slot(ctx, slot, value);
    return true;
}

internal bool llvm_emit_return(LlvmFunctionContext* ctx,
                               const HirFunction*   function,
                               const HirStmt*       stmt)
{
    u32 return_type = llvm_function_return_type(ctx->sema, function->type_index);
    if (stmt->expr_index == U32_MAX || llvm_type_is_void(ctx->sema, return_type)) {
        sb_append_cstr(ctx->sb, "  ret void\n");
        return true;
    }

    LlvmValue value = llvm_emit_expr(ctx, function, stmt->expr_index);
    if (!value.ok) {
        return false;
    }

    string type = llvm_type_string(ctx, return_type);
    sb_format(ctx->sb,
              "  ret " STRINGP " " STRINGP "\n",
              STRINGV(type),
              STRINGV(value.value));
    return true;
}

internal void llvm_collect_assigned_locals(LlvmFunctionContext* ctx,
                                           u32                  block_index)
{
    if (block_index >= array_count(ctx->hir->blocks)) {
        return;
    }

    const HirBlock* block = &ctx->hir->blocks[block_index];
    for (u32 i = 0; i < block->stmt_count; ++i) {
        u32 stmt_index = block->stmt_indices[i];
        if (stmt_index >= array_count(ctx->hir->stmts)) {
            continue;
        }

        const HirStmt* stmt = &ctx->hir->stmts[stmt_index];
        if (stmt->kind != HIR_STMT_Assign ||
            stmt->target_expr_index >= array_count(ctx->hir->exprs)) {
            continue;
        }

        const HirExpr* target = &ctx->hir->exprs[stmt->target_expr_index];
        if (target->kind == HIR_EXPR_LocalRef &&
            target->ref_kind == HIR_REF_Local) {
            llvm_mark_assigned_local(ctx, target->ref_index);
        }
    }
}

internal bool llvm_emit_block(LlvmFunctionContext* ctx,
                              const HirFunction*   function,
                              u32                  block_index)
{
    if (block_index >= array_count(ctx->hir->blocks)) {
        return false;
    }

    const HirBlock* block = &ctx->hir->blocks[block_index];
    for (u32 i = 0; i < block->stmt_count; ++i) {
        u32 stmt_index = block->stmt_indices[i];
        if (stmt_index >= array_count(ctx->hir->stmts)) {
            continue;
        }

        const HirStmt* stmt = &ctx->hir->stmts[stmt_index];
        if (stmt->kind == HIR_STMT_Let) {
            if (!llvm_emit_let(ctx, function, stmt)) {
                return false;
            }
            continue;
        } else if (stmt->kind == HIR_STMT_Assign) {
            if (!llvm_emit_assign(ctx, function, stmt)) {
                return false;
            }
            continue;
        } else if (stmt->kind == HIR_STMT_Return) {
            return llvm_emit_return(ctx, function, stmt);
        }
    }

    return false;
}

internal void llvm_render_import(StringBuilder* sb,
                                 const Lexer*   lexer,
                                 const Sema*    sema,
                                 const HirImport* import)
{
    if (sema == NULL || import->type_index >= array_count(sema->types) ||
        sema->types[import->type_index].kind != STK_Function) {
        return;
    }

    sb_append_cstr(sb, "declare ");
    u32 return_type = llvm_function_return_type(sema, import->type_index);
    llvm_append_type(sb, sema, return_type);
    sb_append_char(sb, ' ');
    llvm_append_symbol_name(sb, lex_symbol(lexer, import->symbol_handle));
    sb_append_char(sb, '(');
    u32 param_count = llvm_function_param_count(sema, import->type_index);
    for (u32 i = 0; i < param_count; ++i) {
        if (i > 0) {
            sb_append_cstr(sb, ", ");
        }
        llvm_append_type(
            sb, sema, llvm_function_param_type(sema, import->type_index, i));
    }
    sb_append_cstr(sb, ")\n");
}

internal void llvm_render_function(StringBuilder*    sb,
                                   const Hir*         hir,
                                   const Lexer*       lexer,
                                   const Sema*        sema,
                                   Arena*             arena,
                                   const HirFunction* function,
                                   u32                function_index)
{
    if (function->kind == HIR_FUNCTION_Ffi ||
        function->body_block_index == U32_MAX) {
        sb_append_cstr(sb, "declare ");
        llvm_append_function_signature(
            sb, hir, lexer, sema, function, function_index);
        sb_append_char(sb, '\n');
        return;
    }

    sb_append_cstr(sb, "define ");
    llvm_append_function_signature(
        sb, hir, lexer, sema, function, function_index);
    sb_append_cstr(sb, " {\n");
    Arena temp = {0};
    arena_init(&temp);
    LlvmFunctionContext ctx = {
        .sb        = sb,
        .hir       = hir,
        .lexer     = lexer,
        .sema      = sema,
        .arena     = &temp,
        .next_temp = 0,
    };
    llvm_collect_assigned_locals(&ctx, function->body_block_index);
    if (!llvm_emit_block(&ctx, function, function->body_block_index)) {
        u32 return_type = llvm_function_return_type(sema, function->type_index);
        llvm_append_default_return(sb, sema, return_type);
    }
    array_free(ctx.locals);
    array_free(ctx.slots);
    array_free(ctx.assigned_locals);
    arena_done(&temp);
    sb_append_cstr(sb, "}\n");
    (void)arena;
}

internal void llvm_render_binding_alias(StringBuilder* sb,
                                        const Hir*     hir,
                                        const Lexer*   lexer,
                                        const Sema*    sema,
                                        const HirBinding* binding)
{
    if (binding->kind != HIR_BINDING_Function ||
        binding->target_index >= array_count(hir->functions)) {
        return;
    }

    const HirFunction* function = &hir->functions[binding->target_index];
    if (!llvm_type_is_function(sema, function->type_index)) {
        return;
    }

    llvm_append_symbol_name(sb, lex_symbol(lexer, binding->symbol_handle));
    sb_append_cstr(sb, " = alias ");
    llvm_append_function_type(sb, sema, function->type_index);
    sb_append_cstr(sb, ", ptr ");
    llvm_append_generated_function_name(sb, binding->target_index);
    sb_append_char(sb, '\n');
}

string llvm_render_hir(const Hir* hir,
                       const Lexer* lexer,
                       const Sema* sema,
                       Arena* arena)
{
    StringBuilder sb = {0};
    sb_init(&sb, arena);

    sb_append_cstr(&sb, "; nerd llvm-ir 0\n");
    sb_append_cstr(&sb, "; generated from HIR\n\n");

    for (u32 i = 0; i < array_count(hir->imports); ++i) {
        llvm_render_import(&sb, lexer, sema, &hir->imports[i]);
    }
    if (array_count(hir->imports) > 0) {
        sb_append_char(&sb, '\n');
    }

    for (u32 i = 0; i < array_count(hir->functions); ++i) {
        llvm_render_function(
            &sb, hir, lexer, sema, arena, &hir->functions[i], i);
        sb_append_char(&sb, '\n');
    }

    for (u32 i = 0; i < array_count(hir->bindings); ++i) {
        llvm_render_binding_alias(&sb, hir, lexer, sema, &hir->bindings[i]);
    }
    if (array_count(hir->bindings) > 0) {
        sb_append_char(&sb, '\n');
    }

    for (u32 i = 0; i < array_count(hir->exports); ++i) {
        const HirExport* export = &hir->exports[i];
        if (export->binding_index >= array_count(hir->bindings)) {
            continue;
        }
        const HirBinding* binding = &hir->bindings[export->binding_index];
        sb_append_cstr(&sb, "; export ");
        sb_append_string(&sb, lex_symbol(lexer, binding->symbol_handle));
        sb_append_char(&sb, '\n');
    }

    return sb_to_string(&sb);
}

bool llvm_save_hir(const Hir* hir,
                   const Lexer* lexer,
                   const Sema* sema,
                   cstr path)
{
    Arena arena = {0};
    arena_init(&arena);
    string rendered = llvm_render_hir(hir, lexer, sema, &arena);

    FILE* file = fopen(path, "wb");
    if (!file) {
        arena_done(&arena);
        return error_runtime("Failed to open LLVM IR file for writing: %s", path);
    }

    usize written = fwrite(rendered.data, 1, rendered.count, file);
    bool close_failed = fclose(file) != 0;
    arena_done(&arena);

    if (written != rendered.count || close_failed) {
        return error_runtime("Failed to write LLVM IR file: %s", path);
    }
    return true;
}

//------------------------------------------------------------------------------
