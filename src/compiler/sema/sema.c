//------------------------------------------------------------------------------
// Semantic analysis implementation
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/build/build.h>
#include <compiler/error/error.h>
#include <compiler/modules/modules.h>
#include <compiler/sema/sema.h>

//------------------------------------------------------------------------------
// Return sentinel semantic indices used for unresolved entries.

u32 sema_no_decl(void) { return U32_MAX; }

u32 sema_no_local(void) { return U32_MAX; }

internal u32 sema_no_scope(void) { return U32_MAX; }

//------------------------------------------------------------------------------
// Return the sentinel type index used for unresolved entries.

u32 sema_no_type(void) { return U32_MAX; }

//------------------------------------------------------------------------------
// Predeclare the current built-in runtime functions.

internal u32       sema_builtin_type(Sema* sema, SemaTypeKind kind);
internal u32       sema_type_index_for_name(Sema* sema, string name);
internal u32       sema_find_decl(const Sema* sema, u32 symbol_handle);
internal ErrorSpan sema_decl_span(const Lexer*    lexer,
                                  const Ast*      ast,
                                  const SemaDecl* decl);

internal u32 sema_find_symbol_handle_by_name(const Lexer* lexer, string name)
{
    for (u32 i = 0; i < array_count(lexer->symbol_handles); ++i) {
        u32 handle = lexer->symbol_handles[i];
        if (string_eq(lex_symbol(lexer, handle), name)) {
            return handle;
        }
    }
    return sema_no_decl();
}

u32 sema_import_symbol_handle(Lexer*       dst_lexer,
                              const Lexer* src_lexer,
                              u32          src_symbol_handle)
{
    InternAddResult added = {0};
    return lex_add_symbol(
        dst_lexer, lex_symbol(src_lexer, src_symbol_handle), &added);
}

//------------------------------------------------------------------------------
// Add one semantic type row and return its canonical index.

internal u32 sema_add_type(Sema* sema, SemaType type)
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

internal u32 sema_add_function_type_ex(Sema* sema,
                                       Array(u32) param_types,
                                       u32 return_type,
                                       u16 flags)
{
    for (u32 i = 0; i < array_count(sema->types); ++i) {
        const SemaType* existing = &sema->types[i];
        if (existing->kind != STK_Function ||
            existing->param_count != array_count(param_types) ||
            existing->return_type != return_type || existing->flags != flags) {
            continue;
        }

        bool matches = true;
        for (u32 j = 0; j < existing->param_count; ++j) {
            if (sema->type_param_types[existing->first_param_type + j] !=
                param_types[j]) {
                matches = false;
                break;
            }
        }
        if (matches) {
            return i;
        }
    }

    u32 first_param = (u32)array_count(sema->type_param_types);
    for (u32 i = 0; i < array_count(param_types); ++i) {
        array_push(sema->type_param_types, param_types[i]);
        array_push(sema->type_param_symbols, U32_MAX);
    }

    return sema_add_type(sema,
                         (SemaType){
                             .kind             = STK_Function,
                             .param_count      = (u16)array_count(param_types),
                             .flags            = flags,
                             .first_param_type = first_param,
                             .return_type      = return_type,
                         });
}

internal u32 sema_add_function_type(Sema* sema,
                                    Array(u32) param_types,
                                    u32 return_type)
{
    return sema_add_function_type_ex(sema, param_types, return_type, STF_None);
}

internal u32 sema_add_module_type_raw(Sema*      sema,
                                      const u32* export_symbols,
                                      const u32* export_types,
                                      u32        export_count,
                                      u32        module_index)
{
    for (u32 i = 0; i < array_count(sema->types); ++i) {
        const SemaType* existing = &sema->types[i];
        if (existing->kind != STK_Module ||
            existing->param_count != export_count ||
            existing->return_type != module_index) {
            continue;
        }

        bool matches = true;
        for (u32 j = 0; j < export_count; ++j) {
            if (sema->type_param_symbols[existing->first_param_type + j] !=
                    export_symbols[j] ||
                sema->type_param_types[existing->first_param_type + j] !=
                    export_types[j]) {
                matches = false;
                break;
            }
        }
        if (matches) {
            return i;
        }
    }

    u32 first_export = (u32)array_count(sema->type_param_types);
    for (u32 i = 0; i < export_count; ++i) {
        array_push(sema->type_param_types, export_types[i]);
        array_push(sema->type_param_symbols, export_symbols[i]);
    }

    return sema_add_type(sema,
                         (SemaType){
                             .kind             = STK_Module,
                             .param_count      = (u16)export_count,
                             .first_param_type = first_export,
                             .return_type      = module_index,
                         });
}

internal u32 sema_add_tuple_type(Sema* sema, Array(u32) item_types)
{
    for (u32 i = 0; i < array_count(sema->types); ++i) {
        const SemaType* existing = &sema->types[i];
        if (existing->kind != STK_Tuple ||
            existing->param_count != array_count(item_types)) {
            continue;
        }

        bool matches = true;
        for (u32 j = 0; j < existing->param_count; ++j) {
            if (sema->type_param_types[existing->first_param_type + j] !=
                item_types[j]) {
                matches = false;
                break;
            }
        }
        if (matches) {
            return i;
        }
    }

    u32 first_item = (u32)array_count(sema->type_param_types);
    for (u32 i = 0; i < array_count(item_types); ++i) {
        array_push(sema->type_param_types, item_types[i]);
        array_push(sema->type_param_symbols, U32_MAX);
    }

    return sema_add_type(sema,
                         (SemaType){
                             .kind             = STK_Tuple,
                             .param_count      = (u16)array_count(item_types),
                             .first_param_type = first_item,
                             .return_type      = sema_no_type(),
                         });
}

internal u32 sema_add_array_type(Sema* sema, u32 item_type, u32 item_count)
{
    return sema_add_type(sema,
                         (SemaType){
                             .kind             = STK_Array,
                             .param_count      = 0,
                             .first_param_type = item_type,
                             .return_type      = item_count,
                         });
}

internal u32 sema_add_slice_type(Sema* sema, u32 item_type)
{
    return sema_add_type(sema,
                         (SemaType){
                             .kind             = STK_Slice,
                             .param_count      = 0,
                             .first_param_type = item_type,
                             .return_type      = sema_no_type(),
                         });
}

internal u32 sema_add_pointer_type(Sema* sema, u32 pointee_type)
{
    return sema_add_type(sema,
                         (SemaType){
                             .kind             = STK_Pointer,
                             .param_count      = 0,
                             .first_param_type = pointee_type,
                             .return_type      = sema_no_type(),
                         });
}

internal u32 sema_add_record_type_raw(Sema*        sema,
                                      SemaTypeKind kind,
                                      const u32*   field_symbols,
                                      const u32*   field_types,
                                      u32          field_count,
                                      u16          flags)
{
    for (u32 i = 0; i < array_count(sema->types); ++i) {
        const SemaType* existing = &sema->types[i];
        if (existing->kind != kind || existing->param_count != field_count ||
            existing->flags != flags) {
            continue;
        }
        bool matches = true;
        for (u32 j = 0; j < field_count; ++j) {
            if (sema->type_param_symbols[existing->first_param_type + j] !=
                    field_symbols[j] ||
                sema->type_param_types[existing->first_param_type + j] !=
                    field_types[j]) {
                matches = false;
                break;
            }
        }
        if (matches) {
            return i;
        }
    }

    u32 first_field = (u32)array_count(sema->type_param_types);
    for (u32 i = 0; i < field_count; ++i) {
        array_push(sema->type_param_types, field_types[i]);
        array_push(sema->type_param_symbols, field_symbols[i]);
    }
    return sema_add_type(sema,
                         (SemaType){
                             .kind             = kind,
                             .param_count      = (u16)field_count,
                             .flags            = flags,
                             .first_param_type = first_field,
                             .return_type      = sema_no_type(),
                         });
}

internal u32 sema_add_plex_type_raw(Sema*      sema,
                                    const u32* field_symbols,
                                    const u32* field_types,
                                    u32        field_count,
                                    u16        flags)
{
    return sema_add_record_type_raw(
        sema, STK_Plex, field_symbols, field_types, field_count, flags);
}

internal u32 sema_add_union_type_raw(Sema*      sema,
                                     const u32* field_symbols,
                                     const u32* field_types,
                                     u32        field_count)
{
    return sema_add_record_type_raw(
        sema, STK_Union, field_symbols, field_types, field_count, STF_None);
}

internal u32 sema_add_plex_type(Sema*               sema,
                                const AstPlexField* fields,
                                Array(u32) field_types,
                                u32 field_count,
                                u32 flags)
{
    Array(u32) field_symbols = NULL;
    for (u32 i = 0; i < field_count; ++i) {
        array_push(field_symbols, fields[i].symbol_handle);
    }
    u32 type_index = sema_add_plex_type_raw(
        sema, field_symbols, field_types, field_count, (u16)flags);
    array_free(field_symbols);
    return type_index;
}

internal u32 sema_add_union_type(Sema*               sema,
                                 const AstPlexField* fields,
                                 Array(u32) field_types,
                                 u32 field_count)
{
    Array(u32) field_symbols = NULL;
    for (u32 i = 0; i < field_count; ++i) {
        array_push(field_symbols, fields[i].symbol_handle);
    }
    u32 type_index =
        sema_add_union_type_raw(sema, field_symbols, field_types, field_count);
    array_free(field_symbols);
    return type_index;
}

internal u32 sema_add_enum_type_raw(Sema*      sema,
                                    const u32* variants,
                                    const u32* payload_types,
                                    u32        count)
{
    for (u32 i = 0; i < array_count(sema->types); ++i) {
        const SemaType* existing = &sema->types[i];
        if (existing->kind != STK_Enum || existing->param_count != count) {
            continue;
        }
        bool matches = true;
        for (u32 j = 0; j < count; ++j) {
            if (sema->type_param_symbols[existing->first_param_type + j] !=
                variants[j]) {
                matches = false;
                break;
            }
            if (sema->type_param_types[existing->first_param_type + j] !=
                payload_types[j]) {
                matches = false;
                break;
            }
        }
        if (matches) {
            return i;
        }
    }

    u32 first_variant = (u32)array_count(sema->type_param_symbols);
    for (u32 i = 0; i < count; ++i) {
        array_push(sema->type_param_symbols, variants[i]);
        array_push(sema->type_param_types, payload_types[i]);
    }
    return sema_add_type(sema,
                         (SemaType){
                             .kind             = STK_Enum,
                             .param_count      = (u16)count,
                             .first_param_type = first_variant,
                             .return_type      = sema_no_type(),
                         });
}

internal u32 sema_add_enum_type(Sema*                 sema,
                                const AstEnumVariant* variants,
                                Array(u32) payload_types,
                                u32 count)
{
    Array(u32) symbols = NULL;
    for (u32 i = 0; i < count; ++i) {
        array_push(symbols, variants[i].symbol_handle);
    }
    u32 type_index =
        sema_add_enum_type_raw(sema, symbols, payload_types, count);
    array_free(symbols);
    return type_index;
}

u32 sema_import_type(Lexer*       dst_lexer,
                     Sema*        dst_sema,
                     const Lexer* src_lexer,
                     const Sema*  src_sema,
                     u32          src_type_index)
{
    if (src_type_index == sema_no_type()) {
        return sema_no_type();
    }

    const SemaType* src_type = &src_sema->types[src_type_index];
    switch (src_type->kind) {
    case STK_Void:
    case STK_UntypedInteger:
    case STK_UntypedFloat:
    case STK_String:
    case STK_Bool:
    case STK_I8:
    case STK_I16:
    case STK_I32:
    case STK_I64:
    case STK_U8:
    case STK_U16:
    case STK_U32:
    case STK_U64:
    case STK_F32:
    case STK_F64:
    case STK_Isize:
    case STK_Usize:
        return sema_builtin_type(dst_sema, src_type->kind);

    case STK_Function:
        {
            Array(u32) param_types = NULL;
            for (u32 i = 0; i < src_type->param_count; ++i) {
                array_push(
                    param_types,
                    sema_import_type(
                        dst_lexer,
                        dst_sema,
                        src_lexer,
                        src_sema,
                        src_sema->type_param_types[src_type->first_param_type +
                                                   i]));
            }
            u32 return_type = sema_import_type(dst_lexer,
                                               dst_sema,
                                               src_lexer,
                                               src_sema,
                                               src_type->return_type);
            u32 imported    = sema_add_function_type_ex(
                dst_sema, param_types, return_type, src_type->flags);
            array_free(param_types);
            return imported;
        }

    case STK_Module:
        {
            Array(u32) export_symbols = NULL;
            Array(u32) export_types   = NULL;
            for (u32 i = 0; i < src_type->param_count; ++i) {
                array_push(export_symbols,
                           sema_import_symbol_handle(
                               dst_lexer,
                               src_lexer,
                               src_sema->type_param_symbols
                                   [src_type->first_param_type + i]));
                array_push(
                    export_types,
                    sema_import_type(
                        dst_lexer,
                        dst_sema,
                        src_lexer,
                        src_sema,
                        src_sema->type_param_types[src_type->first_param_type +
                                                   i]));
            }
            u32 imported =
                sema_add_module_type_raw(dst_sema,
                                         export_symbols,
                                         export_types,
                                         (u32)array_count(export_symbols),
                                         src_type->return_type);
            array_free(export_symbols);
            array_free(export_types);
            return imported;
        }

    case STK_Tuple:
        {
            Array(u32) item_types = NULL;
            for (u32 i = 0; i < src_type->param_count; ++i) {
                array_push(
                    item_types,
                    sema_import_type(
                        dst_lexer,
                        dst_sema,
                        src_lexer,
                        src_sema,
                        src_sema->type_param_types[src_type->first_param_type +
                                                   i]));
            }
            u32 imported = sema_add_tuple_type(dst_sema, item_types);
            array_free(item_types);
            return imported;
        }

    case STK_Array:
        return sema_add_array_type(dst_sema,
                                   sema_import_type(dst_lexer,
                                                    dst_sema,
                                                    src_lexer,
                                                    src_sema,
                                                    src_type->first_param_type),
                                   src_type->return_type);

    case STK_Slice:
        return sema_add_slice_type(
            dst_sema,
            sema_import_type(dst_lexer,
                             dst_sema,
                             src_lexer,
                             src_sema,
                             src_type->first_param_type));

    case STK_Pointer:
        return sema_add_pointer_type(
            dst_sema,
            sema_import_type(dst_lexer,
                             dst_sema,
                             src_lexer,
                             src_sema,
                             src_type->first_param_type));

    case STK_Plex:
    case STK_Union:
        {
            Array(u32) field_symbols = NULL;
            Array(u32) field_types   = NULL;
            for (u32 i = 0; i < src_type->param_count; ++i) {
                array_push(field_symbols,
                           sema_import_symbol_handle(
                               dst_lexer,
                               src_lexer,
                               src_sema->type_param_symbols
                                   [src_type->first_param_type + i]));
                array_push(
                    field_types,
                    sema_import_type(
                        dst_lexer,
                        dst_sema,
                        src_lexer,
                        src_sema,
                        src_sema->type_param_types[src_type->first_param_type +
                                                   i]));
            }
            u32 imported =
                src_type->kind == STK_Plex
                    ? sema_add_plex_type_raw(dst_sema,
                                             field_symbols,
                                             field_types,
                                             (u32)array_count(field_symbols),
                                             src_type->flags)
                    : sema_add_union_type_raw(dst_sema,
                                              field_symbols,
                                              field_types,
                                              (u32)array_count(field_symbols));
            array_free(field_symbols);
            array_free(field_types);
            return imported;
        }

    case STK_Enum:
        {
            Array(u32) variants      = NULL;
            Array(u32) payload_types = NULL;
            for (u32 i = 0; i < src_type->param_count; ++i) {
                array_push(variants,
                           sema_import_symbol_handle(
                               dst_lexer,
                               src_lexer,
                               src_sema->type_param_symbols
                                   [src_type->first_param_type + i]));
                array_push(
                    payload_types,
                    sema_import_type(
                        dst_lexer,
                        dst_sema,
                        src_lexer,
                        src_sema,
                        src_sema->type_param_types[src_type->first_param_type +
                                                   i]));
            }
            u32 imported = sema_add_enum_type_raw(
                dst_sema, variants, payload_types, (u32)array_count(variants));
            array_free(variants);
            array_free(payload_types);
            return imported;
        }
    }

    return sema_no_type();
}

//------------------------------------------------------------------------------
// Return the canonical type index for one built-in type.

internal u32 sema_builtin_type(Sema* sema, SemaTypeKind kind)
{
    return sema_add_type(sema,
                         (SemaType){
                             .kind             = kind,
                             .param_count      = 0,
                             .first_param_type = 0,
                             .return_type      = sema_no_type(),
                         });
}

//------------------------------------------------------------------------------
// Return whether one type kind is integer-like.

internal bool sema_type_kind_is_integer(SemaTypeKind kind)
{
    switch (kind) {
    case STK_UntypedInteger:
    case STK_I8:
    case STK_I16:
    case STK_I32:
    case STK_I64:
    case STK_U8:
    case STK_U16:
    case STK_U32:
    case STK_U64:
    case STK_Isize:
    case STK_Usize:
        return true;
    default:
        return false;
    }
}

//------------------------------------------------------------------------------
// Return whether one semantic type is integer-like.

bool sema_type_is_integer(const Sema* sema, u32 type_index)
{
    return type_index != sema_no_type() &&
           sema_type_kind_is_integer(sema->types[type_index].kind);
}

//------------------------------------------------------------------------------
// Return whether one semantic type is a concrete integer type.

bool sema_type_is_concrete_integer(const Sema* sema, u32 type_index)
{
    return sema_type_is_integer(sema, type_index) &&
           sema->types[type_index].kind != STK_UntypedInteger;
}

internal bool sema_type_kind_is_float(SemaTypeKind kind)
{
    return kind == STK_UntypedFloat || kind == STK_F32 || kind == STK_F64;
}

bool sema_type_is_float(const Sema* sema, u32 type_index)
{
    return type_index != sema_no_type() &&
           sema_type_kind_is_float(sema->types[type_index].kind);
}

internal bool sema_type_is_concrete_float(const Sema* sema, u32 type_index)
{
    return sema_type_is_float(sema, type_index) &&
           sema->types[type_index].kind != STK_UntypedFloat;
}

internal bool sema_type_is_numeric(const Sema* sema, u32 type_index)
{
    return sema_type_is_integer(sema, type_index) ||
           sema_type_is_float(sema, type_index);
}

//------------------------------------------------------------------------------
// Return whether one semantic type is an unsigned concrete integer type.

