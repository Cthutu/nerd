//------------------------------------------------------------------------------
// LLVM IR emission from HIR
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/error/error.h>
#include <compiler/build/build.h>
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

internal u32 llvm_builtin_type(const Sema* sema, SemaTypeKind kind)
{
    if (sema == NULL) {
        return sema_no_type();
    }
    for (u32 i = 0; i < array_count(sema->types); ++i) {
        if (sema->types[i].kind == kind) {
            return i;
        }
    }
    return sema_no_type();
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

internal u32 llvm_union_storage_bits(const Sema* sema, u32 union_type);

internal u32 llvm_type_storage_bits(const Sema* sema, u32 type_index)
{
    u32 int_bits = llvm_integer_bits(sema, type_index);
    if (int_bits > 0) {
        return int_bits;
    }
    u32 float_bits = llvm_float_bits(sema, type_index);
    if (float_bits > 0) {
        return float_bits;
    }
    if (llvm_type_kind(sema, type_index) == STK_Pointer ||
        llvm_type_kind(sema, type_index) == STK_Function) {
        return 64;
    }
    if (llvm_type_kind(sema, type_index) == STK_String ||
        llvm_type_kind(sema, type_index) == STK_Slice) {
        return 128;
    }
    if (llvm_type_kind(sema, type_index) == STK_Tuple ||
        llvm_type_kind(sema, type_index) == STK_Plex) {
        const SemaType* type = &sema->types[type_index];
        u32             bits = 0;
        for (u32 i = 0; i < type->param_count; ++i) {
            bits += llvm_type_storage_bits(
                sema, sema->type_param_types[type->first_param_type + i]);
        }
        return bits;
    }
    if (llvm_type_kind(sema, type_index) == STK_Array) {
        const SemaType* type = &sema->types[type_index];
        return (u32)type->param_count *
               llvm_type_storage_bits(sema, type->first_param_type);
    }
    if (llvm_type_kind(sema, type_index) == STK_Union) {
        return llvm_union_storage_bits(sema, type_index);
    }
    return 0;
}

internal u32 llvm_union_storage_bits(const Sema* sema, u32 union_type)
{
    if (sema == NULL || union_type >= array_count(sema->types) ||
        sema->types[union_type].kind != STK_Union) {
        return 0;
    }

    const SemaType* type = &sema->types[union_type];
    u32             bits = 8;
    for (u32 i = 0; i < type->param_count; ++i) {
        u32 field_type = sema->type_param_types[type->first_param_type + i];
        u32 field_bits = llvm_type_storage_bits(sema, field_type);
        if (field_bits > bits) {
            bits = field_bits;
        }
    }
    return bits;
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
    if (kind == STK_String) {
        for (u32 i = 0; i < array_count(sema->types); ++i) {
            if (sema->types[i].kind == STK_U8) {
                return i;
            }
        }
        return sema_no_type();
    }
    if (kind == STK_Array || kind == STK_Slice || kind == STK_Pointer ||
        kind == STK_DynamicArray) {
        return sema->types[type_index].first_param_type;
    }
    return sema_no_type();
}

internal string llvm_dynamic_array_header_type(void)
{
    return s("{ ptr, i64, i64 }");
}

internal bool llvm_type_is_record(const Sema* sema, u32 type_index)
{
    SemaTypeKind kind = llvm_type_kind(sema, type_index);
    return kind == STK_Tuple || kind == STK_Plex || kind == STK_Union ||
           kind == STK_String;
}

internal u32 llvm_record_field_count(const Sema* sema, u32 type_index)
{
    if (llvm_type_kind(sema, type_index) == STK_String) {
        return 2;
    }
    if (!llvm_type_is_record(sema, type_index)) {
        return 0;
    }
    return sema->types[type_index].param_count;
}

internal u32 llvm_record_field_type(const Sema* sema,
                                    u32         type_index,
                                    u32         field_index)
{
    if (llvm_type_kind(sema, type_index) == STK_String) {
        SemaTypeKind kind = field_index == 0 ? STK_Pointer : STK_Usize;
        for (u32 i = 0; i < array_count(sema->types); ++i) {
            if (sema->types[i].kind == kind) {
                return i;
            }
        }
        return sema_no_type();
    }
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

internal u32 llvm_enum_storage_payload_bits(const Sema* sema, u32 enum_type)
{
    if (sema == NULL || enum_type == sema_no_type() ||
        enum_type >= array_count(sema->types) ||
        sema->types[enum_type].kind != STK_Enum) {
        return 8;
    }

    const SemaType* type = &sema->types[enum_type];
    u32             bits = 8;
    for (u32 i = 0; i < type->param_count; ++i) {
        u32 payload_type = llvm_enum_variant_payload_type(sema, enum_type, i);
        u32 payload_bits = llvm_type_storage_bits(sema, payload_type);
        if (payload_bits > bits) {
            bits = payload_bits;
        }
    }
    return bits;
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
    case STK_String:
        sb_append_cstr(sb, "{ ptr, i64 }");
        break;
    case STK_Function:
    case STK_Pointer:
        sb_append_cstr(sb, "ptr");
        break;
    case STK_Slice:
        sb_append_cstr(sb, "{ ptr, i64 }");
        break;
    case STK_Enum:
        {
            u32 payload_bits = llvm_enum_storage_payload_bits(sema, type_index);
            sb_append_cstr(sb, "{ i64, ");
            sb_format(sb, "i%u", payload_bits);
            sb_append_cstr(sb, " }");
            break;
        }
    case STK_Union:
        sb_format(sb, "i%u", llvm_union_storage_bits(sema, type_index));
        break;
    case STK_DynamicArray:
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

internal void llvm_append_c_symbol_name(StringBuilder* sb, string name)
{
    sb_append_char(sb, '@');
    for (usize i = 0; i < name.count; ++i) {
        u8 ch = name.data[i];
        bool simple = (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
                      (ch >= '0' && ch <= '9') || ch == '_' || ch == '$' ||
                      ch == '.';
        sb_append_char(sb, simple ? (char)ch : '_');
    }
}

internal u32 llvm_function_symbol_handle(const Hir* hir, u32 function_index)
{
    for (u32 i = 0; i < array_count(hir->bindings); ++i) {
        const HirBinding* binding = &hir->bindings[i];
        if (binding->kind == HIR_BINDING_Function &&
            binding->target_index == function_index) {
            return binding->symbol_handle;
        }
    }
    return U32_MAX;
}

internal void llvm_append_generated_function_name(StringBuilder* sb,
                                                  const Hir*     hir,
                                                  u32            function_index)
{
    u32 module_index = hir != NULL ? hir->current_module_index : 0;
    if (module_index == 0 || module_index == U32_MAX) {
        sb_format(sb, "@fn.%u", function_index);
    } else {
        sb_format(sb, "@m%u.fn.%u", module_index, function_index);
    }
}

internal void llvm_append_function_name(StringBuilder* sb,
                                        const Hir*     hir,
                                        const Lexer*   lexer,
                                        u32            function_index)
{
    if (hir != NULL && function_index < array_count(hir->functions) &&
        hir->functions[function_index].kind == HIR_FUNCTION_Ffi) {
        u32 symbol_handle = llvm_function_symbol_handle(hir, function_index);
        if (symbol_handle != U32_MAX) {
            llvm_append_c_symbol_name(sb, lex_symbol(lexer, symbol_handle));
            return;
        }
    }
    llvm_append_generated_function_name(sb, hir, function_index);
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

internal void llvm_append_module_init_name(StringBuilder* sb, const Hir* hir)
{
    u32 module_index = hir != NULL ? hir->current_module_index : 0;
    if (module_index == U32_MAX) {
        module_index = 0;
    }
    sb_format(sb, "@m%u.init", module_index);
}

internal void llvm_append_string_global_name(StringBuilder* sb,
                                            const Hir*     hir,
                                            u32            string_index)
{
    u32 module_index = hir != NULL ? hir->current_module_index : 0;
    if (module_index == U32_MAX) {
        module_index = 0;
    }
    sb_format(sb, "@.str.m%u.%u", module_index, string_index);
}

internal void llvm_append_concat_string_global_name(StringBuilder* sb,
                                                   const Hir*     hir,
                                                   u32            expr_index)
{
    u32 module_index = hir != NULL ? hir->current_module_index : 0;
    if (module_index == U32_MAX) {
        module_index = 0;
    }
    sb_format(sb, "@.str.m%u.concat.%u", module_index, expr_index);
}

internal void llvm_append_source_path_global_name(StringBuilder* sb,
                                                 const Hir*     hir)
{
    u32 module_index = hir != NULL ? hir->current_module_index : 0;
    if (module_index == U32_MAX) {
        module_index = 0;
    }
    sb_format(sb, "@.source_path.m%u", module_index);
}

internal void llvm_append_assert_default_message_global_name(StringBuilder* sb,
                                                            const Hir*     hir)
{
    u32 module_index = hir != NULL ? hir->current_module_index : 0;
    if (module_index == U32_MAX) {
        module_index = 0;
    }
    sb_format(sb, "@.assert.default.m%u", module_index);
}

internal void llvm_append_global_slice_backing_name(StringBuilder* sb,
                                                   const Hir*     hir,
                                                   u32            value_index)
{
    u32 module_index = hir != NULL ? hir->current_module_index : 0;
    if (module_index == U32_MAX) {
        module_index = 0;
    }
    sb_format(sb, "@.slice.literal.m%u.%u", module_index, value_index);
}

internal string llvm_string_global_name_string(const Hir* hir,
                                               Arena*     arena,
                                               u32        string_index)
{
    StringBuilder sb = {0};
    sb_init(&sb, arena);
    llvm_append_string_global_name(&sb, hir, string_index);
    return sb_to_string(&sb);
}

internal string llvm_concat_string_global_name_string(const Hir* hir,
                                                     Arena*     arena,
                                                     u32        expr_index)
{
    StringBuilder sb = {0};
    sb_init(&sb, arena);
    llvm_append_concat_string_global_name(&sb, hir, expr_index);
    return sb_to_string(&sb);
}

internal string llvm_source_path_global_name_string(const Hir* hir, Arena* arena)
{
    StringBuilder sb = {0};
    sb_init(&sb, arena);
    llvm_append_source_path_global_name(&sb, hir);
    return sb_to_string(&sb);
}

internal string llvm_assert_default_message_global_name_string(const Hir* hir,
                                                              Arena*     arena)
{
    StringBuilder sb = {0};
    sb_init(&sb, arena);
    llvm_append_assert_default_message_global_name(&sb, hir);
    return sb_to_string(&sb);
}

internal string llvm_global_slice_backing_name_string(const Hir* hir,
                                                     Arena*     arena,
                                                     u32        value_index)
{
    StringBuilder sb = {0};
    sb_init(&sb, arena);
    llvm_append_global_slice_backing_name(&sb, hir, value_index);
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

internal u32 llvm_value_symbol_handle(const Hir* hir, u32 value_index)
{
    for (u32 i = 0; i < array_count(hir->bindings); ++i) {
        const HirBinding* binding = &hir->bindings[i];
        if (binding->kind == HIR_BINDING_Value &&
            binding->target_index == value_index) {
            return binding->symbol_handle;
        }
    }
    return U32_MAX;
}

internal string llvm_value_name_string(const Hir*   hir,
                                       const Lexer* lexer,
                                       Arena*       arena,
                                       u32          value_index)
{
    u32 symbol_handle = llvm_value_symbol_handle(hir, value_index);
    if (symbol_handle == U32_MAX) {
        return (string){0};
    }
    return llvm_symbol_name_string(lexer, arena, symbol_handle);
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
            string param_name = lex_symbol(lexer, param->symbol_handle);
            sb_append_cstr(sb, " %");
            sb_append_string(sb, param_name);
            if (string_eq_cstr(param_name, "_")) {
                sb_format(sb, ".%u", i);
            }
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

internal void llvm_append_zero_value(StringBuilder* sb,
                                     const Sema*    sema,
                                     u32            type_index)
{
    if (sema == NULL || type_index >= array_count(sema->types)) {
        sb_append_cstr(sb, "null");
        return;
    }

    switch (sema->types[type_index].kind) {
    case STK_F32:
    case STK_F64:
    case STK_UntypedFloat:
        sb_append_cstr(sb, "0.000000e+00");
        break;
    case STK_Function:
    case STK_Pointer:
    case STK_DynamicArray:
    case STK_Module:
        sb_append_cstr(sb, "null");
        break;
    case STK_Union:
        sb_append_cstr(sb, "0");
        break;
    case STK_Enum:
    case STK_Plex:
    case STK_Tuple:
    case STK_Array:
    case STK_Slice:
    case STK_String:
        sb_append_cstr(sb, "zeroinitializer");
        break;
    default:
        sb_append_cstr(sb, "0");
        break;
    }
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

    llvm_append_zero_value(sb, sema, return_type);
    sb_append_char(sb, '\n');
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
    u32    symbol_handle;
    string break_label;
    string continue_label;
    string break_value_ptr;
    u32    break_value_type;
} LlvmControlTarget;

typedef struct {
    StringBuilder*        sb;
    const Hir*            hir;
    const Lexer*          lexer;
    const Sema*           sema;
    Arena*                arena;
    u32                   next_temp;
    u32                   next_label;
    bool                  block_terminated;
    bool                  emitted_break;
    string                break_label;
    string                continue_label;
    string                break_value_ptr;
    u32                   break_value_type;
    u32                   global_init_value_index;
    Array(LlvmLocalValue) locals;
    Array(LlvmLocalSlot)  slots;
    Array(u32)            assigned_locals;
    Array(LlvmControlTarget) control_targets;
} LlvmFunctionContext;

internal string llvm_temp(LlvmFunctionContext* ctx)
{
    return string_format(ctx->arena, "%%t%u", ctx->next_temp++);
}

internal string llvm_label(LlvmFunctionContext* ctx, cstr prefix)
{
    return string_format(ctx->arena, "%s.%u", prefix, ctx->next_label++);
}

internal LlvmControlTarget* llvm_find_control_target(LlvmFunctionContext* ctx,
                                                     u32 symbol_handle)
{
    if (symbol_handle == U32_MAX) {
        return NULL;
    }

    for (u32 i = array_count(ctx->control_targets); i > 0; --i) {
        LlvmControlTarget* target = &ctx->control_targets[i - 1];
        if (target->symbol_handle == symbol_handle) {
            return target;
        }
    }
    return NULL;
}

internal void llvm_push_control_target(LlvmFunctionContext* ctx,
                                       LlvmControlTarget    target)
{
    if (target.symbol_handle != U32_MAX) {
        array_push(ctx->control_targets, target);
    }
}

internal void llvm_pop_control_target(LlvmFunctionContext* ctx,
                                      u32                  symbol_handle)
{
    if (symbol_handle != U32_MAX && array_count(ctx->control_targets) > 0) {
        array_pop(ctx->control_targets);
    }
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
        string name = lex_symbol(lexer, param->symbol_handle);
        if (string_eq_cstr(name, "_")) {
            return string_format(arena, "%%_.%u", i);
        }
        return string_format(
            arena, "%%%.*s", (int)name.count, name.data);
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
    if (kind == STK_Pointer || kind == STK_DynamicArray || kind == STK_Nil) {
        return (LlvmValue){
            .ok         = true,
            .type_index = type_index,
            .value      = s("null"),
        };
    }
    if (kind == STK_Enum || kind == STK_Tuple || kind == STK_Plex ||
        kind == STK_Array || kind == STK_Slice || kind == STK_String) {
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

internal LlvmValue llvm_coerce_value_to_type(LlvmFunctionContext* ctx,
                                             LlvmValue            value,
                                             u32                  target_type)
{
    if (!value.ok || target_type == sema_no_type()) {
        return value;
    }

    if (value.type_index == target_type) {
        return value;
    }

    SemaTypeKind target_kind = llvm_type_kind(ctx->sema, target_type);
    SemaTypeKind source_kind = llvm_type_kind(ctx->sema, value.type_index);
    if ((target_kind == STK_Pointer || target_kind == STK_DynamicArray) &&
        (source_kind == STK_Nil || source_kind == STK_UntypedInteger) &&
        string_eq_cstr(value.value, "0")) {
        value.type_index = target_type;
        value.value      = s("null");
        return value;
    }

    if ((target_kind == STK_Pointer || target_kind == STK_DynamicArray) &&
        llvm_integer_bits(ctx->sema, value.type_index) > 0) {
        string source_type = llvm_type_string(ctx, value.type_index);
        string target_type_string = llvm_type_string(ctx, target_type);
        string temp = llvm_temp(ctx);
        sb_format(ctx->sb,
                  "  " STRINGP " = inttoptr " STRINGP " " STRINGP
                  " to " STRINGP "\n",
                  STRINGV(temp),
                  STRINGV(source_type),
                  STRINGV(value.value),
                  STRINGV(target_type_string));
        return (LlvmValue){
            .ok         = true,
            .type_index = target_type,
            .value      = temp,
        };
    }

    value.type_index = target_type;
    return value;
}

internal LlvmValue llvm_cast_to_union_storage(LlvmFunctionContext* ctx,
                                              LlvmValue            value,
                                              u32                  union_type)
{
    u32 storage_bits = llvm_union_storage_bits(ctx->sema, union_type);
    if (storage_bits == 0) {
        return (LlvmValue){0};
    }

    string storage_type = string_format(ctx->arena, "i%u", storage_bits);
    u32    value_bits   = llvm_type_storage_bits(ctx->sema, value.type_index);
    if (value_bits == storage_bits &&
        llvm_integer_bits(ctx->sema, value.type_index) > 0) {
        value.type_index = union_type;
        return value;
    }

    string value_type = llvm_type_string(ctx, value.type_index);
    string temp       = llvm_temp(ctx);
    if (llvm_float_bits(ctx->sema, value.type_index) == storage_bits) {
        sb_format(ctx->sb,
                  "  " STRINGP " = bitcast " STRINGP " " STRINGP
                  " to " STRINGP "\n",
                  STRINGV(temp),
                  STRINGV(value_type),
                  STRINGV(value.value),
                  STRINGV(storage_type));
    } else if ((llvm_type_kind(ctx->sema, value.type_index) == STK_Pointer ||
                llvm_type_kind(ctx->sema, value.type_index) == STK_Function) &&
               value_bits == storage_bits) {
        sb_format(ctx->sb,
                  "  " STRINGP " = ptrtoint " STRINGP " " STRINGP
                  " to " STRINGP "\n",
                  STRINGV(temp),
                  STRINGV(value_type),
                  STRINGV(value.value),
                  STRINGV(storage_type));
    } else if (value_bits < storage_bits &&
               llvm_integer_bits(ctx->sema, value.type_index) > 0) {
        sb_format(ctx->sb,
                  "  " STRINGP " = zext " STRINGP " " STRINGP
                  " to " STRINGP "\n",
                  STRINGV(temp),
                  STRINGV(value_type),
                  STRINGV(value.value),
                  STRINGV(storage_type));
    } else if (value_bits > storage_bits &&
               llvm_integer_bits(ctx->sema, value.type_index) > 0) {
        sb_format(ctx->sb,
                  "  " STRINGP " = trunc " STRINGP " " STRINGP
                  " to " STRINGP "\n",
                  STRINGV(temp),
                  STRINGV(value_type),
                  STRINGV(value.value),
                  STRINGV(storage_type));
    } else {
        return (LlvmValue){0};
    }

    return (LlvmValue){
        .ok         = true,
        .type_index = union_type,
        .value      = temp,
    };
}

internal LlvmValue llvm_cast_from_union_storage(LlvmFunctionContext* ctx,
                                                LlvmValue            value,
                                                u32                  field_type)
{
    u32 storage_bits = llvm_union_storage_bits(ctx->sema, value.type_index);
    u32 field_bits   = llvm_type_storage_bits(ctx->sema, field_type);
    if (storage_bits == 0 || field_bits == 0) {
        return (LlvmValue){0};
    }

    if (llvm_integer_bits(ctx->sema, field_type) == storage_bits) {
        return (LlvmValue){
            .ok         = true,
            .type_index = field_type,
            .value      = value.value,
        };
    }

    string storage_type = string_format(ctx->arena, "i%u", storage_bits);
    string field_type_s = llvm_type_string(ctx, field_type);
    string temp         = llvm_temp(ctx);
    if (llvm_float_bits(ctx->sema, field_type) == storage_bits) {
        sb_format(ctx->sb,
                  "  " STRINGP " = bitcast " STRINGP " " STRINGP
                  " to " STRINGP "\n",
                  STRINGV(temp),
                  STRINGV(storage_type),
                  STRINGV(value.value),
                  STRINGV(field_type_s));
    } else if ((llvm_type_kind(ctx->sema, field_type) == STK_Pointer ||
                llvm_type_kind(ctx->sema, field_type) == STK_Function) &&
               field_bits == storage_bits) {
        sb_format(ctx->sb,
                  "  " STRINGP " = inttoptr " STRINGP " " STRINGP
                  " to " STRINGP "\n",
                  STRINGV(temp),
                  STRINGV(storage_type),
                  STRINGV(value.value),
                  STRINGV(field_type_s));
    } else if (llvm_integer_bits(ctx->sema, field_type) > 0 &&
               field_bits < storage_bits) {
        sb_format(ctx->sb,
                  "  " STRINGP " = trunc " STRINGP " " STRINGP
                  " to " STRINGP "\n",
                  STRINGV(temp),
                  STRINGV(storage_type),
                  STRINGV(value.value),
                  STRINGV(field_type_s));
    } else if (llvm_integer_bits(ctx->sema, field_type) > 0 &&
               field_bits > storage_bits) {
        sb_format(ctx->sb,
                  "  " STRINGP " = zext " STRINGP " " STRINGP
                  " to " STRINGP "\n",
                  STRINGV(temp),
                  STRINGV(storage_type),
                  STRINGV(value.value),
                  STRINGV(field_type_s));
    } else {
        return (LlvmValue){0};
    }

    return (LlvmValue){
        .ok         = true,
        .type_index = field_type,
        .value      = temp,
    };
}

internal LlvmValue llvm_cast_to_storage_bits(LlvmFunctionContext* ctx,
                                             LlvmValue            value,
                                             u32                  storage_bits)
{
    if (storage_bits == 0) {
        return (LlvmValue){0};
    }

    string storage_type = string_format(ctx->arena, "i%u", storage_bits);
    u32    value_bits   = llvm_type_storage_bits(ctx->sema, value.type_index);
    if (value_bits == storage_bits &&
        llvm_integer_bits(ctx->sema, value.type_index) > 0) {
        value.type_index = sema_no_type();
        return value;
    }

    string value_type = llvm_type_string(ctx, value.type_index);
    string temp       = llvm_temp(ctx);
    if (llvm_float_bits(ctx->sema, value.type_index) == storage_bits) {
        sb_format(ctx->sb,
                  "  " STRINGP " = bitcast " STRINGP " " STRINGP
                  " to " STRINGP "\n",
                  STRINGV(temp),
                  STRINGV(value_type),
                  STRINGV(value.value),
                  STRINGV(storage_type));
    } else if ((llvm_type_kind(ctx->sema, value.type_index) == STK_Pointer ||
                llvm_type_kind(ctx->sema, value.type_index) == STK_Function) &&
               value_bits == storage_bits) {
        sb_format(ctx->sb,
                  "  " STRINGP " = ptrtoint " STRINGP " " STRINGP
                  " to " STRINGP "\n",
                  STRINGV(temp),
                  STRINGV(value_type),
                  STRINGV(value.value),
                  STRINGV(storage_type));
    } else if (llvm_integer_bits(ctx->sema, value.type_index) > 0 &&
               value_bits < storage_bits) {
        sb_format(ctx->sb,
                  "  " STRINGP " = zext " STRINGP " " STRINGP
                  " to " STRINGP "\n",
                  STRINGV(temp),
                  STRINGV(value_type),
                  STRINGV(value.value),
                  STRINGV(storage_type));
    } else if (llvm_integer_bits(ctx->sema, value.type_index) > 0 &&
               value_bits > storage_bits) {
        sb_format(ctx->sb,
                  "  " STRINGP " = trunc " STRINGP " " STRINGP
                  " to " STRINGP "\n",
                  STRINGV(temp),
                  STRINGV(value_type),
                  STRINGV(value.value),
                  STRINGV(storage_type));
    } else if (value_bits > 0 && value_bits <= storage_bits) {
        string slot = llvm_temp(ctx);
        sb_format(ctx->sb,
                  "  " STRINGP " = alloca " STRINGP "\n"
                  "  store " STRINGP " 0, ptr " STRINGP "\n"
                  "  store " STRINGP " " STRINGP ", ptr " STRINGP "\n"
                  "  " STRINGP " = load " STRINGP ", ptr " STRINGP "\n",
                  STRINGV(slot),
                  STRINGV(storage_type),
                  STRINGV(storage_type),
                  STRINGV(slot),
                  STRINGV(value_type),
                  STRINGV(value.value),
                  STRINGV(slot),
                  STRINGV(temp),
                  STRINGV(storage_type),
                  STRINGV(slot));
    } else {
        return (LlvmValue){0};
    }

    return (LlvmValue){
        .ok         = true,
        .type_index = sema_no_type(),
        .value      = temp,
    };
}

internal LlvmValue llvm_cast_from_storage_bits(LlvmFunctionContext* ctx,
                                               LlvmValue            value,
                                               u32                  storage_bits,
                                               u32                  result_type)
{
    u32 result_bits = llvm_type_storage_bits(ctx->sema, result_type);
    if (storage_bits == 0 || result_bits == 0) {
        return (LlvmValue){0};
    }

    if (llvm_integer_bits(ctx->sema, result_type) == storage_bits) {
        return (LlvmValue){
            .ok         = true,
            .type_index = result_type,
            .value      = value.value,
        };
    }

    string storage_type = string_format(ctx->arena, "i%u", storage_bits);
    string result_type_s = llvm_type_string(ctx, result_type);
    string temp          = llvm_temp(ctx);
    if (llvm_float_bits(ctx->sema, result_type) == storage_bits) {
        sb_format(ctx->sb,
                  "  " STRINGP " = bitcast " STRINGP " " STRINGP
                  " to " STRINGP "\n",
                  STRINGV(temp),
                  STRINGV(storage_type),
                  STRINGV(value.value),
                  STRINGV(result_type_s));
    } else if ((llvm_type_kind(ctx->sema, result_type) == STK_Pointer ||
                llvm_type_kind(ctx->sema, result_type) == STK_Function) &&
               result_bits == storage_bits) {
        sb_format(ctx->sb,
                  "  " STRINGP " = inttoptr " STRINGP " " STRINGP
                  " to " STRINGP "\n",
                  STRINGV(temp),
                  STRINGV(storage_type),
                  STRINGV(value.value),
                  STRINGV(result_type_s));
    } else if (llvm_integer_bits(ctx->sema, result_type) > 0 &&
               result_bits < storage_bits) {
        sb_format(ctx->sb,
                  "  " STRINGP " = trunc " STRINGP " " STRINGP
                  " to " STRINGP "\n",
                  STRINGV(temp),
                  STRINGV(storage_type),
                  STRINGV(value.value),
                  STRINGV(result_type_s));
    } else if (llvm_integer_bits(ctx->sema, result_type) > 0 &&
               result_bits > storage_bits) {
        sb_format(ctx->sb,
                  "  " STRINGP " = zext " STRINGP " " STRINGP
                  " to " STRINGP "\n",
                  STRINGV(temp),
                  STRINGV(storage_type),
                  STRINGV(value.value),
                  STRINGV(result_type_s));
    } else if (result_bits <= storage_bits) {
        string slot = llvm_temp(ctx);
        sb_format(ctx->sb,
                  "  " STRINGP " = alloca " STRINGP "\n"
                  "  store " STRINGP " " STRINGP ", ptr " STRINGP "\n"
                  "  " STRINGP " = load " STRINGP ", ptr " STRINGP "\n",
                  STRINGV(slot),
                  STRINGV(storage_type),
                  STRINGV(storage_type),
                  STRINGV(value.value),
                  STRINGV(slot),
                  STRINGV(temp),
                  STRINGV(result_type_s),
                  STRINGV(slot));
    } else {
        return (LlvmValue){0};
    }

    return (LlvmValue){
        .ok         = true,
        .type_index = result_type,
        .value      = temp,
    };
}

internal LlvmValue llvm_build_aggregate_value(LlvmFunctionContext* ctx,
                                              u32                  type_index,
                                              const LlvmValue*     values,
                                              u32                  value_count)
{
    string aggregate_type = llvm_type_string(ctx, type_index);
    string current        = value_count == 0 ? s("zeroinitializer") : s("poison");
    for (u32 i = 0; i < value_count; ++i) {
        string temp       = llvm_temp(ctx);
        string value_type = llvm_type_string(ctx, values[i].type_index);
        sb_format(ctx->sb,
                  "  " STRINGP " = insertvalue " STRINGP " " STRINGP ", "
                  STRINGP " " STRINGP ", %u\n",
                  STRINGV(temp),
                  STRINGV(aggregate_type),
                  STRINGV(current),
                  STRINGV(value_type),
                  STRINGV(values[i].value),
                  i);
        current = temp;
    }

    return (LlvmValue){
        .ok         = true,
        .type_index = type_index,
        .value      = current,
    };
}

internal LlvmValue llvm_emit_expr(LlvmFunctionContext* ctx,
                                  const HirFunction*   function,
                                  u32                  expr_index);

internal LlvmValue llvm_address_of_expr(LlvmFunctionContext* ctx,
                                        const HirFunction*   function,
                                        u32                  expr_index);

internal string llvm_dynamic_array_load_header_field(LlvmFunctionContext* ctx,
                                                     string               header,
                                                     u32                  field_index,
                                                     string               type);

internal u64 llvm_type_storage_bytes(const Sema* sema, u32 type_index);

internal string llvm_dynamic_array_header_field_ptr(LlvmFunctionContext* ctx,
                                                    string               header,
                                                    u32                  field_index);

internal LlvmValue llvm_emit_dynamic_array_field(LlvmFunctionContext* ctx,
                                                 LlvmValue            target,
                                                 const HirExpr*       expr);

internal bool llvm_dynamic_array_callee_method(LlvmFunctionContext* ctx,
                                               u32                  callee_expr_index,
                                               u32*                 receiver_expr_index,
                                               string*              method);

internal LlvmValue llvm_emit_dynamic_array_push(LlvmFunctionContext* ctx,
                                                const HirFunction*   function,
                                                const HirExpr*       call,
                                                u32                  receiver_expr_index);

internal LlvmValue llvm_emit_dynamic_array_reserve(LlvmFunctionContext* ctx,
                                                   const HirFunction*   function,
                                                   const HirExpr*       call,
                                                   u32 receiver_expr_index);

internal LlvmValue llvm_emit_dynamic_array_clear(LlvmFunctionContext* ctx,
                                                 const HirFunction*   function,
                                                 const HirExpr*       call,
                                                 u32 receiver_expr_index);

internal LlvmValue llvm_emit_dynamic_array_free(LlvmFunctionContext* ctx,
                                                const HirFunction*   function,
                                                const HirExpr*       call,
                                                u32 receiver_expr_index);

internal LlvmValue llvm_emit_dynamic_array_pop(LlvmFunctionContext* ctx,
                                               const HirFunction*   function,
                                               const HirExpr*       call,
                                               u32 receiver_expr_index);

internal LlvmValue llvm_emit_dynamic_array_delete(LlvmFunctionContext* ctx,
                                                  const HirFunction*   function,
                                                  const HirExpr*       call,
                                                  u32 receiver_expr_index,
                                                  bool                 swap);

internal LlvmValue llvm_emit_dynamic_array_append(LlvmFunctionContext* ctx,
                                                  const HirFunction*   function,
                                                  const HirExpr*       call,
                                                  u32 receiver_expr_index);

internal bool llvm_emit_assign(LlvmFunctionContext* ctx,
                               const HirFunction*   function,
                               u32                  target_expr_index,
                               LlvmValue            value);

internal u32 llvm_field_index_for_value(LlvmFunctionContext* ctx,
                                        const HirExpr*       field_expr,
                                        LlvmValue            target)
{
    if (field_expr == NULL) {
        return U32_MAX;
    }
    if (field_expr->kind == HIR_EXPR_TupleField) {
        return (u32)field_expr->integer;
    }
    if (field_expr->kind != HIR_EXPR_Field) {
        return U32_MAX;
    }

    SemaTypeKind target_kind = llvm_type_kind(ctx->sema, target.type_index);
    if (target_kind == STK_String || target_kind == STK_Slice) {
        string field = lex_symbol(ctx->lexer, field_expr->symbol_handle);
        if (string_eq_cstr(field, "data")) {
            return 0;
        }
        if (string_eq_cstr(field, "count")) {
            return 1;
        }
        return U32_MAX;
    }

    return llvm_record_field_index(
        ctx->sema, target.type_index, field_expr->symbol_handle);
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

internal bool llvm_emit_index_i64(LlvmFunctionContext* ctx,
                                  const HirFunction*   function,
                                  u32                  expr_index,
                                  string*              out)
{
    if (expr_index >= array_count(ctx->hir->exprs)) {
        return false;
    }

    LlvmValue value = llvm_emit_expr(ctx, function, expr_index);
    if (!value.ok) {
        return false;
    }

    u32 bits = llvm_integer_bits(ctx->sema, value.type_index);
    if (bits == 0) {
        return false;
    }

    if (bits == 64) {
        *out = value.value;
        return true;
    }

    string source_type = llvm_type_string(ctx, value.type_index);
    string instr = bits > 64 ? s("trunc")
                             : ((llvm_type_kind(ctx->sema, value.type_index) ==
                                     STK_Bool ||
                                 llvm_type_is_unsigned_integer(
                                     ctx->sema, value.type_index))
                                    ? s("zext")
                                    : s("sext"));
    string temp = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = " STRINGP " " STRINGP " " STRINGP
              " to i64\n",
              STRINGV(temp),
              STRINGV(instr),
              STRINGV(source_type),
              STRINGV(value.value));
    *out = temp;
    return true;
}

internal string llvm_slice_count_i64(LlvmFunctionContext* ctx,
                                     string               start,
                                     bool                 start_is_constant,
                                     i64                  start_value,
                                     string               end,
                                     bool                 end_is_constant,
                                     i64                  end_value)
{
    if (start_is_constant && end_is_constant) {
        return string_format(ctx->arena,
                             "%lld",
                             (long long)(end_value - start_value));
    }

    if (start_is_constant && start_value == 0) {
        return end;
    }

    string count = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = sub i64 " STRINGP ", " STRINGP "\n",
              STRINGV(count),
              STRINGV(end),
              STRINGV(start));
    return count;
}

internal bool llvm_eval_hir_string_constant(const Hir*   hir,
                                            const Lexer* lexer,
                                            Arena*       arena,
                                            u32          expr_index,
                                            string*      out);

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

    u32    enum_type           = expr->type_index;
    u32    storage_payload_bits =
        llvm_enum_storage_payload_bits(ctx->sema, enum_type);
    string enum_type_string = llvm_type_string(ctx, enum_type);
    string payload_type_string =
        string_format(ctx->arena, "i%u", storage_payload_bits);
    string payload_value = s("0");

    u32 variant_payload_type = llvm_enum_variant_payload_type(ctx->sema,
                                                              enum_type,
                                                              variant_index);
    if (variant_payload_type != sema_no_type() && expr->arg_count > 0) {
        LlvmValue payload = {0};
        if (expr->arg_count == 1 && args[0].type_index == variant_payload_type) {
            payload = args[0];
        } else if (llvm_type_is_record(ctx->sema, variant_payload_type)) {
            string variant_payload_type_string =
                llvm_type_string(ctx, variant_payload_type);
            string aggregate = s("poison");
            for (u32 i = 0; i < expr->arg_count; ++i) {
                LlvmValue arg  = args[i];
                string    temp = llvm_temp(ctx);
                string    arg_type = llvm_type_string(ctx, arg.type_index);
                sb_format(ctx->sb,
                          "  " STRINGP " = insertvalue " STRINGP " "
                          STRINGP ", " STRINGP " " STRINGP ", %u\n",
                          STRINGV(temp),
                          STRINGV(variant_payload_type_string),
                          STRINGV(aggregate),
                          STRINGV(arg_type),
                          STRINGV(arg.value),
                          i);
                aggregate = temp;
            }
            payload = (LlvmValue){
                .ok         = true,
                .type_index = variant_payload_type,
                .value      = aggregate,
            };
        } else {
            payload = args[0];
        }

        payload = llvm_cast_to_storage_bits(ctx, payload, storage_payload_bits);
        if (!payload.ok) {
            array_free(args);
            return (LlvmValue){0};
        }
        payload_value = payload.value;
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

internal void llvm_bind_symbol_value(LlvmFunctionContext* ctx,
                                     u32                  symbol_handle,
                                     LlvmValue            value)
{
    if (ctx->sema == NULL || symbol_handle == U32_MAX) {
        return;
    }

    for (u32 local_index = 0; local_index < array_count(ctx->sema->locals);
         ++local_index) {
        if (ctx->sema->locals[local_index].symbol_handle != symbol_handle) {
            continue;
        }

        LlvmValue local_value = value;
        local_value.type_index = llvm_local_type(ctx, local_index);
        llvm_set_local_value(ctx, local_index, local_value);
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

    if (llvm_type_kind(ctx->sema, scrutinee.type_index) == STK_Enum &&
        (string_eq_cstr(pred, "eq") || string_eq_cstr(pred, "ne"))) {
        string enum_type = llvm_type_string(ctx, scrutinee.type_index);
        string lhs_tag   = llvm_temp(ctx);
        string rhs_tag   = llvm_temp(ctx);
        sb_format(ctx->sb,
                  "  " STRINGP " = extractvalue " STRINGP " " STRINGP
                  ", 0\n",
                  STRINGV(lhs_tag),
                  STRINGV(enum_type),
                  STRINGV(scrutinee.value));
        sb_format(ctx->sb,
                  "  " STRINGP " = extractvalue " STRINGP " " STRINGP
                  ", 0\n",
                  STRINGV(rhs_tag),
                  STRINGV(enum_type),
                  STRINGV(rhs.value));
        string temp = llvm_temp(ctx);
        sb_format(ctx->sb,
                  "  " STRINGP " = icmp " STRINGP " i64 " STRINGP ", "
                  STRINGP "\n",
                  STRINGV(temp),
                  STRINGV(pred),
                  STRINGV(lhs_tag),
                  STRINGV(rhs_tag));
        return (LlvmValue){
            .ok         = true,
            .type_index = llvm_builtin_type(ctx->sema, STK_Bool),
            .value      = temp,
        };
    }

    if (llvm_type_kind(ctx->sema, scrutinee.type_index) == STK_String &&
        (string_eq_cstr(pred, "eq") || string_eq_cstr(pred, "ne"))) {
        string equal = llvm_temp(ctx);
        sb_format(ctx->sb,
                  "  " STRINGP
                  " = call i1 @string_eq({ ptr, i64 } " STRINGP
                  ", { ptr, i64 } " STRINGP ")\n",
                  STRINGV(equal),
                  STRINGV(scrutinee.value),
                  STRINGV(rhs.value));
        if (string_eq_cstr(pred, "eq")) {
            return (LlvmValue){
                .ok         = true,
                .type_index = llvm_builtin_type(ctx->sema, STK_Bool),
                .value      = equal,
            };
        }

        string not_equal = llvm_temp(ctx);
        sb_format(ctx->sb,
                  "  " STRINGP " = xor i1 " STRINGP ", 1\n",
                  STRINGV(not_equal),
                  STRINGV(equal));
        return (LlvmValue){
            .ok         = true,
            .type_index = llvm_builtin_type(ctx->sema, STK_Bool),
            .value      = not_equal,
        };
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

            u32 storage_payload_bits =
                llvm_enum_storage_payload_bits(ctx->sema, scrutinee.type_index);
            u32 variant_payload_type =
                llvm_enum_variant_payload_type(ctx->sema,
                                               scrutinee.type_index,
                                               variant_index);
            if (variant_payload_type == sema_no_type()) {
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
            LlvmValue variant_payload = llvm_cast_from_storage_bits(
                ctx,
                (LlvmValue){
                    .ok         = true,
                    .type_index = sema_no_type(),
                    .value      = payload,
                },
                storage_payload_bits,
                variant_payload_type);
            if (!variant_payload.ok) {
                return (LlvmValue){0};
            }
            for (u32 i = 0; i < pattern->child_count; ++i) {
                u32 child_index = pattern->first_child + i;
                if (child_index >= array_count(ctx->hir->pattern_children)) {
                    return (LlvmValue){0};
                }

                u32 child_type = variant_payload_type;
                string child_value = variant_payload.value;
                if (payload_is_tuple) {
                    child_type = payload_is_tuple
                                     ? llvm_record_field_type(
                                           ctx->sema, variant_payload_type, i)
                                     : variant_payload_type;
                    string variant_payload_type_string =
                        llvm_type_string(ctx, variant_payload_type);
                    child_value = llvm_temp(ctx);
                    sb_format(ctx->sb,
                              "  " STRINGP " = extractvalue " STRINGP " "
                              STRINGP ", %u\n",
                              STRINGV(child_value),
                              STRINGV(variant_payload_type_string),
                              STRINGV(variant_payload.value),
                              i);
                }

	                const HirPatternChild* child =
	                    &ctx->hir->pattern_children[child_index];
	                if (!payload_is_tuple &&
	                    llvm_type_is_record(ctx->sema, variant_payload_type) &&
	                    child->pattern_index < array_count(ctx->hir->patterns)) {
	                    const HirPattern* child_pattern =
	                        &ctx->hir->patterns[child->pattern_index];
	                    u32 child_expr_index = child_pattern->expr_index;
	                    if ((child_pattern->kind == HIR_PATTERN_Value ||
	                         child_pattern->kind == HIR_PATTERN_Equal ||
	                         child_pattern->kind == HIR_PATTERN_NotEqual ||
	                         child_pattern->kind == HIR_PATTERN_Less ||
	                         child_pattern->kind == HIR_PATTERN_LessEqual ||
	                         child_pattern->kind == HIR_PATTERN_Greater ||
	                         child_pattern->kind == HIR_PATTERN_GreaterEqual) &&
	                        child_expr_index < array_count(ctx->hir->exprs)) {
	                        u32 pattern_value_type =
	                            ctx->hir->exprs[child_expr_index].type_index;
	                        if (pattern_value_type != sema_no_type() &&
	                            pattern_value_type != child_type) {
	                            u32 field_count = llvm_record_field_count(
	                                ctx->sema, variant_payload_type);
	                            for (u32 field_index = 0;
	                                 field_index < field_count;
	                                 ++field_index) {
	                                u32 field_type = llvm_record_field_type(
	                                    ctx->sema,
	                                    variant_payload_type,
	                                    field_index);
	                                if (field_type != pattern_value_type) {
	                                    continue;
	                                }

	                                string variant_payload_type_string =
	                                    llvm_type_string(ctx,
	                                                     variant_payload_type);
	                                child_value = llvm_temp(ctx);
	                                sb_format(ctx->sb,
	                                          "  " STRINGP
	                                          " = extractvalue " STRINGP " "
	                                          STRINGP ", %u\n",
	                                          STRINGV(child_value),
	                                          STRINGV(
	                                              variant_payload_type_string),
	                                          STRINGV(variant_payload.value),
	                                          field_index);
	                                child_type = field_type;
	                                break;
	                            }
	                        }
	                    }
	                }
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
    if (expr->kind == HIR_EXPR_Index) {
        if (expr->operand_expr_index >= array_count(ctx->hir->exprs)) {
            return (LlvmValue){0};
        }

        LlvmValue index = llvm_emit_expr(ctx, function, expr->extra_expr_index);
        if (!index.ok) {
            return (LlvmValue){0};
        }

        const HirExpr* target_expr = &ctx->hir->exprs[expr->operand_expr_index];
        u32 target_type = target_expr->type_index;
        u32 item_type   = sema_no_type();
        if (llvm_type_kind(ctx->sema, target_type) == STK_Array) {
            LlvmValue target_address =
                llvm_address_of_expr(ctx, function, expr->operand_expr_index);
            if (!target_address.ok) {
                return (LlvmValue){0};
            }

            item_type = llvm_collection_item_type(ctx->sema, target_type);
            string target_type_string = llvm_type_string(ctx, target_type);
            string index_type = llvm_type_string(ctx, index.type_index);
            string ptr        = llvm_temp(ctx);
            sb_format(ctx->sb,
                      "  " STRINGP " = getelementptr inbounds " STRINGP
                      ", ptr " STRINGP ", i64 0, " STRINGP " " STRINGP "\n",
                      STRINGV(ptr),
                      STRINGV(target_type_string),
                      STRINGV(target_address.value),
                      STRINGV(index_type),
                      STRINGV(index.value));
            return (LlvmValue){
                .ok         = item_type != sema_no_type(),
                .type_index = sema_no_type(),
                .value      = ptr,
            };
        }

        if (llvm_type_kind(ctx->sema, target_type) == STK_Slice ||
            llvm_type_kind(ctx->sema, target_type) == STK_String) {
            LlvmValue target =
                llvm_emit_expr(ctx, function, expr->operand_expr_index);
            if (!target.ok) {
                return (LlvmValue){0};
            }

            item_type = llvm_collection_item_type(ctx->sema, target_type);
            if (item_type == sema_no_type()) {
                return (LlvmValue){0};
            }

            string target_type_string = llvm_type_string(ctx, target_type);
            string data_ptr           = llvm_temp(ctx);
            sb_format(ctx->sb,
                      "  " STRINGP " = extractvalue " STRINGP " " STRINGP
                      ", 0\n",
                      STRINGV(data_ptr),
                      STRINGV(target_type_string),
                      STRINGV(target.value));

            string item_type_string = llvm_type_string(ctx, item_type);
            string index_type       = llvm_type_string(ctx, index.type_index);
            string ptr              = llvm_temp(ctx);
            sb_format(ctx->sb,
                      "  " STRINGP " = getelementptr inbounds " STRINGP
                      ", ptr " STRINGP ", " STRINGP " " STRINGP "\n",
                      STRINGV(ptr),
                      STRINGV(item_type_string),
                      STRINGV(data_ptr),
                      STRINGV(index_type),
                      STRINGV(index.value));
            return (LlvmValue){
                .ok         = true,
                .type_index = sema_no_type(),
                .value      = ptr,
            };
        }

        if (llvm_type_kind(ctx->sema, target_type) == STK_DynamicArray) {
            LlvmValue target =
                llvm_emit_expr(ctx, function, expr->operand_expr_index);
            if (!target.ok) {
                return (LlvmValue){0};
            }

            item_type = llvm_collection_item_type(ctx->sema, target_type);
            if (item_type == sema_no_type()) {
                return (LlvmValue){0};
            }

            string data_ptr = llvm_dynamic_array_load_header_field(
                ctx, target.value, 0, s("ptr"));
            string item_type_string = llvm_type_string(ctx, item_type);
            string index_type       = llvm_type_string(ctx, index.type_index);
            string ptr              = llvm_temp(ctx);
            sb_format(ctx->sb,
                      "  " STRINGP " = getelementptr inbounds " STRINGP
                      ", ptr " STRINGP ", " STRINGP " " STRINGP "\n",
                      STRINGV(ptr),
                      STRINGV(item_type_string),
                      STRINGV(data_ptr),
                      STRINGV(index_type),
                      STRINGV(index.value));
            return (LlvmValue){
                .ok         = true,
                .type_index = sema_no_type(),
                .value      = ptr,
            };
        }

        LlvmValue target =
            llvm_emit_expr(ctx, function, expr->operand_expr_index);
        if (!target.ok) {
            return (LlvmValue){0};
        }

        item_type = llvm_collection_item_type(ctx->sema, target_type);
        if (item_type == sema_no_type()) {
            item_type = llvm_pointee_type(ctx->sema, target_type);
        }
        if (item_type == sema_no_type()) {
            return (LlvmValue){0};
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
        return (LlvmValue){
            .ok         = true,
            .type_index = sema_no_type(),
            .value      = ptr,
        };
    }

    if (expr->kind == HIR_EXPR_LocalRef && expr->ref_kind == HIR_REF_Local) {
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

    LlvmValue value = llvm_emit_expr(ctx, function, expr_index);
    if (!value.ok) {
        return (LlvmValue){0};
    }

    string type = llvm_type_string(ctx, value.type_index);
    string ptr  = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = alloca " STRINGP "\n",
              STRINGV(ptr),
              STRINGV(type));
    sb_format(ctx->sb,
              "  store " STRINGP " " STRINGP ", ptr " STRINGP "\n",
              STRINGV(type),
              STRINGV(value.value),
              STRINGV(ptr));
    return (LlvmValue){
        .ok         = true,
        .type_index = sema_no_type(),
        .value      = ptr,
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

internal string llvm_float_binary_instruction(HirBinaryOp op)
{
    switch (op) {
    case HIR_BINARY_Add:
        return s("fadd");
    case HIR_BINARY_Subtract:
        return s("fsub");
    case HIR_BINARY_Multiply:
        return s("fmul");
    case HIR_BINARY_Divide:
        return s("fdiv");
    case HIR_BINARY_Modulo:
        return s("frem");
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

internal string llvm_float_compare_instruction(HirBinaryOp op)
{
    switch (op) {
    case HIR_BINARY_Equal:
        return s("oeq");
    case HIR_BINARY_NotEqual:
        return s("one");
    case HIR_BINARY_Less:
        return s("olt");
    case HIR_BINARY_LessEqual:
        return s("ole");
    case HIR_BINARY_Greater:
        return s("ogt");
    case HIR_BINARY_GreaterEqual:
        return s("oge");
    default:
        return (string){0};
    }
}

internal string llvm_float_literal_string(const Sema* sema,
                                          Arena*      arena,
                                          u32         type_index,
                                          f64         value)
{
    if (llvm_type_kind(sema, type_index) == STK_F32) {
        value = (f64)(f32)value;
    }
    union {
        f64 value;
        u64 bits;
    } repr = {.value = value};
    return string_format(arena,
                         "0x%016llX",
                         (unsigned long long)repr.bits);
}

internal string llvm_string_helper_suffix(const Sema* sema, u32 type_index)
{
    switch (llvm_type_kind(sema, type_index)) {
    case STK_UntypedInteger:
    case STK_I32:
        return s("i32");
    case STK_String:
        return s("string");
    case STK_Bool:
        return s("bool");
    case STK_I8:
        return s("i8");
    case STK_I16:
        return s("i16");
    case STK_I64:
        return s("i64");
    case STK_U8:
        return s("u8");
    case STK_U16:
        return s("u16");
    case STK_U32:
        return s("u32");
    case STK_U64:
        return s("u64");
    case STK_F32:
        return s("f32");
    case STK_F64:
    case STK_UntypedFloat:
        return s("f64");
    case STK_Isize:
        return s("isize");
    case STK_Usize:
        return s("usize");
    default:
        return (string){0};
    }
}

internal void llvm_emit_append_byte(LlvmFunctionContext* ctx, u8 byte)
{
    sb_format(ctx->sb,
              "  call void @string_builder_append_byte(i8 %u)\n",
              (u32)byte);
}

internal bool llvm_emit_append_string_value(LlvmFunctionContext* ctx,
                                            LlvmValue            value)
{
    SemaTypeKind kind = llvm_type_kind(ctx->sema, value.type_index);
    if (kind == STK_Tuple || kind == STK_Plex) {
        llvm_emit_append_byte(ctx, '(');
        u32 field_count = llvm_record_field_count(ctx->sema, value.type_index);
        string record_type = llvm_type_string(ctx, value.type_index);
        for (u32 i = 0; i < field_count; ++i) {
            if (i > 0) {
                llvm_emit_append_byte(ctx, ',');
                llvm_emit_append_byte(ctx, ' ');
            }

            u32 field_type = llvm_record_field_type(ctx->sema,
                                                    value.type_index,
                                                    i);
            string field = llvm_temp(ctx);
            sb_format(ctx->sb,
                      "  " STRINGP " = extractvalue " STRINGP " "
                      STRINGP ", %u\n",
                      STRINGV(field),
                      STRINGV(record_type),
                      STRINGV(value.value),
                      i);
            if (!llvm_emit_append_string_value(ctx,
                                               (LlvmValue){
                                                   .ok         = true,
                                                   .type_index = field_type,
                                                   .value      = field,
                                               })) {
                return false;
            }
        }
        if (field_count == 1) {
            llvm_emit_append_byte(ctx, ',');
        }
        llvm_emit_append_byte(ctx, ')');
        return true;
    }

    if (kind == STK_Array) {
        llvm_emit_append_byte(ctx, '[');
        u32 item_count = llvm_array_count(ctx->sema, value.type_index);
        u32 item_type  = llvm_collection_item_type(ctx->sema, value.type_index);
        string array_type = llvm_type_string(ctx, value.type_index);
        for (u32 i = 0; i < item_count; ++i) {
            if (i > 0) {
                llvm_emit_append_byte(ctx, ',');
                llvm_emit_append_byte(ctx, ' ');
            }

            string item = llvm_temp(ctx);
            sb_format(ctx->sb,
                      "  " STRINGP " = extractvalue " STRINGP " "
                      STRINGP ", %u\n",
                      STRINGV(item),
                      STRINGV(array_type),
                      STRINGV(value.value),
                      i);
            if (!llvm_emit_append_string_value(ctx,
                                               (LlvmValue){
                                                   .ok         = true,
                                                   .type_index = item_type,
                                                   .value      = item,
                                               })) {
                return false;
            }
        }
        llvm_emit_append_byte(ctx, ']');
        return true;
    }

    if (kind == STK_Slice) {
        llvm_emit_append_byte(ctx, '[');
        string slice_type = llvm_type_string(ctx, value.type_index);
        string data       = llvm_temp(ctx);
        sb_format(ctx->sb,
                  "  " STRINGP " = extractvalue " STRINGP " " STRINGP
                  ", 0\n",
                  STRINGV(data),
                  STRINGV(slice_type),
                  STRINGV(value.value));
        string count = llvm_temp(ctx);
        sb_format(ctx->sb,
                  "  " STRINGP " = extractvalue " STRINGP " " STRINGP
                  ", 1\n",
                  STRINGV(count),
                  STRINGV(slice_type),
                  STRINGV(value.value));

        string index_ptr = llvm_temp(ctx);
        sb_format(ctx->sb,
                  "  " STRINGP " = alloca i64\n"
                  "  store i64 0, ptr " STRINGP "\n",
                  STRINGV(index_ptr),
                  STRINGV(index_ptr));

        string cond_label = llvm_label(ctx, "slice.string.cond");
        string body_label = llvm_label(ctx, "slice.string.body");
        string sep_label  = llvm_label(ctx, "slice.string.sep");
        string item_label = llvm_label(ctx, "slice.string.item");
        string end_label  = llvm_label(ctx, "slice.string.end");

        sb_format(ctx->sb, "  br label %%" STRINGP "\n", STRINGV(cond_label));
        sb_format(ctx->sb, STRINGP ":\n", STRINGV(cond_label));
        string index = llvm_temp(ctx);
        sb_format(ctx->sb,
                  "  " STRINGP " = load i64, ptr " STRINGP "\n",
                  STRINGV(index),
                  STRINGV(index_ptr));
        string more = llvm_temp(ctx);
        sb_format(ctx->sb,
                  "  " STRINGP " = icmp ult i64 " STRINGP ", " STRINGP
                  "\n",
                  STRINGV(more),
                  STRINGV(index),
                  STRINGV(count));
        sb_format(ctx->sb,
                  "  br i1 " STRINGP ", label %%" STRINGP ", label %%"
                  STRINGP "\n",
                  STRINGV(more),
                  STRINGV(body_label),
                  STRINGV(end_label));

        sb_format(ctx->sb, STRINGP ":\n", STRINGV(body_label));
        string needs_sep = llvm_temp(ctx);
        sb_format(ctx->sb,
                  "  " STRINGP " = icmp ne i64 " STRINGP ", 0\n",
                  STRINGV(needs_sep),
                  STRINGV(index));
        sb_format(ctx->sb,
                  "  br i1 " STRINGP ", label %%" STRINGP ", label %%"
                  STRINGP "\n",
                  STRINGV(needs_sep),
                  STRINGV(sep_label),
                  STRINGV(item_label));

        sb_format(ctx->sb, STRINGP ":\n", STRINGV(sep_label));
        llvm_emit_append_byte(ctx, ',');
        llvm_emit_append_byte(ctx, ' ');
        sb_format(ctx->sb, "  br label %%" STRINGP "\n", STRINGV(item_label));

        sb_format(ctx->sb, STRINGP ":\n", STRINGV(item_label));
        u32 item_type = llvm_collection_item_type(ctx->sema, value.type_index);
        if (item_type == sema_no_type()) {
            return false;
        }
        string item_type_string = llvm_type_string(ctx, item_type);
        string item_ptr         = llvm_temp(ctx);
        sb_format(ctx->sb,
                  "  " STRINGP " = getelementptr inbounds " STRINGP
                  ", ptr " STRINGP ", i64 " STRINGP "\n",
                  STRINGV(item_ptr),
                  STRINGV(item_type_string),
                  STRINGV(data),
                  STRINGV(index));
        string item = llvm_temp(ctx);
        sb_format(ctx->sb,
                  "  " STRINGP " = load " STRINGP ", ptr " STRINGP "\n",
                  STRINGV(item),
                  STRINGV(item_type_string),
                  STRINGV(item_ptr));
        if (!llvm_emit_append_string_value(ctx,
                                           (LlvmValue){
                                               .ok         = true,
                                               .type_index = item_type,
                                               .value      = item,
                                           })) {
            return false;
        }
        string next = llvm_temp(ctx);
        sb_format(ctx->sb,
                  "  " STRINGP " = add i64 " STRINGP ", 1\n",
                  STRINGV(next),
                  STRINGV(index));
        sb_format(ctx->sb,
                  "  store i64 " STRINGP ", ptr " STRINGP "\n",
                  STRINGV(next),
                  STRINGV(index_ptr));
        sb_format(ctx->sb, "  br label %%" STRINGP "\n", STRINGV(cond_label));

        sb_format(ctx->sb, STRINGP ":\n", STRINGV(end_label));
        llvm_emit_append_byte(ctx, ']');
        return true;
    }

    string suffix = llvm_string_helper_suffix(ctx->sema, value.type_index);
    if (suffix.count == 0) {
        return false;
    }

    string value_type = llvm_type_string(ctx, value.type_index);
    string converted  = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = call { ptr, i64 } @to_string$" STRINGP
              "(" STRINGP " " STRINGP ")\n",
              STRINGV(converted),
              STRINGV(suffix),
              STRINGV(value_type),
              STRINGV(value.value));
    sb_format(ctx->sb,
              "  call void @string_builder_append_string({ ptr, i64 } "
              STRINGP ")\n",
              STRINGV(converted));
    return true;
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
                               const HirFunction*   function,
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
    if (callee->kind == HIR_EXPR_Field &&
        llvm_type_is_function(ctx->sema, callee->type_index) &&
        callee->symbol_handle != U32_MAX) {
        *out = llvm_symbol_name_string(ctx->lexer,
                                       ctx->arena,
                                       callee->symbol_handle);
        return true;
    }

    if (callee->kind != HIR_EXPR_LocalRef ||
        callee->ref_kind != HIR_REF_Binding ||
        callee->ref_index >= array_count(ctx->hir->bindings)) {
        LlvmValue callee_value = llvm_emit_expr(ctx, function, callee_expr_index);
        if (!callee_value.ok ||
            !llvm_type_is_function(ctx->sema, callee_value.type_index)) {
            return false;
        }
        *out = callee_value.value;
        return true;
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
        break;
    }

    LlvmValue callee_value = llvm_emit_expr(ctx, function, callee_expr_index);
    if (!callee_value.ok ||
        !llvm_type_is_function(ctx->sema, callee_value.type_index)) {
        return false;
    }
    *out = callee_value.value;
    return true;
}

internal bool llvm_callee_function_index(LlvmFunctionContext* ctx,
                                         u32                  callee_expr_index,
                                         u32*                 out)
{
    if (callee_expr_index >= array_count(ctx->hir->exprs)) {
        return false;
    }

    const HirExpr* callee = &ctx->hir->exprs[callee_expr_index];
    if (callee->kind == HIR_EXPR_FunctionRef &&
        callee->ref_index < array_count(ctx->hir->functions)) {
        *out = callee->ref_index;
        return true;
    }
    if (callee->kind == HIR_EXPR_LocalRef &&
        callee->ref_kind == HIR_REF_Binding &&
        callee->ref_index < array_count(ctx->hir->bindings)) {
        const HirBinding* binding = &ctx->hir->bindings[callee->ref_index];
        if (binding->kind == HIR_BINDING_Function &&
            binding->target_index < array_count(ctx->hir->functions)) {
            *out = binding->target_index;
            return true;
        }
    }
    if (callee->kind == HIR_EXPR_LocalRef &&
        callee->ref_kind == HIR_REF_Local) {
        LlvmValue callee_value = {0};
        if (!llvm_find_local_value(ctx, callee->ref_index, &callee_value)) {
            return false;
        }
        for (u32 i = 0; i < array_count(ctx->hir->functions); ++i) {
            string name =
                llvm_function_name_string(ctx->hir, ctx->lexer, ctx->arena, i);
            if (string_eq(name, callee_value.value)) {
                *out = i;
                return true;
            }
        }
    }
    return false;
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
        if (llvm_type_kind(ctx->sema, source_type) == STK_Bool) {
            return s("zext");
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
        SemaTypeKind integer_type_kind = llvm_type_kind(ctx->sema,
                                                        expr->type_index);
        if (integer_type_kind == STK_Pointer ||
            integer_type_kind == STK_DynamicArray) {
            if (expr->integer == 0) {
                return (LlvmValue){
                    .ok         = true,
                    .type_index = expr->type_index,
                    .value      = s("null"),
                };
            }

            string temp = llvm_temp(ctx);
            sb_format(ctx->sb,
                      "  " STRINGP " = inttoptr i64 %lld to ptr\n",
                      STRINGV(temp),
                      (long long)expr->integer);
            return (LlvmValue){
                .ok         = true,
                .type_index = expr->type_index,
                .value      = temp,
            };
        }
        return (LlvmValue){
            .ok         = true,
            .type_index = expr->type_index,
            .value = string_format(ctx->arena, "%lld", (long long)expr->integer),
        };
    case HIR_EXPR_FloatLiteral:
        return (LlvmValue){
            .ok         = true,
            .type_index = expr->type_index,
            .value      = llvm_float_literal_string(
                ctx->sema, ctx->arena, expr->type_index, expr->floating),
        };
    case HIR_EXPR_StringLiteral:
        {
            if (expr->string_index >= array_count(ctx->lexer->strings)) {
                return (LlvmValue){0};
            }

            string global =
                llvm_string_global_name_string(ctx->hir,
                                               ctx->arena,
                                               expr->string_index);
            if (expr->string_is_cstring) {
                return (LlvmValue){
                    .ok         = true,
                    .type_index = expr->type_index,
                    .value      = global,
                };
            }

            string value = string_format(
                ctx->arena,
                "{ ptr " STRINGP ", i64 %zu }",
                STRINGV(global),
                ctx->lexer->strings[expr->string_index].count);
            return (LlvmValue){
                .ok         = true,
                .type_index = expr->type_index != sema_no_type()
                                  ? expr->type_index
                                  : llvm_builtin_type(ctx->sema, STK_String),
                .value      = value,
            };
        }
    case HIR_EXPR_StringConcat:
        {
            string concat_value = {0};
            if (!llvm_eval_hir_string_constant(
                    ctx->hir, ctx->lexer, ctx->arena, expr_index, &concat_value)) {
                return (LlvmValue){0};
            }
            string global =
                llvm_concat_string_global_name_string(ctx->hir,
                                                      ctx->arena,
                                                      expr_index);
            string value = string_format(ctx->arena,
                                         "{ ptr " STRINGP ", i64 %zu }",
                                         STRINGV(global),
                                         concat_value.count);
            return (LlvmValue){
                .ok         = true,
                .type_index = expr->type_index != sema_no_type()
                                  ? expr->type_index
                                  : llvm_builtin_type(ctx->sema, STK_String),
                .value      = value,
            };
        }
    case HIR_EXPR_InterpolatedString:
        {
            string mark = llvm_temp(ctx);
            sb_format(ctx->sb,
                      "  " STRINGP " = call i64 @string_builder_mark()\n",
                      STRINGV(mark));
            for (u32 i = 0; i < expr->arg_count; ++i) {
                const HirCallArg* arg =
                    &ctx->hir->call_args[expr->first_arg + i];
                LlvmValue part = llvm_emit_expr(ctx, function, arg->expr_index);
                if (!part.ok) {
                    return (LlvmValue){0};
                }
                if (!llvm_emit_append_string_value(ctx, part)) {
                    return (LlvmValue){0};
                }
            }
            string result = llvm_temp(ctx);
            sb_format(ctx->sb,
                      "  " STRINGP
                      " = call { ptr, i64 } @string_builder_finish(i64 "
                      STRINGP ")\n",
                      STRINGV(result),
                      STRINGV(mark));
            return (LlvmValue){
                .ok         = true,
                .type_index = expr->type_index,
                .value      = result,
            };
        }
    case HIR_EXPR_BoolLiteral:
        return (LlvmValue){
            .ok         = true,
            .type_index = expr->type_index,
            .value      = expr->boolean ? s("1") : s("0"),
        };
    case HIR_EXPR_NilLiteral:
        return llvm_default_value(ctx, expr->type_index);
    case HIR_EXPR_FunctionRef:
        if (expr->ref_index >= array_count(ctx->hir->functions)) {
            return (LlvmValue){0};
        }
        return (LlvmValue){
            .ok         = true,
            .type_index = expr->type_index,
            .value      = llvm_function_name_string(
                ctx->hir, ctx->lexer, ctx->arena, expr->ref_index),
        };
    case HIR_EXPR_Array:
        {
            Array(LlvmValue) values = NULL;
            for (u32 i = 0; i < expr->arg_count; ++i) {
                const HirCallArg* arg =
                    &ctx->hir->call_args[expr->first_arg + i];
                LlvmValue item = llvm_emit_expr(ctx, function, arg->expr_index);
                if (!item.ok) {
                    array_free(values);
                    return (LlvmValue){0};
                }
                array_push(values, item);
            }

            if (llvm_type_kind(ctx->sema, expr->type_index) == STK_Slice) {
                u32 item_type =
                    llvm_collection_item_type(ctx->sema, expr->type_index);
                if (item_type == sema_no_type()) {
                    array_free(values);
                    return (LlvmValue){0};
                }

                string item_type_string = llvm_type_string(ctx, item_type);
                string array_type = string_format(ctx->arena,
                                                  "[%u x " STRINGP "]",
                                                  expr->arg_count,
                                                  STRINGV(item_type_string));
                string current = expr->arg_count == 0 ? s("zeroinitializer")
                                                      : s("poison");
                for (u32 i = 0; i < expr->arg_count; ++i) {
                    string temp       = llvm_temp(ctx);
                    string value_type = llvm_type_string(ctx, values[i].type_index);
                    sb_format(ctx->sb,
                              "  " STRINGP " = insertvalue " STRINGP " "
                              STRINGP ", " STRINGP " " STRINGP ", %u\n",
                              STRINGV(temp),
                              STRINGV(array_type),
                              STRINGV(current),
                              STRINGV(value_type),
                              STRINGV(values[i].value),
                              i);
                    current = temp;
                }

                string slot = {0};
                if (ctx->global_init_value_index != U32_MAX) {
                    slot = llvm_global_slice_backing_name_string(
                        ctx->hir, ctx->arena, ctx->global_init_value_index);
                    sb_format(ctx->sb,
                              "  store " STRINGP " " STRINGP ", ptr " STRINGP
                              "\n",
                              STRINGV(array_type),
                              STRINGV(current),
                              STRINGV(slot));
                } else {
                    slot = llvm_temp(ctx);
                    sb_format(ctx->sb,
                              "  " STRINGP " = alloca " STRINGP "\n"
                              "  store " STRINGP " " STRINGP ", ptr " STRINGP
                              "\n",
                              STRINGV(slot),
                              STRINGV(array_type),
                              STRINGV(array_type),
                              STRINGV(current),
                              STRINGV(slot));
                }

                string data_ptr = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = getelementptr inbounds " STRINGP
                          ", ptr " STRINGP ", i64 0, i64 0\n",
                          STRINGV(data_ptr),
                          STRINGV(array_type),
                          STRINGV(slot));
                string slice0 = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP
                          " = insertvalue { ptr, i64 } poison, ptr "
                          STRINGP ", 0\n",
                          STRINGV(slice0),
                          STRINGV(data_ptr));
                string slice1 = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = insertvalue { ptr, i64 } "
                          STRINGP ", i64 %u, 1\n",
                          STRINGV(slice1),
                          STRINGV(slice0),
                          expr->arg_count);
                array_free(values);
                return (LlvmValue){
                    .ok         = true,
                    .type_index = expr->type_index,
                    .value      = slice1,
                };
            }

            if (llvm_type_kind(ctx->sema, expr->type_index) ==
                STK_DynamicArray) {
                u32 item_type =
                    llvm_collection_item_type(ctx->sema, expr->type_index);
                if (item_type == sema_no_type()) {
                    array_free(values);
                    return (LlvmValue){0};
                }

                u32 capacity = expr->arg_count;
                if (expr->integer > 0 && (u64)expr->integer > capacity) {
                    capacity = (u32)expr->integer;
                }

                string header = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = call ptr @malloc(i64 24)\n",
                          STRINGV(header));
                string data_ptr_ptr =
                    llvm_dynamic_array_header_field_ptr(ctx, header, 0);
                string count_ptr =
                    llvm_dynamic_array_header_field_ptr(ctx, header, 1);
                string capacity_ptr =
                    llvm_dynamic_array_header_field_ptr(ctx, header, 2);

                string data = s("null");
                if (capacity > 0) {
                    u64 item_size =
                        llvm_type_storage_bytes(ctx->sema, item_type);
                    data = llvm_temp(ctx);
                    sb_format(ctx->sb,
                              "  " STRINGP " = call ptr @malloc(i64 %llu)\n",
                              STRINGV(data),
                              (unsigned long long)item_size * capacity);

                    string item_type_string = llvm_type_string(ctx, item_type);
                    for (u32 i = 0; i < expr->arg_count; ++i) {
                        string item_ptr = llvm_temp(ctx);
                        string value_type =
                            llvm_type_string(ctx, values[i].type_index);
                        sb_format(ctx->sb,
                                  "  " STRINGP
                                  " = getelementptr inbounds " STRINGP
                                  ", ptr " STRINGP ", i64 %u\n"
                                  "  store " STRINGP " " STRINGP
                                  ", ptr " STRINGP "\n",
                                  STRINGV(item_ptr),
                                  STRINGV(item_type_string),
                                  STRINGV(data),
                                  i,
                                  STRINGV(value_type),
                                  STRINGV(values[i].value),
                                  STRINGV(item_ptr));
                    }
                }

                sb_format(ctx->sb,
                          "  store ptr " STRINGP ", ptr " STRINGP "\n"
                          "  store i64 %u, ptr " STRINGP "\n"
                          "  store i64 %u, ptr " STRINGP "\n",
                          STRINGV(data),
                          STRINGV(data_ptr_ptr),
                          expr->arg_count,
                          STRINGV(count_ptr),
                          capacity,
                          STRINGV(capacity_ptr));
                array_free(values);
                return (LlvmValue){
                    .ok         = true,
                    .type_index = expr->type_index,
                    .value      = header,
                };
            }

            LlvmValue result = llvm_build_aggregate_value(ctx,
                                                          expr->type_index,
                                                          values,
                                                          array_count(values));
            array_free(values);
            return result;
        }
    case HIR_EXPR_Tuple:
        {
            Array(LlvmValue) values = NULL;
            for (u32 i = 0; i < expr->arg_count; ++i) {
                const HirCallArg* arg =
                    &ctx->hir->call_args[expr->first_arg + i];
                LlvmValue item = llvm_emit_expr(ctx, function, arg->expr_index);
                if (!item.ok) {
                    array_free(values);
                    return (LlvmValue){0};
                }
                array_push(values, item);
            }
            LlvmValue result = llvm_build_aggregate_value(ctx,
                                                          expr->type_index,
                                                          values,
                                                          array_count(values));
            array_free(values);
            return result;
        }
    case HIR_EXPR_Plex:
    case HIR_EXPR_PlexUpdate:
        {
            if (llvm_type_kind(ctx->sema, expr->type_index) == STK_Union) {
                if (expr->arg_count == 0) {
                    return llvm_default_value(ctx, expr->type_index);
                }

                const HirCallArg* arg =
                    &ctx->hir->call_args[expr->first_arg];
                if (llvm_record_field_index(ctx->sema,
                                            expr->type_index,
                                            arg->symbol_handle) == U32_MAX) {
                    return (LlvmValue){0};
                }
                LlvmValue field_value =
                    llvm_emit_expr(ctx, function, arg->expr_index);
                if (!field_value.ok) {
                    return (LlvmValue){0};
                }
                return llvm_cast_to_union_storage(ctx,
                                                  field_value,
                                                  expr->type_index);
            }

            if (expr->kind == HIR_EXPR_Plex) {
                Array(LlvmValue) values = NULL;
                u32 field_count =
                    llvm_record_field_count(ctx->sema, expr->type_index);
                for (u32 i = 0; i < field_count; ++i) {
                    u32 field_type =
                        llvm_record_field_type(ctx->sema, expr->type_index, i);
                    LlvmValue field_value =
                        llvm_default_value(ctx, field_type);
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
                        array_free(values);
                        return (LlvmValue){0};
                    }
                    field_value =
                        llvm_coerce_value_to_type(ctx, field_value, field_type);
                    if (!field_value.ok) {
                        array_free(values);
                        return (LlvmValue){0};
                    }
                    array_push(values, field_value);
                }
                LlvmValue result = llvm_build_aggregate_value(ctx,
                                                              expr->type_index,
                                                              values,
                                                              field_count);
                array_free(values);
                return result;
            }

            LlvmValue base =
                llvm_emit_expr(ctx, function, expr->operand_expr_index);
            if (!base.ok) {
                return (LlvmValue){0};
            }
            string rendered = base.value;

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
        if (expr->ref_kind == HIR_REF_Binding &&
            expr->ref_index < array_count(ctx->hir->bindings)) {
            const HirBinding* binding = &ctx->hir->bindings[expr->ref_index];
            if (binding->kind == HIR_BINDING_Value &&
                binding->target_index < array_count(ctx->hir->values)) {
                const HirValue* value = &ctx->hir->values[binding->target_index];
                if (value->kind == HIR_VALUE_Constant &&
                    value->value_expr_index != U32_MAX) {
                    return llvm_emit_expr(ctx, function, value->value_expr_index);
                }
                if (value->kind == HIR_VALUE_Global) {
                    string name = llvm_value_name_string(ctx->hir,
                                                         ctx->lexer,
                                                         ctx->arena,
                                                         binding->target_index);
                    if (name.count == 0) {
                        return (LlvmValue){0};
                    }
                    string type   = llvm_type_string(ctx, value->type_index);
                    string loaded = llvm_temp(ctx);
                    sb_format(ctx->sb,
                              "  " STRINGP " = load " STRINGP ", ptr " STRINGP
                              "\n",
                              STRINGV(loaded),
                              STRINGV(type),
                              STRINGV(name));
                    return (LlvmValue){
                        .ok         = true,
                        .type_index = value->type_index,
                        .value      = loaded,
                    };
                }
            }
        }
        if (expr->ref_kind == HIR_REF_None &&
            llvm_type_kind(ctx->sema, expr->type_index) == STK_Enum &&
            expr->symbol_handle != U32_MAX) {
            u32 variant_index = llvm_enum_variant_index(
                ctx->sema, expr->type_index, expr->symbol_handle);
            if (variant_index != U32_MAX) {
                return llvm_emit_enum_constructor(
                    ctx, function, expr, variant_index);
            }
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
                SemaTypeKind lhs_kind = llvm_type_kind(ctx->sema, lhs.type_index);
                SemaTypeKind rhs_kind = llvm_type_kind(ctx->sema, rhs.type_index);
                if ((lhs_kind == STK_Slice || lhs_kind == STK_String) &&
                    lhs_kind == rhs_kind) {
                    string lhs_data = llvm_temp(ctx);
                    string lhs_count = llvm_temp(ctx);
                    string rhs_data = llvm_temp(ctx);
                    string rhs_count = llvm_temp(ctx);
                    sb_format(ctx->sb,
                              "  " STRINGP " = extractvalue " STRINGP " "
                              STRINGP ", 0\n"
                              "  " STRINGP " = extractvalue " STRINGP " "
                              STRINGP ", 1\n",
                              STRINGV(lhs_data),
                              STRINGV(type),
                              STRINGV(lhs.value),
                              STRINGV(lhs_count),
                              STRINGV(type),
                              STRINGV(lhs.value));
                    string rhs_type = llvm_type_string(ctx, rhs.type_index);
                    sb_format(ctx->sb,
                              "  " STRINGP " = extractvalue " STRINGP " "
                              STRINGP ", 0\n"
                              "  " STRINGP " = extractvalue " STRINGP " "
                              STRINGP ", 1\n",
                              STRINGV(rhs_data),
                              STRINGV(rhs_type),
                              STRINGV(rhs.value),
                              STRINGV(rhs_count),
                              STRINGV(rhs_type),
                              STRINGV(rhs.value));
                    string data_eq = llvm_temp(ctx);
                    string count_eq = llvm_temp(ctx);
                    string both_eq = llvm_temp(ctx);
                    sb_format(ctx->sb,
                              "  " STRINGP " = icmp eq ptr " STRINGP ", "
                              STRINGP "\n"
                              "  " STRINGP " = icmp eq i64 " STRINGP ", "
                              STRINGP "\n"
                              "  " STRINGP " = and i1 " STRINGP ", " STRINGP
                              "\n",
                              STRINGV(data_eq),
                              STRINGV(lhs_data),
                              STRINGV(rhs_data),
                              STRINGV(count_eq),
                              STRINGV(lhs_count),
                              STRINGV(rhs_count),
                              STRINGV(both_eq),
                              STRINGV(data_eq),
                              STRINGV(count_eq));
                    if (expr->binary_op == HIR_BINARY_Equal) {
                        temp = both_eq;
                    } else {
                        sb_format(ctx->sb,
                                  "  " STRINGP " = xor i1 " STRINGP ", 1\n",
                                  STRINGV(temp),
                                  STRINGV(both_eq));
                    }
                } else if (lhs_kind == STK_Enum &&
                    llvm_type_kind(ctx->sema, rhs.type_index) == STK_Enum) {
                    string lhs_tag = llvm_temp(ctx);
                    sb_format(ctx->sb,
                              "  " STRINGP " = extractvalue " STRINGP " "
                              STRINGP ", 0\n",
                              STRINGV(lhs_tag),
                              STRINGV(type),
                              STRINGV(lhs.value));
                    string rhs_type = llvm_type_string(ctx, rhs.type_index);
                    string rhs_tag  = llvm_temp(ctx);
                    sb_format(ctx->sb,
                              "  " STRINGP " = extractvalue " STRINGP " "
                              STRINGP ", 0\n",
                              STRINGV(rhs_tag),
                              STRINGV(rhs_type),
                              STRINGV(rhs.value));
                    sb_format(ctx->sb,
                              "  " STRINGP " = icmp " STRINGP " i64 "
                              STRINGP ", " STRINGP "\n",
                              STRINGV(temp),
                              STRINGV(cmp),
                              STRINGV(lhs_tag),
                              STRINGV(rhs_tag));
                } else if (llvm_float_bits(ctx->sema, lhs.type_index) > 0 ||
                    llvm_float_bits(ctx->sema, rhs.type_index) > 0) {
                    cmp = llvm_float_compare_instruction(expr->binary_op);
                    if (cmp.count == 0) {
                        return (LlvmValue){0};
                    }
                    sb_format(ctx->sb,
                              "  " STRINGP " = fcmp " STRINGP " " STRINGP
                              " " STRINGP ", " STRINGP "\n",
                              STRINGV(temp),
                              STRINGV(cmp),
                              STRINGV(type),
                              STRINGV(lhs.value),
                              STRINGV(rhs.value));
                } else {
                    sb_format(ctx->sb,
                              "  " STRINGP " = icmp " STRINGP " " STRINGP
                              " " STRINGP ", " STRINGP "\n",
                              STRINGV(temp),
                              STRINGV(cmp),
                              STRINGV(type),
                              STRINGV(lhs.value),
                              STRINGV(rhs.value));
                }
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
            if (llvm_float_bits(ctx->sema, expr->type_index) > 0) {
                instr = llvm_float_binary_instruction(expr->binary_op);
                if (instr.count == 0) {
                    return (LlvmValue){0};
                }
            }
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
    case HIR_EXPR_Assign:
        {
            LlvmValue value =
                llvm_emit_expr(ctx, function, expr->rhs_expr_index);
            if (!value.ok ||
                !llvm_emit_assign(ctx, function, expr->lhs_expr_index, value)) {
                return (LlvmValue){0};
            }
            value.type_index = expr->type_index;
            return value;
        }
    case HIR_EXPR_Unary:
        {
            if (expr->unary_op == HIR_UNARY_AddressOf) {
                LlvmValue address =
                    llvm_address_of_expr(ctx, function, expr->operand_expr_index);
                if (!address.ok) {
                    return (LlvmValue){0};
                }
                address.type_index = expr->type_index;
                return address;
            }

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
                if (llvm_float_bits(ctx->sema, expr->type_index) > 0) {
                    sb_format(ctx->sb,
                              "  " STRINGP " = fneg " STRINGP " " STRINGP
                              "\n",
                              STRINGV(temp),
                              STRINGV(type),
                              STRINGV(operand.value));
                } else {
                    sb_format(ctx->sb,
                              "  " STRINGP " = sub " STRINGP " 0, " STRINGP
                              "\n",
                              STRINGV(temp),
                              STRINGV(type),
                              STRINGV(operand.value));
                }
                return (LlvmValue){
                    .ok         = true,
                    .type_index = expr->type_index,
                    .value      = temp,
                };
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

            if (llvm_type_kind(ctx->sema, target.type_index) ==
                STK_DynamicArray) {
                u32 item_type =
                    llvm_collection_item_type(ctx->sema, target.type_index);
                if (item_type == sema_no_type()) {
                    item_type = expr->type_index;
                }
                string data_ptr = llvm_dynamic_array_load_header_field(
                    ctx, target.value, 0, s("ptr"));
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
            if (expr->operand_expr_index >= array_count(ctx->hir->exprs)) {
                return (LlvmValue){0};
            }

            const HirExpr* target_expr =
                &ctx->hir->exprs[expr->operand_expr_index];
            u32 target_type = target_expr->type_index;
            if (llvm_type_kind(ctx->sema, target_type) == STK_String) {
                LlvmValue target =
                    llvm_emit_expr(ctx, function, expr->operand_expr_index);
                if (!target.ok) {
                    return (LlvmValue){0};
                }

                string data = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = extractvalue { ptr, i64 } "
                          STRINGP ", 0\n",
                          STRINGV(data),
                          STRINGV(target.value));

                string total_count = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = extractvalue { ptr, i64 } "
                          STRINGP ", 1\n",
                          STRINGV(total_count),
                          STRINGV(target.value));

                i64   start_value       = 0;
                bool  start_is_constant = true;
                string start             = s("0");
                if (expr->lhs_expr_index < array_count(ctx->hir->exprs)) {
                    start_is_constant = llvm_expr_integer_constant(
                        ctx->hir, expr->lhs_expr_index, &start_value);
                    if (start_is_constant) {
                        start = string_format(ctx->arena,
                                              "%lld",
                                              (long long)start_value);
                    } else {
                        llvm_emit_index_i64(
                            ctx, function, expr->lhs_expr_index, &start);
                    }
                }

                i64   end_value       = 0;
                bool  end_is_constant = false;
                string end             = total_count;
                if (expr->rhs_expr_index < array_count(ctx->hir->exprs)) {
                    end_is_constant = llvm_expr_integer_constant(
                        ctx->hir, expr->rhs_expr_index, &end_value);
                    if (end_is_constant) {
                        end = string_format(
                            ctx->arena, "%lld", (long long)end_value);
                    } else {
                        llvm_emit_index_i64(
                            ctx, function, expr->rhs_expr_index, &end);
                    }
                }

                string count = llvm_slice_count_i64(ctx,
                                                    start,
                                                    start_is_constant,
                                                    start_value,
                                                    end,
                                                    end_is_constant,
                                                    end_value);

                string data_ptr = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = getelementptr inbounds i8, ptr "
                          STRINGP ", i64 " STRINGP "\n",
                          STRINGV(data_ptr),
                          STRINGV(data),
                          STRINGV(start));
                string slice0 = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP
                          " = insertvalue { ptr, i64 } poison, ptr "
                          STRINGP ", 0\n",
                          STRINGV(slice0),
                          STRINGV(data_ptr));
                string slice1 = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = insertvalue { ptr, i64 } "
                          STRINGP ", i64 " STRINGP ", 1\n",
                          STRINGV(slice1),
                          STRINGV(slice0),
                          STRINGV(count));
                return (LlvmValue){
                    .ok         = true,
                    .type_index = expr->type_index,
                    .value      = slice1,
                };
            }

            if (llvm_type_kind(ctx->sema, target_type) == STK_Slice) {
                LlvmValue target =
                    llvm_emit_expr(ctx, function, expr->operand_expr_index);
                if (!target.ok) {
                    return (LlvmValue){0};
                }

                string slice_type = llvm_type_string(ctx, target_type);
                string data       = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = extractvalue " STRINGP " "
                          STRINGP ", 0\n",
                          STRINGV(data),
                          STRINGV(slice_type),
                          STRINGV(target.value));

                string total_count = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = extractvalue " STRINGP " "
                          STRINGP ", 1\n",
                          STRINGV(total_count),
                          STRINGV(slice_type),
                          STRINGV(target.value));

                i64   start_value       = 0;
                bool  start_is_constant = true;
                string start             = s("0");
                if (expr->lhs_expr_index < array_count(ctx->hir->exprs)) {
                    start_is_constant = llvm_expr_integer_constant(
                        ctx->hir, expr->lhs_expr_index, &start_value);
                    if (start_is_constant) {
                        start = string_format(ctx->arena,
                                              "%lld",
                                              (long long)start_value);
                    } else {
                        llvm_emit_index_i64(
                            ctx, function, expr->lhs_expr_index, &start);
                    }
                }

                i64   end_value       = 0;
                bool  end_is_constant = false;
                string end             = total_count;
                if (expr->rhs_expr_index < array_count(ctx->hir->exprs)) {
                    end_is_constant = llvm_expr_integer_constant(
                        ctx->hir, expr->rhs_expr_index, &end_value);
                    if (end_is_constant) {
                        end = string_format(
                            ctx->arena, "%lld", (long long)end_value);
                    } else {
                        llvm_emit_index_i64(
                            ctx, function, expr->rhs_expr_index, &end);
                    }
                }

                string count = llvm_slice_count_i64(ctx,
                                                    start,
                                                    start_is_constant,
                                                    start_value,
                                                    end,
                                                    end_is_constant,
                                                    end_value);

                u32 item_type = llvm_collection_item_type(ctx->sema,
                                                          target_type);
                if (item_type == sema_no_type()) {
                    return (LlvmValue){0};
                }
                string item_type_string = llvm_type_string(ctx, item_type);
                string data_ptr         = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = getelementptr inbounds " STRINGP
                          ", ptr " STRINGP ", i64 " STRINGP "\n",
                          STRINGV(data_ptr),
                          STRINGV(item_type_string),
                          STRINGV(data),
                          STRINGV(start));
                string slice0 = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP
                          " = insertvalue { ptr, i64 } poison, ptr "
                          STRINGP ", 0\n",
                          STRINGV(slice0),
                          STRINGV(data_ptr));
                string slice1 = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = insertvalue { ptr, i64 } "
                          STRINGP ", i64 " STRINGP ", 1\n",
                          STRINGV(slice1),
                          STRINGV(slice0),
                          STRINGV(count));
                return (LlvmValue){
                    .ok         = true,
                    .type_index = expr->type_index,
                    .value      = slice1,
                };
            }

            if (llvm_type_kind(ctx->sema, target_type) == STK_DynamicArray) {
                LlvmValue target =
                    llvm_emit_expr(ctx, function, expr->operand_expr_index);
                if (!target.ok) {
                    return (LlvmValue){0};
                }

                string data  = llvm_temp(ctx);
                string count = llvm_temp(ctx);
                string data_slot = llvm_temp(ctx);
                string count_slot = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = alloca ptr\n"
                          "  " STRINGP " = alloca i64\n",
                          STRINGV(data_slot),
                          STRINGV(count_slot));

                string is_null = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = icmp eq ptr " STRINGP ", null\n",
                          STRINGV(is_null),
                          STRINGV(target.value));
                string empty_label = llvm_label(ctx, "dynarray.slice.empty");
                string load_label  = llvm_label(ctx, "dynarray.slice.load");
                string ready_label = llvm_label(ctx, "dynarray.slice.ready");
                sb_format(ctx->sb,
                          "  br i1 " STRINGP ", label %%" STRINGP
                          ", label %%" STRINGP "\n",
                          STRINGV(is_null),
                          STRINGV(empty_label),
                          STRINGV(load_label));

                sb_format(ctx->sb, STRINGP ":\n", STRINGV(empty_label));
                sb_format(ctx->sb,
                          "  store ptr null, ptr " STRINGP "\n"
                          "  store i64 0, ptr " STRINGP "\n"
                          "  br label %%" STRINGP "\n",
                          STRINGV(data_slot),
                          STRINGV(count_slot),
                          STRINGV(ready_label));

                sb_format(ctx->sb, STRINGP ":\n", STRINGV(load_label));
                data = llvm_dynamic_array_load_header_field(
                    ctx, target.value, 0, s("ptr"));
                count = llvm_dynamic_array_load_header_field(
                    ctx, target.value, 1, s("i64"));
                sb_format(ctx->sb,
                          "  store ptr " STRINGP ", ptr " STRINGP "\n"
                          "  store i64 " STRINGP ", ptr " STRINGP "\n"
                          "  br label %%" STRINGP "\n",
                          STRINGV(data),
                          STRINGV(data_slot),
                          STRINGV(count),
                          STRINGV(count_slot),
                          STRINGV(ready_label));

                sb_format(ctx->sb, STRINGP ":\n", STRINGV(ready_label));
                string total_count = llvm_temp(ctx);
                string base_data   = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = load i64, ptr " STRINGP "\n"
                          "  " STRINGP " = load ptr, ptr " STRINGP "\n",
                          STRINGV(total_count),
                          STRINGV(count_slot),
                          STRINGV(base_data),
                          STRINGV(data_slot));

                i64   start_value       = 0;
                bool  start_is_constant = true;
                string start             = s("0");
                if (expr->lhs_expr_index < array_count(ctx->hir->exprs)) {
                    start_is_constant = llvm_expr_integer_constant(
                        ctx->hir, expr->lhs_expr_index, &start_value);
                    if (start_is_constant) {
                        start = string_format(ctx->arena,
                                              "%lld",
                                              (long long)start_value);
                    } else {
                        llvm_emit_index_i64(
                            ctx, function, expr->lhs_expr_index, &start);
                    }
                }

                i64   end_value       = 0;
                bool  end_is_constant = false;
                string end             = total_count;
                if (expr->rhs_expr_index < array_count(ctx->hir->exprs)) {
                    end_is_constant = llvm_expr_integer_constant(
                        ctx->hir, expr->rhs_expr_index, &end_value);
                    if (end_is_constant) {
                        end = string_format(
                            ctx->arena, "%lld", (long long)end_value);
                    } else {
                        llvm_emit_index_i64(
                            ctx, function, expr->rhs_expr_index, &end);
                    }
                }

                string slice_count = llvm_slice_count_i64(ctx,
                                                          start,
                                                          start_is_constant,
                                                          start_value,
                                                          end,
                                                          end_is_constant,
                                                          end_value);
                u32 item_type =
                    llvm_collection_item_type(ctx->sema, target_type);
                if (item_type == sema_no_type()) {
                    return (LlvmValue){0};
                }
                string item_type_string = llvm_type_string(ctx, item_type);
                string data_ptr         = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = getelementptr inbounds " STRINGP
                          ", ptr " STRINGP ", i64 " STRINGP "\n",
                          STRINGV(data_ptr),
                          STRINGV(item_type_string),
                          STRINGV(base_data),
                          STRINGV(start));
                string slice0 = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP
                          " = insertvalue { ptr, i64 } poison, ptr "
                          STRINGP ", 0\n",
                          STRINGV(slice0),
                          STRINGV(data_ptr));
                string slice1 = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = insertvalue { ptr, i64 } "
                          STRINGP ", i64 " STRINGP ", 1\n",
                          STRINGV(slice1),
                          STRINGV(slice0),
                          STRINGV(slice_count));
                ctx->block_terminated = false;
                return (LlvmValue){
                    .ok         = true,
                    .type_index = expr->type_index,
                    .value      = slice1,
                };
            }

            LlvmValue target_address =
                llvm_address_of_expr(ctx, function, expr->operand_expr_index);
            if (!target_address.ok) {
                return (LlvmValue){0};
            }
            u32 item_type =
                llvm_collection_item_type(ctx->sema, target_type);
            if (item_type == sema_no_type()) {
                return (LlvmValue){0};
            }

            i64   start_value       = 0;
            bool  start_is_constant = true;
            string start             = s("0");
            if (expr->lhs_expr_index < array_count(ctx->hir->exprs)) {
                start_is_constant = llvm_expr_integer_constant(
                    ctx->hir, expr->lhs_expr_index, &start_value);
                if (start_is_constant) {
                    start = string_format(
                        ctx->arena, "%lld", (long long)start_value);
                } else {
                    llvm_emit_index_i64(
                        ctx, function, expr->lhs_expr_index, &start);
                }
            }

            i64 end_value = (i64)llvm_array_count(ctx->sema, target_type);
            bool end_is_constant = true;
            string end = string_format(ctx->arena,
                                       "%lld",
                                       (long long)end_value);
            if (expr->rhs_expr_index < array_count(ctx->hir->exprs)) {
                end_is_constant = llvm_expr_integer_constant(
                    ctx->hir, expr->rhs_expr_index, &end_value);
                if (end_is_constant) {
                    end = string_format(
                        ctx->arena, "%lld", (long long)end_value);
                } else {
                    llvm_emit_index_i64(
                        ctx, function, expr->rhs_expr_index, &end);
                }
            }

            string count = llvm_slice_count_i64(ctx,
                                                start,
                                                start_is_constant,
                                                start_value,
                                                end,
                                                end_is_constant,
                                                end_value);

            string target_type_string = llvm_type_string(ctx, target_type);
            string data_ptr           = llvm_temp(ctx);
            sb_format(ctx->sb,
                      "  " STRINGP " = getelementptr inbounds " STRINGP
                      ", ptr " STRINGP ", i64 0, i64 " STRINGP "\n",
                      STRINGV(data_ptr),
                      STRINGV(target_type_string),
                      STRINGV(target_address.value),
                      STRINGV(start));
            string slice0 = llvm_temp(ctx);
            sb_format(ctx->sb,
                      "  " STRINGP " = insertvalue { ptr, i64 } poison, ptr "
                      STRINGP ", 0\n",
                      STRINGV(slice0),
                      STRINGV(data_ptr));
            string slice1 = llvm_temp(ctx);
            sb_format(ctx->sb,
                      "  " STRINGP " = insertvalue { ptr, i64 } " STRINGP
                      ", i64 " STRINGP ", 1\n",
                      STRINGV(slice1),
                      STRINGV(slice0),
                      STRINGV(count));
            return (LlvmValue){
                .ok         = true,
                .type_index = expr->type_index,
                .value      = slice1,
            };
        }
    case HIR_EXPR_TupleField:
    case HIR_EXPR_Field:
        {
            if (expr->kind == HIR_EXPR_Field &&
                llvm_type_kind(ctx->sema, expr->type_index) == STK_Enum &&
                expr->symbol_handle != U32_MAX) {
                u32 variant_index = llvm_enum_variant_index(
                    ctx->sema, expr->type_index, expr->symbol_handle);
                if (variant_index != U32_MAX) {
                    return llvm_emit_enum_constructor(
                        ctx, function, expr, variant_index);
                }
            }

            LlvmValue target =
                llvm_emit_expr(ctx, function, expr->operand_expr_index);
            if (!target.ok) {
                return (LlvmValue){0};
            }

            if (llvm_type_kind(ctx->sema, target.type_index) ==
                STK_DynamicArray) {
                return llvm_emit_dynamic_array_field(ctx, target, expr);
            }

            while (llvm_type_kind(ctx->sema, target.type_index) ==
                   STK_Pointer) {
                u32 pointee_type = llvm_pointee_type(ctx->sema,
                                                     target.type_index);
                SemaTypeKind pointee_kind = llvm_type_kind(ctx->sema,
                                                           pointee_type);
                if (pointee_kind != STK_Tuple && pointee_kind != STK_Plex &&
                    pointee_kind != STK_Union && pointee_kind != STK_String) {
                    break;
                }

                string pointee = llvm_type_string(ctx, pointee_type);
                string loaded  = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = load " STRINGP ", ptr "
                          STRINGP "\n",
                          STRINGV(loaded),
                          STRINGV(pointee),
                          STRINGV(target.value));
                target.type_index = pointee_type;
                target.value      = loaded;
            }

            u32 field_index = U32_MAX;
            if (expr->kind == HIR_EXPR_TupleField) {
                field_index = (u32)expr->integer;
            } else if (llvm_type_kind(ctx->sema, target.type_index) ==
                       STK_String) {
                string field = lex_symbol(ctx->lexer, expr->symbol_handle);
                if (string_eq_cstr(field, "data")) {
                    field_index = 0;
                } else if (string_eq_cstr(field, "count")) {
                    field_index = 1;
                }
            } else if (llvm_type_kind(ctx->sema, target.type_index) ==
                       STK_Slice) {
                string field = lex_symbol(ctx->lexer, expr->symbol_handle);
                if (string_eq_cstr(field, "data")) {
                    field_index = 0;
                } else if (string_eq_cstr(field, "count")) {
                    field_index = 1;
                }
            } else {
                field_index = llvm_record_field_index(
                    ctx->sema, target.type_index, expr->symbol_handle);
            }
            if (field_index == U32_MAX) {
                return (LlvmValue){0};
            }

            if (llvm_type_kind(ctx->sema, target.type_index) == STK_Union) {
                u32 field_type = llvm_record_field_type(ctx->sema,
                                                        target.type_index,
                                                        field_index);
                if (field_type == sema_no_type()) {
                    return (LlvmValue){0};
                }
                return llvm_cast_from_union_storage(ctx, target, field_type);
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

            string end_label = llvm_label(ctx, "block.end");
            string old_break = ctx->break_label;
            string old_continue = ctx->continue_label;
            string old_break_value_ptr = ctx->break_value_ptr;
            u32    old_break_value_type = ctx->break_value_type;
            bool   old_break_emitted = ctx->emitted_break;

            string result_ptr = {0};
            if (!llvm_type_is_void(ctx->sema, expr->type_index)) {
                string result_type = llvm_type_string(ctx, expr->type_index);
                result_ptr         = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = alloca " STRINGP ", align 4\n",
                          STRINGV(result_ptr),
                          STRINGV(result_type));
                LlvmValue default_value = llvm_default_value(ctx, expr->type_index);
                if (!default_value.ok) {
                    return (LlvmValue){0};
                }
                sb_format(ctx->sb,
                          "  store " STRINGP " " STRINGP ", ptr " STRINGP
                          ", align 4\n",
                          STRINGV(result_type),
                          STRINGV(default_value.value),
                          STRINGV(result_ptr));
            }

            ctx->break_label      = end_label;
            ctx->continue_label   = (string){0};
            ctx->break_value_ptr  = result_ptr;
            ctx->break_value_type = expr->type_index;
            ctx->emitted_break    = false;
            llvm_push_control_target(
                ctx,
                (LlvmControlTarget){
                    .symbol_handle    = expr->symbol_handle,
                    .break_label      = end_label,
                    .continue_label   = (string){0},
                    .break_value_ptr  = result_ptr,
                    .break_value_type = expr->type_index,
                });

            bool emitted =
                llvm_emit_effect_block(ctx, function, expr->body_block_index);
            bool block_emitted_break = ctx->emitted_break;
            llvm_pop_control_target(ctx, expr->symbol_handle);
            ctx->break_label      = old_break;
            ctx->continue_label   = old_continue;
            ctx->break_value_ptr  = old_break_value_ptr;
            ctx->break_value_type = old_break_value_type;
            ctx->emitted_break    = old_break_emitted;
            if (!emitted) {
                return (LlvmValue){0};
            }

            if (!ctx->block_terminated) {
                sb_format(ctx->sb,
                          "  br label %%" STRINGP "\n",
                          STRINGV(end_label));
                block_emitted_break = true;
            }

            if (block_emitted_break) {
                sb_format(ctx->sb, STRINGP ":\n", STRINGV(end_label));
                ctx->block_terminated = false;
            }

            if (llvm_type_is_void(ctx->sema, expr->type_index)) {
                return (LlvmValue){
                    .ok         = true,
                    .type_index = expr->type_index,
                    .value      = s(""),
                };
            }

            string result_type = llvm_type_string(ctx, expr->type_index);
            string loaded      = llvm_temp(ctx);
            sb_format(ctx->sb,
                      "  " STRINGP " = load " STRINGP ", ptr " STRINGP
                      ", align 4\n",
                      STRINGV(loaded),
                      STRINGV(result_type),
                      STRINGV(result_ptr));
            return (LlvmValue){
                .ok         = true,
                .type_index = expr->type_index,
                .value      = loaded,
            };
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
                bool             ended_with_else = false;

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
                        ended_with_else = true;
                        break;
                    }
                    sb_format(ctx->sb, STRINGP ":\n", STRINGV(next_label));
                }

                if (!ended_with_else) {
                    sb_append_cstr(ctx->sb, "  unreachable\n");
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

            string end_label = llvm_label(ctx, "on.end");
            Array(LlvmValue) phi_values = NULL;
            Array(string)    phi_labels = NULL;
            bool             ended_with_else = false;

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
                    if (branch->guard_expr_index == U32_MAX) {
                        array_free(phi_values);
                        array_free(phi_labels);
                        return (LlvmValue){0};
                    }
                    LlvmValue condition =
                        llvm_emit_expr(ctx, function, branch->guard_expr_index);
                    if (!condition.ok) {
                        array_free(phi_values);
                        array_free(phi_labels);
                        return (LlvmValue){0};
                    }
                    sb_format(ctx->sb,
                              "  br i1 " STRINGP ", label %%" STRINGP
                              ", label %%" STRINGP "\n",
                              STRINGV(condition.value),
                              STRINGV(body_label),
                              STRINGV(next_label));
                }

                sb_format(ctx->sb, STRINGP ":\n", STRINGV(body_label));
                if (llvm_type_is_void(ctx->sema, expr->type_index)) {
                    ctx->block_terminated = false;
                    if (!llvm_emit_block(
                            ctx, function, branch->body_block_index)) {
                        array_free(phi_values);
                        array_free(phi_labels);
                        return (LlvmValue){0};
                    }
                    if (!ctx->block_terminated) {
                        sb_format(ctx->sb,
                                  "  br label %%" STRINGP "\n",
                                  STRINGV(end_label));
                    }
                    ctx->block_terminated = false;
                } else {
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
                }

                if (branch->is_else) {
                    ended_with_else = true;
                    break;
                }
                sb_format(ctx->sb, STRINGP ":\n", STRINGV(next_label));
            }

            if (!ended_with_else) {
                sb_append_cstr(ctx->sb, "  unreachable\n");
            }

            sb_format(ctx->sb, STRINGP ":\n", STRINGV(end_label));
            if (llvm_type_is_void(ctx->sema, expr->type_index)) {
                array_free(phi_values);
                array_free(phi_labels);
                return (LlvmValue){
                    .ok         = true,
                    .type_index = expr->type_index,
                    .value      = s(""),
                };
            }
            if (array_count(phi_values) == 0) {
                array_free(phi_values);
                array_free(phi_labels);
                return (LlvmValue){0};
            }
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
                SemaTypeKind iterable_kind =
                    llvm_type_kind(ctx->sema, iterable.type_index);
                if (!iterable.ok ||
                    (iterable_kind != STK_Slice &&
                     iterable_kind != STK_String &&
                     iterable_kind != STK_DynamicArray)) {
                    return (LlvmValue){0};
                }

                string data_ptr   = llvm_temp(ctx);
                string count = llvm_temp(ctx);
                if (iterable_kind == STK_DynamicArray) {
                    string data_slot  = llvm_temp(ctx);
                    string count_slot = llvm_temp(ctx);
                    sb_format(ctx->sb,
                              "  " STRINGP " = alloca ptr\n"
                              "  " STRINGP " = alloca i64\n",
                              STRINGV(data_slot),
                              STRINGV(count_slot));

                    string is_null = llvm_temp(ctx);
                    sb_format(ctx->sb,
                              "  " STRINGP " = icmp eq ptr " STRINGP
                              ", null\n",
                              STRINGV(is_null),
                              STRINGV(iterable.value));
                    string empty_label = llvm_label(ctx, "for.in.empty");
                    string load_label  = llvm_label(ctx, "for.in.load");
                    string ready_label = llvm_label(ctx, "for.in.ready");
                    sb_format(ctx->sb,
                              "  br i1 " STRINGP ", label %%" STRINGP
                              ", label %%" STRINGP "\n",
                              STRINGV(is_null),
                              STRINGV(empty_label),
                              STRINGV(load_label));

                    sb_format(ctx->sb, STRINGP ":\n", STRINGV(empty_label));
                    sb_format(ctx->sb,
                              "  store ptr null, ptr " STRINGP "\n"
                              "  store i64 0, ptr " STRINGP "\n"
                              "  br label %%" STRINGP "\n",
                              STRINGV(data_slot),
                              STRINGV(count_slot),
                              STRINGV(ready_label));

                    sb_format(ctx->sb, STRINGP ":\n", STRINGV(load_label));
                    string loaded_data = llvm_dynamic_array_load_header_field(
                        ctx, iterable.value, 0, s("ptr"));
                    string loaded_count = llvm_dynamic_array_load_header_field(
                        ctx, iterable.value, 1, s("i64"));
                    sb_format(ctx->sb,
                              "  store ptr " STRINGP ", ptr " STRINGP "\n"
                              "  store i64 " STRINGP ", ptr " STRINGP "\n"
                              "  br label %%" STRINGP "\n",
                              STRINGV(loaded_data),
                              STRINGV(data_slot),
                              STRINGV(loaded_count),
                              STRINGV(count_slot),
                              STRINGV(ready_label));

                    sb_format(ctx->sb, STRINGP ":\n", STRINGV(ready_label));
                    sb_format(ctx->sb,
                              "  " STRINGP " = load ptr, ptr " STRINGP "\n"
                              "  " STRINGP " = load i64, ptr " STRINGP "\n",
                              STRINGV(data_ptr),
                              STRINGV(data_slot),
                              STRINGV(count),
                              STRINGV(count_slot));
                } else {
                    string slice_type = llvm_type_string(ctx, iterable.type_index);
                    sb_format(ctx->sb,
                              "  " STRINGP " = extractvalue " STRINGP " "
                              STRINGP ", 0\n",
                              STRINGV(data_ptr),
                              STRINGV(slice_type),
                              STRINGV(iterable.value));
                    sb_format(ctx->sb,
                              "  " STRINGP " = extractvalue " STRINGP " "
                              STRINGP ", 1\n",
                              STRINGV(count),
                              STRINGV(slice_type),
                              STRINGV(iterable.value));
                }

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
                llvm_push_control_target(
                    ctx,
                    (LlvmControlTarget){
                        .symbol_handle    = loop->label_symbol,
                        .break_label      = end_label,
                        .continue_label   = cond_label,
                        .break_value_ptr  = (string){0},
                        .break_value_type = sema_no_type(),
                    });
                if (!llvm_emit_effect_block(ctx, function, loop->body_block_index)) {
                    llvm_pop_control_target(ctx, loop->label_symbol);
                    return (LlvmValue){0};
                }
                llvm_pop_control_target(ctx, loop->label_symbol);
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
            string old_break    = ctx->break_label;
            string old_continue = ctx->continue_label;
            bool   old_break_emitted = ctx->emitted_break;
            ctx->break_label    = end_label;
            ctx->continue_label =
                loop->kind == HIR_FOR_CStyle ? update_label : cond_label;
            ctx->emitted_break = false;
            llvm_push_control_target(
                ctx,
                (LlvmControlTarget){
                    .symbol_handle    = loop->label_symbol,
                    .break_label      = end_label,
                    .continue_label   = ctx->continue_label,
                    .break_value_ptr  = (string){0},
                    .break_value_type = sema_no_type(),
                });
            if (!llvm_emit_effect_block(ctx, function, loop->body_block_index)) {
                llvm_pop_control_target(ctx, loop->label_symbol);
                ctx->break_label    = old_break;
                ctx->continue_label = old_continue;
                ctx->emitted_break  = old_break_emitted;
                return (LlvmValue){0};
            }
            bool loop_emitted_break = ctx->emitted_break;
            llvm_pop_control_target(ctx, loop->label_symbol);
            ctx->break_label    = old_break;
            ctx->continue_label = old_continue;
            ctx->emitted_break  = old_break_emitted;
            string next_label =
                loop->kind == HIR_FOR_CStyle ? update_label : cond_label;
            if (!ctx->block_terminated) {
                sb_format(ctx->sb,
                          "  br label %%" STRINGP "\n",
                          STRINGV(next_label));
            }

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

            bool can_reach_end = loop_emitted_break ||
                                 loop->condition_expr_index != U32_MAX;
            if (can_reach_end) {
                sb_format(ctx->sb, STRINGP ":\n", STRINGV(end_label));
                ctx->block_terminated = false;
            }
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
            if (llvm_type_kind(ctx->sema, operand.type_index) == STK_Pointer &&
                llvm_type_kind(ctx->sema, expr->type_index) == STK_Slice &&
                expr->extra_expr_index < array_count(ctx->hir->exprs)) {
                LlvmValue count =
                    llvm_emit_expr(ctx, function, expr->extra_expr_index);
                if (!count.ok) {
                    return (LlvmValue){0};
                }
                string count_value = count.value;
                if (llvm_integer_bits(ctx->sema, count.type_index) != 64) {
                    string count_type = llvm_type_string(ctx, count.type_index);
                    count_value       = llvm_temp(ctx);
                    sb_format(ctx->sb,
                              "  " STRINGP " = zext " STRINGP " " STRINGP
                              " to i64\n",
                              STRINGV(count_value),
                              STRINGV(count_type),
                              STRINGV(count.value));
                }
                string slice0 = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP
                          " = insertvalue { ptr, i64 } poison, ptr "
                          STRINGP ", 0\n",
                          STRINGV(slice0),
                          STRINGV(operand.value));
                string slice1 = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = insertvalue { ptr, i64 } "
                          STRINGP ", i64 " STRINGP ", 1\n",
                          STRINGV(slice1),
                          STRINGV(slice0),
                          STRINGV(count_value));
                return (LlvmValue){
                    .ok         = true,
                    .type_index = expr->type_index,
                    .value      = slice1,
                };
            }

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

            u32    dynarray_receiver = U32_MAX;
            string dynarray_method   = {0};
            if (llvm_dynamic_array_callee_method(ctx,
                                                 expr->callee_expr_index,
                                                 &dynarray_receiver,
                                                 &dynarray_method)) {
                if (string_eq_cstr(dynarray_method, "push")) {
                    return llvm_emit_dynamic_array_push(
                        ctx, function, expr, dynarray_receiver);
                }
                if (string_eq_cstr(dynarray_method, "reserve")) {
                    return llvm_emit_dynamic_array_reserve(
                        ctx, function, expr, dynarray_receiver);
                }
                if (string_eq_cstr(dynarray_method, "clear")) {
                    return llvm_emit_dynamic_array_clear(
                        ctx, function, expr, dynarray_receiver);
                }
                if (string_eq_cstr(dynarray_method, "free")) {
                    return llvm_emit_dynamic_array_free(
                        ctx, function, expr, dynarray_receiver);
                }
                if (string_eq_cstr(dynarray_method, "pop")) {
                    return llvm_emit_dynamic_array_pop(
                        ctx, function, expr, dynarray_receiver);
                }
                if (string_eq_cstr(dynarray_method, "delete")) {
                    return llvm_emit_dynamic_array_delete(
                        ctx, function, expr, dynarray_receiver, false);
                }
                if (string_eq_cstr(dynarray_method, "swap_delete")) {
                    return llvm_emit_dynamic_array_delete(
                        ctx, function, expr, dynarray_receiver, true);
                }
                if (string_eq_cstr(dynarray_method, "append")) {
                    return llvm_emit_dynamic_array_append(
                        ctx, function, expr, dynarray_receiver);
                }
                return (LlvmValue){0};
            }

            string callee = {0};
            if (!llvm_callee_name(ctx, function, expr->callee_expr_index, &callee)) {
                return (LlvmValue){0};
            }

            Array(LlvmValue) args = NULL;
            u32 callee_function_index = U32_MAX;
            const HirFunction* callee_function = NULL;
            if (llvm_callee_function_index(
                    ctx, expr->callee_expr_index, &callee_function_index)) {
                callee_function = &ctx->hir->functions[callee_function_index];
            }
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
            if (callee_function != NULL) {
                Array(LlvmValue) default_values = NULL;
                for (u32 i = 0; i < callee_function->param_count; ++i) {
                    const HirParam* param =
                        &ctx->hir->params[callee_function->first_param + i];
                    LlvmValue context_value = {0};
                    if (param->default_expr_index != U32_MAX) {
                        context_value = llvm_emit_expr(
                            ctx, function, param->default_expr_index);
                        if (!context_value.ok) {
                            array_free(default_values);
                            array_free(args);
                            return (LlvmValue){0};
                        }
                    } else if (i < array_count(args)) {
                        context_value = args[i];
                    }
                    array_push(default_values, context_value);
                    if (context_value.ok && param->local_index != U32_MAX) {
                        llvm_set_local_value(ctx,
                                             param->local_index,
                                             context_value);
                    }
                }

                for (u32 i = expr->arg_count; i < callee_function->param_count;
                     ++i) {
                    if (i >= array_count(default_values) ||
                        !default_values[i].ok) {
                        break;
                    }
                    array_push(args, default_values[i]);
                }
                array_free(default_values);
            }

            string return_type = llvm_type_string(ctx, expr->type_index);
            bool   returns_void =
                llvm_type_is_void(ctx->sema, expr->type_index);
            string temp = returns_void ? (string){0} : llvm_temp(ctx);
            if (returns_void) {
                sb_format(ctx->sb,
                          "  call " STRINGP " " STRINGP "(",
                          STRINGV(return_type),
                          STRINGV(callee));
            } else {
                sb_format(ctx->sb,
                          "  " STRINGP " = call " STRINGP " " STRINGP "(",
                          STRINGV(temp),
                          STRINGV(return_type),
                          STRINGV(callee));
            }
            for (u32 i = 0; i < array_count(args); ++i) {
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

    LlvmValue value = {0};
    const HirExpr* expr =
        stmt->expr_index < array_count(ctx->hir->exprs)
            ? &ctx->hir->exprs[stmt->expr_index]
            : NULL;
    if (expr != NULL && expr->kind == HIR_EXPR_Unsupported) {
        value = llvm_default_value(ctx, stmt->type_index);
    } else {
        value = llvm_emit_expr(ctx, function, stmt->expr_index);
    }
    if (!value.ok) {
        return false;
    }
    value = llvm_coerce_value_to_type(ctx, value, stmt->type_index);
    if (!value.ok) {
        return false;
    }

    if (llvm_local_is_assigned(ctx, stmt->local_index)) {
        LlvmLocalSlot* slot =
            llvm_ensure_local_slot(ctx, stmt->local_index, stmt->type_index);
        llvm_store_local_slot(ctx, slot, value);
        return true;
    }

    llvm_set_local_value(ctx, stmt->local_index, value);
    return true;
}

