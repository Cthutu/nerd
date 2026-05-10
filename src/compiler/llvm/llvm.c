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

internal SemaTypeKind llvm_type_kind(const Sema* sema, u32 type_index)
{
    if (sema == NULL || type_index == sema_no_type() ||
        type_index >= array_count(sema->types)) {
        return STK_Void;
    }
    return sema->types[type_index].kind;
}

internal bool llvm_type_is_unsigned_integer(const Sema* sema, u32 type_index)
{
    switch (llvm_type_kind(sema, type_index)) {
    case STK_U8:
    case STK_U16:
    case STK_U32:
    case STK_U64:
    case STK_Usize:
        return true;
    default:
        return false;
    }
}

internal u32 llvm_integer_bits(const Sema* sema, u32 type_index)
{
    switch (llvm_type_kind(sema, type_index)) {
    case STK_Bool:
        return 1;
    case STK_I8:
    case STK_U8:
        return 8;
    case STK_I16:
    case STK_U16:
        return 16;
    case STK_I32:
    case STK_U32:
    case STK_UntypedInteger:
        return 32;
    case STK_I64:
    case STK_U64:
    case STK_Isize:
    case STK_Usize:
        return 64;
    default:
        return 0;
    }
}

internal u32 llvm_float_bits(const Sema* sema, u32 type_index)
{
    switch (llvm_type_kind(sema, type_index)) {
    case STK_F32:
        return 32;
    case STK_F64:
    case STK_UntypedFloat:
        return 64;
    default:
        return 0;
    }
}

internal u32 llvm_pointee_type(const Sema* sema, u32 type_index)
{
    if (llvm_type_kind(sema, type_index) != STK_Pointer) {
        return sema_no_type();
    }
    return sema->types[type_index].first_param_type;
}

internal u32 llvm_array_count(const Sema* sema, u32 type_index)
{
    if (llvm_type_kind(sema, type_index) != STK_Array) {
        return 0;
    }
    return sema->types[type_index].return_type;
}

internal u32 llvm_collection_item_type(const Sema* sema, u32 type_index)
{
    SemaTypeKind kind = llvm_type_kind(sema, type_index);
    if (kind == STK_Array || kind == STK_Slice || kind == STK_Pointer) {
        return sema->types[type_index].first_param_type;
    }
    return sema_no_type();
}

internal bool llvm_type_is_record(const Sema* sema, u32 type_index)
{
    SemaTypeKind kind = llvm_type_kind(sema, type_index);
    return kind == STK_Tuple || kind == STK_Plex;
}

internal u32 llvm_record_field_count(const Sema* sema, u32 type_index)
{
    if (!llvm_type_is_record(sema, type_index)) {
        return 0;
    }
    return sema->types[type_index].param_count;
}

internal u32 llvm_record_field_type(const Sema* sema,
                                    u32         type_index,
                                    u32         field_index)
{
    if (!llvm_type_is_record(sema, type_index) ||
        field_index >= sema->types[type_index].param_count) {
        return sema_no_type();
    }
    return sema->type_param_types[sema->types[type_index].first_param_type +
                                  field_index];
}

internal u32 llvm_record_field_index(const Sema* sema,
                                     u32         type_index,
                                     u32         symbol_handle)
{
    if (!llvm_type_is_record(sema, type_index)) {
        return U32_MAX;
    }

    const SemaType* type = &sema->types[type_index];
    for (u32 i = 0; i < type->param_count; ++i) {
        if (sema->type_param_symbols[type->first_param_type + i] ==
            symbol_handle) {
            return i;
        }
    }
    return U32_MAX;
}

internal u32 llvm_enum_variant_index(const Sema* sema,
                                     u32         enum_type,
                                     u32         symbol_handle)
{
    if (sema == NULL || enum_type == sema_no_type() ||
        enum_type >= array_count(sema->types) ||
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

internal u32 llvm_enum_variant_payload_type(const Sema* sema,
                                            u32         enum_type,
                                            u32         variant_index)
{
    if (sema == NULL || enum_type == sema_no_type() ||
        enum_type >= array_count(sema->types) ||
        sema->types[enum_type].kind != STK_Enum ||
        variant_index >= sema->types[enum_type].param_count) {
        return sema_no_type();
    }

    return sema->type_param_types[sema->types[enum_type].first_param_type +
                                  variant_index];
}

internal i64 llvm_enum_variant_discriminant(const Sema* sema,
                                            u32         enum_type,
                                            u32         variant_index)
{
    if (sema == NULL || enum_type == sema_no_type() ||
        enum_type >= array_count(sema->types) ||
        sema->types[enum_type].kind != STK_Enum ||
        variant_index >= sema->types[enum_type].param_count) {
        return 0;
    }

    return sema->type_param_values[sema->types[enum_type].first_param_type +
                                   variant_index];
}

internal u32 llvm_enum_payload_width(const Sema* sema, u32 type_index)
{
    if (type_index == sema_no_type() || type_index >= array_count(sema->types)) {
        return 0;
    }

    const SemaType* type = &sema->types[type_index];
    return type->kind == STK_Tuple ? type->param_count : 1;
}

internal u32 llvm_enum_storage_payload_type(const Sema* sema, u32 enum_type)
{
    if (sema == NULL || enum_type == sema_no_type() ||
        enum_type >= array_count(sema->types) ||
        sema->types[enum_type].kind != STK_Enum) {
        return sema_no_type();
    }

    const SemaType* type = &sema->types[enum_type];
    u32             best = sema_no_type();
    u32             best_width = 0;
    for (u32 i = 0; i < type->param_count; ++i) {
        u32 payload_type = llvm_enum_variant_payload_type(sema, enum_type, i);
        u32 width        = llvm_enum_payload_width(sema, payload_type);
        if (payload_type != sema_no_type() && width >= best_width) {
            best       = payload_type;
            best_width = width;
        }
    }
    return best;
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
    case STK_Array:
        sb_format(sb, "[%u x ", type->return_type);
        llvm_append_type(sb, sema, type->first_param_type);
        sb_append_char(sb, ']');
        break;
    case STK_Tuple:
    case STK_Plex:
        sb_append_cstr(sb, "{ ");
        for (u32 i = 0; i < type->param_count; ++i) {
            if (i > 0) {
                sb_append_cstr(sb, ", ");
            }
            llvm_append_type(
                sb, sema, sema->type_param_types[type->first_param_type + i]);
        }
        sb_append_cstr(sb, " }");
        break;
    case STK_Function:
    case STK_Pointer:
    case STK_String:
        sb_append_cstr(sb, "ptr");
        break;
    case STK_Slice:
        sb_append_cstr(sb, "{ ptr, i64 }");
        break;
    case STK_Enum:
        {
            u32 payload_type = llvm_enum_storage_payload_type(sema, type_index);
            sb_append_cstr(sb, "{ i64, ");
            if (payload_type == sema_no_type()) {
                sb_append_cstr(sb, "i8");
            } else {
                llvm_append_type(sb, sema, payload_type);
            }
            sb_append_cstr(sb, " }");
            break;
        }
    case STK_DynamicArray:
    case STK_Union:
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
    case STK_Union:
    case STK_Module:
        sb_append_cstr(sb, "null\n");
        break;
    case STK_Enum:
    case STK_Plex:
    case STK_Tuple:
        sb_append_cstr(sb, "zeroinitializer\n");
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
    u32                   next_label;
    bool                  block_terminated;
    string                break_label;
    string                continue_label;
    Array(LlvmLocalValue) locals;
    Array(LlvmLocalSlot)  slots;
    Array(u32)            assigned_locals;
} LlvmFunctionContext;

internal string llvm_temp(LlvmFunctionContext* ctx)
{
    return string_format(ctx->arena, "%%t%u", ctx->next_temp++);
}

internal string llvm_label(LlvmFunctionContext* ctx, cstr prefix)
{
    return string_format(ctx->arena, "%s.%u", prefix, ctx->next_label++);
}

internal string llvm_type_string(LlvmFunctionContext* ctx, u32 type_index)
{
    StringBuilder sb = {0};
    sb_init(&sb, ctx->arena);
    llvm_append_type(&sb, ctx->sema, type_index);
    return sb_to_string(&sb);
}

internal u32 llvm_local_type(LlvmFunctionContext* ctx, u32 local_index)
{
    return ctx->sema != NULL && local_index < array_count(ctx->sema->locals)
               ? ctx->sema->locals[local_index].type_index
               : sema_no_type();
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

internal LlvmValue llvm_default_value(LlvmFunctionContext* ctx, u32 type_index)
{
    SemaTypeKind kind = llvm_type_kind(ctx->sema, type_index);
    if (kind == STK_Pointer || kind == STK_String || kind == STK_Nil) {
        return (LlvmValue){
            .ok         = true,
            .type_index = type_index,
            .value      = s("null"),
        };
    }
    if (kind == STK_Enum || kind == STK_Tuple || kind == STK_Plex ||
        kind == STK_Array || kind == STK_Slice) {
        return (LlvmValue){
            .ok         = true,
            .type_index = type_index,
            .value      = s("zeroinitializer"),
        };
    }
    if (llvm_float_bits(ctx->sema, type_index) > 0) {
        return (LlvmValue){
            .ok         = true,
            .type_index = type_index,
            .value      = s("0.0"),
        };
    }
    return (LlvmValue){
        .ok         = true,
        .type_index = type_index,
        .value      = s("0"),
    };
}

internal bool llvm_expr_integer_constant(const Hir* hir, u32 expr_index, i64* out)
{
    if (expr_index >= array_count(hir->exprs)) {
        return false;
    }
    const HirExpr* expr = &hir->exprs[expr_index];
    if (expr->kind != HIR_EXPR_IntegerLiteral) {
        return false;
    }
    *out = expr->integer;
    return true;
}

internal LlvmValue llvm_emit_expr(LlvmFunctionContext* ctx,
                                  const HirFunction*   function,
                                  u32                  expr_index);

internal bool llvm_emit_block(LlvmFunctionContext* ctx,
                              const HirFunction*   function,
                              u32                  block_index);

internal bool llvm_callee_symbol_handle(LlvmFunctionContext* ctx,
                                        u32                  callee_expr_index,
                                        u32*                 out_symbol)
{
    if (callee_expr_index >= array_count(ctx->hir->exprs)) {
        return false;
    }

    const HirExpr* callee = &ctx->hir->exprs[callee_expr_index];
    if ((callee->kind == HIR_EXPR_LocalRef || callee->kind == HIR_EXPR_Field) &&
        callee->symbol_handle != U32_MAX) {
        *out_symbol = callee->symbol_handle;
        return true;
    }
    return false;
}

internal LlvmValue llvm_emit_enum_constructor(LlvmFunctionContext* ctx,
                                              const HirFunction*   function,
                                              const HirExpr*       expr,
                                              u32 variant_index)
{
    Array(LlvmValue) args = NULL;
    for (u32 i = 0; i < expr->arg_count; ++i) {
        const HirCallArg* arg = &ctx->hir->call_args[expr->first_arg + i];
        LlvmValue value = llvm_emit_expr(ctx, function, arg->expr_index);
        if (!value.ok) {
            array_free(args);
            return (LlvmValue){0};
        }
        array_push(args, value);
    }

    u32 enum_type            = expr->type_index;
    u32 storage_payload_type = llvm_enum_storage_payload_type(ctx->sema,
                                                              enum_type);
    string enum_type_string  = llvm_type_string(ctx, enum_type);
    string payload_type_string = storage_payload_type == sema_no_type()
                                     ? s("i8")
                                     : llvm_type_string(ctx,
                                                        storage_payload_type);
    string payload_value = storage_payload_type == sema_no_type()
                               ? s("0")
                               : s("zeroinitializer");

    if (storage_payload_type != sema_no_type() && expr->arg_count > 0) {
        if (llvm_type_is_record(ctx->sema, storage_payload_type)) {
            payload_value = s("poison");
            for (u32 i = 0; i < expr->arg_count; ++i) {
                LlvmValue arg  = args[i];
                string    temp = llvm_temp(ctx);
                string    arg_type = llvm_type_string(ctx, arg.type_index);
                sb_format(ctx->sb,
                          "  " STRINGP " = insertvalue " STRINGP " "
                          STRINGP ", " STRINGP " " STRINGP ", %u\n",
                          STRINGV(temp),
                          STRINGV(payload_type_string),
                          STRINGV(payload_value),
                          STRINGV(arg_type),
                          STRINGV(arg.value),
                          i);
                payload_value = temp;
            }
        } else {
            payload_value = args[0].value;
        }
    }

    i64 tag = llvm_enum_variant_discriminant(ctx->sema,
                                             enum_type,
                                             variant_index);
    string with_tag = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = insertvalue " STRINGP " poison, i64 %lld, 0\n",
              STRINGV(with_tag),
              STRINGV(enum_type_string),
              (long long)tag);

    string with_payload = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = insertvalue " STRINGP " " STRINGP ", "
              STRINGP " " STRINGP ", 1\n",
              STRINGV(with_payload),
              STRINGV(enum_type_string),
              STRINGV(with_tag),
              STRINGV(payload_type_string),
              STRINGV(payload_value));

    array_free(args);
    return (LlvmValue){
        .ok         = true,
        .type_index = enum_type,
        .value      = with_payload,
    };
}

internal LlvmValue llvm_emit_block_value(LlvmFunctionContext* ctx,
                                         const HirFunction*   function,
                                         u32                  block_index)
{
    if (block_index >= array_count(ctx->hir->blocks)) {
        return (LlvmValue){0};
    }

    const HirBlock* block = &ctx->hir->blocks[block_index];
    for (u32 i = 0; i < block->stmt_count; ++i) {
        u32 stmt_index = block->stmt_indices[i];
        if (stmt_index >= array_count(ctx->hir->stmts)) {
            return (LlvmValue){0};
        }

        const HirStmt* stmt = &ctx->hir->stmts[stmt_index];
        if (stmt->kind == HIR_STMT_Expr || stmt->kind == HIR_STMT_Break) {
            return llvm_emit_expr(ctx, function, stmt->expr_index);
        }
    }

    return (LlvmValue){0};
}

internal u32 llvm_find_local_by_symbol(LlvmFunctionContext* ctx,
                                       u32                  symbol_handle)
{
    if (ctx->sema == NULL || symbol_handle == U32_MAX) {
        return U32_MAX;
    }

    for (u32 i = 0; i < array_count(ctx->sema->locals); ++i) {
        if (ctx->sema->locals[i].symbol_handle == symbol_handle) {
            return i;
        }
    }
    return U32_MAX;
}

internal void llvm_bind_symbol_value(LlvmFunctionContext* ctx,
                                     u32                  symbol_handle,
                                     LlvmValue            value)
{
    u32 local_index = llvm_find_local_by_symbol(ctx, symbol_handle);
    if (local_index != U32_MAX) {
        value.type_index = llvm_local_type(ctx, local_index);
        llvm_set_local_value(ctx, local_index, value);
    }
}

internal LlvmValue llvm_emit_pattern_compare(LlvmFunctionContext* ctx,
                                             const HirFunction*   function,
                                             LlvmValue            scrutinee,
                                             u32                  expr_index,
                                             string               pred)
{
    LlvmValue rhs = llvm_emit_expr(ctx, function, expr_index);
    if (!rhs.ok) {
        return (LlvmValue){0};
    }

    string type = llvm_type_string(ctx, scrutinee.type_index);
    string temp = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = icmp " STRINGP " " STRINGP " " STRINGP
              ", " STRINGP "\n",
              STRINGV(temp),
              STRINGV(pred),
              STRINGV(type),
              STRINGV(scrutinee.value),
              STRINGV(rhs.value));
    return (LlvmValue){
        .ok         = true,
        .type_index = rhs.type_index,
        .value      = temp,
    };
}

