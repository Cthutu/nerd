//------------------------------------------------------------------------------
// LLVM IR emission from HIR
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/build/build.h>
#include <compiler/error/error.h>
#include <compiler/llvm/llvm.h>
#include <stdio.h>

//------------------------------------------------------------------------------

typedef struct {
    u32  pointer_bits;
    u32  size_bits;
    u32  enum_tag_bits;
    u32  aggregate_payload_align_bits;
    cstr pointer_type;
    cstr size_type;
    cstr enum_tag_type;
} LlvmLayout;

internal const LlvmLayout* llvm_default_layout(void)
{
    static const LlvmLayout layout = {
        .pointer_bits                 = 64,
        .size_bits                    = 64,
        .enum_tag_bits                = 64,
        .aggregate_payload_align_bits = 64,
        .pointer_type                 = "ptr",
        .size_type                    = "i64",
        .enum_tag_type                = "i64",
    };
    return &layout;
}

internal string llvm_layout_string_type(const LlvmLayout* layout)
{
    (void)layout;
    return s("{ ptr, i64 }");
}

internal string llvm_layout_dynamic_array_header_type(const LlvmLayout* layout)
{
    (void)layout;
    return s("{ ptr, i64, i64 }");
}

internal u64 llvm_layout_dynamic_array_header_bytes(const LlvmLayout* layout)
{
    return (u64)((layout->pointer_bits + layout->size_bits * 2) / 8);
}

internal const LlvmLayout* llvm_layout_or_default(const LlvmLayout* layout)
{
    return layout != NULL ? layout : llvm_default_layout();
}

internal u64 llvm_dynamic_array_header_bytes(const LlvmLayout* layout)
{
    return llvm_layout_dynamic_array_header_bytes(
        llvm_layout_or_default(layout));
}

internal string llvm_string_type(const LlvmLayout* layout)
{
    return llvm_layout_string_type(llvm_layout_or_default(layout));
}

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

internal u32 llvm_pointer_type(const Sema* sema, u32 pointee_type)
{
    if (sema == NULL || pointee_type == sema_no_type()) {
        return sema_no_type();
    }
    for (u32 i = 0; i < array_count(sema->types); ++i) {
        const SemaType* type = &sema->types[i];
        if (type->kind == STK_Pointer &&
            type->first_param_type == pointee_type) {
            return i;
        }
    }
    return sema_no_type();
}

internal u32 llvm_add_type_if_missing(Sema* sema, SemaType type)
{
    for (u32 i = 0; i < array_count(sema->types); ++i) {
        if (memcmp(&sema->types[i], &type, sizeof(type)) == 0) {
            return i;
        }
    }
    u32 index = (u32)array_count(sema->types);
    array_push(sema->types, type);
    return index;
}

internal u32 llvm_ensure_builtin_type(Sema* sema, SemaTypeKind kind)
{
    return llvm_add_type_if_missing(sema,
                                    (SemaType){
                                        .kind             = kind,
                                        .param_count      = 0,
                                        .first_param_type = 0,
                                        .return_type      = sema_no_type(),
                                    });
}

internal u32 llvm_ensure_pointer_type(Sema* sema, u32 pointee_type)
{
    return llvm_add_type_if_missing(sema,
                                    (SemaType){
                                        .kind             = STK_Pointer,
                                        .param_count      = 0,
                                        .first_param_type = pointee_type,
                                        .return_type      = sema_no_type(),
                                    });
}

internal Sema llvm_prepare_render_sema(const Sema* sema)
{
    Sema result               = sema != NULL ? *sema : (Sema){0};
    result.types              = NULL;
    result.type_param_types   = NULL;
    result.type_param_symbols = NULL;
    result.type_param_values  = NULL;

    if (sema != NULL) {
        for (u32 i = 0; i < array_count(sema->types); ++i) {
            array_push(result.types, sema->types[i]);
        }
        for (u32 i = 0; i < array_count(sema->type_param_types); ++i) {
            array_push(result.type_param_types, sema->type_param_types[i]);
        }
        for (u32 i = 0; i < array_count(sema->type_param_symbols); ++i) {
            array_push(result.type_param_symbols, sema->type_param_symbols[i]);
        }
        for (u32 i = 0; i < array_count(sema->type_param_values); ++i) {
            array_push(result.type_param_values, sema->type_param_values[i]);
        }
    }

    u32 u8_type = llvm_ensure_builtin_type(&result, STK_U8);
    llvm_ensure_pointer_type(&result, u8_type);
    return result;
}

internal void llvm_render_sema_done(Sema* sema)
{
    array_free(sema->types);
    array_free(sema->type_param_types);
    array_free(sema->type_param_symbols);
    array_free(sema->type_param_values);
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
        return 64;
    case STK_Isize:
    case STK_Usize:
        return llvm_default_layout()->size_bits;
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

internal u32  llvm_union_storage_bits(const Sema* sema, u32 union_type);
internal u32  llvm_enum_storage_payload_bits(const Sema* sema, u32 enum_type);
internal u32  llvm_type_align_bits(const Sema* sema, u32 type_index);
internal bool llvm_root_exports_symbol(const Sema* sema, string name);

internal u32 llvm_align_bits(u32 bits, u32 align_bits)
{
    if (bits == 0 || align_bits == 0) {
        return bits;
    }
    u32 remainder = bits % align_bits;
    return remainder == 0 ? bits : bits + align_bits - remainder;
}

internal u32 llvm_type_storage_bits(const Sema* sema, u32 type_index)
{
    const LlvmLayout* layout   = llvm_default_layout();
    u32               int_bits = llvm_integer_bits(sema, type_index);
    if (int_bits > 0) {
        return int_bits;
    }
    u32 float_bits = llvm_float_bits(sema, type_index);
    if (float_bits > 0) {
        return float_bits;
    }
    if (llvm_type_kind(sema, type_index) == STK_Pointer ||
        llvm_type_kind(sema, type_index) == STK_Function ||
        llvm_type_kind(sema, type_index) == STK_DynamicArray ||
        llvm_type_kind(sema, type_index) == STK_Box) {
        return layout->pointer_bits;
    }
    if (llvm_type_kind(sema, type_index) == STK_String ||
        llvm_type_kind(sema, type_index) == STK_Slice) {
        return layout->pointer_bits + layout->size_bits;
    }
    if (llvm_type_kind(sema, type_index) == STK_Arena) {
        return layout->pointer_bits * 2 + layout->size_bits * 2;
    }
    if (llvm_type_kind(sema, type_index) == STK_Tuple ||
        llvm_type_kind(sema, type_index) == STK_Plex) {
        const SemaType* type           = &sema->types[type_index];
        u32             bits           = 0;
        u32             max_align_bits = 8;
        for (u32 i = 0; i < type->param_count; ++i) {
            u32 field_type = sema->type_param_types[type->first_param_type + i];
            u32 field_align_bits = llvm_type_align_bits(sema, field_type);
            u32 field_bits       = llvm_type_storage_bits(sema, field_type);
            bits                 = llvm_align_bits(bits, field_align_bits);
            bits += field_bits;
            if (field_align_bits > max_align_bits) {
                max_align_bits = field_align_bits;
            }
        }
        return llvm_align_bits(bits, max_align_bits);
    }
    if (llvm_type_kind(sema, type_index) == STK_Array) {
        const SemaType* type = &sema->types[type_index];
        return (u32)type->return_type *
               llvm_type_storage_bits(sema, type->first_param_type);
    }
    if (llvm_type_kind(sema, type_index) == STK_Union) {
        return llvm_union_storage_bits(sema, type_index);
    }
    if (llvm_type_kind(sema, type_index) == STK_Enum) {
        return layout->enum_tag_bits +
               llvm_enum_storage_payload_bits(sema, type_index);
    }
    return 0;
}

internal u32 llvm_type_align_bits(const Sema* sema, u32 type_index)
{
    const LlvmLayout* layout   = llvm_default_layout();
    u32               int_bits = llvm_integer_bits(sema, type_index);
    if (int_bits > 0) {
        return int_bits < 8 ? 8 : int_bits;
    }

    u32 float_bits = llvm_float_bits(sema, type_index);
    if (float_bits > 0) {
        return float_bits;
    }

    SemaTypeKind kind = llvm_type_kind(sema, type_index);
    switch (kind) {
    case STK_Pointer:
    case STK_Function:
    case STK_DynamicArray:
    case STK_Box:
    case STK_String:
    case STK_Slice:
    case STK_Arena:
    case STK_Enum:
        return layout->pointer_bits;
    case STK_Array:
        return llvm_type_align_bits(sema,
                                    sema->types[type_index].first_param_type);
    case STK_Tuple:
    case STK_Plex:
        {
            const SemaType* type           = &sema->types[type_index];
            u32             max_align_bits = 8;
            for (u32 i = 0; i < type->param_count; ++i) {
                u32 field_type =
                    sema->type_param_types[type->first_param_type + i];
                u32 field_align_bits = llvm_type_align_bits(sema, field_type);
                if (field_align_bits > max_align_bits) {
                    max_align_bits = field_align_bits;
                }
            }
            return max_align_bits;
        }
    case STK_Union:
        {
            const SemaType* type           = &sema->types[type_index];
            u32             max_align_bits = 8;
            for (u32 i = 0; i < type->param_count; ++i) {
                u32 field_type =
                    sema->type_param_types[type->first_param_type + i];
                u32 field_align_bits = llvm_type_align_bits(sema, field_type);
                if (field_align_bits > max_align_bits) {
                    max_align_bits = field_align_bits;
                }
            }
            return max_align_bits;
        }
    default:
        return 8;
    }
}

internal bool
llvm_record_type_has_field(const Sema* sema, u32 type_index, u32 symbol_handle)
{
    u32 target_type = type_index;
    if (llvm_type_kind(sema, target_type) == STK_Pointer) {
        u32          pointee_type = sema->types[target_type].first_param_type;
        SemaTypeKind pointee_kind = llvm_type_kind(sema, pointee_type);
        if (pointee_kind == STK_Plex || pointee_kind == STK_Union) {
            target_type = pointee_type;
        }
    }

    SemaTypeKind kind = llvm_type_kind(sema, target_type);
    if (kind != STK_Plex && kind != STK_Union) {
        return false;
    }

    const SemaType* record = &sema->types[target_type];
    for (u32 i = 0; i < record->param_count; ++i) {
        if (sema->type_param_symbols[record->first_param_type + i] ==
            symbol_handle) {
            return true;
        }
    }
    return false;
}

internal u64 llvm_type_sizeof_bytes(const Sema* sema, u32 type_index)
{
    SemaTypeKind kind = llvm_type_kind(sema, type_index);
    if (kind == STK_Void || kind == STK_Nil || kind == STK_Module) {
        return 0;
    }

    u32 bits = llvm_type_storage_bits(sema, type_index);
    return bits == 0 ? 0 : (bits + 7) / 8;
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

internal bool llvm_type_has_dot_members(const Sema* sema, u32 type_index)
{
    switch (llvm_type_kind(sema, type_index)) {
    case STK_Tuple:
    case STK_Array:
    case STK_Slice:
    case STK_String:
    case STK_DynamicArray:
    case STK_Plex:
    case STK_Union:
        return true;
    default:
        return false;
    }
}

internal u32 llvm_member_target_type(const Sema* sema, u32 type_index)
{
    u32 result = sema_materialise_type(sema, type_index);
    if (llvm_type_kind(sema, result) == STK_Box) {
        result =
            sema_materialise_type(sema, sema->types[result].first_param_type);
    }
    while (llvm_type_kind(sema, result) == STK_Pointer) {
        u32 pointee_type =
            sema_materialise_type(sema, llvm_pointee_type(sema, result));
        if (llvm_type_kind(sema, pointee_type) == STK_Box) {
            pointee_type = sema_materialise_type(
                sema, sema->types[pointee_type].first_param_type);
        }
        SemaTypeKind pointee_kind = llvm_type_kind(sema, pointee_type);
        if (pointee_kind != STK_Pointer &&
            !llvm_type_has_dot_members(sema, pointee_type)) {
            break;
        }
        result = pointee_type;
    }
    return result;
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
        kind == STK_DynamicArray || kind == STK_Box) {
        return sema->types[type_index].first_param_type;
    }
    return sema_no_type();
}

internal string llvm_dynamic_array_header_type(void)
{
    return llvm_layout_dynamic_array_header_type(llvm_default_layout());
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
        if (field_index == 0) {
            return llvm_pointer_type(sema, llvm_builtin_type(sema, STK_U8));
        }
        return field_index == 1 ? llvm_builtin_type(sema, STK_Usize)
                                : sema_no_type();
    }
    if (llvm_type_kind(sema, type_index) == STK_Slice) {
        if (field_index == 0) {
            return llvm_pointer_type(
                sema, llvm_collection_item_type(sema, type_index));
        }
        return field_index == 1 ? llvm_builtin_type(sema, STK_Usize)
                                : sema_no_type();
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
    return llvm_align_bits(bits,
                           llvm_default_layout()->aggregate_payload_align_bits);
}

internal void
llvm_append_type(StringBuilder* sb, const Sema* sema, u32 type_index)
{
    const LlvmLayout* layout = llvm_default_layout();
    if (sema == NULL || type_index == sema_no_type() ||
        type_index >= array_count(sema->types)) {
        sb_append_cstr(sb, layout->pointer_type);
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
        sb_append_cstr(sb, layout->size_type);
        break;
    case STK_Arena:
        sb_append_cstr(sb, "{ ptr, ptr, ");
        sb_append_cstr(sb, layout->size_type);
        sb_append_cstr(sb, ", ");
        sb_append_cstr(sb, layout->size_type);
        sb_append_cstr(sb, " }");
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
        sb_append_string(sb, llvm_layout_string_type(layout));
        break;
    case STK_Function:
    case STK_Pointer:
    case STK_Box:
        sb_append_cstr(sb, layout->pointer_type);
        break;
    case STK_Slice:
        sb_append_string(sb, llvm_layout_string_type(layout));
        break;
    case STK_Enum:
        {
            u32 payload_bits = llvm_enum_storage_payload_bits(sema, type_index);
            sb_append_cstr(sb, "{ ");
            sb_append_cstr(sb, layout->enum_tag_type);
            sb_append_cstr(sb, ", ");
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
        sb_append_cstr(sb, layout->pointer_type);
        break;
    }
}

internal void llvm_append_symbol_name(StringBuilder* sb, string name)
{
    sb_append_char(sb, '@');
    sb_append_char(sb, '$');
    for (usize i = 0; i < name.count; ++i) {
        u8   ch     = name.data[i];
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
        u8   ch     = name.data[i];
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

internal u32 llvm_function_decl_symbol_handle(const Sema*        sema,
                                              const HirFunction* function)
{
    if (sema == NULL || function == NULL) {
        return U32_MAX;
    }
    for (u32 i = 0; i < array_count(sema->methods); ++i) {
        const SemaMethod* method = &sema->methods[i];
        if (method->decl_index < array_count(sema->decls) &&
            sema->decls[method->decl_index].value_node_index ==
                function->fn_node_index) {
            return method->symbol_handle;
        }
    }
    if (function->decl_index < array_count(sema->decls)) {
        return sema->decls[function->decl_index].symbol_handle;
    }
    for (u32 i = 0; i < array_count(sema->decls); ++i) {
        const SemaDecl* decl = &sema->decls[i];
        if (decl->value_node_index == function->fn_node_index) {
            return decl->symbol_handle;
        }
    }
    return U32_MAX;
}

internal u32 llvm_function_method_symbol_handle(const Sema*        sema,
                                                const HirFunction* function)
{
    if (sema == NULL || function == NULL) {
        return U32_MAX;
    }
    for (u32 i = 0; i < array_count(sema->methods); ++i) {
        const SemaMethod* method = &sema->methods[i];
        if (method->decl_index < array_count(sema->decls) &&
            sema->decls[method->decl_index].value_node_index ==
                function->fn_node_index) {
            return method->symbol_handle;
        }
    }
    return U32_MAX;
}

internal u32 llvm_generic_function_inst_symbol_handle(
    const Sema* sema, const HirFunction* function)
{
    if (sema == NULL || function == NULL ||
        function->kind != HIR_FUNCTION_GenericInstantiation) {
        return U32_MAX;
    }

    for (u32 i = 0; i < array_count(sema->generic_fn_instantiations); ++i) {
        const SemaGenericFnInstantiation* inst =
            &sema->generic_fn_instantiations[i];
        if (inst->fn_node_index == function->fn_node_index &&
            inst->root_scope_index == function->root_scope_index &&
            inst->type_index == function->type_index) {
            return inst->symbol_handle;
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
        u32 symbol_handle = hir->functions[function_index].ffi_symbol_handle;
        if (symbol_handle == U32_MAX) {
            symbol_handle = llvm_function_symbol_handle(hir, function_index);
        }
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

internal void llvm_append_const_slice_backing_name(StringBuilder* sb,
                                                   const Hir*     hir,
                                                   u32            expr_index)
{
    u32 module_index = hir != NULL ? hir->current_module_index : 0;
    if (module_index == U32_MAX) {
        module_index = 0;
    }
    sb_format(sb, "@.slice.const.m%u.%u", module_index, expr_index);
}

internal void llvm_append_assert_source_path_global_name(StringBuilder* sb,
                                                         const Hir*     hir,
                                                         u32 stmt_index)
{
    u32 module_index = hir != NULL ? hir->current_module_index : 0;
    if (module_index == U32_MAX) {
        module_index = 0;
    }
    sb_format(sb, "@.assert.source_path.m%u.%u", module_index, stmt_index);
}

internal void llvm_append_builtin_module_file_global_name(StringBuilder* sb,
                                                          const Hir*     hir)
{
    u32 module_index = hir != NULL ? hir->current_module_index : 0;
    if (module_index == U32_MAX) {
        module_index = 0;
    }
    sb_format(sb, "@.macro.file.m%u", module_index);
}

internal string llvm_builtin_module_file_global_name_string(const Hir* hir,
                                                            Arena*     arena)
{
    StringBuilder sb = {0};
    sb_init(&sb, arena);
    llvm_append_builtin_module_file_global_name(&sb, hir);
    return sb_to_string(&sb);
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

internal string llvm_const_slice_backing_name_string(const Hir* hir,
                                                     Arena*     arena,
                                                     u32        expr_index)
{
    StringBuilder sb = {0};
    sb_init(&sb, arena);
    llvm_append_const_slice_backing_name(&sb, hir, expr_index);
    return sb_to_string(&sb);
}

internal string llvm_assert_source_path_global_name_string(const Hir* hir,
                                                           Arena*     arena,
                                                           u32 stmt_index)
{
    StringBuilder sb = {0};
    sb_init(&sb, arena);
    llvm_append_assert_source_path_global_name(&sb, hir, stmt_index);
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

internal bool llvm_import_source_function_depth(const Sema*      sema,
                                                const HirImport* import,
                                                const Hir**      out_hir,
                                                u32* out_function_index,
                                                u32  depth)
{
    if (sema == NULL || sema->program == NULL ||
        import->module_index >= array_count(sema->program->modules) ||
        depth > 16) {
        return false;
    }

    const ModuleInfo* module = &sema->program->modules[import->module_index];
    const Hir*        hir    = &module->front_end.hir;
    const Sema*       source_sema = &module->front_end.sema;
    for (u32 i = 0; i < array_count(hir->functions); ++i) {
        if (hir->functions[i].decl_index == import->decl_index) {
            *out_hir            = hir;
            *out_function_index = i;
            return true;
        }
    }

    if (import->decl_index < array_count(source_sema->decls)) {
        u32 source_symbol =
            source_sema->decls[import->decl_index].symbol_handle;
        for (u32 i = 0; i < array_count(hir->bindings); ++i) {
            const HirBinding* binding = &hir->bindings[i];
            if (binding->kind == HIR_BINDING_Import &&
                binding->symbol_handle == source_symbol &&
                binding->target_index < array_count(hir->imports)) {
                return llvm_import_source_function_depth(
                    sema,
                    &hir->imports[binding->target_index],
                    out_hir,
                    out_function_index,
                    depth + 1);
            }
        }
    }
    return false;
}

internal bool llvm_import_source_function(const Sema*      sema,
                                          const HirImport* import,
                                          const Hir**      out_hir,
                                          u32*             out_function_index)
{
    return llvm_import_source_function_depth(
        sema, import, out_hir, out_function_index, 0);
}

internal bool llvm_import_source_generic_function(const Sema*      sema,
                                                  const Lexer*     lexer,
                                                  const HirImport* import,
                                                  u32 callee_symbol_handle,
                                                  u32 callee_type,
                                                  const Hir** out_hir,
                                                  u32* out_function_index)
{
    if (sema == NULL || sema->program == NULL ||
        import->module_index >= array_count(sema->program->modules)) {
        return false;
    }

    const ModuleInfo* module = &sema->program->modules[import->module_index];
    const Hir*        hir    = &module->front_end.hir;
    const Sema*       source_sema  = &module->front_end.sema;
    const Lexer*      source_lexer = &module->front_end.lexer;
    string     callee_name = callee_symbol_handle != U32_MAX
                                 ? lex_symbol(lexer, callee_symbol_handle)
                                 : (string){0};
    const Hir* unique_template_hir            = NULL;
    u32        unique_template_function_index = U32_MAX;
    u32        template_match_count           = 0;

    for (u32 i = 0; i < array_count(hir->functions); ++i) {
        const HirFunction* function = &hir->functions[i];
        if (function->kind != HIR_FUNCTION_GenericInstantiation) {
            continue;
        }

        bool template_matches = false;
        if (import->decl_index < array_count(source_sema->decls) &&
            source_sema->decls[import->decl_index].value_node_index ==
                function->fn_node_index) {
            template_matches = true;
        }

        u32 inst_symbol =
            llvm_generic_function_inst_symbol_handle(source_sema, function);
        bool symbol_matches =
            callee_name.count > 0 && inst_symbol != U32_MAX &&
            string_eq(callee_name, lex_symbol(source_lexer, inst_symbol));

        bool no_type_discriminator = callee_type == sema_no_type() ||
                                     llvm_type_is_void(sema, callee_type);
        if ((template_matches || symbol_matches) &&
            (no_type_discriminator || function->type_index == callee_type ||
             symbol_matches)) {
            *out_hir            = hir;
            *out_function_index = i;
            return true;
        }
        if (template_matches) {
            unique_template_hir            = hir;
            unique_template_function_index = i;
            template_match_count += 1;
        }
    }
    if (template_match_count == 1) {
        *out_hir            = unique_template_hir;
        *out_function_index = unique_template_function_index;
        return true;
    }
    return false;
}

internal bool llvm_import_source_value(const Sema*      sema,
                                       const HirImport* import,
                                       const Hir**      out_hir,
                                       const Lexer**    out_lexer,
                                       const Sema**     out_sema,
                                       u32*             out_value_index)
{
    if (sema == NULL || sema->program == NULL ||
        import->module_index >= array_count(sema->program->modules)) {
        return false;
    }

    const ModuleInfo* module = &sema->program->modules[import->module_index];
    const Hir*        hir    = &module->front_end.hir;
    for (u32 i = 0; i < array_count(hir->values); ++i) {
        if (hir->values[i].decl_index == import->decl_index) {
            *out_hir         = hir;
            *out_lexer       = &module->front_end.lexer;
            *out_sema        = &module->front_end.sema;
            *out_value_index = i;
            return true;
        }
    }
    return false;
}

internal bool llvm_program_function_symbol_conflicts(const Sema* sema,
                                                     const Hir*  hir,
                                                     u32         function_index)
{
    if (sema == NULL || sema->program == NULL ||
        function_index >= array_count(hir->functions)) {
        return false;
    }

    u32 symbol_handle = llvm_function_symbol_handle(hir, function_index);
    if (symbol_handle == U32_MAX) {
        return false;
    }

    const ModuleInfo* module =
        &sema->program->modules[hir->current_module_index];
    string name    = lex_symbol(&module->front_end.lexer, symbol_handle);
    u32    matches = 0;
    for (u32 module_index = 0;
         module_index < array_count(sema->program->modules);
         ++module_index) {
        const ModuleInfo* other     = &sema->program->modules[module_index];
        const Hir*        other_hir = &other->front_end.hir;
        for (u32 i = 0; i < array_count(other_hir->functions); ++i) {
            u32 other_symbol = llvm_function_symbol_handle(other_hir, i);
            if (other_symbol != U32_MAX &&
                string_eq(name,
                          lex_symbol(&other->front_end.lexer, other_symbol))) {
                matches++;
            }
        }
    }
    return matches > 1;
}

internal bool llvm_function_imported_from_other_module(
    const Sema* sema, const Hir* hir, const HirFunction* function)
{
    if (sema == NULL || sema->program == NULL || hir == NULL ||
        function == NULL || function->decl_index == U32_MAX) {
        return false;
    }

    for (u32 module_index = 0;
         module_index < array_count(sema->program->modules);
         ++module_index) {
        if (module_index == hir->current_module_index) {
            continue;
        }

        const Hir* other_hir =
            &sema->program->modules[module_index].front_end.hir;
        for (u32 i = 0; i < array_count(other_hir->imports); ++i) {
            const HirImport* import = &other_hir->imports[i];
            if (import->module_index == hir->current_module_index &&
                import->decl_index == function->decl_index) {
                return true;
            }
        }
    }
    return false;
}

internal bool llvm_function_needs_external_definition(const Sema* sema,
                                                      const Hir*  hir,
                                                      u32 function_index)
{
    if (hir == NULL || function_index >= array_count(hir->functions)) {
        return false;
    }

    const HirFunction* function = &hir->functions[function_index];
    if (function->kind == HIR_FUNCTION_Ffi) {
        return false;
    }

    if (llvm_program_function_symbol_conflicts(sema, hir, function_index)) {
        return true;
    }

    return function->kind == HIR_FUNCTION_GenericInstantiation &&
           llvm_function_imported_from_other_module(sema, hir, function);
}

internal string llvm_import_name_string(const Sema*      sema,
                                        const Lexer*     lexer,
                                        Arena*           arena,
                                        const HirImport* import)
{
    StringBuilder sb = {0};
    sb_init(&sb, arena);
    if (import->ffi_symbol_handle != U32_MAX) {
        llvm_append_c_symbol_name(&sb,
                                  lex_symbol(lexer, import->ffi_symbol_handle));
    } else {
        const Hir* source_hir     = NULL;
        u32        function_index = U32_MAX;
        if (llvm_import_source_function(
                sema, import, &source_hir, &function_index) &&
            (llvm_program_function_symbol_conflicts(
                 sema, source_hir, function_index) ||
             llvm_root_exports_symbol(
                 sema, lex_symbol(lexer, import->symbol_handle)))) {
            llvm_append_generated_function_name(
                &sb, source_hir, function_index);
        } else {
            llvm_append_symbol_name(&sb,
                                    lex_symbol(lexer, import->symbol_handle));
        }
    }
    return sb_to_string(&sb);
}

internal const HirImport* llvm_binding_import(const Hir* hir, u32 binding_index)
{
    if (binding_index >= array_count(hir->bindings)) {
        return NULL;
    }

    const HirBinding* binding = &hir->bindings[binding_index];
    if (binding->kind != HIR_BINDING_Import ||
        binding->target_index >= array_count(hir->imports)) {
        return NULL;
    }
    return &hir->imports[binding->target_index];
}

internal const HirImport*
llvm_import_for_symbol(const Hir* hir, u32 symbol_handle, u32 type_index)
{
    if (symbol_handle == U32_MAX) {
        return NULL;
    }

    for (u32 i = 0; i < array_count(hir->imports); ++i) {
        const HirImport* import = &hir->imports[i];
        if (import->symbol_handle == symbol_handle &&
            import->type_index == type_index) {
            return import;
        }
    }
    return NULL;
}

internal const HirImport* llvm_import_for_symbol_any_type(const Hir* hir,
                                                          u32 symbol_handle)
{
    if (symbol_handle == U32_MAX) {
        return NULL;
    }

    for (u32 i = 0; i < array_count(hir->imports); ++i) {
        const HirImport* import = &hir->imports[i];
        if (import->symbol_handle == symbol_handle) {
            return import;
        }
    }
    return NULL;
}

internal const HirImport* llvm_import_for_symbol_name(const Hir*   hir,
                                                      const Lexer* lexer,
                                                      u32 symbol_handle)
{
    if (hir == NULL || lexer == NULL || symbol_handle == U32_MAX) {
        return NULL;
    }

    string name = lex_symbol(lexer, symbol_handle);
    for (u32 i = 0; i < array_count(hir->imports); ++i) {
        const HirImport* import = &hir->imports[i];
        if (import->symbol_handle != U32_MAX &&
            string_eq(lex_symbol(lexer, import->symbol_handle), name)) {
            return import;
        }
    }
    return NULL;
}

internal const HirImport* llvm_field_import(const Hir*     hir,
                                            const HirExpr* field)
{
    if (field->kind != HIR_EXPR_Field ||
        field->operand_expr_index >= array_count(hir->exprs) ||
        field->symbol_handle == U32_MAX) {
        return NULL;
    }

    const HirExpr* operand = &hir->exprs[field->operand_expr_index];
    if (operand->kind != HIR_EXPR_LocalRef ||
        operand->ref_kind != HIR_REF_Binding ||
        operand->ref_index >= array_count(hir->bindings)) {
        return NULL;
    }

    const HirBinding* module_binding = &hir->bindings[operand->ref_index];
    if (module_binding->kind != HIR_BINDING_Module) {
        return NULL;
    }

    for (u32 i = 0; i < array_count(hir->imports); ++i) {
        const HirImport* import = &hir->imports[i];
        if (import->module_index == module_binding->target_index &&
            import->symbol_handle == field->symbol_handle) {
            return import;
        }
    }
    return NULL;
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

internal void llvm_append_function_signature(StringBuilder*     sb,
                                             const Hir*         hir,
                                             const Lexer*       lexer,
                                             const Sema*        sema,
                                             const HirFunction* function,
                                             u32                function_index)
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
    if (sema != NULL && function->type_index < array_count(sema->types) &&
        (sema->types[function->type_index].flags & STF_FunctionVarargs) != 0) {
        if (function->param_count > 0) {
            sb_append_cstr(sb, ", ");
        }
        sb_append_cstr(sb, "...");
    }
    sb_append_char(sb, ')');
}

internal void
llvm_append_function_type(StringBuilder* sb, const Sema* sema, u32 type_index)
{
    u32 return_type = llvm_function_return_type(sema, type_index);
    llvm_append_type(sb, sema, return_type);
    sb_append_cstr(sb, " (");
    u32 param_count = llvm_function_param_count(sema, type_index);
    for (u32 i = 0; i < param_count; ++i) {
        if (i > 0) {
            sb_append_cstr(sb, ", ");
        }
        llvm_append_type(
            sb, sema, llvm_function_param_type(sema, type_index, i));
    }
    if (sema != NULL && type_index < array_count(sema->types) &&
        (sema->types[type_index].flags & STF_FunctionVarargs) != 0) {
        if (param_count > 0) {
            sb_append_cstr(sb, ", ");
        }
        sb_append_cstr(sb, "...");
    }
    sb_append_char(sb, ')');
}

internal void
llvm_append_zero_value(StringBuilder* sb, const Sema* sema, u32 type_index)
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
    case STK_Box:
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
    case STK_Arena:
        sb_append_cstr(sb, "zeroinitializer");
        break;
    default:
        sb_append_cstr(sb, "0");
        break;
    }
}

internal string llvm_float_literal_string(const Sema* sema,
                                          Arena*      arena,
                                          u32         type_index,
                                          f64         value);
internal bool   llvm_eval_hir_string_constant(const Hir*   hir,
                                              const Lexer* lexer,
                                              Arena*       arena,
                                              u32          expr_index,
                                              string*      out);

internal bool llvm_expr_is_constant_value(const Hir*   hir,
                                          const Lexer* lexer,
                                          const Sema*  sema,
                                          u32          expr_index);

internal bool llvm_append_constant_expr_value(StringBuilder* sb,
                                              const Hir*     hir,
                                              const Lexer*   lexer,
                                              const Sema*    sema,
                                              Arena*         arena,
                                              u32            expr_index,
                                              u32            expected_type);

internal bool llvm_append_constant_aggregate_items(StringBuilder* sb,
                                                   const Hir*     hir,
                                                   const Lexer*   lexer,
                                                   const Sema*    sema,
                                                   Arena*         arena,
                                                   u32            expr_index,
                                                   u32 aggregate_type,
                                                   u32 item_type)
{
    if (expr_index >= array_count(hir->exprs)) {
        return false;
    }
    const HirExpr* expr = &hir->exprs[expr_index];
    for (u32 i = 0; i < expr->arg_count; ++i) {
        if (i > 0) {
            sb_append_cstr(sb, ", ");
        }
        const HirCallArg* arg = &hir->call_args[expr->first_arg + i];
        llvm_append_type(sb, sema, item_type);
        sb_append_char(sb, ' ');
        if (!llvm_append_constant_expr_value(
                sb, hir, lexer, sema, arena, arg->expr_index, item_type)) {
            return false;
        }
    }
    (void)aggregate_type;
    return true;
}

internal bool llvm_append_constant_record_value(StringBuilder* sb,
                                                const Hir*     hir,
                                                const Lexer*   lexer,
                                                const Sema*    sema,
                                                Arena*         arena,
                                                u32            expr_index,
                                                u32            record_type)
{
    if (expr_index >= array_count(hir->exprs)) {
        return false;
    }
    const HirExpr* expr        = &hir->exprs[expr_index];
    u32            field_count = llvm_record_field_count(sema, record_type);
    sb_append_cstr(sb, "{ ");
    for (u32 i = 0; i < field_count; ++i) {
        if (i > 0) {
            sb_append_cstr(sb, ", ");
        }
        u32 field_type = llvm_record_field_type(sema, record_type, i);
        llvm_append_type(sb, sema, field_type);
        sb_append_char(sb, ' ');

        u32 field_expr_index = U32_MAX;
        if (expr->kind == HIR_EXPR_Tuple) {
            if (i < expr->arg_count) {
                field_expr_index =
                    hir->call_args[expr->first_arg + i].expr_index;
            }
        } else if (expr->kind == HIR_EXPR_Plex) {
            for (u32 j = 0; j < expr->arg_count; ++j) {
                const HirCallArg* arg = &hir->call_args[expr->first_arg + j];
                if (llvm_record_field_index(
                        sema, record_type, arg->symbol_handle) == i) {
                    field_expr_index = arg->expr_index;
                    break;
                }
            }
        }

        if (field_expr_index == U32_MAX) {
            llvm_append_zero_value(sb, sema, field_type);
        } else if (!llvm_append_constant_expr_value(sb,
                                                    hir,
                                                    lexer,
                                                    sema,
                                                    arena,
                                                    field_expr_index,
                                                    field_type)) {
            return false;
        }
    }
    sb_append_cstr(sb, " }");
    return true;
}

internal bool llvm_append_constant_expr_value(StringBuilder* sb,
                                              const Hir*     hir,
                                              const Lexer*   lexer,
                                              const Sema*    sema,
                                              Arena*         arena,
                                              u32            expr_index,
                                              u32            expected_type)
{
    if (expr_index >= array_count(hir->exprs)) {
        return false;
    }

    const HirExpr* expr = &hir->exprs[expr_index];
    u32            type_index =
        expected_type != sema_no_type() ? expected_type : expr->type_index;

    switch (expr->kind) {
    case HIR_EXPR_DefaultValue:
    case HIR_EXPR_NilLiteral:
        llvm_append_zero_value(sb, sema, type_index);
        return true;
    case HIR_EXPR_IntegerLiteral:
        sb_format(sb, "%lld", (long long)expr->integer);
        return true;
    case HIR_EXPR_FloatLiteral:
        sb_append_string(
            sb,
            llvm_float_literal_string(sema, arena, type_index, expr->floating));
        return true;
    case HIR_EXPR_BoolLiteral:
        sb_append_cstr(sb, expr->boolean ? "1" : "0");
        return true;
    case HIR_EXPR_BuiltinMacro:
        {
            string name = expr->symbol_handle != U32_MAX
                              ? lex_symbol(lexer, expr->symbol_handle)
                              : (string){0};
            if (string_eq_cstr(name, "file")) {
                StringBuilder global = {0};
                sb_init(&global, arena);
                llvm_append_builtin_module_file_global_name(&global, hir);
                sb_format(sb,
                          "{ ptr " STRINGP ", i64 %zu }",
                          STRINGV(sb_to_string(&global)),
                          lexer->source.source_path.count);
                return true;
            }
            if (string_eq_cstr(name, "line")) {
                sb_format(sb, "%u", expr->source_line);
                return true;
            }
            return false;
        }
    case HIR_EXPR_StringLiteral:
        if (expr->string_is_cstring &&
            llvm_type_kind(sema, type_index) == STK_Pointer) {
            string global =
                llvm_string_global_name_string(hir, arena, expr->string_index);
            sb_append_string(sb, global);
            return true;
        }
        {
            string global =
                llvm_string_global_name_string(hir, arena, expr->string_index);
            sb_format(sb,
                      "{ ptr " STRINGP ", i64 %zu }",
                      STRINGV(global),
                      lexer->strings[expr->string_index].count);
        }
        return true;
    case HIR_EXPR_StringConcat:
        {
            Arena temp = {0};
            arena_init(&temp);
            string value = {0};
            if (!llvm_eval_hir_string_constant(
                    hir, lexer, &temp, expr_index, &value)) {
                arena_done(&temp);
                return false;
            }
            string global =
                llvm_concat_string_global_name_string(hir, arena, expr_index);
            sb_format(sb,
                      "{ ptr " STRINGP ", i64 %zu }",
                      STRINGV(global),
                      value.count);
            arena_done(&temp);
            return true;
        }
    case HIR_EXPR_Array:
        {
            SemaTypeKind type_kind = llvm_type_kind(sema, type_index);
            if (type_kind == STK_Slice) {
                string backing = llvm_const_slice_backing_name_string(
                    hir, arena, expr_index);
                sb_format(sb,
                          "{ ptr " STRINGP ", i64 %u }",
                          STRINGV(backing),
                          expr->arg_count);
                return true;
            }
            if (type_kind != STK_Array) {
                return false;
            }
            u32 item_type = llvm_collection_item_type(sema, type_index);
            sb_append_char(sb, '[');
            bool ok = llvm_append_constant_aggregate_items(
                sb, hir, lexer, sema, arena, expr_index, type_index, item_type);
            sb_append_char(sb, ']');
            return ok;
        }
    case HIR_EXPR_Tuple:
    case HIR_EXPR_Plex:
        return llvm_append_constant_record_value(
            sb, hir, lexer, sema, arena, expr_index, type_index);
    default:
        return false;
    }
}

internal bool llvm_expr_is_constant_value(const Hir*   hir,
                                          const Lexer* lexer,
                                          const Sema*  sema,
                                          u32          expr_index)
{
    if (expr_index >= array_count(hir->exprs)) {
        return false;
    }
    const HirExpr* expr = &hir->exprs[expr_index];
    switch (expr->kind) {
    case HIR_EXPR_DefaultValue:
    case HIR_EXPR_IntegerLiteral:
    case HIR_EXPR_FloatLiteral:
    case HIR_EXPR_StringLiteral:
    case HIR_EXPR_BuiltinMacro:
    case HIR_EXPR_BoolLiteral:
    case HIR_EXPR_NilLiteral:
        return true;
    case HIR_EXPR_StringConcat:
        {
            Arena temp = {0};
            arena_init(&temp);
            string value = {0};
            bool   ok    = llvm_eval_hir_string_constant(
                hir, lexer, &temp, expr_index, &value);
            arena_done(&temp);
            return ok;
        }
    case HIR_EXPR_Array:
    case HIR_EXPR_Tuple:
        for (u32 i = 0; i < expr->arg_count; ++i) {
            const HirCallArg* arg = &hir->call_args[expr->first_arg + i];
            if (!llvm_expr_is_constant_value(
                    hir, lexer, sema, arg->expr_index)) {
                return false;
            }
        }
        return true;
    case HIR_EXPR_Plex:
        for (u32 i = 0; i < expr->arg_count; ++i) {
            const HirCallArg* arg = &hir->call_args[expr->first_arg + i];
            if (!llvm_expr_is_constant_value(
                    hir, lexer, sema, arg->expr_index)) {
                return false;
            }
        }
        return true;
    default:
        return false;
    }
    (void)sema;
}

internal void
llvm_append_default_return(StringBuilder* sb, const Sema* sema, u32 return_type)
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
    u32    break_defer_count;
    u32    continue_defer_count;
    bool   emitted_break;
} LlvmControlTarget;

typedef struct {
    u32 id;
    u32 line;
    u32 scope_id;
} LlvmDebugLocation;

typedef struct {
    string source_path;
    u32    id;
} LlvmDebugFile;

typedef struct {
    u32 parent_scope_id;
    u32 file_id;
    u32 id;
} LlvmDebugFileScope;

typedef struct {
    u32 type_index;
    u32 id;
} LlvmDebugType;

typedef struct {
    u32 sema_scope_index;
    u32 parent_scope_id;
    u32 id;
} LlvmDebugLexicalScope;

typedef struct {
    u32 local_index;
    u32 scope_id;
    u32 id;
} LlvmDebugVariable;

typedef struct {
    Arena         metadata_arena;
    Arena         path_arena;
    Arena*        arena;
    StringBuilder metadata;
    const Lexer*  lexer;
    u32           next_id;
    u32           empty_id;
    u32           expression_id;
    u32           file_id;
    u32           compile_unit_id;
    u32           globals_id;
    string        source_path;
    Array(LlvmDebugLocation) locations;
    Array(LlvmDebugFile) files;
    Array(LlvmDebugFileScope) file_scopes;
    Array(LlvmDebugType) types;
    Array(LlvmDebugLexicalScope) lexical_scopes;
    Array(LlvmDebugVariable) variables;
    Array(u32) global_variables;
    bool uses_dbg_declare;
    bool uses_dbg_value;
} LlvmDebugModule;

typedef struct {
    StringBuilder*    sb;
    StringBuilder*    entry_sb;
    const Hir*        hir;
    const Lexer*      lexer;
    const Sema*       sema;
    Arena*            arena;
    const LlvmLayout* layout;
    u32               next_temp;
    u32               next_label;
    bool              block_terminated;
    bool              emitted_break;
    string            break_label;
    string            continue_label;
    string            break_value_ptr;
    u32               break_value_type;
    u32               break_defer_count;
    u32               continue_defer_count;
    u32               global_init_value_index;
    string            macro_source_path;
    u32               macro_source_line;
    const Hir*        macro_source_hir;
    bool              discard_expr_value;
    LlvmDebugModule*  debug;
    u32               debug_scope_id;
    u32               debug_decl_index;
    Array(LlvmLocalValue) locals;
    Array(LlvmLocalSlot) slots;
    Array(u32) assigned_locals;
    Array(u32) defer_block_indices;
    Array(LlvmControlTarget) control_targets;
} LlvmFunctionContext;

internal cstr llvm_debug_cstr(Arena* arena, string text)
{
    char* copy = (char*)arena_alloc(arena, text.count + 1);
    memcpy(copy, text.data, text.count);
    copy[text.count] = '\0';
    return copy;
}

internal string llvm_debug_canonical_source_path(Arena* arena, string path)
{
    if (path.count == 0) {
        return s("<memory>");
    }

    cstr nul_path  = llvm_debug_cstr(arena, path);
    cstr canonical = path_canonical(arena, nul_path);
    return canonical != NULL ? s(canonical) : path;
}

internal void llvm_debug_append_quoted(StringBuilder* sb, string text)
{
    sb_append_char(sb, '"');
    for (usize i = 0; i < text.count; ++i) {
        u8 ch = text.data[i];
        if (ch == '\\') {
            sb_append_cstr(sb, "\\\\");
        } else if (ch == '"') {
            sb_append_cstr(sb, "\\\"");
        } else if (ch == '\n') {
            sb_append_cstr(sb, "\\0A");
        } else {
            sb_append_char(sb, (char)ch);
        }
    }
    sb_append_char(sb, '"');
}

internal void
llvm_debug_split_path(string path, string* out_dir, string* out_file)
{
    usize last_separator = (usize)-1;
    for (usize i = 0; i < path.count; ++i) {
        if (path.data[i] == '/' || path.data[i] == '\\') {
            last_separator = i;
        }
    }
    if (last_separator == (usize)-1) {
        *out_dir  = s(".");
        *out_file = path;
        return;
    }
    *out_dir = string_from(path.data, last_separator == 0 ? 1 : last_separator);
    *out_file = string_from(path.data + last_separator + 1,
                            path.count - last_separator - 1);
}

internal u32 llvm_debug_alloc_id(LlvmDebugModule* debug)
{
    return debug != NULL ? debug->next_id++ : 0;
}

internal void llvm_debug_init(LlvmDebugModule* debug,
                              Arena*           arena,
                              const Lexer*     lexer,
                              bool             has_globals)
{
    *debug = (LlvmDebugModule){0};
    arena_init(&debug->metadata_arena);
    arena_init(&debug->path_arena);
    debug->arena = &debug->path_arena;
    (void)arena;

    string source_path = llvm_debug_canonical_source_path(
        debug->arena, lexer->source.source_path);
    debug->source_path = source_path;
    string directory   = {0};
    string filename    = {0};
    llvm_debug_split_path(source_path, &directory, &filename);

    sb_init(&debug->metadata, &debug->metadata_arena);
    debug->lexer           = lexer;

    debug->compile_unit_id = llvm_debug_alloc_id(debug);
    debug->file_id         = llvm_debug_alloc_id(debug);
    debug->empty_id        = llvm_debug_alloc_id(debug);
    u32 debug_info_flag    = llvm_debug_alloc_id(debug);
    u32 dwarf_flag         = llvm_debug_alloc_id(debug);
    debug->expression_id   = llvm_debug_alloc_id(debug);
    debug->globals_id      = has_globals ? llvm_debug_alloc_id(debug) : 0;

    sb_format(&debug->metadata,
              "!llvm.dbg.cu = !{!%u}\n"
              "!llvm.module.flags = !{!%u, !%u}\n\n",
              debug->compile_unit_id,
              debug_info_flag,
              dwarf_flag);

    sb_format(&debug->metadata,
              "!%u = distinct !DICompileUnit(language: "
              "DW_LANG_C, file: !%u, producer: ",
              debug->compile_unit_id,
              debug->file_id);
    llvm_debug_append_quoted(&debug->metadata, s("Nerd"));
    sb_format(&debug->metadata,
              ", isOptimized: false, runtimeVersion: 0, emissionKind: "
              "FullDebug, enums: !%u",
              debug->empty_id);
    if (debug->globals_id != 0) {
        sb_format(&debug->metadata, ", globals: !%u", debug->globals_id);
    }
    sb_append_cstr(&debug->metadata, ")\n");

    sb_format(&debug->metadata, "!%u = !DIFile(filename: ", debug->file_id);
    llvm_debug_append_quoted(&debug->metadata, filename);
    sb_append_cstr(&debug->metadata, ", directory: ");
    llvm_debug_append_quoted(&debug->metadata, directory);
    sb_append_cstr(&debug->metadata, ")\n");
    array_push(
        debug->files,
        (LlvmDebugFile){.source_path = source_path, .id = debug->file_id});

    sb_format(&debug->metadata,
              "!%u = !{}\n"
              "!%u = !DIExpression()\n"
              "!%u = !{i32 2, !\"Debug Info Version\", i32 3}\n"
              "!%u = !{i32 2, !\"Dwarf Version\", i32 5}\n",
              debug->empty_id,
              debug->expression_id,
              debug_info_flag,
              dwarf_flag);
}

internal void llvm_debug_done(LlvmDebugModule* debug)
{
    array_free(debug->locations);
    array_free(debug->files);
    array_free(debug->file_scopes);
    array_free(debug->types);
    array_free(debug->lexical_scopes);
    array_free(debug->variables);
    array_free(debug->global_variables);
    arena_done(&debug->metadata_arena);
    arena_done(&debug->path_arena);
}

internal string llvm_debug_function_source_name(const Hir*         hir,
                                                const Lexer*       lexer,
                                                const Sema*        sema,
                                                Arena*             arena,
                                                const HirFunction* function,
                                                u32 function_index)
{
    if (function->decl_index < array_count(sema->decls)) {
        u32 symbol = sema->decls[function->decl_index].symbol_handle;
        if (symbol != U32_MAX) {
            return lex_symbol(lexer, symbol);
        }
    }
    return llvm_function_name_string(hir, lexer, arena, function_index);
}

internal bool llvm_function_has_binding_alias(const Sema* sema,
                                              const Hir*  hir,
                                              u32         function_index)
{
    if (hir == NULL || function_index >= array_count(hir->functions)) {
        return false;
    }
    const HirFunction* function = &hir->functions[function_index];
    if (function->kind == HIR_FUNCTION_Ffi) {
        return false;
    }
    if (sema != NULL && sema->program != NULL &&
        hir->current_module_index != sema->program->root_module_index &&
        llvm_program_function_symbol_conflicts(sema, hir, function_index)) {
        return false;
    }
    return llvm_function_symbol_handle(hir, function_index) != U32_MAX;
}

internal string llvm_debug_function_linkage_name(const Hir*   hir,
                                                 const Lexer* lexer,
                                                 const Sema*  sema,
                                                 Arena*       arena,
                                                 u32          function_index)
{
    StringBuilder sb = {0};
    sb_init(&sb, arena);
    if (llvm_function_has_binding_alias(sema, hir, function_index)) {
        llvm_append_symbol_name(
            &sb,
            lex_symbol(lexer,
                       llvm_function_symbol_handle(hir, function_index)));
    } else {
        llvm_append_function_name(&sb, hir, lexer, function_index);
    }

    string name = sb_to_string(&sb);
    if (name.count > 0 && name.data[0] == '@') {
        name.data += 1;
        name.count -= 1;
    }
    if (name.count > 0 && name.data[0] == '$') {
        name.data += 1;
        name.count -= 1;
    }
    return name;
}

internal u32 llvm_debug_function_line(const Hir*         hir,
                                      const HirFunction* function)
{
    if (function->body_block_index >= array_count(hir->blocks)) {
        return 1;
    }
    const HirBlock* block = &hir->blocks[function->body_block_index];
    for (u32 i = 0; i < block->stmt_count; ++i) {
        u32 stmt_index = block->stmt_indices[i];
        if (stmt_index < array_count(hir->stmts) &&
            hir->stmts[stmt_index].source_line > 0) {
            return hir->stmts[stmt_index].source_line;
        }
    }
    return 1;
}

internal u32 llvm_debug_add_function(LlvmDebugModule*   debug,
                                     const Hir*         hir,
                                     const Lexer*       lexer,
                                     const Sema*        sema,
                                     Arena*             arena,
                                     const HirFunction* function,
                                     u32                function_index)
{
    if (debug == NULL) {
        return 0;
    }

    u32   id         = llvm_debug_alloc_id(debug);
    u32   type_id    = llvm_debug_alloc_id(debug);
    Arena name_arena = {0};
    arena_init(&name_arena);
    string source_name = llvm_debug_function_source_name(
        hir, lexer, sema, &name_arena, function, function_index);
    string linkage_name = llvm_debug_function_linkage_name(
        hir, lexer, sema, &name_arena, function_index);
    u32 line = llvm_debug_function_line(hir, function);

    sb_format(&debug->metadata,
              "!%u = !DISubroutineType(types: !%u)\n",
              type_id,
              debug->empty_id);
    sb_format(&debug->metadata, "!%u = distinct !DISubprogram(name: ", id);
    llvm_debug_append_quoted(&debug->metadata, source_name);
    sb_append_cstr(&debug->metadata, ", linkageName: ");
    llvm_debug_append_quoted(&debug->metadata, linkage_name);
    sb_format(&debug->metadata,
              ", scope: !%u, file: !%u, line: %u, type: !%u, scopeLine: %u, "
              "spFlags: DISPFlagDefinition, unit: !%u, retainedNodes: !%u)\n",
              debug->file_id,
              debug->file_id,
              line,
              type_id,
              line,
              debug->compile_unit_id,
              debug->empty_id);
    arena_done(&name_arena);
    (void)arena;
    return id;
}

internal u32 llvm_debug_file(LlvmDebugModule* debug, string source_path)
{
    if (debug == NULL || source_path.count == 0) {
        return debug != NULL ? debug->file_id : 0;
    }

    source_path = llvm_debug_canonical_source_path(debug->arena, source_path);
    for (u32 i = 0; i < array_count(debug->files); ++i) {
        LlvmDebugFile* file = &debug->files[i];
        if (string_eq(file->source_path, source_path)) {
            return file->id;
        }
    }

    string directory = {0};
    string filename  = {0};
    llvm_debug_split_path(source_path, &directory, &filename);

    u32 id = llvm_debug_alloc_id(debug);
    sb_format(&debug->metadata, "!%u = !DIFile(filename: ", id);
    llvm_debug_append_quoted(&debug->metadata, filename);
    sb_append_cstr(&debug->metadata, ", directory: ");
    llvm_debug_append_quoted(&debug->metadata, directory);
    sb_append_cstr(&debug->metadata, ")\n");
    array_push(debug->files,
               (LlvmDebugFile){.source_path = source_path, .id = id});
    return id;
}

internal u32 llvm_debug_file_scope(LlvmDebugModule* debug,
                                   u32              parent_scope_id,
                                   u32              file_id)
{
    if (debug == NULL || parent_scope_id == 0 || file_id == 0 ||
        file_id == debug->file_id) {
        return parent_scope_id;
    }

    for (u32 i = 0; i < array_count(debug->file_scopes); ++i) {
        LlvmDebugFileScope* scope = &debug->file_scopes[i];
        if (scope->parent_scope_id == parent_scope_id &&
            scope->file_id == file_id) {
            return scope->id;
        }
    }

    u32 id = llvm_debug_alloc_id(debug);
    sb_format(&debug->metadata,
              "!%u = !DILexicalBlockFile(scope: !%u, file: !%u, "
              "discriminator: 0)\n",
              id,
              parent_scope_id,
              file_id);
    array_push(debug->file_scopes,
               (LlvmDebugFileScope){.parent_scope_id = parent_scope_id,
                                    .file_id         = file_id,
                                    .id              = id});
    return id;
}

internal u32 llvm_debug_location(LlvmDebugModule* debug, u32 line, u32 scope_id)
{
    if (debug == NULL || line == 0 || scope_id == 0) {
        return 0;
    }
    for (u32 i = 0; i < array_count(debug->locations); ++i) {
        LlvmDebugLocation* location = &debug->locations[i];
        if (location->line == line && location->scope_id == scope_id) {
            return location->id;
        }
    }

    u32 id = llvm_debug_alloc_id(debug);
    array_push(debug->locations,
               (LlvmDebugLocation){
                   .id       = id,
                   .line     = line,
                   .scope_id = scope_id,
               });
    sb_format(&debug->metadata,
              "!%u = !DILocation(line: %u, column: 1, scope: !%u)\n",
              id,
              line,
              scope_id);
    return id;
}

internal u32 llvm_debug_source_location(LlvmDebugModule* debug,
                                        u32              line,
                                        string           source_path,
                                        u32              scope_id)
{
    if (debug == NULL || line == 0 || scope_id == 0 || source_path.count == 0) {
        return llvm_debug_location(debug, line, scope_id);
    }

    source_path = llvm_debug_canonical_source_path(debug->arena, source_path);
    if (string_eq(source_path, debug->source_path)) {
        return llvm_debug_location(debug, line, scope_id);
    }

    u32 file_id    = llvm_debug_file(debug, source_path);
    u32 file_scope = llvm_debug_file_scope(debug, scope_id, file_id);
    return llvm_debug_location(debug, line, file_scope);
}

internal u32 llvm_debug_lexical_scope(LlvmDebugModule* debug,
                                      u32              sema_scope_index,
                                      u32              parent_scope_id,
                                      u32              line,
                                      string           source_path)
{
    if (debug == NULL || sema_scope_index == U32_MAX || parent_scope_id == 0) {
        return parent_scope_id;
    }

    for (u32 i = 0; i < array_count(debug->lexical_scopes); ++i) {
        LlvmDebugLexicalScope* scope = &debug->lexical_scopes[i];
        if (scope->sema_scope_index == sema_scope_index &&
            scope->parent_scope_id == parent_scope_id) {
            return scope->id;
        }
    }

    u32 file_id = source_path.count > 0 ? llvm_debug_file(debug, source_path)
                                        : debug->file_id;
    if (line == 0) {
        line = 1;
    }

    u32 id = llvm_debug_alloc_id(debug);
    sb_format(&debug->metadata,
              "!%u = distinct !DILexicalBlock(scope: !%u, file: !%u, "
              "line: %u, column: 1)\n",
              id,
              parent_scope_id,
              file_id,
              line);
    array_push(debug->lexical_scopes,
               (LlvmDebugLexicalScope){
                   .sema_scope_index = sema_scope_index,
                   .parent_scope_id  = parent_scope_id,
                   .id               = id,
               });
    return id;
}

internal string llvm_type_string(LlvmFunctionContext* ctx, u32 type_index);
internal u32    llvm_local_type(LlvmFunctionContext* ctx, u32 local_index);

internal bool llvm_debug_type_info(const Sema* sema,
                                   u32         type_index,
                                   string*     out_name,
                                   u32*        out_size,
                                   cstr*       out_encoding,
                                   bool*       out_pointer)
{
    *out_pointer = false;
    switch (llvm_type_kind(sema, type_index)) {
    case STK_Bool:
        *out_name     = s("bool");
        *out_size     = 8;
        *out_encoding = "DW_ATE_boolean";
        return true;
    case STK_I8:
        *out_name     = s("i8");
        *out_size     = 8;
        *out_encoding = "DW_ATE_signed";
        return true;
    case STK_I16:
        *out_name     = s("i16");
        *out_size     = 16;
        *out_encoding = "DW_ATE_signed";
        return true;
    case STK_I32:
        *out_name     = s("i32");
        *out_size     = 32;
        *out_encoding = "DW_ATE_signed";
        return true;
    case STK_I64:
        *out_name     = s("i64");
        *out_size     = 64;
        *out_encoding = "DW_ATE_signed";
        return true;
    case STK_U8:
        *out_name     = s("u8");
        *out_size     = 8;
        *out_encoding = "DW_ATE_unsigned";
        return true;
    case STK_U16:
        *out_name     = s("u16");
        *out_size     = 16;
        *out_encoding = "DW_ATE_unsigned";
        return true;
    case STK_U32:
        *out_name     = s("u32");
        *out_size     = 32;
        *out_encoding = "DW_ATE_unsigned";
        return true;
    case STK_U64:
        *out_name     = s("u64");
        *out_size     = 64;
        *out_encoding = "DW_ATE_unsigned";
        return true;
    case STK_Isize:
        *out_name     = s("isize");
        *out_size     = 64;
        *out_encoding = "DW_ATE_signed";
        return true;
    case STK_Usize:
        *out_name     = s("usize");
        *out_size     = 64;
        *out_encoding = "DW_ATE_unsigned";
        return true;
    case STK_F32:
        *out_name     = s("f32");
        *out_size     = 32;
        *out_encoding = "DW_ATE_float";
        return true;
    case STK_F64:
        *out_name     = s("f64");
        *out_size     = 64;
        *out_encoding = "DW_ATE_float";
        return true;
    case STK_Pointer:
        *out_name    = s("ptr");
        *out_size    = 64;
        *out_pointer = true;
        return true;
    case STK_Box:
        *out_name    = s("box");
        *out_size    = 64;
        *out_pointer = true;
        return true;
    case STK_DynamicArray:
        *out_name    = s("ptr");
        *out_size    = 64;
        *out_pointer = true;
        return true;
    default:
        return false;
    }
}

internal u32 llvm_debug_type(LlvmDebugModule* debug,
                             const Sema*      sema,
                             u32              type_index);

internal string llvm_debug_type_name(LlvmDebugModule* debug,
                                     const Sema*      sema,
                                     u32              type_index,
                                     Arena*           arena)
{
    if (debug != NULL && debug->lexer != NULL) {
        return sema_type_name(debug->lexer, sema, arena, type_index);
    }

    string name       = {0};
    u32    size       = 0;
    cstr   encoding   = NULL;
    bool   is_pointer = false;
    if (llvm_debug_type_info(
            sema, type_index, &name, &size, &encoding, &is_pointer)) {
        return name;
    }
    return s("<unknown>");
}

internal u32 llvm_debug_record_field_count(const Sema* sema, u32 type_index)
{
    SemaTypeKind kind = llvm_type_kind(sema, type_index);
    if (kind == STK_String || kind == STK_Slice) {
        return 2;
    }
    if (kind == STK_Enum) {
        return 2;
    }
    if (kind == STK_Tuple || kind == STK_Plex || kind == STK_Union) {
        return sema->types[type_index].param_count;
    }
    return 0;
}

internal string llvm_debug_record_field_name(LlvmDebugModule* debug,
                                             const Sema*      sema,
                                             u32              type_index,
                                             u32              field_index,
                                             Arena*           arena)
{
    SemaTypeKind kind = llvm_type_kind(sema, type_index);
    if ((kind == STK_String || kind == STK_Slice) && field_index == 0) {
        return s("data");
    }
    if ((kind == STK_String || kind == STK_Slice) && field_index == 1) {
        return s("count");
    }
    if (kind == STK_Enum && field_index == 0) {
        return s("tag");
    }
    if (kind == STK_Enum && field_index == 1) {
        return s("payload");
    }
    if (kind == STK_Tuple) {
        return string_format(arena, "%u", field_index);
    }
    if ((kind == STK_Plex || kind == STK_Union) && debug != NULL &&
        debug->lexer != NULL &&
        field_index < sema->types[type_index].param_count) {
        u32 symbol =
            sema->type_param_symbols[sema->types[type_index].first_param_type +
                                     field_index];
        return lex_symbol(debug->lexer, symbol);
    }
    return string_format(arena, "field%u", field_index);
}

internal u32 llvm_debug_record_field_type(const Sema* sema,
                                          u32         type_index,
                                          u32         field_index)
{
    SemaTypeKind kind = llvm_type_kind(sema, type_index);
    if (kind == STK_String) {
        if (field_index == 0) {
            return llvm_pointer_type(sema, llvm_builtin_type(sema, STK_U8));
        }
        return field_index == 1 ? llvm_builtin_type(sema, STK_Usize)
                                : sema_no_type();
    }
    if (kind == STK_Slice) {
        if (field_index == 0) {
            return llvm_pointer_type(
                sema, llvm_collection_item_type(sema, type_index));
        }
        return field_index == 1 ? llvm_builtin_type(sema, STK_Usize)
                                : sema_no_type();
    }
    if (kind == STK_Enum && field_index == 0) {
        return llvm_builtin_type(sema, STK_U64);
    }
    if ((kind == STK_Tuple || kind == STK_Plex || kind == STK_Union) &&
        field_index < sema->types[type_index].param_count) {
        return sema->type_param_types[sema->types[type_index].first_param_type +
                                      field_index];
    }
    return sema_no_type();
}

internal u32 llvm_debug_record_field_offset_bits(const Sema* sema,
                                                 u32         type_index,
                                                 u32         field_index)
{
    const LlvmLayout* layout = llvm_default_layout();
    SemaTypeKind      kind   = llvm_type_kind(sema, type_index);
    if ((kind == STK_String || kind == STK_Slice) && field_index == 1) {
        return layout->pointer_bits;
    }
    if (kind == STK_Enum && field_index == 1) {
        return layout->enum_tag_bits;
    }
    if (kind == STK_Union) {
        return 0;
    }
    if (kind != STK_Tuple && kind != STK_Plex) {
        return 0;
    }

    u32 offset = 0;
    for (u32 i = 0; i < field_index && i < sema->types[type_index].param_count;
         ++i) {
        u32 field_type =
            sema->type_param_types[sema->types[type_index].first_param_type +
                                   i];
        offset =
            llvm_align_bits(offset, llvm_type_align_bits(sema, field_type));
        offset += llvm_type_storage_bits(sema, field_type);
    }

    u32 field_type =
        llvm_debug_record_field_type(sema, type_index, field_index);
    if (field_type != sema_no_type()) {
        offset =
            llvm_align_bits(offset, llvm_type_align_bits(sema, field_type));
    }
    return offset;
}

internal bool llvm_debug_type_is_record_like(const Sema* sema, u32 type_index)
{
    SemaTypeKind kind = llvm_type_kind(sema, type_index);
    return kind == STK_String || kind == STK_Slice || kind == STK_Tuple ||
           kind == STK_Plex || kind == STK_Union || kind == STK_Enum;
}

internal bool llvm_debug_type_is_array(const Sema* sema, u32 type_index)
{
    return llvm_type_kind(sema, type_index) == STK_Array;
}

internal u32 llvm_debug_emit_fallback_basic_type(LlvmDebugModule* debug,
                                                 string           name,
                                                 u32              size,
                                                 cstr             encoding)
{
    u32 id = llvm_debug_alloc_id(debug);
    sb_format(&debug->metadata, "!%u = !DIBasicType(name: ", id);
    llvm_debug_append_quoted(&debug->metadata, name);
    sb_format(&debug->metadata, ", size: %u, encoding: %s)\n", size, encoding);
    return id;
}

internal u32 llvm_debug_emit_fallback_pointer_type(LlvmDebugModule* debug)
{
    u32 id = llvm_debug_alloc_id(debug);
    sb_format(&debug->metadata,
              "!%u = !DIDerivedType(tag: DW_TAG_pointer_type, name: "
              "\"ptr\", baseType: null, size: %u)\n",
              id,
              llvm_default_layout()->pointer_bits);
    return id;
}

internal u32 llvm_debug_record_fallback_field_type(LlvmDebugModule* debug,
                                                   const Sema*      sema,
                                                   u32              type_index,
                                                   u32              field_index,
                                                   u32*             out_size)
{
    SemaTypeKind kind = llvm_type_kind(sema, type_index);
    if ((kind == STK_String || kind == STK_Slice) && field_index == 0) {
        *out_size = llvm_default_layout()->pointer_bits;
        return llvm_debug_emit_fallback_pointer_type(debug);
    }
    if ((kind == STK_String || kind == STK_Slice) && field_index == 1) {
        *out_size = llvm_default_layout()->size_bits;
        return llvm_debug_emit_fallback_basic_type(
            debug, s("usize"), *out_size, "DW_ATE_unsigned");
    }
    if (kind == STK_Enum && field_index == 1) {
        *out_size = llvm_enum_storage_payload_bits(sema, type_index);
        if (*out_size == 0) {
            *out_size = 8;
        }
        return llvm_debug_emit_fallback_basic_type(
            debug, s("<payload>"), *out_size, "DW_ATE_unsigned");
    }

    *out_size = 8;
    return llvm_debug_emit_fallback_basic_type(
        debug, s("<unknown>"), *out_size, "DW_ATE_unsigned");
}

internal void llvm_debug_emit_composite_type(LlvmDebugModule* debug,
                                             const Sema*      sema,
                                             u32              type_index,
                                             u32              type_id)
{
    Arena name_arena = {0};
    arena_init(&name_arena);

    u32 elements_id       = llvm_debug_alloc_id(debug);
    u32 field_count       = llvm_debug_record_field_count(sema, type_index);
    u32 size              = llvm_type_storage_bits(sema, type_index);
    Array(u32) member_ids = NULL;
    array_requires_capacity(member_ids, field_count);
    for (u32 i = 0; i < field_count; ++i) {
        array_push(member_ids, llvm_debug_alloc_id(debug));
    }

    sb_format(&debug->metadata,
              "!%u = !DICompositeType(tag: %s, name: ",
              type_id,
              llvm_type_kind(sema, type_index) == STK_Union
                  ? "DW_TAG_union_type"
                  : "DW_TAG_structure_type");
    llvm_debug_append_quoted(
        &debug->metadata,
        llvm_debug_type_name(debug, sema, type_index, &name_arena));
    sb_format(&debug->metadata,
              ", file: !%u, size: %u, elements: !%u)\n",
              debug->file_id,
              size,
              elements_id);

    sb_format(&debug->metadata, "!%u = !{", elements_id);
    for (u32 i = 0; i < field_count; ++i) {
        if (i > 0) {
            sb_append_cstr(&debug->metadata, ", ");
        }
        sb_format(&debug->metadata, "!%u", member_ids[i]);
    }
    sb_append_cstr(&debug->metadata, "}\n");

    for (u32 i = 0; i < field_count; ++i) {
        u32 field_id   = member_ids[i];
        u32 field_type = llvm_debug_record_field_type(sema, type_index, i);
        u32 field_debug_type = field_type == sema_no_type()
                                   ? 0
                                   : llvm_debug_type(debug, sema, field_type);
        u32 field_size       = field_type == sema_no_type()
                                   ? 0
                                   : llvm_type_storage_bits(sema, field_type);
        if (field_debug_type == 0) {
            field_debug_type = llvm_debug_record_fallback_field_type(
                debug, sema, type_index, i, &field_size);
        }
        u32 field_offset =
            llvm_debug_record_field_offset_bits(sema, type_index, i);

        sb_format(&debug->metadata,
                  "!%u = !DIDerivedType(tag: DW_TAG_member, name: ",
                  field_id);
        llvm_debug_append_quoted(&debug->metadata,
                                 llvm_debug_record_field_name(
                                     debug, sema, type_index, i, &name_arena));
        sb_format(&debug->metadata,
                  ", scope: !%u, file: !%u",
                  type_id,
                  debug->file_id);
        sb_format(&debug->metadata, ", baseType: !%u", field_debug_type);
        sb_format(&debug->metadata,
                  ", size: %u, offset: %u)\n",
                  field_size,
                  field_offset);
    }

    array_free(member_ids);
    arena_done(&name_arena);
}

internal void llvm_debug_emit_array_type(LlvmDebugModule* debug,
                                         const Sema*      sema,
                                         u32              type_index,
                                         u32              type_id)
{
    Arena name_arena = {0};
    arena_init(&name_arena);

    u32 item_type       = llvm_collection_item_type(sema, type_index);
    u32 item_debug_type = llvm_debug_type(debug, sema, item_type);
    u32 elements_id     = llvm_debug_alloc_id(debug);
    u32 subrange_id     = llvm_debug_alloc_id(debug);

    sb_format(&debug->metadata,
              "!%u = !DICompositeType(tag: DW_TAG_array_type, name: ",
              type_id);
    llvm_debug_append_quoted(
        &debug->metadata,
        llvm_debug_type_name(debug, sema, type_index, &name_arena));
    sb_format(&debug->metadata,
              ", file: !%u, baseType: !%u, size: %u, elements: !%u)\n",
              debug->file_id,
              item_debug_type,
              llvm_type_storage_bits(sema, type_index),
              elements_id);
    sb_format(&debug->metadata, "!%u = !{!%u}\n", elements_id, subrange_id);
    sb_format(&debug->metadata,
              "!%u = !DISubrange(count: %u, lowerBound: 0)\n",
              subrange_id,
              sema->types[type_index].return_type);

    arena_done(&name_arena);
}

internal u32 llvm_debug_type(LlvmDebugModule* debug,
                             const Sema*      sema,
                             u32              type_index)
{
    if (debug == NULL) {
        return 0;
    }
    for (u32 i = 0; i < array_count(debug->types); ++i) {
        if (debug->types[i].type_index == type_index) {
            return debug->types[i].id;
        }
    }

    string name       = {0};
    u32    size       = 0;
    cstr   encoding   = NULL;
    bool   is_pointer = false;
    bool   is_record  = llvm_debug_type_is_record_like(sema, type_index);
    bool   is_array   = llvm_debug_type_is_array(sema, type_index);
    if (!llvm_debug_type_info(
            sema, type_index, &name, &size, &encoding, &is_pointer) &&
        !is_record && !is_array) {
        return 0;
    }

    u32 id = llvm_debug_alloc_id(debug);
    array_push(debug->types,
               (LlvmDebugType){
                   .type_index = type_index,
                   .id         = id,
               });
    if (is_record) {
        llvm_debug_emit_composite_type(debug, sema, type_index, id);
    } else if (is_array) {
        llvm_debug_emit_array_type(debug, sema, type_index, id);
    } else if (is_pointer) {
        Arena name_arena = {0};
        arena_init(&name_arena);
        if (llvm_type_kind(sema, type_index) == STK_DynamicArray ||
            llvm_type_kind(sema, type_index) == STK_Box) {
            name = llvm_debug_type_name(debug, sema, type_index, &name_arena);
        }
        u32 base_type = 0;
        if (type_index < array_count(sema->types) &&
            sema->types[type_index].first_param_type != sema_no_type()) {
            base_type = llvm_debug_type(
                debug, sema, sema->types[type_index].first_param_type);
        }
        sb_format(&debug->metadata,
                  "!%u = !DIDerivedType(tag: DW_TAG_pointer_type, name: ",
                  id);
        llvm_debug_append_quoted(&debug->metadata, name);
        if (base_type != 0) {
            sb_format(&debug->metadata, ", baseType: !%u", base_type);
        } else {
            sb_append_cstr(&debug->metadata, ", baseType: null");
        }
        sb_format(&debug->metadata, ", size: %u)\n", size);
        arena_done(&name_arena);
    } else {
        sb_format(&debug->metadata, "!%u = !DIBasicType(name: ", id);
        llvm_debug_append_quoted(&debug->metadata, name);
        sb_format(
            &debug->metadata, ", size: %u, encoding: %s)\n", size, encoding);
    }
    return id;
}

internal u32 llvm_debug_local_line(const Lexer* lexer,
                                   const Sema*  sema,
                                   u32          local_index)
{
    if (lexer == NULL || sema == NULL ||
        local_index >= array_count(sema->locals)) {
        return 0;
    }
    u32 token_index = sema->locals[local_index].decl_token_index;
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

internal u32 llvm_debug_local_variable(LlvmDebugModule* debug,
                                       const Lexer*     lexer,
                                       const Sema*      sema,
                                       u32              local_index,
                                       u32              type_index,
                                       u32              scope_id,
                                       u32              arg_index)
{
    if (debug == NULL || lexer == NULL || sema == NULL ||
        local_index >= array_count(sema->locals) || scope_id == 0) {
        return 0;
    }
    for (u32 i = 0; i < array_count(debug->variables); ++i) {
        if (debug->variables[i].local_index == local_index &&
            debug->variables[i].scope_id == scope_id) {
            return debug->variables[i].id;
        }
    }

    const SemaLocal* local = &sema->locals[local_index];
    if (local->symbol_handle == U32_MAX) {
        return 0;
    }
    string name = lex_symbol(lexer, local->symbol_handle);
    if (name.count == 0 || string_eq_cstr(name, "_")) {
        return 0;
    }
    u32 type_id = llvm_debug_type(debug, sema, type_index);
    if (type_id == 0) {
        return 0;
    }

    u32 line = llvm_debug_local_line(lexer, sema, local_index);
    if (line == 0) {
        line = 1;
    }
    u32 id = llvm_debug_alloc_id(debug);
    array_push(debug->variables,
               (LlvmDebugVariable){
                   .local_index = local_index,
                   .scope_id    = scope_id,
                   .id          = id,
               });
    sb_format(&debug->metadata, "!%u = !DILocalVariable(name: ", id);
    llvm_debug_append_quoted(&debug->metadata, name);
    sb_format(&debug->metadata,
              ", scope: !%u, file: !%u, line: %u",
              scope_id,
              debug->file_id,
              line);
    if (arg_index > 0) {
        sb_format(&debug->metadata, ", arg: %u", arg_index);
    }
    sb_format(&debug->metadata, ", type: !%u)\n", type_id);
    return id;
}

internal void llvm_debug_emit_declare(LlvmFunctionContext* ctx,
                                      u32                  local_index,
                                      string               ptr,
                                      u32                  type_index,
                                      u32                  arg_index)
{
    if (ctx->sema == NULL || local_index >= array_count(ctx->sema->locals) ||
        ctx->sema->locals[local_index].owner_decl_index !=
            ctx->debug_decl_index) {
        return;
    }
    u32 variable = llvm_debug_local_variable(ctx->debug,
                                             ctx->lexer,
                                             ctx->sema,
                                             local_index,
                                             type_index,
                                             ctx->debug_scope_id,
                                             arg_index);
    if (variable == 0) {
        return;
    }
    ctx->debug->uses_dbg_declare = true;
    u32 line = llvm_debug_local_line(ctx->lexer, ctx->sema, local_index);
    if (line == 0) {
        line = 1;
    }
    u32 location = llvm_debug_location(ctx->debug, line, ctx->debug_scope_id);
    sb_format(ctx->entry_sb != NULL ? ctx->entry_sb : ctx->sb,
              "  call void @llvm.dbg.declare(metadata ptr " STRINGP
              ", metadata !%u, metadata !%u)",
              STRINGV(ptr),
              variable,
              ctx->debug->expression_id);
    if (location != 0) {
        sb_format(ctx->entry_sb != NULL ? ctx->entry_sb : ctx->sb,
                  ", !dbg !%u",
                  location);
    }
    sb_append_char(ctx->entry_sb != NULL ? ctx->entry_sb : ctx->sb, '\n');
}

internal void llvm_debug_emit_value(LlvmFunctionContext* ctx,
                                    u32                  local_index,
                                    LlvmValue            value)
{
    if (!value.ok) {
        return;
    }
    u32 local_type = llvm_local_type(ctx, local_index);
    if (local_type == sema_no_type() || local_type != value.type_index) {
        return;
    }
    if (ctx->sema == NULL || local_index >= array_count(ctx->sema->locals) ||
        ctx->sema->locals[local_index].owner_decl_index !=
            ctx->debug_decl_index) {
        return;
    }
    u32 variable = llvm_debug_local_variable(ctx->debug,
                                             ctx->lexer,
                                             ctx->sema,
                                             local_index,
                                             local_type,
                                             ctx->debug_scope_id,
                                             0);
    if (variable == 0) {
        return;
    }
    string type                = llvm_type_string(ctx, value.type_index);
    ctx->debug->uses_dbg_value = true;
    u32 location               = llvm_debug_location(
        ctx->debug,
        llvm_debug_local_line(ctx->lexer, ctx->sema, local_index),
        ctx->debug_scope_id);
    sb_format(ctx->sb,
              "  call void @llvm.dbg.value(metadata " STRINGP " " STRINGP
              ", metadata !%u, metadata !%u)",
              STRINGV(type),
              STRINGV(value.value),
              variable,
              ctx->debug->expression_id);
    if (location != 0) {
        sb_format(ctx->sb, ", !dbg !%u", location);
    }
    sb_append_char(ctx->sb, '\n');
}

internal void llvm_debug_emit_param_value(LlvmFunctionContext* ctx,
                                          u32                  local_index,
                                          LlvmValue            value,
                                          u32                  arg_index)
{
    if (!value.ok || arg_index == 0) {
        return;
    }
    u32 local_type = llvm_local_type(ctx, local_index);
    if (local_type == sema_no_type() || local_type != value.type_index) {
        return;
    }
    if (ctx->sema == NULL || local_index >= array_count(ctx->sema->locals) ||
        ctx->sema->locals[local_index].owner_decl_index !=
            ctx->debug_decl_index) {
        return;
    }
    u32 variable = llvm_debug_local_variable(ctx->debug,
                                             ctx->lexer,
                                             ctx->sema,
                                             local_index,
                                             local_type,
                                             ctx->debug_scope_id,
                                             arg_index);
    if (variable == 0) {
        return;
    }
    string type                = llvm_type_string(ctx, value.type_index);
    ctx->debug->uses_dbg_value = true;
    u32 location               = llvm_debug_location(
        ctx->debug,
        llvm_debug_local_line(ctx->lexer, ctx->sema, local_index),
        ctx->debug_scope_id);
    sb_format(ctx->sb,
              "  call void @llvm.dbg.value(metadata " STRINGP " " STRINGP
              ", metadata !%u, metadata !%u)",
              STRINGV(type),
              STRINGV(value.value),
              variable,
              ctx->debug->expression_id);
    if (location != 0) {
        sb_format(ctx->sb, ", !dbg !%u", location);
    }
    sb_append_char(ctx->sb, '\n');
}

internal u32 llvm_debug_global_variable(LlvmDebugModule* debug,
                                        const Hir*       hir,
                                        const Lexer*     lexer,
                                        const Sema*      sema,
                                        u32              value_index,
                                        bool             exported)
{
    if (debug == NULL || hir == NULL || lexer == NULL || sema == NULL ||
        value_index >= array_count(hir->values)) {
        return 0;
    }

    const HirValue* value = &hir->values[value_index];
    if (value->kind != HIR_VALUE_Global) {
        return 0;
    }
    (void)exported;

    u32 symbol_handle = llvm_value_symbol_handle(hir, value_index);
    if (symbol_handle == U32_MAX) {
        return 0;
    }

    string name = lex_symbol(lexer, symbol_handle);
    if (name.count == 0) {
        return 0;
    }

    u32 type_id = llvm_debug_type(debug, sema, value->type_index);
    if (type_id == 0) {
        return 0;
    }

    u32 variable_id   = llvm_debug_alloc_id(debug);
    u32 expression_id = llvm_debug_alloc_id(debug);
    sb_format(&debug->metadata,
              "!%u = distinct !DIGlobalVariable(name: ",
              variable_id);
    llvm_debug_append_quoted(&debug->metadata, name);
    sb_format(&debug->metadata,
              ", scope: !%u, file: !%u, line: 1, type: !%u, "
              "isLocal: false, isDefinition: true)\n",
              debug->compile_unit_id,
              debug->file_id,
              type_id);
    sb_format(&debug->metadata,
              "!%u = !DIGlobalVariableExpression(var: !%u, expr: !%u)\n",
              expression_id,
              variable_id,
              debug->expression_id);
    array_push(debug->global_variables, expression_id);
    return expression_id;
}

internal void llvm_debug_emit_global_list(LlvmDebugModule* debug)
{
    if (debug == NULL || debug->globals_id == 0) {
        return;
    }
    sb_format(&debug->metadata, "!%u = !{", debug->globals_id);
    for (u32 i = 0; i < array_count(debug->global_variables); ++i) {
        if (i > 0) {
            sb_append_cstr(&debug->metadata, ", ");
        }
        sb_format(&debug->metadata, "!%u", debug->global_variables[i]);
    }
    sb_append_cstr(&debug->metadata, "}\n");
}

internal void llvm_debug_emit_marker(LlvmFunctionContext* ctx,
                                     u32                  source_line,
                                     string               source_path)
{
    u32 location = llvm_debug_source_location(
        ctx->debug, source_line, source_path, ctx->debug_scope_id);
    if (location != 0) {
        sb_format(ctx->sb, "  ; nerd.dbg !%u\n", location);
    }
}

internal void llvm_debug_block_start(const Hir*      hir,
                                     const HirBlock* block,
                                     u32*            out_line,
                                     string*         out_path);

internal u32 llvm_debug_enter_scope(LlvmFunctionContext* ctx,
                                    const HirFunction*   function,
                                    u32                  scope_index,
                                    u32                  line,
                                    string               source_path);

internal void llvm_debug_emit_step_anchor(LlvmFunctionContext* ctx,
                                          u32                  source_line,
                                          string               source_path)
{
    if (ctx == NULL || ctx->debug == NULL || source_line == 0 ||
        source_path.count == 0) {
        return;
    }

    llvm_debug_emit_marker(ctx, source_line, source_path);
    sb_append_cstr(ctx->sb, "  call void asm sideeffect \"nop\", \"\"()\n");
}

internal void llvm_debug_emit_block_end_anchor(LlvmFunctionContext* ctx,
                                               const HirBlock*      block)
{
    if (block == NULL) {
        return;
    }
    llvm_debug_emit_step_anchor(
        ctx, block->end_source_line, block->end_source_path);
}

internal u32 llvm_debug_for_header_line(LlvmFunctionContext* ctx,
                                        const HirFor*        loop,
                                        const HirExpr*       expr)
{
    if (ctx != NULL && loop != NULL) {
        u32 index_line = llvm_debug_local_line(
            ctx->lexer, ctx->sema, loop->index_local_index);
        if (index_line != 0) {
            return index_line;
        }
        u32 item_line = llvm_debug_local_line(
            ctx->lexer, ctx->sema, loop->item_local_index);
        if (item_line != 0) {
            return item_line;
        }
        if (loop->condition_expr_index < array_count(ctx->hir->exprs)) {
            const HirExpr* condition =
                &ctx->hir->exprs[loop->condition_expr_index];
            if (condition->source_line != 0) {
                return condition->source_line;
            }
        }
        if (loop->iterable_expr_index < array_count(ctx->hir->exprs)) {
            const HirExpr* iterable =
                &ctx->hir->exprs[loop->iterable_expr_index];
            if (iterable->source_line != 0) {
                return iterable->source_line;
            }
        }
    }
    return expr != NULL ? expr->source_line : 0;
}

internal bool llvm_debug_line_is_marker(string line, u32* out_location)
{
    cstr  prefix     = "  ; nerd.dbg !";
    usize prefix_len = strlen(prefix);
    if (line.count < prefix_len || memcmp(line.data, prefix, prefix_len) != 0) {
        return false;
    }

    u32 value = 0;
    for (usize i = prefix_len; i < line.count; ++i) {
        u8 ch = line.data[i];
        if (ch == '\r' || ch == '\n') {
            break;
        }
        if (ch < '0' || ch > '9') {
            return false;
        }
        value = value * 10 + (u32)(ch - '0');
    }
    *out_location = value;
    return true;
}

internal bool llvm_debug_line_contains_dbg(string line)
{
    cstr  needle     = "!dbg";
    usize needle_len = strlen(needle);
    if (line.count < needle_len) {
        return false;
    }
    for (usize i = 0; i + needle_len <= line.count; ++i) {
        if (memcmp(line.data + i, needle, needle_len) == 0) {
            return true;
        }
    }
    return false;
}

internal bool llvm_debug_line_is_annotatable(string line)
{
    if (line.count < 3 || line.data[0] != ' ' || line.data[1] != ' ') {
        return false;
    }
    if (line.data[2] == ';' || line.data[2] == '\n' || line.data[2] == '\r') {
        return false;
    }
    if (llvm_debug_line_contains_dbg(line)) {
        return false;
    }
    return true;
}

internal string llvm_debug_annotate_body(Arena*           arena,
                                         LlvmDebugModule* debug,
                                         string           body)
{
    if (debug == NULL) {
        return body;
    }

    StringBuilder out = {0};
    sb_init(&out, arena);
    u32   current_location = 0;
    usize cursor           = 0;
    while (cursor < body.count) {
        usize start = cursor;
        while (cursor < body.count && body.data[cursor] != '\n') {
            ++cursor;
        }
        if (cursor < body.count) {
            ++cursor;
        }

        string line            = string_from(body.data + start, cursor - start);
        u32    marker_location = 0;
        if (llvm_debug_line_is_marker(line, &marker_location)) {
            current_location = marker_location;
            continue;
        }

        if (current_location != 0 && llvm_debug_line_is_annotatable(line)) {
            usize line_end    = line.count;
            bool  has_newline = line_end > 0 && line.data[line_end - 1] == '\n';
            if (has_newline) {
                --line_end;
                if (line_end > 0 && line.data[line_end - 1] == '\r') {
                    --line_end;
                }
            }
            sb_append_string(&out, string_from(line.data, line_end));
            sb_format(&out, ", !dbg !%u", current_location);
            if (has_newline) {
                sb_append_char(&out, '\n');
            }
        } else {
            sb_append_string(&out, line);
        }
    }
    return sb_to_string(&out);
}

internal u32 llvm_data_field_item_type(LlvmFunctionContext* ctx,
                                       const HirExpr*       field_expr)
{
    if (ctx == NULL || field_expr == NULL ||
        field_expr->kind != HIR_EXPR_Field ||
        field_expr->symbol_handle == U32_MAX ||
        !string_eq_cstr(lex_symbol(ctx->lexer, field_expr->symbol_handle),
                        "data") ||
        field_expr->operand_expr_index >= array_count(ctx->hir->exprs)) {
        return sema_no_type();
    }

    u32 source_type =
        ctx->hir->exprs[field_expr->operand_expr_index].type_index;
    if (llvm_type_kind(ctx->sema, source_type) == STK_Pointer) {
        source_type = llvm_pointee_type(ctx->sema, source_type);
    }
    return llvm_collection_item_type(ctx->sema, source_type);
}

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

internal bool llvm_control_target_emitted_break(LlvmFunctionContext* ctx,
                                                u32 symbol_handle)
{
    LlvmControlTarget* target = llvm_find_control_target(ctx, symbol_handle);
    return target != NULL && target->emitted_break;
}

internal string llvm_type_string(LlvmFunctionContext* ctx, u32 type_index)
{
    StringBuilder sb = {0};
    sb_init(&sb, ctx->arena);
    llvm_append_type(&sb, ctx->sema, type_index);
    return sb_to_string(&sb);
}

internal void
llvm_emit_alloca(LlvmFunctionContext* ctx, string ptr, string type)
{
    StringBuilder* target = ctx->entry_sb != NULL ? ctx->entry_sb : ctx->sb;
    sb_format(target,
              "  " STRINGP " = alloca " STRINGP "\n",
              STRINGV(ptr),
              STRINGV(type));
}

internal string llvm_emit_string_value_pointer(LlvmFunctionContext* ctx,
                                               string               value)
{
    string string_type = llvm_string_type(ctx->layout);
    string ptr         = llvm_temp(ctx);
    llvm_emit_alloca(ctx, ptr, string_type);
    sb_format(ctx->sb,
              "  store " STRINGP " " STRINGP ", ptr " STRINGP "\n",
              STRINGV(string_type),
              STRINGV(value),
              STRINGV(ptr));
    return ptr;
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
        return string_format(arena, "%%%.*s", (int)name.count, name.data);
    }
    return (string){0};
}

internal string llvm_param_value_for_symbol(const HirFunction* function,
                                            const Hir*         hir,
                                            const Lexer*       lexer,
                                            Arena*             arena,
                                            u32                symbol_handle)
{
    if (symbol_handle == U32_MAX) {
        return (string){0};
    }
    for (u32 i = 0; i < function->param_count; ++i) {
        const HirParam* param = &hir->params[function->first_param + i];
        if (param->symbol_handle != symbol_handle) {
            continue;
        }
        string name = lex_symbol(lexer, param->symbol_handle);
        if (string_eq_cstr(name, "_")) {
            return string_format(arena, "%%_.%u", i);
        }
        return string_format(arena, "%%%.*s", (int)name.count, name.data);
    }
    return (string){0};
}

internal u32 llvm_param_type(const HirFunction* function,
                             const Hir*         hir,
                             u32                local_index)
{
    for (u32 i = 0; i < function->param_count; ++i) {
        const HirParam* param = &hir->params[function->first_param + i];
        if (param->local_index == local_index) {
            return param->type_index;
        }
    }
    return sema_no_type();
}

internal u32 llvm_param_type_for_symbol(const HirFunction* function,
                                        const Hir*         hir,
                                        u32                symbol_handle)
{
    if (symbol_handle == U32_MAX) {
        return sema_no_type();
    }
    for (u32 i = 0; i < function->param_count; ++i) {
        const HirParam* param = &hir->params[function->first_param + i];
        if (param->symbol_handle == symbol_handle) {
            return param->type_index;
        }
    }
    return sema_no_type();
}

internal bool
llvm_find_local_value(LlvmFunctionContext* ctx, u32 local_index, LlvmValue* out)
{
    for (u32 i = 0; i < array_count(ctx->locals); ++i) {
        if (ctx->locals[i].local_index == local_index) {
            *out = ctx->locals[i].value;
            return true;
        }
    }
    return false;
}

internal void
llvm_set_local_value(LlvmFunctionContext* ctx, u32 local_index, LlvmValue value)
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
    llvm_debug_emit_value(ctx, local_index, value);
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

internal void llvm_mark_assigned_local(LlvmFunctionContext* ctx,
                                       u32                  local_index)
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

internal LlvmLocalSlot* llvm_ensure_local_slot_ex(LlvmFunctionContext* ctx,
                                                  u32 local_index,
                                                  u32 type_index,
                                                  u32 arg_index)
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
    llvm_emit_alloca(ctx, ptr, type);
    llvm_debug_emit_declare(ctx, local_index, ptr, type_index, arg_index);
    return &ctx->slots[array_count(ctx->slots) - 1];
}

internal LlvmLocalSlot* llvm_ensure_local_slot(LlvmFunctionContext* ctx,
                                               u32                  local_index,
                                               u32                  type_index)
{
    return llvm_ensure_local_slot_ex(ctx, local_index, type_index, 0);
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

internal bool llvm_type_is_box(const Sema* sema, u32 type_index)
{
    return type_index != sema_no_type() &&
           llvm_type_kind(sema, type_index) == STK_Box;
}

internal bool llvm_emit_box_free_slot(LlvmFunctionContext* ctx,
                                      LlvmLocalSlot*       slot)
{
    if (slot == NULL || !llvm_type_is_box(ctx->sema, slot->type_index)) {
        return true;
    }

    string pointer = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = load ptr, ptr " STRINGP "\n",
              STRINGV(pointer),
              STRINGV(slot->ptr));
    string is_null = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = icmp eq ptr " STRINGP ", null\n",
              STRINGV(is_null),
              STRINGV(pointer));
    string free_label = llvm_label(ctx, "box.cleanup");
    string done_label = llvm_label(ctx, "box.cleanup.done");
    sb_format(ctx->sb,
              "  br i1 " STRINGP ", label %%" STRINGP ", label %%" STRINGP "\n",
              STRINGV(is_null),
              STRINGV(done_label),
              STRINGV(free_label));

    sb_format(ctx->sb, STRINGP ":\n", STRINGV(free_label));
    sb_format(ctx->sb,
              "  call void @nrt_mem_free(ptr " STRINGP ")\n"
              "  store ptr null, ptr " STRINGP "\n"
              "  br label %%" STRINGP "\n",
              STRINGV(pointer),
              STRINGV(slot->ptr),
              STRINGV(done_label));

    sb_format(ctx->sb, STRINGP ":\n", STRINGV(done_label));
    ctx->block_terminated = false;
    return true;
}

internal bool llvm_emit_box_cleanup_for_scope(LlvmFunctionContext* ctx,
                                              u32                  scope_index)
{
    if (scope_index == U32_MAX || ctx->sema == NULL ||
        scope_index >= array_count(ctx->sema->scopes)) {
        return true;
    }

    const SemaScope* scope = &ctx->sema->scopes[scope_index];
    for (u32 i = 0; i < scope->local_count; ++i) {
        u32 local_index = scope->first_local + i;
        if (local_index >= array_count(ctx->sema->locals)) {
            continue;
        }
        if (!llvm_type_is_box(ctx->sema,
                              ctx->sema->locals[local_index].type_index)) {
            continue;
        }
        LlvmLocalSlot* slot = llvm_find_local_slot(ctx, local_index);
        if (!llvm_emit_box_free_slot(ctx, slot)) {
            return false;
        }
    }
    return true;
}

internal bool llvm_emit_box_cleanup_all(LlvmFunctionContext* ctx)
{
    for (u32 i = 0; i < array_count(ctx->slots); ++i) {
        if (!llvm_emit_box_free_slot(ctx, &ctx->slots[i])) {
            return false;
        }
    }
    return true;
}

internal u32 llvm_box_move_source_local(LlvmFunctionContext* ctx,
                                        u32                  expr_index)
{
    if (expr_index >= array_count(ctx->hir->exprs)) {
        return U32_MAX;
    }

    const HirExpr* expr = &ctx->hir->exprs[expr_index];
    if (expr->kind != HIR_EXPR_LocalRef || expr->ref_kind != HIR_REF_Local ||
        !llvm_type_is_box(ctx->sema, expr->type_index)) {
        return U32_MAX;
    }
    return expr->ref_index;
}

internal bool llvm_expr_is_box_global_binding(LlvmFunctionContext* ctx,
                                              u32                  expr_index,
                                              u32* out_binding_index)
{
    if (expr_index >= array_count(ctx->hir->exprs)) {
        return false;
    }

    const HirExpr* expr = &ctx->hir->exprs[expr_index];
    if (expr->kind != HIR_EXPR_LocalRef || expr->ref_kind != HIR_REF_Binding ||
        !llvm_type_is_box(ctx->sema, expr->type_index) ||
        expr->ref_index >= array_count(ctx->hir->bindings)) {
        return false;
    }

    const HirBinding* binding = &ctx->hir->bindings[expr->ref_index];
    if (binding->kind != HIR_BINDING_Value ||
        binding->target_index >= array_count(ctx->hir->values) ||
        ctx->hir->values[binding->target_index].kind != HIR_VALUE_Global) {
        return false;
    }

    *out_binding_index = expr->ref_index;
    return true;
}

internal bool llvm_emit_nil_box_local(LlvmFunctionContext* ctx, u32 local_index)
{
    if (local_index == U32_MAX || ctx->sema == NULL ||
        local_index >= array_count(ctx->sema->locals) ||
        !llvm_type_is_box(ctx->sema,
                          ctx->sema->locals[local_index].type_index)) {
        return true;
    }

    LlvmLocalSlot* slot = llvm_ensure_local_slot(
        ctx, local_index, ctx->sema->locals[local_index].type_index);
    sb_format(
        ctx->sb, "  store ptr null, ptr " STRINGP "\n", STRINGV(slot->ptr));
    return true;
}

internal bool llvm_emit_nil_box_binding(LlvmFunctionContext* ctx,
                                        u32                  binding_index)
{
    if (binding_index >= array_count(ctx->hir->bindings)) {
        return true;
    }

    const HirBinding* binding = &ctx->hir->bindings[binding_index];
    if (binding->kind != HIR_BINDING_Value ||
        binding->target_index >= array_count(ctx->hir->values)) {
        return true;
    }

    const HirValue* value = &ctx->hir->values[binding->target_index];
    if (value->kind != HIR_VALUE_Global ||
        !llvm_type_is_box(ctx->sema, value->type_index)) {
        return true;
    }

    string name = llvm_value_name_string(
        ctx->hir, ctx->lexer, ctx->arena, binding->target_index);
    if (name.count == 0) {
        return false;
    }
    sb_format(ctx->sb, "  store ptr null, ptr " STRINGP "\n", STRINGV(name));
    return true;
}

internal bool llvm_consume_box_expr(LlvmFunctionContext* ctx,
                                    u32                  expr_index,
                                    u32                  consumed_type,
                                    u32                  excluded_local)
{
    if (!llvm_type_is_box(ctx->sema, consumed_type)) {
        return true;
    }

    u32 source_local = llvm_box_move_source_local(ctx, expr_index);
    if (source_local == U32_MAX || source_local == excluded_local) {
        u32 binding_index = U32_MAX;
        if (llvm_expr_is_box_global_binding(ctx, expr_index, &binding_index)) {
            return llvm_emit_nil_box_binding(ctx, binding_index);
        }
        return true;
    }

    return llvm_emit_nil_box_local(ctx, source_local);
}

internal LlvmValue llvm_default_value(LlvmFunctionContext* ctx, u32 type_index)
{
    SemaTypeKind kind = llvm_type_kind(ctx->sema, type_index);
    if (kind == STK_Pointer || kind == STK_Function ||
        kind == STK_DynamicArray || kind == STK_Box || kind == STK_Nil) {
        return (LlvmValue){
            .ok         = true,
            .type_index = type_index,
            .value      = s("null"),
        };
    }
    if (kind == STK_Enum || kind == STK_Tuple || kind == STK_Plex ||
        kind == STK_Array || kind == STK_Slice || kind == STK_String ||
        kind == STK_Arena) {
        return (LlvmValue){
            .ok         = true,
            .type_index = type_index,
            .value      = s("zeroinitializer"),
        };
    }
    string type = llvm_type_string(ctx, type_index);
    if (string_eq_cstr(type, "ptr")) {
        return (LlvmValue){
            .ok         = true,
            .type_index = type_index,
            .value      = s("null"),
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
    if ((target_kind == STK_Pointer || target_kind == STK_DynamicArray ||
         target_kind == STK_Box) &&
        (source_kind == STK_Nil || source_kind == STK_UntypedInteger) &&
        string_eq_cstr(value.value, "0")) {
        value.type_index = target_type;
        value.value      = s("null");
        return value;
    }

    if ((target_kind == STK_Pointer || target_kind == STK_DynamicArray ||
         target_kind == STK_Box) &&
        llvm_integer_bits(ctx->sema, value.type_index) > 0) {
        string source_type        = llvm_type_string(ctx, value.type_index);
        string target_type_string = llvm_type_string(ctx, target_type);
        string temp               = llvm_temp(ctx);
        sb_format(ctx->sb,
                  "  " STRINGP " = inttoptr " STRINGP " " STRINGP " to " STRINGP
                  "\n",
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

    if (target_kind == STK_Bool && source_kind == STK_Box) {
        string temp = llvm_temp(ctx);
        sb_format(ctx->sb,
                  "  " STRINGP " = icmp ne ptr " STRINGP ", null\n",
                  STRINGV(temp),
                  STRINGV(value.value));
        return (LlvmValue){
            .ok         = true,
            .type_index = target_type,
            .value      = temp,
        };
    }

    if (target_kind == STK_Pointer && source_kind == STK_Box &&
        ctx->sema->types[target_type].first_param_type ==
            ctx->sema->types[value.type_index].first_param_type) {
        value.type_index = target_type;
        return value;
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
                  "  " STRINGP " = bitcast " STRINGP " " STRINGP " to " STRINGP
                  "\n",
                  STRINGV(temp),
                  STRINGV(value_type),
                  STRINGV(value.value),
                  STRINGV(storage_type));
    } else if ((llvm_type_kind(ctx->sema, value.type_index) == STK_Pointer ||
                llvm_type_kind(ctx->sema, value.type_index) == STK_Function) &&
               value_bits == storage_bits) {
        sb_format(ctx->sb,
                  "  " STRINGP " = ptrtoint " STRINGP " " STRINGP " to " STRINGP
                  "\n",
                  STRINGV(temp),
                  STRINGV(value_type),
                  STRINGV(value.value),
                  STRINGV(storage_type));
    } else if (value_bits < storage_bits &&
               llvm_integer_bits(ctx->sema, value.type_index) > 0) {
        sb_format(ctx->sb,
                  "  " STRINGP " = zext " STRINGP " " STRINGP " to " STRINGP
                  "\n",
                  STRINGV(temp),
                  STRINGV(value_type),
                  STRINGV(value.value),
                  STRINGV(storage_type));
    } else if (value_bits > storage_bits &&
               llvm_integer_bits(ctx->sema, value.type_index) > 0) {
        sb_format(ctx->sb,
                  "  " STRINGP " = trunc " STRINGP " " STRINGP " to " STRINGP
                  "\n",
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
                  "  " STRINGP " = bitcast " STRINGP " " STRINGP " to " STRINGP
                  "\n",
                  STRINGV(temp),
                  STRINGV(storage_type),
                  STRINGV(value.value),
                  STRINGV(field_type_s));
    } else if ((llvm_type_kind(ctx->sema, field_type) == STK_Pointer ||
                llvm_type_kind(ctx->sema, field_type) == STK_Function) &&
               field_bits == storage_bits) {
        sb_format(ctx->sb,
                  "  " STRINGP " = inttoptr " STRINGP " " STRINGP " to " STRINGP
                  "\n",
                  STRINGV(temp),
                  STRINGV(storage_type),
                  STRINGV(value.value),
                  STRINGV(field_type_s));
    } else if (llvm_integer_bits(ctx->sema, field_type) > 0 &&
               field_bits < storage_bits) {
        sb_format(ctx->sb,
                  "  " STRINGP " = trunc " STRINGP " " STRINGP " to " STRINGP
                  "\n",
                  STRINGV(temp),
                  STRINGV(storage_type),
                  STRINGV(value.value),
                  STRINGV(field_type_s));
    } else if (llvm_integer_bits(ctx->sema, field_type) > 0 &&
               field_bits > storage_bits) {
        sb_format(ctx->sb,
                  "  " STRINGP " = zext " STRINGP " " STRINGP " to " STRINGP
                  "\n",
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
                  "  " STRINGP " = bitcast " STRINGP " " STRINGP " to " STRINGP
                  "\n",
                  STRINGV(temp),
                  STRINGV(value_type),
                  STRINGV(value.value),
                  STRINGV(storage_type));
    } else if ((llvm_type_kind(ctx->sema, value.type_index) == STK_Pointer ||
                llvm_type_kind(ctx->sema, value.type_index) == STK_Function) &&
               value_bits == storage_bits) {
        sb_format(ctx->sb,
                  "  " STRINGP " = ptrtoint " STRINGP " " STRINGP " to " STRINGP
                  "\n",
                  STRINGV(temp),
                  STRINGV(value_type),
                  STRINGV(value.value),
                  STRINGV(storage_type));
    } else if (llvm_integer_bits(ctx->sema, value.type_index) > 0 &&
               value_bits < storage_bits) {
        sb_format(ctx->sb,
                  "  " STRINGP " = zext " STRINGP " " STRINGP " to " STRINGP
                  "\n",
                  STRINGV(temp),
                  STRINGV(value_type),
                  STRINGV(value.value),
                  STRINGV(storage_type));
    } else if (llvm_integer_bits(ctx->sema, value.type_index) > 0 &&
               value_bits > storage_bits) {
        sb_format(ctx->sb,
                  "  " STRINGP " = trunc " STRINGP " " STRINGP " to " STRINGP
                  "\n",
                  STRINGV(temp),
                  STRINGV(value_type),
                  STRINGV(value.value),
                  STRINGV(storage_type));
    } else if (value_bits > 0 && value_bits <= storage_bits) {
        string slot = llvm_temp(ctx);
        llvm_emit_alloca(ctx, slot, storage_type);
        sb_format(ctx->sb,
                  "  store " STRINGP " 0, ptr " STRINGP "\n"
                  "  store " STRINGP " " STRINGP ", ptr " STRINGP "\n"
                  "  " STRINGP " = load " STRINGP ", ptr " STRINGP "\n",
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
                                               u32 storage_bits,
                                               u32 result_type)
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

    string storage_type  = string_format(ctx->arena, "i%u", storage_bits);
    string result_type_s = llvm_type_string(ctx, result_type);
    string temp          = llvm_temp(ctx);
    if (llvm_float_bits(ctx->sema, result_type) == storage_bits) {
        sb_format(ctx->sb,
                  "  " STRINGP " = bitcast " STRINGP " " STRINGP " to " STRINGP
                  "\n",
                  STRINGV(temp),
                  STRINGV(storage_type),
                  STRINGV(value.value),
                  STRINGV(result_type_s));
    } else if ((llvm_type_kind(ctx->sema, result_type) == STK_Pointer ||
                llvm_type_kind(ctx->sema, result_type) == STK_Function) &&
               result_bits == storage_bits) {
        sb_format(ctx->sb,
                  "  " STRINGP " = inttoptr " STRINGP " " STRINGP " to " STRINGP
                  "\n",
                  STRINGV(temp),
                  STRINGV(storage_type),
                  STRINGV(value.value),
                  STRINGV(result_type_s));
    } else if (llvm_integer_bits(ctx->sema, result_type) > 0 &&
               result_bits < storage_bits) {
        sb_format(ctx->sb,
                  "  " STRINGP " = trunc " STRINGP " " STRINGP " to " STRINGP
                  "\n",
                  STRINGV(temp),
                  STRINGV(storage_type),
                  STRINGV(value.value),
                  STRINGV(result_type_s));
    } else if (llvm_integer_bits(ctx->sema, result_type) > 0 &&
               result_bits > storage_bits) {
        sb_format(ctx->sb,
                  "  " STRINGP " = zext " STRINGP " " STRINGP " to " STRINGP
                  "\n",
                  STRINGV(temp),
                  STRINGV(storage_type),
                  STRINGV(value.value),
                  STRINGV(result_type_s));
    } else if (result_bits <= storage_bits) {
        string slot = llvm_temp(ctx);
        llvm_emit_alloca(ctx, slot, storage_type);
        sb_format(ctx->sb,
                  "  store " STRINGP " " STRINGP ", ptr " STRINGP "\n"
                  "  " STRINGP " = load " STRINGP ", ptr " STRINGP "\n",
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
    string current = value_count == 0 ? s("zeroinitializer") : s("poison");
    for (u32 i = 0; i < value_count; ++i) {
        string temp       = llvm_temp(ctx);
        string value_type = llvm_type_string(ctx, values[i].type_index);
        sb_format(ctx->sb,
                  "  " STRINGP " = insertvalue " STRINGP " " STRINGP
                  ", " STRINGP " " STRINGP ", %u\n",
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
internal bool      llvm_emit_effect_stmt(LlvmFunctionContext* ctx,
                                         const HirFunction*   function,
                                         const HirStmt*       stmt);
internal bool      llvm_emit_return(LlvmFunctionContext* ctx,
                                    const HirFunction*   function,
                                    const HirStmt*       stmt);

internal LlvmValue llvm_address_of_expr(LlvmFunctionContext* ctx,
                                        const HirFunction*   function,
                                        u32                  expr_index);

internal LlvmValue
llvm_emit_imported_constant_value(LlvmFunctionContext* ctx,
                                  const HirFunction*   function,
                                  const HirImport*     import,
                                  u32                  result_type)
{
    const HirImport* current_import = import;
    HirImport        next_import    = {0};
    for (u32 depth = 0; depth < 16; ++depth) {
        const Hir*   source_hir         = NULL;
        const Lexer* source_lexer       = NULL;
        const Sema*  source_sema        = NULL;
        u32          source_value_index = U32_MAX;
        if (current_import == NULL ||
            !llvm_import_source_value(ctx->sema,
                                      current_import,
                                      &source_hir,
                                      &source_lexer,
                                      &source_sema,
                                      &source_value_index)) {
            return (LlvmValue){0};
        }

        const HirValue* source_value = &source_hir->values[source_value_index];
        if (source_value->kind != HIR_VALUE_Constant) {
            return (LlvmValue){0};
        }

        if (source_value->value_expr_index != U32_MAX) {
            LlvmFunctionContext source_ctx = *ctx;
            source_ctx.hir                 = source_hir;
            source_ctx.lexer               = source_lexer;
            source_ctx.sema                = source_sema;
            LlvmValue result               = llvm_emit_expr(
                &source_ctx, function, source_value->value_expr_index);
            ctx->next_temp  = source_ctx.next_temp;
            ctx->next_label = source_ctx.next_label;
            if (result.ok && result_type != sema_no_type()) {
                result.type_index = result_type;
            }
            return result;
        }

        if (source_value->decl_index >= array_count(source_sema->decls)) {
            return (LlvmValue){0};
        }
        const SemaDecl* source_decl =
            &source_sema->decls[source_value->decl_index];
        if (source_decl->import_module_index == sema_no_decl()) {
            return (LlvmValue){0};
        }

        next_import = (HirImport){
            .module_index  = source_decl->import_module_index,
            .decl_index    = source_decl->import_decl_index,
            .symbol_handle = source_decl->symbol_handle,
            .type_index    = source_decl->type_index,
        };
        current_import = &next_import;
    }
    return (LlvmValue){0};
}

internal string llvm_dynamic_array_load_header_field(LlvmFunctionContext* ctx,
                                                     string               data,
                                                     u32    field_index,
                                                     string type);

internal u64 llvm_type_storage_bytes(const Sema* sema, u32 type_index);

internal string llvm_dynamic_array_header_field_ptr(LlvmFunctionContext* ctx,
                                                    string               header,
                                                    u32 field_index);

internal string llvm_dynamic_array_data_from_header(LlvmFunctionContext* ctx,
                                                    string header);

internal string llvm_dynamic_array_header_from_data(LlvmFunctionContext* ctx,
                                                    string               data);

internal LlvmValue llvm_emit_dynamic_array_field(LlvmFunctionContext* ctx,
                                                 LlvmValue            target,
                                                 const HirExpr*       expr);

internal bool llvm_dynamic_array_callee_method(LlvmFunctionContext* ctx,
                                               u32     callee_expr_index,
                                               u32*    receiver_expr_index,
                                               string* method);

internal bool llvm_box_callee_method(LlvmFunctionContext* ctx,
                                     u32                  callee_expr_index,
                                     u32*                 receiver_expr_index,
                                     string*              method);

internal LlvmValue llvm_emit_box_free(LlvmFunctionContext* ctx,
                                      const HirFunction*   function,
                                      const HirExpr*       call,
                                      u32                  receiver_expr_index);

internal LlvmValue llvm_emit_dynamic_array_push(LlvmFunctionContext* ctx,
                                                const HirFunction*   function,
                                                const HirExpr*       call,
                                                u32 receiver_expr_index);

internal LlvmValue llvm_emit_dynamic_array_reserve(LlvmFunctionContext* ctx,
                                                   const HirFunction* function,
                                                   const HirExpr*     call,
                                                   u32  receiver_expr_index,
                                                   bool relative);

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
                                                  u32  receiver_expr_index,
                                                  bool swap);

internal LlvmValue llvm_emit_dynamic_array_append(LlvmFunctionContext* ctx,
                                                  const HirFunction*   function,
                                                  const HirExpr*       call,
                                                  u32 receiver_expr_index);

internal LlvmValue llvm_emit_dynamic_array_resize(LlvmFunctionContext* ctx,
                                                  const HirFunction*   function,
                                                  const HirExpr*       call,
                                                  u32  receiver_expr_index,
                                                  bool initialize,
                                                  bool relative);

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

internal bool
llvm_expr_integer_constant(const Hir* hir, u32 expr_index, i64* out)
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

internal bool llvm_ref_value_index(const Hir* hir,
                                   HirRefKind ref_kind,
                                   u32        ref_index,
                                   u32*       out_value_index)
{
    if (hir == NULL) {
        return false;
    }

    u32 binding_index = U32_MAX;
    if (ref_kind == HIR_REF_Binding) {
        binding_index = ref_index;
    } else if (ref_kind == HIR_REF_Decl &&
               ref_index < array_count(hir->decl_binding_indices)) {
        binding_index = hir->decl_binding_indices[ref_index];
    } else {
        return false;
    }

    if (binding_index >= array_count(hir->bindings)) {
        return false;
    }

    const HirBinding* binding = &hir->bindings[binding_index];
    if (binding->kind != HIR_BINDING_Value ||
        binding->target_index >= array_count(hir->values)) {
        return false;
    }

    *out_value_index = binding->target_index;
    return true;
}

internal bool llvm_expr_binding_value_index(const Hir* hir,
                                            u32        expr_index,
                                            u32*       out_value_index)
{
    if (hir == NULL || expr_index >= array_count(hir->exprs)) {
        return false;
    }

    const HirExpr* expr = &hir->exprs[expr_index];
    if (expr->kind != HIR_EXPR_LocalRef) {
        return false;
    }

    return llvm_ref_value_index(
        hir, expr->ref_kind, expr->ref_index, out_value_index);
}

internal bool llvm_global_value_is_slice_literal(const Hir*  hir,
                                                 const Sema* sema,
                                                 u32         value_index)
{
    if (hir == NULL || value_index >= array_count(hir->values)) {
        return false;
    }

    const HirValue* value = &hir->values[value_index];
    if (value->kind != HIR_VALUE_Global ||
        value->value_expr_index >= array_count(hir->exprs)) {
        return false;
    }

    const HirExpr* expr = &hir->exprs[value->value_expr_index];
    return expr->kind == HIR_EXPR_Array &&
           llvm_type_kind(sema, expr->type_index) == STK_Slice;
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
    string instr =
        bits > 64
            ? s("trunc")
            : ((llvm_type_kind(ctx->sema, value.type_index) == STK_Bool ||
                llvm_type_is_unsigned_integer(ctx->sema, value.type_index))
                   ? s("zext")
                   : s("sext"));
    string temp = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = " STRINGP " " STRINGP " " STRINGP " to i64\n",
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
        return string_format(
            ctx->arena, "%lld", (long long)(end_value - start_value));
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
        LlvmValue value       = llvm_emit_expr(ctx, function, arg->expr_index);
        if (!value.ok) {
            array_free(args);
            return (LlvmValue){0};
        }
        array_push(args, value);
    }

    u32 enum_type = expr->type_index;
    u32 storage_payload_bits =
        llvm_enum_storage_payload_bits(ctx->sema, enum_type);
    string enum_type_string = llvm_type_string(ctx, enum_type);
    string payload_type_string =
        string_format(ctx->arena, "i%u", storage_payload_bits);
    string payload_value = s("0");

    u32 variant_payload_type =
        llvm_enum_variant_payload_type(ctx->sema, enum_type, variant_index);
    if (variant_payload_type != sema_no_type() && expr->arg_count > 0) {
        LlvmValue payload = {0};
        if (expr->arg_count == 1 &&
            args[0].type_index == variant_payload_type) {
            payload = args[0];
        } else if (llvm_type_is_record(ctx->sema, variant_payload_type)) {
            string variant_payload_type_string =
                llvm_type_string(ctx, variant_payload_type);
            string aggregate = s("poison");
            for (u32 i = 0; i < expr->arg_count; ++i) {
                LlvmValue arg      = args[i];
                string    temp     = llvm_temp(ctx);
                string    arg_type = llvm_type_string(ctx, arg.type_index);
                sb_format(ctx->sb,
                          "  " STRINGP " = insertvalue " STRINGP " " STRINGP
                          ", " STRINGP " " STRINGP ", %u\n",
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

    i64 tag =
        llvm_enum_variant_discriminant(ctx->sema, enum_type, variant_index);
    string with_tag = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = insertvalue " STRINGP " poison, i64 %lld, 0\n",
              STRINGV(with_tag),
              STRINGV(enum_type_string),
              (long long)tag);

    string with_payload = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = insertvalue " STRINGP " " STRINGP ", " STRINGP
              " " STRINGP ", 1\n",
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
        if (stmt->kind == HIR_STMT_Return) {
            bool ok = llvm_emit_return(ctx, function, stmt);
            return (LlvmValue){
                .ok         = ok,
                .type_index = ok ? llvm_builtin_type(ctx->sema, STK_Void)
                                 : sema_no_type(),
                .value      = s(""),
            };
        }
        if (!llvm_emit_effect_stmt(ctx, function, stmt)) {
            return (LlvmValue){0};
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

        LlvmValue local_value  = value;
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
                  "  " STRINGP " = extractvalue " STRINGP " " STRINGP ", 0\n",
                  STRINGV(lhs_tag),
                  STRINGV(enum_type),
                  STRINGV(scrutinee.value));
        sb_format(ctx->sb,
                  "  " STRINGP " = extractvalue " STRINGP " " STRINGP ", 0\n",
                  STRINGV(rhs_tag),
                  STRINGV(enum_type),
                  STRINGV(rhs.value));
        string temp = llvm_temp(ctx);
        sb_format(ctx->sb,
                  "  " STRINGP " = icmp " STRINGP " i64 " STRINGP ", " STRINGP
                  "\n",
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
        string lhs_ptr = llvm_emit_string_value_pointer(ctx, scrutinee.value);
        string rhs_ptr = llvm_emit_string_value_pointer(ctx, rhs.value);
        string equal   = llvm_temp(ctx);
        sb_format(ctx->sb,
                  "  " STRINGP " = call i1 @string_eq(ptr " STRINGP
                  ", ptr " STRINGP ")\n",
                  STRINGV(equal),
                  STRINGV(lhs_ptr),
                  STRINGV(rhs_ptr));
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
        .type_index = llvm_builtin_type(ctx->sema, STK_Bool),
        .value      = temp,
    };
}

internal LlvmValue llvm_emit_pattern_condition(LlvmFunctionContext* ctx,
                                               const HirFunction*   function,
                                               LlvmValue            scrutinee,
                                               u32 pattern_index)
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
        {
            LlvmValue bound = scrutinee;
            if (pattern->symbol_handle != U32_MAX &&
                llvm_type_is_record(ctx->sema, scrutinee.type_index)) {
                u32 field_index = llvm_record_field_index(
                    ctx->sema, scrutinee.type_index, pattern->symbol_handle);
                if (field_index != U32_MAX) {
                    string temp = llvm_temp(ctx);
                    string record_type =
                        llvm_type_string(ctx, scrutinee.type_index);
                    sb_format(ctx->sb,
                              "  " STRINGP " = extractvalue " STRINGP
                              " " STRINGP ", %u\n",
                              STRINGV(temp),
                              STRINGV(record_type),
                              STRINGV(scrutinee.value),
                              field_index);
                    bound = (LlvmValue){
                        .ok         = true,
                        .type_index = llvm_record_field_type(
                            ctx->sema, scrutinee.type_index, field_index),
                        .value = temp,
                    };
                }
            }
            llvm_bind_symbol_value(ctx, pattern->symbol_handle, bound);
        }
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

                string temp = llvm_temp(ctx);
                string record_type =
                    llvm_type_string(ctx, scrutinee.type_index);
                sb_format(ctx->sb,
                          "  " STRINGP " = extractvalue " STRINGP " " STRINGP
                          ", %u\n",
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
                          "  " STRINGP " = and i1 " STRINGP ", " STRINGP "\n",
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

            string tag_matches  = llvm_temp(ctx);
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
            u32 variant_payload_type = llvm_enum_variant_payload_type(
                ctx->sema, scrutinee.type_index, variant_index);
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
            LlvmValue variant_payload =
                llvm_cast_from_storage_bits(ctx,
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

                u32    child_type  = variant_payload_type;
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
                              "  " STRINGP " = extractvalue " STRINGP
                              " " STRINGP ", %u\n",
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
                            for (u32 field_index = 0; field_index < field_count;
                                 ++field_index) {
                                u32 field_type =
                                    llvm_record_field_type(ctx->sema,
                                                           variant_payload_type,
                                                           field_index);
                                if (field_type != pattern_value_type) {
                                    continue;
                                }

                                string variant_payload_type_string =
                                    llvm_type_string(ctx, variant_payload_type);
                                child_value = llvm_temp(ctx);
                                sb_format(ctx->sb,
                                          "  " STRINGP
                                          " = extractvalue " STRINGP " " STRINGP
                                          ", %u\n",
                                          STRINGV(child_value),
                                          STRINGV(variant_payload_type_string),
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
                                                    .ok         = true,
                                                    .type_index = child_type,
                                                    .value      = child_value,
                                                },
                                                child->pattern_index);
                if (!child_condition.ok) {
                    return (LlvmValue){0};
                }

                string and_temp = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = and i1 " STRINGP ", " STRINGP "\n",
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

internal LlvmValue
llvm_emit_branch_pattern_condition(LlvmFunctionContext* ctx,
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
    if (expr->kind == HIR_EXPR_Unary && expr->unary_op == HIR_UNARY_Deref) {
        LlvmValue pointer =
            llvm_emit_expr(ctx, function, expr->operand_expr_index);
        if (!pointer.ok) {
            return (LlvmValue){0};
        }
        pointer.type_index = sema_no_type();
        return pointer;
    }

    if ((expr->kind == HIR_EXPR_Field || expr->kind == HIR_EXPR_TupleField) &&
        expr->operand_expr_index < array_count(ctx->hir->exprs)) {
        const HirExpr* target_expr = &ctx->hir->exprs[expr->operand_expr_index];
        u32            target_type = target_expr->type_index;
        u32            record_type = target_type;
        bool           target_is_pointer =
            llvm_type_kind(ctx->sema, target_type) == STK_Pointer ||
            llvm_type_kind(ctx->sema, target_type) == STK_Box;
        if (target_is_pointer) {
            record_type = llvm_type_kind(ctx->sema, target_type) == STK_Box
                              ? ctx->sema->types[target_type].first_param_type
                              : llvm_pointee_type(ctx->sema, target_type);
        }

        u32 field_index = U32_MAX;
        if (expr->kind == HIR_EXPR_TupleField) {
            field_index = (u32)expr->integer;
        } else if (llvm_type_kind(ctx->sema, record_type) == STK_String ||
                   llvm_type_kind(ctx->sema, record_type) == STK_Slice) {
            string field = lex_symbol(ctx->lexer, expr->symbol_handle);
            if (string_eq_cstr(field, "data")) {
                field_index = 0;
            } else if (string_eq_cstr(field, "count")) {
                field_index = 1;
            }
        } else {
            field_index = llvm_record_field_index(
                ctx->sema, record_type, expr->symbol_handle);
        }
        if (field_index == U32_MAX ||
            llvm_type_kind(ctx->sema, record_type) == STK_Union) {
            return (LlvmValue){0};
        }

        u32 field_type = expr->type_index;
        if (expr->kind == HIR_EXPR_Field &&
            (llvm_type_kind(ctx->sema, record_type) == STK_String ||
             llvm_type_kind(ctx->sema, record_type) == STK_Slice)) {
            u32 record_field_type =
                llvm_record_field_type(ctx->sema, record_type, field_index);
            if (record_field_type != sema_no_type()) {
                field_type = record_field_type;
            }
        } else if (llvm_type_kind(ctx->sema, record_type) == STK_Tuple ||
                   llvm_type_kind(ctx->sema, record_type) == STK_Plex) {
            u32 record_field_type =
                llvm_record_field_type(ctx->sema, record_type, field_index);
            if (record_field_type != sema_no_type()) {
                field_type = record_field_type;
            }
        }

        LlvmValue target_address =
            target_is_pointer
                ? llvm_emit_expr(ctx, function, expr->operand_expr_index)
                : llvm_address_of_expr(ctx, function, expr->operand_expr_index);
        if (!target_address.ok) {
            return (LlvmValue){0};
        }

        string record_type_string = llvm_type_string(ctx, record_type);
        string ptr                = llvm_temp(ctx);
        sb_format(ctx->sb,
                  "  " STRINGP " = getelementptr inbounds " STRINGP
                  ", ptr " STRINGP ", i64 0, i32 %u\n",
                  STRINGV(ptr),
                  STRINGV(record_type_string),
                  STRINGV(target_address.value),
                  field_index);
        return (LlvmValue){
            .ok         = true,
            .type_index = field_type,
            .value      = ptr,
        };
    }

    if (expr->kind == HIR_EXPR_Index) {
        if (expr->operand_expr_index >= array_count(ctx->hir->exprs)) {
            return (LlvmValue){0};
        }

        LlvmValue index = llvm_emit_expr(ctx, function, expr->extra_expr_index);
        if (!index.ok) {
            return (LlvmValue){0};
        }
        if (llvm_integer_bits(ctx->sema, index.type_index) == 0) {
            index = llvm_coerce_value_to_type(
                ctx, index, llvm_builtin_type(ctx->sema, STK_Usize));
            if (!index.ok) {
                return (LlvmValue){0};
            }
        }

        const HirExpr* target_expr = &ctx->hir->exprs[expr->operand_expr_index];
        u32            target_type = target_expr->type_index;
        u32            item_type   = sema_no_type();
        if (llvm_type_kind(ctx->sema, target_type) == STK_Array) {
            LlvmValue target_address =
                llvm_address_of_expr(ctx, function, expr->operand_expr_index);
            if (!target_address.ok) {
                return (LlvmValue){0};
            }

            item_type = llvm_collection_item_type(ctx->sema, target_type);
            string target_type_string = llvm_type_string(ctx, target_type);
            string index_type         = llvm_type_string(ctx, index.type_index);
            string ptr                = llvm_temp(ctx);
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
                .type_index = item_type,
                .value      = ptr,
            };
        }

        if (llvm_type_kind(ctx->sema, target_type) == STK_Slice ||
            llvm_type_kind(ctx->sema, target_type) == STK_String) {
            u32 target_value_index = U32_MAX;
            if (ctx->global_init_value_index != U32_MAX &&
                llvm_expr_binding_value_index(
                    ctx->hir, expr->operand_expr_index, &target_value_index) &&
                target_value_index == ctx->global_init_value_index &&
                llvm_global_value_is_slice_literal(
                    ctx->hir, ctx->sema, target_value_index)) {
                item_type = llvm_collection_item_type(ctx->sema, target_type);
                if (item_type == sema_no_type()) {
                    return (LlvmValue){0};
                }

                string backing = llvm_global_slice_backing_name_string(
                    ctx->hir, ctx->arena, target_value_index);
                string item_type_string = llvm_type_string(ctx, item_type);
                string index_type = llvm_type_string(ctx, index.type_index);
                string ptr        = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = getelementptr inbounds " STRINGP
                          ", ptr " STRINGP ", " STRINGP " " STRINGP "\n",
                          STRINGV(ptr),
                          STRINGV(item_type_string),
                          STRINGV(backing),
                          STRINGV(index_type),
                          STRINGV(index.value));
                return (LlvmValue){
                    .ok         = true,
                    .type_index = item_type,
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
                .type_index = item_type,
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
                .type_index = item_type,
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

    if (expr->kind == HIR_EXPR_LocalRef && expr->ref_kind == HIR_REF_Binding) {
        if (expr->ref_index < array_count(ctx->hir->bindings)) {
            const HirBinding* binding = &ctx->hir->bindings[expr->ref_index];
            if (binding->kind == HIR_BINDING_Value &&
                binding->target_index < array_count(ctx->hir->values)) {
                const HirValue* value =
                    &ctx->hir->values[binding->target_index];
                if (value->kind == HIR_VALUE_Global) {
                    string name = llvm_value_name_string(ctx->hir,
                                                         ctx->lexer,
                                                         ctx->arena,
                                                         binding->target_index);
                    return (LlvmValue){
                        .ok         = name.count > 0,
                        .type_index = value->type_index,
                        .value      = name,
                    };
                }
            }
        }
    }

    if (expr->kind == HIR_EXPR_LocalRef && expr->ref_kind == HIR_REF_Local) {
        LlvmLocalSlot* slot = llvm_find_local_slot(ctx, expr->ref_index);
        if (slot == NULL) {
            slot =
                llvm_ensure_local_slot(ctx, expr->ref_index, expr->type_index);

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

internal string llvm_binary_instruction(const Sema* sema,
                                        u32         type_index,
                                        HirBinaryOp op)
{
    switch (op) {
    case HIR_BINARY_Add:
        return s("add");
    case HIR_BINARY_Subtract:
        return s("sub");
    case HIR_BINARY_Multiply:
        return s("mul");
    case HIR_BINARY_Divide:
        return llvm_type_is_unsigned_integer(sema, type_index) ? s("udiv")
                                                               : s("sdiv");
    case HIR_BINARY_Modulo:
        return llvm_type_is_unsigned_integer(sema, type_index) ? s("urem")
                                                               : s("srem");
    case HIR_BINARY_BitwiseAnd:
        return s("and");
    case HIR_BINARY_BitwiseXor:
        return s("xor");
    case HIR_BINARY_BitwiseOr:
        return s("or");
    case HIR_BINARY_ShiftLeft:
        return s("shl");
    case HIR_BINARY_ShiftRight:
        return llvm_type_is_unsigned_integer(sema, type_index) ? s("lshr")
                                                               : s("ashr");
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

internal bool llvm_emit_pointer_arithmetic(LlvmFunctionContext* ctx,
                                           LlvmValue            lhs,
                                           LlvmValue            rhs,
                                           HirBinaryOp          op,
                                           u32                  result_type,
                                           LlvmValue*           out)
{
    SemaTypeKind lhs_kind = llvm_type_kind(ctx->sema, lhs.type_index);
    SemaTypeKind rhs_kind = llvm_type_kind(ctx->sema, rhs.type_index);
    bool         lhs_ptr  = lhs_kind == STK_Pointer;
    bool         rhs_ptr  = rhs_kind == STK_Pointer;
    bool         lhs_int  = llvm_integer_bits(ctx->sema, lhs.type_index) > 0;
    bool         rhs_int  = llvm_integer_bits(ctx->sema, rhs.type_index) > 0;

    if (op == HIR_BINARY_Add &&
        ((lhs_ptr && rhs_int) || (lhs_int && rhs_ptr))) {
        LlvmValue pointer = lhs_ptr ? lhs : rhs;
        LlvmValue offset  = lhs_ptr ? rhs : lhs;
        u32       pointee = llvm_pointee_type(ctx->sema, pointer.type_index);
        string    pointee_type = llvm_type_string(ctx, pointee);
        string    offset_type  = llvm_type_string(ctx, offset.type_index);
        string    temp         = llvm_temp(ctx);
        sb_format(ctx->sb,
                  "  " STRINGP " = getelementptr inbounds " STRINGP
                  ", ptr " STRINGP ", " STRINGP " " STRINGP "\n",
                  STRINGV(temp),
                  STRINGV(pointee_type),
                  STRINGV(pointer.value),
                  STRINGV(offset_type),
                  STRINGV(offset.value));
        *out = (LlvmValue){
            .ok         = true,
            .type_index = result_type,
            .value      = temp,
        };
        return true;
    }

    if (op == HIR_BINARY_Subtract && lhs_ptr && rhs_int) {
        u32    pointee      = llvm_pointee_type(ctx->sema, lhs.type_index);
        string pointee_type = llvm_type_string(ctx, pointee);
        string offset_type  = llvm_type_string(ctx, rhs.type_index);
        string negative     = llvm_temp(ctx);
        string temp         = llvm_temp(ctx);
        sb_format(ctx->sb,
                  "  " STRINGP " = sub " STRINGP " 0, " STRINGP "\n",
                  STRINGV(negative),
                  STRINGV(offset_type),
                  STRINGV(rhs.value));
        sb_format(ctx->sb,
                  "  " STRINGP " = getelementptr inbounds " STRINGP
                  ", ptr " STRINGP ", " STRINGP " " STRINGP "\n",
                  STRINGV(temp),
                  STRINGV(pointee_type),
                  STRINGV(lhs.value),
                  STRINGV(offset_type),
                  STRINGV(negative));
        *out = (LlvmValue){
            .ok         = true,
            .type_index = result_type,
            .value      = temp,
        };
        return true;
    }

    if (op == HIR_BINARY_Subtract && lhs_ptr && rhs_ptr) {
        u32    pointee       = llvm_pointee_type(ctx->sema, lhs.type_index);
        u64    element_size  = llvm_type_sizeof_bytes(ctx->sema, pointee);
        string lhs_int_value = llvm_temp(ctx);
        string rhs_int_value = llvm_temp(ctx);
        string byte_delta    = llvm_temp(ctx);
        string element_delta = llvm_temp(ctx);
        sb_format(ctx->sb,
                  "  " STRINGP " = ptrtoint ptr " STRINGP " to i64\n"
                  "  " STRINGP " = ptrtoint ptr " STRINGP " to i64\n"
                  "  " STRINGP " = sub i64 " STRINGP ", " STRINGP "\n",
                  STRINGV(lhs_int_value),
                  STRINGV(lhs.value),
                  STRINGV(rhs_int_value),
                  STRINGV(rhs.value),
                  STRINGV(byte_delta),
                  STRINGV(lhs_int_value),
                  STRINGV(rhs_int_value));
        sb_format(ctx->sb,
                  "  " STRINGP " = sdiv i64 " STRINGP ", %llu\n",
                  STRINGV(element_delta),
                  STRINGV(byte_delta),
                  (unsigned long long)(element_size == 0 ? 1 : element_size));
        *out = (LlvmValue){
            .ok         = true,
            .type_index = result_type,
            .value      = element_delta,
        };
        return true;
    }

    return false;
}

internal string llvm_compare_instruction(const Sema* sema,
                                         u32         type_index,
                                         HirBinaryOp op)
{
    switch (op) {
    case HIR_BINARY_Equal:
        return s("eq");
    case HIR_BINARY_NotEqual:
        return s("ne");
    case HIR_BINARY_Less:
        return llvm_type_is_unsigned_integer(sema, type_index) ? s("ult")
                                                               : s("slt");
    case HIR_BINARY_LessEqual:
        return llvm_type_is_unsigned_integer(sema, type_index) ? s("ule")
                                                               : s("sle");
    case HIR_BINARY_Greater:
        return llvm_type_is_unsigned_integer(sema, type_index) ? s("ugt")
                                                               : s("sgt");
    case HIR_BINARY_GreaterEqual:
        return llvm_type_is_unsigned_integer(sema, type_index) ? s("uge")
                                                               : s("sge");
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
    return string_format(arena, "0x%016llX", (unsigned long long)repr.bits);
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
    sb_format(
        ctx->sb, "  call void @string_builder_append_byte(i8 %u)\n", (u32)byte);
}

internal string llvm_runtime_integer_abi_attr(LlvmFunctionContext* ctx,
                                              u32                  type_index)
{
    SemaTypeKind kind = llvm_type_kind(ctx->sema, type_index);
    switch (kind) {
    case STK_Bool:
    case STK_U8:
    case STK_U16:
        return s(" zeroext");
    case STK_I8:
    case STK_I16:
        return s(" signext");
    default:
        return s("");
    }
}

internal const Ast* llvm_current_ast(const LlvmFunctionContext* ctx)
{
    if (ctx == NULL || ctx->hir == NULL || ctx->sema == NULL ||
        ctx->sema->program == NULL ||
        ctx->hir->current_module_index >=
            array_count(ctx->sema->program->modules)) {
        return NULL;
    }
    return &ctx->sema->program->modules[ctx->hir->current_module_index]
                .front_end.ast;
}

internal const AstNode* llvm_ast_unwrap_expr_or_stmt(const Ast* ast,
                                                     u32        node_index)
{
    while (ast != NULL && node_index < array_count(ast->nodes)) {
        const AstNode* node = &ast->nodes[node_index];
        if ((node->kind != AK_Expression && node->kind != AK_Statement) ||
            node->a >= array_count(ast->nodes)) {
            return node;
        }
        node_index = node->a;
    }
    return NULL;
}

internal bool llvm_impl_is_display_trait(const LlvmFunctionContext* ctx,
                                         const Ast*                 ast,
                                         const AstImplInfo*         impl)
{
    const AstNode* trait =
        impl != NULL
            ? llvm_ast_unwrap_expr_or_stmt(ast, impl->trait_type_node_index)
            : NULL;
    if (ctx == NULL || trait == NULL || trait->kind != AK_SymbolRef ||
        trait->a == U32_MAX ||
        !string_eq_cstr(lex_symbol(ctx->lexer, trait->a), "Display")) {
        return false;
    }

    for (u32 i = 0; i < array_count(ctx->sema->decls); ++i) {
        const SemaDecl* decl = &ctx->sema->decls[i];
        if (decl->symbol_handle != trait->a || decl->kind != SK_Trait) {
            continue;
        }
        u32 module_index = decl->import_module_index != sema_no_decl()
                               ? decl->import_module_index
                               : ctx->sema->current_module_index;
        return ctx->sema->program != NULL &&
               module_index < array_count(ctx->sema->program->modules) &&
               string_eq_cstr(
                   ctx->sema->program->modules[module_index].qualified_name,
                   "core");
    }
    return false;
}

internal bool llvm_display_show_function_index(LlvmFunctionContext* ctx,
                                               u32                  type_index,
                                               u32*                 out)
{
    const Ast* ast = llvm_current_ast(ctx);
    if (ctx == NULL || ast == NULL || out == NULL) {
        return false;
    }

    u32 string_type = llvm_builtin_type(ctx->sema, STK_String);
    for (u32 i = 0; i < array_count(ctx->sema->methods); ++i) {
        const SemaMethod* method = &ctx->sema->methods[i];
        if (!method->is_trait_impl || method->generic_params_index != U32_MAX ||
            !method->first_param_is_receiver ||
            method->symbol_handle == U32_MAX ||
            !string_eq_cstr(lex_symbol(ctx->lexer, method->symbol_handle),
                            "show") ||
            method->impl_node_index >= array_count(ast->nodes)) {
            continue;
        }

        const AstNode* impl_node = &ast->nodes[method->impl_node_index];
        if (impl_node->kind != AK_Impl ||
            impl_node->a >= array_count(ast->impls) ||
            !llvm_impl_is_display_trait(ctx, ast, &ast->impls[impl_node->a]) ||
            method->decl_index >= array_count(ctx->sema->decls)) {
            continue;
        }

        u32 fn_type = ctx->sema->decls[method->decl_index].type_index;
        if (!llvm_type_is_function(ctx->sema, fn_type) ||
            llvm_function_param_count(ctx->sema, fn_type) != 1 ||
            llvm_function_param_type(ctx->sema, fn_type, 0) != type_index ||
            llvm_function_return_type(ctx->sema, fn_type) != string_type) {
            continue;
        }

        for (u32 function_index = 0;
             function_index < array_count(ctx->hir->functions);
             ++function_index) {
            const HirFunction* function = &ctx->hir->functions[function_index];
            if (function->decl_index == method->decl_index ||
                function->fn_node_index ==
                    ctx->sema->decls[method->decl_index].value_node_index) {
                *out = function_index;
                return true;
            }
        }
    }
    return false;
}

internal bool
llvm_function_index_for_decl(LlvmFunctionContext* ctx, u32 decl_index, u32* out)
{
    if (ctx == NULL || out == NULL ||
        decl_index >= array_count(ctx->sema->decls)) {
        return false;
    }
    for (u32 function_index = 0;
         function_index < array_count(ctx->hir->functions);
         ++function_index) {
        const HirFunction* function = &ctx->hir->functions[function_index];
        if (function->decl_index == decl_index ||
            function->fn_node_index ==
                ctx->sema->decls[decl_index].value_node_index) {
            *out = function_index;
            return true;
        }
    }
    return false;
}

internal bool llvm_emit_append_display_string_value(LlvmFunctionContext* ctx,
                                                    LlvmValue            value)
{
    u32 function_index = U32_MAX;
    if (!llvm_display_show_function_index(
            ctx, value.type_index, &function_index)) {
        return false;
    }

    string function_name = llvm_function_name_string(
        ctx->hir, ctx->lexer, ctx->arena, function_index);
    string value_type = llvm_type_string(ctx, value.type_index);
    string shown      = llvm_temp(ctx);
    string shown_ptr  = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = call { ptr, i64 } " STRINGP "(" STRINGP
              " " STRINGP ")\n"
              "  " STRINGP " = alloca { ptr, i64 }\n"
              "  store { ptr, i64 } " STRINGP ", ptr " STRINGP "\n"
              "  call void @string_builder_append_string(ptr " STRINGP ")\n",
              STRINGV(shown),
              STRINGV(function_name),
              STRINGV(value_type),
              STRINGV(value.value),
              STRINGV(shown_ptr),
              STRINGV(shown),
              STRINGV(shown_ptr),
              STRINGV(shown_ptr));
    return true;
}

internal bool llvm_emit_append_string_value(LlvmFunctionContext* ctx,
                                            LlvmValue            value)
{
    SemaTypeKind kind = llvm_type_kind(ctx->sema, value.type_index);
    if (llvm_emit_append_display_string_value(ctx, value)) {
        return true;
    }

    if (kind == STK_Tuple || kind == STK_Plex) {
        llvm_emit_append_byte(ctx, '(');
        u32 field_count = llvm_record_field_count(ctx->sema, value.type_index);
        string record_type = llvm_type_string(ctx, value.type_index);
        for (u32 i = 0; i < field_count; ++i) {
            if (i > 0) {
                llvm_emit_append_byte(ctx, ',');
                llvm_emit_append_byte(ctx, ' ');
            }

            u32 field_type =
                llvm_record_field_type(ctx->sema, value.type_index, i);
            string field = llvm_temp(ctx);
            sb_format(ctx->sb,
                      "  " STRINGP " = extractvalue " STRINGP " " STRINGP
                      ", %u\n",
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
                      "  " STRINGP " = extractvalue " STRINGP " " STRINGP
                      ", %u\n",
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
                  "  " STRINGP " = extractvalue " STRINGP " " STRINGP ", 0\n",
                  STRINGV(data),
                  STRINGV(slice_type),
                  STRINGV(value.value));
        string count = llvm_temp(ctx);
        sb_format(ctx->sb,
                  "  " STRINGP " = extractvalue " STRINGP " " STRINGP ", 1\n",
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
                  "  " STRINGP " = icmp ult i64 " STRINGP ", " STRINGP "\n",
                  STRINGV(more),
                  STRINGV(index),
                  STRINGV(count));
        sb_format(ctx->sb,
                  "  br i1 " STRINGP ", label %%" STRINGP ", label %%" STRINGP
                  "\n",
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
                  "  br i1 " STRINGP ", label %%" STRINGP ", label %%" STRINGP
                  "\n",
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

    if (kind == STK_DynamicArray) {
        llvm_emit_append_byte(ctx, '[');

        string data_ptr  = llvm_temp(ctx);
        string count_ptr = llvm_temp(ctx);
        sb_format(ctx->sb,
                  "  " STRINGP " = alloca ptr\n"
                  "  " STRINGP " = alloca i64\n",
                  STRINGV(data_ptr),
                  STRINGV(count_ptr));

        string is_null = llvm_temp(ctx);
        sb_format(ctx->sb,
                  "  " STRINGP " = icmp eq ptr " STRINGP ", null\n",
                  STRINGV(is_null),
                  STRINGV(value.value));
        string empty_label = llvm_label(ctx, "dynarray.string.empty");
        string load_label  = llvm_label(ctx, "dynarray.string.load");
        string ready_label = llvm_label(ctx, "dynarray.string.ready");
        sb_format(ctx->sb,
                  "  br i1 " STRINGP ", label %%" STRINGP ", label %%" STRINGP
                  "\n",
                  STRINGV(is_null),
                  STRINGV(empty_label),
                  STRINGV(load_label));

        sb_format(ctx->sb, STRINGP ":\n", STRINGV(empty_label));
        sb_format(ctx->sb,
                  "  store ptr null, ptr " STRINGP "\n"
                  "  store i64 0, ptr " STRINGP "\n"
                  "  br label %%" STRINGP "\n",
                  STRINGV(data_ptr),
                  STRINGV(count_ptr),
                  STRINGV(ready_label));

        sb_format(ctx->sb, STRINGP ":\n", STRINGV(load_label));
        string loaded_data =
            llvm_dynamic_array_load_header_field(ctx, value.value, 0, s("ptr"));
        string loaded_count =
            llvm_dynamic_array_load_header_field(ctx, value.value, 1, s("i64"));
        sb_format(ctx->sb,
                  "  store ptr " STRINGP ", ptr " STRINGP "\n"
                  "  store i64 " STRINGP ", ptr " STRINGP "\n"
                  "  br label %%" STRINGP "\n",
                  STRINGV(loaded_data),
                  STRINGV(data_ptr),
                  STRINGV(loaded_count),
                  STRINGV(count_ptr),
                  STRINGV(ready_label));

        sb_format(ctx->sb, STRINGP ":\n", STRINGV(ready_label));
        string data  = llvm_temp(ctx);
        string count = llvm_temp(ctx);
        sb_format(ctx->sb,
                  "  " STRINGP " = load ptr, ptr " STRINGP "\n"
                  "  " STRINGP " = load i64, ptr " STRINGP "\n",
                  STRINGV(data),
                  STRINGV(data_ptr),
                  STRINGV(count),
                  STRINGV(count_ptr));

        string index_ptr = llvm_temp(ctx);
        sb_format(ctx->sb,
                  "  " STRINGP " = alloca i64\n"
                  "  store i64 0, ptr " STRINGP "\n",
                  STRINGV(index_ptr),
                  STRINGV(index_ptr));

        string cond_label = llvm_label(ctx, "dynarray.string.cond");
        string body_label = llvm_label(ctx, "dynarray.string.body");
        string sep_label  = llvm_label(ctx, "dynarray.string.sep");
        string item_label = llvm_label(ctx, "dynarray.string.item");
        string end_label  = llvm_label(ctx, "dynarray.string.end");

        sb_format(ctx->sb, "  br label %%" STRINGP "\n", STRINGV(cond_label));
        sb_format(ctx->sb, STRINGP ":\n", STRINGV(cond_label));
        string index = llvm_temp(ctx);
        sb_format(ctx->sb,
                  "  " STRINGP " = load i64, ptr " STRINGP "\n",
                  STRINGV(index),
                  STRINGV(index_ptr));
        string more = llvm_temp(ctx);
        sb_format(ctx->sb,
                  "  " STRINGP " = icmp ult i64 " STRINGP ", " STRINGP "\n",
                  STRINGV(more),
                  STRINGV(index),
                  STRINGV(count));
        sb_format(ctx->sb,
                  "  br i1 " STRINGP ", label %%" STRINGP ", label %%" STRINGP
                  "\n",
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
                  "  br i1 " STRINGP ", label %%" STRINGP ", label %%" STRINGP
                  "\n",
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

    string value_type    = llvm_type_string(ctx, value.type_index);
    string converted_ptr = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = alloca { ptr, i64 }\n",
              STRINGV(converted_ptr));
    if (string_eq_cstr(suffix, "string")) {
        string value_ptr = llvm_emit_string_value_pointer(ctx, value.value);
        sb_format(ctx->sb,
                  "  call void @to_string$string(ptr " STRINGP ", ptr " STRINGP
                  ")\n",
                  STRINGV(converted_ptr),
                  STRINGV(value_ptr));
    } else {
        string abi_attr = llvm_runtime_integer_abi_attr(ctx, value.type_index);
        sb_format(ctx->sb,
                  "  call void @to_string$" STRINGP "(ptr " STRINGP
                  ", " STRINGP STRINGP " " STRINGP ")\n",
                  STRINGV(suffix),
                  STRINGV(converted_ptr),
                  STRINGV(value_type),
                  STRINGV(abi_attr),
                  STRINGV(value.value));
    }
    sb_format(ctx->sb,
              "  call void @string_builder_append_string(ptr " STRINGP ")\n",
              STRINGV(converted_ptr));
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

internal bool llvm_emit_defers_to(LlvmFunctionContext* ctx,
                                  const HirFunction*   function,
                                  u32                  defer_count,
                                  bool                 pop)
{
    if (defer_count > array_count(ctx->defer_block_indices)) {
        return false;
    }

    u32 old_count = array_count(ctx->defer_block_indices);
    for (u32 i = old_count; i > defer_count; --i) {
        u32 block_index = ctx->defer_block_indices[i - 1];
        if (!llvm_emit_effect_block(ctx, function, block_index)) {
            return false;
        }
        if (ctx->block_terminated) {
            return false;
        }
    }

    if (pop && ctx->defer_block_indices != NULL) {
        __array_count(ctx->defer_block_indices) = defer_count;
    }
    return true;
}

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
        const HirImport* import = llvm_field_import(ctx->hir, callee);
        if (import != NULL) {
            const Hir* source_hir     = NULL;
            u32        function_index = U32_MAX;
            if (llvm_import_source_generic_function(ctx->sema,
                                                    ctx->lexer,
                                                    import,
                                                    callee->symbol_handle,
                                                    callee->type_index,
                                                    &source_hir,
                                                    &function_index)) {
                *out = llvm_function_name_string(
                    source_hir, ctx->lexer, ctx->arena, function_index);
            } else {
                *out = llvm_import_name_string(
                    ctx->sema, ctx->lexer, ctx->arena, import);
            }
            return true;
        }
        for (u32 i = 0; i < array_count(ctx->hir->functions); ++i) {
            const HirFunction* candidate = &ctx->hir->functions[i];
            u32                candidate_symbol =
                llvm_function_decl_symbol_handle(ctx->sema, candidate);
            if (candidate_symbol == U32_MAX) {
                candidate_symbol =
                    llvm_function_method_symbol_handle(ctx->sema, candidate);
            }
            if (candidate_symbol != U32_MAX &&
                string_eq(lex_symbol(ctx->lexer, candidate_symbol),
                          lex_symbol(ctx->lexer, callee->symbol_handle))) {
                *out = llvm_function_name_string(
                    ctx->hir, ctx->lexer, ctx->arena, i);
                return true;
            }
        }
        *out = llvm_symbol_name_string(
            ctx->lexer, ctx->arena, callee->symbol_handle);
        return true;
    }

    if (callee->kind == HIR_EXPR_LocalRef && callee->ref_kind == HIR_REF_Decl &&
        callee->ref_index < array_count(ctx->sema->decls)) {
        const SemaDecl* decl = &ctx->sema->decls[callee->ref_index];
        for (u32 i = 0; i < array_count(ctx->hir->functions); ++i) {
            const HirFunction* candidate = &ctx->hir->functions[i];
            if (candidate->decl_index == callee->ref_index ||
                candidate->fn_node_index == decl->value_node_index) {
                *out = llvm_function_name_string(
                    ctx->hir, ctx->lexer, ctx->arena, i);
                return true;
            }
        }
    }

    if (callee->kind != HIR_EXPR_LocalRef ||
        callee->ref_kind != HIR_REF_Binding ||
        callee->ref_index >= array_count(ctx->hir->bindings)) {
        LlvmValue callee_value =
            llvm_emit_expr(ctx, function, callee_expr_index);
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
        {
            const HirImport* import =
                llvm_binding_import(ctx->hir, callee->ref_index);
            if (import == NULL) {
                return false;
            }
            const Hir* source_hir     = NULL;
            u32        function_index = U32_MAX;
            if (llvm_import_source_generic_function(ctx->sema,
                                                    ctx->lexer,
                                                    import,
                                                    callee->symbol_handle,
                                                    callee->type_index,
                                                    &source_hir,
                                                    &function_index)) {
                *out = llvm_function_name_string(
                    source_hir, ctx->lexer, ctx->arena, function_index);
            } else {
                *out = llvm_import_name_string(
                    ctx->sema, ctx->lexer, ctx->arena, import);
            }
            return true;
        }
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

internal bool llvm_is_arena_constructor_call(LlvmFunctionContext* ctx,
                                             const HirExpr*       expr)
{
    if (expr == NULL || expr->kind != HIR_EXPR_Call ||
        llvm_type_kind(ctx->sema, expr->type_index) != STK_Arena ||
        expr->callee_expr_index >= array_count(ctx->hir->exprs)) {
        return false;
    }

    const HirExpr* callee = &ctx->hir->exprs[expr->callee_expr_index];
    return callee->kind == HIR_EXPR_LocalRef &&
           callee->symbol_handle != U32_MAX &&
           string_eq(lex_symbol(ctx->lexer, callee->symbol_handle), s("arena"));
}

internal bool
llvm_type_matches_for_call(const Sema* sema, u32 expected_type, u32 actual_type)
{
    if (expected_type == actual_type) {
        return true;
    }
    if (expected_type == sema_no_type() || actual_type == sema_no_type()) {
        return false;
    }
    if (sema_type_is_concrete_integer(sema, expected_type) &&
        llvm_type_kind(sema, actual_type) == STK_UntypedInteger) {
        return true;
    }
    if (sema_type_is_float(sema, expected_type) &&
        llvm_type_kind(sema, actual_type) == STK_UntypedFloat) {
        return true;
    }
    return false;
}

internal bool llvm_function_type_matches_call(LlvmFunctionContext* ctx,
                                              const HirFunction*   candidate,
                                              const HirExpr*       call)
{
    if (candidate == NULL || call == NULL ||
        !llvm_type_is_function(ctx->sema, candidate->type_index)) {
        return false;
    }

    u32 return_type =
        llvm_function_return_type(ctx->sema, candidate->type_index);
    if (!llvm_type_matches_for_call(ctx->sema, return_type, call->type_index)) {
        return false;
    }

    u32 param_count =
        llvm_function_param_count(ctx->sema, candidate->type_index);
    if (param_count != call->arg_count) {
        return false;
    }
    for (u32 i = 0; i < call->arg_count; ++i) {
        const HirCallArg* arg = &ctx->hir->call_args[call->first_arg + i];
        if (arg->expr_index >= array_count(ctx->hir->exprs)) {
            return false;
        }
        u32 param_type =
            llvm_function_param_type(ctx->sema, candidate->type_index, i);
        u32 arg_type = ctx->hir->exprs[arg->expr_index].type_index;
        if (!llvm_type_matches_for_call(ctx->sema, param_type, arg_type)) {
            return false;
        }
    }
    return true;
}

internal bool llvm_generic_function_for_decl_call(LlvmFunctionContext* ctx,
                                                  u32            decl_index,
                                                  const HirExpr* call,
                                                  u32*           out)
{
    for (u32 i = 0; i < array_count(ctx->hir->functions); ++i) {
        const HirFunction* candidate = &ctx->hir->functions[i];
        if (candidate->kind == HIR_FUNCTION_GenericInstantiation &&
            candidate->decl_index == decl_index &&
            llvm_function_type_matches_call(ctx, candidate, call)) {
            *out = i;
            return true;
        }
    }
    return false;
}

internal bool llvm_generic_function_for_decl_type(LlvmFunctionContext* ctx,
                                                  u32  decl_index,
                                                  u32  type_index,
                                                  u32* out)
{
    for (u32 i = 0; i < array_count(ctx->hir->functions); ++i) {
        const HirFunction* candidate = &ctx->hir->functions[i];
        if (candidate->kind == HIR_FUNCTION_GenericInstantiation &&
            candidate->decl_index == decl_index &&
            candidate->type_index == type_index) {
            *out = i;
            return true;
        }
    }
    return false;
}

internal bool llvm_explicit_generic_function_index(LlvmFunctionContext* ctx,
                                                   const HirExpr*       expr,
                                                   u32*                 out)
{
    if (expr == NULL || expr->kind != HIR_EXPR_Index ||
        !llvm_type_is_function(ctx->sema, expr->type_index) ||
        expr->operand_expr_index >= array_count(ctx->hir->exprs)) {
        return false;
    }

    const HirExpr* target = &ctx->hir->exprs[expr->operand_expr_index];
    if (target->kind == HIR_EXPR_LocalRef && target->ref_kind == HIR_REF_Decl) {
        return llvm_generic_function_for_decl_type(
            ctx, target->ref_index, expr->type_index, out);
    }
    return false;
}

internal bool llvm_generic_callee_name_for_call(LlvmFunctionContext* ctx,
                                                const HirExpr*       call,
                                                string*              out)
{
    if (call == NULL || call->kind != HIR_EXPR_Call ||
        call->callee_expr_index >= array_count(ctx->hir->exprs)) {
        return false;
    }

    const HirExpr* callee         = &ctx->hir->exprs[call->callee_expr_index];
    u32            function_index = U32_MAX;
    if (callee->kind == HIR_EXPR_LocalRef && callee->ref_kind == HIR_REF_Decl &&
        llvm_generic_function_for_decl_call(
            ctx, callee->ref_index, call, &function_index)) {
        *out = llvm_function_name_string(
            ctx->hir, ctx->lexer, ctx->arena, function_index);
        return true;
    }
    if (llvm_explicit_generic_function_index(ctx, callee, &function_index)) {
        *out = llvm_function_name_string(
            ctx->hir, ctx->lexer, ctx->arena, function_index);
        return true;
    }
    if (callee->kind == HIR_EXPR_Index &&
        llvm_type_is_function(ctx->sema, callee->type_index) &&
        callee->operand_expr_index < array_count(ctx->hir->exprs)) {
        const HirExpr* target = &ctx->hir->exprs[callee->operand_expr_index];
        if (target->kind == HIR_EXPR_LocalRef &&
            target->ref_kind == HIR_REF_Binding) {
            const HirImport* import =
                llvm_binding_import(ctx->hir, target->ref_index);
            const Hir* source_hir      = NULL;
            u32        source_fn_index = U32_MAX;
            if (import != NULL &&
                llvm_import_source_generic_function(ctx->sema,
                                                    ctx->lexer,
                                                    import,
                                                    target->symbol_handle,
                                                    callee->type_index,
                                                    &source_hir,
                                                    &source_fn_index)) {
                *out = llvm_function_name_string(
                    source_hir, ctx->lexer, ctx->arena, source_fn_index);
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

    if (llvm_type_kind(ctx->sema, source_type) == STK_Pointer &&
        llvm_type_kind(ctx->sema, target_type) == STK_Pointer) {
        return s("");
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
    case HIR_EXPR_DefaultValue:
        return llvm_default_value(ctx, expr->type_index);
    case HIR_EXPR_IntegerLiteral:
        SemaTypeKind integer_type_kind =
            llvm_type_kind(ctx->sema, expr->type_index);
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
            .value =
                string_format(ctx->arena, "%lld", (long long)expr->integer),
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

            string global = llvm_string_global_name_string(
                ctx->hir, ctx->arena, expr->string_index);
            if (expr->string_is_cstring) {
                return (LlvmValue){
                    .ok         = true,
                    .type_index = expr->type_index,
                    .value      = global,
                };
            }

            string value =
                string_format(ctx->arena,
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
            if (!llvm_eval_hir_string_constant(ctx->hir,
                                               ctx->lexer,
                                               ctx->arena,
                                               expr_index,
                                               &concat_value)) {
                return (LlvmValue){0};
            }
            string global = llvm_concat_string_global_name_string(
                ctx->hir, ctx->arena, expr_index);
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
            string result_ptr = llvm_temp(ctx);
            string result     = llvm_temp(ctx);
            sb_format(ctx->sb,
                      "  " STRINGP " = alloca { ptr, i64 }\n",
                      STRINGV(result_ptr));
            sb_format(ctx->sb,
                      "  call void @string_builder_finish(ptr " STRINGP
                      ", i64 " STRINGP ")\n",
                      STRINGV(result_ptr),
                      STRINGV(mark));
            sb_format(ctx->sb,
                      "  " STRINGP " = load { ptr, i64 }, ptr " STRINGP "\n",
                      STRINGV(result),
                      STRINGV(result_ptr));
            return (LlvmValue){
                .ok         = true,
                .type_index = expr->type_index,
                .value      = result,
            };
        }
    case HIR_EXPR_BuiltinMacro:
        {
            string name = expr->symbol_handle != U32_MAX
                              ? lex_symbol(ctx->lexer, expr->symbol_handle)
                              : (string){0};
            if (string_eq_cstr(name, "file")) {
                string        path      = ctx->macro_source_path.count > 0
                                              ? ctx->macro_source_path
                                              : expr->source_path;
                const Hir*    macro_hir = ctx->macro_source_hir != NULL
                                              ? ctx->macro_source_hir
                                              : ctx->hir;
                StringBuilder sb        = {0};
                sb_init(&sb, ctx->arena);
                llvm_append_builtin_module_file_global_name(&sb, macro_hir);
                string global = sb_to_string(&sb);
                return (LlvmValue){
                    .ok         = true,
                    .type_index = expr->type_index,
                    .value      = string_format(ctx->arena,
                                                "{ ptr " STRINGP ", i64 %zu }",
                                                STRINGV(global),
                                                path.count),
                };
            }
            if (string_eq_cstr(name, "line")) {
                u32 line = ctx->macro_source_line != 0 ? ctx->macro_source_line
                                                       : expr->source_line;
                return (LlvmValue){
                    .ok         = true,
                    .type_index = expr->type_index,
                    .value      = string_format(ctx->arena, "%u", line),
                };
            }
            return (LlvmValue){0};
        }
    case HIR_EXPR_BoolLiteral:
        return (LlvmValue){
            .ok         = true,
            .type_index = expr->type_index,
            .value      = expr->boolean ? s("1") : s("0"),
        };
    case HIR_EXPR_NilLiteral:
        return llvm_default_value(ctx, expr->type_index);
    case HIR_EXPR_Box:
        {
            u32 item_type =
                llvm_collection_item_type(ctx->sema, expr->type_index);
            if (item_type == sema_no_type()) {
                return (LlvmValue){0};
            }

            if (expr->arg_count == 1) {
                const HirCallArg* arg = &ctx->hir->call_args[expr->first_arg];
                LlvmValue         pointer =
                    llvm_emit_expr(ctx, function, arg->expr_index);
                if (!pointer.ok) {
                    return (LlvmValue){0};
                }
                pointer.type_index = expr->type_index;
                return pointer;
            }

            if (expr->arg_count != 0) {
                return (LlvmValue){0};
            }

            u64    size        = llvm_type_storage_bytes(ctx->sema, item_type);
            string source_path = llvm_builtin_module_file_global_name_string(
                ctx->hir, ctx->arena);
            string pointer = llvm_temp(ctx);
            sb_format(
                ctx->sb,
                "  " STRINGP
                " = call ptr @nrt_mem_alloc(i64 %llu, i64 16, ptr " STRINGP
                ", i32 %u)\n",
                STRINGV(pointer),
                (unsigned long long)size,
                STRINGV(source_path),
                expr->source_line);
            LlvmValue initial = llvm_default_value(ctx, item_type);
            if (!initial.ok) {
                return (LlvmValue){0};
            }
            string item_type_string = llvm_type_string(ctx, item_type);
            sb_format(ctx->sb,
                      "  store " STRINGP " " STRINGP ", ptr " STRINGP "\n",
                      STRINGV(item_type_string),
                      STRINGV(initial.value),
                      STRINGV(pointer));
            return (LlvmValue){
                .ok         = true,
                .type_index = expr->type_index,
                .value      = pointer,
            };
        }
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

                if (llvm_expr_is_constant_value(
                        ctx->hir, ctx->lexer, ctx->sema, expr_index)) {
                    string slice = string_format(
                        ctx->arena,
                        "{ ptr " STRINGP ", i64 %u }",
                        STRINGV(llvm_const_slice_backing_name_string(
                            ctx->hir, ctx->arena, expr_index)),
                        expr->arg_count);
                    array_free(values);
                    return (LlvmValue){
                        .ok         = true,
                        .type_index = expr->type_index,
                        .value      = slice,
                    };
                }

                string item_type_string = llvm_type_string(ctx, item_type);
                string array_type = string_format(ctx->arena,
                                                  "[%u x " STRINGP "]",
                                                  expr->arg_count,
                                                  STRINGV(item_type_string));
                string current =
                    expr->arg_count == 0 ? s("zeroinitializer") : s("poison");
                for (u32 i = 0; i < expr->arg_count; ++i) {
                    string temp = llvm_temp(ctx);
                    string value_type =
                        llvm_type_string(ctx, values[i].type_index);
                    sb_format(ctx->sb,
                              "  " STRINGP " = insertvalue " STRINGP " " STRINGP
                              ", " STRINGP " " STRINGP ", %u\n",
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
                          " = insertvalue { ptr, i64 } poison, ptr " STRINGP
                          ", 0\n",
                          STRINGV(slice0),
                          STRINGV(data_ptr));
                string slice1 = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = insertvalue { ptr, i64 } " STRINGP
                          ", i64 %u, 1\n",
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

                string capacity_value = {0};
                bool   runtime_capacity =
                    expr->extra_expr_index != U32_MAX &&
                    expr->extra_expr_index < array_count(ctx->hir->exprs);
                if (runtime_capacity) {
                    LlvmValue capacity =
                        llvm_emit_expr(ctx, function, expr->extra_expr_index);
                    if (!capacity.ok) {
                        array_free(values);
                        return (LlvmValue){0};
                    }
                    capacity = llvm_coerce_value_to_type(
                        ctx, capacity, llvm_builtin_type(ctx->sema, STK_Usize));
                    if (!capacity.ok) {
                        array_free(values);
                        return (LlvmValue){0};
                    }
                    capacity_value = capacity.value;
                    if (expr->arg_count > 0) {
                        string enough = llvm_temp(ctx);
                        string count  = llvm_temp(ctx);
                        sb_format(ctx->sb,
                                  "  " STRINGP " = icmp uge i64 " STRINGP
                                  ", %u\n"
                                  "  " STRINGP " = select i1 " STRINGP
                                  ", i64 " STRINGP ", i64 %u\n",
                                  STRINGV(enough),
                                  STRINGV(capacity_value),
                                  expr->arg_count,
                                  STRINGV(count),
                                  STRINGV(enough),
                                  STRINGV(capacity_value),
                                  expr->arg_count);
                        capacity_value = count;
                    }
                } else {
                    u32 capacity = expr->arg_count;
                    if (expr->integer > 0 && (u64)expr->integer > capacity) {
                        capacity = (u32)expr->integer;
                    }
                    capacity_value = string_format(ctx->arena, "%u", capacity);
                }

                u64 item_size = llvm_type_storage_bytes(ctx->sema, item_type);
                string header = llvm_temp(ctx);
                string source_path =
                    llvm_builtin_module_file_global_name_string(ctx->hir,
                                                                ctx->arena);
                u64 header_bytes = llvm_dynamic_array_header_bytes(ctx->layout);
                string alloc_bytes = string_format(
                    ctx->arena, "%llu", (unsigned long long)header_bytes);
                if (runtime_capacity) {
                    string data_bytes = llvm_temp(ctx);
                    alloc_bytes       = llvm_temp(ctx);
                    sb_format(ctx->sb,
                              "  " STRINGP " = mul i64 " STRINGP ", %llu\n"
                              "  " STRINGP " = add i64 %llu, " STRINGP "\n",
                              STRINGV(data_bytes),
                              STRINGV(capacity_value),
                              (unsigned long long)item_size,
                              STRINGV(alloc_bytes),
                              (unsigned long long)header_bytes,
                              STRINGV(data_bytes));
                } else if (!string_eq_cstr(capacity_value, "0")) {
                    u64 capacity = (u64)expr->arg_count;
                    if (expr->integer > 0 && (u64)expr->integer > capacity) {
                        capacity = (u64)expr->integer;
                    }
                    alloc_bytes = string_format(
                        ctx->arena,
                        "%llu",
                        (unsigned long long)(header_bytes +
                                             capacity * item_size));
                }
                sb_format(ctx->sb,
                          "  " STRINGP " = call ptr @nrt_mem_alloc(i64 " STRINGP
                          ", i64 16, ptr " STRINGP ", i32 %u)\n",
                          STRINGV(header),
                          STRINGV(alloc_bytes),
                          STRINGV(source_path),
                          expr->source_line);
                string data_ptr_ptr =
                    llvm_dynamic_array_header_field_ptr(ctx, header, 0);
                string count_ptr =
                    llvm_dynamic_array_header_field_ptr(ctx, header, 1);
                string capacity_ptr =
                    llvm_dynamic_array_header_field_ptr(ctx, header, 2);

                string data = llvm_dynamic_array_data_from_header(ctx, header);
                if (runtime_capacity || !string_eq_cstr(capacity_value, "0")) {
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
                          "  store i64 " STRINGP ", ptr " STRINGP "\n",
                          STRINGV(data),
                          STRINGV(data_ptr_ptr),
                          expr->arg_count,
                          STRINGV(count_ptr),
                          STRINGV(capacity_value),
                          STRINGV(capacity_ptr));
                array_free(values);
                return (LlvmValue){
                    .ok         = true,
                    .type_index = expr->type_index,
                    .value      = data,
                };
            }

            LlvmValue result = llvm_build_aggregate_value(
                ctx, expr->type_index, values, array_count(values));
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
            LlvmValue result = llvm_build_aggregate_value(
                ctx, expr->type_index, values, array_count(values));
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

                const HirCallArg* arg = &ctx->hir->call_args[expr->first_arg];
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
                return llvm_cast_to_union_storage(
                    ctx, field_value, expr->type_index);
            }

            if (expr->kind == HIR_EXPR_Plex) {
                Array(LlvmValue) values = NULL;
                u32 field_count =
                    llvm_record_field_count(ctx->sema, expr->type_index);
                for (u32 i = 0; i < field_count; ++i) {
                    u32 field_type =
                        llvm_record_field_type(ctx->sema, expr->type_index, i);
                    LlvmValue field_value = llvm_default_value(ctx, field_type);
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
                LlvmValue result = llvm_build_aggregate_value(
                    ctx, expr->type_index, values, field_count);
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
                    string temp = llvm_temp(ctx);
                    string record_type =
                        llvm_type_string(ctx, expr->type_index);
                    string field_type =
                        llvm_type_string(ctx, field_value.type_index);
                    sb_format(ctx->sb,
                              "  " STRINGP " = insertvalue " STRINGP " " STRINGP
                              ", " STRINGP " " STRINGP ", %u\n",
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

            string value = llvm_param_value(
                function, ctx->hir, ctx->lexer, ctx->arena, expr->ref_index);
            u32 param_type =
                llvm_param_type(function, ctx->hir, expr->ref_index);
            if (value.count == 0 && expr->symbol_handle != U32_MAX) {
                value      = llvm_param_value_for_symbol(function,
                                                         ctx->hir,
                                                         ctx->lexer,
                                                         ctx->arena,
                                                         expr->symbol_handle);
                param_type = llvm_param_type_for_symbol(
                    function, ctx->hir, expr->symbol_handle);
            }
            return (LlvmValue){
                .ok         = value.count > 0,
                .type_index = param_type != sema_no_type() ? param_type
                                                           : expr->type_index,
                .value      = value,
            };
        }
        if (expr->ref_kind == HIR_REF_Binding &&
            expr->ref_index < array_count(ctx->hir->bindings)) {
            const HirBinding* binding = &ctx->hir->bindings[expr->ref_index];
            if (binding->kind == HIR_BINDING_Function &&
                binding->target_index < array_count(ctx->hir->functions)) {
                return (LlvmValue){
                    .ok         = true,
                    .type_index = expr->type_index,
                    .value = llvm_function_name_string(ctx->hir,
                                                       ctx->lexer,
                                                       ctx->arena,
                                                       binding->target_index),
                };
            }
            if (binding->kind == HIR_BINDING_Import) {
                const HirImport* import =
                    llvm_binding_import(ctx->hir, expr->ref_index);
                if (import != NULL) {
                    return llvm_emit_imported_constant_value(
                        ctx, function, import, expr->type_index);
                }
            }
        }
        u32 value_index = U32_MAX;
        if (llvm_ref_value_index(
                ctx->hir, expr->ref_kind, expr->ref_index, &value_index)) {
            const HirValue* value = &ctx->hir->values[value_index];
            if (value->kind == HIR_VALUE_Constant &&
                value->value_expr_index != U32_MAX) {
                return llvm_emit_expr(ctx, function, value->value_expr_index);
            }
            if (value->kind == HIR_VALUE_Constant) {
                HirImport        decl_import = {0};
                const HirImport* import      = llvm_import_for_symbol(
                    ctx->hir, expr->symbol_handle, value->type_index);
                if (import == NULL) {
                    import = llvm_import_for_symbol_any_type(
                        ctx->hir, expr->symbol_handle);
                }
                if (import == NULL) {
                    import = llvm_import_for_symbol_name(
                        ctx->hir, ctx->lexer, expr->symbol_handle);
                }
                if (import == NULL && ctx->sema != NULL &&
                    value->decl_index < array_count(ctx->sema->decls)) {
                    const SemaDecl* decl = &ctx->sema->decls[value->decl_index];
                    if (decl->import_module_index != sema_no_decl()) {
                        decl_import = (HirImport){
                            .module_index  = decl->import_module_index,
                            .decl_index    = decl->import_decl_index,
                            .symbol_handle = decl->symbol_handle,
                            .type_index    = decl->type_index,
                        };
                        import = &decl_import;
                    }
                }
                if (import != NULL) {
                    return llvm_emit_imported_constant_value(
                        ctx, function, import, expr->type_index);
                }
            }
            if (value->kind == HIR_VALUE_Global) {
                string name = llvm_value_name_string(
                    ctx->hir, ctx->lexer, ctx->arena, value_index);
                if (name.count == 0) {
                    return (LlvmValue){0};
                }
                string type   = llvm_type_string(ctx, value->type_index);
                string loaded = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = load " STRINGP ", ptr " STRINGP "\n",
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
            string cmp = llvm_compare_instruction(
                ctx->sema, expr->type_index, expr->binary_op);
            if (cmp.count > 0) {
                LlvmValue lhs =
                    llvm_emit_expr(ctx, function, expr->lhs_expr_index);
                LlvmValue rhs =
                    llvm_emit_expr(ctx, function, expr->rhs_expr_index);
                if (!lhs.ok || !rhs.ok) {
                    return (LlvmValue){0};
                }

                cmp = llvm_compare_instruction(
                    ctx->sema, lhs.type_index, expr->binary_op);
                string       type = llvm_type_string(ctx, lhs.type_index);
                string       temp = llvm_temp(ctx);
                SemaTypeKind lhs_kind =
                    llvm_type_kind(ctx->sema, lhs.type_index);
                SemaTypeKind rhs_kind =
                    llvm_type_kind(ctx->sema, rhs.type_index);
                if (lhs_kind == STK_String && rhs_kind == STK_String) {
                    string lhs_ptr =
                        llvm_emit_string_value_pointer(ctx, lhs.value);
                    string rhs_ptr =
                        llvm_emit_string_value_pointer(ctx, rhs.value);
                    sb_format(ctx->sb,
                              "  " STRINGP " = call i1 @string_eq(ptr " STRINGP
                              ", ptr " STRINGP ")\n",
                              STRINGV(temp),
                              STRINGV(lhs_ptr),
                              STRINGV(rhs_ptr));
                    if (expr->binary_op == HIR_BINARY_NotEqual) {
                        string inverted = llvm_temp(ctx);
                        sb_format(ctx->sb,
                                  "  " STRINGP " = xor i1 " STRINGP ", 1\n",
                                  STRINGV(inverted),
                                  STRINGV(temp));
                        temp = inverted;
                    }
                } else if (lhs_kind == STK_Slice && rhs_kind == STK_Slice) {
                    string lhs_data  = llvm_temp(ctx);
                    string lhs_count = llvm_temp(ctx);
                    string rhs_data  = llvm_temp(ctx);
                    string rhs_count = llvm_temp(ctx);
                    sb_format(ctx->sb,
                              "  " STRINGP " = extractvalue " STRINGP
                              " " STRINGP ", 0\n"
                              "  " STRINGP " = extractvalue " STRINGP
                              " " STRINGP ", 1\n",
                              STRINGV(lhs_data),
                              STRINGV(type),
                              STRINGV(lhs.value),
                              STRINGV(lhs_count),
                              STRINGV(type),
                              STRINGV(lhs.value));
                    string rhs_type = llvm_type_string(ctx, rhs.type_index);
                    sb_format(ctx->sb,
                              "  " STRINGP " = extractvalue " STRINGP
                              " " STRINGP ", 0\n"
                              "  " STRINGP " = extractvalue " STRINGP
                              " " STRINGP ", 1\n",
                              STRINGV(rhs_data),
                              STRINGV(rhs_type),
                              STRINGV(rhs.value),
                              STRINGV(rhs_count),
                              STRINGV(rhs_type),
                              STRINGV(rhs.value));
                    string data_eq  = llvm_temp(ctx);
                    string count_eq = llvm_temp(ctx);
                    string both_eq  = llvm_temp(ctx);
                    sb_format(
                        ctx->sb,
                        "  " STRINGP " = icmp eq ptr " STRINGP ", " STRINGP "\n"
                        "  " STRINGP " = icmp eq i64 " STRINGP ", " STRINGP "\n"
                        "  " STRINGP " = and i1 " STRINGP ", " STRINGP "\n",
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
                           llvm_type_kind(ctx->sema, rhs.type_index) ==
                               STK_Enum) {
                    string lhs_tag = llvm_temp(ctx);
                    sb_format(ctx->sb,
                              "  " STRINGP " = extractvalue " STRINGP
                              " " STRINGP ", 0\n",
                              STRINGV(lhs_tag),
                              STRINGV(type),
                              STRINGV(lhs.value));
                    string rhs_type = llvm_type_string(ctx, rhs.type_index);
                    string rhs_tag  = llvm_temp(ctx);
                    sb_format(ctx->sb,
                              "  " STRINGP " = extractvalue " STRINGP
                              " " STRINGP ", 0\n",
                              STRINGV(rhs_tag),
                              STRINGV(rhs_type),
                              STRINGV(rhs.value));
                    sb_format(ctx->sb,
                              "  " STRINGP " = icmp " STRINGP " i64 " STRINGP
                              ", " STRINGP "\n",
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
                    .type_index = llvm_builtin_type(ctx->sema, STK_Bool),
                    .value      = temp,
                };
            }
            LlvmValue lhs = llvm_emit_expr(ctx, function, expr->lhs_expr_index);
            LlvmValue rhs = llvm_emit_expr(ctx, function, expr->rhs_expr_index);
            if (!lhs.ok || !rhs.ok) {
                return (LlvmValue){0};
            }
            if (expr->binary_op == HIR_BINARY_LogicalAnd ||
                expr->binary_op == HIR_BINARY_LogicalOr) {
                u32 bool_type = llvm_builtin_type(ctx->sema, STK_Bool);
                lhs           = llvm_coerce_value_to_type(ctx, lhs, bool_type);
                rhs           = llvm_coerce_value_to_type(ctx, rhs, bool_type);
                if (!lhs.ok || !rhs.ok) {
                    return (LlvmValue){0};
                }
            }

            LlvmValue pointer_arithmetic = {0};
            if (llvm_emit_pointer_arithmetic(ctx,
                                             lhs,
                                             rhs,
                                             expr->binary_op,
                                             expr->type_index,
                                             &pointer_arithmetic)) {
                return pointer_arithmetic;
            }

            u32 result_type = expr->type_index;
            if (llvm_integer_bits(ctx->sema, result_type) == 0 &&
                llvm_float_bits(ctx->sema, result_type) == 0) {
                if (llvm_integer_bits(ctx->sema, lhs.type_index) > 0 ||
                    llvm_float_bits(ctx->sema, lhs.type_index) > 0) {
                    result_type = lhs.type_index;
                } else if (llvm_integer_bits(ctx->sema, rhs.type_index) > 0 ||
                           llvm_float_bits(ctx->sema, rhs.type_index) > 0) {
                    result_type = rhs.type_index;
                }
            }
            string type  = llvm_type_string(ctx, result_type);
            string temp  = llvm_temp(ctx);
            string instr = llvm_binary_instruction(
                ctx->sema, result_type, expr->binary_op);
            if (instr.count == 0) {
                return (LlvmValue){0};
            }
            if (llvm_float_bits(ctx->sema, result_type) > 0) {
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
                .type_index = result_type,
                .value      = temp,
            };
        }
    case HIR_EXPR_Assign:
        {
            bool old_discard_expr_value = ctx->discard_expr_value;
            ctx->discard_expr_value     = false;
            LlvmValue value =
                llvm_emit_expr(ctx, function, expr->rhs_expr_index);
            ctx->discard_expr_value = old_discard_expr_value;
            if (!value.ok ||
                !llvm_emit_assign(ctx, function, expr->lhs_expr_index, value)) {
                return (LlvmValue){0};
            }
            u32 excluded_local = U32_MAX;
            if (expr->lhs_expr_index < array_count(ctx->hir->exprs)) {
                const HirExpr* target = &ctx->hir->exprs[expr->lhs_expr_index];
                if (target->kind == HIR_EXPR_LocalRef &&
                    target->ref_kind == HIR_REF_Local) {
                    excluded_local = target->ref_index;
                }
            }
            if (!llvm_consume_box_expr(ctx,
                                       expr->rhs_expr_index,
                                       value.type_index,
                                       excluded_local)) {
                return (LlvmValue){0};
            }
            value.type_index = expr->type_index;
            return value;
        }
    case HIR_EXPR_Unary:
        {
            if (expr->unary_op == HIR_UNARY_AddressOf) {
                LlvmValue address = llvm_address_of_expr(
                    ctx, function, expr->operand_expr_index);
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
                type    = s("i1");
                operand = llvm_coerce_value_to_type(
                    ctx, operand, llvm_builtin_type(ctx->sema, STK_Bool));
                if (!operand.ok) {
                    return (LlvmValue){0};
                }
                sb_format(ctx->sb,
                          "  " STRINGP " = xor " STRINGP " " STRINGP ", 1\n",
                          STRINGV(temp),
                          STRINGV(type),
                          STRINGV(operand.value));
                return (LlvmValue){
                    .ok         = true,
                    .type_index = llvm_builtin_type(ctx->sema, STK_Bool),
                    .value      = temp,
                };
            case HIR_UNARY_Negate:
                if (llvm_float_bits(ctx->sema, expr->type_index) > 0) {
                    sb_format(ctx->sb,
                              "  " STRINGP " = fneg " STRINGP " " STRINGP "\n",
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
            case HIR_UNARY_BitwiseNot:
                sb_format(ctx->sb,
                          "  " STRINGP " = xor " STRINGP " " STRINGP ", -1\n",
                          STRINGV(temp),
                          STRINGV(type),
                          STRINGV(operand.value));
                return (LlvmValue){
                    .ok         = true,
                    .type_index = expr->type_index,
                    .value      = temp,
                };
            case HIR_UNARY_Deref:
                {
                    string pointee = llvm_type_string(ctx, expr->type_index);
                    sb_format(ctx->sb,
                              "  " STRINGP " = load " STRINGP ", ptr " STRINGP
                              "\n",
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
            u32 generic_function_index = U32_MAX;
            if (llvm_explicit_generic_function_index(
                    ctx, expr, &generic_function_index)) {
                return (LlvmValue){
                    .ok         = true,
                    .type_index = expr->type_index,
                    .value = llvm_function_name_string(ctx->hir,
                                                       ctx->lexer,
                                                       ctx->arena,
                                                       generic_function_index),
                };
            }

            LlvmValue target =
                llvm_emit_expr(ctx, function, expr->operand_expr_index);
            LlvmValue index =
                llvm_emit_expr(ctx, function, expr->extra_expr_index);
            if (!target.ok || !index.ok) {
                return (LlvmValue){0};
            }

            if (llvm_type_kind(ctx->sema, target.type_index) == STK_Array) {
                u32 item_type =
                    llvm_collection_item_type(ctx->sema, target.type_index);
                if (item_type == sema_no_type()) {
                    item_type = expr->type_index;
                }

                i64 index_value = 0;
                if (llvm_expr_integer_constant(
                        ctx->hir, expr->extra_expr_index, &index_value) &&
                    index_value >= 0 &&
                    (u64)index_value <
                        llvm_array_count(ctx->sema, target.type_index)) {
                    string array_type =
                        llvm_type_string(ctx, target.type_index);
                    string loaded = llvm_temp(ctx);
                    sb_format(ctx->sb,
                              "  " STRINGP " = extractvalue " STRINGP
                              " " STRINGP ", %lld\n",
                              STRINGV(loaded),
                              STRINGV(array_type),
                              STRINGV(target.value),
                              (long long)index_value);
                    return (LlvmValue){
                        .ok         = true,
                        .type_index = item_type,
                        .value      = loaded,
                    };
                }

                LlvmValue item_ptr =
                    llvm_address_of_expr(ctx, function, expr_index);
                if (!item_ptr.ok) {
                    return (LlvmValue){0};
                }
                string item_type_string = llvm_type_string(ctx, item_type);
                string loaded           = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = load " STRINGP ", ptr " STRINGP "\n",
                          STRINGV(loaded),
                          STRINGV(item_type_string),
                          STRINGV(item_ptr.value));
                return (LlvmValue){
                    .ok         = true,
                    .type_index = item_type,
                    .value      = loaded,
                };
            }

            if (llvm_type_kind(ctx->sema, target.type_index) == STK_Slice) {
                string slice_type = llvm_type_string(ctx, target.type_index);
                string data_ptr   = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = extractvalue " STRINGP " " STRINGP
                          ", 0\n",
                          STRINGV(data_ptr),
                          STRINGV(slice_type),
                          STRINGV(target.value));

                u32 item_type =
                    llvm_collection_item_type(ctx->sema, target.type_index);
                if (item_type == sema_no_type()) {
                    item_type = expr->type_index;
                }
                string item_type_string = llvm_type_string(ctx, item_type);
                string index_type = llvm_type_string(ctx, index.type_index);
                string item_ptr   = llvm_temp(ctx);
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
                          "  " STRINGP " = load " STRINGP ", ptr " STRINGP "\n",
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
                string index_type = llvm_type_string(ctx, index.type_index);
                string item_ptr   = llvm_temp(ctx);
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
                          "  " STRINGP " = load " STRINGP ", ptr " STRINGP "\n",
                          STRINGV(loaded),
                          STRINGV(item_type_string),
                          STRINGV(item_ptr));
                return (LlvmValue){
                    .ok         = true,
                    .type_index = expr->type_index,
                    .value      = loaded,
                };
            }

            u32 item_type = sema_no_type();
            if (expr->operand_expr_index < array_count(ctx->hir->exprs)) {
                const HirExpr* target_expr =
                    &ctx->hir->exprs[expr->operand_expr_index];
                item_type = llvm_data_field_item_type(ctx, target_expr);
            }
            if (item_type == sema_no_type()) {
                item_type = llvm_pointee_type(ctx->sema, target.type_index);
            }
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
            u32 result_type = expr->type_index;
            if (llvm_integer_bits(ctx->sema, result_type) == 0 &&
                llvm_integer_bits(ctx->sema, item_type) > 0) {
                result_type = item_type;
            }
            return (LlvmValue){
                .ok         = true,
                .type_index = result_type,
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
                          "  " STRINGP " = extractvalue { ptr, i64 } " STRINGP
                          ", 0\n",
                          STRINGV(data),
                          STRINGV(target.value));

                string total_count = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = extractvalue { ptr, i64 } " STRINGP
                          ", 1\n",
                          STRINGV(total_count),
                          STRINGV(target.value));

                i64    start_value       = 0;
                bool   start_is_constant = true;
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

                i64    end_value       = 0;
                bool   end_is_constant = false;
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

                string count    = llvm_slice_count_i64(ctx,
                                                       start,
                                                       start_is_constant,
                                                       start_value,
                                                       end,
                                                       end_is_constant,
                                                       end_value);

                string data_ptr = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP
                          " = getelementptr inbounds i8, ptr " STRINGP
                          ", i64 " STRINGP "\n",
                          STRINGV(data_ptr),
                          STRINGV(data),
                          STRINGV(start));
                string slice0 = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP
                          " = insertvalue { ptr, i64 } poison, ptr " STRINGP
                          ", 0\n",
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

            if (llvm_type_kind(ctx->sema, target_type) == STK_Slice) {
                LlvmValue target =
                    llvm_emit_expr(ctx, function, expr->operand_expr_index);
                if (!target.ok) {
                    return (LlvmValue){0};
                }

                string slice_type = llvm_type_string(ctx, target_type);
                string data       = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = extractvalue " STRINGP " " STRINGP
                          ", 0\n",
                          STRINGV(data),
                          STRINGV(slice_type),
                          STRINGV(target.value));

                string total_count = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = extractvalue " STRINGP " " STRINGP
                          ", 1\n",
                          STRINGV(total_count),
                          STRINGV(slice_type),
                          STRINGV(target.value));

                i64    start_value       = 0;
                bool   start_is_constant = true;
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

                i64    end_value       = 0;
                bool   end_is_constant = false;
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
                          STRINGV(data),
                          STRINGV(start));
                string slice0 = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP
                          " = insertvalue { ptr, i64 } poison, ptr " STRINGP
                          ", 0\n",
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

            if (llvm_type_kind(ctx->sema, target_type) == STK_DynamicArray) {
                LlvmValue target =
                    llvm_emit_expr(ctx, function, expr->operand_expr_index);
                if (!target.ok) {
                    return (LlvmValue){0};
                }

                string data       = llvm_temp(ctx);
                string count      = llvm_temp(ctx);
                string data_slot  = llvm_temp(ctx);
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

                i64    start_value       = 0;
                bool   start_is_constant = true;
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

                i64    end_value       = 0;
                bool   end_is_constant = false;
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
                u32    item_type =
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
                          " = insertvalue { ptr, i64 } poison, ptr " STRINGP
                          ", 0\n",
                          STRINGV(slice0),
                          STRINGV(data_ptr));
                string slice1 = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = insertvalue { ptr, i64 } " STRINGP
                          ", i64 " STRINGP ", 1\n",
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
            u32 item_type = llvm_collection_item_type(ctx->sema, target_type);
            if (item_type == sema_no_type()) {
                return (LlvmValue){0};
            }

            i64    start_value       = 0;
            bool   start_is_constant = true;
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

            i64    end_value = (i64)llvm_array_count(ctx->sema, target_type);
            bool   end_is_constant = true;
            string end =
                string_format(ctx->arena, "%lld", (long long)end_value);
            if (expr->rhs_expr_index < array_count(ctx->hir->exprs)) {
                end_is_constant = llvm_expr_integer_constant(
                    ctx->hir, expr->rhs_expr_index, &end_value);
                if (end_is_constant) {
                    end =
                        string_format(ctx->arena, "%lld", (long long)end_value);
                } else {
                    llvm_emit_index_i64(
                        ctx, function, expr->rhs_expr_index, &end);
                }
            }

            string count              = llvm_slice_count_i64(ctx,
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
                      "  " STRINGP
                      " = insertvalue { ptr, i64 } poison, ptr " STRINGP
                      ", 0\n",
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
                expr->symbol_handle != U32_MAX &&
                string_eq_cstr(lex_symbol(ctx->lexer, expr->symbol_handle),
                               "size") &&
                !llvm_record_type_has_field(
                    ctx->sema,
                    ctx->hir->exprs[expr->operand_expr_index].type_index,
                    expr->symbol_handle)) {
                u32 source_type =
                    ctx->hir->exprs[expr->operand_expr_index].type_index;
                source_type = sema_materialise_type(ctx->sema, source_type);
                return (LlvmValue){
                    .ok         = true,
                    .type_index = llvm_builtin_type(ctx->sema, STK_Usize),
                    .value      = string_format(
                        ctx->arena,
                        "%llu",
                        (unsigned long long)llvm_type_sizeof_bytes(
                            ctx->sema, source_type)),
                };
            }

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

            if (expr->kind == HIR_EXPR_Field &&
                expr->symbol_handle != U32_MAX &&
                string_eq_cstr(lex_symbol(ctx->lexer, expr->symbol_handle),
                               "count")) {
                u32 source_type =
                    ctx->hir->exprs[expr->operand_expr_index].type_index;
                u32 member_type =
                    llvm_member_target_type(ctx->sema, source_type);
                if (llvm_type_kind(ctx->sema, member_type) == STK_Array) {
                    return (LlvmValue){
                        .ok         = true,
                        .type_index = llvm_builtin_type(ctx->sema, STK_Usize),
                        .value      = string_format(
                            ctx->arena,
                            "%llu",
                            (unsigned long long)ctx->sema->types[member_type]
                                .return_type),
                    };
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
                       STK_Pointer ||
                   llvm_type_kind(ctx->sema, target.type_index) == STK_Box) {
                u32 pointee_type =
                    llvm_type_kind(ctx->sema, target.type_index) == STK_Box
                        ? ctx->sema->types[target.type_index].first_param_type
                        : llvm_pointee_type(ctx->sema, target.type_index);
                SemaTypeKind pointee_kind =
                    llvm_type_kind(ctx->sema, pointee_type);
                if (pointee_kind != STK_Pointer && pointee_kind != STK_Tuple &&
                    pointee_kind != STK_Plex && pointee_kind != STK_Union &&
                    pointee_kind != STK_String && pointee_kind != STK_Slice &&
                    pointee_kind != STK_DynamicArray) {
                    break;
                }

                string pointee = llvm_type_string(ctx, pointee_type);
                string loaded  = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = load " STRINGP ", ptr " STRINGP "\n",
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

            u32 result_type = expr->type_index;
            if (expr->kind == HIR_EXPR_Field &&
                (llvm_type_kind(ctx->sema, target.type_index) == STK_String ||
                 llvm_type_kind(ctx->sema, target.type_index) == STK_Slice)) {
                u32 field_type = llvm_record_field_type(
                    ctx->sema, target.type_index, field_index);
                if (field_type != sema_no_type()) {
                    result_type = field_type;
                }
            } else if (llvm_type_kind(ctx->sema, target.type_index) ==
                           STK_Tuple ||
                       llvm_type_kind(ctx->sema, target.type_index) ==
                           STK_Plex) {
                u32 field_type = llvm_record_field_type(
                    ctx->sema, target.type_index, field_index);
                if (field_type != sema_no_type()) {
                    result_type = field_type;
                }
            }

            if (llvm_type_kind(ctx->sema, target.type_index) == STK_Union) {
                u32 field_type = llvm_record_field_type(
                    ctx->sema, target.type_index, field_index);
                if (field_type == sema_no_type()) {
                    return (LlvmValue){0};
                }
                return llvm_cast_from_union_storage(ctx, target, field_type);
            }

            string temp        = llvm_temp(ctx);
            string record_type = llvm_type_string(ctx, target.type_index);
            sb_format(ctx->sb,
                      "  " STRINGP " = extractvalue " STRINGP " " STRINGP
                      ", %u\n",
                      STRINGV(temp),
                      STRINGV(record_type),
                      STRINGV(target.value),
                      field_index);
            return (LlvmValue){
                .ok         = true,
                .type_index = result_type,
                .value      = temp,
            };
        }
    case HIR_EXPR_Block:
        {
            if (expr->body_block_index >= array_count(ctx->hir->blocks)) {
                return (LlvmValue){0};
            }

            string end_label                = llvm_label(ctx, "block.end");
            string old_break                = ctx->break_label;
            string old_continue             = ctx->continue_label;
            string old_break_value_ptr      = ctx->break_value_ptr;
            u32    old_break_value_type     = ctx->break_value_type;
            u32    old_break_defer_count    = ctx->break_defer_count;
            u32    old_continue_defer_count = ctx->continue_defer_count;
            bool   old_break_emitted        = ctx->emitted_break;
            u32    block_defer_base = array_count(ctx->defer_block_indices);

            u32 block_result_type = ctx->discard_expr_value
                                        ? llvm_builtin_type(ctx->sema, STK_Void)
                                        : expr->type_index;
            string result_ptr     = {0};
            if (!llvm_type_is_void(ctx->sema, block_result_type)) {
                string result_type = llvm_type_string(ctx, block_result_type);
                result_ptr         = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = alloca " STRINGP ", align 4\n",
                          STRINGV(result_ptr),
                          STRINGV(result_type));
                LlvmValue default_value =
                    llvm_default_value(ctx, block_result_type);
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

            ctx->break_label          = end_label;
            ctx->continue_label       = old_continue;
            ctx->break_value_ptr      = result_ptr;
            ctx->break_value_type     = block_result_type;
            ctx->break_defer_count    = block_defer_base;
            ctx->continue_defer_count = block_defer_base;
            ctx->emitted_break        = false;
            llvm_push_control_target(
                ctx,
                (LlvmControlTarget){
                    .symbol_handle        = expr->symbol_handle,
                    .break_label          = end_label,
                    .continue_label       = old_continue,
                    .break_value_ptr      = result_ptr,
                    .break_value_type     = block_result_type,
                    .break_defer_count    = block_defer_base,
                    .continue_defer_count = block_defer_base,
                });

            bool emitted =
                llvm_emit_effect_block(ctx, function, expr->body_block_index);
            bool block_emitted_break =
                ctx->emitted_break ||
                llvm_control_target_emitted_break(ctx, expr->symbol_handle);
            llvm_pop_control_target(ctx, expr->symbol_handle);
            ctx->break_label          = old_break;
            ctx->continue_label       = old_continue;
            ctx->break_value_ptr      = old_break_value_ptr;
            ctx->break_value_type     = old_break_value_type;
            ctx->break_defer_count    = old_break_defer_count;
            ctx->continue_defer_count = old_continue_defer_count;
            ctx->emitted_break        = old_break_emitted;
            if (!emitted) {
                return (LlvmValue){0};
            }

            if (ctx->block_terminated && !block_emitted_break) {
                return (LlvmValue){
                    .ok         = true,
                    .type_index = expr->type_index,
                    .value      = s(""),
                };
            }

            if (!ctx->block_terminated) {
                sb_format(
                    ctx->sb, "  br label %%" STRINGP "\n", STRINGV(end_label));
                block_emitted_break = true;
            }

            if (block_emitted_break) {
                sb_format(ctx->sb, STRINGP ":\n", STRINGV(end_label));
                ctx->block_terminated = false;
            }

            if (llvm_type_is_void(ctx->sema, block_result_type)) {
                return (LlvmValue){
                    .ok         = true,
                    .type_index = block_result_type,
                    .value      = s(""),
                };
            }

            string result_type = llvm_type_string(ctx, block_result_type);
            string loaded      = llvm_temp(ctx);
            sb_format(ctx->sb,
                      "  " STRINGP " = load " STRINGP ", ptr " STRINGP
                      ", align 4\n",
                      STRINGV(loaded),
                      STRINGV(result_type),
                      STRINGV(result_ptr));
            return (LlvmValue){
                .ok         = true,
                .type_index = block_result_type,
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
                if (expr->on_kind == HIR_ON_Bool) {
                    scrutinee = llvm_coerce_value_to_type(
                        ctx, scrutinee, llvm_builtin_type(ctx->sema, STK_Bool));
                    if (!scrutinee.ok) {
                        return (LlvmValue){0};
                    }
                }

                if (llvm_type_is_void(ctx->sema, expr->type_index) ||
                    ctx->discard_expr_value) {
                    string end_label = llvm_label(ctx, "on.end");
                    for (u32 i = 0; i < expr->branch_count; ++i) {
                        const HirOnBranch* branch =
                            &ctx->hir->on_branches[expr->first_branch + i];
                        string body_label = llvm_label(ctx, "on.body");
                        string next_label = i + 1 < expr->branch_count
                                                ? llvm_label(ctx, "on.next")
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
                                      "  br i1 " STRINGP ", label %%" STRINGP
                                      ", label %%" STRINGP "\n",
                                      STRINGV(condition.value),
                                      STRINGV(body_label),
                                      STRINGV(next_label));
                        }

                        sb_format(ctx->sb, STRINGP ":\n", STRINGV(body_label));
                        llvm_debug_emit_step_anchor(
                            ctx, branch->source_line, branch->source_path);
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
                            sb_format(
                                ctx->sb, STRINGP ":\n", STRINGV(next_label));
                        }
                    }
                    sb_format(ctx->sb, STRINGP ":\n", STRINGV(end_label));
                    return (LlvmValue){
                        .ok         = true,
                        .type_index = expr->type_index,
                        .value      = s(""),
                    };
                }

                string end_label            = llvm_label(ctx, "on.end");
                Array(LlvmValue) phi_values = NULL;
                Array(string) phi_labels    = NULL;
                bool ended_with_else        = false;

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
                        LlvmValue condition =
                            llvm_emit_branch_pattern_condition(
                                ctx, function, scrutinee, branch);
                        if (!condition.ok) {
                            array_free(phi_values);
                            array_free(phi_labels);
                            return (LlvmValue){0};
                        }

                        if (branch->guard_expr_index != U32_MAX) {
                            string guard_label = llvm_label(ctx, "on.guard");
                            sb_format(ctx->sb,
                                      "  br i1 " STRINGP ", label %%" STRINGP
                                      ", label %%" STRINGP "\n",
                                      STRINGV(condition.value),
                                      STRINGV(guard_label),
                                      STRINGV(next_label));
                            sb_format(
                                ctx->sb, STRINGP ":\n", STRINGV(guard_label));
                            llvm_bind_symbol_value(
                                ctx, branch->binder_symbol_handle, scrutinee);
                            LlvmValue guard = llvm_emit_expr(
                                ctx, function, branch->guard_expr_index);
                            if (!guard.ok) {
                                array_free(phi_values);
                                array_free(phi_labels);
                                return (LlvmValue){0};
                            }
                            guard = llvm_coerce_value_to_type(
                                ctx,
                                guard,
                                llvm_builtin_type(ctx->sema, STK_Bool));
                            if (!guard.ok) {
                                array_free(phi_values);
                                array_free(phi_labels);
                                return (LlvmValue){0};
                            }
                            sb_format(ctx->sb,
                                      "  br i1 " STRINGP ", label %%" STRINGP
                                      ", label %%" STRINGP "\n",
                                      STRINGV(guard.value),
                                      STRINGV(body_label),
                                      STRINGV(next_label));
                        } else {
                            sb_format(ctx->sb,
                                      "  br i1 " STRINGP ", label %%" STRINGP
                                      ", label %%" STRINGP "\n",
                                      STRINGV(condition.value),
                                      STRINGV(body_label),
                                      STRINGV(next_label));
                        }
                    }

                    sb_format(ctx->sb, STRINGP ":\n", STRINGV(body_label));
                    llvm_debug_emit_step_anchor(
                        ctx, branch->source_line, branch->source_path);
                    llvm_bind_symbol_value(
                        ctx, branch->binder_symbol_handle, scrutinee);
                    LlvmValue value = llvm_emit_block_value(
                        ctx, function, branch->body_block_index);
                    if (!value.ok) {
                        array_free(phi_values);
                        array_free(phi_labels);
                        return (LlvmValue){0};
                    }
                    if (!ctx->block_terminated) {
                        if (value.value.count == 0) {
                            value = llvm_default_value(ctx, expr->type_index);
                            if (!value.ok) {
                                array_free(phi_values);
                                array_free(phi_labels);
                                return (LlvmValue){0};
                            }
                        }
                        string value_label = llvm_label(ctx, "on.value");
                        sb_format(ctx->sb,
                                  "  br label %%" STRINGP "\n" STRINGP ":\n"
                                  "  br label %%" STRINGP "\n",
                                  STRINGV(value_label),
                                  STRINGV(value_label),
                                  STRINGV(end_label));
                        array_push(phi_values, value);
                        array_push(phi_labels, value_label);
                    }
                    ctx->block_terminated = false;

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
                    ctx->block_terminated = true;
                    return (LlvmValue){
                        .ok         = true,
                        .type_index = expr->type_index,
                        .value      = s(""),
                    };
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

            string end_label            = llvm_label(ctx, "on.end");
            Array(LlvmValue) phi_values = NULL;
            Array(string) phi_labels    = NULL;
            bool ended_with_else        = false;

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
                    condition = llvm_coerce_value_to_type(
                        ctx, condition, llvm_builtin_type(ctx->sema, STK_Bool));
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
                llvm_debug_emit_step_anchor(
                    ctx, branch->source_line, branch->source_path);
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
                    if (!ctx->block_terminated) {
                        if (value.value.count == 0) {
                            value = llvm_default_value(ctx, expr->type_index);
                            if (!value.ok) {
                                array_free(phi_values);
                                array_free(phi_labels);
                                return (LlvmValue){0};
                            }
                        }
                        string value_label = llvm_label(ctx, "on.value");
                        sb_format(ctx->sb,
                                  "  br label %%" STRINGP "\n" STRINGP ":\n"
                                  "  br label %%" STRINGP "\n",
                                  STRINGV(value_label),
                                  STRINGV(value_label),
                                  STRINGV(end_label));
                        array_push(phi_values, value);
                        array_push(phi_labels, value_label);
                    }
                    ctx->block_terminated = false;
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

            if (llvm_type_is_void(ctx->sema, expr->type_index) ||
                ctx->discard_expr_value) {
                sb_format(ctx->sb, STRINGP ":\n", STRINGV(end_label));
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
                ctx->block_terminated = true;
                return (LlvmValue){
                    .ok         = true,
                    .type_index = expr->type_index,
                    .value      = s(""),
                };
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
                const HirExpr* iterable_expr =
                    loop->iterable_expr_index < array_count(ctx->hir->exprs)
                        ? &ctx->hir->exprs[loop->iterable_expr_index]
                        : NULL;
                if (iterable_expr != NULL &&
                    (iterable_expr->kind == HIR_EXPR_RangeExclusive ||
                     iterable_expr->kind == HIR_EXPR_RangeInclusive)) {
                    LlvmValue start = llvm_emit_expr(
                        ctx, function, iterable_expr->lhs_expr_index);
                    LlvmValue end = llvm_emit_expr(
                        ctx, function, iterable_expr->rhs_expr_index);
                    if (!start.ok || !end.ok) {
                        return (LlvmValue){0};
                    }
                    start = llvm_coerce_value_to_type(
                        ctx, start, iterable_expr->type_index);
                    end = llvm_coerce_value_to_type(
                        ctx, end, iterable_expr->type_index);
                    if (!start.ok || !end.ok) {
                        return (LlvmValue){0};
                    }

                    string result_ptr = {0};
                    bool   has_result =
                        !llvm_type_is_void(ctx->sema, expr->type_index);
                    if (has_result) {
                        string result_type =
                            llvm_type_string(ctx, expr->type_index);
                        result_ptr = llvm_temp(ctx);
                        sb_format(ctx->sb,
                                  "  " STRINGP " = alloca " STRINGP
                                  ", align 4\n",
                                  STRINGV(result_ptr),
                                  STRINGV(result_type));
                        LlvmValue default_value =
                            llvm_default_value(ctx, expr->type_index);
                        if (!default_value.ok) {
                            return (LlvmValue){0};
                        }
                        sb_format(ctx->sb,
                                  "  store " STRINGP " " STRINGP
                                  ", ptr " STRINGP ", align 4\n",
                                  STRINGV(result_type),
                                  STRINGV(default_value.value),
                                  STRINGV(result_ptr));
                    }

                    u32    range_type = iterable_expr->type_index;
                    string range_type_string =
                        llvm_type_string(ctx, range_type);
                    string current_ptr = llvm_temp(ctx);
                    sb_format(ctx->sb,
                              "  " STRINGP " = alloca " STRINGP "\n",
                              STRINGV(current_ptr),
                              STRINGV(range_type_string));
                    sb_format(ctx->sb,
                              "  store " STRINGP " " STRINGP ", ptr " STRINGP
                              "\n",
                              STRINGV(range_type_string),
                              STRINGV(start.value),
                              STRINGV(current_ptr));

                    u32 index_type =
                        llvm_local_type(ctx, loop->index_local_index);
                    if (index_type == sema_no_type()) {
                        index_type = llvm_builtin_type(ctx->sema, STK_Usize);
                    }
                    LlvmLocalSlot* index_slot        = NULL;
                    LlvmLocalSlot  hidden_index_slot = {0};
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
                    sb_format(ctx->sb,
                              "  store i64 0, ptr " STRINGP "\n",
                              STRINGV(index_slot->ptr));

                    u32 item_type =
                        llvm_local_type(ctx, loop->item_local_index);
                    LlvmLocalSlot* item_slot = NULL;
                    if (loop->item_local_index != U32_MAX) {
                        item_slot = llvm_ensure_local_slot(
                            ctx, loop->item_local_index, item_type);
                    }

                    string cond_label   = llvm_label(ctx, "for.range.cond");
                    string body_label   = llvm_label(ctx, "for.range.body");
                    string update_label = llvm_label(ctx, "for.range.update");
                    string else_label  = loop->else_block_index != U32_MAX
                                             ? llvm_label(ctx, "for.range.else")
                                             : (string){0};
                    string end_label   = llvm_label(ctx, "for.range.end");
                    string false_label = loop->else_block_index != U32_MAX
                                             ? else_label
                                             : end_label;
                    sb_format(ctx->sb,
                              "  br label %%" STRINGP "\n",
                              STRINGV(cond_label));

                    sb_format(ctx->sb, STRINGP ":\n", STRINGV(cond_label));
                    string current = llvm_temp(ctx);
                    sb_format(ctx->sb,
                              "  " STRINGP " = load " STRINGP ", ptr " STRINGP
                              "\n",
                              STRINGV(current),
                              STRINGV(range_type_string),
                              STRINGV(current_ptr));
                    string cond = llvm_temp(ctx);
                    bool   inclusive =
                        iterable_expr->kind == HIR_EXPR_RangeInclusive;
                    string predicate =
                        llvm_type_is_unsigned_integer(ctx->sema, range_type)
                            ? (inclusive ? s("ule") : s("ult"))
                            : (inclusive ? s("sle") : s("slt"));
                    sb_format(ctx->sb,
                              "  " STRINGP " = icmp " STRINGP " " STRINGP
                              " " STRINGP ", " STRINGP "\n",
                              STRINGV(cond),
                              STRINGV(predicate),
                              STRINGV(range_type_string),
                              STRINGV(current),
                              STRINGV(end.value));
                    sb_format(ctx->sb,
                              "  br i1 " STRINGP ", label %%" STRINGP
                              ", label %%" STRINGP "\n",
                              STRINGV(cond),
                              STRINGV(body_label),
                              STRINGV(false_label));

                    sb_format(ctx->sb, STRINGP ":\n", STRINGV(body_label));
                    if (item_slot != NULL) {
                        llvm_store_local_slot(ctx,
                                              item_slot,
                                              (LlvmValue){
                                                  .ok         = true,
                                                  .type_index = item_type,
                                                  .value      = current,
                                              });
                    }

                    string old_break                = ctx->break_label;
                    string old_continue             = ctx->continue_label;
                    string old_break_value_ptr      = ctx->break_value_ptr;
                    u32    old_break_value_type     = ctx->break_value_type;
                    u32    old_break_defer_count    = ctx->break_defer_count;
                    u32    old_continue_defer_count = ctx->continue_defer_count;
                    bool   old_break_emitted        = ctx->emitted_break;
                    u32 loop_defer_base = array_count(ctx->defer_block_indices);
                    ctx->break_label    = end_label;
                    ctx->continue_label = update_label;
                    ctx->break_value_ptr      = result_ptr;
                    ctx->break_value_type     = expr->type_index;
                    ctx->break_defer_count    = loop_defer_base;
                    ctx->continue_defer_count = loop_defer_base;
                    ctx->emitted_break        = false;
                    llvm_push_control_target(
                        ctx,
                        (LlvmControlTarget){
                            .symbol_handle        = loop->label_symbol,
                            .break_label          = end_label,
                            .continue_label       = update_label,
                            .break_value_ptr      = result_ptr,
                            .break_value_type     = expr->type_index,
                            .break_defer_count    = loop_defer_base,
                            .continue_defer_count = loop_defer_base,
                        });
                    if (!llvm_emit_effect_block(
                            ctx, function, loop->body_block_index)) {
                        llvm_pop_control_target(ctx, loop->label_symbol);
                        ctx->break_label          = old_break;
                        ctx->continue_label       = old_continue;
                        ctx->break_value_ptr      = old_break_value_ptr;
                        ctx->break_value_type     = old_break_value_type;
                        ctx->break_defer_count    = old_break_defer_count;
                        ctx->continue_defer_count = old_continue_defer_count;
                        ctx->emitted_break        = old_break_emitted;
                        return (LlvmValue){0};
                    }
                    llvm_pop_control_target(ctx, loop->label_symbol);

                    if (!ctx->block_terminated) {
                        llvm_debug_emit_marker(
                            ctx,
                            llvm_debug_for_header_line(ctx, loop, expr),
                            expr->source_path);
                        sb_format(ctx->sb,
                                  "  br label %%" STRINGP "\n",
                                  STRINGV(update_label));
                    }
                    sb_format(ctx->sb, STRINGP ":\n", STRINGV(update_label));
                    string next = llvm_temp(ctx);
                    sb_format(ctx->sb,
                              "  " STRINGP " = add " STRINGP " " STRINGP
                              ", 1\n",
                              STRINGV(next),
                              STRINGV(range_type_string),
                              STRINGV(current));
                    sb_format(ctx->sb,
                              "  store " STRINGP " " STRINGP ", ptr " STRINGP
                              "\n",
                              STRINGV(range_type_string),
                              STRINGV(next),
                              STRINGV(current_ptr));
                    string index_current = llvm_temp(ctx);
                    sb_format(ctx->sb,
                              "  " STRINGP " = load i64, ptr " STRINGP "\n",
                              STRINGV(index_current),
                              STRINGV(index_slot->ptr));
                    string index_next = llvm_temp(ctx);
                    sb_format(ctx->sb,
                              "  " STRINGP " = add i64 " STRINGP ", 1\n",
                              STRINGV(index_next),
                              STRINGV(index_current));
                    sb_format(ctx->sb,
                              "  store i64 " STRINGP ", ptr " STRINGP "\n",
                              STRINGV(index_next),
                              STRINGV(index_slot->ptr));
                    sb_format(ctx->sb,
                              "  br label %%" STRINGP "\n",
                              STRINGV(cond_label));

                    if (loop->else_block_index != U32_MAX) {
                        sb_format(ctx->sb, STRINGP ":\n", STRINGV(else_label));
                        if (!llvm_emit_effect_block(
                                ctx, function, loop->else_block_index)) {
                            ctx->break_label       = old_break;
                            ctx->continue_label    = old_continue;
                            ctx->break_value_ptr   = old_break_value_ptr;
                            ctx->break_value_type  = old_break_value_type;
                            ctx->break_defer_count = old_break_defer_count;
                            ctx->continue_defer_count =
                                old_continue_defer_count;
                            ctx->emitted_break = old_break_emitted;
                            return (LlvmValue){0};
                        }
                        if (!ctx->block_terminated) {
                            sb_format(ctx->sb,
                                      "  br label %%" STRINGP "\n",
                                      STRINGV(end_label));
                        }
                    }
                    ctx->break_label          = old_break;
                    ctx->continue_label       = old_continue;
                    ctx->break_value_ptr      = old_break_value_ptr;
                    ctx->break_value_type     = old_break_value_type;
                    ctx->break_defer_count    = old_break_defer_count;
                    ctx->continue_defer_count = old_continue_defer_count;
                    ctx->emitted_break        = old_break_emitted;
                    ctx->block_terminated     = false;
                    llvm_debug_emit_marker(
                        ctx, expr->source_line, expr->source_path);
                    sb_format(ctx->sb, STRINGP ":\n", STRINGV(end_label));
                    if (has_result) {
                        string result_type =
                            llvm_type_string(ctx, expr->type_index);
                        string loaded = llvm_temp(ctx);
                        sb_format(ctx->sb,
                                  "  " STRINGP " = load " STRINGP
                                  ", ptr " STRINGP ", align 4\n",
                                  STRINGV(loaded),
                                  STRINGV(result_type),
                                  STRINGV(result_ptr));
                        return (LlvmValue){
                            .ok         = true,
                            .type_index = expr->type_index,
                            .value      = loaded,
                        };
                    }
                    return (LlvmValue){
                        .ok         = true,
                        .type_index = expr->type_index,
                        .value      = s(""),
                    };
                }

                if (loop->iterator_next_decl_index != sema_no_decl()) {
                    u32 function_index = U32_MAX;
                    if (!llvm_function_index_for_decl(
                            ctx,
                            loop->iterator_next_decl_index,
                            &function_index)) {
                        return (LlvmValue){0};
                    }

                    const HirExpr* iterable_expr =
                        loop->iterable_expr_index < array_count(ctx->hir->exprs)
                            ? &ctx->hir->exprs[loop->iterable_expr_index]
                            : NULL;
                    if (iterable_expr == NULL) {
                        return (LlvmValue){0};
                    }

                    LlvmValue iterable_ptr = llvm_address_of_expr(
                        ctx, function, loop->iterable_expr_index);
                    if (!iterable_ptr.ok) {
                        return (LlvmValue){0};
                    }

                    const HirFunction* next_function =
                        &ctx->hir->functions[function_index];
                    u32 next_type = next_function->type_index;
                    u32 option_type =
                        llvm_function_return_type(ctx->sema, next_type);
                    if (llvm_type_kind(ctx->sema, option_type) != STK_Enum) {
                        return (LlvmValue){0};
                    }

                    u32 some_variant = U32_MAX;
                    for (u32 i = 0;
                         i < ctx->sema->types[option_type].param_count;
                         ++i) {
                        u32 symbol =
                            ctx->sema
                                ->type_param_symbols[ctx->sema
                                                         ->types[option_type]
                                                         .first_param_type +
                                                     i];
                        if (string_eq_cstr(lex_symbol(ctx->lexer, symbol),
                                           "Some")) {
                            some_variant = i;
                            break;
                        }
                    }
                    if (some_variant == U32_MAX) {
                        return (LlvmValue){0};
                    }

                    string result_ptr = {0};
                    bool   has_result =
                        !llvm_type_is_void(ctx->sema, expr->type_index);
                    if (has_result) {
                        string result_type =
                            llvm_type_string(ctx, expr->type_index);
                        result_ptr = llvm_temp(ctx);
                        sb_format(ctx->sb,
                                  "  " STRINGP " = alloca " STRINGP
                                  ", align 4\n",
                                  STRINGV(result_ptr),
                                  STRINGV(result_type));
                        LlvmValue default_value =
                            llvm_default_value(ctx, expr->type_index);
                        if (!default_value.ok) {
                            return (LlvmValue){0};
                        }
                        sb_format(ctx->sb,
                                  "  store " STRINGP " " STRINGP
                                  ", ptr " STRINGP ", align 4\n",
                                  STRINGV(result_type),
                                  STRINGV(default_value.value),
                                  STRINGV(result_ptr));
                    }

                    u32 index_type =
                        llvm_local_type(ctx, loop->index_local_index);
                    if (index_type == sema_no_type()) {
                        index_type = llvm_builtin_type(ctx->sema, STK_Usize);
                    }
                    LlvmLocalSlot* index_slot        = NULL;
                    LlvmLocalSlot  hidden_index_slot = {0};
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
                    sb_format(ctx->sb,
                              "  store i64 0, ptr " STRINGP "\n",
                              STRINGV(index_slot->ptr));

                    u32 item_type =
                        llvm_local_type(ctx, loop->item_local_index);
                    LlvmLocalSlot* item_slot = NULL;
                    if (loop->item_local_index != U32_MAX) {
                        item_slot = llvm_ensure_local_slot(
                            ctx, loop->item_local_index, item_type);
                    }

                    string cond_label  = llvm_label(ctx, "for.iter.cond");
                    string body_label  = llvm_label(ctx, "for.iter.body");
                    string else_label  = loop->else_block_index != U32_MAX
                                             ? llvm_label(ctx, "for.iter.else")
                                             : (string){0};
                    string end_label   = llvm_label(ctx, "for.iter.end");
                    string false_label = loop->else_block_index != U32_MAX
                                             ? else_label
                                             : end_label;
                    sb_format(ctx->sb,
                              "  br label %%" STRINGP "\n",
                              STRINGV(cond_label));

                    sb_format(ctx->sb, STRINGP ":\n", STRINGV(cond_label));
                    string next_name = llvm_function_name_string(
                        ctx->hir, ctx->lexer, ctx->arena, function_index);
                    string option_type_string =
                        llvm_type_string(ctx, option_type);
                    string self_type_string = llvm_type_string(
                        ctx, llvm_function_param_type(ctx->sema, next_type, 0));
                    string option_value = llvm_temp(ctx);
                    sb_format(ctx->sb,
                              "  " STRINGP " = call " STRINGP " " STRINGP
                              "(" STRINGP " " STRINGP ")\n",
                              STRINGV(option_value),
                              STRINGV(option_type_string),
                              STRINGV(next_name),
                              STRINGV(self_type_string),
                              STRINGV(iterable_ptr.value));
                    string tag = llvm_temp(ctx);
                    sb_format(ctx->sb,
                              "  " STRINGP " = extractvalue " STRINGP
                              " " STRINGP ", 0\n",
                              STRINGV(tag),
                              STRINGV(option_type_string),
                              STRINGV(option_value));
                    string has_value         = llvm_temp(ctx);
                    i64    some_discriminant = llvm_enum_variant_discriminant(
                        ctx->sema, option_type, some_variant);
                    sb_format(ctx->sb,
                              "  " STRINGP " = icmp eq i64 " STRINGP ", %lld\n",
                              STRINGV(has_value),
                              STRINGV(tag),
                              (long long)some_discriminant);
                    sb_format(ctx->sb,
                              "  br i1 " STRINGP ", label %%" STRINGP
                              ", label %%" STRINGP "\n",
                              STRINGV(has_value),
                              STRINGV(body_label),
                              STRINGV(false_label));

                    sb_format(ctx->sb, STRINGP ":\n", STRINGV(body_label));
                    if (item_slot != NULL) {
                        u32 storage_payload_bits =
                            llvm_enum_storage_payload_bits(ctx->sema,
                                                           option_type);
                        u32 payload_type = llvm_enum_variant_payload_type(
                            ctx->sema, option_type, some_variant);
                        string payload_bits = llvm_temp(ctx);
                        sb_format(ctx->sb,
                                  "  " STRINGP " = extractvalue " STRINGP
                                  " " STRINGP ", 1\n",
                                  STRINGV(payload_bits),
                                  STRINGV(option_type_string),
                                  STRINGV(option_value));
                        LlvmValue payload = llvm_cast_from_storage_bits(
                            ctx,
                            (LlvmValue){
                                .ok         = true,
                                .type_index = sema_no_type(),
                                .value      = payload_bits,
                            },
                            storage_payload_bits,
                            payload_type);
                        if (!payload.ok) {
                            return (LlvmValue){0};
                        }
                        llvm_store_local_slot(ctx, item_slot, payload);
                    }

                    string old_break                = ctx->break_label;
                    string old_continue             = ctx->continue_label;
                    string old_break_value_ptr      = ctx->break_value_ptr;
                    u32    old_break_value_type     = ctx->break_value_type;
                    u32    old_break_defer_count    = ctx->break_defer_count;
                    u32    old_continue_defer_count = ctx->continue_defer_count;
                    bool   old_break_emitted        = ctx->emitted_break;
                    u32 loop_defer_base = array_count(ctx->defer_block_indices);
                    ctx->break_label    = end_label;
                    ctx->continue_label = cond_label;
                    ctx->break_value_ptr      = result_ptr;
                    ctx->break_value_type     = expr->type_index;
                    ctx->break_defer_count    = loop_defer_base;
                    ctx->continue_defer_count = loop_defer_base;
                    ctx->emitted_break        = false;
                    llvm_push_control_target(
                        ctx,
                        (LlvmControlTarget){
                            .symbol_handle        = loop->label_symbol,
                            .break_label          = end_label,
                            .continue_label       = cond_label,
                            .break_value_ptr      = result_ptr,
                            .break_value_type     = expr->type_index,
                            .break_defer_count    = loop_defer_base,
                            .continue_defer_count = loop_defer_base,
                        });
                    if (!llvm_emit_effect_block(
                            ctx, function, loop->body_block_index)) {
                        llvm_pop_control_target(ctx, loop->label_symbol);
                        ctx->break_label          = old_break;
                        ctx->continue_label       = old_continue;
                        ctx->break_value_ptr      = old_break_value_ptr;
                        ctx->break_value_type     = old_break_value_type;
                        ctx->break_defer_count    = old_break_defer_count;
                        ctx->continue_defer_count = old_continue_defer_count;
                        ctx->emitted_break        = old_break_emitted;
                        return (LlvmValue){0};
                    }
                    llvm_pop_control_target(ctx, loop->label_symbol);

                    if (!ctx->block_terminated) {
                        llvm_debug_emit_marker(
                            ctx,
                            llvm_debug_for_header_line(ctx, loop, expr),
                            expr->source_path);
                        string current = llvm_temp(ctx);
                        sb_format(ctx->sb,
                                  "  " STRINGP " = load i64, ptr " STRINGP "\n",
                                  STRINGV(current),
                                  STRINGV(index_slot->ptr));
                        string next = llvm_temp(ctx);
                        sb_format(ctx->sb,
                                  "  " STRINGP " = add i64 " STRINGP ", 1\n",
                                  STRINGV(next),
                                  STRINGV(current));
                        sb_format(ctx->sb,
                                  "  store i64 " STRINGP ", ptr " STRINGP "\n",
                                  STRINGV(next),
                                  STRINGV(index_slot->ptr));
                        sb_format(ctx->sb,
                                  "  br label %%" STRINGP "\n",
                                  STRINGV(cond_label));
                    }
                    if (loop->else_block_index != U32_MAX) {
                        sb_format(ctx->sb, STRINGP ":\n", STRINGV(else_label));
                        if (!llvm_emit_effect_block(
                                ctx, function, loop->else_block_index)) {
                            ctx->break_label       = old_break;
                            ctx->continue_label    = old_continue;
                            ctx->break_value_ptr   = old_break_value_ptr;
                            ctx->break_value_type  = old_break_value_type;
                            ctx->break_defer_count = old_break_defer_count;
                            ctx->continue_defer_count =
                                old_continue_defer_count;
                            ctx->emitted_break = old_break_emitted;
                            return (LlvmValue){0};
                        }
                        if (!ctx->block_terminated) {
                            sb_format(ctx->sb,
                                      "  br label %%" STRINGP "\n",
                                      STRINGV(end_label));
                        }
                    }
                    ctx->break_label          = old_break;
                    ctx->continue_label       = old_continue;
                    ctx->break_value_ptr      = old_break_value_ptr;
                    ctx->break_value_type     = old_break_value_type;
                    ctx->break_defer_count    = old_break_defer_count;
                    ctx->continue_defer_count = old_continue_defer_count;
                    ctx->emitted_break        = old_break_emitted;
                    ctx->block_terminated     = false;
                    llvm_debug_emit_marker(
                        ctx, expr->source_line, expr->source_path);
                    sb_format(ctx->sb, STRINGP ":\n", STRINGV(end_label));
                    if (has_result) {
                        string result_type =
                            llvm_type_string(ctx, expr->type_index);
                        string loaded = llvm_temp(ctx);
                        sb_format(ctx->sb,
                                  "  " STRINGP " = load " STRINGP
                                  ", ptr " STRINGP ", align 4\n",
                                  STRINGV(loaded),
                                  STRINGV(result_type),
                                  STRINGV(result_ptr));
                        return (LlvmValue){
                            .ok         = true,
                            .type_index = expr->type_index,
                            .value      = loaded,
                        };
                    }
                    return (LlvmValue){
                        .ok         = true,
                        .type_index = expr->type_index,
                        .value      = s(""),
                    };
                }

                const HirExpr* collection_expr =
                    loop->iterable_expr_index < array_count(ctx->hir->exprs)
                        ? &ctx->hir->exprs[loop->iterable_expr_index]
                        : NULL;
                if (collection_expr == NULL) {
                    return (LlvmValue){0};
                }

                u32          iterable_type = collection_expr->type_index;
                SemaTypeKind iterable_kind =
                    llvm_type_kind(ctx->sema, iterable_type);
                if (iterable_kind != STK_Array && iterable_kind != STK_Slice &&
                    iterable_kind != STK_String &&
                    iterable_kind != STK_DynamicArray) {
                    return (LlvmValue){0};
                }

                string data_ptr = {0};
                string count    = {0};
                if (iterable_kind == STK_Array) {
                    data_ptr            = llvm_temp(ctx);
                    count               = llvm_temp(ctx);
                    LlvmValue array_ptr = llvm_address_of_expr(
                        ctx, function, loop->iterable_expr_index);
                    if (!array_ptr.ok) {
                        return (LlvmValue){0};
                    }

                    u32 item_type =
                        llvm_collection_item_type(ctx->sema, iterable_type);
                    if (item_type == sema_no_type()) {
                        return (LlvmValue){0};
                    }
                    string array_type = llvm_type_string(ctx, iterable_type);
                    sb_format(ctx->sb,
                              "  " STRINGP " = getelementptr inbounds " STRINGP
                              ", ptr " STRINGP ", i64 0, i64 0\n",
                              STRINGV(data_ptr),
                              STRINGV(array_type),
                              STRINGV(array_ptr.value));
                    sb_format(ctx->sb,
                              "  " STRINGP " = add i64 0, %u\n",
                              STRINGV(count),
                              llvm_array_count(ctx->sema, iterable_type));
                } else {
                    LlvmValue iterable = llvm_emit_expr(
                        ctx, function, loop->iterable_expr_index);
                    if (!iterable.ok) {
                        return (LlvmValue){0};
                    }
                    data_ptr = llvm_temp(ctx);
                    count    = llvm_temp(ctx);
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
                        string loaded_data =
                            llvm_dynamic_array_load_header_field(
                                ctx, iterable.value, 0, s("ptr"));
                        string loaded_count =
                            llvm_dynamic_array_load_header_field(
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
                        string slice_type =
                            llvm_type_string(ctx, iterable.type_index);
                        sb_format(ctx->sb,
                                  "  " STRINGP " = extractvalue " STRINGP
                                  " " STRINGP ", 0\n",
                                  STRINGV(data_ptr),
                                  STRINGV(slice_type),
                                  STRINGV(iterable.value));
                        sb_format(ctx->sb,
                                  "  " STRINGP " = extractvalue " STRINGP
                                  " " STRINGP ", 1\n",
                                  STRINGV(count),
                                  STRINGV(slice_type),
                                  STRINGV(iterable.value));
                    }
                }

                string result_ptr = {0};
                bool   has_result =
                    !llvm_type_is_void(ctx->sema, expr->type_index);
                if (has_result) {
                    string result_type =
                        llvm_type_string(ctx, expr->type_index);
                    result_ptr = llvm_temp(ctx);
                    sb_format(ctx->sb,
                              "  " STRINGP " = alloca " STRINGP ", align 4\n",
                              STRINGV(result_ptr),
                              STRINGV(result_type));
                    LlvmValue default_value =
                        llvm_default_value(ctx, expr->type_index);
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
                LlvmLocalSlot* index_slot        = NULL;
                LlvmLocalSlot  hidden_index_slot = {0};
                bool hidden_index = loop->index_local_index == U32_MAX;
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
                string else_label = loop->else_block_index != U32_MAX
                                        ? llvm_label(ctx, "for.in.else")
                                        : (string){0};
                string end_label  = llvm_label(ctx, "for.in.end");
                string false_label =
                    loop->else_block_index != U32_MAX ? else_label : end_label;
                sb_format(
                    ctx->sb, "  br label %%" STRINGP "\n", STRINGV(cond_label));

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
                    hidden_index
                        ? s("i64")
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
                          STRINGV(false_label));

                sb_format(ctx->sb, STRINGP ":\n", STRINGV(body_label));
                u32 value_type =
                    llvm_collection_item_type(ctx->sema, iterable_type);
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
                    LlvmValue item_value = {
                        .ok         = true,
                        .type_index = item_type,
                        .value      = item_ptr,
                    };
                    llvm_store_local_slot(ctx, item_slot, item_value);
                }
                string old_break                = ctx->break_label;
                string old_continue             = ctx->continue_label;
                string old_break_value_ptr      = ctx->break_value_ptr;
                u32    old_break_value_type     = ctx->break_value_type;
                u32    old_break_defer_count    = ctx->break_defer_count;
                u32    old_continue_defer_count = ctx->continue_defer_count;
                bool   old_break_emitted        = ctx->emitted_break;
                u32    loop_defer_base = array_count(ctx->defer_block_indices);
                ctx->break_label       = end_label;
                ctx->continue_label    = cond_label;
                ctx->break_value_ptr   = result_ptr;
                ctx->break_value_type  = expr->type_index;
                ctx->break_defer_count = loop_defer_base;
                ctx->continue_defer_count = loop_defer_base;
                ctx->emitted_break        = false;
                llvm_push_control_target(
                    ctx,
                    (LlvmControlTarget){
                        .symbol_handle        = loop->label_symbol,
                        .break_label          = end_label,
                        .continue_label       = cond_label,
                        .break_value_ptr      = result_ptr,
                        .break_value_type     = expr->type_index,
                        .break_defer_count    = loop_defer_base,
                        .continue_defer_count = loop_defer_base,
                    });
                if (!llvm_emit_effect_block(
                        ctx, function, loop->body_block_index)) {
                    llvm_pop_control_target(ctx, loop->label_symbol);
                    ctx->break_label          = old_break;
                    ctx->continue_label       = old_continue;
                    ctx->break_value_ptr      = old_break_value_ptr;
                    ctx->break_value_type     = old_break_value_type;
                    ctx->break_defer_count    = old_break_defer_count;
                    ctx->continue_defer_count = old_continue_defer_count;
                    ctx->emitted_break        = old_break_emitted;
                    return (LlvmValue){0};
                }
                llvm_pop_control_target(ctx, loop->label_symbol);

                if (!ctx->block_terminated) {
                    llvm_debug_emit_marker(
                        ctx,
                        llvm_debug_for_header_line(ctx, loop, expr),
                        expr->source_path);
                    if (!hidden_index) {
                        index_slot =
                            llvm_find_local_slot(ctx, loop->index_local_index);
                        if (index_slot == NULL) {
                            return (LlvmValue){0};
                        }
                    }
                    LlvmValue current = {0};
                    if (hidden_index) {
                        string loaded = llvm_temp(ctx);
                        sb_format(ctx->sb,
                                  "  " STRINGP " = load i64, ptr " STRINGP "\n",
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
                    string next = llvm_temp(ctx);
                    string type =
                        hidden_index
                            ? s("i64")
                            : llvm_type_string(ctx, current.type_index);
                    sb_format(ctx->sb,
                              "  " STRINGP " = add " STRINGP " " STRINGP
                              ", 1\n",
                              STRINGV(next),
                              STRINGV(type),
                              STRINGV(current.value));
                    if (hidden_index) {
                        sb_format(ctx->sb,
                                  "  store i64 " STRINGP ", ptr " STRINGP "\n",
                                  STRINGV(next),
                                  STRINGV(index_slot->ptr));
                    } else {
                        llvm_store_local_slot(
                            ctx,
                            index_slot,
                            (LlvmValue){
                                .ok         = true,
                                .type_index = current.type_index,
                                .value      = next,
                            });
                    }
                    sb_format(ctx->sb,
                              "  br label %%" STRINGP "\n",
                              STRINGV(cond_label));
                }
                if (loop->else_block_index != U32_MAX) {
                    sb_format(ctx->sb, STRINGP ":\n", STRINGV(else_label));
                    if (!llvm_emit_effect_block(
                            ctx, function, loop->else_block_index)) {
                        ctx->break_label          = old_break;
                        ctx->continue_label       = old_continue;
                        ctx->break_value_ptr      = old_break_value_ptr;
                        ctx->break_value_type     = old_break_value_type;
                        ctx->break_defer_count    = old_break_defer_count;
                        ctx->continue_defer_count = old_continue_defer_count;
                        ctx->emitted_break        = old_break_emitted;
                        return (LlvmValue){0};
                    }
                    if (!ctx->block_terminated) {
                        sb_format(ctx->sb,
                                  "  br label %%" STRINGP "\n",
                                  STRINGV(end_label));
                    }
                }
                ctx->break_label          = old_break;
                ctx->continue_label       = old_continue;
                ctx->break_value_ptr      = old_break_value_ptr;
                ctx->break_value_type     = old_break_value_type;
                ctx->break_defer_count    = old_break_defer_count;
                ctx->continue_defer_count = old_continue_defer_count;
                ctx->emitted_break        = old_break_emitted;
                ctx->block_terminated     = false;
                llvm_debug_emit_marker(
                    ctx,
                    llvm_debug_for_header_line(ctx, loop, expr),
                    expr->source_path);
                sb_format(ctx->sb, STRINGP ":\n", STRINGV(end_label));
                if (has_result) {
                    string result_type =
                        llvm_type_string(ctx, expr->type_index);
                    string loaded = llvm_temp(ctx);
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
                return (LlvmValue){
                    .ok         = true,
                    .type_index = expr->type_index,
                    .value      = s(""),
                };
            }

            if (!llvm_type_is_void(ctx->sema, expr->type_index) &&
                (loop->else_block_index != U32_MAX ||
                 loop->condition_expr_index == U32_MAX)) {
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
                string else_label   = llvm_label(ctx, "for.else");
                string end_label    = llvm_label(ctx, "for.end");

                string result_type  = llvm_type_string(ctx, expr->type_index);
                string result_ptr   = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = alloca " STRINGP ", align 4\n",
                          STRINGV(result_ptr),
                          STRINGV(result_type));
                LlvmValue default_value =
                    llvm_default_value(ctx, expr->type_index);
                if (!default_value.ok) {
                    return (LlvmValue){0};
                }
                sb_format(ctx->sb,
                          "  store " STRINGP " " STRINGP ", ptr " STRINGP
                          ", align 4\n",
                          STRINGV(result_type),
                          STRINGV(default_value.value),
                          STRINGV(result_ptr));

                sb_format(
                    ctx->sb, "  br label %%" STRINGP "\n", STRINGV(cond_label));

                sb_format(ctx->sb, STRINGP ":\n", STRINGV(cond_label));
                if (loop->condition_expr_index != U32_MAX) {
                    LlvmValue condition = llvm_emit_expr(
                        ctx, function, loop->condition_expr_index);
                    if (!condition.ok) {
                        return (LlvmValue){0};
                    }
                    condition = llvm_coerce_value_to_type(
                        ctx, condition, llvm_builtin_type(ctx->sema, STK_Bool));
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
                string old_break                = ctx->break_label;
                string old_continue             = ctx->continue_label;
                string old_break_value_ptr      = ctx->break_value_ptr;
                u32    old_break_value_type     = ctx->break_value_type;
                u32    old_break_defer_count    = ctx->break_defer_count;
                u32    old_continue_defer_count = ctx->continue_defer_count;
                bool   old_break_emitted        = ctx->emitted_break;
                u32    loop_defer_base = array_count(ctx->defer_block_indices);
                ctx->break_label       = end_label;
                ctx->continue_label =
                    loop->kind == HIR_FOR_CStyle ? update_label : cond_label;
                ctx->break_value_ptr      = result_ptr;
                ctx->break_value_type     = expr->type_index;
                ctx->break_defer_count    = loop_defer_base;
                ctx->continue_defer_count = loop_defer_base;
                ctx->emitted_break        = false;
                llvm_push_control_target(
                    ctx,
                    (LlvmControlTarget){
                        .symbol_handle        = loop->label_symbol,
                        .break_label          = end_label,
                        .continue_label       = ctx->continue_label,
                        .break_value_ptr      = result_ptr,
                        .break_value_type     = expr->type_index,
                        .break_defer_count    = loop_defer_base,
                        .continue_defer_count = loop_defer_base,
                    });

                if (!llvm_emit_effect_block(
                        ctx, function, loop->body_block_index)) {
                    llvm_pop_control_target(ctx, loop->label_symbol);
                    ctx->break_label          = old_break;
                    ctx->continue_label       = old_continue;
                    ctx->break_value_ptr      = old_break_value_ptr;
                    ctx->break_value_type     = old_break_value_type;
                    ctx->break_defer_count    = old_break_defer_count;
                    ctx->continue_defer_count = old_continue_defer_count;
                    ctx->emitted_break        = old_break_emitted;
                    return (LlvmValue){0};
                }

                if (!ctx->block_terminated) {
                    string next_label = loop->kind == HIR_FOR_CStyle
                                            ? update_label
                                            : cond_label;
                    sb_format(ctx->sb,
                              "  br label %%" STRINGP "\n",
                              STRINGV(next_label));
                }

                if (loop->kind == HIR_FOR_CStyle) {
                    sb_format(ctx->sb, STRINGP ":\n", STRINGV(update_label));
                    if (!llvm_emit_effect_stmt_indices(
                            ctx,
                            function,
                            ctx->hir->for_update_stmts,
                            loop->first_update_stmt,
                            loop->update_stmt_count)) {
                        llvm_pop_control_target(ctx, loop->label_symbol);
                        ctx->break_label          = old_break;
                        ctx->continue_label       = old_continue;
                        ctx->break_value_ptr      = old_break_value_ptr;
                        ctx->break_value_type     = old_break_value_type;
                        ctx->break_defer_count    = old_break_defer_count;
                        ctx->continue_defer_count = old_continue_defer_count;
                        ctx->emitted_break        = old_break_emitted;
                        return (LlvmValue){0};
                    }
                    sb_format(ctx->sb,
                              "  br label %%" STRINGP "\n",
                              STRINGV(cond_label));
                }

                bool loop_emitted_break =
                    ctx->emitted_break ||
                    llvm_control_target_emitted_break(ctx, loop->label_symbol);
                if (loop->else_block_index != U32_MAX) {
                    sb_format(ctx->sb, STRINGP ":\n", STRINGV(else_label));
                    ctx->block_terminated = false;
                    if (!llvm_emit_effect_block(
                            ctx, function, loop->else_block_index)) {
                        llvm_pop_control_target(ctx, loop->label_symbol);
                        ctx->break_label          = old_break;
                        ctx->continue_label       = old_continue;
                        ctx->break_value_ptr      = old_break_value_ptr;
                        ctx->break_value_type     = old_break_value_type;
                        ctx->break_defer_count    = old_break_defer_count;
                        ctx->continue_defer_count = old_continue_defer_count;
                        ctx->emitted_break        = old_break_emitted;
                        return (LlvmValue){0};
                    }
                    loop_emitted_break =
                        loop_emitted_break || ctx->emitted_break ||
                        llvm_control_target_emitted_break(ctx,
                                                          loop->label_symbol);
                    if (!ctx->block_terminated) {
                        sb_format(ctx->sb,
                                  "  br label %%" STRINGP "\n",
                                  STRINGV(end_label));
                    }
                }

                llvm_pop_control_target(ctx, loop->label_symbol);
                ctx->break_label          = old_break;
                ctx->continue_label       = old_continue;
                ctx->break_value_ptr      = old_break_value_ptr;
                ctx->break_value_type     = old_break_value_type;
                ctx->break_defer_count    = old_break_defer_count;
                ctx->continue_defer_count = old_continue_defer_count;
                ctx->emitted_break        = old_break_emitted;

                bool can_reach_end = loop_emitted_break ||
                                     loop->condition_expr_index != U32_MAX ||
                                     loop->else_block_index != U32_MAX;
                if (!can_reach_end) {
                    ctx->block_terminated = true;
                    return (LlvmValue){
                        .ok         = true,
                        .type_index = expr->type_index,
                        .value      = s(""),
                    };
                }

                sb_format(ctx->sb, STRINGP ":\n", STRINGV(end_label));
                ctx->block_terminated = false;
                string loaded         = llvm_temp(ctx);
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

            u32 old_for_scope = ctx->debug_scope_id;
            if (loop->kind == HIR_FOR_CStyle) {
                old_for_scope = llvm_debug_enter_scope(ctx,
                                                       function,
                                                       loop->scope_index,
                                                       expr->source_line,
                                                       expr->source_path);
                if (!llvm_emit_effect_stmt_indices(ctx,
                                                   function,
                                                   ctx->hir->for_init_stmts,
                                                   loop->first_init_stmt,
                                                   loop->init_stmt_count)) {
                    ctx->debug_scope_id = old_for_scope;
                    return (LlvmValue){0};
                }
            }

            string cond_label   = llvm_label(ctx, "for.cond");
            string body_label   = llvm_label(ctx, "for.body");
            string update_label = llvm_label(ctx, "for.update");
            string else_label   = loop->else_block_index != U32_MAX
                                      ? llvm_label(ctx, "for.else")
                                      : (string){0};
            string end_label    = llvm_label(ctx, "for.end");
            sb_format(
                ctx->sb, "  br label %%" STRINGP "\n", STRINGV(cond_label));

            sb_format(ctx->sb, STRINGP ":\n", STRINGV(cond_label));
            if (loop->condition_expr_index != U32_MAX) {
                LlvmValue condition =
                    llvm_emit_expr(ctx, function, loop->condition_expr_index);
                if (!condition.ok) {
                    ctx->debug_scope_id = old_for_scope;
                    return (LlvmValue){0};
                }
                condition = llvm_coerce_value_to_type(
                    ctx, condition, llvm_builtin_type(ctx->sema, STK_Bool));
                if (!condition.ok) {
                    ctx->debug_scope_id = old_for_scope;
                    return (LlvmValue){0};
                }
                sb_format(ctx->sb,
                          "  br i1 " STRINGP ", label %%" STRINGP
                          ", label %%" STRINGP "\n",
                          STRINGV(condition.value),
                          STRINGV(body_label),
                          STRINGV(loop->else_block_index != U32_MAX
                                      ? else_label
                                      : end_label));
            } else {
                sb_format(
                    ctx->sb, "  br label %%" STRINGP "\n", STRINGV(body_label));
            }

            sb_format(ctx->sb, STRINGP ":\n", STRINGV(body_label));
            string old_break                = ctx->break_label;
            string old_continue             = ctx->continue_label;
            u32    old_break_defer_count    = ctx->break_defer_count;
            u32    old_continue_defer_count = ctx->continue_defer_count;
            bool   old_break_emitted        = ctx->emitted_break;
            u32    loop_defer_base = array_count(ctx->defer_block_indices);
            ctx->break_label       = end_label;
            ctx->continue_label =
                loop->kind == HIR_FOR_CStyle ? update_label : cond_label;
            ctx->break_defer_count    = loop_defer_base;
            ctx->continue_defer_count = loop_defer_base;
            ctx->emitted_break        = false;
            llvm_push_control_target(
                ctx,
                (LlvmControlTarget){
                    .symbol_handle        = loop->label_symbol,
                    .break_label          = end_label,
                    .continue_label       = ctx->continue_label,
                    .break_value_ptr      = (string){0},
                    .break_value_type     = sema_no_type(),
                    .break_defer_count    = loop_defer_base,
                    .continue_defer_count = loop_defer_base,
                });
            if (!llvm_emit_effect_block(
                    ctx, function, loop->body_block_index)) {
                llvm_pop_control_target(ctx, loop->label_symbol);
                ctx->break_label          = old_break;
                ctx->continue_label       = old_continue;
                ctx->break_defer_count    = old_break_defer_count;
                ctx->continue_defer_count = old_continue_defer_count;
                ctx->emitted_break        = old_break_emitted;
                ctx->debug_scope_id       = old_for_scope;
                return (LlvmValue){0};
            }
            bool loop_emitted_break =
                ctx->emitted_break ||
                llvm_control_target_emitted_break(ctx, loop->label_symbol);
            string next_label =
                loop->kind == HIR_FOR_CStyle ? update_label : cond_label;
            if (!ctx->block_terminated) {
                sb_format(
                    ctx->sb, "  br label %%" STRINGP "\n", STRINGV(next_label));
            }

            if (loop->kind == HIR_FOR_CStyle) {
                sb_format(ctx->sb, STRINGP ":\n", STRINGV(update_label));
                if (!llvm_emit_effect_stmt_indices(ctx,
                                                   function,
                                                   ctx->hir->for_update_stmts,
                                                   loop->first_update_stmt,
                                                   loop->update_stmt_count)) {
                    llvm_pop_control_target(ctx, loop->label_symbol);
                    ctx->break_label          = old_break;
                    ctx->continue_label       = old_continue;
                    ctx->break_defer_count    = old_break_defer_count;
                    ctx->continue_defer_count = old_continue_defer_count;
                    ctx->emitted_break        = old_break_emitted;
                    ctx->debug_scope_id       = old_for_scope;
                    return (LlvmValue){0};
                }
                sb_format(
                    ctx->sb, "  br label %%" STRINGP "\n", STRINGV(cond_label));
            }

            if (loop->else_block_index != U32_MAX) {
                sb_format(ctx->sb, STRINGP ":\n", STRINGV(else_label));
                ctx->block_terminated = false;
                if (!llvm_emit_effect_block(
                        ctx, function, loop->else_block_index)) {
                    llvm_pop_control_target(ctx, loop->label_symbol);
                    ctx->break_label          = old_break;
                    ctx->continue_label       = old_continue;
                    ctx->break_defer_count    = old_break_defer_count;
                    ctx->continue_defer_count = old_continue_defer_count;
                    ctx->emitted_break        = old_break_emitted;
                    ctx->debug_scope_id       = old_for_scope;
                    return (LlvmValue){0};
                }
                loop_emitted_break =
                    loop_emitted_break || ctx->emitted_break ||
                    llvm_control_target_emitted_break(ctx, loop->label_symbol);
                if (!ctx->block_terminated) {
                    sb_format(ctx->sb,
                              "  br label %%" STRINGP "\n",
                              STRINGV(end_label));
                }
            }

            llvm_pop_control_target(ctx, loop->label_symbol);
            ctx->break_label          = old_break;
            ctx->continue_label       = old_continue;
            ctx->break_defer_count    = old_break_defer_count;
            ctx->continue_defer_count = old_continue_defer_count;
            ctx->emitted_break        = old_break_emitted;

            bool can_reach_end        = loop_emitted_break ||
                                        loop->condition_expr_index != U32_MAX ||
                                        loop->else_block_index != U32_MAX;
            if (can_reach_end) {
                sb_format(ctx->sb, STRINGP ":\n", STRINGV(end_label));
                ctx->block_terminated = false;
            }
            ctx->debug_scope_id = old_for_scope;
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
                          " = insertvalue { ptr, i64 } poison, ptr " STRINGP
                          ", 0\n",
                          STRINGV(slice0),
                          STRINGV(operand.value));
                string slice1 = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = insertvalue { ptr, i64 } " STRINGP
                          ", i64 " STRINGP ", 1\n",
                          STRINGV(slice1),
                          STRINGV(slice0),
                          STRINGV(count_value));
                return (LlvmValue){
                    .ok         = true,
                    .type_index = expr->type_index,
                    .value      = slice1,
                };
            }

            if (llvm_type_kind(ctx->sema, operand.type_index) ==
                    STK_DynamicArray &&
                llvm_type_kind(ctx->sema, expr->type_index) == STK_String) {
                string result_ptr = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = alloca { ptr, i64 }\n",
                          STRINGV(result_ptr));

                string is_null = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = icmp eq ptr " STRINGP ", null\n",
                          STRINGV(is_null),
                          STRINGV(operand.value));
                string empty_label = llvm_label(ctx, "dynarray.string.empty");
                string load_label  = llvm_label(ctx, "dynarray.string.load");
                string done_label  = llvm_label(ctx, "dynarray.string.done");
                sb_format(ctx->sb,
                          "  br i1 " STRINGP ", label %%" STRINGP
                          ", label %%" STRINGP "\n",
                          STRINGV(is_null),
                          STRINGV(empty_label),
                          STRINGV(load_label));

                sb_format(ctx->sb, STRINGP ":\n", STRINGV(empty_label));
                sb_format(
                    ctx->sb,
                    "  store { ptr, i64 } { ptr null, i64 0 }, ptr " STRINGP
                    "\n"
                    "  br label %%" STRINGP "\n",
                    STRINGV(result_ptr),
                    STRINGV(done_label));

                sb_format(ctx->sb, STRINGP ":\n", STRINGV(load_label));
                string data = llvm_dynamic_array_load_header_field(
                    ctx, operand.value, 0, s("ptr"));
                string count = llvm_dynamic_array_load_header_field(
                    ctx, operand.value, 1, s("i64"));
                string string0 = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP
                          " = insertvalue { ptr, i64 } poison, ptr " STRINGP
                          ", 0\n",
                          STRINGV(string0),
                          STRINGV(data));
                string string1 = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = insertvalue { ptr, i64 } " STRINGP
                          ", i64 " STRINGP ", 1\n",
                          STRINGV(string1),
                          STRINGV(string0),
                          STRINGV(count));
                sb_format(ctx->sb,
                          "  store { ptr, i64 } " STRINGP ", ptr " STRINGP "\n"
                          "  br label %%" STRINGP "\n",
                          STRINGV(string1),
                          STRINGV(result_ptr),
                          STRINGV(done_label));

                sb_format(ctx->sb, STRINGP ":\n", STRINGV(done_label));
                string result = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = load { ptr, i64 }, ptr " STRINGP
                          "\n",
                          STRINGV(result),
                          STRINGV(result_ptr));
                ctx->block_terminated = false;
                return (LlvmValue){
                    .ok         = true,
                    .type_index = expr->type_index,
                    .value      = result,
                };
            }

            if (llvm_type_kind(ctx->sema, operand.type_index) == STK_Enum &&
                llvm_integer_bits(ctx->sema, expr->type_index) > 0) {
                string enum_type = llvm_type_string(ctx, operand.type_index);
                string tag       = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = extractvalue " STRINGP " " STRINGP
                          ", 0\n",
                          STRINGV(tag),
                          STRINGV(enum_type),
                          STRINGV(operand.value));

                u32 target_bits =
                    llvm_integer_bits(ctx->sema, expr->type_index);
                if (target_bits == 64) {
                    return (LlvmValue){
                        .ok         = true,
                        .type_index = expr->type_index,
                        .value      = tag,
                    };
                }

                string temp  = llvm_temp(ctx);
                string instr = target_bits < 64 ? s("trunc") : s("sext");
                sb_format(ctx->sb,
                          "  " STRINGP " = " STRINGP " i64 " STRINGP
                          " to " STRINGP "\n",
                          STRINGV(temp),
                          STRINGV(instr),
                          STRINGV(tag),
                          STRINGV(target_type));
                return (LlvmValue){
                    .ok         = true,
                    .type_index = expr->type_index,
                    .value      = temp,
                };
            }

            if (llvm_integer_bits(ctx->sema, operand.type_index) > 0 &&
                llvm_type_kind(ctx->sema, expr->type_index) == STK_Enum) {
                u32    i64_type = llvm_builtin_type(ctx->sema, STK_I64);
                string tag      = operand.value;
                string instr =
                    llvm_cast_instruction(ctx, operand.type_index, i64_type);
                if (instr.count > 0) {
                    tag = llvm_temp(ctx);
                    sb_format(ctx->sb,
                              "  " STRINGP " = " STRINGP " " STRINGP " " STRINGP
                              " to i64\n",
                              STRINGV(tag),
                              STRINGV(instr),
                              STRINGV(source_type),
                              STRINGV(operand.value));
                }

                string payload_type =
                    string_format(ctx->arena,
                                  "i%u",
                                  llvm_enum_storage_payload_bits(
                                      ctx->sema, expr->type_index));
                string with_tag = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = insertvalue " STRINGP
                          " poison, i64 " STRINGP ", 0\n",
                          STRINGV(with_tag),
                          STRINGV(target_type),
                          STRINGV(tag));

                string with_payload = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = insertvalue " STRINGP " " STRINGP
                          ", " STRINGP " 0, 1\n",
                          STRINGV(with_payload),
                          STRINGV(target_type),
                          STRINGV(with_tag),
                          STRINGV(payload_type));
                return (LlvmValue){
                    .ok         = true,
                    .type_index = expr->type_index,
                    .value      = with_payload,
                };
            }

            string instr = llvm_cast_instruction(
                ctx, operand.type_index, expr->type_index);
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
            if (llvm_is_arena_constructor_call(ctx, expr)) {
                string arena_type = llvm_type_string(ctx, expr->type_index);
                string slot       = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = alloca " STRINGP "\n",
                          STRINGV(slot),
                          STRINGV(arena_type));

                Array(LlvmValue) args = NULL;
                for (u32 i = 0; i < expr->arg_count; ++i) {
                    const HirCallArg* arg =
                        &ctx->hir->call_args[expr->first_arg + i];
                    LlvmValue value =
                        llvm_emit_expr(ctx, function, arg->expr_index);
                    if (!value.ok) {
                        array_free(args);
                        return (LlvmValue){0};
                    }
                    array_push(args,
                               llvm_coerce_value_to_type(
                                   ctx,
                                   value,
                                   llvm_builtin_type(ctx->sema, STK_Usize)));
                    if (!args[array_count(args) - 1].ok) {
                        array_free(args);
                        return (LlvmValue){0};
                    }
                }
                while (array_count(args) < 2) {
                    array_push(args,
                               (LlvmValue){
                                   .ok = true,
                                   .type_index =
                                       llvm_builtin_type(ctx->sema, STK_Usize),
                                   .value = s("0"),
                               });
                }

                string usize_type = llvm_type_string(
                    ctx, llvm_builtin_type(ctx->sema, STK_Usize));
                string source_path =
                    llvm_builtin_module_file_global_name_string(ctx->hir,
                                                                ctx->arena);
                sb_format(ctx->sb,
                          "  call void @nrt_arena_init(ptr " STRINGP
                          ", " STRINGP " " STRINGP ", " STRINGP " " STRINGP
                          ", ptr " STRINGP ", i32 %u"
                          ")\n",
                          STRINGV(slot),
                          STRINGV(usize_type),
                          STRINGV(args[0].value),
                          STRINGV(usize_type),
                          STRINGV(args[1].value),
                          STRINGV(source_path),
                          expr->source_line);
                array_free(args);

                string value = llvm_temp(ctx);
                sb_format(ctx->sb,
                          "  " STRINGP " = load " STRINGP ", ptr " STRINGP "\n",
                          STRINGV(value),
                          STRINGV(arena_type),
                          STRINGV(slot));
                return (LlvmValue){
                    .ok         = true,
                    .type_index = expr->type_index,
                    .value      = value,
                };
            }

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
                if (string_eq_cstr(dynarray_method, "reserve_to")) {
                    return llvm_emit_dynamic_array_reserve(
                        ctx, function, expr, dynarray_receiver, false);
                }
                if (string_eq_cstr(dynarray_method, "reserve_extra")) {
                    return llvm_emit_dynamic_array_reserve(
                        ctx, function, expr, dynarray_receiver, true);
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
                if (string_eq_cstr(dynarray_method, "resize_to")) {
                    return llvm_emit_dynamic_array_resize(
                        ctx, function, expr, dynarray_receiver, true, false);
                }
                if (string_eq_cstr(dynarray_method, "resize_undefined_to")) {
                    return llvm_emit_dynamic_array_resize(
                        ctx, function, expr, dynarray_receiver, false, false);
                }
                if (string_eq_cstr(dynarray_method, "extend")) {
                    return llvm_emit_dynamic_array_resize(
                        ctx, function, expr, dynarray_receiver, true, true);
                }
                if (string_eq_cstr(dynarray_method, "extend_undefined")) {
                    return llvm_emit_dynamic_array_resize(
                        ctx, function, expr, dynarray_receiver, false, true);
                }
                return (LlvmValue){0};
            }

            u32    box_receiver = U32_MAX;
            string box_method   = {0};
            if (llvm_box_callee_method(
                    ctx, expr->callee_expr_index, &box_receiver, &box_method)) {
                if (string_eq_cstr(box_method, "free")) {
                    return llvm_emit_box_free(
                        ctx, function, expr, box_receiver);
                }
                return (LlvmValue){0};
            }

            string callee = {0};
            if (!llvm_generic_callee_name_for_call(ctx, expr, &callee) &&
                !llvm_callee_name(
                    ctx, function, expr->callee_expr_index, &callee)) {
                return (LlvmValue){0};
            }

            Array(LlvmValue) args                    = NULL;
            u32                callee_function_index = U32_MAX;
            const Hir*         callee_hir            = ctx->hir;
            const Lexer*       callee_lexer          = ctx->lexer;
            const Sema*        callee_sema           = ctx->sema;
            const HirFunction* callee_function       = NULL;
            if (llvm_callee_function_index(
                    ctx, expr->callee_expr_index, &callee_function_index)) {
                callee_function = &callee_hir->functions[callee_function_index];
            } else if (expr->callee_expr_index < array_count(ctx->hir->exprs)) {
                const HirExpr* callee_expr =
                    &ctx->hir->exprs[expr->callee_expr_index];
                if (callee_expr->kind == HIR_EXPR_LocalRef &&
                    callee_expr->ref_kind == HIR_REF_Binding) {
                    const HirImport* import =
                        llvm_binding_import(ctx->hir, callee_expr->ref_index);
                    if (import != NULL &&
                        llvm_import_source_function(ctx->sema,
                                                    import,
                                                    &callee_hir,
                                                    &callee_function_index)) {
                        const ModuleInfo* module =
                            &ctx->sema->program->modules[import->module_index];
                        callee_lexer = &module->front_end.lexer;
                        callee_sema  = &module->front_end.sema;
                        callee_function =
                            &callee_hir->functions[callee_function_index];
                    }
                }
            }
            for (u32 i = 0; i < expr->arg_count; ++i) {
                const HirCallArg* arg =
                    &ctx->hir->call_args[expr->first_arg + i];
                LlvmValue value =
                    llvm_emit_expr(ctx, function, arg->expr_index);
                if (!value.ok) {
                    array_free(args);
                    return (LlvmValue){0};
                }
                array_push(args, value);
            }
            if (callee_function != NULL) {
                LlvmFunctionContext  default_ctx      = *ctx;
                LlvmFunctionContext* default_emit_ctx = ctx;
                if (callee_hir != ctx->hir) {
                    default_ctx.hir             = callee_hir;
                    default_ctx.lexer           = callee_lexer;
                    default_ctx.sema            = callee_sema;
                    default_ctx.locals          = NULL;
                    default_ctx.slots           = NULL;
                    default_ctx.assigned_locals = NULL;
                    default_emit_ctx            = &default_ctx;
                }
                Array(LlvmValue) default_values = NULL;
                for (u32 i = 0; i < callee_function->param_count; ++i) {
                    const HirParam* param =
                        &callee_hir->params[callee_function->first_param + i];
                    LlvmValue context_value = {0};
                    if (param->default_expr_index != U32_MAX) {
                        default_emit_ctx->macro_source_path = expr->source_path;
                        default_emit_ctx->macro_source_line = expr->source_line;
                        default_emit_ctx->macro_source_hir  = ctx->hir;
                        context_value =
                            llvm_emit_expr(default_emit_ctx,
                                           function,
                                           param->default_expr_index);
                        default_emit_ctx->macro_source_path = (string){0};
                        default_emit_ctx->macro_source_line = 0;
                        default_emit_ctx->macro_source_hir  = NULL;
                        if (!context_value.ok) {
                            array_free(default_values);
                            array_free(args);
                            if (callee_hir != ctx->hir) {
                                array_free(default_ctx.locals);
                                array_free(default_ctx.slots);
                                array_free(default_ctx.assigned_locals);
                            }
                            return (LlvmValue){0};
                        }
                    } else if (i < array_count(args)) {
                        context_value = args[i];
                    }
                    array_push(default_values, context_value);
                    if (context_value.ok && param->local_index != U32_MAX) {
                        llvm_set_local_value(default_emit_ctx,
                                             param->local_index,
                                             context_value);
                    }
                }
                ctx->next_temp  = default_emit_ctx->next_temp;
                ctx->next_label = default_emit_ctx->next_label;

                for (u32 i = expr->arg_count; i < callee_function->param_count;
                     ++i) {
                    if (i >= array_count(default_values) ||
                        !default_values[i].ok) {
                        break;
                    }
                    array_push(args, default_values[i]);
                }
                array_free(default_values);
                if (callee_hir != ctx->hir) {
                    array_free(default_ctx.locals);
                    array_free(default_ctx.slots);
                    array_free(default_ctx.assigned_locals);
                }
            }

            u32 callee_type = sema_no_type();
            if (callee_function != NULL && callee_hir == ctx->hir) {
                callee_type = callee_function->type_index;
            } else if (expr->callee_expr_index < array_count(ctx->hir->exprs)) {
                callee_type =
                    ctx->hir->exprs[expr->callee_expr_index].type_index;
            }
            u32 return_type_index =
                llvm_function_return_type(ctx->sema, callee_type);
            if (return_type_index == sema_no_type()) {
                return_type_index = expr->type_index;
            }

            for (u32 i = 0; i < array_count(args); ++i) {
                u32 param_type =
                    llvm_function_param_type(ctx->sema, callee_type, i);
                if (param_type == sema_no_type()) {
                    continue;
                }
                args[i] = llvm_coerce_value_to_type(ctx, args[i], param_type);
                if (!args[i].ok) {
                    array_free(args);
                    return (LlvmValue){0};
                }
            }

            string return_type = llvm_type_string(ctx, return_type_index);
            bool returns_void = llvm_type_is_void(ctx->sema, return_type_index);
            string temp       = returns_void ? (string){0} : llvm_temp(ctx);
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
                string    type  = llvm_type_string(ctx, value.type_index);
                sb_format(ctx->sb,
                          STRINGP " " STRINGP,
                          STRINGV(type),
                          STRINGV(value.value));
            }
            sb_append_cstr(ctx->sb, ")\n");
            for (u32 i = 0; i < array_count(args) && i < expr->arg_count; ++i) {
                u32 param_type =
                    llvm_function_param_type(ctx->sema, callee_type, i);
                if (param_type == sema_no_type()) {
                    continue;
                }
                const HirCallArg* arg =
                    &ctx->hir->call_args[expr->first_arg + i];
                if (!llvm_consume_box_expr(
                        ctx, arg->expr_index, param_type, U32_MAX)) {
                    array_free(args);
                    return (LlvmValue){0};
                }
            }
            array_free(args);
            return (LlvmValue){
                .ok         = true,
                .type_index = return_type_index,
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

    LlvmValue      value = {0};
    const HirExpr* expr  = stmt->expr_index < array_count(ctx->hir->exprs)
                               ? &ctx->hir->exprs[stmt->expr_index]
                               : NULL;
    if (expr != NULL && (expr->kind == HIR_EXPR_Unsupported ||
                         expr->kind == HIR_EXPR_DefaultValue)) {
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
    if (!llvm_consume_box_expr(
            ctx, stmt->expr_index, stmt->type_index, stmt->local_index)) {
        return false;
    }

    if (llvm_local_is_assigned(ctx, stmt->local_index) ||
        (ctx->debug != NULL &&
         (llvm_debug_type_is_record_like(ctx->sema, stmt->type_index) ||
          llvm_debug_type_is_array(ctx->sema, stmt->type_index))) ||
        llvm_type_kind(ctx->sema, stmt->type_index) == STK_DynamicArray ||
        llvm_type_is_box(ctx->sema, stmt->type_index)) {
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

    if (target->kind == HIR_EXPR_Index) {
        LlvmValue target_ptr =
            llvm_address_of_expr(ctx, function, target_expr_index);
        if (!target_ptr.ok) {
            return false;
        }

        if (target_ptr.type_index != sema_no_type()) {
            value =
                llvm_coerce_value_to_type(ctx, value, target_ptr.type_index);
            if (!value.ok) {
                return false;
            }
        }
        string value_type = llvm_type_string(ctx, value.type_index);
        sb_format(ctx->sb,
                  "  store " STRINGP " " STRINGP ", ptr " STRINGP "\n",
                  STRINGV(value_type),
                  STRINGV(value.value),
                  STRINGV(target_ptr.value));
        return true;
    }

    if (target->kind == HIR_EXPR_Unary && target->unary_op == HIR_UNARY_Deref) {
        LlvmValue pointer =
            llvm_emit_expr(ctx, function, target->operand_expr_index);
        if (!pointer.ok) {
            return false;
        }

        u32 pointee_type = llvm_pointee_type(ctx->sema, pointer.type_index);
        if (pointee_type != sema_no_type()) {
            value = llvm_coerce_value_to_type(ctx, value, pointee_type);
            if (!value.ok) {
                return false;
            }
        } else {
            pointee_type = target->type_index;
        }
        string type = llvm_type_string(ctx, pointee_type);
        sb_format(ctx->sb,
                  "  store " STRINGP " " STRINGP ", ptr " STRINGP "\n",
                  STRINGV(type),
                  STRINGV(value.value),
                  STRINGV(pointer.value));
        return true;
    }

    if (target->kind == HIR_EXPR_TupleField || target->kind == HIR_EXPR_Field) {
        LlvmValue field_ptr =
            llvm_address_of_expr(ctx, function, target_expr_index);
        if (field_ptr.ok) {
            if (field_ptr.type_index != sema_no_type()) {
                value =
                    llvm_coerce_value_to_type(ctx, value, field_ptr.type_index);
                if (!value.ok) {
                    return false;
                }
            }
            string value_type = llvm_type_string(ctx, value.type_index);
            sb_format(ctx->sb,
                      "  store " STRINGP " " STRINGP ", ptr " STRINGP "\n",
                      STRINGV(value_type),
                      STRINGV(value.value),
                      STRINGV(field_ptr.value));
            return true;
        }

        LlvmValue record =
            llvm_emit_expr(ctx, function, target->operand_expr_index);
        if (!record.ok) {
            return false;
        }

        if (llvm_type_kind(ctx->sema, record.type_index) == STK_Union) {
            LlvmValue union_value =
                llvm_cast_to_union_storage(ctx, value, record.type_index);
            if (!union_value.ok) {
                return false;
            }
            return llvm_emit_assign(
                ctx, function, target->operand_expr_index, union_value);
        }

        u32 field_index = llvm_field_index_for_value(ctx, target, record);
        if (field_index == U32_MAX) {
            return false;
        }

        string record_type = llvm_type_string(ctx, record.type_index);
        string value_type  = llvm_type_string(ctx, value.type_index);
        string updated     = llvm_temp(ctx);
        sb_format(ctx->sb,
                  "  " STRINGP " = insertvalue " STRINGP " " STRINGP
                  ", " STRINGP " " STRINGP ", %u\n",
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

        string name = llvm_value_name_string(
            ctx->hir, ctx->lexer, ctx->arena, binding->target_index);
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
    bool old_discard_expr_value = ctx->discard_expr_value;
    ctx->discard_expr_value     = false;
    LlvmValue value         = llvm_emit_expr(ctx, function, stmt->expr_index);
    ctx->discard_expr_value = old_discard_expr_value;
    if (!value.ok) {
        return false;
    }
    u32 excluded_local = U32_MAX;
    if (stmt->target_expr_index < array_count(ctx->hir->exprs)) {
        const HirExpr* target = &ctx->hir->exprs[stmt->target_expr_index];
        if (target->kind == HIR_EXPR_LocalRef &&
            target->ref_kind == HIR_REF_Local) {
            excluded_local = target->ref_index;
        }
    }
    u32 source_local = llvm_box_move_source_local(ctx, stmt->expr_index);
    if (excluded_local != U32_MAX && excluded_local != source_local &&
        llvm_type_is_box(ctx->sema, value.type_index)) {
        LlvmLocalSlot* target_slot =
            llvm_ensure_local_slot(ctx, excluded_local, value.type_index);
        if (!llvm_emit_box_free_slot(ctx, target_slot)) {
            return false;
        }
    }
    if (!llvm_emit_assign(ctx, function, stmt->target_expr_index, value)) {
        return false;
    }

    return llvm_consume_box_expr(
        ctx, stmt->expr_index, value.type_index, excluded_local);
}

internal u32 llvm_stmt_source_line(const LlvmFunctionContext* ctx,
                                   const HirStmt*             stmt)
{
    (void)ctx;
    return stmt != NULL ? stmt->source_line : 0;
}

internal u32 llvm_stmt_index(const Hir* hir, const HirStmt* stmt)
{
    if (hir == NULL || stmt == NULL || stmt < hir->stmts ||
        stmt >= hir->stmts + array_count(hir->stmts)) {
        return U32_MAX;
    }
    return (u32)(stmt - hir->stmts);
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
    condition = llvm_coerce_value_to_type(
        ctx, condition, llvm_builtin_type(ctx->sema, STK_Bool));
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
        string default_message = llvm_assert_default_message_global_name_string(
            ctx->hir, ctx->arena);
        message_value = string_format(ctx->arena,
                                      "{ ptr " STRINGP ", i64 16 }",
                                      STRINGV(default_message));
    }

    u32    stmt_index  = llvm_stmt_index(ctx->hir, stmt);
    string source_path = llvm_assert_source_path_global_name_string(
        ctx->hir, ctx->arena, stmt_index);
    string message_ptr = llvm_emit_string_value_pointer(ctx, message_value);
    sb_format(ctx->sb,
              "  call void @nerd_assert(i1 " STRINGP ", ptr " STRINGP
              ", i32 %u, ptr " STRINGP ")\n",
              STRINGV(condition.value),
              STRINGV(source_path),
              llvm_stmt_source_line(ctx, stmt),
              STRINGV(message_ptr));
    return true;
}

internal bool llvm_emit_destructure(LlvmFunctionContext* ctx,
                                    const HirFunction*   function,
                                    const HirStmt*       stmt)
{
    bool old_discard_expr_value = ctx->discard_expr_value;
    ctx->discard_expr_value     = false;
    LlvmValue value         = llvm_emit_expr(ctx, function, stmt->expr_index);
    ctx->discard_expr_value = old_discard_expr_value;
    if (!value.ok) {
        return false;
    }

    string tuple_type       = llvm_type_string(ctx, value.type_index);
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
                  "  " STRINGP " = extractvalue " STRINGP " " STRINGP ", %u\n",
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
        if (!llvm_emit_effect_stmt(
                ctx, function, &ctx->hir->stmts[stmt_index])) {
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

    const HirBlock* block     = &ctx->hir->blocks[block_index];
    u32             old_scope = ctx->debug_scope_id;
    if (ctx->debug != NULL && block->scope_index != U32_MAX &&
        block->scope_index != function->root_scope_index) {
        u32    line        = 0;
        string source_path = {0};
        llvm_debug_block_start(ctx->hir, block, &line, &source_path);
        old_scope = llvm_debug_enter_scope(
            ctx, function, block->scope_index, line, source_path);
    }
    u32 defer_base        = array_count(ctx->defer_block_indices);
    ctx->block_terminated = false;
    bool ok               = llvm_emit_effect_stmt_indices(
        ctx, function, block->stmt_indices, 0, block->stmt_count);
    if (!ok) {
        if (ctx->defer_block_indices != NULL) {
            __array_count(ctx->defer_block_indices) = defer_base;
        }
        ctx->debug_scope_id = old_scope;
        return false;
    }
    if (!ctx->block_terminated) {
        llvm_debug_emit_block_end_anchor(ctx, block);
    }
    if (!ctx->block_terminated &&
        !llvm_emit_defers_to(ctx, function, defer_base, true)) {
        ctx->debug_scope_id = old_scope;
        return false;
    }
    if (!ctx->block_terminated &&
        !llvm_emit_box_cleanup_for_scope(ctx, block->scope_index)) {
        ctx->debug_scope_id = old_scope;
        return false;
    }
    if (ctx->block_terminated && ctx->defer_block_indices != NULL) {
        __array_count(ctx->defer_block_indices) = defer_base;
    }
    ctx->debug_scope_id = old_scope;
    return true;
}

internal bool llvm_emit_effect_stmt(LlvmFunctionContext* ctx,
                                    const HirFunction*   function,
                                    const HirStmt*       stmt)
{
    llvm_debug_emit_marker(ctx,
                           stmt != NULL ? stmt->source_line : 0,
                           stmt != NULL ? stmt->source_path : (string){0});
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
        {
            const HirExpr* expr =
                stmt->expr_index < array_count(ctx->hir->exprs)
                    ? &ctx->hir->exprs[stmt->expr_index]
                    : NULL;
            bool old_discard_expr_value = ctx->discard_expr_value;
            if (expr != NULL && expr->kind == HIR_EXPR_On) {
                ctx->discard_expr_value = true;
            }
            if (expr != NULL && expr->kind == HIR_EXPR_Unsupported) {
                return true;
            }
            bool ok = llvm_emit_expr(ctx, function, stmt->expr_index).ok;
            ctx->discard_expr_value = old_discard_expr_value;
            if (ok && expr != NULL && expr->kind == HIR_EXPR_On) {
                ctx->block_terminated = false;
            }
            return ok;
        }
    case HIR_STMT_Assert:
        return llvm_emit_assert(ctx, function, stmt);
    case HIR_STMT_Defer:
        if (stmt->body_block_index == U32_MAX) {
            return false;
        }
        array_push(ctx->defer_block_indices, stmt->body_block_index);
        return true;
    case HIR_STMT_Break:
        {
            string             break_label       = ctx->break_label;
            string             break_value_ptr   = ctx->break_value_ptr;
            u32                break_value_type  = ctx->break_value_type;
            u32                break_defer_count = ctx->break_defer_count;
            LlvmControlTarget* target =
                llvm_find_control_target(ctx, stmt->symbol_handle);
            if (target != NULL) {
                target->emitted_break = true;
                break_label           = target->break_label;
                break_value_ptr       = target->break_value_ptr;
                break_value_type      = target->break_value_type;
                break_defer_count     = target->break_defer_count;
            }
            if (break_label.count == 0) {
                return false;
            }
            if (stmt->expr_index != U32_MAX && break_value_ptr.count > 0) {
                LlvmValue value =
                    llvm_emit_expr(ctx, function, stmt->expr_index);
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
            if (!llvm_emit_defers_to(ctx, function, break_defer_count, false)) {
                return false;
            }
            sb_format(
                ctx->sb, "  br label %%" STRINGP "\n", STRINGV(break_label));
            ctx->block_terminated = true;
            if (target == NULL) {
                ctx->emitted_break = true;
            }
            return true;
        }
    case HIR_STMT_Continue:
        {
            string             continue_label       = ctx->continue_label;
            u32                continue_defer_count = ctx->continue_defer_count;
            LlvmControlTarget* target =
                llvm_find_control_target(ctx, stmt->symbol_handle);
            if (target != NULL) {
                continue_label       = target->continue_label;
                continue_defer_count = target->continue_defer_count;
            }
            if (continue_label.count == 0) {
                return false;
            }
            if (!llvm_emit_defers_to(
                    ctx, function, continue_defer_count, false)) {
                return false;
            }
            sb_format(
                ctx->sb, "  br label %%" STRINGP "\n", STRINGV(continue_label));
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
    u32 return_type =
        llvm_function_return_type(ctx->sema, function->type_index);
    if (stmt->expr_index == U32_MAX ||
        llvm_type_is_void(ctx->sema, return_type)) {
        if (!llvm_emit_defers_to(ctx, function, 0, false)) {
            return false;
        }
        if (!llvm_emit_box_cleanup_all(ctx)) {
            return false;
        }
        sb_append_cstr(ctx->sb, "  ret void\n");
        ctx->block_terminated = true;
        return true;
    }

    bool old_discard_expr_value = ctx->discard_expr_value;
    ctx->discard_expr_value     = false;
    LlvmValue value         = llvm_emit_expr(ctx, function, stmt->expr_index);
    ctx->discard_expr_value = old_discard_expr_value;
    if (!value.ok) {
        return false;
    }
    if (ctx->block_terminated && value.value.count == 0) {
        return true;
    }
    if (!llvm_consume_box_expr(ctx, stmt->expr_index, return_type, U32_MAX)) {
        return false;
    }
    if (!llvm_emit_defers_to(ctx, function, 0, false)) {
        return false;
    }
    if (!llvm_emit_box_cleanup_all(ctx)) {
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
        if (stmt->kind != HIR_STMT_Let || stmt->local_index == U32_MAX ||
            stmt->expr_index >= array_count(ctx->hir->exprs)) {
            continue;
        }

        const HirExpr* expr           = &ctx->hir->exprs[stmt->expr_index];
        u32            function_index = U32_MAX;
        if (expr->kind == HIR_EXPR_FunctionRef &&
            expr->ref_index < array_count(ctx->hir->functions)) {
            function_index = expr->ref_index;
        } else if (expr->kind == HIR_EXPR_LocalRef &&
                   expr->ref_kind == HIR_REF_Binding &&
                   expr->ref_index < array_count(ctx->hir->bindings)) {
            const HirBinding* binding = &ctx->hir->bindings[expr->ref_index];
            if (binding->kind == HIR_BINDING_Function &&
                binding->target_index < array_count(ctx->hir->functions)) {
                function_index = binding->target_index;
            }
        }
        if (function_index == U32_MAX) {
            continue;
        }

        llvm_set_local_value(
            ctx,
            stmt->local_index,
            (LlvmValue){
                .ok         = true,
                .type_index = stmt->type_index,
                .value      = llvm_function_name_string(
                    ctx->hir, ctx->lexer, ctx->arena, function_index),
            });
    }
}

internal void llvm_collect_addressed_expr_locals(LlvmFunctionContext* ctx,
                                                 u32 expr_index);

internal void llvm_collect_addressed_locals(LlvmFunctionContext* ctx,
                                            u32                  block_index);

internal void llvm_mark_addressed_local_base(LlvmFunctionContext* ctx,
                                             u32                  expr_index)
{
    if (expr_index >= array_count(ctx->hir->exprs)) {
        return;
    }

    const HirExpr* expr = &ctx->hir->exprs[expr_index];
    if (expr->kind == HIR_EXPR_LocalRef && expr->ref_kind == HIR_REF_Local) {
        llvm_mark_assigned_local(ctx, expr->ref_index);
        return;
    }
    if (expr->kind == HIR_EXPR_Field || expr->kind == HIR_EXPR_TupleField ||
        expr->kind == HIR_EXPR_Index) {
        llvm_mark_addressed_local_base(ctx, expr->operand_expr_index);
    }
}

internal void llvm_mark_mutated_local_base(LlvmFunctionContext* ctx,
                                           u32                  expr_index,
                                           bool                 direct)
{
    if (expr_index >= array_count(ctx->hir->exprs)) {
        return;
    }

    const HirExpr* expr = &ctx->hir->exprs[expr_index];
    if (expr->kind == HIR_EXPR_LocalRef && expr->ref_kind == HIR_REF_Local) {
        if (direct) {
            llvm_mark_assigned_local(ctx, expr->ref_index);
            return;
        }

        u32          type_index = llvm_local_type(ctx, expr->ref_index);
        SemaTypeKind kind       = llvm_type_kind(ctx->sema, type_index);
        if (kind == STK_Array || kind == STK_Plex || kind == STK_Union ||
            kind == STK_Tuple) {
            llvm_mark_assigned_local(ctx, expr->ref_index);
        }
        return;
    }
    if (expr->kind == HIR_EXPR_Field || expr->kind == HIR_EXPR_TupleField ||
        expr->kind == HIR_EXPR_Index) {
        llvm_mark_mutated_local_base(ctx, expr->operand_expr_index, false);
    }
}

internal void llvm_collect_addressed_expr_locals(LlvmFunctionContext* ctx,
                                                 u32 expr_index)
{
    if (expr_index >= array_count(ctx->hir->exprs)) {
        return;
    }

    const HirExpr* expr = &ctx->hir->exprs[expr_index];
    switch (expr->kind) {
    case HIR_EXPR_Unary:
        if (expr->unary_op == HIR_UNARY_AddressOf) {
            llvm_mark_addressed_local_base(ctx, expr->operand_expr_index);
        }
        llvm_collect_addressed_expr_locals(ctx, expr->operand_expr_index);
        break;
    case HIR_EXPR_Binary:
        llvm_collect_addressed_expr_locals(ctx, expr->lhs_expr_index);
        llvm_collect_addressed_expr_locals(ctx, expr->rhs_expr_index);
        break;
    case HIR_EXPR_Assign:
        llvm_collect_addressed_expr_locals(ctx, expr->rhs_expr_index);
        break;
    case HIR_EXPR_Call:
        llvm_collect_addressed_expr_locals(ctx, expr->callee_expr_index);
        for (u32 i = 0; i < expr->arg_count; ++i) {
            u32 arg_index = expr->first_arg + i;
            if (arg_index < array_count(ctx->hir->call_args)) {
                llvm_collect_addressed_expr_locals(
                    ctx, ctx->hir->call_args[arg_index].expr_index);
            }
        }
        break;
    case HIR_EXPR_Cast:
    case HIR_EXPR_Field:
    case HIR_EXPR_TupleField:
        llvm_collect_addressed_expr_locals(ctx, expr->operand_expr_index);
        break;
    case HIR_EXPR_Index:
        llvm_collect_addressed_expr_locals(ctx, expr->operand_expr_index);
        llvm_collect_addressed_expr_locals(ctx, expr->extra_expr_index);
        break;
    case HIR_EXPR_Block:
        llvm_collect_addressed_locals(ctx, expr->body_block_index);
        break;
    case HIR_EXPR_On:
        for (u32 i = 0; i < expr->branch_count; ++i) {
            u32 branch_index = expr->first_branch + i;
            if (branch_index < array_count(ctx->hir->on_branches)) {
                const HirOnBranch* branch =
                    &ctx->hir->on_branches[branch_index];
                llvm_collect_addressed_expr_locals(ctx,
                                                   branch->guard_expr_index);
                llvm_collect_addressed_locals(ctx, branch->body_block_index);
            }
        }
        break;
    case HIR_EXPR_For:
        if (expr->for_index < array_count(ctx->hir->fors)) {
            const HirFor* loop = &ctx->hir->fors[expr->for_index];
            llvm_collect_addressed_expr_locals(ctx, loop->condition_expr_index);
            llvm_collect_addressed_expr_locals(ctx, loop->iterable_expr_index);
            llvm_collect_addressed_locals(ctx, loop->body_block_index);
            llvm_collect_addressed_locals(ctx, loop->else_block_index);
            for (u32 i = 0; i < loop->init_stmt_count; ++i) {
                u32 stmt_index =
                    ctx->hir->for_init_stmts[loop->first_init_stmt + i];
                if (stmt_index < array_count(ctx->hir->stmts)) {
                    const HirStmt* stmt = &ctx->hir->stmts[stmt_index];
                    llvm_collect_addressed_expr_locals(ctx, stmt->expr_index);
                    llvm_collect_addressed_locals(ctx, stmt->body_block_index);
                }
            }
            for (u32 i = 0; i < loop->update_stmt_count; ++i) {
                u32 stmt_index =
                    ctx->hir->for_update_stmts[loop->first_update_stmt + i];
                if (stmt_index < array_count(ctx->hir->stmts)) {
                    const HirStmt* stmt = &ctx->hir->stmts[stmt_index];
                    llvm_collect_addressed_expr_locals(ctx, stmt->expr_index);
                    llvm_collect_addressed_locals(ctx, stmt->body_block_index);
                }
            }
        }
        break;
    default:
        break;
    }
}

internal void llvm_collect_addressed_locals(LlvmFunctionContext* ctx,
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
        llvm_collect_addressed_expr_locals(ctx, stmt->expr_index);
        llvm_collect_addressed_locals(ctx, stmt->body_block_index);
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
            llvm_mark_mutated_local_base(ctx, stmt->target_expr_index, true);
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
                            ctx->hir->on_branches[branch_index]
                                .body_block_index);
                    }
                }
            }
            if (expr->kind == HIR_EXPR_Assign &&
                expr->lhs_expr_index < array_count(ctx->hir->exprs)) {
                llvm_mark_mutated_local_base(ctx, expr->lhs_expr_index, true);
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
                            llvm_mark_mutated_local_base(
                                ctx, init_stmt->target_expr_index, true);
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
                            llvm_mark_mutated_local_base(
                                ctx, update_stmt->target_expr_index, true);
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

internal bool llvm_initialise_assigned_param_slots(LlvmFunctionContext* ctx,
                                                   const HirFunction* function)
{
    for (u32 i = 0; i < function->param_count; ++i) {
        const HirParam* param = &ctx->hir->params[function->first_param + i];
        if (param->local_index == U32_MAX ||
            (!llvm_local_is_assigned(ctx, param->local_index) &&
             ctx->debug == NULL &&
             !llvm_type_is_box(ctx->sema, param->type_index))) {
            continue;
        }

        string value = llvm_param_value(
            function, ctx->hir, ctx->lexer, ctx->arena, param->local_index);
        if (value.count == 0) {
            return false;
        }

        LlvmLocalSlot* slot = llvm_ensure_local_slot_ex(
            ctx, param->local_index, param->type_index, i + 1);
        llvm_store_local_slot(ctx,
                              slot,
                              (LlvmValue){
                                  .ok         = true,
                                  .type_index = param->type_index,
                                  .value      = value,
                              });
    }
    return true;
}

internal bool llvm_emit_param_debug_values(LlvmFunctionContext* ctx,
                                           const HirFunction*   function)
{
    for (u32 i = 0; i < function->param_count; ++i) {
        const HirParam* param = &ctx->hir->params[function->first_param + i];
        if (param->local_index == U32_MAX ||
            llvm_local_is_assigned(ctx, param->local_index) ||
            llvm_find_local_slot(ctx, param->local_index) != NULL) {
            continue;
        }

        string value = llvm_param_value(
            function, ctx->hir, ctx->lexer, ctx->arena, param->local_index);
        if (value.count == 0) {
            continue;
        }
        llvm_debug_emit_param_value(ctx,
                                    param->local_index,
                                    (LlvmValue){
                                        .ok         = true,
                                        .type_index = param->type_index,
                                        .value      = value,
                                    },
                                    i + 1);
    }
    return true;
}

internal void llvm_debug_block_start(const Hir*      hir,
                                     const HirBlock* block,
                                     u32*            out_line,
                                     string*         out_path)
{
    *out_line = 0;
    *out_path = (string){0};
    if (hir == NULL || block == NULL) {
        return;
    }
    for (u32 i = 0; i < block->stmt_count; ++i) {
        u32 stmt_index = block->stmt_indices[i];
        if (stmt_index < array_count(hir->stmts) &&
            hir->stmts[stmt_index].source_line > 0) {
            *out_line = hir->stmts[stmt_index].source_line;
            *out_path = hir->stmts[stmt_index].source_path;
            return;
        }
    }
}

internal u32 llvm_debug_enter_scope(LlvmFunctionContext* ctx,
                                    const HirFunction*   function,
                                    u32                  scope_index,
                                    u32                  line,
                                    string               source_path)
{
    u32 old_scope = ctx->debug_scope_id;
    if (ctx->debug != NULL && scope_index != U32_MAX &&
        scope_index != function->root_scope_index) {
        ctx->debug_scope_id = llvm_debug_lexical_scope(
            ctx->debug, scope_index, ctx->debug_scope_id, line, source_path);
    }
    return old_scope;
}

internal bool llvm_emit_block(LlvmFunctionContext* ctx,
                              const HirFunction*   function,
                              u32                  block_index)
{
    if (block_index >= array_count(ctx->hir->blocks)) {
        return false;
    }

    const HirBlock* block     = &ctx->hir->blocks[block_index];
    u32             old_scope = ctx->debug_scope_id;
    if (ctx->debug != NULL && block->scope_index != U32_MAX &&
        block->scope_index != function->root_scope_index) {
        u32    line        = 0;
        string source_path = {0};
        llvm_debug_block_start(ctx->hir, block, &line, &source_path);
        old_scope = llvm_debug_enter_scope(
            ctx, function, block->scope_index, line, source_path);
    }

    llvm_bind_block_function_values(ctx, block_index);

    u32 defer_base = array_count(ctx->defer_block_indices);
    for (u32 i = 0; i < block->stmt_count; ++i) {
        if (ctx->block_terminated) {
            break;
        }
        u32 stmt_index = block->stmt_indices[i];
        if (stmt_index >= array_count(ctx->hir->stmts)) {
            continue;
        }

        const HirStmt* stmt = &ctx->hir->stmts[stmt_index];
        llvm_debug_emit_marker(ctx, stmt->source_line, stmt->source_path);
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
            bool ok = llvm_emit_return(ctx, function, stmt);
            if (ctx->defer_block_indices != NULL) {
                __array_count(ctx->defer_block_indices) = defer_base;
            }
            ctx->debug_scope_id = old_scope;
            return ok;
        } else if (stmt->kind == HIR_STMT_Expr) {
            if (stmt->expr_index != U32_MAX) {
                const HirExpr* expr =
                    stmt->expr_index < array_count(ctx->hir->exprs)
                        ? &ctx->hir->exprs[stmt->expr_index]
                        : NULL;
                if (expr != NULL && expr->kind == HIR_EXPR_Unsupported) {
                    continue;
                }
                bool old_discard_expr_value = ctx->discard_expr_value;
                if (expr != NULL && expr->kind == HIR_EXPR_On) {
                    ctx->discard_expr_value = true;
                }
                LlvmValue value =
                    llvm_emit_expr(ctx, function, stmt->expr_index);
                ctx->discard_expr_value = old_discard_expr_value;
                if (!value.ok) {
                    return false;
                }
                if (expr != NULL && expr->kind == HIR_EXPR_On) {
                    ctx->block_terminated = false;
                }
            }
            continue;
        } else if (stmt->kind == HIR_STMT_Assert) {
            if (!llvm_emit_assert(ctx, function, stmt)) {
                return false;
            }
            continue;
        } else if (stmt->kind == HIR_STMT_Defer) {
            if (stmt->body_block_index == U32_MAX) {
                return false;
            }
            array_push(ctx->defer_block_indices, stmt->body_block_index);
            continue;
        } else if (stmt->kind == HIR_STMT_Break) {
            if (!llvm_emit_effect_stmt(ctx, function, stmt)) {
                return false;
            }
            if (ctx->defer_block_indices != NULL) {
                __array_count(ctx->defer_block_indices) = defer_base;
            }
            ctx->debug_scope_id = old_scope;
            return true;
        } else if (stmt->kind == HIR_STMT_Continue) {
            if (!llvm_emit_effect_stmt(ctx, function, stmt)) {
                return false;
            }
            if (ctx->defer_block_indices != NULL) {
                __array_count(ctx->defer_block_indices) = defer_base;
            }
            ctx->debug_scope_id = old_scope;
            return true;
        } else if (stmt->kind == HIR_STMT_Block) {
            if (!llvm_emit_block(ctx, function, stmt->body_block_index)) {
                return false;
            }
            continue;
        }
    }

    if (!ctx->block_terminated) {
        llvm_debug_emit_block_end_anchor(ctx, block);
    }
    if (!ctx->block_terminated &&
        !llvm_emit_defers_to(ctx, function, defer_base, true)) {
        return false;
    }
    if (!ctx->block_terminated &&
        !llvm_emit_box_cleanup_for_scope(ctx, block->scope_index)) {
        return false;
    }
    if (ctx->block_terminated && ctx->defer_block_indices != NULL) {
        __array_count(ctx->defer_block_indices) = defer_base;
    }
    ctx->debug_scope_id = old_scope;
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
                                                    u32 field_index)
{
    string ptr         = llvm_temp(ctx);
    string header_type = llvm_dynamic_array_header_type();
    sb_format(ctx->sb,
              "  " STRINGP " = getelementptr inbounds " STRINGP ", ptr " STRINGP
              ", i64 0, i32 %u\n",
              STRINGV(ptr),
              STRINGV(header_type),
              STRINGV(header),
              field_index);
    return ptr;
}

internal string llvm_dynamic_array_data_from_header(LlvmFunctionContext* ctx,
                                                    string               header)
{
    string data         = llvm_temp(ctx);
    u64    header_bytes = llvm_dynamic_array_header_bytes(ctx->layout);
    sb_format(ctx->sb,
              "  " STRINGP " = getelementptr inbounds i8, ptr " STRINGP
              ", i64 %llu\n",
              STRINGV(data),
              STRINGV(header),
              (unsigned long long)header_bytes);
    return data;
}

internal string llvm_dynamic_array_header_from_data(LlvmFunctionContext* ctx,
                                                    string               data)
{
    string header       = llvm_temp(ctx);
    u64    header_bytes = llvm_dynamic_array_header_bytes(ctx->layout);
    sb_format(ctx->sb,
              "  " STRINGP " = getelementptr inbounds i8, ptr " STRINGP
              ", i64 -%llu\n",
              STRINGV(header),
              STRINGV(data),
              (unsigned long long)header_bytes);
    return header;
}

internal string llvm_dynamic_array_load_header_field(LlvmFunctionContext* ctx,
                                                     string               data,
                                                     u32    field_index,
                                                     string type)
{
    string header = llvm_dynamic_array_header_from_data(ctx, data);
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
                                               const HirExpr*       source_expr,
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
              "  br i1 " STRINGP ", label %%" STRINGP ", label %%" STRINGP "\n",
              STRINGV(is_null),
              STRINGV(alloc_label),
              STRINGV(done_label));

    sb_format(ctx->sb, STRINGP ":\n", STRINGV(alloc_label));
    string allocated = llvm_temp(ctx);
    string source_path =
        llvm_builtin_module_file_global_name_string(ctx->hir, ctx->arena);
    u32 source_line  = source_expr != NULL ? source_expr->source_line : 0;
    u64 header_bytes = llvm_dynamic_array_header_bytes(ctx->layout);
    sb_format(ctx->sb,
              "  " STRINGP
              " = call ptr @nrt_mem_alloc(i64 %llu, i64 16, ptr " STRINGP
              ", i32 %u)\n",
              STRINGV(allocated),
              (unsigned long long)header_bytes,
              STRINGV(source_path),
              source_line);
    string data_ptr  = llvm_dynamic_array_header_field_ptr(ctx, allocated, 0);
    string count_ptr = llvm_dynamic_array_header_field_ptr(ctx, allocated, 1);
    string capacity_ptr =
        llvm_dynamic_array_header_field_ptr(ctx, allocated, 2);
    string data = llvm_dynamic_array_data_from_header(ctx, allocated);
    sb_format(ctx->sb,
              "  store ptr " STRINGP ", ptr " STRINGP "\n"
              "  store i64 0, ptr " STRINGP "\n"
              "  store i64 0, ptr " STRINGP "\n"
              "  store ptr " STRINGP ", ptr " STRINGP "\n"
              "  br label %%" STRINGP "\n",
              STRINGV(data),
              STRINGV(data_ptr),
              STRINGV(count_ptr),
              STRINGV(capacity_ptr),
              STRINGV(data),
              STRINGV(slot),
              STRINGV(done_label));

    sb_format(ctx->sb, STRINGP ":\n", STRINGV(done_label));
    string current_data = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = load ptr, ptr " STRINGP "\n",
              STRINGV(current_data),
              STRINGV(slot));
    string header = llvm_dynamic_array_header_from_data(ctx, current_data);
    *out_header   = header;
    ctx->block_terminated = false;
    return true;
}

internal LlvmValue llvm_emit_dynamic_array_field(LlvmFunctionContext* ctx,
                                                 LlvmValue            target,
                                                 const HirExpr*       expr)
{
    string field       = lex_symbol(ctx->lexer, expr->symbol_handle);
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
              "  br i1 " STRINGP ", label %%" STRINGP ", label %%" STRINGP "\n",
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
    string loaded = llvm_dynamic_array_load_header_field(
        ctx, target.value, field_index, field_type);
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
    u32 result_type       = expr->type_index;
    if (string_eq_cstr(field, "count") || string_eq_cstr(field, "capacity")) {
        result_type = llvm_builtin_type(ctx->sema, STK_Usize);
    }
    return (LlvmValue){
        .ok         = true,
        .type_index = result_type,
        .value      = value,
    };
}

internal bool llvm_dynamic_array_callee_method(LlvmFunctionContext* ctx,
                                               u32     callee_expr_index,
                                               u32*    receiver_expr_index,
                                               string* method)
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

internal bool llvm_box_callee_method(LlvmFunctionContext* ctx,
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
    if (llvm_type_kind(ctx->sema, receiver->type_index) != STK_Box) {
        return false;
    }
    *receiver_expr_index = callee->operand_expr_index;
    *method              = lex_symbol(ctx->lexer, callee->symbol_handle);
    return true;
}

internal LlvmValue llvm_emit_box_free(LlvmFunctionContext* ctx,
                                      const HirFunction*   function,
                                      const HirExpr*       call,
                                      u32                  receiver_expr_index)
{
    if (call->arg_count != 0 ||
        receiver_expr_index >= array_count(ctx->hir->exprs)) {
        return (LlvmValue){0};
    }

    LlvmValue slot = llvm_address_of_expr(ctx, function, receiver_expr_index);
    if (!slot.ok) {
        return (LlvmValue){0};
    }

    string pointer = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = load ptr, ptr " STRINGP "\n",
              STRINGV(pointer),
              STRINGV(slot.value));
    string is_null = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = icmp eq ptr " STRINGP ", null\n",
              STRINGV(is_null),
              STRINGV(pointer));
    string free_label = llvm_label(ctx, "box.free");
    string done_label = llvm_label(ctx, "box.free.done");
    sb_format(ctx->sb,
              "  br i1 " STRINGP ", label %%" STRINGP ", label %%" STRINGP "\n",
              STRINGV(is_null),
              STRINGV(done_label),
              STRINGV(free_label));

    sb_format(ctx->sb, STRINGP ":\n", STRINGV(free_label));
    sb_format(ctx->sb,
              "  call void @nrt_mem_free(ptr " STRINGP ")\n"
              "  store ptr null, ptr " STRINGP "\n"
              "  br label %%" STRINGP "\n",
              STRINGV(pointer),
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

internal LlvmValue llvm_emit_dynamic_array_push(LlvmFunctionContext* ctx,
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

    LlvmValue slot = llvm_address_of_expr(ctx, function, receiver_expr_index);
    if (!slot.ok) {
        return (LlvmValue){0};
    }

    string header = {0};
    if (!llvm_dynamic_array_ensure_header(ctx, slot.value, call, &header)) {
        return (LlvmValue){0};
    }

    const HirCallArg* arg  = &ctx->hir->call_args[call->first_arg];
    LlvmValue         item = llvm_emit_expr(ctx, function, arg->expr_index);
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
              "  br i1 " STRINGP ", label %%" STRINGP ", label %%" STRINGP "\n",
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
              "  " STRINGP " = select i1 " STRINGP ", i64 1, i64 " STRINGP "\n",
              STRINGV(has_capacity),
              STRINGV(capacity),
              STRINGV(doubled),
              STRINGV(capacity),
              STRINGV(new_capacity),
              STRINGV(has_capacity),
              STRINGV(doubled));
    u64    item_size    = llvm_type_storage_bytes(ctx->sema, item_type);
    string byte_count   = llvm_temp(ctx);
    string total_bytes  = llvm_temp(ctx);
    string grown_header = llvm_temp(ctx);
    string source_path =
        llvm_builtin_module_file_global_name_string(ctx->hir, ctx->arena);
    sb_format(ctx->sb,
              "  " STRINGP " = mul i64 " STRINGP ", %llu\n"
              "  " STRINGP " = add i64 %llu, " STRINGP "\n"
              "  " STRINGP " = call ptr @nrt_mem_realloc(ptr " STRINGP
              ", i64 " STRINGP ", i64 16, ptr " STRINGP ", i32 %u)\n",
              STRINGV(byte_count),
              STRINGV(new_capacity),
              (unsigned long long)item_size,
              STRINGV(total_bytes),
              (unsigned long long)llvm_dynamic_array_header_bytes(ctx->layout),
              STRINGV(byte_count),
              STRINGV(grown_header),
              STRINGV(header),
              STRINGV(total_bytes),
              STRINGV(source_path),
              call->source_line);
    string grown_data = llvm_dynamic_array_data_from_header(ctx, grown_header);
    data_ptr_ptr = llvm_dynamic_array_header_field_ptr(ctx, grown_header, 0);
    capacity_ptr = llvm_dynamic_array_header_field_ptr(ctx, grown_header, 2);
    sb_format(ctx->sb,
              "  store ptr " STRINGP ", ptr " STRINGP "\n"
              "  store i64 " STRINGP ", ptr " STRINGP "\n"
              "  store ptr " STRINGP ", ptr " STRINGP "\n"
              "  br label %%" STRINGP "\n",
              STRINGV(grown_data),
              STRINGV(data_ptr_ptr),
              STRINGV(new_capacity),
              STRINGV(capacity_ptr),
              STRINGV(grown_data),
              STRINGV(slot.value),
              STRINGV(store_label));

    sb_format(ctx->sb, STRINGP ":\n", STRINGV(store_label));
    data = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = load ptr, ptr " STRINGP "\n",
              STRINGV(data),
              STRINGV(slot.value));
    header              = llvm_dynamic_array_header_from_data(ctx, data);
    data_ptr_ptr        = llvm_dynamic_array_header_field_ptr(ctx, header, 0);
    count_ptr           = llvm_dynamic_array_header_field_ptr(ctx, header, 1);
    string current_data = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = load ptr, ptr " STRINGP "\n",
              STRINGV(current_data),
              STRINGV(data_ptr_ptr));
    string item_type_string = llvm_type_string(ctx, item_type);
    string item_ptr         = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = getelementptr inbounds " STRINGP ", ptr " STRINGP
              ", i64 " STRINGP "\n",
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
                                                   const HirFunction* function,
                                                   const HirExpr*     call,
                                                   u32  receiver_expr_index,
                                                   bool relative)
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
    LlvmValue requested   = llvm_emit_expr(ctx, function, arg->expr_index);
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

    LlvmValue slot = llvm_address_of_expr(ctx, function, receiver_expr_index);
    if (!slot.ok) {
        return (LlvmValue){0};
    }

    string header = {0};
    if (!llvm_dynamic_array_ensure_header(ctx, slot.value, call, &header)) {
        return (LlvmValue){0};
    }

    string data_ptr_ptr = llvm_dynamic_array_header_field_ptr(ctx, header, 0);
    string capacity_ptr = llvm_dynamic_array_header_field_ptr(ctx, header, 2);
    string capacity     = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = load i64, ptr " STRINGP "\n",
              STRINGV(capacity),
              STRINGV(capacity_ptr));
    if (relative) {
        string count_ptr = llvm_dynamic_array_header_field_ptr(ctx, header, 1);
        string count     = llvm_temp(ctx);
        string needed    = llvm_temp(ctx);
        sb_format(ctx->sb,
                  "  " STRINGP " = load i64, ptr " STRINGP "\n"
                  "  " STRINGP " = add i64 " STRINGP ", " STRINGP "\n",
                  STRINGV(count),
                  STRINGV(count_ptr),
                  STRINGV(needed),
                  STRINGV(count),
                  STRINGV(requested_i64));
        requested_i64 = needed;
    }

    string grows = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = icmp ugt i64 " STRINGP ", " STRINGP "\n",
              STRINGV(grows),
              STRINGV(requested_i64),
              STRINGV(capacity));
    string grow_label = llvm_label(ctx, "dynarray.reserve.grow");
    string done_label = llvm_label(ctx, "dynarray.reserve.done");
    sb_format(ctx->sb,
              "  br i1 " STRINGP ", label %%" STRINGP ", label %%" STRINGP "\n",
              STRINGV(grows),
              STRINGV(grow_label),
              STRINGV(done_label));

    sb_format(ctx->sb, STRINGP ":\n", STRINGV(grow_label));
    u64    item_size   = llvm_type_storage_bytes(ctx->sema, item_type);
    string bytes       = llvm_temp(ctx);
    string total_bytes = llvm_temp(ctx);
    string new_header  = llvm_temp(ctx);
    string source_path =
        llvm_builtin_module_file_global_name_string(ctx->hir, ctx->arena);
    sb_format(ctx->sb,
              "  " STRINGP " = mul i64 " STRINGP ", %llu\n"
              "  " STRINGP " = add i64 %llu, " STRINGP "\n"
              "  " STRINGP " = call ptr @nrt_mem_realloc(ptr " STRINGP
              ", i64 " STRINGP ", i64 16, ptr " STRINGP ", i32 %u)\n",
              STRINGV(bytes),
              STRINGV(requested_i64),
              (unsigned long long)item_size,
              STRINGV(total_bytes),
              (unsigned long long)llvm_dynamic_array_header_bytes(ctx->layout),
              STRINGV(bytes),
              STRINGV(new_header),
              STRINGV(header),
              STRINGV(total_bytes),
              STRINGV(source_path),
              call->source_line);
    string new_data = llvm_dynamic_array_data_from_header(ctx, new_header);
    data_ptr_ptr    = llvm_dynamic_array_header_field_ptr(ctx, new_header, 0);
    capacity_ptr    = llvm_dynamic_array_header_field_ptr(ctx, new_header, 2);
    sb_format(ctx->sb,
              "  store ptr " STRINGP ", ptr " STRINGP "\n"
              "  store i64 " STRINGP ", ptr " STRINGP "\n"
              "  store ptr " STRINGP ", ptr " STRINGP "\n"
              "  br label %%" STRINGP "\n",
              STRINGV(new_data),
              STRINGV(data_ptr_ptr),
              STRINGV(requested_i64),
              STRINGV(capacity_ptr),
              STRINGV(new_data),
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

internal LlvmValue llvm_emit_dynamic_array_clear(LlvmFunctionContext* ctx,
                                                 const HirFunction*   function,
                                                 const HirExpr*       call,
                                                 u32 receiver_expr_index)
{
    if (call->arg_count != 0 ||
        receiver_expr_index >= array_count(ctx->hir->exprs)) {
        return (LlvmValue){0};
    }
    LlvmValue slot = llvm_address_of_expr(ctx, function, receiver_expr_index);
    if (!slot.ok) {
        return (LlvmValue){0};
    }

    string data = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = load ptr, ptr " STRINGP "\n",
              STRINGV(data),
              STRINGV(slot.value));
    string is_null = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = icmp eq ptr " STRINGP ", null\n",
              STRINGV(is_null),
              STRINGV(data));
    string clear_label = llvm_label(ctx, "dynarray.clear");
    string done_label  = llvm_label(ctx, "dynarray.clear.done");
    sb_format(ctx->sb,
              "  br i1 " STRINGP ", label %%" STRINGP ", label %%" STRINGP "\n",
              STRINGV(is_null),
              STRINGV(done_label),
              STRINGV(clear_label));

    sb_format(ctx->sb, STRINGP ":\n", STRINGV(clear_label));
    string header    = llvm_dynamic_array_header_from_data(ctx, data);
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
    LlvmValue slot = llvm_address_of_expr(ctx, function, receiver_expr_index);
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
              "  br i1 " STRINGP ", label %%" STRINGP ", label %%" STRINGP "\n",
              STRINGV(is_null),
              STRINGV(done_label),
              STRINGV(free_label));

    sb_format(ctx->sb, STRINGP ":\n", STRINGV(free_label));
    string allocation = llvm_dynamic_array_header_from_data(ctx, header);
    sb_format(ctx->sb,
              "  call void @nrt_mem_free(ptr " STRINGP ")\n"
              "  store ptr null, ptr " STRINGP "\n"
              "  br label %%" STRINGP "\n",
              STRINGV(allocation),
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

    string data      = target.value;
    string header    = llvm_dynamic_array_header_from_data(ctx, data);
    string count_ptr = llvm_dynamic_array_header_field_ptr(ctx, header, 1);
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
              "  " STRINGP " = getelementptr inbounds " STRINGP ", ptr " STRINGP
              ", i64 " STRINGP "\n"
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
                                                  u32  receiver_expr_index,
                                                  bool swap)
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

    LlvmValue target      = llvm_emit_expr(ctx, function, receiver_expr_index);
    const HirCallArg* arg = &ctx->hir->call_args[call->first_arg];
    LlvmValue         index = llvm_emit_expr(ctx, function, arg->expr_index);
    if (!target.ok || !index.ok) {
        return (LlvmValue){0};
    }

    string data      = target.value;
    string header    = llvm_dynamic_array_header_from_data(ctx, data);
    string count_ptr = llvm_dynamic_array_header_field_ptr(ctx, header, 1);
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
                  "  br label %%" STRINGP "\n" STRINGP ":\n",
                  STRINGV(loop_label),
                  STRINGV(loop_label));

        string cursor = llvm_temp(ctx);
        string more   = llvm_temp(ctx);
        sb_format(ctx->sb,
                  "  " STRINGP " = load i64, ptr " STRINGP "\n"
                  "  " STRINGP " = icmp ult i64 " STRINGP ", " STRINGP "\n"
                  "  br i1 " STRINGP ", label %%" STRINGP ", label %%" STRINGP
                  "\n",
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

    const HirCallArg* arg    = &ctx->hir->call_args[call->first_arg];
    LlvmValue         source = llvm_emit_expr(ctx, function, arg->expr_index);
    if (!source.ok ||
        llvm_type_kind(ctx->sema, source.type_index) != STK_Slice) {
        return (LlvmValue){0};
    }
    string source_type  = llvm_type_string(ctx, source.type_index);
    string source_data  = llvm_temp(ctx);
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

    LlvmValue slot = llvm_address_of_expr(ctx, function, receiver_expr_index);
    if (!slot.ok) {
        return (LlvmValue){0};
    }

    string header = {0};
    if (!llvm_dynamic_array_ensure_header(ctx, slot.value, call, &header)) {
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
              "  br i1 " STRINGP ", label %%" STRINGP ", label %%" STRINGP "\n",
              STRINGV(grows),
              STRINGV(grow_label),
              STRINGV(copy_label));

    sb_format(ctx->sb, STRINGP ":\n", STRINGV(grow_label));
    u64    item_size   = llvm_type_storage_bytes(ctx->sema, item_type);
    string bytes       = llvm_temp(ctx);
    string total_bytes = llvm_temp(ctx);
    string new_header  = llvm_temp(ctx);
    string source_path =
        llvm_builtin_module_file_global_name_string(ctx->hir, ctx->arena);
    sb_format(ctx->sb,
              "  " STRINGP " = mul i64 " STRINGP ", %llu\n"
              "  " STRINGP " = add i64 %llu, " STRINGP "\n"
              "  " STRINGP " = call ptr @nrt_mem_realloc(ptr " STRINGP
              ", i64 " STRINGP ", i64 16, ptr " STRINGP ", i32 %u)\n",
              STRINGV(bytes),
              STRINGV(new_count),
              (unsigned long long)item_size,
              STRINGV(total_bytes),
              (unsigned long long)llvm_dynamic_array_header_bytes(ctx->layout),
              STRINGV(bytes),
              STRINGV(new_header),
              STRINGV(header),
              STRINGV(total_bytes),
              STRINGV(source_path),
              call->source_line);
    string new_data = llvm_dynamic_array_data_from_header(ctx, new_header);
    data_ptr_ptr    = llvm_dynamic_array_header_field_ptr(ctx, new_header, 0);
    capacity_ptr    = llvm_dynamic_array_header_field_ptr(ctx, new_header, 2);
    sb_format(ctx->sb,
              "  store ptr " STRINGP ", ptr " STRINGP "\n"
              "  store i64 " STRINGP ", ptr " STRINGP "\n"
              "  store ptr " STRINGP ", ptr " STRINGP "\n"
              "  br label %%" STRINGP "\n",
              STRINGV(new_data),
              STRINGV(data_ptr_ptr),
              STRINGV(new_count),
              STRINGV(capacity_ptr),
              STRINGV(new_data),
              STRINGV(slot.value),
              STRINGV(copy_label));

    sb_format(ctx->sb, STRINGP ":\n", STRINGV(copy_label));
    data = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = load ptr, ptr " STRINGP "\n",
              STRINGV(data),
              STRINGV(slot.value));
    header              = llvm_dynamic_array_header_from_data(ctx, data);
    data_ptr_ptr        = llvm_dynamic_array_header_field_ptr(ctx, header, 0);
    count_ptr           = llvm_dynamic_array_header_field_ptr(ctx, header, 1);
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
              "  br label %%" STRINGP "\n" STRINGP ":\n",
              STRINGV(loop_label),
              STRINGV(loop_label));

    string cursor = llvm_temp(ctx);
    string more   = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = load i64, ptr " STRINGP "\n"
              "  " STRINGP " = icmp ult i64 " STRINGP ", " STRINGP "\n"
              "  br i1 " STRINGP ", label %%" STRINGP ", label %%" STRINGP "\n",
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
              "  " STRINGP " = getelementptr inbounds " STRINGP ", ptr " STRINGP
              ", i64 " STRINGP "\n"
              "  " STRINGP " = getelementptr inbounds " STRINGP ", ptr " STRINGP
              ", i64 " STRINGP "\n"
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

internal LlvmValue llvm_emit_dynamic_array_resize(LlvmFunctionContext* ctx,
                                                  const HirFunction*   function,
                                                  const HirExpr*       call,
                                                  u32  receiver_expr_index,
                                                  bool initialize,
                                                  bool relative)
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
    LlvmValue requested   = llvm_emit_expr(ctx, function, arg->expr_index);
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

    LlvmValue slot = llvm_address_of_expr(ctx, function, receiver_expr_index);
    if (!slot.ok) {
        return (LlvmValue){0};
    }

    string header = {0};
    if (!llvm_dynamic_array_ensure_header(ctx, slot.value, call, &header)) {
        return (LlvmValue){0};
    }

    string data_ptr_ptr = llvm_dynamic_array_header_field_ptr(ctx, header, 0);
    string count_ptr    = llvm_dynamic_array_header_field_ptr(ctx, header, 1);
    string capacity_ptr = llvm_dynamic_array_header_field_ptr(ctx, header, 2);
    string data         = llvm_temp(ctx);
    string old_count    = llvm_temp(ctx);
    string capacity     = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = load ptr, ptr " STRINGP "\n"
              "  " STRINGP " = load i64, ptr " STRINGP "\n"
              "  " STRINGP " = load i64, ptr " STRINGP "\n",
              STRINGV(data),
              STRINGV(data_ptr_ptr),
              STRINGV(old_count),
              STRINGV(count_ptr),
              STRINGV(capacity),
              STRINGV(capacity_ptr));
    if (relative) {
        string new_count = llvm_temp(ctx);
        sb_format(ctx->sb,
                  "  " STRINGP " = add i64 " STRINGP ", " STRINGP "\n",
                  STRINGV(new_count),
                  STRINGV(old_count),
                  STRINGV(requested_i64));
        requested_i64 = new_count;
    }

    string grows = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = icmp ugt i64 " STRINGP ", " STRINGP "\n",
              STRINGV(grows),
              STRINGV(requested_i64),
              STRINGV(capacity));
    string grow_label  = llvm_label(ctx, "dynarray.resize.grow");
    string init_label  = llvm_label(ctx, "dynarray.resize.init");
    string count_label = llvm_label(ctx, "dynarray.resize.count");
    sb_format(ctx->sb,
              "  br i1 " STRINGP ", label %%" STRINGP ", label %%" STRINGP "\n",
              STRINGV(grows),
              STRINGV(grow_label),
              STRINGV(initialize ? init_label : count_label));

    sb_format(ctx->sb, STRINGP ":\n", STRINGV(grow_label));
    u64    item_size   = llvm_type_storage_bytes(ctx->sema, item_type);
    string bytes       = llvm_temp(ctx);
    string total_bytes = llvm_temp(ctx);
    string new_header  = llvm_temp(ctx);
    string source_path =
        llvm_builtin_module_file_global_name_string(ctx->hir, ctx->arena);
    sb_format(ctx->sb,
              "  " STRINGP " = mul i64 " STRINGP ", %llu\n"
              "  " STRINGP " = add i64 %llu, " STRINGP "\n"
              "  " STRINGP " = call ptr @nrt_mem_realloc(ptr " STRINGP
              ", i64 " STRINGP ", i64 16, ptr " STRINGP ", i32 %u)\n",
              STRINGV(bytes),
              STRINGV(requested_i64),
              (unsigned long long)item_size,
              STRINGV(total_bytes),
              (unsigned long long)llvm_dynamic_array_header_bytes(ctx->layout),
              STRINGV(bytes),
              STRINGV(new_header),
              STRINGV(header),
              STRINGV(total_bytes),
              STRINGV(source_path),
              call->source_line);
    string new_data = llvm_dynamic_array_data_from_header(ctx, new_header);
    data_ptr_ptr    = llvm_dynamic_array_header_field_ptr(ctx, new_header, 0);
    capacity_ptr    = llvm_dynamic_array_header_field_ptr(ctx, new_header, 2);
    sb_format(ctx->sb,
              "  store ptr " STRINGP ", ptr " STRINGP "\n"
              "  store i64 " STRINGP ", ptr " STRINGP "\n"
              "  store ptr " STRINGP ", ptr " STRINGP "\n"
              "  br label %%" STRINGP "\n",
              STRINGV(new_data),
              STRINGV(data_ptr_ptr),
              STRINGV(requested_i64),
              STRINGV(capacity_ptr),
              STRINGV(new_data),
              STRINGV(slot.value),
              STRINGV(initialize ? init_label : count_label));

    if (initialize) {
        sb_format(ctx->sb, STRINGP ":\n", STRINGV(init_label));
        data = llvm_temp(ctx);
        sb_format(ctx->sb,
                  "  " STRINGP " = load ptr, ptr " STRINGP "\n",
                  STRINGV(data),
                  STRINGV(slot.value));
        header       = llvm_dynamic_array_header_from_data(ctx, data);
        data_ptr_ptr = llvm_dynamic_array_header_field_ptr(ctx, header, 0);
        string should_init = llvm_temp(ctx);
        string loop_label  = llvm_label(ctx, "dynarray.resize.init.loop");
        string body_label  = llvm_label(ctx, "dynarray.resize.init.body");
        sb_format(ctx->sb,
                  "  " STRINGP " = icmp ugt i64 " STRINGP ", " STRINGP "\n"
                  "  br i1 " STRINGP ", label %%" STRINGP ", label %%" STRINGP
                  "\n",
                  STRINGV(should_init),
                  STRINGV(requested_i64),
                  STRINGV(old_count),
                  STRINGV(should_init),
                  STRINGV(loop_label),
                  STRINGV(count_label));

        string cursor_slot = llvm_temp(ctx);
        sb_format(ctx->sb,
                  STRINGP ":\n"
                          "  " STRINGP " = alloca i64\n"
                          "  store i64 " STRINGP ", ptr " STRINGP "\n"
                          "  br label %%" STRINGP "\n",
                  STRINGV(loop_label),
                  STRINGV(cursor_slot),
                  STRINGV(old_count),
                  STRINGV(cursor_slot),
                  STRINGV(body_label));

        sb_format(ctx->sb, STRINGP ":\n", STRINGV(body_label));
        string    cursor           = llvm_temp(ctx);
        string    more             = llvm_temp(ctx);
        string    item_type_string = llvm_type_string(ctx, item_type);
        string    current_data     = llvm_temp(ctx);
        string    item_ptr         = llvm_temp(ctx);
        string    next             = llvm_temp(ctx);
        LlvmValue default_value    = llvm_default_value(ctx, item_type);
        if (!default_value.ok) {
            return (LlvmValue){0};
        }
        u32 store_label_index = ctx->next_label++;
        sb_format(ctx->sb,
                  "  " STRINGP " = load i64, ptr " STRINGP "\n"
                  "  " STRINGP " = icmp ult i64 " STRINGP ", " STRINGP "\n"
                  "  br i1 " STRINGP ", label %%"
                  "dynarray.resize.init.store.%u, label %%" STRINGP "\n"
                  "dynarray.resize.init.store.%u:\n"
                  "  " STRINGP " = load ptr, ptr " STRINGP "\n"
                  "  " STRINGP " = getelementptr inbounds " STRINGP
                  ", ptr " STRINGP ", i64 " STRINGP "\n"
                  "  store " STRINGP " " STRINGP ", ptr " STRINGP "\n"
                  "  " STRINGP " = add i64 " STRINGP ", 1\n"
                  "  store i64 " STRINGP ", ptr " STRINGP "\n"
                  "  br label %%" STRINGP "\n",
                  STRINGV(cursor),
                  STRINGV(cursor_slot),
                  STRINGV(more),
                  STRINGV(cursor),
                  STRINGV(requested_i64),
                  STRINGV(more),
                  store_label_index,
                  STRINGV(count_label),
                  store_label_index,
                  STRINGV(current_data),
                  STRINGV(data_ptr_ptr),
                  STRINGV(item_ptr),
                  STRINGV(item_type_string),
                  STRINGV(current_data),
                  STRINGV(cursor),
                  STRINGV(item_type_string),
                  STRINGV(default_value.value),
                  STRINGV(item_ptr),
                  STRINGV(next),
                  STRINGV(cursor),
                  STRINGV(next),
                  STRINGV(cursor_slot),
                  STRINGV(body_label));
    }

    sb_format(ctx->sb, STRINGP ":\n", STRINGV(count_label));
    data = llvm_temp(ctx);
    sb_format(ctx->sb,
              "  " STRINGP " = load ptr, ptr " STRINGP "\n",
              STRINGV(data),
              STRINGV(slot.value));
    header    = llvm_dynamic_array_header_from_data(ctx, data);
    count_ptr = llvm_dynamic_array_header_field_ptr(ctx, header, 1);
    sb_format(ctx->sb,
              "  store i64 " STRINGP ", ptr " STRINGP "\n",
              STRINGV(requested_i64),
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
                                          const Hir*     hir,
                                          const Lexer*   lexer)
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

internal void llvm_render_builtin_macro_globals(StringBuilder* sb,
                                                const Hir*     hir,
                                                const Lexer*   lexer)
{
    llvm_append_builtin_module_file_global_name(sb, hir);
    sb_format(sb,
              " = private unnamed_addr constant [%zu x i8] c\"",
              lexer->source.source_path.count + 1);
    llvm_append_escaped_string_bytes(sb, lexer->source.source_path);
    sb_append_cstr(sb, "\\00\"\n");
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
                                         const Hir*     hir,
                                         const Lexer*   lexer)
{
    for (u32 i = 0; i < array_count(hir->stmts); ++i) {
        const HirStmt* stmt = &hir->stmts[i];
        if (stmt->kind != HIR_STMT_Assert) {
            continue;
        }
        string source_path =
            stmt->source_path.count > 0
                ? stmt->source_path
                : (lexer != NULL ? lexer->source.source_path : s(""));
        llvm_append_assert_source_path_global_name(sb, hir, i);
        sb_format(sb,
                  " = private unnamed_addr constant [%zu x i8] c\"",
                  source_path.count + 1);
        llvm_append_escaped_string_bytes(sb, source_path);
        sb_append_cstr(sb, "\\00\"\n");
    }

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

    if (expr->kind == HIR_EXPR_BuiltinMacro && expr->symbol_handle != U32_MAX &&
        string_eq_cstr(lex_symbol(lexer, expr->symbol_handle), "file")) {
        *out = lexer->source.source_path;
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
                                                 const Hir*     hir,
                                                 const Lexer*   lexer,
                                                 Arena*         arena)
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

internal bool llvm_render_const_slice_backing_values(StringBuilder* sb,
                                                     const Hir*     hir,
                                                     const Lexer*   lexer,
                                                     const Sema*    sema,
                                                     Arena*         arena)
{
    bool rendered = false;
    for (u32 i = 0; i < array_count(hir->exprs); ++i) {
        const HirExpr* expr = &hir->exprs[i];
        if (expr->kind != HIR_EXPR_Array ||
            llvm_type_kind(sema, expr->type_index) != STK_Slice ||
            !llvm_expr_is_constant_value(hir, lexer, sema, i)) {
            continue;
        }

        u32 item_type = llvm_collection_item_type(sema, expr->type_index);
        if (item_type == sema_no_type()) {
            continue;
        }

        llvm_append_const_slice_backing_name(sb, hir, i);
        sb_format(
            sb, " = private unnamed_addr constant [%u x ", expr->arg_count);
        llvm_append_type(sb, sema, item_type);
        sb_append_cstr(sb, "] [");
        Arena temp = {0};
        arena_init(&temp);
        llvm_append_constant_aggregate_items(
            sb, hir, lexer, sema, &temp, i, expr->type_index, item_type);
        arena_done(&temp);
        sb_append_cstr(sb, "]\n");
        rendered = true;
    }
    (void)arena;
    return rendered;
}

typedef struct {
    cstr return_type;
    cstr name;
    cstr params;
} LlvmRuntimeDecl;

internal void llvm_render_runtime_declarations(StringBuilder*         sb,
                                               const LlvmRuntimeDecl* decls,
                                               u32 decl_count)
{
    for (u32 i = 0; i < decl_count; ++i) {
        const LlvmRuntimeDecl* decl = &decls[i];
        sb_format(sb,
                  "declare %s @%s(%s)\n",
                  decl->return_type,
                  decl->name,
                  decl->params);
    }
}

internal void llvm_render_string_runtime_declarations(StringBuilder* sb)
{
    static const LlvmRuntimeDecl decls[] = {
        {"i1", "string_eq", "ptr, ptr"},
        {"void", "string_builder_reset", ""},
        {"i64", "string_builder_mark", ""},
        {"void", "string_builder_append_string", "ptr"},
        {"void", "string_builder_append_byte", "i8"},
        {"void", "string_builder_finish", "ptr, i64"},
        {"void", "to_string$string", "ptr, ptr"},
        {"void", "to_string$bool", "ptr, i1"},
        {"void", "to_string$i8", "ptr, i8"},
        {"void", "to_string$i16", "ptr, i16"},
        {"void", "to_string$i32", "ptr, i32"},
        {"void", "to_string$i64", "ptr, i64"},
        {"void", "to_string$u8", "ptr, i8"},
        {"void", "to_string$u16", "ptr, i16"},
        {"void", "to_string$u32", "ptr, i32"},
        {"void", "to_string$u64", "ptr, i64"},
        {"void", "to_string$isize", "ptr, i64"},
        {"void", "to_string$usize", "ptr, i64"},
        {"void", "to_string$f32", "ptr, float"},
        {"void", "to_string$f64", "ptr, double"},
    };
    llvm_render_runtime_declarations(
        sb, decls, (u32)(sizeof(decls) / sizeof(decls[0])));
}

internal void llvm_render_assert_runtime_declarations(StringBuilder* sb)
{
    static const LlvmRuntimeDecl decls[] = {
        {"void", "nerd_assert", "i1, ptr, i32, ptr"},
    };
    llvm_render_runtime_declarations(
        sb, decls, (u32)(sizeof(decls) / sizeof(decls[0])));
}

internal bool llvm_hir_uses_dynamic_array_runtime(const Hir*  hir,
                                                  const Sema* sema)
{
    for (u32 i = 0; i < array_count(hir->exprs); ++i) {
        SemaTypeKind kind = llvm_type_kind(sema, hir->exprs[i].type_index);
        if (kind == STK_DynamicArray || kind == STK_Box ||
            hir->exprs[i].kind == HIR_EXPR_Box) {
            return true;
        }
    }
    for (u32 i = 0; i < array_count(hir->stmts); ++i) {
        SemaTypeKind kind = llvm_type_kind(sema, hir->stmts[i].type_index);
        if (kind == STK_DynamicArray || kind == STK_Box) {
            return true;
        }
    }
    return false;
}

internal void llvm_render_dynamic_array_runtime_declarations(StringBuilder* sb)
{
    static const LlvmRuntimeDecl decls[] = {
        {"ptr", "nrt_mem_alloc", "i64, i64, ptr, i32"},
        {"ptr", "nrt_mem_realloc", "ptr, i64, i64, ptr, i32"},
        {"void", "nrt_mem_free", "ptr"},
    };
    llvm_render_runtime_declarations(
        sb, decls, (u32)(sizeof(decls) / sizeof(decls[0])));
}

internal void llvm_render_arena_runtime_declarations(StringBuilder* sb)
{
    static const LlvmRuntimeDecl decls[] = {
        {"void", "nrt_arena_init", "ptr, i64, i64, ptr, i32"},
    };
    llvm_render_runtime_declarations(
        sb, decls, (u32)(sizeof(decls) / sizeof(decls[0])));
}

internal bool llvm_hir_uses_arena_runtime(const Hir*   hir,
                                          const Lexer* lexer,
                                          const Sema*  sema)
{
    for (u32 i = 0; i < array_count(hir->exprs); ++i) {
        const HirExpr* expr = &hir->exprs[i];
        if (expr->kind != HIR_EXPR_Call ||
            llvm_type_kind(sema, expr->type_index) != STK_Arena ||
            expr->callee_expr_index >= array_count(hir->exprs)) {
            continue;
        }
        const HirExpr* callee = &hir->exprs[expr->callee_expr_index];
        if (callee->kind == HIR_EXPR_LocalRef &&
            callee->symbol_handle != U32_MAX &&
            string_eq(lex_symbol(lexer, callee->symbol_handle), s("arena"))) {
            return true;
        }
    }
    return false;
}

internal bool llvm_hir_uses_string_runtime(const Hir* hir, const Sema* sema)
{
    for (u32 i = 0; i < array_count(hir->exprs); ++i) {
        const HirExpr* expr = &hir->exprs[i];
        if (expr->kind == HIR_EXPR_InterpolatedString) {
            return true;
        }
        if (expr->kind == HIR_EXPR_Binary &&
            (expr->binary_op == HIR_BINARY_Equal ||
             expr->binary_op == HIR_BINARY_NotEqual) &&
            expr->lhs_expr_index < array_count(hir->exprs) &&
            expr->rhs_expr_index < array_count(hir->exprs) &&
            llvm_type_kind(sema, hir->exprs[expr->lhs_expr_index].type_index) ==
                STK_String &&
            llvm_type_kind(sema, hir->exprs[expr->rhs_expr_index].type_index) ==
                STK_String) {
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

internal bool llvm_hir_has_ffi_function_symbol(const Hir* hir,
                                               u32        symbol_handle)
{
    if (symbol_handle == U32_MAX) {
        return false;
    }
    for (u32 i = 0; i < array_count(hir->functions); ++i) {
        const HirFunction* function = &hir->functions[i];
        if (function->kind == HIR_FUNCTION_Ffi &&
            function->ffi_symbol_handle == symbol_handle) {
            return true;
        }
    }
    return false;
}

internal bool llvm_imports_same_runtime_symbol(const HirImport* a,
                                               const HirImport* b)
{
    if (a->ffi_symbol_handle != U32_MAX || b->ffi_symbol_handle != U32_MAX) {
        return a->ffi_symbol_handle != U32_MAX &&
               a->ffi_symbol_handle == b->ffi_symbol_handle;
    }
    if (a->module_index != U32_MAX || b->module_index != U32_MAX) {
        return a->module_index == b->module_index &&
               a->decl_index == b->decl_index;
    }
    return a->symbol_handle == b->symbol_handle;
}

internal void llvm_render_import(StringBuilder*   sb,
                                 const Hir*       hir,
                                 const Lexer*     lexer,
                                 const Sema*      sema,
                                 const HirImport* import)
{
    if (sema == NULL || import->type_index >= array_count(sema->types) ||
        sema->types[import->type_index].kind != STK_Function) {
        return;
    }
    if (import->ffi_symbol_handle != U32_MAX &&
        llvm_hir_has_ffi_function_symbol(hir, import->ffi_symbol_handle)) {
        return;
    }

    sb_append_cstr(sb, "declare ");
    u32 return_type = llvm_function_return_type(sema, import->type_index);
    llvm_append_type(sb, sema, return_type);
    sb_append_char(sb, ' ');
    if (import->ffi_symbol_handle != U32_MAX) {
        llvm_append_c_symbol_name(sb,
                                  lex_symbol(lexer, import->ffi_symbol_handle));
    } else if (sema != NULL && sema->program != NULL) {
        const Hir* source_hir     = NULL;
        u32        function_index = U32_MAX;
        if (llvm_import_source_function(
                sema, import, &source_hir, &function_index) &&
            llvm_program_function_symbol_conflicts(
                sema, source_hir, function_index)) {
            llvm_append_generated_function_name(sb, source_hir, function_index);
        } else {
            llvm_append_symbol_name(sb,
                                    lex_symbol(lexer, import->symbol_handle));
        }
    } else {
        llvm_append_symbol_name(sb, lex_symbol(lexer, import->symbol_handle));
    }
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

internal void llvm_render_imported_generic_function_declarations(
    StringBuilder* sb, const Hir* hir, const Sema* sema)
{
    if (sema == NULL || sema->program == NULL) {
        return;
    }

    for (u32 i = 0; i < array_count(hir->imports); ++i) {
        const HirImport* import = &hir->imports[i];
        if (import->module_index >= array_count(sema->program->modules)) {
            continue;
        }

        const ModuleInfo* module =
            &sema->program->modules[import->module_index];
        const Hir*   source_hir   = &module->front_end.hir;
        const Lexer* source_lexer = &module->front_end.lexer;
        const Sema*  source_sema  = &module->front_end.sema;

        for (u32 j = 0; j < array_count(source_hir->functions); ++j) {
            const HirFunction* function = &source_hir->functions[j];
            if (function->kind != HIR_FUNCTION_GenericInstantiation ||
                import->decl_index >= array_count(source_sema->decls) ||
                source_sema->decls[import->decl_index].value_node_index !=
                    function->fn_node_index) {
                continue;
            }

            sb_append_cstr(sb, "declare ");
            llvm_append_function_signature(
                sb, source_hir, source_lexer, source_sema, function, j);
            sb_append_char(sb, '\n');
        }
    }
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

internal bool llvm_hir_is_root_module(const Sema* sema, const Hir* hir)
{
    return sema != NULL && sema->program != NULL && hir != NULL &&
           hir->current_module_index == sema->program->root_module_index;
}

internal bool llvm_hir_binding_is_exported(const Sema* sema,
                                           const Hir*  hir,
                                           u32         binding_index)
{
    if (!llvm_hir_is_root_module(sema, hir)) {
        return false;
    }
    for (u32 i = 0; i < array_count(hir->exports); ++i) {
        if (hir->exports[i].binding_index == binding_index) {
            return true;
        }
    }
    return false;
}

internal bool llvm_root_exports_symbol(const Sema* sema, string name)
{
    if (sema == NULL || sema->program == NULL ||
        sema->program->root_module_index >=
            array_count(sema->program->modules)) {
        return false;
    }

    const ModuleInfo* root =
        &sema->program->modules[sema->program->root_module_index];
    const Hir*   hir   = &root->front_end.hir;
    const Lexer* lexer = &root->front_end.lexer;
    for (u32 i = 0; i < array_count(hir->exports); ++i) {
        u32 binding_index = hir->exports[i].binding_index;
        if (binding_index >= array_count(hir->bindings)) {
            continue;
        }
        u32 symbol = hir->bindings[binding_index].symbol_handle;
        if (symbol != U32_MAX && string_eq(lex_symbol(lexer, symbol), name)) {
            return true;
        }
    }
    return false;
}

internal bool llvm_symbol_is_main(const Lexer* lexer, u32 symbol_handle)
{
    if (symbol_handle == U32_MAX) {
        return false;
    }
    return string_eq(lex_symbol(lexer, symbol_handle), s("main"));
}

internal u32 llvm_hir_value_binding_index(const Hir* hir, u32 value_index)
{
    for (u32 i = 0; i < array_count(hir->bindings); ++i) {
        const HirBinding* binding = &hir->bindings[i];
        if (binding->kind == HIR_BINDING_Value &&
            binding->target_index == value_index) {
            return i;
        }
    }
    return U32_MAX;
}

internal void llvm_render_global_values(StringBuilder*   sb,
                                        const Hir*       hir,
                                        const Lexer*     lexer,
                                        const Sema*      sema,
                                        Arena*           arena,
                                        LlvmDebugModule* debug)
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

        u32  binding_index = llvm_hir_value_binding_index(hir, i);
        bool exported = binding_index != U32_MAX &&
                        llvm_hir_binding_is_exported(sema, hir, binding_index);
        llvm_append_symbol_name(sb, lex_symbol(lexer, symbol_handle));
        sb_append_cstr(sb, exported ? " = global " : " = internal global ");
        llvm_append_type(sb, sema, value->type_index);
        sb_append_cstr(sb, " ");
        llvm_append_zero_value(sb, sema, value->type_index);
        u32 debug_global =
            llvm_debug_global_variable(debug, hir, lexer, sema, i, exported);
        if (debug_global != 0) {
            sb_format(sb, ", !dbg !%u", debug_global);
        }
        sb_append_char(sb, '\n');
    }
    (void)arena;
}

internal void llvm_render_global_slice_backing_values(StringBuilder* sb,
                                                      const Hir*     hir,
                                                      const Lexer*   lexer,
                                                      const Sema*    sema)
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
        if (llvm_expr_is_constant_value(
                hir, lexer, sema, value->value_expr_index)) {
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
                                      const Hir*     hir,
                                      const Lexer*   lexer,
                                      const Sema*    sema,
                                      Arena*         arena)
{
    if (!llvm_hir_has_globals(hir)) {
        return;
    }

    sb_append_cstr(sb, "define void ");
    llvm_append_module_init_name(sb, hir);
    sb_append_cstr(sb, "() {\n");
    Arena temp        = {0};
    Arena entry_arena = {0};
    Arena body_arena  = {0};
    arena_init(&temp);
    arena_init(&entry_arena);
    arena_init(&body_arena);
    StringBuilder entry_sb = {0};
    StringBuilder body_sb  = {0};
    sb_init(&entry_sb, &entry_arena);
    sb_init(&body_sb, &body_arena);
    LlvmFunctionContext ctx = {
        .sb                      = &body_sb,
        .entry_sb                = &entry_sb,
        .hir                     = hir,
        .lexer                   = lexer,
        .sema                    = sema,
        .arena                   = &temp,
        .layout                  = llvm_default_layout(),
        .next_temp               = 0,
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
        LlvmValue init_value =
            llvm_emit_expr(&ctx, NULL, value->value_expr_index);
        ctx.global_init_value_index = U32_MAX;
        if (!init_value.ok) {
            continue;
        }

        string type = llvm_type_string(&ctx, value->type_index);
        sb_format(&body_sb,
                  "  store " STRINGP " " STRINGP ", ptr " STRINGP "\n",
                  STRINGV(type),
                  STRINGV(init_value.value),
                  STRINGV(name));
    }
    sb_append_cstr(&body_sb, "  ret void\n");
    sb_append_string(sb, sb_to_string(&entry_sb));
    sb_append_string(sb, sb_to_string(&body_sb));
    sb_append_cstr(sb, "}\n");
    array_free(ctx.locals);
    array_free(ctx.slots);
    array_free(ctx.assigned_locals);
    array_free(ctx.defer_block_indices);
    array_free(ctx.control_targets);
    arena_done(&body_arena);
    arena_done(&entry_arena);
    arena_done(&temp);
    (void)arena;
}

internal void llvm_render_function(StringBuilder*     sb,
                                   const Hir*         hir,
                                   const Lexer*       lexer,
                                   const Sema*        sema,
                                   Arena*             arena,
                                   LlvmDebugModule*   debug,
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

    u32 debug_scope_id = llvm_debug_add_function(
        debug, hir, lexer, sema, arena, function, function_index);

    sb_append_cstr(sb, "define ");
    if (!llvm_function_needs_external_definition(sema, hir, function_index)) {
        sb_append_cstr(sb, "internal ");
    }
    llvm_append_function_signature(
        sb, hir, lexer, sema, function, function_index);
    if (debug_scope_id != 0) {
        sb_format(sb, " !dbg !%u", debug_scope_id);
    }
    sb_append_cstr(sb, " {\n");
    Arena temp        = {0};
    Arena entry_arena = {0};
    Arena body_arena  = {0};
    arena_init(&temp);
    arena_init(&entry_arena);
    arena_init(&body_arena);
    StringBuilder entry_sb = {0};
    StringBuilder body_sb  = {0};
    sb_init(&entry_sb, &entry_arena);
    sb_init(&body_sb, &body_arena);
    LlvmFunctionContext ctx = {
        .sb                      = &body_sb,
        .entry_sb                = &entry_sb,
        .hir                     = hir,
        .lexer                   = lexer,
        .sema                    = sema,
        .arena                   = &temp,
        .layout                  = llvm_default_layout(),
        .next_temp               = 0,
        .global_init_value_index = U32_MAX,
        .debug                   = debug,
        .debug_scope_id          = debug_scope_id,
        .debug_decl_index        = function->decl_index,
    };
    llvm_collect_assigned_locals(&ctx, function->body_block_index);
    llvm_collect_addressed_locals(&ctx, function->body_block_index);
    bool emitted = llvm_initialise_assigned_param_slots(&ctx, function) &&
                   llvm_emit_param_debug_values(&ctx, function) &&
                   llvm_emit_block(&ctx, function, function->body_block_index);
    if (!emitted || !ctx.block_terminated) {
        u32 return_type = llvm_function_return_type(sema, function->type_index);
        llvm_append_default_return(&body_sb, sema, return_type);
    }
    sb_append_string(sb, sb_to_string(&entry_sb));
    sb_append_string(
        sb,
        llvm_debug_annotate_body(&body_arena, debug, sb_to_string(&body_sb)));
    array_free(ctx.locals);
    array_free(ctx.slots);
    array_free(ctx.assigned_locals);
    array_free(ctx.defer_block_indices);
    array_free(ctx.control_targets);
    arena_done(&body_arena);
    arena_done(&entry_arena);
    arena_done(&temp);
    sb_append_cstr(sb, "}\n");
    (void)arena;
}

internal void llvm_render_binding_alias(StringBuilder*    sb,
                                        const Hir*        hir,
                                        const Lexer*      lexer,
                                        const Sema*       sema,
                                        const HirBinding* binding,
                                        bool              exported,
                                        bool              entry_point)
{
    if (binding->kind == HIR_BINDING_Import &&
        binding->target_index < array_count(hir->imports) && exported) {
        const HirImport* import         = &hir->imports[binding->target_index];
        const Hir*       source_hir     = NULL;
        u32              function_index = U32_MAX;
        if (!llvm_import_source_function(
                sema, import, &source_hir, &function_index)) {
            return;
        }
        llvm_append_symbol_name(sb, lex_symbol(lexer, binding->symbol_handle));
        sb_append_cstr(sb, " = alias ");
        llvm_append_function_type(sb, sema, import->type_index);
        sb_append_cstr(sb, ", ptr ");
        llvm_append_generated_function_name(sb, source_hir, function_index);
        sb_append_char(sb, '\n');
        return;
    }

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
    if (!llvm_hir_is_root_module(sema, hir) &&
        llvm_root_exports_symbol(sema,
                                 lex_symbol(lexer, binding->symbol_handle))) {
        return;
    }
    if (sema != NULL && sema->program != NULL &&
        hir->current_module_index != sema->program->root_module_index &&
        llvm_program_function_symbol_conflicts(
            sema, hir, binding->target_index)) {
        return;
    }

    llvm_append_symbol_name(sb, lex_symbol(lexer, binding->symbol_handle));
    sb_append_cstr(
        sb, exported || entry_point ? " = alias " : " = internal alias ");
    llvm_append_function_type(sb, sema, function->type_index);
    sb_append_cstr(sb, ", ptr ");
    llvm_append_function_name(sb, hir, lexer, binding->target_index);
    sb_append_char(sb, '\n');
}

internal void llvm_render_ffi_export_wrapper(StringBuilder* sb,
                                             const Lexer*   lexer,
                                             const Sema*    sema,
                                             u32            nerd_symbol_handle,
                                             u32            ffi_symbol_handle,
                                             u32            type_index)
{
    if (nerd_symbol_handle == U32_MAX || ffi_symbol_handle == U32_MAX ||
        type_index >= array_count(sema->types) ||
        sema->types[type_index].kind != STK_Function) {
        return;
    }

    u32 return_type = llvm_function_return_type(sema, type_index);
    sb_append_cstr(sb, "define ");
    llvm_append_type(sb, sema, return_type);
    sb_append_char(sb, ' ');
    llvm_append_symbol_name(sb, lex_symbol(lexer, nerd_symbol_handle));
    sb_append_char(sb, '(');
    u32 param_count = llvm_function_param_count(sema, type_index);
    for (u32 i = 0; i < param_count; ++i) {
        if (i > 0) {
            sb_append_cstr(sb, ", ");
        }
        llvm_append_type(
            sb, sema, llvm_function_param_type(sema, type_index, i));
        sb_format(sb, " %%p%u", i);
    }
    sb_append_cstr(sb, ") {\n  ");
    if (!llvm_type_is_void(sema, return_type)) {
        sb_append_cstr(sb, "%t0 = ");
    }
    sb_append_cstr(sb, "call ");
    llvm_append_type(sb, sema, return_type);
    sb_append_char(sb, ' ');
    llvm_append_c_symbol_name(sb, lex_symbol(lexer, ffi_symbol_handle));
    sb_append_char(sb, '(');
    for (u32 i = 0; i < param_count; ++i) {
        if (i > 0) {
            sb_append_cstr(sb, ", ");
        }
        llvm_append_type(
            sb, sema, llvm_function_param_type(sema, type_index, i));
        sb_format(sb, " %%p%u", i);
    }
    sb_append_cstr(sb, ")\n");
    if (llvm_type_is_void(sema, return_type)) {
        sb_append_cstr(sb, "  ret void\n}\n");
        return;
    }
    sb_append_cstr(sb, "  ret ");
    llvm_append_type(sb, sema, return_type);
    sb_append_cstr(sb, " %t0\n}\n");
}

internal void llvm_render_export_wrapper(StringBuilder*    sb,
                                         const Hir*        hir,
                                         const Lexer*      lexer,
                                         const Sema*       sema,
                                         const HirBinding* binding,
                                         bool              exported)
{
    if (!exported) {
        return;
    }
    if (binding->kind == HIR_BINDING_Function &&
        binding->target_index < array_count(hir->functions)) {
        const HirFunction* function = &hir->functions[binding->target_index];
        if (function->kind == HIR_FUNCTION_Ffi) {
            llvm_render_ffi_export_wrapper(sb,
                                           lexer,
                                           sema,
                                           binding->symbol_handle,
                                           function->ffi_symbol_handle,
                                           function->type_index);
        }
        return;
    }
    if (binding->kind == HIR_BINDING_Import &&
        binding->target_index < array_count(hir->imports)) {
        const HirImport* import = &hir->imports[binding->target_index];
        llvm_render_ffi_export_wrapper(sb,
                                       lexer,
                                       sema,
                                       binding->symbol_handle,
                                       import->ffi_symbol_handle,
                                       import->type_index);
    }
}

string llvm_render_hir(const Hir*   hir,
                       const Lexer* lexer,
                       const Sema*  sema,
                       Arena*       arena,
                       bool         emit_debug)
{
    Sema          render_sema_storage = llvm_prepare_render_sema(sema);
    const Sema*   render_sema         = &render_sema_storage;
    StringBuilder sb                  = {0};
    sb_init(&sb, arena);
    LlvmDebugModule  debug_storage = {0};
    LlvmDebugModule* debug         = NULL;
    if (emit_debug) {
        debug = &debug_storage;
        llvm_debug_init(debug, arena, lexer, llvm_hir_has_globals(hir));
    }

    sb_append_cstr(&sb, "; nerd llvm-ir 0\n");
    sb_append_cstr(&sb, "; generated from HIR\n\n");

    llvm_render_builtin_macro_globals(&sb, hir, lexer);
    if (array_count(lexer->strings) > 0) {
        llvm_render_string_literals(&sb, hir, lexer);
        llvm_render_concat_string_literals(&sb, hir, lexer, arena);
    }
    (void)llvm_render_const_slice_backing_values(
        &sb, hir, lexer, render_sema, arena);
    if (llvm_hir_uses_assert(hir)) {
        llvm_render_assert_globals(&sb, hir, lexer);
    }
    sb_append_char(&sb, '\n');

    bool uses_string_runtime = llvm_hir_uses_string_runtime(hir, render_sema);
    bool uses_assert_runtime = llvm_hir_uses_assert(hir);
    bool uses_dynamic_array_runtime =
        llvm_hir_uses_dynamic_array_runtime(hir, render_sema);
    bool uses_arena_runtime =
        llvm_hir_uses_arena_runtime(hir, lexer, render_sema);

    if (uses_string_runtime) {
        llvm_render_string_runtime_declarations(&sb);
    }
    if (uses_assert_runtime) {
        llvm_render_assert_runtime_declarations(&sb);
    }
    if (uses_dynamic_array_runtime) {
        llvm_render_dynamic_array_runtime_declarations(&sb);
    }
    if (uses_arena_runtime) {
        llvm_render_arena_runtime_declarations(&sb);
    }
    if (uses_string_runtime || uses_assert_runtime ||
        uses_dynamic_array_runtime || uses_arena_runtime) {
        sb_append_char(&sb, '\n');
    }

    bool rendered_import = false;
    for (u32 i = 0; i < array_count(hir->imports); ++i) {
        bool already_rendered = false;
        for (u32 j = 0; j < i; ++j) {
            if (llvm_imports_same_runtime_symbol(&hir->imports[i],
                                                 &hir->imports[j])) {
                already_rendered = true;
                break;
            }
        }
        if (already_rendered) {
            continue;
        }
        usize before = sb.size;
        llvm_render_import(&sb, hir, lexer, render_sema, &hir->imports[i]);
        rendered_import = rendered_import || sb.size != before;
    }
    {
        usize before = sb.size;
        llvm_render_imported_generic_function_declarations(
            &sb, hir, render_sema);
        rendered_import = rendered_import || sb.size != before;
    }
    if (rendered_import) {
        sb_append_char(&sb, '\n');
    }

    if (llvm_hir_has_globals(hir)) {
        llvm_render_global_slice_backing_values(&sb, hir, lexer, render_sema);
        llvm_render_global_values(&sb, hir, lexer, render_sema, arena, debug);
        sb_append_char(&sb, '\n');
        llvm_render_global_init(&sb, hir, lexer, render_sema, arena);
        sb_append_char(&sb, '\n');
    }

    for (u32 i = 0; i < array_count(hir->functions); ++i) {
        llvm_render_function(
            &sb, hir, lexer, render_sema, arena, debug, &hir->functions[i], i);
        sb_append_char(&sb, '\n');
    }

    for (u32 i = 0; i < array_count(hir->bindings); ++i) {
        const HirBinding* binding = &hir->bindings[i];
        bool exported    = llvm_hir_binding_is_exported(render_sema, hir, i);
        bool entry_point = llvm_symbol_is_main(lexer, binding->symbol_handle);
        llvm_render_binding_alias(
            &sb, hir, lexer, render_sema, binding, exported, entry_point);
        llvm_render_export_wrapper(
            &sb, hir, lexer, render_sema, binding, exported);
    }
    if (array_count(hir->bindings) > 0) {
        sb_append_char(&sb, '\n');
    }

    if (debug != NULL && debug->uses_dbg_declare) {
        sb_append_cstr(
            &sb,
            "declare void @llvm.dbg.declare(metadata, metadata, metadata)\n");
    }
    if (debug != NULL && debug->uses_dbg_value) {
        sb_append_cstr(
            &sb,
            "declare void @llvm.dbg.value(metadata, metadata, metadata)\n");
    }
    if (debug != NULL && (debug->uses_dbg_declare || debug->uses_dbg_value)) {
        sb_append_char(&sb, '\n');
    }

    if (debug != NULL) {
        llvm_debug_emit_global_list(debug);
        sb_append_string(&sb, sb_to_string(&debug->metadata));
    }

    string rendered = sb_to_string(&sb);
    if (debug != NULL) {
        llvm_debug_done(debug);
    }
    llvm_render_sema_done(&render_sema_storage);
    return rendered;
}

bool llvm_save_hir(const Hir*   hir,
                   const Lexer* lexer,
                   const Sema*  sema,
                   cstr         path)
{
    Arena arena = {0};
    arena_init(&arena);
    string rendered = llvm_render_hir(hir, lexer, sema, &arena, true);

    FILE* file      = fopen(path, "wb");
    if (!file) {
        arena_done(&arena);
        return error_runtime("Failed to open LLVM IR file for writing: %s",
                             path);
    }

    usize written      = fwrite(rendered.data, 1, rendered.count, file);
    bool  close_failed = fclose(file) != 0;
    arena_done(&arena);

    if (written != rendered.count || close_failed) {
        return error_runtime("Failed to write LLVM IR file: %s", path);
    }
    return true;
}

//------------------------------------------------------------------------------