internal bool llvm_emit_assign(LlvmFunctionContext* ctx,
                               const HirFunction*   function,
                               u32                  target_expr_index,
                               LlvmValue            value)
{
    if (target_expr_index >= array_count(ctx->hir->exprs)) {
        return false;
    }

    const HirExpr* target = &ctx->hir->exprs[target_expr_index];

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

    if (target->kind == HIR_EXPR_TupleField ||
        target->kind == HIR_EXPR_Field) {
        LlvmValue record =
            llvm_emit_expr(ctx, function, target->operand_expr_index);
        if (!record.ok) {
            return false;
        }

        u32 field_index = llvm_field_index_for_value(ctx, target, record);
        if (field_index == U32_MAX ||
            llvm_type_kind(ctx->sema, record.type_index) == STK_Union) {
            return false;
        }

        string record_type = llvm_type_string(ctx, record.type_index);
        string value_type  = llvm_type_string(ctx, value.type_index);
        string updated     = llvm_temp(ctx);
        sb_format(ctx->sb,
                  "  " STRINGP " = insertvalue " STRINGP " " STRINGP ", "
                  STRINGP " " STRINGP ", %u\n",
                  STRINGV(updated),
                  STRINGV(record_type),
                  STRINGV(record.value),
                  STRINGV(value_type),
                  STRINGV(value.value),
                  field_index);
        return llvm_emit_assign(ctx,
                                function,
                                target->operand_expr_index,
                                (LlvmValue){
                                    .ok         = true,
                                    .type_index = record.type_index,
                                    .value      = updated,
                                });
    }

    if (target->kind != HIR_EXPR_LocalRef ||
        (target->ref_kind != HIR_REF_Local &&
         target->ref_kind != HIR_REF_Binding)) {
        return false;
    }

    if (target->ref_kind == HIR_REF_Binding &&
        target->ref_index < array_count(ctx->hir->bindings)) {
        const HirBinding* binding = &ctx->hir->bindings[target->ref_index];
        if (binding->kind != HIR_BINDING_Value ||
            binding->target_index >= array_count(ctx->hir->values)) {
            return false;
        }

        const HirValue* target_value = &ctx->hir->values[binding->target_index];
        if (target_value->kind != HIR_VALUE_Global) {
            return false;
        }

        string name = llvm_value_name_string(ctx->hir,
                                             ctx->lexer,
                                             ctx->arena,
                                             binding->target_index);
        if (name.count == 0) {
            return false;
        }

        string type = llvm_type_string(ctx, target_value->type_index);
        sb_format(ctx->sb,
                  "  store " STRINGP " " STRINGP ", ptr " STRINGP "\n",
                  STRINGV(type),
                  STRINGV(value.value),
                  STRINGV(name));
        return true;
    }

    u32 type_index = target->type_index != sema_no_type() ? target->type_index
                                                          : value.type_index;
    LlvmLocalSlot* slot =
        llvm_ensure_local_slot(ctx, target->ref_index, type_index);
    llvm_store_local_slot(ctx, slot, value);
    return true;
}