internal LlvmValue llvm_emit_pattern_condition(LlvmFunctionContext* ctx,
                                               const HirFunction*   function,
                                               LlvmValue            scrutinee,
                                               u32                  pattern_index)
{
    if (pattern_index >= array_count(ctx->hir->patterns)) {
        return (LlvmValue){0};
    }

    const HirPattern* pattern = &ctx->hir->patterns[pattern_index];
    switch (pattern->kind) {
    case HIR_PATTERN_Ignore:
        return (LlvmValue){
            .ok         = true,
            .type_index = sema_no_type(),
            .value      = s("1"),
        };
    case HIR_PATTERN_Bind:
        llvm_bind_symbol_value(ctx, pattern->symbol_handle, scrutinee);
        return (LlvmValue){
            .ok         = true,
            .type_index = sema_no_type(),
            .value      = s("1"),
        };
    case HIR_PATTERN_Value:
    case HIR_PATTERN_Equal:
        return llvm_emit_pattern_compare(
            ctx, function, scrutinee, pattern->expr_index, s("eq"));
    case HIR_PATTERN_NotEqual:
        return llvm_emit_pattern_compare(
            ctx, function, scrutinee, pattern->expr_index, s("ne"));
    case HIR_PATTERN_Less:
        return llvm_emit_pattern_compare(
            ctx, function, scrutinee, pattern->expr_index, s("slt"));
    case HIR_PATTERN_LessEqual:
        return llvm_emit_pattern_compare(
            ctx, function, scrutinee, pattern->expr_index, s("sle"));
    case HIR_PATTERN_Greater:
        return llvm_emit_pattern_compare(
            ctx, function, scrutinee, pattern->expr_index, s("sgt"));
    case HIR_PATTERN_GreaterEqual:
        return llvm_emit_pattern_compare(
            ctx, function, scrutinee, pattern->expr_index, s("sge"));
    case HIR_PATTERN_RangeExclusive:
    case HIR_PATTERN_RangeInclusive:
        {
            LlvmValue lower = llvm_emit_pattern_compare(
                ctx, function, scrutinee, pattern->expr_index, s("sge"));
            LlvmValue upper = llvm_emit_pattern_compare(
                ctx,
                function,
                scrutinee,
                pattern->extra_expr_index,
                pattern->kind == HIR_PATTERN_RangeInclusive ? s("sle")
                                                            : s("slt"));
            if (!lower.ok || !upper.ok) {
                return (LlvmValue){0};
            }
            string temp = llvm_temp(ctx);
            sb_format(ctx->sb,
                      "  " STRINGP " = and i1 " STRINGP ", " STRINGP "\n",
                      STRINGV(temp),
                      STRINGV(lower.value),
                      STRINGV(upper.value));
            return (LlvmValue){
                .ok         = true,
                .type_index = lower.type_index,
                .value      = temp,
            };
        }
    case HIR_PATTERN_Plex:
    case HIR_PATTERN_Tuple:
        {
            LlvmValue result = {0};
            for (u32 i = 0; i < pattern->child_count; ++i) {
                u32 child_index = pattern->first_child + i;
                if (child_index >= array_count(ctx->hir->pattern_children)) {
                    return (LlvmValue){0};
                }

                const HirPatternChild* child =
                    &ctx->hir->pattern_children[child_index];
                u32 field_index =
                    pattern->kind == HIR_PATTERN_Plex
                        ? llvm_record_field_index(ctx->sema,
                                                  scrutinee.type_index,
                                                  child->symbol_handle)
                        : i;
                if (field_index == U32_MAX) {
                    return (LlvmValue){0};
                }

                string temp        = llvm_temp(ctx);
                string record_type = llvm_type_string(ctx, scrutinee.type_index);
                sb_format(ctx->sb,
                          "  " STRINGP " = extractvalue " STRINGP " "
                          STRINGP ", %u\n",
                          STRINGV(temp),
                          STRINGV(record_type),
                          STRINGV(scrutinee.value),
                          field_index);

                LlvmValue field = {
                    .ok         = true,
                    .type_index = llvm_record_field_type(
                        ctx->sema, scrutinee.type_index, field_index),
                    .value = temp,
                };
                LlvmValue condition = llvm_emit_pattern_condition(
                    ctx, function, field, child->pattern_index);
                if (!condition.ok) {
                    return (LlvmValue){0};
                }

                if (!result.ok) {
                    result = condition;
                    continue;
                }

                string and_temp = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = and i1 " STRINGP ", " STRINGP
                          "\n",
                          STRINGV(and_temp),
                          STRINGV(result.value),
                          STRINGV(condition.value));
                result.value = and_temp;
            }
            return result.ok ? result
                             : (LlvmValue){
                                   .ok         = true,
                                   .type_index = sema_no_type(),
                                   .value      = s("1"),
                               };
        }
    case HIR_PATTERN_EnumVariant:
        {
            u32 variant_index = llvm_enum_variant_index(
                ctx->sema, scrutinee.type_index, pattern->symbol_handle);
            if (variant_index == U32_MAX) {
                return (LlvmValue){0};
            }

            string enum_type = llvm_type_string(ctx, scrutinee.type_index);
            string tag       = llvm_temp(ctx);
            sb_format(ctx->sb,
                      "  " STRINGP " = extractvalue " STRINGP " " STRINGP
                      ", 0\n",
                      STRINGV(tag),
                      STRINGV(enum_type),
                      STRINGV(scrutinee.value));

            string tag_matches = llvm_temp(ctx);
            i64    discriminant = llvm_enum_variant_discriminant(
                ctx->sema, scrutinee.type_index, variant_index);
            sb_format(ctx->sb,
                      "  " STRINGP " = icmp eq i64 " STRINGP ", %lld\n",
                      STRINGV(tag_matches),
                      STRINGV(tag),
                      (long long)discriminant);

            LlvmValue result = {
                .ok         = true,
                .type_index = sema_no_type(),
                .value      = tag_matches,
            };
            if (pattern->child_count == 0) {
                return result;
            }

            u32 storage_payload_type =
                llvm_enum_storage_payload_type(ctx->sema, scrutinee.type_index);
            u32 variant_payload_type =
                llvm_enum_variant_payload_type(ctx->sema,
                                               scrutinee.type_index,
                                               variant_index);
            if (storage_payload_type == sema_no_type() ||
                variant_payload_type == sema_no_type()) {
                return result;
            }

            string payload = llvm_temp(ctx);
            sb_format(ctx->sb,
                      "  " STRINGP " = extractvalue " STRINGP " " STRINGP
                      ", 1\n",
                      STRINGV(payload),
                      STRINGV(enum_type),
                      STRINGV(scrutinee.value));

            bool payload_is_tuple =
                llvm_type_kind(ctx->sema, variant_payload_type) == STK_Tuple;
            for (u32 i = 0; i < pattern->child_count; ++i) {
                u32 child_index = pattern->first_child + i;
                if (child_index >= array_count(ctx->hir->pattern_children)) {
                    return (LlvmValue){0};
                }

                u32 child_type = variant_payload_type;
                string child_value = payload;
                if (payload_is_tuple || storage_payload_type != child_type) {
                    child_type = payload_is_tuple
                                     ? llvm_record_field_type(
                                           ctx->sema, variant_payload_type, i)
                                     : variant_payload_type;
                    string storage_type =
                        llvm_type_string(ctx, storage_payload_type);
                    child_value = llvm_temp(ctx);
                    sb_format(ctx->sb,
                              "  " STRINGP " = extractvalue " STRINGP " "
                              STRINGP ", %u\n",
                              STRINGV(child_value),
                              STRINGV(storage_type),
                              STRINGV(payload),
                              i);
                }

                const HirPatternChild* child =
                    &ctx->hir->pattern_children[child_index];
                LlvmValue child_condition =
                    llvm_emit_pattern_condition(ctx,
                                                function,
                                                (LlvmValue){
                                                    .ok = true,
                                                    .type_index = child_type,
                                                    .value      = child_value,
                                                },
                                                child->pattern_index);
                if (!child_condition.ok) {
                    return (LlvmValue){0};
                }

                string and_temp = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = and i1 " STRINGP ", " STRINGP
                          "\n",
                          STRINGV(and_temp),
                          STRINGV(result.value),
                          STRINGV(child_condition.value));
                result.value = and_temp;
            }
            return result;
        }
    default:
        return (LlvmValue){0};
    }
}

