//------------------------------------------------------------------------------
// LSP completion support
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/build/front/front.h>
#include <compiler/error/error.h>
#include <compiler/modules/modules.h>
#include <compiler/sema/sema.h>
#include <lsp/lsp.h>

//------------------------------------------------------------------------------

internal bool lsp_completion_is_ident_char(u8 c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
           (c >= '0' && c <= '9') || c == '_';
}

internal string lsp_completion_ident_before(string source, usize end)
{
    while (end > 0 &&
           (source.data[end - 1] == ' ' || source.data[end - 1] == '\t')) {
        end--;
    }

    usize start = end;
    while (start > 0 && lsp_completion_is_ident_char(source.data[start - 1])) {
        start--;
    }
    return (string){.data = source.data + start, .count = end - start};
}

internal bool lsp_completion_after_open_brace(string source, usize offset)
{
    while (offset > 0 && (source.data[offset - 1] == ' ' ||
                          source.data[offset - 1] == '\t')) {
        offset--;
    }
    return offset > 0 && source.data[offset - 1] == '{';
}

internal bool lsp_completion_label_matches_prefix(string label, string prefix)
{
    if (prefix.count == 0) {
        return true;
    }
    if (label.count < prefix.count) {
        return false;
    }
    return memcmp(label.data, prefix.data, prefix.count) == 0;
}

internal bool lsp_completion_parse_u32(string value, u32* out)
{
    if (value.count == 0) {
        return false;
    }

    u64 result = 0;
    for (usize i = 0; i < value.count; ++i) {
        u8 ch = value.data[i];
        if (ch < '0' || ch > '9') {
            return false;
        }
        result = result * 10 + (u64)(ch - '0');
        if (result > UINT32_MAX) {
            return false;
        }
    }
    *out = (u32)result;
    return true;
}

internal u32 lsp_completion_builtin_type(const Sema* sema, SemaTypeKind kind)
{
    for (u32 i = 0; i < array_count(sema->types); ++i) {
        if (sema->types[i].kind == kind) {
            return i;
        }
    }
    return sema_no_type();
}

internal u32 lsp_completion_pointer_type(const Sema* sema, u32 pointee_type)
{
    if (pointee_type == sema_no_type()) {
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

internal void lsp_completion_filter_items(JsonValue* items, string prefix)
{
    if (prefix.count == 0 || items->kind != JSON_ARRAY) {
        return;
    }
    if (array_count(items->array.values) == 0) {
        return;
    }

    usize write_index = 0;
    for (usize read_index = 0; read_index < array_count(items->array.values);
         ++read_index) {
        JsonValue* item  = items->array.values[read_index];
        JsonValue* label = json_object_get_cstr(item, "label");
        if (label == NULL || label->kind != JSON_STRING ||
            !lsp_completion_label_matches_prefix(json_string(label), prefix)) {
            continue;
        }
        items->array.values[write_index++] = item;
    }
    __array_count(items->array.values) = write_index;
}

internal bool
lsp_completion_member_context(string source, usize offset, string* out_receiver)
{
    usize prefix_start = offset;
    while (prefix_start > 0 &&
           lsp_completion_is_ident_char(source.data[prefix_start - 1])) {
        prefix_start--;
    }
    if (prefix_start == 0 || source.data[prefix_start - 1] != '.') {
        return false;
    }

    usize receiver_end   = prefix_start - 1;
    usize receiver_start = receiver_end;
    while (receiver_start > 0) {
        usize ident_end   = receiver_start;
        usize ident_start = ident_end;
        while (ident_start > 0 &&
               lsp_completion_is_ident_char(source.data[ident_start - 1])) {
            ident_start--;
        }
        if (ident_start == ident_end) {
            break;
        }
        receiver_start = ident_start;
        if (receiver_start == 0 || source.data[receiver_start - 1] != '.') {
            break;
        }
        receiver_start--;
    }

    string receiver = {
        .data  = source.data + receiver_start,
        .count = receiver_end - receiver_start,
    };
    if (receiver.count == 0) {
        return false;
    }

    *out_receiver = receiver;
    return true;
}

internal usize lsp_completion_local_decl_offset(const LspDocument* doc,
                                                const SemaLocal*   local)
{
    const Lexer* lexer = &doc->front_end.lexer;
    if (local->decl_token_index != U32_MAX &&
        local->decl_token_index < array_count(lexer->tokens)) {
        return lexer->tokens[local->decl_token_index].offset;
    }
    if (local->decl_node_index < array_count(doc->front_end.ast.nodes)) {
        const AstNode* node = &doc->front_end.ast.nodes[local->decl_node_index];
        if (node->token_index < array_count(lexer->tokens)) {
            return lexer->tokens[node->token_index].offset;
        }
    }
    return 0;
}

internal u32 lsp_completion_type_for_symbol(const LspDocument* doc,
                                            string             symbol,
                                            usize              offset)
{
    if (!doc->type_facts_partial) {
        return sema_no_type();
    }

    const Lexer* lexer             = &doc->front_end.lexer;
    const Sema*  sema              = &doc->front_end.sema;
    u32          best_local_type   = sema_no_type();
    usize        best_local_offset = 0;
    for (u32 i = 0; i < array_count(sema->locals); ++i) {
        const SemaLocal* local = &sema->locals[i];
        if (local->symbol_handle != U32_MAX &&
            string_eq(lex_symbol(lexer, local->symbol_handle), symbol)) {
            usize local_offset = lsp_completion_local_decl_offset(doc, local);
            if (local_offset <= offset && local_offset >= best_local_offset) {
                best_local_type   = local->type_index;
                best_local_offset = local_offset;
            }
        }
    }
    if (best_local_offset != 0 || best_local_type != sema_no_type()) {
        return best_local_type;
    }

    for (u32 i = 0; i < array_count(sema->decls); ++i) {
        const SemaDecl* decl = &sema->decls[i];
        if (decl->symbol_handle != U32_MAX &&
            string_eq(lex_symbol(lexer, decl->symbol_handle), symbol)) {
            return decl->type_index;
        }
    }

    return sema_no_type();
}

internal u32 lsp_completion_field_type(const Sema*  sema,
                                       const Lexer* lexer,
                                       u32          receiver_type,
                                       string       field)
{
    if (receiver_type == sema_no_type() ||
        receiver_type >= array_count(sema->types)) {
        return sema_no_type();
    }

    receiver_type = sema_materialise_type(sema, receiver_type);
    if (receiver_type == sema_no_type() ||
        receiver_type >= array_count(sema->types)) {
        return sema_no_type();
    }

    const SemaType* type = &sema->types[receiver_type];
    if (type->kind == STK_Pointer &&
        type->first_param_type < array_count(sema->types)) {
        while (type->kind == STK_Pointer &&
               type->first_param_type < array_count(sema->types)) {
            u32 pointee_type =
                sema_materialise_type(sema, type->first_param_type);
            if (pointee_type >= array_count(sema->types)) {
                break;
            }
            SemaTypeKind pointee_kind = sema->types[pointee_type].kind;
            if (pointee_kind != STK_Pointer && pointee_kind != STK_Box &&
                pointee_kind != STK_Tuple && pointee_kind != STK_Array &&
                pointee_kind != STK_Slice && pointee_kind != STK_String &&
                pointee_kind != STK_DynamicArray && pointee_kind != STK_Plex &&
                pointee_kind != STK_Union && pointee_kind != STK_Arena) {
                break;
            }
            receiver_type = pointee_type;
            type          = &sema->types[receiver_type];
        }
    }
    if (type->kind == STK_Box &&
        type->first_param_type < array_count(sema->types)) {
        receiver_type = sema_materialise_type(sema, type->first_param_type);
        type          = &sema->types[receiver_type];
    }

    if (type->kind == STK_Tuple) {
        u32 index = 0;
        if (!lsp_completion_parse_u32(field, &index) ||
            index >= type->param_count) {
            return sema_no_type();
        }
        return sema->type_param_types[type->first_param_type + index];
    }

    if (type->kind == STK_Array) {
        return (string_eq(field, s("count")) || string_eq(field, s("size")) ||
                string_eq(field, s("bytes")))
                   ? lsp_completion_builtin_type(sema, STK_Usize)
                   : sema_no_type();
    }

    if (type->kind == STK_String || type->kind == STK_Slice) {
        if (string_eq(field, s("data"))) {
            u32 item_type = type->kind == STK_String
                                ? lsp_completion_builtin_type(sema, STK_U8)
                                : type->first_param_type;
            return lsp_completion_pointer_type(sema, item_type);
        }
        if (string_eq(field, s("count")) || string_eq(field, s("size")) ||
            string_eq(field, s("bytes"))) {
            return lsp_completion_builtin_type(sema, STK_Usize);
        }
        return sema_no_type();
    }

    if (type->kind == STK_DynamicArray) {
        if (string_eq(field, s("data"))) {
            return lsp_completion_pointer_type(sema, type->first_param_type);
        }
        if (string_eq(field, s("count")) || string_eq(field, s("capacity")) ||
            string_eq(field, s("size"))) {
            return lsp_completion_builtin_type(sema, STK_Usize);
        }
        return sema_no_type();
    }

    if (type->kind != STK_Plex && type->kind != STK_Union) {
        return sema_no_type();
    }

    for (u32 i = 0; i < type->param_count; ++i) {
        u32 param_index      = type->first_param_type + i;
        u32 symbol           = U32_MAX;
        u32 param_type_index = sema_no_type();
        if (!lsp_sema_type_param(
                sema, param_index, &symbol, &param_type_index)) {
            continue;
        }
        if (symbol != U32_MAX && string_eq(lex_symbol(lexer, symbol), field)) {
            return param_type_index;
        }
    }

    return sema_no_type();
}

internal u32 lsp_completion_type_for_receiver(const LspDocument* doc,
                                              string             receiver,
                                              usize              offset)
{
    usize i = 0;
    while (i < receiver.count &&
           lsp_completion_is_ident_char(receiver.data[i])) {
        i++;
    }
    if (i == 0) {
        return sema_no_type();
    }

    u32 type_index = lsp_completion_type_for_symbol(
        doc, (string){.data = receiver.data, .count = i}, offset);
    const Sema*  sema  = &doc->front_end.sema;
    const Lexer* lexer = &doc->front_end.lexer;
    while (i < receiver.count) {
        if (receiver.data[i] != '.') {
            return sema_no_type();
        }
        i++;
        usize start = i;
        while (i < receiver.count &&
               lsp_completion_is_ident_char(receiver.data[i])) {
            i++;
        }
        if (i == start) {
            return sema_no_type();
        }
        type_index = lsp_completion_field_type(
            sema,
            lexer,
            type_index,
            (string){.data = receiver.data + start, .count = i - start});
    }

    return type_index;
}

internal void
lsp_completion_add(Arena* arena, JsonValue* items, string label, u32 kind)
{
    for (usize i = 0; i < array_count(items->array.values); ++i) {
        JsonValue* existing       = items->array.values[i];
        JsonValue* existing_label = json_object_get_cstr(existing, "label");
        if (existing_label != NULL && existing_label->kind == JSON_STRING &&
            string_eq(json_string(existing_label), label)) {
            return;
        }
    }

    JsonValue* item = json_new_object(arena);
    json_object_set_string(item, arena, "label", label);
    json_object_set_number(item, arena, "kind", kind);
    json_array_push(items, item);
}

internal void lsp_completion_add_plex_literal_field(Arena*     arena,
                                                    JsonValue* items,
                                                    string     label)
{
    for (usize i = 0; i < array_count(items->array.values); ++i) {
        JsonValue* existing       = items->array.values[i];
        JsonValue* existing_label = json_object_get_cstr(existing, "label");
        if (existing_label != NULL && existing_label->kind == JSON_STRING &&
            string_eq(json_string(existing_label), label)) {
            return;
        }
    }

    JsonValue* item = json_new_object(arena);
    json_object_set_string(item, arena, "label", label);
    json_object_set_number(item, arena, "kind", 5); // Field
    json_object_set_string(item,
                           arena,
                           "insertText",
                           string_format(arena, STRINGP ": ", STRINGV(label)));
    json_array_push(items, item);
}

internal void lsp_completion_add_member_details(Arena* arena, JsonValue* items)
{
    if (items->kind != JSON_ARRAY) {
        return;
    }

    for (usize i = 0; i < array_count(items->array.values); ++i) {
        JsonValue* item = items->array.values[i];
        JsonValue* kind = json_object_get_cstr(item, "kind");
        if (kind == NULL || kind->kind != JSON_NUMBER) {
            continue;
        }

        u32 completion_kind = (u32)kind->number;
        if (completion_kind == 2) { // Method
            json_object_set_string(item, arena, "detail", s("method"));
        } else if (completion_kind == 5) { // Field
            json_object_set_string(item, arena, "detail", s("field"));
        }
    }
}

internal void lsp_completion_add_dynamic_array_members(Arena*     arena,
                                                       JsonValue* items)
{
    lsp_completion_add(arena, items, s("data"), 5);                // Field
    lsp_completion_add(arena, items, s("count"), 5);               // Field
    lsp_completion_add(arena, items, s("capacity"), 5);            // Field
    lsp_completion_add(arena, items, s("append"), 2);              // Method
    lsp_completion_add(arena, items, s("clear"), 2);               // Method
    lsp_completion_add(arena, items, s("delete"), 2);              // Method
    lsp_completion_add(arena, items, s("free"), 2);                // Method
    lsp_completion_add(arena, items, s("pop"), 2);                 // Method
    lsp_completion_add(arena, items, s("push"), 2);                // Method
    lsp_completion_add(arena, items, s("reserve_to"), 2);          // Method
    lsp_completion_add(arena, items, s("reserve_extra"), 2);       // Method
    lsp_completion_add(arena, items, s("resize_to"), 2);           // Method
    lsp_completion_add(arena, items, s("swap_delete"), 2);         // Method
    lsp_completion_add(arena, items, s("resize_undefined_to"), 2); // Method
    lsp_completion_add(arena, items, s("extend"), 2);              // Method
    lsp_completion_add(arena, items, s("extend_undefined"), 2);    // Method
}

internal void lsp_completion_add_box_members(Arena* arena, JsonValue* items)
{
    lsp_completion_add(arena, items, s("free"), 2); // Method
}

internal void lsp_completion_add_array_members(Arena* arena, JsonValue* items)
{
    lsp_completion_add(arena, items, s("count"), 5); // Field
    lsp_completion_add(arena, items, s("size"), 5);  // Field
    lsp_completion_add(arena, items, s("bytes"), 5); // Field
}

internal void lsp_completion_add_slice_members(Arena* arena, JsonValue* items)
{
    lsp_completion_add(arena, items, s("data"), 5);  // Field
    lsp_completion_add(arena, items, s("count"), 5); // Field
    lsp_completion_add(arena, items, s("size"), 5);  // Field
    lsp_completion_add(arena, items, s("bytes"), 5); // Field
}

internal void lsp_completion_add_string_members(Arena* arena, JsonValue* items)
{
    lsp_completion_add(arena, items, s("data"), 5);  // Field
    lsp_completion_add(arena, items, s("count"), 5); // Field
    lsp_completion_add(arena, items, s("size"), 5);  // Field
    lsp_completion_add(arena, items, s("bytes"), 5); // Field
}

internal bool lsp_completion_items_have_label(JsonValue* items, string label)
{
    if (items->kind != JSON_ARRAY) {
        return false;
    }
    for (u32 i = 0; i < array_count(items->array.values); ++i) {
        JsonValue* item       = items->array.values[i];
        JsonValue* item_label = json_object_get_cstr(item, "label");
        if (item_label != NULL && item_label->kind == JSON_STRING &&
            string_eq(json_string(item_label), label)) {
            return true;
        }
    }
    return false;
}

internal void lsp_completion_add_unique(Arena*     arena,
                                        JsonValue* items,
                                        string     label,
                                        u32        kind)
{
    if (!lsp_completion_items_have_label(items, label)) {
        lsp_completion_add(arena, items, label, kind);
    }
}

internal bool lsp_completion_is_internal_label(string label)
{
    string impl_prefix = s("__impl_");
    return label.count >= impl_prefix.count &&
           memcmp(label.data, impl_prefix.data, impl_prefix.count) == 0;
}

internal u32 lsp_completion_decl_kind(SemaDeclKind kind)
{
    switch (kind) {
    case SK_Function:
    case SK_FfiFunction:
    case SK_BuiltinFunction:
    case SK_GenericFunction:
        return 3; // Function
    case SK_Module:
        return 9; // Module
    case SK_TypeAlias:
    case SK_GenericTypeAlias:
        return 22; // Struct
    case SK_Trait:
        return 8; // Interface
    case SK_Constant:
        return 21; // Constant
    case SK_Variable:
        return 6; // Variable
    }
    return 1; // Text
}

internal u32 lsp_completion_local_kind(SemaLocalKind kind)
{
    switch (kind) {
    case SLK_Function:
        return 3; // Function
    case SLK_TypeAlias:
        return 22; // Struct
    case SLK_Constant:
        return 21; // Constant
    case SLK_Param:
    case SLK_Variable:
    case SLK_Binder:
        return 6; // Variable
    }
    return 1; // Text
}

internal void lsp_completion_add_module_exports(Arena*        arena,
                                                JsonValue*    items,
                                                LspModuleView module)
{
    for (u32 i = 0; i < lsp_module_export_count(&module); ++i) {
        const SemaDecl* decl = NULL;
        if (!lsp_module_export_decl(&module, i, &decl, NULL)) {
            continue;
        }
        if (decl->symbol_handle == U32_MAX) {
            continue;
        }
        string label = lex_symbol(module.lexer, decl->symbol_handle);
        if (lsp_completion_is_internal_label(label)) {
            continue;
        }
        lsp_completion_add(
            arena, items, label, lsp_completion_decl_kind(decl->kind));
    }
}

internal u32 lsp_completion_ast_export_kind(const Ast* ast, const AstNode* node)
{
    if (node->kind == AK_FfiDef) {
        return 3; // Function
    }
    if (node->kind == AK_Variable) {
        return 6; // Variable
    }
    if (node->kind != AK_Bind || node->b >= array_count(ast->nodes)) {
        return 1; // Text
    }

    const AstNode* value = &ast->nodes[node->b];
    switch (value->kind) {
    case AK_FnDef:
        return 3; // Function
    case AK_IntegerLiteral:
    case AK_FloatLiteral:
    case AK_StringLiteral:
    case AK_BoolLiteral:
    case AK_NilLiteral:
        return 21; // Constant
    default:
        break;
    }
    return 22; // Struct/type alias
}

internal void lsp_completion_add_ast_exports(Arena*       arena,
                                             JsonValue*   items,
                                             const Lexer* lexer,
                                             const Ast*   ast)
{
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (!ast_has_flag(node, ANF_Public)) {
            continue;
        }

        u32 symbol_handle = U32_MAX;
        if (node->kind == AK_Bind || node->kind == AK_Variable) {
            symbol_handle = node->a;
        } else if (node->kind == AK_FfiDef &&
                   node->a < array_count(ast->ffi_infos)) {
            symbol_handle = ast->ffi_infos[node->a].symbol_handle;
        }

        if (symbol_handle == U32_MAX) {
            continue;
        }
        lsp_completion_add(arena,
                           items,
                           lex_symbol(lexer, symbol_handle),
                           lsp_completion_ast_export_kind(ast, node));
    }
}

internal bool lsp_completion_path_is_module_part_file(cstr path)
{
    string filename = path_filename(s(path));
    return !string_eq(filename, s("mod.n")) &&
           path_has_extension(filename, ".n");
}

internal int lsp_completion_compare_cstr_ptr(const void* lhs, const void* rhs)
{
    const cstr* a = lhs;
    const cstr* b = rhs;
    return strcmp(*a, *b);
}

internal bool
lsp_completion_mod_ref_matches_path(const Lexer*         lexer,
                                    const Ast*           ast,
                                    const AstModulePath* module_path,
                                    cstr                 path)
{
    if (module_path->symbol_count != 1) {
        return false;
    }

    string module_name =
        lex_symbol(lexer, ast->module_path_symbols[module_path->first_symbol]);
    return string_eq(module_name, path_stem(s(path)));
}

internal bool lsp_completion_mod_explicitly_uses_child_path(const Lexer* lexer,
                                                            const Ast*   ast,
                                                            cstr         path)
{
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind != AK_ModRef ||
            node->a >= array_count(ast->module_paths)) {
            continue;
        }
        if (lsp_completion_mod_ref_matches_path(
                lexer, ast, &ast->module_paths[node->a], path)) {
            return true;
        }
    }
    return false;
}

internal bool lsp_completion_add_module_ast_exports_from_file(Arena*     arena,
                                                              JsonValue* items,
                                                              cstr       path)
{
    FileMap map    = {0};
    string  source = filemap_load(path, &map);
    if (source.data == NULL) {
        return false;
    }

    ErrorRenderMode previous_mode = error_system_mode();
    bool            previous_emit = error_system_should_emit_output();
    error_system_set_mode(ERROR_RENDER_DIAGNOSTICS);
    error_system_set_emit_output(false);

    usize before = array_count(items->array.values);
    Lexer lexer  = {0};
    if (lex((NerdSource){.source = source, .source_path = s(path)}, &lexer)) {
        Ast ast = ast_parse(&lexer);
        if (array_count(ast.nodes) > 0) {
            lsp_completion_add_ast_exports(arena, items, &lexer, &ast);
        }
        ast_done(&ast);
    }
    lex_done(&lexer);

    error_system_set_mode(previous_mode);
    error_system_set_emit_output(previous_emit);
    filemap_unload(&map);
    return array_count(items->array.values) > before;
}

internal bool
lsp_completion_add_module_part_ast_exports(Arena*       arena,
                                           JsonValue*   items,
                                           const Lexer* root_lexer,
                                           const Ast*   root_ast,
                                           cstr         resolved_path)
{
    if (!string_eq(path_filename(s(resolved_path)), s("mod.n"))) {
        return false;
    }

    Arena temp = {0};
    arena_init(&temp);
    cstr module_dir        = path_dirname(&temp, resolved_path);

    Array(cstr) part_paths = NULL;
    DirIter iter           = {0};
    if (dir_iter_init(&iter, module_dir)) {
        cstr path         = NULL;
        bool is_directory = false;
        while (dir_iter_next(&iter, &temp, &path, &is_directory)) {
            if (!is_directory &&
                lsp_completion_path_is_module_part_file(path)) {
                array_push(part_paths, path);
            }
        }
        dir_iter_done(&iter);
    }

    if (array_count(part_paths) > 1) {
        qsort(part_paths,
              array_count(part_paths),
              sizeof(part_paths[0]),
              lsp_completion_compare_cstr_ptr);
    }

    bool added = false;
    for (u32 i = 0; i < array_count(part_paths); ++i) {
        if (lsp_completion_mod_explicitly_uses_child_path(
                root_lexer, root_ast, part_paths[i])) {
            continue;
        }
        if (lsp_completion_add_module_ast_exports_from_file(
                arena, items, part_paths[i])) {
            added = true;
        }
    }

    array_free(part_paths);
    arena_done(&temp);
    return added;
}