internal bool sema_type_is_unsigned_integer(const Sema* sema, u32 type_index)
{
    if (type_index == sema_no_type()) {
        return false;
    }

    switch (sema->types[type_index].kind) {
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

//------------------------------------------------------------------------------
// Materialise abstract semantic types for runtime-facing uses.

u32 sema_materialise_type(const Sema* sema, u32 type_index)
{
    if (type_index == sema_no_type()) {
        return type_index;
    }

    if (sema->types[type_index].kind == STK_UntypedInteger) {
        for (u32 i = 0; i < array_count(sema->types); ++i) {
            if (sema->types[i].kind == STK_I32) {
                return i;
            }
        }
    }

    if (sema->types[type_index].kind == STK_UntypedFloat) {
        for (u32 i = 0; i < array_count(sema->types); ++i) {
            if (sema->types[i].kind == STK_F64) {
                return i;
            }
        }
    }

    if (sema->types[type_index].kind == STK_Array) {
        const SemaType* array = &sema->types[type_index];
        return sema_add_array_type(
            (Sema*)sema,
            sema_materialise_type(sema, array->first_param_type),
            array->return_type);
    }

    if (sema->types[type_index].kind == STK_Slice) {
        const SemaType* slice = &sema->types[type_index];
        return sema_add_slice_type(
            (Sema*)sema, sema_materialise_type(sema, slice->first_param_type));
    }

    if (sema->types[type_index].kind == STK_Pointer) {
        const SemaType* pointer = &sema->types[type_index];
        return sema_add_pointer_type(
            (Sema*)sema,
            sema_materialise_type(sema, pointer->first_param_type));
    }

    if (sema->types[type_index].kind == STK_Plex ||
        sema->types[type_index].kind == STK_Union) {
        Array(u32) field_types   = NULL;
        Array(u32) field_symbols = NULL;
        const SemaType* record   = &sema->types[type_index];
        for (u32 i = 0; i < record->param_count; ++i) {
            array_push(
                field_types,
                sema_materialise_type(
                    sema,
                    sema->type_param_types[record->first_param_type + i]));
            array_push(field_symbols,
                       sema->type_param_symbols[record->first_param_type + i]);
        }
        u32 materialised = record->kind == STK_Plex
                               ? sema_add_plex_type_raw((Sema*)sema,
                                                        field_symbols,
                                                        field_types,
                                                        record->param_count,
                                                        record->flags)
                               : sema_add_union_type_raw((Sema*)sema,
                                                         field_symbols,
                                                         field_types,
                                                         record->param_count);
        array_free(field_types);
        array_free(field_symbols);
        return materialised;
    }

    if (sema->types[type_index].kind == STK_Enum) {
        const SemaType* enum_type = &sema->types[type_index];
        Array(u32) variants       = NULL;
        Array(u32) payload_types  = NULL;
        for (u32 i = 0; i < enum_type->param_count; ++i) {
            array_push(
                variants,
                sema->type_param_symbols[enum_type->first_param_type + i]);
            u32 payload_type =
                sema->type_param_types[enum_type->first_param_type + i];
            array_push(payload_types,
                       payload_type == sema_no_type()
                           ? sema_no_type()
                           : sema_materialise_type(sema, payload_type));
        }
        u32 materialised = sema_add_enum_type_raw(
            (Sema*)sema, variants, payload_types, enum_type->param_count);
        array_free(variants);
        array_free(payload_types);
        return materialised;
    }

    if (sema->types[type_index].kind != STK_Tuple) {
        return type_index;
    }

    Array(u32) items      = NULL;
    const SemaType* tuple = &sema->types[type_index];
    for (u32 i = 0; i < tuple->param_count; ++i) {
        array_push(
            items,
            sema_materialise_type(
                sema, sema->type_param_types[tuple->first_param_type + i]));
    }
    u32 materialised = sema_add_tuple_type((Sema*)sema, items);
    array_free(items);
    return materialised;
}

//------------------------------------------------------------------------------
// Render one semantic type as source-facing text.

string sema_type_name(const Lexer* lexer,
                      const Sema*  sema,
                      Arena*       arena,
                      u32          type_index)
{
    if (type_index == sema_no_type()) {
        return s("<unknown>");
    }

    const SemaType* type = &sema->types[type_index];
    switch (type->kind) {
    case STK_Void:
        return s("void");
    case STK_UntypedInteger:
        return s("untyped integer");
    case STK_UntypedFloat:
        return s("untyped float");
    case STK_String:
        return s("string");
    case STK_Bool:
        return s("bool");
    case STK_I8:
        return s("i8");
    case STK_I16:
        return s("i16");
    case STK_I32:
        return s("i32");
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
        return s("f64");
    case STK_Isize:
        return s("isize");
    case STK_Usize:
        return s("usize");
    case STK_Function:
        {
            StringBuilder sb = {0};
            sb_init(&sb, arena);
            sb_append_cstr(&sb, "fn (");
            for (u32 i = 0; i < type->param_count; ++i) {
                if (i > 0) {
                    sb_append_cstr(&sb, ", ");
                }
                sb_append_string(
                    &sb,
                    sema_type_name(
                        lexer,
                        sema,
                        arena,
                        sema->type_param_types[type->first_param_type + i]));
            }
            if (type->flags & STF_FunctionVarargs) {
                if (type->param_count > 0) {
                    sb_append_cstr(&sb, ", ");
                }
                sb_append_cstr(&sb, "...");
            }
            sb_append_cstr(&sb, ") -> ");
            sb_append_string(
                &sb, sema_type_name(lexer, sema, arena, type->return_type));
            return sb_to_string(&sb);
        }
    case STK_Module:
        return s("module");
    case STK_Tuple:
        {
            StringBuilder sb = {0};
            sb_init(&sb, arena);
            sb_append_cstr(&sb, "(");
            for (u32 i = 0; i < type->param_count; ++i) {
                if (i > 0) {
                    sb_append_cstr(&sb, ", ");
                }
                sb_append_string(
                    &sb,
                    sema_type_name(
                        lexer,
                        sema,
                        arena,
                        sema->type_param_types[type->first_param_type + i]));
            }
            if (type->param_count == 1) {
                sb_append_cstr(&sb, ",");
            }
            sb_append_cstr(&sb, ")");
            return sb_to_string(&sb);
        }
    case STK_Array:
        {
            StringBuilder sb = {0};
            sb_init(&sb, arena);
            sb_format(&sb, "[%u]", type->return_type);
            sb_append_string(
                &sb,
                sema_type_name(lexer, sema, arena, type->first_param_type));
            return sb_to_string(&sb);
        }
    case STK_Slice:
        {
            StringBuilder sb = {0};
            sb_init(&sb, arena);
            sb_append_cstr(&sb, "[]");
            sb_append_string(
                &sb,
                sema_type_name(lexer, sema, arena, type->first_param_type));
            return sb_to_string(&sb);
        }
    case STK_Pointer:
        {
            StringBuilder sb = {0};
            sb_init(&sb, arena);
            sb_append_char(&sb, '^');
            sb_append_string(
                &sb,
                sema_type_name(lexer, sema, arena, type->first_param_type));
            return sb_to_string(&sb);
        }
    case STK_Plex:
    case STK_Union:
        {
            StringBuilder sb = {0};
            sb_init(&sb, arena);
            sb_append_cstr(&sb,
                           type->kind == STK_Plex ? "plex { " : "union { ");
            for (u32 i = 0; i < type->param_count; ++i) {
                if (i > 0) {
                    sb_append_cstr(&sb, ", ");
                }
                sb_append_string(
                    &sb,
                    sema_type_name(
                        lexer,
                        sema,
                        arena,
                        sema->type_param_types[type->first_param_type + i]));
                sb_append_char(&sb, ' ');
                sb_append_string(
                    &sb,
                    lex_symbol(
                        lexer,
                        sema->type_param_symbols[type->first_param_type + i]));
            }
            sb_append_cstr(&sb, " }");
            return sb_to_string(&sb);
        }
    case STK_Enum:
        {
            StringBuilder sb = {0};
            sb_init(&sb, arena);
            sb_append_cstr(&sb, "enum { ");
            for (u32 i = 0; i < type->param_count; ++i) {
                if (i > 0) {
                    sb_append_cstr(&sb, ", ");
                }
                sb_append_string(
                    &sb,
                    lex_symbol(
                        lexer,
                        sema->type_param_symbols[type->first_param_type + i]));
                u32 payload_type =
                    sema->type_param_types[type->first_param_type + i];
                if (payload_type != sema_no_type()) {
                    sb_append_char(&sb, '(');
                    sb_append_string(
                        &sb, sema_type_name(lexer, sema, arena, payload_type));
                    sb_append_char(&sb, ')');
                }
            }
            sb_append_cstr(&sb, " }");
            return sb_to_string(&sb);
        }
    default:
        return s("<unknown>");
    }
}

internal const AstFnSignature*
sema_ast_signature(const Ast* ast, const AstNode* signature_owner)
{
    return &ast->fn_signatures[signature_owner->a];
}

internal ErrorSpan sema_node_span(const Lexer* lexer, const AstNode* node);
internal ErrorSpan sema_local_span(const Lexer*     lexer,
                                   const Ast*       ast,
                                   const SemaLocal* local);
internal u32       sema_find_local_in_scope(const Sema* sema,
                                            u32         scope_index,
                                            u32         symbol_handle);
internal bool      sema_infer_node_type(const Lexer* lexer,
                                        const Ast*   ast,
                                        Sema*        sema,
                                        u32          node_index,
                                        u32          expected_type,
                                        u32*         out_type_index);
internal bool      sema_resolve_node_refs(const Lexer* lexer,
                                          const Ast*   ast,
                                          u32          owner_decl_index,
                                          u32          current_function_symbol,
                                          u32          capture_scope_index,
                                          u32          scope_index,
                                          u32          node_index,
                                          Sema*        sema);

internal u32 sema_find_program_module_by_path(const ProgramInfo* program,
                                              cstr               resolved_path)
{
    for (u32 i = 0; i < array_count(program->modules); ++i) {
        if (strcmp(program->modules[i].resolved_path, resolved_path) == 0) {
            return i;
        }
    }
    return U32_MAX;
}

internal bool sema_resolve_loaded_module(const Lexer* lexer,
                                         const Ast*   ast,
                                         Sema*        sema,
                                         u32          module_path_index,
                                         ErrorSpan    span,
                                         u32*         out_type_index)
{
    if (sema->program == NULL) {
        return error_0304_type_mismatch(
            lexer->source, span, s("known module"), s("module path"));
    }

    Arena arena = {0};
    arena_init(&arena);
    ModuleResolveResult resolved = {0};
    ModuleResolveStatus status =
        module_resolve_path(&arena,
                            sema->program->root_source,
                            lexer,
                            ast,
                            &ast->module_paths[module_path_index],
                            &resolved);
    if (status != MRS_Found) {
        arena_done(&arena);
        return error_0304_type_mismatch(
            lexer->source, span, s("known module"), s("module path"));
    }

    u32 module_index =
        sema_find_program_module_by_path(sema->program, resolved.resolved_path);
    if (module_index == U32_MAX) {
        arena_done(&arena);
        return error_0304_type_mismatch(
            lexer->source, span, s("known module"), s("module path"));
    }

    const ModuleInfo* module  = &sema->program->modules[module_index];
    Array(u32) export_symbols = NULL;
    Array(u32) export_types   = NULL;
    for (u32 i = 0; i < array_count(module->export_decl_indices); ++i) {
        u32 export_decl_index = module->export_decl_indices[i];
        if (export_decl_index >= array_count(module->front_end.sema.decls)) {
            continue;
        }

        const SemaDecl* export_decl =
            &module->front_end.sema.decls[export_decl_index];
        array_push(export_symbols,
                   sema_import_symbol_handle((Lexer*)lexer,
                                             &module->front_end.lexer,
                                             export_decl->symbol_handle));
        array_push(export_types,
                   sema_import_type((Lexer*)lexer,
                                    sema,
                                    &module->front_end.lexer,
                                    &module->front_end.sema,
                                    export_decl->type_index));
    }

    *out_type_index = sema_add_module_type_raw(sema,
                                               export_symbols,
                                               export_types,
                                               (u32)array_count(export_symbols),
                                               module_index);
    array_free(export_symbols);
    array_free(export_types);
    arena_done(&arena);
    return true;
}

internal u32 sema_ensure_module_export_decl(Sema*        sema,
                                            u32          symbol_handle,
                                            u32          type_index,
                                            SemaDeclKind import_decl_kind,
                                            u32          import_module_index,
                                            u32          import_decl_index)
{
    u32 decl_index = sema_find_decl(sema, symbol_handle);
    if (decl_index != sema_no_decl()) {
        return decl_index;
    }

    SemaDeclKind kind = SK_Constant;
    if (import_decl_kind == SK_TypeAlias) {
        kind = SK_TypeAlias;
    } else if (type_index != sema_no_type() &&
               sema->types[type_index].kind == STK_Function) {
        kind = SK_BuiltinFunction;
    } else if (type_index != sema_no_type() &&
               sema->types[type_index].kind == STK_Module) {
        kind = SK_Module;
    }

    array_push(sema->decls,
               (SemaDecl){
                   .kind                = kind,
                   .symbol_handle       = symbol_handle,
                   .bind_node_index     = sema_no_decl(),
                   .type_node_index     = sema_no_type(),
                   .value_node_index    = sema_no_decl(),
                   .type_index          = type_index,
                   .import_module_index = import_module_index,
                   .import_decl_index   = import_decl_index,
               });
    return (u32)array_count(sema->decls) - 1;
}

internal bool sema_import_module_exports_to_scope(const Lexer* lexer,
                                                  const Ast*   ast,
                                                  Sema*        sema,
                                                  u32          use_node_index,
                                                  u32          module_type,
                                                  u32          scope_index)
{
    if (module_type == sema_no_type() ||
        sema->types[module_type].kind != STK_Module) {
        return error_0304_type_mismatch(
            lexer->source,
            sema_node_span(lexer, &ast->nodes[use_node_index]),
            s("module"),
            sema_type_name(lexer, sema, &temp_arena, module_type));
    }

    const SemaType* module = &sema->types[module_type];
    for (u32 i = 0; i < module->param_count; ++i) {
        u32 symbol = sema->type_param_symbols[module->first_param_type + i];
        u32 type   = sema->type_param_types[module->first_param_type + i];
        u32 import_module_index = module->return_type;
        u32 import_decl_index   = sema_no_decl();

        if (sema->program != NULL &&
            import_module_index < array_count(sema->program->modules)) {
            const ModuleInfo* import_module =
                &sema->program->modules[import_module_index];
            if (i < array_count(import_module->export_decl_indices)) {
                import_decl_index = import_module->export_decl_indices[i];
            }
        }

        u32 existing_decl = sema_find_decl(sema, symbol);
        if (existing_decl != sema_no_decl()) {
            const SemaDecl* decl = &sema->decls[existing_decl];
            if (decl->bind_node_index == sema_no_decl() &&
                decl->import_module_index == import_module_index &&
                decl->import_decl_index == import_decl_index) {
                continue;
            }
            return error_0301_duplicate_binding(
                lexer->source,
                sema_node_span(lexer, &ast->nodes[use_node_index]),
                lex_symbol(lexer, symbol),
                sema_decl_span(lexer, ast, decl));
        }

        u32 duplicate = sema_find_local_in_scope(sema, scope_index, symbol);
        if (duplicate != sema_no_local()) {
            const SemaLocal* local = &sema->locals[duplicate];
            if (local->decl_node_index == use_node_index) {
                continue;
            }
            return error_0301_duplicate_binding(
                lexer->source,
                sema_node_span(lexer, &ast->nodes[use_node_index]),
                lex_symbol(lexer, symbol),
                sema_local_span(lexer, ast, local));
        }

        array_push(
            sema->locals,
            (SemaLocal){
                .kind = sema->types[type].kind == STK_Function ? SLK_Function
                                                               : SLK_Constant,
                .symbol_handle    = symbol,
                .owner_decl_index = sema->scopes[scope_index].owner_decl_index,
                .scope_index      = scope_index,
                .decl_node_index  = use_node_index,
                .decl_token_index = U32_MAX,
                .type_node_index  = sema_no_type(),
                .value_node_index = sema_no_decl(),
                .type_index       = type,
                .lowered_symbol_handle = symbol,
            });
        sema->scopes[scope_index].local_count++;
    }

    return true;
}

internal bool sema_import_module_exports_to_decls(const Lexer* lexer,
                                                  const Ast*   ast,
                                                  Sema*        sema,
                                                  u32          use_node_index,
                                                  u32          module_type)
{
    if (module_type == sema_no_type() ||
        sema->types[module_type].kind != STK_Module) {
        return error_0304_type_mismatch(
            lexer->source,
            sema_node_span(lexer, &ast->nodes[use_node_index]),
            s("module"),
            sema_type_name(lexer, sema, &temp_arena, module_type));
    }

    const SemaType* module = &sema->types[module_type];
    for (u32 i = 0; i < module->param_count; ++i) {
        u32 symbol = sema->type_param_symbols[module->first_param_type + i];
        u32 type   = sema->type_param_types[module->first_param_type + i];
        u32 import_module_index       = module->return_type;
        u32 import_decl_index         = sema_no_decl();
        SemaDeclKind import_decl_kind = SK_Constant;
        if (sema->program != NULL &&
            import_module_index < array_count(sema->program->modules)) {
            const ModuleInfo* import_module =
                &sema->program->modules[import_module_index];
            if (i < array_count(import_module->export_decl_indices)) {
                import_decl_index = import_module->export_decl_indices[i];
                if (import_decl_index <
                    array_count(import_module->front_end.sema.decls)) {
                    import_decl_kind =
                        import_module->front_end.sema.decls[import_decl_index]
                            .kind;
                }
            }
        }

        u32 existing_decl = sema_find_decl(sema, symbol);
        if (existing_decl != sema_no_decl()) {
            const SemaDecl* decl = &sema->decls[existing_decl];
            if (decl->bind_node_index == sema_no_decl() &&
                decl->import_module_index == import_module_index &&
                decl->import_decl_index == import_decl_index) {
                continue;
            }
            return error_0301_duplicate_binding(
                lexer->source,
                sema_node_span(lexer, &ast->nodes[use_node_index]),
                lex_symbol(lexer, symbol),
                sema_decl_span(lexer, ast, decl));
        }

        sema_ensure_module_export_decl(sema,
                                       symbol,
                                       type,
                                       import_decl_kind,
                                       import_module_index,
                                       import_decl_index);
    }

    return true;
}

internal bool sema_infer_use_module_type(const Lexer* lexer,
                                         const Ast*   ast,
                                         Sema*        sema,
                                         u32          use_node_index,
                                         u32          scope_index,
                                         u32*         out_module_type)
{
    const AstNode* use_node    = &ast->nodes[use_node_index];
    u32            module_node = use_node->a;
    u32            root_node   = module_node;

    while (ast->nodes[root_node].kind == AK_Expression ||
           ast->nodes[root_node].kind == AK_Statement) {
        root_node = ast->nodes[root_node].a;
    }

    if (scope_index != sema_no_scope() &&
        !sema_resolve_node_refs(lexer,
                                ast,
                                sema->scopes[scope_index].owner_decl_index,
                                sema_no_decl(),
                                sema_no_scope(),
                                scope_index,
                                module_node,
                                sema)) {
        return false;
    }

    if (scope_index == sema_no_scope() &&
        ast->nodes[root_node].kind == AK_SymbolRef) {
        u32 symbol     = ast->nodes[root_node].a;
        u32 decl_index = sema_find_decl(sema, symbol);
        if (decl_index == sema_no_decl()) {
            return error_0300_unknown_symbol(
                lexer->source,
                sema_node_span(lexer, &ast->nodes[root_node]),
                lex_symbol(lexer, symbol));
        }
        sema->node_decl_indices[root_node] = decl_index;
        SemaDecl* decl                     = &sema->decls[decl_index];
        if (decl->type_index == sema_no_type() &&
            decl->value_node_index != sema_no_decl()) {
            if (!sema_infer_node_type(lexer,
                                      ast,
                                      sema,
                                      decl->value_node_index,
                                      sema_no_type(),
                                      &decl->type_index)) {
                return false;
            }
            if (decl->bind_node_index != sema_no_decl()) {
                sema->node_type_indices[decl->bind_node_index] =
                    decl->type_index;
            }
        }
        *out_module_type = decl->type_index;
        return true;
    }

    return sema_infer_node_type(
        lexer, ast, sema, module_node, sema_no_type(), out_module_type);
}

internal bool sema_type_is_ffi_safe(const Sema* sema, u32 type_index)
{
    if (type_index == sema_no_type()) {
        return false;
    }

    const SemaType* type = &sema->types[type_index];
    switch (type->kind) {
    case STK_Void:
    case STK_Bool:
    case STK_I8:
    case STK_I16:
    case STK_I32:
    case STK_I64:
    case STK_U8:
    case STK_U16:
    case STK_U32:
    case STK_U64:
    case STK_F32:
    case STK_F64:
    case STK_Isize:
    case STK_Usize:
    case STK_Pointer:
    case STK_Union:
        return true;
    case STK_Plex:
        return (type->flags & (STF_PlexC | STF_PlexPacked)) != 0;
    default:
        return false;
    }
}

//------------------------------------------------------------------------------
// Return whether one node already has a known folded constant result.

internal bool sema_try_get_constant(const Ast*  ast,
                                    const Sema* sema,
                                    u32         node_index,
                                    i64*        out)
{
    if (node_index >= array_count(sema->node_const_known) ||
        !sema->node_const_known[node_index] ||
        !ast_has_flag(&ast->nodes[node_index], ANF_ConstKnown)) {
        return false;
    }

    *out = sema->node_const_values[node_index];
    return true;
}

//------------------------------------------------------------------------------
// Store one folded constant result for an AST node.

internal void sema_set_constant(Ast* ast, Sema* sema, u32 node_index, i64 value)
{
    sema->node_const_known[node_index]  = true;
    sema->node_const_values[node_index] = value;
    ast_set_flag(&ast->nodes[node_index], ANF_ConstKnown);
}

internal u32  sema_enum_variant_index(const Sema* sema,
                                      u32         enum_type,
                                      u32         symbol_handle);
internal bool sema_node_is_contextual_enum_variant(const Ast*  ast,
                                                   const Sema* sema,
                                                   u32         node_index);

//------------------------------------------------------------------------------
// Return whether one expression can be treated as compile-time constant before
// the constant-folding pass has populated side tables.

internal bool
sema_expr_is_constantish(const Ast* ast, const Sema* sema, u32 node_index)
{
    const AstNode* node = &ast->nodes[node_index];

    switch (node->kind) {
    case AK_IntegerLiteral:
    case AK_StringLiteral:
    case AK_BoolLiteral:
    case AK_EnumVariant:
        return true;
    case AK_StringConcat:
        return sema_expr_is_constantish(ast, sema, node->a) &&
               sema_expr_is_constantish(ast, sema, node->b);
    case AK_RangeExclusive:
    case AK_RangeInclusive:
        return sema_expr_is_constantish(ast, sema, node->a) &&
               sema_expr_is_constantish(ast, sema, node->b);
    case AK_SymbolRef:
        if (sema_node_is_contextual_enum_variant(ast, sema, node_index)) {
            return true;
        }
        if (node_index < array_count(sema->node_local_indices)) {
            u32 local_index = sema->node_local_indices[node_index];
            if (local_index != sema_no_local()) {
                return sema->locals[local_index].kind == SLK_Constant;
            }
        }
        if (node_index < array_count(sema->node_decl_indices)) {
            u32 decl_index = sema->node_decl_indices[node_index];
            return decl_index != sema_no_decl() &&
                   sema->decls[decl_index].kind == SK_Constant;
        }
        return false;
    case AK_Field:
        if (node_index < array_count(sema->node_type_indices) &&
            sema->node_type_indices[node_index] != sema_no_type() &&
            sema->types[sema->node_type_indices[node_index]].kind == STK_Enum) {
            return true;
        }
        if (node_index < array_count(sema->node_decl_indices) &&
            sema->node_decl_indices[node_index] != sema_no_decl()) {
            const SemaDecl* decl =
                &sema->decls[sema->node_decl_indices[node_index]];
            return decl->kind == SK_BuiltinFunction ||
                   decl->kind == SK_FfiFunction;
        }
        return false;
    case AK_IntegerNegate:
    case AK_Expression:
    case AK_Statement:
        return sema_expr_is_constantish(ast, sema, node->a);
    case AK_IntegerPlus:
    case AK_IntegerMinus:
    case AK_IntegerMultiply:
    case AK_IntegerDivide:
    case AK_IntegerModulo:
        return sema_expr_is_constantish(ast, sema, node->a) &&
               sema_expr_is_constantish(ast, sema, node->b);
    default:
        return false;
    }
}

//------------------------------------------------------------------------------
// Evaluate one integer-like constant expression before the folding pass has
// populated semantic constant tables.

internal bool sema_try_eval_integer_constant(const Lexer* lexer,
                                             const Ast*   ast,
                                             const Sema*  sema,
                                             u32          node_index,
                                             i64*         out_value)
{
    const AstNode* node = &ast->nodes[node_index];

    switch (node->kind) {
    case AK_IntegerLiteral:
        *out_value = (i64)ast_get_integer(lexer, node);
        return true;
    case AK_BoolLiteral:
        *out_value = node->a != 0 ? 1 : 0;
        return true;
    case AK_Expression:
    case AK_Statement:
    case AK_InterpPartExpr:
        return sema_try_eval_integer_constant(
            lexer, ast, sema, node->a, out_value);
    case AK_Return:
    case AK_ReturnExpr:
        return node->a != U32_MAX && sema_try_eval_integer_constant(
                                         lexer, ast, sema, node->a, out_value);
    case AK_IntegerNegate:
        if (!sema_try_eval_integer_constant(
                lexer, ast, sema, node->a, out_value)) {
            return false;
        }
        *out_value = -*out_value;
        return true;
    case AK_IntegerPlus:
    case AK_IntegerMinus:
    case AK_IntegerMultiply:
    case AK_IntegerDivide:
    case AK_IntegerModulo:
        {
            i64 lhs = 0;
            i64 rhs = 0;
            if (!sema_try_eval_integer_constant(
                    lexer, ast, sema, node->a, &lhs) ||
                !sema_try_eval_integer_constant(
                    lexer, ast, sema, node->b, &rhs)) {
                return false;
            }

            switch (node->kind) {
            case AK_IntegerPlus:
                *out_value = lhs + rhs;
                return true;
            case AK_IntegerMinus:
                *out_value = lhs - rhs;
                return true;
            case AK_IntegerMultiply:
                *out_value = lhs * rhs;
                return true;
            case AK_IntegerDivide:
                if (rhs == 0) {
                    return false;
                }
                *out_value = lhs / rhs;
                return true;
            case AK_IntegerModulo:
                if (rhs == 0) {
                    return false;
                }
                *out_value = lhs % rhs;
                return true;
            default:
                return false;
            }
        }
    case AK_RangeExclusive:
    case AK_RangeInclusive:
        return false;
    case AK_SymbolRef:
        if (node_index < array_count(sema->node_local_indices)) {
            u32 local_index = sema->node_local_indices[node_index];
            if (local_index != sema_no_local() &&
                sema->locals[local_index].kind == SLK_Constant) {
                return sema_try_eval_integer_constant(
                    lexer,
                    ast,
                    sema,
                    sema->locals[local_index].value_node_index,
                    out_value);
            }
        }
        if (node_index < array_count(sema->node_decl_indices)) {
            u32 decl_index = sema->node_decl_indices[node_index];
            if (decl_index != sema_no_decl() &&
                sema->decls[decl_index].kind == SK_Constant) {
                return sema_try_eval_integer_constant(
                    lexer,
                    ast,
                    sema,
                    sema->decls[decl_index].value_node_index,
                    out_value);
            }
        }
        return false;
    default:
        return false;
    }
}

//------------------------------------------------------------------------------
// Compute the source span covered by an AST node's main token.

internal ErrorSpan sema_node_span(const Lexer* lexer, const AstNode* node)
{
    const Token* token = &lexer->tokens[node->token_index];
    return (ErrorSpan){.start = token->offset,
                       .end   = lex_token_end_offset(lexer, token)};
}

internal ErrorSpan sema_pattern_span(const Lexer*      lexer,
                                     const AstPattern* pattern)
{
    const Token* token = &lexer->tokens[pattern->token_index];
    return (ErrorSpan){.start = token->offset,
                       .end   = lex_token_end_offset(lexer, token)};
}

//------------------------------------------------------------------------------
// Look up the source span for a collected declaration binding.

internal ErrorSpan sema_decl_span(const Lexer*    lexer,
                                  const Ast*      ast,
                                  const SemaDecl* decl)
{
    if (decl->bind_node_index == sema_no_decl()) {
        return (ErrorSpan){0};
    }
    return sema_node_span(lexer, &ast->nodes[decl->bind_node_index]);
}

internal ErrorSpan sema_local_span(const Lexer*     lexer,
                                   const Ast*       ast,
                                   const SemaLocal* local)
{
    if (local->decl_token_index != U32_MAX) {
        const Token* token = &lexer->tokens[local->decl_token_index];
        return (ErrorSpan){
            .start = token->offset,
            .end   = lex_token_end_offset(lexer, token),
        };
    }
    if (local->decl_node_index == sema_no_decl()) {
        return sema_node_span(lexer, &ast->nodes[local->type_node_index]);
    }
    return sema_node_span(lexer, &ast->nodes[local->decl_node_index]);
}

internal ErrorSpan sema_token_span(const Lexer* lexer, u32 token_index)
{
    const Token* token = &lexer->tokens[token_index];
    return (ErrorSpan){.start = token->offset,
                       .end   = lex_token_end_offset(lexer, token)};
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

typedef enum : u8 {
    SEMA_ALIAS_UNSEEN,
    SEMA_ALIAS_RESOLVING,
    SEMA_ALIAS_DONE,
} SemaAliasState;

internal bool sema_try_classify_type_alias(const Lexer* lexer,
                                           const Ast*   ast,
                                           Sema*        sema,
                                           u32          owner_decl_index,
                                           u32          decl_index,
                                           Array(u8) alias_states,
                                           bool* out_is_type,
                                           u32*  out_type_index);

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

internal bool sema_node_is_inside_function_body(const Ast* ast, u32 node_index)
{
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind != AK_FnDef) {
            continue;
        }

        const AstNode* fn_start = &ast->nodes[node->a];
        if (node_index > node->a && node_index < fn_start->b) {
            return true;
        }
    }

    return false;
}

internal bool sema_try_classify_type_node(const Lexer* lexer,
                                          const Ast*   ast,
                                          Sema*        sema,
                                          u32          owner_decl_index,
                                          u32          node_index,
                                          Array(u8) alias_states,
                                          bool* out_is_type,
                                          u32*  out_type_index)
{
    const AstNode* node = &ast->nodes[node_index];

    switch (node->kind) {
    case AK_SymbolRef:
        {
            u32 builtin =
                sema_type_index_for_name(sema, lex_symbol(lexer, node->a));
            if (builtin != sema_no_type()) {
                *out_is_type    = true;
                *out_type_index = builtin;
                return true;
            }

            u32 decl_index = sema_find_decl(sema, node->a);
            if (decl_index == sema_no_decl()) {
                *out_is_type    = false;
                *out_type_index = sema_no_type();
                return true;
            }

            return sema_try_classify_type_alias(lexer,
                                                ast,
                                                sema,
                                                owner_decl_index,
                                                decl_index,
                                                alias_states,
                                                out_is_type,
                                                out_type_index);
        }

    case AK_TypeFn:
        {
            const AstFnSignature* signature = sema_ast_signature(ast, node);
            Array(u32) param_types          = NULL;

            for (u32 i = 0; i < signature->param_count; ++i) {
                bool param_is_type = false;
                u32  param_type    = sema_no_type();
                if (!sema_try_classify_type_node(
                        lexer,
                        ast,
                        sema,
                        owner_decl_index,
                        ast->params[signature->first_param + i].type_node_index,
                        alias_states,
                        &param_is_type,
                        &param_type)) {
                    array_free(param_types);
                    return false;
                }
                if (!param_is_type) {
                    array_free(param_types);
                    *out_is_type    = false;
                    *out_type_index = sema_no_type();
                    return true;
                }
                array_push(param_types, param_type);
            }

            bool return_is_type = false;
            u32  return_type    = sema_no_type();
            if (!sema_try_classify_type_node(lexer,
                                             ast,
                                             sema,
                                             owner_decl_index,
                                             signature->return_type_node_index,
                                             alias_states,
                                             &return_is_type,
                                             &return_type)) {
                array_free(param_types);
                return false;
            }
            if (!return_is_type) {
                array_free(param_types);
                *out_is_type    = false;
                *out_type_index = sema_no_type();
                return true;
            }

            *out_is_type = true;
            *out_type_index =
                sema_add_function_type(sema, param_types, return_type);
            array_free(param_types);
            return true;
        }

    case AK_TypeTuple:
        {
            Array(u32) item_types = NULL;
            for (u32 i = 0; i < node->b; ++i) {
                bool item_is_type = false;
                u32  item_type    = sema_no_type();
                if (!sema_try_classify_type_node(lexer,
                                                 ast,
                                                 sema,
                                                 owner_decl_index,
                                                 ast->tuple_items[node->a + i],
                                                 alias_states,
                                                 &item_is_type,
                                                 &item_type)) {
                    array_free(item_types);
                    return false;
                }
                if (!item_is_type) {
                    array_free(item_types);
                    *out_is_type    = false;
                    *out_type_index = sema_no_type();
                    return true;
                }
                array_push(item_types, item_type);
            }

            u32 type_index = sema_add_tuple_type(sema, item_types);
            array_free(item_types);
            *out_is_type    = true;
            *out_type_index = type_index;
            return true;
        }

    case AK_TypeArray:
        {
            i64 item_count = 0;
            if (!sema_try_eval_integer_constant(
                    lexer, ast, sema, node->a, &item_count) ||
                item_count < 0 || item_count > UINT32_MAX) {
                *out_is_type    = false;
                *out_type_index = sema_no_type();
                return true;
            }

            bool item_is_type = false;
            u32  item_type    = sema_no_type();
            if (!sema_try_classify_type_node(lexer,
                                             ast,
                                             sema,
                                             owner_decl_index,
                                             node->b,
                                             alias_states,
                                             &item_is_type,
                                             &item_type)) {
                return false;
            }
            if (!item_is_type) {
                *out_is_type    = false;
                *out_type_index = sema_no_type();
                return true;
            }

            *out_is_type = true;
            *out_type_index =
                sema_add_array_type(sema, item_type, (u32)item_count);
            return true;
        }

    case AK_TypeSlice:
        {
            bool item_is_type = false;
            u32  item_type    = sema_no_type();
            if (!sema_try_classify_type_node(lexer,
                                             ast,
                                             sema,
                                             owner_decl_index,
                                             node->a,
                                             alias_states,
                                             &item_is_type,
                                             &item_type)) {
                return false;
            }
            if (!item_is_type) {
                *out_is_type    = false;
                *out_type_index = sema_no_type();
                return true;
            }
            *out_is_type    = true;
            *out_type_index = sema_add_slice_type(sema, item_type);
            return true;
        }

    case AK_TypePointer:
        {
            bool pointee_is_type = false;
            u32  pointee_type    = sema_no_type();
            if (!sema_try_classify_type_node(lexer,
                                             ast,
                                             sema,
                                             owner_decl_index,
                                             node->a,
                                             alias_states,
                                             &pointee_is_type,
                                             &pointee_type)) {
                return false;
            }
            if (!pointee_is_type) {
                *out_is_type    = false;
                *out_type_index = sema_no_type();
                return true;
            }
            *out_is_type    = true;
            *out_type_index = sema_add_pointer_type(sema, pointee_type);
            return true;
        }

    case AK_TypePlex:
        {
            const AstPlexTypeInfo* plex = &ast->plex_types[node->a];
            Array(u32) field_types      = NULL;
            for (u32 i = 0; i < plex->field_count; ++i) {
                bool field_is_type = false;
                u32  field_type    = sema_no_type();
                if (!sema_try_classify_type_node(
                        lexer,
                        ast,
                        sema,
                        owner_decl_index,
                        ast->plex_fields[plex->first_field + i].type_node_index,
                        alias_states,
                        &field_is_type,
                        &field_type)) {
                    array_free(field_types);
                    return false;
                }
                if (!field_is_type) {
                    array_free(field_types);
                    *out_is_type    = false;
                    *out_type_index = sema_no_type();
                    return true;
                }
                array_push(field_types, field_type);
            }
            *out_is_type = true;
            *out_type_index =
                (plex->flags & APTF_Union)
                    ? sema_add_union_type(sema,
                                          &ast->plex_fields[plex->first_field],
                                          field_types,
                                          plex->field_count)
                    : sema_add_plex_type(sema,
                                         &ast->plex_fields[plex->first_field],
                                         field_types,
                                         plex->field_count,
                                         plex->flags);
            array_free(field_types);
            return true;
        }

    case AK_TypeEnum:
        {
            const AstEnumTypeInfo* enum_type = &ast->enum_types[node->a];
            Array(u32) payload_types         = NULL;
            for (u32 i = 0; i < enum_type->variant_count; ++i) {
                const AstEnumVariant* variant =
                    &ast->enum_variants[enum_type->first_variant + i];
                bool payload_is_type = true;
                u32  payload_type    = sema_no_type();
                if (variant->type_node_index != U32_MAX &&
                    !sema_try_classify_type_node(lexer,
                                                 ast,
                                                 sema,
                                                 owner_decl_index,
                                                 variant->type_node_index,
                                                 alias_states,
                                                 &payload_is_type,
                                                 &payload_type)) {
                    array_free(payload_types);
                    return false;
                }
                if (!payload_is_type) {
                    array_free(payload_types);
                    *out_is_type    = false;
                    *out_type_index = sema_no_type();
                    return true;
                }
                array_push(payload_types, payload_type);
            }
            *out_is_type    = true;
            *out_type_index = sema_add_enum_type(
                sema,
                &ast->enum_variants[enum_type->first_variant],
                payload_types,
                enum_type->variant_count);
            array_free(payload_types);
            return true;
        }

    default:
        *out_is_type    = false;
        *out_type_index = sema_no_type();
        return true;
    }
}

internal bool sema_try_classify_type_alias(const Lexer* lexer,
                                           const Ast*   ast,
                                           Sema*        sema,
                                           u32          owner_decl_index,
                                           u32          decl_index,
                                           Array(u8) alias_states,
                                           bool* out_is_type,
                                           u32*  out_type_index)
{
    SemaDecl* decl = &sema->decls[decl_index];
    if (decl->kind == SK_TypeAlias) {
        *out_is_type    = true;
        *out_type_index = decl->type_index;
        return true;
    }
    if (decl->kind == SK_Variable || decl->kind == SK_Function ||
        decl->kind == SK_FfiFunction || decl->kind == SK_Module ||
        decl->kind == SK_BuiltinFunction) {
        *out_is_type    = false;
        *out_type_index = sema_no_type();
        return true;
    }
    if (decl->type_node_index != sema_no_type() ||
        decl->value_node_index == sema_no_decl()) {
        *out_is_type    = false;
        *out_type_index = sema_no_type();
        return true;
    }

    switch ((SemaAliasState)alias_states[decl_index]) {
    case SEMA_ALIAS_DONE:
        *out_is_type    = decl->kind == SK_TypeAlias;
        *out_type_index = decl->type_index;
        return true;
    case SEMA_ALIAS_RESOLVING:
        return error_0309_type_alias_cycle(
            lexer->source,
            sema_decl_span(lexer, ast, &sema->decls[owner_decl_index]),
            lex_symbol(lexer, sema->decls[owner_decl_index].symbol_handle),
            sema_decl_span(lexer, ast, decl),
            lex_symbol(lexer, decl->symbol_handle));
    case SEMA_ALIAS_UNSEEN:
        break;
    }

    alias_states[decl_index] = SEMA_ALIAS_RESOLVING;
    bool rhs_is_type         = false;
    u32  rhs_type            = sema_no_type();
    if (!sema_try_classify_type_node(lexer,
                                     ast,
                                     sema,
                                     decl_index,
                                     decl->value_node_index,
                                     alias_states,
                                     &rhs_is_type,
                                     &rhs_type)) {
        return false;
    }

    if (rhs_is_type) {
        decl->kind       = SK_TypeAlias;
        decl->type_index = rhs_type;
    }

    alias_states[decl_index] = SEMA_ALIAS_DONE;
    *out_is_type             = rhs_is_type;
    *out_type_index          = rhs_type;
    return true;
}

internal bool sema_keyword_is_defined(const FrontEndOptions* options,
                                      const Lexer*           lexer,
                                      u32                    symbol_handle)
{
    string name = lex_symbol(lexer, symbol_handle);

    if (!options->release && string_eq_cstr(name, "debug")) {
        return true;
    }
#if OS_WINDOWS
    if (string_eq_cstr(name, "windows")) {
        return true;
    }
#endif
#if OS_LINUX
    if (string_eq_cstr(name, "linux")) {
        return true;
    }
#endif
#if OS_MACOS
    if (string_eq_cstr(name, "macos")) {
        return true;
    }
#endif
#if OS_BSD
    if (string_eq_cstr(name, "bsd")) {
        return true;
    }
#endif
#if OS_POSIX
    if (string_eq_cstr(name, "posix")) {
        return true;
    }
#endif

    for (u32 i = 0; i < array_count(options->keywords); ++i) {
        if (string_eq(name, options->keywords[i])) {
            return true;
        }
    }

    return false;
}

internal bool sema_top_on_is_enabled(const FrontEndOptions* options,
                                     const Lexer*           lexer,
                                     const Ast*             ast,
                                     const AstNode*         node)
{
    ASSERT(node->kind == AK_TopOn, "Expected top-level on node");
    const AstTopOnInfo* info = &ast->top_ons[node->a];
    bool enabled = sema_keyword_is_defined(options, lexer, info->symbol_handle);
    return info->is_negated ? !enabled : enabled;
}

internal bool sema_node_is_inside_top_on_body(const Ast* ast,
                                              u32        node_index,
                                              u32 current_body_node_index)
{
    u32 innermost_body = U32_MAX;
    u32 innermost_span = U32_MAX;

    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* owner = &ast->nodes[i];
        if (owner->kind != AK_TopOn) {
            continue;
        }

        const AstTopOnInfo* info = &ast->top_ons[owner->a];
        const AstNode*      body = &ast->nodes[info->body_node_index];
        if (body->a <= node_index && node_index < body->b) {
            u32 span = body->b - body->a;
            if (innermost_body == U32_MAX || span < innermost_span) {
                innermost_body = info->body_node_index;
                innermost_span = span;
            }
        }
    }

    return innermost_body != U32_MAX &&
           innermost_body != current_body_node_index;
}

internal bool sema_collect_decls_in_range(const Lexer*           lexer,
                                          const Ast*             ast,
                                          const FrontEndOptions* options,
                                          u32                    first_node,
                                          u32                    end_node,
                                          u32   current_body_node_index,
                                          Sema* sema)
{
    for (u32 i = first_node; i < end_node; ++i) {
        const AstNode* node = &ast->nodes[i];
        if (sema_node_is_inside_top_on_body(ast, i, current_body_node_index)) {
            continue;
        }
        if (node->kind == AK_TopOn) {
            if (sema_top_on_is_enabled(options, lexer, ast, node)) {
                const AstTopOnInfo* info = &ast->top_ons[node->a];
                const AstNode*      body = &ast->nodes[info->body_node_index];
                ASSERT(body->kind == AK_Block, "Expected top-level on body");
                if (!sema_collect_decls_in_range(lexer,
                                                 ast,
                                                 options,
                                                 body->a,
                                                 body->b,
                                                 info->body_node_index,
                                                 sema)) {
                    return false;
                }
            }
            continue;
        }
        if (node->kind != AK_Bind && node->kind != AK_Variable &&
            node->kind != AK_FfiDef) {
            continue;
        }
        if (sema_node_is_inside_function_body(ast, i)) {
            continue;
        }

        u32 symbol_handle       = node->kind == AK_FfiDef
                                      ? ast->ffi_infos[node->a].symbol_handle
                                      : ast_get_symbol(node);

        u32 existing_decl_index = sema_find_decl(sema, symbol_handle);
        if (existing_decl_index != sema_no_decl()) {
            const SemaDecl* existing_decl = &sema->decls[existing_decl_index];
            return error_0301_duplicate_binding(
                lexer->source,
                sema_node_span(lexer, node),
                lex_symbol(lexer, symbol_handle),
                existing_decl->bind_node_index == sema_no_decl()
                    ? sema_node_span(lexer, node)
                    : sema_node_span(
                          lexer, &ast->nodes[existing_decl->bind_node_index]));
        }

        u32            type_node_index  = sema_no_type();
        u32            value_node_index = node->kind == AK_FfiDef ? i : node->b;
        const AstNode* value            = &ast->nodes[value_node_index];
        if (value->kind == AK_AnnotatedValue) {
            type_node_index  = value->a;
            value_node_index = value->b;
            value            = &ast->nodes[value_node_index];
        } else if (value->kind == AK_ZeroInit) {
            type_node_index  = value->a;
            value_node_index = sema_no_decl();
        }

        SemaDeclKind kind = SK_Constant;
        if (node->kind == AK_FfiDef) {
            kind = SK_FfiFunction;
        } else if (node->kind == AK_Variable) {
            kind = SK_Variable;
        } else if (value->kind == AK_FnDef) {
            kind = SK_Function;
        } else if (value->kind == AK_FfiDef) {
            kind = SK_FfiFunction;
        } else if (value->kind == AK_ModRef) {
            kind = SK_Module;
        }

        array_push(sema->decls,
                   (SemaDecl){
                       .kind                = kind,
                       .symbol_handle       = symbol_handle,
                       .bind_node_index     = i,
                       .type_node_index     = type_node_index,
                       .value_node_index    = value_node_index,
                       .type_index          = sema_no_type(),
                       .import_module_index = sema_no_decl(),
                       .import_decl_index   = sema_no_decl(),
                   });
    }
    return true;
}

internal bool sema_collect_decls(const Lexer*           lexer,
                                 const Ast*             ast,
                                 const FrontEndOptions* options,
                                 Sema*                  sema)
{
    return sema_collect_decls_in_range(
        lexer, ast, options, 0, (u32)array_count(ast->nodes), U32_MAX, sema);
}

internal bool
sema_classify_type_aliases(const Lexer* lexer, const Ast* ast, Sema* sema)
{
    Array(u8) alias_states = NULL;
    for (u32 i = 0; i < array_count(sema->decls); ++i) {
        array_push(alias_states, SEMA_ALIAS_UNSEEN);
    }

    bool ok = true;
    for (u32 i = 0; i < array_count(sema->decls); ++i) {
        bool ignored_is_type = false;
        u32  ignored_type    = sema_no_type();
        if (!sema_try_classify_type_alias(lexer,
                                          ast,
                                          sema,
                                          i,
                                          i,
                                          alias_states,
                                          &ignored_is_type,
                                          &ignored_type)) {
            ok = false;
            break;
        }
    }

    array_free(alias_states);
    return ok;
}

//------------------------------------------------------------------------------
// Mark one type-annotation subtree so normal symbol resolution can skip it.

internal void
sema_mark_type_expr_nodes(const Ast* ast, Sema* sema, u32 node_index)
{
    if (node_index >= array_count(sema->node_is_type_expr) ||
        sema->node_is_type_expr[node_index]) {
        return;
    }

    sema->node_is_type_expr[node_index] = true;
    const AstNode* node                 = &ast->nodes[node_index];

    switch (node->kind) {
    case AK_TypeFn:
        {
            const AstFnSignature* signature = &ast->fn_signatures[node->a];
            for (u32 i = 0; i < signature->param_count; ++i) {
                sema_mark_type_expr_nodes(
                    ast,
                    sema,
                    ast->params[signature->first_param + i].type_node_index);
            }
            if (signature->return_type_node_index != U32_MAX) {
                sema_mark_type_expr_nodes(
                    ast, sema, signature->return_type_node_index);
            }
        }
        break;
    case AK_TypeTuple:
        for (u32 i = 0; i < node->b; ++i) {
            sema_mark_type_expr_nodes(ast, sema, ast->tuple_items[node->a + i]);
        }
        break;
    case AK_TypeArray:
        sema_mark_type_expr_nodes(ast, sema, node->a);
        sema_mark_type_expr_nodes(ast, sema, node->b);
        break;
    case AK_TypeSlice:
        sema_mark_type_expr_nodes(ast, sema, node->a);
        break;
    case AK_TypePointer:
        sema_mark_type_expr_nodes(ast, sema, node->a);
        break;
    case AK_TypePlex:
        {
            const AstPlexTypeInfo* plex = &ast->plex_types[node->a];
            for (u32 i = 0; i < plex->field_count; ++i) {
                sema_mark_type_expr_nodes(
                    ast,
                    sema,
                    ast->plex_fields[plex->first_field + i].type_node_index);
            }
        }
        break;
    case AK_TypeEnum:
        break;
    default:
        break;
    }
}

internal u32 sema_add_scope(Sema* sema, u32 owner_decl_index, u32 parent_scope)
{
    array_push(sema->scopes,
               (SemaScope){
                   .owner_decl_index   = owner_decl_index,
                   .parent_scope_index = parent_scope,
                   .first_local        = (u32)array_count(sema->locals),
                   .local_count        = 0,
               });
    return (u32)array_count(sema->scopes) - 1;
}

internal u32 sema_find_local_in_scope(const Sema* sema,
                                      u32         scope_index,
                                      u32         symbol_handle)
{
    if (scope_index == sema_no_scope()) {
        return sema_no_local();
    }

    const SemaScope* scope = &sema->scopes[scope_index];
    u32              end   = scope->first_local + scope->local_count;
    for (u32 i = end; i-- > scope->first_local;) {
        const SemaLocal* local = &sema->locals[i];
        if (local->symbol_handle == symbol_handle) {
            return i;
        }
    }
    return sema_no_local();
}

internal u32 sema_lookup_local(const Sema* sema,
                               u32         scope_index,
                               u32         symbol_handle)
{
    for (u32 current = scope_index; current != sema_no_scope();
         current     = sema->scopes[current].parent_scope_index) {
        u32 local_index =
            sema_find_local_in_scope(sema, current, symbol_handle);
        if (local_index != sema_no_local()) {
            return local_index;
        }
    }
    return sema_no_local();
}

internal u32 sema_find_decl_local_in_scope(const Sema* sema,
                                           u32         scope_index,
                                           u32         symbol_handle)
{
    if (scope_index == sema_no_scope()) {
        return sema_no_local();
    }

    const SemaScope* scope = &sema->scopes[scope_index];
    u32              end   = scope->first_local + scope->local_count;
    for (u32 i = end; i-- > scope->first_local;) {
        const SemaLocal* local = &sema->locals[i];
        if ((local->kind == SLK_Constant || local->kind == SLK_Function ||
             local->kind == SLK_TypeAlias) &&
            local->symbol_handle == symbol_handle) {
            return i;
        }
    }

    return sema_no_local();
}

internal u32 sema_lookup_decl_local(const Sema* sema,
                                    u32         scope_index,
                                    u32         symbol_handle)
{
    for (u32 current = scope_index; current != sema_no_scope();
         current     = sema->scopes[current].parent_scope_index) {
        u32 local_index =
            sema_find_decl_local_in_scope(sema, current, symbol_handle);
        if (local_index != sema_no_local()) {
            return local_index;
        }
    }
    return sema_no_local();
}

internal bool sema_local_is_runtime_value(const SemaLocal* local)
{
    return local->kind == SLK_Param || local->kind == SLK_Variable ||
           local->kind == SLK_Binder;
}

internal bool sema_local_is_decl_binding(const SemaLocal* local)
{
    return local->kind == SLK_Constant || local->kind == SLK_Function ||
           local->kind == SLK_TypeAlias;
}

internal u32 sema_mangle_child_function_symbol(const Lexer* lexer,
                                               u32    parent_symbol_handle,
                                               string child_name)
{
    string owner   = lex_symbol(lexer, parent_symbol_handle);
    string mangled = string_format(
        &temp_arena, STRINGP "$" STRINGP, STRINGV(owner), STRINGV(child_name));
    InternAddResult ignored = 0;
    return lex_add_symbol((Lexer*)lexer, mangled, &ignored);
}

internal u32 sema_mangle_nested_function_symbol(const Lexer* lexer,
                                                u32 parent_symbol_handle,
                                                u32 symbol_handle)
{
    return sema_mangle_child_function_symbol(
        lexer, parent_symbol_handle, lex_symbol(lexer, symbol_handle));
}

internal u32 sema_mangle_anonymous_function_symbol(const Lexer* lexer,
                                                   u32 parent_symbol_handle,
                                                   u32 fn_node_index)
{
    string suffix = string_format(&temp_arena, "anon%u", fn_node_index);
    return sema_mangle_child_function_symbol(
        lexer, parent_symbol_handle, suffix);
}

internal u32 sema_mangle_on_pattern_binder_symbol(const Lexer* lexer,
                                                  u32 current_function_symbol,
                                                  u32 symbol_handle,
                                                  u32 token_index)
{
    string name = lex_symbol(lexer, symbol_handle);
    string suffix =
        string_format(&temp_arena, STRINGP "$on%u", STRINGV(name), token_index);
    return sema_mangle_child_function_symbol(
        lexer, current_function_symbol, suffix);
}

internal void
sema_mark_fn_signature_type_nodes(const Ast* ast, Sema* sema, u32 fn_node_index)
{
    const AstNode*        fn_def    = &ast->nodes[fn_node_index];
    const AstNode*        fn_start  = &ast->nodes[fn_def->a];
    const AstFnSignature* signature = &ast->fn_signatures[fn_start->a];
    for (u32 i = 0; i < signature->param_count; ++i) {
        sema_mark_type_expr_nodes(
            ast, sema, ast->params[signature->first_param + i].type_node_index);
    }
    if (signature->return_type_node_index != sema_no_type()) {
        sema_mark_type_expr_nodes(ast, sema, signature->return_type_node_index);
    }
}

internal bool sema_resolve_node_refs(const Lexer* lexer,
                                     const Ast*   ast,
                                     u32          owner_decl_index,
                                     u32          current_function_symbol,
                                     u32          capture_scope_index,
                                     u32          scope_index,
                                     u32          node_index,
                                     Sema*        sema);
internal bool sema_resolve_pattern_refs(const Lexer* lexer,
                                        const Ast*   ast,
                                        u32          owner_decl_index,
                                        u32          current_function_symbol,
                                        u32          capture_scope_index,
                                        u32          scope_index,
                                        u32          pattern_index,
                                        Sema*        sema);
