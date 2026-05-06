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

internal void lsp_completion_filter_items(JsonValue* items, string prefix)
{
    if (prefix.count == 0 || items->kind != JSON_ARRAY) {
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

    string receiver = lsp_completion_ident_before(source, prefix_start - 1);
    if (receiver.count == 0) {
        return false;
    }

    *out_receiver = receiver;
    return true;
}

internal u32 lsp_completion_type_for_symbol(const LspDocument* doc,
                                            string             symbol)
{
    if (!doc->semantic_ready) {
        return sema_no_type();
    }

    const Lexer* lexer = &doc->front_end.lexer;
    const Sema*  sema  = &doc->front_end.sema;
    for (u32 i = 0; i < array_count(sema->locals); ++i) {
        const SemaLocal* local = &sema->locals[i];
        if (local->symbol_handle != U32_MAX &&
            string_eq(lex_symbol(lexer, local->symbol_handle), symbol)) {
            return local->type_index;
        }
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

internal void lsp_completion_add_module_exports(Arena*            arena,
                                                JsonValue*        items,
                                                const ModuleInfo* module)
{
    for (u32 i = 0; i < array_count(module->export_decl_indices); ++i) {
        u32 decl_index = module->export_decl_indices[i];
        if (decl_index >= array_count(module->front_end.sema.decls)) {
            continue;
        }
        const SemaDecl* decl = &module->front_end.sema.decls[decl_index];
        if (decl->symbol_handle == U32_MAX) {
            continue;
        }
        lsp_completion_add(
            arena,
            items,
            lex_symbol(&module->front_end.lexer, decl->symbol_handle),
            lsp_completion_decl_kind(decl->kind));
    }
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
        u32 pointee_type = sema_materialise_type(sema, type->first_param_type);
        if (pointee_type < array_count(sema->types) &&
            (sema->types[pointee_type].kind == STK_Plex ||
             sema->types[pointee_type].kind == STK_Union ||
             sema->types[pointee_type].kind == STK_DynamicArray)) {
            type_index = pointee_type;
            type       = &sema->types[type_index];
        }
    }

    if (type->kind == STK_Module && doc->program.modules != NULL &&
        type->return_type < array_count(doc->program.modules)) {
        const ModuleInfo* module = &doc->program.modules[type->return_type];
        lsp_completion_add_module_exports(arena, items, module);
        return;
    }

    if (type->kind == STK_String || type->kind == STK_Slice) {
        lsp_completion_add(arena, items, s("data"), 5);  // Field
        lsp_completion_add(arena, items, s("count"), 5); // Field
        return;
    }

    if (type->kind == STK_DynamicArray) {
        lsp_completion_add(arena, items, s("data"), 5);             // Field
        lsp_completion_add(arena, items, s("count"), 5);            // Field
        lsp_completion_add(arena, items, s("capacity"), 5);         // Field
        lsp_completion_add(arena, items, s("append"), 2);           // Method
        lsp_completion_add(arena, items, s("clear"), 2);            // Method
        lsp_completion_add(arena, items, s("free"), 2);             // Method
        lsp_completion_add(arena, items, s("pop"), 2);              // Method
        lsp_completion_add(arena, items, s("push"), 2);             // Method
        lsp_completion_add(arena, items, s("reserve"), 2);          // Method
        lsp_completion_add(arena, items, s("resize"), 2);           // Method
        lsp_completion_add(arena, items, s("resize_undefined"), 2); // Method
        return;
    }

    if (type->kind != STK_Plex && type->kind != STK_Union) {
        return;
    }

    const Lexer* lexer = &doc->front_end.lexer;
    for (u32 i = 0; i < type->param_count; ++i) {
        u32 symbol = sema->type_param_symbols[type->first_param_type + i];
        if (symbol != U32_MAX) {
            lsp_completion_add(arena, items, lex_symbol(lexer, symbol), 5);
        }
    }

    for (u32 i = 0; i < array_count(sema->methods); ++i) {
        const SemaMethod* method = &sema->methods[i];
        if (method->symbol_handle == U32_MAX) {
            continue;
        }

        u32 target_type = sema_no_type();
        if (method->target_type_node_index <
            array_count(sema->node_type_indices)) {
            target_type = sema_materialise_type(
                sema, sema->node_type_indices[method->target_type_node_index]);
        }

        if (target_type != sema_no_type() &&
            target_type != sema_materialise_type(sema, receiver_type) &&
            target_type != type_index) {
            continue;
        }

        lsp_completion_add(
            arena, items, lex_symbol(lexer, method->symbol_handle), 2);
    }
}

internal u32 lsp_completion_ast_type_symbol(const Ast* ast, u32 type_node_index)
{
    if (type_node_index >= array_count(ast->nodes)) {
        return U32_MAX;
    }

    const AstNode* type_node = &ast->nodes[type_node_index];
    if (type_node->kind == AK_SymbolRef) {
        return type_node->a;
    }
    if (type_node->kind == AK_TypePointer) {
        return lsp_completion_ast_type_symbol(ast, type_node->a);
    }
    return U32_MAX;
}

internal u32 lsp_completion_ast_receiver_type_symbol(const LspDocument* doc,
                                                     string receiver)
{
    const Lexer* lexer = &doc->front_end.lexer;
    const Ast*   ast   = &doc->front_end.ast;

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

        const AstNode* value = &ast->nodes[node->b];
        if (value->kind == AK_AnnotatedValue || value->kind == AK_ZeroInit ||
            value->kind == AK_Undefined) {
            return lsp_completion_ast_type_symbol(ast, value->a);
        }
    }

    return U32_MAX;
}

internal void lsp_completion_add_ast_members(Arena*             arena,
                                             JsonValue*         items,
                                             const LspDocument* doc,
                                             string             receiver)
{
    const Lexer* lexer = &doc->front_end.lexer;
    const Ast*   ast   = &doc->front_end.ast;
    u32 type_symbol    = lsp_completion_ast_receiver_type_symbol(doc, receiver);
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

internal u32 lsp_completion_find_program_module_by_path(
    const ProgramInfo* program, cstr resolved_path)
{
    for (u32 i = 0; i < array_count(program->modules); ++i) {
        const ModuleInfo* module = &program->modules[i];
        if (module->resolved_path != NULL &&
            strcmp(module->resolved_path, resolved_path) == 0) {
            return i;
        }
    }
    return U32_MAX;
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
        .skip_ir_generation   = true,
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

    bool added = false;
    if (array_count(program.modules) > 0 &&
        program.root_module_index < array_count(program.modules)) {
        ModuleInfo* module = &program.modules[program.root_module_index];
        if (ok || array_count(module->export_decl_indices) > 0) {
            lsp_completion_add_module_exports(arena, items, module);
            added = array_count(module->export_decl_indices) > 0;
        }
    }

    program_info_done(&program);
    filemap_unload(&map);
    return added;
}

internal void lsp_completion_add_resolved_module_exports(Arena*     arena,
                                                         JsonValue* items,
                                                         const LspDocument* doc,
                                                         cstr resolved_path)
{
    u32 module_index = lsp_completion_find_program_module_by_path(
        &doc->program, resolved_path);
    if (module_index != U32_MAX) {
        lsp_completion_add_module_exports(
            arena, items, &doc->program.modules[module_index]);
        return;
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
                                                 cstr*              out_path)
{
    cstr current_path =
        module_source_file_path(arena, doc->front_end.lexer.source);
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
                                                       string receiver)
{
    Arena temp = {0};
    arena_init(&temp);
    string module_path = {0};
    if (lsp_completion_source_module_path_for_binding(
            &temp, doc->source, receiver, &module_path)) {
        cstr resolved_path = NULL;
        if (lsp_completion_resolve_text_module(
                &temp, doc, module_path, &resolved_path)) {
            lsp_completion_add_resolved_module_exports(
                arena, items, doc, resolved_path);
        }
    }
    arena_done(&temp);
}

internal void lsp_completion_add_keywords(Arena* arena, JsonValue* items)
{
    static cstr keywords[] = {
        "assert",
        "break",
        "continue",
        "defer",
        "else",
        "enum",
        "ffi",
        "fn",
        "for",
        "impl",
        "on",
        "pub",
        "return",
        "test",
        "union",
        "use",
    };

    for (usize i = 0; i < sizeof(keywords) / sizeof(keywords[0]); ++i) {
        lsp_completion_add(arena, items, s(keywords[i]), 14); // Keyword
    }
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
                                                 string     module_path)
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
        lsp_completion_add_modules_in_root(
            arena, items, path_dirname(&temp, current_path), module_path);
    }

    cstr root_path = module_source_file_path(&temp, doc->program.root_source);
    if (root_path != NULL) {
        lsp_completion_add_modules_in_root(
            arena, items, path_dirname(&temp, root_path), module_path);
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
                    arena, items, root, module_path);
            }
            if (end == NULL) {
                break;
            }
            cursor = end + 1;
        }
    }

    cstr exe_dir  = path_executable_dir(&temp);
    cstr mods_dir = path_join(&temp, exe_dir, "mods");
    lsp_completion_add_modules_in_root(arena, items, mods_dir, module_path);

    arena_done(&temp);
}