internal bool lsp_completion_add_module_ast_exports_from_source(
    Arena* arena, JsonValue* items, string source, cstr resolved_path)
{
    ErrorRenderMode previous_mode = error_system_mode();
    bool            previous_emit = error_system_should_emit_output();
    error_system_set_mode(ERROR_RENDER_DIAGNOSTICS);
    error_system_set_emit_output(false);

    Lexer lexer = {0};
    bool  ok =
        lex((NerdSource){.source = source, .source_path = s(resolved_path)},
            &lexer);
    if (ok) {
        Ast ast = ast_parse(&lexer);
        if (array_count(ast.nodes) > 0) {
            lsp_completion_add_ast_exports(arena, items, &lexer, &ast);
            if (lsp_completion_add_module_part_ast_exports(
                    arena, items, &lexer, &ast, resolved_path)) {
                ok = true;
            }
        }
        ast_done(&ast);
    }
    lex_done(&lexer);

    error_system_set_mode(previous_mode);
    error_system_set_emit_output(previous_emit);
    return array_count(items->array.values) > 0;
}

internal bool lsp_completion_method_receiver_type(const Sema*       sema,
                                                  const SemaMethod* method,
                                                  u32* out_receiver_type)
{
    if (method->decl_index >= array_count(sema->decls)) {
        return false;
    }

    const SemaDecl* decl    = &sema->decls[method->decl_index];
    u32 fn_type_index       = sema_materialise_type(sema, decl->type_index);
    const SemaType* fn_type = NULL;
    if (!lsp_sema_type(sema, fn_type_index, &fn_type) ||
        fn_type->kind != STK_Function || fn_type->param_count == 0 ||
        fn_type->first_param_type >= array_count(sema->type_param_types)) {
        return false;
    }

    *out_receiver_type = sema_materialise_type(
        sema, sema->type_param_types[fn_type->first_param_type]);
    return *out_receiver_type != sema_no_type();
}

internal bool lsp_completion_receiver_types_match(const Sema* sema,
                                                  u32         expected,
                                                  u32         receiver_type,
                                                  u32         member_type)
{
    expected      = sema_materialise_type(sema, expected);
    receiver_type = sema_materialise_type(sema, receiver_type);
    member_type   = sema_materialise_type(sema, member_type);
    if (expected == sema_no_type()) {
        return false;
    }
    if (expected == receiver_type || expected == member_type) {
        return true;
    }

    u32 receiver_pointer = lsp_completion_pointer_type(sema, receiver_type);
    if (receiver_pointer != sema_no_type() &&
        sema_materialise_type(sema, receiver_pointer) == expected) {
        return true;
    }

    u32 member_pointer = lsp_completion_pointer_type(sema, member_type);
    if (member_pointer != sema_no_type() &&
        sema_materialise_type(sema, member_pointer) == expected) {
        return true;
    }

    if (receiver_type < array_count(sema->types) &&
        sema->types[receiver_type].kind == STK_Pointer &&
        sema_materialise_type(
            sema, sema->types[receiver_type].first_param_type) == expected) {
        return true;
    }

    return false;
}

internal bool lsp_completion_method_matches_receiver(const LspDocument* doc,
                                                     const SemaMethod*  method,
                                                     u32 receiver_type,
                                                     u32 member_type)
{
    if (!method->first_param_is_receiver || method->symbol_handle == U32_MAX) {
        return false;
    }

    const Sema* sema     = &doc->front_end.sema;
    u32         expected = sema_no_type();
    if (!lsp_completion_method_receiver_type(sema, method, &expected)) {
        u32 target_type = sema_no_type();
        if (!lsp_sema_node_type(
                sema, method->target_type_node_index, &target_type)) {
            return true;
        }
        target_type   = sema_materialise_type(sema, target_type);
        receiver_type = sema_materialise_type(sema, receiver_type);
        member_type   = sema_materialise_type(sema, member_type);
        return target_type == sema_no_type() || target_type == receiver_type ||
               target_type == member_type;
    }

    return lsp_completion_receiver_types_match(
        sema, expected, receiver_type, member_type);
}

internal bool
lsp_completion_module_method_matches_receiver(const LspDocument*   doc,
                                              const LspModuleView* module,
                                              const SemaMethod*    method,
                                              u32 receiver_type,
                                              u32 member_type)
{
    if (!method->is_public || !method->first_param_is_receiver ||
        method->symbol_handle == U32_MAX) {
        return false;
    }

    u32  source_type      = sema_no_type();
    bool source_is_target = false;
    if (!lsp_completion_method_receiver_type(
            module->sema, method, &source_type)) {
        u32 target_node_index = method->target_type_node_index;
        while (target_node_index < array_count(module->ast->nodes) &&
               (module->ast->nodes[target_node_index].kind == AK_Expression ||
                module->ast->nodes[target_node_index].kind == AK_Statement)) {
            target_node_index = module->ast->nodes[target_node_index].a;
        }
        if (target_node_index >= array_count(module->ast->nodes) ||
            module->ast->nodes[target_node_index].kind != AK_SymbolRef) {
            return false;
        }

        u32 target_symbol = module->ast->nodes[target_node_index].a;
        for (u32 i = 0; i < array_count(module->sema->decls); ++i) {
            const SemaDecl* decl = &module->sema->decls[i];
            if (decl->symbol_handle == target_symbol &&
                decl->type_index != sema_no_type()) {
                source_type      = decl->type_index;
                source_is_target = true;
                break;
            }
        }
        if (source_type == sema_no_type()) {
            return false;
        }
    }

    const Lexer* lexer       = &doc->front_end.lexer;
    const Sema*  sema        = &doc->front_end.sema;
    u32          target_type = sema_import_type(
        (Lexer*)lexer, (Sema*)sema, module->lexer, module->sema, source_type);
    if (source_is_target) {
        target_type   = sema_materialise_type(sema, target_type);
        receiver_type = sema_materialise_type(sema, receiver_type);
        member_type   = sema_materialise_type(sema, member_type);
        return target_type != sema_no_type() &&
               (target_type == receiver_type || target_type == member_type);
    }

    return lsp_completion_receiver_types_match(
        sema, target_type, receiver_type, member_type);
}

internal void lsp_completion_add_members(Arena*             arena,
                                         JsonValue*         items,
                                         const LspDocument* doc,
                                         u32                type_index)
{
    const Sema* sema = &doc->front_end.sema;
    if (type_index == sema_no_type() ||
        type_index >= array_count(sema->types)) {
        return;
    }

    type_index = sema_materialise_type(sema, type_index);
    if (type_index == sema_no_type() ||
        type_index >= array_count(sema->types)) {
        return;
    }

    u32             receiver_type = type_index;
    const SemaType* type          = &sema->types[type_index];
    if (type->kind == STK_Pointer &&
        type->first_param_type < array_count(sema->types)) {
        while (type->kind == STK_Pointer &&
               type->first_param_type < array_count(sema->types)) {
            u32 pointee_type =
                sema_materialise_type(sema, type->first_param_type);
            if (pointee_type >= array_count(sema->types)) {
                break;
            }
            SemaTypeKind pointee_kind = sema->types[pointee_type].kind;
            if (pointee_kind != STK_Pointer && pointee_kind != STK_Box &&
                pointee_kind != STK_Tuple && pointee_kind != STK_Array &&
                pointee_kind != STK_Slice && pointee_kind != STK_String &&
                pointee_kind != STK_DynamicArray && pointee_kind != STK_Plex &&
                pointee_kind != STK_Union && pointee_kind != STK_Arena) {
                break;
            }
            type_index = pointee_type;
            type       = &sema->types[type_index];
        }
    }
    if (type->kind == STK_Box &&
        type->first_param_type < array_count(sema->types)) {
        lsp_completion_add_box_members(arena, items);
        type_index = sema_materialise_type(sema, type->first_param_type);
        type       = &sema->types[type_index];
    }

    LspModuleView module = {0};
    if (lsp_program_module_view_by_type(&doc->program, type, &module)) {
        lsp_completion_add_module_exports(arena, items, module);
        return;
    }

    const Lexer* lexer = &doc->front_end.lexer;
    if (type->kind == STK_String) {
        lsp_completion_add_string_members(arena, items);
    } else if (type->kind == STK_Slice) {
        lsp_completion_add_slice_members(arena, items);
    } else if (type->kind == STK_Array) {
        lsp_completion_add_array_members(arena, items);
    } else if (type->kind == STK_DynamicArray) {
        lsp_completion_add_dynamic_array_members(arena, items);
        lsp_completion_add(arena, items, s("size"), 5); // Field
    } else if (type->kind == STK_Tuple) {
        for (u32 i = 0; i < type->param_count; ++i) {
            lsp_completion_add(arena, items, string_format(arena, "%u", i), 5);
        }
    } else if (type->kind == STK_Plex || type->kind == STK_Union) {
        for (u32 i = 0; i < type->param_count; ++i) {
            u32 param_index = type->first_param_type + i;
            u32 symbol      = U32_MAX;
            if (!lsp_sema_type_param(sema, param_index, &symbol, NULL)) {
                continue;
            }
            if (symbol != U32_MAX) {
                lsp_completion_add(arena, items, lex_symbol(lexer, symbol), 5);
            }
        }
    }

    for (u32 i = 0; i < array_count(sema->methods); ++i) {
        const SemaMethod* method = &sema->methods[i];
        if (!lsp_completion_method_matches_receiver(
                doc, method, receiver_type, type_index)) {
            continue;
        }

        lsp_completion_add_unique(
            arena, items, lex_symbol(lexer, method->symbol_handle), 2);
    }

    for (u32 module_index = 0; module_index < array_count(doc->program.modules);
         ++module_index) {
        LspModuleView type_module = {0};
        if (!lsp_program_module_view(
                &doc->program, module_index, &type_module)) {
            continue;
        }

        for (u32 i = 0; i < array_count(type_module.sema->methods); ++i) {
            const SemaMethod* method = &type_module.sema->methods[i];
            if (!lsp_completion_module_method_matches_receiver(
                    doc, &type_module, method, receiver_type, type_index)) {
                continue;
            }

            lsp_completion_add_unique(
                arena,
                items,
                lex_symbol(type_module.lexer, method->symbol_handle),
                2);
        }
    }
}

internal string lsp_completion_repair_member_line(Arena* arena,
                                                  string source,
                                                  usize  offset,
                                                  string receiver)
{
    usize line_start = offset;
    while (line_start > 0 && source.data[line_start - 1] != '\n') {
        line_start--;
    }

    usize line_end = offset;
    while (line_end < source.count && source.data[line_end] != '\n') {
        line_end++;
    }

    usize indent_end = line_start;
    while (indent_end < line_end && (source.data[indent_end] == ' ' ||
                                     source.data[indent_end] == '\t')) {
        indent_end++;
    }

    StringBuilder sb = {0};
    sb_init(&sb, arena);
    sb_append_string(&sb, (string){.data = source.data, .count = line_start});
    sb_append_string(&sb,
                     (string){.data  = source.data + line_start,
                              .count = indent_end - line_start});
    sb_append_cstr(&sb, "_ := ");
    sb_append_string(&sb, receiver);
    sb_append_cstr(&sb, ".__nerd_completion_probe");
    sb_append_string(&sb,
                     (string){.data  = source.data + line_end,
                              .count = source.count - line_end});
    return sb_to_string(&sb);
}

internal void lsp_completion_add_ast_on_payload_members(Arena*       arena,
                                                        JsonValue*   items,
                                                        const Lexer* lexer,
                                                        const Ast*   ast,
                                                        string       receiver);
internal void lsp_completion_add_ast_members(Arena*             arena,
                                             JsonValue*         items,
                                             const LspDocument* doc,
                                             string             receiver,
                                             usize              offset);
internal void lsp_completion_add_imported_ast_methods(Arena*             arena,
                                                      JsonValue*         items,
                                                      const LspDocument* doc,
                                                      string             uri,
                                                      string receiver,
                                                      usize  offset);
internal bool lsp_completion_source_module_path_for_binding(Arena*  arena,
                                                            string  source,
                                                            string  receiver,
                                                            string* out_path);
internal bool lsp_completion_resolve_text_module(Arena*             arena,
                                                 const LspDocument* doc,
                                                 string             module_path,
                                                 string current_source_path,
                                                 cstr*  out_path);
internal void lsp_completion_add_resolved_module_exports(Arena*     arena,
                                                         JsonValue* items,
                                                         const LspDocument* doc,
                                                         cstr resolved_path);
internal bool
lsp_completion_match_ident_at(string source, usize* cursor, string ident);
internal bool   lsp_completion_receiver_is_single_ident(string receiver);
internal string lsp_completion_trim(string value);
internal string lsp_completion_strip_comment(string line);
internal void   lsp_completion_update_impl_depth(string line, i32* impl_depth);
internal void
lsp_completion_add_qualified_ast_on_payload_members(Arena*             arena,
                                                    JsonValue*         items,
                                                    const LspDocument* doc,
                                                    string             uri,
                                                    string receiver);
internal void
lsp_completion_add_source_on_payload_members(Arena*             arena,
                                             JsonValue*         items,
                                             const LspDocument* doc,
                                             string             uri,
                                             usize              offset,
                                             string             receiver);

internal void lsp_completion_add_repaired_members(Arena*             arena,
                                                  JsonValue*         items,
                                                  const LspDocument* doc,
                                                  string             uri,
                                                  usize              offset,
                                                  string             receiver)
{
    Arena temp = {0};
    arena_init(&temp);

    string repaired =
        lsp_completion_repair_member_line(&temp, doc->source, offset, receiver);

    ErrorRenderMode previous_mode = error_system_mode();
    bool            previous_emit = error_system_should_emit_output();
    error_system_set_mode(ERROR_RENDER_DIAGNOSTICS);
    error_system_set_emit_output(false);
    FrontEndOptions options = {
        .verbose              = false,
        .release              = false,
        .require_entry_point  = false,
        .skip_hir_generation  = true,
        .keep_partial_results = true,
    };

    ProgramInfo program = {0};
    bool        analysis_ok =
        front_end_program((NerdSource){.source = repaired, .source_path = uri},
                          &options,
                          NULL,
                          &program);
    error_system_set_mode(previous_mode);
    error_system_set_emit_output(previous_emit);

    if (array_count(program.modules) > 0 &&
        program.root_module_index < array_count(program.modules)) {
        LspDocument repaired_doc = {
            .source      = repaired,
            .front_end   = program.modules[program.root_module_index].front_end,
            .program     = program,
            .analysis_ok = analysis_ok,
            .source_ready = true,
            .tokens_ready = front_end_product_is_available(
                program.modules[program.root_module_index]
                    .front_end.readiness.lexer),
            .syntax_ready = front_end_product_is_available(
                program.modules[program.root_module_index]
                    .front_end.readiness.ast),
            .decls_ready = front_end_product_is_available(
                program.modules[program.root_module_index]
                    .front_end.readiness.semantic.declarations),
            .bindings_ready = front_end_product_is_available(
                program.modules[program.root_module_index]
                    .front_end.readiness.semantic.bindings),
            .type_facts_partial = front_end_product_is_available(
                program.modules[program.root_module_index]
                    .front_end.readiness.semantic.type_facts),
            .type_facts_complete = front_end_product_is_complete(
                program.modules[program.root_module_index]
                    .front_end.readiness.semantic.type_facts),
            .sema_partial = front_end_product_is_available(
                program.modules[program.root_module_index]
                    .front_end.readiness.sema),
            .sema_complete = front_end_product_is_complete(
                program.modules[program.root_module_index]
                    .front_end.readiness.sema),
        };
        for (u32 i = 0; i < array_count(program.modules); ++i) {
            program.modules[i].front_end.sema.program = &program;
        }
        repaired_doc.front_end.sema.program = &program;
        lsp_completion_add_members(
            arena,
            items,
            &repaired_doc,
            lsp_completion_type_for_receiver(&repaired_doc, receiver, offset));
        lsp_completion_add_imported_ast_methods(
            arena, items, &repaired_doc, uri, receiver, offset);
        if (array_count(items->array.values) == 0) {
            lsp_completion_add_ast_members(
                arena, items, &repaired_doc, receiver, offset);
        }
        if (array_count(items->array.values) == 0) {
            lsp_completion_add_ast_on_payload_members(
                arena,
                items,
                &repaired_doc.front_end.lexer,
                &repaired_doc.front_end.ast,
                receiver);
        }
        if (array_count(items->array.values) == 0) {
            lsp_completion_add_qualified_ast_on_payload_members(
                arena, items, &repaired_doc, uri, receiver);
        }
        if (array_count(items->array.values) == 0) {
            lsp_completion_add_source_on_payload_members(
                arena, items, &repaired_doc, uri, offset, receiver);
        }
    }

    program_info_done(&program);
    arena_done(&temp);
}

internal u32 lsp_completion_ast_type_symbol_with_self(const Lexer* lexer,
                                                      const Ast*   ast,
                                                      u32 type_node_index,
                                                      u32 self_symbol)
{
    if (type_node_index >= array_count(ast->nodes)) {
        return U32_MAX;
    }

    const AstNode* type_node = &ast->nodes[type_node_index];
    if (type_node->kind == AK_Expression || type_node->kind == AK_Statement) {
        return lsp_completion_ast_type_symbol_with_self(
            lexer, ast, type_node->a, self_symbol);
    }
    if (type_node->kind == AK_SymbolRef) {
        if (self_symbol != U32_MAX &&
            string_eq(lex_symbol(lexer, type_node->a), s("Self"))) {
            return self_symbol;
        }
        return type_node->a;
    }
    if (type_node->kind == AK_TypePointer) {
        return lsp_completion_ast_type_symbol_with_self(
            lexer, ast, type_node->a, self_symbol);
    }
    return U32_MAX;
}

internal u32 lsp_completion_ast_type_symbol(const Lexer* lexer,
                                            const Ast*   ast,
                                            u32          type_node_index)
{
    return lsp_completion_ast_type_symbol_with_self(
        lexer, ast, type_node_index, U32_MAX);
}

internal u32 lsp_completion_ast_collection_item_type_symbol(const Lexer* lexer,
                                                            const Ast*   ast,
                                                            u32 type_node_index,
                                                            u32 self_symbol)
{
    if (type_node_index >= array_count(ast->nodes)) {
        return U32_MAX;
    }

    const AstNode* type_node = &ast->nodes[type_node_index];
    if (type_node->kind == AK_Expression || type_node->kind == AK_Statement) {
        return lsp_completion_ast_collection_item_type_symbol(
            lexer, ast, type_node->a, self_symbol);
    }
    if (type_node->kind == AK_TypePointer) {
        return lsp_completion_ast_collection_item_type_symbol(
            lexer, ast, type_node->a, self_symbol);
    }
    if (type_node->kind == AK_TypeDynamicArray ||
        type_node->kind == AK_TypeSlice || type_node->kind == AK_TypeArray) {
        return lsp_completion_ast_type_symbol_with_self(
            lexer, ast, type_node->b, self_symbol);
    }
    return U32_MAX;
}

internal u32 lsp_completion_ast_impl_self_symbol_at_offset(
    const LspDocument* doc, usize offset)
{
    const Lexer* lexer = &doc->front_end.lexer;
    const Ast*   ast   = &doc->front_end.ast;

    for (u32 impl_index = 0; impl_index < array_count(ast->impls);
         ++impl_index) {
        const AstImplInfo* impl = &ast->impls[impl_index];
        if (impl->body_node_index >= array_count(ast->nodes)) {
            continue;
        }

        u32 impl_node_index = U32_MAX;
        for (u32 node_index = 0; node_index < array_count(ast->nodes);
             ++node_index) {
            const AstNode* node = &ast->nodes[node_index];
            if (node->kind == AK_Impl && node->a == impl_index) {
                impl_node_index = node_index;
                break;
            }
        }
        if (impl_node_index == U32_MAX) {
            continue;
        }

        const AstNode* impl_node = &ast->nodes[impl_node_index];
        if (impl_node->token_index >= array_count(lexer->tokens) ||
            lexer->tokens[impl_node->token_index].offset > offset) {
            continue;
        }

        usize          end  = lexer->source.source.count;
        const AstNode* body = &ast->nodes[impl->body_node_index];
        if (body->kind == AK_Block) {
            end = 0;
            for (u32 node_index = body->a;
                 node_index < body->b && node_index < array_count(ast->nodes);
                 ++node_index) {
                const AstNode* node = &ast->nodes[node_index];
                if (node->token_index < array_count(lexer->tokens)) {
                    end = MAX(end,
                              lex_token_end_offset(
                                  lexer, &lexer->tokens[node->token_index]));
                }
            }
        }
        if (end != 0 && offset > end) {
            continue;
        }

        return lsp_completion_ast_type_symbol(
            lexer, ast, impl->target_type_node_index);
    }

    return U32_MAX;
}

internal u32 lsp_completion_ast_field_type_node_for_symbol(
    const LspDocument* doc, u32 type_symbol, u32 field_symbol)
{
    const Ast* ast = &doc->front_end.ast;
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind != AK_Bind || node->a != type_symbol ||
            node->b >= array_count(ast->nodes)) {
            continue;
        }

        const AstNode* value = &ast->nodes[node->b];
        if (value->kind != AK_TypePlex ||
            value->a >= array_count(ast->plex_types)) {
            continue;
        }

        const AstPlexTypeInfo* plex = &ast->plex_types[value->a];
        for (u32 field_index = 0; field_index < plex->field_count;
             ++field_index) {
            u32 ast_field_index = plex->first_field + field_index;
            if (ast_field_index >= array_count(ast->plex_fields)) {
                break;
            }

            const AstPlexField* field = &ast->plex_fields[ast_field_index];
            if (field->symbol_handle == field_symbol) {
                return field->type_node_index;
            }
        }
    }
    return U32_MAX;
}

internal u32 lsp_completion_ast_field_type_node_for_name(const LspDocument* doc,
                                                         u32    type_symbol,
                                                         string field_name)
{
    const Lexer* lexer = &doc->front_end.lexer;
    const Ast*   ast   = &doc->front_end.ast;
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind != AK_Bind || node->a != type_symbol ||
            node->b >= array_count(ast->nodes)) {
            continue;
        }

        const AstNode* value = &ast->nodes[node->b];
        if (value->kind != AK_TypePlex ||
            value->a >= array_count(ast->plex_types)) {
            continue;
        }

        const AstPlexTypeInfo* plex = &ast->plex_types[value->a];
        for (u32 field_index = 0; field_index < plex->field_count;
             ++field_index) {
            u32 ast_field_index = plex->first_field + field_index;
            if (ast_field_index >= array_count(ast->plex_fields)) {
                break;
            }

            const AstPlexField* field = &ast->plex_fields[ast_field_index];
            if (field->symbol_handle != U32_MAX &&
                string_eq(lex_symbol(lexer, field->symbol_handle),
                          field_name)) {
                return field->type_node_index;
            }
        }
    }
    return U32_MAX;
}

internal u32 lsp_completion_ast_receiver_type_symbol(const LspDocument* doc,
                                                     string receiver,
                                                     usize  offset);