internal bool llvm_emit_assign_stmt(LlvmFunctionContext* ctx,
                                    const HirFunction*   function,
                                    const HirStmt*       stmt)
{
    LlvmValue value = llvm_emit_expr(ctx, function, stmt->expr_index);
    if (!value.ok) {
        return false;
    }
    return llvm_emit_assign(ctx, function, stmt->target_expr_index, value);
}

internal u32 llvm_stmt_source_line(const LlvmFunctionContext* ctx,
                                   const HirStmt*            stmt)
{
    (void)ctx;
    return stmt != NULL ? stmt->source_line : 0;
}

internal bool llvm_emit_assert(LlvmFunctionContext* ctx,
                               const HirFunction*   function,
                               const HirStmt*       stmt)
{
    if (stmt->expr_index == U32_MAX) {
        return true;
    }

    LlvmValue condition = llvm_emit_expr(ctx, function, stmt->expr_index);
    if (!condition.ok) {
        return false;
    }

    string message_value = {0};
    if (stmt->target_expr_index != U32_MAX) {
        LlvmValue message =
            llvm_emit_expr(ctx, function, stmt->target_expr_index);
        if (!message.ok) {
            return false;
        }
        message_value = message.value;
    } else {
        string default_message =
            llvm_assert_default_message_global_name_string(ctx->hir, ctx->arena);
        message_value = string_format(ctx->arena,
                                      "{ ptr " STRINGP ", i64 16 }",
                                      STRINGV(default_message));
    }

    string source_path =
        llvm_source_path_global_name_string(ctx->hir, ctx->arena);
    sb_format(ctx->sb,
              "  call void @nerd_assert(i1 " STRINGP
              ", ptr " STRINGP ", i32 %u, { ptr, i64 } " STRINGP ")\n",
              STRINGV(condition.value),
              STRINGV(source_path),
              llvm_stmt_source_line(ctx, stmt),
              STRINGV(message_value));
    return true;
}

