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

internal cstr sema_cstr_from_string(Arena* arena, string value)
{
    char* result = arena_alloc(arena, value.count + 1);
    memcpy(result, value.data, value.count);
    result[value.count] = '\0';
    return result;
}

internal bool sema_path_is_absolute(string path)
{
    if (path.count == 0) {
        return false;
    }
    if (path.data[0] == '/' || path.data[0] == '\\') {
        return true;
    }
    return path.count >= 3 && path.data[1] == ':' &&
           (path.data[2] == '/' || path.data[2] == '\\');
}

internal cstr sema_resolve_source_relative_path(Arena* arena,
                                                string source_path,
                                                string relative_path)
{
    cstr path = sema_cstr_from_string(arena, relative_path);
    if (sema_path_is_absolute(relative_path)) {
        return path;
    }

    cstr source = module_source_file_path(arena,
                                          (NerdSource){
                                              .source_path = source_path,
                                          });
    if (source == NULL) {
        source = sema_cstr_from_string(arena, source_path);
    }
    return path_join(arena, path_dirname(arena, source), path);
}

//------------------------------------------------------------------------------
// Predeclare the current built-in runtime functions.

typedef struct {
    const Lexer*          lexer;
    const Ast*            ast;
    Sema*                 sema;
    const AstFnSignature* signature;
    bool                  imported;
} SemaKnownCallSignature;

internal SemaTypeSubstitution g_sema_type_subst = {0};

internal u32  sema_builtin_type(Sema* sema, SemaTypeKind kind);
internal u32  sema_type_index_for_name(Sema* sema, string name);
internal u32  sema_find_decl(const Sema* sema, u32 symbol_handle);
internal bool sema_resolve_type_node(const Lexer* lexer,
                                     const Ast*   ast,
                                     Sema*        sema,
                                     u32          node_index,
                                     u32*         out_type_index);
internal bool sema_resolve_type_node_ex(const Lexer*         lexer,
                                        const Ast*           ast,
                                        Sema*                sema,
                                        u32                  node_index,
                                        SemaTypeSubstitution subst,
                                        u32*                 out_type_index);
internal bool sema_try_resolve_type_symbol(const Lexer* lexer,
                                           const Ast*   ast,
                                           Sema*        sema,
                                           u32          node_index,
                                           u32*         out_type_index);
internal bool sema_try_resolve_qualified_type_node(const Lexer* lexer,
                                                   const Ast*   ast,
                                                   Sema*        sema,
                                                   u32          node_index,
                                                   u32*         out_type_index);
internal u32  sema_ast_enclosing_function_start_node(const Ast* ast,
                                                     u32        node_index);
internal bool
sema_type_matches(const Sema* sema, u32 expected_type, u32 actual_type);
internal const AstCastInfo* sema_cast_info(const Ast* ast, const AstNode* node);
internal ErrorSpan sema_node_span(const Lexer* lexer, const AstNode* node);
internal ErrorSpan sema_decl_span(const Lexer*    lexer,
                                  const Ast*      ast,
                                  const SemaDecl* decl);

internal u32  sema_enclosing_function_return_type(const Sema* sema,
                                                  u32         node_index);
internal u32  sema_ast_enclosing_function_return_type(const Lexer* lexer,
                                                      const Ast*   ast,
                                                      Sema*        sema,
                                                      u32          node_index);
internal bool sema_module_is_core(const ProgramInfo* program, u32 module_index);
internal bool sema_decl_is_from_core(const Sema* sema, const SemaDecl* decl);

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

internal u32 sema_reserve_type(Sema* sema, SemaType type)
{
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
        array_push(sema->type_param_values, 0);
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
        array_push(sema->type_param_values, 0);
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
        array_push(sema->type_param_values, 0);
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

internal u32 sema_add_dynamic_array_type(Sema* sema, u32 item_type)
{
    return sema_add_type(sema,
                         (SemaType){
                             .kind             = STK_DynamicArray,
                             .param_count      = 0,
                             .first_param_type = item_type,
                             .return_type      = sema_no_type(),
                         });
}

internal u32 sema_add_box_type(Sema* sema, u32 item_type)
{
    return sema_add_type(sema,
                         (SemaType){
                             .kind             = STK_Box,
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
        array_push(sema->type_param_values, 0);
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
                                    const i64* discriminants,
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
            if (sema->type_param_values[existing->first_param_type + j] !=
                discriminants[j]) {
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
        array_push(sema->type_param_values, discriminants[i]);
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
                                Array(i64) discriminants,
                                u32 count)
{
    Array(u32) symbols = NULL;
    for (u32 i = 0; i < count; ++i) {
        array_push(symbols, variants[i].symbol_handle);
    }
    u32 type_index = sema_add_enum_type_raw(
        sema, symbols, payload_types, discriminants, count);
    array_free(symbols);
    return type_index;
}

internal u32 sema_add_optional_type(const Lexer* lexer,
                                    Sema*        sema,
                                    u32          payload_type)
{
    InternAddResult ignored    = {0};
    u32             variants[] = {
        lex_add_symbol((Lexer*)lexer, s("__optional_absent"), &ignored),
        lex_add_symbol((Lexer*)lexer, s("__optional_present"), &ignored),
    };
    u32 payloads[] = {sema_no_type(), payload_type};
    i64 tags[]     = {0, 1};
    u32 type_index = sema_add_enum_type_raw(sema, variants, payloads, tags, 2);
    sema->types[type_index].flags |= STF_Optional;
    return type_index;
}

internal u32 sema_add_result_type(const Lexer* lexer,
                                  Sema*        sema,
                                  u32          success_type,
                                  u32          error_type)
{
    InternAddResult ignored    = {0};
    u32             variants[] = {
        lex_add_symbol((Lexer*)lexer, s("__result_success"), &ignored),
        lex_add_symbol((Lexer*)lexer, s("__result_error"), &ignored),
    };
    u32 payloads[] = {success_type, error_type};
    i64 tags[]     = {0, 1};
    u32 type_index = sema_add_enum_type_raw(sema, variants, payloads, tags, 2);
    sema->types[type_index].flags |= STF_Result;
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
    case STK_Never:
    case STK_UntypedInteger:
    case STK_UntypedFloat:
    case STK_Nil:
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
    case STK_Arena:
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

    case STK_DynamicArray:
        return sema_add_dynamic_array_type(
            dst_sema,
            sema_import_type(dst_lexer,
                             dst_sema,
                             src_lexer,
                             src_sema,
                             src_type->first_param_type));

    case STK_Box:
        return sema_add_box_type(dst_sema,
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
            Array(i64) discriminants = NULL;
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
                array_push(
                    discriminants,
                    src_sema
                        ->type_param_values[src_type->first_param_type + i]);
            }
            u32 imported = sema_add_enum_type_raw(dst_sema,
                                                  variants,
                                                  payload_types,
                                                  discriminants,
                                                  (u32)array_count(variants));
            dst_sema->types[imported].flags |= src_type->flags;
            array_free(variants);
            array_free(payload_types);
            array_free(discriminants);
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

internal bool sema_type_is_pointer_sized_integer(const Sema* sema,
                                                 u32         type_index)
{
    if (type_index == sema_no_type()) {
        return false;
    }
    return sema->types[type_index].kind == STK_Usize ||
           sema->types[type_index].kind == STK_Isize;
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

    if (sema->types[type_index].kind == STK_DynamicArray) {
        const SemaType* dynarray = &sema->types[type_index];
        return sema_add_dynamic_array_type(
            (Sema*)sema,
            sema_materialise_type(sema, dynarray->first_param_type));
    }

    if (sema->types[type_index].kind == STK_Box) {
        const SemaType* box = &sema->types[type_index];
        return sema_add_box_type(
            (Sema*)sema, sema_materialise_type(sema, box->first_param_type));
    }

    if (sema->types[type_index].kind == STK_Pointer) {
        return type_index;
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
        Array(i64) discriminants  = NULL;
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
            array_push(
                discriminants,
                sema->type_param_values[enum_type->first_param_type + i]);
        }
        u32 materialised = sema_add_enum_type_raw((Sema*)sema,
                                                  variants,
                                                  payload_types,
                                                  discriminants,
                                                  enum_type->param_count);
        array_free(variants);
        array_free(payload_types);
        array_free(discriminants);
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

internal bool sema_type_has_dot_members(const Sema* sema, u32 type_index)
{
    if (type_index == sema_no_type() ||
        type_index >= array_count(sema->types)) {
        return false;
    }

    switch (sema->types[type_index].kind) {
    case STK_Tuple:
    case STK_Array:
    case STK_Slice:
    case STK_String:
    case STK_DynamicArray:
    case STK_Box:
    case STK_Plex:
    case STK_Union:
        return true;
    default:
        return false;
    }
}

internal u32 sema_member_target_type(const Sema* sema, u32 type_index)
{
    if (type_index == sema_no_type() ||
        type_index >= array_count(sema->types)) {
        return type_index;
    }

    u32 result = type_index;
    if (result != sema_no_type() && result < array_count(sema->types) &&
        sema->types[result].kind == STK_Box) {
        result =
            sema_materialise_type(sema, sema->types[result].first_param_type);
    }
    while (result != sema_no_type() && result < array_count(sema->types) &&
           sema->types[result].kind == STK_Pointer) {
        u32 pointee_type =
            sema_materialise_type(sema, sema->types[result].first_param_type);
        if (pointee_type != sema_no_type() &&
            pointee_type < array_count(sema->types) &&
            sema->types[pointee_type].kind == STK_Box) {
            pointee_type = sema_materialise_type(
                sema, sema->types[pointee_type].first_param_type);
        }
        if (pointee_type == sema_no_type() ||
            pointee_type >= array_count(sema->types) ||
            (sema->types[pointee_type].kind != STK_Pointer &&
             sema->types[pointee_type].kind != STK_Box &&
             !sema_type_has_dot_members(sema, pointee_type))) {
            break;
        }
        result = pointee_type;
    }
    return result;
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
    if (type->kind == STK_Plex || type->kind == STK_Union ||
        type->kind == STK_Enum) {
        for (u32 i = 0; i < array_count(sema->decls); ++i) {
            const SemaDecl* decl = &sema->decls[i];
            if (decl->kind == SK_TypeAlias && decl->type_index == type_index) {
                return lex_symbol(lexer, decl->symbol_handle);
            }
        }
    }

    switch (type->kind) {
    case STK_Void:
        return s("void");
    case STK_Never:
        return s("!");
    case STK_UntypedInteger:
        return s("untyped integer");
    case STK_UntypedFloat:
        return s("untyped float");
    case STK_Nil:
        return s("nil");
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
    case STK_Arena:
        return s("arena");
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
    case STK_DynamicArray:
        {
            StringBuilder sb = {0};
            sb_init(&sb, arena);
            sb_append_cstr(&sb, "[..]");
            sb_append_string(
                &sb,
                sema_type_name(lexer, sema, arena, type->first_param_type));
            return sb_to_string(&sb);
        }
    case STK_Box:
        {
            StringBuilder sb = {0};
            sb_init(&sb, arena);
            sb_append_cstr(&sb, "box[");
            sb_append_string(
                &sb,
                sema_type_name(lexer, sema, arena, type->first_param_type));
            sb_append_char(&sb, ']');
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
            if (type->flags & STF_Optional) {
                sb_append_char(&sb, '?');
                sb_append_string(
                    &sb,
                    sema_type_name(
                        lexer,
                        sema,
                        arena,
                        sema->type_param_types[type->first_param_type + 1]));
                return sb_to_string(&sb);
            }
            if (type->flags & STF_Result) {
                sb_append_string(
                    &sb,
                    sema_type_name(
                        lexer,
                        sema,
                        arena,
                        sema->type_param_types[type->first_param_type]));
                sb_append_char(&sb, '\\');
                sb_append_string(
                    &sb,
                    sema_type_name(
                        lexer,
                        sema,
                        arena,
                        sema->type_param_types[type->first_param_type + 1]));
                return sb_to_string(&sb);
            }
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
                i64 discriminant =
                    sema->type_param_values[type->first_param_type + i];
                bool show_discriminant =
                    i == 0
                        ? discriminant != 0
                        : discriminant !=
                              sema->type_param_values[type->first_param_type +
                                                      i - 1] +
                                  1;
                if (show_discriminant) {
                    sb_format(&sb, " = %lld", discriminant);
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

internal u32 sema_unwrap_expr_node(const Ast* ast, u32 node_index)
{
    while (node_index < array_count(ast->nodes) &&
           (ast->nodes[node_index].kind == AK_Expression ||
            ast->nodes[node_index].kind == AK_Statement)) {
        node_index = ast->nodes[node_index].a;
    }
    return node_index;
}

internal u32 sema_signature_required_param_count(
    const Ast* ast, const AstFnSignature* signature)
{
    for (u32 i = 0; i < signature->param_count; ++i) {
        const AstParam* param = &ast->params[signature->first_param + i];
        if (param->default_node_index != U32_MAX) {
            return i;
        }
    }
    return signature->param_count;
}

internal bool sema_call_arg_value_node(const Lexer*    arg_lexer,
                                       const Ast*      ast,
                                       const Lexer*    param_lexer,
                                       const AstParam* param,
                                       u32             arg_node,
                                       u32*            out_value_node)
{
    const AstNode* arg = &ast->nodes[arg_node];
    if (arg->kind != AK_Assign) {
        *out_value_node = arg_node;
        return true;
    }

    const AstNode* target = &ast->nodes[arg->a];
    if (target->kind != AK_SymbolRef || param == NULL ||
        param->symbol_handle == U32_MAX ||
        !string_eq(lex_symbol(arg_lexer, target->a),
                   lex_symbol(param_lexer, param->symbol_handle))) {
        return error_0340_named_argument_position(
            arg_lexer->source,
            sema_node_span(arg_lexer, target),
            param != NULL && param->symbol_handle != U32_MAX
                ? lex_symbol(param_lexer, param->symbol_handle)
                : s("positional argument"),
            target->kind == AK_SymbolRef ? lex_symbol(arg_lexer, target->a)
                                         : s("named argument"));
    }

    *out_value_node = arg->b;
    return true;
}

internal bool sema_node_is_named_call_arg(const Ast* ast, u32 node_index)
{
    if (node_index >= array_count(ast->nodes) ||
        ast->nodes[node_index].kind != AK_Assign) {
        return false;
    }

    for (u32 call_index = 0; call_index < array_count(ast->calls);
         ++call_index) {
        const AstCallInfo* call = &ast->calls[call_index];
        for (u32 i = 0; i < call->arg_count; ++i) {
            if (ast->call_args[call->first_arg + i] == node_index) {
                return true;
            }
        }
    }

    return false;
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
internal bool      sema_infer_local_binding_type(const Lexer* lexer,
                                                 const Ast*   ast,
                                                 Sema*        sema,
                                                 u32          local_index,
                                                 u32*         out_type_index);
internal bool      sema_imported_decl_source(Sema*           sema,
                                             const SemaDecl* decl,
                                             const Lexer**   out_lexer,
                                             const Ast**     out_ast,
                                             Sema**          out_sema,
                                             u32*            out_decl_index);
internal const SemaMethod* sema_find_method_for_decl(const Sema* sema,
                                                     u32         decl_index);
internal void sema_import_method_for_decl(Lexer*       dst_lexer,
                                          Sema*        sema,
                                          const Lexer* source_lexer,
                                          const Sema*  source_sema,
                                          u32          source_decl_index,
                                          u32          dst_decl_index);
internal void sema_import_public_methods_from_module(Lexer* dst_lexer,
                                                     Sema*  sema,
                                                     const ModuleInfo* module,
                                                     u32 module_index);
internal bool
sema_import_implicit_core_method(Lexer* lexer, Sema* sema, u32 method_symbol);
internal bool
sema_type_matches(const Sema* sema, u32 expected_type, u32 actual_type);
internal bool sema_method_matches_trait_symbol(const Lexer*      lexer,
                                               const Lexer*      source_lexer,
                                               const Ast*        source_ast,
                                               const SemaMethod* source_method,
                                               u32               trait_symbol);
internal bool sema_resolve_node_refs(const Lexer* lexer,
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
    for (u32 i = 0; i < array_count(sema->decls); ++i) {
        const SemaDecl* decl = &sema->decls[i];
        if (decl->symbol_handle != symbol_handle) {
            continue;
        }
        bool same_import = decl->import_module_index == import_module_index &&
                           decl->import_decl_index == import_decl_index;
        bool both_local  = decl->import_module_index == sema_no_decl() &&
                           import_module_index == sema_no_decl();
        if (same_import || both_local) {
            return i;
        }
    }

    SemaDeclKind kind = SK_Constant;
    if (import_decl_kind == SK_TypeAlias) {
        kind = SK_TypeAlias;
    } else if (import_decl_kind == SK_GenericTypeAlias) {
        kind = SK_GenericTypeAlias;
    } else if (import_decl_kind == SK_Variable) {
        kind = SK_Variable;
    } else if (import_decl_kind == SK_GenericFunction) {
        kind = SK_GenericFunction;
    } else if (import_decl_kind == SK_Trait) {
        kind = SK_Trait;
    } else if (import_decl_kind == SK_FfiFunction) {
        kind = SK_FfiFunction;
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

internal u32 sema_imported_export_lowered_symbol_handle(Lexer*      dst_lexer,
                                                        const Sema* sema,
                                                        u32 module_index,
                                                        u32 decl_index,
                                                        u32 fallback)
{
    if (sema->program == NULL ||
        module_index >= array_count(sema->program->modules)) {
        return fallback;
    }

    const ModuleInfo* module = &sema->program->modules[module_index];
    if (decl_index >= array_count(module->front_end.sema.decls)) {
        return fallback;
    }

    const SemaDecl* decl = &module->front_end.sema.decls[decl_index];
    if (decl->value_node_index != sema_no_decl() &&
        decl->value_node_index <
            array_count(module->front_end.sema.node_lowered_symbol_handles)) {
        u32 lowered = module->front_end.sema
                          .node_lowered_symbol_handles[decl->value_node_index];
        if (lowered != U32_MAX) {
            return sema_import_symbol_handle(
                dst_lexer, &module->front_end.lexer, lowered);
        }
    }

    return fallback;
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

        SemaDeclKind import_decl_kind = SK_Constant;
        if (sema->program != NULL &&
            import_module_index < array_count(sema->program->modules) &&
            import_decl_index <
                array_count(sema->program->modules[import_module_index]
                                .front_end.sema.decls)) {
            import_decl_kind = sema->program->modules[import_module_index]
                                   .front_end.sema.decls[import_decl_index]
                                   .kind;
        }

        u32 imported_decl = sema_ensure_module_export_decl(sema,
                                                           symbol,
                                                           type,
                                                           import_decl_kind,
                                                           import_module_index,
                                                           import_decl_index);
        if (sema->program != NULL &&
            import_module_index < array_count(sema->program->modules) &&
            import_decl_index != sema_no_decl()) {
            const ModuleInfo* import_module =
                &sema->program->modules[import_module_index];
            sema_import_method_for_decl((Lexer*)lexer,
                                        sema,
                                        &import_module->front_end.lexer,
                                        &import_module->front_end.sema,
                                        import_decl_index,
                                        imported_decl);
        }
        u32 lowered_symbol_handle =
            sema_imported_export_lowered_symbol_handle((Lexer*)lexer,
                                                       sema,
                                                       import_module_index,
                                                       import_decl_index,
                                                       symbol);

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
                .lowered_symbol_handle = lowered_symbol_handle,
            });
        sema->scopes[scope_index].local_count++;
    }

    if (sema->program != NULL &&
        module->return_type < array_count(sema->program->modules)) {
        sema_import_public_methods_from_module(
            (Lexer*)lexer,
            sema,
            &sema->program->modules[module->return_type],
            module->return_type);
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

        u32 imported_decl = sema_ensure_module_export_decl(sema,
                                                           symbol,
                                                           type,
                                                           import_decl_kind,
                                                           import_module_index,
                                                           import_decl_index);
        if (sema->program != NULL &&
            import_module_index < array_count(sema->program->modules) &&
            import_decl_index != sema_no_decl()) {
            const ModuleInfo* import_module =
                &sema->program->modules[import_module_index];
            sema_import_method_for_decl((Lexer*)lexer,
                                        sema,
                                        &import_module->front_end.lexer,
                                        &import_module->front_end.sema,
                                        import_decl_index,
                                        imported_decl);
        }
    }

    if (sema->program != NULL &&
        module->return_type < array_count(sema->program->modules)) {
        sema_import_public_methods_from_module(
            (Lexer*)lexer,
            sema,
            &sema->program->modules[module->return_type],
            module->return_type);
    }

    return true;
}

internal bool sema_import_implicit_core_decls(const Lexer* lexer, Sema* sema)
{
    if (sema == NULL || sema->program == NULL ||
        sema_module_is_core(sema->program, sema->current_module_index)) {
        return true;
    }

    u32 core_module_index = U32_MAX;
    for (u32 i = 0; i < array_count(sema->program->modules); ++i) {
        if (sema_module_is_core(sema->program, i)) {
            core_module_index = i;
            break;
        }
    }
    if (core_module_index == U32_MAX) {
        return true;
    }

    const ModuleInfo* core_module = &sema->program->modules[core_module_index];
    const Lexer*      core_lexer  = &core_module->front_end.lexer;
    const Sema*       core_sema   = &core_module->front_end.sema;
    for (u32 i = 0; i < array_count(core_module->export_decl_indices); ++i) {
        u32 import_decl_index = core_module->export_decl_indices[i];
        if (import_decl_index >= array_count(core_sema->decls)) {
            continue;
        }

        const SemaDecl* export_decl = &core_sema->decls[import_decl_index];
        if (sema_find_method_for_decl(core_sema, import_decl_index) != NULL) {
            continue;
        }
        if (export_decl->symbol_handle == U32_MAX ||
            sema_find_symbol_handle_by_name(
                lexer, lex_symbol(core_lexer, export_decl->symbol_handle)) ==
                sema_no_decl()) {
            continue;
        }

        string name = lex_symbol(core_lexer, export_decl->symbol_handle);
        if (!string_eq_cstr(name, "Display") && !string_eq_cstr(name, "Eq") &&
            !string_eq_cstr(name, "Order") &&
            !string_eq_cstr(name, "Default") &&
            !string_eq_cstr(name, "Iterator") &&
            !string_eq_cstr(name, "arena") &&
            !string_eq_cstr(name, "temp_arena") &&
            !string_eq_cstr(name, "pr") && !string_eq_cstr(name, "prn") &&
            !string_eq_cstr(name, "epr") && !string_eq_cstr(name, "eprn")) {
            continue;
        }

        u32 symbol = sema_import_symbol_handle(
            (Lexer*)lexer, core_lexer, export_decl->symbol_handle);
        if (sema_find_decl(sema, symbol) != sema_no_decl()) {
            continue;
        }

        u32 type          = sema_import_type((Lexer*)lexer,
                                             sema,
                                             core_lexer,
                                             core_sema,
                                             export_decl->type_index);
        u32 imported_decl = sema_ensure_module_export_decl(sema,
                                                           symbol,
                                                           type,
                                                           export_decl->kind,
                                                           core_module_index,
                                                           import_decl_index);
        (void)imported_decl;
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
    case STK_Never:
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
    case STK_Arena:
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
internal i64  sema_enum_variant_discriminant(const Sema* sema,
                                             u32         enum_type,
                                             u32         variant_index);
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
    case AK_FloatLiteral:
    case AK_StringLiteral:
    case AK_BuiltinMacro:
    case AK_BoolLiteral:
    case AK_NilLiteral:
    case AK_EnumVariant:
        return true;
    case AK_InterpolatedString:
        for (u32 i = node->a; i < node->b; ++i) {
            const AstNode* part = &ast->nodes[i];
            if (part->kind == AK_StringLiteral) {
                continue;
            }
            if (part->kind != AK_InterpPartExpr ||
                !sema_expr_is_constantish(ast, sema, part->a)) {
                return false;
            }
        }
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
    case AK_BitwiseNot:
    case AK_Expression:
    case AK_Statement:
        return sema_expr_is_constantish(ast, sema, node->a);
    case AK_IntegerPlus:
    case AK_IntegerMinus:
    case AK_IntegerMultiply:
    case AK_IntegerDivide:
    case AK_IntegerModulo:
    case AK_BitwiseAnd:
    case AK_BitwiseXor:
    case AK_BitwiseOr:
    case AK_ShiftLeft:
    case AK_ShiftRight:
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
    case AK_NilLiteral:
        *out_value = 0;
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
    case AK_BitwiseNot:
        if (!sema_try_eval_integer_constant(
                lexer, ast, sema, node->a, out_value)) {
            return false;
        }
        *out_value = node->kind == AK_BitwiseNot ? ~*out_value : -*out_value;
        return true;
    case AK_IntegerPlus:
    case AK_IntegerMinus:
    case AK_IntegerMultiply:
    case AK_IntegerDivide:
    case AK_IntegerModulo:
    case AK_BitwiseAnd:
    case AK_BitwiseXor:
    case AK_BitwiseOr:
    case AK_ShiftLeft:
    case AK_ShiftRight:
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
            case AK_BitwiseAnd:
                *out_value = lhs & rhs;
                return true;
            case AK_BitwiseXor:
                *out_value = lhs ^ rhs;
                return true;
            case AK_BitwiseOr:
                *out_value = lhs | rhs;
                return true;
            case AK_ShiftLeft:
                if (rhs < 0 || rhs >= 64) {
                    return false;
                }
                *out_value = lhs << rhs;
                return true;
            case AK_ShiftRight:
                if (rhs < 0 || rhs >= 64) {
                    return false;
                }
                *out_value = lhs >> rhs;
                return true;
            default:
                return false;
            }
        }
    case AK_Field:
        {
            u32 qualified_type = sema_no_type();
            if (!sema_try_resolve_type_symbol(
                    lexer, ast, (Sema*)sema, node->a, &qualified_type) ||
                qualified_type == sema_no_type() ||
                sema->types[qualified_type].kind != STK_Enum) {
                return false;
            }
            u32 variant =
                sema_enum_variant_index(sema, qualified_type, node->b);
            if (variant == U32_MAX) {
                return false;
            }
            *out_value =
                sema_enum_variant_discriminant(sema, qualified_type, variant);
            return true;
        }
    case AK_RangeExclusive:
    case AK_RangeInclusive:
        return false;
    case AK_SymbolRef:
        if (node_index < array_count(sema->node_local_indices)) {
            u32 local_index = sema->node_local_indices[node_index];
            if (local_index != sema_no_local() &&
                sema->locals[local_index].kind == SLK_Constant &&
                sema->locals[local_index].value_node_index != sema_no_decl()) {
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
                const SemaDecl* decl = &sema->decls[decl_index];
                if (decl->value_node_index != sema_no_decl()) {
                    return sema_try_eval_integer_constant(
                        lexer, ast, sema, decl->value_node_index, out_value);
                }

                const Lexer* source_lexer      = NULL;
                const Ast*   source_ast        = NULL;
                Sema*        source_sema       = NULL;
                u32          source_decl_index = sema_no_decl();
                if (sema_imported_decl_source((Sema*)sema,
                                              decl,
                                              &source_lexer,
                                              &source_ast,
                                              &source_sema,
                                              &source_decl_index)) {
                    const SemaDecl* source_decl =
                        &source_sema->decls[source_decl_index];
                    if (source_decl->value_node_index != sema_no_decl()) {
                        return sema_try_eval_integer_constant(
                            source_lexer,
                            source_ast,
                            source_sema,
                            source_decl->value_node_index,
                            out_value);
                    }
                }
            }
        }
        {
            u32 decl_index = sema_find_decl((Sema*)sema, node->a);
            if (decl_index != sema_no_decl() &&
                sema->decls[decl_index].kind == SK_Constant) {
                const SemaDecl* decl = &sema->decls[decl_index];
                if (decl->value_node_index != sema_no_decl()) {
                    return sema_try_eval_integer_constant(
                        lexer, ast, sema, decl->value_node_index, out_value);
                }

                const Lexer* source_lexer      = NULL;
                const Ast*   source_ast        = NULL;
                Sema*        source_sema       = NULL;
                u32          source_decl_index = sema_no_decl();
                if (sema_imported_decl_source((Sema*)sema,
                                              decl,
                                              &source_lexer,
                                              &source_ast,
                                              &source_sema,
                                              &source_decl_index)) {
                    const SemaDecl* source_decl =
                        &source_sema->decls[source_decl_index];
                    if (source_decl->value_node_index != sema_no_decl()) {
                        return sema_try_eval_integer_constant(
                            source_lexer,
                            source_ast,
                            source_sema,
                            source_decl->value_node_index,
                            out_value);
                    }
                }
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

internal ErrorSpan sema_enum_variant_discriminant_span(
    const Lexer* lexer, const Ast* ast, const AstEnumVariant* variant)
{
    if (variant->value_node_index != U32_MAX) {
        const AstNode* value_node = &ast->nodes[variant->value_node_index];
        if (value_node->kind == AK_Expression) {
            value_node = &ast->nodes[value_node->a];
        }
        return sema_node_span(lexer, value_node);
    }
    return sema_token_span(lexer, variant->token_index);
}

internal bool
sema_check_enum_discriminant_unique(const Lexer*          lexer,
                                    const Ast*            ast,
                                    const AstEnumVariant* variants,
                                    Array(i64) discriminants,
                                    u32 variant_index,
                                    i64 discriminant)
{
    for (u32 i = 0; i < array_count(discriminants); ++i) {
        if (discriminants[i] != discriminant) {
            continue;
        }
        return error_0342_duplicate_enum_discriminant(
            lexer->source,
            sema_enum_variant_discriminant_span(
                lexer, ast, &variants[variant_index]),
            discriminant,
            sema_enum_variant_discriminant_span(lexer, ast, &variants[i]));
    }
    return true;
}

internal bool sema_check_enum_variant_name_unique(
    const Lexer* lexer, const AstEnumVariant* variants, u32 variant_index)
{
    const AstEnumVariant* variant = &variants[variant_index];
    for (u32 i = 0; i < variant_index; ++i) {
        if (variants[i].symbol_handle != variant->symbol_handle) {
            continue;
        }
        return error_0343_duplicate_enum_variant(
            lexer->source,
            sema_token_span(lexer, variant->token_index),
            lex_symbol(lexer, variant->symbol_handle),
            sema_token_span(lexer, variants[i].token_index));
    }
    return true;
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

internal bool sema_module_is_core(const ProgramInfo* program, u32 module_index)
{
    return program != NULL && module_index < array_count(program->modules) &&
           string_eq_cstr(program->modules[module_index].qualified_name,
                          "core");
}

internal bool sema_decl_is_from_core(const Sema* sema, const SemaDecl* decl)
{
    if (sema == NULL || decl == NULL) {
        return false;
    }
    if (decl->import_module_index != sema_no_decl()) {
        return sema_module_is_core(sema->program, decl->import_module_index);
    }
    return sema_module_is_core(sema->program, sema->current_module_index);
}

internal u32 sema_find_core_trait_symbol(const Lexer* lexer,
                                         const Sema*  sema,
                                         string       name)
{
    u32 symbol = sema_find_symbol_handle_by_name(lexer, name);
    if (symbol == sema_no_decl()) {
        return sema_no_decl();
    }

    u32 decl_index = sema_find_decl(sema, symbol);
    if (decl_index == sema_no_decl() ||
        decl_index >= array_count(sema->decls)) {
        return sema_no_decl();
    }

    const SemaDecl* decl = &sema->decls[decl_index];
    return decl->kind == SK_Trait && sema_decl_is_from_core(sema, decl)
               ? symbol
               : sema_no_decl();
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

internal bool sema_type_contains_unboxed_type(const Sema* sema,
                                              u32         type_index,
                                              u32         target_type)
{
    if (type_index == sema_no_type()) {
        return false;
    }
    if (type_index == target_type) {
        return true;
    }

    const SemaType* type = &sema->types[type_index];
    switch (type->kind) {
    case STK_Array:
        return sema_type_contains_unboxed_type(
            sema, type->first_param_type, target_type);
    case STK_Tuple:
    case STK_Plex:
    case STK_Union:
    case STK_Enum:
        for (u32 i = 0; i < type->param_count; ++i) {
            if (sema_type_contains_unboxed_type(
                    sema,
                    sema->type_param_types[type->first_param_type + i],
                    target_type)) {
                return true;
            }
        }
        return false;
    default:
        return false;
    }
}

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

#if OS_WINDOWS
internal bool sema_ascii_eq_ci(string a, cstr b)
{
    usize i = 0;
    for (; i < a.count && b[i] != '\0'; ++i) {
        char ac = a.data[i];
        char bc = b[i];
        if (ac >= 'A' && ac <= 'Z') {
            ac = (char)(ac - 'A' + 'a');
        }
        if (bc >= 'A' && bc <= 'Z') {
            bc = (char)(bc - 'A' + 'a');
        }
        if (ac != bc) {
            return false;
        }
    }
    return i == a.count && b[i] == '\0';
}
#endif

internal bool sema_windows_library_should_validate(string library)
{
#if OS_WINDOWS
    return sema_ascii_eq_ci(library, "user32") ||
           sema_ascii_eq_ci(library, "user32.dll") ||
           sema_ascii_eq_ci(library, "kernel32") ||
           sema_ascii_eq_ci(library, "kernel32.dll") ||
           sema_ascii_eq_ci(library, "gdi32") ||
           sema_ascii_eq_ci(library, "gdi32.dll") ||
           sema_ascii_eq_ci(library, "shell32") ||
           sema_ascii_eq_ci(library, "shell32.dll") ||
           sema_ascii_eq_ci(library, "advapi32") ||
           sema_ascii_eq_ci(library, "advapi32.dll") ||
           sema_ascii_eq_ci(library, "comdlg32") ||
           sema_ascii_eq_ci(library, "comdlg32.dll") ||
           sema_ascii_eq_ci(library, "ole32") ||
           sema_ascii_eq_ci(library, "ole32.dll");
#else
    UNUSED(library);
    return false;
#endif
}

#if OS_WINDOWS
internal cstr sema_string_to_cstr(Arena* arena, string value)
{
    return sema_cstr_from_string(arena, value);
}
#endif

internal bool
sema_windows_ffi_symbol_exists(Arena* arena, string library, string symbol)
{
#if OS_WINDOWS
    bool has_extension = false;
    for (usize i = 0; i < library.count; ++i) {
        if (library.data[i] == '.') {
            has_extension = true;
            break;
        }
    }
    string dll_name =
        has_extension ? library
                      : string_format(arena, STRINGP ".dll", STRINGV(library));
    void* module = LoadLibraryA(sema_string_to_cstr(arena, dll_name));
    if (module == NULL) {
        return true;
    }
    void* proc = GetProcAddress(module, sema_string_to_cstr(arena, symbol));
    FreeLibrary(module);
    return proc != NULL;
#else
    UNUSED(arena);
    UNUSED(library);
    UNUSED(symbol);
    return true;
#endif
}

internal bool sema_ffi_library_literal(const Lexer* lexer,
                                       const Ast*   ast,
                                       u32          node_index,
                                       string*      out)
{
    if (node_index >= array_count(ast->nodes)) {
        return false;
    }
    const AstNode* node = &ast->nodes[node_index];
    if (node->kind == AK_StringLiteral) {
        *out = ast_get_string(lexer, node);
        return true;
    }
    if ((node->kind == AK_Expression || node->kind == AK_Statement) &&
        node->a < array_count(ast->nodes)) {
        return sema_ffi_library_literal(lexer, ast, node->a, out);
    }
    if (node->kind == AK_AnnotatedValue && node->b < array_count(ast->nodes)) {
        return sema_ffi_library_literal(lexer, ast, node->b, out);
    }
    return false;
}

internal u32 sema_enclosing_impl_node_index(const Ast* ast, u32 node_index)
{
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* owner = &ast->nodes[i];
        if (owner->kind != AK_Impl) {
            continue;
        }
        const AstImplInfo* impl = &ast->impls[owner->a];
        const AstNode*     body = &ast->nodes[impl->body_node_index];
        if (body->a <= node_index && node_index < body->b) {
            return i;
        }
    }
    return U32_MAX;
}

internal bool sema_node_is_inside_impl_body(const Ast* ast, u32 node_index)
{
    return sema_enclosing_impl_node_index(ast, node_index) != U32_MAX;
}

internal bool sema_node_is_inside_trait_body(const Ast* ast, u32 node_index)
{
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* owner = &ast->nodes[i];
        if (owner->kind != AK_Trait ||
            owner->a >= array_count(ast->trait_infos)) {
            continue;
        }
        const AstTraitInfo* trait = &ast->trait_infos[owner->a];
        if (trait->body_node_index >= array_count(ast->nodes)) {
            continue;
        }
        const AstNode* body = &ast->nodes[trait->body_node_index];
        if (body->a <= node_index && node_index < body->b) {
            return true;
        }
    }
    return false;
}

internal u32 sema_trait_self_alias_symbol(const Lexer*   lexer,
                                          const Ast*     ast,
                                          const AstNode* trait_node)
{
    if (trait_node->a < array_count(ast->trait_infos)) {
        const AstTraitInfo* trait = &ast->trait_infos[trait_node->a];
        if (trait->self_alias_symbol != U32_MAX) {
            return trait->self_alias_symbol;
        }
    }
    return sema_find_symbol_handle_by_name(lexer, s("Self"));
}

internal void sema_append_mangle_part(StringBuilder* sb, string value)
{
    for (usize i = 0; i < value.count; ++i) {
        u8   ch     = value.data[i];
        bool simple = (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
                      (ch >= '0' && ch <= '9') || ch == '_';
        sb_append_char(sb, simple ? (char)ch : '_');
    }
}

internal void sema_append_type_node_mangle(StringBuilder* sb,
                                           const Lexer*   lexer,
                                           const Ast*     ast,
                                           u32            node_index)
{
    if (node_index >= array_count(ast->nodes)) {
        sb_append_cstr(sb, "unknown");
        return;
    }

    const AstNode* node = &ast->nodes[node_index];
    while ((node->kind == AK_Expression || node->kind == AK_Statement) &&
           node->a < array_count(ast->nodes)) {
        node_index = node->a;
        node       = &ast->nodes[node_index];
    }

    switch (node->kind) {
    case AK_SymbolRef:
        sema_append_mangle_part(sb, lex_symbol(lexer, node->a));
        return;
    case AK_TypeApply:
        {
            if (node->a >= array_count(ast->type_applications)) {
                sb_append_cstr(sb, "apply");
                return;
            }
            const AstTypeApplyInfo* apply = &ast->type_applications[node->a];
            sema_append_type_node_mangle(
                sb, lexer, ast, apply->target_node_index);
            for (u32 i = 0; i < apply->arg_count; ++i) {
                sb_append_char(sb, '_');
                sema_append_type_node_mangle(
                    sb, lexer, ast, ast->tuple_items[apply->first_arg + i]);
            }
            return;
        }
    case AK_TypePointer:
        sb_append_cstr(sb, "ptr_");
        sema_append_type_node_mangle(sb, lexer, ast, node->a);
        return;
    case AK_TypeSlice:
        sb_append_cstr(sb, "slice_");
        sema_append_type_node_mangle(sb, lexer, ast, node->a);
        return;
    case AK_TypeDynamicArray:
        sb_append_cstr(sb, "array_");
        sema_append_type_node_mangle(sb, lexer, ast, node->b);
        return;
    case AK_TypeArray:
        sb_append_cstr(sb, "array_");
        sema_append_type_node_mangle(sb, lexer, ast, node->b);
        return;
    case AK_TypeTuple:
        sb_append_cstr(sb, "tuple");
        for (u32 i = 0; i < node->b; ++i) {
            sb_append_char(sb, '_');
            sema_append_type_node_mangle(
                sb, lexer, ast, ast->tuple_items[node->a + i]);
        }
        return;
    default:
        sb_append_cstr(sb, "type");
        return;
    }
}

internal u32 sema_mangle_method_symbol(const Lexer*       lexer,
                                       const Ast*         ast,
                                       const AstImplInfo* impl,
                                       u32                method_symbol)
{
    StringBuilder sb = {0};
    sb_init(&sb, &temp_arena);
    if (impl->trait_type_node_index != U32_MAX) {
        sb_append_cstr(&sb, "__trait_");
        sema_append_type_node_mangle(
            &sb, lexer, ast, impl->trait_type_node_index);
        sb_append_cstr(&sb, "_for_");
    } else {
        sb_append_cstr(&sb, "__impl_");
    }
    sema_append_type_node_mangle(&sb, lexer, ast, impl->target_type_node_index);
    sb_append_char(&sb, '_');
    sema_append_mangle_part(&sb, lex_symbol(lexer, method_symbol));

    string          mangled = sb_to_string(&sb);
    InternAddResult ignored = {0};
    return lex_add_symbol((Lexer*)lexer, mangled, &ignored);
}

internal u32 sema_type_node_generic_params_index(const Ast* ast, u32 node_index)
{
    const AstNode* node = &ast->nodes[node_index];
    if (node->kind == AK_TypePlex) {
        return ast->plex_types[node->a].generic_params_index;
    }
    if (node->kind == AK_TypeEnum) {
        return ast->enum_types[node->a].generic_params_index;
    }
    if (node->kind == AK_TypeFn) {
        return ast->fn_signatures[node->a].generic_params_index;
    }
    return U32_MAX;
}

internal u32 sema_unwrap_type_candidate_node(const Ast* ast, u32 node_index)
{
    while (node_index < array_count(ast->nodes) &&
           (ast->nodes[node_index].kind == AK_Expression ||
            ast->nodes[node_index].kind == AK_Statement)) {
        node_index = ast->nodes[node_index].a;
    }
    return node_index;
}

internal bool sema_type_syntax_contains_self(const Lexer* lexer,
                                             const Ast*   ast,
                                             u32          node_index)
{
    node_index = sema_unwrap_type_candidate_node(ast, node_index);
    if (node_index >= array_count(ast->nodes)) {
        return false;
    }

    const AstNode* node = &ast->nodes[node_index];
    switch (node->kind) {
    case AK_SymbolRef:
        return string_eq(lex_symbol(lexer, node->a), s("Self"));

    case AK_TypeApply:
        {
            if (node->a >= array_count(ast->type_applications)) {
                return false;
            }
            const AstTypeApplyInfo* apply = &ast->type_applications[node->a];
            if (sema_type_syntax_contains_self(
                    lexer, ast, apply->target_node_index)) {
                return true;
            }
            for (u32 i = 0; i < apply->arg_count; ++i) {
                if (sema_type_syntax_contains_self(
                        lexer, ast, ast->tuple_items[apply->first_arg + i])) {
                    return true;
                }
            }
            return false;
        }

    case AK_TypeTuple:
        for (u32 i = 0; i < node->b; ++i) {
            if (sema_type_syntax_contains_self(
                    lexer, ast, ast->tuple_items[node->a + i])) {
                return true;
            }
        }
        return false;

    case AK_TypeArray:
    case AK_TypeDynamicArray:
        return sema_type_syntax_contains_self(lexer, ast, node->b);

    case AK_TypeSlice:
    case AK_TypePointer:
    case AK_TypeOptional:
        return sema_type_syntax_contains_self(lexer, ast, node->a);

    case AK_TypeResult:
        return sema_type_syntax_contains_self(lexer, ast, node->a) ||
               sema_type_syntax_contains_self(lexer, ast, node->b);

    default:
        return false;
    }
}

internal bool sema_node_is_type_syntax(const Ast* ast, u32 node_index)
{
    node_index = sema_unwrap_type_candidate_node(ast, node_index);
    if (node_index >= array_count(ast->nodes)) {
        return false;
    }

    switch (ast->nodes[node_index].kind) {
    case AK_Field:
    case AK_TypeApply:
    case AK_TypeFn:
    case AK_TypeTuple:
    case AK_TypeArray:
    case AK_TypeSlice:
    case AK_TypeDynamicArray:
    case AK_TypePointer:
    case AK_TypePlex:
    case AK_TypeEnum:
        return true;
    default:
        return false;
    }
}

internal u32 sema_fn_def_generic_params_index(const Ast* ast, u32 fn_node_index)
{
    const AstNode* fn_def = &ast->nodes[fn_node_index];
    if (fn_def->kind != AK_FnDef) {
        return U32_MAX;
    }
    const AstNode* fn_start = &ast->nodes[fn_def->a];
    return ast->fn_signatures[fn_start->a].generic_params_index;
}

internal bool sema_imported_decl_source(Sema*           sema,
                                        const SemaDecl* decl,
                                        const Lexer**   out_lexer,
                                        const Ast**     out_ast,
                                        Sema**          out_sema,
                                        u32*            out_decl_index)
{
    if (decl->import_module_index == sema_no_decl() ||
        decl->import_decl_index == sema_no_decl() || sema->program == NULL ||
        decl->import_module_index >= array_count(sema->program->modules)) {
        return false;
    }

    ProgramInfo* program = (ProgramInfo*)sema->program;
    ModuleInfo*  module  = &program->modules[decl->import_module_index];
    if (decl->import_decl_index >= array_count(module->front_end.sema.decls)) {
        return false;
    }

    *out_lexer      = &module->front_end.lexer;
    *out_ast        = &module->front_end.ast;
    *out_sema       = &module->front_end.sema;
    *out_decl_index = decl->import_decl_index;
    return true;
}

internal bool
sema_known_call_signature_ex(const Lexer*            lexer,
                             const Ast*              ast,
                             Sema*                   sema,
                             u32                     callee_node_index,
                             u32                     depth,
                             SemaKnownCallSignature* out_signature)
{
    if (depth > 16) {
        return false;
    }

    callee_node_index     = sema_unwrap_expr_node(ast, callee_node_index);
    const AstNode* callee = &ast->nodes[callee_node_index];

    if (callee->kind != AK_SymbolRef) {
        return false;
    }

    u32 local_index = sema_no_local();
    if (callee_node_index < array_count(sema->node_local_indices)) {
        local_index = sema->node_local_indices[callee_node_index];
    }
    if (local_index != sema_no_local()) {
        const SemaLocal* local = &sema->locals[local_index];
        if (local->kind == SLK_Function &&
            local->value_node_index != sema_no_decl() &&
            ast->nodes[local->value_node_index].kind == AK_FnDef) {
            const AstNode* fn_start =
                &ast->nodes[ast->nodes[local->value_node_index].a];
            *out_signature = (SemaKnownCallSignature){
                .lexer     = lexer,
                .ast       = ast,
                .sema      = sema,
                .signature = &ast->fn_signatures[fn_start->a],
                .imported  = false,
            };
            return true;
        }

        if ((local->kind == SLK_Variable || local->kind == SLK_Constant ||
             local->kind == SLK_Function) &&
            local->value_node_index != sema_no_decl() &&
            sema_known_call_signature_ex(lexer,
                                         ast,
                                         sema,
                                         local->value_node_index,
                                         depth + 1,
                                         out_signature)) {
            return true;
        }

        u32 decl_index = sema_find_decl(sema, local->symbol_handle);
        if (decl_index != sema_no_decl()) {
            const SemaDecl* decl              = &sema->decls[decl_index];
            const Lexer*    source_lexer      = NULL;
            const Ast*      source_ast        = NULL;
            Sema*           source_sema       = NULL;
            u32             source_decl_index = sema_no_decl();
            if (depth == 0 && sema_imported_decl_source(sema,
                                                        decl,
                                                        &source_lexer,
                                                        &source_ast,
                                                        &source_sema,
                                                        &source_decl_index)) {
                const SemaDecl* source_decl =
                    &source_sema->decls[source_decl_index];
                if (source_decl->value_node_index != sema_no_decl() &&
                    source_ast->nodes[source_decl->value_node_index].kind ==
                        AK_FnDef) {
                    const AstNode* fn_start =
                        &source_ast
                             ->nodes[source_ast
                                         ->nodes[source_decl->value_node_index]
                                         .a];
                    *out_signature = (SemaKnownCallSignature){
                        .lexer     = source_lexer,
                        .ast       = source_ast,
                        .sema      = source_sema,
                        .signature = &source_ast->fn_signatures[fn_start->a],
                        .imported  = true,
                    };
                    return true;
                }
            }
        }
    }

    if (callee_node_index < array_count(sema->node_decl_indices)) {
        u32 decl_index = sema->node_decl_indices[callee_node_index];
        if (decl_index != sema_no_decl()) {
            const SemaDecl* decl = &sema->decls[decl_index];
            if (decl->kind == SK_Function &&
                decl->value_node_index != sema_no_decl() &&
                ast->nodes[decl->value_node_index].kind == AK_FnDef) {
                const AstNode* fn_start =
                    &ast->nodes[ast->nodes[decl->value_node_index].a];
                *out_signature = (SemaKnownCallSignature){
                    .lexer     = lexer,
                    .ast       = ast,
                    .sema      = sema,
                    .signature = &ast->fn_signatures[fn_start->a],
                    .imported  = false,
                };
                return true;
            }

            if ((decl->kind == SK_Constant || decl->kind == SK_Function) &&
                decl->value_node_index != sema_no_decl() &&
                sema_known_call_signature_ex(lexer,
                                             ast,
                                             sema,
                                             decl->value_node_index,
                                             depth + 1,
                                             out_signature)) {
                return true;
            }

            const Lexer* source_lexer      = NULL;
            const Ast*   source_ast        = NULL;
            Sema*        source_sema       = NULL;
            u32          source_decl_index = sema_no_decl();
            if (depth == 0 && sema_imported_decl_source(sema,
                                                        decl,
                                                        &source_lexer,
                                                        &source_ast,
                                                        &source_sema,
                                                        &source_decl_index)) {
                const SemaDecl* source_decl =
                    &source_sema->decls[source_decl_index];
                if (source_decl->value_node_index != sema_no_decl() &&
                    source_ast->nodes[source_decl->value_node_index].kind ==
                        AK_FnDef) {
                    const AstNode* fn_start =
                        &source_ast
                             ->nodes[source_ast
                                         ->nodes[source_decl->value_node_index]
                                         .a];
                    *out_signature = (SemaKnownCallSignature){
                        .lexer     = source_lexer,
                        .ast       = source_ast,
                        .sema      = source_sema,
                        .signature = &source_ast->fn_signatures[fn_start->a],
                        .imported  = true,
                    };
                    return true;
                }
            }
        }
    }

    return false;
}

internal bool sema_known_call_signature(const Lexer* lexer,
                                        const Ast*   ast,
                                        Sema*        sema,
                                        u32          callee_node_index,
                                        SemaKnownCallSignature* out_signature)
{
    return sema_known_call_signature_ex(
        lexer, ast, sema, callee_node_index, 0, out_signature);
}

internal const SemaMethod* sema_find_method_for_decl(const Sema* sema,
                                                     u32         decl_index)
{
    for (u32 i = 0; i < array_count(sema->methods); ++i) {
        if (sema->methods[i].decl_index == decl_index) {
            return &sema->methods[i];
        }
    }
    return NULL;
}

internal void sema_import_method_for_decl(Lexer*       dst_lexer,
                                          Sema*        sema,
                                          const Lexer* source_lexer,
                                          const Sema*  source_sema,
                                          u32          source_decl_index,
                                          u32          dst_decl_index)
{
    if (sema_find_method_for_decl(sema, dst_decl_index) != NULL) {
        return;
    }

    const SemaMethod* source_method =
        sema_find_method_for_decl(source_sema, source_decl_index);
    if (source_method == NULL || !source_method->is_public) {
        return;
    }

    array_push(
        sema->methods,
        (SemaMethod){
            .symbol_handle = sema_import_symbol_handle(
                dst_lexer, source_lexer, source_method->symbol_handle),
            .decl_index              = dst_decl_index,
            .impl_node_index         = source_method->impl_node_index,
            .target_type_node_index  = source_method->target_type_node_index,
            .generic_params_index    = source_method->generic_params_index,
            .is_public               = true,
            .is_associated           = source_method->is_associated,
            .first_param_is_receiver = source_method->first_param_is_receiver,
            .returns_self            = source_method->returns_self,
            .is_trait_impl           = source_method->is_trait_impl,
        });
}

internal bool sema_module_exports_decl(const ModuleInfo* module, u32 decl_index)
{
    for (u32 i = 0; i < array_count(module->export_decl_indices); ++i) {
        if (module->export_decl_indices[i] == decl_index) {
            return true;
        }
    }
    return false;
}

internal void sema_import_public_methods_from_module(Lexer* dst_lexer,
                                                     Sema*  sema,
                                                     const ModuleInfo* module,
                                                     u32 module_index)
{
    const Lexer* source_lexer = &module->front_end.lexer;
    const Sema*  source_sema  = &module->front_end.sema;
    for (u32 i = 0; i < array_count(source_sema->methods); ++i) {
        const SemaMethod* source_method = &source_sema->methods[i];
        if (!source_method->is_public ||
            source_method->decl_index >= array_count(source_sema->decls)) {
            continue;
        }

        const SemaDecl* source_decl =
            &source_sema->decls[source_method->decl_index];
        if (source_decl->import_module_index != sema_no_decl() &&
            !sema_module_exports_decl(module, source_method->decl_index)) {
            continue;
        }

        u32 symbol = sema_import_symbol_handle(
            dst_lexer, source_lexer, source_decl->symbol_handle);
        u32 type = sema_import_type(dst_lexer,
                                    sema,
                                    source_lexer,
                                    source_sema,
                                    source_decl->type_index);
        u32 imported_decl =
            sema_ensure_module_export_decl(sema,
                                           symbol,
                                           type,
                                           source_decl->kind,
                                           module_index,
                                           source_method->decl_index);
        sema_import_method_for_decl(dst_lexer,
                                    sema,
                                    source_lexer,
                                    source_sema,
                                    source_method->decl_index,
                                    imported_decl);
    }
}

internal bool
sema_import_implicit_core_method(Lexer* lexer, Sema* sema, u32 method_symbol)
{
    if (sema == NULL || sema->program == NULL ||
        sema_module_is_core(sema->program, sema->current_module_index)) {
        return true;
    }

    for (u32 i = 0; i < array_count(sema->program->modules); ++i) {
        if (sema_module_is_core(sema->program, i)) {
            const ModuleInfo* module       = &sema->program->modules[i];
            const Lexer*      source_lexer = &module->front_end.lexer;
            const Sema*       source_sema  = &module->front_end.sema;
            string            method_name  = lex_symbol(lexer, method_symbol);
            for (u32 j = 0; j < array_count(source_sema->methods); ++j) {
                const SemaMethod* source_method = &source_sema->methods[j];
                if (!source_method->is_public ||
                    source_method->decl_index >=
                        array_count(source_sema->decls) ||
                    !string_eq(
                        lex_symbol(source_lexer, source_method->symbol_handle),
                        method_name)) {
                    continue;
                }

                const SemaDecl* source_decl =
                    &source_sema->decls[source_method->decl_index];
                u32 symbol = sema_import_symbol_handle(
                    lexer, source_lexer, source_decl->symbol_handle);
                u32 type = sema_import_type(lexer,
                                            sema,
                                            source_lexer,
                                            source_sema,
                                            source_decl->type_index);
                u32 imported_decl =
                    sema_ensure_module_export_decl(sema,
                                                   symbol,
                                                   type,
                                                   source_decl->kind,
                                                   i,
                                                   source_method->decl_index);
                sema_import_method_for_decl(lexer,
                                            sema,
                                            source_lexer,
                                            source_sema,
                                            source_method->decl_index,
                                            imported_decl);
            }
            return true;
        }
    }

    return true;
}

internal bool sema_resolve_generic_type_application(const Lexer* lexer,
                                                    const Ast*   ast,
                                                    Sema*        sema,
                                                    u32          node_index,
                                                    SemaTypeSubstitution subst,
                                                    u32* out_type_index)
{
    const AstNode*          node   = &ast->nodes[node_index];
    const AstTypeApplyInfo* apply  = &ast->type_applications[node->a];
    const AstNode*          target = &ast->nodes[apply->target_node_index];
    if (target->kind != AK_SymbolRef) {
        return error_0303_unknown_type(
            lexer->source, sema_node_span(lexer, target), s("<generic>"));
    }

    if (string_eq(lex_symbol(lexer, target->a), s("box"))) {
        if (apply->arg_count != 1) {
            return error_0313_argument_count_mismatch(
                lexer->source,
                sema_node_span(lexer, node),
                1,
                apply->arg_count);
        }
        u32 item_type = sema_no_type();
        if (!sema_resolve_type_node_ex(lexer,
                                       ast,
                                       sema,
                                       ast->tuple_items[apply->first_arg],
                                       subst,
                                       &item_type)) {
            return false;
        }
        *out_type_index = sema_add_box_type(sema, item_type);
        sema->node_type_indices[node_index] = *out_type_index;
        return true;
    }

    u32 decl_index = sema_find_decl(sema, target->a);
    if (decl_index == sema_no_decl() ||
        sema->decls[decl_index].kind != SK_GenericTypeAlias) {
        return error_0303_unknown_type(lexer->source,
                                       sema_node_span(lexer, target),
                                       lex_symbol(lexer, target->a));
    }

    const Lexer*    source_lexer      = lexer;
    const Ast*      source_ast        = ast;
    Sema*           source_sema       = sema;
    u32             source_decl_index = decl_index;
    const SemaDecl* decl              = &sema->decls[decl_index];
    bool            imported    = sema_imported_decl_source(sema,
                                                            decl,
                                                            &source_lexer,
                                                            &source_ast,
                                                            &source_sema,
                                                            &source_decl_index);
    const SemaDecl* source_decl = &source_sema->decls[source_decl_index];

    u32 template_node           = sema_unwrap_type_candidate_node(
        source_ast, source_decl->value_node_index);
    u32 generic_params_index =
        sema_type_node_generic_params_index(source_ast, template_node);
    ASSERT(generic_params_index != U32_MAX,
           "Expected generic type alias template");
    const AstGenericParams* generic =
        &source_ast->generic_params[generic_params_index];
    if (apply->arg_count != generic->symbol_count) {
        return error_0313_argument_count_mismatch(lexer->source,
                                                  sema_node_span(lexer, node),
                                                  generic->symbol_count,
                                                  apply->arg_count);
    }

    Array(u32) source_arg_types = NULL;
    for (u32 i = 0; i < apply->arg_count; ++i) {
        u32 arg_type = sema_no_type();
        if (!sema_resolve_type_node_ex(lexer,
                                       ast,
                                       sema,
                                       ast->tuple_items[apply->first_arg + i],
                                       subst,
                                       &arg_type)) {
            array_free(source_arg_types);
            return false;
        }
        if (imported) {
            arg_type = sema_import_type(
                (Lexer*)source_lexer, source_sema, lexer, sema, arg_type);
        }
        array_push(source_arg_types, arg_type);
    }

    SemaTypeSubstitution template_subst = {
        .param_symbols =
            &source_ast->generic_param_symbols[generic->first_symbol],
        .arg_types = source_arg_types,
        .count     = generic->symbol_count,
    };
    u32  source_type = sema_no_type();
    bool ok          = sema_resolve_type_node_ex(source_lexer,
                                                 source_ast,
                                                 source_sema,
                                                 template_node,
                                                 template_subst,
                                                 &source_type);
    array_free(source_arg_types);
    if (!ok) {
        return false;
    }

    *out_type_index =
        imported
            ? sema_import_type(
                  (Lexer*)lexer, sema, source_lexer, source_sema, source_type)
            : source_type;
    sema->node_type_indices[node_index] = *out_type_index;
    return true;
}

internal bool sema_is_builtin_type_name(string name)
{
    return string_eq(name, s("void")) || string_eq(name, s("bool")) ||
           string_eq(name, s("string")) || string_eq(name, s("i8")) ||
           string_eq(name, s("i16")) || string_eq(name, s("i32")) ||
           string_eq(name, s("i64")) || string_eq(name, s("u8")) ||
           string_eq(name, s("u16")) || string_eq(name, s("u32")) ||
           string_eq(name, s("u64")) || string_eq(name, s("f32")) ||
           string_eq(name, s("f64")) || string_eq(name, s("isize")) ||
           string_eq(name, s("usize")) || string_eq(name, s("arena")) ||
           string_eq(name, s("box"));
}

internal bool sema_find_unknown_type_ref_in_type_syntax(const Lexer* lexer,
                                                        const Ast*   ast,
                                                        const Sema*  sema,
                                                        u32          node_index,
                                                        u32          depth,
                                                        u32* out_node_index)
{
    if (depth > array_count(sema->decls)) {
        return false;
    }

    node_index          = sema_unwrap_type_candidate_node(ast, node_index);
    const AstNode* node = &ast->nodes[node_index];

    switch (node->kind) {
    case AK_TypeNever:
        return false;

    case AK_SymbolRef:
        if (sema_is_builtin_type_name(lex_symbol(lexer, node->a))) {
            return false;
        }
        u32 decl_index = sema_find_decl(sema, node->a);
        if (decl_index == sema_no_decl()) {
            *out_node_index = node_index;
            return true;
        }
        const SemaDecl* decl = &sema->decls[decl_index];
        if (decl->kind != SK_TypeAlias &&
            decl->type_node_index == sema_no_type() &&
            decl->value_node_index != sema_no_decl()) {
            return sema_find_unknown_type_ref_in_type_syntax(
                lexer,
                ast,
                sema,
                decl->value_node_index,
                depth + 1,
                out_node_index);
        }
        return false;

    case AK_TypeFn:
        {
            const AstFnSignature* signature = sema_ast_signature(ast, node);
            for (u32 i = 0; i < signature->param_count; ++i) {
                if (sema_find_unknown_type_ref_in_type_syntax(
                        lexer,
                        ast,
                        sema,
                        ast->params[signature->first_param + i].type_node_index,
                        depth,
                        out_node_index)) {
                    return true;
                }
            }
            return signature->return_type_node_index != U32_MAX &&
                   sema_find_unknown_type_ref_in_type_syntax(
                       lexer,
                       ast,
                       sema,
                       signature->return_type_node_index,
                       depth,
                       out_node_index);
        }

    case AK_TypeApply:
        {
            const AstTypeApplyInfo* apply = &ast->type_applications[node->a];
            if (sema_find_unknown_type_ref_in_type_syntax(
                    lexer,
                    ast,
                    sema,
                    apply->target_node_index,
                    depth,
                    out_node_index)) {
                return true;
            }
            for (u32 i = 0; i < apply->arg_count; ++i) {
                if (sema_find_unknown_type_ref_in_type_syntax(
                        lexer,
                        ast,
                        sema,
                        ast->tuple_items[apply->first_arg + i],
                        depth,
                        out_node_index)) {
                    return true;
                }
            }
            return false;
        }

    case AK_TypeTuple:
        for (u32 i = 0; i < node->b; ++i) {
            if (sema_find_unknown_type_ref_in_type_syntax(
                    lexer,
                    ast,
                    sema,
                    ast->tuple_items[node->a + i],
                    depth,
                    out_node_index)) {
                return true;
            }
        }
        return false;

    case AK_TypeArray:
    case AK_TypeDynamicArray:
        return sema_find_unknown_type_ref_in_type_syntax(
            lexer, ast, sema, node->b, depth, out_node_index);

    case AK_TypeSlice:
    case AK_TypePointer:
        return sema_find_unknown_type_ref_in_type_syntax(
            lexer, ast, sema, node->a, depth, out_node_index);

    case AK_TypePlex:
        {
            const AstPlexTypeInfo* plex = &ast->plex_types[node->a];
            for (u32 i = 0; i < plex->field_count; ++i) {
                const AstPlexField* field =
                    &ast->plex_fields[plex->first_field + i];
                if (sema_find_unknown_type_ref_in_type_syntax(
                        lexer,
                        ast,
                        sema,
                        field->type_node_index,
                        depth,
                        out_node_index)) {
                    return true;
                }
            }
            return false;
        }

    case AK_TypeEnum:
        {
            const AstEnumTypeInfo* enum_type = &ast->enum_types[node->a];
            for (u32 i = 0; i < enum_type->variant_count; ++i) {
                const AstEnumVariant* variant =
                    &ast->enum_variants[enum_type->first_variant + i];
                if (variant->type_node_index != U32_MAX &&
                    sema_find_unknown_type_ref_in_type_syntax(
                        lexer,
                        ast,
                        sema,
                        variant->type_node_index,
                        depth,
                        out_node_index)) {
                    return true;
                }
            }
            return false;
        }

    default:
        return false;
    }
}

internal bool sema_error_non_type_in_type_context(const Lexer* lexer,
                                                  const Ast*   ast,
                                                  const Sema*  sema,
                                                  u32          node_index,
                                                  string       expected)
{
    u32 bad_type_ref = U32_MAX;
    if (sema_find_unknown_type_ref_in_type_syntax(
            lexer, ast, sema, node_index, 0, &bad_type_ref)) {
        const AstNode* bad_node = &ast->nodes[bad_type_ref];
        return error_0303_unknown_type(lexer->source,
                                       sema_node_span(lexer, bad_node),
                                       lex_symbol(lexer, bad_node->a));
    }

    u32 bad_node_index      = sema_unwrap_type_candidate_node(ast, node_index);
    const AstNode* bad_node = &ast->nodes[bad_node_index];
    return error_0304_type_mismatch(lexer->source,
                                    sema_node_span(lexer, bad_node),
                                    expected,
                                    s("non-type value"));
}

internal bool sema_decl_is_public(const Ast* ast, const SemaDecl* decl)
{
    if (decl->bind_node_index == sema_no_decl()) {
        return true;
    }
    if (decl->bind_node_index >= array_count(ast->nodes)) {
        return false;
    }
    return ast_has_flag(&ast->nodes[decl->bind_node_index], ANF_Public);
}

internal bool sema_type_node_contains_private_type_ref(const Lexer* lexer,
                                                       const Ast*   ast,
                                                       const Sema*  sema,
                                                       u32          node_index,
                                                       u32* out_decl_index,
                                                       u32* out_ref_node_index)
{
    if (node_index >= array_count(ast->nodes)) {
        return false;
    }

    const AstNode* node = &ast->nodes[node_index];
    switch (node->kind) {
    case AK_Expression:
    case AK_Statement:
    case AK_AnnotatedValue:
        return sema_type_node_contains_private_type_ref(
            lexer, ast, sema, node->a, out_decl_index, out_ref_node_index);

    case AK_SymbolRef:
        {
            if (sema_type_index_for_name((Sema*)sema,
                                         lex_symbol(lexer, node->a)) !=
                sema_no_type()) {
                return false;
            }

            u32 decl_index = sema_find_decl(sema, node->a);
            if (decl_index == sema_no_decl()) {
                return false;
            }

            const SemaDecl* decl = &sema->decls[decl_index];
            if ((decl->kind == SK_TypeAlias ||
                 decl->kind == SK_GenericTypeAlias) &&
                !sema_decl_is_public(ast, decl)) {
                *out_decl_index     = decl_index;
                *out_ref_node_index = node_index;
                return true;
            }
            return false;
        }

    case AK_Field:
        {
            if (sema_type_node_contains_private_type_ref(lexer,
                                                         ast,
                                                         sema,
                                                         node->a,
                                                         out_decl_index,
                                                         out_ref_node_index)) {
                return true;
            }

            u32 decl_index = node_index < array_count(sema->node_decl_indices)
                                 ? sema->node_decl_indices[node_index]
                                 : sema_no_decl();
            if (decl_index == sema_no_decl()) {
                return false;
            }

            const SemaDecl* decl = &sema->decls[decl_index];
            if ((decl->kind == SK_TypeAlias ||
                 decl->kind == SK_GenericTypeAlias) &&
                !sema_decl_is_public(ast, decl)) {
                *out_decl_index     = decl_index;
                *out_ref_node_index = node_index;
                return true;
            }
            return false;
        }

    case AK_TypeApply:
        {
            const AstTypeApplyInfo* apply = &ast->type_applications[node->a];
            if (sema_type_node_contains_private_type_ref(
                    lexer,
                    ast,
                    sema,
                    apply->target_node_index,
                    out_decl_index,
                    out_ref_node_index)) {
                return true;
            }
            for (u32 i = 0; i < apply->arg_count; ++i) {
                if (sema_type_node_contains_private_type_ref(
                        lexer,
                        ast,
                        sema,
                        ast->tuple_items[apply->first_arg + i],
                        out_decl_index,
                        out_ref_node_index)) {
                    return true;
                }
            }
            return false;
        }

    case AK_TypeFn:
        {
            const AstFnSignature* signature = sema_ast_signature(ast, node);
            for (u32 i = 0; i < signature->param_count; ++i) {
                if (sema_type_node_contains_private_type_ref(
                        lexer,
                        ast,
                        sema,
                        ast->params[signature->first_param + i].type_node_index,
                        out_decl_index,
                        out_ref_node_index)) {
                    return true;
                }
            }
            if (signature->return_type_node_index != U32_MAX &&
                sema_type_node_contains_private_type_ref(
                    lexer,
                    ast,
                    sema,
                    signature->return_type_node_index,
                    out_decl_index,
                    out_ref_node_index)) {
                return true;
            }
            return false;
        }

    case AK_TypeTuple:
        for (u32 i = 0; i < node->b; ++i) {
            if (sema_type_node_contains_private_type_ref(
                    lexer,
                    ast,
                    sema,
                    ast->tuple_items[node->a + i],
                    out_decl_index,
                    out_ref_node_index)) {
                return true;
            }
        }
        return false;

    case AK_TypeArray:
        return sema_type_node_contains_private_type_ref(
            lexer, ast, sema, node->b, out_decl_index, out_ref_node_index);

    case AK_TypeSlice:
    case AK_TypeDynamicArray:
    case AK_TypePointer:
        return sema_type_node_contains_private_type_ref(
            lexer, ast, sema, node->a, out_decl_index, out_ref_node_index);

    case AK_TypePlex:
        {
            const AstPlexTypeInfo* plex = &ast->plex_types[node->a];
            for (u32 i = 0; i < plex->field_count; ++i) {
                if (sema_type_node_contains_private_type_ref(
                        lexer,
                        ast,
                        sema,
                        ast->plex_fields[plex->first_field + i].type_node_index,
                        out_decl_index,
                        out_ref_node_index)) {
                    return true;
                }
            }
            return false;
        }

    case AK_TypeEnum:
        {
            const AstEnumTypeInfo* enum_type = &ast->enum_types[node->a];
            for (u32 i = 0; i < enum_type->variant_count; ++i) {
                u32 payload_node =
                    ast->enum_variants[enum_type->first_variant + i]
                        .type_node_index;
                if (payload_node != U32_MAX &&
                    sema_type_node_contains_private_type_ref(
                        lexer,
                        ast,
                        sema,
                        payload_node,
                        out_decl_index,
                        out_ref_node_index)) {
                    return true;
                }
            }
            return false;
        }

    default:
        return false;
    }
}

internal bool sema_check_public_field_type_visibility(const Lexer* lexer,
                                                      const Ast*   ast,
                                                      const Sema*  sema,
                                                      u32 owner_decl_index,
                                                      u32 field_type_node_index)
{
    if (owner_decl_index >= array_count(sema->decls)) {
        return true;
    }

    const SemaDecl* owner_decl = &sema->decls[owner_decl_index];
    if (!sema_decl_is_public(ast, owner_decl)) {
        return true;
    }

    u32 private_decl_index = sema_no_decl();
    u32 ref_node_index     = U32_MAX;
    if (!sema_type_node_contains_private_type_ref(lexer,
                                                  ast,
                                                  sema,
                                                  field_type_node_index,
                                                  &private_decl_index,
                                                  &ref_node_index)) {
        return true;
    }

    ASSERT(private_decl_index < array_count(sema->decls),
           "Expected private declaration index");
    ASSERT(ref_node_index < array_count(ast->nodes),
           "Expected private type reference node");
    const SemaDecl* private_decl = &sema->decls[private_decl_index];
    return error_0341_private_type_in_public_field(
        lexer->source,
        sema_node_span(lexer, &ast->nodes[ref_node_index]),
        lex_symbol(lexer, owner_decl->symbol_handle),
        lex_symbol(lexer, private_decl->symbol_handle));
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
    case AK_TypeNever:
        *out_is_type    = true;
        *out_type_index = sema_builtin_type(sema, STK_Never);
        return true;

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

    case AK_Field:
        {
            u32 type_index = sema_no_type();
            if (!sema_try_resolve_qualified_type_node(
                    lexer, ast, sema, node_index, &type_index)) {
                *out_is_type    = false;
                *out_type_index = sema_no_type();
                return true;
            }
            *out_is_type    = true;
            *out_type_index = type_index;
            return true;
        }

    case AK_TypeApply:
        {
            u32 type_index = sema_no_type();
            if (!sema_resolve_generic_type_application(
                    lexer,
                    ast,
                    sema,
                    node_index,
                    (SemaTypeSubstitution){0},
                    &type_index)) {
                return false;
            }
            *out_is_type    = true;
            *out_type_index = type_index;
            return true;
        }

    case AK_TypeFn:
        {
            const AstFnSignature* signature = sema_ast_signature(ast, node);
            Array(u32) param_types          = NULL;
            if (signature->generic_params_index != U32_MAX) {
                return error_0339_generics_not_implemented(
                    lexer->source,
                    sema_node_span(lexer, node),
                    s("generic function type"));
            }

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

            u32 return_type = sema_builtin_type(sema, STK_Void);
            if (signature->return_type_node_index != U32_MAX) {
                bool return_is_type = false;
                if (!sema_try_classify_type_node(
                        lexer,
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
                    lexer, ast, sema, node->a, &item_count)) {
                u32 actual_type = sema_no_type();
                if (!sema_infer_node_type(lexer,
                                          ast,
                                          sema,
                                          node->a,
                                          sema_no_type(),
                                          &actual_type)) {
                    return false;
                }
                Arena temp_arena = {0};
                arena_init(&temp_arena);
                bool ok = error_0304_type_mismatch(
                    lexer->source,
                    sema_node_span(lexer, &ast->nodes[node->a]),
                    s("non-negative integer constant"),
                    actual_type == sema_no_type()
                        ? s("<unknown>")
                        : sema_type_name(
                              lexer, sema, &temp_arena, actual_type));
                arena_done(&temp_arena);
                return ok;
            }
            if (item_count < 0 || item_count > UINT32_MAX) {
                return error_0304_type_mismatch(
                    lexer->source,
                    sema_node_span(lexer, &ast->nodes[node->a]),
                    s("non-negative integer constant"),
                    s("out-of-range integer constant"));
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

    case AK_TypeDynamicArray:
        {
            if (node->a != U32_MAX) {
                i64 min_capacity = 0;
                if (!sema_try_eval_integer_constant(
                        lexer, ast, sema, node->a, &min_capacity)) {
                    u32 actual_type = sema_no_type();
                    if (!sema_infer_node_type(lexer,
                                              ast,
                                              sema,
                                              node->a,
                                              sema_no_type(),
                                              &actual_type)) {
                        return false;
                    }
                    if (!sema_type_is_integer(sema, actual_type)) {
                        Arena temp_arena = {0};
                        arena_init(&temp_arena);
                        bool ok = error_0304_type_mismatch(
                            lexer->source,
                            sema_node_span(lexer, &ast->nodes[node->a]),
                            s("integer dynamic array capacity"),
                            actual_type == sema_no_type()
                                ? s("<unknown>")
                                : sema_type_name(
                                      lexer, sema, &temp_arena, actual_type));
                        arena_done(&temp_arena);
                        return ok;
                    }
                } else if (min_capacity < 0 || min_capacity > INT32_MAX) {
                    return error_0304_type_mismatch(
                        lexer->source,
                        sema_node_span(lexer, &ast->nodes[node->a]),
                        s("non-negative integer constant"),
                        s("out-of-range integer constant"));
                }
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
            *out_is_type    = true;
            *out_type_index = sema_add_dynamic_array_type(sema, item_type);
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

    case AK_TypeOptional:
        {
            bool payload_is_type = false;
            u32  payload_type    = sema_no_type();
            if (!sema_try_classify_type_node(lexer,
                                             ast,
                                             sema,
                                             owner_decl_index,
                                             node->a,
                                             alias_states,
                                             &payload_is_type,
                                             &payload_type)) {
                return false;
            }
            *out_is_type = payload_is_type;
            *out_type_index =
                payload_is_type
                    ? sema_add_optional_type(lexer, sema, payload_type)
                    : sema_no_type();
            return true;
        }

    case AK_TypeResult:
        {
            bool success_is_type = false;
            bool error_is_type   = false;
            u32  success_type    = sema_no_type();
            u32  error_type      = sema_no_type();
            if (!sema_try_classify_type_node(lexer,
                                             ast,
                                             sema,
                                             owner_decl_index,
                                             node->a,
                                             alias_states,
                                             &success_is_type,
                                             &success_type) ||
                !sema_try_classify_type_node(lexer,
                                             ast,
                                             sema,
                                             owner_decl_index,
                                             node->b,
                                             alias_states,
                                             &error_is_type,
                                             &error_type)) {
                return false;
            }
            *out_is_type    = success_is_type && error_is_type;
            *out_type_index = *out_is_type
                                  ? sema_add_result_type(
                                        lexer, sema, success_type, error_type)
                                  : sema_no_type();
            return true;
        }

    case AK_TypePlex:
        {
            const AstPlexTypeInfo* plex = &ast->plex_types[node->a];
            Array(u32) field_types      = NULL;
            if (plex->generic_params_index != U32_MAX) {
                return error_0339_generics_not_implemented(
                    lexer->source,
                    sema_node_span(lexer, node),
                    (plex->flags & APTF_Union) ? s("generic union")
                                               : s("generic plex"));
            }
            for (u32 i = 0; i < plex->field_count; ++i) {
                const AstPlexField* field =
                    &ast->plex_fields[plex->first_field + i];
                bool field_is_type = false;
                u32  field_type    = sema_no_type();
                if (!sema_try_classify_type_node(lexer,
                                                 ast,
                                                 sema,
                                                 owner_decl_index,
                                                 field->type_node_index,
                                                 alias_states,
                                                 &field_is_type,
                                                 &field_type)) {
                    array_free(field_types);
                    return false;
                }
                if (!field_is_type) {
                    array_free(field_types);
                    return sema_error_non_type_in_type_context(
                        lexer,
                        ast,
                        sema,
                        field->type_node_index,
                        s("plex field type"));
                }
                if (!sema_check_public_field_type_visibility(
                        lexer,
                        ast,
                        sema,
                        owner_decl_index,
                        field->type_node_index)) {
                    array_free(field_types);
                    return false;
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
            Array(i64) discriminants         = NULL;
            i64 next_discriminant            = 0;
            if (enum_type->generic_params_index != U32_MAX) {
                return error_0339_generics_not_implemented(
                    lexer->source,
                    sema_node_span(lexer, node),
                    s("generic enum"));
            }
            for (u32 i = 0; i < enum_type->variant_count; ++i) {
                const AstEnumVariant* variant =
                    &ast->enum_variants[enum_type->first_variant + i];
                if (!sema_check_enum_variant_name_unique(
                        lexer,
                        &ast->enum_variants[enum_type->first_variant],
                        i)) {
                    array_free(payload_types);
                    array_free(discriminants);
                    return false;
                }
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
                i64 discriminant = next_discriminant;
                if (variant->value_node_index != U32_MAX) {
                    if (!sema_try_eval_integer_constant(
                            lexer,
                            ast,
                            sema,
                            variant->value_node_index,
                            &discriminant)) {
                        u32 actual_type = sema_no_type();
                        if (!sema_infer_node_type(lexer,
                                                  ast,
                                                  sema,
                                                  variant->value_node_index,
                                                  sema_no_type(),
                                                  &actual_type)) {
                            array_free(payload_types);
                            array_free(discriminants);
                            return false;
                        }
                        Arena temp_arena = {0};
                        arena_init(&temp_arena);
                        bool ok = error_0304_type_mismatch(
                            lexer->source,
                            sema_node_span(
                                lexer, &ast->nodes[variant->value_node_index]),
                            s("integer constant expression"),
                            actual_type == sema_no_type()
                                ? s("<unknown>")
                                : sema_type_name(
                                      lexer, sema, &temp_arena, actual_type));
                        arena_done(&temp_arena);
                        array_free(payload_types);
                        array_free(discriminants);
                        return ok;
                    }
                }
                if (discriminant < 0 || discriminant > UINT32_MAX) {
                    array_free(payload_types);
                    array_free(discriminants);
                    return error_0304_type_mismatch(
                        lexer->source,
                        variant->value_node_index == U32_MAX
                            ? sema_token_span(lexer, variant->token_index)
                            : sema_node_span(
                                  lexer,
                                  &ast->nodes[variant->value_node_index]),
                        s("non-negative integer constant"),
                        s("out-of-range integer constant"));
                }
                if (!sema_check_enum_discriminant_unique(
                        lexer,
                        ast,
                        &ast->enum_variants[enum_type->first_variant],
                        discriminants,
                        i,
                        discriminant)) {
                    array_free(payload_types);
                    array_free(discriminants);
                    return false;
                }
                array_push(discriminants, discriminant);
                next_discriminant = discriminant + 1;
            }
            *out_is_type    = true;
            *out_type_index = sema_add_enum_type(
                sema,
                &ast->enum_variants[enum_type->first_variant],
                payload_types,
                discriminants,
                enum_type->variant_count);
            array_free(payload_types);
            array_free(discriminants);
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
    if (decl->kind == SK_GenericTypeAlias) {
        *out_is_type    = false;
        *out_type_index = sema_no_type();
        return true;
    }
    if (decl->kind == SK_TypeAlias) {
        *out_is_type    = true;
        *out_type_index = decl->type_index;
        return true;
    }
    if (decl->kind == SK_Variable || decl->kind == SK_Function ||
        decl->kind == SK_FfiFunction || decl->kind == SK_Module ||
        decl->kind == SK_BuiltinFunction || decl->kind == SK_Trait) {
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
        if (decl->type_index != sema_no_type()) {
            *out_is_type    = true;
            *out_type_index = decl->type_index;
            return true;
        }
        return error_0309_type_alias_cycle(
            lexer->source,
            sema_decl_span(lexer, ast, &sema->decls[owner_decl_index]),
            lex_symbol(lexer, sema->decls[owner_decl_index].symbol_handle),
            sema_decl_span(lexer, ast, decl),
            lex_symbol(lexer, decl->symbol_handle));
    case SEMA_ALIAS_UNSEEN:
        break;
    }

    u32 value_node_index = decl->value_node_index;
    value_node_index = sema_unwrap_type_candidate_node(ast, value_node_index);

    u32 generic_params_index =
        sema_type_node_generic_params_index(ast, value_node_index);
    if (generic_params_index != U32_MAX) {
        decl->kind               = SK_GenericTypeAlias;
        decl->type_index         = sema_no_type();
        alias_states[decl_index] = SEMA_ALIAS_DONE;
        *out_is_type             = false;
        *out_type_index          = sema_no_type();
        return true;
    }

    u32 reserved_type = sema_no_type();
    if (ast->nodes[value_node_index].kind == AK_TypePlex) {
        const AstPlexTypeInfo* plex =
            &ast->plex_types[ast->nodes[value_node_index].a];
        reserved_type = sema_reserve_type(
            sema,
            (SemaType){
                .kind = (plex->flags & APTF_Union) ? STK_Union : STK_Plex,
                .param_count      = 0,
                .flags            = plex->flags,
                .first_param_type = 0,
                .return_type      = sema_no_type(),
            });
        decl->type_index = reserved_type;
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
        if (reserved_type != sema_no_type()) {
            sema->types[reserved_type] = sema->types[rhs_type];
            rhs_type                   = reserved_type;
            const SemaType* type       = &sema->types[rhs_type];
            for (u32 i = 0; i < type->param_count; ++i) {
                if (sema_type_contains_unboxed_type(
                        sema,
                        sema->type_param_types[type->first_param_type + i],
                        rhs_type)) {
                    return error_0309_type_alias_cycle(
                        lexer->source,
                        sema_decl_span(lexer, ast, decl),
                        lex_symbol(lexer, decl->symbol_handle),
                        sema_decl_span(lexer, ast, decl),
                        lex_symbol(lexer, decl->symbol_handle));
                }
            }
        }
        decl->kind       = SK_TypeAlias;
        decl->type_index = rhs_type;
    } else if (reserved_type != sema_no_type()) {
        return error_0304_type_mismatch(
            lexer->source,
            sema_decl_span(lexer, ast, decl),
            s("type declaration"),
            s("non-type value in type declaration"));
    }

    alias_states[decl_index] = SEMA_ALIAS_DONE;
    *out_is_type             = rhs_is_type;
    *out_type_index          = rhs_type;
    return true;
}

internal bool sema_keyword_is_defined(const FrontEndOptions* options,
                                      string                 name)
{
    if (!options->release && string_eq_cstr(name, "debug")) {
        return true;
    }
    if (options->release && string_eq_cstr(name, "release")) {
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
#if ARCH_X86_64
    if (string_eq_cstr(name, "x64")) {
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
    bool                enabled =
        sema_keyword_is_defined(options, lexer->strings[info->string_index]);
    return info->is_negated ? !enabled : enabled;
}

internal bool sema_validate_top_on_assertion(const FrontEndOptions* options,
                                             const Lexer*           lexer,
                                             const Ast*             ast,
                                             const AstNode*         node)
{
    ASSERT(node->kind == AK_TopOn, "Expected top-level on node");
    const AstTopOnInfo* info = &ast->top_ons[node->a];
    if (!info->is_assert || sema_top_on_is_enabled(options, lexer, ast, node)) {
        return true;
    }
    return error_0336_platform_assertion_failed(
        lexer->source,
        sema_node_span(lexer, node),
        lexer->strings[info->string_index],
        info->is_negated);
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
        if (info->body_node_index == U32_MAX) {
            continue;
        }
        const AstNode* body = &ast->nodes[info->body_node_index];
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

internal bool
sema_node_is_inside_disabled_top_on_body(const FrontEndOptions* options,
                                         const Lexer*           lexer,
                                         const Ast*             ast,
                                         u32                    node_index)
{
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* owner = &ast->nodes[i];
        if (owner->kind != AK_TopOn) {
            continue;
        }

        const AstTopOnInfo* info = &ast->top_ons[owner->a];
        if (info->is_assert || info->body_node_index == U32_MAX ||
            sema_top_on_is_enabled(options, lexer, ast, owner)) {
            continue;
        }

        const AstNode* body = &ast->nodes[info->body_node_index];
        if (body->a <= node_index && node_index < body->b) {
            return true;
        }
    }

    return false;
}

internal u32 sema_trait_symbol_from_type_node(const Ast* ast, u32 node_index)
{
    if (node_index >= array_count(ast->nodes)) {
        return U32_MAX;
    }

    const AstNode* node = &ast->nodes[node_index];
    if (node->kind == AK_SymbolRef) {
        return node->a;
    }
    if (node->kind == AK_TypeApply &&
        node->a < array_count(ast->type_applications)) {
        return sema_trait_symbol_from_type_node(
            ast, ast->type_applications[node->a].target_node_index);
    }
    return U32_MAX;
}

internal u32 sema_trait_type_arg_count(const Ast* ast, u32 node_index)
{
    if (node_index >= array_count(ast->nodes)) {
        return 0;
    }

    const AstNode* node = &ast->nodes[node_index];
    while ((node->kind == AK_Expression || node->kind == AK_Statement) &&
           node->a < array_count(ast->nodes)) {
        node_index = node->a;
        node       = &ast->nodes[node_index];
    }

    if (node->kind == AK_TypeApply &&
        node->a < array_count(ast->type_applications)) {
        return ast->type_applications[node->a].arg_count;
    }
    return 0;
}

internal u32 sema_trait_type_target_node_index(const Ast* ast, u32 node_index)
{
    if (node_index >= array_count(ast->nodes)) {
        return node_index;
    }

    const AstNode* node = &ast->nodes[node_index];
    while ((node->kind == AK_Expression || node->kind == AK_Statement) &&
           node->a < array_count(ast->nodes)) {
        node_index = node->a;
        node       = &ast->nodes[node_index];
    }

    if (node->kind == AK_TypeApply &&
        node->a < array_count(ast->type_applications)) {
        return ast->type_applications[node->a].target_node_index;
    }
    return node_index;
}

internal string
sema_trait_missing_generic_params(const Lexer*            lexer,
                                  const Ast*              ast,
                                  const AstGenericParams* generic,
                                  u32                     actual_count,
                                  Arena*                  arena)
{
    StringBuilder sb = {0};
    sb_init(&sb, arena);
    for (u32 i = actual_count; i < generic->symbol_count; ++i) {
        if (i > actual_count) {
            sb_append_cstr(&sb,
                           i + 1 == generic->symbol_count ? " and " : ", ");
        }
        sb_append_string(
            &sb,
            lex_symbol(lexer,
                       ast->generic_param_symbols[generic->first_symbol + i]));
    }
    return sb_to_string(&sb);
}

internal bool sema_validate_trait_generic_type_args(const Lexer* lexer,
                                                    const Ast*   ast,
                                                    Sema*        sema,
                                                    u32 trait_type_node_index,
                                                    u32 trait_decl_index)
{
    if (trait_decl_index == sema_no_decl() ||
        trait_decl_index >= array_count(sema->decls) ||
        sema->decls[trait_decl_index].value_node_index >=
            array_count(ast->nodes)) {
        return true;
    }

    const AstNode* trait_node =
        &ast->nodes[sema->decls[trait_decl_index].value_node_index];
    if (trait_node->kind != AK_Trait ||
        trait_node->a >= array_count(ast->trait_infos)) {
        return true;
    }

    const AstTraitInfo* trait          = &ast->trait_infos[trait_node->a];
    u32                 expected_count = 0;
    if (trait->generic_params_index != U32_MAX) {
        expected_count =
            ast->generic_params[trait->generic_params_index].symbol_count;
    }
    u32 actual_count = sema_trait_type_arg_count(ast, trait_type_node_index);
    if (actual_count == expected_count) {
        return true;
    }

    Arena temp_arena = {0};
    arena_init(&temp_arena);
    string missing = {0};
    if (actual_count < expected_count &&
        trait->generic_params_index != U32_MAX) {
        missing = sema_trait_missing_generic_params(
            lexer,
            ast,
            &ast->generic_params[trait->generic_params_index],
            actual_count,
            &temp_arena);
    }

    u32 trait_symbol =
        sema_trait_symbol_from_type_node(ast, trait_type_node_index);
    bool ok = error_0355_trait_generic_argument_count(
        lexer->source,
        sema_node_span(lexer,
                       &ast->nodes[sema_trait_type_target_node_index(
                           ast, trait_type_node_index)]),
        trait_symbol != U32_MAX ? lex_symbol(lexer, trait_symbol)
                                : s("<trait>"),
        expected_count,
        actual_count,
        missing);
    arena_done(&temp_arena);
    return ok;
}

internal const AstNode* sema_trait_member_value(const Ast*     ast,
                                                const AstNode* member)
{
    if (member->kind != AK_Bind || member->b >= array_count(ast->nodes)) {
        return NULL;
    }
    const AstNode* value = &ast->nodes[member->b];
    if (value->kind == AK_AnnotatedValue &&
        value->b < array_count(ast->nodes)) {
        value = &ast->nodes[value->b];
    }
    return value;
}

internal u32 sema_find_trait_with_member(const Ast* ast,
                                         Sema*      sema,
                                         u32        member_symbol)
{
    for (u32 i = 0; i < array_count(sema->decls); ++i) {
        const SemaDecl* decl = &sema->decls[i];
        if (decl->kind != SK_Trait ||
            decl->value_node_index >= array_count(ast->nodes)) {
            continue;
        }

        const AstNode* trait_node = &ast->nodes[decl->value_node_index];
        if (trait_node->kind != AK_Trait ||
            trait_node->a >= array_count(ast->trait_infos)) {
            continue;
        }

        const AstTraitInfo* trait = &ast->trait_infos[trait_node->a];
        if (trait->body_node_index >= array_count(ast->nodes)) {
            continue;
        }
        const AstNode* body = &ast->nodes[trait->body_node_index];
        for (u32 member_index = body->a; member_index < body->b;
             ++member_index) {
            const AstNode* member = &ast->nodes[member_index];
            if (member->kind == AK_Bind &&
                ast_get_symbol(member) == member_symbol) {
                return decl->symbol_handle;
            }
        }
    }
    return U32_MAX;
}

internal bool sema_impl_has_member(const Ast*     ast,
                                   const AstNode* body,
                                   u32            symbol,
                                   u32*           out_member_node_index)
{
    for (u32 i = body->a; i < body->b; ++i) {
        const AstNode* member = &ast->nodes[i];
        if (member->kind == AK_Bind && ast_get_symbol(member) == symbol) {
            *out_member_node_index = i;
            return true;
        }
    }
    return false;
}

internal u32 sema_find_impl_method_decl(const Sema* sema,
                                        u32         impl_node_index,
                                        u32         symbol)
{
    for (u32 i = 0; i < array_count(sema->methods); ++i) {
        const SemaMethod* method = &sema->methods[i];
        if (method->impl_node_index == impl_node_index &&
            method->symbol_handle == symbol) {
            return method->decl_index;
        }
    }
    return sema_no_decl();
}

internal bool sema_validate_trait_impl(const Lexer* lexer,
                                       const Ast*   ast,
                                       Sema*        sema,
                                       u32          impl_node_index)
{
    const AstNode*     impl_node = &ast->nodes[impl_node_index];
    const AstImplInfo* impl      = &ast->impls[impl_node->a];
    u32                trait_symbol =
        sema_trait_symbol_from_type_node(ast, impl->trait_type_node_index);
    if (trait_symbol == U32_MAX) {
        return error_0304_type_mismatch(lexer->source,
                                        sema_node_span(lexer, impl_node),
                                        s("known trait"),
                                        s("non-trait type expression"));
    }

    u32 trait_decl_index = sema_find_decl(sema, trait_symbol);
    if (trait_decl_index == sema_no_decl() ||
        sema->decls[trait_decl_index].kind != SK_Trait) {
        return error_0304_type_mismatch(lexer->source,
                                        sema_node_span(lexer, impl_node),
                                        s("known trait"),
                                        lex_symbol(lexer, trait_symbol));
    }

    if (impl->generic_params_index == U32_MAX) {
        u32 ignored_target_type = sema_no_type();
        if (!sema_resolve_type_node(lexer,
                                    ast,
                                    sema,
                                    impl->target_type_node_index,
                                    &ignored_target_type)) {
            return false;
        }
    }

    if (sema->decls[trait_decl_index].value_node_index >=
        array_count(ast->nodes)) {
        return true;
    }

    const AstNode* trait_node =
        &ast->nodes[sema->decls[trait_decl_index].value_node_index];
    if (trait_node->kind != AK_Trait ||
        trait_node->a >= array_count(ast->trait_infos)) {
        return true;
    }

    const AstTraitInfo* trait = &ast->trait_infos[trait_node->a];
    if (trait->generic_params_index != U32_MAX) {
        return error_0339_generics_not_implemented(
            lexer->source,
            sema_node_span(lexer, &ast->nodes[impl->trait_type_node_index]),
            s("generic trait implementation"));
    }
    if (trait->body_node_index >= array_count(ast->nodes)) {
        return true;
    }
    const AstNode* trait_body = &ast->nodes[trait->body_node_index];
    const AstNode* impl_body  = &ast->nodes[impl->body_node_index];

    Arena temp_arena          = {0};
    arena_init(&temp_arena);
    StringBuilder missing = {0};
    sb_init(&missing, &temp_arena);
    u32 missing_count = 0;
    for (u32 i = trait_body->a; i < trait_body->b; ++i) {
        const AstNode* required = &ast->nodes[i];
        if (required->kind != AK_Bind) {
            continue;
        }
        u32 required_symbol = ast_get_symbol(required);
        u32 member_index    = U32_MAX;
        if (!sema_impl_has_member(
                ast, impl_body, required_symbol, &member_index)) {
            if (missing_count > 0) {
                sb_append_cstr(&missing, ", ");
            }
            sb_append_char(&missing, '`');
            sb_append_string(&missing, lex_symbol(lexer, required_symbol));
            sb_append_char(&missing, '`');
            missing_count++;
        }
    }

    if (missing_count > 0) {
        bool ok = error_0352_missing_trait_impl_members(
            lexer->source,
            sema_node_span(lexer, impl_node),
            lex_symbol(lexer, trait_symbol),
            sb_to_string(&missing),
            missing_count);
        arena_done(&temp_arena);
        return ok;
    }

    arena_done(&temp_arena);

    for (u32 i = trait_body->a; i < trait_body->b; ++i) {
        const AstNode* required = &ast->nodes[i];
        if (required->kind != AK_Bind) {
            continue;
        }
        u32 required_symbol = ast_get_symbol(required);
        u32 member_index    = U32_MAX;
        if (!sema_impl_has_member(
                ast, impl_body, required_symbol, &member_index)) {
            continue;
        }

        const AstNode* value =
            sema_trait_member_value(ast, &ast->nodes[member_index]);
        if (value == NULL || value->kind != AK_FnDef) {
            return error_0304_type_mismatch(
                lexer->source,
                sema_node_span(lexer, &ast->nodes[member_index]),
                s("function trait member"),
                s("non-function binding"));
        }
    }

    return true;
}

internal bool sema_generic_params_contain_symbol(const Ast* ast,
                                                 u32 generic_params_index,
                                                 u32 symbol)
{
    if (generic_params_index == U32_MAX) {
        return false;
    }
    const AstGenericParams* generic =
        &ast->generic_params[generic_params_index];
    for (u32 i = 0; i < generic->symbol_count; ++i) {
        if (ast->generic_param_symbols[generic->first_symbol + i] == symbol) {
            return true;
        }
    }
    return false;
}

internal bool sema_type_nodes_may_overlap(const Ast* ast,
                                          u32        lhs_node_index,
                                          u32        lhs_generic_params_index,
                                          u32        rhs_node_index,
                                          u32        rhs_generic_params_index)
{
    if (lhs_node_index >= array_count(ast->nodes) ||
        rhs_node_index >= array_count(ast->nodes)) {
        return false;
    }

    const AstNode* lhs = &ast->nodes[lhs_node_index];
    const AstNode* rhs = &ast->nodes[rhs_node_index];
    while ((lhs->kind == AK_Expression || lhs->kind == AK_Statement) &&
           lhs->a < array_count(ast->nodes)) {
        lhs_node_index = lhs->a;
        lhs            = &ast->nodes[lhs_node_index];
    }
    while ((rhs->kind == AK_Expression || rhs->kind == AK_Statement) &&
           rhs->a < array_count(ast->nodes)) {
        rhs_node_index = rhs->a;
        rhs            = &ast->nodes[rhs_node_index];
    }

    if (lhs->kind == AK_SymbolRef &&
        sema_generic_params_contain_symbol(
            ast, lhs_generic_params_index, lhs->a)) {
        return true;
    }
    if (rhs->kind == AK_SymbolRef &&
        sema_generic_params_contain_symbol(
            ast, rhs_generic_params_index, rhs->a)) {
        return true;
    }

    if (lhs->kind != rhs->kind) {
        return false;
    }

    switch (lhs->kind) {
    case AK_TypeNever:
        return true;
    case AK_SymbolRef:
        return lhs->a == rhs->a;
    case AK_TypePointer:
    case AK_TypeSlice:
    case AK_TypeOptional:
        return sema_type_nodes_may_overlap(ast,
                                           lhs->a,
                                           lhs_generic_params_index,
                                           rhs->a,
                                           rhs_generic_params_index);
    case AK_TypeResult:
        return sema_type_nodes_may_overlap(ast,
                                           lhs->a,
                                           lhs_generic_params_index,
                                           rhs->a,
                                           rhs_generic_params_index) &&
               sema_type_nodes_may_overlap(ast,
                                           lhs->b,
                                           lhs_generic_params_index,
                                           rhs->b,
                                           rhs_generic_params_index);
    case AK_TypeArray:
        return sema_type_nodes_may_overlap(ast,
                                           lhs->b,
                                           lhs_generic_params_index,
                                           rhs->b,
                                           rhs_generic_params_index);
    case AK_TypeDynamicArray:
        return sema_type_nodes_may_overlap(ast,
                                           lhs->b,
                                           lhs_generic_params_index,
                                           rhs->b,
                                           rhs_generic_params_index);
    case AK_TypeApply:
        {
            const AstTypeApplyInfo* lhs_apply = &ast->type_applications[lhs->a];
            const AstTypeApplyInfo* rhs_apply = &ast->type_applications[rhs->a];
            if (lhs_apply->arg_count != rhs_apply->arg_count ||
                !sema_type_nodes_may_overlap(ast,
                                             lhs_apply->target_node_index,
                                             lhs_generic_params_index,
                                             rhs_apply->target_node_index,
                                             rhs_generic_params_index)) {
                return false;
            }
            for (u32 i = 0; i < lhs_apply->arg_count; ++i) {
                if (!sema_type_nodes_may_overlap(
                        ast,
                        ast->tuple_items[lhs_apply->first_arg + i],
                        lhs_generic_params_index,
                        ast->tuple_items[rhs_apply->first_arg + i],
                        rhs_generic_params_index)) {
                    return false;
                }
            }
            return true;
        }
    default:
        return lhs_node_index == rhs_node_index;
    }
}

internal bool sema_validate_duplicate_trait_impl(const Lexer* lexer,
                                                 const Ast*   ast,
                                                 Sema*        sema,
                                                 u32          impl_node_index)
{
    const AstNode*     impl_node = &ast->nodes[impl_node_index];
    const AstImplInfo* impl      = &ast->impls[impl_node->a];
    u32                trait_symbol =
        sema_trait_symbol_from_type_node(ast, impl->trait_type_node_index);
    if (trait_symbol == U32_MAX) {
        return true;
    }

    for (u32 i = 0; i < impl_node_index; ++i) {
        const AstNode* previous_node = &ast->nodes[i];
        if (previous_node->kind != AK_Impl ||
            previous_node->a >= array_count(ast->impls)) {
            continue;
        }

        const AstImplInfo* previous = &ast->impls[previous_node->a];
        if (previous->trait_type_node_index == U32_MAX ||
            sema_trait_symbol_from_type_node(
                ast, previous->trait_type_node_index) != trait_symbol) {
            continue;
        }

        if (!sema_type_nodes_may_overlap(ast,
                                         previous->target_type_node_index,
                                         previous->generic_params_index,
                                         impl->target_type_node_index,
                                         impl->generic_params_index)) {
            continue;
        }

        Arena temp_arena = {0};
        arena_init(&temp_arena);
        string impl_name =
            string_format(&temp_arena,
                          STRINGP " implementation",
                          STRINGV(lex_symbol(lexer, trait_symbol)));
        if (previous->generic_params_index == U32_MAX &&
            impl->generic_params_index == U32_MAX) {
            u32 target_type = sema_no_type();
            if (!sema_resolve_type_node(lexer,
                                        ast,
                                        sema,
                                        impl->target_type_node_index,
                                        &target_type)) {
                arena_done(&temp_arena);
                return false;
            }
            target_type = sema_materialise_type(sema, target_type);
            string target_name =
                sema_type_name(lexer, sema, &temp_arena, target_type);
            impl_name = string_format(&temp_arena,
                                      STRINGP " for " STRINGP,
                                      STRINGV(lex_symbol(lexer, trait_symbol)),
                                      STRINGV(target_name));
        }
        bool ok =
            error_0301_duplicate_binding(lexer->source,
                                         sema_node_span(lexer, impl_node),
                                         impl_name,
                                         sema_node_span(lexer, previous_node));
        arena_done(&temp_arena);
        return ok;
    }

    return true;
}

internal const AstFnSignature* sema_fn_def_signature(const Ast*     ast,
                                                     const AstNode* fn_def)
{
    if (fn_def->kind != AK_FnDef || fn_def->a >= array_count(ast->nodes)) {
        return NULL;
    }
    const AstNode* fn_start = &ast->nodes[fn_def->a];
    if (fn_start->kind != AK_FnStart ||
        fn_start->a >= array_count(ast->fn_signatures)) {
        return NULL;
    }
    return &ast->fn_signatures[fn_start->a];
}

internal bool sema_validate_trait_impl_signature(const Lexer* lexer,
                                                 const Ast*   ast,
                                                 Sema*        sema,
                                                 u32          impl_node_index,
                                                 u32 trait_member_node_index,
                                                 u32 impl_member_node_index)
{
    const AstNode*     impl_node = &ast->nodes[impl_node_index];
    const AstImplInfo* impl      = &ast->impls[impl_node->a];
    if (impl->generic_params_index != U32_MAX) {
        return true;
    }

    u32 target_type = sema_no_type();
    if (!sema_resolve_type_node(
            lexer, ast, sema, impl->target_type_node_index, &target_type)) {
        return false;
    }

    u32 trait_symbol =
        sema_trait_symbol_from_type_node(ast, impl->trait_type_node_index);
    u32 trait_decl_index = sema_find_decl(sema, trait_symbol);
    if (trait_decl_index == sema_no_decl() ||
        trait_decl_index >= array_count(sema->decls) ||
        sema->decls[trait_decl_index].value_node_index >=
            array_count(ast->nodes)) {
        return true;
    }
    const AstNode* trait_node =
        &ast->nodes[sema->decls[trait_decl_index].value_node_index];

    u32 self_symbol      = sema_trait_self_alias_symbol(lexer, ast, trait_node);
    u32 subst_symbols[1] = {self_symbol};
    u32 subst_types[1]   = {target_type};
    SemaTypeSubstitution subst = {
        .param_symbols = subst_symbols,
        .arg_types     = subst_types,
        .count         = self_symbol == sema_no_decl() ? 0 : 1,
    };

    const AstNode* required_member = &ast->nodes[trait_member_node_index];
    const AstNode* required_value =
        sema_trait_member_value(ast, required_member);
    const AstNode* impl_member = &ast->nodes[impl_member_node_index];
    const AstNode* impl_value  = sema_trait_member_value(ast, impl_member);
    if (required_value == NULL || required_value->kind != AK_TypeFn ||
        required_value->a >= array_count(ast->fn_signatures)) {
        return error_0304_type_mismatch(lexer->source,
                                        sema_node_span(lexer, required_member),
                                        s("function trait member"),
                                        s("non-function type"));
    }
    if (impl_value == NULL || impl_value->kind != AK_FnDef) {
        return error_0304_type_mismatch(lexer->source,
                                        sema_node_span(lexer, impl_member),
                                        s("function trait member"),
                                        s("non-function binding"));
    }

    const AstFnSignature* impl_sig = sema_fn_def_signature(ast, impl_value);
    if (impl_sig == NULL) {
        return error_0304_type_mismatch(lexer->source,
                                        sema_node_span(lexer, impl_member),
                                        s("function trait member"),
                                        s("non-function binding"));
    }

    u32 expected_fn_type     = sema_no_type();
    u32 required_value_index = (u32)(required_value - ast->nodes);
    if (!sema_resolve_type_node_ex(
            lexer, ast, sema, required_value_index, subst, &expected_fn_type)) {
        return false;
    }
    u32 actual_decl_index = sema_find_impl_method_decl(
        sema, impl_node_index, ast_get_symbol(impl_member));
    if (actual_decl_index == sema_no_decl() ||
        actual_decl_index >= array_count(sema->decls)) {
        return true;
    }
    u32 actual_fn_type = sema->decls[actual_decl_index].type_index;
    if (!sema_type_matches(sema, expected_fn_type, actual_fn_type) ||
        !sema_type_matches(sema, actual_fn_type, expected_fn_type)) {
        Arena temp_arena = {0};
        arena_init(&temp_arena);
        bool ok = error_0304_type_mismatch(
            lexer->source,
            sema_node_span(lexer, impl_member),
            sema_type_name(lexer, sema, &temp_arena, expected_fn_type),
            sema_type_name(lexer, sema, &temp_arena, actual_fn_type));
        arena_done(&temp_arena);
        return ok;
    }

    return true;
}

internal bool
sema_validate_where_constraints(const Lexer* lexer, const Ast* ast, Sema* sema)
{
    for (u32 i = 0; i < array_count(ast->where_constraints); ++i) {
        const AstWhereConstraint* constraint = &ast->where_constraints[i];
        u32 trait_symbol                     = sema_trait_symbol_from_type_node(
            ast, constraint->trait_type_node_index);
        if (trait_symbol == U32_MAX) {
            return error_0304_type_mismatch(
                lexer->source,
                sema_node_span(lexer,
                               &ast->nodes[constraint->trait_type_node_index]),
                s("known trait"),
                s("non-trait type expression"));
        }

        u32 trait_decl_index = sema_find_decl(sema, trait_symbol);
        if (trait_decl_index == sema_no_decl() ||
            sema->decls[trait_decl_index].kind != SK_Trait) {
            return error_0304_type_mismatch(
                lexer->source,
                sema_node_span(lexer,
                               &ast->nodes[constraint->trait_type_node_index]),
                s("known trait"),
                lex_symbol(lexer, trait_symbol));
        }

        if (!sema_validate_trait_generic_type_args(
                lexer,
                ast,
                sema,
                constraint->trait_type_node_index,
                trait_decl_index)) {
            return false;
        }
    }
    return true;
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
        if (node->kind == AK_Impl) {
            const AstImplInfo* impl = &ast->impls[node->a];
            const AstNode*     body = &ast->nodes[impl->body_node_index];
            bool               impl_is_public = ast_has_flag(node, ANF_Public);
            for (u32 method_node_index = body->a; method_node_index < body->b;
                 ++method_node_index) {
                const AstNode* method_node = &ast->nodes[method_node_index];
                if (method_node->kind != AK_Bind) {
                    continue;
                }

                u32 method_symbol = ast_get_symbol(method_node);
                u32 hidden_symbol =
                    sema_mangle_method_symbol(lexer, ast, impl, method_symbol);

                u32            type_node_index  = sema_no_type();
                u32            value_node_index = method_node->b;
                const AstNode* value            = &ast->nodes[value_node_index];
                if (value->kind == AK_AnnotatedValue) {
                    type_node_index  = value->a;
                    value_node_index = value->b;
                    value            = &ast->nodes[value_node_index];
                }

                if (value->kind != AK_FnDef) {
                    return error_0304_type_mismatch(
                        lexer->source,
                        sema_node_span(lexer, method_node),
                        s("function method"),
                        s("non-function binding"));
                }

                const AstNode*        fn_start = &ast->nodes[value->a];
                const AstFnSignature* signature =
                    &ast->fn_signatures[fn_start->a];
                bool first_param_is_receiver = false;
                if (signature->param_count > 0) {
                    u32 first_param_type =
                        ast->params[signature->first_param].type_node_index;
                    while (ast->nodes[first_param_type].kind == AK_Expression ||
                           ast->nodes[first_param_type].kind == AK_Statement) {
                        first_param_type = ast->nodes[first_param_type].a;
                    }
                    const AstNode* first_type = &ast->nodes[first_param_type];
                    if (first_type->kind == AK_SymbolRef &&
                        string_eq(lex_symbol(lexer, first_type->a),
                                  s("Self"))) {
                        first_param_is_receiver = true;
                    } else if (first_type->kind == AK_TypePointer) {
                        u32 pointee = first_type->a;
                        while (ast->nodes[pointee].kind == AK_Expression ||
                               ast->nodes[pointee].kind == AK_Statement) {
                            pointee = ast->nodes[pointee].a;
                        }
                        const AstNode* pointee_node = &ast->nodes[pointee];
                        first_param_is_receiver =
                            pointee_node->kind == AK_SymbolRef &&
                            string_eq(lex_symbol(lexer, pointee_node->a),
                                      s("Self"));
                    }
                }

                bool returns_self = false;
                if (signature->return_type_node_index != U32_MAX) {
                    returns_self = sema_type_syntax_contains_self(
                        lexer, ast, signature->return_type_node_index);
                }

                SemaDeclKind kind = (impl->generic_params_index != U32_MAX ||
                                     sema_fn_def_generic_params_index(
                                         ast, value_node_index) != U32_MAX)
                                        ? SK_GenericFunction
                                        : SK_Function;

                u32 decl_index    = (u32)array_count(sema->decls);
                array_push(sema->decls,
                           (SemaDecl){
                               .kind                = kind,
                               .symbol_handle       = hidden_symbol,
                               .bind_node_index     = method_node_index,
                               .type_node_index     = type_node_index,
                               .value_node_index    = value_node_index,
                               .type_index          = sema_no_type(),
                               .import_module_index = sema_no_decl(),
                               .import_decl_index   = sema_no_decl(),
                           });
                array_push(
                    sema->methods,
                    (SemaMethod){
                        .symbol_handle          = method_symbol,
                        .decl_index             = decl_index,
                        .impl_node_index        = i,
                        .target_type_node_index = impl->target_type_node_index,
                        .generic_params_index   = impl->generic_params_index,
                        .is_public = impl_is_public ||
                                     ast_has_flag(method_node, ANF_Public),
                        .is_associated =
                            returns_self && !first_param_is_receiver,
                        .first_param_is_receiver = first_param_is_receiver,
                        .returns_self            = returns_self,
                        .is_trait_impl = impl->trait_type_node_index != U32_MAX,
                    });
            }
            continue;
        }
        if (node->kind == AK_Trait) {
            continue;
        }
        if (node->kind == AK_TopOn) {
            const AstTopOnInfo* info = &ast->top_ons[node->a];
            if (info->is_assert) {
                if (!sema_validate_top_on_assertion(
                        options, lexer, ast, node)) {
                    return false;
                }
                i++;
                continue;
            }
            if (sema_top_on_is_enabled(options, lexer, ast, node)) {
                const AstNode* body = &ast->nodes[info->body_node_index];
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
            const AstNode* body = &ast->nodes[info->body_node_index];
            ASSERT(body->kind == AK_Block, "Expected top-level on body");
            i = body->b - 1;
            continue;
        }
        if (node->kind != AK_Bind && node->kind != AK_Variable &&
            node->kind != AK_FfiDef) {
            continue;
        }
        if (sema_node_is_inside_function_body(ast, i)) {
            continue;
        }
        if (sema_node_is_inside_impl_body(ast, i)) {
            continue;
        }
        if (sema_node_is_inside_trait_body(ast, i)) {
            continue;
        }
        if (node->kind == AK_FfiDef) {
            bool wrapped_by_binding = false;
            for (u32 j = 0; j < array_count(ast->nodes); ++j) {
                const AstNode* candidate = &ast->nodes[j];
                if ((candidate->kind != AK_Bind &&
                     candidate->kind != AK_Variable) ||
                    sema_node_is_inside_function_body(ast, j)) {
                    continue;
                }

                u32            candidate_value_index = candidate->b;
                const AstNode* candidate_value =
                    &ast->nodes[candidate_value_index];
                if (candidate_value->kind == AK_AnnotatedValue) {
                    candidate_value_index = candidate_value->b;
                } else if (candidate_value->kind == AK_ZeroInit ||
                           candidate_value->kind == AK_Undefined) {
                    continue;
                }

                if (candidate_value_index == i) {
                    wrapped_by_binding = true;
                    break;
                }
            }
            if (wrapped_by_binding) {
                continue;
            }
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
        } else if (value->kind == AK_ZeroInit || value->kind == AK_Undefined) {
            type_node_index  = value->a;
            value_node_index = sema_no_decl();
        }

        SemaDeclKind kind = SK_Constant;
        if (node->kind == AK_FfiDef) {
            kind = SK_FfiFunction;
        } else if (node->kind == AK_Variable) {
            kind = SK_Variable;
        } else if (value->kind == AK_FnDef) {
            kind = sema_fn_def_generic_params_index(ast, value_node_index) !=
                           U32_MAX
                       ? SK_GenericFunction
                       : SK_Function;
        } else if (value->kind == AK_FfiDef) {
            kind = SK_FfiFunction;
        } else if (value->kind == AK_ModRef) {
            kind = SK_Module;
        } else if (value->kind == AK_Trait) {
            kind = SK_Trait;
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
sema_validate_trait_impls(const Lexer* lexer, const Ast* ast, Sema* sema)
{
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind != AK_Impl || node->a >= array_count(ast->impls)) {
            continue;
        }
        const AstImplInfo* impl = &ast->impls[node->a];
        if (impl->trait_type_node_index == U32_MAX) {
            continue;
        }
        if (!sema_validate_duplicate_trait_impl(lexer, ast, sema, i)) {
            return false;
        }
        if (!sema_validate_trait_impl(lexer, ast, sema, i)) {
            return false;
        }
    }
    return true;
}

internal bool sema_validate_trait_impl_signatures(const Lexer* lexer,
                                                  const Ast*   ast,
                                                  Sema*        sema)
{
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind != AK_Impl || node->a >= array_count(ast->impls)) {
            continue;
        }
        const AstImplInfo* impl = &ast->impls[node->a];
        if (impl->trait_type_node_index == U32_MAX) {
            continue;
        }

        u32 trait_symbol =
            sema_trait_symbol_from_type_node(ast, impl->trait_type_node_index);
        if (trait_symbol == U32_MAX) {
            continue;
        }
        u32 trait_decl_index = sema_find_decl(sema, trait_symbol);
        if (trait_decl_index == sema_no_decl() ||
            sema->decls[trait_decl_index].kind != SK_Trait ||
            sema->decls[trait_decl_index].value_node_index >=
                array_count(ast->nodes)) {
            continue;
        }

        const AstNode* trait_node =
            &ast->nodes[sema->decls[trait_decl_index].value_node_index];
        if (trait_node->kind != AK_Trait ||
            trait_node->a >= array_count(ast->trait_infos)) {
            continue;
        }

        const AstTraitInfo* trait = &ast->trait_infos[trait_node->a];
        if (trait->body_node_index >= array_count(ast->nodes)) {
            continue;
        }
        const AstNode* trait_body = &ast->nodes[trait->body_node_index];
        const AstNode* impl_body  = &ast->nodes[impl->body_node_index];
        for (u32 member_index = trait_body->a; member_index < trait_body->b;
             ++member_index) {
            const AstNode* required = &ast->nodes[member_index];
            if (required->kind != AK_Bind) {
                continue;
            }

            u32 impl_member_index = U32_MAX;
            if (!sema_impl_has_member(ast,
                                      impl_body,
                                      ast_get_symbol(required),
                                      &impl_member_index)) {
                continue;
            }
            if (!sema_validate_trait_impl_signature(
                    lexer, ast, sema, i, member_index, impl_member_index)) {
                return false;
            }
        }
    }
    return true;
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
    case AK_TypeNever:
        break;

    case AK_TypeApply:
        {
            const AstTypeApplyInfo* apply = &ast->type_applications[node->a];
            sema_mark_type_expr_nodes(ast, sema, apply->target_node_index);
            for (u32 i = 0; i < apply->arg_count; ++i) {
                sema_mark_type_expr_nodes(
                    ast, sema, ast->tuple_items[apply->first_arg + i]);
            }
        }
        break;
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
    case AK_TypeDynamicArray:
        sema_mark_type_expr_nodes(ast, sema, node->b);
        break;
    case AK_TypePointer:
        sema_mark_type_expr_nodes(ast, sema, node->a);
        break;
    case AK_Field:
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

internal void sema_assign_qualified_type_target_scope(const Ast* ast,
                                                      Sema*      sema,
                                                      u32        node_index,
                                                      u32        scope_index)
{
    if (node_index >= array_count(sema->node_scope_indices)) {
        return;
    }

    sema->node_scope_indices[node_index] = scope_index;

    const AstNode* node                  = &ast->nodes[node_index];
    if (node->kind == AK_Field) {
        sema_assign_qualified_type_target_scope(
            ast, sema, node->a, scope_index);
    }
}

internal void sema_mark_type_expr_nodes_in_scope(const Ast* ast,
                                                 Sema*      sema,
                                                 u32        node_index,
                                                 u32        scope_index)
{
    if (node_index >= array_count(sema->node_is_type_expr)) {
        return;
    }

    sema->node_scope_indices[node_index] = scope_index;
    sema_mark_type_expr_nodes(ast, sema, node_index);
    const AstNode* node = &ast->nodes[node_index];

    switch (node->kind) {
    case AK_Expression:
    case AK_Statement:
        sema_mark_type_expr_nodes_in_scope(ast, sema, node->a, scope_index);
        break;
    case AK_TypeApply:
        {
            const AstTypeApplyInfo* apply = &ast->type_applications[node->a];
            sema_mark_type_expr_nodes_in_scope(
                ast, sema, apply->target_node_index, scope_index);
            for (u32 i = 0; i < apply->arg_count; ++i) {
                sema_mark_type_expr_nodes_in_scope(
                    ast,
                    sema,
                    ast->tuple_items[apply->first_arg + i],
                    scope_index);
            }
        }
        break;
    case AK_TypeFn:
        {
            const AstFnSignature* signature = &ast->fn_signatures[node->a];
            for (u32 i = 0; i < signature->param_count; ++i) {
                sema_mark_type_expr_nodes_in_scope(
                    ast,
                    sema,
                    ast->params[signature->first_param + i].type_node_index,
                    scope_index);
            }
            if (signature->return_type_node_index != U32_MAX) {
                sema_mark_type_expr_nodes_in_scope(
                    ast, sema, signature->return_type_node_index, scope_index);
            }
        }
        break;
    case AK_TypeTuple:
        for (u32 i = 0; i < node->b; ++i) {
            sema_mark_type_expr_nodes_in_scope(
                ast, sema, ast->tuple_items[node->a + i], scope_index);
        }
        break;
    case AK_TypeArray:
        sema_mark_type_expr_nodes_in_scope(ast, sema, node->a, scope_index);
        sema_mark_type_expr_nodes_in_scope(ast, sema, node->b, scope_index);
        break;
    case AK_TypeSlice:
    case AK_TypePointer:
        sema_mark_type_expr_nodes_in_scope(ast, sema, node->a, scope_index);
        break;
    case AK_Field:
        sema_assign_qualified_type_target_scope(
            ast, sema, node->a, scope_index);
        break;
    case AK_TypeDynamicArray:
        if (node->a != U32_MAX) {
            sema->node_scope_indices[node->a] = scope_index;
        }
        sema_mark_type_expr_nodes_in_scope(ast, sema, node->b, scope_index);
        break;
    case AK_TypePlex:
        {
            const AstPlexTypeInfo* plex = &ast->plex_types[node->a];
            for (u32 i = 0; i < plex->field_count; ++i) {
                sema_mark_type_expr_nodes_in_scope(
                    ast,
                    sema,
                    ast->plex_fields[plex->first_field + i].type_node_index,
                    scope_index);
            }
        }
        break;
    case AK_TypeEnum:
        {
            const AstEnumTypeInfo* enum_type = &ast->enum_types[node->a];
            for (u32 i = 0; i < enum_type->variant_count; ++i) {
                const AstEnumVariant* variant =
                    &ast->enum_variants[enum_type->first_variant + i];
                if (variant->type_node_index != U32_MAX) {
                    sema_mark_type_expr_nodes_in_scope(
                        ast, sema, variant->type_node_index, scope_index);
                }
                if (variant->value_node_index != U32_MAX) {
                    sema->node_scope_indices[variant->value_node_index] =
                        scope_index;
                }
            }
        }
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

    for (u32 i = array_count(sema->locals); i-- > 0;) {
        const SemaLocal* local = &sema->locals[i];
        if (local->scope_index == scope_index &&
            local->symbol_handle == symbol_handle) {
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

    for (u32 i = array_count(sema->locals); i-- > 0;) {
        const SemaLocal* local = &sema->locals[i];
        if (local->scope_index == scope_index &&
            (local->kind == SLK_Constant || local->kind == SLK_Function ||
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
    return local->kind == SLK_Variable || local->kind == SLK_Constant ||
           local->kind == SLK_Function || local->kind == SLK_TypeAlias;
}

internal bool sema_symbol_is_discard(const Lexer* lexer, u32 symbol_handle)
{
    return string_eq(lex_symbol(lexer, symbol_handle), s("_"));
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

internal u32 sema_mangle_for_item_symbol(const Lexer* lexer,
                                         u32          current_function_symbol,
                                         u32          symbol_handle,
                                         u32          token_index)
{
    string name   = lex_symbol(lexer, symbol_handle);
    string suffix = string_format(
        &temp_arena, STRINGP "$for%u", STRINGV(name), token_index);
    return sema_mangle_child_function_symbol(
        lexer, current_function_symbol, suffix);
}

internal u32 sema_mangle_discard_symbol(const Lexer* lexer,
                                        u32          current_function_symbol,
                                        u32          token_index)
{
    string suffix = string_format(&temp_arena, "_$discard%u", token_index);
    if (current_function_symbol != U32_MAX) {
        return sema_mangle_child_function_symbol(
            lexer, current_function_symbol, suffix);
    }

    InternAddResult ignored = 0;
    return lex_add_symbol((Lexer*)lexer, suffix, &ignored);
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

internal void sema_mark_fn_signature_type_nodes_in_scope(const Ast* ast,
                                                         Sema*      sema,
                                                         u32 fn_node_index,
                                                         u32 scope_index)
{
    const AstNode*        fn_def    = &ast->nodes[fn_node_index];
    const AstNode*        fn_start  = &ast->nodes[fn_def->a];
    const AstFnSignature* signature = &ast->fn_signatures[fn_start->a];
    for (u32 i = 0; i < signature->param_count; ++i) {
        sema_mark_type_expr_nodes_in_scope(
            ast,
            sema,
            ast->params[signature->first_param + i].type_node_index,
            scope_index);
    }
    if (signature->return_type_node_index != sema_no_type()) {
        sema_mark_type_expr_nodes_in_scope(
            ast, sema, signature->return_type_node_index, scope_index);
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
internal bool sema_resolve_type_node_ex(const Lexer*         lexer,
                                        const Ast*           ast,
                                        Sema*                sema,
                                        u32                  node_index,
                                        SemaTypeSubstitution subst,
                                        u32*                 out_type_index);
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
        bool discard = sema_symbol_is_discard(lexer, symbol);
        u32  duplicate =
            discard ? sema_no_local()
                    : sema_find_local_in_scope(sema, scope_index, symbol);
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
                       .kind             = kind,
                       .symbol_handle    = symbol,
                       .owner_decl_index = owner_decl_index,
                       .scope_index      = scope_index,
                       .decl_node_index  = decl_node_index,
                       .decl_token_index = pattern->token_index,
                       .type_node_index  = sema_no_type(),
                       .value_node_index = ast->nodes[decl_node_index].b,
                       .type_index       = sema_no_type(),
                       .lowered_symbol_handle =
                           discard ? sema_mangle_discard_symbol(
                                         lexer, U32_MAX, pattern->token_index)
                                   : symbol,
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
        if (sema->pattern_local_indices[pattern_index] != sema_no_local()) {
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

        bool discard = sema_symbol_is_discard(lexer, pattern->a);
        u32  duplicate =
            discard ? sema_no_local()
                    : sema_find_local_in_scope(sema, scope_index, pattern->a);
        if (duplicate != sema_no_local()) {
            const SemaLocal* previous = &sema->locals[duplicate];
            return error_0301_duplicate_binding(
                lexer->source,
                sema_pattern_span(lexer, pattern),
                lex_symbol(lexer, pattern->a),
                sema_local_span(lexer, ast, previous));
        }
        u32 lowered_symbol =
            discard
                ? sema_mangle_discard_symbol(
                      lexer, current_function_symbol, pattern->token_index)
                : sema_mangle_on_pattern_binder_symbol(lexer,
                                                       current_function_symbol,
                                                       pattern->a,
                                                       pattern->token_index);
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
    case APK_ForValue:
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
            sema->locals[local_index].kind == SLK_Param ||
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
    if (scope_index != sema_no_scope() &&
        scope_index < array_count(sema->scopes)) {
        sema->scopes[scope_index].locals_collected = true;
    }

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

        bool discard = sema_symbol_is_discard(lexer, node->a);
        u32  duplicate_local =
            discard ? sema_no_local()
                    : sema_find_local_in_scope(sema, scope_index, node->a);
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
            discard ? sema_mangle_discard_symbol(
                          lexer, current_function_symbol, node->token_index)
            : kind == SLK_Function
                ? sema_mangle_nested_function_symbol(
                      lexer, current_function_symbol, node->a)
                : node->a;

        if (type_node_index != sema_no_type()) {
            sema_mark_type_expr_nodes_in_scope(
                ast, sema, type_node_index, scope_index);
        }
        if (sema_node_is_type_syntax(ast, value_node_index)) {
            sema_mark_type_expr_nodes_in_scope(
                ast, sema, value_node_index, scope_index);
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
            sema_mark_fn_signature_type_nodes_in_scope(
                ast, sema, value_node_index, scope_index);
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
        if (node->kind == AK_FnStart) {
            i = node->b + 2;
            continue;
        }
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
            sema->node_scope_indices[i] = child_scope;
            if (for_info->iterable_node_index != U32_MAX) {
                if (for_info->index_symbol != U32_MAX) {
                    bool discard =
                        sema_symbol_is_discard(lexer, for_info->index_symbol);
                    u32 duplicate_index = sema_find_local_in_scope(
                        sema, child_scope, for_info->index_symbol);
                    if (discard) {
                        duplicate_index = sema_no_local();
                    }
                    if (duplicate_index != sema_no_local()) {
                        const SemaLocal* previous =
                            &sema->locals[duplicate_index];
                        return error_0301_duplicate_binding(
                            lexer->source,
                            sema_node_span(lexer, node),
                            lex_symbol(lexer, for_info->index_symbol),
                            sema_local_span(lexer, ast, previous));
                    }
                    array_push(
                        sema->locals,
                        (SemaLocal){
                            .kind             = SLK_Variable,
                            .symbol_handle    = for_info->index_symbol,
                            .owner_decl_index = owner_decl_index,
                            .scope_index      = child_scope,
                            .decl_node_index  = i,
                            .decl_token_index = for_info->index_token_index,
                            .type_node_index  = sema_no_type(),
                            .value_node_index = sema_no_decl(),
                            .type_index = sema_builtin_type(sema, STK_Usize),
                            .lowered_symbol_handle =
                                discard ? sema_mangle_discard_symbol(
                                              lexer,
                                              current_function_symbol,
                                              for_info->index_token_index)
                                        : sema_mangle_for_item_symbol(
                                              lexer,
                                              current_function_symbol,
                                              for_info->index_symbol,
                                              for_info->index_token_index),
                        });
                    sema->scopes[child_scope].local_count++;
                }
                bool discard =
                    sema_symbol_is_discard(lexer, for_info->item_symbol);
                u32 duplicate_index =
                    discard ? sema_no_local()
                            : sema_find_local_in_scope(
                                  sema, child_scope, for_info->item_symbol);
                if (duplicate_index != sema_no_local()) {
                    const SemaLocal* previous = &sema->locals[duplicate_index];
                    return error_0301_duplicate_binding(
                        lexer->source,
                        sema_node_span(lexer, node),
                        lex_symbol(lexer, for_info->item_symbol),
                        sema_local_span(lexer, ast, previous));
                }
                array_push(sema->locals,
                           (SemaLocal){
                               .kind             = SLK_Variable,
                               .symbol_handle    = for_info->item_symbol,
                               .owner_decl_index = owner_decl_index,
                               .scope_index      = child_scope,
                               .decl_node_index  = i,
                               .decl_token_index = for_info->item_token_index,
                               .type_node_index  = sema_no_type(),
                               .value_node_index = sema_no_decl(),
                               .type_index       = sema_no_type(),
                               .lowered_symbol_handle =
                                   discard ? sema_mangle_discard_symbol(
                                                 lexer,
                                                 current_function_symbol,
                                                 for_info->item_token_index)
                                           : sema_mangle_for_item_symbol(
                                                 lexer,
                                                 current_function_symbol,
                                                 for_info->item_symbol,
                                                 for_info->item_token_index),
                           });
                sema->scopes[child_scope].local_count++;
            }
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
            if (for_info->iterable_node_index != U32_MAX &&
                !sema_resolve_node_refs(lexer,
                                        ast,
                                        owner_decl_index,
                                        current_function_symbol,
                                        sema_no_scope(),
                                        scope_index,
                                        for_info->iterable_node_index,
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
                sema_mark_type_expr_nodes_in_scope(
                    ast, sema, type_node_index, scope_index);
                if (!sema_resolve_node_refs(lexer,
                                            ast,
                                            owner_decl_index,
                                            current_function_symbol,
                                            sema_no_scope(),
                                            scope_index,
                                            type_node_index,
                                            sema)) {
                    return false;
                }
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
            bool discard = sema_symbol_is_discard(lexer, node->a);
            u32  duplicate_index =
                discard ? sema_no_local()
                        : sema_find_local_in_scope(sema, scope_index, node->a);
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
            } else if (payload->kind == AK_ZeroInit ||
                       payload->kind == AK_Undefined) {
                type_node_index  = payload->a;
                value_node_index = sema_no_decl();
            }

            if (type_node_index != sema_no_type()) {
                sema_mark_type_expr_nodes_in_scope(
                    ast, sema, type_node_index, scope_index);
                if (!sema_resolve_node_refs(lexer,
                                            ast,
                                            owner_decl_index,
                                            current_function_symbol,
                                            sema_no_scope(),
                                            scope_index,
                                            type_node_index,
                                            sema)) {
                    return false;
                }
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
                           .kind             = SLK_Variable,
                           .symbol_handle    = node->a,
                           .owner_decl_index = owner_decl_index,
                           .scope_index      = scope_index,
                           .decl_node_index  = i,
                           .decl_token_index = U32_MAX,
                           .type_node_index  = type_node_index,
                           .value_node_index = value_node_index,
                           .type_index       = sema_no_type(),
                           .lowered_symbol_handle =
                               discard ? sema_mangle_discard_symbol(
                                             lexer,
                                             current_function_symbol,
                                             node->token_index)
                                       : node->a,
                       });
            sema->node_local_indices[i] = (u32)array_count(sema->locals) - 1;
            sema->scopes[scope_index].local_count++;
            i++;
            continue;
        }

        if (node->kind == AK_Assign && !sema_node_is_named_call_arg(ast, i)) {
            if (!sema_resolve_node_refs(lexer,
                                        ast,
                                        owner_decl_index,
                                        current_function_symbol,
                                        sema_no_scope(),
                                        scope_index,
                                        node->a,
                                        sema) ||
                !sema_resolve_node_refs(lexer,
                                        ast,
                                        owner_decl_index,
                                        current_function_symbol,
                                        sema_no_scope(),
                                        scope_index,
                                        node->b,
                                        sema)) {
                return false;
            }

            const AstNode* target = &ast->nodes[node->a];
            if (target->kind == AK_SymbolRef) {
                u32 local_index = sema->node_local_indices[node->a];
                if (local_index != sema_no_local()) {
                    if (sema->locals[local_index].kind == SLK_Param ||
                        sema->locals[local_index].kind == SLK_Binder ||
                        !sema_local_is_runtime_value(
                            &sema->locals[local_index])) {
                        return error_0305_invalid_assignment_target(
                            lexer->source,
                            sema_node_span(lexer, target),
                            lex_symbol(lexer, target->a));
                    }
                    sema->node_local_indices[i] = local_index;
                } else {
                    u32 decl_index = sema->node_decl_indices[node->a];
                    if (decl_index == sema_no_decl()) {
                        return error_0300_unknown_symbol(
                            lexer->source,
                            sema_node_span(lexer, target),
                            lex_symbol(lexer, target->a));
                    }
                    sema->node_decl_indices[i] = decl_index;
                }
            }
            if (i + 2 < end_node && ast->nodes[i + 1].kind == AK_Expression &&
                ast->nodes[i + 1].a == i &&
                ast->nodes[i + 2].kind == AK_Statement &&
                ast->nodes[i + 2].a == i + 1) {
                i += 3;
            } else {
                i++;
            }
            continue;
        }

        if (node->kind != AK_Return && node->kind != AK_Statement &&
            node->kind != AK_For && node->kind != AK_Break &&
            node->kind != AK_Continue && node->kind != AK_Assert) {
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

internal bool sema_ensure_block_scope_collected(const Lexer* lexer,
                                                const Ast*   ast,
                                                u32          owner_decl_index,
                                                u32   current_function_symbol,
                                                u32   scope_index,
                                                u32   first_node,
                                                u32   end_node,
                                                Sema* sema)
{
    if (scope_index == sema_no_scope() ||
        scope_index >= array_count(sema->scopes) ||
        sema->scopes[scope_index].locals_collected) {
        return true;
    }

    return sema_collect_block_statements(lexer,
                                         ast,
                                         owner_decl_index,
                                         current_function_symbol,
                                         scope_index,
                                         first_node,
                                         end_node,
                                         sema);
}

internal u32 sema_node_find_symbol_ref(const Ast* ast,
                                       u32        node_index,
                                       u32        symbol_handle)
{
    if (node_index == U32_MAX || node_index >= array_count(ast->nodes)) {
        return U32_MAX;
    }

    const AstNode* node = &ast->nodes[node_index];
    switch (node->kind) {
    case AK_SymbolRef:
        return node->a == symbol_handle ? node_index : U32_MAX;
    case AK_Expression:
    case AK_Statement:
    case AK_IntegerNegate:
    case AK_LogicalNot:
    case AK_ErrorInject:
    case AK_Propagate:
    case AK_BitwiseNot:
    case AK_AddressOf:
    case AK_Deref:
        return sema_node_find_symbol_ref(ast, node->a, symbol_handle);
    case AK_Call:
        {
            u32 found = sema_node_find_symbol_ref(ast, node->a, symbol_handle);
            if (found != U32_MAX) {
                return found;
            }
            const AstCallInfo* call = &ast->calls[node->b];
            for (u32 i = 0; i < call->arg_count; ++i) {
                found = sema_node_find_symbol_ref(
                    ast, ast->call_args[call->first_arg + i], symbol_handle);
                if (found != U32_MAX) {
                    return found;
                }
            }
            return U32_MAX;
        }
    case AK_Field:
    case AK_TupleField:
        return sema_node_find_symbol_ref(ast, node->a, symbol_handle);
    case AK_Index:
    case AK_RangeExclusive:
    case AK_RangeInclusive:
    case AK_IntegerPlus:
    case AK_IntegerMinus:
    case AK_IntegerMultiply:
    case AK_IntegerDivide:
    case AK_IntegerModulo:
    case AK_BitwiseAnd:
    case AK_BitwiseXor:
    case AK_BitwiseOr:
    case AK_ShiftLeft:
    case AK_ShiftRight:
    case AK_Equal:
    case AK_NotEqual:
    case AK_Less:
    case AK_LessEqual:
    case AK_Greater:
    case AK_GreaterEqual:
    case AK_LogicalAnd:
    case AK_LogicalOr:
        {
            u32 found = sema_node_find_symbol_ref(ast, node->a, symbol_handle);
            return found != U32_MAX
                       ? found
                       : sema_node_find_symbol_ref(ast, node->b, symbol_handle);
        }
    default:
        return U32_MAX;
    }
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
    if (signature->generic_params_index != U32_MAX &&
        g_sema_type_subst.count == 0) {
        return error_0339_generics_not_implemented(
            lexer->source,
            sema_node_span(lexer, fn_def),
            s("generic function"));
    }

    bool seen_default = false;
    for (u32 i = 0; i < signature->param_count; ++i) {
        const AstParam* param = &ast->params[signature->first_param + i];
        if (param->default_node_index != U32_MAX) {
            seen_default = true;
        } else if (seen_default) {
            return error_0336_required_param_after_default(
                lexer->source,
                sema_token_span(lexer, param->token_index),
                lex_symbol(lexer, param->symbol_handle));
        }

        bool discard = sema_symbol_is_discard(lexer, param->symbol_handle);
        u32  duplicate_index =
            discard ? sema_no_local()
                    : sema_find_local_in_scope(
                          sema, scope_index, param->symbol_handle);
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
        array_push(
            sema->locals,
            (SemaLocal){
                .kind                  = SLK_Param,
                .symbol_handle         = param->symbol_handle,
                .owner_decl_index      = owner_decl_index,
                .scope_index           = scope_index,
                .decl_node_index       = sema_no_decl(),
                .decl_token_index      = param->token_index,
                .type_node_index       = param->type_node_index,
                .value_node_index      = sema_no_decl(),
                .type_index            = param_type,
                .lowered_symbol_handle = discard ? sema_mangle_discard_symbol(
                                                       lexer,
                                                       current_function_symbol,
                                                       param->token_index)
                                                 : param->symbol_handle,
            });
        sema->scopes[scope_index].local_count++;

        if (param->default_node_index != U32_MAX) {
            for (u32 later = i; later < signature->param_count; ++later) {
                const AstParam* later_param =
                    &ast->params[signature->first_param + later];
                u32 ref_node = sema_node_find_symbol_ref(
                    ast, param->default_node_index, later_param->symbol_handle);
                if (ref_node != U32_MAX) {
                    return error_0338_default_param_later_reference(
                        lexer->source,
                        sema_node_span(lexer, &ast->nodes[ref_node]),
                        lex_symbol(lexer, later_param->symbol_handle));
                }
            }

            if (!sema_resolve_node_refs(lexer,
                                        ast,
                                        owner_decl_index,
                                        current_function_symbol,
                                        capture_scope_index,
                                        scope_index,
                                        param->default_node_index,
                                        sema)) {
                return false;
            }

            u32 default_type = sema_no_type();
            if (!sema_infer_node_type(lexer,
                                      ast,
                                      sema,
                                      param->default_node_index,
                                      param_type,
                                      &default_type)) {
                return false;
            }
            if (!sema_type_matches(sema, param_type, default_type)) {
                return error_0304_type_mismatch(
                    lexer->source,
                    sema_node_span(lexer,
                                   &ast->nodes[param->default_node_index]),
                    sema_type_name(lexer, sema, &temp_arena, param_type),
                    sema_type_name(lexer, sema, &temp_arena, default_type));
            }
        }
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
    case APK_ForValue:
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
    if (sema->node_is_type_expr[node_index]) {
        sema->node_scope_indices[node_index] = scope_index;
        if (node->kind == AK_TypeDynamicArray && node->a != U32_MAX) {
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
        }
        return true;
    }

    switch (node->kind) {
    case AK_IntegerLiteral:
    case AK_FloatLiteral:
    case AK_StringLiteral:
    case AK_BuiltinMacro:
    case AK_BoolLiteral:
    case AK_NilLiteral:
    case AK_EnumVariant:
    case AK_ZeroInit:
    case AK_Undefined:
    case AK_Continue:
    case AK_ContinueExpr:
        return true;
    case AK_Block:
        {
            u32 block_scope = sema->node_scope_indices[node_index];
            if (block_scope == sema_no_scope()) {
                block_scope =
                    sema_add_scope(sema, owner_decl_index, scope_index);
                sema->node_scope_indices[node_index] = block_scope;
            } else if (block_scope < array_count(sema->scopes)) {
                sema->scopes[block_scope].parent_scope_index = scope_index;
            }
            if (!sema_ensure_block_scope_collected(lexer,
                                                   ast,
                                                   owner_decl_index,
                                                   current_function_symbol,
                                                   block_scope,
                                                   node->a,
                                                   node->b,
                                                   sema)) {
                return false;
            }
            for (u32 i = node->a; i < node->b; ++i) {
                if (!ast_node_is_block_statement(&ast->nodes[i])) {
                    continue;
                }
                if (!sema_resolve_node_refs(lexer,
                                            ast,
                                            owner_decl_index,
                                            current_function_symbol,
                                            capture_scope_index,
                                            block_scope,
                                            i,
                                            sema)) {
                    return false;
                }
                i = ast_block_statement_end_exclusive(ast, i) - 1;
            }
            return true;
        }
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
            u32 block_scope = sema->node_scope_indices[node->a];
            if (block_scope == sema_no_scope()) {
                block_scope =
                    sema_add_scope(sema, owner_decl_index, scope_index);
                sema->node_scope_indices[node->a] = block_scope;
            } else if (block_scope < array_count(sema->scopes)) {
                sema->scopes[block_scope].parent_scope_index = scope_index;
            }
            if (!sema_ensure_block_scope_collected(lexer,
                                                   ast,
                                                   owner_decl_index,
                                                   current_function_symbol,
                                                   block_scope,
                                                   block->a,
                                                   block->b,
                                                   sema)) {
                return false;
            }
            for (u32 i = block->a; i < block->b; ++i) {
                if (!ast_node_is_block_statement(&ast->nodes[i])) {
                    continue;
                }
                if (!sema_resolve_node_refs(lexer,
                                            ast,
                                            owner_decl_index,
                                            current_function_symbol,
                                            capture_scope_index,
                                            block_scope,
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
    case AK_Variable:
        {
            u32 value_node_index = node->b;
            if (value_node_index >= array_count(ast->nodes)) {
                return true;
            }
            if (ast->nodes[value_node_index].kind == AK_AnnotatedValue) {
                sema_mark_type_expr_nodes(
                    ast, sema, ast->nodes[value_node_index].a);
                value_node_index = ast->nodes[value_node_index].b;
            }
            if (value_node_index >= array_count(ast->nodes) ||
                ast->nodes[value_node_index].kind == AK_ZeroInit ||
                ast->nodes[value_node_index].kind == AK_Undefined) {
                return true;
            }
            return sema_resolve_node_refs(lexer,
                                          ast,
                                          owner_decl_index,
                                          current_function_symbol,
                                          capture_scope_index,
                                          scope_index,
                                          value_node_index,
                                          sema);
        }
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
    case AK_ShiftLeft:
    case AK_ShiftRight:
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
                u32 branch_scope = scope_index;
                if (branch->binder_symbol_handle != U32_MAX ||
                    !(branch->flags & AOBF_Else)) {
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
            const AstNode* callee = &ast->nodes[node->a];
            if (callee->kind == AK_Index) {
                const AstNode* generic_target = &ast->nodes[callee->a];
                bool explicit_generic_call = generic_target->kind == AK_Field;
                if (generic_target->kind == AK_SymbolRef) {
                    u32 decl_index = sema_find_decl(sema, generic_target->a);
                    explicit_generic_call =
                        decl_index != sema_no_decl() &&
                        sema->decls[decl_index].kind == SK_GenericFunction;
                }
                if (explicit_generic_call) {
                    sema_mark_type_expr_nodes(ast, sema, callee->b);
                }
            }

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
                u32 arg_node = ast->call_args[call->first_arg + i];
                if (ast->nodes[arg_node].kind == AK_Assign) {
                    arg_node = ast->nodes[arg_node].b;
                }
                if (!sema_resolve_node_refs(lexer,
                                            ast,
                                            owner_decl_index,
                                            current_function_symbol,
                                            capture_scope_index,
                                            scope_index,
                                            arg_node,
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
                if (literal->target_node_index != U32_MAX) {
                    sema_mark_type_expr_nodes(
                        ast, sema, literal->target_node_index);
                }
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
        {
            const AstNode* target = &ast->nodes[node->a];
            if (target->kind == AK_SymbolRef) {
                if (string_eq(lex_symbol(lexer, target->a), s("box"))) {
                    sema_mark_type_expr_nodes(ast, sema, node->b);
                    return sema_resolve_node_refs(lexer,
                                                  ast,
                                                  owner_decl_index,
                                                  current_function_symbol,
                                                  capture_scope_index,
                                                  scope_index,
                                                  node->b,
                                                  sema);
                }
                u32 decl_index = sema_find_decl(sema, target->a);
                if (decl_index != sema_no_decl() &&
                    sema->decls[decl_index].kind == SK_Trait) {
                    sema_mark_type_expr_nodes(ast, sema, node->b);
                }
            }
        }
        // fallthrough
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
    case AK_ErrorInject:
    case AK_Propagate:
    case AK_BitwiseNot:
    case AK_AddressOf:
    case AK_Deref:
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
    case AK_Cast:
        {
            const AstCastInfo* cast = sema_cast_info(ast, node);
            return sema_resolve_node_refs(lexer,
                                          ast,
                                          owner_decl_index,
                                          current_function_symbol,
                                          capture_scope_index,
                                          scope_index,
                                          node->a,
                                          sema) &&
                   (cast->extra_node_index == U32_MAX ||
                    sema_resolve_node_refs(lexer,
                                           ast,
                                           owner_decl_index,
                                           current_function_symbol,
                                           capture_scope_index,
                                           scope_index,
                                           cast->extra_node_index,
                                           sema));
        }
    case AK_FfiDef:
        {
            const AstFfiInfo* ffi_info = &ast->ffi_infos[node->a];
            if (ffi_info->library_node_index == U32_MAX) {
                return true;
            }
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
    case AK_Assign:
        if (!sema_node_is_named_call_arg(ast, node_index) &&
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
        return sema_resolve_node_refs(lexer,
                                      ast,
                                      owner_decl_index,
                                      current_function_symbol,
                                      capture_scope_index,
                                      scope_index,
                                      node->b,
                                      sema);
    case AK_Assert:
        return sema_resolve_node_refs(lexer,
                                      ast,
                                      owner_decl_index,
                                      current_function_symbol,
                                      capture_scope_index,
                                      scope_index,
                                      node->a,
                                      sema) &&
               (node->b == U32_MAX ||
                sema_resolve_node_refs(lexer,
                                       ast,
                                       owner_decl_index,
                                       current_function_symbol,
                                       capture_scope_index,
                                       scope_index,
                                       node->b,
                                       sema));
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
            if (for_info->iterable_node_index != U32_MAX &&
                !sema_resolve_node_refs(lexer,
                                        ast,
                                        owner_decl_index,
                                        current_function_symbol,
                                        capture_scope_index,
                                        scope_index,
                                        for_info->iterable_node_index,
                                        sema)) {
                return false;
            }
            return for_info->condition_node_index == U32_MAX ||
                   sema_resolve_node_refs(lexer,
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
            if (sema->decls[decl_index].kind == SK_TypeAlias ||
                sema->decls[decl_index].kind == SK_GenericTypeAlias) {
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

internal void sema_collect_address_deps(const Ast*  ast,
                                        const Sema* sema,
                                        u32         owner_decl_index,
                                        u32         node_index,
                                        Sema*       out_sema);

internal bool sema_address_path_targets_decl(const Ast*  ast,
                                             const Sema* sema,
                                             u32         node_index,
                                             u32         target_decl_index)
{
    node_index          = sema_unwrap_expr_node(ast, node_index);
    const AstNode* node = &ast->nodes[node_index];

    switch (node->kind) {
    case AK_SymbolRef:
        return sema->node_local_indices[node_index] == sema_no_local() &&
               sema->node_decl_indices[node_index] == target_decl_index;
    case AK_Field:
    case AK_TupleField:
        return sema_address_path_targets_decl(
            ast, sema, node->a, target_decl_index);
    case AK_Index:
        if (!sema_expr_is_constantish(ast, sema, node->b)) {
            return false;
        }
        return sema_address_path_targets_decl(
            ast, sema, node->a, target_decl_index);
    default:
        return false;
    }
}

internal bool sema_decl_is_pointer_alias_to_decl(const Ast*  ast,
                                                 const Sema* sema,
                                                 u32         decl_index,
                                                 u32         target_decl_index)
{
    if (decl_index == sema_no_decl() ||
        decl_index >= array_count(sema->decls)) {
        return false;
    }
    const SemaDecl* decl = &sema->decls[decl_index];
    if (decl->kind != SK_Constant || decl->value_node_index == sema_no_decl()) {
        return false;
    }

    u32 value_node_index = sema_unwrap_expr_node(ast, decl->value_node_index);
    if (value_node_index >= array_count(ast->nodes) ||
        ast->nodes[value_node_index].kind != AK_AddressOf) {
        return false;
    }
    return sema_address_path_targets_decl(
        ast, sema, ast->nodes[value_node_index].a, target_decl_index);
}

internal void sema_collect_pattern_deps(const Ast*  ast,
                                        const Sema* sema,
                                        u32         owner_decl_index,
                                        u32         pattern_index,
                                        Sema*       out_sema)
{
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
        if (pattern->c != 0) {
            sema_collect_node_deps(
                ast, sema, owner_decl_index, pattern->c - 1, out_sema);
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
    case AK_BuiltinMacro:
    case AK_BoolLiteral:
    case AK_NilLiteral:
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
                u32 arg_node = ast->call_args[call->first_arg + i];
                if (ast->nodes[arg_node].kind == AK_Assign) {
                    arg_node = ast->nodes[arg_node].b;
                }
                sema_collect_node_deps(
                    ast, sema, owner_decl_index, arg_node, out_sema);
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
        sema_collect_address_deps(
            ast, sema, owner_decl_index, node->a, out_sema);
        return;
    case AK_TypeSlice:
    case AK_TypePointer:
        sema_collect_node_deps(ast, sema, owner_decl_index, node->a, out_sema);
        return;
    case AK_FfiDef:
        {
            const AstFfiInfo* ffi_info = &ast->ffi_infos[node->a];
            if (ffi_info->library_node_index == U32_MAX) {
                return;
            }
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
            if (sema_decl_is_pointer_alias_to_decl(
                    ast, sema, decl_index, owner_decl_index)) {
                return;
            }
            sema_add_dep(out_sema, owner_decl_index, decl_index);
            return;
        }
    case AK_IntegerNegate:
    case AK_LogicalNot:
    case AK_ErrorInject:
    case AK_Propagate:
    case AK_BitwiseNot:
    case AK_Expression:
    case AK_Statement:
    case AK_Use:
        sema_collect_node_deps(ast, sema, owner_decl_index, node->a, out_sema);
        return;
    case AK_Cast:
        {
            const AstCastInfo* cast = sema_cast_info(ast, node);
            sema_collect_node_deps(
                ast, sema, owner_decl_index, node->a, out_sema);
            if (cast->extra_node_index != U32_MAX) {
                sema_collect_node_deps(ast,
                                       sema,
                                       owner_decl_index,
                                       cast->extra_node_index,
                                       out_sema);
            }
            return;
        }
    case AK_Return:
    case AK_ReturnExpr:
        if (node->a != U32_MAX) {
            sema_collect_node_deps(
                ast, sema, owner_decl_index, node->a, out_sema);
        }
        return;
    case AK_Assert:
        sema_collect_node_deps(ast, sema, owner_decl_index, node->a, out_sema);
        if (node->b != U32_MAX) {
            sema_collect_node_deps(
                ast, sema, owner_decl_index, node->b, out_sema);
        }
        return;
    case AK_Assign:
        sema_collect_node_deps(ast, sema, owner_decl_index, node->a, out_sema);
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
            ast->nodes[node->b].kind != AK_ZeroInit &&
            ast->nodes[node->b].kind != AK_Undefined) {
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
    case AK_ShiftLeft:
    case AK_ShiftRight:
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
            const AstFnSignature* signature = &ast->fn_signatures[fn_start->a];
            for (u32 i = 0; i < signature->param_count; ++i) {
                const AstParam* param =
                    &ast->params[signature->first_param + i];
                if (param->default_node_index != U32_MAX) {
                    sema_collect_node_deps(ast,
                                           sema,
                                           owner_decl_index,
                                           param->default_node_index,
                                           out_sema);
                }
            }
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

internal void sema_collect_address_deps(const Ast*  ast,
                                        const Sema* sema,
                                        u32         owner_decl_index,
                                        u32         node_index,
                                        Sema*       out_sema)
{
    const AstNode* node = &ast->nodes[node_index];

    switch (node->kind) {
    case AK_TypeNever:
        return;

    case AK_SymbolRef:
        {
            if (sema->node_is_type_expr[node_index]) {
                return;
            }
            if (sema->node_local_indices[node_index] != sema_no_local()) {
                return;
            }
            u32 decl_index = sema->node_decl_indices[node_index];
            if (decl_index == owner_decl_index) {
                return;
            }
            sema_collect_node_deps(
                ast, sema, owner_decl_index, node_index, out_sema);
            return;
        }
    case AK_TupleField:
    case AK_Field:
        sema_collect_address_deps(
            ast, sema, owner_decl_index, node->a, out_sema);
        return;
    case AK_Index:
        sema_collect_address_deps(
            ast, sema, owner_decl_index, node->a, out_sema);
        sema_collect_node_deps(ast, sema, owner_decl_index, node->b, out_sema);
        return;
    default:
        sema_collect_node_deps(
            ast, sema, owner_decl_index, node_index, out_sema);
        return;
    }
}

//------------------------------------------------------------------------------
// Collect dependency edges for all top-level declarations.

internal void
sema_collect_deps(const Ast* ast, const Sema* sema, Sema* out_sema)
{
    for (u32 i = 0; i < array_count(sema->decls); ++i) {
        if (sema->decls[i].kind == SK_TypeAlias ||
            sema->decls[i].kind == SK_GenericTypeAlias) {
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
        if (decl->kind == SK_GenericTypeAlias ||
            decl->kind == SK_GenericFunction) {
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

internal bool sema_symbol_list_contains(Array(u32) symbols, u32 symbol)
{
    for (u32 i = 0; i < array_count(symbols); ++i) {
        if (symbols[i] == symbol) {
            return true;
        }
    }
    return false;
}

internal void sema_symbol_list_add_unique(Array(u32) * symbols, u32 symbol)
{
    if (symbol == U32_MAX || sema_symbol_list_contains(*symbols, symbol)) {
        return;
    }
    array_push(*symbols, symbol);
}

internal bool sema_generic_param_has_trait_constraint(const Ast*   ast,
                                                      const Lexer* lexer,
                                                      u32          node_index,
                                                      u32          param_symbol,
                                                      string       trait_name);

internal bool sema_enum_variant_symbol_matches(const Lexer* variant_lexer,
                                               u32          variant_symbol,
                                               const Lexer* target_lexer,
                                               u32          target_symbol)
{
    if (variant_lexer == target_lexer) {
        return variant_symbol == target_symbol;
    }
    return string_eq(lex_symbol(variant_lexer, variant_symbol),
                     lex_symbol(target_lexer, target_symbol));
}

internal bool sema_type_node_has_enum_variant_symbol(const Lexer* type_lexer,
                                                     const Ast*   type_ast,
                                                     u32          node_index,
                                                     const Lexer* target_lexer,
                                                     u32          target_symbol)
{
    if (node_index == U32_MAX || node_index >= array_count(type_ast->nodes)) {
        return false;
    }

    const AstNode* node = &type_ast->nodes[node_index];
    switch (node->kind) {
    case AK_Expression:
    case AK_Statement:
    case AK_AnnotatedValue:
        return sema_type_node_has_enum_variant_symbol(
            type_lexer, type_ast, node->a, target_lexer, target_symbol);
    case AK_TypeEnum:
        {
            const AstEnumTypeInfo* enum_type = &type_ast->enum_types[node->a];
            for (u32 i = 0; i < enum_type->variant_count; ++i) {
                const AstEnumVariant* variant =
                    &type_ast->enum_variants[enum_type->first_variant + i];
                if (sema_enum_variant_symbol_matches(type_lexer,
                                                     variant->symbol_handle,
                                                     target_lexer,
                                                     target_symbol)) {
                    return true;
                }
            }
            return false;
        }
    default:
        return false;
    }
}

internal bool sema_decl_type_node_has_enum_variant_symbol(const Lexer* lexer,
                                                          const Ast*   ast,
                                                          const Sema*  sema,
                                                          u32 decl_index,
                                                          u32 symbol)
{
    const Lexer* source_lexer      = lexer;
    const Ast*   source_ast        = ast;
    const Sema*  source_sema       = sema;
    u32          source_decl_index = decl_index;

    if (decl_index >= array_count(sema->decls)) {
        return false;
    }

    const SemaDecl* decl = &sema->decls[decl_index];
    if (decl->import_module_index != sema_no_decl() &&
        decl->import_decl_index != sema_no_decl()) {
        if (sema->program == NULL ||
            decl->import_module_index >= array_count(sema->program->modules)) {
            return false;
        }
        const ModuleInfo* module =
            &sema->program->modules[decl->import_module_index];
        source_lexer      = &module->front_end.lexer;
        source_ast        = &module->front_end.ast;
        source_sema       = &module->front_end.sema;
        source_decl_index = decl->import_decl_index;
    }

    if (source_decl_index >= array_count(source_sema->decls)) {
        return false;
    }

    const SemaDecl* source_decl = &source_sema->decls[source_decl_index];
    if (source_decl->kind != SK_TypeAlias &&
        source_decl->kind != SK_GenericTypeAlias) {
        return false;
    }

    return sema_type_node_has_enum_variant_symbol(
        source_lexer, source_ast, source_decl->value_node_index, lexer, symbol);
}

internal bool sema_symbol_is_known_enum_variant(const Lexer* lexer,
                                                const Ast*   ast,
                                                const Sema*  sema,
                                                u32          symbol)
{
    for (u32 i = 0; i < array_count(sema->types); ++i) {
        if (sema->types[i].kind == STK_Enum &&
            sema_enum_variant_index(sema, i, symbol) != U32_MAX) {
            return true;
        }
    }
    for (u32 i = 0; i < array_count(sema->decls); ++i) {
        if (sema_decl_type_node_has_enum_variant_symbol(
                lexer, ast, sema, i, symbol)) {
            return true;
        }
    }
    return false;
}

internal u32 sema_generic_param_symbol_from_type_node(const Ast* ast,
                                                      Array(u32) generic_params,
                                                      u32 type_node_index)
{
    if (type_node_index == U32_MAX ||
        type_node_index >= array_count(ast->nodes)) {
        return U32_MAX;
    }

    const AstNode* type_node = &ast->nodes[type_node_index];
    while (
        (type_node->kind == AK_Expression || type_node->kind == AK_Statement) &&
        type_node->a < array_count(ast->nodes)) {
        type_node_index = type_node->a;
        type_node       = &ast->nodes[type_node_index];
    }

    if (type_node->kind != AK_SymbolRef ||
        !sema_symbol_list_contains(generic_params, type_node->a)) {
        return U32_MAX;
    }
    return type_node->a;
}

internal u32 sema_generic_param_symbol_from_value_node(
    const Ast* ast, const Sema* sema, Array(u32) generic_params, u32 node_index)
{
    if (node_index == U32_MAX || node_index >= array_count(ast->nodes)) {
        return U32_MAX;
    }

    const AstNode* node = &ast->nodes[node_index];
    while ((node->kind == AK_Expression || node->kind == AK_Statement) &&
           node->a < array_count(ast->nodes)) {
        node_index = node->a;
        node       = &ast->nodes[node_index];
    }

    if (node->kind != AK_SymbolRef) {
        return U32_MAX;
    }

    if (node_index < array_count(sema->node_local_indices)) {
        u32 local_index = sema->node_local_indices[node_index];
        if (local_index != sema_no_local() &&
            local_index < array_count(sema->locals)) {
            return sema_generic_param_symbol_from_type_node(
                ast, generic_params, sema->locals[local_index].type_node_index);
        }
    }

    u32 fn_start_index =
        sema_ast_enclosing_function_start_node(ast, node_index);
    if (fn_start_index == U32_MAX ||
        fn_start_index >= array_count(ast->nodes)) {
        return U32_MAX;
    }
    const AstNode* fn_start = &ast->nodes[fn_start_index];
    if (fn_start->kind != AK_FnStart ||
        fn_start->a >= array_count(ast->fn_signatures)) {
        return U32_MAX;
    }

    const AstFnSignature* signature = &ast->fn_signatures[fn_start->a];
    for (u32 i = 0; i < signature->param_count; ++i) {
        const AstParam* param = &ast->params[signature->first_param + i];
        if (param->symbol_handle == node->a) {
            return sema_generic_param_symbol_from_type_node(
                ast, generic_params, param->type_node_index);
        }
    }
    return U32_MAX;
}

internal bool
sema_validate_generic_body_binary_trait_constraint(const Lexer* lexer,
                                                   const Ast*   ast,
                                                   const Sema*  sema,
                                                   Array(u32) generic_params,
                                                   u32 node_index)
{
    const AstNode* node       = &ast->nodes[node_index];
    string         trait_name = {0};
    switch (node->kind) {
    case AK_Equal:
    case AK_NotEqual:
        trait_name = s("Eq");
        break;
    case AK_Less:
    case AK_LessEqual:
    case AK_Greater:
    case AK_GreaterEqual:
        trait_name = s("Order");
        break;
    default:
        return true;
    }

    u32 lhs_param = sema_generic_param_symbol_from_value_node(
        ast, sema, generic_params, node->a);
    u32 rhs_param = sema_generic_param_symbol_from_value_node(
        ast, sema, generic_params, node->b);
    if (lhs_param == U32_MAX || lhs_param != rhs_param) {
        return true;
    }

    if (sema_generic_param_has_trait_constraint(
            ast, lexer, node_index, lhs_param, trait_name)) {
        return true;
    }

    return error_0304_type_mismatch(
        lexer->source,
        sema_node_span(lexer, &ast->nodes[node->a]),
        string_format(&temp_arena, STRINGP " constraint", STRINGV(trait_name)),
        lex_symbol(lexer, lhs_param));
}

internal void sema_collect_pattern_symbols(const Ast* ast,
                                           u32        pattern_index,
                                           Array(u32) * out_symbols)
{
    if (pattern_index >= array_count(ast->patterns)) {
        return;
    }

    const AstPattern* pattern = &ast->patterns[pattern_index];
    switch (pattern->kind) {
    case APK_Bind:
        sema_symbol_list_add_unique(out_symbols, pattern->a);
        if (pattern->b != U32_MAX) {
            sema_collect_pattern_symbols(ast, pattern->b, out_symbols);
        }
        return;
    case APK_Tuple:
        for (u32 i = 0; i < pattern->b; ++i) {
            sema_collect_pattern_symbols(
                ast, ast->pattern_items[pattern->a + i], out_symbols);
        }
        return;
    case APK_Plex:
        for (u32 i = 0; i < pattern->b; ++i) {
            sema_collect_pattern_symbols(
                ast,
                ast->pattern_fields[pattern->a + i].pattern_index,
                out_symbols);
        }
        return;
    case APK_EnumVariant:
        {
            const AstEnumPattern* enum_pattern =
                &ast->enum_patterns[pattern->a];
            for (u32 i = 0; i < enum_pattern->pattern_count; ++i) {
                sema_collect_pattern_symbols(
                    ast,
                    ast->pattern_items[enum_pattern->first_pattern + i],
                    out_symbols);
            }
            return;
        }
    default:
        return;
    }
}

internal void sema_collect_generic_body_symbols(const Ast* ast,
                                                u32        fn_node_index,
                                                Array(u32) * out_symbols)
{
    const AstNode* fn_def = &ast->nodes[fn_node_index];
    if (fn_def->kind != AK_FnDef) {
        return;
    }

    const AstNode*        fn_start  = &ast->nodes[fn_def->a];
    const AstFnSignature* signature = &ast->fn_signatures[fn_start->a];
    for (u32 i = 0; i < signature->param_count; ++i) {
        const AstParam* param = &ast->params[signature->first_param + i];
        sema_symbol_list_add_unique(out_symbols, param->symbol_handle);
    }

    for (u32 i = fn_def->a + 1; i < fn_start->b; ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind == AK_FnDef) {
            const AstNode* nested_start = &ast->nodes[node->a];
            i                           = nested_start->b;
            continue;
        }

        if (node->kind == AK_Bind || node->kind == AK_Variable) {
            sema_symbol_list_add_unique(out_symbols, node->a);
            continue;
        }
        if (node->kind == AK_DestructureBind ||
            node->kind == AK_DestructureVariable) {
            sema_collect_pattern_symbols(ast, node->a, out_symbols);
            continue;
        }
        if (node->kind == AK_For) {
            const AstForInfo* for_info = &ast->fors[node->a];
            sema_symbol_list_add_unique(out_symbols, for_info->index_symbol);
            sema_symbol_list_add_unique(out_symbols, for_info->item_symbol);
            continue;
        }
        if (node->kind == AK_On) {
            const AstOnInfo* on = &ast->ons[node->b];
            for (u32 branch = 0; branch < on->branch_count; ++branch) {
                const AstOnBranch* on_branch =
                    &ast->on_branches[on->first_branch + branch];
                sema_symbol_list_add_unique(out_symbols,
                                            on_branch->binder_symbol_handle);
                for (u32 pattern = 0; pattern < on_branch->pattern_count;
                     ++pattern) {
                    sema_collect_pattern_symbols(
                        ast,
                        ast->pattern_items[on_branch->pattern_index + pattern],
                        out_symbols);
                }
            }
        }
    }
}

internal bool sema_validate_generic_body_refs_node(const Lexer* lexer,
                                                   const Ast*   ast,
                                                   Sema*        sema,
                                                   Array(u32) locals,
                                                   Array(u32) generic_params,
                                                   u32 node_index);

internal bool sema_validate_generic_body_refs_pattern(const Lexer* lexer,
                                                      const Ast*   ast,
                                                      Sema*        sema,
                                                      Array(u32) locals,
                                                      Array(u32) generic_params,
                                                      u32 pattern_index)
{
    if (pattern_index >= array_count(ast->patterns)) {
        return true;
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
        return sema_validate_generic_body_refs_node(
            lexer, ast, sema, locals, generic_params, pattern->a);
    case APK_RangeExclusive:
    case APK_RangeInclusive:
        return sema_validate_generic_body_refs_node(
                   lexer, ast, sema, locals, generic_params, pattern->a) &&
               sema_validate_generic_body_refs_node(
                   lexer, ast, sema, locals, generic_params, pattern->b);
    case APK_Bind:
        return pattern->b == U32_MAX ||
               sema_validate_generic_body_refs_pattern(
                   lexer, ast, sema, locals, generic_params, pattern->b);
    case APK_Tuple:
        for (u32 i = 0; i < pattern->b; ++i) {
            if (!sema_validate_generic_body_refs_pattern(
                    lexer,
                    ast,
                    sema,
                    locals,
                    generic_params,
                    ast->pattern_items[pattern->a + i])) {
                return false;
            }
        }
        return true;
    case APK_Plex:
        for (u32 i = 0; i < pattern->b; ++i) {
            if (!sema_validate_generic_body_refs_pattern(
                    lexer,
                    ast,
                    sema,
                    locals,
                    generic_params,
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
                if (!sema_validate_generic_body_refs_pattern(
                        lexer,
                        ast,
                        sema,
                        locals,
                        generic_params,
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

internal bool sema_validate_generic_body_refs_node(const Lexer* lexer,
                                                   const Ast*   ast,
                                                   Sema*        sema,
                                                   Array(u32) locals,
                                                   Array(u32) generic_params,
                                                   u32 node_index)
{
    if (node_index == U32_MAX || node_index >= array_count(ast->nodes)) {
        return true;
    }

    const AstNode* node = &ast->nodes[node_index];
    if (sema->node_is_type_expr[node_index]) {
        return true;
    }

    switch (node->kind) {
    case AK_IntegerLiteral:
    case AK_FloatLiteral:
    case AK_StringLiteral:
    case AK_BoolLiteral:
    case AK_NilLiteral:
    case AK_BuiltinMacro:
    case AK_EnumVariant:
    case AK_ZeroInit:
    case AK_Undefined:
    case AK_Continue:
    case AK_ContinueExpr:
    case AK_FnStart:
    case AK_FnEnd:
        return true;
    case AK_SymbolRef:
        if (sema_symbol_list_contains(locals, node->a) ||
            sema_symbol_list_contains(generic_params, node->a) ||
            sema_symbol_is_known_enum_variant(lexer, ast, sema, node->a) ||
            sema_find_decl(sema, node->a) != sema_no_decl()) {
            return true;
        }
        return error_0300_unknown_symbol(lexer->source,
                                         sema_node_span(lexer, node),
                                         lex_symbol(lexer, node->a));
    case AK_Expression:
    case AK_Statement:
    case AK_Return:
    case AK_ReturnExpr:
    case AK_Break:
    case AK_BreakExpr:
    case AK_Defer:
    case AK_ExprBlock:
    case AK_IntegerNegate:
    case AK_LogicalNot:
    case AK_ErrorInject:
    case AK_Propagate:
    case AK_BitwiseNot:
    case AK_AddressOf:
    case AK_Deref:
    case AK_InterpPartExpr:
        return sema_validate_generic_body_refs_node(
            lexer, ast, sema, locals, generic_params, node->a);
    case AK_StringConcat:
    case AK_IntegerPlus:
    case AK_IntegerMinus:
    case AK_IntegerMultiply:
    case AK_IntegerDivide:
    case AK_IntegerModulo:
    case AK_BitwiseAnd:
    case AK_BitwiseXor:
    case AK_BitwiseOr:
    case AK_ShiftLeft:
    case AK_ShiftRight:
    case AK_Equal:
    case AK_NotEqual:
    case AK_Less:
    case AK_LessEqual:
    case AK_Greater:
    case AK_GreaterEqual:
    case AK_LogicalAnd:
    case AK_LogicalOr:
        if (!sema_validate_generic_body_refs_node(
                lexer, ast, sema, locals, generic_params, node->a) ||
            !sema_validate_generic_body_refs_node(
                lexer, ast, sema, locals, generic_params, node->b)) {
            return false;
        }
        return sema_validate_generic_body_binary_trait_constraint(
            lexer, ast, sema, generic_params, node_index);
    case AK_RangeExclusive:
    case AK_RangeInclusive:
    case AK_Index:
        return sema_validate_generic_body_refs_node(
                   lexer, ast, sema, locals, generic_params, node->a) &&
               sema_validate_generic_body_refs_node(
                   lexer, ast, sema, locals, generic_params, node->b);
    case AK_Assign:
        if (!sema_node_is_named_call_arg(ast, node_index) &&
            !sema_validate_generic_body_refs_node(
                lexer, ast, sema, locals, generic_params, node->a)) {
            return false;
        }
        return sema_validate_generic_body_refs_node(
            lexer, ast, sema, locals, generic_params, node->b);
    case AK_Call:
        {
            if (!sema_validate_generic_body_refs_node(
                    lexer, ast, sema, locals, generic_params, node->a)) {
                return false;
            }
            const AstCallInfo* call = &ast->calls[node->b];
            for (u32 i = 0; i < call->arg_count; ++i) {
                if (!sema_validate_generic_body_refs_node(
                        lexer,
                        ast,
                        sema,
                        locals,
                        generic_params,
                        ast->call_args[call->first_arg + i])) {
                    return false;
                }
            }
            return true;
        }
    case AK_Cast:
        {
            const AstCastInfo* cast = &ast->casts[node->b];
            return sema_validate_generic_body_refs_node(
                       lexer, ast, sema, locals, generic_params, node->a) &&
                   (cast->extra_node_index == U32_MAX ||
                    sema_validate_generic_body_refs_node(
                        lexer,
                        ast,
                        sema,
                        locals,
                        generic_params,
                        cast->extra_node_index));
        }
    case AK_Tuple:
    case AK_Array:
        for (u32 i = 0; i < node->b; ++i) {
            if (!sema_validate_generic_body_refs_node(
                    lexer, ast, sema, locals, generic_params, node->a + i)) {
                return false;
            }
        }
        return true;
    case AK_TupleField:
    case AK_Field:
        return sema_validate_generic_body_refs_node(
            lexer, ast, sema, locals, generic_params, node->a);
    case AK_Slice:
        {
            const AstSliceInfo* slice = &ast->slices[node->a];
            return sema_validate_generic_body_refs_node(
                       lexer,
                       ast,
                       sema,
                       locals,
                       generic_params,
                       slice->target_node_index) &&
                   (slice->start_node_index == U32_MAX ||
                    sema_validate_generic_body_refs_node(
                        lexer,
                        ast,
                        sema,
                        locals,
                        generic_params,
                        slice->start_node_index)) &&
                   (slice->end_node_index == U32_MAX ||
                    sema_validate_generic_body_refs_node(
                        lexer,
                        ast,
                        sema,
                        locals,
                        generic_params,
                        slice->end_node_index));
        }
    case AK_InterpolatedString:
        for (u32 i = node->a; i < node->b; ++i) {
            if (!sema_validate_generic_body_refs_node(
                    lexer, ast, sema, locals, generic_params, i)) {
                return false;
            }
        }
        return true;
    case AK_Plex:
    case AK_PlexUpdate:
        {
            const AstPlexLiteralInfo* plex = &ast->plex_literals[node->a];
            if (plex->target_node_index != U32_MAX &&
                !sema_validate_generic_body_refs_node(
                    lexer,
                    ast,
                    sema,
                    locals,
                    generic_params,
                    plex->target_node_index)) {
                return false;
            }
            for (u32 i = 0; i < plex->field_count; ++i) {
                const AstPlexLiteralField* field =
                    &ast->plex_literal_fields[plex->first_field + i];
                if (!sema_validate_generic_body_refs_node(
                        lexer,
                        ast,
                        sema,
                        locals,
                        generic_params,
                        field->value_node_index)) {
                    return false;
                }
            }
            return true;
        }
    case AK_On:
        {
            const AstOnInfo* on = &ast->ons[node->b];
            if (node->a != U32_MAX &&
                !sema_validate_generic_body_refs_node(
                    lexer, ast, sema, locals, generic_params, node->a)) {
                return false;
            }
            for (u32 i = 0; i < on->branch_count; ++i) {
                const AstOnBranch* branch =
                    &ast->on_branches[on->first_branch + i];
                for (u32 pattern = 0; pattern < branch->pattern_count;
                     ++pattern) {
                    if (!sema_validate_generic_body_refs_pattern(
                            lexer,
                            ast,
                            sema,
                            locals,
                            generic_params,
                            ast->pattern_items[branch->pattern_index +
                                               pattern])) {
                        return false;
                    }
                }
                if (branch->guard_node_index != U32_MAX &&
                    !sema_validate_generic_body_refs_node(
                        lexer,
                        ast,
                        sema,
                        locals,
                        generic_params,
                        branch->guard_node_index)) {
                    return false;
                }
                if (!sema_validate_generic_body_refs_node(
                        lexer,
                        ast,
                        sema,
                        locals,
                        generic_params,
                        branch->expr_node_index)) {
                    return false;
                }
            }
            return true;
        }
    case AK_DestructureBind:
    case AK_DestructureVariable:
    case AK_DestructureAssign:
        return sema_validate_generic_body_refs_node(
            lexer, ast, sema, locals, generic_params, node->b);
    case AK_AnnotatedValue:
        return sema_validate_generic_body_refs_node(
            lexer, ast, sema, locals, generic_params, node->b);
    case AK_Block:
        for (u32 i = node->a; i < node->b; ++i) {
            if (!sema_validate_generic_body_refs_node(
                    lexer, ast, sema, locals, generic_params, i)) {
                return false;
            }
            if (ast_node_is_block_statement(&ast->nodes[i])) {
                i = ast_block_statement_end_exclusive(ast, i) - 1;
            }
        }
        return true;
    case AK_For:
        {
            const AstForInfo* for_info = &ast->fors[node->a];
            for (u32 i = 0; i < for_info->init_count; ++i) {
                if (!sema_validate_generic_body_refs_node(lexer,
                                                          ast,
                                                          sema,
                                                          locals,
                                                          generic_params,
                                                          for_info->first_init +
                                                              i)) {
                    return false;
                }
            }
            if (for_info->condition_node_index != U32_MAX &&
                !sema_validate_generic_body_refs_node(
                    lexer,
                    ast,
                    sema,
                    locals,
                    generic_params,
                    for_info->condition_node_index)) {
                return false;
            }
            if (for_info->iterable_node_index != U32_MAX &&
                !sema_validate_generic_body_refs_node(
                    lexer,
                    ast,
                    sema,
                    locals,
                    generic_params,
                    for_info->iterable_node_index)) {
                return false;
            }
            for (u32 i = 0; i < for_info->update_count; ++i) {
                if (!sema_validate_generic_body_refs_node(
                        lexer,
                        ast,
                        sema,
                        locals,
                        generic_params,
                        for_info->first_update + i)) {
                    return false;
                }
            }
            if (!sema_validate_generic_body_refs_node(
                    lexer, ast, sema, locals, generic_params, node->b)) {
                return false;
            }
            return for_info->else_block_index == U32_MAX ||
                   sema_validate_generic_body_refs_node(
                       lexer,
                       ast,
                       sema,
                       locals,
                       generic_params,
                       for_info->else_block_index);
        }
    case AK_FnDef:
        return true;
    case AK_Bind:
    case AK_Variable:
        return sema_validate_generic_body_refs_node(
            lexer, ast, sema, locals, generic_params, node->b);
    case AK_Assert:
        return sema_validate_generic_body_refs_node(
                   lexer, ast, sema, locals, generic_params, node->a) &&
               (node->b == U32_MAX ||
                sema_validate_generic_body_refs_node(
                    lexer, ast, sema, locals, generic_params, node->b));
    case AK_FfiDef:
        {
            const AstFfiInfo* ffi = &ast->ffi_infos[node->a];
            return ffi->library_node_index == U32_MAX ||
                   sema_validate_generic_body_refs_node(
                       lexer,
                       ast,
                       sema,
                       locals,
                       generic_params,
                       ffi->library_node_index);
        }
    default:
        return true;
    }
}

internal bool sema_validate_generic_function_body_refs(const Lexer* lexer,
                                                       const Ast*   ast,
                                                       Sema*        sema,
                                                       u32          decl_index)
{
    const SemaDecl* decl = &sema->decls[decl_index];
    if (decl->kind != SK_GenericFunction ||
        decl->value_node_index == sema_no_decl()) {
        return true;
    }

    const AstNode* fn_def = &ast->nodes[decl->value_node_index];
    if (fn_def->kind != AK_FnDef || fn_def->b != AFK_Block) {
        return true;
    }

    const AstNode*        fn_start  = &ast->nodes[fn_def->a];
    const AstFnSignature* signature = &ast->fn_signatures[fn_start->a];

    Array(u32) generic_params       = NULL;
    if (signature->generic_params_index != U32_MAX) {
        const AstGenericParams* generic =
            &ast->generic_params[signature->generic_params_index];
        for (u32 i = 0; i < generic->symbol_count; ++i) {
            sema_symbol_list_add_unique(
                &generic_params,
                ast->generic_param_symbols[generic->first_symbol + i]);
        }
    }
    u32 impl_node_index =
        sema_enclosing_impl_node_index(ast, decl->value_node_index);
    if (impl_node_index != U32_MAX) {
        const AstImplInfo* impl = &ast->impls[ast->nodes[impl_node_index].a];
        if (impl->generic_params_index != U32_MAX) {
            const AstGenericParams* generic =
                &ast->generic_params[impl->generic_params_index];
            for (u32 i = 0; i < generic->symbol_count; ++i) {
                sema_symbol_list_add_unique(
                    &generic_params,
                    ast->generic_param_symbols[generic->first_symbol + i]);
            }
        }
        sema_symbol_list_add_unique(
            &generic_params, sema_find_symbol_handle_by_name(lexer, s("Self")));
    }

    Array(u32) locals = NULL;
    sema_collect_generic_body_symbols(ast, decl->value_node_index, &locals);
    bool ok = true;
    if (fn_def->b == AFK_Block) {
        for (u32 i = fn_def->a + 1; ok && i < fn_start->b; ++i) {
            ok = sema_validate_generic_body_refs_node(
                lexer, ast, sema, locals, generic_params, i);
            if (ast_node_is_block_statement(&ast->nodes[i])) {
                i = ast_block_statement_end_exclusive(ast, i) - 1;
            }
        }
    } else {
        ok = sema_validate_generic_body_refs_node(
            lexer, ast, sema, locals, generic_params, fn_start->b - 1);
    }
    array_free(locals);
    array_free(generic_params);
    return ok;
}

internal bool
sema_validate_generic_body_refs(const Lexer* lexer, const Ast* ast, Sema* sema)
{
    for (u32 i = 0; i < array_count(sema->decls); ++i) {
        if (!sema_validate_generic_function_body_refs(lexer, ast, sema, i)) {
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
            const AstTopOnInfo* info = &ast->top_ons[node->a];
            if (info->is_assert) {
                if (!sema_validate_top_on_assertion(
                        options, lexer, ast, node)) {
                    return false;
                }
                i++;
                continue;
            }
            const AstNode* body = &ast->nodes[info->body_node_index];
            ASSERT(body->kind == AK_Block, "Expected top-level on body");
            if (sema_top_on_is_enabled(options, lexer, ast, node)) {
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
            i = body->b;
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
    if (string_eq(name, s("arena"))) {
        return sema_builtin_type(sema, STK_Arena);
    }

    return sema_no_type();
}

//------------------------------------------------------------------------------
// Resolve one parsed type node into a semantic type row.

internal bool sema_resolve_type_node_ex(const Lexer*         lexer,
                                        const Ast*           ast,
                                        Sema*                sema,
                                        u32                  node_index,
                                        SemaTypeSubstitution subst,
                                        u32*                 out_type_index)
{
    const AstNode* node = &ast->nodes[node_index];

    switch (node->kind) {
    case AK_TypeNever:
        *out_type_index = sema_builtin_type(sema, STK_Never);
        sema->node_type_indices[node_index] = *out_type_index;
        return true;

    case AK_SymbolRef:
        {
            if (string_eq(lex_symbol(lexer, node->a), s("Self"))) {
                u32 impl_node_index =
                    sema_enclosing_impl_node_index(ast, node_index);
                if (impl_node_index != U32_MAX) {
                    const AstImplInfo* impl =
                        &ast->impls[ast->nodes[impl_node_index].a];
                    if (!sema_resolve_type_node_ex(lexer,
                                                   ast,
                                                   sema,
                                                   impl->target_type_node_index,
                                                   subst,
                                                   out_type_index)) {
                        return false;
                    }
                    sema->node_type_indices[node_index] = *out_type_index;
                    return true;
                }
            }

            for (u32 i = 0; i < subst.count; ++i) {
                if (subst.param_symbols[i] == node->a) {
                    u32 type_index                      = subst.arg_types[i];
                    sema->node_type_indices[node_index] = type_index;
                    *out_type_index                     = type_index;
                    return true;
                }
            }

            u32 type_index =
                sema_type_index_for_name(sema, lex_symbol(lexer, node->a));
            if (type_index == sema_no_type()) {
                u32 scope_index = sema->node_scope_indices[node_index];
                u32 local_index =
                    sema_lookup_decl_local(sema, scope_index, node->a);
                if (local_index == sema_no_local()) {
                    for (u32 i = array_count(sema->locals); i-- > 0;) {
                        const SemaLocal* candidate = &sema->locals[i];
                        if (candidate->symbol_handle == node->a &&
                            (candidate->kind == SLK_TypeAlias ||
                             (candidate->kind == SLK_Constant &&
                              candidate->type_node_index == sema_no_type() &&
                              sema_node_is_type_syntax(
                                  ast, candidate->value_node_index)))) {
                            local_index = i;
                            break;
                        }
                    }
                }
                if (local_index != sema_no_local()) {
                    SemaLocal* local = &sema->locals[local_index];
                    if (local->kind == SLK_TypeAlias) {
                        type_index = local->type_index;
                    } else if (local->kind == SLK_Constant &&
                               local->type_index != sema_no_type() &&
                               sema_node_is_type_syntax(
                                   ast, local->value_node_index)) {
                        type_index = local->type_index;
                    } else if (local->kind == SLK_Constant &&
                               local->type_node_index == sema_no_type()) {
                        if (local->type_index == sema_no_type() &&
                            !sema_infer_local_binding_type(
                                lexer, ast, sema, local_index, &type_index)) {
                            return false;
                        }
                        if (local->kind == SLK_TypeAlias) {
                            type_index = local->type_index;
                        } else {
                            type_index = sema_no_type();
                        }
                    }
                }
            }
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

    case AK_Field:
        {
            u32 type_index = sema_no_type();
            if (!sema_try_resolve_qualified_type_node(
                    lexer, ast, sema, node_index, &type_index)) {
                return error_0303_unknown_type(lexer->source,
                                               sema_node_span(lexer, node),
                                               lex_symbol(lexer, node->b));
            }
            *out_type_index = type_index;
            return true;
        }

    case AK_TypeApply:
        {
            u32 type_index = sema_no_type();
            if (!sema_resolve_generic_type_application(
                    lexer, ast, sema, node_index, subst, &type_index)) {
                return false;
            }
            *out_type_index = type_index;
            return true;
        }

    case AK_TypeFn:
        {
            const AstFnSignature* signature = sema_ast_signature(ast, node);
            Array(u32) param_types          = NULL;
            if (signature->generic_params_index != U32_MAX) {
                return error_0339_generics_not_implemented(
                    lexer->source,
                    sema_node_span(lexer, node),
                    s("generic function type"));
            }

            for (u32 i = 0; i < signature->param_count; ++i) {
                u32 param_type = sema_no_type();
                if (!sema_resolve_type_node_ex(
                        lexer,
                        ast,
                        sema,
                        ast->params[signature->first_param + i].type_node_index,
                        subst,
                        &param_type)) {
                    array_free(param_types);
                    return false;
                }
                array_push(param_types, param_type);
            }

            u32 return_type = sema_builtin_type(sema, STK_Void);
            if (signature->return_type_node_index != U32_MAX &&
                !sema_resolve_type_node_ex(lexer,
                                           ast,
                                           sema,
                                           signature->return_type_node_index,
                                           subst,
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
                if (!sema_resolve_type_node_ex(lexer,
                                               ast,
                                               sema,
                                               ast->tuple_items[node->a + i],
                                               subst,
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
            if (!sema_resolve_type_node_ex(
                    lexer, ast, sema, node->b, subst, &item_type)) {
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
            if (!sema_resolve_type_node_ex(
                    lexer, ast, sema, node->a, subst, &item_type)) {
                return false;
            }

            u32 type_index = sema_add_slice_type(sema, item_type);
            sema->node_type_indices[node_index] = type_index;
            *out_type_index                     = type_index;
            return true;
        }

    case AK_TypeDynamicArray:
        {
            if (node->a != U32_MAX) {
                i64 min_capacity = 0;
                if (!sema_try_eval_integer_constant(
                        lexer, ast, sema, node->a, &min_capacity)) {
                    u32 capacity_type = sema_no_type();
                    if (!sema_infer_node_type(lexer,
                                              ast,
                                              sema,
                                              node->a,
                                              sema_no_type(),
                                              &capacity_type)) {
                        return false;
                    }
                    if (!sema_type_is_integer(sema, capacity_type)) {
                        return error_0304_type_mismatch(
                            lexer->source,
                            sema_node_span(lexer, &ast->nodes[node->a]),
                            s("integer dynamic array capacity"),
                            sema_type_name(
                                lexer, sema, &temp_arena, capacity_type));
                    }
                } else if (min_capacity < 0 || min_capacity > INT32_MAX) {
                    return error_0304_type_mismatch(
                        lexer->source,
                        sema_node_span(lexer, &ast->nodes[node->a]),
                        s("non-negative integer constant"),
                        s("out-of-range integer constant"));
                }
            }

            u32 item_type = sema_no_type();
            if (!sema_resolve_type_node_ex(
                    lexer, ast, sema, node->b, subst, &item_type)) {
                return false;
            }

            u32 type_index = sema_add_dynamic_array_type(sema, item_type);
            sema->node_type_indices[node_index] = type_index;
            *out_type_index                     = type_index;
            return true;
        }

    case AK_TypePointer:
        {
            u32 pointee_type = sema_no_type();
            if (!sema_resolve_type_node_ex(
                    lexer, ast, sema, node->a, subst, &pointee_type)) {
                return false;
            }

            u32 type_index = sema_add_pointer_type(sema, pointee_type);
            sema->node_type_indices[node_index] = type_index;
            *out_type_index                     = type_index;
            return true;
        }

    case AK_TypeOptional:
        {
            u32 payload_type = sema_no_type();
            if (!sema_resolve_type_node_ex(
                    lexer, ast, sema, node->a, subst, &payload_type)) {
                return false;
            }
            u32 type_index = sema_add_optional_type(lexer, sema, payload_type);
            sema->node_type_indices[node_index] = type_index;
            *out_type_index                     = type_index;
            return true;
        }

    case AK_TypeResult:
        {
            u32 success_type = sema_no_type();
            u32 error_type   = sema_no_type();
            if (!sema_resolve_type_node_ex(
                    lexer, ast, sema, node->a, subst, &success_type) ||
                !sema_resolve_type_node_ex(
                    lexer, ast, sema, node->b, subst, &error_type)) {
                return false;
            }
            u32 type_index =
                sema_add_result_type(lexer, sema, success_type, error_type);
            sema->node_type_indices[node_index] = type_index;
            *out_type_index                     = type_index;
            return true;
        }

    case AK_TypePlex:
        {
            const AstPlexTypeInfo* plex = &ast->plex_types[node->a];
            Array(u32) field_types      = NULL;
            if (plex->generic_params_index != U32_MAX && subst.count == 0) {
                return error_0339_generics_not_implemented(
                    lexer->source,
                    sema_node_span(lexer, node),
                    (plex->flags & APTF_Union) ? s("generic union")
                                               : s("generic plex"));
            }
            for (u32 i = 0; i < plex->field_count; ++i) {
                u32 field_type = sema_no_type();
                if (!sema_resolve_type_node_ex(
                        lexer,
                        ast,
                        sema,
                        ast->plex_fields[plex->first_field + i].type_node_index,
                        subst,
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

    case AK_TypeEnum:
        {
            const AstEnumTypeInfo* enum_type = &ast->enum_types[node->a];
            if (enum_type->generic_params_index != U32_MAX &&
                subst.count == 0) {
                return error_0339_generics_not_implemented(
                    lexer->source,
                    sema_node_span(lexer, node),
                    s("generic enum"));
            }

            Array(u32) payload_types = NULL;
            Array(i64) discriminants = NULL;
            i64 next_discriminant    = 0;
            for (u32 i = 0; i < enum_type->variant_count; ++i) {
                const AstEnumVariant* variant =
                    &ast->enum_variants[enum_type->first_variant + i];
                if (!sema_check_enum_variant_name_unique(
                        lexer,
                        &ast->enum_variants[enum_type->first_variant],
                        i)) {
                    array_free(payload_types);
                    array_free(discriminants);
                    return false;
                }
                u32 payload_type = sema_no_type();
                if (variant->type_node_index != U32_MAX &&
                    !sema_resolve_type_node_ex(lexer,
                                               ast,
                                               sema,
                                               variant->type_node_index,
                                               subst,
                                               &payload_type)) {
                    array_free(payload_types);
                    array_free(discriminants);
                    return false;
                }
                array_push(payload_types, payload_type);

                i64 discriminant = next_discriminant;
                if (variant->value_node_index != U32_MAX &&
                    !sema_try_eval_integer_constant(lexer,
                                                    ast,
                                                    sema,
                                                    variant->value_node_index,
                                                    &discriminant)) {
                    array_free(payload_types);
                    array_free(discriminants);
                    return error_0303_unknown_type(
                        lexer->source,
                        sema_node_span(lexer,
                                       &ast->nodes[variant->value_node_index]),
                        s("<enum discriminant>"));
                }
                if (!sema_check_enum_discriminant_unique(
                        lexer,
                        ast,
                        &ast->enum_variants[enum_type->first_variant],
                        discriminants,
                        i,
                        discriminant)) {
                    array_free(payload_types);
                    array_free(discriminants);
                    return false;
                }
                array_push(discriminants, discriminant);
                next_discriminant = discriminant + 1;
            }

            u32 type_index = sema_add_enum_type(
                sema,
                &ast->enum_variants[enum_type->first_variant],
                payload_types,
                discriminants,
                enum_type->variant_count);
            array_free(payload_types);
            array_free(discriminants);
            sema->node_type_indices[node_index] = type_index;
            *out_type_index                     = type_index;
            return true;
        }

    default:
        return error_0303_unknown_type(
            lexer->source, sema_node_span(lexer, node), s("<expression>"));
    }
}

internal bool sema_resolve_type_node(const Lexer* lexer,
                                     const Ast*   ast,
                                     Sema*        sema,
                                     u32          node_index,
                                     u32*         out_type_index)
{
    return sema_resolve_type_node_ex(
        lexer, ast, sema, node_index, g_sema_type_subst, out_type_index);
}

//------------------------------------------------------------------------------
// Return whether one source type satisfies one target type without casts.

internal bool
sema_type_matches(const Sema* sema, u32 expected_type, u32 actual_type)
{
    if (expected_type == sema_no_type() || actual_type == sema_no_type()) {
        return true;
    }

    if (sema_type_is_concrete_integer(sema, expected_type) &&
        sema->types[actual_type].kind == STK_UntypedInteger) {
        return true;
    }

    if (sema_type_is_concrete_float(sema, expected_type) &&
        sema->types[actual_type].kind == STK_UntypedFloat) {
        return true;
    }

    expected_type = sema_materialise_type(sema, expected_type);
    actual_type   = sema_materialise_type(sema, actual_type);
    if (expected_type == actual_type) {
        return true;
    }

    if (sema->types[actual_type].kind == STK_Never) {
        return true;
    }

    const SemaType* expected = &sema->types[expected_type];
    if (expected->kind == STK_Enum && (expected->flags & STF_Optional)) {
        if (sema->types[actual_type].kind == STK_Nil) {
            return true;
        }
        u32 payload_type =
            sema->type_param_types[expected->first_param_type + 1];
        return sema_type_matches(sema, payload_type, actual_type);
    }
    if (expected->kind == STK_Enum && (expected->flags & STF_Result)) {
        u32 success_type = sema->type_param_types[expected->first_param_type];
        return sema_type_matches(sema, success_type, actual_type);
    }

    if (sema->types[expected_type].kind == STK_Slice &&
        sema->types[actual_type].kind == STK_Array &&
        sema->types[expected_type].first_param_type ==
            sema->types[actual_type].first_param_type) {
        return true;
    }

    if (sema->types[expected_type].kind == STK_Pointer &&
        sema->types[actual_type].kind == STK_Pointer &&
        sema->types[sema->types[expected_type].first_param_type].kind ==
            STK_Void) {
        return true;
    }

    if (sema->types[expected_type].kind == STK_Pointer &&
        sema->types[actual_type].kind == STK_Box &&
        sema->types[expected_type].first_param_type ==
            sema->types[actual_type].first_param_type) {
        return true;
    }

    if (sema->types[expected_type].kind == STK_Bool &&
        sema->types[actual_type].kind == STK_Box) {
        return true;
    }

    if ((sema->types[expected_type].kind == STK_Pointer ||
         sema->types[expected_type].kind == STK_Function ||
         sema->types[expected_type].kind == STK_Slice ||
         sema->types[expected_type].kind == STK_DynamicArray ||
         sema->types[expected_type].kind == STK_Box) &&
        sema->types[actual_type].kind == STK_Nil) {
        return true;
    }

    return false;
}

internal bool sema_type_is_never(const Sema* sema, u32 type_index)
{
    if (type_index == sema_no_type()) {
        return false;
    }
    type_index = sema_materialise_type(sema, type_index);
    return type_index < array_count(sema->types) &&
           sema->types[type_index].kind == STK_Never;
}

internal bool sema_type_is_pointer_integer_mismatch(const Sema* sema,
                                                    u32         expected_type,
                                                    u32         actual_type)
{
    if (expected_type == sema_no_type() || actual_type == sema_no_type()) {
        return false;
    }

    expected_type = sema_materialise_type(sema, expected_type);
    actual_type   = sema_materialise_type(sema, actual_type);
    return sema->types[expected_type].kind == STK_Pointer &&
           sema_type_is_integer(sema, actual_type);
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

internal i64 sema_enum_variant_discriminant(const Sema* sema,
                                            u32         enum_type,
                                            u32         variant_index)
{
    if (enum_type == sema_no_type() ||
        sema->types[enum_type].kind != STK_Enum ||
        variant_index >= sema->types[enum_type].param_count) {
        return 0;
    }
    return sema->type_param_values[sema->types[enum_type].first_param_type +
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

    u32 decl_index = sema_find_decl(sema, node->a);
    if (decl_index != sema_no_decl() &&
        sema->decls[decl_index].kind != SK_TypeAlias) {
        return false;
    }

    u32 type_index = sema_type_index_for_name(sema, lex_symbol(lexer, node->a));
    if (type_index == sema_no_type()) {
        for (u32 i = 0; i < g_sema_type_subst.count; ++i) {
            if (g_sema_type_subst.param_symbols[i] == node->a) {
                type_index = g_sema_type_subst.arg_types[i];
                break;
            }
        }
    }
    if (type_index == sema_no_type()) {
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

internal bool sema_try_resolve_qualified_type_node(const Lexer* lexer,
                                                   const Ast*   ast,
                                                   Sema*        sema,
                                                   u32          node_index,
                                                   u32*         out_type_index)
{
    const AstNode* node = &ast->nodes[node_index];
    if (node->kind != AK_Field) {
        return sema_try_resolve_type_symbol(
            lexer, ast, sema, node_index, out_type_index);
    }

    u32 target_type = sema_no_type();
    if (!sema_infer_node_type(
            lexer, ast, sema, node->a, sema_no_type(), &target_type)) {
        return false;
    }
    if (target_type == sema_no_type() ||
        sema->types[target_type].kind != STK_Module) {
        return false;
    }

    const SemaType* module = &sema->types[target_type];
    for (u32 i = 0; i < module->param_count; ++i) {
        if (sema->type_param_symbols[module->first_param_type + i] != node->b) {
            continue;
        }

        u32 type_index = sema->type_param_types[module->first_param_type + i];
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

        if (import_decl_kind != SK_TypeAlias) {
            return false;
        }

        sema->node_is_type_expr[node_index] = true;
        sema->node_type_indices[node_index] = type_index;
        sema->node_decl_indices[node_index] =
            sema_ensure_module_export_decl(sema,
                                           node->b,
                                           type_index,
                                           import_decl_kind,
                                           import_module_index,
                                           import_decl_index);
        *out_type_index = type_index;
        return true;
    }

    return false;
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

internal bool sema_type_is_u8_slice(const Sema* sema, u32 type_index)
{
    if (type_index == sema_no_type()) {
        return false;
    }

    const SemaType* type = &sema->types[type_index];
    return type->kind == STK_Slice &&
           type->first_param_type != sema_no_type() &&
           sema->types[type->first_param_type].kind == STK_U8;
}

internal bool sema_type_is_byte_collection(const Sema* sema, u32 type_index)
{
    if (type_index == sema_no_type()) {
        return false;
    }

    const SemaType* type = &sema->types[type_index];
    if (type->kind != STK_Array && type->kind != STK_Slice &&
        type->kind != STK_DynamicArray) {
        return false;
    }
    if (type->first_param_type == sema_no_type()) {
        return false;
    }

    SemaTypeKind item_kind = sema->types[type->first_param_type].kind;
    return item_kind == STK_I8 || item_kind == STK_U8;
}

internal const AstCastInfo* sema_cast_info(const Ast* ast, const AstNode* node)
{
    ASSERT(node->kind == AK_Cast, "Expected cast node");
    return &ast->casts[node->b];
}

internal bool sema_dynarray_method_signature(Sema*        sema,
                                             const Lexer* lexer,
                                             u32          dynarray_type,
                                             u32          method_symbol,
                                             u32*         out_type_index)
{
    if (dynarray_type == sema_no_type() ||
        sema->types[dynarray_type].kind != STK_DynamicArray) {
        return false;
    }

    u32    item_type  = sema->types[dynarray_type].first_param_type;
    string method     = lex_symbol(lexer, method_symbol);
    Array(u32) params = NULL;
    u32  result       = sema_builtin_type(sema, STK_Void);
    bool matched      = true;

    if (string_eq(method, s("push"))) {
        array_push(params, item_type);
    } else if (string_eq(method, s("append"))) {
        array_push(params, sema_add_slice_type(sema, item_type));
    } else if (string_eq(method, s("reserve_to")) ||
               string_eq(method, s("reserve_extra"))) {
        array_push(params, sema_builtin_type(sema, STK_Usize));
    } else if (string_eq(method, s("resize_to")) ||
               string_eq(method, s("resize_undefined_to")) ||
               string_eq(method, s("extend")) ||
               string_eq(method, s("extend_undefined"))) {
        array_push(params, sema_builtin_type(sema, STK_Usize));
    } else if (string_eq(method, s("delete")) ||
               string_eq(method, s("swap_delete"))) {
        array_push(params, sema_builtin_type(sema, STK_Usize));
    } else if (string_eq(method, s("pop"))) {
        result = item_type;
    } else if (string_eq(method, s("clear")) || string_eq(method, s("free"))) {
        // No params.
    } else {
        matched = false;
    }

    if (!matched) {
        array_free(params);
        return false;
    }

    *out_type_index = sema_add_function_type(sema, params, result);
    array_free(params);
    return true;
}

internal bool sema_box_method_signature(Sema*        sema,
                                        const Lexer* lexer,
                                        u32          box_type,
                                        u32          method_symbol,
                                        u32*         out_type_index)
{
    if (box_type == sema_no_type() || sema->types[box_type].kind != STK_Box) {
        return false;
    }

    string method = lex_symbol(lexer, method_symbol);
    if (!string_eq(method, s("free"))) {
        return false;
    }

    Array(u32) params = NULL;
    *out_type_index =
        sema_add_function_type(sema, params, sema_builtin_type(sema, STK_Void));
    return true;
}

internal bool sema_node_can_mutate_dynarray(const Ast* ast, u32 node_index)
{
    const AstNode* node = &ast->nodes[node_index];
    switch (node->kind) {
    case AK_SymbolRef:
    case AK_Field:
    case AK_Deref:
        return true;
    case AK_Expression:
        return sema_node_can_mutate_dynarray(ast, node->a);
    default:
        return false;
    }
}

internal bool sema_node_is_box_constructor(const Lexer* lexer,
                                           const Ast*   ast,
                                           Sema*        sema,
                                           u32          callee_node_index,
                                           u32*         out_box_type)
{
    if (callee_node_index >= array_count(ast->nodes)) {
        return false;
    }

    const AstNode* callee = &ast->nodes[callee_node_index];
    if (callee->kind != AK_Index || callee->a >= array_count(ast->nodes)) {
        return false;
    }

    const AstNode* target = &ast->nodes[callee->a];
    if (target->kind != AK_SymbolRef ||
        !string_eq(lex_symbol(lexer, target->a), s("box"))) {
        return false;
    }

    u32 item_type = sema_no_type();
    if (!sema_resolve_type_node(lexer, ast, sema, callee->b, &item_type)) {
        *out_box_type = sema_no_type();
        return true;
    }

    *out_box_type = sema_add_box_type(sema, item_type);
    return true;
}

internal bool sema_assignment_target_writes_param_storage(const Ast*  ast,
                                                          const Sema* sema,
                                                          u32  node_index,
                                                          u32* out_local_index,
                                                          u32* out_root_node)
{
    const AstNode* node = &ast->nodes[node_index];
    switch (node->kind) {
    case AK_SymbolRef:
        {
            u32 local_index = sema->node_local_indices[node_index];
            if (local_index != sema_no_local() &&
                sema->locals[local_index].kind == SLK_Param) {
                *out_local_index = local_index;
                *out_root_node   = node_index;
                return true;
            }
            return false;
        }
    case AK_Expression:
    case AK_Statement:
        return sema_assignment_target_writes_param_storage(
            ast, sema, node->a, out_local_index, out_root_node);
    case AK_Deref:
        return false;
    case AK_Field:
    case AK_TupleField:
        {
            u32 base_type = node->a < array_count(sema->node_type_indices)
                                ? sema->node_type_indices[node->a]
                                : sema_no_type();
            if (base_type != sema_no_type() &&
                sema->types[base_type].kind == STK_Pointer) {
                return false;
            }
            return sema_assignment_target_writes_param_storage(
                ast, sema, node->a, out_local_index, out_root_node);
        }
    case AK_Index:
        {
            u32 base_type = node->a < array_count(sema->node_type_indices)
                                ? sema->node_type_indices[node->a]
                                : sema_no_type();
            if (base_type != sema_no_type() &&
                (sema->types[base_type].kind == STK_Pointer ||
                 sema->types[base_type].kind == STK_Slice ||
                 sema->types[base_type].kind == STK_DynamicArray)) {
                return false;
            }
            return sema_assignment_target_writes_param_storage(
                ast, sema, node->a, out_local_index, out_root_node);
        }
    default:
        return false;
    }
}

internal u32 sema_enclosing_function_return_type(const Sema* sema,
                                                 u32         node_index)
{
    if (node_index >= array_count(sema->node_scope_indices)) {
        return sema_no_type();
    }

    u32 scope_index = sema->node_scope_indices[node_index];
    while (scope_index != sema_no_scope()) {
        const SemaScope* scope = &sema->scopes[scope_index];
        if (scope->owner_decl_index != sema_no_decl()) {
            const SemaDecl* decl = &sema->decls[scope->owner_decl_index];
            if ((decl->kind == SK_Function || decl->kind == SK_FfiFunction ||
                 decl->kind == SK_BuiltinFunction) &&
                decl->type_index != sema_no_type() &&
                sema->types[decl->type_index].kind == STK_Function) {
                return sema->types[decl->type_index].return_type;
            }
        }
        scope_index = scope->parent_scope_index;
    }

    return sema_no_type();
}

internal u32 sema_ast_enclosing_function_return_type(const Lexer* lexer,
                                                     const Ast*   ast,
                                                     Sema*        sema,
                                                     u32          node_index)
{
    for (u32 i = node_index + 1; i-- > 0;) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind != AK_FnStart || node->b <= node_index) {
            continue;
        }
        const AstFnSignature* signature = &ast->fn_signatures[node->a];
        if (signature->return_type_node_index == U32_MAX) {
            return sema_no_type();
        }
        u32 return_type = sema_no_type();
        if (!sema_resolve_type_node(lexer,
                                    ast,
                                    sema,
                                    signature->return_type_node_index,
                                    &return_type)) {
            return sema_no_type();
        }
        return return_type;
    }
    return sema_no_type();
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
    case STK_DynamicArray:
    case STK_Box:
    case STK_Pointer:
    case STK_Enum:
    case STK_Arena:
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

internal bool sema_type_is_builtin_interpolatable(const Sema* sema,
                                                  u32         type_index)
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
                if (!sema_type_is_builtin_interpolatable(
                        sema,
                        sema->type_param_types[tuple->first_param_type + i])) {
                    return false;
                }
            }
            return true;
        }
    case STK_Array:
        return sema_type_is_builtin_interpolatable(
            sema, sema->types[type_index].first_param_type);
    case STK_Slice:
    case STK_DynamicArray:
        return sema_type_is_builtin_interpolatable(
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
    case STK_Box:
        return true;
    default:
        return sema_type_is_numeric(sema, type_index);
    }
}

internal bool sema_pointer_types_are_equality_comparable(const Sema* sema,
                                                         u32         lhs_type,
                                                         u32         rhs_type)
{
    if (lhs_type == sema_no_type() || rhs_type == sema_no_type()) {
        return false;
    }
    lhs_type = sema_materialise_type(sema, lhs_type);
    rhs_type = sema_materialise_type(sema, rhs_type);
    if (sema->types[lhs_type].kind != STK_Pointer ||
        sema->types[rhs_type].kind != STK_Pointer) {
        return false;
    }
    u32 lhs_pointee = sema->types[lhs_type].first_param_type;
    u32 rhs_pointee = sema->types[rhs_type].first_param_type;
    return lhs_pointee == rhs_pointee ||
           sema->types[lhs_pointee].kind == STK_Void ||
           sema->types[rhs_pointee].kind == STK_Void;
}

internal bool sema_pointer_arithmetic_result_type(
    Sema* sema, AstKind op, u32 lhs_type, u32 rhs_type, u32* out_type)
{
    lhs_type = sema_materialise_type(sema, lhs_type);
    rhs_type = sema_materialise_type(sema, rhs_type);
    bool lhs_pointer =
        lhs_type != sema_no_type() && sema->types[lhs_type].kind == STK_Pointer;
    bool rhs_pointer =
        rhs_type != sema_no_type() && sema->types[rhs_type].kind == STK_Pointer;
    bool lhs_integer = sema_type_is_integer(sema, lhs_type);
    bool rhs_integer = sema_type_is_integer(sema, rhs_type);
    bool lhs_pointer_sized =
        lhs_pointer &&
        sema->types[sema->types[lhs_type].first_param_type].kind != STK_Void;
    bool rhs_pointer_sized =
        rhs_pointer &&
        sema->types[sema->types[rhs_type].first_param_type].kind != STK_Void;

    if (op == AK_IntegerPlus) {
        if (lhs_pointer_sized && rhs_integer) {
            *out_type = lhs_type;
            return true;
        }
        if (lhs_integer && rhs_pointer_sized) {
            *out_type = rhs_type;
            return true;
        }
    }
    if (op == AK_IntegerMinus) {
        if (lhs_pointer_sized && rhs_integer) {
            *out_type = lhs_type;
            return true;
        }
        if (lhs_pointer_sized && rhs_pointer_sized &&
            sema_pointer_types_are_equality_comparable(
                sema, lhs_type, rhs_type)) {
            *out_type = sema_builtin_type(sema, STK_Isize);
            return true;
        }
    }
    return false;
}

internal u32 sema_find_core_eq_method_decl(
    const Lexer* lexer, const Ast* ast, Sema* sema, u32 lhs_type, u32 rhs_type)
{
    u32 eq_trait = sema_find_core_trait_symbol(lexer, sema, s("Eq"));
    if (eq_trait == sema_no_decl() || lhs_type == sema_no_type() ||
        rhs_type == sema_no_type()) {
        return sema_no_decl();
    }

    for (u32 i = 0; i < array_count(sema->methods); ++i) {
        const SemaMethod* method = &sema->methods[i];
        if (!method->is_trait_impl || method->generic_params_index != U32_MAX ||
            method->symbol_handle == U32_MAX ||
            !string_eq_cstr(lex_symbol(lexer, method->symbol_handle), "eq") ||
            method->impl_node_index >= array_count(ast->nodes) ||
            method->decl_index >= array_count(sema->decls)) {
            continue;
        }

        const AstNode* impl_node = &ast->nodes[method->impl_node_index];
        if (impl_node->kind != AK_Impl ||
            impl_node->a >= array_count(ast->impls)) {
            continue;
        }

        const AstImplInfo* impl = &ast->impls[impl_node->a];
        if (sema_trait_symbol_from_type_node(
                ast, impl->trait_type_node_index) != eq_trait) {
            continue;
        }

        u32 target_type = sema_no_type();
        if (!sema_resolve_type_node(lexer,
                                    ast,
                                    sema,
                                    method->target_type_node_index,
                                    &target_type) ||
            !sema_type_matches(sema, target_type, lhs_type)) {
            continue;
        }

        u32 fn_type = sema->decls[method->decl_index].type_index;
        if (fn_type >= array_count(sema->types) ||
            sema->types[fn_type].kind != STK_Function ||
            sema->types[fn_type].param_count < 2 ||
            sema->types[fn_type].return_type !=
                sema_builtin_type(sema, STK_Bool)) {
            continue;
        }

        u32 first_param = sema->types[fn_type].first_param_type;
        if (first_param + 1 >= array_count(sema->type_param_types) ||
            !sema_type_matches(
                sema, sema->type_param_types[first_param], lhs_type) ||
            !sema_type_matches(
                sema, sema->type_param_types[first_param + 1], rhs_type)) {
            continue;
        }

        return method->decl_index;
    }

    return sema_no_decl();
}

internal u32 sema_find_core_order_method_decl(
    const Lexer* lexer, const Ast* ast, Sema* sema, u32 lhs_type, u32 rhs_type)
{
    u32 order_trait = sema_find_core_trait_symbol(lexer, sema, s("Order"));
    if (order_trait == sema_no_decl() || lhs_type == sema_no_type() ||
        rhs_type == sema_no_type()) {
        return sema_no_decl();
    }

    for (u32 i = 0; i < array_count(sema->methods); ++i) {
        const SemaMethod* method = &sema->methods[i];
        if (!method->is_trait_impl || method->generic_params_index != U32_MAX ||
            method->symbol_handle == U32_MAX ||
            !string_eq_cstr(lex_symbol(lexer, method->symbol_handle),
                            "compare") ||
            method->impl_node_index >= array_count(ast->nodes) ||
            method->decl_index >= array_count(sema->decls)) {
            continue;
        }

        const AstNode* impl_node = &ast->nodes[method->impl_node_index];
        if (impl_node->kind != AK_Impl ||
            impl_node->a >= array_count(ast->impls)) {
            continue;
        }

        const AstImplInfo* impl = &ast->impls[impl_node->a];
        if (sema_trait_symbol_from_type_node(
                ast, impl->trait_type_node_index) != order_trait) {
            continue;
        }

        u32 target_type = sema_no_type();
        if (!sema_resolve_type_node(lexer,
                                    ast,
                                    sema,
                                    method->target_type_node_index,
                                    &target_type) ||
            !sema_type_matches(sema, target_type, lhs_type)) {
            continue;
        }

        u32 fn_type = sema->decls[method->decl_index].type_index;
        if (fn_type >= array_count(sema->types) ||
            sema->types[fn_type].kind != STK_Function ||
            sema->types[fn_type].param_count < 2 ||
            sema->types[fn_type].return_type !=
                sema_builtin_type(sema, STK_I32)) {
            continue;
        }

        u32 first_param = sema->types[fn_type].first_param_type;
        if (first_param + 1 >= array_count(sema->type_param_types) ||
            !sema_type_matches(
                sema, sema->type_param_types[first_param], lhs_type) ||
            !sema_type_matches(
                sema, sema->type_param_types[first_param + 1], rhs_type)) {
            continue;
        }

        return method->decl_index;
    }

    return sema_no_decl();
}

internal bool sema_type_is_core_option_like(const Sema* sema,
                                            u32         type_index,
                                            u32*        out_item_type)
{
    if (out_item_type != NULL) {
        *out_item_type = sema_no_type();
    }
    type_index = sema_materialise_type(sema, type_index);
    if (type_index >= array_count(sema->types) ||
        sema->types[type_index].kind != STK_Enum ||
        !(sema->types[type_index].flags & STF_Optional)) {
        return false;
    }
    u32 some_payload =
        sema->type_param_types[sema->types[type_index].first_param_type + 1];
    if (some_payload == sema_no_type()) {
        return false;
    }
    if (out_item_type != NULL) {
        *out_item_type = some_payload;
    }
    return true;
}

internal u32 sema_find_core_iterator_method_decl(const Lexer* lexer,
                                                 const Ast*   ast,
                                                 Sema*        sema,
                                                 u32          iterable_type,
                                                 u32*         out_item_type)
{
    if (out_item_type != NULL) {
        *out_item_type = sema_no_type();
    }
    u32 iterator_trait =
        sema_find_core_trait_symbol(lexer, sema, s("Iterator"));
    if (iterator_trait == sema_no_decl() || iterable_type == sema_no_type()) {
        return sema_no_decl();
    }

    iterable_type = sema_materialise_type(sema, iterable_type);
    for (u32 i = 0; i < array_count(sema->methods); ++i) {
        const SemaMethod* method = &sema->methods[i];
        if (!method->is_trait_impl || method->generic_params_index != U32_MAX ||
            method->symbol_handle == U32_MAX ||
            !string_eq_cstr(lex_symbol(lexer, method->symbol_handle), "next") ||
            method->decl_index >= array_count(sema->decls)) {
            continue;
        }

        const SemaDecl*   decl              = &sema->decls[method->decl_index];
        const Lexer*      source_lexer      = lexer;
        const Ast*        source_ast        = ast;
        Sema*             source_sema       = sema;
        u32               source_decl_index = method->decl_index;
        const SemaMethod* source_method     = method;
        bool imported = decl->import_module_index != sema_no_decl();
        if (imported) {
            if (!sema_imported_decl_source(sema,
                                           decl,
                                           &source_lexer,
                                           &source_ast,
                                           &source_sema,
                                           &source_decl_index)) {
                continue;
            }
            source_method =
                sema_find_method_for_decl(source_sema, source_decl_index);
            if (source_method == NULL) {
                continue;
            }
        }

        if (source_method->impl_node_index >= array_count(source_ast->nodes) ||
            !sema_method_matches_trait_symbol(lexer,
                                              source_lexer,
                                              source_ast,
                                              source_method,
                                              iterator_trait)) {
            continue;
        }

        u32 target_type = sema_no_type();
        if (!sema_resolve_type_node(source_lexer,
                                    source_ast,
                                    source_sema,
                                    source_method->target_type_node_index,
                                    &target_type) ||
            (imported &&
             (target_type = sema_import_type((Lexer*)lexer,
                                             sema,
                                             source_lexer,
                                             source_sema,
                                             target_type)) == sema_no_type()) ||
            !sema_type_matches(sema, target_type, iterable_type)) {
            continue;
        }

        u32 fn_type = source_sema->decls[source_decl_index].type_index;
        if (imported) {
            fn_type = sema_import_type(
                (Lexer*)lexer, sema, source_lexer, source_sema, fn_type);
        }
        if (fn_type >= array_count(sema->types) ||
            sema->types[fn_type].kind != STK_Function ||
            sema->types[fn_type].param_count != 1) {
            continue;
        }
        u32 expected_self = sema_add_pointer_type(sema, iterable_type);
        u32 actual_self =
            sema->type_param_types[sema->types[fn_type].first_param_type];
        if (!sema_type_matches(sema, expected_self, actual_self)) {
            continue;
        }

        u32 item_type = sema_no_type();
        if (!sema_type_is_core_option_like(
                sema, sema->types[fn_type].return_type, &item_type)) {
            continue;
        }
        if (out_item_type != NULL) {
            *out_item_type = item_type;
        }
        return method->decl_index;
    }

    return sema_no_decl();
}

internal u32 sema_find_core_default_method_decl(const Lexer* lexer,
                                                const Ast*   ast,
                                                Sema*        sema,
                                                u32          target_type)
{
    u32 default_trait = sema_find_core_trait_symbol(lexer, sema, s("Default"));
    if (default_trait == sema_no_decl() || target_type == sema_no_type()) {
        return sema_no_decl();
    }

    for (u32 i = 0; i < array_count(sema->methods); ++i) {
        const SemaMethod* method = &sema->methods[i];
        if (!method->is_trait_impl || method->generic_params_index != U32_MAX ||
            method->symbol_handle == U32_MAX ||
            !string_eq_cstr(lex_symbol(lexer, method->symbol_handle),
                            "default") ||
            method->impl_node_index >= array_count(ast->nodes) ||
            method->decl_index >= array_count(sema->decls)) {
            continue;
        }

        const AstNode* impl_node = &ast->nodes[method->impl_node_index];
        if (impl_node->kind != AK_Impl ||
            impl_node->a >= array_count(ast->impls)) {
            continue;
        }

        if (!sema_method_matches_trait_symbol(
                lexer, lexer, ast, method, default_trait)) {
            continue;
        }

        u32 impl_target_type = sema_no_type();
        if (!sema_resolve_type_node(lexer,
                                    ast,
                                    sema,
                                    method->target_type_node_index,
                                    &impl_target_type) ||
            !sema_type_matches(sema, impl_target_type, target_type)) {
            continue;
        }

        const SemaDecl* decl = &sema->decls[method->decl_index];
        if (decl->value_node_index >= array_count(ast->nodes) ||
            ast->nodes[decl->value_node_index].kind != AK_FnDef) {
            continue;
        }
        const AstNode*        fn_def    = &ast->nodes[decl->value_node_index];
        const AstNode*        fn_start  = &ast->nodes[fn_def->a];
        const AstFnSignature* signature = &ast->fn_signatures[fn_start->a];
        if (signature->param_count != 0 || !method->is_associated) {
            continue;
        }

        return method->decl_index;
    }

    for (u32 i = 0; i < array_count(sema->decls); ++i) {
        const SemaDecl* decl = &sema->decls[i];
        if (decl->kind != SK_Function ||
            decl->bind_node_index >= array_count(ast->nodes)) {
            continue;
        }

        const AstNode* bind = &ast->nodes[decl->bind_node_index];
        if (bind->kind != AK_Bind ||
            !string_eq_cstr(lex_symbol(lexer, ast_get_symbol(bind)),
                            "default")) {
            continue;
        }

        u32 impl_node_index =
            sema_enclosing_impl_node_index(ast, decl->bind_node_index);
        if (impl_node_index >= array_count(ast->nodes)) {
            continue;
        }

        const AstNode* impl_node = &ast->nodes[impl_node_index];
        if (impl_node->kind != AK_Impl ||
            impl_node->a >= array_count(ast->impls)) {
            continue;
        }

        const AstImplInfo* impl = &ast->impls[impl_node->a];
        u32                trait_symbol =
            sema_trait_symbol_from_type_node(ast, impl->trait_type_node_index);
        if (trait_symbol == U32_MAX ||
            !string_eq(lex_symbol(lexer, trait_symbol),
                       lex_symbol(lexer, default_trait))) {
            continue;
        }

        u32 impl_target_type = sema_no_type();
        if (!sema_resolve_type_node(lexer,
                                    ast,
                                    sema,
                                    impl->target_type_node_index,
                                    &impl_target_type) ||
            !sema_type_matches(sema, impl_target_type, target_type)) {
            continue;
        }

        if (decl->value_node_index >= array_count(ast->nodes) ||
            ast->nodes[decl->value_node_index].kind != AK_FnDef) {
            continue;
        }
        const AstNode*        fn_def    = &ast->nodes[decl->value_node_index];
        const AstNode*        fn_start  = &ast->nodes[fn_def->a];
        const AstFnSignature* signature = &ast->fn_signatures[fn_start->a];
        if (signature->param_count != 0 ||
            signature->return_type_node_index == U32_MAX) {
            continue;
        }

        u32 return_type = signature->return_type_node_index;
        while (ast->nodes[return_type].kind == AK_Expression ||
               ast->nodes[return_type].kind == AK_Statement) {
            return_type = ast->nodes[return_type].a;
        }
        const AstNode* return_node = &ast->nodes[return_type];
        if (return_node->kind == AK_SymbolRef &&
            string_eq(lex_symbol(lexer, return_node->a), s("Self"))) {
            return i;
        }
    }

    return sema_no_decl();
}

internal bool sema_on_covers_all_enum_variants(const Ast*  ast,
                                               const Sema* sema,
                                               u32         on_index,
                                               u32         enum_type)
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

internal Array(u32) sema_copy_u32_array(Array(u32) source)
{
    Array(u32) copy = NULL;
    for (u32 i = 0; i < array_count(source); ++i) {
        array_push(copy, source[i]);
    }
    return copy;
}

internal Array(bool) sema_copy_bool_array(Array(bool) source)
{
    Array(bool) copy = NULL;
    for (u32 i = 0; i < array_count(source); ++i) {
        array_push(copy, source[i]);
    }
    return copy;
}

internal Array(i64) sema_copy_i64_array(Array(i64) source)
{
    Array(i64) copy = NULL;
    for (u32 i = 0; i < array_count(source); ++i) {
        array_push(copy, source[i]);
    }
    return copy;
}

typedef struct {
    Array(u32) node_decl_indices;
    Array(u32) node_local_indices;
    Array(u32) node_scope_indices;
    Array(u32) node_lowered_symbol_handles;
    Array(u32) node_type_indices;
    Array(u32) node_method_call_decl_indices;
    Array(bool) node_method_call_receiver_refs;
    Array(bool) node_method_call_receiver_derefs;
    Array(bool) node_method_call_explicit_traits;
    Array(u32) node_implicit_array_type_indices;
    Array(bool) node_is_type_expr;
    Array(bool) node_const_known;
    Array(i64) node_const_values;
} SemaNodeTablesSnapshot;

internal SemaNodeTablesSnapshot sema_snapshot_node_tables(const Sema* sema)
{
    return (SemaNodeTablesSnapshot){
        .node_decl_indices  = sema_copy_u32_array(sema->node_decl_indices),
        .node_local_indices = sema_copy_u32_array(sema->node_local_indices),
        .node_scope_indices = sema_copy_u32_array(sema->node_scope_indices),
        .node_lowered_symbol_handles =
            sema_copy_u32_array(sema->node_lowered_symbol_handles),
        .node_type_indices = sema_copy_u32_array(sema->node_type_indices),
        .node_method_call_decl_indices =
            sema_copy_u32_array(sema->node_method_call_decl_indices),
        .node_method_call_receiver_refs =
            sema_copy_bool_array(sema->node_method_call_receiver_refs),
        .node_method_call_receiver_derefs =
            sema_copy_bool_array(sema->node_method_call_receiver_derefs),
        .node_method_call_explicit_traits =
            sema_copy_bool_array(sema->node_method_call_explicit_traits),
        .node_implicit_array_type_indices =
            sema_copy_u32_array(sema->node_implicit_array_type_indices),
        .node_is_type_expr = sema_copy_bool_array(sema->node_is_type_expr),
        .node_const_known  = sema_copy_bool_array(sema->node_const_known),
        .node_const_values = sema_copy_i64_array(sema->node_const_values),
    };
}

internal void sema_free_live_node_tables(Sema* sema)
{
    array_free(sema->node_decl_indices);
    array_free(sema->node_local_indices);
    array_free(sema->node_scope_indices);
    array_free(sema->node_lowered_symbol_handles);
    array_free(sema->node_type_indices);
    array_free(sema->node_method_call_decl_indices);
    array_free(sema->node_method_call_receiver_refs);
    array_free(sema->node_method_call_receiver_derefs);
    array_free(sema->node_method_call_explicit_traits);
    array_free(sema->node_implicit_array_type_indices);
    array_free(sema->node_is_type_expr);
    array_free(sema->node_const_known);
    array_free(sema->node_const_values);
}

internal void sema_restore_node_tables(Sema*                   sema,
                                       SemaNodeTablesSnapshot* snapshot)
{
    sema_free_live_node_tables(sema);
    sema->node_decl_indices           = snapshot->node_decl_indices;
    sema->node_local_indices          = snapshot->node_local_indices;
    sema->node_scope_indices          = snapshot->node_scope_indices;
    sema->node_lowered_symbol_handles = snapshot->node_lowered_symbol_handles;
    sema->node_type_indices           = snapshot->node_type_indices;
    sema->node_method_call_decl_indices =
        snapshot->node_method_call_decl_indices;
    sema->node_method_call_receiver_refs =
        snapshot->node_method_call_receiver_refs;
    sema->node_method_call_receiver_derefs =
        snapshot->node_method_call_receiver_derefs;
    sema->node_method_call_explicit_traits =
        snapshot->node_method_call_explicit_traits;
    sema->node_implicit_array_type_indices =
        snapshot->node_implicit_array_type_indices;
    sema->node_is_type_expr = snapshot->node_is_type_expr;
    sema->node_const_known  = snapshot->node_const_known;
    sema->node_const_values = snapshot->node_const_values;
    *snapshot               = (SemaNodeTablesSnapshot){0};
}

internal u64 sema_hash_u64(u64 hash, u64 value)
{
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

internal u32 sema_generic_instantiation_symbol(const Lexer* lexer,
                                               Sema*        sema,
                                               u32          base_symbol,
                                               const u32*   arg_types,
                                               u32          arg_count)
{
    (void)sema;
    u64 hash = 1469598103934665603ULL;
    hash     = sema_hash_u64(hash, base_symbol);
    for (u32 i = 0; i < arg_count; ++i) {
        hash = sema_hash_u64(hash, arg_types[i]);
    }

    string          base      = lex_symbol(lexer, base_symbol);
    cstr            name_cstr = (cstr)arena_format(&temp_arena,
                                                   STRINGP "_g_%08x",
                                                   STRINGV(base),
                                                   (u32)(hash ^ (hash >> 32)));
    string          name      = string_from((u8*)name_cstr, strlen(name_cstr));
    InternAddResult ignored   = {0};
    return lex_add_symbol((Lexer*)lexer, name, &ignored);
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
internal u32 sema_find_runtime_interpolated_string_node(const Ast*  ast,
                                                        const Sema* sema,
                                                        u32         node_index);

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

internal u8 sema_decode_packed_integer_escape(string source, usize* io_index)
{
    u8 escaped = source.data[(*io_index)++];
    switch (escaped) {
    case '\\':
        return '\\';
    case 'n':
        return '\n';
    case 'r':
        return '\r';
    case 't':
        return '\t';
    case '0':
        return '\0';
    case 'a':
        return '\a';
    case 'b':
        return '\b';
    case 'f':
        return '\f';
    case 'v':
        return '\v';
    case '\'':
        return '\'';
    case 'x':
        {
            u8  value  = 0;
            u32 digits = 0;
            while (*io_index < source.count && digits < 2) {
                u8 hex    = source.data[*io_index];
                u8 nibble = 0;
                if (hex >= '0' && hex <= '9') {
                    nibble = (u8)(hex - '0');
                } else if (hex >= 'a' && hex <= 'f') {
                    nibble = (u8)(10 + (hex - 'a'));
                } else if (hex >= 'A' && hex <= 'F') {
                    nibble = (u8)(10 + (hex - 'A'));
                } else {
                    break;
                }
                value = (u8)((value << 4) | nibble);
                digits++;
                (*io_index)++;
            }
            return digits == 0 ? 'x' : value;
        }
    default:
        return escaped;
    }
}

internal usize sema_decode_utf8_first(const u8* data, usize count)
{
    if (count == 0) {
        return 0;
    }

    u8 b0 = data[0];
    if (b0 < 0x80) {
        return 1;
    }
    if ((b0 & 0xE0) == 0xC0) {
        if (count < 2 || (data[1] & 0xC0) != 0x80) {
            return 0;
        }
        u32 cp = ((u32)(b0 & 0x1F) << 6) | (u32)(data[1] & 0x3F);
        return cp < 0x80 ? 0 : 2;
    }
    if ((b0 & 0xF0) == 0xE0) {
        if (count < 3 || (data[1] & 0xC0) != 0x80 || (data[2] & 0xC0) != 0x80) {
            return 0;
        }
        u32 cp = ((u32)(b0 & 0x0F) << 12) | ((u32)(data[1] & 0x3F) << 6) |
                 (u32)(data[2] & 0x3F);
        return cp < 0x800 || (cp >= 0xD800 && cp <= 0xDFFF) ? 0 : 3;
    }
    if ((b0 & 0xF8) == 0xF0) {
        if (count < 4 || (data[1] & 0xC0) != 0x80 || (data[2] & 0xC0) != 0x80 ||
            (data[3] & 0xC0) != 0x80) {
            return 0;
        }
        u32 cp = ((u32)(b0 & 0x07) << 18) | ((u32)(data[1] & 0x3F) << 12) |
                 ((u32)(data[2] & 0x3F) << 6) | (u32)(data[3] & 0x3F);
        return cp < 0x10000 || cp > 0x10FFFF ? 0 : 4;
    }
    return 0;
}

internal bool sema_packed_integer_literal_is_character(const Lexer*   lexer,
                                                       const AstNode* node)
{
    const Token* token = &lexer->tokens[node->token_index];
    usize        start = token->offset + 1;
    usize        end   = lex_token_end_offset(lexer, token);
    if (end <= start) {
        return false;
    }
    end--;

    u8    bytes[8];
    usize count = 0;
    for (usize i = start; i < end && count < sizeof(bytes); ++i) {
        u8 ch = lexer->source.source.data[i];
        if (ch == '\\' && i + 1 < end) {
            i++;
            ch = sema_decode_packed_integer_escape(lexer->source.source, &i);
            i--;
        }
        bytes[count++] = ch;
    }

    return count > 0 && sema_decode_utf8_first(bytes, count) == count;
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

internal bool sema_infer_node_type(const Lexer* lexer,
                                   const Ast*   ast,
                                   Sema*        sema,
                                   u32          node_index,
                                   u32          expected_type,
                                   u32*         out_type_index);

internal u32 sema_generic_param_position(const Ast*              ast,
                                         const AstGenericParams* generic,
                                         u32                     symbol_handle)
{
    for (u32 i = 0; i < generic->symbol_count; ++i) {
        if (ast->generic_param_symbols[generic->first_symbol + i] ==
            symbol_handle) {
            return i;
        }
    }
    return U32_MAX;
}

bool sema_bind_generic_type_node(const Lexer*            lexer,
                                 const Ast*              ast,
                                 Sema*                   sema,
                                 const AstGenericParams* generic,
                                 u32                     type_node_index,
                                 u32                     actual_type,
                                 Array(u32) arg_types)
{
    const AstNode* type_node = &ast->nodes[type_node_index];
    if (type_node->kind == AK_SymbolRef) {
        u32 pos = sema_generic_param_position(ast, generic, type_node->a);
        if (pos != U32_MAX) {
            actual_type = sema_materialise_type(sema, actual_type);
            if (arg_types[pos] == sema_no_type()) {
                arg_types[pos] = actual_type;
                return true;
            }
            return sema_type_matches(sema, arg_types[pos], actual_type);
        }
    }

    if (actual_type == sema_no_type()) {
        return false;
    }

    if (type_node->kind == AK_TypePointer &&
        sema->types[actual_type].kind == STK_Pointer) {
        return sema_bind_generic_type_node(
            lexer,
            ast,
            sema,
            generic,
            type_node->a,
            sema->types[actual_type].first_param_type,
            arg_types);
    }

    if (type_node->kind == AK_TypeSlice &&
        sema->types[actual_type].kind == STK_Slice) {
        return sema_bind_generic_type_node(
            lexer,
            ast,
            sema,
            generic,
            type_node->a,
            sema->types[actual_type].first_param_type,
            arg_types);
    }

    if (type_node->kind == AK_TypeDynamicArray &&
        sema->types[actual_type].kind == STK_DynamicArray) {
        return sema_bind_generic_type_node(
            lexer,
            ast,
            sema,
            generic,
            type_node->b,
            sema->types[actual_type].first_param_type,
            arg_types);
    }

    if (type_node->kind == AK_TypeArray &&
        sema->types[actual_type].kind == STK_Array) {
        return sema_bind_generic_type_node(
            lexer,
            ast,
            sema,
            generic,
            type_node->b,
            sema->types[actual_type].first_param_type,
            arg_types);
    }

    if (type_node->kind == AK_TypeApply) {
        const AstTypeApplyInfo* apply  = &ast->type_applications[type_node->a];
        const AstNode*          target = &ast->nodes[apply->target_node_index];
        if (target->kind == AK_SymbolRef) {
            u32 decl_index = sema_find_decl(sema, target->a);
            if (decl_index != sema_no_decl() &&
                sema->decls[decl_index].kind == SK_GenericTypeAlias &&
                sema->decls[decl_index].value_node_index != sema_no_decl()) {
                u32 template_node = sema_unwrap_type_candidate_node(
                    ast, sema->decls[decl_index].value_node_index);
                return sema_bind_generic_type_node(lexer,
                                                   ast,
                                                   sema,
                                                   generic,
                                                   template_node,
                                                   actual_type,
                                                   arg_types);
            }
        }
    }

    if (type_node->kind == AK_TypePlex &&
        (sema->types[actual_type].kind == STK_Plex ||
         sema->types[actual_type].kind == STK_Union)) {
        const AstPlexTypeInfo* plex   = &ast->plex_types[type_node->a];
        const SemaType*        actual = &sema->types[actual_type];
        if (((plex->flags & APTF_Union) != 0) !=
            (sema->types[actual_type].kind == STK_Union)) {
            return false;
        }
        if (plex->field_count != actual->param_count) {
            return false;
        }
        for (u32 i = 0; i < plex->field_count; ++i) {
            const AstPlexField* field =
                &ast->plex_fields[plex->first_field + i];
            if (field->symbol_handle !=
                sema->type_param_symbols[actual->first_param_type + i]) {
                return false;
            }
            if (!sema_bind_generic_type_node(
                    lexer,
                    ast,
                    sema,
                    generic,
                    field->type_node_index,
                    sema->type_param_types[actual->first_param_type + i],
                    arg_types)) {
                return false;
            }
        }
        return true;
    }

    if (type_node->kind == AK_TypeEnum &&
        sema->types[actual_type].kind == STK_Enum) {
        const AstEnumTypeInfo* enum_type = &ast->enum_types[type_node->a];
        const SemaType*        actual    = &sema->types[actual_type];
        if (enum_type->variant_count != actual->param_count) {
            return false;
        }

        i64 next_discriminant = 0;
        for (u32 i = 0; i < enum_type->variant_count; ++i) {
            const AstEnumVariant* variant =
                &ast->enum_variants[enum_type->first_variant + i];
            if (variant->symbol_handle !=
                sema->type_param_symbols[actual->first_param_type + i]) {
                return false;
            }

            i64 discriminant = next_discriminant;
            if (variant->value_node_index != U32_MAX &&
                !sema_try_eval_integer_constant(lexer,
                                                ast,
                                                sema,
                                                variant->value_node_index,
                                                &discriminant)) {
                return false;
            }
            if (discriminant !=
                sema->type_param_values[actual->first_param_type + i]) {
                return false;
            }
            next_discriminant = discriminant + 1;

            u32 payload_type =
                sema->type_param_types[actual->first_param_type + i];
            if (variant->type_node_index == U32_MAX) {
                if (payload_type != sema_no_type()) {
                    return false;
                }
                continue;
            }
            if (payload_type == sema_no_type() ||
                !sema_bind_generic_type_node(lexer,
                                             ast,
                                             sema,
                                             generic,
                                             variant->type_node_index,
                                             payload_type,
                                             arg_types)) {
                return false;
            }
        }
        return true;
    }

    u32                  expected = sema_no_type();
    SemaTypeSubstitution subst    = {
        .param_symbols = &ast->generic_param_symbols[generic->first_symbol],
        .arg_types     = arg_types,
        .count         = generic->symbol_count,
    };
    if (!sema_resolve_type_node_ex(
            lexer, ast, sema, type_node_index, subst, &expected)) {
        return false;
    }
    return sema_type_matches(sema, expected, actual_type);
}

internal void sema_collect_generic_arg_nodes(const Ast* ast,
                                             u32        arg_node,
                                             Array(u32) * out_nodes)
{
    const AstNode* node = &ast->nodes[arg_node];
    if (node->kind == AK_Tuple) {
        for (u32 i = 0; i < node->b; ++i) {
            array_push(*out_nodes, ast->tuple_items[node->a + i]);
        }
    } else {
        array_push(*out_nodes, arg_node);
    }
}

internal bool
sema_generic_arg_lists_match(const Sema*                       sema,
                             const SemaGenericFnInstantiation* inst,
                             const u32*                        arg_types,
                             u32                               arg_count)
{
    if (inst->arg_count != arg_count) {
        return false;
    }
    for (u32 i = 0; i < arg_count; ++i) {
        if (sema->type_param_types[inst->first_arg_type + i] != arg_types[i]) {
            return false;
        }
    }
    return true;
}

internal const AstGenericParams*
sema_decl_generic_params(const Ast* ast, const Sema* sema, u32 decl_index)
{
    const SemaDecl* decl = &sema->decls[decl_index];
    if (decl->value_node_index != sema_no_decl() &&
        ast->nodes[decl->value_node_index].kind == AK_FnDef) {
        u32 signature_index =
            sema_fn_def_generic_params_index(ast, decl->value_node_index);
        if (signature_index != U32_MAX) {
            return &ast->generic_params[signature_index];
        }
    }

    const SemaMethod* method = sema_find_method_for_decl(sema, decl_index);
    if (method != NULL && method->generic_params_index != U32_MAX) {
        return &ast->generic_params[method->generic_params_index];
    }

    return NULL;
}

internal const AstFnSignature*
sema_decl_fn_signature(const Ast* ast, const Sema* sema, u32 decl_index)
{
    const SemaDecl* decl = &sema->decls[decl_index];
    if (decl->value_node_index == sema_no_decl() ||
        decl->value_node_index >= array_count(ast->nodes) ||
        ast->nodes[decl->value_node_index].kind != AK_FnDef) {
        return NULL;
    }
    const AstNode* fn_def   = &ast->nodes[decl->value_node_index];
    const AstNode* fn_start = &ast->nodes[fn_def->a];
    if (fn_start->kind != AK_FnStart ||
        fn_start->a >= array_count(ast->fn_signatures)) {
        return NULL;
    }
    return &ast->fn_signatures[fn_start->a];
}

internal bool
sema_validate_where_constraint_args(const Lexer*            lexer,
                                    const Ast*              ast,
                                    Sema*                   sema,
                                    const AstGenericParams* generic,
                                    Array(u32) arg_types,
                                    u32       first_constraint,
                                    u32       constraint_count,
                                    ErrorSpan site_span);

internal bool sema_type_satisfies_trait_constraint(const Lexer* lexer,
                                                   const Ast*   ast,
                                                   Sema*        sema,
                                                   u32          actual_type,
                                                   u32          trait_symbol,
                                                   ErrorSpan    site_span)
{
    actual_type = sema_materialise_type(sema, actual_type);

    for (u32 i = 0; i < array_count(sema->methods); ++i) {
        const SemaMethod* method = &sema->methods[i];
        if (!method->is_trait_impl) {
            continue;
        }

        const SemaDecl*   decl              = &sema->decls[method->decl_index];
        const Lexer*      source_lexer      = lexer;
        const Ast*        source_ast        = ast;
        Sema*             source_sema       = sema;
        u32               source_decl_index = method->decl_index;
        const SemaMethod* source_method     = method;
        bool imported = decl->import_module_index != sema_no_decl();
        if (imported) {
            if (!sema_imported_decl_source(sema,
                                           decl,
                                           &source_lexer,
                                           &source_ast,
                                           &source_sema,
                                           &source_decl_index)) {
                continue;
            }
            source_method =
                sema_find_method_for_decl(source_sema, source_decl_index);
            if (source_method == NULL) {
                continue;
            }
        }

        if (!sema_method_matches_trait_symbol(
                lexer, source_lexer, source_ast, source_method, trait_symbol)) {
            continue;
        }

        u32 source_actual_type = imported
                                     ? sema_import_type((Lexer*)source_lexer,
                                                        source_sema,
                                                        lexer,
                                                        sema,
                                                        actual_type)
                                     : actual_type;

        const AstNode* impl_node =
            &source_ast->nodes[source_method->impl_node_index];
        const AstImplInfo*      impl    = &source_ast->impls[impl_node->a];
        const AstGenericParams* generic = NULL;
        Array(u32) source_arg_types     = NULL;
        bool target_matched             = false;
        if (impl->generic_params_index != U32_MAX) {
            generic = &source_ast->generic_params[impl->generic_params_index];
            for (u32 j = 0; j < generic->symbol_count; ++j) {
                array_push(source_arg_types, sema_no_type());
            }
            target_matched =
                sema_bind_generic_type_node(source_lexer,
                                            source_ast,
                                            source_sema,
                                            generic,
                                            impl->target_type_node_index,
                                            source_actual_type,
                                            source_arg_types);
            for (u32 j = 0; target_matched && j < generic->symbol_count; ++j) {
                if (source_arg_types[j] == sema_no_type()) {
                    target_matched = false;
                }
            }
        } else {
            u32 target_type = sema_no_type();
            target_matched =
                sema_resolve_type_node(source_lexer,
                                       source_ast,
                                       source_sema,
                                       impl->target_type_node_index,
                                       &target_type) &&
                sema_type_matches(source_sema, target_type, source_actual_type);
        }

        if (!target_matched) {
            array_free(source_arg_types);
            continue;
        }

        if (generic != NULL &&
            !sema_validate_where_constraint_args(source_lexer,
                                                 source_ast,
                                                 source_sema,
                                                 generic,
                                                 source_arg_types,
                                                 impl->first_constraint,
                                                 impl->constraint_count,
                                                 site_span)) {
            array_free(source_arg_types);
            return false;
        }
        array_free(source_arg_types);
        return true;
    }

    return error_0304_type_mismatch(
        lexer->source,
        site_span,
        string_format(&temp_arena,
                      STRINGP " implementation",
                      STRINGV(lex_symbol(lexer, trait_symbol))),
        sema_type_name(lexer, sema, &temp_arena, actual_type));
}

internal bool
sema_validate_where_constraint_args(const Lexer*            lexer,
                                    const Ast*              ast,
                                    Sema*                   sema,
                                    const AstGenericParams* generic,
                                    Array(u32) arg_types,
                                    u32       first_constraint,
                                    u32       constraint_count,
                                    ErrorSpan site_span)
{
    for (u32 i = 0; i < constraint_count; ++i) {
        const AstWhereConstraint* constraint =
            &ast->where_constraints[first_constraint + i];
        u32 param_pos =
            sema_generic_param_position(ast, generic, constraint->param_symbol);
        if (param_pos == U32_MAX) {
            return error_0304_type_mismatch(
                lexer->source,
                sema_node_span(lexer,
                               &ast->nodes[constraint->trait_type_node_index]),
                s("generic type parameter"),
                lex_symbol(lexer, constraint->param_symbol));
        }

        u32 trait_symbol = sema_trait_symbol_from_type_node(
            ast, constraint->trait_type_node_index);
        if (trait_symbol == U32_MAX) {
            return error_0304_type_mismatch(
                lexer->source,
                sema_node_span(lexer,
                               &ast->nodes[constraint->trait_type_node_index]),
                s("known trait"),
                s("non-trait type expression"));
        }

        if (!sema_type_satisfies_trait_constraint(lexer,
                                                  ast,
                                                  sema,
                                                  arg_types[param_pos],
                                                  trait_symbol,
                                                  site_span)) {
            return false;
        }
    }
    return true;
}

internal bool
sema_validate_decl_where_constraints(const Lexer*            lexer,
                                     const Ast*              ast,
                                     Sema*                   sema,
                                     u32                     decl_index,
                                     const AstGenericParams* generic,
                                     Array(u32) arg_types,
                                     ErrorSpan site_span)
{
    const AstFnSignature* signature =
        sema_decl_fn_signature(ast, sema, decl_index);
    if (signature != NULL &&
        !sema_validate_where_constraint_args(lexer,
                                             ast,
                                             sema,
                                             generic,
                                             arg_types,
                                             signature->first_constraint,
                                             signature->constraint_count,
                                             site_span)) {
        return false;
    }

    const SemaMethod* method = sema_find_method_for_decl(sema, decl_index);
    if (method != NULL && method->impl_node_index < array_count(ast->nodes)) {
        const AstNode* impl_node = &ast->nodes[method->impl_node_index];
        if (impl_node->kind == AK_Impl &&
            impl_node->a < array_count(ast->impls)) {
            const AstImplInfo* impl = &ast->impls[impl_node->a];
            if (!sema_validate_where_constraint_args(lexer,
                                                     ast,
                                                     sema,
                                                     generic,
                                                     arg_types,
                                                     impl->first_constraint,
                                                     impl->constraint_count,
                                                     site_span)) {
                return false;
            }
        }
    }
    return true;
}

internal bool sema_where_constraints_include_trait(const Lexer* lexer,
                                                   const Ast*   ast,
                                                   u32    first_constraint,
                                                   u32    constraint_count,
                                                   u32    param_symbol,
                                                   string trait_name)
{
    for (u32 i = 0; i < constraint_count; ++i) {
        const AstWhereConstraint* constraint =
            &ast->where_constraints[first_constraint + i];
        if (constraint->param_symbol != param_symbol) {
            continue;
        }
        u32 constraint_trait = sema_trait_symbol_from_type_node(
            ast, constraint->trait_type_node_index);
        if (constraint_trait != U32_MAX &&
            string_eq(lex_symbol(lexer, constraint_trait), trait_name)) {
            return true;
        }
    }
    return false;
}

internal bool sema_generic_param_has_trait_constraint(const Ast*   ast,
                                                      const Lexer* lexer,
                                                      u32          node_index,
                                                      u32          param_symbol,
                                                      string       trait_name)
{
    u32 fn_start_index =
        sema_ast_enclosing_function_start_node(ast, node_index);
    if (fn_start_index != U32_MAX) {
        const AstNode* fn_start = &ast->nodes[fn_start_index];
        if (fn_start->a < array_count(ast->fn_signatures)) {
            const AstFnSignature* signature = &ast->fn_signatures[fn_start->a];
            if (sema_where_constraints_include_trait(
                    lexer,
                    ast,
                    signature->first_constraint,
                    signature->constraint_count,
                    param_symbol,
                    trait_name)) {
                return true;
            }
        }
    }

    u32 impl_node_index = sema_enclosing_impl_node_index(ast, node_index);
    if (impl_node_index != U32_MAX) {
        const AstNode* impl_node = &ast->nodes[impl_node_index];
        if (impl_node->a < array_count(ast->impls)) {
            const AstImplInfo* impl = &ast->impls[impl_node->a];
            if (sema_where_constraints_include_trait(lexer,
                                                     ast,
                                                     impl->first_constraint,
                                                     impl->constraint_count,
                                                     param_symbol,
                                                     trait_name)) {
                return true;
            }
        }
    }
    return false;
}

internal u32 sema_direct_generic_param_symbol_from_type_node(
    const Lexer* lexer, const Ast* ast, u32 type_node_index)
{
    if (type_node_index >= array_count(ast->nodes)) {
        return U32_MAX;
    }

    const AstNode* type_node = &ast->nodes[type_node_index];
    while (
        (type_node->kind == AK_Expression || type_node->kind == AK_Statement) &&
        type_node->a < array_count(ast->nodes)) {
        type_node_index = type_node->a;
        type_node       = &ast->nodes[type_node_index];
    }

    if (type_node->kind != AK_SymbolRef) {
        return U32_MAX;
    }

    for (u32 i = 0; i < g_sema_type_subst.count; ++i) {
        if (g_sema_type_subst.param_symbols[i] == type_node->a) {
            return type_node->a;
        }
    }
    (void)lexer;
    return U32_MAX;
}

internal u32 sema_direct_generic_param_symbol_from_value_node(
    const Lexer* lexer, const Ast* ast, const Sema* sema, u32 node_index)
{
    if (node_index >= array_count(ast->nodes)) {
        return U32_MAX;
    }

    const AstNode* node = &ast->nodes[node_index];
    while ((node->kind == AK_Expression || node->kind == AK_Statement) &&
           node->a < array_count(ast->nodes)) {
        node_index = node->a;
        node       = &ast->nodes[node_index];
    }

    u32 local_index = sema->node_local_indices[node_index];
    if (local_index != sema_no_local() &&
        local_index < array_count(sema->locals)) {
        const SemaLocal* local = &sema->locals[local_index];
        return sema_direct_generic_param_symbol_from_type_node(
            lexer, ast, local->type_node_index);
    }
    return U32_MAX;
}

internal u32 sema_trait_method_receiver_node_index(const Ast* ast,
                                                   u32        call_node_index,
                                                   u32        call_arg_offset)
{
    const AstNode*     call = &ast->nodes[call_node_index];
    const AstCallInfo* info = &ast->calls[call->b];
    if (call_arg_offset > 0 && info->arg_count >= call_arg_offset) {
        return ast->call_args[info->first_arg];
    }

    const AstNode* callee = &ast->nodes[call->a];
    if (callee->kind == AK_Index && callee->a < array_count(ast->nodes)) {
        callee = &ast->nodes[callee->a];
    }
    if (callee->kind == AK_Field) {
        return callee->a;
    }
    return U32_MAX;
}

internal bool sema_require_trait_constraint_for_generic_receiver(
    const Lexer*      lexer,
    const Ast*        ast,
    const Sema*       sema,
    u32               call_node_index,
    u32               call_arg_offset,
    const Lexer*      source_lexer,
    const Ast*        source_ast,
    const SemaMethod* source_method)
{
    if (!source_method->is_trait_impl) {
        return true;
    }

    u32 receiver_node_index = sema_trait_method_receiver_node_index(
        ast, call_node_index, call_arg_offset);
    u32 param_symbol = sema_direct_generic_param_symbol_from_value_node(
        lexer, ast, sema, receiver_node_index);
    if (param_symbol == U32_MAX) {
        return true;
    }

    const AstNode* impl_node =
        source_method->impl_node_index < array_count(source_ast->nodes)
            ? &source_ast->nodes[source_method->impl_node_index]
            : NULL;
    if (impl_node == NULL || impl_node->kind != AK_Impl ||
        impl_node->a >= array_count(source_ast->impls)) {
        return true;
    }
    const AstImplInfo* impl         = &source_ast->impls[impl_node->a];
    u32                trait_symbol = sema_trait_symbol_from_type_node(
        source_ast, impl->trait_type_node_index);
    if (trait_symbol == U32_MAX) {
        return true;
    }
    string trait_name = lex_symbol(source_lexer, trait_symbol);
    if (sema_generic_param_has_trait_constraint(
            ast, lexer, call_node_index, param_symbol, trait_name)) {
        return true;
    }

    return error_0304_type_mismatch(
        lexer->source,
        sema_node_span(lexer, &ast->nodes[receiver_node_index]),
        string_format(&temp_arena, STRINGP " constraint", STRINGV(trait_name)),
        lex_symbol(lexer, param_symbol));
}

internal bool sema_emit_generic_function_instantiation(const Lexer* lexer,
                                                       const Ast*   ast,
                                                       Sema*        sema,
                                                       u32          decl_index,
                                                       Array(u32) arg_types,
                                                       u32* out_symbol,
                                                       u32* out_type)
{
    const SemaDecl* decl = &sema->decls[decl_index];
    ASSERT(decl->kind == SK_GenericFunction, "Expected generic function");

    const AstGenericParams* generic =
        sema_decl_generic_params(ast, sema, decl_index);
    ASSERT(generic != NULL, "Expected generic function parameters");

    for (u32 i = 0; i < array_count(sema->generic_fn_instantiations); ++i) {
        SemaGenericFnInstantiation* inst = &sema->generic_fn_instantiations[i];
        if (inst->template_decl_index == decl_index &&
            sema_generic_arg_lists_match(
                sema, inst, arg_types, generic->symbol_count)) {
            *out_symbol = inst->symbol_handle;
            *out_type   = inst->type_index;
            return true;
        }
    }

    u32 symbol = sema_generic_instantiation_symbol(
        lexer, sema, decl->symbol_handle, arg_types, generic->symbol_count);

    u32 first_arg_type = (u32)array_count(sema->type_param_types);
    for (u32 i = 0; i < generic->symbol_count; ++i) {
        array_push(sema->type_param_types, arg_types[i]);
        array_push(sema->type_param_symbols,
                   ast->generic_param_symbols[generic->first_symbol + i]);
        array_push(sema->type_param_values, 0);
    }

    SemaTypeSubstitution previous = g_sema_type_subst;
    g_sema_type_subst             = (SemaTypeSubstitution){
        .param_symbols = &ast->generic_param_symbols[generic->first_symbol],
        .arg_types     = arg_types,
        .count         = generic->symbol_count,
    };

    SemaNodeTablesSnapshot node_tables = sema_snapshot_node_tables(sema);
    sema->node_lowered_symbol_handles[decl->value_node_index] = symbol;
    if (!sema_collect_function_locals(lexer,
                                      ast,
                                      decl_index,
                                      symbol,
                                      sema_no_scope(),
                                      decl->value_node_index,
                                      sema)) {
        g_sema_type_subst = previous;
        sema_restore_node_tables(sema, &node_tables);
        return false;
    }

    u32 type_index = sema_no_type();
    if (!sema_infer_node_type(lexer,
                              ast,
                              sema,
                              decl->value_node_index,
                              sema_no_type(),
                              &type_index)) {
        g_sema_type_subst = previous;
        sema_restore_node_tables(sema, &node_tables);
        return false;
    }
    g_sema_type_subst = previous;

    u32 root_scope    = sema->node_scope_indices[decl->value_node_index];
    array_push(
        sema->generic_fn_instantiations,
        (SemaGenericFnInstantiation){
            .symbol_handle       = symbol,
            .template_decl_index = decl_index,
            .fn_node_index       = decl->value_node_index,
            .root_scope_index    = root_scope,
            .type_index          = type_index,
            .first_arg_type      = first_arg_type,
            .arg_count           = generic->symbol_count,
            .node_decl_indices   = sema_copy_u32_array(sema->node_decl_indices),
            .node_local_indices = sema_copy_u32_array(sema->node_local_indices),
            .node_scope_indices = sema_copy_u32_array(sema->node_scope_indices),
            .node_lowered_symbol_handles =
                sema_copy_u32_array(sema->node_lowered_symbol_handles),
            .node_type_indices = sema_copy_u32_array(sema->node_type_indices),
            .node_method_call_decl_indices =
                sema_copy_u32_array(sema->node_method_call_decl_indices),
            .node_method_call_receiver_refs =
                sema_copy_bool_array(sema->node_method_call_receiver_refs),
            .node_method_call_receiver_derefs =
                sema_copy_bool_array(sema->node_method_call_receiver_derefs),
            .node_method_call_explicit_traits =
                sema_copy_bool_array(sema->node_method_call_explicit_traits),
        });
    sema_restore_node_tables(sema, &node_tables);

    *out_symbol = symbol;
    *out_type   = type_index;
    return true;
}

internal bool
sema_instantiate_imported_generic_function(const Lexer*    lexer,
                                           const Ast*      ast,
                                           Sema*           sema,
                                           const SemaDecl* decl,
                                           const u32*      explicit_arg_nodes,
                                           u32             explicit_arg_count,
                                           const AstCallInfo* call,
                                           u32                expected_type,
                                           u32*               out_symbol,
                                           u32*               out_type)
{
    const Lexer* source_lexer      = NULL;
    const Ast*   source_ast        = NULL;
    Sema*        source_sema       = NULL;
    u32          source_decl_index = sema_no_decl();
    if (!sema_imported_decl_source(sema,
                                   decl,
                                   &source_lexer,
                                   &source_ast,
                                   &source_sema,
                                   &source_decl_index)) {
        return false;
    }

    const SemaDecl* source_decl = &source_sema->decls[source_decl_index];
    ASSERT(source_decl->kind == SK_GenericFunction,
           "Expected imported generic function");

    const AstNode* source_fn_def =
        &source_ast->nodes[source_decl->value_node_index];
    const AstNode* source_fn_start = &source_ast->nodes[source_fn_def->a];
    const AstFnSignature* source_signature =
        &source_ast->fn_signatures[source_fn_start->a];
    const AstGenericParams* source_generic =
        sema_decl_generic_params(source_ast, source_sema, source_decl_index);
    ASSERT(source_generic != NULL, "Expected generic function parameters");

    if (explicit_arg_count != 0 &&
        explicit_arg_count != source_generic->symbol_count) {
        return error_0313_argument_count_mismatch(
            lexer->source,
            sema_node_span(source_lexer, source_fn_def),
            source_generic->symbol_count,
            explicit_arg_count);
    }

    if (call != NULL) {
        u32 required_count =
            sema_signature_required_param_count(source_ast, source_signature);
        if (call->arg_count < required_count ||
            call->arg_count > source_signature->param_count) {
            u32 expected_count = call->arg_count < required_count
                                     ? required_count
                                     : source_signature->param_count;
            return error_0313_argument_count_mismatch(
                lexer->source,
                sema_node_span(source_lexer, source_fn_def),
                expected_count,
                call->arg_count);
        }
    }

    Array(u32) source_arg_types = NULL;
    for (u32 i = 0; i < source_generic->symbol_count; ++i) {
        array_push(source_arg_types, sema_no_type());
    }

    if (explicit_arg_count != 0) {
        for (u32 i = 0; i < explicit_arg_count; ++i) {
            u32 dst_type = sema_no_type();
            if (!sema_resolve_type_node(
                    lexer, ast, sema, explicit_arg_nodes[i], &dst_type)) {
                array_free(source_arg_types);
                return false;
            }
            source_arg_types[i] = sema_import_type(
                (Lexer*)source_lexer, source_sema, lexer, sema, dst_type);
        }
    }

    u32 call_arg_count = call != NULL ? call->arg_count : 0;
    for (u32 i = 0; i < call_arg_count; ++i) {
        const AstParam* source_param =
            &source_ast->params[source_signature->first_param + i];
        u32 arg_node = ast->call_args[call->first_arg + i];
        if (!sema_call_arg_value_node(
                lexer, ast, source_lexer, source_param, arg_node, &arg_node)) {
            array_free(source_arg_types);
            return false;
        }
        u32 expected_src = sema_no_type();
        u32 expected_dst = sema_no_type();

        if (explicit_arg_count != 0) {
            SemaTypeSubstitution source_subst = {
                .param_symbols =
                    &source_ast
                         ->generic_param_symbols[source_generic->first_symbol],
                .arg_types = source_arg_types,
                .count     = source_generic->symbol_count,
            };
            if (!sema_resolve_type_node_ex(source_lexer,
                                           source_ast,
                                           source_sema,
                                           source_param->type_node_index,
                                           source_subst,
                                           &expected_src)) {
                array_free(source_arg_types);
                return false;
            }
            expected_dst = sema_import_type(
                (Lexer*)lexer, sema, source_lexer, source_sema, expected_src);
        }

        u32 actual_dst = sema_no_type();
        if (!sema_infer_node_type(
                lexer, ast, sema, arg_node, expected_dst, &actual_dst)) {
            array_free(source_arg_types);
            return false;
        }
        u32 actual_src = sema_import_type(
            (Lexer*)source_lexer, source_sema, lexer, sema, actual_dst);

        if (!sema_bind_generic_type_node(source_lexer,
                                         source_ast,
                                         source_sema,
                                         source_generic,
                                         source_param->type_node_index,
                                         actual_src,
                                         source_arg_types)) {
            array_free(source_arg_types);
            return error_0304_type_mismatch(
                lexer->source,
                sema_node_span(lexer, &ast->nodes[arg_node]),
                s("generic parameter-compatible argument"),
                sema_type_name(lexer, sema, &temp_arena, actual_dst));
        }
    }

    if (expected_type != sema_no_type() &&
        source_signature->return_type_node_index != U32_MAX) {
        u32 expected_src = sema_import_type(
            (Lexer*)source_lexer, source_sema, lexer, sema, expected_type);
        (void)sema_bind_generic_type_node(
            source_lexer,
            source_ast,
            source_sema,
            source_generic,
            source_signature->return_type_node_index,
            expected_src,
            source_arg_types);
    }

    for (u32 i = 0; i < source_generic->symbol_count; ++i) {
        if (source_arg_types[i] == sema_no_type()) {
            u32 symbol =
                source_ast
                    ->generic_param_symbols[source_generic->first_symbol + i];
            array_free(source_arg_types);
            return error_0304_type_mismatch(
                lexer->source,
                sema_node_span(source_lexer, source_fn_def),
                s("inferable generic type parameter"),
                lex_symbol(source_lexer, symbol));
        }
    }

    if (!sema_validate_decl_where_constraints(
            source_lexer,
            source_ast,
            source_sema,
            source_decl_index,
            source_generic,
            source_arg_types,
            sema_node_span(source_lexer, source_fn_def))) {
        array_free(source_arg_types);
        return false;
    }

    u32 source_symbol = U32_MAX;
    u32 source_type   = sema_no_type();
    if (!sema_emit_generic_function_instantiation(source_lexer,
                                                  source_ast,
                                                  source_sema,
                                                  source_decl_index,
                                                  source_arg_types,
                                                  &source_symbol,
                                                  &source_type)) {
        array_free(source_arg_types);
        return false;
    }
    array_free(source_arg_types);

    *out_symbol =
        sema_import_symbol_handle((Lexer*)lexer, source_lexer, source_symbol);
    *out_type = sema_import_type(
        (Lexer*)lexer, sema, source_lexer, source_sema, source_type);
    return true;
}

internal bool sema_instantiate_generic_function(const Lexer* lexer,
                                                const Ast*   ast,
                                                Sema*        sema,
                                                u32          decl_index,
                                                const u32*   explicit_arg_nodes,
                                                u32          explicit_arg_count,
                                                const AstCallInfo* call,
                                                u32  expected_type,
                                                u32* out_symbol,
                                                u32* out_type)
{
    const SemaDecl* decl = &sema->decls[decl_index];
    ASSERT(decl->kind == SK_GenericFunction, "Expected generic function");

    if (decl->import_module_index != sema_no_decl()) {
        return sema_instantiate_imported_generic_function(lexer,
                                                          ast,
                                                          sema,
                                                          decl,
                                                          explicit_arg_nodes,
                                                          explicit_arg_count,
                                                          call,
                                                          expected_type,
                                                          out_symbol,
                                                          out_type);
    }

    const AstNode*          fn_def    = &ast->nodes[decl->value_node_index];
    const AstNode*          fn_start  = &ast->nodes[fn_def->a];
    const AstFnSignature*   signature = &ast->fn_signatures[fn_start->a];
    const AstGenericParams* generic =
        sema_decl_generic_params(ast, sema, decl_index);
    ASSERT(generic != NULL, "Expected generic function parameters");

    if (explicit_arg_count != 0 &&
        explicit_arg_count != generic->symbol_count) {
        return error_0313_argument_count_mismatch(lexer->source,
                                                  sema_node_span(lexer, fn_def),
                                                  generic->symbol_count,
                                                  explicit_arg_count);
    }

    if (call != NULL) {
        u32 required_count =
            sema_signature_required_param_count(ast, signature);
        if (call->arg_count < required_count ||
            call->arg_count > signature->param_count) {
            u32 expected_count = call->arg_count < required_count
                                     ? required_count
                                     : signature->param_count;
            return error_0313_argument_count_mismatch(
                lexer->source,
                sema_node_span(lexer, fn_def),
                expected_count,
                call->arg_count);
        }
    }

    Array(u32) arg_types = NULL;
    for (u32 i = 0; i < generic->symbol_count; ++i) {
        array_push(arg_types, sema_no_type());
    }

    if (explicit_arg_count != 0) {
        for (u32 i = 0; i < explicit_arg_count; ++i) {
            u32 type_index = sema_no_type();
            if (!sema_resolve_type_node(
                    lexer, ast, sema, explicit_arg_nodes[i], &type_index)) {
                array_free(arg_types);
                return false;
            }
            arg_types[i] = type_index;
        }
    }

    u32 call_arg_count = call != NULL ? call->arg_count : 0;
    for (u32 i = 0; i < call_arg_count; ++i) {
        const AstParam* param    = &ast->params[signature->first_param + i];
        u32             arg_node = ast->call_args[call->first_arg + i];
        if (!sema_call_arg_value_node(
                lexer, ast, lexer, param, arg_node, &arg_node)) {
            array_free(arg_types);
            return false;
        }
        u32 expected_arg = sema_no_type();

        if (explicit_arg_count != 0) {
            SemaTypeSubstitution subst = {
                .param_symbols =
                    &ast->generic_param_symbols[generic->first_symbol],
                .arg_types = arg_types,
                .count     = generic->symbol_count,
            };
            if (!sema_resolve_type_node_ex(lexer,
                                           ast,
                                           sema,
                                           param->type_node_index,
                                           subst,
                                           &expected_arg)) {
                array_free(arg_types);
                return false;
            }
        }

        u32 actual = sema_no_type();
        if (!sema_infer_node_type(
                lexer, ast, sema, arg_node, expected_arg, &actual)) {
            array_free(arg_types);
            return false;
        }

        if (!sema_bind_generic_type_node(lexer,
                                         ast,
                                         sema,
                                         generic,
                                         param->type_node_index,
                                         actual,
                                         arg_types)) {
            array_free(arg_types);
            return error_0304_type_mismatch(
                lexer->source,
                sema_node_span(lexer, &ast->nodes[arg_node]),
                s("generic parameter-compatible argument"),
                sema_type_name(lexer, sema, &temp_arena, actual));
        }
    }

    if (expected_type != sema_no_type() &&
        signature->return_type_node_index != U32_MAX) {
        (void)sema_bind_generic_type_node(lexer,
                                          ast,
                                          sema,
                                          generic,
                                          signature->return_type_node_index,
                                          expected_type,
                                          arg_types);
    }

    for (u32 i = 0; i < generic->symbol_count; ++i) {
        if (arg_types[i] == sema_no_type()) {
            u32 symbol = ast->generic_param_symbols[generic->first_symbol + i];
            array_free(arg_types);
            return error_0304_type_mismatch(
                lexer->source,
                sema_node_span(lexer, fn_def),
                s("inferable generic type parameter"),
                lex_symbol(lexer, symbol));
        }
    }

    ErrorSpan constraint_site = sema_node_span(lexer, fn_def);
    if (call != NULL && call->arg_count > 0) {
        constraint_site =
            sema_node_span(lexer, &ast->nodes[ast->call_args[call->first_arg]]);
    }
    if (!sema_validate_decl_where_constraints(lexer,
                                              ast,
                                              sema,
                                              decl_index,
                                              generic,
                                              arg_types,
                                              constraint_site)) {
        array_free(arg_types);
        return false;
    }

    if (!sema_emit_generic_function_instantiation(
            lexer, ast, sema, decl_index, arg_types, out_symbol, out_type)) {
        array_free(arg_types);
        return false;
    }
    array_free(arg_types);
    return true;
}

typedef struct {
    u32  decl_index;
    u32  fn_type_index;
    u32  lowered_symbol_handle;
    bool receiver_ref;
    bool receiver_deref;
} SemaResolvedMethodCall;

internal bool sema_seed_local_from_call_arg_expected(const Lexer* lexer,
                                                     const Ast*   ast,
                                                     Sema*        sema,
                                                     u32 arg_node_index,
                                                     u32 expected_type);

internal bool sema_method_matches_trait_symbol(const Lexer*      lexer,
                                               const Lexer*      source_lexer,
                                               const Ast*        source_ast,
                                               const SemaMethod* source_method,
                                               u32               trait_symbol)
{
    if (trait_symbol == U32_MAX) {
        return true;
    }
    if (!source_method->is_trait_impl ||
        source_method->impl_node_index >= array_count(source_ast->nodes)) {
        return false;
    }
    const AstNode* impl_node =
        &source_ast->nodes[source_method->impl_node_index];
    if (impl_node->kind != AK_Impl ||
        impl_node->a >= array_count(source_ast->impls)) {
        return false;
    }
    const AstImplInfo* impl = &source_ast->impls[impl_node->a];
    if (impl->trait_type_node_index == U32_MAX) {
        return false;
    }
    u32 source_trait_symbol = sema_trait_symbol_from_type_node(
        source_ast, impl->trait_type_node_index);
    return source_trait_symbol != U32_MAX &&
           string_eq(lex_symbol(source_lexer, source_trait_symbol),
                     lex_symbol(lexer, trait_symbol));
}

internal bool sema_try_resolve_method_call(const Lexer* lexer,
                                           const Ast*   ast,
                                           Sema*        sema,
                                           u32          call_node_index,
                                           u32          receiver_type,
                                           u32          method_symbol,
                                           u32          explicit_arg_node_index,
                                           u32          explicit_trait_symbol,
                                           u32          call_arg_offset,
                                           bool         seed_arg_contexts,
                                           bool*        out_found,
                                           SemaResolvedMethodCall* out_call)
{
    const AstNode*     call_node            = &ast->nodes[call_node_index];
    const AstCallInfo* call                 = &ast->calls[call_node->b];
    *out_found                              = false;
    bool                   found_trait_call = false;
    SemaResolvedMethodCall trait_call       = {0};

    if (!seed_arg_contexts &&
        !sema_import_implicit_core_method((Lexer*)lexer, sema, method_symbol)) {
        return false;
    }

    u32 first_pass = explicit_trait_symbol == U32_MAX ? 0 : 1;
    for (u32 pass = first_pass; pass < 2; ++pass) {
        bool want_trait_method = pass == 1;
        for (u32 i = 0; i < array_count(sema->methods); ++i) {
            const SemaMethod* method = &sema->methods[i];
            if (method->symbol_handle != method_symbol ||
                method->is_trait_impl != want_trait_method) {
                continue;
            }

            const SemaDecl*   decl         = &sema->decls[method->decl_index];
            const Lexer*      source_lexer = lexer;
            const Ast*        source_ast   = ast;
            Sema*             source_sema  = sema;
            u32               source_decl_index = method->decl_index;
            const SemaMethod* source_method     = method;
            bool imported = decl->import_module_index != sema_no_decl();
            if (imported) {
                if (!sema_imported_decl_source(sema,
                                               decl,
                                               &source_lexer,
                                               &source_ast,
                                               &source_sema,
                                               &source_decl_index)) {
                    continue;
                }
                source_method =
                    sema_find_method_for_decl(source_sema, source_decl_index);
                if (source_method == NULL) {
                    continue;
                }
            }
            if (!sema_method_matches_trait_symbol(lexer,
                                                  source_lexer,
                                                  source_ast,
                                                  source_method,
                                                  explicit_trait_symbol)) {
                continue;
            }

            const SemaDecl* source_decl =
                &source_sema->decls[source_decl_index];
            if (source_decl->value_node_index >=
                array_count(source_ast->nodes)) {
                continue;
            }
            const AstNode* source_fn_def =
                &source_ast->nodes[source_decl->value_node_index];
            if (source_fn_def->kind != AK_FnDef ||
                source_fn_def->a >= array_count(source_ast->nodes)) {
                continue;
            }
            const AstNode* source_fn_start =
                &source_ast->nodes[source_fn_def->a];
            if (source_fn_start->a >= array_count(source_ast->fn_signatures)) {
                continue;
            }
            const AstFnSignature* source_signature =
                &source_ast->fn_signatures[source_fn_start->a];
            if (source_signature->param_count == 0) {
                continue;
            }

            const AstGenericParams* generic =
                source_decl->kind == SK_GenericFunction
                    ? sema_decl_generic_params(
                          source_ast, source_sema, source_decl_index)
                    : NULL;
            Array(u32) source_arg_types = NULL;
            if (generic != NULL) {
                if (seed_arg_contexts) {
                    array_free(source_arg_types);
                    continue;
                }
                for (u32 j = 0; j < generic->symbol_count; ++j) {
                    array_push(source_arg_types, sema_no_type());
                }
                if (explicit_arg_node_index != U32_MAX) {
                    Array(u32) explicit_arg_nodes = NULL;
                    sema_collect_generic_arg_nodes(
                        ast, explicit_arg_node_index, &explicit_arg_nodes);
                    u32 explicit_arg_count =
                        (u32)array_count(explicit_arg_nodes);
                    if (explicit_arg_count != generic->symbol_count) {
                        array_free(explicit_arg_nodes);
                        array_free(source_arg_types);
                        return error_0313_argument_count_mismatch(
                            lexer->source,
                            sema_node_span(
                                lexer, &ast->nodes[explicit_arg_node_index]),
                            generic->symbol_count,
                            explicit_arg_count);
                    }
                    for (u32 j = 0; j < array_count(explicit_arg_nodes); ++j) {
                        u32 explicit_type = sema_no_type();
                        if (!sema_resolve_type_node(lexer,
                                                    ast,
                                                    sema,
                                                    explicit_arg_nodes[j],
                                                    &explicit_type)) {
                            array_free(explicit_arg_nodes);
                            array_free(source_arg_types);
                            return false;
                        }
                        source_arg_types[j] =
                            imported ? sema_import_type((Lexer*)source_lexer,
                                                        source_sema,
                                                        lexer,
                                                        sema,
                                                        explicit_type)
                                     : explicit_type;
                    }
                    array_free(explicit_arg_nodes);
                }
            } else if (explicit_arg_node_index != U32_MAX) {
                continue;
            }

            u32 source_receiver_type =
                imported ? sema_import_type((Lexer*)source_lexer,
                                            source_sema,
                                            lexer,
                                            sema,
                                            receiver_type)
                         : receiver_type;
            u32  source_target_receiver = source_receiver_type;
            bool target_matched         = true;
            if (generic != NULL) {
                target_matched = sema_bind_generic_type_node(
                    source_lexer,
                    source_ast,
                    source_sema,
                    generic,
                    source_method->target_type_node_index,
                    source_target_receiver,
                    source_arg_types);
                if (!target_matched && source_receiver_type != sema_no_type() &&
                    source_sema->types[source_receiver_type].kind ==
                        STK_Pointer) {
                    source_target_receiver =
                        source_sema->types[source_receiver_type]
                            .first_param_type;
                    target_matched = sema_bind_generic_type_node(
                        source_lexer,
                        source_ast,
                        source_sema,
                        generic,
                        source_method->target_type_node_index,
                        source_target_receiver,
                        source_arg_types);
                }
            } else {
                u32 source_target_type = sema_no_type();
                target_matched         = sema_resolve_type_node(
                    source_lexer,
                    source_ast,
                    source_sema,
                    source_method->target_type_node_index,
                    &source_target_type);
                if (target_matched && imported) {
                    u32 target_type = sema_import_type((Lexer*)lexer,
                                                       sema,
                                                       source_lexer,
                                                       source_sema,
                                                       source_target_type);
                    target_matched =
                        sema_type_matches(sema, target_type, receiver_type);
                    if (!target_matched && receiver_type != sema_no_type() &&
                        sema->types[receiver_type].kind == STK_Pointer) {
                        u32 pointer_target =
                            sema->types[receiver_type].first_param_type;
                        target_matched = sema_type_matches(
                            sema, target_type, pointer_target);
                    }
                } else if (target_matched) {
                    target_matched = sema_type_matches(source_sema,
                                                       source_target_type,
                                                       source_target_receiver);
                    if (!target_matched &&
                        source_receiver_type != sema_no_type() &&
                        source_sema->types[source_receiver_type].kind ==
                            STK_Pointer) {
                        source_target_receiver =
                            source_sema->types[source_receiver_type]
                                .first_param_type;
                        target_matched =
                            sema_type_matches(source_sema,
                                              source_target_type,
                                              source_target_receiver);
                    }
                }
            }
            if (!target_matched) {
                array_free(source_arg_types);
                continue;
            }

            SemaTypeSubstitution subst = {0};
            if (generic != NULL) {
                subst = (SemaTypeSubstitution){
                    .param_symbols =
                        &source_ast
                             ->generic_param_symbols[generic->first_symbol],
                    .arg_types = source_arg_types,
                    .count     = generic->symbol_count,
                };
            }

            const AstParam* self_param =
                &source_ast->params[source_signature->first_param];
            u32 source_expected_self = sema_no_type();
            if (!sema_resolve_type_node_ex(source_lexer,
                                           source_ast,
                                           source_sema,
                                           self_param->type_node_index,
                                           subst,
                                           &source_expected_self)) {
                array_free(source_arg_types);
                return false;
            }
            u32 expected_self = imported
                                    ? sema_import_type((Lexer*)lexer,
                                                       sema,
                                                       source_lexer,
                                                       source_sema,
                                                       source_expected_self)
                                    : source_expected_self;
            u32 receiver_match_type =
                imported ? receiver_type : source_receiver_type;
            Sema* receiver_match_sema = imported ? sema : source_sema;

            bool receiver_ref         = false;
            bool receiver_deref       = false;
            if (!sema_type_matches(
                    receiver_match_sema, expected_self, receiver_match_type)) {
                u32 pointer_receiver = sema_add_pointer_type(
                    receiver_match_sema, receiver_match_type);
                if (sema_type_matches(
                        receiver_match_sema, expected_self, pointer_receiver)) {
                    receiver_ref = true;
                } else if (receiver_match_type != sema_no_type() &&
                           receiver_match_sema->types[receiver_match_type]
                                   .kind == STK_Pointer &&
                           sema_type_matches(
                               receiver_match_sema,
                               expected_self,
                               receiver_match_sema->types[receiver_match_type]
                                   .first_param_type)) {
                    receiver_deref = true;
                } else {
                    array_free(source_arg_types);
                    continue;
                }
            }

            u32 required_count = sema_signature_required_param_count(
                source_ast, source_signature);
            if (required_count > 0) {
                required_count--;
            }
            u32 max_count = source_signature->param_count - 1;
            if (call->arg_count < call_arg_offset) {
                array_free(source_arg_types);
                return error_0313_argument_count_mismatch(
                    lexer->source,
                    sema_node_span(lexer, call_node),
                    call_arg_offset,
                    call->arg_count);
            }
            u32 visible_arg_count = call->arg_count - call_arg_offset;
            if (visible_arg_count < required_count ||
                visible_arg_count > max_count) {
                array_free(source_arg_types);
                u32 expected_count = visible_arg_count < required_count
                                         ? required_count
                                         : max_count;
                return error_0313_argument_count_mismatch(
                    lexer->source,
                    sema_node_span(lexer, call_node),
                    expected_count + call_arg_offset,
                    call->arg_count);
            }

            for (u32 j = 0; j < visible_arg_count; ++j) {
                const AstParam* source_param =
                    &source_ast->params[source_signature->first_param + 1 + j];
                u32 expected_source = sema_no_type();
                if (!sema_resolve_type_node_ex(source_lexer,
                                               source_ast,
                                               source_sema,
                                               source_param->type_node_index,
                                               subst,
                                               &expected_source)) {
                    array_free(source_arg_types);
                    return false;
                }
                u32 expected_dst = imported ? sema_import_type((Lexer*)lexer,
                                                               sema,
                                                               source_lexer,
                                                               source_sema,
                                                               expected_source)
                                            : expected_source;
                u32 arg_node =
                    ast->call_args[call->first_arg + call_arg_offset + j];
                if (!sema_call_arg_value_node(lexer,
                                              ast,
                                              source_lexer,
                                              source_param,
                                              arg_node,
                                              &arg_node)) {
                    array_free(source_arg_types);
                    return false;
                }
                if (seed_arg_contexts) {
                    if (!sema_seed_local_from_call_arg_expected(
                            lexer, ast, sema, arg_node, expected_dst)) {
                        array_free(source_arg_types);
                        return false;
                    }
                    continue;
                }
                u32 arg_type = sema_no_type();
                if (!sema_infer_node_type(
                        lexer, ast, sema, arg_node, expected_dst, &arg_type)) {
                    array_free(source_arg_types);
                    return false;
                }
                if (!sema_type_matches(sema, expected_dst, arg_type)) {
                    array_free(source_arg_types);
                    return error_0304_type_mismatch(
                        lexer->source,
                        sema_node_span(lexer, &ast->nodes[arg_node]),
                        sema_type_name(lexer, sema, &temp_arena, expected_dst),
                        sema_type_name(lexer, sema, &temp_arena, arg_type));
                }
            }

            for (u32 j = 0; generic != NULL && j < generic->symbol_count; ++j) {
                if (source_arg_types[j] == sema_no_type()) {
                    u32 symbol =
                        source_ast
                            ->generic_param_symbols[generic->first_symbol + j];
                    array_free(source_arg_types);
                    return error_0304_type_mismatch(
                        lexer->source,
                        sema_node_span(lexer, &ast->nodes[call_node->a]),
                        s("inferable generic type parameter"),
                        lex_symbol(source_lexer, symbol));
                }
            }

            if (generic != NULL &&
                !sema_validate_decl_where_constraints(
                    source_lexer,
                    source_ast,
                    source_sema,
                    source_decl_index,
                    generic,
                    source_arg_types,
                    sema_node_span(source_lexer, source_fn_def))) {
                array_free(source_arg_types);
                return false;
            }

            if (!sema_require_trait_constraint_for_generic_receiver(
                    lexer,
                    ast,
                    sema,
                    call_node_index,
                    call_arg_offset,
                    source_lexer,
                    source_ast,
                    source_method)) {
                array_free(source_arg_types);
                return false;
            }

            u32 symbol        = source_decl->symbol_handle;
            u32 fn_type_index = source_decl->type_index;
            if (source_decl->kind == SK_GenericFunction) {
                if (!sema_emit_generic_function_instantiation(source_lexer,
                                                              source_ast,
                                                              source_sema,
                                                              source_decl_index,
                                                              source_arg_types,
                                                              &symbol,
                                                              &fn_type_index)) {
                    array_free(source_arg_types);
                    return false;
                }
            } else if (fn_type_index == sema_no_type()) {
                if (!sema_infer_node_type(source_lexer,
                                          source_ast,
                                          source_sema,
                                          source_decl->value_node_index,
                                          sema_no_type(),
                                          &fn_type_index)) {
                    array_free(source_arg_types);
                    return false;
                }
            }
            array_free(source_arg_types);

            if (imported) {
                symbol = sema_import_symbol_handle(
                    (Lexer*)lexer, source_lexer, symbol);
                fn_type_index = sema_import_type((Lexer*)lexer,
                                                 sema,
                                                 source_lexer,
                                                 source_sema,
                                                 fn_type_index);
            }

            SemaResolvedMethodCall resolved_call = {
                .decl_index            = method->decl_index,
                .fn_type_index         = fn_type_index,
                .lowered_symbol_handle = symbol,
                .receiver_ref          = receiver_ref,
                .receiver_deref        = receiver_deref,
            };
            if (!want_trait_method) {
                *out_call  = resolved_call;
                *out_found = true;
                return true;
            }
            if (found_trait_call) {
                return error_0348_ambiguous_method_call(
                    lexer->source,
                    sema_node_span(lexer, &ast->nodes[call_node->a]),
                    lex_symbol(lexer, method_symbol));
            }
            trait_call       = resolved_call;
            found_trait_call = true;
        }
        if (found_trait_call) {
            *out_call  = trait_call;
            *out_found = true;
            return true;
        }
    }

    return true;
}

internal u32 sema_string_edit_distance(string left, string right)
{
    Array(u32) previous = NULL;
    Array(u32) current  = NULL;
    for (u32 i = 0; i <= right.count; ++i) {
        array_push(previous, i);
        array_push(current, 0);
    }

    for (u32 i = 1; i <= left.count; ++i) {
        current[0] = i;
        for (u32 j = 1; j <= right.count; ++j) {
            u32 substitution = previous[j - 1] +
                               (left.data[i - 1] == right.data[j - 1] ? 0 : 1);
            u32 insertion    = current[j - 1] + 1;
            u32 deletion     = previous[j] + 1;
            u32 best   = substitution < insertion ? substitution : insertion;
            current[j] = best < deletion ? best : deletion;
        }
        for (u32 j = 0; j <= right.count; ++j) {
            previous[j] = current[j];
        }
    }

    u32 result = previous[right.count];
    array_free(previous);
    array_free(current);
    return result;
}

internal bool sema_member_suggestion_is_close(string misspelled,
                                              string candidate,
                                              u32    distance)
{
    if (candidate.count == 0) {
        return false;
    }

    usize longer =
        misspelled.count > candidate.count ? misspelled.count : candidate.count;
    return distance <= 2 || distance * 3 <= longer;
}

internal void sema_consider_member_suggestion(string  misspelled,
                                              string  candidate,
                                              string* best,
                                              u32*    best_distance)
{
    u32 distance = sema_string_edit_distance(misspelled, candidate);
    if (!sema_member_suggestion_is_close(misspelled, candidate, distance)) {
        return;
    }
    if (best->count == 0 || distance < *best_distance ||
        (distance == *best_distance && candidate.count < best->count)) {
        *best          = candidate;
        *best_distance = distance;
    }
}

internal bool sema_record_member_type(const Sema* sema,
                                      u32         record_type,
                                      u32         member_symbol,
                                      u32*        out_type)
{
    if (record_type == sema_no_type() ||
        record_type >= array_count(sema->types)) {
        return false;
    }

    const SemaType* record = &sema->types[record_type];
    if (record->kind != STK_Plex && record->kind != STK_Union) {
        return false;
    }
    for (u32 i = 0; i < record->param_count; ++i) {
        if (sema->type_param_symbols[record->first_param_type + i] ==
            member_symbol) {
            if (out_type != NULL) {
                *out_type =
                    sema->type_param_types[record->first_param_type + i];
            }
            return true;
        }
    }
    return false;
}

internal bool
sema_method_target_matches_receiver(const Lexer*      lexer,
                                    Sema*             sema,
                                    const Lexer*      source_lexer,
                                    const Ast*        source_ast,
                                    Sema*             source_sema,
                                    const SemaMethod* source_method,
                                    u32               receiver_type,
                                    bool              imported)
{
    u32 source_target_type = sema_no_type();
    if (source_method->target_type_node_index <
        array_count(source_sema->node_type_indices)) {
        source_target_type =
            source_sema
                ->node_type_indices[source_method->target_type_node_index];
    }
    if (source_target_type == sema_no_type()) {
        if (imported) {
            return false;
        }
        if (!sema_resolve_type_node(source_lexer,
                                    source_ast,
                                    source_sema,
                                    source_method->target_type_node_index,
                                    &source_target_type)) {
            return false;
        }
    }

    u32  target_type = imported ? sema_import_type((Lexer*)lexer,
                                                   sema,
                                                   source_lexer,
                                                   source_sema,
                                                   source_target_type)
                                : source_target_type;
    bool matches     = sema_type_matches(sema, target_type, receiver_type);
    if (!matches && receiver_type != sema_no_type() &&
        receiver_type < array_count(sema->types) &&
        sema->types[receiver_type].kind == STK_Pointer) {
        matches = sema_type_matches(
            sema, target_type, sema->types[receiver_type].first_param_type);
    }
    return matches;
}

internal bool sema_find_private_method_member(const Lexer* lexer,
                                              const Ast*   ast,
                                              Sema*        sema,
                                              u32          receiver_type,
                                              u32          member_symbol,
                                              string*      out_module)
{
    (void)ast;

    if (sema->program == NULL) {
        return false;
    }

    string member = lex_symbol(lexer, member_symbol);
    for (u32 module_index = 0;
         module_index < array_count(sema->program->modules);
         ++module_index) {
        if (module_index == sema->current_module_index) {
            continue;
        }

        const ModuleInfo* module       = &sema->program->modules[module_index];
        const Lexer*      source_lexer = &module->front_end.lexer;
        const Ast*        source_ast   = &module->front_end.ast;
        Sema*             source_sema  = (Sema*)&module->front_end.sema;
        for (u32 i = 0; i < array_count(source_sema->methods); ++i) {
            const SemaMethod* source_method = &source_sema->methods[i];
            if (source_method->is_public ||
                !source_method->first_param_is_receiver ||
                !string_eq(
                    lex_symbol(source_lexer, source_method->symbol_handle),
                    member)) {
                continue;
            }

            if (!sema_method_target_matches_receiver(lexer,
                                                     sema,
                                                     source_lexer,
                                                     source_ast,
                                                     source_sema,
                                                     source_method,
                                                     receiver_type,
                                                     true)) {
                continue;
            }

            *out_module = module->qualified_name;
            return true;
        }
    }

    return false;
}

internal void sema_consider_method_member_suggestions(const Lexer* lexer,
                                                      const Ast*   ast,
                                                      Sema*        sema,
                                                      u32     receiver_type,
                                                      string  member,
                                                      string* best,
                                                      u32*    best_distance)
{
    for (u32 i = 0; i < array_count(sema->methods); ++i) {
        const SemaMethod* method = &sema->methods[i];
        if (!method->first_param_is_receiver ||
            method->decl_index >= array_count(sema->decls)) {
            continue;
        }

        const SemaDecl*   decl              = &sema->decls[method->decl_index];
        const Lexer*      source_lexer      = lexer;
        const Ast*        source_ast        = ast;
        Sema*             source_sema       = sema;
        u32               source_decl_index = method->decl_index;
        const SemaMethod* source_method     = method;
        bool imported = decl->import_module_index != sema_no_decl();
        if (imported) {
            if (!sema_imported_decl_source(sema,
                                           decl,
                                           &source_lexer,
                                           &source_ast,
                                           &source_sema,
                                           &source_decl_index)) {
                continue;
            }
            source_method =
                sema_find_method_for_decl(source_sema, source_decl_index);
            if (source_method == NULL ||
                !source_method->first_param_is_receiver) {
                continue;
            }
        }

        if (!sema_method_target_matches_receiver(lexer,
                                                 sema,
                                                 source_lexer,
                                                 source_ast,
                                                 source_sema,
                                                 source_method,
                                                 receiver_type,
                                                 imported)) {
            continue;
        }

        sema_consider_member_suggestion(
            member,
            lex_symbol(source_lexer, source_method->symbol_handle),
            best,
            best_distance);
    }

    if (sema->program == NULL) {
        return;
    }

    for (u32 module_index = 0;
         module_index < array_count(sema->program->modules);
         ++module_index) {
        if (module_index == sema->current_module_index) {
            continue;
        }

        const ModuleInfo* module       = &sema->program->modules[module_index];
        const Lexer*      source_lexer = &module->front_end.lexer;
        const Ast*        source_ast   = &module->front_end.ast;
        Sema*             source_sema  = (Sema*)&module->front_end.sema;
        for (u32 i = 0; i < array_count(source_sema->methods); ++i) {
            const SemaMethod* source_method = &source_sema->methods[i];
            if (!source_method->first_param_is_receiver) {
                continue;
            }

            if (!sema_method_target_matches_receiver(lexer,
                                                     sema,
                                                     source_lexer,
                                                     source_ast,
                                                     source_sema,
                                                     source_method,
                                                     receiver_type,
                                                     true)) {
                continue;
            }

            sema_consider_member_suggestion(
                member,
                lex_symbol(source_lexer, source_method->symbol_handle),
                best,
                best_distance);
        }
    }
}

internal string sema_member_suggestion(const Lexer* lexer,
                                       const Ast*   ast,
                                       Sema*        sema,
                                       u32          receiver_type,
                                       u32          member_symbol)
{
    string member          = lex_symbol(lexer, member_symbol);
    string best            = {0};
    u32    best_distance   = U32_MAX;

    u32 member_target_type = sema_member_target_type(sema, receiver_type);
    if (member_target_type != sema_no_type() &&
        member_target_type < array_count(sema->types)) {
        const SemaType* type = &sema->types[member_target_type];
        if (type->kind == STK_Plex || type->kind == STK_Union) {
            for (u32 i = 0; i < type->param_count; ++i) {
                sema_consider_member_suggestion(
                    member,
                    lex_symbol(
                        lexer,
                        sema->type_param_symbols[type->first_param_type + i]),
                    &best,
                    &best_distance);
            }
        } else if (type->kind == STK_String || type->kind == STK_Slice ||
                   type->kind == STK_Array || type->kind == STK_DynamicArray) {
            sema_consider_member_suggestion(
                member, s("data"), &best, &best_distance);
            sema_consider_member_suggestion(
                member, s("count"), &best, &best_distance);
            if (type->kind == STK_DynamicArray) {
                sema_consider_member_suggestion(
                    member, s("capacity"), &best, &best_distance);
            }
        }
    }

    sema_consider_method_member_suggestions(
        lexer, ast, sema, receiver_type, member, &best, &best_distance);
    return best;
}

internal bool sema_error_unknown_member(const Lexer*   lexer,
                                        const Ast*     ast,
                                        Sema*          sema,
                                        const AstNode* member_node,
                                        u32            receiver_type,
                                        u32            member_symbol)
{
    string receiver = sema_type_name(lexer, sema, &temp_arena, receiver_type);
    string private_module = {0};
    if (sema_find_private_method_member(
            lexer, ast, sema, receiver_type, member_symbol, &private_module)) {
        return error_0354_private_method(lexer->source,
                                         sema_node_span(lexer, member_node),
                                         lex_symbol(lexer, member_symbol),
                                         receiver,
                                         private_module);
    }

    string suggestion =
        sema_member_suggestion(lexer, ast, sema, receiver_type, member_symbol);
    return error_0353_unknown_member(lexer->source,
                                     sema_node_span(lexer, member_node),
                                     lex_symbol(lexer, member_symbol),
                                     receiver,
                                     suggestion);
}

internal bool sema_try_resolve_associated_call(const Lexer* lexer,
                                               const Ast*   ast,
                                               Sema*        sema,
                                               u32          call_node_index,
                                               u32          target_type,
                                               u32          method_symbol,
                                               u32   explicit_trait_symbol,
                                               bool* out_found,
                                               SemaResolvedMethodCall* out_call)
{
    const AstNode*     call_node     = &ast->nodes[call_node_index];
    const AstCallInfo* call          = &ast->calls[call_node->b];
    *out_found                       = false;

    u32 associated_target_node_index = U32_MAX;
    if (call_node->a < array_count(ast->nodes)) {
        u32            callee_index = sema_unwrap_expr_node(ast, call_node->a);
        const AstNode* callee       = &ast->nodes[callee_index];
        if (callee->kind == AK_Field) {
            associated_target_node_index = callee->a;
        } else if (callee->kind == AK_Index &&
                   callee->a < array_count(ast->nodes) &&
                   ast->nodes[callee->a].kind == AK_Field) {
            associated_target_node_index = ast->nodes[callee->a].a;
        }
    }

    for (u32 i = 0; i < array_count(sema->methods); ++i) {
        const SemaMethod* method = &sema->methods[i];
        if (method->symbol_handle != method_symbol) {
            continue;
        }

        const SemaDecl*   decl              = &sema->decls[method->decl_index];
        const Lexer*      source_lexer      = lexer;
        const Ast*        source_ast        = ast;
        Sema*             source_sema       = sema;
        u32               source_decl_index = method->decl_index;
        const SemaMethod* source_method     = method;
        bool imported = decl->import_module_index != sema_no_decl();
        if (imported) {
            if (!sema_imported_decl_source(sema,
                                           decl,
                                           &source_lexer,
                                           &source_ast,
                                           &source_sema,
                                           &source_decl_index)) {
                continue;
            }
            source_method =
                sema_find_method_for_decl(source_sema, source_decl_index);
            if (source_method == NULL) {
                continue;
            }
        }
        if (!sema_method_matches_trait_symbol(lexer,
                                              source_lexer,
                                              source_ast,
                                              source_method,
                                              explicit_trait_symbol)) {
            continue;
        }

        if (explicit_trait_symbol == U32_MAX &&
            associated_target_node_index < array_count(ast->nodes) &&
            source_method->target_type_node_index <
                array_count(source_ast->nodes)) {
            u32 target_node_index = associated_target_node_index;
            while (ast->nodes[target_node_index].kind == AK_Expression ||
                   ast->nodes[target_node_index].kind == AK_Statement) {
                target_node_index = ast->nodes[target_node_index].a;
            }

            u32 source_target_node_index =
                source_method->target_type_node_index;
            while (source_ast->nodes[source_target_node_index].kind ==
                       AK_Expression ||
                   source_ast->nodes[source_target_node_index].kind ==
                       AK_Statement) {
                source_target_node_index =
                    source_ast->nodes[source_target_node_index].a;
            }

            const AstNode* target_node = &ast->nodes[target_node_index];
            const AstNode* source_target_node =
                &source_ast->nodes[source_target_node_index];
            if (target_node->kind == AK_SymbolRef &&
                source_target_node->kind == AK_SymbolRef &&
                !string_eq(lex_symbol(lexer, target_node->a),
                           lex_symbol(source_lexer, source_target_node->a))) {
                continue;
            }
        }

        const SemaDecl* source_decl = &source_sema->decls[source_decl_index];
        const AstNode*  source_fn_def =
            &source_ast->nodes[source_decl->value_node_index];
        const AstNode* source_fn_start = &source_ast->nodes[source_fn_def->a];
        const AstFnSignature* source_signature =
            &source_ast->fn_signatures[source_fn_start->a];

        const AstGenericParams* generic =
            source_decl->kind == SK_GenericFunction
                ? sema_decl_generic_params(
                      source_ast, source_sema, source_decl_index)
                : NULL;
        Array(u32) source_arg_types = NULL;
        if (generic != NULL) {
            for (u32 j = 0; j < generic->symbol_count; ++j) {
                array_push(source_arg_types, sema_no_type());
            }
        }

        u32  source_target_type = imported
                                      ? sema_import_type((Lexer*)source_lexer,
                                                         source_sema,
                                                         lexer,
                                                         sema,
                                                         target_type)
                                      : target_type;
        bool target_matched     = true;
        if (generic != NULL) {
            target_matched = sema_bind_generic_type_node(
                source_lexer,
                source_ast,
                source_sema,
                generic,
                source_method->target_type_node_index,
                source_target_type,
                source_arg_types);
        } else {
            u32 expected_target = sema_no_type();
            target_matched =
                sema_resolve_type_node(source_lexer,
                                       source_ast,
                                       source_sema,
                                       source_method->target_type_node_index,
                                       &expected_target) &&
                sema_type_matches(
                    source_sema, expected_target, source_target_type);
        }
        if (!target_matched) {
            array_free(source_arg_types);
            continue;
        }

        if (!method->is_associated) {
            array_free(source_arg_types);
            if (!method->first_param_is_receiver && !method->returns_self) {
                return error_0344_invalid_associated_function_return(
                    lexer->source,
                    sema_node_span(lexer, call_node),
                    lex_symbol(lexer, method_symbol));
            }
            continue;
        }

        SemaTypeSubstitution subst = {0};
        if (generic != NULL) {
            subst = (SemaTypeSubstitution){
                .param_symbols =
                    &source_ast->generic_param_symbols[generic->first_symbol],
                .arg_types = source_arg_types,
                .count     = generic->symbol_count,
            };
        }

        if (source_signature->return_type_node_index == U32_MAX) {
            array_free(source_arg_types);
            continue;
        }
        u32 source_return = sema_no_type();
        if (!sema_resolve_type_node_ex(source_lexer,
                                       source_ast,
                                       source_sema,
                                       source_signature->return_type_node_index,
                                       subst,
                                       &source_return)) {
            array_free(source_arg_types);
            return false;
        }
        (void)source_return;

        u32 required_count =
            sema_signature_required_param_count(source_ast, source_signature);
        u32 max_count = source_signature->param_count;
        if (call->arg_count < required_count || call->arg_count > max_count) {
            array_free(source_arg_types);
            u32 expected_count =
                call->arg_count < required_count ? required_count : max_count;
            return error_0313_argument_count_mismatch(
                lexer->source,
                sema_node_span(lexer, call_node),
                expected_count,
                call->arg_count);
        }

        for (u32 j = 0; j < call->arg_count; ++j) {
            const AstParam* source_param =
                &source_ast->params[source_signature->first_param + j];
            u32 expected_source = sema_no_type();
            if (!sema_resolve_type_node_ex(source_lexer,
                                           source_ast,
                                           source_sema,
                                           source_param->type_node_index,
                                           subst,
                                           &expected_source)) {
                array_free(source_arg_types);
                return false;
            }
            u32 expected_dst = imported ? sema_import_type((Lexer*)lexer,
                                                           sema,
                                                           source_lexer,
                                                           source_sema,
                                                           expected_source)
                                        : expected_source;
            u32 arg_node     = ast->call_args[call->first_arg + j];
            if (!sema_call_arg_value_node(lexer,
                                          ast,
                                          source_lexer,
                                          source_param,
                                          arg_node,
                                          &arg_node)) {
                array_free(source_arg_types);
                return false;
            }
            u32 arg_type = sema_no_type();
            if (!sema_infer_node_type(
                    lexer, ast, sema, arg_node, expected_dst, &arg_type)) {
                array_free(source_arg_types);
                return false;
            }
            if (!sema_type_matches(sema, expected_dst, arg_type)) {
                array_free(source_arg_types);
                return error_0304_type_mismatch(
                    lexer->source,
                    sema_node_span(lexer, &ast->nodes[arg_node]),
                    sema_type_name(lexer, sema, &temp_arena, expected_dst),
                    sema_type_name(lexer, sema, &temp_arena, arg_type));
            }
        }

        for (u32 j = 0; generic != NULL && j < generic->symbol_count; ++j) {
            if (source_arg_types[j] == sema_no_type()) {
                u32 symbol =
                    source_ast
                        ->generic_param_symbols[generic->first_symbol + j];
                array_free(source_arg_types);
                return error_0304_type_mismatch(
                    lexer->source,
                    sema_node_span(lexer, &ast->nodes[call_node->a]),
                    s("inferable generic type parameter"),
                    lex_symbol(source_lexer, symbol));
            }
        }

        if (generic != NULL &&
            !sema_validate_decl_where_constraints(
                source_lexer,
                source_ast,
                source_sema,
                source_decl_index,
                generic,
                source_arg_types,
                sema_node_span(source_lexer, source_fn_def))) {
            array_free(source_arg_types);
            return false;
        }

        u32 symbol        = source_decl->symbol_handle;
        u32 fn_type_index = source_decl->type_index;
        if (source_decl->kind == SK_GenericFunction) {
            if (!sema_emit_generic_function_instantiation(source_lexer,
                                                          source_ast,
                                                          source_sema,
                                                          source_decl_index,
                                                          source_arg_types,
                                                          &symbol,
                                                          &fn_type_index)) {
                array_free(source_arg_types);
                return false;
            }
        } else if (fn_type_index == sema_no_type()) {
            if (!sema_infer_node_type(source_lexer,
                                      source_ast,
                                      source_sema,
                                      source_decl->value_node_index,
                                      sema_no_type(),
                                      &fn_type_index)) {
                array_free(source_arg_types);
                return false;
            }
        }
        array_free(source_arg_types);

        if (imported) {
            symbol =
                sema_import_symbol_handle((Lexer*)lexer, source_lexer, symbol);
            fn_type_index = sema_import_type(
                (Lexer*)lexer, sema, source_lexer, source_sema, fn_type_index);
        }

        *out_call = (SemaResolvedMethodCall){
            .decl_index            = method->decl_index,
            .fn_type_index         = fn_type_index,
            .lowered_symbol_handle = symbol,
            .receiver_ref          = false,
            .receiver_deref        = false,
        };
        *out_found = true;
        return true;
    }

    return true;
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

internal bool sema_block_is_expr_block_body(const Ast* ast, u32 block_index);

internal bool sema_assign_destructure_pattern_type(const Lexer* lexer,
                                                   const Ast*   ast,
                                                   Sema*        sema,
                                                   u32          pattern_index,
                                                   u32          value_type);

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
    case APK_ForValue:
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
            if (pattern->c != 0) {
                u32 qualified_type = sema_no_type();
                if (!sema_resolve_type_node(
                        lexer, ast, sema, pattern->c - 1, &qualified_type)) {
                    return false;
                }
                if (qualified_type != value_type) {
                    return error_0304_type_mismatch(
                        lexer->source,
                        sema_node_span(lexer, &ast->nodes[pattern->c - 1]),
                        sema_type_name(lexer, sema, &temp_arena, value_type),
                        sema_type_name(
                            lexer, sema, &temp_arena, qualified_type));
                }
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
                    string field_name = lex_symbol(lexer, field->symbol_handle);
                    string suggested_field_name = {0};
                    if (field_name.count > 1 && field_name.data[0] == '_') {
                        string unprefixed_field_name = string_from(
                            field_name.data + 1, field_name.count - 1);
                        for (u32 j = 0; j < plex->param_count; ++j) {
                            string plex_field_name =
                                lex_symbol(lexer,
                                           sema->type_param_symbols
                                               [plex->first_param_type + j]);
                            if (string_eq(plex_field_name,
                                          unprefixed_field_name)) {
                                suggested_field_name = plex_field_name;
                                break;
                            }
                        }
                    }
                    return error_0304_unknown_plex_pattern_field(
                        lexer->source,
                        sema_token_span(lexer, field->token_index),
                        field_name,
                        suggested_field_name);
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
            const AstEnumPattern* enum_pattern =
                &ast->enum_patterns[pattern->a];
            if (value_type == sema_no_type() ||
                sema->types[value_type].kind != STK_Enum) {
                if (enum_pattern->braced_payload) {
                    return error_0304_type_mismatch(
                        lexer->source,
                        sema_pattern_span(lexer, pattern),
                        sema_type_name(lexer, sema, &temp_arena, value_type),
                        lex_symbol(lexer, enum_pattern->symbol_handle));
                }
                return error_0304_type_mismatch(
                    lexer->source,
                    sema_pattern_span(lexer, pattern),
                    s("enum"),
                    sema_type_name(lexer, sema, &temp_arena, value_type));
            }
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
            if (payload_type != sema_no_type() &&
                sema->types[payload_type].kind == STK_Plex &&
                !enum_pattern->braced_payload &&
                enum_pattern->pattern_count > 1) {
                return error_0304_enum_payload_pattern_field_names(
                    lexer->source,
                    sema_pattern_span(lexer, pattern),
                    lex_symbol(lexer, enum_pattern->symbol_handle));
            }
            if (payload_type != sema_no_type() &&
                sema->types[payload_type].kind == STK_Tuple &&
                enum_pattern->braced_payload) {
                return error_0304_type_mismatch(
                    lexer->source,
                    sema_pattern_span(lexer, pattern),
                    s("tuple enum payload pattern"),
                    s("braced enum payload pattern"));
            }
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
            return error_0322_invalid_on_wildcard_pattern(
                lexer->source, sema_pattern_span(lexer, pattern));
        }
        return true;
    }
    return true;
}

internal bool sema_check_multi_on_pattern_payload_variant(const Lexer* lexer,
                                                          const Ast*   ast,
                                                          Sema*        sema,
                                                          u32 pattern_index,
                                                          u32 value_type)
{
    if (value_type == sema_no_type() ||
        sema->types[value_type].kind != STK_Enum ||
        pattern_index >= array_count(ast->patterns)) {
        return true;
    }

    const AstPattern* pattern        = &ast->patterns[pattern_index];
    u32               variant_symbol = U32_MAX;
    if (pattern->kind == APK_EnumVariant) {
        const AstEnumPattern* enum_pattern = &ast->enum_patterns[pattern->a];
        variant_symbol                     = enum_pattern->symbol_handle;
    } else if (pattern->kind == APK_Value &&
               pattern->a < array_count(ast->nodes)) {
        const AstNode* node = &ast->nodes[pattern->a];
        if (node->kind == AK_EnumVariant || node->kind == AK_SymbolRef) {
            variant_symbol = node->a;
        } else if (node->kind == AK_Field &&
                   sema->node_type_indices[pattern->a] == value_type) {
            variant_symbol = node->b;
        }
    }
    if (variant_symbol == U32_MAX) {
        return true;
    }

    u32 variant = sema_enum_variant_index(sema, value_type, variant_symbol);
    if (variant == U32_MAX) {
        return true;
    }
    u32 payload_type =
        sema_enum_variant_payload_type(sema, value_type, variant);
    if (payload_type == sema_no_type()) {
        return true;
    }

    return error_0304_type_mismatch(lexer->source,
                                    sema_pattern_span(lexer, pattern),
                                    s("non-payload enum variant"),
                                    lex_symbol(lexer, variant_symbol));
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
                   sema->types[target_type].kind == STK_Pointer ||
                   sema->types[target_type].kind == STK_Slice ||
                   sema->types[target_type].kind == STK_DynamicArray ||
                   sema->types[target_type].kind == STK_String;
        }
    case AK_Field:
    case AK_TupleField:
        {
            u32 target_type = node->a < array_count(sema->node_type_indices)
                                  ? sema->node_type_indices[node->a]
                                  : sema_no_type();
            if (target_type != sema_no_type() &&
                target_type < array_count(sema->types) &&
                (sema->types[target_type].kind == STK_Pointer ||
                 sema->types[target_type].kind == STK_Box)) {
                return true;
            }
            return sema_node_is_addressable(ast, sema, node->a);
        }
    case AK_Deref:
        return true;
    default:
        return false;
    }
}

internal const SemaLocal*
sema_address_of_constant_local(const Ast* ast, Sema* sema, u32 node_index)
{
    const AstNode* node = &ast->nodes[node_index];
    if (node->kind != AK_SymbolRef) {
        return NULL;
    }

    u32 local_index = sema->node_local_indices[node_index];
    if (local_index == sema_no_local()) {
        return NULL;
    }

    const SemaLocal* local = &sema->locals[local_index];
    return local->kind == SLK_Constant ? local : NULL;
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
    ASSERT(break_node->kind == AK_Break || break_node->kind == AK_BreakExpr,
           "Expected break node");
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
    if (node->kind == AK_Break || node->kind == AK_BreakExpr) {
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
    if (node->kind == AK_Break || node->kind == AK_BreakExpr) {
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

internal void sema_select_default_method_for_local(const Lexer* lexer,
                                                   const Ast*   ast,
                                                   Sema*        sema,
                                                   SemaLocal*   local)
{
    if (local->kind != SLK_Variable ||
        local->value_node_index != sema_no_decl() ||
        local->type_index == sema_no_type() ||
        local->decl_node_index >= array_count(ast->nodes)) {
        return;
    }

    const AstNode* payload = &ast->nodes[local->decl_node_index];
    if (payload->kind != AK_Variable || payload->b >= array_count(ast->nodes)) {
        return;
    }

    u32 payload_index = payload->b;
    payload           = &ast->nodes[payload_index];
    if (payload->kind != AK_ZeroInit) {
        return;
    }

    u32 default_method_decl =
        sema_find_core_default_method_decl(lexer, ast, sema, local->type_index);
    if (default_method_decl != sema_no_decl()) {
        sema->node_method_call_decl_indices[local->decl_node_index] =
            default_method_decl;
        sema->node_method_call_decl_indices[payload_index] =
            default_method_decl;
    }
}

internal bool sema_infer_local_binding_type(const Lexer* lexer,
                                            const Ast*   ast,
                                            Sema*        sema,
                                            u32          local_index,
                                            u32*         out_type_index)
{
    SemaLocal* local = &sema->locals[local_index];
    if (local->type_index != sema_no_type()) {
        sema_select_default_method_for_local(lexer, ast, sema, local);
        *out_type_index = local->type_index;
        return true;
    }

    AstNode* bind_node = &ast->nodes[local->decl_node_index];
    if (ast_has_flag(bind_node, ANF_ConstBusy)) {
        if (local->type_index != sema_no_type() &&
            sema_node_is_type_syntax(ast, local->value_node_index)) {
            *out_type_index = local->type_index;
            return true;
        }
        return error_0302_dependency_cycle(
            lexer->source,
            sema_local_span(lexer, ast, local),
            lex_symbol(lexer, local->symbol_handle),
            sema_local_span(lexer, ast, local),
            lex_symbol(lexer, local->symbol_handle));
    }

    ast_set_flag(bind_node, ANF_ConstBusy);

    u32  annotated      = sema_no_type();
    u32  inferred       = sema_no_type();
    bool ok             = true;
    u32  reserved_type  = sema_no_type();

    u32 value_candidate = local->value_node_index;
    if (value_candidate != sema_no_decl()) {
        value_candidate = sema_unwrap_type_candidate_node(ast, value_candidate);
    }
    if (local->type_node_index == sema_no_type() &&
        value_candidate < array_count(ast->nodes) &&
        ast->nodes[value_candidate].kind == AK_TypePlex) {
        const AstPlexTypeInfo* plex =
            &ast->plex_types[ast->nodes[value_candidate].a];
        reserved_type = sema_reserve_type(
            sema,
            (SemaType){
                .kind = (plex->flags & APTF_Union) ? STK_Union : STK_Plex,
                .param_count      = 0,
                .flags            = plex->flags,
                .first_param_type = 0,
                .return_type      = sema_no_type(),
            });
        local->type_index = reserved_type;
    }

    if (local->type_node_index != sema_no_type() &&
        !sema_resolve_type_node(
            lexer, ast, sema, local->type_node_index, &annotated)) {
        ok = false;
    }

    if (ok && local->type_node_index == sema_no_type() &&
        sema_node_is_type_syntax(ast, local->value_node_index) &&
        !sema_resolve_type_node(
            lexer, ast, sema, local->value_node_index, &inferred)) {
        ok = false;
    }

    if (ok && local->value_node_index != sema_no_decl() &&
        inferred == sema_no_type() &&
        !sema_infer_node_type(
            lexer, ast, sema, local->value_node_index, annotated, &inferred)) {
        ok = false;
    }

    if (ok) {
        if (reserved_type != sema_no_type()) {
            sema->types[reserved_type] = sema->types[inferred];
            inferred                   = reserved_type;
            const SemaType* type       = &sema->types[inferred];
            for (u32 i = 0; i < type->param_count; ++i) {
                if (sema_type_contains_unboxed_type(
                        sema,
                        sema->type_param_types[type->first_param_type + i],
                        inferred)) {
                    ok = error_0309_type_alias_cycle(
                        lexer->source,
                        sema_local_span(lexer, ast, local),
                        lex_symbol(lexer, local->symbol_handle),
                        sema_local_span(lexer, ast, local),
                        lex_symbol(lexer, local->symbol_handle));
                    break;
                }
            }
        }
    }

    if (ok) {
        bool local_is_type_alias =
            local->type_node_index == sema_no_type() &&
            sema_node_is_type_syntax(ast, local->value_node_index);
        if (local_is_type_alias) {
            local->kind       = SLK_TypeAlias;
            local->type_index = inferred;
        } else {
            local->type_index =
                annotated != sema_no_type()
                    ? annotated
                    : (local->kind == SLK_Function ||
                               local->kind == SLK_Constant
                           ? inferred
                           : sema_materialise_type(sema, inferred));
        }
        sema->node_type_indices[local->decl_node_index] = local->type_index;
        sema_select_default_method_for_local(lexer, ast, sema, local);
        *out_type_index = local->type_index;
    }

    ast_clear_flag(bind_node, ANF_ConstBusy);
    return ok;
}

internal bool sema_type_can_adopt_expected_return_context(const Sema* sema,
                                                          u32 natural_type,
                                                          u32 expected_type)
{
    if (natural_type == sema_no_type() || expected_type == sema_no_type()) {
        return false;
    }

    SemaTypeKind natural_kind = sema->types[natural_type].kind;
    return natural_kind == STK_UntypedInteger ||
           natural_kind == STK_UntypedFloat || natural_kind == STK_Nil;
}

internal bool sema_local_value_is_contextual_plex_literal(const Ast* ast,
                                                          Sema*      sema,
                                                          u32 value_node_index,
                                                          u32 expected_type)
{
    if (value_node_index >= array_count(ast->nodes) ||
        expected_type == sema_no_type()) {
        return false;
    }

    u32 node_index = sema_unwrap_expr_node(ast, value_node_index);
    if (node_index >= array_count(ast->nodes) ||
        ast->nodes[node_index].kind != AK_Plex) {
        return false;
    }

    const AstPlexLiteralInfo* literal =
        &ast->plex_literals[ast->nodes[node_index].a];
    if (literal->target_node_index != U32_MAX) {
        return false;
    }

    SemaTypeKind expected_kind = sema->types[expected_type].kind;
    return expected_kind == STK_Plex || expected_kind == STK_Union;
}

internal bool sema_seed_local_type_from_expected(const Lexer* lexer,
                                                 const Ast*   ast,
                                                 Sema*        sema,
                                                 u32          local_index,
                                                 u32          expected_type)
{
    if (local_index == sema_no_local() || expected_type == sema_no_type()) {
        return true;
    }

    SemaLocal* local = &sema->locals[local_index];
    if ((local->kind != SLK_Variable && local->kind != SLK_Constant) ||
        local->type_index != sema_no_type() ||
        local->type_node_index != sema_no_type() ||
        local->value_node_index == sema_no_decl()) {
        return true;
    }

    if (!sema_local_value_is_contextual_plex_literal(
            ast, sema, local->value_node_index, expected_type)) {
        u32 natural_type = sema_no_type();
        if (!sema_infer_node_type(lexer,
                                  ast,
                                  sema,
                                  local->value_node_index,
                                  sema_no_type(),
                                  &natural_type)) {
            return false;
        }

        if (!sema_type_can_adopt_expected_return_context(
                sema, natural_type, expected_type)) {
            return true;
        }
        if (sema_materialise_type(sema, natural_type) == expected_type) {
            return true;
        }
    }

    u32 contextual_type = sema_no_type();
    if (!sema_infer_node_type(lexer,
                              ast,
                              sema,
                              local->value_node_index,
                              expected_type,
                              &contextual_type)) {
        return false;
    }
    if (!sema_type_matches(sema, expected_type, contextual_type)) {
        return true;
    }
    if (!sema_type_is_variable_storage(sema, expected_type)) {
        return error_0306_invalid_variable_type(
            lexer->source,
            sema_local_span(lexer, ast, local),
            sema_type_name(lexer, sema, &temp_arena, expected_type));
    }

    local->type_index                               = expected_type;
    sema->node_type_indices[local->decl_node_index] = expected_type;
    return true;
}

internal bool sema_find_destructure_local_tuple_path(const Ast*  ast,
                                                     const Sema* sema,
                                                     u32         pattern_index,
                                                     u32         local_index,
                                                     Array(u32) * out_path)
{
    const AstPattern* pattern = &ast->patterns[pattern_index];
    u32 symbol = sema_destructure_binder_symbol(ast, pattern_index);
    if (symbol != U32_MAX) {
        return sema->pattern_local_indices[pattern_index] == local_index;
    }

    switch (pattern->kind) {
    case APK_Tuple:
        for (u32 i = 0; i < pattern->b; ++i) {
            array_push(*out_path, i);
            if (sema_find_destructure_local_tuple_path(
                    ast,
                    sema,
                    ast->pattern_items[pattern->a + i],
                    local_index,
                    out_path)) {
                return true;
            }
            (void)array_pop(*out_path);
        }
        return false;
    case APK_Bind:
        if (pattern->b != U32_MAX) {
            return sema_find_destructure_local_tuple_path(
                ast, sema, pattern->b, local_index, out_path);
        }
        return false;
    default:
        return false;
    }
}

internal bool sema_tuple_type_with_expected_at_path(Sema*      sema,
                                                    u32        natural_type,
                                                    const u32* path,
                                                    u32        path_count,
                                                    u32        depth,
                                                    u32        expected_type,
                                                    u32*       out_type)
{
    if (depth == path_count) {
        *out_type = expected_type;
        return true;
    }
    if (natural_type == sema_no_type() ||
        sema->types[natural_type].kind != STK_Tuple) {
        return false;
    }

    const SemaType* tuple = &sema->types[natural_type];
    u32             item  = path[depth];
    if (item >= tuple->param_count) {
        return false;
    }

    Array(u32) item_types = NULL;
    for (u32 i = 0; i < tuple->param_count; ++i) {
        u32 item_type = sema->type_param_types[tuple->first_param_type + i];
        if (i == item && !sema_tuple_type_with_expected_at_path(sema,
                                                                item_type,
                                                                path,
                                                                path_count,
                                                                depth + 1,
                                                                expected_type,
                                                                &item_type)) {
            array_free(item_types);
            return false;
        }
        array_push(item_types, item_type);
    }

    *out_type = sema_add_tuple_type(sema, item_types);
    array_free(item_types);
    return true;
}

internal bool
sema_seed_destructured_local_type_from_expected(const Lexer* lexer,
                                                const Ast*   ast,
                                                Sema*        sema,
                                                u32          local_index,
                                                u32          expected_type)
{
    if (local_index == sema_no_local() || expected_type == sema_no_type()) {
        return true;
    }
    if (!sema_type_is_concrete_integer(sema, expected_type) &&
        !sema_type_is_concrete_float(sema, expected_type)) {
        return true;
    }

    SemaLocal* local = &sema->locals[local_index];
    if (local->decl_node_index >= array_count(ast->nodes) ||
        local->value_node_index == sema_no_decl()) {
        return true;
    }

    const AstNode* decl = &ast->nodes[local->decl_node_index];
    if (decl->kind != AK_DestructureBind &&
        decl->kind != AK_DestructureVariable) {
        return true;
    }
    if (ast->nodes[decl->b].kind == AK_AnnotatedValue) {
        return true;
    }

    Array(u32) path = NULL;
    bool found      = sema_find_destructure_local_tuple_path(
        ast, sema, decl->a, local_index, &path);
    if (!found || array_count(path) == 0) {
        array_free(path);
        return true;
    }

    u32 natural_type = sema_no_type();
    if (!sema_infer_node_type(lexer,
                              ast,
                              sema,
                              local->value_node_index,
                              sema_no_type(),
                              &natural_type)) {
        array_free(path);
        return false;
    }

    u32 contextual_expected = sema_no_type();
    if (!sema_tuple_type_with_expected_at_path(sema,
                                               natural_type,
                                               path,
                                               (u32)array_count(path),
                                               0,
                                               expected_type,
                                               &contextual_expected)) {
        array_free(path);
        return true;
    }
    array_free(path);

    u32 contextual_type = sema_no_type();
    if (!sema_infer_node_type(lexer,
                              ast,
                              sema,
                              local->value_node_index,
                              contextual_expected,
                              &contextual_type)) {
        return false;
    }
    if (!sema_type_matches(sema, contextual_expected, contextual_type)) {
        return true;
    }
    if (!sema_type_is_variable_storage(sema, expected_type)) {
        return error_0306_invalid_variable_type(
            lexer->source,
            sema_local_span(lexer, ast, local),
            sema_type_name(lexer, sema, &temp_arena, expected_type));
    }

    if (!sema_assign_destructure_pattern_type(
            lexer, ast, sema, decl->a, contextual_expected)) {
        return false;
    }
    sema->locals[local_index].type_index = expected_type;
    return true;
}

internal bool sema_seed_local_type_from_return(const Lexer* lexer,
                                               const Ast*   ast,
                                               Sema*        sema,
                                               u32          local_index,
                                               u32          expected_type)
{
    return sema_seed_local_type_from_expected(
        lexer, ast, sema, local_index, expected_type);
}

internal bool sema_node_is_order_comparison(const AstNode* node)
{
    return node->kind == AK_Less || node->kind == AK_LessEqual ||
           node->kind == AK_Greater || node->kind == AK_GreaterEqual;
}

internal bool sema_seed_for_condition_side(const Lexer* lexer,
                                           const Ast*   ast,
                                           Sema*        sema,
                                           u32          local_node_index,
                                           u32          expected_node_index)
{
    if (local_node_index >= array_count(ast->nodes) ||
        expected_node_index >= array_count(ast->nodes)) {
        return true;
    }

    const AstNode* local_node = &ast->nodes[local_node_index];
    while (local_node->kind == AK_Expression &&
           local_node->a < array_count(ast->nodes)) {
        local_node_index = local_node->a;
        local_node       = &ast->nodes[local_node_index];
    }
    if (local_node->kind != AK_SymbolRef) {
        return true;
    }

    u32 local_index = sema->node_local_indices[local_node_index];
    if (local_index == sema_no_local()) {
        return true;
    }

    const SemaLocal* local = &sema->locals[local_index];
    if (local->type_index != sema_no_type() ||
        local->type_node_index != sema_no_type()) {
        return true;
    }

    u32 expected_type = sema_no_type();
    if (!sema_infer_node_type(lexer,
                              ast,
                              sema,
                              expected_node_index,
                              sema_no_type(),
                              &expected_type)) {
        return false;
    }
    if (!sema_type_is_concrete_integer(sema, expected_type) &&
        !sema_type_is_concrete_float(sema, expected_type)) {
        return true;
    }

    return sema_seed_local_type_from_expected(
        lexer, ast, sema, local_index, expected_type);
}

internal bool sema_seed_for_condition_local_types(const Lexer* lexer,
                                                  const Ast*   ast,
                                                  Sema*        sema,
                                                  u32 condition_node_index)
{
    if (condition_node_index >= array_count(ast->nodes)) {
        return true;
    }

    const AstNode* condition = &ast->nodes[condition_node_index];
    while (condition->kind == AK_Expression &&
           condition->a < array_count(ast->nodes)) {
        condition_node_index = condition->a;
        condition            = &ast->nodes[condition_node_index];
    }

    if (condition->kind == AK_LogicalAnd || condition->kind == AK_LogicalOr) {
        return sema_seed_for_condition_local_types(
                   lexer, ast, sema, condition->a) &&
               sema_seed_for_condition_local_types(
                   lexer, ast, sema, condition->b);
    }

    if (!sema_node_is_order_comparison(condition)) {
        return true;
    }

    return sema_seed_for_condition_side(
               lexer, ast, sema, condition->a, condition->b) &&
           sema_seed_for_condition_side(
               lexer, ast, sema, condition->b, condition->a);
}

internal bool sema_seed_return_expr_context(const Lexer* lexer,
                                            const Ast*   ast,
                                            Sema*        sema,
                                            u32          expr_node_index,
                                            u32          expected_type)
{
    if (expr_node_index == U32_MAX || expected_type == sema_no_type()) {
        return true;
    }

    const AstNode* expr = &ast->nodes[expr_node_index];
    while (expr->kind == AK_Expression && expr->a < array_count(ast->nodes)) {
        expr_node_index = expr->a;
        expr            = &ast->nodes[expr_node_index];
    }

    if (expr->kind != AK_SymbolRef) {
        return true;
    }

    return sema_seed_local_type_from_return(
        lexer,
        ast,
        sema,
        sema->node_local_indices[expr_node_index],
        expected_type);
}

internal u32 sema_ast_enclosing_function_start_node(const Ast* ast,
                                                    u32        node_index)
{
    for (u32 i = node_index + 1; i-- > 0;) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind == AK_FnStart && node->b > node_index) {
            return i;
        }
    }
    return U32_MAX;
}

internal bool sema_node_is_inside_generic_impl(const Ast* ast, u32 node_index)
{
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind != AK_Impl) {
            continue;
        }
        const AstImplInfo* impl = &ast->impls[node->a];
        if (impl->generic_params_index == U32_MAX) {
            continue;
        }
        const AstNode* body = &ast->nodes[impl->body_node_index];
        if (body->a <= node_index && node_index < body->b) {
            return true;
        }
    }
    return false;
}

internal bool sema_node_is_inside_generic_function(const Ast* ast,
                                                   u32        node_index)
{
    u32 fn_start_index =
        sema_ast_enclosing_function_start_node(ast, node_index);
    if (fn_start_index == U32_MAX) {
        return false;
    }

    const AstNode* fn_start = &ast->nodes[fn_start_index];
    if (fn_start->kind != AK_FnStart ||
        fn_start->a >= array_count(ast->fn_signatures)) {
        return false;
    }

    const AstFnSignature* signature = &ast->fn_signatures[fn_start->a];
    return signature->generic_params_index != U32_MAX;
}

internal bool sema_node_is_inside_function_with_params(const Ast* ast,
                                                       u32        node_index)
{
    u32 fn_start_index =
        sema_ast_enclosing_function_start_node(ast, node_index);
    if (fn_start_index == U32_MAX) {
        return false;
    }

    const AstNode* fn_start = &ast->nodes[fn_start_index];
    if (fn_start->kind != AK_FnStart ||
        fn_start->a >= array_count(ast->fn_signatures)) {
        return false;
    }

    const AstFnSignature* signature = &ast->fn_signatures[fn_start->a];
    return signature->param_count != 0;
}

internal bool sema_seed_function_return_contexts(const Lexer* lexer,
                                                 const Ast*   ast,
                                                 Sema*        sema,
                                                 u32          fn_node_index)
{
    const AstNode* fn_node = &ast->nodes[fn_node_index];
    if (fn_node->kind != AK_FnDef || fn_node->b != AFK_Block) {
        return true;
    }

    const AstNode*        fn_start  = &ast->nodes[fn_node->a];
    const AstFnSignature* signature = &ast->fn_signatures[fn_start->a];
    if (signature->return_type_node_index == U32_MAX ||
        signature->generic_params_index != U32_MAX ||
        sema_node_is_inside_generic_impl(ast, fn_node_index)) {
        return true;
    }

    u32 return_type = sema_no_type();
    if (!sema_resolve_type_node(lexer,
                                ast,
                                sema,
                                signature->return_type_node_index,
                                &return_type)) {
        return false;
    }

    for (u32 i = fn_node->a + 1; i < fn_start->b; ++i) {
        const AstNode* node = &ast->nodes[i];
        if (sema_ast_enclosing_function_start_node(ast, i) != fn_node->a) {
            continue;
        }
        if ((node->kind == AK_Return || node->kind == AK_ReturnExpr) &&
            !sema_seed_return_expr_context(
                lexer, ast, sema, node->a, return_type)) {
            return false;
        }
    }

    return true;
}

internal bool
sema_seed_return_context_local_types(const Lexer*           lexer,
                                     const Ast*             ast,
                                     const FrontEndOptions* options,
                                     Sema*                  sema)
{
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        if (ast->nodes[i].kind != AK_FnDef) {
            continue;
        }
        if (sema_node_is_inside_disabled_top_on_body(options, lexer, ast, i)) {
            continue;
        }
        if (!sema_seed_function_return_contexts(lexer, ast, sema, i)) {
            return false;
        }
    }
    return true;
}

internal bool sema_seed_local_from_call_arg_expected(const Lexer* lexer,
                                                     const Ast*   ast,
                                                     Sema*        sema,
                                                     u32 arg_node_index,
                                                     u32 expected_type)
{
    if (arg_node_index >= array_count(ast->nodes) ||
        expected_type == sema_no_type()) {
        return true;
    }

    arg_node_index                = sema_unwrap_expr_node(ast, arg_node_index);
    const AstNode* node           = &ast->nodes[arg_node_index];
    u32            local_expected = expected_type;

    if (node->kind == AK_AddressOf &&
        sema->types[expected_type].kind == STK_Pointer) {
        arg_node_index = sema_unwrap_expr_node(ast, node->a);
        node           = &ast->nodes[arg_node_index];
        local_expected = sema->types[expected_type].first_param_type;
    }

    if (node->kind != AK_SymbolRef) {
        return true;
    }

    return sema_seed_local_type_from_expected(
        lexer,
        ast,
        sema,
        sema->node_local_indices[arg_node_index],
        local_expected);
}

internal bool sema_seed_method_call_arg_contexts(const Lexer* lexer,
                                                 const Ast*   ast,
                                                 Sema*        sema,
                                                 u32          call_node_index,
                                                 u32          field_node_index,
                                                 u32          call_arg_offset)
{
    const AstNode* field_node = &ast->nodes[field_node_index];
    if (field_node->kind != AK_Field) {
        return true;
    }

    u32 receiver_type = sema_no_type();
    if (!sema_infer_node_type(
            lexer, ast, sema, field_node->a, sema_no_type(), &receiver_type)) {
        return false;
    }
    if (receiver_type == sema_no_type()) {
        return true;
    }

    bool                   found_method = false;
    SemaResolvedMethodCall method_call  = {0};
    if (!sema_try_resolve_method_call(lexer,
                                      ast,
                                      sema,
                                      call_node_index,
                                      receiver_type,
                                      field_node->b,
                                      U32_MAX,
                                      U32_MAX,
                                      call_arg_offset,
                                      true,
                                      &found_method,
                                      &method_call)) {
        return false;
    }
    return true;
}

internal bool sema_call_has_seedable_local_arg(const Ast*         ast,
                                               const AstCallInfo* call,
                                               u32 call_arg_offset)
{
    if (call->arg_count <= call_arg_offset) {
        return false;
    }

    for (u32 i = call_arg_offset; i < call->arg_count; ++i) {
        u32 arg_node_index = ast->call_args[call->first_arg + i];
        if (arg_node_index >= array_count(ast->nodes)) {
            continue;
        }

        const AstNode* arg_node = &ast->nodes[arg_node_index];
        if (arg_node->kind == AK_Assign) {
            arg_node_index = arg_node->b;
        }

        arg_node_index = sema_unwrap_expr_node(ast, arg_node_index);
        if (arg_node_index >= array_count(ast->nodes)) {
            continue;
        }

        arg_node = &ast->nodes[arg_node_index];
        if (arg_node->kind == AK_AddressOf) {
            arg_node_index = sema_unwrap_expr_node(ast, arg_node->a);
            if (arg_node_index >= array_count(ast->nodes)) {
                continue;
            }
            arg_node = &ast->nodes[arg_node_index];
        }

        if (arg_node->kind == AK_SymbolRef) {
            return true;
        }
    }

    return false;
}

internal bool
sema_seed_call_arg_context_local_types(const Lexer*           lexer,
                                       const Ast*             ast,
                                       const FrontEndOptions* options,
                                       Sema*                  sema)
{
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind != AK_Call) {
            continue;
        }
        if (sema_node_is_inside_disabled_top_on_body(options, lexer, ast, i)) {
            continue;
        }
        if (sema_node_is_inside_generic_function(ast, i) ||
            sema_node_is_inside_generic_impl(ast, i) ||
            sema_node_is_inside_function_with_params(ast, i)) {
            continue;
        }

        const AstCallInfo* call = &ast->calls[node->b];
        if (!sema_call_has_seedable_local_arg(ast, call, 0)) {
            continue;
        }

        u32            callee_node_index = sema_unwrap_expr_node(ast, node->a);
        const AstNode* callee            = &ast->nodes[callee_node_index];
        u32            call_arg_offset   = 0;
        if (callee->kind == AK_Index) {
            const AstNode* generic_target = &ast->nodes[callee->a];
            if (generic_target->kind == AK_Field) {
                callee_node_index = callee->a;
                callee            = generic_target;
            }
        }
        if (callee->kind == AK_Field &&
            !sema_seed_method_call_arg_contexts(
                lexer, ast, sema, i, callee_node_index, call_arg_offset)) {
            return false;
        }
    }
    return true;
}

internal bool sema_infer_range_iterable_type(const Lexer* lexer,
                                             const Ast*   ast,
                                             Sema*        sema,
                                             u32          node_index,
                                             u32*         out_item_type)
{
    const AstNode* node = &ast->nodes[node_index];
    if (node->kind != AK_RangeExclusive && node->kind != AK_RangeInclusive) {
        return false;
    }

    u32 start_type = sema_no_type();
    if (!sema_infer_node_type(
            lexer, ast, sema, node->a, sema_no_type(), &start_type)) {
        return false;
    }

    u32 end_expected = sema_type_is_concrete_integer(sema, start_type)
                           ? start_type
                           : sema_no_type();
    u32 end_type     = sema_no_type();
    if (!sema_infer_node_type(
            lexer, ast, sema, node->b, end_expected, &end_type)) {
        return false;
    }

    if (!sema_type_is_integer(sema, start_type)) {
        Arena temp_arena = {0};
        arena_init(&temp_arena);
        bool ok = error_0304_type_mismatch(
            lexer->source,
            sema_node_span(lexer, &ast->nodes[node->a]),
            s("integer"),
            sema_type_name(lexer, sema, &temp_arena, start_type));
        arena_done(&temp_arena);
        return ok;
    }
    if (!sema_type_is_integer(sema, end_type)) {
        Arena temp_arena = {0};
        arena_init(&temp_arena);
        bool ok = error_0304_type_mismatch(
            lexer->source,
            sema_node_span(lexer, &ast->nodes[node->b]),
            s("integer"),
            sema_type_name(lexer, sema, &temp_arena, end_type));
        arena_done(&temp_arena);
        return ok;
    }

    u32 item_type = sema_no_type();
    if (sema_type_is_concrete_integer(sema, start_type)) {
        item_type = start_type;
    } else if (sema_type_is_concrete_integer(sema, end_type)) {
        item_type = end_type;
    } else {
        item_type = sema_materialise_type(sema, start_type);
    }

    if (!sema_type_matches(sema, item_type, start_type) ||
        !sema_type_matches(sema, item_type, end_type)) {
        Arena temp_arena = {0};
        arena_init(&temp_arena);
        bool ok = error_0304_type_mismatch(
            lexer->source,
            sema_node_span(lexer, node),
            sema_type_name(lexer, sema, &temp_arena, item_type),
            sema_type_name(lexer, sema, &temp_arena, end_type));
        arena_done(&temp_arena);
        return ok;
    }

    sema->node_type_indices[node_index] = item_type;
    *out_item_type                      = item_type;
    return true;
}

internal bool sema_infer_for_iterable_type(const Lexer*      lexer,
                                           const Ast*        ast,
                                           Sema*             sema,
                                           u32               for_node_index,
                                           const AstForInfo* for_info,
                                           u32               for_scope,
                                           u32*              out_iterable_type)
{
    u32 iterable_type = sema_no_type();
    if (for_info->iterable_node_index == U32_MAX) {
        *out_iterable_type = iterable_type;
        return true;
    }

    u32 iterable_node_index =
        sema_unwrap_expr_node(ast, for_info->iterable_node_index);
    const AstNode* iterable  = &ast->nodes[iterable_node_index];
    u32            item_type = sema_no_type();
    if (iterable->kind == AK_RangeExclusive ||
        iterable->kind == AK_RangeInclusive) {
        if (!sema_infer_range_iterable_type(
                lexer, ast, sema, iterable_node_index, &item_type)) {
            return false;
        }
        iterable_type = item_type;
    } else {
        if (!sema_infer_node_type(lexer,
                                  ast,
                                  sema,
                                  for_info->iterable_node_index,
                                  sema_no_type(),
                                  &iterable_type)) {
            return false;
        }
        iterable_type = sema_materialise_type(sema, iterable_type);
        switch (sema->types[iterable_type].kind) {
        case STK_Array:
        case STK_Slice:
        case STK_DynamicArray:
            item_type = sema->types[iterable_type].first_param_type;
            break;
        case STK_String:
            item_type = sema_builtin_type(sema, STK_U8);
            break;
        default:
            item_type              = sema_no_type();
            u32 iterator_next_decl = sema_find_core_iterator_method_decl(
                lexer, ast, sema, iterable_type, &item_type);
            if (iterator_next_decl != sema_no_decl()) {
                if (for_node_index <
                    array_count(sema->node_method_call_decl_indices)) {
                    sema->node_method_call_decl_indices[for_node_index] =
                        iterator_next_decl;
                }
                break;
            } else {
                Arena temp_arena = {0};
                arena_init(&temp_arena);
                bool ok = error_0304_type_mismatch(
                    lexer->source,
                    sema_node_span(lexer, iterable),
                    s("array, slice, dynamic array, string, range, or "
                      "Iterator"),
                    sema_type_name(lexer, sema, &temp_arena, iterable_type));
                arena_done(&temp_arena);
                return ok;
            }
        }
        if (for_node_index >=
                array_count(sema->node_method_call_decl_indices) ||
            sema->node_method_call_decl_indices[for_node_index] ==
                sema_no_decl()) {
            item_type = sema_add_pointer_type(sema, item_type);
        }
    }

    if (for_scope != sema_no_scope()) {
        u32 local_index =
            sema_lookup_local(sema, for_scope, for_info->item_symbol);
        if (local_index != sema_no_local()) {
            sema->locals[local_index].type_index = item_type;
        }
    }
    *out_iterable_type = iterable_type;
    return true;
}

internal bool
sema_node_definitely_returns(const Ast* ast, Sema* sema, u32 node_index);

internal bool sema_block_definitely_returns(const Ast* ast,
                                            Sema*      sema,
                                            u32        first_node,
                                            u32        end_node)
{
    for (u32 i = first_node; i < end_node; ++i) {
        const AstNode* stmt = &ast->nodes[i];
        if (stmt->kind == AK_Block && sema_block_is_expr_block_body(ast, i)) {
            i = stmt->b - 1;
            continue;
        }
        if (!ast_node_is_block_statement(stmt)) {
            continue;
        }
        if (sema_node_definitely_returns(ast, sema, i)) {
            return true;
        }
        i = ast_block_statement_end_exclusive(ast, i) - 1;
    }
    return false;
}

internal bool
sema_node_definitely_returns(const Ast* ast, Sema* sema, u32 node_index)
{
    const AstNode* node = &ast->nodes[node_index];
    switch (node->kind) {
    case AK_Return:
    case AK_ReturnExpr:
        return true;
    case AK_Expression:
    case AK_Statement:
        return sema_node_definitely_returns(ast, sema, node->a);
    case AK_ExprBlock:
        {
            const AstNode* block = &ast->nodes[node->a];
            return block->kind == AK_Block &&
                   sema_block_definitely_returns(ast, sema, block->a, block->b);
        }
    case AK_Block:
        return sema_block_definitely_returns(ast, sema, node->a, node->b);
    case AK_On:
        {
            const AstOnInfo* on         = &ast->ons[node->b];
            bool             has_else   = false;
            bool             exhaustive = false;
            for (u32 i = 0; i < on->branch_count; ++i) {
                const AstOnBranch* branch =
                    &ast->on_branches[on->first_branch + i];
                if (branch->flags & AOBF_Else) {
                    has_else = true;
                }
                if (branch->flags & AOBF_Error) {
                    has_else = true;
                }
                if (!sema_node_definitely_returns(
                        ast, sema, branch->expr_node_index)) {
                    return false;
                }
            }
            if (has_else) {
                exhaustive = true;
            } else if (on->kind != AOK_Condition && node->a != U32_MAX) {
                u32 scrutinee_type = sema->node_type_indices[node->a];
                exhaustive         = sema_on_covers_all_enum_variants(
                    ast, sema, node->b, scrutinee_type);
            }
            return exhaustive;
        }
    default:
        return false;
    }
}

internal bool sema_infer_block_statements(const Lexer* lexer,
                                          const Ast*   ast,
                                          Sema*        sema,
                                          u32          first_node,
                                          u32          end_node,
                                          u32          expected_return_type,
                                          u32*         in_out_return_type,
                                          bool*        out_has_return)
{
    for (u32 i = first_node; i < end_node; ++i) {
        const AstNode* stmt = &ast->nodes[i];

        if (stmt->kind == AK_FnStart) {
            i = stmt->b - 1;
            continue;
        }

        if (!ast_node_is_block_statement(stmt)) {
            continue;
        }

        if (stmt->kind == AK_Block) {
            if (sema_block_is_expr_block_body(ast, i)) {
                i = stmt->b - 1;
                continue;
            }
            if (!sema_infer_block_statements(lexer,
                                             ast,
                                             sema,
                                             stmt->a,
                                             stmt->b,
                                             expected_return_type,
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
            if (for_info->condition_node_index != U32_MAX &&
                !sema_seed_for_condition_local_types(
                    lexer, ast, sema, for_info->condition_node_index)) {
                return false;
            }
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
            if (for_info->iterable_node_index != U32_MAX) {
                u32 iterable_type = sema_no_type();
                u32 for_scope     = sema->node_scope_indices[i];
                if (!sema_infer_for_iterable_type(lexer,
                                                  ast,
                                                  sema,
                                                  i,
                                                  for_info,
                                                  for_scope,
                                                  &iterable_type)) {
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
                                             expected_return_type,
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

        if (stmt->kind == AK_Bind || stmt->kind == AK_Variable ||
            stmt->kind == AK_Assign || stmt->kind == AK_DestructureBind ||
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

        if (stmt->kind == AK_Pragma) {
            sema->node_type_indices[i] = sema_builtin_type(sema, STK_Void);
            continue;
        }

        if (stmt->kind == AK_Defer) {
            u32 ignored = sema_no_type();
            if (!sema_infer_node_type(
                    lexer, ast, sema, stmt->a, sema_no_type(), &ignored)) {
                return false;
            }
            sema->node_type_indices[i] = sema_builtin_type(sema, STK_Void);
            i = ast_block_statement_end_exclusive(ast, i) - 1;
            continue;
        }

        if (stmt->kind == AK_Assert) {
            u32 ignored = sema_no_type();
            if (!sema_infer_node_type(
                    lexer, ast, sema, i, sema_no_type(), &ignored)) {
                return false;
            }
            continue;
        }

        if (stmt->kind == AK_Statement) {
            u32 ignored = sema_no_type();
            if (!sema_infer_node_type(
                    lexer, ast, sema, i, sema_no_type(), &ignored)) {
                return false;
            }
            if (ignored != sema_no_type()) {
                const AstNode* expr = &ast->nodes[stmt->a];
                if (expr->kind == AK_Expression) {
                    expr = &ast->nodes[expr->a];
                }
                u32 materialised = sema_materialise_type(sema, ignored);
                if (expr->kind != AK_Bind && expr->kind != AK_Assign &&
                    expr->kind != AK_DestructureAssign &&
                    sema->types[materialised].kind != STK_Void &&
                    sema->types[materialised].kind != STK_Never) {
                    return error_0345_discarded_value(
                        lexer->source,
                        sema_node_span(lexer, stmt),
                        sema_type_name(lexer, sema, &temp_arena, materialised));
                }
                if (sema->types[materialised].kind == STK_Never) {
                    sema->node_type_indices[i] = materialised;
                    *out_has_return            = true;
                    return true;
                }
            }
            if (sema_node_contains_interpolation(ast, i) &&
                !sema_validate_interpolated_strings(lexer, ast, sema, i)) {
                return false;
            }
            if (sema_node_definitely_returns(ast, sema, i)) {
                sema->node_type_indices[i] = *in_out_return_type;
                *out_has_return            = true;
                return true;
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

        if (stmt->a != U32_MAX && expected_return_type != sema_no_type() &&
            sema->types[expected_return_type].kind == STK_Void) {
            return error_0350_unexpected_return_value(
                lexer->source, sema_node_span(lexer, stmt));
        }

        if (stmt->a == U32_MAX && expected_return_type != sema_no_type() &&
            sema_type_is_never(sema, expected_return_type)) {
            return error_0304_type_mismatch(
                lexer->source,
                sema_node_span(lexer, stmt),
                sema_type_name(lexer, sema, &temp_arena, expected_return_type),
                sema_type_name(lexer,
                               sema,
                               &temp_arena,
                               sema_builtin_type(sema, STK_Void)));
        }

        if (stmt->a != U32_MAX) {
            u32 expected_return = expected_return_type;
            if (!sema_infer_node_type(lexer,
                                      ast,
                                      sema,
                                      stmt->a,
                                      expected_return,
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
        if (stmt->kind == AK_Block && sema_block_is_expr_block_body(ast, i)) {
            i = stmt->b - 1;
            continue;
        }
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
        if (stmt->kind == AK_Return || stmt->kind == AK_ReturnExpr) {
            definitely_exits = true;
            break;
        }
        if (sema_type_is_never(sema, ignored)) {
            result_type = result_type == sema_no_type() ? ignored : result_type;
            definitely_exits = true;
            break;
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
    case AK_ErrorInject:
    case AK_Propagate:
    case AK_BitwiseNot:
        return sema_node_contains_interpolation(ast, node->a);
    case AK_Cast:
        {
            const AstCastInfo* cast = sema_cast_info(ast, node);
            return sema_node_contains_interpolation(ast, node->a) ||
                   (cast->extra_node_index != U32_MAX &&
                    sema_node_contains_interpolation(ast,
                                                     cast->extra_node_index));
        }
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
    case AK_ShiftLeft:
    case AK_ShiftRight:
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

//------------------------------------------------------------------------------
// Return the first runtime interpolated string under one expression. Constant
// interpolations can be lowered to string literals and do not use the temporary
// runtime builder.

internal u32 sema_find_runtime_interpolated_string_node(const Ast*  ast,
                                                        const Sema* sema,
                                                        u32         node_index)
{
    u32 found = sema_find_interpolated_string_node(ast, node_index);
    if (found == sema_no_decl()) {
        return sema_no_decl();
    }

    return sema_expr_is_constantish(ast, sema, found) ? sema_no_decl() : found;
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
    case AK_ErrorInject:
    case AK_Propagate:
    case AK_BitwiseNot:
        return sema_find_interpolated_string_node(ast, node->a);
    case AK_Cast:
        {
            const AstCastInfo* cast = sema_cast_info(ast, node);
            u32 found = sema_find_interpolated_string_node(ast, node->a);
            if (found != sema_no_decl()) {
                return found;
            }
            return cast->extra_node_index == U32_MAX
                       ? sema_no_decl()
                       : sema_find_interpolated_string_node(
                             ast, cast->extra_node_index);
        }
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
    case AK_ShiftLeft:
    case AK_ShiftRight:
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
            const AstOnInfo* on    = &ast->ons[node->b];
            u32              found = sema_no_decl();
            if (node->a != U32_MAX) {
                found = sema_find_interpolated_string_node(ast, node->a);
                if (found != sema_no_decl()) {
                    return found;
                }
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
    case APK_ForValue:
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
    case APK_ForValue:
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
    case AK_ErrorInject:
    case AK_Propagate:
    case AK_BitwiseNot:
        return sema_validate_interpolated_strings(lexer, ast, sema, node->a);
    case AK_Cast:
        {
            const AstCastInfo* cast = sema_cast_info(ast, node);
            return sema_validate_interpolated_strings(
                       lexer, ast, sema, node->a) &&
                   (cast->extra_node_index == U32_MAX ||
                    sema_validate_interpolated_strings(
                        lexer, ast, sema, cast->extra_node_index));
        }
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
    case AK_ShiftLeft:
    case AK_ShiftRight:
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
            if (node->a != U32_MAX && !sema_validate_interpolated_strings(
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
    case APK_ForValue:
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

internal bool
sema_destructure_pattern_type_from_existing_locals(const Ast* ast,
                                                   Sema*      sema,
                                                   u32        pattern_index,
                                                   u32        natural_type,
                                                   u32*       out_type)
{
    const AstPattern* pattern = &ast->patterns[pattern_index];
    u32 symbol = sema_destructure_binder_symbol(ast, pattern_index);
    if (symbol != U32_MAX) {
        u32 local_index = sema->pattern_local_indices[pattern_index];
        if (local_index != sema_no_local() &&
            sema->locals[local_index].type_index != sema_no_type()) {
            *out_type = sema->locals[local_index].type_index;
        } else {
            *out_type = natural_type;
        }
        return true;
    }

    switch (pattern->kind) {
    case APK_Ignore:
        *out_type = natural_type;
        return true;
    case APK_Tuple:
        {
            if (natural_type == sema_no_type() ||
                sema->types[natural_type].kind != STK_Tuple ||
                sema->types[natural_type].param_count != pattern->b) {
                *out_type = natural_type;
                return true;
            }

            const SemaType* tuple = &sema->types[natural_type];
            Array(u32) item_types = NULL;
            bool changed          = false;
            for (u32 i = 0; i < pattern->b; ++i) {
                u32 natural_item =
                    sema->type_param_types[tuple->first_param_type + i];
                u32 item_type = natural_item;
                if (!sema_destructure_pattern_type_from_existing_locals(
                        ast,
                        sema,
                        ast->pattern_items[pattern->a + i],
                        natural_item,
                        &item_type)) {
                    array_free(item_types);
                    return false;
                }
                if (item_type != natural_item) {
                    changed = true;
                }
                array_push(item_types, item_type);
            }

            *out_type =
                changed ? sema_add_tuple_type(sema, item_types) : natural_type;
            array_free(item_types);
            return true;
        }
    default:
        *out_type = natural_type;
        return true;
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
    const AstNode* node           = &ast->nodes[node_index];
    u32            type_index     = sema_no_type();
    u32            value_expected = expected_type;
    if (expected_type != sema_no_type() &&
        sema->types[expected_type].kind == STK_Enum &&
        (sema->types[expected_type].flags & (STF_Optional | STF_Result))) {
        const SemaType* wrapper = &sema->types[expected_type];
        value_expected =
            sema->type_param_types[wrapper->first_param_type +
                                   ((wrapper->flags & STF_Optional) ? 1 : 0)];
    }

    switch (node->kind) {
    case AK_IntegerLiteral:
        if (sema_integer_literal_is_packed(lexer, node)) {
            if (sema_type_is_concrete_integer(sema, value_expected)) {
                type_index = value_expected;
            } else if (sema_packed_integer_literal_is_character(lexer, node)) {
                type_index = sema_builtin_type(sema, STK_UntypedInteger);
            } else {
                type_index =
                    sema_packed_integer_literal_type(lexer, node, sema);
            }
        } else {
            type_index = sema_type_is_concrete_integer(sema, value_expected)
                             ? value_expected
                             : sema_builtin_type(sema, STK_UntypedInteger);
        }
        break;

    case AK_FloatLiteral:
        type_index = sema_type_is_concrete_float(sema, value_expected)
                         ? value_expected
                         : sema_builtin_type(sema, STK_UntypedFloat);
        break;

    case AK_BoolLiteral:
        type_index = sema_builtin_type(sema, STK_Bool);
        break;

    case AK_NilLiteral:
        if (expected_type != sema_no_type() &&
            (sema->types[expected_type].kind == STK_Pointer ||
             sema->types[expected_type].kind == STK_Function ||
             sema->types[expected_type].kind == STK_Slice ||
             sema->types[expected_type].kind == STK_DynamicArray ||
             sema->types[expected_type].kind == STK_Box)) {
            type_index = expected_type;
        } else {
            type_index = sema_builtin_type(sema, STK_Nil);
        }
        break;

    case AK_StringLiteral:
    case AK_StringConcat:
        if (node->kind == AK_StringLiteral &&
            lexer->tokens[node->token_index].kind == TK_CString) {
            type_index =
                sema_add_pointer_type(sema, sema_builtin_type(sema, STK_I8));
        } else {
            type_index = sema_builtin_type(sema, STK_String);
        }
        break;

    case AK_BuiltinMacro:
        {
            string name = lex_symbol(lexer, node->a);
            if (string_eq_cstr(name, "file")) {
                type_index = sema_builtin_type(sema, STK_String);
            } else if (string_eq_cstr(name, "line")) {
                type_index = sema_type_is_concrete_integer(sema, expected_type)
                                 ? expected_type
                                 : sema_builtin_type(sema, STK_UntypedInteger);
            } else if (string_eq_cstr(name, "embed")) {
                if (node->b == U32_MAX ||
                    ast->nodes[node->b].kind != AK_StringLiteral) {
                    return error_0304_type_mismatch(lexer->source,
                                                    sema_node_span(lexer, node),
                                                    s("string literal"),
                                                    s("@embed argument"));
                }

                string relative_path = lexer->strings[ast->nodes[node->b].a];
                cstr   resolved_path = sema_resolve_source_relative_path(
                    &temp_arena, lexer->source.source_path, relative_path);
                if (!path_exists(resolved_path) ||
                    path_is_directory(resolved_path)) {
                    return error_0304_type_mismatch(
                        lexer->source,
                        sema_node_span(lexer, node),
                        s("readable embedded file"),
                        string_format(&temp_arena, "`%s`", resolved_path));
                }

                type_index =
                    sema_add_slice_type(sema, sema_builtin_type(sema, STK_U8));
            } else {
                return error_0204_unexpected_token_here(
                    lexer->source,
                    sema_node_span(lexer, node),
                    lexer->tokens[node->token_index].kind,
                    "unknown built-in macro",
                    "Use `@file`, `@line`, or `@embed(\"path\")`");
            }
        }
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
                const SemaLocal* constant =
                    sema_address_of_constant_local(ast, sema, node->a);
                if (constant) {
                    return error_0304_address_of_constant_binding(
                        lexer->source,
                        sema_node_span(lexer, &ast->nodes[node->a]),
                        sema_type_name(lexer, sema, &temp_arena, pointee_type),
                        lex_symbol(lexer, constant->symbol_handle));
                }
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
            if (sema_type_is_builtin_interpolatable(sema, part_type)) {
                continue;
            }
            u32 display_symbol =
                sema_find_core_trait_symbol(lexer, sema, s("Display"));
            if (display_symbol != sema_no_decl()) {
                if (!sema_type_satisfies_trait_constraint(
                        lexer,
                        ast,
                        sema,
                        part_type,
                        display_symbol,
                        sema_node_span(lexer, &ast->nodes[part->a]))) {
                    return false;
                }
                continue;
            }
            {
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
            tuple_type = sema_member_target_type(sema, tuple_type);
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
            u32                       target_type              = sema_no_type();
            bool                      enum_variant_constructor = false;
            u32                       enum_constructor_type    = sema_no_type();
            if (node->kind == AK_Plex) {
                if (literal->target_node_index == U32_MAX) {
                    target_type = expected_type;
                    if (target_type != sema_no_type() &&
                        sema->types[target_type].kind == STK_Enum &&
                        (sema->types[target_type].flags &
                         (STF_Optional | STF_Result))) {
                        const SemaType* wrapper = &sema->types[target_type];
                        target_type =
                            sema->type_param_types
                                [wrapper->first_param_type +
                                 ((wrapper->flags & STF_Optional) ? 1 : 0)];
                    }
                } else {
                    const AstNode* target_node =
                        &ast->nodes[literal->target_node_index];
                    if (target_node->kind == AK_Field) {
                        u32 qualified_type = sema_no_type();
                        if (sema_try_resolve_type_symbol(lexer,
                                                         ast,
                                                         sema,
                                                         target_node->a,
                                                         &qualified_type) &&
                            sema->types[qualified_type].kind == STK_Enum) {
                            u32 variant = sema_enum_variant_index(
                                sema, qualified_type, target_node->b);
                            if (variant == U32_MAX) {
                                return error_0304_type_mismatch(
                                    lexer->source,
                                    sema_node_span(lexer, target_node),
                                    s("known enum variant"),
                                    lex_symbol(lexer, target_node->b));
                            }
                            u32 payload_type = sema_enum_variant_payload_type(
                                sema, qualified_type, variant);
                            if (payload_type == sema_no_type() ||
                                sema->types[payload_type].kind != STK_Plex) {
                                return error_0304_type_mismatch(
                                    lexer->source,
                                    sema_node_span(lexer, node),
                                    s("enum variant with named-field payload"),
                                    sema_type_name(lexer,
                                                   sema,
                                                   &temp_arena,
                                                   payload_type));
                            }
                            target_type              = payload_type;
                            enum_variant_constructor = true;
                            enum_constructor_type    = qualified_type;
                        }
                    }
                    if (target_type == sema_no_type() &&
                        !sema_resolve_type_node(lexer,
                                                ast,
                                                sema,
                                                literal->target_node_index,
                                                &target_type)) {
                        return false;
                    }
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
            bool target_is_plex  = target_type != sema_no_type() &&
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
            }
            if (target_is_plex && node->kind == AK_Plex &&
                literal->field_count != record->param_count &&
                !(literal->flags & APLF_DefaultMissing)) {
                StringBuilder missing = {0};
                sb_init(&missing, &temp_arena);
                u32 missing_count = 0;
                for (u32 i = 0; i < record->param_count; ++i) {
                    if (seen[i]) {
                        continue;
                    }
                    if (missing_count > 0) {
                        sb_append_cstr(&missing, ", ");
                    }
                    sb_append_char(&missing, '`');
                    sb_append_string(
                        &missing,
                        lex_symbol(
                            lexer,
                            sema->type_param_symbols[record->first_param_type +
                                                     i]));
                    sb_append_char(&missing, '`');
                    missing_count++;
                }

                if (missing_count > 0) {
                    return error_0304_missing_plex_fields(
                        lexer->source,
                        sema_node_span(lexer, node),
                        sb_to_string(&missing),
                        missing_count);
                }

                return error_0304_type_mismatch(lexer->source,
                                                sema_node_span(lexer, node),
                                                s("all plex fields"),
                                                s("different field count"));
            }

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
                    if (literal->flags & APLF_DefaultMissing) {
                        continue;
                    }
                    return error_0304_type_mismatch(lexer->source,
                                                    sema_node_span(lexer, node),
                                                    s("all plex fields"),
                                                    s("missing field"));
                }
            }
            type_index =
                enum_variant_constructor ? enum_constructor_type : target_type;
        }
        break;

    case AK_Field:
        {
            string field          = lex_symbol(lexer, node->b);
            u32    qualified_type = sema_no_type();
            if (sema_try_resolve_type_symbol(
                    lexer, ast, sema, node->a, &qualified_type)) {
                if (string_eq(field, s("size"))) {
                    type_index = sema_builtin_type(sema, STK_Usize);
                    break;
                }
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
                if (sema->types[qualified_type].kind == STK_Arena) {
                    sema->node_is_type_expr[node->a] = false;
                    sema->node_type_indices[node->a] = sema_no_type();
                    sema->node_decl_indices[node->a] = sema_no_decl();
                }
            }

            u32 target_type = sema_no_type();
            if (!sema_infer_node_type(
                    lexer, ast, sema, node->a, sema_no_type(), &target_type)) {
                return false;
            }
            if (target_type != sema_no_type() &&
                sema->types[target_type].kind == STK_Box) {
                if (string_eq(field, s("data"))) {
                    type_index = sema_add_pointer_type(
                        sema, sema->types[target_type].first_param_type);
                    break;
                }
                if (string_eq(field, s("count"))) {
                    type_index = sema_builtin_type(sema, STK_Usize);
                    break;
                }
            }
            u32 field_target_type = target_type;
            field_target_type     = sema_member_target_type(sema, target_type);
            if (field_target_type != sema_no_type() &&
                (sema->types[field_target_type].kind == STK_Plex ||
                 sema->types[field_target_type].kind == STK_Union)) {
                sema_record_member_type(
                    sema, field_target_type, node->b, &type_index);
                if (type_index == sema_no_type()) {
                    if (!string_eq(field, s("size"))) {
                        return sema_error_unknown_member(
                            lexer, ast, sema, node, target_type, node->b);
                    }
                } else {
                    break;
                }
            }
            if (string_eq(field, s("size"))) {
                if (target_type != sema_no_type() &&
                    sema->types[target_type].kind == STK_Module) {
                    return error_0304_type_mismatch(lexer->source,
                                                    sema_node_span(lexer, node),
                                                    s("runtime-sized value"),
                                                    s("module"));
                }
                type_index = sema_builtin_type(sema, STK_Usize);
                break;
            }
            if (string_eq(field, s("bytes"))) {
                if (target_type == sema_no_type() ||
                    (sema->types[field_target_type].kind != STK_String &&
                     sema->types[field_target_type].kind != STK_Array &&
                     sema->types[field_target_type].kind != STK_Slice)) {
                    return error_0304_type_mismatch(
                        lexer->source,
                        sema_node_span(lexer, node),
                        s("string, array, or slice field `.bytes`"),
                        sema_type_name(lexer, sema, &temp_arena, target_type));
                }
                type_index = sema_builtin_type(sema, STK_Usize);
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
                (sema->types[field_target_type].kind != STK_Array &&
                 sema->types[field_target_type].kind != STK_Slice &&
                 sema->types[field_target_type].kind != STK_String &&
                 sema->types[field_target_type].kind != STK_DynamicArray &&
                 sema->types[field_target_type].kind != STK_Enum)) {
                return error_0304_type_mismatch(
                    lexer->source,
                    sema_node_span(lexer, node),
                    s("array, slice, string, dynamic array, module, plex, "
                      "union, enum, or pointer to memberable value"),
                    sema_type_name(lexer, sema, &temp_arena, target_type));
            }
            if (string_eq(field, s("data"))) {
                u32 item_type =
                    sema->types[field_target_type].kind == STK_String
                        ? sema_builtin_type(sema, STK_U8)
                        : sema->types[field_target_type].first_param_type;
                type_index = sema_add_pointer_type(sema, item_type);
            } else if (string_eq(field, s("count")) ||
                       (string_eq(field, s("bytes")) &&
                        (sema->types[field_target_type].kind == STK_String ||
                         sema->types[field_target_type].kind == STK_Array ||
                         sema->types[field_target_type].kind == STK_Slice))) {
                type_index = sema_builtin_type(sema, STK_Usize);
            } else if (sema->types[field_target_type].kind ==
                           STK_DynamicArray &&
                       string_eq(field, s("capacity"))) {
                type_index = sema_builtin_type(sema, STK_Usize);
            } else if (sema->types[field_target_type].kind ==
                           STK_DynamicArray &&
                       sema_dynarray_method_signature(sema,
                                                      lexer,
                                                      field_target_type,
                                                      node->b,
                                                      &type_index)) {
                break;
            } else if (sema->types[field_target_type].kind == STK_Enum) {
                return sema_error_unknown_member(
                    lexer, ast, sema, node, target_type, node->b);
            } else {
                string expected =
                    sema->types[field_target_type].kind == STK_String
                        ? s("string field .data, .count, .bytes, or defined "
                            "method")
                    : sema->types[field_target_type].kind == STK_DynamicArray
                        ? s("dynamic array field .data, .count, .capacity, "
                            "or defined method")
                    : sema->types[field_target_type].kind == STK_Array
                        ? s("array field .data, .count, .bytes, or defined "
                            "method")
                        : s("slice field .data, .count, .bytes, or defined "
                            "method");
                return error_0304_type_mismatch(lexer->source,
                                                sema_node_span(lexer, node),
                                                expected,
                                                field);
            }
        }
        break;

    case AK_Array:
        {
            u32 expected_materialised =
                expected_type == sema_no_type()
                    ? sema_no_type()
                    : sema_materialise_type(sema, expected_type);
            const SemaType* expected_slice =
                expected_materialised != sema_no_type() &&
                        (sema->types[expected_materialised].kind == STK_Slice ||
                         sema->types[expected_materialised].kind ==
                             STK_DynamicArray)
                    ? &sema->types[expected_materialised]
                    : NULL;
            const SemaType* expected_array =
                expected_materialised != sema_no_type() &&
                        sema->types[expected_materialised].kind == STK_Array
                    ? &sema->types[expected_materialised]
                    : NULL;
            if (expected_array != NULL &&
                expected_array->return_type != node->b) {
                return error_0304_type_mismatch(
                    lexer->source,
                    sema_node_span(lexer, node),
                    sema_type_name(lexer, sema, &temp_arena, expected_type),
                    s("array with different length"));
            }

            u32 item_type =
                expected_array != NULL
                    ? expected_array->first_param_type
                    : (expected_slice != NULL ? expected_slice->first_param_type
                                              : sema_no_type());
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

            u32 array_type = sema_add_array_type(sema, item_type, node->b);
            if (expected_slice != NULL) {
                type_index = expected_type;
                if (sema->types[expected_type].kind == STK_Slice) {
                    sema->node_implicit_array_type_indices[node_index] =
                        array_type;
                }
            } else {
                type_index = array_type;
            }
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
                 sema->types[target_type].kind != STK_DynamicArray &&
                 sema->types[target_type].kind != STK_String)) {
                return error_0304_type_mismatch(
                    lexer->source,
                    sema_node_span(lexer, node),
                    s("array, slice, dynamic array, or string"),
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
            const AstNode* target = &ast->nodes[node->a];
            if (target->kind == AK_SymbolRef) {
                if (string_eq(lex_symbol(lexer, target->a), s("box"))) {
                    u32 item_type = sema_no_type();
                    if (!sema_resolve_type_node(
                            lexer, ast, sema, node->b, &item_type)) {
                        return false;
                    }
                    type_index = sema_add_box_type(sema, item_type);
                    break;
                }

                u32 decl_index = sema->node_decl_indices[node->a];
                if (decl_index != sema_no_decl() &&
                    sema->decls[decl_index].kind == SK_GenericFunction) {
                    Array(u32) explicit_arg_nodes = NULL;
                    sema_collect_generic_arg_nodes(
                        ast, node->b, &explicit_arg_nodes);
                    u32 symbol = U32_MAX;
                    if (!sema_instantiate_generic_function(
                            lexer,
                            ast,
                            sema,
                            decl_index,
                            explicit_arg_nodes,
                            (u32)array_count(explicit_arg_nodes),
                            NULL,
                            expected_type,
                            &symbol,
                            &type_index)) {
                        array_free(explicit_arg_nodes);
                        return false;
                    }
                    sema->node_lowered_symbol_handles[node_index] = symbol;
                    array_free(explicit_arg_nodes);
                    break;
                }
            }

            u32 array_type = sema_no_type();
            if (!sema_infer_node_type(
                    lexer, ast, sema, node->a, sema_no_type(), &array_type)) {
                return false;
            }
            if (array_type == sema_no_type() ||
                (sema->types[array_type].kind != STK_Array &&
                 sema->types[array_type].kind != STK_Slice &&
                 sema->types[array_type].kind != STK_DynamicArray &&
                 sema->types[array_type].kind != STK_String &&
                 sema->types[array_type].kind != STK_Pointer)) {
                return error_0304_type_mismatch(
                    lexer->source,
                    sema_node_span(lexer, node),
                    s("array, slice, dynamic array, string, or pointer"),
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

            type_index = sema->types[array_type].kind == STK_String
                             ? sema_builtin_type(sema, STK_U8)
                             : sema->types[array_type].first_param_type;
        }
        break;

    case AK_Deref:
        {
            u32 pointer_type = sema_no_type();
            if (!sema_infer_node_type(
                    lexer, ast, sema, node->a, sema_no_type(), &pointer_type)) {
                return false;
            }
            if (pointer_type == sema_no_type() ||
                sema->types[pointer_type].kind != STK_Pointer) {
                return error_0304_type_mismatch(
                    lexer->source,
                    sema_node_span(lexer, node),
                    s("pointer"),
                    sema_type_name(lexer, sema, &temp_arena, pointer_type));
            }
            type_index = sema->types[pointer_type].first_param_type;
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
        {
            u32 return_expected = expected_type;
            if (return_expected == sema_no_type()) {
                return_expected =
                    sema_enclosing_function_return_type(sema, node_index);
            }
            if (return_expected == sema_no_type()) {
                return_expected = sema_ast_enclosing_function_return_type(
                    lexer, ast, sema, node_index);
            }

            if (node->a == U32_MAX) {
                type_index = return_expected != sema_no_type()
                                 ? return_expected
                                 : sema_builtin_type(sema, STK_Void);
            } else if (!sema_infer_node_type(lexer,
                                             ast,
                                             sema,
                                             node->a,
                                             return_expected,
                                             &type_index)) {
                return false;
            }
        }
        break;

    case AK_Defer:
        {
            u32 ignored = sema_no_type();
            if (!sema_infer_node_type(
                    lexer, ast, sema, node->a, sema_no_type(), &ignored)) {
                return false;
            }
            type_index = sema_builtin_type(sema, STK_Void);
        }
        break;

    case AK_Assert:
        {
            u32 condition_type = sema_no_type();
            u32 bool_type      = sema_builtin_type(sema, STK_Bool);
            if (!sema_infer_node_type(
                    lexer, ast, sema, node->a, bool_type, &condition_type)) {
                return false;
            }
            if (!sema_type_matches(sema, bool_type, condition_type)) {
                return error_0304_type_mismatch(
                    lexer->source,
                    sema_node_span(lexer, &ast->nodes[node->a]),
                    sema_type_name(lexer, sema, &temp_arena, bool_type),
                    sema_type_name(lexer, sema, &temp_arena, condition_type));
            }
            if (node->b != U32_MAX) {
                u32 message_type = sema_no_type();
                u32 string_type  = sema_builtin_type(sema, STK_String);
                if (!sema_infer_node_type(lexer,
                                          ast,
                                          sema,
                                          node->b,
                                          string_type,
                                          &message_type)) {
                    return false;
                }
                if (message_type != string_type) {
                    return error_0304_type_mismatch(
                        lexer->source,
                        sema_node_span(lexer, &ast->nodes[node->b]),
                        sema_type_name(lexer, sema, &temp_arena, string_type),
                        sema_type_name(lexer, sema, &temp_arena, message_type));
                }
            }
            type_index = sema_builtin_type(sema, STK_Void);
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

    case AK_Block:
        {
            bool has_return = false;
            if (!sema_infer_block_statements(lexer,
                                             ast,
                                             sema,
                                             node->a,
                                             node->b,
                                             sema_no_type(),
                                             &type_index,
                                             &has_return)) {
                return false;
            }
            type_index = sema_builtin_type(sema, STK_Void);
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
            if (for_info->condition_node_index != U32_MAX &&
                !sema_seed_for_condition_local_types(
                    lexer, ast, sema, for_info->condition_node_index)) {
                return false;
            }
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
            if (for_info->iterable_node_index != U32_MAX) {
                u32 iterable_type = sema_no_type();
                u32 for_scope     = sema->node_scope_indices[node_index];
                if (!sema_infer_for_iterable_type(lexer,
                                                  ast,
                                                  sema,
                                                  node_index,
                                                  for_info,
                                                  for_scope,
                                                  &iterable_type)) {
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
                                             sema_no_type(),
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
                                                 sema_no_type(),
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
            if (else_block != NULL && else_has_value_break &&
                !body_has_value_break) {
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
                u32        local_index = sema->node_local_indices[node_index];
                SemaLocal* local       = &sema->locals[local_index];
                if (sema_local_is_decl_binding(local) &&
                    local->type_index == sema_no_type()) {
                    if (!sema_infer_local_binding_type(
                            lexer, ast, sema, local_index, &type_index)) {
                        return false;
                    }
                } else {
                    type_index = local->type_index;
                }
                if (expected_type != sema_no_type() &&
                    type_index != sema_no_type() &&
                    !sema_type_matches(sema, expected_type, type_index)) {
                    if (!sema_seed_destructured_local_type_from_expected(
                            lexer, ast, sema, local_index, expected_type)) {
                        return false;
                    }
                    local      = &sema->locals[local_index];
                    type_index = local->type_index;
                }
            } else {
                u32 decl_index = sema->node_decl_indices[node_index];
                if (decl_index == sema_no_decl()) {
                    decl_index = sema_find_decl(sema, node->a);
                    if (decl_index != sema_no_decl()) {
                        sema->node_decl_indices[node_index] = decl_index;
                    }
                }
                if (decl_index == sema_no_decl()) {
                    u32 contextual_payload = sema_no_type();
                    if (expected_type != sema_no_type() &&
                        sema->types[expected_type].kind == STK_Enum &&
                        (sema->types[expected_type].flags &
                         (STF_Optional | STF_Result))) {
                        const SemaType* wrapper = &sema->types[expected_type];
                        contextual_payload =
                            sema->type_param_types
                                [wrapper->first_param_type +
                                 ((wrapper->flags & STF_Optional) ? 1 : 0)];
                    }
                    if (contextual_payload != sema_no_type() &&
                        sema->types[contextual_payload].kind == STK_Enum &&
                        sema_enum_variant_index(
                            sema, contextual_payload, node->a) != U32_MAX) {
                        type_index = contextual_payload;
                        break;
                    }
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
                if (sema->decls[decl_index].type_index == sema_no_type()) {
                    u32 decl_type = sema_no_type();
                    if (sema->decls[decl_index].type_node_index !=
                        sema_no_type()) {
                        if (!sema_resolve_type_node(
                                lexer,
                                ast,
                                sema,
                                sema->decls[decl_index].type_node_index,
                                &decl_type)) {
                            return false;
                        }
                    } else if (sema->decls[decl_index].value_node_index !=
                               sema_no_decl()) {
                        if (!sema_infer_node_type(
                                lexer,
                                ast,
                                sema,
                                sema->decls[decl_index].value_node_index,
                                sema_no_type(),
                                &decl_type)) {
                            return false;
                        }
                    }
                    sema->decls[decl_index].type_index = decl_type;
                }
                type_index = sema->decls[decl_index].type_index;
            }
            if (sema_type_is_concrete_integer(sema, value_expected) &&
                type_index != sema_no_type() &&
                sema->types[type_index].kind == STK_UntypedInteger) {
                if (sema_type_is_unsigned_integer(sema, value_expected)) {
                    i64 value = 0;
                    if (sema_try_eval_integer_constant(
                            lexer, ast, sema, node_index, &value) &&
                        value < 0) {
                        return error_0323_negative_unsigned_inference(
                            lexer->source,
                            sema_node_span(lexer, node),
                            sema_type_name(
                                lexer, sema, &temp_arena, value_expected));
                    }
                }
                type_index = value_expected;
            } else if (sema_type_is_concrete_float(sema, value_expected) &&
                       type_index != sema_no_type() &&
                       sema->types[type_index].kind == STK_UntypedFloat) {
                type_index = value_expected;
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
    case AK_BitwiseNot:
        if (node->kind == AK_LogicalNot) {
            u32 bool_type = sema_builtin_type(sema, STK_Bool);
            if (!sema_infer_node_type(
                    lexer, ast, sema, node->a, bool_type, &type_index)) {
                return false;
            }
            if (!sema_type_matches(sema, bool_type, type_index)) {
                return error_0325_invalid_unary_operand(
                    lexer->source,
                    sema_node_span(lexer, node),
                    s("!"),
                    s("bool"),
                    sema_type_name(lexer, sema, &temp_arena, type_index));
            }
            type_index = bool_type;
            break;
        }
        if (!sema_infer_node_type(
                lexer, ast, sema, node->a, expected_type, &type_index)) {
            return false;
        }
        if (node->kind == AK_BitwiseNot) {
            if (!sema_type_is_integer(sema, type_index)) {
                return error_0325_invalid_unary_operand(
                    lexer->source,
                    sema_node_span(lexer, node),
                    s("~"),
                    s("integer"),
                    sema_type_name(lexer, sema, &temp_arena, type_index));
            }
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

    case AK_ErrorInject:
        {
            u32 result_type = expected_type;
            if (result_type == sema_no_type()) {
                result_type = sema_ast_enclosing_function_return_type(
                    lexer, ast, sema, node_index);
            }
            if (result_type == sema_no_type() ||
                sema->types[result_type].kind != STK_Enum ||
                !(sema->types[result_type].flags & STF_Result)) {
                return error_0304_type_mismatch(
                    lexer->source,
                    sema_node_span(lexer, node),
                    s("contextual result type T\\E"),
                    s("error injection without an expected result type"));
            }
            u32 error_type =
                sema->type_param_types
                    [sema->types[result_type].first_param_type + 1];
            u32 actual = sema_no_type();
            if (!sema_infer_node_type(
                    lexer, ast, sema, node->a, error_type, &actual)) {
                return false;
            }
            if (!sema_type_matches(sema, error_type, actual)) {
                return error_0304_type_mismatch(
                    lexer->source,
                    sema_node_span(lexer, &ast->nodes[node->a]),
                    sema_type_name(lexer, sema, &temp_arena, error_type),
                    sema_type_name(lexer, sema, &temp_arena, actual));
            }
            type_index = result_type;
        }
        break;

    case AK_Propagate:
        {
            u32 operand_type = sema_no_type();
            if (!sema_infer_node_type(
                    lexer, ast, sema, node->a, sema_no_type(), &operand_type)) {
                return false;
            }
            if (operand_type == sema_no_type() ||
                sema->types[operand_type].kind != STK_Enum ||
                !(sema->types[operand_type].flags &
                  (STF_Optional | STF_Result))) {
                return error_0304_type_mismatch(
                    lexer->source,
                    sema_node_span(lexer, node),
                    s("optional or result value"),
                    sema_type_name(lexer, sema, &temp_arena, operand_type));
            }
            const SemaType* operand = &sema->types[operand_type];
            type_index =
                sema->type_param_types[operand->first_param_type +
                                       ((operand->flags & STF_Optional) ? 1
                                                                        : 0)];
            u32 enclosing = sema_ast_enclosing_function_return_type(
                lexer, ast, sema, node_index);
            if (enclosing == sema_no_type() ||
                sema->types[enclosing].kind != STK_Enum ||
                ((operand->flags & STF_Optional) &&
                 !(sema->types[enclosing].flags & STF_Optional)) ||
                ((operand->flags & STF_Result) &&
                 !(sema->types[enclosing].flags & STF_Result))) {
                return error_0304_type_mismatch(
                    lexer->source,
                    sema_node_span(lexer, node),
                    sema_type_name(lexer, sema, &temp_arena, operand_type),
                    s("incompatible enclosing return type"));
            }
            if ((operand->flags & STF_Result) &&
                (sema->types[enclosing].flags & STF_Result)) {
                u32 operand_error =
                    sema->type_param_types[operand->first_param_type + 1];
                u32 enclosing_error =
                    sema->type_param_types
                        [sema->types[enclosing].first_param_type + 1];
                if (!sema_type_matches(sema, enclosing_error, operand_error)) {
                    return error_0304_type_mismatch(
                        lexer->source,
                        sema_node_span(lexer, node),
                        sema_type_name(
                            lexer, sema, &temp_arena, enclosing_error),
                        sema_type_name(
                            lexer, sema, &temp_arena, operand_error));
                }
            }
        }
        break;

    case AK_Cast:
        {
            const AstCastInfo* cast        = sema_cast_info(ast, node);
            u32                source_type = sema_no_type();
            u32                target_type = sema_no_type();
            if (!sema_infer_node_type(
                    lexer, ast, sema, node->a, sema_no_type(), &source_type)) {
                return false;
            }
            if (!sema_resolve_type_node(
                    lexer, ast, sema, cast->type_node_index, &target_type)) {
                return false;
            }

            if (cast->extra_node_index != U32_MAX) {
                u32 size_type = sema_no_type();
                if (!sema_infer_node_type(lexer,
                                          ast,
                                          sema,
                                          cast->extra_node_index,
                                          sema_builtin_type(sema, STK_Usize),
                                          &size_type)) {
                    return false;
                }

                if (source_type == sema_no_type() ||
                    sema->types[source_type].kind != STK_Pointer) {
                    return error_0307_invalid_cast(
                        lexer->source,
                        sema_node_span(lexer, node),
                        sema_type_name(lexer, sema, &temp_arena, source_type),
                        sema_type_name(lexer, sema, &temp_arena, target_type));
                }
                if (target_type == sema_no_type() ||
                    sema->types[target_type].kind != STK_Slice) {
                    return error_0307_invalid_cast(
                        lexer->source,
                        sema_node_span(lexer, node),
                        sema_type_name(lexer, sema, &temp_arena, source_type),
                        sema_type_name(lexer, sema, &temp_arena, target_type));
                }
                if (!sema_type_is_integer(sema, size_type)) {
                    return error_0304_type_mismatch(
                        lexer->source,
                        sema_node_span(lexer,
                                       &ast->nodes[cast->extra_node_index]),
                        s("integer"),
                        sema_type_name(lexer, sema, &temp_arena, size_type));
                }

                u32  source_item = sema->types[source_type].first_param_type;
                u32  target_item = sema->types[target_type].first_param_type;
                bool item_match  = source_item == target_item ||
                                   sema->types[source_item].kind == STK_Void ||
                                   sema->types[target_item].kind == STK_Void;
                if (!item_match) {
                    return error_0307_invalid_cast(
                        lexer->source,
                        sema_node_span(lexer, node),
                        sema_type_name(lexer, sema, &temp_arena, source_type),
                        sema_type_name(lexer, sema, &temp_arena, target_type));
                }

                type_index = target_type;
                break;
            }

            bool primitive_cast =
                sema_type_is_castable_primitive(sema, source_type) &&
                sema_type_is_castable_primitive(sema, target_type) &&
                sema->types[target_type].kind != STK_UntypedInteger &&
                sema->types[target_type].kind != STK_UntypedFloat;

            bool enum_integer_cast =
                source_type != sema_no_type() &&
                target_type != sema_no_type() &&
                ((sema->types[source_type].kind == STK_Enum &&
                  sema_type_is_integer(sema, target_type)) ||
                 (sema_type_is_integer(sema, source_type) &&
                  sema->types[target_type].kind == STK_Enum));

            bool string_slice_cast =
                (source_type == sema_builtin_type(sema, STK_String) &&
                 sema_type_is_u8_slice(sema, target_type)) ||
                (target_type == sema_builtin_type(sema, STK_String) &&
                 sema_type_is_byte_collection(sema, source_type));

            bool untyped_integer_pointer_cast =
                source_type != sema_no_type() &&
                target_type != sema_no_type() &&
                sema->types[source_type].kind == STK_UntypedInteger &&
                sema->types[target_type].kind == STK_Pointer;

            bool pointer_sized_integer_pointer_cast =
                source_type != sema_no_type() &&
                target_type != sema_no_type() &&
                sema_type_is_pointer_sized_integer(sema, source_type) &&
                sema->types[target_type].kind == STK_Pointer;

            bool pointer_pointer_sized_integer_cast =
                source_type != sema_no_type() &&
                target_type != sema_no_type() &&
                sema->types[source_type].kind == STK_Pointer &&
                sema_type_is_pointer_sized_integer(sema, target_type);

            bool void_pointer_cast =
                source_type != sema_no_type() &&
                target_type != sema_no_type() &&
                sema->types[source_type].kind == STK_Pointer &&
                sema->types[target_type].kind == STK_Pointer &&
                (sema->types[sema->types[source_type].first_param_type].kind ==
                     STK_Void ||
                 sema->types[sema->types[target_type].first_param_type].kind ==
                     STK_Void);

            bool pointer_pointer_cast =
                source_type != sema_no_type() &&
                target_type != sema_no_type() &&
                sema->types[source_type].kind == STK_Pointer &&
                sema->types[target_type].kind == STK_Pointer;

            bool pointer_function_cast =
                source_type != sema_no_type() &&
                target_type != sema_no_type() &&
                ((sema->types[source_type].kind == STK_Pointer &&
                  sema->types[target_type].kind == STK_Function) ||
                 (sema->types[source_type].kind == STK_Function &&
                  sema->types[target_type].kind == STK_Pointer));

            if (!(primitive_cast || enum_integer_cast || string_slice_cast ||
                  untyped_integer_pointer_cast ||
                  pointer_sized_integer_pointer_cast ||
                  pointer_pointer_sized_integer_cast || void_pointer_cast ||
                  pointer_pointer_cast || pointer_function_cast)) {
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
        return error_0304_type_mismatch(
            lexer->source, sema_node_span(lexer, node), s("value"), s("range"));

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
            if (annotated == sema_no_type()) {
                u32 contextual_type = sema_no_type();
                if (!sema_destructure_pattern_type_from_existing_locals(
                        ast, sema, node->a, value_type, &contextual_type)) {
                    return false;
                }
                if (contextual_type != value_type) {
                    u32 reinferred = sema_no_type();
                    if (!sema_infer_node_type(lexer,
                                              ast,
                                              sema,
                                              value_node,
                                              contextual_type,
                                              &reinferred)) {
                        return false;
                    }
                    if (sema_type_matches(sema, contextual_type, reinferred)) {
                        value_type = contextual_type;
                    }
                }
            }
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
                if (!sema_type_matches(sema, bool_type, scrutinee_type)) {
                    return error_0319_invalid_on_condition(
                        lexer->source,
                        sema_node_span(lexer, &ast->nodes[node->a]),
                        sema_type_name(
                            lexer, sema, &temp_arena, scrutinee_type));
                }
                scrutinee_type = bool_type;
            } else if (on->kind == AOK_Extract) {
                if (scrutinee_type == sema_no_type() ||
                    sema->types[scrutinee_type].kind != STK_Enum ||
                    !(sema->types[scrutinee_type].flags &
                      (STF_Optional | STF_Result))) {
                    return error_0321_invalid_on_match_type(
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
                      sema->types[scrutinee_type].kind == STK_Pointer ||
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
                if (branch->flags & AOBF_Error) {
                    has_else = true;
                }
                u32 branch_local_index =
                    sema->on_branch_local_indices[on->first_branch + i];
                if (branch_local_index != sema_no_local()) {
                    u32 binder_type = scrutinee_type;
                    if (on->kind == AOK_Extract) {
                        const SemaType* extracted =
                            &sema->types[scrutinee_type];
                        if ((branch->flags & AOBF_Else) &&
                            (extracted->flags & STF_Optional)) {
                            return error_0304_type_mismatch(
                                lexer->source,
                                sema_node_span(lexer, node),
                                s("optional else branch without a payload "
                                  "binder"),
                                s("payload binder"));
                        }
                        u32 variant = (branch->flags & AOBF_Else) ? 1 : 0;
                        if (extracted->flags & STF_Optional) {
                            variant = 1;
                        }
                        binder_type =
                            sema->type_param_types[extracted->first_param_type +
                                                   variant];
                    }
                    sema->locals[branch_local_index].type_index = binder_type;
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
                    if (!sema_type_matches(sema, bool_type, condition_type)) {
                        return error_0319_invalid_on_condition(
                            lexer->source,
                            sema_node_span(
                                lexer, &ast->nodes[branch->guard_node_index]),
                            sema_type_name(
                                lexer, sema, &temp_arena, condition_type));
                    }
                } else if (on->kind != AOK_Extract &&
                           !(branch->flags & AOBF_Else)) {
                    u32 pattern_type = scrutinee_type;
                    if (on->kind == AOK_Value &&
                        sema->types[scrutinee_type].kind == STK_Enum &&
                        (sema->types[scrutinee_type].flags &
                         (STF_Optional | STF_Result))) {
                        u32 variant = (branch->flags & AOBF_Error) ? 1 : 0;
                        if (sema->types[scrutinee_type].flags & STF_Optional) {
                            variant = 1;
                        }
                        pattern_type =
                            sema->type_param_types[sema->types[scrutinee_type]
                                                       .first_param_type +
                                                   variant];
                    }
                    for (u32 pattern = 0; pattern < branch->pattern_count;
                         ++pattern) {
                        u32 pattern_index =
                            ast->pattern_items[branch->pattern_index + pattern];
                        if (!sema_check_on_pattern_type(
                                lexer,
                                ast,
                                sema,
                                pattern_index,
                                pattern_type,
                                !(branch->flags & AOBF_Error))) {
                            return false;
                        }
                        if (branch->pattern_count > 1 &&
                            !sema_check_multi_on_pattern_payload_variant(
                                lexer,
                                ast,
                                sema,
                                pattern_index,
                                scrutinee_type)) {
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
                    if (!sema_type_matches(sema, bool_type, guard_type)) {
                        return error_0319_invalid_on_condition(
                            lexer->source,
                            sema_node_span(
                                lexer, &ast->nodes[branch->guard_node_index]),
                            sema_type_name(
                                lexer, sema, &temp_arena, guard_type));
                    }
                }

                if (statement_form) {
                    u32 ignored         = sema_no_type();
                    u32 branch_expected = sema_no_type();
                    if (sema_node_definitely_returns(
                            ast, sema, branch->expr_node_index)) {
                        branch_expected = sema_enclosing_function_return_type(
                            sema, node_index);
                    }
                    if (!sema_infer_node_type(lexer,
                                              ast,
                                              sema,
                                              branch->expr_node_index,
                                              branch_expected,
                                              &ignored)) {
                        return false;
                    }
                    continue;
                }

                u32  current_expected          = expected_type;
                bool branch_definitely_returns = sema_node_definitely_returns(
                    ast, sema, branch->expr_node_index);
                if (current_expected == sema_no_type() &&
                    branch_definitely_returns) {
                    current_expected =
                        sema_enclosing_function_return_type(sema, node_index);
                }
                if (current_expected == sema_no_type() &&
                    !branch_definitely_returns &&
                    branch_type != sema_no_type() &&
                    sema_type_is_concrete_integer(sema, branch_type)) {
                    current_expected = branch_type;
                } else if (current_expected == sema_no_type() &&
                           !branch_definitely_returns &&
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

                if (current_expected != sema_no_type() &&
                    sema->types[current_expected].kind == STK_Enum &&
                    (sema->types[current_expected].flags &
                     (STF_Optional | STF_Result)) &&
                    sema_type_matches(sema, current_expected, current_type)) {
                    current_type = current_expected;
                }

                if (branch_definitely_returns) {
                    continue;
                }

                if (sema_type_is_never(sema, current_type)) {
                    continue;
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
                if (type_index == sema_no_type()) {
                    type_index = expected_type != sema_no_type()
                                     ? expected_type
                                     : sema_builtin_type(sema, STK_Never);
                }
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
    case AK_ShiftLeft:
    case AK_ShiftRight:
    case AK_Equal:
    case AK_NotEqual:
    case AK_Less:
    case AK_LessEqual:
    case AK_Greater:
    case AK_GreaterEqual:
    case AK_LogicalAnd:
    case AK_LogicalOr:
        {
            if (node->kind == AK_LogicalAnd || node->kind == AK_LogicalOr) {
                u32 bool_type = sema_builtin_type(sema, STK_Bool);
                u32 lhs_type  = sema_no_type();
                u32 rhs_type  = sema_no_type();
                if (!sema_infer_node_type(
                        lexer, ast, sema, node->a, sema_no_type(), &lhs_type) ||
                    !sema_infer_node_type(
                        lexer, ast, sema, node->b, sema_no_type(), &rhs_type)) {
                    return false;
                }
                if (!sema_type_matches(sema, bool_type, lhs_type) ||
                    !sema_type_matches(sema, bool_type, rhs_type)) {
                    return error_0326_invalid_binary_operands(
                        lexer->source,
                        sema_node_span(lexer, node),
                        node->kind == AK_LogicalAnd ? s("&&") : s("||"),
                        s("matching bool operands"),
                        sema_type_name(lexer, sema, &temp_arena, lhs_type),
                        sema_type_name(lexer, sema, &temp_arena, rhs_type));
                }
                type_index = bool_type;
                break;
            }

            u32 lhs_type = sema_no_type();
            u32 rhs_type = sema_no_type();
            u32 binary_expected =
                sema_expected_numeric_type(sema, expected_type);
            if ((node->kind == AK_Equal || node->kind == AK_NotEqual) &&
                ast->nodes[node->a].kind == AK_NilLiteral) {
                if (!sema_infer_node_type(lexer,
                                          ast,
                                          sema,
                                          node->b,
                                          binary_expected,
                                          &rhs_type)) {
                    return false;
                }
                if (!sema_infer_node_type(
                        lexer, ast, sema, node->a, rhs_type, &lhs_type)) {
                    return false;
                }
            } else {
                if (!sema_infer_node_type(lexer,
                                          ast,
                                          sema,
                                          node->a,
                                          binary_expected,
                                          &lhs_type)) {
                    return false;
                }
                u32  rhs_expected = lhs_type;
                bool lhs_pointer =
                    lhs_type != sema_no_type() &&
                    sema->types[sema_materialise_type(sema, lhs_type)].kind ==
                        STK_Pointer;
                bool lhs_untyped_integer =
                    lhs_type != sema_no_type() &&
                    sema->types[lhs_type].kind == STK_UntypedInteger;
                if (lhs_pointer ||
                    (node->kind == AK_IntegerPlus && lhs_untyped_integer)) {
                    rhs_expected = sema_no_type();
                } else if (sema_type_is_numeric(sema, lhs_type)) {
                    rhs_expected = sema_expected_numeric_type(sema, lhs_type);
                }
                if (!sema_infer_node_type(
                        lexer, ast, sema, node->b, rhs_expected, &rhs_type)) {
                    return false;
                }
            }

            if ((node->kind == AK_Equal || node->kind == AK_NotEqual) &&
                ast->nodes[node->b].kind == AK_NilLiteral &&
                (rhs_type == sema_no_type() ||
                 sema->types[rhs_type].kind == STK_Nil)) {
                if (!sema_infer_node_type(
                        lexer, ast, sema, node->b, lhs_type, &rhs_type)) {
                    return false;
                }
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

            if ((node->kind == AK_Equal || node->kind == AK_NotEqual) &&
                lhs_type != rhs_type) {
                if (lhs_type != sema_no_type() &&
                    sema->types[lhs_type].kind == STK_Enum &&
                    (sema->types[lhs_type].flags &
                     (STF_Optional | STF_Result)) &&
                    sema_type_matches(sema, lhs_type, rhs_type)) {
                    rhs_type = lhs_type;
                } else if (rhs_type != sema_no_type() &&
                           sema->types[rhs_type].kind == STK_Enum &&
                           (sema->types[rhs_type].flags &
                            (STF_Optional | STF_Result)) &&
                           sema_type_matches(sema, rhs_type, lhs_type)) {
                    lhs_type = rhs_type;
                }
            }

            u32  pointer_arithmetic_type = sema_no_type();
            bool pointer_arithmetic      = sema_pointer_arithmetic_result_type(
                sema, node->kind, lhs_type, rhs_type, &pointer_arithmetic_type);
            bool pointer_equality =
                (node->kind == AK_Equal || node->kind == AK_NotEqual) &&
                sema_pointer_types_are_equality_comparable(
                    sema, lhs_type, rhs_type);
            if (lhs_type != rhs_type && !pointer_equality &&
                !pointer_arithmetic) {
                return error_0304_type_mismatch(
                    lexer->source,
                    sema_node_span(lexer, node),
                    sema_type_name(lexer, sema, &temp_arena, lhs_type),
                    sema_type_name(lexer, sema, &temp_arena, rhs_type));
            }

            switch (node->kind) {
            case AK_IntegerPlus:
            case AK_IntegerMinus:
                if (pointer_arithmetic) {
                    type_index = pointer_arithmetic_type;
                    break;
                }
                // fallthrough
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
            case AK_ShiftLeft:
            case AK_ShiftRight:
                if (!sema_type_is_integer(sema, lhs_type)) {
                    string op = s("|");
                    switch (node->kind) {
                    case AK_IntegerModulo:
                        op = s("%");
                        break;
                    case AK_BitwiseAnd:
                        op = s("&");
                        break;
                    case AK_BitwiseXor:
                        op = s("^");
                        break;
                    case AK_BitwiseOr:
                        op = s("|");
                        break;
                    case AK_ShiftLeft:
                        op = s("<<");
                        break;
                    case AK_ShiftRight:
                        op = s(">>");
                        break;
                    default:
                        break;
                    }
                    return error_0326_invalid_binary_operands(
                        lexer->source,
                        sema_node_span(lexer, node),
                        op,
                        s("matching integer operands"),
                        sema_type_name(lexer, sema, &temp_arena, lhs_type),
                        sema_type_name(lexer, sema, &temp_arena, rhs_type));
                }
                type_index = lhs_type;
                break;
            case AK_Equal:
            case AK_NotEqual:
                if (lhs_type != rhs_type &&
                    ast->nodes[node->a].kind == AK_NilLiteral) {
                    lhs_type = rhs_type;
                } else if (lhs_type != rhs_type &&
                           ast->nodes[node->b].kind == AK_NilLiteral) {
                    rhs_type = lhs_type;
                }
                if (sema_pointer_types_are_equality_comparable(
                        sema, lhs_type, rhs_type)) {
                    type_index = sema_builtin_type(sema, STK_Bool);
                    break;
                }
                u32 eq_method_decl = sema_find_core_eq_method_decl(
                    lexer, ast, sema, lhs_type, rhs_type);
                if (eq_method_decl != sema_no_decl()) {
                    sema->node_method_call_decl_indices[node_index] =
                        eq_method_decl;
                    type_index = sema_builtin_type(sema, STK_Bool);
                    break;
                }
                if (!sema_type_is_equality_comparable(sema, lhs_type)) {
                    if ((ast->nodes[node->a].kind == AK_NilLiteral ||
                         ast->nodes[node->b].kind == AK_NilLiteral) &&
                        lhs_type != sema_no_type() &&
                        (sema->types[lhs_type].kind == STK_Slice ||
                         sema->types[lhs_type].kind == STK_DynamicArray)) {
                        type_index = sema_builtin_type(sema, STK_Bool);
                        break;
                    }
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
                {
                    u32 order_method_decl = sema_find_core_order_method_decl(
                        lexer, ast, sema, lhs_type, rhs_type);
                    if (order_method_decl != sema_no_decl()) {
                        sema->node_method_call_decl_indices[node_index] =
                            order_method_decl;
                        type_index = sema_builtin_type(sema, STK_Bool);
                        break;
                    }
                }
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
            const AstCallInfo* call        = &ast->calls[node->b];
            const AstNode*     callee_node = &ast->nodes[node->a];
            u32                box_type    = sema_no_type();
            if (sema_node_is_box_constructor(
                    lexer, ast, sema, node->a, &box_type)) {
                if (box_type == sema_no_type()) {
                    return false;
                }
                if (call->arg_count > 1) {
                    return error_0313_argument_count_mismatch(
                        lexer->source,
                        sema_node_span(lexer, node),
                        1,
                        call->arg_count);
                }
                if (call->arg_count == 1) {
                    u32 item_type    = sema->types[box_type].first_param_type;
                    u32 pointer_type = sema_add_pointer_type(sema, item_type);
                    u32 arg_node     = ast->call_args[call->first_arg];
                    u32 arg_type     = sema_no_type();
                    if (!sema_infer_node_type(lexer,
                                              ast,
                                              sema,
                                              arg_node,
                                              sema_no_type(),
                                              &arg_type)) {
                        return false;
                    }
                    bool pointer_arg =
                        sema_type_matches(sema, pointer_type, arg_type);
                    bool count_arg = sema_type_is_integer(sema, arg_type);
                    if (!pointer_arg && !count_arg) {
                        return error_0304_type_mismatch(
                            lexer->source,
                            sema_node_span(lexer, &ast->nodes[arg_node]),
                            string_format(
                                &temp_arena,
                                STRINGP " or integer count",
                                STRINGV(sema_type_name(
                                    lexer, sema, &temp_arena, pointer_type))),
                            sema_type_name(lexer, sema, &temp_arena, arg_type));
                    }
                }
                sema->node_type_indices[node->a] = box_type;
                type_index                       = box_type;
                break;
            }
            if (callee_node->kind == AK_SymbolRef &&
                string_eq(lex_symbol(lexer, callee_node->a), s("arena")) &&
                sema->node_local_indices[node->a] == sema_no_local() &&
                sema->node_decl_indices[node->a] == sema_no_decl()) {
                if (call->arg_count < 1 || call->arg_count > 2) {
                    return error_0313_argument_count_mismatch(
                        lexer->source,
                        sema_node_span(lexer, node),
                        call->arg_count < 1 ? 1 : 2,
                        call->arg_count);
                }

                u32 usize_type = sema_builtin_type(sema, STK_Usize);
                for (u32 i = 0; i < call->arg_count; ++i) {
                    u32 arg_node = ast->call_args[call->first_arg + i];
                    u32 arg_type = sema_no_type();
                    if (!sema_infer_node_type(lexer,
                                              ast,
                                              sema,
                                              arg_node,
                                              usize_type,
                                              &arg_type)) {
                        return false;
                    }
                    if (!sema_type_is_integer(sema, arg_type)) {
                        return error_0304_type_mismatch(
                            lexer->source,
                            sema_node_span(lexer, &ast->nodes[arg_node]),
                            s("integer"),
                            sema_type_name(lexer, sema, &temp_arena, arg_type));
                    }
                }

                sema->node_type_indices[node->a] =
                    sema_builtin_type(sema, STK_Arena);
                type_index = sema_builtin_type(sema, STK_Arena);
                break;
            }
            if (callee_node->kind == AK_SymbolRef &&
                sema->node_local_indices[node->a] == sema_no_local() &&
                sema->node_decl_indices[node->a] == sema_no_decl()) {
                u32 trait_symbol =
                    sema_find_trait_with_member(ast, sema, callee_node->a);
                if (trait_symbol != U32_MAX) {
                    return error_0349_unqualified_trait_member_call(
                        lexer->source,
                        sema_node_span(lexer, callee_node),
                        lex_symbol(lexer, callee_node->a),
                        lex_symbol(lexer, trait_symbol));
                }
            }
            const AstNode* field_callee                   = callee_node;
            u32            explicit_method_arg_node_index = U32_MAX;
            if (callee_node->kind == AK_Index) {
                const AstNode* generic_target = &ast->nodes[callee_node->a];
                if (generic_target->kind == AK_Field) {
                    field_callee                   = generic_target;
                    explicit_method_arg_node_index = callee_node->b;
                }
            }
            if (field_callee->kind == AK_Field) {
                u32 associated_target = sema_no_type();
                if (sema_try_resolve_type_symbol(lexer,
                                                 ast,
                                                 sema,
                                                 field_callee->a,
                                                 &associated_target)) {
                    bool                   found_associated = false;
                    SemaResolvedMethodCall associated_call  = {0};
                    if (!sema_try_resolve_associated_call(lexer,
                                                          ast,
                                                          sema,
                                                          node_index,
                                                          associated_target,
                                                          field_callee->b,
                                                          U32_MAX,
                                                          &found_associated,
                                                          &associated_call)) {
                        return false;
                    }
                    if (found_associated) {
                        sema->node_type_indices[node->a] =
                            associated_call.fn_type_index;
                        sema->node_lowered_symbol_handles[node->a] =
                            associated_call.lowered_symbol_handle;
                        type_index = sema->types[associated_call.fn_type_index]
                                         .return_type;
                        break;
                    }
                }

                if (field_callee->a < array_count(ast->nodes) &&
                    ast->nodes[field_callee->a].kind == AK_Index) {
                    const AstNode* trait_target =
                        &ast->nodes[ast->nodes[field_callee->a].a];
                    if (trait_target->kind == AK_SymbolRef) {
                        u32 trait_symbol = trait_target->a;
                        u32 trait_decl   = sema_find_decl(sema, trait_symbol);
                        if (trait_decl != sema_no_decl() &&
                            sema->decls[trait_decl].kind == SK_Trait) {
                            u32 explicit_target_type = sema_no_type();
                            if (!sema_resolve_type_node(
                                    lexer,
                                    ast,
                                    sema,
                                    ast->nodes[field_callee->a].b,
                                    &explicit_target_type)) {
                                return false;
                            }

                            bool                   found_associated = false;
                            SemaResolvedMethodCall associated_call  = {0};
                            if (!sema_try_resolve_associated_call(
                                    lexer,
                                    ast,
                                    sema,
                                    node_index,
                                    explicit_target_type,
                                    field_callee->b,
                                    trait_symbol,
                                    &found_associated,
                                    &associated_call)) {
                                return false;
                            }
                            if (found_associated) {
                                sema->node_type_indices[node->a] =
                                    associated_call.fn_type_index;
                                sema->node_lowered_symbol_handles[node->a] =
                                    associated_call.lowered_symbol_handle;
                                type_index =
                                    sema->types[associated_call.fn_type_index]
                                        .return_type;
                                break;
                            }
                        }
                    }
                }

                if (field_callee->a < array_count(ast->nodes) &&
                    ast->nodes[field_callee->a].kind == AK_SymbolRef) {
                    u32 trait_symbol = ast->nodes[field_callee->a].a;
                    u32 trait_decl   = sema_find_decl(sema, trait_symbol);
                    if (trait_decl != sema_no_decl() &&
                        sema->decls[trait_decl].kind == SK_Trait) {
                        if (call->arg_count == 0) {
                            return error_0313_argument_count_mismatch(
                                lexer->source,
                                sema_node_span(lexer, node),
                                1,
                                0);
                        }
                        u32 receiver_node = ast->call_args[call->first_arg];
                        u32 explicit_receiver_type = sema_no_type();
                        if (!sema_infer_node_type(lexer,
                                                  ast,
                                                  sema,
                                                  receiver_node,
                                                  sema_no_type(),
                                                  &explicit_receiver_type)) {
                            return false;
                        }

                        bool                   found_trait_method = false;
                        SemaResolvedMethodCall trait_method_call  = {0};
                        if (!sema_try_resolve_method_call(
                                lexer,
                                ast,
                                sema,
                                node_index,
                                explicit_receiver_type,
                                field_callee->b,
                                explicit_method_arg_node_index,
                                trait_symbol,
                                1,
                                false,
                                &found_trait_method,
                                &trait_method_call)) {
                            return false;
                        }
                        if (found_trait_method) {
                            sema->node_type_indices[node->a] =
                                trait_method_call.fn_type_index;
                            sema->node_lowered_symbol_handles[node->a] =
                                trait_method_call.lowered_symbol_handle;
                            sema->node_method_call_decl_indices[node_index] =
                                trait_method_call.decl_index;
                            sema->node_method_call_receiver_refs[node_index] =
                                trait_method_call.receiver_ref;
                            sema->node_method_call_receiver_derefs[node_index] =
                                trait_method_call.receiver_deref;
                            sema->node_method_call_explicit_traits[node_index] =
                                true;
                            type_index =
                                sema->types[trait_method_call.fn_type_index]
                                    .return_type;
                            break;
                        }
                    }
                }

                u32 receiver_type = sema_no_type();
                if (!sema_infer_node_type(lexer,
                                          ast,
                                          sema,
                                          field_callee->a,
                                          sema_no_type(),
                                          &receiver_type)) {
                    return false;
                }
                if (receiver_type != sema_no_type() &&
                    sema->types[receiver_type].kind == STK_DynamicArray) {
                    if (!sema_node_can_mutate_dynarray(ast, field_callee->a)) {
                        return error_0305_invalid_assignment_target(
                            lexer->source,
                            sema_node_span(lexer, &ast->nodes[field_callee->a]),
                            s("expression"));
                    }

                    u32 method_type = sema_no_type();
                    if (!sema_dynarray_method_signature(sema,
                                                        lexer,
                                                        receiver_type,
                                                        field_callee->b,
                                                        &method_type)) {
                        return error_0304_type_mismatch(
                            lexer->source,
                            sema_node_span(lexer, field_callee),
                            s("dynamic array method"),
                            lex_symbol(lexer, field_callee->b));
                    }

                    string method = lex_symbol(lexer, field_callee->b);
                    u32 item_type = sema->types[receiver_type].first_param_type;
                    u32 slice_type = sema_add_slice_type(sema, item_type);
                    u32 usize_type = sema_builtin_type(sema, STK_Usize);

                    if (string_eq(method, s("push"))) {
                        if (call->arg_count != 1) {
                            return error_0313_argument_count_mismatch(
                                lexer->source,
                                sema_node_span(lexer, node),
                                1,
                                call->arg_count);
                        }
                        u32 arg_type = sema_no_type();
                        if (!sema_infer_node_type(
                                lexer,
                                ast,
                                sema,
                                ast->call_args[call->first_arg],
                                item_type,
                                &arg_type)) {
                            return false;
                        }
                        if (!sema_type_matches(sema, item_type, arg_type)) {
                            return error_0304_type_mismatch(
                                lexer->source,
                                sema_node_span(
                                    lexer,
                                    &ast->nodes
                                         [ast->call_args[call->first_arg]]),
                                sema_type_name(
                                    lexer, sema, &temp_arena, item_type),
                                sema_type_name(
                                    lexer, sema, &temp_arena, arg_type));
                        }
                    } else if (string_eq(method, s("append"))) {
                        if (call->arg_count != 1) {
                            return error_0313_argument_count_mismatch(
                                lexer->source,
                                sema_node_span(lexer, node),
                                1,
                                call->arg_count);
                        }
                        u32 arg_node = ast->call_args[call->first_arg];
                        u32 arg_type = sema_no_type();
                        if (!sema_infer_node_type(lexer,
                                                  ast,
                                                  sema,
                                                  arg_node,
                                                  slice_type,
                                                  &arg_type)) {
                            return false;
                        }
                        bool ok =
                            sema_type_matches(sema, slice_type, arg_type) ||
                            (arg_type != sema_no_type() &&
                             sema->types[arg_type].kind == STK_DynamicArray &&
                             sema->types[arg_type].first_param_type ==
                                 item_type);
                        if (!ok) {
                            return error_0304_type_mismatch(
                                lexer->source,
                                sema_node_span(lexer, &ast->nodes[arg_node]),
                                s("slice or dynamic array with matching item "
                                  "type"),
                                sema_type_name(
                                    lexer, sema, &temp_arena, arg_type));
                        }
                    } else if (string_eq(method, s("reserve_to")) ||
                               string_eq(method, s("reserve_extra")) ||
                               string_eq(method, s("resize_to")) ||
                               string_eq(method, s("resize_undefined_to")) ||
                               string_eq(method, s("extend")) ||
                               string_eq(method, s("extend_undefined")) ||
                               string_eq(method, s("delete")) ||
                               string_eq(method, s("swap_delete"))) {
                        if (call->arg_count != 1) {
                            return error_0313_argument_count_mismatch(
                                lexer->source,
                                sema_node_span(lexer, node),
                                1,
                                call->arg_count);
                        }
                        u32 arg_type = sema_no_type();
                        if (!sema_infer_node_type(
                                lexer,
                                ast,
                                sema,
                                ast->call_args[call->first_arg],
                                usize_type,
                                &arg_type)) {
                            return false;
                        }
                        if (!sema_type_is_integer(sema, arg_type)) {
                            return error_0304_type_mismatch(
                                lexer->source,
                                sema_node_span(
                                    lexer,
                                    &ast->nodes
                                         [ast->call_args[call->first_arg]]),
                                s("integer"),
                                sema_type_name(
                                    lexer, sema, &temp_arena, arg_type));
                        }
                    } else if (string_eq(method, s("pop"))) {
                        if (call->arg_count != 0) {
                            return error_0313_argument_count_mismatch(
                                lexer->source,
                                sema_node_span(lexer, node),
                                0,
                                call->arg_count);
                        }
                    } else if (string_eq(method, s("clear")) ||
                               string_eq(method, s("free"))) {
                        if (call->arg_count != 0) {
                            return error_0313_argument_count_mismatch(
                                lexer->source,
                                sema_node_span(lexer, node),
                                0,
                                call->arg_count);
                        }
                    }

                    sema->node_type_indices[node->a] = method_type;
                    type_index = string_eq(method, s("pop"))
                                     ? item_type
                                     : sema_builtin_type(sema, STK_Void);
                    break;
                }

                if (receiver_type != sema_no_type() &&
                    sema->types[receiver_type].kind == STK_Box) {
                    if (!sema_node_can_mutate_dynarray(ast, field_callee->a)) {
                        return error_0305_invalid_assignment_target(
                            lexer->source,
                            sema_node_span(lexer, &ast->nodes[field_callee->a]),
                            s("expression"));
                    }

                    u32 method_type = sema_no_type();
                    if (!sema_box_method_signature(sema,
                                                   lexer,
                                                   receiver_type,
                                                   field_callee->b,
                                                   &method_type)) {
                        return error_0304_type_mismatch(
                            lexer->source,
                            sema_node_span(lexer, field_callee),
                            s("box method"),
                            lex_symbol(lexer, field_callee->b));
                    }
                    if (call->arg_count != 0) {
                        return error_0313_argument_count_mismatch(
                            lexer->source,
                            sema_node_span(lexer, node),
                            0,
                            call->arg_count);
                    }
                    sema->node_type_indices[node->a] = method_type;
                    type_index = sema_builtin_type(sema, STK_Void);
                    break;
                }

                bool                   found_method = false;
                SemaResolvedMethodCall method_call  = {0};
                if (!sema_try_resolve_method_call(
                        lexer,
                        ast,
                        sema,
                        node_index,
                        receiver_type,
                        field_callee->b,
                        explicit_method_arg_node_index,
                        U32_MAX,
                        0,
                        false,
                        &found_method,
                        &method_call)) {
                    return false;
                }
                if (found_method) {
                    sema->node_type_indices[node->a] =
                        method_call.fn_type_index;
                    sema->node_lowered_symbol_handles[node->a] =
                        method_call.lowered_symbol_handle;
                    sema->node_method_call_decl_indices[node_index] =
                        method_call.decl_index;
                    sema->node_method_call_receiver_refs[node_index] =
                        method_call.receiver_ref;
                    sema->node_method_call_receiver_derefs[node_index] =
                        method_call.receiver_deref;
                    type_index =
                        sema->types[method_call.fn_type_index].return_type;
                    break;
                }
                u32 member_target_type =
                    sema_member_target_type(sema, receiver_type);
                u32 member_type = sema_no_type();
                if (member_target_type != sema_no_type() &&
                    member_target_type < array_count(sema->types) &&
                    (sema->types[member_target_type].kind == STK_Plex ||
                     sema->types[member_target_type].kind == STK_Union) &&
                    !sema_record_member_type(sema,
                                             member_target_type,
                                             field_callee->b,
                                             &member_type)) {
                    return sema_error_unknown_member(lexer,
                                                     ast,
                                                     sema,
                                                     field_callee,
                                                     receiver_type,
                                                     field_callee->b);
                }
            }
            u32 enum_context = sema_no_type();
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

            {
                const AstNode* generic_callee = &ast->nodes[node->a];
                u32            generic_decl   = sema_no_decl();
                Array(u32) explicit_arg_nodes = NULL;

                if (generic_callee->kind == AK_SymbolRef) {
                    u32 decl_index = sema->node_decl_indices[node->a];
                    if (decl_index != sema_no_decl() &&
                        sema->decls[decl_index].kind == SK_GenericFunction) {
                        generic_decl = decl_index;
                    }
                } else if (generic_callee->kind == AK_Index) {
                    const AstNode* target = &ast->nodes[generic_callee->a];
                    if (target->kind == AK_SymbolRef) {
                        u32 decl_index =
                            sema->node_decl_indices[generic_callee->a];
                        if (decl_index != sema_no_decl() &&
                            sema->decls[decl_index].kind ==
                                SK_GenericFunction) {
                            generic_decl = decl_index;
                            sema_collect_generic_arg_nodes(
                                ast, generic_callee->b, &explicit_arg_nodes);
                        }
                    }
                }

                if (generic_decl != sema_no_decl()) {
                    u32 symbol        = U32_MAX;
                    u32 fn_type_index = sema_no_type();
                    if (!sema_instantiate_generic_function(
                            lexer,
                            ast,
                            sema,
                            generic_decl,
                            explicit_arg_nodes,
                            (u32)array_count(explicit_arg_nodes),
                            call,
                            expected_type,
                            &symbol,
                            &fn_type_index)) {
                        array_free(explicit_arg_nodes);
                        return false;
                    }
                    sema->node_type_indices[node->a]           = fn_type_index;
                    sema->node_lowered_symbol_handles[node->a] = symbol;
                    type_index = sema->types[fn_type_index].return_type;
                    array_free(explicit_arg_nodes);
                    break;
                }
            }

            if (callee_node->kind == AK_SymbolRef) {
                u32 decl_index = sema->node_decl_indices[node->a];
                if (decl_index != sema_no_decl()) {
                    SemaDecl* decl = &sema->decls[decl_index];
                    if (decl->kind == SK_Function &&
                        decl->type_index == sema_no_type() &&
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

            u32 fn_first_param_type = sema->types[callee_type].first_param_type;
            u32 fn_param_count      = sema->types[callee_type].param_count;
            u32 fn_return_type      = sema->types[callee_type].return_type;
            u32 fn_flags            = sema->types[callee_type].flags;
            bool is_varargs         = (fn_flags & STF_FunctionVarargs) != 0;
            SemaKnownCallSignature known_signature = {0};
            u32                    required_count  = fn_param_count;
            if (!is_varargs &&
                sema_known_call_signature(
                    lexer, ast, sema, node->a, &known_signature)) {
                required_count = sema_signature_required_param_count(
                    known_signature.ast, known_signature.signature);
            }

            bool wrong_arity = is_varargs ? call->arg_count < fn_param_count
                                          : (call->arg_count < required_count ||
                                             call->arg_count > fn_param_count);
            if (wrong_arity) {
                u32 expected_count = call->arg_count < required_count
                                         ? required_count
                                         : fn_param_count;
                return error_0313_argument_count_mismatch(
                    lexer->source,
                    sema_node_span(lexer, node),
                    expected_count,
                    call->arg_count);
            }

            for (u32 i = 0; i < call->arg_count; ++i) {
                u32 arg_node = ast->call_args[call->first_arg + i];
                u32 expected_arg =
                    i < fn_param_count
                        ? sema->type_param_types[fn_first_param_type + i]
                        : sema_no_type();
                const AstParam* expected_param =
                    known_signature.signature != NULL &&
                            i < known_signature.signature->param_count
                        ? &known_signature.ast
                               ->params[known_signature.signature->first_param +
                                        i]
                        : NULL;
                if (!sema_call_arg_value_node(lexer,
                                              ast,
                                              known_signature.signature != NULL
                                                  ? known_signature.lexer
                                                  : lexer,
                                              expected_param,
                                              arg_node,
                                              &arg_node)) {
                    return false;
                }
                u32 arg_type = sema_no_type();
                if (!sema_infer_node_type(
                        lexer, ast, sema, arg_node, expected_arg, &arg_type)) {
                    return false;
                }
                if (i >= fn_param_count) {
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

            if (known_signature.signature != NULL) {
                for (u32 i = call->arg_count; i < fn_param_count; ++i) {
                    const AstParam* param =
                        &known_signature.ast
                             ->params[known_signature.signature->first_param +
                                      i];
                    ASSERT(param->default_node_index != U32_MAX,
                           "Expected omitted parameter to have a default");
                    u32 expected_arg =
                        sema->type_param_types[fn_first_param_type + i];
                    u32 arg_type = sema_no_type();
                    if (known_signature.imported) {
                        u32 expected_source =
                            sema_import_type((Lexer*)known_signature.lexer,
                                             known_signature.sema,
                                             lexer,
                                             sema,
                                             expected_arg);
                        if (!sema_infer_node_type(known_signature.lexer,
                                                  known_signature.ast,
                                                  known_signature.sema,
                                                  param->default_node_index,
                                                  expected_source,
                                                  &arg_type)) {
                            return false;
                        }
                        if (!sema_type_matches(known_signature.sema,
                                               expected_source,
                                               arg_type)) {
                            return error_0304_type_mismatch(
                                known_signature.lexer->source,
                                sema_node_span(
                                    known_signature.lexer,
                                    &known_signature.ast
                                         ->nodes[param->default_node_index]),
                                sema_type_name(known_signature.lexer,
                                               known_signature.sema,
                                               &temp_arena,
                                               expected_source),
                                sema_type_name(known_signature.lexer,
                                               known_signature.sema,
                                               &temp_arena,
                                               arg_type));
                        }
                    } else {
                        if (!sema_infer_node_type(lexer,
                                                  ast,
                                                  sema,
                                                  param->default_node_index,
                                                  expected_arg,
                                                  &arg_type)) {
                            return false;
                        }
                        if (!sema_type_matches(sema, expected_arg, arg_type)) {
                            return error_0304_type_mismatch(
                                lexer->source,
                                sema_node_span(
                                    lexer,
                                    &ast->nodes[param->default_node_index]),
                                sema_type_name(
                                    lexer, sema, &temp_arena, expected_arg),
                                sema_type_name(
                                    lexer, sema, &temp_arena, arg_type));
                        }
                    }
                }
            }

            type_index = fn_return_type;
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
                sema_select_default_method_for_local(lexer, ast, sema, local);
                type_index = annotated;
                break;
            }

            if (!sema_infer_node_type(lexer,
                                      ast,
                                      sema,
                                      local->value_node_index,
                                      annotated,
                                      &type_index)) {
                return false;
            }

            local      = &sema->locals[local_index];
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
            u32            target_type = sema_no_type();
            const AstNode* target      = &ast->nodes[node->a];
            if (target->kind == AK_SymbolRef) {
                if (sema->node_local_indices[node->a] != sema_no_local()) {
                    target_type =
                        sema->locals[sema->node_local_indices[node->a]]
                            .type_index;
                } else {
                    u32 decl_index = sema->node_decl_indices[node->a];
                    ASSERT(decl_index != sema_no_decl(),
                           "Expected resolved target");
                    if (sema->decls[decl_index].kind != SK_Variable) {
                        return error_0305_invalid_assignment_target(
                            lexer->source,
                            sema_node_span(lexer, target),
                            lex_symbol(lexer, target->a));
                    }
                    target_type = sema->decls[decl_index].type_index;
                }
            } else if (target->kind == AK_Deref) {
                if (!sema_infer_node_type(lexer,
                                          ast,
                                          sema,
                                          node->a,
                                          sema_no_type(),
                                          &target_type)) {
                    return false;
                }
            } else if (target->kind == AK_Field) {
                u32 base_type = sema_no_type();
                if (!sema_infer_node_type(lexer,
                                          ast,
                                          sema,
                                          target->a,
                                          sema_no_type(),
                                          &base_type)) {
                    return false;
                }
                u32 record_type = base_type;
                if (base_type != sema_no_type() &&
                    sema->types[base_type].kind == STK_Pointer) {
                    u32 pointee_type = sema->types[base_type].first_param_type;
                    if (sema->types[pointee_type].kind == STK_Plex ||
                        sema->types[pointee_type].kind == STK_Union) {
                        record_type = pointee_type;
                    }
                } else if (base_type != sema_no_type() &&
                           sema->types[base_type].kind == STK_Box) {
                    u32 item_type = sema->types[base_type].first_param_type;
                    if (sema->types[item_type].kind == STK_Plex ||
                        sema->types[item_type].kind == STK_Union) {
                        record_type = item_type;
                    }
                }
                if (record_type == sema_no_type() ||
                    (sema->types[record_type].kind != STK_Plex &&
                     sema->types[record_type].kind != STK_Union)) {
                    return error_0305_invalid_assignment_target(
                        lexer->source,
                        sema_node_span(lexer, target),
                        s("expression"));
                }
                if (!sema_infer_node_type(lexer,
                                          ast,
                                          sema,
                                          node->a,
                                          sema_no_type(),
                                          &target_type)) {
                    return false;
                }
            } else if (target->kind == AK_Index) {
                if (!sema_infer_node_type(lexer,
                                          ast,
                                          sema,
                                          node->a,
                                          sema_no_type(),
                                          &target_type)) {
                    return false;
                }
            } else {
                return error_0305_invalid_assignment_target(
                    lexer->source,
                    sema_node_span(lexer, target),
                    s("expression"));
            }

            u32 param_local     = sema_no_local();
            u32 param_root_node = U32_MAX;
            if (sema_assignment_target_writes_param_storage(
                    ast, sema, node->a, &param_local, &param_root_node)) {
                const SemaLocal* local = &sema->locals[param_local];
                return error_0305_invalid_assignment_target(
                    lexer->source,
                    sema_node_span(lexer, &ast->nodes[param_root_node]),
                    lex_symbol(lexer, local->symbol_handle));
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
                sema, node->b == AFK_Block ? STK_Void : STK_UntypedInteger);
            bool is_unannotated_main = false;
            if (!has_explicit_return_type) {
                u32 main_symbol =
                    sema_find_symbol_handle_by_name(lexer, s("main"));
                for (u32 i = 0; i < array_count(sema->decls); ++i) {
                    const SemaDecl* decl = &sema->decls[i];
                    if (decl->symbol_handle == main_symbol &&
                        decl->value_node_index == node_index) {
                        is_unannotated_main = true;
                        break;
                    }
                }
            }

            if (has_explicit_return_type &&
                !sema_resolve_type_node(lexer,
                                        ast,
                                        sema,
                                        signature->return_type_node_index,
                                        &return_type)) {
                return false;
            }
            u32 declared_return_type = return_type;

            if (node->b == AFK_Expr) {
                u32 expected_return =
                    has_explicit_return_type ? return_type : sema_no_type();
                if (!sema_infer_node_type(lexer,
                                          ast,
                                          sema,
                                          fn_start->b - 1,
                                          expected_return,
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
                                                 has_explicit_return_type ||
                                                         !is_unannotated_main
                                                     ? return_type
                                                     : sema_no_type(),
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
                if (has_explicit_return_type) {
                    return_type = declared_return_type;
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
            if (ffi_info->library_node_index != U32_MAX) {
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
                        sema_node_span(
                            lexer, &ast->nodes[ffi_info->library_node_index]),
                        sema_type_name(lexer, sema, &temp_arena, string_type),
                        sema_type_name(lexer, sema, &temp_arena, library_type));
                }
                if (!sema_expr_is_constantish(
                        ast, sema, ffi_info->library_node_index)) {
                    return error_0304_type_mismatch(
                        lexer->source,
                        sema_node_span(
                            lexer, &ast->nodes[ffi_info->library_node_index]),
                        s("compile-time string"),
                        sema_type_name(lexer, sema, &temp_arena, library_type));
                }
                string library = {0};
                if (sema_ffi_library_literal(
                        lexer, ast, ffi_info->library_node_index, &library) &&
                    sema_windows_library_should_validate(library)) {
                    string foreign_symbol =
                        lex_symbol(lexer, ffi_info->foreign_symbol_handle);
                    if (!sema_windows_ffi_symbol_exists(
                            &temp_arena, library, foreign_symbol)) {
                        return error_0346_unknown_ffi_symbol(
                            lexer->source,
                            sema_token_span(
                                lexer, ffi_info->foreign_symbol_token_index),
                            foreign_symbol,
                            library);
                    }
                }
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
                const AstParam* param =
                    &ast->params[signature->first_param + i];
                if (param->default_node_index != U32_MAX) {
                    array_free(param_types);
                    string symbol =
                        param->symbol_handle == U32_MAX
                            ? s("<unnamed>")
                            : lex_symbol(lexer, param->symbol_handle);
                    return error_0337_default_param_on_ffi(
                        lexer->source,
                        sema_token_span(lexer, param->token_index),
                        symbol);
                }
                u32 param_type_node = param->type_node_index;
                u32 param_type      = sema_no_type();
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
        if (sema_type_is_pointer_integer_mismatch(
                sema, expected_type, type_index)) {
            return error_0304_integer_used_as_pointer(
                lexer->source,
                sema_node_span(lexer, node),
                sema_type_name(lexer, sema, &temp_arena, expected_type),
                sema_type_name(lexer, sema, &temp_arena, type_index));
        }
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
        u32       decl_index    = sema->ordered_decl_indices[i];
        SemaDecl* decl          = &sema->decls[decl_index];
        u32       annotated     = sema_no_type();
        u32       inferred_type = sema_no_type();

        if (decl->kind == SK_TypeAlias || decl->kind == SK_GenericTypeAlias ||
            decl->kind == SK_GenericFunction || decl->kind == SK_Trait) {
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

        if (annotated != sema_no_type()) {
            decl->type_index = annotated;
        }

        if (decl->value_node_index != sema_no_decl()) {
            if (decl->kind != SK_Function && decl->kind != SK_FfiFunction &&
                decl->kind != SK_Module) {
                u32 interp_index = sema_find_runtime_interpolated_string_node(
                    ast, sema, decl->value_node_index);
                if (interp_index != sema_no_decl()) {
                    return error_0310_invalid_interpolation_context(
                        lexer->source,
                        sema_node_span(lexer, &ast->nodes[interp_index]));
                }
            }

            if (!sema_infer_node_type(lexer,
                                      ast,
                                      sema,
                                      decl->value_node_index,
                                      annotated,
                                      &inferred_type)) {
                return false;
            }
        }

        // Recursive inference can grow sema->decls through module export
        // import. Reacquire the declaration pointer before writing back
        // inferred types.
        decl = &sema->decls[decl_index];

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

        if (decl->kind == SK_Module && decl->type_index != sema_no_type() &&
            sema->types[decl->type_index].kind == STK_Module &&
            sema->program != NULL) {
            u32 module_index = sema->types[decl->type_index].return_type;
            if (module_index < array_count(sema->program->modules)) {
                sema_import_public_methods_from_module(
                    (Lexer*)lexer,
                    sema,
                    &sema->program->modules[module_index],
                    module_index);
            }
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

    case AK_NilLiteral:
        value = 0;
        ok    = true;
        break;

    case AK_StringLiteral:
    case AK_InterpPartExpr:
    case AK_InterpolatedString:
    case AK_BuiltinMacro:
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
    case AK_BitwiseNot:
        ok = sema_try_get_constant(ast, out_sema, node->a, &value);
        if (ok) {
            value = node->kind == AK_BitwiseNot ? ~value : -value;
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
    case AK_ShiftLeft:
    case AK_ShiftRight:
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
            case AK_BitwiseAnd:
                value = lhs & rhs;
                break;
            case AK_BitwiseXor:
                value = lhs ^ rhs;
                break;
            case AK_BitwiseOr:
                value = lhs | rhs;
                break;
            case AK_ShiftLeft:
                if (rhs < 0 || rhs >= 64) {
                    ok = false;
                    break;
                }
                value = lhs << rhs;
                break;
            case AK_ShiftRight:
                if (rhs < 0 || rhs >= 64) {
                    ok = false;
                    break;
                }
                value = lhs >> rhs;
                break;
            default:
                ok = false;
                break;
            }
        }
        break;

    case AK_Field:
        ok = sema_try_eval_integer_constant(lex, ast, sema, node_index, &value);
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
                        if (local->kind == SLK_Constant &&
                            local->value_node_index != sema_no_decl()) {
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
                    if (decl->kind == SK_Constant &&
                        decl->value_node_index != sema_no_decl()) {
                        sema_push_fold_frame(&stack, decl->value_node_index);
                    }
                }
                break;

            case AK_Expression:
            case AK_Statement:
            case AK_IntegerNegate:
            case AK_BitwiseNot:
            case AK_InterpPartExpr:
                sema_push_fold_frame(&stack, node->a);
                break;
            case AK_Cast:
                {
                    const AstCastInfo* cast = sema_cast_info(ast, node);
                    if (cast->extra_node_index != U32_MAX) {
                        sema_push_fold_frame(&stack, cast->extra_node_index);
                    }
                    sema_push_fold_frame(&stack, node->a);
                }
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
            case AK_BitwiseAnd:
            case AK_BitwiseXor:
            case AK_BitwiseOr:
            case AK_ShiftLeft:
            case AK_ShiftRight:
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

//------------------------------------------------------------------------------
// Definite assignment validation for function-local mutable storage.

typedef struct {
    Array(bool) assigned;
    bool reachable;
} SemaAssignState;

internal SemaAssignState sema_assign_state_copy(const SemaAssignState* state)
{
    SemaAssignState copy = {.reachable = state->reachable};
    for (u32 i = 0; i < array_count(state->assigned); ++i) {
        array_push(copy.assigned, state->assigned[i]);
    }
    return copy;
}

internal void sema_assign_state_done(SemaAssignState* state)
{
    array_free(state->assigned);
    state->assigned  = NULL;
    state->reachable = false;
}

internal void sema_assign_state_merge_reachable(SemaAssignState*       out,
                                                const SemaAssignState* branch,
                                                bool*                  any)
{
    if (!branch->reachable) {
        return;
    }
    if (!*any) {
        for (u32 i = 0; i < array_count(out->assigned); ++i) {
            out->assigned[i] = branch->assigned[i];
        }
        out->reachable = true;
        *any           = true;
        return;
    }
    for (u32 i = 0; i < array_count(out->assigned); ++i) {
        out->assigned[i] = out->assigned[i] && branch->assigned[i];
    }
}

internal bool sema_local_starts_unassigned(const Ast*       ast,
                                           const SemaLocal* local)
{
    if (local->kind != SLK_Variable ||
        local->decl_node_index == sema_no_decl() ||
        local->decl_node_index >= array_count(ast->nodes)) {
        return false;
    }

    const AstNode* decl = &ast->nodes[local->decl_node_index];
    if (decl->kind != AK_Variable && decl->kind != AK_DestructureVariable) {
        return false;
    }

    const AstNode* payload = &ast->nodes[decl->b];
    if (payload->kind == AK_AnnotatedValue) {
        payload = &ast->nodes[payload->b];
    }
    return payload->kind == AK_Undefined;
}

internal bool sema_assignment_on_covers_all_bool_values(const Ast* ast,
                                                        u32        on_index)
{
    const AstOnInfo* on        = &ast->ons[on_index];
    bool             has_true  = false;
    bool             has_false = false;

    for (u32 branch_index = 0; branch_index < on->branch_count;
         ++branch_index) {
        const AstOnBranch* branch =
            &ast->on_branches[on->first_branch + branch_index];
        if ((branch->flags & AOBF_Else) ||
            branch->guard_node_index != U32_MAX) {
            continue;
        }

        for (u32 pattern = 0; pattern < branch->pattern_count; ++pattern) {
            const AstPattern* ast_pattern =
                &ast->patterns[ast->pattern_items[branch->pattern_index +
                                                  pattern]];
            if (ast_pattern->kind != APK_Value ||
                ast_pattern->a >= array_count(ast->nodes)) {
                continue;
            }

            const AstNode* pattern_node = &ast->nodes[ast_pattern->a];
            if (pattern_node->kind != AK_BoolLiteral) {
                continue;
            }
            if (pattern_node->a) {
                has_true = true;
            } else {
                has_false = true;
            }
        }
    }

    return has_true && has_false;
}

internal bool sema_validate_assignment_node(const Lexer*     lexer,
                                            const Ast*       ast,
                                            const Sema*      sema,
                                            u32              node_index,
                                            SemaAssignState* state);

internal bool sema_validate_assignment_pattern(const Lexer*     lexer,
                                               const Ast*       ast,
                                               const Sema*      sema,
                                               u32              pattern_index,
                                               SemaAssignState* state,
                                               bool             mark_assigned)
{
    const AstPattern* pattern = &ast->patterns[pattern_index];
    u32 local_index = pattern_index < array_count(sema->pattern_local_indices)
                          ? sema->pattern_local_indices[pattern_index]
                          : sema_no_local();
    if (local_index != sema_no_local() &&
        local_index < array_count(state->assigned)) {
        if (mark_assigned) {
            state->assigned[local_index] = true;
        }
        return true;
    }

    switch (pattern->kind) {
    case APK_Value:
    case APK_ForValue:
    case APK_Equal:
    case APK_NotEqual:
    case APK_Less:
    case APK_LessEqual:
    case APK_Greater:
    case APK_GreaterEqual:
        return sema_validate_assignment_node(
            lexer, ast, sema, pattern->a, state);
    case APK_RangeExclusive:
    case APK_RangeInclusive:
        return sema_validate_assignment_node(
                   lexer, ast, sema, pattern->a, state) &&
               sema_validate_assignment_node(
                   lexer, ast, sema, pattern->b, state);
    case APK_Bind:
        return pattern->b == U32_MAX ||
               sema_validate_assignment_pattern(
                   lexer, ast, sema, pattern->b, state, mark_assigned);
    case APK_Tuple:
        for (u32 i = 0; i < pattern->b; ++i) {
            if (!sema_validate_assignment_pattern(
                    lexer,
                    ast,
                    sema,
                    ast->pattern_items[pattern->a + i],
                    state,
                    mark_assigned)) {
                return false;
            }
        }
        return true;
    case APK_Plex:
        for (u32 i = 0; i < pattern->b; ++i) {
            if (!sema_validate_assignment_pattern(
                    lexer,
                    ast,
                    sema,
                    ast->pattern_fields[pattern->a + i].pattern_index,
                    state,
                    mark_assigned)) {
                return false;
            }
        }
        return true;
    case APK_EnumVariant:
        {
            const AstEnumPattern* enum_pattern =
                &ast->enum_patterns[pattern->a];
            for (u32 i = 0; i < enum_pattern->pattern_count; ++i) {
                if (!sema_validate_assignment_pattern(
                        lexer,
                        ast,
                        sema,
                        ast->pattern_items[enum_pattern->first_pattern + i],
                        state,
                        mark_assigned)) {
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

internal bool sema_validate_assignment_read(const Lexer*     lexer,
                                            const Ast*       ast,
                                            const Sema*      sema,
                                            u32              node_index,
                                            SemaAssignState* state)
{
    u32 local_index = node_index < array_count(sema->node_local_indices)
                          ? sema->node_local_indices[node_index]
                          : sema_no_local();
    if (local_index == sema_no_local() ||
        local_index >= array_count(state->assigned) ||
        state->assigned[local_index]) {
        return true;
    }

    const SemaLocal* local = &sema->locals[local_index];
    if (!sema_local_is_runtime_value(local)) {
        return true;
    }

    return error_0334_read_before_assignment(
        lexer->source,
        sema_node_span(lexer, &ast->nodes[node_index]),
        lex_symbol(lexer, local->symbol_handle),
        sema_local_span(lexer, ast, local));
}

internal bool sema_validate_assignment_block(const Lexer*     lexer,
                                             const Ast*       ast,
                                             const Sema*      sema,
                                             u32              first_node,
                                             u32              end_node,
                                             SemaAssignState* state)
{
    for (u32 i = first_node; i < end_node && state->reachable; ++i) {
        if (!ast_node_is_block_statement(&ast->nodes[i])) {
            continue;
        }
        if (!sema_validate_assignment_node(lexer, ast, sema, i, state)) {
            return false;
        }
        i = ast_block_statement_end_exclusive(ast, i) - 1;
    }
    return true;
}

internal bool sema_validate_assignment_node(const Lexer*     lexer,
                                            const Ast*       ast,
                                            const Sema*      sema,
                                            u32              node_index,
                                            SemaAssignState* state)
{
    if (!state->reachable || node_index >= array_count(ast->nodes)) {
        return true;
    }

    const AstNode* node = &ast->nodes[node_index];
    switch (node->kind) {
    case AK_IntegerLiteral:
    case AK_FloatLiteral:
    case AK_StringLiteral:
    case AK_BuiltinMacro:
    case AK_BoolLiteral:
    case AK_NilLiteral:
    case AK_EnumVariant:
    case AK_ZeroInit:
    case AK_Undefined:
    case AK_FnDef:
    case AK_FnStart:
    case AK_FnEnd:
    case AK_FfiDef:
    case AK_ModRef:
    case AK_Use:
    case AK_Impl:
    case AK_Trait:
    case AK_Pragma:
    case AK_TypeNever:
    case AK_TypeOptional:
    case AK_TypeResult:
        return true;

    case AK_SymbolRef:
        if (node_index < array_count(sema->node_is_type_expr) &&
            sema->node_is_type_expr[node_index]) {
            return true;
        }
        return sema_validate_assignment_read(
            lexer, ast, sema, node_index, state);

    case AK_Expression:
    case AK_Statement:
    case AK_InterpPartExpr:
    case AK_IntegerNegate:
    case AK_LogicalNot:
    case AK_ErrorInject:
    case AK_Propagate:
    case AK_BitwiseNot:
    case AK_Deref:
        return sema_validate_assignment_node(lexer, ast, sema, node->a, state);

    case AK_AddressOf:
        {
            const AstNode* target = &ast->nodes[node->a];
            if (target->kind == AK_SymbolRef) {
                return true;
            }
            return sema_validate_assignment_node(
                lexer, ast, sema, node->a, state);
        }

    case AK_Field:
        if (string_eq(lex_symbol(lexer, node->b), s("size")) ||
            string_eq(lex_symbol(lexer, node->b), s("bytes"))) {
            return true;
        }
        return sema_validate_assignment_node(lexer, ast, sema, node->a, state);

    case AK_TupleField:
        return sema_validate_assignment_node(lexer, ast, sema, node->a, state);

    case AK_Index:
    case AK_TypeArray:
    case AK_StringConcat:
    case AK_IntegerPlus:
    case AK_IntegerMinus:
    case AK_IntegerMultiply:
    case AK_IntegerDivide:
    case AK_IntegerModulo:
    case AK_BitwiseAnd:
    case AK_BitwiseXor:
    case AK_BitwiseOr:
    case AK_ShiftLeft:
    case AK_ShiftRight:
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
        return sema_validate_assignment_node(
                   lexer, ast, sema, node->a, state) &&
               sema_validate_assignment_node(lexer, ast, sema, node->b, state);

    case AK_Cast:
        {
            const AstCastInfo* cast = sema_cast_info(ast, node);
            return sema_validate_assignment_node(
                       lexer, ast, sema, node->a, state) &&
                   (cast->extra_node_index == U32_MAX ||
                    sema_validate_assignment_node(
                        lexer, ast, sema, cast->extra_node_index, state));
        }

    case AK_Call:
        {
            if (!sema_validate_assignment_node(
                    lexer, ast, sema, node->a, state)) {
                return false;
            }
            const AstCallInfo* call = &ast->calls[node->b];
            for (u32 i = 0; i < call->arg_count; ++i) {
                u32 arg_node = ast->call_args[call->first_arg + i];
                if (ast->nodes[arg_node].kind == AK_Assign) {
                    arg_node = ast->nodes[arg_node].b;
                }
                if (!sema_validate_assignment_node(
                        lexer, ast, sema, arg_node, state)) {
                    return false;
                }
            }
            return true;
        }

    case AK_TypeApply:
        {
            const AstTypeApplyInfo* apply = &ast->type_applications[node->a];
            if (!sema_validate_assignment_node(
                    lexer, ast, sema, apply->target_node_index, state)) {
                return false;
            }
            for (u32 i = 0; i < apply->arg_count; ++i) {
                if (!sema_validate_assignment_node(
                        lexer,
                        ast,
                        sema,
                        ast->tuple_items[apply->first_arg + i],
                        state)) {
                    return false;
                }
            }
            return true;
        }

    case AK_Tuple:
    case AK_Array:
        for (u32 i = 0; i < node->b; ++i) {
            if (!sema_validate_assignment_node(
                    lexer, ast, sema, ast->tuple_items[node->a + i], state)) {
                return false;
            }
        }
        return true;

    case AK_Plex:
    case AK_PlexUpdate:
        {
            const AstPlexLiteralInfo* literal = &ast->plex_literals[node->a];
            if (node->kind == AK_PlexUpdate &&
                !sema_validate_assignment_node(
                    lexer, ast, sema, literal->target_node_index, state)) {
                return false;
            }
            for (u32 i = 0; i < literal->field_count; ++i) {
                if (!sema_validate_assignment_node(
                        lexer,
                        ast,
                        sema,
                        ast->plex_literal_fields[literal->first_field + i]
                            .value_node_index,
                        state)) {
                    return false;
                }
            }
            return true;
        }

    case AK_Slice:
        {
            const AstSliceInfo* slice = &ast->slices[node->a];
            if (!sema_validate_assignment_node(
                    lexer, ast, sema, slice->target_node_index, state)) {
                return false;
            }
            if (slice->start_node_index != U32_MAX &&
                !sema_validate_assignment_node(
                    lexer, ast, sema, slice->start_node_index, state)) {
                return false;
            }
            return slice->end_node_index == U32_MAX ||
                   sema_validate_assignment_node(
                       lexer, ast, sema, slice->end_node_index, state);
        }

    case AK_InterpolatedString:
        for (u32 i = node->a; i < node->b; ++i) {
            if (!sema_validate_assignment_node(lexer, ast, sema, i, state)) {
                return false;
            }
        }
        return true;

    case AK_Variable:
        {
            u32 local_index = sema->node_local_indices[node_index];
            if (local_index == sema_no_local()) {
                return true;
            }
            const SemaLocal* local = &sema->locals[local_index];
            if (local->value_node_index != sema_no_decl() &&
                !sema_validate_assignment_node(
                    lexer, ast, sema, local->value_node_index, state)) {
                return false;
            }
            state->assigned[local_index] =
                !sema_local_starts_unassigned(ast, local);
            return true;
        }

    case AK_DestructureBind:
    case AK_DestructureVariable:
        if (!sema_validate_assignment_node(lexer, ast, sema, node->b, state)) {
            return false;
        }
        return sema_validate_assignment_pattern(lexer,
                                                ast,
                                                sema,
                                                node->a,
                                                state,
                                                node->kind ==
                                                    AK_DestructureVariable);

    case AK_DestructureAssign:
        if (!sema_validate_assignment_node(lexer, ast, sema, node->b, state)) {
            return false;
        }
        return sema_validate_assignment_pattern(
            lexer, ast, sema, node->a, state, true);

    case AK_Assign:
        {
            if (!sema_validate_assignment_node(
                    lexer, ast, sema, node->b, state)) {
                return false;
            }
            const AstNode* target = &ast->nodes[node->a];
            if (target->kind == AK_SymbolRef) {
                u32 local_index = sema->node_local_indices[node_index];
                if (local_index != sema_no_local() &&
                    local_index < array_count(state->assigned)) {
                    state->assigned[local_index] = true;
                }
                return true;
            }
            return sema_validate_assignment_node(
                lexer, ast, sema, node->a, state);
        }

    case AK_Block:
        return sema_validate_assignment_block(
            lexer, ast, sema, node->a, node->b, state);

    case AK_ExprBlock:
        {
            const AstNode* block = &ast->nodes[node->a];
            ASSERT(block->kind == AK_Block, "Expected expression block");
            return sema_validate_assignment_block(
                lexer, ast, sema, block->a, block->b, state);
        }

    case AK_On:
        {
            const AstOnInfo* on = &ast->ons[node->b];
            if (node->a != U32_MAX && !sema_validate_assignment_node(
                                          lexer, ast, sema, node->a, state)) {
                return false;
            }

            SemaAssignState incoming = sema_assign_state_copy(state);
            SemaAssignState merged   = sema_assign_state_copy(state);
            merged.reachable         = false;
            bool any_reachable       = false;
            bool has_else            = false;

            for (u32 i = 0; i < on->branch_count; ++i) {
                const AstOnBranch* branch =
                    &ast->on_branches[on->first_branch + i];
                if (branch->flags & AOBF_Else) {
                    has_else = true;
                }

                SemaAssignState branch_state = sema_assign_state_copy(state);
                if (!(branch->flags & AOBF_Else)) {
                    for (u32 pattern = 0; pattern < branch->pattern_count;
                         ++pattern) {
                        if (!sema_validate_assignment_pattern(
                                lexer,
                                ast,
                                sema,
                                ast->pattern_items[branch->pattern_index +
                                                   pattern],
                                &branch_state,
                                true)) {
                            sema_assign_state_done(&branch_state);
                            sema_assign_state_done(&incoming);
                            sema_assign_state_done(&merged);
                            return false;
                        }
                    }
                }

                if (branch->guard_node_index != U32_MAX &&
                    !sema_validate_assignment_node(lexer,
                                                   ast,
                                                   sema,
                                                   branch->guard_node_index,
                                                   &branch_state)) {
                    sema_assign_state_done(&branch_state);
                    sema_assign_state_done(&incoming);
                    sema_assign_state_done(&merged);
                    return false;
                }
                if (!sema_validate_assignment_node(lexer,
                                                   ast,
                                                   sema,
                                                   branch->expr_node_index,
                                                   &branch_state)) {
                    sema_assign_state_done(&branch_state);
                    sema_assign_state_done(&incoming);
                    sema_assign_state_done(&merged);
                    return false;
                }
                sema_assign_state_merge_reachable(
                    &merged, &branch_state, &any_reachable);
                sema_assign_state_done(&branch_state);
            }

            bool exhaustive = has_else;
            if (!exhaustive && on->kind != AOK_Condition &&
                node->a < array_count(sema->node_type_indices)) {
                u32 scrutinee_type = sema->node_type_indices[node->a];
                if (scrutinee_type < array_count(sema->types) &&
                    sema->types[scrutinee_type].kind == STK_Bool) {
                    exhaustive =
                        sema_assignment_on_covers_all_bool_values(ast, node->b);
                } else {
                    exhaustive = sema_on_covers_all_enum_variants(
                        ast, sema, node->b, scrutinee_type);
                }
            }
            if (!exhaustive) {
                sema_assign_state_merge_reachable(
                    &merged, &incoming, &any_reachable);
            }
            for (u32 i = 0; i < array_count(state->assigned); ++i) {
                state->assigned[i] = merged.assigned[i];
            }
            state->reachable = any_reachable;
            sema_assign_state_done(&incoming);
            sema_assign_state_done(&merged);
            return true;
        }

    case AK_For:
        {
            const AstForInfo* for_info = &ast->fors[node->a];
            for (u32 i = 0; i < for_info->init_count; ++i) {
                if (!sema_validate_assignment_node(
                        lexer,
                        ast,
                        sema,
                        ast->for_items[for_info->first_init + i],
                        state)) {
                    return false;
                }
            }
            if (for_info->iterable_node_index != U32_MAX &&
                !sema_validate_assignment_node(
                    lexer, ast, sema, for_info->iterable_node_index, state)) {
                return false;
            }
            if (for_info->condition_node_index != U32_MAX &&
                !sema_validate_assignment_node(
                    lexer, ast, sema, for_info->condition_node_index, state)) {
                return false;
            }
            SemaAssignState loop_state = sema_assign_state_copy(state);
            u32             for_scope  = sema->node_scope_indices[node_index];
            if (for_info->iterable_node_index != U32_MAX &&
                for_scope != sema_no_scope()) {
                if (for_info->index_symbol != U32_MAX) {
                    u32 index_local = sema_lookup_local(
                        sema, for_scope, for_info->index_symbol);
                    if (index_local != sema_no_local() &&
                        index_local < array_count(loop_state.assigned)) {
                        loop_state.assigned[index_local] = true;
                    }
                }
                u32 item_local =
                    sema_lookup_local(sema, for_scope, for_info->item_symbol);
                if (item_local != sema_no_local() &&
                    item_local < array_count(loop_state.assigned)) {
                    loop_state.assigned[item_local] = true;
                }
            }
            const AstNode* body = &ast->nodes[node->b];
            ASSERT(body->kind == AK_Block, "Expected for body block");
            if (!sema_validate_assignment_block(
                    lexer, ast, sema, body->a, body->b, &loop_state)) {
                sema_assign_state_done(&loop_state);
                return false;
            }
            for (u32 i = 0; i < for_info->update_count; ++i) {
                if (!sema_validate_assignment_node(
                        lexer,
                        ast,
                        sema,
                        ast->for_items[for_info->first_update + i],
                        &loop_state)) {
                    sema_assign_state_done(&loop_state);
                    return false;
                }
            }
            sema_assign_state_done(&loop_state);
            if (for_info->else_block_index != U32_MAX) {
                const AstNode* else_block =
                    &ast->nodes[for_info->else_block_index];
                SemaAssignState else_state = sema_assign_state_copy(state);
                if (!sema_validate_assignment_block(lexer,
                                                    ast,
                                                    sema,
                                                    else_block->a,
                                                    else_block->b,
                                                    &else_state)) {
                    sema_assign_state_done(&else_state);
                    return false;
                }
                sema_assign_state_done(&else_state);
            }
            return true;
        }

    case AK_Return:
    case AK_ReturnExpr:
        if (node->a != U32_MAX &&
            !sema_validate_assignment_node(lexer, ast, sema, node->a, state)) {
            return false;
        }
        state->reachable = false;
        return true;

    case AK_Break:
    case AK_BreakExpr:
        if (node->a != U32_MAX &&
            !sema_validate_assignment_node(lexer, ast, sema, node->a, state)) {
            return false;
        }
        state->reachable = false;
        return true;

    case AK_Continue:
    case AK_ContinueExpr:
        state->reachable = false;
        return true;

    case AK_Defer:
        return sema_validate_assignment_node(lexer, ast, sema, node->a, state);

    case AK_Assert:
        return sema_validate_assignment_node(
                   lexer, ast, sema, node->a, state) &&
               (node->b == U32_MAX || sema_validate_assignment_node(
                                          lexer, ast, sema, node->b, state));

    case AK_TypePointer:
    case AK_TypeSlice:
    case AK_TypeDynamicArray:
    case AK_TypeTuple:
    case AK_TypeFn:
    case AK_TypeEnum:
    case AK_TypePlex:
    case AK_AnnotatedValue:
        return true;

    case AK_Bind:
    case AK_TopOn:
        return true;
    }
    return true;
}

internal bool sema_validate_definite_assignment_function(const Lexer* lexer,
                                                         const Ast*   ast,
                                                         const Sema*  sema,
                                                         u32 fn_node_index)
{
    const AstNode* fn_def = &ast->nodes[fn_node_index];
    if (fn_def->kind != AK_FnDef || fn_def->b != AFK_Block) {
        return true;
    }
    u32 scope_index = sema->node_scope_indices[fn_node_index];
    if (scope_index == sema_no_scope()) {
        return true;
    }

    SemaAssignState state = {.reachable = true};
    for (u32 i = 0; i < array_count(sema->locals); ++i) {
        const SemaLocal* local    = &sema->locals[i];
        bool             assigned = !sema_local_starts_unassigned(ast, local);
        if (local->kind == SLK_Param || local->kind == SLK_Binder) {
            assigned = true;
        }
        array_push(state.assigned, assigned);
    }

    const AstNode* fn_start = &ast->nodes[fn_def->a];
    bool           ok       = sema_validate_assignment_block(
        lexer, ast, sema, fn_def->a + 1, fn_start->b, &state);
    sema_assign_state_done(&state);
    return ok;
}

internal bool sema_validate_definite_assignment(const Lexer*           lexer,
                                                const Ast*             ast,
                                                const FrontEndOptions* options,
                                                const Sema*            sema)
{
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        if (ast->nodes[i].kind != AK_FnDef) {
            continue;
        }
        if (sema_node_is_inside_disabled_top_on_body(options, lexer, ast, i)) {
            continue;
        }
        if (!sema_validate_definite_assignment_function(lexer, ast, sema, i)) {
            return false;
        }
    }
    return true;
}

//------------------------------------------------------------------------------
// Validate that function-local runtime bindings are read at least once.

internal bool sema_symbol_is_deliberately_unused(string symbol)
{
    return symbol.count > 0 && symbol.data[0] == '_';
}

internal string sema_unused_local_kind_name(const SemaLocal* local)
{
    switch (local->kind) {
    case SLK_Param:
        return s("parameter");
    case SLK_Binder:
        return s("pattern binder");
    case SLK_Variable:
        return s("local variable");
    case SLK_Constant:
    case SLK_Function:
    case SLK_TypeAlias:
        return s("local binding");
    }
    return s("local binding");
}

internal bool sema_local_is_method_receiver(const Sema* sema, u32 local_index)
{
    if (local_index >= array_count(sema->locals)) {
        return false;
    }
    const SemaLocal* local = &sema->locals[local_index];
    if (local->kind != SLK_Param) {
        return false;
    }

    bool method_receiver = false;
    for (u32 i = 0; i < array_count(sema->methods); ++i) {
        const SemaMethod* method = &sema->methods[i];
        if (method->decl_index == local->owner_decl_index &&
            method->first_param_is_receiver) {
            method_receiver = true;
            break;
        }
    }
    if (!method_receiver) {
        return false;
    }

    for (u32 i = 0; i < array_count(sema->locals); ++i) {
        const SemaLocal* candidate = &sema->locals[i];
        if (candidate->owner_decl_index == local->owner_decl_index &&
            candidate->kind == SLK_Param) {
            return i == local_index;
        }
    }
    return false;
}

internal void sema_count_local_ref(const Sema* sema,
                                   u32         local_index,
                                   Array(u32) read_counts,
                                   i32 delta)
{
    if (local_index == sema_no_local() ||
        local_index >= array_count(read_counts) ||
        !sema_local_is_runtime_value(&sema->locals[local_index])) {
        return;
    }
    if (delta < 0) {
        read_counts[local_index] -=
            read_counts[local_index] > 0 ? (u32)-delta : 0;
        return;
    }
    read_counts[local_index] += (u32)delta;
}

internal u32 sema_symbol_ref_runtime_local(const Ast*  ast,
                                           const Sema* sema,
                                           u32         node_index)
{
    if (node_index >= array_count(ast->nodes) ||
        ast->nodes[node_index].kind != AK_SymbolRef) {
        return sema_no_local();
    }

    u32 local_index = sema->node_local_indices[node_index];
    if (local_index != sema_no_local()) {
        return local_index;
    }

    u32 scope_index  = sema->node_scope_indices[node_index];
    u32 scoped_local = sema_no_local();
    if (scope_index != sema_no_scope()) {
        scoped_local =
            sema_lookup_local(sema, scope_index, ast->nodes[node_index].a);
        if (scoped_local != sema_no_local()) {
            return scoped_local;
        }
    }

    u32 symbol     = ast->nodes[node_index].a;
    u32 best_local = sema_no_local();
    u32 best_token = 0;
    for (u32 i = 0; i < array_count(sema->locals); ++i) {
        const SemaLocal* local = &sema->locals[i];
        if (local->symbol_handle != symbol ||
            !sema_local_is_runtime_value(local)) {
            continue;
        }
        if (ast->nodes[node_index].token_index <= local->decl_token_index) {
            continue;
        }
        if (best_local == sema_no_local() ||
            local->decl_token_index > best_token) {
            best_local = i;
            best_token = local->decl_token_index;
        }
    }
    return best_local;
}

internal u32 sema_first_local_read_node(const Ast*  ast,
                                        const Sema* sema,
                                        u32         local_index)
{
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind != AK_SymbolRef ||
            sema_symbol_ref_runtime_local(ast, sema, i) != local_index) {
            continue;
        }

        bool assignment_target = false;
        for (u32 assign = 0; assign < array_count(ast->nodes); ++assign) {
            const AstNode* assign_node = &ast->nodes[assign];
            if (assign_node->kind == AK_Assign && assign_node->a == i) {
                assignment_target = true;
                break;
            }
        }
        if (!assignment_target) {
            return i;
        }
    }

    return U32_MAX;
}

internal bool sema_validate_unused_locals(const Lexer* lexer,
                                          const Ast*   ast,
                                          const Sema*  sema)
{
    Array(u32) read_counts = NULL;
    for (u32 i = 0; i < array_count(sema->locals); ++i) {
        array_push(read_counts, 0);
    }

    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind == AK_SymbolRef) {
            sema_count_local_ref(sema,
                                 sema_symbol_ref_runtime_local(ast, sema, i),
                                 read_counts,
                                 1);
        }
    }

    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind != AK_Assign) {
            continue;
        }
        const AstNode* target = &ast->nodes[node->a];
        if (target->kind == AK_SymbolRef) {
            sema_count_local_ref(
                sema,
                sema_symbol_ref_runtime_local(ast, sema, node->a),
                read_counts,
                -1);
        }
    }

    for (u32 i = 0; i < array_count(sema->locals); ++i) {
        const SemaLocal* local = &sema->locals[i];
        bool             generic_function_local =
            local->owner_decl_index < array_count(sema->decls) &&
            sema->decls[local->owner_decl_index].kind == SK_GenericFunction;
        string symbol = lex_symbol(lexer, local->symbol_handle);

        if (sema_local_is_runtime_value(local) && !generic_function_local &&
            sema_symbol_is_deliberately_unused(symbol) && read_counts[i] > 0) {
            u32  use_node = sema_first_local_read_node(ast, sema, i);
            bool ok       = error_0347_used_underscore_local(
                lexer->source,
                use_node != U32_MAX
                    ? sema_node_span(lexer, &ast->nodes[use_node])
                    : sema_local_span(lexer, ast, local),
                sema_local_span(lexer, ast, local),
                symbol,
                sema_unused_local_kind_name(local));
            array_free(read_counts);
            return ok;
        }

        if (!sema_local_is_runtime_value(local) || generic_function_local ||
            sema_local_is_method_receiver(sema, i) ||
            sema_symbol_is_deliberately_unused(symbol) || read_counts[i] > 0) {
            continue;
        }
        string binding_kind = sema_unused_local_kind_name(local);
        bool   ok = error_0335_unused_local(lexer->source,
                                            sema_local_span(lexer, ast, local),
                                            symbol,
                                            binding_kind);
        array_free(read_counts);
        return ok;
    }

    array_free(read_counts);
    return true;
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

    const SemaType* fn_type      = &sema->types[type_index];
    bool            valid_params = fn_type->param_count == 0;
    if (!valid_params && fn_type->param_count == 1) {
        u32 string_type = sema_builtin_type(sema, STK_String);
        u32 args_type   = sema_add_slice_type(sema, string_type);
        valid_params    = sema_type_matches(
            sema, args_type, sema->type_param_types[fn_type->first_param_type]);
    }

    if (!valid_params || (!sema_type_is_integer(sema, fn_type->return_type) &&
                          sema->types[fn_type->return_type].kind != STK_Void)) {
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
        if (node->kind == AK_On) {
            const AstOnInfo* on = &ast->ons[node->b];
            for (u32 branch = 0; branch < on->branch_count; ++branch) {
                const AstOnBranch* on_branch =
                    &ast->on_branches[on->first_branch + branch];
                u32 branch_expr = on_branch->expr_node_index;
                if (branch_expr < array_count(ast->nodes) &&
                    ast->nodes[branch_expr].kind == AK_Expression) {
                    branch_expr = ast->nodes[branch_expr].a;
                }
                if (branch_expr < array_count(ast->nodes) &&
                    ast->nodes[branch_expr].kind == AK_ExprBlock) {
                    branch_expr = ast->nodes[branch_expr].a;
                }
                if (branch_expr == block_index) {
                    return true;
                }
            }
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
                lexer->source, sema_node_span(lexer, node), s("again"));
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
    case AK_Defer:
        if (node->kind == AK_Defer) {
            return sema_validate_loop_control(lexer,
                                              ast,
                                              node->a,
                                              loop_depth,
                                              expr_block_depth,
                                              expr_labels,
                                              expr_label_count);
        }
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
                for_info->mode != AFM_In &&
                for_info->condition_node_index == U32_MAX) {
                return error_0351_loop_else_on_infinite_loop(
                    lexer->source,
                    sema_node_span(lexer,
                                   &ast->nodes[for_info->else_block_index]));
            }
            if (for_info->else_block_index != U32_MAX &&
                sema_block_has_value_break_for_target(
                    ast, for_info->else_block_index, for_info->label_symbol) &&
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
                u32 arg_node = ast->call_args[call->first_arg + i];
                if (ast->nodes[arg_node].kind == AK_Assign) {
                    arg_node = ast->nodes[arg_node].b;
                }
                if (!sema_validate_loop_control(lexer,
                                                ast,
                                                arg_node,
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
    case AK_ErrorInject:
    case AK_Propagate:
    case AK_BitwiseNot:
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
    case AK_ShiftLeft:
    case AK_ShiftRight:
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
    case AK_Cast:
        {
            const AstCastInfo* cast = sema_cast_info(ast, node);
            return sema_validate_loop_control(lexer,
                                              ast,
                                              node->a,
                                              loop_depth,
                                              expr_block_depth,
                                              expr_labels,
                                              expr_label_count) &&
                   (cast->extra_node_index == U32_MAX ||
                    sema_validate_loop_control(lexer,
                                               ast,
                                               cast->extra_node_index,
                                               loop_depth,
                                               expr_block_depth,
                                               expr_labels,
                                               expr_label_count));
        }
    case AK_Bind:
    case AK_Variable:
        return sema_validate_loop_control(lexer,
                                          ast,
                                          node->b,
                                          loop_depth,
                                          expr_block_depth,
                                          expr_labels,
                                          expr_label_count);
    case AK_Assign:
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
    default:
        return true;
    }
}

internal bool sema_validate_all_loop_control(const Lexer*           lexer,
                                             const Ast*             ast,
                                             const FrontEndOptions* options)
{
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        u32 expr_labels[SEMA_MAX_CONTROL_LABELS] = {0};
        if (ast->nodes[i].kind != AK_FnDef) {
            continue;
        }
        if (sema_node_is_inside_disabled_top_on_body(options, lexer, ast, i)) {
            continue;
        }
        if (!sema_validate_loop_control(lexer, ast, i, 0, 0, expr_labels, 0)) {
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
    sema.program              = effective_options.program;
    sema.current_module_index = effective_options.current_module_index;

    // Seed commonly-used built-in types so later materialisation can always
    // canonicalise untyped numeric literals to runtime storage types.
    sema_builtin_type(&sema, STK_Void);
    sema_builtin_type(&sema, STK_UntypedInteger);
    sema_builtin_type(&sema, STK_UntypedFloat);
    sema_builtin_type(&sema, STK_Nil);
    sema_builtin_type(&sema, STK_String);
    sema_builtin_type(&sema, STK_Bool);
    sema_builtin_type(&sema, STK_Arena);
    sema_builtin_type(&sema, STK_I32);
    sema_builtin_type(&sema, STK_F64);

    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        array_push(sema.node_decl_indices, sema_no_decl());
        array_push(sema.node_local_indices, sema_no_local());
        array_push(sema.node_scope_indices, sema_no_scope());
        array_push(sema.node_lowered_symbol_handles, U32_MAX);
        array_push(sema.node_type_indices, sema_no_type());
        array_push(sema.node_method_call_decl_indices, sema_no_decl());
        array_push(sema.node_method_call_receiver_refs, false);
        array_push(sema.node_method_call_receiver_derefs, false);
        array_push(sema.node_method_call_explicit_traits, false);
        array_push(sema.node_implicit_array_type_indices, sema_no_type());
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
    if (!sema_import_implicit_core_decls(lexer, &sema)) {
        sema_done(&sema);
        return false;
    }
    if (!sema_validate_where_constraints(lexer, ast, &sema)) {
        sema_done(&sema);
        return false;
    }
    if (!sema_collect_top_level_uses(lexer, ast, &effective_options, &sema)) {
        sema_done(&sema);
        return false;
    }
    if (!sema_classify_type_aliases(lexer, ast, &sema)) {
        sema_done(&sema);
        return false;
    }
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        if (ast->nodes[i].kind == AK_AnnotatedValue ||
            ast->nodes[i].kind == AK_ZeroInit ||
            ast->nodes[i].kind == AK_Undefined) {
            sema_mark_type_expr_nodes(ast, &sema, ast->nodes[i].a);
        } else if (ast->nodes[i].kind == AK_Cast) {
            sema_mark_type_expr_nodes(
                ast, &sema, ast->casts[ast->nodes[i].b].type_node_index);
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
    if (!sema_validate_trait_impls(lexer, ast, &sema)) {
        sema_done(&sema);
        return false;
    }
    if (!sema_resolve_symbol_refs(lexer, ast, &sema)) {
        sema_done(&sema);
        return false;
    }
    if (!sema_validate_generic_body_refs(lexer, ast, &sema)) {
        sema_done(&sema);
        return false;
    }
    if (!sema_validate_all_loop_control(lexer, ast, options)) {
        sema_done(&sema);
        return false;
    }
    sema_collect_deps(ast, &sema, &sema);
    if (!sema_order_decls(lexer, ast, &sema)) {
        sema_done(&sema);
        return false;
    }
    if (!sema_seed_return_context_local_types(lexer, ast, options, &sema)) {
        sema_done(&sema);
        return false;
    }
    if (!sema_seed_call_arg_context_local_types(lexer, ast, options, &sema)) {
        sema_done(&sema);
        return false;
    }
    if (!sema_assign_decl_types(lexer, ast, &sema)) {
        sema_done(&sema);
        return false;
    }
    if (!sema_validate_trait_impl_signatures(lexer, ast, &sema)) {
        sema_done(&sema);
        return false;
    }
    if (!sema_assign_local_types(lexer, ast, &sema)) {
        sema_done(&sema);
        return false;
    }
    if (!sema_validate_definite_assignment(lexer, ast, options, &sema)) {
        sema_done(&sema);
        return false;
    }
    if (!sema_validate_unused_locals(lexer, ast, &sema)) {
        *out_sema = sema;
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
    for (u32 i = 0; i < array_count(sema->generic_fn_instantiations); ++i) {
        SemaGenericFnInstantiation* inst = &sema->generic_fn_instantiations[i];
        array_free(inst->node_decl_indices);
        array_free(inst->node_local_indices);
        array_free(inst->node_scope_indices);
        array_free(inst->node_lowered_symbol_handles);
        array_free(inst->node_type_indices);
        array_free(inst->node_method_call_decl_indices);
        array_free(inst->node_method_call_receiver_refs);
        array_free(inst->node_method_call_receiver_derefs);
        array_free(inst->node_method_call_explicit_traits);
    }
    array_free(sema->types);
    array_free(sema->type_param_types);
    array_free(sema->type_param_symbols);
    array_free(sema->type_param_values);
    array_free(sema->decls);
    array_free(sema->generic_fn_instantiations);
    array_free(sema->methods);
    array_free(sema->locals);
    array_free(sema->scopes);
    array_free(sema->deps);
    array_free(sema->ordered_decl_indices);
    array_free(sema->node_decl_indices);
    array_free(sema->node_local_indices);
    array_free(sema->node_scope_indices);
    array_free(sema->node_lowered_symbol_handles);
    array_free(sema->node_type_indices);
    array_free(sema->node_method_call_decl_indices);
    array_free(sema->node_method_call_receiver_refs);
    array_free(sema->node_method_call_receiver_derefs);
    array_free(sema->node_method_call_explicit_traits);
    array_free(sema->node_implicit_array_type_indices);
    array_free(sema->on_branch_local_indices);
    array_free(sema->pattern_local_indices);
    array_free(sema->node_is_type_expr);
    array_free(sema->node_const_known);
    array_free(sema->node_const_values);
    *sema = (Sema){0};
}

//------------------------------------------------------------------------------