internal u32 lsp_completion_ast_iterable_item_type_symbol(
    const LspDocument* doc, const AstForInfo* for_info, usize offset)
{
    const Lexer* lexer = &doc->front_end.lexer;
    const Ast*   ast   = &doc->front_end.ast;
    u32          self_symbol =
        lsp_completion_ast_impl_self_symbol_at_offset(doc, offset);

    if (for_info->iterable_node_index >= array_count(ast->nodes)) {
        return U32_MAX;
    }

    const AstNode* iterable = &ast->nodes[for_info->iterable_node_index];
    if (iterable->kind == AK_Field && iterable->a < array_count(ast->nodes)) {
        const AstNode* receiver_node = &ast->nodes[iterable->a];
        if (receiver_node->kind == AK_SymbolRef) {
            string receiver_name = lex_symbol(lexer, receiver_node->a);
            u32    receiver_type = lsp_completion_ast_receiver_type_symbol(
                doc, receiver_name, offset);
            if (receiver_type != U32_MAX) {
                u32 field_type_node =
                    lsp_completion_ast_field_type_node_for_symbol(
                        doc, receiver_type, iterable->b);
                return lsp_completion_ast_collection_item_type_symbol(
                    lexer, ast, field_type_node, self_symbol);
            }
        }
    }

    if (iterable->kind == AK_SymbolRef) {
        u32 type_symbol = lsp_completion_ast_receiver_type_symbol(
            doc, lex_symbol(lexer, iterable->a), offset);
        if (type_symbol != U32_MAX) {
            for (u32 i = 0; i < array_count(ast->nodes); ++i) {
                const AstNode* node = &ast->nodes[i];
                if (node->kind != AK_Bind || node->a != type_symbol ||
                    node->b >= array_count(ast->nodes)) {
                    continue;
                }
                return lsp_completion_ast_collection_item_type_symbol(
                    lexer, ast, node->b, self_symbol);
            }
        }
    }

    return U32_MAX;
}

internal u32 lsp_completion_ast_receiver_type_symbol(const LspDocument* doc,
                                                     string receiver,
                                                     usize  offset)
{
    const Lexer* lexer     = &doc->front_end.lexer;
    const Ast*   ast       = &doc->front_end.ast;

    u32   best_type_symbol = U32_MAX;
    usize best_offset      = 0;

    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind != AK_Variable && node->kind != AK_Bind) {
            continue;
        }
        if (node->a == U32_MAX ||
            !string_eq(lex_symbol(lexer, node->a), receiver)) {
            continue;
        }
        if (node->b >= array_count(ast->nodes)) {
            continue;
        }
        if (node->token_index >= array_count(lexer->tokens)) {
            continue;
        }
        usize node_offset = lexer->tokens[node->token_index].offset;
        if (node_offset > offset || node_offset < best_offset) {
            continue;
        }

        const AstNode* value = &ast->nodes[node->b];
        if (value->kind == AK_AnnotatedValue || value->kind == AK_ZeroInit ||
            value->kind == AK_Undefined) {
            u32 type_symbol =
                lsp_completion_ast_type_symbol(lexer, ast, value->a);
            if (type_symbol != U32_MAX) {
                best_type_symbol = type_symbol;
                best_offset      = node_offset;
            }
        }
    }

    for (u32 for_node_index = 0; for_node_index < array_count(ast->nodes);
         ++for_node_index) {
        const AstNode* for_node = &ast->nodes[for_node_index];
        if (for_node->kind != AK_For || for_node->a >= array_count(ast->fors)) {
            continue;
        }

        const AstForInfo* for_info = &ast->fors[for_node->a];
        if (for_info->item_symbol == U32_MAX ||
            !string_eq(lex_symbol(lexer, for_info->item_symbol), receiver) ||
            for_info->item_token_index >= array_count(lexer->tokens)) {
            continue;
        }

        usize item_offset = lexer->tokens[for_info->item_token_index].offset;
        if (item_offset > offset || item_offset < best_offset) {
            continue;
        }

        u32 type_symbol =
            lsp_completion_ast_iterable_item_type_symbol(doc, for_info, offset);
        if (type_symbol != U32_MAX) {
            best_type_symbol = type_symbol;
            best_offset      = item_offset;
        }
    }

    for (u32 signature_index = 0;
         signature_index < array_count(ast->fn_signatures);
         ++signature_index) {
        const AstFnSignature* signature = &ast->fn_signatures[signature_index];
        for (u32 i = 0; i < signature->param_count; ++i) {
            u32 param_index = signature->first_param + i;
            if (param_index >= array_count(ast->params)) {
                break;
            }
            const AstParam* param = &ast->params[param_index];
            if (param->symbol_handle == U32_MAX ||
                !string_eq(lex_symbol(lexer, param->symbol_handle), receiver) ||
                param->token_index >= array_count(lexer->tokens)) {
                continue;
            }
            usize param_offset = lexer->tokens[param->token_index].offset;
            if (param_offset > offset || param_offset < best_offset) {
                continue;
            }

            u32 self_symbol =
                lsp_completion_ast_impl_self_symbol_at_offset(doc, offset);
            u32 type_symbol = lsp_completion_ast_type_symbol_with_self(
                lexer, ast, param->type_node_index, self_symbol);
            if (type_symbol != U32_MAX) {
                best_type_symbol = type_symbol;
                best_offset      = param_offset;
            }
        }
    }

    if (best_type_symbol != U32_MAX) {
        return best_type_symbol;
    }

    return U32_MAX;
}

internal void lsp_completion_add_ast_members(Arena*             arena,
                                             JsonValue*         items,
                                             const LspDocument* doc,
                                             string             receiver,
                                             usize              offset)
{
    const Lexer* lexer = &doc->front_end.lexer;
    const Ast*   ast   = &doc->front_end.ast;

    for (usize dot = 0; dot < receiver.count; ++dot) {
        if (receiver.data[dot] != '.') {
            continue;
        }

        string owner = {.data = receiver.data, .count = dot};
        string field = {.data  = receiver.data + dot + 1,
                        .count = receiver.count - dot - 1};
        if (owner.count == 0 || field.count == 0) {
            return;
        }

        u32 owner_type =
            lsp_completion_ast_receiver_type_symbol(doc, owner, offset);
        if (owner_type == U32_MAX) {
            return;
        }

        u32 field_type_node =
            lsp_completion_ast_field_type_node_for_name(doc, owner_type, field);
        if (field_type_node >= array_count(ast->nodes)) {
            return;
        }

        const AstNode* field_type = &ast->nodes[field_type_node];
        if (field_type->kind == AK_TypeDynamicArray) {
            lsp_completion_add_dynamic_array_members(arena, items);
        }
        return;
    }

    u32 type_symbol =
        lsp_completion_ast_receiver_type_symbol(doc, receiver, offset);
    if (type_symbol == U32_MAX) {
        return;
    }

    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind != AK_Bind || node->a != type_symbol ||
            node->b >= array_count(ast->nodes)) {
            continue;
        }

        const AstNode* value = &ast->nodes[node->b];
        if (value->kind != AK_TypePlex) {
            continue;
        }
        if (value->a >= array_count(ast->plex_types)) {
            return;
        }

        const AstPlexTypeInfo* plex = &ast->plex_types[value->a];
        for (u32 field_index = 0; field_index < plex->field_count;
             ++field_index) {
            u32 ast_field_index = plex->first_field + field_index;
            if (ast_field_index >= array_count(ast->plex_fields)) {
                break;
            }

            const AstPlexField* field = &ast->plex_fields[ast_field_index];
            if (field->symbol_handle != U32_MAX) {
                lsp_completion_add(
                    arena, items, lex_symbol(lexer, field->symbol_handle), 5);
            }
        }
        return;
    }
}

internal bool lsp_completion_impl_node(const Ast*      ast,
                                       u32             impl_index,
                                       const AstNode** out_node)
{
    for (u32 node_index = 0; node_index < array_count(ast->nodes);
         ++node_index) {
        const AstNode* node = &ast->nodes[node_index];
        if (node->kind == AK_Impl && node->a == impl_index) {
            *out_node = node;
            return true;
        }
    }
    return false;
}

internal bool lsp_completion_source_receiver_has_binding(string source,
                                                         string receiver,
                                                         usize  offset)
{
    usize limit = MIN(offset, source.count);
    for (usize i = 0; i + receiver.count < limit; ++i) {
        if (i > 0 && lsp_completion_is_ident_char(source.data[i - 1])) {
            continue;
        }
        if (memcmp(source.data + i, receiver.data, receiver.count) != 0) {
            continue;
        }
        usize cursor = i + receiver.count;
        if (cursor < limit &&
            lsp_completion_is_ident_char(source.data[cursor])) {
            continue;
        }
        while (cursor < limit &&
               (source.data[cursor] == ' ' || source.data[cursor] == '\t')) {
            cursor++;
        }
        if (cursor < limit && source.data[cursor] == ':') {
            return true;
        }
    }
    return false;
}

internal bool lsp_completion_source_receiver_type_name(string  source,
                                                       string  receiver,
                                                       usize   offset,
                                                       string* out_type_name)
{
    usize limit = MIN(offset, source.count);
    for (usize i = 0; i + receiver.count < limit; ++i) {
        if (i > 0 && lsp_completion_is_ident_char(source.data[i - 1])) {
            continue;
        }
        if (memcmp(source.data + i, receiver.data, receiver.count) != 0) {
            continue;
        }
        usize cursor = i + receiver.count;
        if (cursor < limit &&
            lsp_completion_is_ident_char(source.data[cursor])) {
            continue;
        }
        while (cursor < limit &&
               (source.data[cursor] == ' ' || source.data[cursor] == '\t')) {
            cursor++;
        }
        if (cursor >= limit || source.data[cursor] != ':') {
            continue;
        }
        cursor++;
        bool inferred = false;
        while (cursor < limit &&
               (source.data[cursor] == ' ' || source.data[cursor] == '\t')) {
            cursor++;
        }
        if (cursor < limit && source.data[cursor] == '=') {
            inferred = true;
            cursor++;
        }
        while (cursor < limit &&
               (source.data[cursor] == ' ' || source.data[cursor] == '\t' ||
                source.data[cursor] == '^')) {
            cursor++;
        }

        usize start = cursor;
        while (cursor < limit &&
               lsp_completion_is_ident_char(source.data[cursor])) {
            cursor++;
        }
        if (cursor > start) {
            usize type_end = cursor;
            while (cursor < limit && (source.data[cursor] == ' ' ||
                                      source.data[cursor] == '\t')) {
                cursor++;
            }
            if (!inferred || (cursor < limit && source.data[cursor] == '.')) {
                *out_type_name = (string){.data  = source.data + start,
                                          .count = type_end - start};
                return true;
            }
        }
    }
    return out_type_name->count != 0;
}

internal bool lsp_completion_add_source_collection_members(JsonValue* items,
                                                           Arena*     arena,
                                                           string     source,
                                                           usize      offset,
                                                           string     receiver)
{
    usize line_start = 0;
    bool  added      = false;
    while (line_start < source.count && line_start < offset) {
        usize line_end = line_start;
        while (line_end < source.count && source.data[line_end] != '\n') {
            line_end++;
        }

        string line = {.data  = source.data + line_start,
                       .count = MIN(line_end, offset) - line_start};
        line        = lsp_completion_trim(lsp_completion_strip_comment(line));

        usize i     = 0;
        if (!lsp_completion_match_ident_at(line, &i, receiver)) {
            line_start = line_end + (line_end < source.count ? 1 : 0);
            continue;
        }
        while (i < line.count &&
               (line.data[i] == ' ' || line.data[i] == '\t')) {
            i++;
        }
        if (i >= line.count || line.data[i] != ':') {
            line_start = line_end + (line_end < source.count ? 1 : 0);
            continue;
        }
        i++;

        bool constant_decl = false;
        if (i < line.count && line.data[i] == ':') {
            constant_decl = true;
            i++;
        }
        while (i < line.count &&
               (line.data[i] == ' ' || line.data[i] == '\t')) {
            i++;
        }
        if (i >= line.count || line.data[i] != '[') {
            line_start = line_end + (line_end < source.count ? 1 : 0);
            continue;
        }
        i++;

        if (constant_decl) {
            lsp_completion_add_array_members(arena, items);
            added = true;
        } else if (i + 1 < line.count && line.data[i] == '.' &&
                   line.data[i + 1] == '.') {
            lsp_completion_add_dynamic_array_members(arena, items);
            lsp_completion_add(arena, items, s("size"), 5); // Field
            added = true;
        } else if (i < line.count && line.data[i] == ']') {
            lsp_completion_add_slice_members(arena, items);
            added = true;
        } else {
            usize digit_start = i;
            while (i < line.count && line.data[i] >= '0' &&
                   line.data[i] <= '9') {
                i++;
            }
            if (i > digit_start && i < line.count && line.data[i] == ']') {
                lsp_completion_add_array_members(arena, items);
                added = true;
            }
        }

        line_start = line_end + (line_end < source.count ? 1 : 0);
    }
    return added;
}

internal bool lsp_completion_line_use_path(string line, string* out_path)
{
    usize i = 0;
    if (lsp_completion_match_ident_at(line, &i, s("pub"))) {
        while (i < line.count &&
               (line.data[i] == ' ' || line.data[i] == '\t')) {
            i++;
        }
    }

    usize checkpoint = i;
    if (!lsp_completion_match_ident_at(line, &i, s("use"))) {
        i = checkpoint;
        while (i < line.count && lsp_completion_is_ident_char(line.data[i])) {
            i++;
        }
        while (i < line.count &&
               (line.data[i] == ' ' || line.data[i] == '\t')) {
            i++;
        }
        if (i + 2 > line.count || line.data[i] != ':' ||
            line.data[i + 1] != ':') {
            return false;
        }
        i += 2;
        while (i < line.count &&
               (line.data[i] == ' ' || line.data[i] == '\t')) {
            i++;
        }
        if (!lsp_completion_match_ident_at(line, &i, s("use"))) {
            return false;
        }
    }

    while (i < line.count && (line.data[i] == ' ' || line.data[i] == '\t')) {
        i++;
    }
    usize start = i;
    while (i < line.count && (lsp_completion_is_ident_char(line.data[i]) ||
                              line.data[i] == '.')) {
        i++;
    }
    if (i == start) {
        return false;
    }
    *out_path = (string){.data = line.data + start, .count = i - start};
    return true;
}

internal void lsp_completion_update_impl_depth(string line, i32* impl_depth)
{
    for (usize j = 0; j < line.count; ++j) {
        if (line.data[j] == '{') {
            (*impl_depth)++;
        } else if (line.data[j] == '}') {
            (*impl_depth)--;
        }
    }
}

internal void lsp_completion_add_text_impl_methods(Arena*     arena,
                                                   JsonValue* items,
                                                   string     source,
                                                   string     type_name,
                                                   bool       associated_only)
{
    bool  in_impl     = false;
    bool  impl_public = false;
    i32   impl_depth  = 0;
    usize line_start  = 0;
    while (line_start < source.count) {
        usize line_end = line_start;
        while (line_end < source.count && source.data[line_end] != '\n') {
            line_end++;
        }
        string line = {.data  = source.data + line_start,
                       .count = line_end - line_start};
        line        = lsp_completion_trim(lsp_completion_strip_comment(line));

        if (!in_impl) {
            usize i     = 0;
            impl_public = lsp_completion_match_ident_at(line, &i, s("pub"));
            if (impl_public) {
                while (i < line.count &&
                       (line.data[i] == ' ' || line.data[i] == '\t')) {
                    i++;
                }
            }
            if (!lsp_completion_match_ident_at(line, &i, s("impl"))) {
                line_start = line_end + (line_end < source.count ? 1 : 0);
                continue;
            }
            while (i < line.count &&
                   (line.data[i] == ' ' || line.data[i] == '\t')) {
                i++;
            }
            if (lsp_completion_match_ident_at(line, &i, type_name)) {
                in_impl    = true;
                impl_depth = 0;
                for (usize j = 0; j < line.count; ++j) {
                    if (line.data[j] == '{') {
                        impl_depth++;
                    } else if (line.data[j] == '}') {
                        impl_depth--;
                    }
                }
                if (impl_depth <= 0) {
                    impl_depth = 1;
                }
            }
            line_start = line_end + (line_end < source.count ? 1 : 0);
            continue;
        }

        if (line.count > 0 && line.data[0] == '}') {
            lsp_completion_update_impl_depth(line, &impl_depth);
            if (impl_depth <= 0) {
                in_impl = false;
            }
            line_start = line_end + (line_end < source.count ? 1 : 0);
            continue;
        }

        usize i             = 0;
        bool  member_public = impl_public;
        if (lsp_completion_match_ident_at(line, &i, s("pub"))) {
            member_public = true;
            while (i < line.count &&
                   (line.data[i] == ' ' || line.data[i] == '\t')) {
                i++;
            }
        }
        if (!member_public) {
            lsp_completion_update_impl_depth(line, &impl_depth);
            if (impl_depth <= 0) {
                in_impl = false;
            }
            line_start = line_end + (line_end < source.count ? 1 : 0);
            continue;
        }

        usize name_start = i;
        while (i < line.count && lsp_completion_is_ident_char(line.data[i])) {
            i++;
        }
        usize name_end = i;
        while (i < line.count &&
               (line.data[i] == ' ' || line.data[i] == '\t')) {
            i++;
        }
        if (i + 2 <= line.count && line.data[i] == ':' &&
            line.data[i + 1] == ':') {
            i += 2;
        } else {
            lsp_completion_update_impl_depth(line, &impl_depth);
            if (impl_depth <= 0) {
                in_impl = false;
            }
            line_start = line_end + (line_end < source.count ? 1 : 0);
            continue;
        }
        while (i < line.count &&
               (line.data[i] == ' ' || line.data[i] == '\t')) {
            i++;
        }
        if (!lsp_completion_match_ident_at(line, &i, s("fn"))) {
            lsp_completion_update_impl_depth(line, &impl_depth);
            if (impl_depth <= 0) {
                in_impl = false;
            }
            line_start = line_end + (line_end < source.count ? 1 : 0);
            continue;
        }

        if (associated_only) {
            while (i < line.count &&
                   (line.data[i] == ' ' || line.data[i] == '\t')) {
                i++;
            }
            if (i >= line.count || line.data[i] != '(') {
                lsp_completion_update_impl_depth(line, &impl_depth);
                if (impl_depth <= 0) {
                    in_impl = false;
                }
                line_start = line_end + (line_end < source.count ? 1 : 0);
                continue;
            }
            i++;
            while (i < line.count &&
                   (line.data[i] == ' ' || line.data[i] == '\t')) {
                i++;
            }

            bool first_param_is_receiver = false;
            if (i < line.count && line.data[i] != ')') {
                while (i < line.count && line.data[i] != ':' &&
                       line.data[i] != ')' && line.data[i] != ',') {
                    i++;
                }
                if (i < line.count && line.data[i] == ':') {
                    i++;
                    while (i < line.count &&
                           (line.data[i] == ' ' || line.data[i] == '\t' ||
                            line.data[i] == '^')) {
                        i++;
                    }
                    first_param_is_receiver =
                        lsp_completion_match_ident_at(line, &i, s("Self"));
                }
                while (i < line.count && line.data[i] != ')') {
                    i++;
                }
            }
            if (i >= line.count || line.data[i] != ')') {
                lsp_completion_update_impl_depth(line, &impl_depth);
                if (impl_depth <= 0) {
                    in_impl = false;
                }
                line_start = line_end + (line_end < source.count ? 1 : 0);
                continue;
            }
            i++;
            while (i < line.count &&
                   (line.data[i] == ' ' || line.data[i] == '\t')) {
                i++;
            }
            if (i + 2 > line.count || line.data[i] != '-' ||
                line.data[i + 1] != '>') {
                lsp_completion_update_impl_depth(line, &impl_depth);
                if (impl_depth <= 0) {
                    in_impl = false;
                }
                line_start = line_end + (line_end < source.count ? 1 : 0);
                continue;
            }
            i += 2;
            while (i < line.count &&
                   (line.data[i] == ' ' || line.data[i] == '\t' ||
                    line.data[i] == '^')) {
                i++;
            }
            if (first_param_is_receiver ||
                !lsp_completion_match_ident_at(line, &i, s("Self"))) {
                lsp_completion_update_impl_depth(line, &impl_depth);
                if (impl_depth <= 0) {
                    in_impl = false;
                }
                line_start = line_end + (line_end < source.count ? 1 : 0);
                continue;
            }
        }

        if (name_end > name_start) {
            lsp_completion_add_unique(arena,
                                      items,
                                      (string){.data  = line.data + name_start,
                                               .count = name_end - name_start},
                                      2);
        }
        lsp_completion_update_impl_depth(line, &impl_depth);
        if (impl_depth <= 0) {
            in_impl = false;
        }
        line_start = line_end + (line_end < source.count ? 1 : 0);
    }
}

internal bool
lsp_completion_add_text_impl_methods_from_file(Arena*     arena,
                                               JsonValue* items,
                                               cstr       path,
                                               string     type_name,
                                               bool       associated_only)
{
    FileMap map    = {0};
    string  source = filemap_load(path, &map);
    if (source.data == NULL) {
        return false;
    }

    usize before = array_count(items->array.values);
    lsp_completion_add_text_impl_methods(
        arena, items, source, type_name, associated_only);
    filemap_unload(&map);
    return array_count(items->array.values) > before;
}

internal bool
lsp_completion_add_text_impl_methods_from_module(Arena*     arena,
                                                 JsonValue* items,
                                                 cstr       resolved,
                                                 string     type_name,
                                                 bool       associated_only)
{
    bool added = lsp_completion_add_text_impl_methods_from_file(
        arena, items, resolved, type_name, associated_only);
    if (!string_eq(path_filename(s(resolved)), s("mod.n"))) {
        return added;
    }

    Arena temp = {0};
    arena_init(&temp);
    cstr module_dir        = path_dirname(&temp, resolved);

    Array(cstr) part_paths = NULL;
    DirIter iter           = {0};
    if (dir_iter_init(&iter, module_dir)) {
        cstr path         = NULL;
        bool is_directory = false;
        while (dir_iter_next(&iter, &temp, &path, &is_directory)) {
            if (!is_directory &&
                lsp_completion_path_is_module_part_file(path)) {
                array_push(part_paths, path);
            }
        }
        dir_iter_done(&iter);
    }

    if (array_count(part_paths) > 1) {
        qsort(part_paths,
              array_count(part_paths),
              sizeof(part_paths[0]),
              lsp_completion_compare_cstr_ptr);
    }

    for (u32 i = 0; i < array_count(part_paths); ++i) {
        if (lsp_completion_add_text_impl_methods_from_file(
                arena, items, part_paths[i], type_name, associated_only)) {
            added = true;
        }
    }

    array_free(part_paths);
    arena_done(&temp);
    return added;
}