internal LlvmValue llvm_emit_branch_pattern_condition(
    LlvmFunctionContext* ctx,
    const HirFunction*   function,
    LlvmValue            scrutinee,
    const HirOnBranch*   branch)
{
    LlvmValue result = {0};
    for (u32 i = 0; i < branch->pattern_count; ++i) {
        u32 pattern_index_index = branch->first_pattern + i;
        if (pattern_index_index >= array_count(ctx->hir->on_branch_patterns)) {
            return (LlvmValue){0};
        }

        LlvmValue condition = llvm_emit_pattern_condition(
            ctx,
            function,
            scrutinee,
            ctx->hir->on_branch_patterns[pattern_index_index]);
        if (!condition.ok) {
            return (LlvmValue){0};
        }

        if (!result.ok) {
            result = condition;
            continue;
        }

        string temp = llvm_temp(ctx);
        sb_format(ctx->sb,
                  "  " STRINGP " = or i1 " STRINGP ", " STRINGP "\n",
                  STRINGV(temp),
                  STRINGV(result.value),
                  STRINGV(condition.value));
        result.value = temp;
    }
    return result;
}

internal LlvmValue llvm_address_of_expr(LlvmFunctionContext* ctx,
                                        const HirFunction*   function,
                                        u32                  expr_index)
{
    if (expr_index >= array_count(ctx->hir->exprs)) {
        return (LlvmValue){0};
    }

    const HirExpr* expr = &ctx->hir->exprs[expr_index];
    if (expr->kind != HIR_EXPR_LocalRef || expr->ref_kind != HIR_REF_Local) {
        return (LlvmValue){0};
    }

    LlvmLocalSlot* slot = llvm_find_local_slot(ctx, expr->ref_index);
    if (slot == NULL) {
        slot = llvm_ensure_local_slot(ctx, expr->ref_index, expr->type_index);

        LlvmValue current = {0};
        if (llvm_find_local_value(ctx, expr->ref_index, &current)) {
            llvm_store_local_slot(ctx, slot, current);
        } else {
            string param = llvm_param_value(function,
                                            ctx->hir,
                                            ctx->lexer,
                                            ctx->arena,
                                            expr->ref_index);
            if (param.count == 0) {
                return (LlvmValue){0};
            }
            llvm_store_local_slot(ctx,
                                  slot,
                                  (LlvmValue){
                                      .ok         = true,
                                      .type_index = expr->type_index,
                                      .value      = param,
                                  });
        }
    }

    return (LlvmValue){
        .ok         = true,
        .type_index = sema_no_type(),
        .value      = slot->ptr,
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
    case HIR_BINARY_LogicalAnd:
        return s("and");
    case HIR_BINARY_LogicalOr:
        return s("or");
    default:
        return (string){0};
    }
}

internal string llvm_compare_instruction(HirBinaryOp op)
{
    switch (op) {
    case HIR_BINARY_Equal:
        return s("eq");
    case HIR_BINARY_NotEqual:
        return s("ne");
    case HIR_BINARY_Less:
        return s("slt");
    case HIR_BINARY_LessEqual:
        return s("sle");
    case HIR_BINARY_Greater:
        return s("sgt");
    case HIR_BINARY_GreaterEqual:
        return s("sge");
    default:
        return (string){0};
    }
}

internal bool llvm_emit_effect_stmt_indices(LlvmFunctionContext* ctx,
                                            const HirFunction*   function,
                                            const u32*           stmt_indices,
                                            u32                  first_stmt,
                                            u32                  stmt_count);
internal bool llvm_emit_effect_block(LlvmFunctionContext* ctx,
                                     const HirFunction*   function,
                                     u32                  block_index);

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