internal bool llvm_emit_destructure(LlvmFunctionContext* ctx,
                                    const HirFunction*   function,
                                    const HirStmt*       stmt)
{
    LlvmValue value = llvm_emit_expr(ctx, function, stmt->expr_index);
    if (!value.ok) {
        return false;
    }

    string tuple_type = llvm_type_string(ctx, value.type_index);
    Array(LlvmValue) fields = NULL;
    for (u32 i = 0; i < stmt->body_block_index; ++i) {
        u32 item_index = stmt->target_expr_index + i;
        if (item_index >= array_count(ctx->hir->destructure_items)) {
            array_free(fields);
            return false;
        }

        const HirDestructureItem* item =
            &ctx->hir->destructure_items[item_index];
        string field = llvm_temp(ctx);
        sb_format(ctx->sb,
                  "  " STRINGP " = extractvalue " STRINGP " " STRINGP
                  ", %u\n",
                  STRINGV(field),
                  STRINGV(tuple_type),
                  STRINGV(value.value),
                  item->field_index);
        array_push(fields,
                   (LlvmValue){
                       .ok         = true,
                       .type_index = item->type_index,
                       .value      = field,
                   });
    }

    for (u32 i = 0; i < stmt->body_block_index; ++i) {
        const HirDestructureItem* item =
            &ctx->hir->destructure_items[stmt->target_expr_index + i];
        LlvmValue field = fields[i];
        if (stmt->kind == HIR_STMT_DestructureLet &&
            !llvm_local_is_assigned(ctx, item->local_index)) {
            llvm_set_local_value(ctx, item->local_index, field);
            continue;
        }

        LlvmLocalSlot* slot =
            llvm_ensure_local_slot(ctx, item->local_index, item->type_index);
        llvm_store_local_slot(ctx, slot, field);
    }

    array_free(fields);
    return true;
}