internal bool sema_resolve_type_node(const Lexer* lexer,
                                     const Ast*   ast,
                                     Sema*        sema,
                                     u32          node_index,
                                     u32*         out_type_index);
internal bool sema_try_resolve_type_symbol(const Lexer* lexer,
                                           const Ast*   ast,
                                           Sema*        sema,
                                           u32          node_index,
                                           u32*         out_type_index);

internal u32 sema_destructure_binder_symbol(const Ast* ast, u32 pattern_index)
{
    const AstPattern* pattern = &ast->patterns[pattern_index];
    if (pattern->kind == APK_Bind) {
        return pattern->a;
    }
    if (pattern->kind == APK_Value) {
        const AstNode* node = &ast->nodes[pattern->a];
        if (node->kind == AK_SymbolRef) {
            return node->a;
        }
    }
    return U32_MAX;
}

internal bool sema_collect_destructure_pattern(const Lexer*  lexer,
                                               const Ast*    ast,
                                               Sema*         sema,
                                               u32           owner_decl_index,
                                               u32           scope_index,
                                               u32           decl_node_index,
                                               SemaLocalKind kind,
                                               u32           pattern_index)
{
    const AstPattern* pattern = &ast->patterns[pattern_index];
    u32 symbol = sema_destructure_binder_symbol(ast, pattern_index);
    if (symbol != U32_MAX) {
        u32 duplicate = sema_find_local_in_scope(sema, scope_index, symbol);
        if (duplicate != sema_no_local()) {
            const SemaLocal* previous = &sema->locals[duplicate];
            return error_0301_duplicate_binding(
                lexer->source,
                sema_pattern_span(lexer, pattern),
                lex_symbol(lexer, symbol),
                sema_local_span(lexer, ast, previous));
        }

        array_push(sema->locals,
                   (SemaLocal){
                       .kind                  = kind,
                       .symbol_handle         = symbol,
                       .owner_decl_index      = owner_decl_index,
                       .scope_index           = scope_index,
                       .decl_node_index       = decl_node_index,
                       .decl_token_index      = pattern->token_index,
                       .type_node_index       = sema_no_type(),
                       .value_node_index      = ast->nodes[decl_node_index].b,
                       .type_index            = sema_no_type(),
                       .lowered_symbol_handle = symbol,
                   });
        sema->pattern_local_indices[pattern_index] =
            (u32)array_count(sema->locals) - 1;
        sema->scopes[scope_index].local_count++;
        if (pattern->kind == APK_Bind && pattern->b != U32_MAX) {
            return sema_collect_destructure_pattern(lexer,
                                                    ast,
                                                    sema,
                                                    owner_decl_index,
                                                    scope_index,
                                                    decl_node_index,
                                                    kind,
                                                    pattern->b);
        }
        return true;
    }

    switch (pattern->kind) {
    case APK_Ignore:
        return true;
    case APK_Tuple:
        for (u32 i = 0; i < pattern->b; ++i) {
            if (!sema_collect_destructure_pattern(
                    lexer,
                    ast,
                    sema,
                    owner_decl_index,
                    scope_index,
                    decl_node_index,
                    kind,
                    ast->pattern_items[pattern->a + i])) {
                return false;
            }
        }
        return true;
    case APK_Plex:
        for (u32 i = 0; i < pattern->b; ++i) {
            if (!sema_collect_destructure_pattern(
                    lexer,
                    ast,
                    sema,
                    owner_decl_index,
                    scope_index,
                    decl_node_index,
                    kind,
                    ast->pattern_fields[pattern->a + i].pattern_index)) {
                return false;
            }
        }
        return true;
    default:
        return error_0304_type_mismatch(lexer->source,
                                        sema_pattern_span(lexer, pattern),
                                        s("destructuring binder"),
                                        s("value pattern"));
    }
}

internal bool sema_pattern_contains_binder(const Ast* ast, u32 pattern_index)
{
    const AstPattern* pattern = &ast->patterns[pattern_index];
    if (pattern->kind == APK_Bind) {
        return true;
    }
    if (pattern->kind == APK_Tuple) {
        for (u32 i = 0; i < pattern->b; ++i) {
            if (sema_pattern_contains_binder(
                    ast, ast->pattern_items[pattern->a + i])) {
                return true;
            }
        }
        return false;
    }
    if (pattern->kind == APK_Plex) {
        for (u32 i = 0; i < pattern->b; ++i) {
            if (sema_pattern_contains_binder(
                    ast, ast->pattern_fields[pattern->a + i].pattern_index)) {
                return true;
            }
        }
    }
    if (pattern->kind == APK_EnumVariant) {
        const AstEnumPattern* enum_pattern = &ast->enum_patterns[pattern->a];
        for (u32 i = 0; i < enum_pattern->pattern_count; ++i) {
            if (sema_pattern_contains_binder(
                    ast, ast->pattern_items[enum_pattern->first_pattern + i])) {
                return true;
            }
        }
    }
    return false;
}

internal bool sema_collect_on_pattern_binders(const Lexer* lexer,
                                              const Ast*   ast,
                                              Sema*        sema,
                                              u32          owner_decl_index,
                                              u32 current_function_symbol,
                                              u32 scope_index,
                                              u32 pattern_index)
{
    const AstPattern* pattern = &ast->patterns[pattern_index];
    if (pattern->kind == APK_Bind) {
        u32 duplicate = sema_find_local_in_scope(sema, scope_index, pattern->a);
        if (duplicate != sema_no_local()) {
            const SemaLocal* previous = &sema->locals[duplicate];
            return error_0301_duplicate_binding(
                lexer->source,
                sema_pattern_span(lexer, pattern),
                lex_symbol(lexer, pattern->a),
                sema_local_span(lexer, ast, previous));
        }
        u32 lowered_symbol = sema_mangle_on_pattern_binder_symbol(
            lexer, current_function_symbol, pattern->a, pattern->token_index);
        array_push(sema->locals,
                   (SemaLocal){
                       .kind                  = SLK_Binder,
                       .symbol_handle         = pattern->a,
                       .owner_decl_index      = owner_decl_index,
                       .scope_index           = scope_index,
                       .decl_node_index       = sema_no_decl(),
                       .decl_token_index      = pattern->token_index,
                       .type_node_index       = sema_no_type(),
                       .value_node_index      = sema_no_decl(),
                       .type_index            = sema_no_type(),
                       .lowered_symbol_handle = lowered_symbol,
                   });
        sema->pattern_local_indices[pattern_index] =
            (u32)array_count(sema->locals) - 1;
        sema->scopes[scope_index].local_count++;
        if (pattern->b != U32_MAX) {
            return sema_collect_on_pattern_binders(lexer,
                                                   ast,
                                                   sema,
                                                   owner_decl_index,
                                                   current_function_symbol,
                                                   scope_index,
                                                   pattern->b);
        }
        return true;
    }

    switch (pattern->kind) {
    case APK_Bind:
        return true;
    case APK_Tuple:
        for (u32 i = 0; i < pattern->b; ++i) {
            if (!sema_collect_on_pattern_binders(
                    lexer,
                    ast,
                    sema,
                    owner_decl_index,
                    current_function_symbol,
                    scope_index,
                    ast->pattern_items[pattern->a + i])) {
                return false;
            }
        }
        return true;
    case APK_Plex:
        for (u32 i = 0; i < pattern->b; ++i) {
            if (!sema_collect_on_pattern_binders(
                    lexer,
                    ast,
                    sema,
                    owner_decl_index,
                    current_function_symbol,
                    scope_index,
                    ast->pattern_fields[pattern->a + i].pattern_index)) {
                return false;
            }
        }
        return true;
    case APK_EnumVariant:
        {
            const AstEnumPattern* enum_pattern =
                &ast->enum_patterns[pattern->a];
            for (u32 i = 0; i < enum_pattern->pattern_count; ++i) {
                if (!sema_collect_on_pattern_binders(
                        lexer,
                        ast,
                        sema,
                        owner_decl_index,
                        current_function_symbol,
                        scope_index,
                        ast->pattern_items[enum_pattern->first_pattern + i])) {
                    return false;
                }
            }
            return true;
        }
    case APK_Value:
    case APK_Equal:
    case APK_NotEqual:
    case APK_Less:
    case APK_LessEqual:
    case APK_Greater:
    case APK_GreaterEqual:
    case APK_RangeExclusive:
    case APK_RangeInclusive:
    case APK_Ignore:
        return true;
    }
    return true;
}

internal bool sema_resolve_destructure_assign_pattern(const Lexer* lexer,
                                                      const Ast*   ast,
                                                      Sema*        sema,
                                                      u32          scope_index,
                                                      u32 pattern_index)
{
    const AstPattern* pattern = &ast->patterns[pattern_index];
    u32 symbol = sema_destructure_binder_symbol(ast, pattern_index);
    if (symbol != U32_MAX) {
        u32 local_index = sema_lookup_local(sema, scope_index, symbol);
        if (local_index == sema_no_local() ||
            sema->locals[local_index].kind == SLK_Binder ||
            !sema_local_is_runtime_value(&sema->locals[local_index])) {
            return error_0305_invalid_assignment_target(
                lexer->source,
                sema_pattern_span(lexer, pattern),
                lex_symbol(lexer, symbol));
        }
        sema->pattern_local_indices[pattern_index] = local_index;
        return true;
    }

    switch (pattern->kind) {
    case APK_Ignore:
        return true;
    case APK_Tuple:
        for (u32 i = 0; i < pattern->b; ++i) {
            if (!sema_resolve_destructure_assign_pattern(
                    lexer,
                    ast,
                    sema,
                    scope_index,
                    ast->pattern_items[pattern->a + i])) {
                return false;
            }
        }
        return true;
    case APK_Plex:
        for (u32 i = 0; i < pattern->b; ++i) {
            if (!sema_resolve_destructure_assign_pattern(
                    lexer,
                    ast,
                    sema,
                    scope_index,
                    ast->pattern_fields[pattern->a + i].pattern_index)) {
                return false;
            }
        }
        return true;
    default:
        return error_0305_invalid_assignment_target(
            lexer->source, sema_pattern_span(lexer, pattern), s("pattern"));
    }
}

internal bool sema_collect_block_statements(const Lexer* lexer,
                                            const Ast*   ast,
                                            u32          owner_decl_index,
                                            u32   current_function_symbol,
                                            u32   scope_index,
                                            u32   first_node,
                                            u32   end_node,
                                            Sema* sema)
{
    for (u32 i = first_node; i < end_node;) {
        if (sema->node_is_type_expr[i]) {
            i++;
            continue;
        }

        const AstNode* node = &ast->nodes[i];
        if (node->kind == AK_FnStart) {
            i = node->b + 2;
            continue;
        }
        if (node->kind == AK_Block) {
            i = node->b;
            continue;
        }
        if (node->kind == AK_For) {
            const AstForInfo* for_info = &ast->fors[node->a];
            u32               for_end  = ast->nodes[node->b].b;
            if (for_info->else_block_index != U32_MAX) {
                for_end = ast->nodes[for_info->else_block_index].b;
            }
            i = for_end;
            continue;
        }
        if (node->kind != AK_Bind) {
            i++;
            continue;
        }

        u32            value_node_index = node->b;
        u32            type_node_index  = sema_no_type();
        const AstNode* value            = &ast->nodes[value_node_index];
        if (value->kind == AK_AnnotatedValue) {
            type_node_index  = value->a;
            value_node_index = value->b;
            value            = &ast->nodes[value_node_index];
        }

        u32 duplicate_local =
            sema_find_local_in_scope(sema, scope_index, node->a);
        if (duplicate_local != sema_no_local()) {
            const SemaLocal* previous = &sema->locals[duplicate_local];
            return error_0301_duplicate_binding(
                lexer->source,
                sema_node_span(lexer, node),
                lex_symbol(lexer, node->a),
                sema_local_span(lexer, ast, previous));
        }

        SemaLocalKind kind = ast->nodes[value_node_index].kind == AK_FnDef
                                 ? SLK_Function
                                 : SLK_Constant;
        u32           lowered_symbol_handle =
            kind == SLK_Function ? sema_mangle_nested_function_symbol(
                                       lexer, current_function_symbol, node->a)
                                           : node->a;

        if (type_node_index != sema_no_type()) {
            sema_mark_type_expr_nodes(ast, sema, type_node_index);
        }

        array_push(sema->locals,
                   (SemaLocal){
                       .kind                  = kind,
                       .symbol_handle         = node->a,
                       .owner_decl_index      = owner_decl_index,
                       .scope_index           = scope_index,
                       .decl_node_index       = i,
                       .decl_token_index      = U32_MAX,
                       .type_node_index       = type_node_index,
                       .value_node_index      = value_node_index,
                       .type_index            = sema_no_type(),
                       .lowered_symbol_handle = lowered_symbol_handle,
                   });
        sema->node_local_indices[i] = (u32)array_count(sema->locals) - 1;
        if (kind == SLK_Function) {
            sema->node_lowered_symbol_handles[value_node_index] =
                lowered_symbol_handle;
            sema_mark_fn_signature_type_nodes(ast, sema, value_node_index);
        }
        sema->scopes[scope_index].local_count++;
        i++;
    }

    for (u32 i = first_node; i < end_node;) {
        if (sema->node_is_type_expr[i]) {
            i++;
            continue;
        }
        const AstNode* node = &ast->nodes[i];
        if (node->kind == AK_Block) {
            u32 child_scope =
                sema_add_scope(sema, owner_decl_index, scope_index);
            sema->node_scope_indices[i] = child_scope;
            if (!sema_collect_block_statements(lexer,
                                               ast,
                                               owner_decl_index,
                                               current_function_symbol,
                                               child_scope,
                                               node->a,
                                               node->b,
                                               sema)) {
                return false;
            }
            i = node->b;
            continue;
        }

        if (node->kind == AK_Use) {
            u32 module_type = sema_no_type();
            if (!sema_infer_use_module_type(
                    lexer, ast, sema, i, scope_index, &module_type) ||
                !sema_import_module_exports_to_scope(
                    lexer, ast, sema, i, module_type, scope_index)) {
                return false;
            }
            i++;
            continue;
        }

        if (node->kind == AK_For) {
            const AstForInfo* for_info = &ast->fors[node->a];
            const AstNode*    body     = &ast->nodes[node->b];
            ASSERT(body->kind == AK_Block, "Expected for body block");
            u32 for_end = body->b;
            if (for_info->else_block_index != U32_MAX) {
                const AstNode* else_block =
                    &ast->nodes[for_info->else_block_index];
                ASSERT(else_block->kind == AK_Block, "Expected for else block");
                for_end = else_block->b;
            }
            u32 child_scope =
                sema_add_scope(sema, owner_decl_index, scope_index);
            if (!sema_collect_block_statements(lexer,
                                               ast,
                                               owner_decl_index,
                                               current_function_symbol,
                                               child_scope,
                                               i + 1,
                                               for_end,
                                               sema)) {
                return false;
            }
            if (for_info->condition_node_index != U32_MAX &&
                !sema_resolve_node_refs(lexer,
                                        ast,
                                        owner_decl_index,
                                        current_function_symbol,
                                        sema_no_scope(),
                                        child_scope,
                                        for_info->condition_node_index,
                                        sema)) {
                return false;
            }
            i = for_end;
            continue;
        }

        if (node->kind == AK_DestructureBind ||
            node->kind == AK_DestructureVariable) {
            u32            value_node_index = node->b;
            u32            type_node_index  = sema_no_type();
            const AstNode* payload          = &ast->nodes[value_node_index];
            if (payload->kind == AK_AnnotatedValue) {
                type_node_index  = payload->a;
                value_node_index = payload->b;
            }
            if (type_node_index != sema_no_type()) {
                sema_mark_type_expr_nodes(ast, sema, type_node_index);
            }
            if (!sema_resolve_node_refs(lexer,
                                        ast,
                                        owner_decl_index,
                                        current_function_symbol,
                                        sema_no_scope(),
                                        scope_index,
                                        value_node_index,
                                        sema)) {
                return false;
            }
            if (!sema_collect_destructure_pattern(
                    lexer,
                    ast,
                    sema,
                    owner_decl_index,
                    scope_index,
                    i,
                    node->kind == AK_DestructureBind ? SLK_Binder
                                                     : SLK_Variable,
                    node->a)) {
                return false;
            }
            i++;
            continue;
        }

        if (node->kind == AK_DestructureAssign) {
            if (!sema_resolve_destructure_assign_pattern(
                    lexer, ast, sema, scope_index, node->a)) {
                return false;
            }
            if (!sema_resolve_node_refs(lexer,
                                        ast,
                                        owner_decl_index,
                                        current_function_symbol,
                                        sema_no_scope(),
                                        scope_index,
                                        node->b,
                                        sema)) {
                return false;
            }
            i++;
            continue;
        }

        if (node->kind == AK_Bind) {
            u32 local_index = sema->node_local_indices[i];
            ASSERT(local_index != sema_no_local(), "Expected local bind");
            SemaLocal* local = &sema->locals[local_index];

            if (!sema_resolve_node_refs(lexer,
                                        ast,
                                        owner_decl_index,
                                        current_function_symbol,
                                        scope_index,
                                        scope_index,
                                        local->value_node_index,
                                        sema)) {
                return false;
            }
            i++;
            continue;
        }

        if (node->kind == AK_Variable) {
            u32 duplicate_index =
                sema_find_local_in_scope(sema, scope_index, node->a);
            if (duplicate_index != sema_no_local()) {
                const SemaLocal* previous = &sema->locals[duplicate_index];
                return error_0301_duplicate_binding(
                    lexer->source,
                    sema_node_span(lexer, node),
                    lex_symbol(lexer, node->a),
                    sema_local_span(lexer, ast, previous));
            }

            u32            type_node_index  = sema_no_type();
            u32            value_node_index = node->b;
            const AstNode* payload          = &ast->nodes[value_node_index];
            if (payload->kind == AK_AnnotatedValue) {
                type_node_index  = payload->a;
                value_node_index = payload->b;
            } else if (payload->kind == AK_ZeroInit) {
                type_node_index  = payload->a;
                value_node_index = sema_no_decl();
            }

            if (type_node_index != sema_no_type()) {
                sema_mark_type_expr_nodes(ast, sema, type_node_index);
            }
            if (value_node_index != sema_no_decl() &&
                !sema_resolve_node_refs(lexer,
                                        ast,
                                        owner_decl_index,
                                        current_function_symbol,
                                        sema_no_scope(),
                                        scope_index,
                                        value_node_index,
                                        sema)) {
                return false;
            }

            array_push(sema->locals,
                       (SemaLocal){
                           .kind                  = SLK_Variable,
                           .symbol_handle         = node->a,
                           .owner_decl_index      = owner_decl_index,
                           .scope_index           = scope_index,
                           .decl_node_index       = i,
                           .decl_token_index      = U32_MAX,
                           .type_node_index       = type_node_index,
                           .value_node_index      = value_node_index,
                           .type_index            = sema_no_type(),
                           .lowered_symbol_handle = node->a,
                       });
            sema->node_local_indices[i] = (u32)array_count(sema->locals) - 1;
            sema->scopes[scope_index].local_count++;
            i++;
            continue;
        }

        if (node->kind == AK_Assign) {
            u32 local_index = sema_lookup_local(sema, scope_index, node->a);
            if (local_index != sema_no_local()) {
                if (sema->locals[local_index].kind == SLK_Binder ||
                    !sema_local_is_runtime_value(&sema->locals[local_index])) {
                    return error_0305_invalid_assignment_target(
                        lexer->source,
                        sema_node_span(lexer, node),
                        lex_symbol(lexer, node->a));
                }
                sema->node_local_indices[i] = local_index;
            } else {
                u32 decl_index = sema_find_decl(sema, node->a);
                if (decl_index == sema_no_decl()) {
                    return error_0300_unknown_symbol(
                        lexer->source,
                        sema_node_span(lexer, node),
                        lex_symbol(lexer, node->a));
                }
                sema->node_decl_indices[i] = decl_index;
            }
            if (!sema_resolve_node_refs(lexer,
                                        ast,
                                        owner_decl_index,
                                        current_function_symbol,
                                        sema_no_scope(),
                                        scope_index,
                                        node->b,
                                        sema)) {
                return false;
            }
            i++;
            continue;
        }

        if (node->kind != AK_Return && node->kind != AK_Statement &&
            node->kind != AK_For && node->kind != AK_Break &&
            node->kind != AK_Continue) {
            i++;
            continue;
        }

        if (!sema_resolve_node_refs(lexer,
                                    ast,
                                    owner_decl_index,
                                    current_function_symbol,
                                    sema_no_scope(),
                                    scope_index,
                                    i,
                                    sema)) {
            return false;
        }
        i++;
    }

    return true;
}

internal bool sema_collect_function_locals(const Lexer* lexer,
                                           const Ast*   ast,
                                           u32          owner_decl_index,
                                           u32          current_function_symbol,
                                           u32          capture_scope_index,
                                           u32          fn_node_index,
                                           Sema*        sema)
{
    const AstNode* fn_def = &ast->nodes[fn_node_index];
    if (fn_def->kind != AK_FnDef) {
        return true;
    }

    const AstNode* fn_start = &ast->nodes[fn_def->a];
    u32 scope_index = sema_add_scope(sema, owner_decl_index, sema_no_scope());
    sema->node_scope_indices[fn_node_index] = scope_index;
    if (sema->node_lowered_symbol_handles[fn_node_index] == U32_MAX) {
        sema->node_lowered_symbol_handles[fn_node_index] =
            current_function_symbol;
    }
    const AstFnSignature* signature = &ast->fn_signatures[fn_start->a];

    for (u32 i = 0; i < signature->param_count; ++i) {
        const AstParam* param = &ast->params[signature->first_param + i];
        u32             duplicate_index =
            sema_find_local_in_scope(sema, scope_index, param->symbol_handle);
        if (duplicate_index != sema_no_local()) {
            const SemaLocal* previous = &sema->locals[duplicate_index];
            return error_0301_duplicate_binding(
                lexer->source,
                sema_token_span(lexer, param->token_index),
                lex_symbol(lexer, param->symbol_handle),
                sema_local_span(lexer, ast, previous));
        }

        sema_mark_type_expr_nodes(ast, sema, param->type_node_index);
        u32 param_type = sema_no_type();
        if (!sema_resolve_type_node(
                lexer, ast, sema, param->type_node_index, &param_type)) {
            return false;
        }
        array_push(sema->locals,
                   (SemaLocal){
                       .kind                  = SLK_Param,
                       .symbol_handle         = param->symbol_handle,
                       .owner_decl_index      = owner_decl_index,
                       .scope_index           = scope_index,
                       .decl_node_index       = sema_no_decl(),
                       .decl_token_index      = U32_MAX,
                       .type_node_index       = param->type_node_index,
                       .value_node_index      = sema_no_decl(),
                       .type_index            = param_type,
                       .lowered_symbol_handle = param->symbol_handle,
                   });
        sema->scopes[scope_index].local_count++;
    }

    if (fn_def->b != AFK_Block) {
        return sema_resolve_node_refs(lexer,
                                      ast,
                                      owner_decl_index,
                                      current_function_symbol,
                                      capture_scope_index,
                                      scope_index,
                                      ast->nodes[fn_def->a].b - 1,
                                      sema);
    }

    return sema_collect_block_statements(lexer,
                                         ast,
                                         owner_decl_index,
                                         current_function_symbol,
                                         scope_index,
                                         fn_def->a + 1,
                                         fn_start->b,
                                         sema);
}

internal bool sema_resolve_pattern_refs(const Lexer* lexer,
                                        const Ast*   ast,
                                        u32          owner_decl_index,
                                        u32          current_function_symbol,
                                        u32          capture_scope_index,
                                        u32          scope_index,
                                        u32          pattern_index,
                                        Sema*        sema)
{
    const AstPattern* pattern = &ast->patterns[pattern_index];
    switch (pattern->kind) {
    case APK_Value:
    case APK_Equal:
    case APK_NotEqual:
    case APK_Less:
    case APK_LessEqual:
    case APK_Greater:
    case APK_GreaterEqual:
        return sema_resolve_node_refs(lexer,
                                      ast,
                                      owner_decl_index,
                                      current_function_symbol,
                                      capture_scope_index,
                                      scope_index,
                                      pattern->a,
                                      sema);
    case APK_RangeExclusive:
    case APK_RangeInclusive:
        return sema_resolve_node_refs(lexer,
                                      ast,
                                      owner_decl_index,
                                      current_function_symbol,
                                      capture_scope_index,
                                      scope_index,
                                      pattern->a,
                                      sema) &&
               sema_resolve_node_refs(lexer,
                                      ast,
                                      owner_decl_index,
                                      current_function_symbol,
                                      capture_scope_index,
                                      scope_index,
                                      pattern->b,
                                      sema);
    case APK_Bind:
        return pattern->b == U32_MAX
                   ? true
                   : sema_resolve_pattern_refs(lexer,
                                               ast,
                                               owner_decl_index,
                                               current_function_symbol,
                                               capture_scope_index,
                                               scope_index,
                                               pattern->b,
                                               sema);
    case APK_Tuple:
        for (u32 i = 0; i < pattern->b; ++i) {
            if (!sema_resolve_pattern_refs(lexer,
                                           ast,
                                           owner_decl_index,
                                           current_function_symbol,
                                           capture_scope_index,
                                           scope_index,
                                           ast->pattern_items[pattern->a + i],
                                           sema)) {
                return false;
            }
        }
        return true;
    case APK_Plex:
        for (u32 i = 0; i < pattern->b; ++i) {
            if (!sema_resolve_pattern_refs(
                    lexer,
                    ast,
                    owner_decl_index,
                    current_function_symbol,
                    capture_scope_index,
                    scope_index,
                    ast->pattern_fields[pattern->a + i].pattern_index,
                    sema)) {
                return false;
            }
        }
        return true;
    case APK_EnumVariant:
        {
            const AstEnumPattern* enum_pattern =
                &ast->enum_patterns[pattern->a];
            for (u32 i = 0; i < enum_pattern->pattern_count; ++i) {
                if (!sema_resolve_pattern_refs(
                        lexer,
                        ast,
                        owner_decl_index,
                        current_function_symbol,
                        capture_scope_index,
                        scope_index,
                        ast->pattern_items[enum_pattern->first_pattern + i],
                        sema)) {
                    return false;
                }
            }
            return true;
        }
    case APK_Ignore:
        return true;
    }
    return true;
}

internal bool sema_resolve_node_refs(const Lexer* lexer,
                                     const Ast*   ast,
                                     u32          owner_decl_index,
                                     u32          current_function_symbol,
                                     u32          capture_scope_index,
                                     u32          scope_index,
                                     u32          node_index,
                                     Sema*        sema)
{
    const AstNode* node = &ast->nodes[node_index];

    switch (node->kind) {
    case AK_IntegerLiteral:
    case AK_FloatLiteral:
    case AK_StringLiteral:
    case AK_BoolLiteral:
    case AK_EnumVariant:
    case AK_ZeroInit:
    case AK_Continue:
    case AK_ContinueExpr:
    case AK_Block:
        return true;
    case AK_Break:
    case AK_BreakExpr:
        return node->a == U32_MAX ||
               sema_resolve_node_refs(lexer,
                                      ast,
                                      owner_decl_index,
                                      current_function_symbol,
                                      capture_scope_index,
                                      scope_index,
                                      node->a,
                                      sema);
    case AK_ExprBlock:
        {
            const AstNode* block = &ast->nodes[node->a];
            ASSERT(block->kind == AK_Block, "Expected expression block body");
            for (u32 i = block->a; i < block->b; ++i) {
                if (!ast_node_is_block_statement(&ast->nodes[i])) {
                    continue;
                }
                if (!sema_resolve_node_refs(lexer,
                                            ast,
                                            owner_decl_index,
                                            current_function_symbol,
                                            capture_scope_index,
                                            scope_index,
                                            i,
                                            sema)) {
                    return false;
                }
                i = ast_block_statement_end_exclusive(ast, i) - 1;
            }
            return true;
        }
    case AK_DestructureBind:
    case AK_DestructureVariable:
    case AK_DestructureAssign:
        return sema_resolve_node_refs(lexer,
                                      ast,
                                      owner_decl_index,
                                      current_function_symbol,
                                      capture_scope_index,
                                      scope_index,
                                      node->b,
                                      sema);
    case AK_Field:
        {
            u32 ignored = sema_no_type();
            if (sema_try_resolve_type_symbol(
                    lexer, ast, sema, node->a, &ignored)) {
                return true;
            }
            return sema_resolve_node_refs(lexer,
                                          ast,
                                          owner_decl_index,
                                          current_function_symbol,
                                          capture_scope_index,
                                          scope_index,
                                          node->a,
                                          sema);
        }
    case AK_InterpPartExpr:
        return sema_resolve_node_refs(lexer,
                                      ast,
                                      owner_decl_index,
                                      current_function_symbol,
                                      capture_scope_index,
                                      scope_index,
                                      node->a,
                                      sema);
    case AK_Slice:
        {
            const AstSliceInfo* slice = &ast->slices[node->a];
            if (!sema_resolve_node_refs(lexer,
                                        ast,
                                        owner_decl_index,
                                        current_function_symbol,
                                        capture_scope_index,
                                        scope_index,
                                        slice->target_node_index,
                                        sema)) {
                return false;
            }
            if (slice->start_node_index != U32_MAX &&
                !sema_resolve_node_refs(lexer,
                                        ast,
                                        owner_decl_index,
                                        current_function_symbol,
                                        capture_scope_index,
                                        scope_index,
                                        slice->start_node_index,
                                        sema)) {
                return false;
            }
            return slice->end_node_index == U32_MAX ||
                   sema_resolve_node_refs(lexer,
                                          ast,
                                          owner_decl_index,
                                          current_function_symbol,
                                          capture_scope_index,
                                          scope_index,
                                          slice->end_node_index,
                                          sema);
        }
    case AK_InterpolatedString:
        if (owner_decl_index == sema_no_decl() ||
            sema->decls[owner_decl_index].kind != SK_Function) {
            return error_0310_invalid_interpolation_context(
                lexer->source, sema_node_span(lexer, node));
        }
        for (u32 i = node->a; i < node->b; ++i) {
            if (!sema_resolve_node_refs(lexer,
                                        ast,
                                        owner_decl_index,
                                        current_function_symbol,
                                        capture_scope_index,
                                        scope_index,
                                        i,
                                        sema)) {
                return false;
            }
        }
        return true;
    case AK_StringConcat:
    case AK_On:
    case AK_IntegerPlus:
    case AK_IntegerMinus:
    case AK_IntegerMultiply:
    case AK_IntegerDivide:
    case AK_IntegerModulo:
    case AK_BitwiseAnd:
    case AK_BitwiseXor:
    case AK_BitwiseOr:
    case AK_Equal:
    case AK_NotEqual:
    case AK_Less:
    case AK_LessEqual:
    case AK_Greater:
    case AK_GreaterEqual:
    case AK_LogicalAnd:
    case AK_LogicalOr:
    case AK_RangeExclusive:
    case AK_RangeInclusive:
        if (node->kind == AK_On) {
            const AstOnInfo* on = &ast->ons[node->b];
            if (node->a != U32_MAX &&
                !sema_resolve_node_refs(lexer,
                                        ast,
                                        owner_decl_index,
                                        current_function_symbol,
                                        capture_scope_index,
                                        scope_index,
                                        node->a,
                                        sema)) {
                return false;
            }
            for (u32 i = 0; i < on->branch_count; ++i) {
                const AstOnBranch* branch =
                    &ast->on_branches[on->first_branch + i];
                u32  branch_scope        = scope_index;
                bool has_pattern_binders = false;
                if (!(branch->flags & AOBF_Else)) {
                    for (u32 pattern = 0; pattern < branch->pattern_count;
                         ++pattern) {
                        if (sema_pattern_contains_binder(
                                ast,
                                ast->pattern_items[branch->pattern_index +
                                                   pattern])) {
                            has_pattern_binders = true;
                            break;
                        }
                    }
                }
                if (branch->binder_symbol_handle != U32_MAX ||
                    has_pattern_binders) {
                    branch_scope =
                        sema_add_scope(sema, owner_decl_index, scope_index);
                }
                if (branch->binder_symbol_handle != U32_MAX) {
                    array_push(
                        sema->locals,
                        (SemaLocal){
                            .kind             = SLK_Binder,
                            .symbol_handle    = branch->binder_symbol_handle,
                            .owner_decl_index = owner_decl_index,
                            .scope_index      = branch_scope,
                            .decl_node_index  = sema_no_decl(),
                            .decl_token_index = branch->binder_token_index,
                            .type_node_index  = sema_no_type(),
                            .value_node_index = node->a,
                            .type_index       = sema_no_type(),
                            .lowered_symbol_handle =
                                branch->binder_symbol_handle,
                        });
                    sema->on_branch_local_indices[on->first_branch + i] =
                        (u32)array_count(sema->locals) - 1;
                    sema->scopes[branch_scope].local_count++;
                }
                if (!(branch->flags & AOBF_Else)) {
                    for (u32 pattern = 0; pattern < branch->pattern_count;
                         ++pattern) {
                        u32 pattern_index =
                            ast->pattern_items[branch->pattern_index + pattern];
                        if (!sema_collect_on_pattern_binders(
                                lexer,
                                ast,
                                sema,
                                owner_decl_index,
                                current_function_symbol,
                                branch_scope,
                                pattern_index)) {
                            return false;
                        }
                        if (!sema_resolve_pattern_refs(lexer,
                                                       ast,
                                                       owner_decl_index,
                                                       current_function_symbol,
                                                       capture_scope_index,
                                                       scope_index,
                                                       pattern_index,
                                                       sema)) {
                            return false;
                        }
                    }
                }
                if (branch->guard_node_index != U32_MAX &&
                    !sema_resolve_node_refs(lexer,
                                            ast,
                                            owner_decl_index,
                                            current_function_symbol,
                                            capture_scope_index,
                                            branch_scope,
                                            branch->guard_node_index,
                                            sema)) {
                    return false;
                }
                if (!sema_resolve_node_refs(lexer,
                                            ast,
                                            owner_decl_index,
                                            current_function_symbol,
                                            capture_scope_index,
                                            branch_scope,
                                            branch->expr_node_index,
                                            sema)) {
                    return false;
                }
            }
            return true;
        }
        return sema_resolve_node_refs(lexer,
                                      ast,
                                      owner_decl_index,
                                      current_function_symbol,
                                      capture_scope_index,
                                      scope_index,
                                      node->a,
                                      sema) &&
               sema_resolve_node_refs(lexer,
                                      ast,
                                      owner_decl_index,
                                      current_function_symbol,
                                      capture_scope_index,
                                      scope_index,
                                      node->b,
                                      sema);
    case AK_Call:
        {
            if (!sema_resolve_node_refs(lexer,
                                        ast,
                                        owner_decl_index,
                                        current_function_symbol,
                                        capture_scope_index,
                                        scope_index,
                                        node->a,
                                        sema)) {
                return false;
            }
            const AstCallInfo* call = &ast->calls[node->b];
            for (u32 i = 0; i < call->arg_count; ++i) {
                if (!sema_resolve_node_refs(lexer,
                                            ast,
                                            owner_decl_index,
                                            current_function_symbol,
                                            capture_scope_index,
                                            scope_index,
                                            ast->call_args[call->first_arg + i],
                                            sema)) {
                    return false;
                }
            }
            return true;
        }
    case AK_Tuple:
    case AK_Array:
        for (u32 i = 0; i < node->b; ++i) {
            if (!sema_resolve_node_refs(lexer,
                                        ast,
                                        owner_decl_index,
                                        current_function_symbol,
                                        capture_scope_index,
                                        scope_index,
                                        ast->tuple_items[node->a + i],
                                        sema)) {
                return false;
            }
        }
        return true;
    case AK_Plex:
    case AK_PlexUpdate:
        {
            const AstPlexLiteralInfo* literal = &ast->plex_literals[node->a];
            if (node->kind == AK_Plex) {
                sema_mark_type_expr_nodes(
                    ast, sema, literal->target_node_index);
            } else if (!sema_resolve_node_refs(lexer,
                                               ast,
                                               owner_decl_index,
                                               current_function_symbol,
                                               capture_scope_index,
                                               scope_index,
                                               literal->target_node_index,
                                               sema)) {
                return false;
            }
            for (u32 i = 0; i < literal->field_count; ++i) {
                if (!sema_resolve_node_refs(
                        lexer,
                        ast,
                        owner_decl_index,
                        current_function_symbol,
                        capture_scope_index,
                        scope_index,
                        ast->plex_literal_fields[literal->first_field + i]
                            .value_node_index,
                        sema)) {
                    return false;
                }
            }
            return true;
        }
    case AK_TupleField:
        return sema_resolve_node_refs(lexer,
                                      ast,
                                      owner_decl_index,
                                      current_function_symbol,
                                      capture_scope_index,
                                      scope_index,
                                      node->a,
                                      sema);
    case AK_Index:
    case AK_TypeArray:
        return sema_resolve_node_refs(lexer,
                                      ast,
                                      owner_decl_index,
                                      current_function_symbol,
                                      capture_scope_index,
                                      scope_index,
                                      node->a,
                                      sema) &&
               sema_resolve_node_refs(lexer,
                                      ast,
                                      owner_decl_index,
                                      current_function_symbol,
                                      capture_scope_index,
                                      scope_index,
                                      node->b,
                                      sema);
    case AK_IntegerNegate:
    case AK_LogicalNot:
    case AK_AddressOf:
    case AK_Cast:
    case AK_Expression:
    case AK_Statement:
    case AK_TypePointer:
    case AK_Use:
        return sema_resolve_node_refs(lexer,
                                      ast,
                                      owner_decl_index,
                                      current_function_symbol,
                                      capture_scope_index,
                                      scope_index,
                                      node->a,
                                      sema);
    case AK_FfiDef:
        {
            const AstFfiInfo* ffi_info = &ast->ffi_infos[node->a];
            return sema_resolve_node_refs(lexer,
                                          ast,
                                          owner_decl_index,
                                          current_function_symbol,
                                          capture_scope_index,
                                          scope_index,
                                          ffi_info->library_node_index,
                                          sema);
        }
    case AK_Return:
    case AK_ReturnExpr:
        return node->a == U32_MAX
                   ? true
                   : sema_resolve_node_refs(lexer,
                                            ast,
                                            owner_decl_index,
                                            current_function_symbol,
                                            capture_scope_index,
                                            scope_index,
                                            node->a,
                                            sema);
    case AK_For:
        {
            const AstForInfo* for_info  = &ast->fors[node->a];
            u32               for_scope = sema->node_scope_indices[node_index];
            if (for_scope == sema_no_scope()) {
                const AstNode* body = &ast->nodes[node->b];
                ASSERT(body->kind == AK_Block, "Expected for body block");
                u32 for_end = body->b;
                if (for_info->else_block_index != U32_MAX) {
                    const AstNode* else_block =
                        &ast->nodes[for_info->else_block_index];
                    ASSERT(else_block->kind == AK_Block,
                           "Expected for else block");
                    for_end = else_block->b;
                }
                for_scope = sema_add_scope(sema, owner_decl_index, scope_index);
                sema->node_scope_indices[node_index] = for_scope;
                if (!sema_collect_block_statements(lexer,
                                                   ast,
                                                   owner_decl_index,
                                                   current_function_symbol,
                                                   for_scope,
                                                   node_index + 1,
                                                   for_end,
                                                   sema)) {
                    return false;
                }
            }
            return for_info->condition_node_index == U32_MAX
                       ? true
                       : sema_resolve_node_refs(lexer,
                                                ast,
                                                owner_decl_index,
                                                current_function_symbol,
                                                capture_scope_index,
                                                for_scope,
                                                for_info->condition_node_index,
                                                sema);
        }
    case AK_SymbolRef:
        if (sema->node_is_type_expr[node_index]) {
            return true;
        }
        {
            u32 local_index = sema_lookup_local(sema, scope_index, node->a);
            if (local_index != sema_no_local()) {
                sema->node_local_indices[node_index] = local_index;
                return true;
            }

            local_index = sema_lookup_decl_local(sema, scope_index, node->a);
            if (local_index != sema_no_local()) {
                sema->node_local_indices[node_index] = local_index;
                return true;
            }

            u32 decl_index = sema_find_decl(sema, node->a);
            if (decl_index == sema_no_decl()) {
                if (capture_scope_index != sema_no_scope()) {
                    u32 capture_local =
                        sema_lookup_local(sema, capture_scope_index, node->a);
                    if (capture_local == sema_no_local()) {
                        capture_local = sema_lookup_decl_local(
                            sema, capture_scope_index, node->a);
                    }
                    if (capture_local != sema_no_local()) {
                        return error_0317_non_closure_capture(
                            lexer->source,
                            sema_node_span(lexer, node),
                            lex_symbol(lexer, node->a));
                    }
                }
                return true;
            }
            if (sema->decls[decl_index].kind == SK_TypeAlias) {
                return error_0308_type_used_as_value(
                    lexer->source,
                    sema_node_span(lexer, node),
                    lex_symbol(lexer, node->a));
            }
            sema->node_decl_indices[node_index] = decl_index;
            return true;
        }
    case AK_FnDef:
        if (sema->node_lowered_symbol_handles[node_index] == U32_MAX) {
            sema->node_lowered_symbol_handles[node_index] =
                sema_mangle_anonymous_function_symbol(
                    lexer, current_function_symbol, node_index);
        }
        sema_mark_fn_signature_type_nodes(ast, sema, node_index);
        return sema_collect_function_locals(
            lexer,
            ast,
            owner_decl_index,
            sema->node_lowered_symbol_handles[node_index],
            scope_index,
            node_index,
            sema);
    default:
        return true;
    }
}