internal void lsp_completion_add_symbols(Arena*             arena,
                                         JsonValue*         items,
                                         const LspDocument* doc)
{
    if (!doc->semantic_ready) {
        return;
    }

    const Lexer* lexer = &doc->front_end.lexer;
    const Sema*  sema  = &doc->front_end.sema;
    for (u32 i = 0; i < array_count(sema->decls); ++i) {
        const SemaDecl* decl = &sema->decls[i];
        if (decl->symbol_handle == U32_MAX) {
            continue;
        }
        lsp_completion_add(arena,
                           items,
                           lex_symbol(lexer, decl->symbol_handle),
                           lsp_completion_decl_kind(decl->kind));
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

    LspDocument* doc = LspDocumentMap_find(&state->documents, uri);
    if (!doc) {
        lsp_cancel(response, message->arena);
        return;
    }

    u64 line      = 0;
    u64 character = 0;
    (void)lsp_get_u64_param(message, "params.position.line", &line);
    (void)lsp_get_u64_param(message, "params.position.character", &character);
    usize  offset    = lsp_offset_from_position(doc->source, line, character);
    string prefix    = lsp_completion_ident_before(doc->source, offset);

    JsonValue* items = json_new_array(message->arena);
    string     use_path = {0};
    if (lsp_completion_use_context(doc->source, offset, &use_path)) {
        lsp_completion_add_modules(message->arena, items, doc, use_path);
        lsp_completion_filter_items(items, prefix);
        json_object_set_array(response, "result", items);
        lsp_send_response(message->arena, response);
        return;
    }

    string receiver = {0};
    if (lsp_completion_member_context(doc->source, offset, &receiver)) {
        lsp_completion_add_members(
            message->arena,
            items,
            doc,
            lsp_completion_type_for_symbol(doc, receiver));
        if (array_count(items->array.values) == 0) {
            lsp_completion_add_ast_members(
                message->arena, items, doc, receiver);
        }
        if (array_count(items->array.values) == 0) {
            lsp_completion_add_ast_module_members(
                message->arena, items, doc, receiver);
        }
        if (array_count(items->array.values) == 0) {
            lsp_completion_add_source_module_members(
                message->arena, items, doc, receiver);
        }
        lsp_completion_filter_items(items, prefix);
        json_object_set_array(response, "result", items);
        lsp_send_response(message->arena, response);
        return;
    }

    lsp_completion_add_keywords(message->arena, items);
    lsp_completion_add_symbols(message->arena, items, doc);
    lsp_completion_filter_items(items, prefix);
    json_object_set_array(response, "result", items);
    lsp_send_response(message->arena, response);
}

//------------------------------------------------------------------------------