internal bool llvm_emit_effect_stmt(LlvmFunctionContext* ctx,
                                    const HirFunction*   function,
                                    const HirStmt*       stmt);
internal bool llvm_emit_return(LlvmFunctionContext* ctx,
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
        return llvm_emit_assign_stmt(ctx, function, stmt);
    case HIR_STMT_DestructureLet:
    case HIR_STMT_DestructureAssign:
        return llvm_emit_destructure(ctx, function, stmt);
    case HIR_STMT_Return:
        return llvm_emit_return(ctx, function, stmt);
    case HIR_STMT_Expr:
        if (stmt->expr_index == U32_MAX) {
            return true;
        }
        return llvm_emit_expr(ctx, function, stmt->expr_index).ok;
    case HIR_STMT_Assert:
        return llvm_emit_assert(ctx, function, stmt);
    case HIR_STMT_Defer:
        return true;
    case HIR_STMT_Break:
        {
            string break_label      = ctx->break_label;
            string break_value_ptr  = ctx->break_value_ptr;
            u32    break_value_type = ctx->break_value_type;
            LlvmControlTarget* target =
                llvm_find_control_target(ctx, stmt->symbol_handle);
            if (target != NULL) {
                break_label      = target->break_label;
                break_value_ptr  = target->break_value_ptr;
                break_value_type = target->break_value_type;
            }
            if (break_label.count == 0) {
                return false;
            }
            if (stmt->expr_index != U32_MAX && break_value_ptr.count > 0) {
                LlvmValue value = llvm_emit_expr(ctx, function, stmt->expr_index);
                if (!value.ok) {
                    return false;
                }
                string type = llvm_type_string(ctx, break_value_type);
                sb_format(ctx->sb,
                          "  store " STRINGP " " STRINGP ", ptr " STRINGP
                          ", align 4\n",
                          STRINGV(type),
                          STRINGV(value.value),
                          STRINGV(break_value_ptr));
            }
            sb_format(ctx->sb,
                      "  br label %%" STRINGP "\n",
                      STRINGV(break_label));
            ctx->block_terminated = true;
            ctx->emitted_break     = true;
            return true;
        }
    case HIR_STMT_Continue:
        {
            string continue_label = ctx->continue_label;
            LlvmControlTarget* target =
                llvm_find_control_target(ctx, stmt->symbol_handle);
            if (target != NULL) {
                continue_label = target->continue_label;
            }
            if (continue_label.count == 0) {
                return false;
            }
            sb_format(ctx->sb,
                      "  br label %%" STRINGP "\n",
                      STRINGV(continue_label));
            ctx->block_terminated = true;
            return true;
        }
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

internal void llvm_bind_block_function_values(LlvmFunctionContext* ctx,
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
        if (stmt->kind != HIR_STMT_Let ||
            stmt->local_index == U32_MAX ||
            stmt->expr_index >= array_count(ctx->hir->exprs)) {
            continue;
        }

        const HirExpr* expr = &ctx->hir->exprs[stmt->expr_index];
        if (expr->kind != HIR_EXPR_FunctionRef ||
            expr->ref_index >= array_count(ctx->hir->functions)) {
            continue;
        }

        llvm_set_local_value(
            ctx,
            stmt->local_index,
            (LlvmValue){
                .ok         = true,
                .type_index = stmt->type_index,
                .value      = llvm_function_name_string(
                    ctx->hir, ctx->lexer, ctx->arena, expr->ref_index),
            });
    }
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
        if (stmt->kind == HIR_STMT_DestructureAssign) {
            for (u32 j = 0; j < stmt->body_block_index; ++j) {
                u32 item_index = stmt->target_expr_index + j;
                if (item_index < array_count(ctx->hir->destructure_items)) {
                    llvm_mark_assigned_local(
                        ctx,
                        ctx->hir->destructure_items[item_index].local_index);
                }
            }
        }

        if (stmt->kind == HIR_STMT_Block) {
            llvm_collect_assigned_locals(ctx, stmt->body_block_index);
        }

        if (stmt->expr_index < array_count(ctx->hir->exprs)) {
            const HirExpr* expr = &ctx->hir->exprs[stmt->expr_index];
            if (expr->kind == HIR_EXPR_Block) {
                llvm_collect_assigned_locals(ctx, expr->body_block_index);
            }
            if (expr->kind == HIR_EXPR_On) {
                for (u32 j = 0; j < expr->branch_count; ++j) {
                    u32 branch_index = expr->first_branch + j;
                    if (branch_index < array_count(ctx->hir->on_branches)) {
                        llvm_collect_assigned_locals(
                            ctx,
                            ctx->hir->on_branches[branch_index].body_block_index);
                    }
                }
            }
            if (expr->kind == HIR_EXPR_Assign &&
                expr->lhs_expr_index < array_count(ctx->hir->exprs)) {
                const HirExpr* target =
                    &ctx->hir->exprs[expr->lhs_expr_index];
                if (target->kind == HIR_EXPR_LocalRef &&
                    target->ref_kind == HIR_REF_Local) {
                    llvm_mark_assigned_local(ctx, target->ref_index);
                }
            }
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

    llvm_bind_block_function_values(ctx, block_index);

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
            if (!llvm_emit_assign_stmt(ctx, function, stmt)) {
                return false;
            }
            continue;
        } else if (stmt->kind == HIR_STMT_DestructureLet ||
                   stmt->kind == HIR_STMT_DestructureAssign) {
            if (!llvm_emit_destructure(ctx, function, stmt)) {
                return false;
            }
            continue;
        } else if (stmt->kind == HIR_STMT_Return) {
            return llvm_emit_return(ctx, function, stmt);
        } else if (stmt->kind == HIR_STMT_Expr) {
            if (stmt->expr_index != U32_MAX) {
                const HirExpr* expr = stmt->expr_index < array_count(ctx->hir->exprs)
                                          ? &ctx->hir->exprs[stmt->expr_index]
                                          : NULL;
                if (expr != NULL && expr->kind == HIR_EXPR_Unsupported) {
                    continue;
                }
                LlvmValue value = llvm_emit_expr(ctx, function, stmt->expr_index);
                if (!value.ok) {
                    return false;
                }
            }
            continue;
        } else if (stmt->kind == HIR_STMT_Assert) {
            if (!llvm_emit_assert(ctx, function, stmt)) {
                return false;
            }
            continue;
        } else if (stmt->kind == HIR_STMT_Defer) {
            continue;
        } else if (stmt->kind == HIR_STMT_Break) {
            if (!llvm_emit_effect_stmt(ctx, function, stmt)) {
                return false;
            }
            return true;
        } else if (stmt->kind == HIR_STMT_Continue) {
            if (!llvm_emit_effect_stmt(ctx, function, stmt)) {
                return false;
            }
            return true;
        } else if (stmt->kind == HIR_STMT_Block) {
            if (!llvm_emit_block(ctx, function, stmt->body_block_index)) {
                return false;
            }
            continue;
        }
    }

    return true;
}