internal void
lsp_completion_add_text_impl_methods_from_uses(Arena*             arena,
                                               JsonValue*         items,
                                               const LspDocument* doc,
                                               string             uri,
                                               string             type_name,
                                               bool associated_only)
{
    Arena temp = {0};
    arena_init(&temp);

    (void)uri;
    string current_path = {0};
    usize  line_start   = 0;
    while (line_start < doc->source.count) {
        usize line_end = line_start;
        while (line_end < doc->source.count &&
               doc->source.data[line_end] != '\n') {
            line_end++;
        }
        string line = {.data  = doc->source.data + line_start,
                       .count = line_end - line_start};
        line        = lsp_completion_trim(lsp_completion_strip_comment(line));

        string module_path = {0};
        cstr   resolved    = NULL;
        if (lsp_completion_line_use_path(line, &module_path) &&
            lsp_completion_resolve_text_module(
                &temp, doc, module_path, current_path, &resolved)) {
            (void)lsp_completion_add_text_impl_methods_from_module(
                arena, items, resolved, type_name, associated_only);
        }

        line_start = line_end + (line_end < doc->source.count ? 1 : 0);
    }

    arena_done(&temp);
}

internal void lsp_completion_add_imported_ast_methods(Arena*             arena,
                                                      JsonValue*         items,
                                                      const LspDocument* doc,
                                                      string             uri,
                                                      string receiver,
                                                      usize  offset)
{
    const Lexer* lexer = &doc->front_end.lexer;
    u32          type_symbol =
        lsp_completion_ast_receiver_type_symbol(doc, receiver, offset);
    string type_name = {0};
    if (type_symbol != U32_MAX) {
        type_name = lex_symbol(lexer, type_symbol);
    } else if (!lsp_completion_source_receiver_type_name(
                   doc->source, receiver, offset, &type_name)) {
        return;
    }
    for (u32 module_index = 0; module_index < array_count(doc->program.modules);
         ++module_index) {
        LspModuleView module = {0};
        if (!lsp_program_module_view(&doc->program, module_index, &module)) {
            continue;
        }

        for (u32 impl_index = 0; impl_index < array_count(module.ast->impls);
             ++impl_index) {
            const AstImplInfo* impl          = &module.ast->impls[impl_index];
            u32                target_symbol = lsp_completion_ast_type_symbol(
                module.lexer, module.ast, impl->target_type_node_index);
            if (target_symbol == U32_MAX ||
                !string_eq(lex_symbol(module.lexer, target_symbol),
                           type_name)) {
                continue;
            }

            const AstNode* impl_node = NULL;
            if (!lsp_completion_impl_node(module.ast, impl_index, &impl_node) ||
                impl->body_node_index >= array_count(module.ast->nodes)) {
                continue;
            }

            const AstNode* body = &module.ast->nodes[impl->body_node_index];
            if (body->kind != AK_Block) {
                continue;
            }

            bool impl_public = ast_has_flag(impl_node, ANF_Public);
            for (u32 item = body->a;
                 item < body->b && item < array_count(module.ast->nodes);
                 ++item) {
                const AstNode* member = &module.ast->nodes[item];
                if (!ast_node_is_binding_like(member) ||
                    (!impl_public && !ast_has_flag(member, ANF_Public))) {
                    continue;
                }

                u32 symbol = ast_get_symbol(member);
                if (symbol != U32_MAX) {
                    lsp_completion_add_unique(
                        arena, items, lex_symbol(module.lexer, symbol), 2);
                }
            }
        }
    }

    lsp_completion_add_text_impl_methods_from_uses(
        arena, items, doc, uri, type_name, false);
}

internal bool lsp_completion_ast_method_is_associated(const Lexer*       lexer,
                                                      const Ast*         ast,
                                                      const AstImplInfo* impl,
                                                      const AstNode*     member)
{
    if (member->kind != AK_Bind || member->b >= array_count(ast->nodes)) {
        return false;
    }

    u32 target_symbol = lsp_completion_ast_type_symbol(
        lexer, ast, impl->target_type_node_index);
    if (target_symbol == U32_MAX) {
        return false;
    }

    const AstNode* value = &ast->nodes[member->b];
    if (value->kind == AK_AnnotatedValue) {
        if (value->b >= array_count(ast->nodes)) {
            return false;
        }
        value = &ast->nodes[value->b];
    }
    if (value->kind != AK_FnDef || value->a >= array_count(ast->nodes)) {
        return false;
    }

    const AstNode* fn_start = &ast->nodes[value->a];
    if (fn_start->a >= array_count(ast->fn_signatures)) {
        return false;
    }

    const AstFnSignature* signature = &ast->fn_signatures[fn_start->a];
    bool                  first_param_is_receiver = false;
    if (signature->param_count > 0 &&
        signature->first_param < array_count(ast->params)) {
        const AstParam* first_param      = &ast->params[signature->first_param];
        u32             first_param_type = lsp_completion_ast_type_symbol(
            lexer, ast, first_param->type_node_index);
        first_param_is_receiver =
            first_param_type != U32_MAX &&
            string_eq(lex_symbol(lexer, first_param_type), s("Self"));
    }

    if (signature->return_type_node_index == U32_MAX) {
        return false;
    }
    u32 return_type = lsp_completion_ast_type_symbol_with_self(
        lexer, ast, signature->return_type_node_index, target_symbol);
    return return_type == target_symbol && !first_param_is_receiver;
}

internal void lsp_completion_add_associated_ast_methods_from_module(
    Arena*               arena,
    JsonValue*           items,
    const LspModuleView* module,
    string               type_name)
{
    for (u32 impl_index = 0; impl_index < array_count(module->ast->impls);
         ++impl_index) {
        const AstImplInfo* impl          = &module->ast->impls[impl_index];
        u32                target_symbol = lsp_completion_ast_type_symbol(
            module->lexer, module->ast, impl->target_type_node_index);
        if (target_symbol == U32_MAX ||
            !string_eq(lex_symbol(module->lexer, target_symbol), type_name)) {
            continue;
        }

        const AstNode* impl_node = NULL;
        if (!lsp_completion_impl_node(module->ast, impl_index, &impl_node) ||
            impl->body_node_index >= array_count(module->ast->nodes)) {
            continue;
        }

        const AstNode* body = &module->ast->nodes[impl->body_node_index];
        if (body->kind != AK_Block) {
            continue;
        }

        bool impl_public = ast_has_flag(impl_node, ANF_Public);
        for (u32 item = body->a;
             item < body->b && item < array_count(module->ast->nodes);
             ++item) {
            const AstNode* member = &module->ast->nodes[item];
            if (!ast_node_is_binding_like(member) ||
                (!impl_public && !ast_has_flag(member, ANF_Public)) ||
                !lsp_completion_ast_method_is_associated(
                    module->lexer, module->ast, impl, member)) {
                continue;
            }

            u32 symbol = ast_get_symbol(member);
            if (symbol != U32_MAX) {
                lsp_completion_add_unique(
                    arena, items, lex_symbol(module->lexer, symbol), 2);
            }
        }
    }
}

internal void lsp_completion_add_associated_methods(Arena*             arena,
                                                    JsonValue*         items,
                                                    const LspDocument* doc,
                                                    string             uri,
                                                    string             receiver)
{
    if (!lsp_completion_receiver_is_single_ident(receiver)) {
        return;
    }

    for (u32 module_index = 0; module_index < array_count(doc->program.modules);
         ++module_index) {
        LspModuleView module = {0};
        if (!lsp_program_module_view(&doc->program, module_index, &module)) {
            continue;
        }
        lsp_completion_add_associated_ast_methods_from_module(
            arena, items, &module, receiver);
    }

    lsp_completion_add_text_impl_methods_from_uses(
        arena, items, doc, uri, receiver, true);
}

internal bool lsp_completion_ast_pattern_matches_receiver(const Lexer* lexer,
                                                          const Ast*   ast,
                                                          u32    pattern_index,
                                                          string receiver)
{
    if (pattern_index >= array_count(ast->patterns)) {
        return false;
    }

    const AstPattern* pattern = &ast->patterns[pattern_index];
    if (pattern->kind == APK_Bind && pattern->a != U32_MAX) {
        return string_eq(lex_symbol(lexer, pattern->a), receiver);
    }
    if (pattern->kind == APK_Value && pattern->a < array_count(ast->nodes)) {
        const AstNode* value = &ast->nodes[pattern->a];
        return value->kind == AK_SymbolRef && value->a != U32_MAX &&
               string_eq(lex_symbol(lexer, value->a), receiver);
    }
    if (pattern->kind == APK_Tuple) {
        for (u32 i = 0; i < pattern->b; ++i) {
            if (lsp_completion_ast_pattern_matches_receiver(
                    lexer, ast, ast->pattern_items[pattern->a + i], receiver)) {
                return true;
            }
        }
    }
    if (pattern->kind == APK_Plex) {
        for (u32 i = 0; i < pattern->b; ++i) {
            if (lsp_completion_ast_pattern_matches_receiver(
                    lexer,
                    ast,
                    ast->pattern_fields[pattern->a + i].pattern_index,
                    receiver)) {
                return true;
            }
        }
    }
    return false;
}

internal u32 lsp_completion_ast_symbol_type_node(const Lexer* lexer,
                                                 const Ast*   ast,
                                                 u32          symbol)
{
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind != AK_Variable || node->a != symbol ||
            node->b >= array_count(ast->nodes)) {
            continue;
        }
        const AstNode* value = &ast->nodes[node->b];
        if (value->kind == AK_AnnotatedValue || value->kind == AK_ZeroInit ||
            value->kind == AK_Undefined) {
            return value->a;
        }
    }
    (void)lexer;
    return U32_MAX;
}

internal const AstNode* lsp_completion_ast_find_type_bind(const Ast* ast,
                                                          u32        symbol)
{
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind == AK_Bind && node->a == symbol &&
            node->b < array_count(ast->nodes)) {
            return &ast->nodes[node->b];
        }
    }
    return NULL;
}

internal u32 lsp_completion_ast_enum_payload_symbol(const Ast* ast,
                                                    u32        enum_symbol,
                                                    u32        variant_symbol)
{
    const AstNode* value = lsp_completion_ast_find_type_bind(ast, enum_symbol);
    if (value == NULL || value->kind != AK_TypeEnum ||
        value->a >= array_count(ast->enum_types)) {
        return U32_MAX;
    }

    const AstEnumTypeInfo* enum_type = &ast->enum_types[value->a];
    for (u32 i = 0; i < enum_type->variant_count; ++i) {
        const AstEnumVariant* variant =
            &ast->enum_variants[enum_type->first_variant + i];
        if (variant->symbol_handle != variant_symbol ||
            variant->type_node_index >= array_count(ast->nodes)) {
            continue;
        }

        const AstNode* payload = &ast->nodes[variant->type_node_index];
        if (payload->kind == AK_SymbolRef) {
            return payload->a;
        }
    }
    return U32_MAX;
}

internal bool lsp_completion_seen_name(Array(string) seen, string name);

internal void lsp_completion_add_ast_plex_fields_for_symbol(Arena*       arena,
                                                            JsonValue*   items,
                                                            const Lexer* lexer,
                                                            const Ast*   ast,
                                                            u32 type_symbol)
{
    const AstNode* value = lsp_completion_ast_find_type_bind(ast, type_symbol);
    if (value == NULL || value->kind != AK_TypePlex ||
        value->a >= array_count(ast->plex_types)) {
        return;
    }

    const AstPlexTypeInfo* plex = &ast->plex_types[value->a];
    for (u32 i = 0; i < plex->field_count; ++i) {
        const AstPlexField* field = &ast->plex_fields[plex->first_field + i];
        if (field->symbol_handle != U32_MAX) {
            lsp_completion_add(
                arena, items, lex_symbol(lexer, field->symbol_handle), 5);
        }
    }
}

internal bool
lsp_completion_add_ast_plex_literal_fields_for_name(Arena*       arena,
                                                    JsonValue*   items,
                                                    const Lexer* lexer,
                                                    const Ast*   ast,
                                                    string       type_name,
                                                    Array(string) seen)
{
    usize before = array_count(items->array.values);
    for (u32 node_index = 0; node_index < array_count(ast->nodes);
         ++node_index) {
        const AstNode* node = &ast->nodes[node_index];
        if (node->kind != AK_Bind || node->a == U32_MAX ||
            !string_eq(lex_symbol(lexer, node->a), type_name) ||
            node->b >= array_count(ast->nodes)) {
            continue;
        }

        const AstNode* value = &ast->nodes[node->b];
        if (value->kind != AK_TypePlex ||
            value->a >= array_count(ast->plex_types)) {
            continue;
        }

        const AstPlexTypeInfo* plex = &ast->plex_types[value->a];
        for (u32 field_index = 0; field_index < plex->field_count;
             ++field_index) {
            u32 ast_field_index = plex->first_field + field_index;
            if (ast_field_index >= array_count(ast->plex_fields)) {
                break;
            }

            const AstPlexField* field = &ast->plex_fields[ast_field_index];
            if (field->symbol_handle == U32_MAX) {
                continue;
            }
            string name = lex_symbol(lexer, field->symbol_handle);
            if (!lsp_completion_seen_name(seen, name)) {
                lsp_completion_add_plex_literal_field(arena, items, name);
            }
        }
        break;
    }
    return array_count(items->array.values) > before;
}

internal void lsp_completion_add_ast_on_payload_members(Arena*       arena,
                                                        JsonValue*   items,
                                                        const Lexer* lexer,
                                                        const Ast*   ast,
                                                        string       receiver)
{
    for (u32 node_index = 0; node_index < array_count(ast->nodes);
         ++node_index) {
        const AstNode* node = &ast->nodes[node_index];
        if (node->kind != AK_On || node->a >= array_count(ast->nodes) ||
            node->b >= array_count(ast->ons)) {
            continue;
        }

        const AstNode* scrutinee = &ast->nodes[node->a];
        if (scrutinee->kind != AK_SymbolRef) {
            continue;
        }

        u32 type_node =
            lsp_completion_ast_symbol_type_node(lexer, ast, scrutinee->a);
        if (type_node >= array_count(ast->nodes) ||
            ast->nodes[type_node].kind != AK_SymbolRef) {
            continue;
        }
        u32 enum_symbol     = ast->nodes[type_node].a;

        const AstOnInfo* on = &ast->ons[node->b];
        for (u32 branch_index = 0; branch_index < on->branch_count;
             ++branch_index) {
            const AstOnBranch* branch =
                &ast->on_branches[on->first_branch + branch_index];
            if (branch->flags & AOBF_Else) {
                continue;
            }
            for (u32 pattern = 0; pattern < branch->pattern_count; ++pattern) {
                u32 pattern_index =
                    ast->pattern_items[branch->pattern_index + pattern];
                if (pattern_index >= array_count(ast->patterns) ||
                    ast->patterns[pattern_index].kind != APK_EnumVariant) {
                    continue;
                }

                const AstEnumPattern* enum_pattern =
                    &ast->enum_patterns[ast->patterns[pattern_index].a];
                bool matched = false;
                for (u32 item = 0; item < enum_pattern->pattern_count; ++item) {
                    if (lsp_completion_ast_pattern_matches_receiver(
                            lexer,
                            ast,
                            ast->pattern_items[enum_pattern->first_pattern +
                                               item],
                            receiver)) {
                        matched = true;
                        break;
                    }
                }
                if (!matched) {
                    continue;
                }

                u32 payload_symbol = lsp_completion_ast_enum_payload_symbol(
                    ast, enum_symbol, enum_pattern->symbol_handle);
                if (payload_symbol != U32_MAX) {
                    lsp_completion_add_ast_plex_fields_for_symbol(
                        arena, items, lexer, ast, payload_symbol);
                    return;
                }
            }
        }
    }
}

internal bool lsp_completion_add_payload_fields_from_ast(Arena*       arena,
                                                         JsonValue*   items,
                                                         const Lexer* lexer,
                                                         const Ast*   ast,
                                                         string       enum_name,
                                                         string variant_name)
{
    u32 enum_symbol    = U32_MAX;
    u32 variant_symbol = U32_MAX;
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind == AK_Bind && node->a != U32_MAX &&
            string_eq(lex_symbol(lexer, node->a), enum_name)) {
            enum_symbol = node->a;
            break;
        }
    }
    for (u32 type_index = 0; type_index < array_count(ast->enum_types);
         ++type_index) {
        const AstEnumTypeInfo* enum_type = &ast->enum_types[type_index];
        for (u32 i = 0; i < enum_type->variant_count; ++i) {
            const AstEnumVariant* variant =
                &ast->enum_variants[enum_type->first_variant + i];
            if (variant->symbol_handle != U32_MAX &&
                string_eq(lex_symbol(lexer, variant->symbol_handle),
                          variant_name)) {
                variant_symbol = variant->symbol_handle;
                break;
            }
        }
        if (variant_symbol != U32_MAX) {
            break;
        }
    }
    if (enum_symbol == U32_MAX || variant_symbol == U32_MAX) {
        return false;
    }

    u32 payload_symbol = lsp_completion_ast_enum_payload_symbol(
        ast, enum_symbol, variant_symbol);
    if (payload_symbol == U32_MAX) {
        return false;
    }

    usize before = array_count(items->array.values);
    lsp_completion_add_ast_plex_fields_for_symbol(
        arena, items, lexer, ast, payload_symbol);
    return array_count(items->array.values) > before;
}

internal string lsp_completion_strip_comment(string line)
{
    for (usize i = 0; i + 1 < line.count; ++i) {
        if (line.data[i] == '-' && line.data[i + 1] == '-') {
            return (string){.data = line.data, .count = i};
        }
    }
    return line;
}

internal bool
lsp_completion_line_starts_decl(string line, string name, string keyword)
{
    line    = lsp_completion_trim(lsp_completion_strip_comment(line));
    usize i = 0;
    if (lsp_completion_match_ident_at(line, &i, s("pub"))) {
        while (i < line.count &&
               (line.data[i] == ' ' || line.data[i] == '\t')) {
            i++;
        }
    }
    if (!lsp_completion_match_ident_at(line, &i, name)) {
        return false;
    }
    while (i < line.count && (line.data[i] == ' ' || line.data[i] == '\t')) {
        i++;
    }
    if (i + 2 > line.count || line.data[i] != ':' || line.data[i + 1] != ':') {
        return false;
    }
    i += 2;
    while (i < line.count && (line.data[i] == ' ' || line.data[i] == '\t')) {
        i++;
    }
    return lsp_completion_match_ident_at(line, &i, keyword);
}

internal bool lsp_completion_text_enum_payload_type(Arena*  arena,
                                                    string  source,
                                                    string  enum_name,
                                                    string  variant_name,
                                                    string* out_payload_type)
{
    usize line_start = 0;
    bool  in_enum    = false;
    while (line_start < source.count) {
        usize line_end = line_start;
        while (line_end < source.count && source.data[line_end] != '\n') {
            line_end++;
        }
        string line = {.data  = source.data + line_start,
                       .count = line_end - line_start};
        line        = lsp_completion_trim(lsp_completion_strip_comment(line));

        if (!in_enum) {
            if (lsp_completion_line_starts_decl(line, enum_name, s("enum"))) {
                in_enum = true;
            }
            line_start = line_end + (line_end < source.count ? 1 : 0);
            continue;
        }

        if (line.count > 0 && line.data[0] == '}') {
            return false;
        }

        usize i = 0;
        if (!lsp_completion_match_ident_at(line, &i, variant_name)) {
            line_start = line_end + (line_end < source.count ? 1 : 0);
            continue;
        }
        while (i < line.count &&
               (line.data[i] == ' ' || line.data[i] == '\t')) {
            i++;
        }
        if (i >= line.count || line.data[i] != '(') {
            return false;
        }
        i++;
        while (i < line.count &&
               (line.data[i] == ' ' || line.data[i] == '\t')) {
            i++;
        }
        usize start = i;
        while (i < line.count && lsp_completion_is_ident_char(line.data[i])) {
            i++;
        }
        if (i == start) {
            return false;
        }
        u8* copy = arena_alloc(arena, i - start);
        memcpy(copy, line.data + start, i - start);
        *out_payload_type = (string){.data = copy, .count = i - start};
        return true;
    }
    return false;
}

internal bool lsp_completion_add_text_plex_fields(Arena*     arena,
                                                  JsonValue* items,
                                                  string     source,
                                                  string     type_name)
{
    usize line_start = 0;
    bool  in_plex    = false;
    usize before     = array_count(items->array.values);
    while (line_start < source.count) {
        usize line_end = line_start;
        while (line_end < source.count && source.data[line_end] != '\n') {
            line_end++;
        }
        string line = {.data  = source.data + line_start,
                       .count = line_end - line_start};
        line        = lsp_completion_trim(lsp_completion_strip_comment(line));

        if (!in_plex) {
            if (lsp_completion_line_starts_decl(line, type_name, s("plex"))) {
                in_plex = true;
            }
            line_start = line_end + (line_end < source.count ? 1 : 0);
            continue;
        }

        if (line.count > 0 && line.data[0] == '}') {
            break;
        }
        usize i = 0;
        if (lsp_completion_match_ident_at(line, &i, s("pub"))) {
            while (i < line.count &&
                   (line.data[i] == ' ' || line.data[i] == '\t')) {
                i++;
            }
        }
        usize start = i;
        while (i < line.count && lsp_completion_is_ident_char(line.data[i])) {
            i++;
        }
        if (i > start) {
            lsp_completion_add(
                arena,
                items,
                (string){.data = line.data + start, .count = i - start},
                5);
        }
        line_start = line_end + (line_end < source.count ? 1 : 0);
    }
    return array_count(items->array.values) > before;
}

internal bool lsp_completion_add_text_enum_variants(Arena*     arena,
                                                    JsonValue* items,
                                                    string     source,
                                                    string     type_name)
{
    usize line_start = 0;
    bool  in_enum    = false;
    usize before     = array_count(items->array.values);
    while (line_start < source.count) {
        usize line_end = line_start;
        while (line_end < source.count && source.data[line_end] != '\n') {
            line_end++;
        }
        string line = {.data  = source.data + line_start,
                       .count = line_end - line_start};
        line        = lsp_completion_trim(lsp_completion_strip_comment(line));

        if (!in_enum) {
            if (lsp_completion_line_starts_decl(line, type_name, s("enum"))) {
                in_enum = true;
            }
            line_start = line_end + (line_end < source.count ? 1 : 0);
            continue;
        }

        if (line.count > 0 && line.data[0] == '}') {
            break;
        }
        usize i = 0;
        if (lsp_completion_match_ident_at(line, &i, s("pub"))) {
            while (i < line.count &&
                   (line.data[i] == ' ' || line.data[i] == '\t')) {
                i++;
            }
        }
        usize start = i;
        while (i < line.count && lsp_completion_is_ident_char(line.data[i])) {
            i++;
        }
        if (i > start) {
            lsp_completion_add(
                arena,
                items,
                (string){.data = line.data + start, .count = i - start},
                20);
        }
        line_start = line_end + (line_end < source.count ? 1 : 0);
    }
    return array_count(items->array.values) > before;
}

internal bool lsp_completion_add_text_enum_variants_from_file(Arena*     arena,
                                                              JsonValue* items,
                                                              cstr       path,
                                                              string type_name)
{
    FileMap map    = {0};
    string  source = filemap_load(path, &map);
    if (source.data == NULL) {
        return false;
    }

    bool added =
        lsp_completion_add_text_enum_variants(arena, items, source, type_name);
    filemap_unload(&map);
    return added;
}