internal string llvm_cast_instruction(LlvmFunctionContext* ctx,
                                      u32                  source_type,
                                      u32                  target_type)
{
    u32 source_int_bits   = llvm_integer_bits(ctx->sema, source_type);
    u32 target_int_bits   = llvm_integer_bits(ctx->sema, target_type);
    u32 source_float_bits = llvm_float_bits(ctx->sema, source_type);
    u32 target_float_bits = llvm_float_bits(ctx->sema, target_type);

    if (source_int_bits > 0 && target_int_bits > 0) {
        if (source_int_bits == target_int_bits) {
            return s("");
        }
        return source_int_bits > target_int_bits
                   ? s("trunc")
                   : (llvm_type_is_unsigned_integer(ctx->sema, source_type)
                          ? s("zext")
                          : s("sext"));
    }

    if (source_float_bits > 0 && target_float_bits > 0) {
        if (source_float_bits == target_float_bits) {
            return s("");
        }
        return source_float_bits > target_float_bits ? s("fptrunc")
                                                     : s("fpext");
    }

    if (source_float_bits > 0 && target_int_bits > 0) {
        return llvm_type_is_unsigned_integer(ctx->sema, target_type)
                   ? s("fptoui")
                   : s("fptosi");
    }

    if (source_int_bits > 0 && target_float_bits > 0) {
        return llvm_type_is_unsigned_integer(ctx->sema, source_type)
                   ? s("uitofp")
                   : s("sitofp");
    }

    if (llvm_type_kind(ctx->sema, source_type) == STK_Pointer &&
        target_int_bits > 0) {
        return s("ptrtoint");
    }

    if (source_int_bits > 0 &&
        llvm_type_kind(ctx->sema, target_type) == STK_Pointer) {
        return s("inttoptr");
    }

    return (string){0};
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
    case HIR_EXPR_FloatLiteral:
        return (LlvmValue){
            .ok         = true,
            .type_index = expr->type_index,
            .value      = string_format(ctx->arena, "%.17g", expr->floating),
        };
    case HIR_EXPR_StringLiteral:
        return (LlvmValue){
            .ok         = true,
            .type_index = expr->type_index,
            .value      = s("null"),
        };
    case HIR_EXPR_BoolLiteral:
        return (LlvmValue){
            .ok         = true,
            .type_index = expr->type_index,
            .value      = expr->boolean ? s("1") : s("0"),
        };
    case HIR_EXPR_NilLiteral:
        return (LlvmValue){
            .ok         = true,
            .type_index = expr->type_index,
            .value      = s("null"),
        };
    case HIR_EXPR_Array:
        {
            Arena array_arena = {0};
            arena_init(&array_arena);
            StringBuilder value = {0};
            sb_init(&value, &array_arena);
            sb_append_char(&value, '[');
            for (u32 i = 0; i < expr->arg_count; ++i) {
                if (i > 0) {
                    sb_append_cstr(&value, ", ");
                }
                const HirCallArg* arg =
                    &ctx->hir->call_args[expr->first_arg + i];
                LlvmValue item = llvm_emit_expr(ctx, function, arg->expr_index);
                if (!item.ok) {
                    return (LlvmValue){0};
                }
                string item_type = llvm_type_string(ctx, item.type_index);
                sb_format(&value,
                          STRINGP " " STRINGP,
                          STRINGV(item_type),
                          STRINGV(item.value));
            }
            sb_append_char(&value, ']');
            string rendered =
                string_format(ctx->arena, STRINGP, STRINGV(sb_to_string(&value)));
            arena_done(&array_arena);
            return (LlvmValue){
                .ok         = true,
                .type_index = expr->type_index,
                .value      = rendered,
            };
        }
    case HIR_EXPR_Tuple:
        {
            Arena tuple_arena = {0};
            arena_init(&tuple_arena);
            StringBuilder value = {0};
            sb_init(&value, &tuple_arena);
            sb_append_cstr(&value, "{ ");
            for (u32 i = 0; i < expr->arg_count; ++i) {
                if (i > 0) {
                    sb_append_cstr(&value, ", ");
                }
                const HirCallArg* arg =
                    &ctx->hir->call_args[expr->first_arg + i];
                LlvmValue item = llvm_emit_expr(ctx, function, arg->expr_index);
                if (!item.ok) {
                    arena_done(&tuple_arena);
                    return (LlvmValue){0};
                }
                string item_type = llvm_type_string(ctx, item.type_index);
                sb_format(&value,
                          STRINGP " " STRINGP,
                          STRINGV(item_type),
                          STRINGV(item.value));
            }
            sb_append_cstr(&value, " }");
            string rendered =
                string_format(ctx->arena, STRINGP, STRINGV(sb_to_string(&value)));
            arena_done(&tuple_arena);
            return (LlvmValue){
                .ok         = true,
                .type_index = expr->type_index,
                .value      = rendered,
            };
        }
    case HIR_EXPR_Plex:
    case HIR_EXPR_PlexUpdate:
        {
            Arena plex_arena = {0};
            arena_init(&plex_arena);
            StringBuilder value = {0};
            sb_init(&value, &plex_arena);

            LlvmValue base = {0};
            if (expr->kind == HIR_EXPR_PlexUpdate) {
                base = llvm_emit_expr(ctx, function, expr->operand_expr_index);
                if (!base.ok) {
                    arena_done(&plex_arena);
                    return (LlvmValue){0};
                }
                sb_append_string(&value, base.value);
            } else {
                sb_append_cstr(&value, "{ ");
                u32 field_count =
                    llvm_record_field_count(ctx->sema, expr->type_index);
                for (u32 i = 0; i < field_count; ++i) {
                    if (i > 0) {
                        sb_append_cstr(&value, ", ");
                    }

                    LlvmValue field_value =
                        llvm_default_value(ctx,
                                           llvm_record_field_type(
                                               ctx->sema, expr->type_index, i));
                    for (u32 j = 0; j < expr->arg_count; ++j) {
                        const HirCallArg* arg =
                            &ctx->hir->call_args[expr->first_arg + j];
                        if (llvm_record_field_index(ctx->sema,
                                                    expr->type_index,
                                                    arg->symbol_handle) == i) {
                            field_value =
                                llvm_emit_expr(ctx, function, arg->expr_index);
                            break;
                        }
                    }
                    if (!field_value.ok) {
                        arena_done(&plex_arena);
                        return (LlvmValue){0};
                    }
                    string field_type =
                        llvm_type_string(ctx, field_value.type_index);
                    sb_format(&value,
                              STRINGP " " STRINGP,
                              STRINGV(field_type),
                              STRINGV(field_value.value));
                }
                sb_append_cstr(&value, " }");
            }

            string rendered =
                string_format(ctx->arena, STRINGP, STRINGV(sb_to_string(&value)));
            arena_done(&plex_arena);

            if (expr->kind == HIR_EXPR_PlexUpdate) {
                for (u32 i = 0; i < expr->arg_count; ++i) {
                    const HirCallArg* arg =
                        &ctx->hir->call_args[expr->first_arg + i];
                    u32 field_index = llvm_record_field_index(
                        ctx->sema, expr->type_index, arg->symbol_handle);
                    if (field_index == U32_MAX) {
                        return (LlvmValue){0};
                    }
                    LlvmValue field_value =
                        llvm_emit_expr(ctx, function, arg->expr_index);
                    if (!field_value.ok) {
                        return (LlvmValue){0};
                    }
                    string temp        = llvm_temp(ctx);
                    string record_type = llvm_type_string(ctx, expr->type_index);
                    string field_type =
                        llvm_type_string(ctx, field_value.type_index);
                    sb_format(ctx->sb,
                              "  " STRINGP " = insertvalue " STRINGP " "
                              STRINGP ", " STRINGP " " STRINGP ", %u\n",
                              STRINGV(temp),
                              STRINGV(record_type),
                              STRINGV(rendered),
                              STRINGV(field_type),
                              STRINGV(field_value.value),
                              field_index);
                    rendered = temp;
                }
            }

            return (LlvmValue){
                .ok         = true,
                .type_index = expr->type_index,
                .value      = rendered,
            };
        }
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
            string cmp = llvm_compare_instruction(expr->binary_op);
            if (cmp.count > 0) {
                LlvmValue lhs =
                    llvm_emit_expr(ctx, function, expr->lhs_expr_index);
                LlvmValue rhs =
                    llvm_emit_expr(ctx, function, expr->rhs_expr_index);
                if (!lhs.ok || !rhs.ok) {
                    return (LlvmValue){0};
                }

                string type = llvm_type_string(ctx, lhs.type_index);
                string temp = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = icmp " STRINGP " " STRINGP
                          " " STRINGP ", " STRINGP "\n",
                          STRINGV(temp),
                          STRINGV(cmp),
                          STRINGV(type),
                          STRINGV(lhs.value),
                          STRINGV(rhs.value));
                return (LlvmValue){
                    .ok         = true,
                    .type_index = expr->type_index,
                    .value      = temp,
                };
            }

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
    case HIR_EXPR_Unary:
        {
            LlvmValue operand =
                llvm_emit_expr(ctx, function, expr->operand_expr_index);
            if (!operand.ok) {
                return (LlvmValue){0};
            }

            string type = llvm_type_string(ctx, expr->type_index);
            string temp = llvm_temp(ctx);
            switch (expr->unary_op) {
            case HIR_UNARY_LogicalNot:
                sb_format(ctx->sb,
                          "  " STRINGP " = xor " STRINGP " " STRINGP
                          ", 1\n",
                          STRINGV(temp),
                          STRINGV(type),
                          STRINGV(operand.value));
                return (LlvmValue){
                    .ok         = true,
                    .type_index = expr->type_index,
                    .value      = temp,
                };
            case HIR_UNARY_Negate:
                sb_format(ctx->sb,
                          "  " STRINGP " = sub " STRINGP " 0, " STRINGP
                          "\n",
                          STRINGV(temp),
                          STRINGV(type),
                          STRINGV(operand.value));
                return (LlvmValue){
                    .ok         = true,
                    .type_index = expr->type_index,
                    .value      = temp,
                };
            case HIR_UNARY_AddressOf:
                {
                    LlvmValue address =
                        llvm_address_of_expr(ctx, function, expr->operand_expr_index);
                    if (!address.ok) {
                        return (LlvmValue){0};
                    }
                    address.type_index = expr->type_index;
                    return address;
                }
            case HIR_UNARY_Deref:
                {
                    string pointee = llvm_type_string(ctx, expr->type_index);
                    sb_format(ctx->sb,
                              "  " STRINGP " = load " STRINGP ", ptr "
                              STRINGP "\n",
                              STRINGV(temp),
                              STRINGV(pointee),
                              STRINGV(operand.value));
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
    case HIR_EXPR_Index:
        {
            LlvmValue target =
                llvm_emit_expr(ctx, function, expr->operand_expr_index);
            LlvmValue index =
                llvm_emit_expr(ctx, function, expr->extra_expr_index);
            if (!target.ok || !index.ok) {
                return (LlvmValue){0};
            }

            if (llvm_type_kind(ctx->sema, target.type_index) == STK_Array) {
                string temp       = llvm_temp(ctx);
                string array_type = llvm_type_string(ctx, target.type_index);
                sb_format(ctx->sb,
                          "  " STRINGP " = extractvalue " STRINGP " "
                          STRINGP ", " STRINGP "\n",
                          STRINGV(temp),
                          STRINGV(array_type),
                          STRINGV(target.value),
                          STRINGV(index.value));
                return (LlvmValue){
                    .ok         = true,
                    .type_index = expr->type_index,
                    .value      = temp,
                };
            }

            if (llvm_type_kind(ctx->sema, target.type_index) == STK_Slice) {
                string slice_type = llvm_type_string(ctx, target.type_index);
                string data_ptr   = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = extractvalue " STRINGP " "
                          STRINGP ", 0\n",
                          STRINGV(data_ptr),
                          STRINGV(slice_type),
                          STRINGV(target.value));

                u32 item_type =
                    llvm_collection_item_type(ctx->sema, target.type_index);
                if (item_type == sema_no_type()) {
                    item_type = expr->type_index;
                }
                string item_type_string = llvm_type_string(ctx, item_type);
                string index_type       = llvm_type_string(ctx, index.type_index);
                string item_ptr         = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = getelementptr inbounds " STRINGP
                          ", ptr " STRINGP ", " STRINGP " " STRINGP "\n",
                          STRINGV(item_ptr),
                          STRINGV(item_type_string),
                          STRINGV(data_ptr),
                          STRINGV(index_type),
                          STRINGV(index.value));
                string loaded = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = load " STRINGP ", ptr " STRINGP
                          "\n",
                          STRINGV(loaded),
                          STRINGV(item_type_string),
                          STRINGV(item_ptr));
                return (LlvmValue){
                    .ok         = true,
                    .type_index = expr->type_index,
                    .value      = loaded,
                };
            }

            u32 item_type = llvm_pointee_type(ctx->sema, target.type_index);
            if (item_type == sema_no_type()) {
                item_type = expr->type_index;
            }
            string item_type_string = llvm_type_string(ctx, item_type);
            string index_type       = llvm_type_string(ctx, index.type_index);
            string ptr              = llvm_temp(ctx);
            sb_format(ctx->sb,
                      "  " STRINGP " = getelementptr inbounds " STRINGP
                      ", ptr " STRINGP ", " STRINGP " " STRINGP "\n",
                      STRINGV(ptr),
                      STRINGV(item_type_string),
                      STRINGV(target.value),
                      STRINGV(index_type),
                      STRINGV(index.value));
            string loaded = llvm_temp(ctx);
            sb_format(ctx->sb,
                      "  " STRINGP " = load " STRINGP ", ptr " STRINGP "\n",
                      STRINGV(loaded),
                      STRINGV(item_type_string),
                      STRINGV(ptr));
            return (LlvmValue){
                .ok         = true,
                .type_index = expr->type_index,
                .value      = loaded,
            };
        }
    case HIR_EXPR_Slice:
        {
            LlvmValue target_address =
                llvm_address_of_expr(ctx, function, expr->operand_expr_index);
            if (!target_address.ok ||
                expr->operand_expr_index >= array_count(ctx->hir->exprs)) {
                return (LlvmValue){0};
            }

            const HirExpr* target_expr =
                &ctx->hir->exprs[expr->operand_expr_index];
            u32 target_type = target_expr->type_index;
            u32 item_type =
                llvm_collection_item_type(ctx->sema, target_type);
            if (item_type == sema_no_type()) {
                return (LlvmValue){0};
            }

            i64 start_value = 0;
            i64 end_value   = (i64)llvm_array_count(ctx->sema, target_type);
            llvm_expr_integer_constant(
                ctx->hir, expr->lhs_expr_index, &start_value);
            llvm_expr_integer_constant(
                ctx->hir, expr->rhs_expr_index, &end_value);
            i64 count_value = end_value - start_value;

            string target_type_string = llvm_type_string(ctx, target_type);
            string data_ptr           = llvm_temp(ctx);
            sb_format(ctx->sb,
                      "  " STRINGP " = getelementptr inbounds " STRINGP
                      ", ptr " STRINGP ", i64 0, i64 %lld\n",
                      STRINGV(data_ptr),
                      STRINGV(target_type_string),
                      STRINGV(target_address.value),
                      (long long)start_value);
            string slice0 = llvm_temp(ctx);
            sb_format(ctx->sb,
                      "  " STRINGP " = insertvalue { ptr, i64 } poison, ptr "
                      STRINGP ", 0\n",
                      STRINGV(slice0),
                      STRINGV(data_ptr));
            string slice1 = llvm_temp(ctx);
            sb_format(ctx->sb,
                      "  " STRINGP " = insertvalue { ptr, i64 } " STRINGP
                      ", i64 %lld, 1\n",
                      STRINGV(slice1),
                      STRINGV(slice0),
                      (long long)count_value);
            return (LlvmValue){
                .ok         = true,
                .type_index = expr->type_index,
                .value      = slice1,
            };
        }
    case HIR_EXPR_TupleField:
    case HIR_EXPR_Field:
        {
            LlvmValue target =
                llvm_emit_expr(ctx, function, expr->operand_expr_index);
            if (!target.ok) {
                return (LlvmValue){0};
            }

            u32 field_index = U32_MAX;
            if (expr->kind == HIR_EXPR_TupleField) {
                field_index = (u32)expr->integer;
            } else {
                field_index = llvm_record_field_index(
                    ctx->sema, target.type_index, expr->symbol_handle);
            }
            if (field_index == U32_MAX) {
                return (LlvmValue){0};
            }

            string temp        = llvm_temp(ctx);
            string record_type = llvm_type_string(ctx, target.type_index);
            sb_format(ctx->sb,
                      "  " STRINGP " = extractvalue " STRINGP " "
                      STRINGP ", %u\n",
                      STRINGV(temp),
                      STRINGV(record_type),
                      STRINGV(target.value),
                      field_index);
            return (LlvmValue){
                .ok         = true,
                .type_index = expr->type_index,
                .value      = temp,
            };
        }
    case HIR_EXPR_Block:
        {
            if (expr->body_block_index >= array_count(ctx->hir->blocks)) {
                return (LlvmValue){0};
            }

            const HirBlock* block = &ctx->hir->blocks[expr->body_block_index];
            for (u32 i = 0; i < block->stmt_count; ++i) {
                u32 stmt_index = block->stmt_indices[i];
                if (stmt_index >= array_count(ctx->hir->stmts)) {
                    return (LlvmValue){0};
                }

                const HirStmt* stmt = &ctx->hir->stmts[stmt_index];
                if (stmt->kind == HIR_STMT_Break) {
                    return llvm_emit_expr(ctx, function, stmt->expr_index);
                }
            }

            return (LlvmValue){0};
        }
    case HIR_EXPR_On:
        {
            if (expr->on_kind != HIR_ON_Condition) {
                LlvmValue scrutinee =
                    llvm_emit_expr(ctx, function, expr->operand_expr_index);
                if (!scrutinee.ok || expr->branch_count == 0) {
                    return (LlvmValue){0};
                }

                if (llvm_type_is_void(ctx->sema, expr->type_index)) {
                    string end_label = llvm_label(ctx, "on.end");
                    for (u32 i = 0; i < expr->branch_count; ++i) {
                        const HirOnBranch* branch =
                            &ctx->hir->on_branches[expr->first_branch + i];
                        string body_label = llvm_label(ctx, "on.body");
                        string next_label =
                            i + 1 < expr->branch_count ? llvm_label(ctx, "on.next")
                                                       : end_label;

                        if (branch->is_else) {
                            sb_format(ctx->sb,
                                      "  br label %%" STRINGP "\n",
                                      STRINGV(body_label));
                        } else {
                            LlvmValue condition =
                                llvm_emit_branch_pattern_condition(
                                    ctx, function, scrutinee, branch);
                            if (!condition.ok) {
                                return (LlvmValue){0};
                            }
                            sb_format(ctx->sb,
                                      "  br i1 " STRINGP ", label %%"
                                      STRINGP ", label %%" STRINGP "\n",
                                      STRINGV(condition.value),
                                      STRINGV(body_label),
                                      STRINGV(next_label));
                        }

                        sb_format(ctx->sb,
                                  STRINGP ":\n",
                                  STRINGV(body_label));
                        llvm_bind_symbol_value(
                            ctx, branch->binder_symbol_handle, scrutinee);
                        ctx->block_terminated = false;
                        if (!llvm_emit_block(
                                ctx, function, branch->body_block_index)) {
                            return (LlvmValue){0};
                        }
                        if (!ctx->block_terminated) {
                            sb_format(ctx->sb,
                                      "  br label %%" STRINGP "\n",
                                      STRINGV(end_label));
                        }
                        ctx->block_terminated = false;

                        if (branch->is_else) {
                            break;
                        }
                        if (!string_eq(next_label, end_label)) {
                            sb_format(ctx->sb,
                                      STRINGP ":\n",
                                      STRINGV(next_label));
                        }
                    }
                    sb_format(ctx->sb, STRINGP ":\n", STRINGV(end_label));
                    return (LlvmValue){
                        .ok         = true,
                        .type_index = expr->type_index,
                        .value      = s(""),
                    };
                }

                string end_label = llvm_label(ctx, "on.end");
                Array(LlvmValue) phi_values = NULL;
                Array(string)    phi_labels = NULL;

                for (u32 i = 0; i < expr->branch_count; ++i) {
                    const HirOnBranch* branch =
                        &ctx->hir->on_branches[expr->first_branch + i];
                    string body_label = llvm_label(ctx, "on.body");
                    string next_label = llvm_label(ctx, "on.next");

                    if (branch->is_else) {
                        sb_format(ctx->sb,
                                  "  br label %%" STRINGP "\n",
                                  STRINGV(body_label));
                    } else {
                        LlvmValue condition = llvm_emit_branch_pattern_condition(
                            ctx, function, scrutinee, branch);
                        if (!condition.ok) {
                            array_free(phi_values);
                            array_free(phi_labels);
                            return (LlvmValue){0};
                        }

                        if (branch->guard_expr_index != U32_MAX) {
                            string guard_label = llvm_label(ctx, "on.guard");
                            sb_format(ctx->sb,
                                      "  br i1 " STRINGP ", label %%"
                                      STRINGP ", label %%" STRINGP "\n",
                                      STRINGV(condition.value),
                                      STRINGV(guard_label),
                                      STRINGV(next_label));
                            sb_format(ctx->sb,
                                      STRINGP ":\n",
                                      STRINGV(guard_label));
                            llvm_bind_symbol_value(
                                ctx, branch->binder_symbol_handle, scrutinee);
                            LlvmValue guard = llvm_emit_expr(
                                ctx, function, branch->guard_expr_index);
                            if (!guard.ok) {
                                array_free(phi_values);
                                array_free(phi_labels);
                                return (LlvmValue){0};
                            }
                            sb_format(ctx->sb,
                                      "  br i1 " STRINGP ", label %%"
                                      STRINGP ", label %%" STRINGP "\n",
                                      STRINGV(guard.value),
                                      STRINGV(body_label),
                                      STRINGV(next_label));
                        } else {
                            sb_format(ctx->sb,
                                      "  br i1 " STRINGP ", label %%"
                                      STRINGP ", label %%" STRINGP "\n",
                                      STRINGV(condition.value),
                                      STRINGV(body_label),
                                      STRINGV(next_label));
                        }
                    }

                    sb_format(ctx->sb, STRINGP ":\n", STRINGV(body_label));
                    llvm_bind_symbol_value(
                        ctx, branch->binder_symbol_handle, scrutinee);
                    LlvmValue value = llvm_emit_block_value(
                        ctx, function, branch->body_block_index);
                    if (!value.ok) {
                        array_free(phi_values);
                        array_free(phi_labels);
                        return (LlvmValue){0};
                    }
                    sb_format(ctx->sb,
                              "  br label %%" STRINGP "\n",
                              STRINGV(end_label));
                    array_push(phi_values, value);
                    array_push(phi_labels, body_label);

                    if (branch->is_else) {
                        break;
                    }
                    sb_format(ctx->sb, STRINGP ":\n", STRINGV(next_label));
                }

                if (array_count(phi_values) == 0) {
                    array_free(phi_values);
                    array_free(phi_labels);
                    return (LlvmValue){0};
                }

                sb_format(ctx->sb, STRINGP ":\n", STRINGV(end_label));
                string type = llvm_type_string(ctx, expr->type_index);
                string phi  = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = phi " STRINGP " ",
                          STRINGV(phi),
                          STRINGV(type));
                for (u32 i = 0; i < array_count(phi_values); ++i) {
                    if (i > 0) {
                        sb_append_cstr(ctx->sb, ", ");
                    }
                    sb_format(ctx->sb,
                              "[" STRINGP ", %%" STRINGP "]",
                              STRINGV(phi_values[i].value),
                              STRINGV(phi_labels[i]));
                }
                sb_append_char(ctx->sb, '\n');
                array_free(phi_values);
                array_free(phi_labels);
                return (LlvmValue){
                    .ok         = true,
                    .type_index = expr->type_index,
                    .value      = phi,
                };
            }

            if (expr->branch_count != 2) {
                return (LlvmValue){0};
            }

            const HirOnBranch* then_branch =
                &ctx->hir->on_branches[expr->first_branch];
            const HirOnBranch* else_branch =
                &ctx->hir->on_branches[expr->first_branch + 1];
            if (then_branch->is_else || !else_branch->is_else ||
                then_branch->guard_expr_index == U32_MAX) {
                return (LlvmValue){0};
            }

            LlvmValue condition =
                llvm_emit_expr(ctx, function, then_branch->guard_expr_index);
            if (!condition.ok) {
                return (LlvmValue){0};
            }

            string then_label = llvm_label(ctx, "on.then");
            string else_label = llvm_label(ctx, "on.else");
            string end_label  = llvm_label(ctx, "on.end");
            sb_format(ctx->sb,
                      "  br i1 " STRINGP ", label %%" STRINGP
                      ", label %%" STRINGP "\n",
                      STRINGV(condition.value),
                      STRINGV(then_label),
                      STRINGV(else_label));

            sb_format(ctx->sb, STRINGP ":\n", STRINGV(then_label));
            LlvmValue then_value = llvm_emit_block_value(
                ctx, function, then_branch->body_block_index);
            if (!then_value.ok) {
                return (LlvmValue){0};
            }
            sb_format(ctx->sb,
                      "  br label %%" STRINGP "\n",
                      STRINGV(end_label));

            sb_format(ctx->sb, STRINGP ":\n", STRINGV(else_label));
            LlvmValue else_value = llvm_emit_block_value(
                ctx, function, else_branch->body_block_index);
            if (!else_value.ok) {
                return (LlvmValue){0};
            }
            sb_format(ctx->sb,
                      "  br label %%" STRINGP "\n",
                      STRINGV(end_label));

            sb_format(ctx->sb, STRINGP ":\n", STRINGV(end_label));
            string type = llvm_type_string(ctx, expr->type_index);
            string phi  = llvm_temp(ctx);
            sb_format(ctx->sb,
                      "  " STRINGP " = phi " STRINGP " [" STRINGP ", %%"
                      STRINGP "], [" STRINGP ", %%" STRINGP "]\n",
                      STRINGV(phi),
                      STRINGV(type),
                      STRINGV(then_value.value),
                      STRINGV(then_label),
                      STRINGV(else_value.value),
                      STRINGV(else_label));
            return (LlvmValue){
                .ok         = true,
                .type_index = expr->type_index,
                .value      = phi,
            };
        }
    case HIR_EXPR_For:
        {
            if (expr->for_index >= array_count(ctx->hir->fors)) {
                return (LlvmValue){0};
            }

            const HirFor* loop = &ctx->hir->fors[expr->for_index];
            if (loop->kind != HIR_FOR_Condition &&
                loop->kind != HIR_FOR_CStyle && loop->kind != HIR_FOR_In) {
                return (LlvmValue){0};
            }

            if (loop->kind == HIR_FOR_In) {
                LlvmValue iterable =
                    llvm_emit_expr(ctx, function, loop->iterable_expr_index);
                if (!iterable.ok ||
                    llvm_type_kind(ctx->sema, iterable.type_index) !=
                        STK_Slice) {
                    return (LlvmValue){0};
                }

                string slice_type = llvm_type_string(ctx, iterable.type_index);
                string data_ptr   = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = extractvalue " STRINGP " "
                          STRINGP ", 0\n",
                          STRINGV(data_ptr),
                          STRINGV(slice_type),
                          STRINGV(iterable.value));
                string count = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = extractvalue " STRINGP " "
                          STRINGP ", 1\n",
                          STRINGV(count),
                          STRINGV(slice_type),
                          STRINGV(iterable.value));

                u32 index_type = llvm_local_type(ctx, loop->index_local_index);
                if (index_type == sema_no_type()) {
                    index_type = sema_no_type();
                    for (u32 i = 0; i < array_count(ctx->sema->types); ++i) {
                        if (ctx->sema->types[i].kind == STK_Usize) {
                            index_type = i;
                            break;
                        }
                    }
                }
                LlvmLocalSlot* index_slot = NULL;
                LlvmLocalSlot  hidden_index_slot = {0};
                bool           hidden_index      = loop->index_local_index == U32_MAX;
                if (loop->index_local_index != U32_MAX) {
                    index_slot = llvm_ensure_local_slot(
                        ctx, loop->index_local_index, index_type);
                } else {
                    hidden_index_slot = (LlvmLocalSlot){
                        .local_index = U32_MAX,
                        .type_index  = index_type,
                        .ptr         = llvm_temp(ctx),
                    };
                    sb_format(ctx->sb,
                              "  " STRINGP " = alloca i64\n",
                              STRINGV(hidden_index_slot.ptr));
                    index_slot = &hidden_index_slot;
                }
                if (hidden_index) {
                    sb_format(ctx->sb,
                              "  store i64 0, ptr " STRINGP "\n",
                              STRINGV(index_slot->ptr));
                } else {
                    llvm_store_local_slot(ctx,
                                          index_slot,
                                          (LlvmValue){
                                              .ok         = true,
                                              .type_index = index_type,
                                              .value      = s("0"),
                                          });
                }

                u32 item_type = llvm_local_type(ctx, loop->item_local_index);
                LlvmLocalSlot* item_slot = NULL;
                if (loop->item_local_index != U32_MAX) {
                    item_slot = llvm_ensure_local_slot(
                        ctx, loop->item_local_index, item_type);
                }

                string cond_label = llvm_label(ctx, "for.in.cond");
                string body_label = llvm_label(ctx, "for.in.body");
                string end_label  = llvm_label(ctx, "for.in.end");
                sb_format(ctx->sb,
                          "  br label %%" STRINGP "\n",
                          STRINGV(cond_label));

                sb_format(ctx->sb, STRINGP ":\n", STRINGV(cond_label));
                LlvmValue index_value = {0};
                if (hidden_index) {
                    string loaded = llvm_temp(ctx);
                    sb_format(ctx->sb,
                              "  " STRINGP " = load i64, ptr " STRINGP "\n",
                              STRINGV(loaded),
                              STRINGV(index_slot->ptr));
                    index_value = (LlvmValue){
                        .ok         = true,
                        .type_index = sema_no_type(),
                        .value      = loaded,
                    };
                } else {
                    index_value = llvm_load_local_slot(ctx, index_slot);
                }
                string cond = llvm_temp(ctx);
                string index_type_string =
                    hidden_index ? s("i64")
                                 : llvm_type_string(ctx, index_value.type_index);
                sb_format(ctx->sb,
                          "  " STRINGP " = icmp ult " STRINGP " " STRINGP
                          ", " STRINGP "\n",
                          STRINGV(cond),
                          STRINGV(index_type_string),
                          STRINGV(index_value.value),
                          STRINGV(count));
                sb_format(ctx->sb,
                          "  br i1 " STRINGP ", label %%" STRINGP
                          ", label %%" STRINGP "\n",
                          STRINGV(cond),
                          STRINGV(body_label),
                          STRINGV(end_label));

                sb_format(ctx->sb, STRINGP ":\n", STRINGV(body_label));
                u32 value_type =
                    llvm_collection_item_type(ctx->sema, iterable.type_index);
                string value_type_string = llvm_type_string(ctx, value_type);
                string item_ptr          = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = getelementptr inbounds " STRINGP
                          ", ptr " STRINGP ", " STRINGP " " STRINGP "\n",
                          STRINGV(item_ptr),
                          STRINGV(value_type_string),
                          STRINGV(data_ptr),
                          STRINGV(index_type_string),
                          STRINGV(index_value.value));
                if (item_slot != NULL) {
                    llvm_store_local_slot(ctx,
                                          item_slot,
                                          (LlvmValue){
                                              .ok         = true,
                                              .type_index = item_type,
                                              .value      = item_ptr,
                                          });
                }
                string old_break    = ctx->break_label;
                string old_continue = ctx->continue_label;
                ctx->break_label    = end_label;
                ctx->continue_label = cond_label;
                if (!llvm_emit_effect_block(ctx, function, loop->body_block_index)) {
                    return (LlvmValue){0};
                }
                ctx->break_label    = old_break;
                ctx->continue_label = old_continue;

                if (!ctx->block_terminated) {
                    LlvmValue current = {0};
                    if (hidden_index) {
                        string loaded = llvm_temp(ctx);
                        sb_format(ctx->sb,
                                  "  " STRINGP " = load i64, ptr " STRINGP
                                  "\n",
                                  STRINGV(loaded),
                                  STRINGV(index_slot->ptr));
                        current = (LlvmValue){
                            .ok         = true,
                            .type_index = sema_no_type(),
                            .value      = loaded,
                        };
                    } else {
                        current = llvm_load_local_slot(ctx, index_slot);
                    }
                    string next       = llvm_temp(ctx);
                    string type = hidden_index ? s("i64")
                                               : llvm_type_string(
                                                     ctx, current.type_index);
                    sb_format(ctx->sb,
                              "  " STRINGP " = add " STRINGP " " STRINGP
                              ", 1\n",
                              STRINGV(next),
                              STRINGV(type),
                              STRINGV(current.value));
                    if (hidden_index) {
                        sb_format(ctx->sb,
                                  "  store i64 " STRINGP ", ptr " STRINGP
                                  "\n",
                                  STRINGV(next),
                                  STRINGV(index_slot->ptr));
                    } else {
                        llvm_store_local_slot(ctx,
                                              index_slot,
                                              (LlvmValue){
                                                  .ok = true,
                                                  .type_index =
                                                      current.type_index,
                                                  .value = next,
                                              });
                    }
                    sb_format(ctx->sb,
                              "  br label %%" STRINGP "\n",
                              STRINGV(cond_label));
                }
                ctx->block_terminated = false;
                sb_format(ctx->sb, STRINGP ":\n", STRINGV(end_label));
                return (LlvmValue){
                    .ok         = true,
                    .type_index = expr->type_index,
                    .value      = s(""),
                };
            }

            if (!llvm_type_is_void(ctx->sema, expr->type_index)) {
                if (loop->kind == HIR_FOR_CStyle &&
                    !llvm_emit_effect_stmt_indices(ctx,
                                                   function,
                                                   ctx->hir->for_init_stmts,
                                                   loop->first_init_stmt,
                                                   loop->init_stmt_count)) {
                    return (LlvmValue){0};
                }

                string cond_label = llvm_label(ctx, "for.cond");
                string body_label = llvm_label(ctx, "for.body");
                string else_label = llvm_label(ctx, "for.else");
                string end_label  = llvm_label(ctx, "for.end");
                sb_format(ctx->sb,
                          "  br label %%" STRINGP "\n",
                          STRINGV(cond_label));

                sb_format(ctx->sb, STRINGP ":\n", STRINGV(cond_label));
                if (loop->condition_expr_index != U32_MAX) {
                    LlvmValue condition =
                        llvm_emit_expr(ctx, function, loop->condition_expr_index);
                    if (!condition.ok) {
                        return (LlvmValue){0};
                    }
                    string false_label = loop->else_block_index != U32_MAX
                                             ? else_label
                                             : end_label;
                    sb_format(ctx->sb,
                              "  br i1 " STRINGP ", label %%" STRINGP
                              ", label %%" STRINGP "\n",
                              STRINGV(condition.value),
                              STRINGV(body_label),
                              STRINGV(false_label));
                } else {
                    sb_format(ctx->sb,
                              "  br label %%" STRINGP "\n",
                              STRINGV(body_label));
                }

                sb_format(ctx->sb, STRINGP ":\n", STRINGV(body_label));
                LlvmValue body_value =
                    llvm_emit_block_value(ctx, function, loop->body_block_index);
                if (!body_value.ok) {
                    return (LlvmValue){0};
                }
                sb_format(ctx->sb,
                          "  br label %%" STRINGP "\n",
                          STRINGV(end_label));

                LlvmValue else_value = {0};
                if (loop->else_block_index != U32_MAX) {
                    sb_format(ctx->sb, STRINGP ":\n", STRINGV(else_label));
                    else_value = llvm_emit_block_value(
                        ctx, function, loop->else_block_index);
                    if (!else_value.ok) {
                        return (LlvmValue){0};
                    }
                    sb_format(ctx->sb,
                              "  br label %%" STRINGP "\n",
                              STRINGV(end_label));
                }

                sb_format(ctx->sb, STRINGP ":\n", STRINGV(end_label));
                string type = llvm_type_string(ctx, expr->type_index);
                string phi  = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = phi " STRINGP " [" STRINGP
                          ", %%" STRINGP,
                          STRINGV(phi),
                          STRINGV(type),
                          STRINGV(body_value.value),
                          STRINGV(body_label));
                if (else_value.ok) {
                    sb_format(ctx->sb,
                              "], [" STRINGP ", %%" STRINGP,
                              STRINGV(else_value.value),
                              STRINGV(else_label));
                }
                sb_append_cstr(ctx->sb, "]\n");
                return (LlvmValue){
                    .ok         = true,
                    .type_index = expr->type_index,
                    .value      = phi,
                };
            }

            if (loop->kind == HIR_FOR_CStyle &&
                !llvm_emit_effect_stmt_indices(ctx,
                                               function,
                                               ctx->hir->for_init_stmts,
                                               loop->first_init_stmt,
                                               loop->init_stmt_count)) {
                return (LlvmValue){0};
            }

            string cond_label   = llvm_label(ctx, "for.cond");
            string body_label   = llvm_label(ctx, "for.body");
            string update_label = llvm_label(ctx, "for.update");
            string end_label    = llvm_label(ctx, "for.end");
            sb_format(ctx->sb,
                      "  br label %%" STRINGP "\n",
                      STRINGV(cond_label));

            sb_format(ctx->sb, STRINGP ":\n", STRINGV(cond_label));
            if (loop->condition_expr_index != U32_MAX) {
                LlvmValue condition =
                    llvm_emit_expr(ctx, function, loop->condition_expr_index);
                if (!condition.ok) {
                    return (LlvmValue){0};
                }
                sb_format(ctx->sb,
                          "  br i1 " STRINGP ", label %%" STRINGP
                          ", label %%" STRINGP "\n",
                          STRINGV(condition.value),
                          STRINGV(body_label),
                          STRINGV(end_label));
            } else {
                sb_format(ctx->sb,
                          "  br label %%" STRINGP "\n",
                          STRINGV(body_label));
            }

            sb_format(ctx->sb, STRINGP ":\n", STRINGV(body_label));
            if (!llvm_emit_effect_block(ctx, function, loop->body_block_index)) {
                return (LlvmValue){0};
            }
            string next_label =
                loop->kind == HIR_FOR_CStyle ? update_label : cond_label;
            sb_format(ctx->sb,
                      "  br label %%" STRINGP "\n",
                      STRINGV(next_label));

            if (loop->kind == HIR_FOR_CStyle) {
                sb_format(ctx->sb, STRINGP ":\n", STRINGV(update_label));
                if (!llvm_emit_effect_stmt_indices(ctx,
                                                   function,
                                                   ctx->hir->for_update_stmts,
                                                   loop->first_update_stmt,
                                                   loop->update_stmt_count)) {
                    return (LlvmValue){0};
                }
                sb_format(ctx->sb,
                          "  br label %%" STRINGP "\n",
                          STRINGV(cond_label));
            }

            sb_format(ctx->sb, STRINGP ":\n", STRINGV(end_label));
            return (LlvmValue){
                .ok         = true,
                .type_index = expr->type_index,
                .value      = s(""),
            };
        }
    case HIR_EXPR_Cast:
        {
            LlvmValue operand =
                llvm_emit_expr(ctx, function, expr->operand_expr_index);
            if (!operand.ok) {
                return (LlvmValue){0};
            }

            string source_type = llvm_type_string(ctx, operand.type_index);
            string target_type = llvm_type_string(ctx, expr->type_index);
            string instr =
                llvm_cast_instruction(ctx, operand.type_index, expr->type_index);
            if (instr.count == 0) {
                if (string_eq(source_type, target_type)) {
                    operand.type_index = expr->type_index;
                    return operand;
                }
                return (LlvmValue){0};
            }

            string temp = llvm_temp(ctx);
            sb_format(ctx->sb,
                      "  " STRINGP " = " STRINGP " " STRINGP " " STRINGP
                      " to " STRINGP "\n",
                      STRINGV(temp),
                      STRINGV(instr),
                      STRINGV(source_type),
                      STRINGV(operand.value),
                      STRINGV(target_type));
            return (LlvmValue){
                .ok         = true,
                .type_index = expr->type_index,
                .value      = temp,
            };
        }
    case HIR_EXPR_Call:
        {
            if (llvm_type_kind(ctx->sema, expr->type_index) == STK_Enum) {
                u32 variant_symbol = U32_MAX;
                if (llvm_callee_symbol_handle(
                        ctx, expr->callee_expr_index, &variant_symbol)) {
                    u32 variant_index = llvm_enum_variant_index(
                        ctx->sema, expr->type_index, variant_symbol);
                    if (variant_index != U32_MAX) {
                        return llvm_emit_enum_constructor(
                            ctx, function, expr, variant_index);
                    }
                }
            }

            string callee = {0};
            if (!llvm_callee_name(ctx, expr->callee_expr_index, &callee)) {
                return (LlvmValue){0};
            }

            Array(LlvmValue) args = NULL;
            for (u32 i = 0; i < expr->arg_count; ++i) {
                const HirCallArg* arg =
                    &ctx->hir->call_args[expr->first_arg + i];
                LlvmValue value = llvm_emit_expr(ctx, function, arg->expr_index);
                if (!value.ok) {
                    array_free(args);
                    return (LlvmValue){0};
                }
                array_push(args, value);
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
                LlvmValue value = args[i];
                string type = llvm_type_string(ctx, value.type_index);
                sb_format(ctx->sb,
                          STRINGP " " STRINGP,
                          STRINGV(type),
                          STRINGV(value.value));
            }
            sb_append_cstr(ctx->sb, ")\n");
            array_free(args);
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
    LlvmValue value = llvm_emit_expr(ctx, function, stmt->expr_index);
    if (!value.ok) {
        return false;
    }

    if (target->kind == HIR_EXPR_Unary &&
        target->unary_op == HIR_UNARY_Deref) {
        LlvmValue pointer =
            llvm_emit_expr(ctx, function, target->operand_expr_index);
        if (!pointer.ok) {
            return false;
        }

        string type = llvm_type_string(ctx, target->type_index);
        sb_format(ctx->sb,
                  "  store " STRINGP " " STRINGP ", ptr " STRINGP "\n",
                  STRINGV(type),
                  STRINGV(value.value),
                  STRINGV(pointer.value));
        return true;
    }

    if (target->kind != HIR_EXPR_LocalRef ||
        target->ref_kind != HIR_REF_Local) {
        return false;
    }

    u32 type_index = target->type_index != sema_no_type() ? target->type_index
                                                          : value.type_index;
    LlvmLocalSlot* slot =
        llvm_ensure_local_slot(ctx, target->ref_index, type_index);
    llvm_store_local_slot(ctx, slot, value);
    return true;
}