internal u64 llvm_type_storage_bytes(const Sema* sema, u32 type_index)
{
    u32 bits = llvm_type_storage_bits(sema, type_index);
    if (bits == 0) {
        return 1;
    }
    return (bits + 7) / 8;
}

internal string llvm_dynamic_array_header_field_ptr(LlvmFunctionContext* ctx,
                                                    string               header,
                                                    u32                  field_index)
{
    string ptr = llvm_temp(ctx);
    string header_type = llvm_dynamic_array_header_type();
    sb_format(ctx->sb,
              "  " STRINGP
              " = getelementptr inbounds " STRINGP ", ptr " STRINGP
              ", i64 0, i32 %u\n",
              STRINGV(ptr),
              STRINGV(header_type),
              STRINGV(header),
              field_index);
    return ptr;
}

internal string llvm_dynamic_array_load_header_field(LlvmFunctionContext* ctx,
                                                     string               header,
                                                     u32                  field_index,
                                                     string               type)
{
    string field_ptr =
        llvm_dynamic_array_header_field_ptr(ctx, header, field_index);
    string value = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = load " STRINGP ", ptr " STRINGP "\n",
              STRINGV(value),
              STRINGV(type),
              STRINGV(field_ptr));
    return value;
}

internal bool llvm_dynamic_array_ensure_header(LlvmFunctionContext* ctx,
                                               string               slot,
                                               string*              out_header)
{
    string initial = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = load ptr, ptr " STRINGP "\n",
              STRINGV(initial),
              STRINGV(slot));

    string is_null = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = icmp eq ptr " STRINGP ", null\n",
              STRINGV(is_null),
              STRINGV(initial));

    string alloc_label = llvm_label(ctx, "dynarray.alloc");
    string done_label  = llvm_label(ctx, "dynarray.ready");
    sb_format(ctx->sb,
              "  br i1 " STRINGP ", label %%" STRINGP ", label %%"
              STRINGP "\n",
              STRINGV(is_null),
              STRINGV(alloc_label),
              STRINGV(done_label));

    sb_format(ctx->sb, STRINGP ":\n", STRINGV(alloc_label));
    string allocated = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = call ptr @malloc(i64 24)\n",
              STRINGV(allocated));
    string data_ptr = llvm_dynamic_array_header_field_ptr(ctx, allocated, 0);
    string count_ptr = llvm_dynamic_array_header_field_ptr(ctx, allocated, 1);
    string capacity_ptr =
        llvm_dynamic_array_header_field_ptr(ctx, allocated, 2);
    sb_format(ctx->sb,
              "  store ptr null, ptr " STRINGP "\n"
              "  store i64 0, ptr " STRINGP "\n"
              "  store i64 0, ptr " STRINGP "\n"
              "  store ptr " STRINGP ", ptr " STRINGP "\n"
              "  br label %%" STRINGP "\n",
              STRINGV(data_ptr),
              STRINGV(count_ptr),
              STRINGV(capacity_ptr),
              STRINGV(allocated),
              STRINGV(slot),
              STRINGV(done_label));

    sb_format(ctx->sb, STRINGP ":\n", STRINGV(done_label));
    string header = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = load ptr, ptr " STRINGP "\n",
              STRINGV(header),
              STRINGV(slot));
    *out_header = header;
    ctx->block_terminated = false;
    return true;
}

internal LlvmValue llvm_emit_dynamic_array_field(LlvmFunctionContext* ctx,
                                                 LlvmValue            target,
                                                 const HirExpr*       expr)
{
    string field = lex_symbol(ctx->lexer, expr->symbol_handle);
    u32    field_index = U32_MAX;
    string field_type  = {0};
    string empty_value = {0};
    if (string_eq_cstr(field, "data")) {
        field_index = 0;
        field_type  = s("ptr");
        empty_value = s("null");
    } else if (string_eq_cstr(field, "count")) {
        field_index = 1;
        field_type  = s("i64");
        empty_value = s("0");
    } else if (string_eq_cstr(field, "capacity")) {
        field_index = 2;
        field_type  = s("i64");
        empty_value = s("0");
    } else {
        return (LlvmValue){0};
    }

    string result = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = alloca " STRINGP "\n",
              STRINGV(result),
              STRINGV(field_type));

    string is_null = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = icmp eq ptr " STRINGP ", null\n",
              STRINGV(is_null),
              STRINGV(target.value));
    string empty_label = llvm_label(ctx, "dynarray.field.empty");
    string load_label  = llvm_label(ctx, "dynarray.field.load");
    string done_label  = llvm_label(ctx, "dynarray.field.done");
    sb_format(ctx->sb,
              "  br i1 " STRINGP ", label %%" STRINGP ", label %%"
              STRINGP "\n",
              STRINGV(is_null),
              STRINGV(empty_label),
              STRINGV(load_label));

    sb_format(ctx->sb, STRINGP ":\n", STRINGV(empty_label));
    sb_format(ctx->sb,
              "  store " STRINGP " " STRINGP ", ptr " STRINGP "\n"
              "  br label %%" STRINGP "\n",
              STRINGV(field_type),
              STRINGV(empty_value),
              STRINGV(result),
              STRINGV(done_label));

    sb_format(ctx->sb, STRINGP ":\n", STRINGV(load_label));
    string loaded =
        llvm_dynamic_array_load_header_field(ctx, target.value, field_index, field_type);
    sb_format(ctx->sb,
              "  store " STRINGP " " STRINGP ", ptr " STRINGP "\n"
              "  br label %%" STRINGP "\n",
              STRINGV(field_type),
              STRINGV(loaded),
              STRINGV(result),
              STRINGV(done_label));

    sb_format(ctx->sb, STRINGP ":\n", STRINGV(done_label));
    string value = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = load " STRINGP ", ptr " STRINGP "\n",
              STRINGV(value),
              STRINGV(field_type),
              STRINGV(result));
    ctx->block_terminated = false;
    return (LlvmValue){
        .ok         = true,
        .type_index = expr->type_index,
        .value      = value,
    };
}