internal bool lsp_completion_add_text_enum_variants_from_module(
    Arena* arena, JsonValue* items, cstr resolved, string type_name)
{
    if (lsp_completion_add_text_enum_variants_from_file(
            arena, items, resolved, type_name)) {
        return true;
    }
    if (!string_eq(path_filename(s(resolved)), s("mod.n"))) {
        return false;
    }

    Arena temp = {0};
    arena_init(&temp);
    cstr module_dir        = path_dirname(&temp, resolved);

    Array(cstr) part_paths = NULL;
    DirIter iter           = {0};
    if (dir_iter_init(&iter, module_dir)) {
        cstr path         = NULL;
        bool is_directory = false;
        while (dir_iter_next(&iter, &temp, &path, &is_directory)) {
            if (!is_directory &&
                lsp_completion_path_is_module_part_file(path)) {
                array_push(part_paths, path);
            }
        }
        dir_iter_done(&iter);
    }

    if (array_count(part_paths) > 1) {
        qsort(part_paths,
              array_count(part_paths),
              sizeof(part_paths[0]),
              lsp_completion_compare_cstr_ptr);
    }

    bool added = false;
    for (u32 i = 0; i < array_count(part_paths); ++i) {
        if (lsp_completion_add_text_enum_variants_from_file(
                arena, items, part_paths[i], type_name)) {
            added = true;
            break;
        }
    }

    array_free(part_paths);
    arena_done(&temp);
    return added;
}

internal bool lsp_completion_seen_name(Array(string) seen, string name)
{
    for (u32 i = 0; i < array_count(seen); ++i) {
        if (string_eq(seen[i], name)) {
            return true;
        }
    }
    return false;
}

internal bool lsp_completion_add_text_plex_literal_fields(Arena*     arena,
                                                          JsonValue* items,
                                                          string     source,
                                                          string     type_name,
                                                          Array(string) seen)
{
    usize line_start = 0;
    bool  in_plex    = false;
    usize before     = array_count(items->array.values);
    while (line_start < source.count) {
        usize line_end = line_start;
        while (line_end < source.count && source.data[line_end] != '\n') {
            line_end++;
        }
        string line = {.data  = source.data + line_start,
                       .count = line_end - line_start};
        line        = lsp_completion_trim(lsp_completion_strip_comment(line));

        if (!in_plex) {
            if (lsp_completion_line_starts_decl(line, type_name, s("plex"))) {
                in_plex = true;
            }
            line_start = line_end + (line_end < source.count ? 1 : 0);
            continue;
        }

        if (line.count > 0 && line.data[0] == '}') {
            break;
        }

        usize i = 0;
        if (lsp_completion_match_ident_at(line, &i, s("pub"))) {
            while (i < line.count &&
                   (line.data[i] == ' ' || line.data[i] == '\t')) {
                i++;
            }
        }
        usize start = i;
        while (i < line.count && lsp_completion_is_ident_char(line.data[i])) {
            i++;
        }
        if (i > start) {
            string name = {.data = line.data + start, .count = i - start};
            if (!lsp_completion_seen_name(seen, name)) {
                lsp_completion_add_plex_literal_field(arena, items, name);
            }
        }
        line_start = line_end + (line_end < source.count ? 1 : 0);
    }
    return array_count(items->array.values) > before;
}

internal bool lsp_completion_source_impl_type_before(Arena*  arena,
                                                     string  source,
                                                     usize   offset,
                                                     string* out_type)
{
    usize line_start = 0;
    bool  found      = false;
    while (line_start < source.count && line_start < offset) {
        usize line_end = line_start;
        while (line_end < source.count && source.data[line_end] != '\n') {
            line_end++;
        }
        string line = {.data  = source.data + line_start,
                       .count = MIN(line_end, offset) - line_start};
        line        = lsp_completion_trim(lsp_completion_strip_comment(line));

        usize i     = 0;
        if (lsp_completion_match_ident_at(line, &i, s("impl"))) {
            while (i < line.count &&
                   (line.data[i] == ' ' || line.data[i] == '\t')) {
                i++;
            }
            usize start = i;
            while (i < line.count &&
                   lsp_completion_is_ident_char(line.data[i])) {
                i++;
            }
            if (i > start) {
                u8* copy = arena_alloc(arena, i - start);
                memcpy(copy, line.data + start, i - start);
                *out_type = (string){.data = copy, .count = i - start};
                found     = true;
            }
        }

        line_start = line_end + (line_end < source.count ? 1 : 0);
    }
    return found;
}

internal bool lsp_completion_source_param_type_before(Arena*  arena,
                                                      string  source,
                                                      usize   offset,
                                                      string  receiver,
                                                      string* out_type)
{
    usize line_start = 0;
    bool  found      = false;
    while (line_start < source.count && line_start < offset) {
        usize line_end = line_start;
        while (line_end < source.count && source.data[line_end] != '\n') {
            line_end++;
        }
        string line = {.data  = source.data + line_start,
                       .count = MIN(line_end, offset) - line_start};
        line        = lsp_completion_trim(lsp_completion_strip_comment(line));

        for (usize i = 0; i < line.count; ++i) {
            if (!lsp_completion_match_ident_at(line, &i, receiver)) {
                continue;
            }
            while (i < line.count &&
                   (line.data[i] == ' ' || line.data[i] == '\t')) {
                i++;
            }
            if (i >= line.count || line.data[i] != ':') {
                continue;
            }
            i++;
            while (i < line.count &&
                   (line.data[i] == ' ' || line.data[i] == '\t')) {
                i++;
            }
            if (i < line.count && line.data[i] == '^') {
                i++;
            }
            if (i < line.count && line.data[i] == '(') {
                i++;
            }
            usize start = i;
            while (i < line.count &&
                   lsp_completion_is_ident_char(line.data[i])) {
                i++;
            }
            if (i > start) {
                u8* copy = arena_alloc(arena, i - start);
                memcpy(copy, line.data + start, i - start);
                *out_type = (string){.data = copy, .count = i - start};
                found     = true;
            }
        }

        line_start = line_end + (line_end < source.count ? 1 : 0);
    }
    return found;
}

internal bool
lsp_completion_source_field_collection_item_type(Arena*  arena,
                                                 string  source,
                                                 string  owner_type,
                                                 string  field_name,
                                                 string* out_type)
{
    usize line_start = 0;
    bool  in_plex    = false;
    while (line_start < source.count) {
        usize line_end = line_start;
        while (line_end < source.count && source.data[line_end] != '\n') {
            line_end++;
        }
        string line = {.data  = source.data + line_start,
                       .count = line_end - line_start};
        line        = lsp_completion_trim(lsp_completion_strip_comment(line));

        if (!in_plex) {
            if (lsp_completion_line_starts_decl(line, owner_type, s("plex"))) {
                in_plex = true;
            }
            line_start = line_end + (line_end < source.count ? 1 : 0);
            continue;
        }

        if (line.count > 0 && line.data[0] == '}') {
            return false;
        }

        usize i = 0;
        if (!lsp_completion_match_ident_at(line, &i, field_name)) {
            line_start = line_end + (line_end < source.count ? 1 : 0);
            continue;
        }
        while (i < line.count &&
               (line.data[i] == ' ' || line.data[i] == '\t')) {
            i++;
        }
        if (i + 3 > line.count || line.data[i] != '[' ||
            line.data[i + 1] != '.' || line.data[i + 2] != '.') {
            return false;
        }
        i += 3;
        while (i < line.count && line.data[i] != ']') {
            i++;
        }
        if (i >= line.count) {
            return false;
        }
        i++;
        while (i < line.count &&
               (line.data[i] == ' ' || line.data[i] == '\t')) {
            i++;
        }
        usize start = i;
        while (i < line.count && lsp_completion_is_ident_char(line.data[i])) {
            i++;
        }
        if (i == start) {
            return false;
        }

        u8* copy = arena_alloc(arena, i - start);
        memcpy(copy, line.data + start, i - start);
        *out_type = (string){.data = copy, .count = i - start};
        return true;
    }
    return false;
}

internal bool lsp_completion_source_for_item_type_before(Arena*  arena,
                                                         string  source,
                                                         usize   offset,
                                                         string  receiver,
                                                         string* out_type)
{
    usize line_start = 0;
    bool  found      = false;
    while (line_start < source.count && line_start < offset) {
        usize line_end = line_start;
        while (line_end < source.count && source.data[line_end] != '\n') {
            line_end++;
        }
        string line = {.data  = source.data + line_start,
                       .count = MIN(line_end, offset) - line_start};
        line        = lsp_completion_trim(lsp_completion_strip_comment(line));

        usize i     = 0;
        if (lsp_completion_match_ident_at(line, &i, s("return"))) {
            while (i < line.count &&
                   (line.data[i] == ' ' || line.data[i] == '\t')) {
                i++;
            }
        }
        if (!lsp_completion_match_ident_at(line, &i, s("for"))) {
            line_start = line_end + (line_end < source.count ? 1 : 0);
            continue;
        }
        while (i < line.count &&
               (line.data[i] == ' ' || line.data[i] == '\t')) {
            i++;
        }
        if (i < line.count && line.data[i] == '^') {
            i++;
        }
        if (!lsp_completion_match_ident_at(line, &i, receiver)) {
            line_start = line_end + (line_end < source.count ? 1 : 0);
            continue;
        }
        while (i < line.count &&
               (line.data[i] == ' ' || line.data[i] == '\t')) {
            i++;
        }
        if (!lsp_completion_match_ident_at(line, &i, s("in"))) {
            line_start = line_end + (line_end < source.count ? 1 : 0);
            continue;
        }
        while (i < line.count &&
               (line.data[i] == ' ' || line.data[i] == '\t')) {
            i++;
        }
        usize owner_start = i;
        while (i < line.count && lsp_completion_is_ident_char(line.data[i])) {
            i++;
        }
        if (i == owner_start || i >= line.count || line.data[i] != '.') {
            line_start = line_end + (line_end < source.count ? 1 : 0);
            continue;
        }
        string owner = {.data  = line.data + owner_start,
                        .count = i - owner_start};
        i++;
        usize field_start = i;
        while (i < line.count && lsp_completion_is_ident_char(line.data[i])) {
            i++;
        }
        if (i == field_start) {
            line_start = line_end + (line_end < source.count ? 1 : 0);
            continue;
        }
        string field      = {.data  = line.data + field_start,
                             .count = i - field_start};

        string owner_type = {0};
        if (!lsp_completion_source_param_type_before(
                arena, source, line_start, owner, &owner_type)) {
            line_start = line_end + (line_end < source.count ? 1 : 0);
            continue;
        }
        if (string_eq(owner_type, s("Self"))) {
            string self_type = {0};
            if (lsp_completion_source_impl_type_before(
                    arena, source, line_start, &self_type)) {
                owner_type = self_type;
            }
        }
        if (lsp_completion_source_field_collection_item_type(
                arena, source, owner_type, field, out_type)) {
            found = true;
        }

        line_start = line_end + (line_end < source.count ? 1 : 0);
    }
    return found;
}

internal void lsp_completion_add_source_for_item_members(Arena*     arena,
                                                         JsonValue* items,
                                                         const LspDocument* doc,
                                                         usize  offset,
                                                         string receiver)
{
    Arena temp = {0};
    arena_init(&temp);

    string type_name = {0};
    if (lsp_completion_source_for_item_type_before(
            &temp, doc->source, offset, receiver, &type_name)) {
        (void)lsp_completion_add_text_plex_fields(
            arena, items, doc->source, type_name);
    }

    arena_done(&temp);
}

internal void lsp_completion_add_source_param_members(Arena*             arena,
                                                      JsonValue*         items,
                                                      const LspDocument* doc,
                                                      usize              offset,
                                                      string receiver)
{
    Arena temp = {0};
    arena_init(&temp);

    string type_name = {0};
    if (lsp_completion_source_param_type_before(
            &temp, doc->source, offset, receiver, &type_name)) {
        if (string_eq(type_name, s("Self"))) {
            string self_type = {0};
            if (lsp_completion_source_impl_type_before(
                    &temp, doc->source, offset, &self_type)) {
                type_name = self_type;
            }
        }
        (void)lsp_completion_add_text_plex_fields(
            arena, items, doc->source, type_name);
    }

    arena_done(&temp);
}

internal bool lsp_completion_add_text_plex_fields_from_file(Arena*     arena,
                                                            JsonValue* items,
                                                            cstr       path,
                                                            string type_name)
{
    FileMap map    = {0};
    string  source = filemap_load(path, &map);
    if (source.data == NULL) {
        return false;
    }

    bool added =
        lsp_completion_add_text_plex_fields(arena, items, source, type_name);
    filemap_unload(&map);
    return added;
}

internal bool lsp_completion_add_text_plex_fields_from_module(Arena*     arena,
                                                              JsonValue* items,
                                                              cstr   resolved,
                                                              string type_name)
{
    if (lsp_completion_add_text_plex_fields_from_file(
            arena, items, resolved, type_name)) {
        return true;
    }
    if (!string_eq(path_filename(s(resolved)), s("mod.n"))) {
        return false;
    }

    Arena temp = {0};
    arena_init(&temp);
    cstr module_dir        = path_dirname(&temp, resolved);

    Array(cstr) part_paths = NULL;
    DirIter iter           = {0};
    if (dir_iter_init(&iter, module_dir)) {
        cstr path         = NULL;
        bool is_directory = false;
        while (dir_iter_next(&iter, &temp, &path, &is_directory)) {
            if (!is_directory &&
                lsp_completion_path_is_module_part_file(path)) {
                array_push(part_paths, path);
            }
        }
        dir_iter_done(&iter);
    }

    if (array_count(part_paths) > 1) {
        qsort(part_paths,
              array_count(part_paths),
              sizeof(part_paths[0]),
              lsp_completion_compare_cstr_ptr);
    }

    bool added = false;
    for (u32 i = 0; i < array_count(part_paths); ++i) {
        if (lsp_completion_add_text_plex_fields_from_file(
                arena, items, part_paths[i], type_name)) {
            added = true;
            break;
        }
    }

    array_free(part_paths);
    arena_done(&temp);
    return added;
}

internal void lsp_completion_add_imported_type_members(Arena*             arena,
                                                       JsonValue*         items,
                                                       const LspDocument* doc,
                                                       string             uri,
                                                       usize  offset,
                                                       string receiver)
{
    Arena temp = {0};
    arena_init(&temp);

    string type_name = {0};
    if (!lsp_completion_source_param_type_before(
            &temp, doc->source, offset, receiver, &type_name)) {
        arena_done(&temp);
        return;
    }

    usize line_start = 0;
    while (line_start < doc->source.count) {
        usize line_end = line_start;
        while (line_end < doc->source.count &&
               doc->source.data[line_end] != '\n') {
            line_end++;
        }

        string line = {.data  = doc->source.data + line_start,
                       .count = line_end - line_start};
        line        = lsp_completion_trim(lsp_completion_strip_comment(line));

        usize i     = 0;
        if (lsp_completion_match_ident_at(line, &i, s("pub"))) {
            while (i < line.count &&
                   (line.data[i] == ' ' || line.data[i] == '\t')) {
                i++;
            }
        }
        if (!lsp_completion_match_ident_at(line, &i, s("use"))) {
            line_start = line_end + (line_end < doc->source.count ? 1 : 0);
            continue;
        }

        while (i < line.count &&
               (line.data[i] == ' ' || line.data[i] == '\t')) {
            i++;
        }
        usize path_start = i;
        while (i < line.count && (lsp_completion_is_ident_char(line.data[i]) ||
                                  line.data[i] == '.')) {
            i++;
        }
        if (i == path_start) {
            line_start = line_end + (line_end < doc->source.count ? 1 : 0);
            continue;
        }

        string module_path = {.data  = line.data + path_start,
                              .count = i - path_start};
        cstr   resolved    = NULL;
        if (lsp_completion_resolve_text_module(
                &temp, doc, module_path, uri, &resolved)) {
            if (lsp_completion_add_text_plex_fields_from_module(
                    arena, items, resolved, type_name)) {
                break;
            }
        }

        line_start = line_end + (line_end < doc->source.count ? 1 : 0);
    }

    arena_done(&temp);
}

internal void lsp_completion_add_source_symbols(Arena*     arena,
                                                JsonValue* items,
                                                string     source,
                                                usize      offset)
{
    usize line_start = 0;
    while (line_start < source.count && line_start < offset) {
        usize line_end = line_start;
        while (line_end < source.count && source.data[line_end] != '\n') {
            line_end++;
        }
        string line = {.data  = source.data + line_start,
                       .count = MIN(line_end, offset) - line_start};
        line        = lsp_completion_trim(lsp_completion_strip_comment(line));

        usize i     = 0;
        if (lsp_completion_match_ident_at(line, &i, s("pub"))) {
            while (i < line.count &&
                   (line.data[i] == ' ' || line.data[i] == '\t')) {
                i++;
            }
        }

        usize start = i;
        while (i < line.count && lsp_completion_is_ident_char(line.data[i])) {
            i++;
        }
        if (i == start) {
            line_start = line_end + (line_end < source.count ? 1 : 0);
            continue;
        }
        string label = {.data = line.data + start, .count = i - start};

        while (i < line.count &&
               (line.data[i] == ' ' || line.data[i] == '\t')) {
            i++;
        }
        if (i + 1 < line.count && line.data[i] == ':' &&
            (line.data[i + 1] == ':' || line.data[i + 1] == '=')) {
            lsp_completion_add(arena, items, label, 1);
        } else if (i < line.count && line.data[i] == ':') {
            lsp_completion_add(arena, items, label, 6);
        }

        line_start = line_end + (line_end < source.count ? 1 : 0);
    }
}

internal void lsp_completion_add_source_top_level_symbols(Arena*     arena,
                                                          JsonValue* items,
                                                          string     source)
{
    i32   depth      = 0;
    usize line_start = 0;
    while (line_start < source.count) {
        usize line_end = line_start;
        while (line_end < source.count && source.data[line_end] != '\n') {
            line_end++;
        }
        string line = {.data  = source.data + line_start,
                       .count = line_end - line_start};

        if (depth == 0) {
            string trimmed =
                lsp_completion_trim(lsp_completion_strip_comment(line));

            usize i = 0;
            if (lsp_completion_match_ident_at(trimmed, &i, s("pub"))) {
                while (i < trimmed.count &&
                       (trimmed.data[i] == ' ' || trimmed.data[i] == '\t')) {
                    i++;
                }
            }

            usize start = i;
            while (i < trimmed.count &&
                   lsp_completion_is_ident_char(trimmed.data[i])) {
                i++;
            }
            if (i > start) {
                string label = {.data  = trimmed.data + start,
                                .count = i - start};

                while (i < trimmed.count &&
                       (trimmed.data[i] == ' ' || trimmed.data[i] == '\t')) {
                    i++;
                }
                if (i + 1 < trimmed.count && trimmed.data[i] == ':' &&
                    trimmed.data[i + 1] == ':') {
                    lsp_completion_add_unique(arena, items, label, 21);
                } else if (i + 1 < trimmed.count && trimmed.data[i] == ':' &&
                           trimmed.data[i + 1] == '=') {
                    lsp_completion_add_unique(arena, items, label, 6);
                } else if (i < trimmed.count && trimmed.data[i] == ':') {
                    lsp_completion_add_unique(arena, items, label, 6);
                }
            }
        }

        lsp_completion_update_impl_depth(line, &depth);
        if (depth < 0) {
            depth = 0;
        }
        line_start = line_end + (line_end < source.count ? 1 : 0);
    }
}

internal bool lsp_completion_line_starts_string_literal(string line)
{
    line = lsp_completion_trim(lsp_completion_strip_comment(line));
    return line.count > 0 && line.data[0] == '"';
}

internal bool lsp_completion_source_top_level_string_binding(string source,
                                                             string receiver)
{
    if (!lsp_completion_receiver_is_single_ident(receiver)) {
        return false;
    }

    i32   depth      = 0;
    usize line_start = 0;
    while (line_start < source.count) {
        usize line_end = line_start;
        while (line_end < source.count && source.data[line_end] != '\n') {
            line_end++;
        }
        string line = {.data  = source.data + line_start,
                       .count = line_end - line_start};

        if (depth == 0) {
            string trimmed =
                lsp_completion_trim(lsp_completion_strip_comment(line));

            usize i = 0;
            if (lsp_completion_match_ident_at(trimmed, &i, s("pub"))) {
                while (i < trimmed.count &&
                       (trimmed.data[i] == ' ' || trimmed.data[i] == '\t')) {
                    i++;
                }
            }

            if (!lsp_completion_match_ident_at(trimmed, &i, receiver)) {
                lsp_completion_update_impl_depth(line, &depth);
                if (depth < 0) {
                    depth = 0;
                }
                line_start = line_end + (line_end < source.count ? 1 : 0);
                continue;
            }
            while (i < trimmed.count &&
                   (trimmed.data[i] == ' ' || trimmed.data[i] == '\t')) {
                i++;
            }
            if (i + 1 < trimmed.count && trimmed.data[i] == ':' &&
                trimmed.data[i + 1] == ':') {
                i += 2;
                while (i < trimmed.count &&
                       (trimmed.data[i] == ' ' || trimmed.data[i] == '\t')) {
                    i++;
                }
                if (i < trimmed.count && trimmed.data[i] == '"') {
                    return true;
                }
                if (i >= trimmed.count) {
                    usize next_start =
                        line_end + (line_end < source.count ? 1 : 0);
                    while (next_start < source.count) {
                        usize next_end = next_start;
                        while (next_end < source.count &&
                               source.data[next_end] != '\n') {
                            next_end++;
                        }
                        string next_line    = {.data  = source.data + next_start,
                                               .count = next_end - next_start};
                        string next_trimmed = lsp_completion_trim(
                            lsp_completion_strip_comment(next_line));
                        if (next_trimmed.count == 0) {
                            next_start =
                                next_end + (next_end < source.count ? 1 : 0);
                            continue;
                        }
                        return lsp_completion_line_starts_string_literal(
                            next_trimmed);
                    }
                }
            }
        }

        lsp_completion_update_impl_depth(line, &depth);
        if (depth < 0) {
            depth = 0;
        }
        line_start = line_end + (line_end < source.count ? 1 : 0);
    }
    return false;
}