internal bool llvm_emit_effect_stmt(LlvmFunctionContext* ctx,
                                    const HirFunction*   function,
                                    const HirStmt*       stmt);

internal bool llvm_emit_effect_stmt_indices(LlvmFunctionContext* ctx,
                                            const HirFunction*   function,
                                            const u32*           stmt_indices,
                                            u32                  first_stmt,
                                            u32                  stmt_count)
{
    for (u32 i = 0; i < stmt_count; ++i) {
        if (ctx->block_terminated) {
            return true;
        }
        u32 stmt_index = stmt_indices[first_stmt + i];
        if (stmt_index >= array_count(ctx->hir->stmts)) {
            return false;
        }
        if (!llvm_emit_effect_stmt(ctx,
                                   function,
                                   &ctx->hir->stmts[stmt_index])) {
            return false;
        }
    }
    return true;
}

internal bool llvm_emit_effect_block(LlvmFunctionContext* ctx,
                                     const HirFunction*   function,
                                     u32                  block_index)
{
    if (block_index >= array_count(ctx->hir->blocks)) {
        return false;
    }

    const HirBlock* block = &ctx->hir->blocks[block_index];
    ctx->block_terminated = false;
    return llvm_emit_effect_stmt_indices(
        ctx, function, block->stmt_indices, 0, block->stmt_count);
}