internal bool llvm_dynamic_array_callee_method(LlvmFunctionContext* ctx,
                                               u32                  callee_expr_index,
                                               u32*                 receiver_expr_index,
                                               string*              method)
{
    if (callee_expr_index >= array_count(ctx->hir->exprs)) {
        return false;
    }
    const HirExpr* callee = &ctx->hir->exprs[callee_expr_index];
    if (callee->kind != HIR_EXPR_Field ||
        callee->operand_expr_index >= array_count(ctx->hir->exprs)) {
        return false;
    }
    const HirExpr* receiver = &ctx->hir->exprs[callee->operand_expr_index];
    if (llvm_type_kind(ctx->sema, receiver->type_index) != STK_DynamicArray) {
        return false;
    }
    *receiver_expr_index = callee->operand_expr_index;
    *method              = lex_symbol(ctx->lexer, callee->symbol_handle);
    return true;
}

internal LlvmValue llvm_emit_dynamic_array_push(LlvmFunctionContext* ctx,
                                                const HirFunction*   function,
                                                const HirExpr*       call,
                                                u32                  receiver_expr_index)
{
    if (call->arg_count != 1 || receiver_expr_index >= array_count(ctx->hir->exprs)) {
        return (LlvmValue){0};
    }
    const HirExpr* receiver = &ctx->hir->exprs[receiver_expr_index];
    u32 item_type = llvm_collection_item_type(ctx->sema, receiver->type_index);
    if (item_type == sema_no_type()) {
        return (LlvmValue){0};
    }

    LlvmValue slot =
        llvm_address_of_expr(ctx, function, receiver_expr_index);
    if (!slot.ok) {
        return (LlvmValue){0};
    }

    string header = {0};
    if (!llvm_dynamic_array_ensure_header(ctx, slot.value, &header)) {
        return (LlvmValue){0};
    }

    const HirCallArg* arg = &ctx->hir->call_args[call->first_arg];
    LlvmValue item = llvm_emit_expr(ctx, function, arg->expr_index);
    if (!item.ok) {
        return (LlvmValue){0};
    }
    item = llvm_coerce_value_to_type(ctx, item, item_type);
    if (!item.ok) {
        return (LlvmValue){0};
    }

    string data_ptr_ptr = llvm_dynamic_array_header_field_ptr(ctx, header, 0);
    string count_ptr    = llvm_dynamic_array_header_field_ptr(ctx, header, 1);
    string capacity_ptr = llvm_dynamic_array_header_field_ptr(ctx, header, 2);
    string data         = llvm_temp(ctx);
    string count        = llvm_temp(ctx);
    string capacity     = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = load ptr, ptr " STRINGP "\n"
              "  " STRINGP " = load i64, ptr " STRINGP "\n"
              "  " STRINGP " = load i64, ptr " STRINGP "\n",
              STRINGV(data),
              STRINGV(data_ptr_ptr),
              STRINGV(count),
              STRINGV(count_ptr),
              STRINGV(capacity),
              STRINGV(capacity_ptr));

    string needed = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = add i64 " STRINGP ", 1\n",
              STRINGV(needed),
              STRINGV(count));

    string grows = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = icmp ugt i64 " STRINGP ", " STRINGP "\n",
              STRINGV(grows),
              STRINGV(needed),
              STRINGV(capacity));
    string grow_label  = llvm_label(ctx, "dynarray.grow");
    string store_label = llvm_label(ctx, "dynarray.store");
    sb_format(ctx->sb,
              "  br i1 " STRINGP ", label %%" STRINGP ", label %%"
              STRINGP "\n",
              STRINGV(grows),
              STRINGV(grow_label),
              STRINGV(store_label));

    sb_format(ctx->sb, STRINGP ":\n", STRINGV(grow_label));
    string has_capacity = llvm_temp(ctx);
    string doubled      = llvm_temp(ctx);
    string new_capacity = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = icmp eq i64 " STRINGP ", 0\n"
              "  " STRINGP " = mul i64 " STRINGP ", 2\n"
              "  " STRINGP " = select i1 " STRINGP ", i64 1, i64 "
              STRINGP "\n",
              STRINGV(has_capacity),
              STRINGV(capacity),
              STRINGV(doubled),
              STRINGV(capacity),
              STRINGV(new_capacity),
              STRINGV(has_capacity),
              STRINGV(doubled));
    u64 item_size = llvm_type_storage_bytes(ctx->sema, item_type);
    string byte_count = llvm_temp(ctx);
    string grown_data = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = mul i64 " STRINGP ", %llu\n"
              "  " STRINGP " = call ptr @realloc(ptr " STRINGP
              ", i64 " STRINGP ")\n"
              "  store ptr " STRINGP ", ptr " STRINGP "\n"
              "  store i64 " STRINGP ", ptr " STRINGP "\n"
              "  br label %%" STRINGP "\n",
              STRINGV(byte_count),
              STRINGV(new_capacity),
              (unsigned long long)item_size,
              STRINGV(grown_data),
              STRINGV(data),
              STRINGV(byte_count),
              STRINGV(grown_data),
              STRINGV(data_ptr_ptr),
              STRINGV(new_capacity),
              STRINGV(capacity_ptr),
              STRINGV(store_label));

    sb_format(ctx->sb, STRINGP ":\n", STRINGV(store_label));
    string current_data = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = load ptr, ptr " STRINGP "\n",
              STRINGV(current_data),
              STRINGV(data_ptr_ptr));
    string item_type_string = llvm_type_string(ctx, item_type);
    string item_ptr         = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = getelementptr inbounds " STRINGP
              ", ptr " STRINGP ", i64 " STRINGP "\n",
              STRINGV(item_ptr),
              STRINGV(item_type_string),
              STRINGV(current_data),
              STRINGV(count));
    string value_type = llvm_type_string(ctx, item.type_index);
    sb_format(ctx->sb,
              "  store " STRINGP " " STRINGP ", ptr " STRINGP "\n"
              "  store i64 " STRINGP ", ptr " STRINGP "\n",
              STRINGV(value_type),
              STRINGV(item.value),
              STRINGV(item_ptr),
              STRINGV(needed),
              STRINGV(count_ptr));

    ctx->block_terminated = false;
    return (LlvmValue){
        .ok         = true,
        .type_index = call->type_index,
        .value      = s(""),
    };
}

internal LlvmValue llvm_emit_dynamic_array_reserve(LlvmFunctionContext* ctx,
                                                   const HirFunction*   function,
                                                   const HirExpr*       call,
                                                   u32 receiver_expr_index)
{
    if (call->arg_count != 1 ||
        receiver_expr_index >= array_count(ctx->hir->exprs)) {
        return (LlvmValue){0};
    }
    const HirExpr* receiver = &ctx->hir->exprs[receiver_expr_index];
    u32 item_type = llvm_collection_item_type(ctx->sema, receiver->type_index);
    if (item_type == sema_no_type()) {
        return (LlvmValue){0};
    }

    const HirCallArg* arg = &ctx->hir->call_args[call->first_arg];
    LlvmValue requested = llvm_emit_expr(ctx, function, arg->expr_index);
    if (!requested.ok) {
        return (LlvmValue){0};
    }
    string requested_type = llvm_type_string(ctx, requested.type_index);
    string requested_i64  = requested.value;
    if (!string_eq_cstr(requested_type, "i64")) {
        requested_i64 = llvm_temp(ctx);
        sb_format(ctx->sb,
                  "  " STRINGP " = zext " STRINGP " " STRINGP " to i64\n",
                  STRINGV(requested_i64),
                  STRINGV(requested_type),
                  STRINGV(requested.value));
    }

    LlvmValue slot =
        llvm_address_of_expr(ctx, function, receiver_expr_index);
    if (!slot.ok) {
        return (LlvmValue){0};
    }

    string header = {0};
    if (!llvm_dynamic_array_ensure_header(ctx, slot.value, &header)) {
        return (LlvmValue){0};
    }

    string data_ptr_ptr = llvm_dynamic_array_header_field_ptr(ctx, header, 0);
    string capacity_ptr = llvm_dynamic_array_header_field_ptr(ctx, header, 2);
    string data         = llvm_temp(ctx);
    string capacity     = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = load ptr, ptr " STRINGP "\n"
              "  " STRINGP " = load i64, ptr " STRINGP "\n",
              STRINGV(data),
              STRINGV(data_ptr_ptr),
              STRINGV(capacity),
              STRINGV(capacity_ptr));

    string grows = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = icmp ugt i64 " STRINGP ", " STRINGP "\n",
              STRINGV(grows),
              STRINGV(requested_i64),
              STRINGV(capacity));
    string grow_label = llvm_label(ctx, "dynarray.reserve.grow");
    string done_label = llvm_label(ctx, "dynarray.reserve.done");
    sb_format(ctx->sb,
              "  br i1 " STRINGP ", label %%" STRINGP ", label %%"
              STRINGP "\n",
              STRINGV(grows),
              STRINGV(grow_label),
              STRINGV(done_label));

    sb_format(ctx->sb, STRINGP ":\n", STRINGV(grow_label));
    u64 item_size  = llvm_type_storage_bytes(ctx->sema, item_type);
    string bytes   = llvm_temp(ctx);
    string new_data = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = mul i64 " STRINGP ", %llu\n"
              "  " STRINGP " = call ptr @realloc(ptr " STRINGP
              ", i64 " STRINGP ")\n"
              "  store ptr " STRINGP ", ptr " STRINGP "\n"
              "  store i64 " STRINGP ", ptr " STRINGP "\n"
              "  br label %%" STRINGP "\n",
              STRINGV(bytes),
              STRINGV(requested_i64),
              (unsigned long long)item_size,
              STRINGV(new_data),
              STRINGV(data),
              STRINGV(bytes),
              STRINGV(new_data),
              STRINGV(data_ptr_ptr),
              STRINGV(requested_i64),
              STRINGV(capacity_ptr),
              STRINGV(done_label));

    sb_format(ctx->sb, STRINGP ":\n", STRINGV(done_label));
    ctx->block_terminated = false;
    return (LlvmValue){
        .ok         = true,
        .type_index = call->type_index,
        .value      = s(""),
    };
}

internal LlvmValue llvm_emit_dynamic_array_clear(LlvmFunctionContext* ctx,
                                                 const HirFunction*   function,
                                                 const HirExpr*       call,
                                                 u32 receiver_expr_index)
{
    if (call->arg_count != 0 ||
        receiver_expr_index >= array_count(ctx->hir->exprs)) {
        return (LlvmValue){0};
    }
    LlvmValue slot =
        llvm_address_of_expr(ctx, function, receiver_expr_index);
    if (!slot.ok) {
        return (LlvmValue){0};
    }

    string header = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = load ptr, ptr " STRINGP "\n",
              STRINGV(header),
              STRINGV(slot.value));
    string is_null = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = icmp eq ptr " STRINGP ", null\n",
              STRINGV(is_null),
              STRINGV(header));
    string clear_label = llvm_label(ctx, "dynarray.clear");
    string done_label  = llvm_label(ctx, "dynarray.clear.done");
    sb_format(ctx->sb,
              "  br i1 " STRINGP ", label %%" STRINGP ", label %%"
              STRINGP "\n",
              STRINGV(is_null),
              STRINGV(done_label),
              STRINGV(clear_label));

    sb_format(ctx->sb, STRINGP ":\n", STRINGV(clear_label));
    string count_ptr = llvm_dynamic_array_header_field_ptr(ctx, header, 1);
    sb_format(ctx->sb,
              "  store i64 0, ptr " STRINGP "\n"
              "  br label %%" STRINGP "\n",
              STRINGV(count_ptr),
              STRINGV(done_label));

    sb_format(ctx->sb, STRINGP ":\n", STRINGV(done_label));
    ctx->block_terminated = false;
    return (LlvmValue){
        .ok         = true,
        .type_index = call->type_index,
        .value      = s(""),
    };
}

internal LlvmValue llvm_emit_dynamic_array_free(LlvmFunctionContext* ctx,
                                                const HirFunction*   function,
                                                const HirExpr*       call,
                                                u32 receiver_expr_index)
{
    if (call->arg_count != 0 ||
        receiver_expr_index >= array_count(ctx->hir->exprs)) {
        return (LlvmValue){0};
    }
    LlvmValue slot =
        llvm_address_of_expr(ctx, function, receiver_expr_index);
    if (!slot.ok) {
        return (LlvmValue){0};
    }

    string header = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = load ptr, ptr " STRINGP "\n",
              STRINGV(header),
              STRINGV(slot.value));
    string is_null = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = icmp eq ptr " STRINGP ", null\n",
              STRINGV(is_null),
              STRINGV(header));
    string free_label = llvm_label(ctx, "dynarray.free");
    string done_label = llvm_label(ctx, "dynarray.free.done");
    sb_format(ctx->sb,
              "  br i1 " STRINGP ", label %%" STRINGP ", label %%"
              STRINGP "\n",
              STRINGV(is_null),
              STRINGV(done_label),
              STRINGV(free_label));

    sb_format(ctx->sb, STRINGP ":\n", STRINGV(free_label));
    string data = llvm_dynamic_array_load_header_field(ctx, header, 0, s("ptr"));
    sb_format(ctx->sb,
              "  call void @free(ptr " STRINGP ")\n"
              "  call void @free(ptr " STRINGP ")\n"
              "  store ptr null, ptr " STRINGP "\n"
              "  br label %%" STRINGP "\n",
              STRINGV(data),
              STRINGV(header),
              STRINGV(slot.value),
              STRINGV(done_label));

    sb_format(ctx->sb, STRINGP ":\n", STRINGV(done_label));
    ctx->block_terminated = false;
    return (LlvmValue){
        .ok         = true,
        .type_index = call->type_index,
        .value      = s(""),
    };
}

internal LlvmValue llvm_emit_dynamic_array_pop(LlvmFunctionContext* ctx,
                                               const HirFunction*   function,
                                               const HirExpr*       call,
                                               u32 receiver_expr_index)
{
    (void)function;
    if (call->arg_count != 0 ||
        receiver_expr_index >= array_count(ctx->hir->exprs)) {
        return (LlvmValue){0};
    }
    const HirExpr* receiver = &ctx->hir->exprs[receiver_expr_index];
    u32 item_type = llvm_collection_item_type(ctx->sema, receiver->type_index);
    if (item_type == sema_no_type()) {
        return (LlvmValue){0};
    }

    LlvmValue target = llvm_emit_expr(ctx, function, receiver_expr_index);
    if (!target.ok) {
        return (LlvmValue){0};
    }

    string data = llvm_dynamic_array_load_header_field(ctx, target.value, 0, s("ptr"));
    string count_ptr = llvm_dynamic_array_header_field_ptr(ctx, target.value, 1);
    string count     = llvm_temp(ctx);
    string last      = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = load i64, ptr " STRINGP "\n"
              "  " STRINGP " = sub i64 " STRINGP ", 1\n",
              STRINGV(count),
              STRINGV(count_ptr),
              STRINGV(last),
              STRINGV(count));

    string item_type_string = llvm_type_string(ctx, item_type);
    string item_ptr         = llvm_temp(ctx);
    string item             = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = getelementptr inbounds " STRINGP
              ", ptr " STRINGP ", i64 " STRINGP "\n"
              "  " STRINGP " = load " STRINGP ", ptr " STRINGP "\n"
              "  store i64 " STRINGP ", ptr " STRINGP "\n",
              STRINGV(item_ptr),
              STRINGV(item_type_string),
              STRINGV(data),
              STRINGV(last),
              STRINGV(item),
              STRINGV(item_type_string),
              STRINGV(item_ptr),
              STRINGV(last),
              STRINGV(count_ptr));

    return (LlvmValue){
        .ok         = true,
        .type_index = call->type_index,
        .value      = item,
    };
}

internal LlvmValue llvm_emit_dynamic_array_delete(LlvmFunctionContext* ctx,
                                                  const HirFunction*   function,
                                                  const HirExpr*       call,
                                                  u32 receiver_expr_index,
                                                  bool                 swap)
{
    if (call->arg_count != 1 ||
        receiver_expr_index >= array_count(ctx->hir->exprs)) {
        return (LlvmValue){0};
    }
    const HirExpr* receiver = &ctx->hir->exprs[receiver_expr_index];
    u32 item_type = llvm_collection_item_type(ctx->sema, receiver->type_index);
    if (item_type == sema_no_type()) {
        return (LlvmValue){0};
    }

    LlvmValue target = llvm_emit_expr(ctx, function, receiver_expr_index);
    const HirCallArg* arg = &ctx->hir->call_args[call->first_arg];
    LlvmValue index = llvm_emit_expr(ctx, function, arg->expr_index);
    if (!target.ok || !index.ok) {
        return (LlvmValue){0};
    }

    string data = llvm_dynamic_array_load_header_field(ctx, target.value, 0, s("ptr"));
    string count_ptr = llvm_dynamic_array_header_field_ptr(ctx, target.value, 1);
    string count     = llvm_temp(ctx);
    string last      = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = load i64, ptr " STRINGP "\n"
              "  " STRINGP " = sub i64 " STRINGP ", 1\n",
              STRINGV(count),
              STRINGV(count_ptr),
              STRINGV(last),
              STRINGV(count));

    string index_type = llvm_type_string(ctx, index.type_index);
    string index_i64  = index.value;
    if (!string_eq_cstr(index_type, "i64")) {
        index_i64 = llvm_temp(ctx);
        sb_format(ctx->sb,
                  "  " STRINGP " = zext " STRINGP " " STRINGP " to i64\n",
                  STRINGV(index_i64),
                  STRINGV(index_type),
                  STRINGV(index.value));
    }

    string item_type_string = llvm_type_string(ctx, item_type);
    if (swap) {
        string source_ptr = llvm_temp(ctx);
        string target_ptr = llvm_temp(ctx);
        string value      = llvm_temp(ctx);
        sb_format(ctx->sb,
                  "  " STRINGP " = getelementptr inbounds " STRINGP
                  ", ptr " STRINGP ", i64 " STRINGP "\n"
                  "  " STRINGP " = getelementptr inbounds " STRINGP
                  ", ptr " STRINGP ", i64 " STRINGP "\n"
                  "  " STRINGP " = load " STRINGP ", ptr " STRINGP "\n"
                  "  store " STRINGP " " STRINGP ", ptr " STRINGP "\n"
                  "  store i64 " STRINGP ", ptr " STRINGP "\n",
                  STRINGV(source_ptr),
                  STRINGV(item_type_string),
                  STRINGV(data),
                  STRINGV(last),
                  STRINGV(target_ptr),
                  STRINGV(item_type_string),
                  STRINGV(data),
                  STRINGV(index_i64),
                  STRINGV(value),
                  STRINGV(item_type_string),
                  STRINGV(source_ptr),
                  STRINGV(item_type_string),
                  STRINGV(value),
                  STRINGV(target_ptr),
                  STRINGV(last),
                  STRINGV(count_ptr));
    } else {
        string index_slot = llvm_temp(ctx);
        sb_format(ctx->sb,
                  "  " STRINGP " = alloca i64\n"
                  "  store i64 " STRINGP ", ptr " STRINGP "\n",
                  STRINGV(index_slot),
                  STRINGV(index_i64),
                  STRINGV(index_slot));

        string loop_label = llvm_label(ctx, "dynarray.delete.loop");
        string body_label = llvm_label(ctx, "dynarray.delete.body");
        string done_label = llvm_label(ctx, "dynarray.delete.done");
        sb_format(ctx->sb,
                  "  br label %%" STRINGP "\n"
                  STRINGP ":\n",
                  STRINGV(loop_label),
                  STRINGV(loop_label));

        string cursor = llvm_temp(ctx);
        string more   = llvm_temp(ctx);
        sb_format(ctx->sb,
                  "  " STRINGP " = load i64, ptr " STRINGP "\n"
                  "  " STRINGP " = icmp ult i64 " STRINGP ", " STRINGP "\n"
                  "  br i1 " STRINGP ", label %%" STRINGP ", label %%"
                  STRINGP "\n",
                  STRINGV(cursor),
                  STRINGV(index_slot),
                  STRINGV(more),
                  STRINGV(cursor),
                  STRINGV(last),
                  STRINGV(more),
                  STRINGV(body_label),
                  STRINGV(done_label));

        sb_format(ctx->sb, STRINGP ":\n", STRINGV(body_label));
        string next       = llvm_temp(ctx);
        string source_ptr = llvm_temp(ctx);
        string target_ptr = llvm_temp(ctx);
        string value      = llvm_temp(ctx);
        sb_format(ctx->sb,
                  "  " STRINGP " = add i64 " STRINGP ", 1\n"
                  "  " STRINGP " = getelementptr inbounds " STRINGP
                  ", ptr " STRINGP ", i64 " STRINGP "\n"
                  "  " STRINGP " = getelementptr inbounds " STRINGP
                  ", ptr " STRINGP ", i64 " STRINGP "\n"
                  "  " STRINGP " = load " STRINGP ", ptr " STRINGP "\n"
                  "  store " STRINGP " " STRINGP ", ptr " STRINGP "\n"
                  "  store i64 " STRINGP ", ptr " STRINGP "\n"
                  "  br label %%" STRINGP "\n",
                  STRINGV(next),
                  STRINGV(cursor),
                  STRINGV(source_ptr),
                  STRINGV(item_type_string),
                  STRINGV(data),
                  STRINGV(next),
                  STRINGV(target_ptr),
                  STRINGV(item_type_string),
                  STRINGV(data),
                  STRINGV(cursor),
                  STRINGV(value),
                  STRINGV(item_type_string),
                  STRINGV(source_ptr),
                  STRINGV(item_type_string),
                  STRINGV(value),
                  STRINGV(target_ptr),
                  STRINGV(next),
                  STRINGV(index_slot),
                  STRINGV(loop_label));

        sb_format(ctx->sb, STRINGP ":\n", STRINGV(done_label));
        sb_format(ctx->sb,
                  "  store i64 " STRINGP ", ptr " STRINGP "\n",
                  STRINGV(last),
                  STRINGV(count_ptr));
        ctx->block_terminated = false;
    }

    return (LlvmValue){
        .ok         = true,
        .type_index = call->type_index,
        .value      = s(""),
    };
}