//------------------------------------------------------------------------------
// Walk an expression subtree and record declaration dependencies.

internal void sema_collect_node_deps(const Ast*  ast,
                                     const Sema* sema,
                                     u32         owner_decl_index,
                                     u32         node_index,
                                     Sema*       out_sema);

internal void sema_collect_pattern_deps(const Ast*  ast,
                                        const Sema* sema,
                                        u32         owner_decl_index,
                                        u32         pattern_index,
                                        Sema*       out_sema)
{
    const AstPattern* pattern = &ast->patterns[pattern_index];
    switch (pattern->kind) {
    case APK_Value:
    case APK_Equal:
    case APK_NotEqual:
    case APK_Less:
    case APK_LessEqual:
    case APK_Greater:
    case APK_GreaterEqual:
        sema_collect_node_deps(
            ast, sema, owner_decl_index, pattern->a, out_sema);
        return;
    case APK_RangeExclusive:
    case APK_RangeInclusive:
        sema_collect_node_deps(
            ast, sema, owner_decl_index, pattern->a, out_sema);
        sema_collect_node_deps(
            ast, sema, owner_decl_index, pattern->b, out_sema);
        return;
    case APK_Bind:
        if (pattern->b != U32_MAX) {
            sema_collect_pattern_deps(
                ast, sema, owner_decl_index, pattern->b, out_sema);
        }
        return;
    case APK_Tuple:
        for (u32 i = 0; i < pattern->b; ++i) {
            sema_collect_pattern_deps(ast,
                                      sema,
                                      owner_decl_index,
                                      ast->pattern_items[pattern->a + i],
                                      out_sema);
        }
        return;
    case APK_Plex:
        for (u32 i = 0; i < pattern->b; ++i) {
            sema_collect_pattern_deps(
                ast,
                sema,
                owner_decl_index,
                ast->pattern_fields[pattern->a + i].pattern_index,
                out_sema);
        }
        return;
    case APK_EnumVariant:
        {
            const AstEnumPattern* enum_pattern =
                &ast->enum_patterns[pattern->a];
            for (u32 i = 0; i < enum_pattern->pattern_count; ++i) {
                sema_collect_pattern_deps(
                    ast,
                    sema,
                    owner_decl_index,
                    ast->pattern_items[enum_pattern->first_pattern + i],
                    out_sema);
            }
        }
        return;
    case APK_Ignore:
        return;
    }
}