internal bool llvm_emit_effect_stmt(LlvmFunctionContext* ctx,
                                    const HirFunction*   function,
                                    const HirStmt*       stmt)
{
    switch (stmt->kind) {
    case HIR_STMT_Let:
        return llvm_emit_let(ctx, function, stmt);
    case HIR_STMT_Assign:
        return llvm_emit_assign(ctx, function, stmt);
    case HIR_STMT_Expr:
    case HIR_STMT_Assert:
        if (stmt->expr_index == U32_MAX) {
            return true;
        }
        return llvm_emit_expr(ctx, function, stmt->expr_index).ok;
    case HIR_STMT_Defer:
        return true;
    case HIR_STMT_Break:
        if (ctx->break_label.count == 0) {
            return false;
        }
        sb_format(ctx->sb,
                  "  br label %%" STRINGP "\n",
                  STRINGV(ctx->break_label));
        ctx->block_terminated = true;
        return true;
    case HIR_STMT_Continue:
        if (ctx->continue_label.count == 0) {
            return false;
        }
        sb_format(ctx->sb,
                  "  br label %%" STRINGP "\n",
                  STRINGV(ctx->continue_label));
        ctx->block_terminated = true;
        return true;
    case HIR_STMT_Block:
        return llvm_emit_effect_block(ctx, function, stmt->body_block_index);
    default:
        return false;
    }
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
    ctx->block_terminated = true;
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
        if (stmt->kind == HIR_STMT_Assign &&
            stmt->target_expr_index < array_count(ctx->hir->exprs)) {
            const HirExpr* target = &ctx->hir->exprs[stmt->target_expr_index];
            if (target->kind == HIR_EXPR_LocalRef &&
                target->ref_kind == HIR_REF_Local) {
                llvm_mark_assigned_local(ctx, target->ref_index);
            }
        }

        if (stmt->kind == HIR_STMT_Block) {
            llvm_collect_assigned_locals(ctx, stmt->body_block_index);
        }

        if (stmt->expr_index < array_count(ctx->hir->exprs)) {
            const HirExpr* expr = &ctx->hir->exprs[stmt->expr_index];
            if (expr->kind == HIR_EXPR_For &&
                expr->for_index < array_count(ctx->hir->fors)) {
                const HirFor* loop = &ctx->hir->fors[expr->for_index];
                llvm_collect_assigned_locals(ctx, loop->body_block_index);
                llvm_collect_assigned_locals(ctx, loop->else_block_index);
                for (u32 j = 0; j < loop->init_stmt_count; ++j) {
                    u32 init_stmt_index =
                        ctx->hir->for_init_stmts[loop->first_init_stmt + j];
                    if (init_stmt_index < array_count(ctx->hir->stmts)) {
                        const HirStmt* init_stmt =
                            &ctx->hir->stmts[init_stmt_index];
                        if (init_stmt->kind == HIR_STMT_Assign &&
                            init_stmt->target_expr_index <
                                array_count(ctx->hir->exprs)) {
                            const HirExpr* init_target =
                                &ctx->hir->exprs[init_stmt->target_expr_index];
                            if (init_target->kind == HIR_EXPR_LocalRef &&
                                init_target->ref_kind == HIR_REF_Local) {
                                llvm_mark_assigned_local(ctx,
                                                         init_target->ref_index);
                            }
                        }
                        if (init_stmt->kind == HIR_STMT_Block) {
                            llvm_collect_assigned_locals(
                                ctx, init_stmt->body_block_index);
                        }
                    }
                }
                for (u32 j = 0; j < loop->update_stmt_count; ++j) {
                    u32 update_stmt_index =
                        ctx->hir->for_update_stmts[loop->first_update_stmt + j];
                    if (update_stmt_index < array_count(ctx->hir->stmts)) {
                        const HirStmt* update_stmt =
                            &ctx->hir->stmts[update_stmt_index];
                        if (update_stmt->kind == HIR_STMT_Assign &&
                            update_stmt->target_expr_index <
                                array_count(ctx->hir->exprs)) {
                            const HirExpr* update_target =
                                &ctx->hir->exprs[update_stmt->target_expr_index];
                            if (update_target->kind == HIR_EXPR_LocalRef &&
                                update_target->ref_kind == HIR_REF_Local) {
                                llvm_mark_assigned_local(
                                    ctx, update_target->ref_index);
                            }
                        }
                        if (update_stmt->kind == HIR_STMT_Block) {
                            llvm_collect_assigned_locals(
                                ctx, update_stmt->body_block_index);
                        }
                    }
                }
            }
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
        } else if (stmt->kind == HIR_STMT_Expr ||
                   stmt->kind == HIR_STMT_Assert) {
            if (stmt->expr_index != U32_MAX) {
                LlvmValue value = llvm_emit_expr(ctx, function, stmt->expr_index);
                if (!value.ok) {
                    return false;
                }
            }
            continue;
        } else if (stmt->kind == HIR_STMT_Defer) {
            continue;
        } else if (stmt->kind == HIR_STMT_Block) {
            if (!llvm_emit_block(ctx, function, stmt->body_block_index)) {
                return false;
            }
            continue;
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