internal void lsp_completion_add_source_use_exports(Arena*             arena,
                                                    JsonValue*         items,
                                                    const LspDocument* doc,
                                                    string document_uri)
{
    Arena temp = {0};
    arena_init(&temp);

    usize line_start = 0;
    while (line_start < doc->source.count) {
        usize line_end = line_start;
        while (line_end < doc->source.count &&
               doc->source.data[line_end] != '\n') {
            line_end++;
        }

        string line = {.data  = doc->source.data + line_start,
                       .count = line_end - line_start};
        line        = lsp_completion_trim(lsp_completion_strip_comment(line));

        usize i     = 0;
        if (lsp_completion_match_ident_at(line, &i, s("pub"))) {
            while (i < line.count &&
                   (line.data[i] == ' ' || line.data[i] == '\t')) {
                i++;
            }
        }
        if (!lsp_completion_match_ident_at(line, &i, s("use"))) {
            line_start = line_end + (line_end < doc->source.count ? 1 : 0);
            continue;
        }

        while (i < line.count &&
               (line.data[i] == ' ' || line.data[i] == '\t')) {
            i++;
        }

        usize path_start = i;
        while (i < line.count && (lsp_completion_is_ident_char(line.data[i]) ||
                                  line.data[i] == '.')) {
            i++;
        }
        if (i == path_start) {
            line_start = line_end + (line_end < doc->source.count ? 1 : 0);
            continue;
        }

        string module_path   = {.data  = line.data + path_start,
                                .count = i - path_start};
        cstr   resolved_path = NULL;
        if (lsp_completion_resolve_text_module(
                &temp, doc, module_path, document_uri, &resolved_path)) {
            lsp_completion_add_resolved_module_exports(
                arena, items, doc, resolved_path);
        }

        line_start = line_end + (line_end < doc->source.count ? 1 : 0);
    }

    arena_done(&temp);
}

internal void lsp_completion_add_source_params_from_line(Arena*     arena,
                                                         JsonValue* items,
                                                         string     line)
{
    line    = lsp_completion_trim(lsp_completion_strip_comment(line));
    usize i = 0;
    while (i < line.count) {
        if (lsp_completion_match_ident_at(line, &i, s("fn"))) {
            break;
        }
        i++;
    }
    if (i >= line.count) {
        return;
    }
    while (i < line.count && (line.data[i] == ' ' || line.data[i] == '\t')) {
        i++;
    }
    if (i >= line.count || line.data[i] != '(') {
        return;
    }
    i++;

    while (i < line.count) {
        while (i < line.count && (line.data[i] == ' ' || line.data[i] == '\t' ||
                                  line.data[i] == ',')) {
            i++;
        }
        if (i >= line.count || line.data[i] == ')') {
            return;
        }

        usize start = i;
        while (i < line.count && lsp_completion_is_ident_char(line.data[i])) {
            i++;
        }
        if (i == start) {
            i++;
            continue;
        }

        string label = {.data = line.data + start, .count = i - start};
        while (i < line.count &&
               (line.data[i] == ' ' || line.data[i] == '\t')) {
            i++;
        }
        if (i < line.count && line.data[i] == ':') {
            lsp_completion_add(arena, items, label, 6);
        }

        while (i < line.count && line.data[i] != ',' && line.data[i] != ')') {
            i++;
        }
    }
}

internal void lsp_completion_add_source_params(Arena*     arena,
                                               JsonValue* items,
                                               string     source,
                                               usize      offset)
{
    usize line_start = 0;
    while (line_start < source.count && line_start < offset) {
        usize line_end = line_start;
        while (line_end < source.count && source.data[line_end] != '\n') {
            line_end++;
        }
        string line = {.data  = source.data + line_start,
                       .count = MIN(line_end, offset) - line_start};
        lsp_completion_add_source_params_from_line(arena, items, line);
        line_start = line_end + (line_end < source.count ? 1 : 0);
    }
}

internal bool lsp_completion_add_payload_fields_from_text(Arena*     arena,
                                                          JsonValue* items,
                                                          string     source,
                                                          string     enum_name,
                                                          string variant_name)
{
    Arena temp = {0};
    arena_init(&temp);
    string payload_type = {0};
    bool   added        = false;
    if (lsp_completion_text_enum_payload_type(
            &temp, source, enum_name, variant_name, &payload_type)) {
        added = lsp_completion_add_text_plex_fields(
            arena, items, source, payload_type);
    }
    arena_done(&temp);
    return added;
}

internal bool lsp_completion_add_payload_fields_from_file(Arena*     arena,
                                                          JsonValue* items,
                                                          cstr       path,
                                                          string     enum_name,
                                                          string variant_name)
{
    FileMap map    = {0};
    string  source = filemap_load(path, &map);
    if (source.data == NULL) {
        return false;
    }

    bool            added         = false;
    ErrorRenderMode previous_mode = error_system_mode();
    bool            previous_emit = error_system_should_emit_output();
    error_system_set_mode(ERROR_RENDER_DIAGNOSTICS);
    error_system_set_emit_output(false);

    Lexer lexer = {0};
    if (lex((NerdSource){.source = source, .source_path = s(path)}, &lexer)) {
        Ast ast = ast_parse(&lexer);
        if (array_count(ast.nodes) > 0) {
            added = lsp_completion_add_payload_fields_from_ast(
                arena, items, &lexer, &ast, enum_name, variant_name);
        }
        ast_done(&ast);
    }
    lex_done(&lexer);

    if (!added) {
        added = lsp_completion_add_payload_fields_from_text(
            arena, items, source, enum_name, variant_name);
    }

    error_system_set_mode(previous_mode);
    error_system_set_emit_output(previous_emit);
    filemap_unload(&map);
    return added;
}

internal void lsp_completion_add_module_payload_members(Arena*     arena,
                                                        JsonValue* items,
                                                        const LspDocument* doc,
                                                        string             uri,
                                                        string module_binding,
                                                        string enum_name,
                                                        string variant_name)
{
    Arena temp = {0};
    arena_init(&temp);

    string module_path = {0};
    cstr   resolved    = NULL;
    if (!lsp_completion_source_module_path_for_binding(
            &temp, doc->source, module_binding, &module_path) ||
        !lsp_completion_resolve_text_module(
            &temp, doc, module_path, uri, &resolved)) {
        arena_done(&temp);
        return;
    }

    if (lsp_completion_add_payload_fields_from_file(
            arena, items, resolved, enum_name, variant_name)) {
        arena_done(&temp);
        return;
    }

    if (string_eq(path_filename(s(resolved)), s("mod.n"))) {
        cstr    module_dir = path_dirname(&temp, resolved);
        DirIter iter       = {0};
        if (dir_iter_init(&iter, module_dir)) {
            cstr path         = NULL;
            bool is_directory = false;
            while (dir_iter_next(&iter, &temp, &path, &is_directory)) {
                if (!is_directory &&
                    lsp_completion_path_is_module_part_file(path) &&
                    lsp_completion_add_payload_fields_from_file(
                        arena, items, path, enum_name, variant_name)) {
                    break;
                }
            }
            dir_iter_done(&iter);
        }
    }

    arena_done(&temp);
}

internal void
lsp_completion_add_qualified_ast_on_payload_members(Arena*             arena,
                                                    JsonValue*         items,
                                                    const LspDocument* doc,
                                                    string             uri,
                                                    string             receiver)
{
    const Lexer* lexer = &doc->front_end.lexer;
    const Ast*   ast   = &doc->front_end.ast;
    for (u32 node_index = 0; node_index < array_count(ast->nodes);
         ++node_index) {
        const AstNode* node = &ast->nodes[node_index];
        if (node->kind != AK_On || node->a >= array_count(ast->nodes) ||
            node->b >= array_count(ast->ons)) {
            continue;
        }
        const AstNode* scrutinee = &ast->nodes[node->a];
        if (scrutinee->kind != AK_SymbolRef) {
            continue;
        }
        u32 type_node =
            lsp_completion_ast_symbol_type_node(lexer, ast, scrutinee->a);
        if (type_node >= array_count(ast->nodes) ||
            ast->nodes[type_node].kind != AK_Field ||
            ast->nodes[type_node].a >= array_count(ast->nodes)) {
            continue;
        }
        const AstNode* module = &ast->nodes[ast->nodes[type_node].a];
        if (module->kind != AK_SymbolRef) {
            continue;
        }
        string module_binding = lex_symbol(lexer, module->a);
        string enum_name      = lex_symbol(lexer, ast->nodes[type_node].b);

        const AstOnInfo* on   = &ast->ons[node->b];
        for (u32 branch_index = 0; branch_index < on->branch_count;
             ++branch_index) {
            const AstOnBranch* branch =
                &ast->on_branches[on->first_branch + branch_index];
            if (branch->flags & AOBF_Else) {
                continue;
            }
            for (u32 pattern = 0; pattern < branch->pattern_count; ++pattern) {
                u32 pattern_index =
                    ast->pattern_items[branch->pattern_index + pattern];
                if (pattern_index >= array_count(ast->patterns) ||
                    ast->patterns[pattern_index].kind != APK_EnumVariant) {
                    continue;
                }
                const AstEnumPattern* enum_pattern =
                    &ast->enum_patterns[ast->patterns[pattern_index].a];
                bool matched = false;
                for (u32 item = 0; item < enum_pattern->pattern_count; ++item) {
                    if (lsp_completion_ast_pattern_matches_receiver(
                            lexer,
                            ast,
                            ast->pattern_items[enum_pattern->first_pattern +
                                               item],
                            receiver)) {
                        matched = true;
                        break;
                    }
                }
                if (matched) {
                    lsp_completion_add_module_payload_members(
                        arena,
                        items,
                        doc,
                        uri,
                        module_binding,
                        enum_name,
                        lex_symbol(lexer, enum_pattern->symbol_handle));
                    return;
                }
            }
        }
    }
}

internal string lsp_completion_trim(string value)
{
    while (value.count > 0 && (value.data[0] == ' ' || value.data[0] == '\t')) {
        value.data++;
        value.count--;
    }
    while (value.count > 0 && (value.data[value.count - 1] == ' ' ||
                               value.data[value.count - 1] == '\t')) {
        value.count--;
    }
    return value;
}

internal bool
lsp_completion_line_before(string source, usize* cursor, string* out_line)
{
    if (*cursor == 0) {
        return false;
    }
    usize end = *cursor;
    if (end > 0 && source.data[end - 1] == '\n') {
        end--;
    }
    usize start = end;
    while (start > 0 && source.data[start - 1] != '\n') {
        start--;
    }
    *cursor   = start;
    *out_line = (string){.data = source.data + start, .count = end - start};
    return true;
}

internal bool lsp_completion_find_source_variant(string  source,
                                                 usize   offset,
                                                 string  receiver,
                                                 string* out_variant)
{
    usize  cursor = offset;
    string line   = {0};
    while (lsp_completion_line_before(source, &cursor, &line)) {
        for (usize i = 0; i < line.count; ++i) {
            if (!lsp_completion_is_ident_char(line.data[i])) {
                continue;
            }
            usize start = i;
            while (i < line.count &&
                   lsp_completion_is_ident_char(line.data[i])) {
                i++;
            }
            string variant = {.data = line.data + start, .count = i - start};
            usize  j       = i;
            while (j < line.count &&
                   (line.data[j] == ' ' || line.data[j] == '\t')) {
                j++;
            }
            if (j >= line.count || line.data[j] != '(') {
                continue;
            }
            j++;
            while (j < line.count &&
                   (line.data[j] == ' ' || line.data[j] == '\t')) {
                j++;
            }
            if (j + 2 < line.count && memcmp(line.data + j, "as", 2) == 0 &&
                !lsp_completion_is_ident_char(line.data[j + 2])) {
                j += 2;
                while (j < line.count &&
                       (line.data[j] == ' ' || line.data[j] == '\t')) {
                    j++;
                }
            }
            if (j + receiver.count <= line.count &&
                memcmp(line.data + j, receiver.data, receiver.count) == 0 &&
                (j + receiver.count == line.count ||
                 !lsp_completion_is_ident_char(
                     line.data[j + receiver.count]))) {
                *out_variant = variant;
                return true;
            }
        }
    }
    return false;
}

internal bool lsp_completion_find_source_on_scrutinee(string  source,
                                                      usize   offset,
                                                      string* out_scrutinee)
{
    usize  cursor = offset;
    string line   = {0};
    while (lsp_completion_line_before(source, &cursor, &line)) {
        line    = lsp_completion_trim(line);
        usize i = 0;
        if (!lsp_completion_match_ident_at(line, &i, s("on"))) {
            continue;
        }
        while (i < line.count &&
               (line.data[i] == ' ' || line.data[i] == '\t')) {
            i++;
        }
        usize start = i;
        while (i < line.count && lsp_completion_is_ident_char(line.data[i])) {
            i++;
        }
        if (i > start) {
            *out_scrutinee =
                (string){.data = line.data + start, .count = i - start};
            return true;
        }
    }
    return false;
}

internal bool lsp_completion_find_source_qualified_type(string  source,
                                                        string  binding,
                                                        string* out_module,
                                                        string* out_type)
{
    usize  cursor = source.count;
    string line   = {0};
    while (lsp_completion_line_before(source, &cursor, &line)) {
        line    = lsp_completion_trim(line);
        usize i = 0;
        if (!lsp_completion_match_ident_at(line, &i, binding)) {
            continue;
        }
        while (i < line.count &&
               (line.data[i] == ' ' || line.data[i] == '\t')) {
            i++;
        }
        if (i >= line.count || line.data[i] != ':') {
            continue;
        }
        i++;
        while (i < line.count &&
               (line.data[i] == ' ' || line.data[i] == '\t')) {
            i++;
        }
        usize module_start = i;
        while (i < line.count && lsp_completion_is_ident_char(line.data[i])) {
            i++;
        }
        if (i == module_start || i >= line.count || line.data[i] != '.') {
            continue;
        }
        *out_module = (string){.data  = line.data + module_start,
                               .count = i - module_start};
        i++;
        usize type_start = i;
        while (i < line.count && lsp_completion_is_ident_char(line.data[i])) {
            i++;
        }
        if (i > type_start) {
            *out_type = (string){.data  = line.data + type_start,
                                 .count = i - type_start};
            return true;
        }
    }
    return false;
}

internal void
lsp_completion_add_source_on_payload_members(Arena*             arena,
                                             JsonValue*         items,
                                             const LspDocument* doc,
                                             string             uri,
                                             usize              offset,
                                             string             receiver)
{
    string variant   = {0};
    string scrutinee = {0};
    string module    = {0};
    string enum_name = {0};
    if (!lsp_completion_find_source_variant(
            doc->source, offset, receiver, &variant) ||
        !lsp_completion_find_source_on_scrutinee(
            doc->source, offset, &scrutinee) ||
        !lsp_completion_find_source_qualified_type(
            doc->source, scrutinee, &module, &enum_name)) {
        return;
    }

    lsp_completion_add_module_payload_members(
        arena, items, doc, uri, module, enum_name, variant);
}

internal bool lsp_completion_add_module_exports_from_path(Arena*     arena,
                                                          JsonValue* items,
                                                          cstr resolved_path)
{
    FileMap map    = {0};
    string  source = filemap_load(resolved_path, &map);
    if (source.data == NULL) {
        return false;
    }

    ErrorRenderMode previous_mode = error_system_mode();
    bool            previous_emit = error_system_should_emit_output();
    error_system_set_mode(ERROR_RENDER_DIAGNOSTICS);
    error_system_set_emit_output(false);
    FrontEndOptions options = {
        .verbose              = false,
        .release              = false,
        .require_entry_point  = false,
        .skip_hir_generation  = true,
        .keep_partial_results = true,
    };

    ProgramInfo program = {0};
    bool        ok      = front_end_program(
        (NerdSource){
            .source      = source,
            .source_path = s(resolved_path),
        },
        &options,
        NULL,
        &program);
    error_system_set_mode(previous_mode);
    error_system_set_emit_output(previous_emit);

    bool          added  = false;
    LspModuleView module = {0};
    if (lsp_program_module_view(&program, program.root_module_index, &module)) {
        u32 export_count = lsp_module_export_count(&module);
        if (ok || export_count > 0) {
            lsp_completion_add_module_exports(arena, items, module);
            added = export_count > 0;
        }
    }

    program_info_done(&program);
    if (!added) {
        added = lsp_completion_add_module_ast_exports_from_source(
            arena, items, source, resolved_path);
    }
    filemap_unload(&map);
    return added;
}

internal void lsp_completion_add_resolved_module_exports(Arena*     arena,
                                                         JsonValue* items,
                                                         const LspDocument* doc,
                                                         cstr resolved_path)
{
    LspModuleView module = {0};
    if (lsp_program_module_view_by_path(
            &doc->program, resolved_path, &module)) {
        lsp_completion_add_module_exports(arena, items, module);
    }

    (void)lsp_completion_add_module_exports_from_path(
        arena, items, resolved_path);
}

internal u32 lsp_completion_ast_module_path_for_binding(const LspDocument* doc,
                                                        string receiver)
{
    const Lexer* lexer = &doc->front_end.lexer;
    const Ast*   ast   = &doc->front_end.ast;

    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind != AK_Bind || node->a == U32_MAX ||
            !string_eq(lex_symbol(lexer, node->a), receiver) ||
            node->b >= array_count(ast->nodes)) {
            continue;
        }

        const AstNode* value = &ast->nodes[node->b];
        if (value->kind == AK_Use && value->a < array_count(ast->nodes)) {
            value = &ast->nodes[value->a];
        }
        if (value->kind == AK_ModRef &&
            value->a < array_count(ast->module_paths)) {
            return value->a;
        }
    }

    return U32_MAX;
}

internal void lsp_completion_add_ast_module_members(Arena*             arena,
                                                    JsonValue*         items,
                                                    const LspDocument* doc,
                                                    string             receiver)
{
    u32 module_path_index =
        lsp_completion_ast_module_path_for_binding(doc, receiver);
    if (module_path_index == U32_MAX) {
        return;
    }

    Arena temp = {0};
    arena_init(&temp);
    NerdSource root_source = doc->program.root_source.source_path.count > 0
                                 ? doc->program.root_source
                                 : doc->front_end.lexer.source;
    ModuleResolveResult resolved = {0};
    ModuleResolveStatus status =
        module_resolve_path(&temp,
                            root_source,
                            &doc->front_end.lexer,
                            &doc->front_end.ast,
                            &doc->front_end.ast.module_paths[module_path_index],
                            &resolved);
    if (status == MRS_Found) {
        lsp_completion_add_resolved_module_exports(
            arena, items, doc, resolved.resolved_path);
    }
    arena_done(&temp);
}

internal bool
lsp_completion_match_ident_at(string source, usize* cursor, string ident)
{
    usize i = *cursor;
    while (i < source.count &&
           (source.data[i] == ' ' || source.data[i] == '\t')) {
        i++;
    }
    if (i + ident.count > source.count ||
        memcmp(source.data + i, ident.data, ident.count) != 0) {
        return false;
    }
    if (i + ident.count < source.count &&
        lsp_completion_is_ident_char(source.data[i + ident.count])) {
        return false;
    }
    *cursor = i + ident.count;
    return true;
}

internal bool lsp_completion_source_module_path_for_binding(Arena*  arena,
                                                            string  source,
                                                            string  receiver,
                                                            string* out_path)
{
    usize line_start = 0;
    while (line_start < source.count) {
        usize line_end = line_start;
        while (line_end < source.count && source.data[line_end] != '\n') {
            line_end++;
        }

        string line = {.data  = source.data + line_start,
                       .count = line_end - line_start};
        usize  i    = 0;
        while (i < line.count &&
               (line.data[i] == ' ' || line.data[i] == '\t')) {
            i++;
        }
        if (i + 3 <= line.count && memcmp(line.data + i, "pub", 3) == 0 &&
            (i + 3 == line.count ||
             !lsp_completion_is_ident_char(line.data[i + 3]))) {
            i += 3;
        }
        if (!lsp_completion_match_ident_at(line, &i, receiver)) {
            line_start = line_end + (line_end < source.count ? 1 : 0);
            continue;
        }
        while (i < line.count &&
               (line.data[i] == ' ' || line.data[i] == '\t')) {
            i++;
        }
        if (i + 2 > line.count || line.data[i] != ':' ||
            line.data[i + 1] != ':') {
            line_start = line_end + (line_end < source.count ? 1 : 0);
            continue;
        }
        i += 2;
        if (!lsp_completion_match_ident_at(line, &i, s("use"))) {
            line_start = line_end + (line_end < source.count ? 1 : 0);
            continue;
        }
        while (i < line.count &&
               (line.data[i] == ' ' || line.data[i] == '\t')) {
            i++;
        }

        usize path_start = i;
        while (i < line.count && (lsp_completion_is_ident_char(line.data[i]) ||
                                  line.data[i] == '.')) {
            i++;
        }
        if (i == path_start) {
            return false;
        }

        u8* copy = arena_alloc(arena, i - path_start);
        memcpy(copy, line.data + path_start, i - path_start);
        *out_path = (string){.data = copy, .count = i - path_start};
        return true;
    }

    return false;
}

internal cstr lsp_completion_text_module_relative(Arena* arena,
                                                  string module_path,
                                                  cstr   extension)
{
    StringBuilder sb = {0};
    sb_init(&sb, arena);
    for (usize i = 0; i < module_path.count; ++i) {
        if (module_path.data[i] == '.') {
#if OS_WINDOWS
            sb_append_char(&sb, '\\');
#else
            sb_append_char(&sb, '/');
#endif
        } else {
            sb_append_char(&sb, (char)module_path.data[i]);
        }
    }
    sb_append_cstr(&sb, extension);
    sb_append_null(&sb);
    return (cstr)sb_to_string(&sb).data;
}

internal bool lsp_completion_resolve_text_module_in_root(Arena* arena,
                                                         string module_path,
                                                         cstr   root,
                                                         cstr*  out_path)
{
    cstr module_file = path_join(
        arena,
        root,
        lsp_completion_text_module_relative(arena, module_path, ".n"));
    if (!path_exists(module_file) || path_is_directory(module_file)) {
        cstr module_dir = path_join(
            arena,
            root,
            lsp_completion_text_module_relative(arena, module_path, ""));
        module_file = path_join(arena, module_dir, "mod.n");
    }

    if (!path_exists(module_file) || path_is_directory(module_file)) {
        return false;
    }
    cstr canonical = path_canonical(arena, module_file);
    if (canonical == NULL) {
        return false;
    }
    *out_path = canonical;
    return true;
}

internal bool lsp_completion_resolve_text_module(Arena*             arena,
                                                 const LspDocument* doc,
                                                 string             module_path,
                                                 string current_source_path,
                                                 cstr*  out_path)
{
    NerdSource current_source = doc->front_end.lexer.source;
    if (current_source.source_path.count == 0) {
        current_source.source_path = current_source_path;
    }
    cstr current_path = module_source_file_path(arena, current_source);
    if (current_path != NULL &&
        lsp_completion_resolve_text_module_in_root(
            arena, module_path, path_dirname(arena, current_path), out_path)) {
        return true;
    }

    NerdSource root_source = doc->program.root_source.source_path.count > 0
                                 ? doc->program.root_source
                                 : doc->front_end.lexer.source;
    cstr       root_path   = module_source_file_path(arena, root_source);
    if (root_path != NULL &&
        lsp_completion_resolve_text_module_in_root(
            arena, module_path, path_dirname(arena, root_path), out_path)) {
        return true;
    }

    cstr lib_path = getenv("NERD_LIB_PATH");
    if (lib_path != NULL && *lib_path != '\0') {
#if OS_WINDOWS
        char separator = ';';
#else
        char separator = ':';
#endif
        const char* cursor = lib_path;
        while (*cursor != '\0') {
            const char* end = strchr(cursor, separator);
            usize len = end != NULL ? (usize)(end - cursor) : strlen(cursor);
            if (len > 0) {
                char* root = arena_alloc(arena, len + 1);
                memcpy(root, cursor, len);
                root[len] = '\0';
                if (lsp_completion_resolve_text_module_in_root(
                        arena, module_path, root, out_path)) {
                    return true;
                }
            }
            if (end == NULL) {
                break;
            }
            cursor = end + 1;
        }
    }

    cstr exe_dir  = path_executable_dir(arena);
    cstr mods_dir = path_join(arena, exe_dir, "mods");
    return lsp_completion_resolve_text_module_in_root(
        arena, module_path, mods_dir, out_path);
}