internal void sema_collect_node_deps(const Ast*  ast,
                                     const Sema* sema,
                                     u32         owner_decl_index,
                                     u32         node_index,
                                     Sema*       out_sema)
{
    const AstNode* node = &ast->nodes[node_index];

    switch (node->kind) {
    case AK_IntegerLiteral:
    case AK_FloatLiteral:
    case AK_StringLiteral:
    case AK_BoolLiteral:
        return;
    case AK_InterpPartExpr:
        sema_collect_node_deps(ast, sema, owner_decl_index, node->a, out_sema);
        return;
    case AK_InterpolatedString:
        for (u32 i = node->a; i < node->b; ++i) {
            sema_collect_node_deps(ast, sema, owner_decl_index, i, out_sema);
        }
        return;
    case AK_StringConcat:
        sema_collect_node_deps(ast, sema, owner_decl_index, node->a, out_sema);
        sema_collect_node_deps(ast, sema, owner_decl_index, node->b, out_sema);
        return;
    case AK_On:
        {
            const AstOnInfo* on = &ast->ons[node->b];
            if (node->a != U32_MAX) {
                sema_collect_node_deps(
                    ast, sema, owner_decl_index, node->a, out_sema);
            }
            for (u32 i = 0; i < on->branch_count; ++i) {
                const AstOnBranch* branch =
                    &ast->on_branches[on->first_branch + i];
                if (!(branch->flags & AOBF_Else)) {
                    for (u32 pattern = 0; pattern < branch->pattern_count;
                         ++pattern) {
                        sema_collect_pattern_deps(
                            ast,
                            sema,
                            owner_decl_index,
                            ast->pattern_items[branch->pattern_index + pattern],
                            out_sema);
                    }
                }
                if (branch->guard_node_index != U32_MAX) {
                    sema_collect_node_deps(ast,
                                           sema,
                                           owner_decl_index,
                                           branch->guard_node_index,
                                           out_sema);
                }
                sema_collect_node_deps(ast,
                                       sema,
                                       owner_decl_index,
                                       branch->expr_node_index,
                                       out_sema);
            }
            return;
        }
    case AK_Call:
        sema_collect_node_deps(ast, sema, owner_decl_index, node->a, out_sema);
        {
            const AstCallInfo* call = &ast->calls[node->b];
            for (u32 i = 0; i < call->arg_count; ++i) {
                sema_collect_node_deps(ast,
                                       sema,
                                       owner_decl_index,
                                       ast->call_args[call->first_arg + i],
                                       out_sema);
            }
        }
        return;
    case AK_Tuple:
    case AK_Array:
        for (u32 i = 0; i < node->b; ++i) {
            sema_collect_node_deps(ast,
                                   sema,
                                   owner_decl_index,
                                   ast->tuple_items[node->a + i],
                                   out_sema);
        }
        return;
    case AK_Plex:
    case AK_PlexUpdate:
        {
            const AstPlexLiteralInfo* literal = &ast->plex_literals[node->a];
            if (node->kind == AK_PlexUpdate) {
                sema_collect_node_deps(ast,
                                       sema,
                                       owner_decl_index,
                                       literal->target_node_index,
                                       out_sema);
            }
            for (u32 i = 0; i < literal->field_count; ++i) {
                sema_collect_node_deps(
                    ast,
                    sema,
                    owner_decl_index,
                    ast->plex_literal_fields[literal->first_field + i]
                        .value_node_index,
                    out_sema);
            }
            return;
        }
    case AK_TupleField:
    case AK_Field:
    case AK_Index:
        if (node->kind == AK_Field &&
            node_index < array_count(sema->node_type_indices) &&
            sema->node_type_indices[node_index] != sema_no_type() &&
            sema->types[sema->node_type_indices[node_index]].kind == STK_Enum) {
            return;
        }
        sema_collect_node_deps(ast, sema, owner_decl_index, node->a, out_sema);
        if (node->kind == AK_Index) {
            sema_collect_node_deps(
                ast, sema, owner_decl_index, node->b, out_sema);
        }
        return;
    case AK_Slice:
        {
            const AstSliceInfo* slice = &ast->slices[node->a];
            sema_collect_node_deps(ast,
                                   sema,
                                   owner_decl_index,
                                   slice->target_node_index,
                                   out_sema);
            if (slice->start_node_index != U32_MAX) {
                sema_collect_node_deps(ast,
                                       sema,
                                       owner_decl_index,
                                       slice->start_node_index,
                                       out_sema);
            }
            if (slice->end_node_index != U32_MAX) {
                sema_collect_node_deps(ast,
                                       sema,
                                       owner_decl_index,
                                       slice->end_node_index,
                                       out_sema);
            }
            return;
        }
    case AK_AddressOf:
    case AK_TypeSlice:
    case AK_TypePointer:
        sema_collect_node_deps(ast, sema, owner_decl_index, node->a, out_sema);
        return;
    case AK_FfiDef:
        {
            const AstFfiInfo* ffi_info = &ast->ffi_infos[node->a];
            sema_collect_node_deps(ast,
                                   sema,
                                   owner_decl_index,
                                   ffi_info->library_node_index,
                                   out_sema);
            return;
        }
    case AK_SymbolRef:
        {
            if (sema->node_is_type_expr[node_index]) {
                return;
            }
            if (sema->node_local_indices[node_index] != sema_no_local()) {
                return;
            }
            u32 decl_index = sema->node_decl_indices[node_index];
            if (decl_index == sema_no_decl()) {
                return;
            }
            if (sema->decls[decl_index].kind == SK_BuiltinFunction ||
                sema->decls[decl_index].kind == SK_FfiFunction ||
                sema->decls[decl_index].kind == SK_Module) {
                return;
            }
            sema_add_dep(out_sema, owner_decl_index, decl_index);
            return;
        }
    case AK_IntegerNegate:
    case AK_LogicalNot:
    case AK_Cast:
    case AK_Expression:
    case AK_Statement:
    case AK_Use:
        sema_collect_node_deps(ast, sema, owner_decl_index, node->a, out_sema);
        return;
    case AK_Return:
    case AK_ReturnExpr:
        if (node->a != U32_MAX) {
            sema_collect_node_deps(
                ast, sema, owner_decl_index, node->a, out_sema);
        }
        return;
    case AK_Assign:
        sema_collect_node_deps(ast, sema, owner_decl_index, node->b, out_sema);
        if (sema->node_local_indices[node_index] == sema_no_local() &&
            sema->node_decl_indices[node_index] != sema_no_decl()) {
            sema_add_dep(out_sema,
                         owner_decl_index,
                         sema->node_decl_indices[node_index]);
        }
        return;
    case AK_DestructureBind:
    case AK_DestructureVariable:
    case AK_DestructureAssign:
        sema_collect_node_deps(ast, sema, owner_decl_index, node->b, out_sema);
        return;
    case AK_For:
        {
            const AstForInfo* for_info = &ast->fors[node->a];
            for (u32 i = 0; i < for_info->init_count; ++i) {
                sema_collect_node_deps(ast,
                                       sema,
                                       owner_decl_index,
                                       ast->for_items[for_info->first_init + i],
                                       out_sema);
            }
            if (for_info->condition_node_index != U32_MAX) {
                sema_collect_node_deps(ast,
                                       sema,
                                       owner_decl_index,
                                       for_info->condition_node_index,
                                       out_sema);
            }
            for (u32 i = 0; i < for_info->update_count; ++i) {
                sema_collect_node_deps(
                    ast,
                    sema,
                    owner_decl_index,
                    ast->for_items[for_info->first_update + i],
                    out_sema);
            }
            sema_collect_node_deps(
                ast, sema, owner_decl_index, node->b, out_sema);
            if (for_info->else_block_index != U32_MAX) {
                sema_collect_node_deps(ast,
                                       sema,
                                       owner_decl_index,
                                       for_info->else_block_index,
                                       out_sema);
            }
        }
        return;
    case AK_Block:
        for (u32 i = node->a; i < node->b; ++i) {
            sema_collect_node_deps(ast, sema, owner_decl_index, i, out_sema);
            i = ast_block_statement_end_exclusive(ast, i) - 1;
        }
        return;
    case AK_Variable:
        if (node->b < array_count(ast->nodes) &&
            ast->nodes[node->b].kind != AK_ZeroInit) {
            u32 value_node_index = node->b;
            if (ast->nodes[value_node_index].kind == AK_AnnotatedValue) {
                value_node_index = ast->nodes[value_node_index].b;
            }
            sema_collect_node_deps(
                ast, sema, owner_decl_index, value_node_index, out_sema);
        }
        return;
    case AK_IntegerPlus:
    case AK_IntegerMinus:
    case AK_IntegerMultiply:
    case AK_IntegerDivide:
    case AK_IntegerModulo:
    case AK_BitwiseAnd:
    case AK_BitwiseXor:
    case AK_BitwiseOr:
    case AK_Equal:
    case AK_NotEqual:
    case AK_Less:
    case AK_LessEqual:
    case AK_Greater:
    case AK_GreaterEqual:
    case AK_LogicalAnd:
    case AK_LogicalOr:
    case AK_RangeExclusive:
    case AK_RangeInclusive:
        sema_collect_node_deps(ast, sema, owner_decl_index, node->a, out_sema);
        sema_collect_node_deps(ast, sema, owner_decl_index, node->b, out_sema);
        return;
    case AK_FnDef:
        {
            const AstNode* fn_start = &ast->nodes[node->a];
            ASSERT(fn_start->kind == AK_FnStart,
                   "Expected function start node");
            for (u32 i = node->a + 1; i < fn_start->b; ++i) {
                if (ast->nodes[i].kind == AK_Block ||
                    ast->nodes[i].kind == AK_Statement ||
                    ast->nodes[i].kind == AK_Return ||
                    ast->nodes[i].kind == AK_For ||
                    ast->nodes[i].kind == AK_Variable ||
                    ast->nodes[i].kind == AK_Assign ||
                    (node->b == AFK_Expr &&
                     ast->nodes[i].kind == AK_Expression)) {
                    sema_collect_node_deps(
                        ast, sema, owner_decl_index, i, out_sema);
                    i = ast_block_statement_end_exclusive(ast, i) - 1;
                }
            }
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
        if (sema->decls[i].kind == SK_TypeAlias) {
            continue;
        }
        if (sema->decls[i].value_node_index == sema_no_decl()) {
            continue;
        }
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
    for (u32 i = 0; i < array_count(sema->decls); ++i) {
        const SemaDecl* decl = &sema->decls[i];
        if (decl->value_node_index == sema_no_decl()) {
            continue;
        }
        if (decl->kind == SK_Function) {
            sema->node_lowered_symbol_handles[decl->value_node_index] =
                decl->symbol_handle;
        }
        if (!sema_resolve_node_refs(lexer,
                                    ast,
                                    i,
                                    decl->symbol_handle,
                                    sema_no_scope(),
                                    sema_no_scope(),
                                    decl->value_node_index,
                                    sema)) {
            return false;
        }
    }

    return true;
}

internal bool
sema_collect_top_level_uses_in_range(const Lexer*           lexer,
                                     const Ast*             ast,
                                     const FrontEndOptions* options,
                                     u32                    first_node,
                                     u32                    end_node,
                                     u32   current_body_node_index,
                                     Sema* sema)
{
    for (u32 i = first_node; i < end_node;) {
        const AstNode* node = &ast->nodes[i];
        if (sema_node_is_inside_top_on_body(ast, i, current_body_node_index)) {
            i++;
            continue;
        }
        if (node->kind == AK_TopOn) {
            if (sema_top_on_is_enabled(options, lexer, ast, node)) {
                const AstTopOnInfo* info = &ast->top_ons[node->a];
                const AstNode*      body = &ast->nodes[info->body_node_index];
                ASSERT(body->kind == AK_Block, "Expected top-level on body");
                if (!sema_collect_top_level_uses_in_range(lexer,
                                                          ast,
                                                          options,
                                                          body->a,
                                                          body->b,
                                                          info->body_node_index,
                                                          sema)) {
                    return false;
                }
            }
            i++;
            continue;
        }
        if (node->kind == AK_FnStart) {
            i = node->b + 2;
            continue;
        }
        if (node->kind == AK_Block) {
            i = node->b;
            continue;
        }
        if (node->kind == AK_For) {
            const AstForInfo* for_info = &ast->fors[node->a];
            u32               for_end  = ast->nodes[node->b].b;
            if (for_info->else_block_index != U32_MAX) {
                for_end = ast->nodes[for_info->else_block_index].b;
            }
            i = for_end;
            continue;
        }
        if (node->kind != AK_Use) {
            i++;
            continue;
        }

        u32 module_type = sema_no_type();
        if (!sema_infer_use_module_type(
                lexer, ast, sema, i, sema_no_scope(), &module_type) ||
            !sema_import_module_exports_to_decls(
                lexer, ast, sema, i, module_type)) {
            return false;
        }
        i++;
    }
    return true;
}

internal bool sema_collect_top_level_uses(const Lexer*           lexer,
                                          const Ast*             ast,
                                          const FrontEndOptions* options,
                                          Sema*                  sema)
{
    return sema_collect_top_level_uses_in_range(
        lexer, ast, options, 0, (u32)array_count(ast->nodes), U32_MAX, sema);
}

//------------------------------------------------------------------------------
// Resolve one type-name symbol to a built-in semantic type.

internal u32 sema_type_index_for_name(Sema* sema, string name)
{
    if (string_eq(name, s("void"))) {
        return sema_builtin_type(sema, STK_Void);
    }
    if (string_eq(name, s("bool"))) {
        return sema_builtin_type(sema, STK_Bool);
    }
    if (string_eq(name, s("string"))) {
        return sema_builtin_type(sema, STK_String);
    }
    if (string_eq(name, s("i8"))) {
        return sema_builtin_type(sema, STK_I8);
    }
    if (string_eq(name, s("i16"))) {
        return sema_builtin_type(sema, STK_I16);
    }
    if (string_eq(name, s("i32"))) {
        return sema_builtin_type(sema, STK_I32);
    }
    if (string_eq(name, s("i64"))) {
        return sema_builtin_type(sema, STK_I64);
    }
    if (string_eq(name, s("u8"))) {
        return sema_builtin_type(sema, STK_U8);
    }
    if (string_eq(name, s("u16"))) {
        return sema_builtin_type(sema, STK_U16);
    }
    if (string_eq(name, s("u32"))) {
        return sema_builtin_type(sema, STK_U32);
    }
    if (string_eq(name, s("u64"))) {
        return sema_builtin_type(sema, STK_U64);
    }
    if (string_eq(name, s("f32"))) {
        return sema_builtin_type(sema, STK_F32);
    }
    if (string_eq(name, s("f64"))) {
        return sema_builtin_type(sema, STK_F64);
    }
    if (string_eq(name, s("isize"))) {
        return sema_builtin_type(sema, STK_Isize);
    }
    if (string_eq(name, s("usize"))) {
        return sema_builtin_type(sema, STK_Usize);
    }

    return sema_no_type();
}

//------------------------------------------------------------------------------
// Resolve one parsed type node into a semantic type row.

internal bool sema_resolve_type_node(const Lexer* lexer,
                                     const Ast*   ast,
                                     Sema*        sema,
                                     u32          node_index,
                                     u32*         out_type_index)
{
    const AstNode* node = &ast->nodes[node_index];

    switch (node->kind) {
    case AK_SymbolRef:
        {
            u32 type_index =
                sema_type_index_for_name(sema, lex_symbol(lexer, node->a));
            if (type_index == sema_no_type()) {
                u32 decl_index = sema_find_decl(sema, node->a);
                if (decl_index != sema_no_decl() &&
                    sema->decls[decl_index].kind == SK_TypeAlias) {
                    type_index = sema->decls[decl_index].type_index;
                }
            }
            if (type_index == sema_no_type()) {
                return error_0303_unknown_type(lexer->source,
                                               sema_node_span(lexer, node),
                                               lex_symbol(lexer, node->a));
            }
            sema->node_type_indices[node_index] = type_index;
            *out_type_index                     = type_index;
            return true;
        }

    case AK_TypeFn:
        {
            const AstFnSignature* signature = sema_ast_signature(ast, node);
            Array(u32) param_types          = NULL;

            for (u32 i = 0; i < signature->param_count; ++i) {
                u32 param_type = sema_no_type();
                if (!sema_resolve_type_node(
                        lexer,
                        ast,
                        sema,
                        ast->params[signature->first_param + i].type_node_index,
                        &param_type)) {
                    array_free(param_types);
                    return false;
                }
                array_push(param_types, param_type);
            }

            u32 return_type = sema_no_type();
            if (!sema_resolve_type_node(lexer,
                                        ast,
                                        sema,
                                        signature->return_type_node_index,
                                        &return_type)) {
                array_free(param_types);
                return false;
            }

            u32 type_index =
                sema_add_function_type(sema, param_types, return_type);
            array_free(param_types);
            sema->node_type_indices[node_index] = type_index;
            *out_type_index                     = type_index;
            return true;
        }

    case AK_TypeTuple:
        {
            Array(u32) item_types = NULL;
            for (u32 i = 0; i < node->b; ++i) {
                u32 item_type = sema_no_type();
                if (!sema_resolve_type_node(lexer,
                                            ast,
                                            sema,
                                            ast->tuple_items[node->a + i],
                                            &item_type)) {
                    array_free(item_types);
                    return false;
                }
                array_push(item_types, item_type);
            }

            u32 type_index = sema_add_tuple_type(sema, item_types);
            array_free(item_types);
            sema->node_type_indices[node_index] = type_index;
            *out_type_index                     = type_index;
            return true;
        }

    case AK_TypeArray:
        {
            i64 item_count = 0;
            if (!sema_try_eval_integer_constant(
                    lexer, ast, sema, node->a, &item_count) ||
                item_count < 0 || item_count > UINT32_MAX) {
                return error_0303_unknown_type(
                    lexer->source, sema_node_span(lexer, node), s("<array>"));
            }

            u32 item_type = sema_no_type();
            if (!sema_resolve_type_node(
                    lexer, ast, sema, node->b, &item_type)) {
                return false;
            }

            u32 type_index =
                sema_add_array_type(sema, item_type, (u32)item_count);
            sema->node_type_indices[node_index] = type_index;
            *out_type_index                     = type_index;
            return true;
        }

    case AK_TypeSlice:
        {
            u32 item_type = sema_no_type();
            if (!sema_resolve_type_node(
                    lexer, ast, sema, node->a, &item_type)) {
                return false;
            }

            u32 type_index = sema_add_slice_type(sema, item_type);
            sema->node_type_indices[node_index] = type_index;
            *out_type_index                     = type_index;
            return true;
        }

    case AK_TypePointer:
        {
            u32 pointee_type = sema_no_type();
            if (!sema_resolve_type_node(
                    lexer, ast, sema, node->a, &pointee_type)) {
                return false;
            }

            u32 type_index = sema_add_pointer_type(sema, pointee_type);
            sema->node_type_indices[node_index] = type_index;
            *out_type_index                     = type_index;
            return true;
        }

    case AK_TypePlex:
        {
            const AstPlexTypeInfo* plex = &ast->plex_types[node->a];
            Array(u32) field_types      = NULL;
            for (u32 i = 0; i < plex->field_count; ++i) {
                u32 field_type = sema_no_type();
                if (!sema_resolve_type_node(
                        lexer,
                        ast,
                        sema,
                        ast->plex_fields[plex->first_field + i].type_node_index,
                        &field_type)) {
                    array_free(field_types);
                    return false;
                }
                array_push(field_types, field_type);
            }
            u32 type_index =
                (plex->flags & APTF_Union)
                    ? sema_add_union_type(sema,
                                          &ast->plex_fields[plex->first_field],
                                          field_types,
                                          plex->field_count)
                    : sema_add_plex_type(sema,
                                         &ast->plex_fields[plex->first_field],
                                         field_types,
                                         plex->field_count,
                                         plex->flags);
            array_free(field_types);
            sema->node_type_indices[node_index] = type_index;
            *out_type_index                     = type_index;
            return true;
        }

    default:
        return error_0303_unknown_type(
            lexer->source, sema_node_span(lexer, node), s("<expression>"));
    }
}

//------------------------------------------------------------------------------
// Return whether one source type satisfies one target type without casts.

internal bool
sema_type_matches(const Sema* sema, u32 expected_type, u32 actual_type)
{
    if (expected_type == sema_no_type() || actual_type == sema_no_type()) {
        return true;
    }
    if (expected_type == actual_type) {
        return true;
    }

    if (sema_type_is_concrete_integer(sema, expected_type) &&
        sema->types[actual_type].kind == STK_UntypedInteger) {
        return true;
    }

    return sema_type_is_concrete_float(sema, expected_type) &&
           sema->types[actual_type].kind == STK_UntypedFloat;
}

internal u32 sema_enum_variant_index(const Sema* sema,
                                     u32         enum_type,
                                     u32         symbol_handle)
{
    if (enum_type == sema_no_type() ||
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

internal u32 sema_enum_variant_payload_type(const Sema* sema,
                                            u32         enum_type,
                                            u32         variant_index)
{
    if (enum_type == sema_no_type() ||
        sema->types[enum_type].kind != STK_Enum ||
        variant_index >= sema->types[enum_type].param_count) {
        return sema_no_type();
    }
    return sema->type_param_types[sema->types[enum_type].first_param_type +
                                  variant_index];
}

internal bool sema_node_is_contextual_enum_variant(const Ast*  ast,
                                                   const Sema* sema,
                                                   u32         node_index)
{
    if (node_index >= array_count(sema->node_type_indices)) {
        return false;
    }
    const AstNode* node = &ast->nodes[node_index];
    if (node->kind != AK_SymbolRef) {
        return false;
    }
    u32 type_index = sema->node_type_indices[node_index];
    return type_index != sema_no_type() &&
           sema_enum_variant_index(sema, type_index, node->a) != U32_MAX;
}

internal bool sema_try_resolve_type_symbol(const Lexer* lexer,
                                           const Ast*   ast,
                                           Sema*        sema,
                                           u32          node_index,
                                           u32*         out_type_index)
{
    const AstNode* node = &ast->nodes[node_index];
    if (node->kind != AK_SymbolRef ||
        sema->node_local_indices[node_index] != sema_no_local()) {
        return false;
    }

    u32 type_index = sema_type_index_for_name(sema, lex_symbol(lexer, node->a));
    if (type_index == sema_no_type()) {
        u32 decl_index = sema_find_decl(sema, node->a);
        if (decl_index != sema_no_decl() &&
            sema->decls[decl_index].kind == SK_TypeAlias) {
            sema->node_decl_indices[node_index] = decl_index;
            type_index = sema->decls[decl_index].type_index;
        }
    }
    if (type_index == sema_no_type()) {
        return false;
    }
    sema->node_is_type_expr[node_index] = true;
    sema->node_type_indices[node_index] = type_index;
    *out_type_index                     = type_index;
    return true;
}

internal bool sema_type_is_castable_primitive(const Sema* sema, u32 type_index)
{
    if (type_index == sema_no_type()) {
        return false;
    }

    switch (sema->types[type_index].kind) {
    case STK_UntypedInteger:
    case STK_UntypedFloat:
    case STK_Bool:
    case STK_I8:
    case STK_I16:
    case STK_I32:
    case STK_I64:
    case STK_U8:
    case STK_U16:
    case STK_U32:
    case STK_U64:
    case STK_F32:
    case STK_F64:
    case STK_Isize:
    case STK_Usize:
        return true;
    default:
        return false;
    }
}

internal bool sema_type_is_variable_storage(const Sema* sema, u32 type_index)
{
    if (type_index == sema_no_type()) {
        return false;
    }

    switch (sema->types[type_index].kind) {
    case STK_String:
    case STK_Bool:
    case STK_F32:
    case STK_F64:
    case STK_Function:
    case STK_Slice:
    case STK_Pointer:
    case STK_Enum:
        return true;
    case STK_Plex:
    case STK_Union:
        {
            const SemaType* record = &sema->types[type_index];
            for (u32 i = 0; i < record->param_count; ++i) {
                if (!sema_type_is_variable_storage(
                        sema,
                        sema->type_param_types[record->first_param_type + i])) {
                    return false;
                }
            }
            return true;
        }
    case STK_Tuple:
        {
            const SemaType* tuple = &sema->types[type_index];
            for (u32 i = 0; i < tuple->param_count; ++i) {
                if (!sema_type_is_variable_storage(
                        sema,
                        sema->type_param_types[tuple->first_param_type + i])) {
                    return false;
                }
            }
            return true;
        }
    case STK_Array:
        return sema_type_is_variable_storage(
            sema, sema->types[type_index].first_param_type);
    default:
        return sema_type_is_concrete_integer(sema, type_index);
    }
}

internal bool sema_type_is_interpolatable(const Sema* sema, u32 type_index)
{
    if (type_index == sema_no_type()) {
        return false;
    }

    switch (sema->types[type_index].kind) {
    case STK_String:
    case STK_Bool:
    case STK_F32:
    case STK_F64:
        return true;
    case STK_Tuple:
        {
            const SemaType* tuple = &sema->types[type_index];
            for (u32 i = 0; i < tuple->param_count; ++i) {
                if (!sema_type_is_interpolatable(
                        sema,
                        sema->type_param_types[tuple->first_param_type + i])) {
                    return false;
                }
            }
            return true;
        }
    case STK_Array:
        return sema_type_is_interpolatable(
            sema, sema->types[type_index].first_param_type);
    case STK_Slice:
        return sema_type_is_interpolatable(
            sema, sema->types[type_index].first_param_type);
    default:
        return sema_type_is_integer(sema, type_index);
    }
}

internal bool sema_type_is_equality_comparable(const Sema* sema, u32 type_index)
{
    if (type_index == sema_no_type()) {
        return false;
    }

    switch (sema->types[type_index].kind) {
    case STK_String:
    case STK_Bool:
    case STK_Enum:
        return true;
    default:
        return sema_type_is_numeric(sema, type_index);
    }
}

internal bool sema_on_covers_all_enum_variants(const Ast* ast,
                                               Sema*      sema,
                                               u32        on_index,
                                               u32        enum_type)
{
    if (enum_type == sema_no_type() ||
        sema->types[enum_type].kind != STK_Enum) {
        return false;
    }
    const AstOnInfo* on        = &ast->ons[on_index];
    const SemaType*  enum_info = &sema->types[enum_type];
    bool*            covered =
        arena_alloc(&temp_arena, sizeof(bool) * enum_info->param_count);
    memset(covered, 0, sizeof(bool) * enum_info->param_count);

    for (u32 branch_index = 0; branch_index < on->branch_count;
         ++branch_index) {
        const AstOnBranch* branch =
            &ast->on_branches[on->first_branch + branch_index];
        if ((branch->flags & AOBF_Else) ||
            branch->guard_node_index != U32_MAX) {
            continue;
        }
        for (u32 pattern_index = 0; pattern_index < branch->pattern_count;
             ++pattern_index) {
            const AstPattern* pattern =
                &ast->patterns[ast->pattern_items[branch->pattern_index +
                                                  pattern_index]];
            u32 variant = U32_MAX;
            if (pattern->kind == APK_EnumVariant) {
                const AstEnumPattern* enum_pattern =
                    &ast->enum_patterns[pattern->a];
                variant = sema_enum_variant_index(
                    sema, enum_type, enum_pattern->symbol_handle);
            } else if (pattern->kind == APK_Value) {
                const AstNode* pattern_node = &ast->nodes[pattern->a];
                if (pattern_node->kind != AK_EnumVariant &&
                    pattern_node->kind != AK_SymbolRef &&
                    pattern_node->kind != AK_Field) {
                    continue;
                }
                if (pattern_node->kind == AK_Field &&
                    sema->node_type_indices[pattern->a] == enum_type) {
                    variant = sema_enum_variant_index(
                        sema, enum_type, pattern_node->b);
                } else {
                    variant = sema_enum_variant_index(
                        sema, enum_type, pattern_node->a);
                }
            }
            if (variant != U32_MAX) {
                covered[variant] = true;
            }
        }
    }

    for (u32 i = 0; i < enum_info->param_count; ++i) {
        if (!covered[i]) {
            return false;
        }
    }
    return true;
}

internal u32 sema_expected_numeric_type(const Sema* sema, u32 expected_type)
{
    if (expected_type == sema_no_type()) {
        return sema_no_type();
    }

    if (sema_type_is_concrete_integer(sema, expected_type) ||
        sema_type_is_concrete_float(sema, expected_type)) {
        return expected_type;
    }

    return sema_no_type();
}

internal bool sema_infer_node_type(const Lexer* lexer,
                                   const Ast*   ast,
                                   Sema*        sema,
                                   u32          node_index,
                                   u32          expected_type,
                                   u32*         out_type_index);
internal bool sema_node_contains_interpolation(const Ast* ast, u32 node_index);
internal bool sema_pattern_contains_interpolation(const Ast* ast,
                                                  u32        pattern_index);
internal u32 sema_find_interpolated_string_node(const Ast* ast, u32 node_index);
internal u32 sema_find_interpolated_string_pattern(const Ast* ast,
                                                   u32        pattern_index);

internal bool sema_integer_literal_is_packed(const Lexer*   lexer,
                                             const AstNode* node)
{
    ASSERT(node->kind == AK_IntegerLiteral, "Expected integer literal node");
    ASSERT(node->token_index < array_count(lexer->tokens),
           "Integer literal token index out of bounds");
    const Token* token = &lexer->tokens[node->token_index];
    return token->kind == TK_Integer &&
           token->offset < lexer->source.source.count &&
           lexer->source.source.data[token->offset] == '\'';
}

internal u32 sema_packed_integer_literal_type(const Lexer*   lexer,
                                              const AstNode* node,
                                              Sema*          sema)
{
    const Token* token = &lexer->tokens[node->token_index];
    usize        start = token->offset + 1;
    usize        end   = lex_token_end_offset(lexer, token);
    usize        bytes = 0;

    ASSERT(end > start, "Packed integer literal must include closing quote");
    end--;

    for (usize i = start; i < end; ++i) {
        if (lexer->source.source.data[i] == '\\' && i + 1 < end) {
            i++;
        }
        bytes++;
    }

    if (bytes <= 1) {
        return sema_builtin_type(sema, STK_U8);
    }
    if (bytes <= 2) {
        return sema_builtin_type(sema, STK_U16);
    }
    if (bytes <= 4) {
        return sema_builtin_type(sema, STK_U32);
    }
    return sema_builtin_type(sema, STK_U64);
}
internal bool sema_validate_interpolated_strings(const Lexer* lexer,
                                                 const Ast*   ast,
                                                 Sema*        sema,
                                                 u32          node_index);
internal bool sema_validate_interpolated_string_pattern(const Lexer* lexer,
                                                        const Ast*   ast,
                                                        Sema*        sema,
                                                        u32 pattern_index);
internal bool sema_infer_local_binding_type(const Lexer* lexer,
                                            const Ast*   ast,
                                            Sema*        sema,
                                            u32          local_index,
                                            u32*         out_type_index);

internal bool sema_check_on_pattern_type(const Lexer* lexer,
                                         const Ast*   ast,
                                         Sema*        sema,
                                         u32          pattern_index,
                                         u32          value_type,
                                         bool         top_level)
{
    const AstPattern* pattern = &ast->patterns[pattern_index];
    switch (pattern->kind) {
    case APK_Value:
    case APK_Equal:
    case APK_NotEqual:
    case APK_Less:
    case APK_LessEqual:
    case APK_Greater:
    case APK_GreaterEqual:
        {
            u32 pattern_type = sema_no_type();
            if (!sema_infer_node_type(
                    lexer, ast, sema, pattern->a, value_type, &pattern_type)) {
                return false;
            }
            if (!sema_expr_is_constantish(ast, sema, pattern->a)) {
                return error_0322_non_constant_on_pattern(
                    lexer->source,
                    sema_node_span(lexer, &ast->nodes[pattern->a]));
            }
            if (pattern->kind != APK_Value && pattern_type != value_type) {
                return error_0304_type_mismatch(
                    lexer->source,
                    sema_pattern_span(lexer, pattern),
                    sema_type_name(lexer, sema, &temp_arena, value_type),
                    sema_type_name(lexer, sema, &temp_arena, pattern_type));
            }
            if ((pattern->kind == APK_Less || pattern->kind == APK_LessEqual ||
                 pattern->kind == APK_Greater ||
                 pattern->kind == APK_GreaterEqual) &&
                !sema_type_is_numeric(sema, value_type)) {
                string actual =
                    sema_type_name(lexer, sema, &temp_arena, value_type);
                return error_0326_invalid_binary_operands(
                    lexer->source,
                    sema_pattern_span(lexer, pattern),
                    pattern->kind == APK_Less
                        ? s("<")
                        : (pattern->kind == APK_LessEqual
                               ? s("<=")
                               : (pattern->kind == APK_Greater ? s(">")
                                                               : s(">="))),
                    s("matching numeric operands"),
                    actual,
                    actual);
            }
            return true;
        }
    case APK_RangeExclusive:
    case APK_RangeInclusive:
        {
            if (!sema_type_is_concrete_integer(sema, value_type)) {
                string actual =
                    sema_type_name(lexer, sema, &temp_arena, value_type);
                return error_0326_invalid_binary_operands(
                    lexer->source,
                    sema_pattern_span(lexer, pattern),
                    s(".."),
                    s("integer range bounds"),
                    actual,
                    actual);
            }
            u32 start_type = sema_no_type();
            u32 end_type   = sema_no_type();
            if (!sema_infer_node_type(
                    lexer, ast, sema, pattern->a, value_type, &start_type) ||
                !sema_infer_node_type(
                    lexer, ast, sema, pattern->b, value_type, &end_type)) {
                return false;
            }
            if (!(sema_expr_is_constantish(ast, sema, pattern->a) &&
                  sema_expr_is_constantish(ast, sema, pattern->b))) {
                return error_0322_non_constant_on_pattern(
                    lexer->source, sema_pattern_span(lexer, pattern));
            }

            i64 start_value = 0;
            i64 end_value   = 0;
            if (sema_try_eval_integer_constant(
                    lexer, ast, sema, pattern->a, &start_value) &&
                sema_try_eval_integer_constant(
                    lexer, ast, sema, pattern->b, &end_value)) {
                bool inclusive = pattern->kind == APK_RangeInclusive;
                bool valid     = inclusive ? start_value <= end_value
                                           : start_value < end_value;
                if (!valid) {
                    return error_0324_invalid_on_range_bounds(
                        lexer->source,
                        sema_pattern_span(lexer, pattern),
                        inclusive);
                }
            }
            return true;
        }
    case APK_Bind:
        {
            u32 local_index = sema->pattern_local_indices[pattern_index];
            if (local_index != sema_no_local()) {
                sema->locals[local_index].type_index = value_type;
            }
            if (pattern->b != U32_MAX) {
                return sema_check_on_pattern_type(
                    lexer, ast, sema, pattern->b, value_type, false);
            }
            return true;
        }
    case APK_Tuple:
        {
            if (value_type == sema_no_type() ||
                sema->types[value_type].kind != STK_Tuple) {
                return error_0304_type_mismatch(
                    lexer->source,
                    sema_pattern_span(lexer, pattern),
                    s("tuple"),
                    sema_type_name(lexer, sema, &temp_arena, value_type));
            }
            const SemaType* tuple = &sema->types[value_type];
            if (tuple->param_count != pattern->b) {
                return error_0304_type_mismatch(
                    lexer->source,
                    sema_pattern_span(lexer, pattern),
                    s("tuple with matching arity"),
                    sema_type_name(lexer, sema, &temp_arena, value_type));
            }
            for (u32 i = 0; i < pattern->b; ++i) {
                if (!sema_check_on_pattern_type(
                        lexer,
                        ast,
                        sema,
                        ast->pattern_items[pattern->a + i],
                        sema->type_param_types[tuple->first_param_type + i],
                        false)) {
                    return false;
                }
            }
            return true;
        }
    case APK_Plex:
        {
            if (value_type == sema_no_type() ||
                sema->types[value_type].kind != STK_Plex) {
                return error_0304_type_mismatch(
                    lexer->source,
                    sema_pattern_span(lexer, pattern),
                    s("plex"),
                    sema_type_name(lexer, sema, &temp_arena, value_type));
            }
            const SemaType* plex = &sema->types[value_type];
            for (u32 i = 0; i < pattern->b; ++i) {
                const AstPlexPatternField* field =
                    &ast->pattern_fields[pattern->a + i];
                u32 field_type = sema_no_type();
                for (u32 j = 0; j < plex->param_count; ++j) {
                    if (sema->type_param_symbols[plex->first_param_type + j] ==
                        field->symbol_handle) {
                        field_type =
                            sema->type_param_types[plex->first_param_type + j];
                        break;
                    }
                }
                if (field_type == sema_no_type()) {
                    return error_0304_type_mismatch(
                        lexer->source,
                        sema_pattern_span(lexer, pattern),
                        s("known plex field"),
                        lex_symbol(lexer, field->symbol_handle));
                }
                if (!sema_check_on_pattern_type(lexer,
                                                ast,
                                                sema,
                                                field->pattern_index,
                                                field_type,
                                                false)) {
                    return false;
                }
            }
            return true;
        }
    case APK_EnumVariant:
        {
            if (value_type == sema_no_type() ||
                sema->types[value_type].kind != STK_Enum) {
                return error_0304_type_mismatch(
                    lexer->source,
                    sema_pattern_span(lexer, pattern),
                    s("enum"),
                    sema_type_name(lexer, sema, &temp_arena, value_type));
            }
            const AstEnumPattern* enum_pattern =
                &ast->enum_patterns[pattern->a];
            if (enum_pattern->qualifier_node_index != U32_MAX) {
                u32 qualified_type = sema_no_type();
                if (!sema_resolve_type_node(lexer,
                                            ast,
                                            sema,
                                            enum_pattern->qualifier_node_index,
                                            &qualified_type)) {
                    return false;
                }
                if (qualified_type != value_type) {
                    return error_0304_type_mismatch(
                        lexer->source,
                        sema_pattern_span(lexer, pattern),
                        sema_type_name(lexer, sema, &temp_arena, value_type),
                        sema_type_name(
                            lexer, sema, &temp_arena, qualified_type));
                }
            }
            u32 variant = sema_enum_variant_index(
                sema, value_type, enum_pattern->symbol_handle);
            if (variant == U32_MAX) {
                return error_0304_type_mismatch(
                    lexer->source,
                    sema_pattern_span(lexer, pattern),
                    s("known enum variant"),
                    lex_symbol(lexer, enum_pattern->symbol_handle));
            }
            u32 payload_type =
                sema_enum_variant_payload_type(sema, value_type, variant);
            u32 expected_count =
                payload_type == sema_no_type()
                    ? 0
                    : (sema->types[payload_type].kind == STK_Tuple
                           ? sema->types[payload_type].param_count
                           : 1);
            if (expected_count != enum_pattern->pattern_count) {
                return error_0313_argument_count_mismatch(
                    lexer->source,
                    sema_pattern_span(lexer, pattern),
                    expected_count,
                    enum_pattern->pattern_count);
            }
            for (u32 i = 0; i < enum_pattern->pattern_count; ++i) {
                u32 expected_item =
                    sema->types[payload_type].kind == STK_Tuple
                        ? sema->type_param_types
                              [sema->types[payload_type].first_param_type + i]
                        : payload_type;
                if (!sema_check_on_pattern_type(
                        lexer,
                        ast,
                        sema,
                        ast->pattern_items[enum_pattern->first_pattern + i],
                        expected_item,
                        false)) {
                    return false;
                }
            }
            return true;
        }
    case APK_Ignore:
        if (top_level) {
            return error_0322_non_constant_on_pattern(
                lexer->source, sema_pattern_span(lexer, pattern));
        }
        return true;
    }
    return true;
}

internal bool
sema_node_is_addressable(const Ast* ast, Sema* sema, u32 node_index)
{
    const AstNode* node = &ast->nodes[node_index];
    switch (node->kind) {
    case AK_SymbolRef:
        if (sema->node_local_indices[node_index] != sema_no_local()) {
            const SemaLocal* local =
                &sema->locals[sema->node_local_indices[node_index]];
            return local->kind == SLK_Param || local->kind == SLK_Variable ||
                   local->kind == SLK_Binder;
        }
        if (sema->node_decl_indices[node_index] != U32_MAX) {
            const SemaDecl* decl =
                &sema->decls[sema->node_decl_indices[node_index]];
            return decl->kind == SK_Constant || decl->kind == SK_Variable ||
                   decl->kind == SK_Function || decl->kind == SK_FfiFunction;
        }
        return false;
    case AK_Array:
        return true;
    case AK_Index:
        {
            u32 target_type = sema->node_type_indices[node->a];
            if (target_type == sema_no_type()) {
                return false;
            }
            return sema->types[target_type].kind == STK_Array ||
                   sema->types[target_type].kind == STK_Pointer;
        }
    default:
        return false;
    }
}

internal bool sema_merge_control_break_type(const Lexer* lexer,
                                            const Ast*   ast,
                                            Sema*        sema,
                                            u32          break_node_index,
                                            u32          expected_type,
                                            u32*         in_out_result_type,
                                            u32* in_out_result_type_node)
{
    const AstNode* break_node = &ast->nodes[break_node_index];
    ASSERT(break_node->kind == AK_Break, "Expected break node");
    ASSERT(break_node->a != U32_MAX, "Expected value-producing break");

    u32 break_expected = *in_out_result_type == sema_no_type()
                             ? expected_type
                             : *in_out_result_type;
    u32 break_type     = sema_no_type();
    if (!sema_infer_node_type(
            lexer, ast, sema, break_node->a, break_expected, &break_type)) {
        return false;
    }

    if (*in_out_result_type == sema_no_type()) {
        *in_out_result_type      = break_type;
        *in_out_result_type_node = break_node->a;
    } else if (sema_type_is_concrete_integer(sema, *in_out_result_type) &&
               sema->types[break_type].kind == STK_UntypedInteger) {
        break_type = *in_out_result_type;
    } else if (sema_type_is_concrete_float(sema, *in_out_result_type) &&
               sema->types[break_type].kind == STK_UntypedFloat) {
        break_type = *in_out_result_type;
    } else if (*in_out_result_type != break_type) {
        Arena temp_arena = {0};
        arena_init(&temp_arena);
        bool ok = error_0320_on_branch_type_mismatch(
            lexer->source,
            sema_node_span(lexer, &ast->nodes[*in_out_result_type_node]),
            sema_type_name(lexer, sema, &temp_arena, *in_out_result_type),
            sema_node_span(lexer, &ast->nodes[break_node->a]),
            sema_type_name(lexer, sema, &temp_arena, break_type));
        arena_done(&temp_arena);
        return ok;
    }

    sema->node_type_indices[break_node_index] = break_type;
    return true;
}

internal bool sema_collect_value_breaks_for_target(const Lexer* lexer,
                                                   const Ast*   ast,
                                                   Sema*        sema,
                                                   u32          node_index,
                                                   u32          target_label,
                                                   u32          expected_type,
                                                   u32* in_out_result_type,
                                                   u32* in_out_result_type_node,
                                                   bool* out_has_value_break)
{
    const AstNode* node = &ast->nodes[node_index];
    if (node->kind == AK_Break) {
        if (node->a != U32_MAX &&
            (node->b == U32_MAX || node->b == target_label)) {
            if (!sema_merge_control_break_type(lexer,
                                               ast,
                                               sema,
                                               node_index,
                                               expected_type,
                                               in_out_result_type,
                                               in_out_result_type_node)) {
                return false;
            }
            *out_has_value_break = true;
        }
        return true;
    }

    if (node->kind == AK_Statement || node->kind == AK_Expression) {
        return sema_collect_value_breaks_for_target(lexer,
                                                    ast,
                                                    sema,
                                                    node->a,
                                                    target_label,
                                                    expected_type,
                                                    in_out_result_type,
                                                    in_out_result_type_node,
                                                    out_has_value_break);
    }

    if (node->kind == AK_On) {
        const AstOnInfo* on = &ast->ons[node->b];
        for (u32 i = 0; i < on->branch_count; ++i) {
            const AstOnBranch* branch = &ast->on_branches[on->first_branch + i];
            if (!sema_collect_value_breaks_for_target(lexer,
                                                      ast,
                                                      sema,
                                                      branch->expr_node_index,
                                                      target_label,
                                                      expected_type,
                                                      in_out_result_type,
                                                      in_out_result_type_node,
                                                      out_has_value_break)) {
                return false;
            }
        }
    }

    return true;
}

internal bool sema_node_has_value_break_for_target(const Ast* ast,
                                                   u32        node_index,
                                                   u32        target_label)
{
    const AstNode* node = &ast->nodes[node_index];
    if (node->kind == AK_Break) {
        return node->a != U32_MAX &&
               (node->b == U32_MAX || node->b == target_label);
    }
    if (node->kind == AK_Statement || node->kind == AK_Expression) {
        return sema_node_has_value_break_for_target(ast, node->a, target_label);
    }
    if (node->kind == AK_On) {
        const AstOnInfo* on = &ast->ons[node->b];
        for (u32 i = 0; i < on->branch_count; ++i) {
            const AstOnBranch* branch = &ast->on_branches[on->first_branch + i];
            if (sema_node_has_value_break_for_target(
                    ast, branch->expr_node_index, target_label)) {
                return true;
            }
        }
    }
    return false;
}

internal bool sema_block_has_value_break_for_target(const Ast* ast,
                                                    u32        block_index,
                                                    u32        target_label)
{
    const AstNode* block = &ast->nodes[block_index];
    ASSERT(block->kind == AK_Block, "Expected block");
    for (u32 i = block->a; i < block->b; ++i) {
        if (ast_node_is_block_statement(&ast->nodes[i]) &&
            sema_node_has_value_break_for_target(ast, i, target_label)) {
            return true;
        }
        i = ast_block_statement_end_exclusive(ast, i) - 1;
    }
    return false;
}

internal bool sema_infer_local_binding_type(const Lexer* lexer,
                                            const Ast*   ast,
                                            Sema*        sema,
                                            u32          local_index,
                                            u32*         out_type_index)
{
    SemaLocal* local = &sema->locals[local_index];
    if (local->type_index != sema_no_type()) {
        *out_type_index = local->type_index;
        return true;
    }

    AstNode* bind_node = &ast->nodes[local->decl_node_index];
    if (ast_has_flag(bind_node, ANF_ConstBusy)) {
        return error_0302_dependency_cycle(
            lexer->source,
            sema_local_span(lexer, ast, local),
            lex_symbol(lexer, local->symbol_handle),
            sema_local_span(lexer, ast, local),
            lex_symbol(lexer, local->symbol_handle));
    }

    ast_set_flag(bind_node, ANF_ConstBusy);

    u32  annotated = sema_no_type();
    u32  inferred  = sema_no_type();
    bool ok        = true;

    if (local->type_node_index != sema_no_type() &&
        !sema_resolve_type_node(
            lexer, ast, sema, local->type_node_index, &annotated)) {
        ok = false;
    }

    if (ok && local->value_node_index != sema_no_decl() &&
        !sema_infer_node_type(
            lexer, ast, sema, local->value_node_index, annotated, &inferred)) {
        ok = false;
    }

    if (ok) {
        local->type_index =
            annotated != sema_no_type()
                ? annotated
                : (local->kind == SLK_Function || local->kind == SLK_Constant
                       ? inferred
                       : sema_materialise_type(sema, inferred));
        sema->node_type_indices[local->decl_node_index] = local->type_index;
        *out_type_index                                 = local->type_index;
    }

    ast_clear_flag(bind_node, ANF_ConstBusy);
    return ok;
}

internal bool sema_infer_block_statements(const Lexer* lexer,
                                          const Ast*   ast,
                                          Sema*        sema,
                                          u32          first_node,
                                          u32          end_node,
                                          u32*         in_out_return_type,
                                          bool*        out_has_return)
{
    for (u32 i = first_node; i < end_node; ++i) {
        const AstNode* stmt = &ast->nodes[i];
        if (!ast_node_is_block_statement(stmt)) {
            continue;
        }

        if (stmt->kind == AK_Block) {
            if (!sema_infer_block_statements(lexer,
                                             ast,
                                             sema,
                                             stmt->a,
                                             stmt->b,
                                             in_out_return_type,
                                             out_has_return)) {
                return false;
            }
            if (*out_has_return) {
                return true;
            }
            i = stmt->b - 1;
            continue;
        }

        if (stmt->kind == AK_For) {
            const AstNode* body = &ast->nodes[stmt->b];
            ASSERT(body->kind == AK_Block, "Expected for body block");
            const AstForInfo* for_info = &ast->fors[stmt->a];
            for (u32 item = 0; item < for_info->init_count; ++item) {
                u32 item_node = ast->for_items[for_info->first_init + item];
                if (ast->nodes[item_node].kind == AK_Bind) {
                    continue;
                }
                u32 ignored = sema_no_type();
                if (!sema_infer_node_type(lexer,
                                          ast,
                                          sema,
                                          item_node,
                                          sema_no_type(),
                                          &ignored)) {
                    return false;
                }
            }
            if (for_info->condition_node_index != U32_MAX) {
                u32 condition_type = sema_no_type();
                if (!sema_infer_node_type(lexer,
                                          ast,
                                          sema,
                                          for_info->condition_node_index,
                                          sema_builtin_type(sema, STK_Bool),
                                          &condition_type)) {
                    return false;
                }
            }
            bool loop_has_return = false;
            if (!sema_infer_block_statements(lexer,
                                             ast,
                                             sema,
                                             body->a,
                                             body->b,
                                             in_out_return_type,
                                             &loop_has_return)) {
                return false;
            }
            for (u32 item = 0; item < for_info->update_count; ++item) {
                u32 item_node = ast->for_items[for_info->first_update + item];
                u32 ignored   = sema_no_type();
                if (!sema_infer_node_type(lexer,
                                          ast,
                                          sema,
                                          item_node,
                                          sema_no_type(),
                                          &ignored)) {
                    return false;
                }
            }
            sema->node_type_indices[i] = sema_builtin_type(sema, STK_Void);
            if (for_info->condition_node_index == U32_MAX && loop_has_return) {
                *out_has_return = true;
                return true;
            }
            i = body->b - 1;
            continue;
        }

        if (stmt->kind == AK_Variable || stmt->kind == AK_Assign ||
            stmt->kind == AK_DestructureBind ||
            stmt->kind == AK_DestructureVariable ||
            stmt->kind == AK_DestructureAssign) {
            u32 ignored = sema_no_type();
            if (!sema_infer_node_type(
                    lexer, ast, sema, i, sema_no_type(), &ignored)) {
                return false;
            }
            continue;
        }

        if (stmt->kind == AK_Use) {
            sema->node_type_indices[i] = sema_builtin_type(sema, STK_Void);
            continue;
        }

        if (stmt->kind == AK_Statement) {
            u32 ignored = sema_no_type();
            if (!sema_infer_node_type(
                    lexer, ast, sema, i, sema_no_type(), &ignored)) {
                return false;
            }
            if (sema_node_contains_interpolation(ast, i) &&
                !sema_validate_interpolated_strings(lexer, ast, sema, i)) {
                return false;
            }
            continue;
        }

        if (stmt->kind == AK_For) {
            u32 ignored = sema_no_type();
            if (!sema_infer_node_type(
                    lexer, ast, sema, i, sema_no_type(), &ignored)) {
                return false;
            }
            continue;
        }

        if (stmt->kind == AK_Break || stmt->kind == AK_Continue) {
            if (stmt->kind == AK_Break && stmt->a != U32_MAX) {
                u32 ignored = sema_no_type();
                if (!sema_infer_node_type(
                        lexer, ast, sema, stmt->a, sema_no_type(), &ignored)) {
                    return false;
                }
                sema->node_type_indices[i] = ignored;
            } else {
                sema->node_type_indices[i] = sema_builtin_type(sema, STK_Void);
            }
            continue;
        }

        if (stmt->kind != AK_Return) {
            continue;
        }

        if (stmt->a != U32_MAX &&
            sema_node_contains_interpolation(ast, stmt->a)) {
            u32 interp_index = sema_find_interpolated_string_node(ast, stmt->a);
            return error_0312_interpolated_string_escapes(
                lexer->source,
                sema_node_span(lexer,
                               &ast->nodes[interp_index == sema_no_decl()
                                               ? stmt->a
                                               : interp_index]));
        }

        if (stmt->a != U32_MAX) {
            if (!sema_infer_node_type(lexer,
                                      ast,
                                      sema,
                                      stmt->a,
                                      sema_no_type(),
                                      in_out_return_type)) {
                return false;
            }
            *in_out_return_type =
                sema_materialise_type(sema, *in_out_return_type);
        }
        sema->node_type_indices[i] = *in_out_return_type;
        *out_has_return            = true;
        return true;
    }

    return true;
}

internal bool sema_infer_expr_block_type(const Lexer* lexer,
                                         const Ast*   ast,
                                         Sema*        sema,
                                         u32          node_index,
                                         u32          expected_type,
                                         u32*         out_type_index)
{
    const AstNode* node  = &ast->nodes[node_index];
    const AstNode* block = &ast->nodes[node->a];
    ASSERT(node->kind == AK_ExprBlock, "Expected expression block");
    ASSERT(block->kind == AK_Block, "Expected expression block body");

    u32   result_type      = expected_type;
    u32   result_type_node = node_index;
    bool  has_value_break  = false;
    bool  definitely_exits = false;
    u32   void_type        = sema_builtin_type(sema, STK_Void);
    Arena temp_arena       = {0};
    arena_init(&temp_arena);

    for (u32 i = block->a; i < block->b; ++i) {
        const AstNode* stmt = &ast->nodes[i];
        if (!ast_node_is_block_statement(stmt)) {
            continue;
        }

        if (stmt->kind == AK_Break &&
            (stmt->b == U32_MAX || stmt->b == node->b)) {
            if (stmt->a == U32_MAX) {
                sema->node_type_indices[i] = void_type;
                result_type =
                    result_type == sema_no_type() ? void_type : result_type;
            } else {
                u32 break_expected =
                    result_type == sema_no_type() ? expected_type : result_type;
                u32 break_type = sema_no_type();
                if (!sema_infer_node_type(lexer,
                                          ast,
                                          sema,
                                          stmt->a,
                                          break_expected,
                                          &break_type)) {
                    arena_done(&temp_arena);
                    return false;
                }

                if (result_type == sema_no_type()) {
                    result_type      = break_type;
                    result_type_node = stmt->a;
                } else if (sema_type_is_concrete_integer(sema, result_type) &&
                           sema->types[break_type].kind == STK_UntypedInteger) {
                    break_type = result_type;
                } else if (sema_type_is_concrete_float(sema, result_type) &&
                           sema->types[break_type].kind == STK_UntypedFloat) {
                    break_type = result_type;
                } else if (result_type != break_type) {
                    bool ok = error_0320_on_branch_type_mismatch(
                        lexer->source,
                        sema_node_span(lexer, &ast->nodes[result_type_node]),
                        sema_type_name(lexer, sema, &temp_arena, result_type),
                        sema_node_span(lexer, &ast->nodes[stmt->a]),
                        sema_type_name(lexer, sema, &temp_arena, break_type));
                    arena_done(&temp_arena);
                    return ok;
                }

                sema->node_type_indices[i] = break_type;
                has_value_break            = true;
            }

            definitely_exits = true;
            break;
        }

        u32 ignored = sema_no_type();
        if (!sema_infer_node_type(
                lexer, ast, sema, i, sema_no_type(), &ignored)) {
            arena_done(&temp_arena);
            return false;
        }
        i = ast_block_statement_end_exclusive(ast, i) - 1;
    }

    if (!has_value_break) {
        result_type = result_type == sema_no_type() ? void_type : result_type;
    }

    if (result_type != void_type && !definitely_exits) {
        bool ok = error_0329_missing_expression_block_break(
            lexer->source,
            sema_node_span(lexer, node),
            sema_type_name(lexer, sema, &temp_arena, result_type));
        arena_done(&temp_arena);
        return ok;
    }

    arena_done(&temp_arena);
    *out_type_index = result_type;
    return true;
}

internal bool sema_node_contains_interpolation(const Ast* ast, u32 node_index)
{
    const AstNode* node = &ast->nodes[node_index];

    switch (node->kind) {
    case AK_InterpolatedString:
        return true;
    case AK_InterpPartExpr:
    case AK_Expression:
    case AK_Statement:
    case AK_IntegerNegate:
    case AK_LogicalNot:
    case AK_Cast:
        return sema_node_contains_interpolation(ast, node->a);
    case AK_Return:
    case AK_ReturnExpr:
        return node->a != U32_MAX &&
               sema_node_contains_interpolation(ast, node->a);
    case AK_StringConcat:
    case AK_On:
    case AK_IntegerPlus:
    case AK_IntegerMinus:
    case AK_IntegerMultiply:
    case AK_IntegerDivide:
    case AK_IntegerModulo:
    case AK_BitwiseAnd:
    case AK_BitwiseXor:
    case AK_BitwiseOr:
    case AK_Equal:
    case AK_NotEqual:
    case AK_Less:
    case AK_LessEqual:
    case AK_Greater:
    case AK_GreaterEqual:
    case AK_LogicalAnd:
    case AK_LogicalOr:
    case AK_RangeExclusive:
    case AK_RangeInclusive:
        if (node->kind == AK_On) {
            const AstOnInfo* on = &ast->ons[node->b];
            if (node->a != U32_MAX &&
                sema_node_contains_interpolation(ast, node->a)) {
                return true;
            }
            for (u32 i = 0; i < on->branch_count; ++i) {
                const AstOnBranch* branch =
                    &ast->on_branches[on->first_branch + i];
                if (!(branch->flags & AOBF_Else)) {
                    for (u32 pattern = 0; pattern < branch->pattern_count;
                         ++pattern) {
                        if (sema_pattern_contains_interpolation(
                                ast,
                                ast->pattern_items[branch->pattern_index +
                                                   pattern])) {
                            return true;
                        }
                    }
                }
                if (branch->guard_node_index != U32_MAX &&
                    sema_node_contains_interpolation(
                        ast, branch->guard_node_index)) {
                    return true;
                }
                if (sema_node_contains_interpolation(ast,
                                                     branch->expr_node_index)) {
                    return true;
                }
            }
            return false;
        }
        return sema_node_contains_interpolation(ast, node->a) ||
               sema_node_contains_interpolation(ast, node->b);
    case AK_Call:
        if (sema_node_contains_interpolation(ast, node->a)) {
            return true;
        }
        {
            const AstCallInfo* call = &ast->calls[node->b];
            for (u32 i = 0; i < call->arg_count; ++i) {
                if (sema_node_contains_interpolation(
                        ast, ast->call_args[call->first_arg + i])) {
                    return true;
                }
            }
            return false;
        }
    case AK_Tuple:
    case AK_Array:
        for (u32 i = 0; i < node->b; ++i) {
            if (sema_node_contains_interpolation(
                    ast, ast->tuple_items[node->a + i])) {
                return true;
            }
        }
        return false;
    case AK_TupleField:
    case AK_Index:
        if (node->kind == AK_Index &&
            sema_node_contains_interpolation(ast, node->b)) {
            return true;
        }
        return sema_node_contains_interpolation(ast, node->a);
    case AK_Plex:
    case AK_PlexUpdate:
        {
            const AstPlexLiteralInfo* literal = &ast->plex_literals[node->a];
            if (node->kind == AK_PlexUpdate &&
                sema_node_contains_interpolation(ast,
                                                 literal->target_node_index)) {
                return true;
            }
            for (u32 i = 0; i < literal->field_count; ++i) {
                if (sema_node_contains_interpolation(
                        ast,
                        ast->plex_literal_fields[literal->first_field + i]
                            .value_node_index)) {
                    return true;
                }
            }
            return false;
        }
    case AK_AddressOf:
    case AK_TypePointer:
        return sema_node_contains_interpolation(ast, node->a);
    case AK_Variable:
    case AK_Assign:
    case AK_DestructureBind:
    case AK_DestructureVariable:
    case AK_DestructureAssign:
    case AK_AnnotatedValue:
        return sema_node_contains_interpolation(ast, node->b);
    case AK_Break:
    case AK_BreakExpr:
        return node->a != U32_MAX &&
               sema_node_contains_interpolation(ast, node->a);
    case AK_ExprBlock:
        return sema_node_contains_interpolation(ast, node->a);
    case AK_For:
        {
            const AstForInfo* for_info = &ast->fors[node->a];
            for (u32 i = 0; i < for_info->init_count; ++i) {
                if (sema_node_contains_interpolation(
                        ast, ast->for_items[for_info->first_init + i])) {
                    return true;
                }
            }
            if (for_info->condition_node_index != U32_MAX &&
                sema_node_contains_interpolation(
                    ast, for_info->condition_node_index)) {
                return true;
            }
            for (u32 i = 0; i < for_info->update_count; ++i) {
                if (sema_node_contains_interpolation(
                        ast, ast->for_items[for_info->first_update + i])) {
                    return true;
                }
            }
            return sema_node_contains_interpolation(ast, node->b) ||
                   (for_info->else_block_index != U32_MAX &&
                    sema_node_contains_interpolation(
                        ast, for_info->else_block_index));
        }
    case AK_Block:
        for (u32 i = node->a; i < node->b; ++i) {
            if (sema_node_contains_interpolation(ast, i)) {
                return true;
            }
            i = ast_block_statement_end_exclusive(ast, i) - 1;
        }
        return false;
    case AK_TypeFn:
        return false;
    case AK_FnDef:
        {
            const AstNode* fn_start = &ast->nodes[node->a];
            for (u32 i = node->a + 1; i < fn_start->b; ++i) {
                if (sema_node_contains_interpolation(ast, i)) {
                    return true;
                }
                i = ast_block_statement_end_exclusive(ast, i) - 1;
            }
            return false;
        }
    default:
        return false;
    }
}

internal u32 sema_find_interpolated_string_node(const Ast* ast, u32 node_index)
{
    const AstNode* node = &ast->nodes[node_index];

    if (node->kind == AK_InterpolatedString) {
        return node_index;
    }

    switch (node->kind) {
    case AK_InterpPartExpr:
    case AK_Expression:
    case AK_Statement:
    case AK_IntegerNegate:
    case AK_LogicalNot:
    case AK_Cast:
        return sema_find_interpolated_string_node(ast, node->a);
    case AK_Return:
    case AK_ReturnExpr:
        return node->a == U32_MAX
                   ? sema_no_decl()
                   : sema_find_interpolated_string_node(ast, node->a);
    case AK_StringConcat:
    case AK_On:
    case AK_IntegerPlus:
    case AK_IntegerMinus:
    case AK_IntegerMultiply:
    case AK_IntegerDivide:
    case AK_IntegerModulo:
    case AK_BitwiseAnd:
    case AK_BitwiseXor:
    case AK_BitwiseOr:
    case AK_Equal:
    case AK_NotEqual:
    case AK_Less:
    case AK_LessEqual:
    case AK_Greater:
    case AK_GreaterEqual:
    case AK_LogicalAnd:
    case AK_LogicalOr:
    case AK_RangeExclusive:
    case AK_RangeInclusive:
        if (node->kind == AK_On) {
            const AstOnInfo* on = &ast->ons[node->b];
            u32 found = sema_find_interpolated_string_node(ast, node->a);
            if (found != sema_no_decl()) {
                return found;
            }
            for (u32 i = 0; i < on->branch_count; ++i) {
                const AstOnBranch* branch =
                    &ast->on_branches[on->first_branch + i];
                if (!(branch->flags & AOBF_Else)) {
                    for (u32 pattern = 0; pattern < branch->pattern_count;
                         ++pattern) {
                        found = sema_find_interpolated_string_pattern(
                            ast,
                            ast->pattern_items[branch->pattern_index +
                                               pattern]);
                        if (found != sema_no_decl()) {
                            return found;
                        }
                    }
                }
                if (branch->guard_node_index != U32_MAX) {
                    found = sema_find_interpolated_string_node(
                        ast, branch->guard_node_index);
                    if (found != sema_no_decl()) {
                        return found;
                    }
                }
                found = sema_find_interpolated_string_node(
                    ast, branch->expr_node_index);
                if (found != sema_no_decl()) {
                    return found;
                }
            }
            return sema_no_decl();
        }
        {
            u32 left = sema_find_interpolated_string_node(ast, node->a);
            return left != sema_no_decl()
                       ? left
                       : sema_find_interpolated_string_node(ast, node->b);
        }
    case AK_Call:
        {
            u32 found = sema_find_interpolated_string_node(ast, node->a);
            if (found != sema_no_decl()) {
                return found;
            }

            const AstCallInfo* call = &ast->calls[node->b];
            for (u32 i = 0; i < call->arg_count; ++i) {
                found = sema_find_interpolated_string_node(
                    ast, ast->call_args[call->first_arg + i]);
                if (found != sema_no_decl()) {
                    return found;
                }
            }
            return sema_no_decl();
        }
    case AK_Tuple:
    case AK_Array:
        for (u32 i = 0; i < node->b; ++i) {
            u32 found = sema_find_interpolated_string_node(
                ast, ast->tuple_items[node->a + i]);
            if (found != sema_no_decl()) {
                return found;
            }
        }
        return sema_no_decl();
    case AK_TupleField:
    case AK_Index:
        if (node->kind == AK_Index) {
            u32 found = sema_find_interpolated_string_node(ast, node->b);
            if (found != sema_no_decl()) {
                return found;
            }
        }
        return sema_find_interpolated_string_node(ast, node->a);
    case AK_Plex:
    case AK_PlexUpdate:
        {
            const AstPlexLiteralInfo* literal = &ast->plex_literals[node->a];
            if (node->kind == AK_PlexUpdate) {
                u32 found = sema_find_interpolated_string_node(
                    ast, literal->target_node_index);
                if (found != sema_no_decl()) {
                    return found;
                }
            }
            for (u32 i = 0; i < literal->field_count; ++i) {
                u32 found = sema_find_interpolated_string_node(
                    ast,
                    ast->plex_literal_fields[literal->first_field + i]
                        .value_node_index);
                if (found != sema_no_decl()) {
                    return found;
                }
            }
            return sema_no_decl();
        }
    case AK_AddressOf:
    case AK_TypePointer:
        return sema_find_interpolated_string_node(ast, node->a);
    case AK_Block:
        for (u32 i = node->a; i < node->b; ++i) {
            u32 found = sema_find_interpolated_string_node(ast, i);
            if (found != sema_no_decl()) {
                return found;
            }
            i = ast_block_statement_end_exclusive(ast, i) - 1;
        }
        return sema_no_decl();
    case AK_For:
        {
            const AstForInfo* for_info = &ast->fors[node->a];
            for (u32 i = 0; i < for_info->init_count; ++i) {
                u32 found = sema_find_interpolated_string_node(
                    ast, ast->for_items[for_info->first_init + i]);
                if (found != sema_no_decl()) {
                    return found;
                }
            }
            if (for_info->condition_node_index != U32_MAX) {
                u32 found = sema_find_interpolated_string_node(
                    ast, for_info->condition_node_index);
                if (found != sema_no_decl()) {
                    return found;
                }
            }
            for (u32 i = 0; i < for_info->update_count; ++i) {
                u32 found = sema_find_interpolated_string_node(
                    ast, ast->for_items[for_info->first_update + i]);
                if (found != sema_no_decl()) {
                    return found;
                }
            }
            {
                u32 found = sema_find_interpolated_string_node(ast, node->b);
                if (found != sema_no_decl()) {
                    return found;
                }
            }
            if (for_info->else_block_index != U32_MAX) {
                return sema_find_interpolated_string_node(
                    ast, for_info->else_block_index);
            }
            return sema_no_decl();
        }
    default:
        return sema_no_decl();
    }
}

internal bool sema_pattern_contains_interpolation(const Ast* ast,
                                                  u32        pattern_index)
{
    const AstPattern* pattern = &ast->patterns[pattern_index];
    switch (pattern->kind) {
    case APK_Value:
    case APK_Equal:
    case APK_NotEqual:
    case APK_Less:
    case APK_LessEqual:
    case APK_Greater:
    case APK_GreaterEqual:
        return sema_node_contains_interpolation(ast, pattern->a);
    case APK_RangeExclusive:
    case APK_RangeInclusive:
        return sema_node_contains_interpolation(ast, pattern->a) ||
               sema_node_contains_interpolation(ast, pattern->b);
    case APK_Bind:
        return pattern->b != U32_MAX &&
               sema_pattern_contains_interpolation(ast, pattern->b);
    case APK_Tuple:
        for (u32 i = 0; i < pattern->b; ++i) {
            if (sema_pattern_contains_interpolation(
                    ast, ast->pattern_items[pattern->a + i])) {
                return true;
            }
        }
        return false;
    case APK_Plex:
        for (u32 i = 0; i < pattern->b; ++i) {
            if (sema_pattern_contains_interpolation(
                    ast, ast->pattern_fields[pattern->a + i].pattern_index)) {
                return true;
            }
        }
        return false;
    case APK_EnumVariant:
        {
            const AstEnumPattern* enum_pattern =
                &ast->enum_patterns[pattern->a];
            for (u32 i = 0; i < enum_pattern->pattern_count; ++i) {
                if (sema_pattern_contains_interpolation(
                        ast,
                        ast->pattern_items[enum_pattern->first_pattern + i])) {
                    return true;
                }
            }
            return false;
        }
    case APK_Ignore:
        return false;
    }
    return false;
}

internal u32 sema_find_interpolated_string_pattern(const Ast* ast,
                                                   u32        pattern_index)
{
    const AstPattern* pattern = &ast->patterns[pattern_index];
    switch (pattern->kind) {
    case APK_Value:
    case APK_Equal:
    case APK_NotEqual:
    case APK_Less:
    case APK_LessEqual:
    case APK_Greater:
    case APK_GreaterEqual:
        return sema_find_interpolated_string_node(ast, pattern->a);
    case APK_RangeExclusive:
    case APK_RangeInclusive:
        {
            u32 found = sema_find_interpolated_string_node(ast, pattern->a);
            return found != sema_no_decl()
                       ? found
                       : sema_find_interpolated_string_node(ast, pattern->b);
        }
    case APK_Bind:
        return pattern->b == U32_MAX
                   ? sema_no_decl()
                   : sema_find_interpolated_string_pattern(ast, pattern->b);
    case APK_Tuple:
        for (u32 i = 0; i < pattern->b; ++i) {
            u32 found = sema_find_interpolated_string_pattern(
                ast, ast->pattern_items[pattern->a + i]);
            if (found != sema_no_decl()) {
                return found;
            }
        }
        return sema_no_decl();
    case APK_Plex:
        for (u32 i = 0; i < pattern->b; ++i) {
            u32 found = sema_find_interpolated_string_pattern(
                ast, ast->pattern_fields[pattern->a + i].pattern_index);
            if (found != sema_no_decl()) {
                return found;
            }
        }
        return sema_no_decl();
    case APK_EnumVariant:
        {
            const AstEnumPattern* enum_pattern =
                &ast->enum_patterns[pattern->a];
            for (u32 i = 0; i < enum_pattern->pattern_count; ++i) {
                u32 found = sema_find_interpolated_string_pattern(
                    ast, ast->pattern_items[enum_pattern->first_pattern + i]);
                if (found != sema_no_decl()) {
                    return found;
                }
            }
            return sema_no_decl();
        }
    case APK_Ignore:
        return sema_no_decl();
    }
    return sema_no_decl();
}

internal bool sema_validate_interpolated_strings(const Lexer* lexer,
                                                 const Ast*   ast,
                                                 Sema*        sema,
                                                 u32          node_index)
{
    const AstNode* node = &ast->nodes[node_index];

    if (node->kind == AK_InterpolatedString) {
        u32 ignored = sema_no_type();
        return sema_infer_node_type(
            lexer, ast, sema, node_index, sema_no_type(), &ignored);
    }

    switch (node->kind) {
    case AK_InterpPartExpr:
    case AK_Expression:
    case AK_Statement:
    case AK_IntegerNegate:
    case AK_LogicalNot:
    case AK_Cast:
        return sema_validate_interpolated_strings(lexer, ast, sema, node->a);
    case AK_Return:
    case AK_ReturnExpr:
        return node->a == U32_MAX ||
               sema_validate_interpolated_strings(lexer, ast, sema, node->a);
    case AK_StringConcat:
    case AK_On:
    case AK_IntegerPlus:
    case AK_IntegerMinus:
    case AK_IntegerMultiply:
    case AK_IntegerDivide:
    case AK_IntegerModulo:
    case AK_BitwiseAnd:
    case AK_BitwiseXor:
    case AK_BitwiseOr:
    case AK_Equal:
    case AK_NotEqual:
    case AK_Less:
    case AK_LessEqual:
    case AK_Greater:
    case AK_GreaterEqual:
    case AK_LogicalAnd:
    case AK_LogicalOr:
    case AK_RangeExclusive:
    case AK_RangeInclusive:
        if (node->kind == AK_On) {
            const AstOnInfo* on = &ast->ons[node->b];
            if (!sema_validate_interpolated_strings(
                    lexer, ast, sema, node->a)) {
                return false;
            }
            for (u32 i = 0; i < on->branch_count; ++i) {
                const AstOnBranch* branch =
                    &ast->on_branches[on->first_branch + i];
                if (!(branch->flags & AOBF_Else)) {
                    for (u32 pattern = 0; pattern < branch->pattern_count;
                         ++pattern) {
                        if (!sema_validate_interpolated_string_pattern(
                                lexer,
                                ast,
                                sema,
                                ast->pattern_items[branch->pattern_index +
                                                   pattern])) {
                            return false;
                        }
                    }
                }
                if (branch->guard_node_index != U32_MAX &&
                    !sema_validate_interpolated_strings(
                        lexer, ast, sema, branch->guard_node_index)) {
                    return false;
                }
                if (!sema_validate_interpolated_strings(
                        lexer, ast, sema, branch->expr_node_index)) {
                    return false;
                }
            }
            return true;
        }
        return sema_validate_interpolated_strings(lexer, ast, sema, node->a) &&
               sema_validate_interpolated_strings(lexer, ast, sema, node->b);
    case AK_Call:
        if (!sema_validate_interpolated_strings(lexer, ast, sema, node->a)) {
            return false;
        }
        {
            const AstCallInfo* call = &ast->calls[node->b];
            for (u32 i = 0; i < call->arg_count; ++i) {
                if (!sema_validate_interpolated_strings(
                        lexer,
                        ast,
                        sema,
                        ast->call_args[call->first_arg + i])) {
                    return false;
                }
            }
            return true;
        }
    case AK_Tuple:
    case AK_Array:
        for (u32 i = 0; i < node->b; ++i) {
            if (!sema_validate_interpolated_strings(
                    lexer, ast, sema, ast->tuple_items[node->a + i])) {
                return false;
            }
        }
        return true;
    case AK_TupleField:
    case AK_Index:
        if (node->kind == AK_Index &&
            !sema_validate_interpolated_strings(lexer, ast, sema, node->b)) {
            return false;
        }
        return sema_validate_interpolated_strings(lexer, ast, sema, node->a);
    case AK_Plex:
    case AK_PlexUpdate:
        {
            const AstPlexLiteralInfo* literal = &ast->plex_literals[node->a];
            if (node->kind == AK_PlexUpdate &&
                !sema_validate_interpolated_strings(
                    lexer, ast, sema, literal->target_node_index)) {
                return false;
            }
            for (u32 i = 0; i < literal->field_count; ++i) {
                if (!sema_validate_interpolated_strings(
                        lexer,
                        ast,
                        sema,
                        ast->plex_literal_fields[literal->first_field + i]
                            .value_node_index)) {
                    return false;
                }
            }
            return true;
        }
    case AK_AddressOf:
    case AK_TypePointer:
        return sema_validate_interpolated_strings(lexer, ast, sema, node->a);
    case AK_Block:
        for (u32 i = node->a; i < node->b; ++i) {
            if (!sema_validate_interpolated_strings(lexer, ast, sema, i)) {
                return false;
            }
            i = ast_block_statement_end_exclusive(ast, i) - 1;
        }
        return true;
    case AK_ExprBlock:
        return sema_validate_interpolated_strings(lexer, ast, sema, node->a);
    case AK_Break:
    case AK_BreakExpr:
        return node->a == U32_MAX ||
               sema_validate_interpolated_strings(lexer, ast, sema, node->a);
    case AK_DestructureBind:
    case AK_DestructureVariable:
    case AK_DestructureAssign:
        return sema_validate_interpolated_strings(lexer, ast, sema, node->b);
    case AK_For:
        {
            const AstForInfo* for_info = &ast->fors[node->a];
            for (u32 i = 0; i < for_info->init_count; ++i) {
                if (!sema_validate_interpolated_strings(
                        lexer,
                        ast,
                        sema,
                        ast->for_items[for_info->first_init + i])) {
                    return false;
                }
            }
            if (for_info->condition_node_index != U32_MAX &&
                !sema_validate_interpolated_strings(
                    lexer, ast, sema, for_info->condition_node_index)) {
                return false;
            }
            for (u32 i = 0; i < for_info->update_count; ++i) {
                if (!sema_validate_interpolated_strings(
                        lexer,
                        ast,
                        sema,
                        ast->for_items[for_info->first_update + i])) {
                    return false;
                }
            }
            if (!sema_validate_interpolated_strings(
                    lexer, ast, sema, node->b)) {
                return false;
            }
            return for_info->else_block_index == U32_MAX ||
                   sema_validate_interpolated_strings(
                       lexer, ast, sema, for_info->else_block_index);
        }
    default:
        return true;
    }
}

internal bool sema_validate_interpolated_string_pattern(const Lexer* lexer,
                                                        const Ast*   ast,
                                                        Sema*        sema,
                                                        u32 pattern_index)
{
    const AstPattern* pattern = &ast->patterns[pattern_index];
    switch (pattern->kind) {
    case APK_Value:
    case APK_Equal:
    case APK_NotEqual:
    case APK_Less:
    case APK_LessEqual:
    case APK_Greater:
    case APK_GreaterEqual:
        return sema_validate_interpolated_strings(lexer, ast, sema, pattern->a);
    case APK_RangeExclusive:
    case APK_RangeInclusive:
        return sema_validate_interpolated_strings(
                   lexer, ast, sema, pattern->a) &&
               sema_validate_interpolated_strings(lexer, ast, sema, pattern->b);
    case APK_Bind:
        return pattern->b == U32_MAX ||
               sema_validate_interpolated_string_pattern(
                   lexer, ast, sema, pattern->b);
    case APK_Tuple:
        for (u32 i = 0; i < pattern->b; ++i) {
            if (!sema_validate_interpolated_string_pattern(
                    lexer, ast, sema, ast->pattern_items[pattern->a + i])) {
                return false;
            }
        }
        return true;
    case APK_Plex:
        for (u32 i = 0; i < pattern->b; ++i) {
            if (!sema_validate_interpolated_string_pattern(
                    lexer,
                    ast,
                    sema,
                    ast->pattern_fields[pattern->a + i].pattern_index)) {
                return false;
            }
        }
        return true;
    case APK_EnumVariant:
        {
            const AstEnumPattern* enum_pattern =
                &ast->enum_patterns[pattern->a];
            for (u32 i = 0; i < enum_pattern->pattern_count; ++i) {
                if (!sema_validate_interpolated_string_pattern(
                        lexer,
                        ast,
                        sema,
                        ast->pattern_items[enum_pattern->first_pattern + i])) {
                    return false;
                }
            }
            return true;
        }
    case APK_Ignore:
        return true;
    }
    return true;
}

internal bool sema_assign_destructure_pattern_type(const Lexer* lexer,
                                                   const Ast*   ast,
                                                   Sema*        sema,
                                                   u32          pattern_index,
                                                   u32          value_type)
{
    const AstPattern* pattern = &ast->patterns[pattern_index];
    u32 symbol = sema_destructure_binder_symbol(ast, pattern_index);
    if (symbol != U32_MAX) {
        u32 local_index = sema->pattern_local_indices[pattern_index];
        ASSERT(local_index != sema_no_local(),
               "Expected destructuring pattern local");
        sema->locals[local_index].type_index = value_type;
        if (pattern->kind == APK_Bind && pattern->b != U32_MAX) {
            return sema_assign_destructure_pattern_type(
                lexer, ast, sema, pattern->b, value_type);
        }
        return true;
    }

    switch (pattern->kind) {
    case APK_Ignore:
        return true;
    case APK_Tuple:
        {
            if (value_type == sema_no_type() ||
                sema->types[value_type].kind != STK_Tuple) {
                return error_0304_type_mismatch(
                    lexer->source,
                    sema_pattern_span(lexer, pattern),
                    s("tuple"),
                    sema_type_name(lexer, sema, &temp_arena, value_type));
            }
            const SemaType* tuple = &sema->types[value_type];
            if (tuple->param_count != pattern->b) {
                return error_0304_type_mismatch(
                    lexer->source,
                    sema_pattern_span(lexer, pattern),
                    s("tuple with matching arity"),
                    sema_type_name(lexer, sema, &temp_arena, value_type));
            }
            for (u32 i = 0; i < pattern->b; ++i) {
                if (!sema_assign_destructure_pattern_type(
                        lexer,
                        ast,
                        sema,
                        ast->pattern_items[pattern->a + i],
                        sema->type_param_types[tuple->first_param_type + i])) {
                    return false;
                }
            }
            return true;
        }
    case APK_Plex:
        {
            if (value_type == sema_no_type() ||
                sema->types[value_type].kind != STK_Plex) {
                return error_0304_type_mismatch(
                    lexer->source,
                    sema_pattern_span(lexer, pattern),
                    s("plex"),
                    sema_type_name(lexer, sema, &temp_arena, value_type));
            }
            const SemaType* plex = &sema->types[value_type];
            for (u32 i = 0; i < pattern->b; ++i) {
                const AstPlexPatternField* field =
                    &ast->pattern_fields[pattern->a + i];
                u32 field_type = sema_no_type();
                for (u32 j = 0; j < plex->param_count; ++j) {
                    if (sema->type_param_symbols[plex->first_param_type + j] ==
                        field->symbol_handle) {
                        field_type =
                            sema->type_param_types[plex->first_param_type + j];
                        break;
                    }
                }
                if (field_type == sema_no_type()) {
                    return error_0304_type_mismatch(
                        lexer->source,
                        sema_pattern_span(lexer, pattern),
                        s("known plex field"),
                        lex_symbol(lexer, field->symbol_handle));
                }
                if (!sema_assign_destructure_pattern_type(
                        lexer, ast, sema, field->pattern_index, field_type)) {
                    return false;
                }
            }
            return true;
        }
    default:
        return error_0304_type_mismatch(lexer->source,
                                        sema_pattern_span(lexer, pattern),
                                        s("destructuring binder"),
                                        s("value pattern"));
    }
}

internal bool sema_check_destructure_assign_pattern_type(const Lexer* lexer,
                                                         const Ast*   ast,
                                                         Sema*        sema,
                                                         u32 pattern_index,
                                                         u32 value_type)
{
    const AstPattern* pattern = &ast->patterns[pattern_index];
    u32 symbol = sema_destructure_binder_symbol(ast, pattern_index);
    if (symbol != U32_MAX) {
        u32 local_index = sema->pattern_local_indices[pattern_index];
        ASSERT(local_index != sema_no_local(),
               "Expected destructuring assignment target");
        u32 target_type = sema->locals[local_index].type_index;
        if (!sema_type_matches(sema, target_type, value_type)) {
            return error_0304_type_mismatch(
                lexer->source,
                sema_pattern_span(lexer, pattern),
                sema_type_name(lexer, sema, &temp_arena, target_type),
                sema_type_name(lexer, sema, &temp_arena, value_type));
        }
        if (pattern->kind == APK_Bind && pattern->b != U32_MAX) {
            return sema_check_destructure_assign_pattern_type(
                lexer, ast, sema, pattern->b, value_type);
        }
        return true;
    }

    switch (pattern->kind) {
    case APK_Ignore:
        return true;
    case APK_Tuple:
        {
            if (value_type == sema_no_type() ||
                sema->types[value_type].kind != STK_Tuple) {
                return error_0304_type_mismatch(
                    lexer->source,
                    sema_pattern_span(lexer, pattern),
                    s("tuple"),
                    sema_type_name(lexer, sema, &temp_arena, value_type));
            }
            const SemaType* tuple = &sema->types[value_type];
            if (tuple->param_count != pattern->b) {
                return error_0304_type_mismatch(
                    lexer->source,
                    sema_pattern_span(lexer, pattern),
                    s("tuple with matching arity"),
                    sema_type_name(lexer, sema, &temp_arena, value_type));
            }
            for (u32 i = 0; i < pattern->b; ++i) {
                if (!sema_check_destructure_assign_pattern_type(
                        lexer,
                        ast,
                        sema,
                        ast->pattern_items[pattern->a + i],
                        sema->type_param_types[tuple->first_param_type + i])) {
                    return false;
                }
            }
            return true;
        }
    case APK_Plex:
        {
            if (value_type == sema_no_type() ||
                sema->types[value_type].kind != STK_Plex) {
                return error_0304_type_mismatch(
                    lexer->source,
                    sema_pattern_span(lexer, pattern),
                    s("plex"),
                    sema_type_name(lexer, sema, &temp_arena, value_type));
            }
            const SemaType* plex = &sema->types[value_type];
            for (u32 i = 0; i < pattern->b; ++i) {
                const AstPlexPatternField* field =
                    &ast->pattern_fields[pattern->a + i];
                u32 field_type = sema_no_type();
                for (u32 j = 0; j < plex->param_count; ++j) {
                    if (sema->type_param_symbols[plex->first_param_type + j] ==
                        field->symbol_handle) {
                        field_type =
                            sema->type_param_types[plex->first_param_type + j];
                        break;
                    }
                }
                if (field_type == sema_no_type()) {
                    return error_0304_type_mismatch(
                        lexer->source,
                        sema_pattern_span(lexer, pattern),
                        s("known plex field"),
                        lex_symbol(lexer, field->symbol_handle));
                }
                if (!sema_check_destructure_assign_pattern_type(
                        lexer, ast, sema, field->pattern_index, field_type)) {
                    return false;
                }
            }
            return true;
        }
    default:
        return error_0305_invalid_assignment_target(
            lexer->source, sema_pattern_span(lexer, pattern), s("pattern"));
    }
}

//------------------------------------------------------------------------------
// Infer one AST node type, optionally using an expected context type.

internal bool sema_infer_node_type(const Lexer* lexer,
                                   const Ast*   ast,
                                   Sema*        sema,
                                   u32          node_index,
                                   u32          expected_type,
                                   u32*         out_type_index)
{
    const AstNode* node       = &ast->nodes[node_index];
    u32            type_index = sema_no_type();

    switch (node->kind) {
    case AK_IntegerLiteral:
        if (sema_integer_literal_is_packed(lexer, node)) {
            type_index =
                sema_type_is_concrete_integer(sema, expected_type)
                    ? expected_type
                    : sema_packed_integer_literal_type(lexer, node, sema);
        } else {
            type_index = sema_type_is_concrete_integer(sema, expected_type)
                             ? expected_type
                             : sema_builtin_type(sema, STK_UntypedInteger);
        }
        break;

    case AK_FloatLiteral:
        type_index = sema_type_is_concrete_float(sema, expected_type)
                         ? expected_type
                         : sema_builtin_type(sema, STK_UntypedFloat);
        break;

    case AK_BoolLiteral:
        type_index = sema_builtin_type(sema, STK_Bool);
        break;

    case AK_StringLiteral:
    case AK_StringConcat:
        type_index = sema_builtin_type(sema, STK_String);
        break;

    case AK_EnumVariant:
        if (expected_type == sema_no_type() ||
            sema->types[expected_type].kind != STK_Enum) {
            return error_0304_type_mismatch(
                lexer->source,
                sema_node_span(lexer, node),
                s("enum context"),
                expected_type == sema_no_type()
                    ? s("<unknown>")
                    : sema_type_name(lexer, sema, &temp_arena, expected_type));
        }
        if (sema_enum_variant_index(sema, expected_type, node->a) == U32_MAX) {
            return error_0304_type_mismatch(lexer->source,
                                            sema_node_span(lexer, node),
                                            s("known enum variant"),
                                            lex_symbol(lexer, node->a));
        }
        type_index = expected_type;
        break;

    case AK_InterpPartExpr:
        if (!sema_infer_node_type(
                lexer, ast, sema, node->a, expected_type, &type_index)) {
            return false;
        }
        break;

    case AK_AddressOf:
        {
            u32 pointee_type = sema_no_type();
            if (!sema_infer_node_type(
                    lexer, ast, sema, node->a, sema_no_type(), &pointee_type)) {
                return false;
            }
            if (!sema_node_is_addressable(ast, sema, node->a)) {
                return error_0304_type_mismatch(
                    lexer->source,
                    sema_node_span(lexer, &ast->nodes[node->a]),
                    s("addressable value"),
                    sema_type_name(lexer, sema, &temp_arena, pointee_type));
            }
            type_index = sema_add_pointer_type(sema, pointee_type);
        }
        break;

    case AK_InterpolatedString:
        for (u32 i = node->a; i < node->b; ++i) {
            const AstNode* part      = &ast->nodes[i];
            u32            part_type = sema_no_type();

            if (part->kind == AK_StringLiteral) {
                continue;
            }
            ASSERT(part->kind == AK_InterpPartExpr,
                   "Expected interpolated string part wrapper");
            if (!sema_infer_node_type(
                    lexer, ast, sema, part->a, sema_no_type(), &part_type)) {
                return false;
            }
            part_type                  = sema_materialise_type(sema, part_type);
            sema->node_type_indices[i] = part_type;
            if (!sema_type_is_interpolatable(sema, part_type)) {
                return error_0311_invalid_interpolation_type(
                    lexer->source,
                    sema_node_span(lexer, &ast->nodes[part->a]),
                    sema_type_name(lexer, sema, &temp_arena, part_type));
            }
        }
        type_index = sema_builtin_type(sema, STK_String);
        break;

    case AK_Tuple:
        {
            Array(u32) item_types = NULL;
            const SemaType* expected_tuple =
                expected_type != sema_no_type() &&
                        sema->types[expected_type].kind == STK_Tuple
                    ? &sema->types[expected_type]
                    : NULL;
            if (expected_tuple != NULL &&
                expected_tuple->param_count != node->b) {
                array_free(item_types);
                return error_0304_type_mismatch(
                    lexer->source,
                    sema_node_span(lexer, node),
                    sema_type_name(lexer, sema, &temp_arena, expected_type),
                    s("tuple with different arity"));
            }
            for (u32 i = 0; i < node->b; ++i) {
                u32 item_node = ast->tuple_items[node->a + i];
                u32 expected_item =
                    expected_tuple == NULL
                        ? sema_no_type()
                        : sema->type_param_types
                              [expected_tuple->first_param_type + i];
                u32 item_type = sema_no_type();
                if (!sema_infer_node_type(lexer,
                                          ast,
                                          sema,
                                          item_node,
                                          expected_item,
                                          &item_type)) {
                    array_free(item_types);
                    return false;
                }
                item_type = expected_item != sema_no_type()
                                ? expected_item
                                : sema_materialise_type(sema, item_type);
                array_push(item_types, item_type);
            }
            type_index = sema_add_tuple_type(sema, item_types);
            array_free(item_types);
        }
        break;

    case AK_TupleField:
        {
            u32 tuple_type = sema_no_type();
            if (!sema_infer_node_type(
                    lexer, ast, sema, node->a, sema_no_type(), &tuple_type)) {
                return false;
            }
            if (tuple_type == sema_no_type() ||
                sema->types[tuple_type].kind != STK_Tuple) {
                return error_0304_type_mismatch(
                    lexer->source,
                    sema_node_span(lexer, node),
                    s("tuple"),
                    sema_type_name(lexer, sema, &temp_arena, tuple_type));
            }
            const SemaType* tuple = &sema->types[tuple_type];
            if (node->b >= tuple->param_count) {
                return error_0304_type_mismatch(
                    lexer->source,
                    sema_node_span(lexer, node),
                    s("valid tuple field"),
                    sema_type_name(lexer, sema, &temp_arena, tuple_type));
            }
            type_index =
                sema->type_param_types[tuple->first_param_type + node->b];
        }
        break;

    case AK_Plex:
    case AK_PlexUpdate:
        {
            const AstPlexLiteralInfo* literal = &ast->plex_literals[node->a];
            u32                       target_type = sema_no_type();
            if (node->kind == AK_Plex) {
                if (!sema_resolve_type_node(lexer,
                                            ast,
                                            sema,
                                            literal->target_node_index,
                                            &target_type)) {
                    return false;
                }
            } else {
                if (!sema_infer_node_type(lexer,
                                          ast,
                                          sema,
                                          literal->target_node_index,
                                          sema_no_type(),
                                          &target_type)) {
                    return false;
                }
            }
            bool target_is_union = target_type != sema_no_type() &&
                                   sema->types[target_type].kind == STK_Union;
            bool target_is_plex = target_type != sema_no_type() &&
                                  sema->types[target_type].kind == STK_Plex;
            if (!target_is_plex && !target_is_union) {
                return error_0304_type_mismatch(
                    lexer->source,
                    sema_node_span(lexer, node),
                    node->kind == AK_Plex ? s("plex or union type")
                                          : s("plex value"),
                    sema_type_name(lexer, sema, &temp_arena, target_type));
            }
            if (target_is_union && node->kind == AK_PlexUpdate) {
                return error_0304_type_mismatch(
                    lexer->source,
                    sema_node_span(lexer, node),
                    s("plex value"),
                    sema_type_name(lexer, sema, &temp_arena, target_type));
            }
            const SemaType* record = &sema->types[target_type];
            if (target_is_plex && node->kind == AK_Plex &&
                literal->field_count != record->param_count) {
                return error_0304_type_mismatch(lexer->source,
                                                sema_node_span(lexer, node),
                                                s("all plex fields"),
                                                s("different field count"));
            }
            if (target_is_union && literal->field_count != 1) {
                return error_0304_type_mismatch(lexer->source,
                                                sema_node_span(lexer, node),
                                                s("one union field"),
                                                s("different field count"));
            }
            bool* seen =
                arena_alloc(&temp_arena, sizeof(bool) * record->param_count);
            memset(seen, 0, sizeof(bool) * record->param_count);
            for (u32 i = 0; i < literal->field_count; ++i) {
                const AstPlexLiteralField* field =
                    &ast->plex_literal_fields[literal->first_field + i];
                u32 field_index = U32_MAX;
                for (u32 j = 0; j < record->param_count; ++j) {
                    if (sema->type_param_symbols[record->first_param_type +
                                                 j] == field->symbol_handle) {
                        field_index = j;
                        break;
                    }
                }
                if (field_index == U32_MAX) {
                    return error_0304_type_mismatch(
                        lexer->source,
                        sema_node_span(lexer,
                                       &ast->nodes[field->value_node_index]),
                        target_is_union ? s("known union field")
                                        : s("known plex field"),
                        lex_symbol(lexer, field->symbol_handle));
                }
                if (seen[field_index]) {
                    return error_0304_type_mismatch(
                        lexer->source,
                        sema_node_span(lexer,
                                       &ast->nodes[field->value_node_index]),
                        target_is_union ? s("unique union field")
                                        : s("unique plex field"),
                        lex_symbol(lexer, field->symbol_handle));
                }
                seen[field_index] = true;
                u32 expected_field =
                    sema->type_param_types[record->first_param_type +
                                           field_index];
                u32 actual_field = sema_no_type();
                if (!sema_infer_node_type(lexer,
                                          ast,
                                          sema,
                                          field->value_node_index,
                                          expected_field,
                                          &actual_field)) {
                    return false;
                }
            }
            for (u32 i = 0; i < record->param_count; ++i) {
                if (target_is_plex && node->kind == AK_Plex && !seen[i]) {
                    return error_0304_type_mismatch(lexer->source,
                                                    sema_node_span(lexer, node),
                                                    s("all plex fields"),
                                                    s("missing field"));
                }
            }
            type_index = target_type;
        }
        break;

    case AK_Field:
        {
            u32 qualified_type = sema_no_type();
            if (sema_try_resolve_type_symbol(
                    lexer, ast, sema, node->a, &qualified_type)) {
                if (sema->types[qualified_type].kind == STK_Enum) {
                    if (sema_enum_variant_index(
                            sema, qualified_type, node->b) == U32_MAX) {
                        return error_0304_type_mismatch(
                            lexer->source,
                            sema_node_span(lexer, node),
                            s("known enum variant"),
                            lex_symbol(lexer, node->b));
                    }
                    type_index = qualified_type;
                    break;
                }
            }

            u32 target_type = sema_no_type();
            if (!sema_infer_node_type(
                    lexer, ast, sema, node->a, sema_no_type(), &target_type)) {
                return false;
            }
            u32 field_target_type = target_type;
            if (target_type != sema_no_type() &&
                sema->types[target_type].kind == STK_Pointer) {
                u32 pointee_type = sema->types[target_type].first_param_type;
                if (sema->types[pointee_type].kind == STK_Plex ||
                    sema->types[pointee_type].kind == STK_Union) {
                    field_target_type = pointee_type;
                }
            }
            if (field_target_type != sema_no_type() &&
                (sema->types[field_target_type].kind == STK_Plex ||
                 sema->types[field_target_type].kind == STK_Union)) {
                const SemaType* record = &sema->types[field_target_type];
                for (u32 i = 0; i < record->param_count; ++i) {
                    if (sema->type_param_symbols[record->first_param_type +
                                                 i] == node->b) {
                        type_index =
                            sema->type_param_types[record->first_param_type +
                                                   i];
                        break;
                    }
                }
                if (type_index == sema_no_type()) {
                    return error_0304_type_mismatch(lexer->source,
                                                    sema_node_span(lexer, node),
                                                    record->kind == STK_Union
                                                        ? s("known union field")
                                                        : s("known plex field"),
                                                    lex_symbol(lexer, node->b));
                }
                break;
            }
            if (target_type != sema_no_type() &&
                sema->types[target_type].kind == STK_Module) {
                const SemaType* module = &sema->types[target_type];
                for (u32 i = 0; i < module->param_count; ++i) {
                    if (sema->type_param_symbols[module->first_param_type +
                                                 i] == node->b) {
                        type_index =
                            sema->type_param_types[module->first_param_type +
                                                   i];
                        u32          import_module_index = module->return_type;
                        u32          import_decl_index   = sema_no_decl();
                        SemaDeclKind import_decl_kind    = SK_Constant;
                        if (sema->program != NULL &&
                            import_module_index <
                                array_count(sema->program->modules)) {
                            const ModuleInfo* import_module =
                                &sema->program->modules[import_module_index];
                            if (i < array_count(
                                        import_module->export_decl_indices)) {
                                import_decl_index =
                                    import_module->export_decl_indices[i];
                                if (import_decl_index <
                                    array_count(
                                        import_module->front_end.sema.decls)) {
                                    import_decl_kind =
                                        import_module->front_end.sema
                                            .decls[import_decl_index]
                                            .kind;
                                }
                            }
                        }
                        sema->node_decl_indices[node - ast->nodes] =
                            sema_ensure_module_export_decl(sema,
                                                           node->b,
                                                           type_index,
                                                           import_decl_kind,
                                                           import_module_index,
                                                           import_decl_index);
                        break;
                    }
                }
                if (type_index == sema_no_type()) {
                    return error_0304_type_mismatch(lexer->source,
                                                    sema_node_span(lexer, node),
                                                    s("known module export"),
                                                    lex_symbol(lexer, node->b));
                }
                break;
            }
            if (target_type == sema_no_type() ||
                (sema->types[target_type].kind != STK_Slice &&
                 sema->types[target_type].kind != STK_String)) {
                return error_0304_type_mismatch(
                    lexer->source,
                    sema_node_span(lexer, node),
                    s("slice, string, module, plex, union, or pointer to "
                      "plex/union"),
                    sema_type_name(lexer, sema, &temp_arena, target_type));
            }
            string field = lex_symbol(lexer, node->b);
            if (string_eq(field, s("data"))) {
                u32 item_type = sema->types[target_type].kind == STK_String
                                    ? sema_builtin_type(sema, STK_U8)
                                    : sema->types[target_type].first_param_type;
                type_index    = sema_add_pointer_type(sema, item_type);
            } else if (string_eq(field, s("count"))) {
                type_index = sema_builtin_type(sema, STK_Usize);
            } else {
                string expected = sema->types[target_type].kind == STK_String
                                      ? s("string field `.data` or `.count`")
                                      : s("slice field `.data` or `.count`");
                return error_0304_type_mismatch(lexer->source,
                                                sema_node_span(lexer, node),
                                                expected,
                                                field);
            }
        }
        break;

    case AK_Array:
        {
            const SemaType* expected_array =
                expected_type != sema_no_type() &&
                        sema->types[expected_type].kind == STK_Array
                    ? &sema->types[expected_type]
                    : NULL;
            if (expected_array != NULL &&
                expected_array->return_type != node->b) {
                return error_0304_type_mismatch(
                    lexer->source,
                    sema_node_span(lexer, node),
                    sema_type_name(lexer, sema, &temp_arena, expected_type),
                    s("array with different length"));
            }

            u32 item_type = expected_array != NULL
                                ? expected_array->first_param_type
                                : sema_no_type();
            if (node->b == 0 && item_type == sema_no_type()) {
                return error_0304_type_mismatch(
                    lexer->source,
                    sema_node_span(lexer, node),
                    s("array with known element type"),
                    s("empty array literal"));
            }

            for (u32 i = 0; i < node->b; ++i) {
                u32 item_node     = ast->tuple_items[node->a + i];
                u32 expected_item = item_type;
                u32 actual_item   = sema_no_type();
                if (!sema_infer_node_type(lexer,
                                          ast,
                                          sema,
                                          item_node,
                                          expected_item,
                                          &actual_item)) {
                    return false;
                }
                actual_item = expected_item != sema_no_type()
                                  ? expected_item
                                  : sema_materialise_type(sema, actual_item);
                if (item_type == sema_no_type()) {
                    item_type = actual_item;
                } else if (item_type != actual_item) {
                    return error_0304_type_mismatch(
                        lexer->source,
                        sema_node_span(lexer, &ast->nodes[item_node]),
                        sema_type_name(lexer, sema, &temp_arena, item_type),
                        sema_type_name(lexer, sema, &temp_arena, actual_item));
                }
            }

            type_index = sema_add_array_type(sema, item_type, node->b);
        }
        break;

    case AK_Slice:
        {
            const AstSliceInfo* slice       = &ast->slices[node->a];
            u32                 target_type = sema_no_type();
            if (!sema_infer_node_type(lexer,
                                      ast,
                                      sema,
                                      slice->target_node_index,
                                      sema_no_type(),
                                      &target_type)) {
                return false;
            }
            if (target_type == sema_no_type() ||
                (sema->types[target_type].kind != STK_Array &&
                 sema->types[target_type].kind != STK_Slice &&
                 sema->types[target_type].kind != STK_String)) {
                return error_0304_type_mismatch(
                    lexer->source,
                    sema_node_span(lexer, node),
                    s("array, slice, or string"),
                    sema_type_name(lexer, sema, &temp_arena, target_type));
            }
            if (slice->start_node_index != U32_MAX) {
                u32 start_type = sema_no_type();
                if (!sema_infer_node_type(lexer,
                                          ast,
                                          sema,
                                          slice->start_node_index,
                                          sema_no_type(),
                                          &start_type)) {
                    return false;
                }
                start_type = sema_materialise_type(sema, start_type);
                if (!sema_type_is_integer(sema, start_type)) {
                    return error_0304_type_mismatch(
                        lexer->source,
                        sema_node_span(lexer,
                                       &ast->nodes[slice->start_node_index]),
                        s("integer slice bound"),
                        sema_type_name(lexer, sema, &temp_arena, start_type));
                }
            }
            if (slice->end_node_index != U32_MAX) {
                u32 end_type = sema_no_type();
                if (!sema_infer_node_type(lexer,
                                          ast,
                                          sema,
                                          slice->end_node_index,
                                          sema_no_type(),
                                          &end_type)) {
                    return false;
                }
                end_type = sema_materialise_type(sema, end_type);
                if (!sema_type_is_integer(sema, end_type)) {
                    return error_0304_type_mismatch(
                        lexer->source,
                        sema_node_span(lexer,
                                       &ast->nodes[slice->end_node_index]),
                        s("integer slice bound"),
                        sema_type_name(lexer, sema, &temp_arena, end_type));
                }
            }
            type_index =
                sema->types[target_type].kind == STK_String
                    ? sema_builtin_type(sema, STK_String)
                    : sema_add_slice_type(
                          sema, sema->types[target_type].first_param_type);
        }
        break;

    case AK_Index:
        {
            u32 array_type = sema_no_type();
            if (!sema_infer_node_type(
                    lexer, ast, sema, node->a, sema_no_type(), &array_type)) {
                return false;
            }
            if (array_type == sema_no_type() ||
                (sema->types[array_type].kind != STK_Array &&
                 sema->types[array_type].kind != STK_Slice &&
                 sema->types[array_type].kind != STK_Pointer)) {
                return error_0304_type_mismatch(
                    lexer->source,
                    sema_node_span(lexer, node),
                    s("array, slice, or pointer"),
                    sema_type_name(lexer, sema, &temp_arena, array_type));
            }

            u32 index_type = sema_no_type();
            if (!sema_infer_node_type(
                    lexer, ast, sema, node->b, sema_no_type(), &index_type)) {
                return false;
            }
            index_type = sema_materialise_type(sema, index_type);
            if (!sema_type_is_integer(sema, index_type)) {
                return error_0304_type_mismatch(
                    lexer->source,
                    sema_node_span(lexer, &ast->nodes[node->b]),
                    s("integer index"),
                    sema_type_name(lexer, sema, &temp_arena, index_type));
            }

            type_index = sema->types[array_type].first_param_type;
        }
        break;

    case AK_Expression:
        if (!sema_infer_node_type(
                lexer, ast, sema, node->a, expected_type, &type_index)) {
            return false;
        }
        break;

    case AK_Return:
    case AK_ReturnExpr:
        if (node->a == U32_MAX) {
            type_index = expected_type != sema_no_type()
                             ? expected_type
                             : sema_builtin_type(sema, STK_Void);
        } else if (!sema_infer_node_type(
                       lexer, ast, sema, node->a, expected_type, &type_index)) {
            return false;
        }
        break;

    case AK_Break:
    case AK_BreakExpr:
        if (node->a == U32_MAX) {
            type_index = sema_builtin_type(sema, STK_Void);
        } else if (!sema_infer_node_type(
                       lexer, ast, sema, node->a, expected_type, &type_index)) {
            return false;
        }
        break;

    case AK_Continue:
    case AK_ContinueExpr:
        type_index = sema_builtin_type(sema, STK_Void);
        break;

    case AK_ExprBlock:
        if (!sema_infer_expr_block_type(
                lexer, ast, sema, node_index, expected_type, &type_index)) {
            return false;
        }
        break;

    case AK_Statement:
        {
            u32            statement_expected = expected_type;
            const AstNode* expr               = &ast->nodes[node->a];
            u32            expr_root_index    = node->a;
            if (expr->kind == AK_Expression) {
                expr_root_index = expr->a;
                expr            = &ast->nodes[expr_root_index];
            }
            if (expr->kind == AK_On) {
                statement_expected = sema_builtin_type(sema, STK_Void);
            }
            if (!sema_infer_node_type(lexer,
                                      ast,
                                      sema,
                                      node->a,
                                      statement_expected,
                                      &type_index)) {
                return false;
            }
        }
        break;

    case AK_For:
        {
            const AstNode* body = &ast->nodes[node->b];
            ASSERT(body->kind == AK_Block, "Expected for body block");
            const AstForInfo* for_info = &ast->fors[node->a];
            for (u32 item = 0; item < for_info->init_count; ++item) {
                u32 item_node = ast->for_items[for_info->first_init + item];
                if (ast->nodes[item_node].kind == AK_Bind) {
                    continue;
                }
                u32 ignored = sema_no_type();
                if (!sema_infer_node_type(lexer,
                                          ast,
                                          sema,
                                          item_node,
                                          sema_no_type(),
                                          &ignored)) {
                    return false;
                }
            }
            if (for_info->condition_node_index != U32_MAX) {
                u32 condition_type = sema_no_type();
                if (!sema_infer_node_type(lexer,
                                          ast,
                                          sema,
                                          for_info->condition_node_index,
                                          sema_builtin_type(sema, STK_Bool),
                                          &condition_type)) {
                    return false;
                }
            }
            u32  ignored_type = sema_no_type();
            bool has_return   = false;
            if (!sema_infer_block_statements(lexer,
                                             ast,
                                             sema,
                                             body->a,
                                             body->b,
                                             &ignored_type,
                                             &has_return)) {
                return false;
            }
            const AstNode* else_block = NULL;
            if (for_info->else_block_index != U32_MAX) {
                else_block = &ast->nodes[for_info->else_block_index];
                ASSERT(else_block->kind == AK_Block, "Expected for else block");
                ignored_type = sema_no_type();
                has_return   = false;
                if (!sema_infer_block_statements(lexer,
                                                 ast,
                                                 sema,
                                                 else_block->a,
                                                 else_block->b,
                                                 &ignored_type,
                                                 &has_return)) {
                    return false;
                }
            }
            u32  loop_type            = sema_no_type();
            u32  loop_type_node       = node_index;
            bool body_has_value_break = false;
            bool else_has_value_break = false;
            for (u32 i = body->a; i < body->b; ++i) {
                const AstNode* stmt = &ast->nodes[i];
                if (!ast_node_is_block_statement(stmt)) {
                    continue;
                }
                if (!sema_collect_value_breaks_for_target(
                        lexer,
                        ast,
                        sema,
                        i,
                        for_info->label_symbol,
                        expected_type,
                        &loop_type,
                        &loop_type_node,
                        &body_has_value_break)) {
                    return false;
                }
                i = ast_block_statement_end_exclusive(ast, i) - 1;
            }
            if (else_block != NULL) {
                for (u32 i = else_block->a; i < else_block->b; ++i) {
                    const AstNode* stmt = &ast->nodes[i];
                    if (!ast_node_is_block_statement(stmt)) {
                        continue;
                    }
                    if (!sema_collect_value_breaks_for_target(
                            lexer,
                            ast,
                            sema,
                            i,
                            for_info->label_symbol,
                            expected_type,
                            &loop_type,
                            &loop_type_node,
                            &else_has_value_break)) {
                        return false;
                    }
                    i = ast_block_statement_end_exclusive(ast, i) - 1;
                }
            }
            for (u32 item = 0; item < for_info->update_count; ++item) {
                u32 item_node = ast->for_items[for_info->first_update + item];
                u32 ignored   = sema_no_type();
                if (!sema_infer_node_type(lexer,
                                          ast,
                                          sema,
                                          item_node,
                                          sema_no_type(),
                                          &ignored)) {
                    return false;
                }
            }
            if (else_block != NULL && !body_has_value_break) {
                return error_0333_invalid_loop_else(
                    lexer->source, sema_node_span(lexer, else_block));
            }
            if (body_has_value_break &&
                for_info->condition_node_index != U32_MAX &&
                else_block == NULL) {
                Arena temp_arena = {0};
                arena_init(&temp_arena);
                bool ok = error_0332_missing_loop_else(
                    lexer->source,
                    sema_node_span(lexer, node),
                    sema_type_name(lexer, sema, &temp_arena, loop_type));
                arena_done(&temp_arena);
                return ok;
            }
            if (body_has_value_break && else_block != NULL &&
                !else_has_value_break) {
                Arena temp_arena = {0};
                arena_init(&temp_arena);
                bool ok = error_0332_missing_loop_else(
                    lexer->source,
                    sema_node_span(lexer, else_block),
                    sema_type_name(lexer, sema, &temp_arena, loop_type));
                arena_done(&temp_arena);
                return ok;
            }
            type_index = body_has_value_break
                             ? loop_type
                             : sema_builtin_type(sema, STK_Void);
        }
        break;

    case AK_SymbolRef:
        {
            if (sema->node_is_type_expr[node_index]) {
                if (!sema_resolve_type_node(
                        lexer, ast, sema, node_index, &type_index)) {
                    return false;
                }
                break;
            }
            if (sema->node_local_indices[node_index] != sema_no_local()) {
                u32 local_index        = sema->node_local_indices[node_index];
                const SemaLocal* local = &sema->locals[local_index];
                if (sema_local_is_decl_binding(local) &&
                    local->type_index == sema_no_type()) {
                    if (!sema_infer_local_binding_type(
                            lexer, ast, sema, local_index, &type_index)) {
                        return false;
                    }
                } else {
                    type_index = local->type_index;
                }
            } else {
                u32 decl_index = sema->node_decl_indices[node_index];
                if (decl_index == sema_no_decl()) {
                    if (expected_type != sema_no_type() &&
                        sema->types[expected_type].kind == STK_Enum &&
                        sema_enum_variant_index(sema, expected_type, node->a) !=
                            U32_MAX) {
                        type_index = expected_type;
                        break;
                    }
                    if (expected_type != sema_no_type() &&
                        sema->types[expected_type].kind == STK_Enum) {
                        return error_0304_type_mismatch(
                            lexer->source,
                            sema_node_span(lexer, node),
                            s("known enum variant"),
                            lex_symbol(lexer, node->a));
                    }
                    return error_0300_unknown_symbol(
                        lexer->source,
                        sema_node_span(lexer, node),
                        lex_symbol(lexer, node->a));
                }
                type_index = sema->decls[decl_index].type_index;
            }
            if (sema_type_is_concrete_integer(sema, expected_type) &&
                type_index != sema_no_type() &&
                sema->types[type_index].kind == STK_UntypedInteger) {
                if (sema_type_is_unsigned_integer(sema, expected_type)) {
                    i64 value = 0;
                    if (sema_try_eval_integer_constant(
                            lexer, ast, sema, node_index, &value) &&
                        value < 0) {
                        return error_0323_negative_unsigned_inference(
                            lexer->source,
                            sema_node_span(lexer, node),
                            sema_type_name(
                                lexer, sema, &temp_arena, expected_type));
                    }
                }
                type_index = expected_type;
            } else if (sema_type_is_concrete_float(sema, expected_type) &&
                       type_index != sema_no_type() &&
                       sema->types[type_index].kind == STK_UntypedFloat) {
                type_index = expected_type;
            }
        }
        break;

    case AK_Bind:
        {
            u32 local_index = sema->node_local_indices[node_index];
            ASSERT(local_index != sema_no_local(),
                   "Expected resolved local bind");
            if (!sema_infer_local_binding_type(
                    lexer, ast, sema, local_index, &type_index)) {
                return false;
            }
        }
        break;

    case AK_IntegerNegate:
    case AK_LogicalNot:
        if (!sema_infer_node_type(
                lexer, ast, sema, node->a, expected_type, &type_index)) {
            return false;
        }
        if (node->kind == AK_LogicalNot) {
            if (type_index != sema_builtin_type(sema, STK_Bool)) {
                return error_0325_invalid_unary_operand(
                    lexer->source,
                    sema_node_span(lexer, node),
                    s("!"),
                    s("bool"),
                    sema_type_name(lexer, sema, &temp_arena, type_index));
            }
            type_index = sema_builtin_type(sema, STK_Bool);
            break;
        }
        if (!sema_type_is_numeric(sema, type_index)) {
            return error_0325_invalid_unary_operand(
                lexer->source,
                sema_node_span(lexer, node),
                s("-"),
                s("numeric"),
                sema_type_name(lexer, sema, &temp_arena, type_index));
        }
        break;

    case AK_Cast:
        {
            u32 source_type = sema_no_type();
            u32 target_type = sema_no_type();
            if (!sema_infer_node_type(
                    lexer, ast, sema, node->a, sema_no_type(), &source_type)) {
                return false;
            }
            if (!sema_resolve_type_node(
                    lexer, ast, sema, node->b, &target_type)) {
                return false;
            }

            if (!(sema_type_is_castable_primitive(sema, source_type) &&
                  sema_type_is_castable_primitive(sema, target_type) &&
                  sema->types[target_type].kind != STK_UntypedInteger &&
                  sema->types[target_type].kind != STK_UntypedFloat)) {
                return error_0307_invalid_cast(
                    lexer->source,
                    sema_node_span(lexer, node),
                    sema_type_name(lexer, sema, &temp_arena, source_type),
                    sema_type_name(lexer, sema, &temp_arena, target_type));
            }

            type_index = target_type;
        }
        break;

    case AK_RangeExclusive:
    case AK_RangeInclusive:
        return error_ice(
            "Range patterns must only appear inside block-form `on`");

    case AK_DestructureBind:
    case AK_DestructureVariable:
    case AK_DestructureAssign:
        {
            u32 value_node = node->b;
            u32 annotated  = sema_no_type();
            if (ast->nodes[value_node].kind == AK_AnnotatedValue) {
                annotated = sema_no_type();
                if (!sema_resolve_type_node(lexer,
                                            ast,
                                            sema,
                                            ast->nodes[value_node].a,
                                            &annotated)) {
                    return false;
                }
                value_node = ast->nodes[value_node].b;
            }
            u32 value_type = sema_no_type();
            if (!sema_infer_node_type(
                    lexer, ast, sema, value_node, annotated, &value_type)) {
                return false;
            }
            value_type = annotated != sema_no_type()
                             ? annotated
                             : sema_materialise_type(sema, value_type);
            if (node->kind == AK_DestructureAssign
                    ? !sema_check_destructure_assign_pattern_type(
                          lexer, ast, sema, node->a, value_type)
                    : !sema_assign_destructure_pattern_type(
                          lexer, ast, sema, node->a, value_type)) {
                return false;
            }
            type_index = sema_builtin_type(sema, STK_Void);
        }
        break;

    case AK_On:
        {
            const AstOnInfo* on             = &ast->ons[node->b];
            u32              bool_type      = sema_builtin_type(sema, STK_Bool);
            u32              void_type      = sema_builtin_type(sema, STK_Void);
            bool             statement_form = expected_type == void_type;
            bool             has_else       = false;
            u32              scrutinee_type = sema_no_type();
            if (on->kind != AOK_Condition &&
                !sema_infer_node_type(lexer,
                                      ast,
                                      sema,
                                      node->a,
                                      sema_no_type(),
                                      &scrutinee_type)) {
                return false;
            }
            if (on->kind == AOK_Condition) {
                scrutinee_type = bool_type;
            } else if (on->kind == AOK_Bool) {
                if (scrutinee_type != bool_type) {
                    return error_0319_invalid_on_condition(
                        lexer->source,
                        sema_node_span(lexer, &ast->nodes[node->a]),
                        sema_type_name(
                            lexer, sema, &temp_arena, scrutinee_type));
                }
            } else {
                if (sema->types[scrutinee_type].kind == STK_UntypedInteger) {
                    scrutinee_type =
                        sema_materialise_type(sema, scrutinee_type);
                    sema->node_type_indices[node->a] = scrutinee_type;
                }
                if (!(scrutinee_type == bool_type ||
                      sema->types[scrutinee_type].kind == STK_String ||
                      sema->types[scrutinee_type].kind == STK_Tuple ||
                      sema->types[scrutinee_type].kind == STK_Plex ||
                      sema->types[scrutinee_type].kind == STK_Enum ||
                      sema_type_is_concrete_integer(sema, scrutinee_type))) {
                    return error_0321_invalid_on_match_type(
                        lexer->source,
                        sema_node_span(lexer, &ast->nodes[node->a]),
                        sema_type_name(
                            lexer, sema, &temp_arena, scrutinee_type));
                }
            }

            u32 branch_type      = sema_no_type();
            u32 branch_type_node = sema_no_decl();
            for (u32 i = 0; i < on->branch_count; ++i) {
                const AstOnBranch* branch =
                    &ast->on_branches[on->first_branch + i];
                if (branch->flags & AOBF_Else) {
                    has_else = true;
                }
                u32 branch_local_index =
                    sema->on_branch_local_indices[on->first_branch + i];
                if (branch_local_index != sema_no_local()) {
                    sema->locals[branch_local_index].type_index =
                        scrutinee_type;
                }
                if (on->kind == AOK_Condition && !(branch->flags & AOBF_Else)) {
                    u32 condition_type = sema_no_type();
                    if (!sema_infer_node_type(lexer,
                                              ast,
                                              sema,
                                              branch->guard_node_index,
                                              bool_type,
                                              &condition_type)) {
                        return false;
                    }
                    if (condition_type != bool_type) {
                        return error_0319_invalid_on_condition(
                            lexer->source,
                            sema_node_span(
                                lexer, &ast->nodes[branch->guard_node_index]),
                            sema_type_name(
                                lexer, sema, &temp_arena, condition_type));
                    }
                } else if (!(branch->flags & AOBF_Else)) {
                    for (u32 pattern = 0; pattern < branch->pattern_count;
                         ++pattern) {
                        u32 pattern_index =
                            ast->pattern_items[branch->pattern_index + pattern];
                        if (!sema_check_on_pattern_type(lexer,
                                                        ast,
                                                        sema,
                                                        pattern_index,
                                                        scrutinee_type,
                                                        true)) {
                            return false;
                        }
                    }
                }

                if (branch->guard_node_index != U32_MAX &&
                    on->kind != AOK_Condition) {
                    u32 guard_type = sema_no_type();
                    if (!sema_infer_node_type(lexer,
                                              ast,
                                              sema,
                                              branch->guard_node_index,
                                              bool_type,
                                              &guard_type)) {
                        return false;
                    }
                    if (guard_type != bool_type) {
                        return error_0319_invalid_on_condition(
                            lexer->source,
                            sema_node_span(
                                lexer, &ast->nodes[branch->guard_node_index]),
                            sema_type_name(
                                lexer, sema, &temp_arena, guard_type));
                    }
                }

                if (statement_form) {
                    u32 ignored = sema_no_type();
                    if (!sema_infer_node_type(lexer,
                                              ast,
                                              sema,
                                              branch->expr_node_index,
                                              sema_no_type(),
                                              &ignored)) {
                        return false;
                    }
                    continue;
                }

                u32 current_expected = expected_type;
                if (current_expected == sema_no_type() &&
                    branch_type != sema_no_type() &&
                    sema_type_is_concrete_integer(sema, branch_type)) {
                    current_expected = branch_type;
                } else if (current_expected == sema_no_type() &&
                           branch_type != sema_no_type() &&
                           sema_type_is_concrete_float(sema, branch_type)) {
                    current_expected = branch_type;
                }

                u32 current_type = sema_no_type();
                if (!sema_infer_node_type(lexer,
                                          ast,
                                          sema,
                                          branch->expr_node_index,
                                          current_expected,
                                          &current_type)) {
                    return false;
                }

                if (branch_type == sema_no_type()) {
                    branch_type      = current_type;
                    branch_type_node = branch->expr_node_index;
                    continue;
                }

                if (sema_type_is_concrete_integer(sema, branch_type) &&
                    current_type != sema_no_type() &&
                    sema->types[current_type].kind == STK_UntypedInteger) {
                    current_type = branch_type;
                } else if (sema_type_is_concrete_integer(sema, current_type) &&
                           branch_type != sema_no_type() &&
                           sema->types[branch_type].kind ==
                               STK_UntypedInteger) {
                    branch_type      = current_type;
                    branch_type_node = branch->expr_node_index;
                } else if (sema_type_is_concrete_float(sema, branch_type) &&
                           current_type != sema_no_type() &&
                           sema->types[current_type].kind == STK_UntypedFloat) {
                    current_type = branch_type;
                } else if (sema_type_is_concrete_float(sema, current_type) &&
                           branch_type != sema_no_type() &&
                           sema->types[branch_type].kind == STK_UntypedFloat) {
                    branch_type      = current_type;
                    branch_type_node = branch->expr_node_index;
                }

                if (branch_type != current_type) {
                    return error_0320_on_branch_type_mismatch(
                        lexer->source,
                        sema_node_span(lexer, &ast->nodes[branch_type_node]),
                        sema_type_name(lexer, sema, &temp_arena, branch_type),
                        sema_node_span(lexer,
                                       &ast->nodes[branch->expr_node_index]),
                        sema_type_name(lexer, sema, &temp_arena, current_type));
                }
            }

            if (statement_form) {
                type_index = void_type;
            } else {
                if (!has_else && (on->kind == AOK_Condition ||
                                  !sema_on_covers_all_enum_variants(
                                      ast, sema, node->b, scrutinee_type))) {
                    return error_0327_non_exhaustive_on(
                        lexer->source, sema_node_span(lexer, node));
                }
                type_index = branch_type;
            }
        }
        break;

    case AK_IntegerPlus:
    case AK_IntegerMinus:
    case AK_IntegerMultiply:
    case AK_IntegerDivide:
    case AK_IntegerModulo:
    case AK_BitwiseAnd:
    case AK_BitwiseXor:
    case AK_BitwiseOr:
    case AK_Equal:
    case AK_NotEqual:
    case AK_Less:
    case AK_LessEqual:
    case AK_Greater:
    case AK_GreaterEqual:
    case AK_LogicalAnd:
    case AK_LogicalOr:
        {
            u32 lhs_type = sema_no_type();
            u32 rhs_type = sema_no_type();
            u32 binary_expected =
                sema_expected_numeric_type(sema, expected_type);

            if (!sema_infer_node_type(
                    lexer, ast, sema, node->a, binary_expected, &lhs_type)) {
                return false;
            }
            u32 rhs_expected = lhs_type;
            if (sema_type_is_numeric(sema, lhs_type)) {
                rhs_expected = sema_expected_numeric_type(sema, lhs_type);
            }
            if (!sema_infer_node_type(
                    lexer, ast, sema, node->b, rhs_expected, &rhs_type)) {
                return false;
            }

            if (sema_type_is_concrete_integer(sema, lhs_type) &&
                rhs_type != sema_no_type() &&
                sema->types[rhs_type].kind == STK_UntypedInteger) {
                rhs_type = lhs_type;
            } else if (sema_type_is_concrete_integer(sema, rhs_type) &&
                       lhs_type != sema_no_type() &&
                       sema->types[lhs_type].kind == STK_UntypedInteger) {
                lhs_type = rhs_type;
            } else if (sema_type_is_concrete_float(sema, lhs_type) &&
                       rhs_type != sema_no_type() &&
                       sema->types[rhs_type].kind == STK_UntypedFloat) {
                rhs_type = lhs_type;
            } else if (sema_type_is_concrete_float(sema, rhs_type) &&
                       lhs_type != sema_no_type() &&
                       sema->types[lhs_type].kind == STK_UntypedFloat) {
                lhs_type = rhs_type;
            }

            if (lhs_type != rhs_type) {
                return error_0304_type_mismatch(
                    lexer->source,
                    sema_node_span(lexer, node),
                    sema_type_name(lexer, sema, &temp_arena, lhs_type),
                    sema_type_name(lexer, sema, &temp_arena, rhs_type));
            }

            switch (node->kind) {
            case AK_IntegerPlus:
            case AK_IntegerMinus:
            case AK_IntegerMultiply:
            case AK_IntegerDivide:
                if (!sema_type_is_numeric(sema, lhs_type)) {
                    return error_0326_invalid_binary_operands(
                        lexer->source,
                        sema_node_span(lexer, node),
                        node->kind == AK_IntegerPlus
                            ? s("+")
                            : (node->kind == AK_IntegerMinus
                                   ? s("-")
                                   : (node->kind == AK_IntegerMultiply
                                          ? s("*")
                                          : s("/"))),
                        s("matching numeric operands"),
                        sema_type_name(lexer, sema, &temp_arena, lhs_type),
                        sema_type_name(lexer, sema, &temp_arena, rhs_type));
                }
                type_index = lhs_type;
                break;
            case AK_IntegerModulo:
            case AK_BitwiseAnd:
            case AK_BitwiseXor:
            case AK_BitwiseOr:
                if (!sema_type_is_integer(sema, lhs_type)) {
                    return error_0326_invalid_binary_operands(
                        lexer->source,
                        sema_node_span(lexer, node),
                        node->kind == AK_IntegerModulo
                            ? s("%")
                            : (node->kind == AK_BitwiseAnd
                                   ? s("&")
                                   : (node->kind == AK_BitwiseXor ? s("^")
                                                                  : s("|"))),
                        s("matching integer operands"),
                        sema_type_name(lexer, sema, &temp_arena, lhs_type),
                        sema_type_name(lexer, sema, &temp_arena, rhs_type));
                }
                type_index = lhs_type;
                break;
            case AK_Equal:
            case AK_NotEqual:
                if (!sema_type_is_equality_comparable(sema, lhs_type)) {
                    return error_0326_invalid_binary_operands(
                        lexer->source,
                        sema_node_span(lexer, node),
                        node->kind == AK_Equal ? s("==") : s("!="),
                        s("matching numeric or bool operands"),
                        sema_type_name(lexer, sema, &temp_arena, lhs_type),
                        sema_type_name(lexer, sema, &temp_arena, rhs_type));
                }
                type_index = sema_builtin_type(sema, STK_Bool);
                break;
            case AK_Less:
            case AK_LessEqual:
            case AK_Greater:
            case AK_GreaterEqual:
                if (!sema_type_is_numeric(sema, lhs_type)) {
                    return error_0326_invalid_binary_operands(
                        lexer->source,
                        sema_node_span(lexer, node),
                        node->kind == AK_Less
                            ? s("<")
                            : (node->kind == AK_LessEqual
                                   ? s("<=")
                                   : (node->kind == AK_Greater ? s(">")
                                                               : s(">="))),
                        s("matching numeric operands"),
                        sema_type_name(lexer, sema, &temp_arena, lhs_type),
                        sema_type_name(lexer, sema, &temp_arena, rhs_type));
                }
                type_index = sema_builtin_type(sema, STK_Bool);
                break;
            case AK_LogicalAnd:
            case AK_LogicalOr:
                if (lhs_type != sema_builtin_type(sema, STK_Bool)) {
                    return error_0326_invalid_binary_operands(
                        lexer->source,
                        sema_node_span(lexer, node),
                        node->kind == AK_LogicalAnd ? s("&&") : s("||"),
                        s("matching bool operands"),
                        sema_type_name(lexer, sema, &temp_arena, lhs_type),
                        sema_type_name(lexer, sema, &temp_arena, rhs_type));
                }
                type_index = sema_builtin_type(sema, STK_Bool);
                break;
            default:
                type_index = lhs_type;
                break;
            }
        }
        break;

    case AK_Call:
        {
            const AstCallInfo* call         = &ast->calls[node->b];
            u32                enum_context = sema_no_type();
            if (expected_type != sema_no_type() &&
                sema->types[expected_type].kind == STK_Enum) {
                enum_context = expected_type;
            }
            const AstNode* possible_callee = &ast->nodes[node->a];
            if (enum_context == sema_no_type() &&
                possible_callee->kind == AK_Field) {
                u32 qualified_type = sema_no_type();
                if (sema_try_resolve_type_symbol(lexer,
                                                 ast,
                                                 sema,
                                                 possible_callee->a,
                                                 &qualified_type) &&
                    sema->types[qualified_type].kind == STK_Enum) {
                    enum_context = qualified_type;
                }
            }
            if (enum_context != sema_no_type()) {
                const AstNode* callee         = &ast->nodes[node->a];
                u32            variant_symbol = U32_MAX;
                if (callee->kind == AK_SymbolRef &&
                    sema->node_local_indices[node->a] == sema_no_local() &&
                    sema->node_decl_indices[node->a] == sema_no_decl()) {
                    variant_symbol = callee->a;
                } else if (callee->kind == AK_Field) {
                    u32 qualified_type = sema_no_type();
                    if (!sema_try_resolve_type_symbol(
                            lexer, ast, sema, callee->a, &qualified_type)) {
                        qualified_type = sema_no_type();
                    }
                    if (qualified_type != enum_context) {
                        return error_0304_type_mismatch(
                            lexer->source,
                            sema_node_span(lexer, callee),
                            sema_type_name(
                                lexer, sema, &temp_arena, enum_context),
                            sema_type_name(
                                lexer, sema, &temp_arena, qualified_type));
                    }
                    variant_symbol = callee->b;
                }
                if (variant_symbol != U32_MAX) {
                    u32 variant = sema_enum_variant_index(
                        sema, enum_context, variant_symbol);
                    if (variant == U32_MAX) {
                        return error_0304_type_mismatch(
                            lexer->source,
                            sema_node_span(lexer, callee),
                            s("known enum variant"),
                            lex_symbol(lexer, variant_symbol));
                    }
                    u32 payload_type = sema_enum_variant_payload_type(
                        sema, enum_context, variant);
                    u32 expected_count =
                        payload_type == sema_no_type()
                            ? 0
                            : (sema->types[payload_type].kind == STK_Tuple
                                   ? sema->types[payload_type].param_count
                                   : 1);
                    if (expected_count != call->arg_count) {
                        return error_0313_argument_count_mismatch(
                            lexer->source,
                            sema_node_span(lexer, node),
                            expected_count,
                            call->arg_count);
                    }
                    for (u32 i = 0; i < call->arg_count; ++i) {
                        u32 arg_node = ast->call_args[call->first_arg + i];
                        u32 expected_arg =
                            sema->types[payload_type].kind == STK_Tuple
                                ? sema->type_param_types
                                      [sema->types[payload_type]
                                           .first_param_type +
                                       i]
                                : payload_type;
                        u32 arg_type = sema_no_type();
                        if (!sema_infer_node_type(lexer,
                                                  ast,
                                                  sema,
                                                  arg_node,
                                                  expected_arg,
                                                  &arg_type)) {
                            return false;
                        }
                        if (!sema_type_matches(sema, expected_arg, arg_type)) {
                            return error_0304_type_mismatch(
                                lexer->source,
                                sema_node_span(lexer, &ast->nodes[arg_node]),
                                sema_type_name(
                                    lexer, sema, &temp_arena, expected_arg),
                                sema_type_name(
                                    lexer, sema, &temp_arena, arg_type));
                        }
                    }
                    sema->node_type_indices[node->a] = enum_context;
                    type_index                       = enum_context;
                    break;
                }
            }

            u32 callee_type = sema_no_type();
            if (!sema_infer_node_type(
                    lexer, ast, sema, node->a, sema_no_type(), &callee_type)) {
                return false;
            }
            if (callee_type == sema_no_type() ||
                sema->types[callee_type].kind != STK_Function) {
                return error_0304_type_mismatch(
                    lexer->source,
                    sema_node_span(lexer, node),
                    s("function"),
                    sema_type_name(lexer, sema, &temp_arena, callee_type));
            }

            const SemaType* fn_type = &sema->types[callee_type];
            bool is_varargs = (fn_type->flags & STF_FunctionVarargs) != 0;
            if ((!is_varargs && fn_type->param_count != call->arg_count) ||
                (is_varargs && call->arg_count < fn_type->param_count)) {
                return error_0313_argument_count_mismatch(
                    lexer->source,
                    sema_node_span(lexer, node),
                    fn_type->param_count,
                    call->arg_count);
            }

            for (u32 i = 0; i < call->arg_count; ++i) {
                u32 arg_node = ast->call_args[call->first_arg + i];
                u32 expected_arg =
                    i < fn_type->param_count
                        ? sema->type_param_types[fn_type->first_param_type + i]
                        : sema_no_type();
                u32 arg_type = sema_no_type();
                if (!sema_infer_node_type(
                        lexer, ast, sema, arg_node, expected_arg, &arg_type)) {
                    return false;
                }
                if (i >= fn_type->param_count) {
                    arg_type = sema_materialise_type(sema, arg_type);
                    if (!sema_type_is_ffi_safe(sema, arg_type)) {
                        return error_0304_type_mismatch(
                            lexer->source,
                            sema_node_span(lexer, &ast->nodes[arg_node]),
                            s("FFI-safe vararg type"),
                            sema_type_name(lexer, sema, &temp_arena, arg_type));
                    }
                    continue;
                }
                if (!sema_type_matches(sema, expected_arg, arg_type)) {
                    return error_0304_type_mismatch(
                        lexer->source,
                        sema_node_span(lexer, &ast->nodes[arg_node]),
                        sema_type_name(lexer, sema, &temp_arena, expected_arg),
                        sema_type_name(lexer, sema, &temp_arena, arg_type));
                }
            }

            type_index = fn_type->return_type;
        }
        break;

    case AK_Variable:
        {
            u32 local_index = sema->node_local_indices[node_index];
            ASSERT(local_index != sema_no_local(), "Expected resolved local");
            SemaLocal* local = &sema->locals[local_index];
            if (local->type_index != sema_no_type()) {
                type_index = local->type_index;
                break;
            }

            u32 annotated = sema_no_type();
            if (local->type_node_index != sema_no_type() &&
                !sema_resolve_type_node(
                    lexer, ast, sema, local->type_node_index, &annotated)) {
                return false;
            }

            if (local->value_node_index == sema_no_decl()) {
                if (!sema_type_is_variable_storage(sema, annotated)) {
                    return error_0306_invalid_variable_type(
                        lexer->source,
                        sema_local_span(lexer, ast, local),
                        sema_type_name(lexer, sema, &temp_arena, annotated));
                }
                local->type_index = annotated;
                type_index        = annotated;
                break;
            }

            if (sema_node_contains_interpolation(ast,
                                                 local->value_node_index)) {
                u32 interp_index = sema_find_interpolated_string_node(
                    ast, local->value_node_index);
                return error_0312_interpolated_string_escapes(
                    lexer->source,
                    sema_node_span(lexer,
                                   &ast->nodes[interp_index == sema_no_decl()
                                                   ? local->value_node_index
                                                   : interp_index]));
            }

            if (!sema_infer_node_type(lexer,
                                      ast,
                                      sema,
                                      local->value_node_index,
                                      annotated,
                                      &type_index)) {
                return false;
            }

            type_index = annotated != sema_no_type()
                             ? annotated
                             : sema_materialise_type(sema, type_index);
            if (!sema_type_is_variable_storage(sema, type_index)) {
                return error_0306_invalid_variable_type(
                    lexer->source,
                    sema_local_span(lexer, ast, local),
                    sema_type_name(lexer, sema, &temp_arena, type_index));
            }
            local->type_index = type_index;
        }
        break;

    case AK_Assign:
        {
            u32 target_type = sema_no_type();
            if (sema->node_local_indices[node_index] != sema_no_local()) {
                target_type = sema->locals[sema->node_local_indices[node_index]]
                                  .type_index;
            } else {
                u32 decl_index = sema->node_decl_indices[node_index];
                ASSERT(decl_index != sema_no_decl(),
                       "Expected resolved target");
                if (sema->decls[decl_index].kind != SK_Variable) {
                    return error_0305_invalid_assignment_target(
                        lexer->source,
                        sema_node_span(lexer, node),
                        lex_symbol(lexer, node->a));
                }
                target_type = sema->decls[decl_index].type_index;
            }

            if (sema_node_contains_interpolation(ast, node->b)) {
                u32 interp_index =
                    sema_find_interpolated_string_node(ast, node->b);
                return error_0312_interpolated_string_escapes(
                    lexer->source,
                    sema_node_span(lexer,
                                   &ast->nodes[interp_index == sema_no_decl()
                                                   ? node->b
                                                   : interp_index]));
            }

            if (!sema_infer_node_type(
                    lexer, ast, sema, node->b, target_type, &type_index)) {
                return false;
            }
            type_index = target_type;
        }
        break;

    case AK_FnDef:
        {
            const AstNode*        fn_start  = &ast->nodes[node->a];
            const AstFnSignature* signature = &ast->fn_signatures[fn_start->a];
            bool                  has_explicit_return_type =
                signature->return_type_node_index != U32_MAX;

            if (node->b == AFK_Expr && has_explicit_return_type) {
                return error_0318_mixed_function_return_style(
                    lexer->source, sema_node_span(lexer, node));
            }

            u32 return_type = sema_builtin_type(
                sema, node->b == AFK_Block ? STK_I32 : STK_UntypedInteger);

            if (has_explicit_return_type &&
                !sema_resolve_type_node(lexer,
                                        ast,
                                        sema,
                                        signature->return_type_node_index,
                                        &return_type)) {
                return false;
            }

            if (node->b == AFK_Expr) {
                if (sema_node_contains_interpolation(ast, fn_start->b - 1)) {
                    u32 interp_index = sema_find_interpolated_string_node(
                        ast, fn_start->b - 1);
                    return error_0312_interpolated_string_escapes(
                        lexer->source,
                        sema_node_span(
                            lexer,
                            &ast->nodes[interp_index == sema_no_decl()
                                            ? fn_start->b - 1
                                            : interp_index]));
                }
                if (!sema_infer_node_type(lexer,
                                          ast,
                                          sema,
                                          fn_start->b - 1,
                                          sema_no_type(),
                                          &return_type)) {
                    return false;
                }
                return_type = sema_materialise_type(sema, return_type);
            } else {
                bool has_return = false;
                if (!sema_infer_block_statements(lexer,
                                                 ast,
                                                 sema,
                                                 node->a + 1,
                                                 fn_start->b,
                                                 &return_type,
                                                 &has_return)) {
                    return false;
                }
                if (has_explicit_return_type && !has_return) {
                    return error_0314_missing_return(
                        lexer->source,
                        sema_node_span(lexer, node),
                        sema_type_name(lexer, sema, &temp_arena, return_type));
                }
            }

            Array(u32) param_types = NULL;
            for (u32 i = 0; i < signature->param_count; ++i) {
                u32 param_type = sema_no_type();
                if (!sema_resolve_type_node(
                        lexer,
                        ast,
                        sema,
                        ast->params[signature->first_param + i].type_node_index,
                        &param_type)) {
                    array_free(param_types);
                    return false;
                }
                array_push(param_types, param_type);
            }
            type_index = sema_add_function_type_ex(
                sema,
                param_types,
                return_type,
                signature->is_varargs ? STF_FunctionVarargs : STF_None);
            array_free(param_types);
        }
        break;

    case AK_FfiDef:
        {
            const AstFfiInfo*     ffi_info = &ast->ffi_infos[node->a];
            const AstFnSignature* signature =
                &ast->fn_signatures[ffi_info->signature_index];
            u32 string_type  = sema_builtin_type(sema, STK_String);
            u32 library_type = sema_no_type();
            if (!sema_infer_node_type(lexer,
                                      ast,
                                      sema,
                                      ffi_info->library_node_index,
                                      string_type,
                                      &library_type)) {
                return false;
            }
            if (!sema_type_matches(sema, library_type, string_type)) {
                return error_0304_type_mismatch(
                    lexer->source,
                    sema_node_span(lexer,
                                   &ast->nodes[ffi_info->library_node_index]),
                    sema_type_name(lexer, sema, &temp_arena, string_type),
                    sema_type_name(lexer, sema, &temp_arena, library_type));
            }
            if (!sema_expr_is_constantish(
                    ast, sema, ffi_info->library_node_index)) {
                return error_0304_type_mismatch(
                    lexer->source,
                    sema_node_span(lexer,
                                   &ast->nodes[ffi_info->library_node_index]),
                    s("compile-time string"),
                    sema_type_name(lexer, sema, &temp_arena, library_type));
            }

            u32 return_type = sema_builtin_type(sema, STK_Void);
            if (signature->return_type_node_index != U32_MAX &&
                !sema_resolve_type_node(lexer,
                                        ast,
                                        sema,
                                        signature->return_type_node_index,
                                        &return_type)) {
                return false;
            }
            if (!sema_type_is_ffi_safe(sema, return_type)) {
                u32 span_node = signature->return_type_node_index != U32_MAX
                                    ? signature->return_type_node_index
                                    : node_index;
                return error_0304_type_mismatch(
                    lexer->source,
                    sema_node_span(lexer, &ast->nodes[span_node]),
                    s("FFI-safe return type"),
                    sema_type_name(lexer, sema, &temp_arena, return_type));
            }

            Array(u32) param_types = NULL;
            for (u32 i = 0; i < signature->param_count; ++i) {
                u32 param_type_node =
                    ast->params[signature->first_param + i].type_node_index;
                u32 param_type = sema_no_type();
                if (!sema_resolve_type_node(
                        lexer, ast, sema, param_type_node, &param_type)) {
                    array_free(param_types);
                    return false;
                }
                if (!sema_type_is_ffi_safe(sema, param_type)) {
                    array_free(param_types);
                    return error_0304_type_mismatch(
                        lexer->source,
                        sema_node_span(lexer, &ast->nodes[param_type_node]),
                        s("FFI-safe parameter type"),
                        sema_type_name(lexer, sema, &temp_arena, param_type));
                }
                array_push(param_types, param_type);
            }
            type_index = sema_add_function_type_ex(
                sema,
                param_types,
                return_type,
                signature->is_varargs ? STF_FunctionVarargs : STF_None);
            array_free(param_types);
        }
        break;

    case AK_ModRef:
        if (!sema_resolve_loaded_module(lexer,
                                        ast,
                                        sema,
                                        node->a,
                                        sema_node_span(lexer, node),
                                        &type_index)) {
            return false;
        }
        break;

    case AK_Use:
        type_index = sema_builtin_type(sema, STK_Void);
        break;

    default:
        type_index = sema_no_type();
        break;
    }

    if (expected_type != sema_no_type() &&
        sema_type_is_unsigned_integer(sema, expected_type) &&
        type_index != sema_no_type() &&
        sema->types[type_index].kind == STK_UntypedInteger) {
        i64 value = 0;
        if (sema_try_eval_integer_constant(
                lexer, ast, sema, node_index, &value) &&
            value < 0) {
            return error_0323_negative_unsigned_inference(
                lexer->source,
                sema_node_span(lexer, node),
                sema_type_name(lexer, sema, &temp_arena, expected_type));
        }
    }

    if (expected_type != sema_no_type() &&
        !sema_type_matches(sema, expected_type, type_index)) {
        return error_0304_type_mismatch(
            lexer->source,
            sema_node_span(lexer, node),
            sema_type_name(lexer, sema, &temp_arena, expected_type),
            sema_type_name(lexer, sema, &temp_arena, type_index));
    }

    sema->node_type_indices[node_index] = type_index;
    *out_type_index                     = type_index;
    return true;
}

//------------------------------------------------------------------------------
// Resolve type annotations and infer declaration/value types.

internal bool
sema_assign_decl_types(const Lexer* lexer, const Ast* ast, Sema* sema)
{
    for (u32 i = 0; i < array_count(sema->ordered_decl_indices); ++i) {
        SemaDecl* decl          = &sema->decls[sema->ordered_decl_indices[i]];
        u32       annotated     = sema_no_type();
        u32       inferred_type = sema_no_type();

        if (decl->kind == SK_TypeAlias) {
            if (decl->bind_node_index != sema_no_decl()) {
                sema->node_type_indices[decl->bind_node_index] =
                    decl->type_index;
            }
            continue;
        }

        if (decl->kind == SK_BuiltinFunction) {
            continue;
        }

        if (decl->bind_node_index == sema_no_decl() &&
            decl->value_node_index == sema_no_decl() &&
            decl->type_index != sema_no_type()) {
            continue;
        }

        if (decl->type_node_index != sema_no_type() &&
            !sema_resolve_type_node(
                lexer, ast, sema, decl->type_node_index, &annotated)) {
            return false;
        }

        if (decl->value_node_index != sema_no_decl()) {
            if (!sema_infer_node_type(lexer,
                                      ast,
                                      sema,
                                      decl->value_node_index,
                                      annotated,
                                      &inferred_type)) {
                return false;
            }
        }

        if (decl->kind == SK_Variable) {
            decl->type_index = annotated != sema_no_type()
                                   ? annotated
                                   : sema_materialise_type(sema, inferred_type);
            if (!sema_type_is_variable_storage(sema, decl->type_index)) {
                return error_0306_invalid_variable_type(
                    lexer->source,
                    sema_decl_span(lexer, ast, decl),
                    sema_type_name(lexer, sema, &temp_arena, decl->type_index));
            }
        } else {
            decl->type_index =
                annotated != sema_no_type()
                    ? annotated
                    : (decl->kind == SK_Function ||
                               decl->kind == SK_FfiFunction ||
                               decl->kind == SK_Module ||
                               decl->kind == SK_Constant
                           ? inferred_type
                           : sema_materialise_type(sema, inferred_type));
        }

        if (decl->bind_node_index != sema_no_decl()) {
            sema->node_type_indices[decl->bind_node_index] = decl->type_index;
        }
    }

    return true;
}

internal bool
sema_assign_local_types(const Lexer* lexer, const Ast* ast, Sema* sema)
{
    for (u32 i = 0; i < array_count(sema->locals); ++i) {
        SemaLocal* local = &sema->locals[i];
        if (!sema_local_is_decl_binding(local)) {
            continue;
        }

        u32 ignored = sema_no_type();
        if (!sema_infer_local_binding_type(lexer, ast, sema, i, &ignored)) {
            return false;
        }
    }

    return true;
}

//------------------------------------------------------------------------------
// Stack-machine phases used while folding constant expressions.

typedef enum : u8 {
    SEMA_FOLD_ENTER,
    SEMA_FOLD_REDUCE,
} SemaFoldPhase;

//------------------------------------------------------------------------------
// One explicit evaluator frame for the constant-folder VM.

typedef struct {
    u32           node_index;
    SemaFoldPhase phase;
} SemaFoldFrame;

//------------------------------------------------------------------------------
// Clear AST-local fold flags before a fresh semantic pass.

internal void sema_clear_fold_flags(Ast* ast)
{
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        ast->nodes[i].flags &= ~(ANF_ConstKnown | ANF_ConstBusy);
    }
}

//------------------------------------------------------------------------------
// Push one AST node onto the folding stack for later evaluation.

internal void sema_push_fold_frame(Array(SemaFoldFrame) * stack, u32 node_index)
{
    array_push(*stack,
               (SemaFoldFrame){
                   .node_index = node_index,
                   .phase      = SEMA_FOLD_ENTER,
               });
}

//------------------------------------------------------------------------------
// Reduce one AST node after its children have already been visited.

internal bool sema_reduce_folded_node(const Lexer* lex,
                                      Ast*         ast,
                                      const Sema*  sema,
                                      u32          node_index,
                                      Sema*        out_sema,
                                      i64*         out_value)
{
    AstNode* node  = &ast->nodes[node_index];
    i64      value = 0;
    bool     ok    = false;

    switch (node->kind) {
    case AK_IntegerLiteral:
        value = (i64)ast_get_integer(lex, node);
        ok    = true;
        break;

    case AK_BoolLiteral:
        value = node->a != 0 ? 1 : 0;
        ok    = true;
        break;

    case AK_StringLiteral:
    case AK_InterpPartExpr:
    case AK_InterpolatedString:
        ok = false;
        break;
    case AK_StringConcat:
        ok = false;
        break;

    case AK_SymbolRef:
        {
            if (out_sema->node_local_indices[node_index] != sema_no_local()) {
                const SemaLocal* local =
                    &sema->locals[out_sema->node_local_indices[node_index]];
                if (local->kind == SLK_Constant) {
                    ok = sema_try_get_constant(
                        ast, out_sema, local->value_node_index, &value);
                } else {
                    ok = false;
                }
                break;
            }
            u32 decl_index = sema->node_decl_indices[node_index];
            if (decl_index == sema_no_decl()) {
                ok = false;
                break;
            }
            const SemaDecl* decl = &sema->decls[decl_index];
            if (decl->kind == SK_Constant) {
                ok = sema_try_get_constant(
                    ast, out_sema, decl->value_node_index, &value);
            }
        }
        break;

    case AK_Expression:
    case AK_Statement:
        ok = sema_try_get_constant(ast, out_sema, node->a, &value);
        break;
    case AK_Return:
    case AK_ReturnExpr:
        ok = node->a != U32_MAX &&
             sema_try_get_constant(ast, out_sema, node->a, &value);
        break;

    case AK_Call:
    case AK_Cast:
    case AK_RangeExclusive:
    case AK_RangeInclusive:
    case AK_FnDef:
    case AK_Variable:
    case AK_Assign:
    case AK_Break:
    case AK_Continue:
    case AK_BreakExpr:
    case AK_ContinueExpr:
        ok = false;
        break;

    case AK_IntegerNegate:
        ok = sema_try_get_constant(ast, out_sema, node->a, &value);
        if (ok) {
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
            ok      = sema_try_get_constant(ast, out_sema, node->a, &lhs) &&
                 sema_try_get_constant(ast, out_sema, node->b, &rhs);
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

    ast_clear_flag(node, ANF_ConstBusy);
    if (!ok) {
        return false;
    }

    sema_set_constant(ast, out_sema, node_index, value);
    *out_value = value;
    return true;
}

//------------------------------------------------------------------------------
// Fold one AST node to a signed integer using an explicit VM-style stack.

internal bool sema_fold_node(const Lexer* lex,
                             Ast*         ast,
                             const Sema*  sema,
                             u32          root_node_index,
                             Sema*        out_sema,
                             i64*         out_value)
{
    if (root_node_index < array_count(sema->node_is_type_expr) &&
        sema->node_is_type_expr[root_node_index]) {
        return false;
    }

    Array(SemaFoldFrame) stack = NULL;
    sema_push_fold_frame(&stack, root_node_index);

    while (array_count(stack) > 0) {
        SemaFoldFrame* frame = &stack[array_count(stack) - 1];
        AstNode*       node  = &ast->nodes[frame->node_index];
        i64            value = 0;

        if (frame->node_index < array_count(sema->node_is_type_expr) &&
            sema->node_is_type_expr[frame->node_index]) {
            array_pop(stack);
            continue;
        }

        if (frame->phase == SEMA_FOLD_ENTER) {
            if (sema_try_get_constant(
                    ast, out_sema, frame->node_index, &value)) {
                array_pop(stack);
                continue;
            }

            if (ast_has_flag(node, ANF_ConstBusy)) {
                array_pop(stack);
                continue;
            }

            ast_set_flag(node, ANF_ConstBusy);
            frame->phase = SEMA_FOLD_REDUCE;

            switch (node->kind) {
            case AK_SymbolRef:
                {
                    if (out_sema->node_local_indices[frame->node_index] !=
                        sema_no_local()) {
                        const SemaLocal* local =
                            &sema->locals[out_sema->node_local_indices
                                              [frame->node_index]];
                        if (local->kind == SLK_Constant) {
                            sema_push_fold_frame(&stack,
                                                 local->value_node_index);
                        }
                        break;
                    }
                    u32 decl_index = sema->node_decl_indices[frame->node_index];
                    if (decl_index == sema_no_decl()) {
                        break;
                    }
                    const SemaDecl* decl = &sema->decls[decl_index];
                    if (decl->kind == SK_Constant) {
                        sema_push_fold_frame(&stack, decl->value_node_index);
                    }
                }
                break;

            case AK_Expression:
            case AK_Statement:
            case AK_IntegerNegate:
            case AK_Cast:
            case AK_InterpPartExpr:
                sema_push_fold_frame(&stack, node->a);
                break;
            case AK_Return:
            case AK_ReturnExpr:
                if (node->a != U32_MAX) {
                    sema_push_fold_frame(&stack, node->a);
                }
                break;

            case AK_Call:
                {
                    const AstCallInfo* call = &ast->calls[node->b];
                    for (u32 i = call->arg_count; i-- > 0;) {
                        sema_push_fold_frame(
                            &stack, ast->call_args[call->first_arg + i]);
                    }
                }
                sema_push_fold_frame(&stack, node->a);
                break;

            case AK_InterpolatedString:
                for (u32 i = node->b; i-- > node->a;) {
                    sema_push_fold_frame(&stack, i);
                }
                break;

            case AK_IntegerPlus:
            case AK_IntegerMinus:
            case AK_IntegerMultiply:
            case AK_IntegerDivide:
            case AK_IntegerModulo:
            case AK_RangeExclusive:
            case AK_RangeInclusive:
                sema_push_fold_frame(&stack, node->b);
                sema_push_fold_frame(&stack, node->a);
                break;

            default:
                break;
            }

            continue;
        }

        sema_reduce_folded_node(
            lex, ast, sema, frame->node_index, out_sema, &value);
        array_pop(stack);
    }

    array_free(stack);
    return sema_try_get_constant(ast, out_sema, root_node_index, out_value);
}

//------------------------------------------------------------------------------
// Fold all constant-capable AST nodes into semantic side tables.

internal void sema_fold_constants(const Lexer* lex, Ast* ast, Sema* sema)
{
    sema_clear_fold_flags(ast);

    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        i64 ignored = 0;
        sema_fold_node(lex, ast, sema, i, sema, &ignored);
    }
}

internal bool
sema_validate_entry_point(const Lexer* lexer, const Ast* ast, Sema* sema)
{
    u32 main_symbol = sema_find_symbol_handle_by_name(lexer, s("main"));
    if (main_symbol == sema_no_decl()) {
        return error_0315_missing_entry_point(
            lexer->source, (ErrorSpan){.start = 0, .end = 0});
    }

    u32 decl_index = sema_find_decl(sema, main_symbol);
    if (decl_index == sema_no_decl()) {
        return error_0315_missing_entry_point(
            lexer->source, (ErrorSpan){.start = 0, .end = 0});
    }

    const SemaDecl* decl       = &sema->decls[decl_index];
    u32             type_index = decl->type_index;
    if (decl->kind != SK_Function || type_index == sema_no_type() ||
        sema->types[type_index].kind != STK_Function) {
        return error_0316_invalid_entry_point(
            lexer->source,
            sema_decl_span(lexer, ast, decl),
            sema_type_name(lexer, sema, &temp_arena, type_index));
    }

    const SemaType* fn_type = &sema->types[type_index];
    if (fn_type->param_count != 0 ||
        !sema_type_is_integer(sema, fn_type->return_type)) {
        return error_0316_invalid_entry_point(
            lexer->source,
            sema_decl_span(lexer, ast, decl),
            sema_type_name(lexer, sema, &temp_arena, type_index));
    }

    return true;
}

//------------------------------------------------------------------------------
// Validate loop-control statements against their lexical loop nesting.

internal bool sema_block_is_expr_block_body(const Ast* ast, u32 block_index)
{
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind == AK_ExprBlock && node->a == block_index) {
            return true;
        }
    }
    return false;
}

#define SEMA_MAX_CONTROL_LABELS 64
#define SEMA_LOOP_LABEL_FLAG 0x80000000u

internal bool
sema_control_label_is_visible(const u32* labels, u32 label_count, u32 label)
{
    for (u32 i = label_count; i > 0; --i) {
        u32 visible_label = labels[i - 1] & ~SEMA_LOOP_LABEL_FLAG;
        if (visible_label == label) {
            return true;
        }
    }
    return false;
}

internal bool
sema_loop_label_is_visible(const u32* labels, u32 label_count, u32 label)
{
    for (u32 i = label_count; i > 0; --i) {
        if ((labels[i - 1] & SEMA_LOOP_LABEL_FLAG) &&
            (labels[i - 1] & ~SEMA_LOOP_LABEL_FLAG) == label) {
            return true;
        }
    }
    return false;
}

internal bool sema_validate_loop_control(const Lexer* lexer,
                                         const Ast*   ast,
                                         u32          node_index,
                                         u32          loop_depth,
                                         u32          expr_block_depth,
                                         u32*         expr_labels,
                                         u32          expr_label_count)
{
    const AstNode* node = &ast->nodes[node_index];

    switch (node->kind) {
    case AK_Break:
    case AK_BreakExpr:
        if (node->b != U32_MAX && !sema_control_label_is_visible(
                                      expr_labels, expr_label_count, node->b)) {
            return error_0330_unknown_control_label(lexer->source,
                                                    sema_node_span(lexer, node),
                                                    lex_symbol(lexer, node->b));
        }
        if (loop_depth == 0 && expr_block_depth == 0) {
            return error_0328_loop_control_outside_loop(
                lexer->source, sema_node_span(lexer, node), s("break"));
        }
        if (node->a != U32_MAX) {
            return sema_validate_loop_control(lexer,
                                              ast,
                                              node->a,
                                              loop_depth,
                                              expr_block_depth,
                                              expr_labels,
                                              expr_label_count);
        }
        return true;
    case AK_Continue:
    case AK_ContinueExpr:
        if (loop_depth == 0) {
            return error_0328_loop_control_outside_loop(
                lexer->source, sema_node_span(lexer, node), s("continue"));
        }
        if (node->b != U32_MAX) {
            if (sema_loop_label_is_visible(
                    expr_labels, expr_label_count, node->b)) {
                return true;
            }
            if (sema_control_label_is_visible(
                    expr_labels, expr_label_count, node->b)) {
                return error_0331_continue_to_non_loop_label(
                    lexer->source,
                    sema_node_span(lexer, node),
                    lex_symbol(lexer, node->b));
            }
            return error_0330_unknown_control_label(lexer->source,
                                                    sema_node_span(lexer, node),
                                                    lex_symbol(lexer, node->b));
        }
        return true;
    case AK_Block:
        for (u32 i = node->a; i < node->b; ++i) {
            if (ast->nodes[i].kind == AK_Block &&
                sema_block_is_expr_block_body(ast, i)) {
                i = ast->nodes[i].b - 1;
                continue;
            }
            if (!ast_node_is_block_statement(&ast->nodes[i])) {
                continue;
            }
            if (!sema_validate_loop_control(lexer,
                                            ast,
                                            i,
                                            loop_depth,
                                            expr_block_depth,
                                            expr_labels,
                                            expr_label_count)) {
                return false;
            }
            i = ast_block_statement_end_exclusive(ast, i) - 1;
        }
        return true;
    case AK_ExprBlock:
        {
            ASSERT(expr_label_count < SEMA_MAX_CONTROL_LABELS,
                   "Too many nested expression block labels");
            u32 next_label_count = expr_label_count;
            if (node->b != U32_MAX) {
                expr_labels[next_label_count++] = node->b;
            }
            return sema_validate_loop_control(lexer,
                                              ast,
                                              node->a,
                                              loop_depth,
                                              expr_block_depth + 1,
                                              expr_labels,
                                              next_label_count);
        }
    case AK_For:
        {
            const AstForInfo* for_info = &ast->fors[node->a];
            ASSERT(expr_label_count < SEMA_MAX_CONTROL_LABELS,
                   "Too many nested control labels");
            u32 next_label_count = expr_label_count;
            if (for_info->label_symbol != U32_MAX) {
                expr_labels[next_label_count++] =
                    for_info->label_symbol | SEMA_LOOP_LABEL_FLAG;
            }
            for (u32 i = 0; i < for_info->init_count; ++i) {
                if (!sema_validate_loop_control(
                        lexer,
                        ast,
                        ast->for_items[for_info->first_init + i],
                        0,
                        expr_block_depth,
                        expr_labels,
                        next_label_count)) {
                    return false;
                }
            }
            if (for_info->condition_node_index != U32_MAX &&
                !sema_validate_loop_control(lexer,
                                            ast,
                                            for_info->condition_node_index,
                                            0,
                                            expr_block_depth,
                                            expr_labels,
                                            next_label_count)) {
                return false;
            }
            for (u32 i = 0; i < for_info->update_count; ++i) {
                if (!sema_validate_loop_control(
                        lexer,
                        ast,
                        ast->for_items[for_info->first_update + i],
                        0,
                        expr_block_depth,
                        expr_labels,
                        next_label_count)) {
                    return false;
                }
            }
            if (for_info->else_block_index != U32_MAX &&
                !sema_block_has_value_break_for_target(
                    ast, node->b, for_info->label_symbol)) {
                return error_0333_invalid_loop_else(
                    lexer->source,
                    sema_node_span(lexer,
                                   &ast->nodes[for_info->else_block_index]));
            }
            if (!sema_validate_loop_control(lexer,
                                            ast,
                                            node->b,
                                            loop_depth + 1,
                                            expr_block_depth,
                                            expr_labels,
                                            next_label_count)) {
                return false;
            }
            return for_info->else_block_index == U32_MAX ||
                   sema_validate_loop_control(lexer,
                                              ast,
                                              for_info->else_block_index,
                                              loop_depth + 1,
                                              expr_block_depth,
                                              expr_labels,
                                              next_label_count);
        }
    case AK_FnDef:
        {
            const AstNode* fn_start = &ast->nodes[node->a];
            if (node->b == AFK_Block) {
                for (u32 i = node->a + 1; i < fn_start->b; ++i) {
                    if (ast->nodes[i].kind == AK_Block &&
                        sema_block_is_expr_block_body(ast, i)) {
                        i = ast->nodes[i].b - 1;
                        continue;
                    }
                    if (!ast_node_is_block_statement(&ast->nodes[i])) {
                        continue;
                    }
                    u32 fn_labels[SEMA_MAX_CONTROL_LABELS] = {0};
                    if (!sema_validate_loop_control(
                            lexer, ast, i, 0, 0, fn_labels, 0)) {
                        return false;
                    }
                    i = ast_block_statement_end_exclusive(ast, i) - 1;
                }
                return true;
            }
            {
                u32 fn_labels[SEMA_MAX_CONTROL_LABELS] = {0};
                return sema_validate_loop_control(
                    lexer, ast, fn_start->b - 1, 0, 0, fn_labels, 0);
            }
        }
    case AK_On:
        {
            const AstOnInfo* on = &ast->ons[node->b];
            if (node->a != U32_MAX &&
                !sema_validate_loop_control(lexer,
                                            ast,
                                            node->a,
                                            loop_depth,
                                            expr_block_depth,
                                            expr_labels,
                                            expr_label_count)) {
                return false;
            }
            for (u32 i = 0; i < on->branch_count; ++i) {
                const AstOnBranch* branch =
                    &ast->on_branches[on->first_branch + i];
                if (on->kind == AOK_Condition && !(branch->flags & AOBF_Else) &&
                    !sema_validate_loop_control(lexer,
                                                ast,
                                                branch->guard_node_index,
                                                loop_depth,
                                                expr_block_depth,
                                                expr_labels,
                                                expr_label_count)) {
                    return false;
                }
                if (!sema_validate_loop_control(lexer,
                                                ast,
                                                branch->expr_node_index,
                                                loop_depth,
                                                expr_block_depth,
                                                expr_labels,
                                                expr_label_count)) {
                    return false;
                }
            }
            return true;
        }
    case AK_Call:
        {
            if (!sema_validate_loop_control(lexer,
                                            ast,
                                            node->a,
                                            loop_depth,
                                            expr_block_depth,
                                            expr_labels,
                                            expr_label_count)) {
                return false;
            }
            const AstCallInfo* call = &ast->calls[node->b];
            for (u32 i = 0; i < call->arg_count; ++i) {
                if (!sema_validate_loop_control(
                        lexer,
                        ast,
                        ast->call_args[call->first_arg + i],
                        loop_depth,
                        expr_block_depth,
                        expr_labels,
                        expr_label_count)) {
                    return false;
                }
            }
            return true;
        }
    case AK_Tuple:
    case AK_Array:
        for (u32 i = 0; i < node->b; ++i) {
            if (!sema_validate_loop_control(lexer,
                                            ast,
                                            ast->tuple_items[node->a + i],
                                            loop_depth,
                                            expr_block_depth,
                                            expr_labels,
                                            expr_label_count)) {
                return false;
            }
        }
        return true;
    case AK_TupleField:
    case AK_Index:
        if (node->kind == AK_Index &&
            !sema_validate_loop_control(lexer,
                                        ast,
                                        node->b,
                                        loop_depth,
                                        expr_block_depth,
                                        expr_labels,
                                        expr_label_count)) {
            return false;
        }
        return sema_validate_loop_control(lexer,
                                          ast,
                                          node->a,
                                          loop_depth,
                                          expr_block_depth,
                                          expr_labels,
                                          expr_label_count);
    case AK_Plex:
    case AK_PlexUpdate:
        {
            const AstPlexLiteralInfo* literal = &ast->plex_literals[node->a];
            if (node->kind == AK_PlexUpdate &&
                !sema_validate_loop_control(lexer,
                                            ast,
                                            literal->target_node_index,
                                            loop_depth,
                                            expr_block_depth,
                                            expr_labels,
                                            expr_label_count)) {
                return false;
            }
            for (u32 i = 0; i < literal->field_count; ++i) {
                if (!sema_validate_loop_control(
                        lexer,
                        ast,
                        ast->plex_literal_fields[literal->first_field + i]
                            .value_node_index,
                        loop_depth,
                        expr_block_depth,
                        expr_labels,
                        expr_label_count)) {
                    return false;
                }
            }
            return true;
        }
    case AK_InterpolatedString:
        for (u32 i = node->a; i < node->b; ++i) {
            if (!sema_validate_loop_control(lexer,
                                            ast,
                                            i,
                                            loop_depth,
                                            expr_block_depth,
                                            expr_labels,
                                            expr_label_count)) {
                return false;
            }
        }
        return true;
    case AK_InterpPartExpr:
    case AK_Expression:
    case AK_Statement:
    case AK_Return:
    case AK_ReturnExpr:
    case AK_AddressOf:
    case AK_IntegerNegate:
    case AK_LogicalNot:
        return node->a == U32_MAX
                   ? true
                   : sema_validate_loop_control(lexer,
                                                ast,
                                                node->a,
                                                loop_depth,
                                                expr_block_depth,
                                                expr_labels,
                                                expr_label_count);
    case AK_StringConcat:
    case AK_IntegerPlus:
    case AK_IntegerMinus:
    case AK_IntegerMultiply:
    case AK_IntegerDivide:
    case AK_IntegerModulo:
    case AK_BitwiseAnd:
    case AK_BitwiseXor:
    case AK_BitwiseOr:
    case AK_Equal:
    case AK_NotEqual:
    case AK_Less:
    case AK_LessEqual:
    case AK_Greater:
    case AK_GreaterEqual:
    case AK_LogicalAnd:
    case AK_LogicalOr:
    case AK_Cast:
    case AK_RangeExclusive:
    case AK_RangeInclusive:
    case AK_AnnotatedValue:
        return sema_validate_loop_control(lexer,
                                          ast,
                                          node->a,
                                          loop_depth,
                                          expr_block_depth,
                                          expr_labels,
                                          expr_label_count) &&
               sema_validate_loop_control(lexer,
                                          ast,
                                          node->b,
                                          loop_depth,
                                          expr_block_depth,
                                          expr_labels,
                                          expr_label_count);
    case AK_Bind:
    case AK_Variable:
    case AK_Assign:
        return sema_validate_loop_control(lexer,
                                          ast,
                                          node->b,
                                          loop_depth,
                                          expr_block_depth,
                                          expr_labels,
                                          expr_label_count);
    default:
        return true;
    }
}

internal bool sema_validate_all_loop_control(const Lexer* lexer, const Ast* ast)
{
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        u32 expr_labels[SEMA_MAX_CONTROL_LABELS] = {0};
        if (ast->nodes[i].kind == AK_FnDef &&
            !sema_validate_loop_control(lexer, ast, i, 0, 0, expr_labels, 0)) {
            return false;
        }
    }
    return true;
}

//------------------------------------------------------------------------------
// Analyse the AST into compact declaration and resolution tables.

bool sema_analyse(const Lexer*           lexer,
                  Ast*                   ast,
                  const FrontEndOptions* options,
                  Sema*                  out_sema)
{
    Sema            sema = {0};
    FrontEndOptions effective_options =
        options ? *options : (FrontEndOptions){0};
    if (options == NULL) {
        effective_options.require_entry_point = true;
    }
    sema.program = effective_options.program;

    // Seed commonly-used built-in types so later materialisation can always
    // canonicalise untyped numeric literals to runtime storage types.
    sema_builtin_type(&sema, STK_Void);
    sema_builtin_type(&sema, STK_UntypedInteger);
    sema_builtin_type(&sema, STK_UntypedFloat);
    sema_builtin_type(&sema, STK_String);
    sema_builtin_type(&sema, STK_Bool);
    sema_builtin_type(&sema, STK_I32);
    sema_builtin_type(&sema, STK_F64);

    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        array_push(sema.node_decl_indices, sema_no_decl());
        array_push(sema.node_local_indices, sema_no_local());
        array_push(sema.node_scope_indices, sema_no_scope());
        array_push(sema.node_lowered_symbol_handles, U32_MAX);
        array_push(sema.node_type_indices, sema_no_type());
        array_push(sema.node_is_type_expr, false);
        array_push(sema.node_const_known, false);
        array_push(sema.node_const_values, 0);
    }
    for (u32 i = 0; i < array_count(ast->on_branches); ++i) {
        array_push(sema.on_branch_local_indices, sema_no_local());
    }
    for (u32 i = 0; i < array_count(ast->patterns); ++i) {
        array_push(sema.pattern_local_indices, sema_no_local());
    }

    if (!sema_collect_decls(lexer, ast, &effective_options, &sema)) {
        sema_done(&sema);
        return false;
    }
    if (!sema_classify_type_aliases(lexer, ast, &sema)) {
        sema_done(&sema);
        return false;
    }
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        if (ast->nodes[i].kind == AK_AnnotatedValue ||
            ast->nodes[i].kind == AK_ZeroInit) {
            sema_mark_type_expr_nodes(ast, &sema, ast->nodes[i].a);
        } else if (ast->nodes[i].kind == AK_Cast) {
            sema_mark_type_expr_nodes(ast, &sema, ast->nodes[i].b);
        }
    }
    for (u32 i = 0; i < array_count(sema.decls); ++i) {
        if (sema.decls[i].kind == SK_TypeAlias &&
            sema.decls[i].value_node_index != sema_no_decl()) {
            sema_mark_type_expr_nodes(
                ast, &sema, sema.decls[i].value_node_index);
        }
        if (sema.decls[i].type_node_index != sema_no_type()) {
            sema_mark_type_expr_nodes(
                ast, &sema, sema.decls[i].type_node_index);
        }
    }
    if (!sema_collect_top_level_uses(lexer, ast, &effective_options, &sema)) {
        sema_done(&sema);
        return false;
    }
    if (!sema_resolve_symbol_refs(lexer, ast, &sema)) {
        sema_done(&sema);
        return false;
    }
    if (!sema_validate_all_loop_control(lexer, ast)) {
        sema_done(&sema);
        return false;
    }
    sema_collect_deps(ast, &sema, &sema);
    if (!sema_order_decls(lexer, ast, &sema)) {
        sema_done(&sema);
        return false;
    }
    if (!sema_assign_decl_types(lexer, ast, &sema)) {
        sema_done(&sema);
        return false;
    }
    if (!sema_assign_local_types(lexer, ast, &sema)) {
        sema_done(&sema);
        return false;
    }
    if (effective_options.require_entry_point &&
        !sema_validate_entry_point(lexer, ast, &sema)) {
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
    array_free(sema->types);
    array_free(sema->type_param_types);
    array_free(sema->type_param_symbols);
    array_free(sema->decls);
    array_free(sema->locals);
    array_free(sema->scopes);
    array_free(sema->deps);
    array_free(sema->ordered_decl_indices);
    array_free(sema->node_decl_indices);
    array_free(sema->node_local_indices);
    array_free(sema->node_scope_indices);
    array_free(sema->node_lowered_symbol_handles);
    array_free(sema->node_type_indices);
    array_free(sema->on_branch_local_indices);
    array_free(sema->pattern_local_indices);
    array_free(sema->node_is_type_expr);
    array_free(sema->node_const_known);
    array_free(sema->node_const_values);
    *sema = (Sema){0};
}

//------------------------------------------------------------------------------