internal LlvmValue llvm_emit_dynamic_array_append(LlvmFunctionContext* ctx,
                                                  const HirFunction*   function,
                                                  const HirExpr*       call,
                                                  u32 receiver_expr_index)
{
    if (call->arg_count != 1 ||
        receiver_expr_index >= array_count(ctx->hir->exprs)) {
        return (LlvmValue){0};
    }
    const HirExpr* receiver = &ctx->hir->exprs[receiver_expr_index];
    u32 item_type = llvm_collection_item_type(ctx->sema, receiver->type_index);
    if (item_type == sema_no_type()) {
        return (LlvmValue){0};
    }

    const HirCallArg* arg = &ctx->hir->call_args[call->first_arg];
    LlvmValue source = llvm_emit_expr(ctx, function, arg->expr_index);
    if (!source.ok || llvm_type_kind(ctx->sema, source.type_index) != STK_Slice) {
        return (LlvmValue){0};
    }
    string source_type = llvm_type_string(ctx, source.type_index);
    string source_data = llvm_temp(ctx);
    string source_count = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = extractvalue " STRINGP " " STRINGP ", 0\n"
              "  " STRINGP " = extractvalue " STRINGP " " STRINGP ", 1\n",
              STRINGV(source_data),
              STRINGV(source_type),
              STRINGV(source.value),
              STRINGV(source_count),
              STRINGV(source_type),
              STRINGV(source.value));

    LlvmValue slot =
        llvm_address_of_expr(ctx, function, receiver_expr_index);
    if (!slot.ok) {
        return (LlvmValue){0};
    }

    string header = {0};
    if (!llvm_dynamic_array_ensure_header(ctx, slot.value, &header)) {
        return (LlvmValue){0};
    }

    string data_ptr_ptr = llvm_dynamic_array_header_field_ptr(ctx, header, 0);
    string count_ptr    = llvm_dynamic_array_header_field_ptr(ctx, header, 1);
    string capacity_ptr = llvm_dynamic_array_header_field_ptr(ctx, header, 2);
    string data         = llvm_temp(ctx);
    string count        = llvm_temp(ctx);
    string capacity     = llvm_temp(ctx);
    string new_count    = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = load ptr, ptr " STRINGP "\n"
              "  " STRINGP " = load i64, ptr " STRINGP "\n"
              "  " STRINGP " = load i64, ptr " STRINGP "\n"
              "  " STRINGP " = add i64 " STRINGP ", " STRINGP "\n",
              STRINGV(data),
              STRINGV(data_ptr_ptr),
              STRINGV(count),
              STRINGV(count_ptr),
              STRINGV(capacity),
              STRINGV(capacity_ptr),
              STRINGV(new_count),
              STRINGV(count),
              STRINGV(source_count));

    string grows = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = icmp ugt i64 " STRINGP ", " STRINGP "\n",
              STRINGV(grows),
              STRINGV(new_count),
              STRINGV(capacity));
    string grow_label = llvm_label(ctx, "dynarray.append.grow");
    string copy_label = llvm_label(ctx, "dynarray.append.copy");
    sb_format(ctx->sb,
              "  br i1 " STRINGP ", label %%" STRINGP ", label %%"
              STRINGP "\n",
              STRINGV(grows),
              STRINGV(grow_label),
              STRINGV(copy_label));

    sb_format(ctx->sb, STRINGP ":\n", STRINGV(grow_label));
    u64 item_size  = llvm_type_storage_bytes(ctx->sema, item_type);
    string bytes   = llvm_temp(ctx);
    string new_data = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = mul i64 " STRINGP ", %llu\n"
              "  " STRINGP " = call ptr @realloc(ptr " STRINGP
              ", i64 " STRINGP ")\n"
              "  store ptr " STRINGP ", ptr " STRINGP "\n"
              "  store i64 " STRINGP ", ptr " STRINGP "\n"
              "  br label %%" STRINGP "\n",
              STRINGV(bytes),
              STRINGV(new_count),
              (unsigned long long)item_size,
              STRINGV(new_data),
              STRINGV(data),
              STRINGV(bytes),
              STRINGV(new_data),
              STRINGV(data_ptr_ptr),
              STRINGV(new_count),
              STRINGV(capacity_ptr),
              STRINGV(copy_label));

    sb_format(ctx->sb, STRINGP ":\n", STRINGV(copy_label));
    string current_data = llvm_temp(ctx);
    string index_slot   = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = load ptr, ptr " STRINGP "\n"
              "  " STRINGP " = alloca i64\n"
              "  store i64 0, ptr " STRINGP "\n",
              STRINGV(current_data),
              STRINGV(data_ptr_ptr),
              STRINGV(index_slot),
              STRINGV(index_slot));

    string loop_label = llvm_label(ctx, "dynarray.append.loop");
    string body_label = llvm_label(ctx, "dynarray.append.body");
    string done_label = llvm_label(ctx, "dynarray.append.done");
    sb_format(ctx->sb,
              "  br label %%" STRINGP "\n"
              STRINGP ":\n",
              STRINGV(loop_label),
              STRINGV(loop_label));

    string cursor = llvm_temp(ctx);
    string more   = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = load i64, ptr " STRINGP "\n"
              "  " STRINGP " = icmp ult i64 " STRINGP ", " STRINGP "\n"
              "  br i1 " STRINGP ", label %%" STRINGP ", label %%"
              STRINGP "\n",
              STRINGV(cursor),
              STRINGV(index_slot),
              STRINGV(more),
              STRINGV(cursor),
              STRINGV(source_count),
              STRINGV(more),
              STRINGV(body_label),
              STRINGV(done_label));

    sb_format(ctx->sb, STRINGP ":\n", STRINGV(body_label));
    string item_type_string = llvm_type_string(ctx, item_type);
    string target_index     = llvm_temp(ctx);
    string source_ptr       = llvm_temp(ctx);
    string target_ptr       = llvm_temp(ctx);
    string value            = llvm_temp(ctx);
    string next             = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = add i64 " STRINGP ", " STRINGP "\n"
              "  " STRINGP " = getelementptr inbounds " STRINGP
              ", ptr " STRINGP ", i64 " STRINGP "\n"
              "  " STRINGP " = getelementptr inbounds " STRINGP
              ", ptr " STRINGP ", i64 " STRINGP "\n"
              "  " STRINGP " = load " STRINGP ", ptr " STRINGP "\n"
              "  store " STRINGP " " STRINGP ", ptr " STRINGP "\n"
              "  " STRINGP " = add i64 " STRINGP ", 1\n"
              "  store i64 " STRINGP ", ptr " STRINGP "\n"
              "  br label %%" STRINGP "\n",
              STRINGV(target_index),
              STRINGV(count),
              STRINGV(cursor),
              STRINGV(source_ptr),
              STRINGV(item_type_string),
              STRINGV(source_data),
              STRINGV(cursor),
              STRINGV(target_ptr),
              STRINGV(item_type_string),
              STRINGV(current_data),
              STRINGV(target_index),
              STRINGV(value),
              STRINGV(item_type_string),
              STRINGV(source_ptr),
              STRINGV(item_type_string),
              STRINGV(value),
              STRINGV(target_ptr),
              STRINGV(next),
              STRINGV(cursor),
              STRINGV(next),
              STRINGV(index_slot),
              STRINGV(loop_label));

    sb_format(ctx->sb, STRINGP ":\n", STRINGV(done_label));
    sb_format(ctx->sb,
              "  store i64 " STRINGP ", ptr " STRINGP "\n",
              STRINGV(new_count),
              STRINGV(count_ptr));
    ctx->block_terminated = false;
    return (LlvmValue){
        .ok         = true,
        .type_index = call->type_index,
        .value      = s(""),
    };
}

internal void llvm_append_escaped_string_bytes(StringBuilder* sb, string value)
{
    static const char hex[] = "0123456789ABCDEF";
    for (usize i = 0; i < value.count; ++i) {
        u8 ch = value.data[i];
        if (ch >= 0x20 && ch <= 0x7e && ch != '"' && ch != '\\') {
            sb_append_char(sb, (char)ch);
            continue;
        }
        sb_append_char(sb, '\\');
        sb_append_char(sb, hex[(ch >> 4) & 0xf]);
        sb_append_char(sb, hex[ch & 0xf]);
    }
}

internal void llvm_render_string_literals(StringBuilder* sb,
                                          const Hir*      hir,
                                          const Lexer*    lexer)
{
    for (u32 i = 0; i < array_count(lexer->strings); ++i) {
        string value = lexer->strings[i];
        llvm_append_string_global_name(sb, hir, i);
        sb_format(sb,
                  " = private unnamed_addr constant [%zu x i8] c\"",
                  value.count + 1);
        llvm_append_escaped_string_bytes(sb, value);
        sb_append_cstr(sb, "\\00\"\n");
    }
}

internal bool llvm_hir_uses_assert(const Hir* hir)
{
    for (u32 i = 0; i < array_count(hir->stmts); ++i) {
        if (hir->stmts[i].kind == HIR_STMT_Assert) {
            return true;
        }
    }
    return false;
}

internal bool llvm_hir_uses_default_assert_message(const Hir* hir)
{
    for (u32 i = 0; i < array_count(hir->stmts); ++i) {
        if (hir->stmts[i].kind == HIR_STMT_Assert &&
            hir->stmts[i].target_expr_index == U32_MAX) {
            return true;
        }
    }
    return false;
}

internal void llvm_render_assert_globals(StringBuilder* sb,
                                         const Hir*      hir,
                                         const Lexer*    lexer)
{
    string source_path = lexer != NULL ? lexer->source.source_path : s("");
    llvm_append_source_path_global_name(sb, hir);
    sb_format(sb,
              " = private unnamed_addr constant [%zu x i8] c\"",
              source_path.count + 1);
    llvm_append_escaped_string_bytes(sb, source_path);
    sb_append_cstr(sb, "\\00\"\n");

    if (llvm_hir_uses_default_assert_message(hir)) {
        string message = s("assertion failed");
        llvm_append_assert_default_message_global_name(sb, hir);
        sb_format(sb,
                  " = private unnamed_addr constant [%zu x i8] c\"",
                  message.count + 1);
        llvm_append_escaped_string_bytes(sb, message);
        sb_append_cstr(sb, "\\00\"\n");
    }
}

internal bool llvm_eval_hir_string_constant(const Hir*   hir,
                                            const Lexer* lexer,
                                            Arena*       arena,
                                            u32          expr_index,
                                            string*      out)
{
    if (expr_index >= array_count(hir->exprs)) {
        return false;
    }

    const HirExpr* expr = &hir->exprs[expr_index];
    if (expr->kind == HIR_EXPR_StringLiteral) {
        if (expr->string_index >= array_count(lexer->strings)) {
            return false;
        }
        *out = lexer->strings[expr->string_index];
        return true;
    }

    if (expr->kind != HIR_EXPR_StringConcat) {
        return false;
    }

    string lhs = {0};
    string rhs = {0};
    if (!llvm_eval_hir_string_constant(
            hir, lexer, arena, expr->lhs_expr_index, &lhs) ||
        !llvm_eval_hir_string_constant(
            hir, lexer, arena, expr->rhs_expr_index, &rhs)) {
        return false;
    }

    u8* data = arena_alloc(arena, lhs.count + rhs.count);
    memcpy(data, lhs.data, lhs.count);
    memcpy(data + lhs.count, rhs.data, rhs.count);
    *out = (string){
        .data  = data,
        .count = lhs.count + rhs.count,
    };
    return true;
}

internal void llvm_render_concat_string_literals(StringBuilder* sb,
                                                 const Hir*      hir,
                                                 const Lexer*    lexer,
                                                 Arena*          arena)
{
    for (u32 i = 0; i < array_count(hir->exprs); ++i) {
        if (hir->exprs[i].kind != HIR_EXPR_StringConcat) {
            continue;
        }

        Arena temp = {0};
        arena_init(&temp);
        string value = {0};
        if (!llvm_eval_hir_string_constant(hir, lexer, &temp, i, &value)) {
            arena_done(&temp);
            continue;
        }

        llvm_append_concat_string_global_name(sb, hir, i);
        sb_format(sb,
                  " = private unnamed_addr constant [%zu x i8] c\"",
                  value.count + 1);
        llvm_append_escaped_string_bytes(sb, value);
        sb_append_cstr(sb, "\\00\"\n");
        arena_done(&temp);
    }
    (void)arena;
}

internal void llvm_render_string_runtime_declarations(StringBuilder* sb)
{
    sb_append_cstr(sb,
                   "declare i1 @string_eq({ ptr, i64 }, { ptr, i64 })\n"
                   "declare void @string_builder_reset()\n"
                   "declare i64 @string_builder_mark()\n"
                   "declare void @string_builder_append_string({ ptr, i64 })\n"
                   "declare void @string_builder_append_byte(i8)\n"
                   "declare { ptr, i64 } @string_builder_finish(i64)\n"
                   "declare { ptr, i64 } @to_string$string({ ptr, i64 })\n"
                   "declare { ptr, i64 } @to_string$bool(i1)\n"
                   "declare { ptr, i64 } @to_string$i8(i8)\n"
                   "declare { ptr, i64 } @to_string$i16(i16)\n"
                   "declare { ptr, i64 } @to_string$i32(i32)\n"
                   "declare { ptr, i64 } @to_string$i64(i64)\n"
                   "declare { ptr, i64 } @to_string$u8(i8)\n"
                   "declare { ptr, i64 } @to_string$u16(i16)\n"
                   "declare { ptr, i64 } @to_string$u32(i32)\n"
                   "declare { ptr, i64 } @to_string$u64(i64)\n"
                   "declare { ptr, i64 } @to_string$isize(i64)\n"
                   "declare { ptr, i64 } @to_string$usize(i64)\n"
                   "declare { ptr, i64 } @to_string$f32(float)\n"
                   "declare { ptr, i64 } @to_string$f64(double)\n");
}

internal void llvm_render_assert_runtime_declarations(StringBuilder* sb)
{
    sb_append_cstr(
        sb,
        "declare void @nerd_assert(i1, ptr, i32, { ptr, i64 })\n");
}

internal bool llvm_hir_uses_dynamic_array_runtime(const Hir* hir, const Sema* sema)
{
    for (u32 i = 0; i < array_count(hir->exprs); ++i) {
        if (llvm_type_kind(sema, hir->exprs[i].type_index) == STK_DynamicArray) {
            return true;
        }
    }
    for (u32 i = 0; i < array_count(hir->stmts); ++i) {
        if (llvm_type_kind(sema, hir->stmts[i].type_index) == STK_DynamicArray) {
            return true;
        }
    }
    return false;
}

internal void llvm_render_dynamic_array_runtime_declarations(StringBuilder* sb)
{
    sb_append_cstr(sb,
                   "declare ptr @malloc(i64)\n"
                   "declare ptr @realloc(ptr, i64)\n"
                   "declare void @free(ptr)\n");
}

internal bool llvm_hir_uses_string_runtime(const Hir* hir, const Sema* sema)
{
    for (u32 i = 0; i < array_count(hir->exprs); ++i) {
        if (hir->exprs[i].kind == HIR_EXPR_InterpolatedString) {
            return true;
        }
    }
    for (u32 i = 0; i < array_count(hir->patterns); ++i) {
        const HirPattern* pattern = &hir->patterns[i];
        if ((pattern->kind == HIR_PATTERN_Value ||
             pattern->kind == HIR_PATTERN_Equal ||
            pattern->kind == HIR_PATTERN_NotEqual) &&
            pattern->expr_index < array_count(hir->exprs) &&
            llvm_type_kind(sema, hir->exprs[pattern->expr_index].type_index) ==
                STK_String) {
            return true;
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

internal bool llvm_hir_has_globals(const Hir* hir)
{
    for (u32 i = 0; i < array_count(hir->values); ++i) {
        if (hir->values[i].kind == HIR_VALUE_Global) {
            return true;
        }
    }
    return false;
}

internal void llvm_render_global_values(StringBuilder* sb,
                                        const Hir*      hir,
                                        const Lexer*    lexer,
                                        const Sema*     sema,
                                        Arena*          arena)
{
    for (u32 i = 0; i < array_count(hir->values); ++i) {
        const HirValue* value = &hir->values[i];
        if (value->kind != HIR_VALUE_Global) {
            continue;
        }

        u32 symbol_handle = llvm_value_symbol_handle(hir, i);
        if (symbol_handle == U32_MAX) {
            continue;
        }

        llvm_append_symbol_name(sb, lex_symbol(lexer, symbol_handle));
        sb_append_cstr(sb, " = global ");
        llvm_append_type(sb, sema, value->type_index);
        sb_append_cstr(sb, " ");
        llvm_append_zero_value(sb, sema, value->type_index);
        sb_append_char(sb, '\n');
    }
    (void)arena;
}

internal void llvm_render_global_slice_backing_values(StringBuilder* sb,
                                                     const Hir*      hir,
                                                     const Sema*     sema)
{
    for (u32 i = 0; i < array_count(hir->values); ++i) {
        const HirValue* value = &hir->values[i];
        if (value->kind != HIR_VALUE_Global ||
            value->value_expr_index >= array_count(hir->exprs)) {
            continue;
        }
        const HirExpr* expr = &hir->exprs[value->value_expr_index];
        if (expr->kind != HIR_EXPR_Array ||
            llvm_type_kind(sema, expr->type_index) != STK_Slice) {
            continue;
        }

        u32 item_type = llvm_collection_item_type(sema, expr->type_index);
        if (item_type == sema_no_type()) {
            continue;
        }
        llvm_append_global_slice_backing_name(sb, hir, i);
        sb_format(sb, " = private global [%u x ", expr->arg_count);
        llvm_append_type(sb, sema, item_type);
        sb_append_cstr(sb, "] zeroinitializer\n");
    }
}

internal void llvm_render_global_init(StringBuilder* sb,
                                      const Hir*      hir,
                                      const Lexer*    lexer,
                                      const Sema*     sema,
                                      Arena*          arena)
{
    if (!llvm_hir_has_globals(hir)) {
        return;
    }

    sb_append_cstr(sb, "define void ");
    llvm_append_module_init_name(sb, hir);
    sb_append_cstr(sb, "() {\n");
    Arena temp = {0};
    arena_init(&temp);
    LlvmFunctionContext ctx = {
        .sb        = sb,
        .hir       = hir,
        .lexer     = lexer,
        .sema      = sema,
        .arena     = &temp,
        .next_temp = 0,
        .global_init_value_index = U32_MAX,
    };
    for (u32 i = 0; i < array_count(hir->values); ++i) {
        const HirValue* value = &hir->values[i];
        if (value->kind != HIR_VALUE_Global ||
            value->value_expr_index == U32_MAX) {
            continue;
        }

        string name = llvm_value_name_string(hir, lexer, &temp, i);
        if (name.count == 0) {
            continue;
        }

        ctx.global_init_value_index = i;
        LlvmValue init_value = llvm_emit_expr(&ctx, NULL, value->value_expr_index);
        ctx.global_init_value_index = U32_MAX;
        if (!init_value.ok) {
            continue;
        }

        string type = llvm_type_string(&ctx, value->type_index);
        sb_format(sb,
                  "  store " STRINGP " " STRINGP ", ptr " STRINGP "\n",
                  STRINGV(type),
                  STRINGV(init_value.value),
                  STRINGV(name));
    }
    sb_append_cstr(sb, "  ret void\n");
    sb_append_cstr(sb, "}\n");
    array_free(ctx.locals);
    array_free(ctx.slots);
    array_free(ctx.assigned_locals);
    array_free(ctx.control_targets);
    arena_done(&temp);
    (void)arena;
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
        .global_init_value_index = U32_MAX,
    };
    llvm_collect_assigned_locals(&ctx, function->body_block_index);
    bool emitted = llvm_emit_block(&ctx, function, function->body_block_index);
    if (!emitted || !ctx.block_terminated) {
        u32 return_type = llvm_function_return_type(sema, function->type_index);
        llvm_append_default_return(sb, sema, return_type);
    }
    array_free(ctx.locals);
    array_free(ctx.slots);
    array_free(ctx.assigned_locals);
    array_free(ctx.control_targets);
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
    if (function->kind == HIR_FUNCTION_Ffi) {
        return;
    }

    llvm_append_symbol_name(sb, lex_symbol(lexer, binding->symbol_handle));
    sb_append_cstr(sb, " = alias ");
    llvm_append_function_type(sb, sema, function->type_index);
    sb_append_cstr(sb, ", ptr ");
    llvm_append_function_name(sb, hir, lexer, binding->target_index);
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

    if (array_count(lexer->strings) > 0) {
        llvm_render_string_literals(&sb, hir, lexer);
        llvm_render_concat_string_literals(&sb, hir, lexer, arena);
    }
    if (llvm_hir_uses_assert(hir)) {
        llvm_render_assert_globals(&sb, hir, lexer);
    }
    if (array_count(lexer->strings) > 0 || llvm_hir_uses_assert(hir)) {
        sb_append_char(&sb, '\n');
    }

    bool uses_string_runtime       = llvm_hir_uses_string_runtime(hir, sema);
    bool uses_assert_runtime       = llvm_hir_uses_assert(hir);
    bool uses_dynamic_array_runtime =
        llvm_hir_uses_dynamic_array_runtime(hir, sema);

    if (uses_string_runtime) {
        llvm_render_string_runtime_declarations(&sb);
    }
    if (uses_assert_runtime) {
        llvm_render_assert_runtime_declarations(&sb);
    }
    if (uses_dynamic_array_runtime) {
        llvm_render_dynamic_array_runtime_declarations(&sb);
    }
    if (uses_string_runtime || uses_assert_runtime ||
        uses_dynamic_array_runtime) {
        sb_append_char(&sb, '\n');
    }

    for (u32 i = 0; i < array_count(hir->imports); ++i) {
        llvm_render_import(&sb, lexer, sema, &hir->imports[i]);
    }
    if (array_count(hir->imports) > 0) {
        sb_append_char(&sb, '\n');
    }

    if (llvm_hir_has_globals(hir)) {
    llvm_render_global_slice_backing_values(&sb, hir, sema);
    llvm_render_global_values(&sb, hir, lexer, sema, arena);
        sb_append_char(&sb, '\n');
        llvm_render_global_init(&sb, hir, lexer, sema, arena);
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