internal void lsp_completion_add_source_module_members(Arena*             arena,
                                                       JsonValue*         items,
                                                       const LspDocument* doc,
                                                       string receiver,
                                                       string document_uri)
{
    Arena temp = {0};
    arena_init(&temp);
    string module_path = {0};
    if (lsp_completion_source_module_path_for_binding(
            &temp, doc->source, receiver, &module_path)) {
        cstr resolved_path = NULL;
        if (lsp_completion_resolve_text_module(
                &temp, doc, module_path, document_uri, &resolved_path)) {
            lsp_completion_add_resolved_module_exports(
                arena, items, doc, resolved_path);
        }
    }
    arena_done(&temp);
}

internal bool lsp_completion_token_is_open(TokenKind kind)
{
    return kind == TK_LBrace || kind == TK_LParen || kind == TK_LBracket;
}

internal bool lsp_completion_token_is_close(TokenKind kind)
{
    return kind == TK_RBrace || kind == TK_RParen || kind == TK_RBracket;
}

internal u32 lsp_completion_symbol_handle_at_token(const Lexer* lexer,
                                                   u32          token_index)
{
    u32 symbol_index = 0;
    for (u32 i = 0; i <= token_index && i < array_count(lexer->tokens); ++i) {
        if (lexer->tokens[i].kind != TK_Symbol) {
            continue;
        }
        if (i == token_index) {
            return symbol_index < array_count(lexer->symbol_handles)
                       ? lexer->symbol_handles[symbol_index]
                       : U32_MAX;
        }
        symbol_index++;
    }
    return U32_MAX;
}

internal void lsp_completion_add_enum_variants(Arena*             arena,
                                               JsonValue*         items,
                                               const LspDocument* doc,
                                               u32                enum_type)
{
    const Sema*     sema = &doc->front_end.sema;
    const SemaType* type = NULL;
    enum_type            = sema_materialise_type(sema, enum_type);
    if (!lsp_sema_type(sema, enum_type, &type) || type->kind != STK_Enum) {
        return;
    }

    for (u32 i = 0; i < type->param_count; ++i) {
        u32 symbol = sema->type_param_symbols[type->first_param_type + i];
        if (symbol != U32_MAX) {
            lsp_completion_add(
                arena, items, lex_symbol(&doc->front_end.lexer, symbol), 20);
        }
    }
}

internal bool lsp_completion_receiver_is_single_ident(string receiver)
{
    if (receiver.count == 0) {
        return false;
    }
    for (usize i = 0; i < receiver.count; ++i) {
        if (!lsp_completion_is_ident_char(receiver.data[i])) {
            return false;
        }
    }
    return true;
}

internal bool lsp_completion_type_is_enum(const Sema* sema, u32 type_index)
{
    const SemaType* type = NULL;
    type_index           = sema_materialise_type(sema, type_index);
    return lsp_sema_type(sema, type_index, &type) && type->kind == STK_Enum;
}

internal bool lsp_completion_add_ast_enum_variants(Arena*             arena,
                                                   JsonValue*         items,
                                                   const LspDocument* doc,
                                                   string             receiver)
{
    const Lexer* lexer = &doc->front_end.lexer;
    const Ast*   ast   = &doc->front_end.ast;
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind != AK_Bind || node->a == U32_MAX ||
            !string_eq(lex_symbol(lexer, node->a), receiver) ||
            node->b >= array_count(ast->nodes)) {
            continue;
        }

        const AstNode* value = &ast->nodes[node->b];
        if (value->kind != AK_TypeEnum ||
            value->a >= array_count(ast->enum_types)) {
            return false;
        }

        const AstEnumTypeInfo* enum_type = &ast->enum_types[value->a];
        for (u32 variant_index = 0; variant_index < enum_type->variant_count;
             ++variant_index) {
            const AstEnumVariant* variant =
                &ast->enum_variants[enum_type->first_variant + variant_index];
            if (variant->symbol_handle != U32_MAX) {
                lsp_completion_add(arena,
                                   items,
                                   lex_symbol(lexer, variant->symbol_handle),
                                   20);
            }
        }
        return enum_type->variant_count > 0;
    }
    return false;
}

internal bool lsp_completion_add_qualified_enum_variants(Arena*     arena,
                                                         JsonValue* items,
                                                         const LspDocument* doc,
                                                         string receiver,
                                                         usize  offset)
{
    if (!lsp_completion_receiver_is_single_ident(receiver)) {
        return false;
    }

    const Lexer* lexer = &doc->front_end.lexer;
    const Sema*  sema  = &doc->front_end.sema;
    if (doc->bindings_ready) {
        u32   best_local      = sema_no_local();
        usize best_local_span = 0;
        for (u32 i = 0; i < array_count(sema->locals); ++i) {
            const SemaLocal* local = &sema->locals[i];
            if (local->symbol_handle == U32_MAX ||
                !string_eq(lex_symbol(lexer, local->symbol_handle), receiver)) {
                continue;
            }
            usize local_offset = lsp_completion_local_decl_offset(doc, local);
            if (local_offset <= offset && local_offset >= best_local_span) {
                best_local      = i;
                best_local_span = local_offset;
            }
        }
        if (best_local != sema_no_local()) {
            const SemaLocal* local = &sema->locals[best_local];
            if (local->kind != SLK_TypeAlias ||
                !lsp_completion_type_is_enum(sema, local->type_index)) {
                return false;
            }
            usize before = array_count(items->array.values);
            lsp_completion_add_enum_variants(
                arena, items, doc, local->type_index);
            return array_count(items->array.values) > before;
        }

        for (u32 i = 0; i < array_count(sema->decls); ++i) {
            const SemaDecl* decl = &sema->decls[i];
            if (decl->kind != SK_TypeAlias || decl->symbol_handle == U32_MAX ||
                !string_eq(lex_symbol(lexer, decl->symbol_handle), receiver) ||
                !lsp_completion_type_is_enum(sema, decl->type_index)) {
                continue;
            }
            usize before = array_count(items->array.values);
            lsp_completion_add_enum_variants(
                arena, items, doc, decl->type_index);
            return array_count(items->array.values) > before;
        }
    }

    if (lsp_completion_add_ast_enum_variants(arena, items, doc, receiver) ||
        lsp_completion_add_text_enum_variants(
            arena, items, doc->source, receiver)) {
        return true;
    }

    Arena temp = {0};
    arena_init(&temp);
    usize line_start = 0;
    while (line_start < doc->source.count) {
        usize line_end = line_start;
        while (line_end < doc->source.count &&
               doc->source.data[line_end] != '\n') {
            line_end++;
        }

        string line = {.data  = doc->source.data + line_start,
                       .count = line_end - line_start};
        line        = lsp_completion_trim(lsp_completion_strip_comment(line));

        usize i     = 0;
        if (lsp_completion_match_ident_at(line, &i, s("pub"))) {
            while (i < line.count &&
                   (line.data[i] == ' ' || line.data[i] == '\t')) {
                i++;
            }
        }
        if (!lsp_completion_match_ident_at(line, &i, s("use"))) {
            line_start = line_end + (line_end < doc->source.count ? 1 : 0);
            continue;
        }
        while (i < line.count &&
               (line.data[i] == ' ' || line.data[i] == '\t')) {
            i++;
        }

        usize path_start = i;
        while (i < line.count && (lsp_completion_is_ident_char(line.data[i]) ||
                                  line.data[i] == '.')) {
            i++;
        }
        if (i == path_start) {
            line_start = line_end + (line_end < doc->source.count ? 1 : 0);
            continue;
        }

        string module_path = {.data  = line.data + path_start,
                              .count = i - path_start};
        cstr   resolved    = NULL;
        if (lsp_completion_resolve_text_module(
                &temp,
                doc,
                module_path,
                doc->front_end.lexer.source.source_path,
                &resolved) &&
            lsp_completion_add_text_enum_variants_from_module(
                arena, items, resolved, receiver)) {
            arena_done(&temp);
            return true;
        }

        line_start = line_end + (line_end < doc->source.count ? 1 : 0);
    }

    arena_done(&temp);
    return false;
}

internal bool lsp_completion_on_scrutinee_before_offset(string  source,
                                                        usize   offset,
                                                        string* out_scrutinee)
{
    usize limit     = MIN(offset, source.count);
    usize candidate = SIZE_MAX;
    for (usize i = 0; i + 2 <= limit; ++i) {
        if (i > 0 && lsp_completion_is_ident_char(source.data[i - 1])) {
            continue;
        }
        if (source.data[i] != 'o' || source.data[i + 1] != 'n') {
            continue;
        }
        if (i + 2 < limit && lsp_completion_is_ident_char(source.data[i + 2])) {
            continue;
        }
        candidate = i;
    }
    if (candidate == SIZE_MAX) {
        return false;
    }

    usize cursor = candidate + 2;
    while (cursor < limit &&
           (source.data[cursor] == ' ' || source.data[cursor] == '\t')) {
        cursor++;
    }
    usize start = cursor;
    while (cursor < limit &&
           lsp_completion_is_ident_char(source.data[cursor])) {
        cursor++;
    }
    if (cursor == start) {
        return false;
    }
    string scrutinee = {.data = source.data + start, .count = cursor - start};
    while (cursor < limit &&
           (source.data[cursor] == ' ' || source.data[cursor] == '\t')) {
        cursor++;
    }
    if (cursor >= limit || source.data[cursor] != '{') {
        return false;
    }

    i32 depth = 1;
    for (usize i = cursor + 1; i < limit; ++i) {
        if (source.data[i] == '{') {
            depth++;
        } else if (source.data[i] == '}') {
            depth--;
            if (depth <= 0) {
                return false;
            }
        }
    }

    *out_scrutinee = scrutinee;
    return true;
}

internal bool lsp_completion_add_enum_variants_from_uses(Arena*     arena,
                                                         JsonValue* items,
                                                         const LspDocument* doc,
                                                         string             uri,
                                                         string type_name)
{
    Arena temp = {0};
    arena_init(&temp);
    usize before     = array_count(items->array.values);
    usize line_start = 0;
    while (line_start < doc->source.count) {
        usize line_end = line_start;
        while (line_end < doc->source.count &&
               doc->source.data[line_end] != '\n') {
            line_end++;
        }

        string line = {.data  = doc->source.data + line_start,
                       .count = line_end - line_start};
        line        = lsp_completion_trim(lsp_completion_strip_comment(line));

        usize i     = 0;
        if (lsp_completion_match_ident_at(line, &i, s("pub"))) {
            while (i < line.count &&
                   (line.data[i] == ' ' || line.data[i] == '\t')) {
                i++;
            }
        }
        if (!lsp_completion_match_ident_at(line, &i, s("use"))) {
            line_start = line_end + (line_end < doc->source.count ? 1 : 0);
            continue;
        }
        while (i < line.count &&
               (line.data[i] == ' ' || line.data[i] == '\t')) {
            i++;
        }

        usize path_start = i;
        while (i < line.count && (lsp_completion_is_ident_char(line.data[i]) ||
                                  line.data[i] == '.')) {
            i++;
        }
        if (i == path_start) {
            line_start = line_end + (line_end < doc->source.count ? 1 : 0);
            continue;
        }

        string module_path = {.data  = line.data + path_start,
                              .count = i - path_start};
        cstr   resolved    = NULL;
        if (lsp_completion_resolve_text_module(
                &temp, doc, module_path, uri, &resolved)) {
            lsp_completion_add_text_enum_variants_from_module(
                arena, items, resolved, type_name);
        }

        line_start = line_end + (line_end < doc->source.count ? 1 : 0);
    }

    arena_done(&temp);
    return array_count(items->array.values) > before;
}

internal bool lsp_completion_add_source_on_enum_variants(Arena*     arena,
                                                         JsonValue* items,
                                                         const LspDocument* doc,
                                                         string             uri,
                                                         usize offset)
{
    string scrutinee = {0};
    if (!lsp_completion_on_scrutinee_before_offset(
            doc->source, offset, &scrutinee)) {
        return false;
    }

    string type_name = {0};
    if (!lsp_completion_source_receiver_type_name(
            doc->source, scrutinee, offset, &type_name)) {
        return false;
    }

    usize        before = array_count(items->array.values);
    const Lexer* lexer  = &doc->front_end.lexer;
    const Sema*  sema   = &doc->front_end.sema;
    for (u32 i = 0; i < array_count(sema->decls); ++i) {
        const SemaDecl* decl = &sema->decls[i];
        if (decl->kind == SK_TypeAlias && decl->symbol_handle != U32_MAX &&
            string_eq(lex_symbol(lexer, decl->symbol_handle), type_name) &&
            lsp_completion_type_is_enum(sema, decl->type_index)) {
            lsp_completion_add_enum_variants(
                arena, items, doc, decl->type_index);
            break;
        }
    }
    lsp_completion_add_text_enum_variants(arena, items, doc->source, type_name);
    lsp_completion_add_enum_variants_from_uses(
        arena, items, doc, uri, type_name);
    return array_count(items->array.values) > before;
}

internal bool lsp_completion_arg_contains_offset(const Lexer* lexer,
                                                 const Ast*   ast,
                                                 u32          arg_node_index,
                                                 usize        offset)
{
    if (arg_node_index >= array_count(ast->nodes)) {
        return false;
    }

    const AstNode* arg = &ast->nodes[arg_node_index];
    if (arg->token_index >= array_count(lexer->tokens)) {
        return false;
    }

    usize start = lexer->tokens[arg->token_index].offset;
    usize end   = lex_token_end_offset(lexer, &lexer->tokens[arg->token_index]);
    return offset >= start && offset <= end;
}

internal bool lsp_completion_expected_enum_type_at_offset(
    const LspDocument* doc, usize offset, u32* out_enum_type)
{
    const Lexer* lexer = &doc->front_end.lexer;
    const Ast*   ast   = &doc->front_end.ast;
    const Sema*  sema  = &doc->front_end.sema;

    u32   best_type    = sema_no_type();
    usize best_span    = SIZE_MAX;

    for (u32 node_index = 0; node_index < array_count(ast->nodes);
         ++node_index) {
        const AstNode* node = &ast->nodes[node_index];
        if (node->kind != AK_Call || node->b >= array_count(ast->calls)) {
            continue;
        }

        const AstCallInfo* call = &ast->calls[node->b];
        for (u32 arg_index = 0; arg_index < call->arg_count; ++arg_index) {
            u32 arg_node = ast->call_args[call->first_arg + arg_index];
            if (!lsp_completion_arg_contains_offset(
                    lexer, ast, arg_node, offset)) {
                continue;
            }

            const AstNode* arg   = &ast->nodes[arg_node];
            usize          start = lexer->tokens[arg->token_index].offset;
            usize          end =
                lex_token_end_offset(lexer, &lexer->tokens[arg->token_index]);
            usize span = end - start;
            if (span >= best_span) {
                continue;
            }

            u32 callee_type = sema_no_type();
            if (!lsp_sema_node_type(sema, node->a, &callee_type)) {
                continue;
            }
            callee_type             = sema_materialise_type(sema, callee_type);

            const SemaType* fn_type = NULL;
            if (!lsp_sema_type(sema, callee_type, &fn_type) ||
                fn_type->kind != STK_Function ||
                arg_index >= fn_type->param_count) {
                continue;
            }

            u32 expected_type =
                sema->type_param_types[fn_type->first_param_type + arg_index];
            expected_type = sema_materialise_type(sema, expected_type);
            const SemaType* expected = NULL;
            if (!lsp_sema_type(sema, expected_type, &expected) ||
                expected->kind != STK_Enum) {
                continue;
            }

            best_type = expected_type;
            best_span = span;
        }
    }

    if (best_type == sema_no_type()) {
        return false;
    }
    *out_enum_type = best_type;
    return true;
}

internal bool lsp_completion_enclosing_brace(const Lexer* lexer,
                                             usize        offset,
                                             u32*         out_open_token)
{
    Array(u32) stack = NULL;
    for (u32 i = 0; i < array_count(lexer->tokens); ++i) {
        const Token* token = &lexer->tokens[i];
        if (token->offset >= offset) {
            break;
        }
        if (lsp_completion_token_is_open(token->kind)) {
            array_push(stack, i);
        } else if (lsp_completion_token_is_close(token->kind) &&
                   array_count(stack) > 0) {
            (void)array_pop(stack);
        }
    }

    bool found = false;
    for (usize i = array_count(stack); i > 0; --i) {
        u32 token_index = stack[i - 1];
        if (lexer->tokens[token_index].kind == TK_LBrace) {
            *out_open_token = token_index;
            found           = true;
            break;
        }
    }

    array_free(stack);
    return found;
}

internal bool lsp_completion_matching_close(const Lexer* lexer,
                                            u32          open_token_index,
                                            u32*         out_close_token_index)
{
    if (open_token_index >= array_count(lexer->tokens)) {
        return false;
    }

    TokenKind open_kind = lexer->tokens[open_token_index].kind;
    if (!lsp_completion_token_is_open(open_kind)) {
        return false;
    }

    u32 depth = 0;
    for (u32 i = open_token_index; i < array_count(lexer->tokens); ++i) {
        TokenKind kind = lexer->tokens[i].kind;
        if (lsp_completion_token_is_open(kind)) {
            depth++;
        } else if (lsp_completion_token_is_close(kind)) {
            if (depth == 0) {
                return false;
            }
            depth--;
            if (depth == 0) {
                *out_close_token_index = i;
                return true;
            }
        }
    }
    return false;
}

internal bool lsp_completion_find_on_block_range(const Lexer* lexer,
                                                 u32          on_token_index,
                                                 usize        offset,
                                                 u32*         out_open_token,
                                                 u32*         out_close_token)
{
    if (on_token_index >= array_count(lexer->tokens)) {
        return false;
    }
    bool  found     = false;
    usize best_span = SIZE_MAX;
    for (u32 i = 0; i < array_count(lexer->tokens); ++i) {
        const Token* token = &lexer->tokens[i];
        if (token->kind != TK_LBrace) {
            continue;
        }
        u32 close_token = U32_MAX;
        if (!lsp_completion_matching_close(lexer, i, &close_token)) {
            return false;
        }
        usize start = token->offset;
        usize end   = lex_token_end_offset(lexer, &lexer->tokens[close_token]);
        if (offset >= start && offset <= end && close_token > on_token_index &&
            end - start < best_span) {
            *out_open_token  = i;
            *out_close_token = close_token;
            best_span        = end - start;
            found            = true;
        }
    }
    return found;
}

internal bool lsp_completion_offset_inside_branch_body(const Lexer* lexer,
                                                       const Ast*   ast,
                                                       u32   expr_node_index,
                                                       usize offset)
{
    if (expr_node_index >= array_count(ast->nodes)) {
        return false;
    }
    const AstNode* expr = &ast->nodes[expr_node_index];
    if (expr->token_index >= array_count(lexer->tokens)) {
        return false;
    }
    usize start = lexer->tokens[expr->token_index].offset;
    usize end = lex_token_end_offset(lexer, &lexer->tokens[expr->token_index]);
    if (expr->kind == AK_Block) {
        u32 close_token = U32_MAX;
        if (lsp_completion_matching_close(
                lexer, expr->token_index, &close_token)) {
            end = lex_token_end_offset(lexer, &lexer->tokens[close_token]);
        }
    }
    return offset >= start && offset <= end;
}

internal bool lsp_completion_expected_enum_on_pattern_at_offset(
    const LspDocument* doc, usize offset, u32* out_enum_type)
{
    const Lexer* lexer = &doc->front_end.lexer;
    const Ast*   ast   = &doc->front_end.ast;
    const Sema*  sema  = &doc->front_end.sema;

    u32   best_type    = sema_no_type();
    usize best_span    = SIZE_MAX;
    for (u32 node_index = 0; node_index < array_count(ast->nodes);
         ++node_index) {
        const AstNode* node = &ast->nodes[node_index];
        if (node->kind != AK_On || node->a >= array_count(ast->nodes) ||
            node->b >= array_count(ast->ons) ||
            node->token_index >= array_count(lexer->tokens)) {
            continue;
        }

        u32 open_token  = U32_MAX;
        u32 close_token = U32_MAX;
        if (!lsp_completion_find_on_block_range(
                lexer, node->token_index, offset, &open_token, &close_token)) {
            continue;
        }

        const AstOnInfo* on         = &ast->ons[node->b];
        bool             in_pattern = true;
        for (u32 branch_index = 0; branch_index < on->branch_count;
             ++branch_index) {
            const AstOnBranch* branch =
                &ast->on_branches[on->first_branch + branch_index];
            if (lsp_completion_offset_inside_branch_body(
                    lexer, ast, branch->expr_node_index, offset)) {
                in_pattern = false;
                break;
            }
        }
        if (!in_pattern) {
            continue;
        }

        u32 scrutinee_type = sema_no_type();
        if (!lsp_sema_node_type(sema, node->a, &scrutinee_type)) {
            continue;
        }
        scrutinee_type       = sema_materialise_type(sema, scrutinee_type);
        const SemaType* type = NULL;
        if (!lsp_sema_type(sema, scrutinee_type, &type) ||
            type->kind != STK_Enum) {
            continue;
        }

        usize start = lexer->tokens[open_token].offset;
        usize end   = lex_token_end_offset(lexer, &lexer->tokens[close_token]);
        usize span  = end - start;
        if (span < best_span) {
            best_type = scrutinee_type;
            best_span = span;
        }
    }

    if (best_type == sema_no_type()) {
        return false;
    }
    *out_enum_type = best_type;
    return true;
}

internal bool lsp_completion_plex_literal_field_position(string source,
                                                         usize  offset,
                                                         usize  open_end,
                                                         string prefix)
{
    usize line_start = offset;
    while (line_start > 0 && source.data[line_start - 1] != '\n') {
        line_start--;
    }

    usize content_start = MAX(line_start, open_end);
    usize prefix_start =
        offset >= prefix.count ? offset - prefix.count : offset;
    for (usize i = content_start; i < prefix_start; ++i) {
        if (source.data[i] != ' ' && source.data[i] != '\t') {
            return false;
        }
    }
    for (usize i = content_start; i < offset; ++i) {
        if (source.data[i] == ':') {
            return false;
        }
    }
    return true;
}

internal bool lsp_completion_plex_literal_type_path(const Lexer* lexer,
                                                    u32          open_token,
                                                    string*      out_module,
                                                    string*      out_type)
{
    if (open_token == 0 || lexer->tokens[open_token - 1].kind != TK_Symbol) {
        return false;
    }

    u32 type_symbol =
        lsp_completion_symbol_handle_at_token(lexer, open_token - 1);
    if (type_symbol == U32_MAX) {
        return false;
    }
    *out_type = lex_symbol(lexer, type_symbol);

    if (open_token >= 3 && lexer->tokens[open_token - 2].kind == TK_Dot &&
        lexer->tokens[open_token - 3].kind == TK_Symbol) {
        u32 module_symbol =
            lsp_completion_symbol_handle_at_token(lexer, open_token - 3);
        if (module_symbol != U32_MAX) {
            *out_module = lex_symbol(lexer, module_symbol);
        }
    }

    return true;
}

internal void lsp_completion_plex_literal_seen_fields(const Lexer* lexer,
                                                      u32          open_token,
                                                      Array(string) * out_seen)
{
    u32 depth = 0;
    for (u32 i = open_token + 1; i + 1 < array_count(lexer->tokens); ++i) {
        TokenKind kind = lexer->tokens[i].kind;
        if (kind == TK_LBrace || kind == TK_LParen || kind == TK_LBracket) {
            depth++;
            continue;
        }
        if (kind == TK_RBrace || kind == TK_RParen || kind == TK_RBracket) {
            if (depth == 0) {
                return;
            }
            depth--;
            continue;
        }
        if (depth != 0 || kind != TK_Symbol ||
            lexer->tokens[i + 1].kind != TK_Colon) {
            continue;
        }
        u32 symbol = lsp_completion_symbol_handle_at_token(lexer, i);
        if (symbol != U32_MAX) {
            array_push(*out_seen, lex_symbol(lexer, symbol));
        }
    }
}

internal bool lsp_completion_add_plex_literal_fields_from_file(Arena*     arena,
                                                               JsonValue* items,
                                                               cstr       path,
                                                               string type_name,
                                                               Array(string)
                                                                   seen)
{
    FileMap map    = {0};
    string  source = filemap_load(path, &map);
    if (source.data == NULL) {
        return false;
    }

    ErrorRenderMode previous_mode = error_system_mode();
    bool            previous_emit = error_system_should_emit_output();
    error_system_set_mode(ERROR_RENDER_DIAGNOSTICS);
    error_system_set_emit_output(false);

    bool  added = false;
    Lexer lexer = {0};
    if (lex((NerdSource){.source = source, .source_path = s(path)}, &lexer)) {
        Ast ast = ast_parse(&lexer);
        if (array_count(ast.nodes) > 0) {
            added = lsp_completion_add_ast_plex_literal_fields_for_name(
                arena, items, &lexer, &ast, type_name, seen);
        }
        ast_done(&ast);
    }
    lex_done(&lexer);

    if (!added) {
        added = lsp_completion_add_text_plex_literal_fields(
            arena, items, source, type_name, seen);
    }

    error_system_set_mode(previous_mode);
    error_system_set_emit_output(previous_emit);
    filemap_unload(&map);
    return added;
}

internal bool lsp_completion_add_unqualified_import_plex_literal_fields(
    Arena*             arena,
    JsonValue*         items,
    const LspDocument* doc,
    string             uri,
    string             type_name,
    Array(string) seen)
{
    Arena temp = {0};
    arena_init(&temp);

    usize before     = array_count(items->array.values);
    usize line_start = 0;
    while (line_start < doc->source.count) {
        usize line_end = line_start;
        while (line_end < doc->source.count &&
               doc->source.data[line_end] != '\n') {
            line_end++;
        }

        string line = {.data  = doc->source.data + line_start,
                       .count = line_end - line_start};
        line        = lsp_completion_trim(lsp_completion_strip_comment(line));

        usize i     = 0;
        if (lsp_completion_match_ident_at(line, &i, s("pub"))) {
            while (i < line.count &&
                   (line.data[i] == ' ' || line.data[i] == '\t')) {
                i++;
            }
        }
        if (!lsp_completion_match_ident_at(line, &i, s("use"))) {
            line_start = line_end + (line_end < doc->source.count ? 1 : 0);
            continue;
        }

        while (i < line.count &&
               (line.data[i] == ' ' || line.data[i] == '\t')) {
            i++;
        }
        usize path_start = i;
        while (i < line.count && (lsp_completion_is_ident_char(line.data[i]) ||
                                  line.data[i] == '.')) {
            i++;
        }
        if (i == path_start) {
            line_start = line_end + (line_end < doc->source.count ? 1 : 0);
            continue;
        }

        string module_path = {.data  = line.data + path_start,
                              .count = i - path_start};
        cstr   resolved    = NULL;
        if (lsp_completion_resolve_text_module(
                &temp, doc, module_path, uri, &resolved)) {
            if (!lsp_completion_add_plex_literal_fields_from_file(
                    arena, items, resolved, type_name, seen) &&
                string_eq(path_filename(s(resolved)), s("mod.n"))) {
                cstr    module_dir = path_dirname(&temp, resolved);
                DirIter iter       = {0};
                if (dir_iter_init(&iter, module_dir)) {
                    cstr path         = NULL;
                    bool is_directory = false;
                    while (dir_iter_next(&iter, &temp, &path, &is_directory)) {
                        if (!is_directory &&
                            lsp_completion_path_is_module_part_file(path) &&
                            lsp_completion_add_plex_literal_fields_from_file(
                                arena, items, path, type_name, seen)) {
                            break;
                        }
                    }
                    dir_iter_done(&iter);
                }
            }
        }

        line_start = line_end + (line_end < doc->source.count ? 1 : 0);
    }

    arena_done(&temp);
    return array_count(items->array.values) > before;
}

internal bool lsp_completion_add_plex_literal_fields(Arena*             arena,
                                                     JsonValue*         items,
                                                     const LspDocument* doc,
                                                     string             uri,
                                                     string             prefix,
                                                     usize              offset)
{
    const Lexer* lexer      = &doc->front_end.lexer;
    u32          open_token = U32_MAX;
    if (!lsp_completion_enclosing_brace(lexer, offset, &open_token) ||
        open_token >= array_count(lexer->tokens)) {
        return false;
    }

    usize open_end = lex_token_end_offset(lexer, &lexer->tokens[open_token]);
    if (!lsp_completion_plex_literal_field_position(
            doc->source, offset, open_end, prefix)) {
        return false;
    }

    string module_name = {0};
    string type_name   = {0};
    if (!lsp_completion_plex_literal_type_path(
            lexer, open_token, &module_name, &type_name)) {
        return false;
    }

    Array(string) seen = NULL;
    lsp_completion_plex_literal_seen_fields(lexer, open_token, &seen);

    if (module_name.count == 0) {
        if (!lsp_completion_add_ast_plex_literal_fields_for_name(
                arena, items, lexer, &doc->front_end.ast, type_name, seen)) {
            if (!lsp_completion_add_text_plex_literal_fields(
                    arena, items, doc->source, type_name, seen)) {
                (void)lsp_completion_add_unqualified_import_plex_literal_fields(
                    arena, items, doc, uri, type_name, seen);
            }
        }
        array_free(seen);
        return true;
    }

    Arena temp = {0};
    arena_init(&temp);
    string module_path = {0};
    cstr   resolved    = NULL;
    if (lsp_completion_source_module_path_for_binding(
            &temp, doc->source, module_name, &module_path) &&
        lsp_completion_resolve_text_module(
            &temp, doc, module_path, uri, &resolved)) {
        if (!lsp_completion_add_plex_literal_fields_from_file(
                arena, items, resolved, type_name, seen) &&
            string_eq(path_filename(s(resolved)), s("mod.n"))) {
            cstr    module_dir = path_dirname(&temp, resolved);
            DirIter iter       = {0};
            if (dir_iter_init(&iter, module_dir)) {
                cstr path         = NULL;
                bool is_directory = false;
                while (dir_iter_next(&iter, &temp, &path, &is_directory)) {
                    if (!is_directory &&
                        lsp_completion_path_is_module_part_file(path) &&
                        lsp_completion_add_plex_literal_fields_from_file(
                            arena, items, path, type_name, seen)) {
                        break;
                    }
                }
                dir_iter_done(&iter);
            }
        }
    }
    arena_done(&temp);
    array_free(seen);
    return true;
}

internal bool
lsp_completion_use_context(string source, usize offset, string* out_path)
{
    usize line_start = offset;
    while (line_start > 0 && source.data[line_start - 1] != '\n') {
        line_start--;
    }

    usize line_end = offset;
    while (line_end > line_start && (source.data[line_end - 1] == ' ' ||
                                     source.data[line_end - 1] == '\t')) {
        line_end--;
    }

    usize i = line_start;
    while (i < line_end && (source.data[i] == ' ' || source.data[i] == '\t')) {
        i++;
    }

    if (i + 3 <= line_end && memcmp(source.data + i, "pub", 3) == 0 &&
        (i + 3 == line_end || source.data[i + 3] == ' ' ||
         source.data[i + 3] == '\t')) {
        i += 3;
        while (i < line_end &&
               (source.data[i] == ' ' || source.data[i] == '\t')) {
            i++;
        }
    }

    if (i + 3 > line_end || memcmp(source.data + i, "use", 3) != 0 ||
        (i + 3 < line_end && source.data[i + 3] != ' ' &&
         source.data[i + 3] != '\t')) {
        return false;
    }
    i += 3;
    while (i < line_end && (source.data[i] == ' ' || source.data[i] == '\t')) {
        i++;
    }

    *out_path = (string){.data = source.data + i, .count = line_end - i};
    return true;
}

internal void
lsp_completion_add_module_dir(Arena* arena, JsonValue* items, cstr dir)
{
    DirIter iter = {0};
    if (!dir_iter_init(&iter, dir)) {
        return;
    }

    Array(string) module_names = NULL;
    cstr entry_path            = NULL;
    bool is_directory          = false;
    while (dir_iter_next(&iter, arena, &entry_path, &is_directory)) {
        string filename = path_filename(s(entry_path));
        if (filename.count == 0 ||
            (filename.count > 0 && filename.data[0] == '.')) {
            continue;
        }

        if (is_directory) {
            cstr mod_path = path_join(arena, entry_path, "mod.n");
            if (path_exists(mod_path)) {
                array_push(module_names, filename);
            }
            continue;
        }

        if (path_has_extension(s(entry_path), ".n") &&
            !string_eq(filename, s("mod.n"))) {
            array_push(module_names, path_stem(s(entry_path)));
        }
    }

    dir_iter_done(&iter);

    for (u32 i = 1; i < array_count(module_names); ++i) {
        string value = module_names[i];
        u32    j     = i;
        while (j > 0) {
            string prev = module_names[j - 1];
            int    cmp  = strncmp((cstr)value.data,
                                  (cstr)prev.data,
                                  MIN(value.count, prev.count));
            if (cmp > 0 || (cmp == 0 && value.count >= prev.count)) {
                break;
            }
            module_names[j] = prev;
            j--;
        }
        module_names[j] = value;
    }

    for (u32 i = 0; i < array_count(module_names); ++i) {
        lsp_completion_add(arena, items, module_names[i], 9); // Module
    }
    array_free(module_names);
}

internal void lsp_completion_add_modules_in_root(Arena*     arena,
                                                 JsonValue* items,
                                                 cstr       root,
                                                 string     module_path,
                                                 cstr       current_path)
{
    StringBuilder sb = {0};
    sb_init(&sb, arena);
    for (usize i = 0; i < module_path.count; ++i) {
        if (module_path.data[i] == '.') {
#if OS_WINDOWS
            sb_append_char(&sb, '\\');
#else
            sb_append_char(&sb, '/');
#endif
        } else {
            sb_append_char(&sb, (char)module_path.data[i]);
        }
    }
    sb_append_null(&sb);

    cstr relative = (cstr)sb_to_string(&sb).data;
    cstr dir = module_path.count == 0 ? root : path_join(arena, root, relative);
    if (module_path.count > 0) {
        cstr mod_path = path_join(arena, dir, "mod.n");
        if (path_exists(mod_path) && !path_is_directory(mod_path)) {
            cstr canonical_mod     = path_canonical(arena, mod_path);
            cstr canonical_current = current_path != NULL
                                         ? path_canonical(arena, current_path)
                                         : NULL;
            if (canonical_mod == NULL || canonical_current == NULL ||
                strcmp(canonical_mod, canonical_current) != 0) {
                return;
            }
        }
    }
    lsp_completion_add_module_dir(arena, items, dir);
}

internal void lsp_completion_add_modules(Arena*             arena,
                                         JsonValue*         items,
                                         const LspDocument* doc,
                                         string             use_path)
{
    usize last_dot = 0;
    bool  has_dot  = false;
    for (usize i = 0; i < use_path.count; ++i) {
        if (use_path.data[i] == '.') {
            last_dot = i;
            has_dot  = true;
        }
    }
    string module_path =
        has_dot ? (string){.data = use_path.data, .count = last_dot}
                : (string){0};

    Arena temp = {0};
    arena_init(&temp);

    cstr current_path =
        module_source_file_path(&temp, doc->front_end.lexer.source);
    if (current_path != NULL) {
        lsp_completion_add_modules_in_root(arena,
                                           items,
                                           path_dirname(&temp, current_path),
                                           module_path,
                                           current_path);
    }

    cstr root_path = module_source_file_path(&temp, doc->program.root_source);
    if (root_path != NULL) {
        lsp_completion_add_modules_in_root(arena,
                                           items,
                                           path_dirname(&temp, root_path),
                                           module_path,
                                           current_path);
    }

    cstr lib_path = getenv("NERD_LIB_PATH");
    if (lib_path != NULL && *lib_path != '\0') {
#if OS_WINDOWS
        char separator = ';';
#else
        char separator = ':';
#endif
        const char* cursor = lib_path;
        while (*cursor != '\0') {
            const char* end = strchr(cursor, separator);
            usize len = end != NULL ? (usize)(end - cursor) : strlen(cursor);
            if (len > 0) {
                char* root = (char*)arena_alloc(&temp, len + 1);
                memcpy(root, cursor, len);
                root[len] = '\0';
                lsp_completion_add_modules_in_root(
                    arena, items, root, module_path, current_path);
            }
            if (end == NULL) {
                break;
            }
            cursor = end + 1;
        }
    }

    cstr exe_dir  = path_executable_dir(&temp);
    cstr mods_dir = path_join(&temp, exe_dir, "mods");
    lsp_completion_add_modules_in_root(
        arena, items, mods_dir, module_path, current_path);

    arena_done(&temp);
}

internal void lsp_completion_add_symbols(Arena*             arena,
                                         JsonValue*         items,
                                         const LspDocument* doc)
{
    if (!doc->bindings_ready) {
        const Lexer* lexer = &doc->front_end.lexer;
        const Ast*   ast   = &doc->front_end.ast;
        for (u32 i = 0; i < array_count(ast->nodes); ++i) {
            const AstNode* node          = &ast->nodes[i];
            u32            symbol_handle = U32_MAX;
            if (node->kind == AK_Bind || node->kind == AK_Variable) {
                symbol_handle = node->a;
            } else if (node->kind == AK_FfiDef &&
                       node->a < array_count(ast->ffi_infos)) {
                symbol_handle = ast->ffi_infos[node->a].symbol_handle;
            }
            if (symbol_handle == U32_MAX) {
                continue;
            }
            string label = lex_symbol(lexer, symbol_handle);
            if (lsp_completion_is_internal_label(label)) {
                continue;
            }
            lsp_completion_add(
                arena, items, label, lsp_completion_ast_export_kind(ast, node));
        }
        for (u32 i = 0; i < array_count(ast->params); ++i) {
            const AstParam* param = &ast->params[i];
            if (param->symbol_handle != U32_MAX) {
                lsp_completion_add(
                    arena, items, lex_symbol(lexer, param->symbol_handle), 6);
            }
        }
        return;
    }

    const Lexer* lexer = &doc->front_end.lexer;
    const Sema*  sema  = &doc->front_end.sema;
    for (u32 i = 0; i < array_count(sema->decls); ++i) {
        const SemaDecl* decl = &sema->decls[i];
        if (decl->symbol_handle == U32_MAX) {
            continue;
        }
        string label = lex_symbol(lexer, decl->symbol_handle);
        if (lsp_completion_is_internal_label(label)) {
            continue;
        }
        lsp_completion_add(
            arena, items, label, lsp_completion_decl_kind(decl->kind));
    }

    for (u32 i = 0; i < array_count(sema->locals); ++i) {
        const SemaLocal* local = &sema->locals[i];
        if (local->symbol_handle == U32_MAX) {
            continue;
        }
        lsp_completion_add(arena,
                           items,
                           lex_symbol(lexer, local->symbol_handle),
                           lsp_completion_local_kind(local->kind));
    }
}

void lsp_handle_completion(LspState* state, const LspMessage* message)
{
    JsonValue* response = lsp_prepare_response(message);

    string uri          = {0};
    if (!lsp_get_string_param(message, "params.textDocument.uri", &uri)) {
        lsp_cancel(response, message->arena);
        return;
    }

    LspSourceView view = {0};
    if (!lsp_source_view(state, uri, &view)) {
        lsp_cancel(response, message->arena);
        return;
    }
    const LspDocument* doc = view.doc;

    u64 line               = 0;
    u64 character          = 0;
    (void)lsp_get_u64_param(message, "params.position.line", &line);
    (void)lsp_get_u64_param(message, "params.position.character", &character);
    usize  offset    = lsp_offset_from_position(view.source, line, character);
    string prefix    = lsp_completion_ident_before(view.source, offset);

    JsonValue* items = json_new_array(message->arena);
    string     use_path = {0};
    if (lsp_completion_use_context(view.source, offset, &use_path)) {
        lsp_completion_add_modules(message->arena, items, doc, use_path);
        lsp_completion_filter_items(items, prefix);
        json_object_set_array(response, "result", items);
        lsp_send_response(message->arena, response);
        return;
    }

    string early_receiver = {0};
    if (!lsp_completion_member_context(view.source, offset, &early_receiver)) {
        u32 expected_on_enum = sema_no_type();
        if (lsp_completion_expected_enum_on_pattern_at_offset(
                doc, offset, &expected_on_enum)) {
            lsp_completion_add_enum_variants(
                message->arena, items, doc, expected_on_enum);
        }
        lsp_completion_add_source_on_enum_variants(
            message->arena, items, doc, uri, offset);
        if (array_count(items->array.values) > 0) {
            lsp_completion_filter_items(items, prefix);
            json_object_set_array(response, "result", items);
            lsp_send_response(message->arena, response);
            return;
        }
    }

    if (lsp_completion_add_plex_literal_fields(
            message->arena, items, doc, uri, prefix, offset)) {
        lsp_completion_filter_items(items, prefix);
        json_object_set_array(response, "result", items);
        lsp_send_response(message->arena, response);
        return;
    }
    if (lsp_completion_after_open_brace(view.source, offset)) {
        json_object_set_array(response, "result", items);
        lsp_send_response(message->arena, response);
        return;
    }

    string receiver = {0};
    if (lsp_completion_member_context(view.source, offset, &receiver)) {
        if (lsp_completion_add_qualified_enum_variants(
                message->arena, items, doc, receiver, offset)) {
            lsp_completion_filter_items(items, prefix);
            json_object_set_array(response, "result", items);
            lsp_send_response(message->arena, response);
            return;
        }
        bool receiver_has_value_binding =
            lsp_completion_ast_receiver_type_symbol(doc, receiver, offset) !=
                U32_MAX ||
            lsp_completion_source_receiver_has_binding(
                view.source, receiver, offset);
        if (!receiver_has_value_binding) {
            lsp_completion_add_associated_methods(
                message->arena, items, doc, uri, receiver);
            if (array_count(items->array.values) > 0) {
                lsp_completion_add_member_details(message->arena, items);
                lsp_completion_filter_items(items, prefix);
                json_object_set_array(response, "result", items);
                lsp_send_response(message->arena, response);
                return;
            }
        }
        lsp_completion_add_source_for_item_members(
            message->arena, items, doc, offset, receiver);
        lsp_completion_add_members(
            message->arena,
            items,
            doc,
            lsp_completion_type_for_receiver(doc, receiver, offset));
        if (array_count(items->array.values) == 0) {
            lsp_completion_add_source_collection_members(
                items, message->arena, view.source, offset, receiver);
        }
        if (array_count(items->array.values) == 0 &&
            lsp_completion_source_top_level_string_binding(view.source,
                                                           receiver)) {
            lsp_completion_add_string_members(message->arena, items);
        }
        if (array_count(items->array.values) == 0) {
            lsp_completion_add_ast_members(
                message->arena, items, doc, receiver, offset);
        }
        if (array_count(items->array.values) == 0) {
            lsp_completion_add_ast_module_members(
                message->arena, items, doc, receiver);
        }
        if (array_count(items->array.values) == 0) {
            lsp_completion_add_source_module_members(
                message->arena, items, doc, receiver, uri);
        }
        if (array_count(items->array.values) == 0) {
            lsp_completion_add_source_param_members(
                message->arena, items, doc, offset, receiver);
        }
        if (array_count(items->array.values) == 0) {
            lsp_completion_add_imported_type_members(
                message->arena, items, doc, uri, offset, receiver);
        }
        lsp_completion_add_imported_ast_methods(
            message->arena, items, doc, uri, receiver, offset);
        string source_receiver_type = {0};
        if (lsp_completion_source_receiver_type_name(
                view.source, receiver, offset, &source_receiver_type)) {
            lsp_completion_add_text_impl_methods_from_uses(
                message->arena, items, doc, uri, source_receiver_type, false);
        }
        if (array_count(items->array.values) == 0) {
            lsp_completion_add_source_on_payload_members(
                message->arena, items, doc, uri, offset, receiver);
        }
        if (array_count(items->array.values) == 0) {
            lsp_completion_add_repaired_members(
                message->arena, items, doc, uri, offset, receiver);
        }
        lsp_completion_add_member_details(message->arena, items);
        lsp_completion_filter_items(items, prefix);
        json_object_set_array(response, "result", items);
        lsp_send_response(message->arena, response);
        return;
    }

    lsp_completion_add_symbols(message->arena, items, doc);
    u32 expected_enum = sema_no_type();
    if (lsp_completion_expected_enum_on_pattern_at_offset(
            doc, offset, &expected_enum) ||
        lsp_completion_expected_enum_type_at_offset(
            doc, offset, &expected_enum)) {
        lsp_completion_add_enum_variants(
            message->arena, items, doc, expected_enum);
    }
    lsp_completion_add_source_symbols(
        message->arena, items, view.source, offset);
    if (!doc->sema_complete) {
        lsp_completion_add_source_top_level_symbols(
            message->arena, items, view.source);
    }
    if (!doc->bindings_ready || !doc->sema_complete) {
        lsp_completion_add_source_use_exports(message->arena, items, doc, uri);
    }
    if (!doc->bindings_ready || array_count(doc->front_end.sema.locals) == 0) {
        lsp_completion_add_source_params(
            message->arena, items, view.source, offset);
    }
    lsp_completion_filter_items(items, prefix);
    json_object_set_array(response, "result", items);
    lsp_send_response(message->arena, response);
}

//------------------------------------------------------------------------------
