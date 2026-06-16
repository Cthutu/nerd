//------------------------------------------------------------------------------
// Symbol-aware LSP queries
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/build/front/front.h>
#include <compiler/error/error.h>
#include <compiler/modules/modules.h>
#include <lsp/lsp.h>

#define LSP_NO_DECL UINT32_MAX
#define LSP_SYMBOL_KIND_INTERFACE 11
#define LSP_SYMBOL_KIND_FUNCTION 12
#define LSP_SYMBOL_KIND_VARIABLE 13
#define LSP_SYMBOL_KIND_CONSTANT 14

//------------------------------------------------------------------------------
// Convert one source offset into an LSP position object.

internal JsonValue*
lsp_make_position(Arena* arena, NerdSource source, usize offset)
{
    u32 line = 0;
    u32 col  = 0;
    if (!lex_offset_to_line_col(source, offset, &line, &col)) {
        kill("Expected valid source offset: %zu", offset);
    }

    JsonValue* position = json_new_object(arena);
    json_object_set_number(position, arena, "line", line);
    json_object_set_number(position, arena, "character", col);
    return position;
}

//------------------------------------------------------------------------------
// Convert one source offset pair into an LSP range object.

internal JsonValue* lsp_make_range(Arena*     arena,
                                   NerdSource source,
                                   usize      start_offset,
                                   usize      end_offset)
{
    JsonValue* range = json_new_object(arena);
    json_object_set_object(
        range, "start", lsp_make_position(arena, source, start_offset));
    json_object_set_object(
        range, "end", lsp_make_position(arena, source, end_offset));
    return range;
}

//------------------------------------------------------------------------------
// Return the index of a token pointer inside the lexer's token table.

internal u32 lsp_token_index_from_pointer(const Lexer* lexer,
                                          const Token* token)
{
    ASSERT(token >= lexer->tokens &&
               token < lexer->tokens + array_count(lexer->tokens),
           "Expected token pointer to belong to the lexer");
    return (u32)(token - lexer->tokens);
}

internal cstr lsp_cstr_from_string(Arena* arena, string text)
{
    char* copy = arena_alloc(arena, text.count + 1);
    memcpy(copy, text.data, text.count);
    copy[text.count] = '\0';
    return copy;
}

//------------------------------------------------------------------------------
// Return the analysed-source offset where the open editor document begins.
// Folder modules analyse `mod.n` plus implicit sibling parts as one source, but
// LSP requests are still expressed relative to the file open in the editor.

internal bool lsp_document_visible_start(const LspDocument* doc,
                                         usize*             out_start)
{
    string analysed = doc->front_end.lexer.source.source;
    if (string_eq(analysed, doc->source)) {
        *out_start = 0;
        return true;
    }

    if (doc->source.count <= analysed.count &&
        memcmp(analysed.data, doc->source.data, doc->source.count) == 0) {
        *out_start = 0;
        return true;
    }

    if (doc->source.count == 0) {
        *out_start = 0;
        return true;
    }

    for (usize i = 0; i + doc->source.count <= analysed.count; ++i) {
        if (memcmp(analysed.data + i, doc->source.data, doc->source.count) ==
            0) {
            *out_start = i;
            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------
// Build a range relative to the open editor document when possible.

internal JsonValue* lsp_make_document_range(const LspDocument* doc,
                                            Arena*             arena,
                                            usize              start_offset,
                                            usize              end_offset)
{
    usize visible_start = 0;
    if (lsp_document_visible_start(doc, &visible_start) &&
        start_offset >= visible_start && end_offset >= visible_start &&
        end_offset <= visible_start + doc->source.count) {
        NerdSource visible_source = {
            .source      = doc->source,
            .source_path = doc->front_end.lexer.source.source_path,
        };
        return lsp_make_range(arena,
                              visible_source,
                              start_offset - visible_start,
                              end_offset - visible_start);
    }

    return lsp_make_range(
        arena, doc->front_end.lexer.source, start_offset, end_offset);
}

//------------------------------------------------------------------------------
// Return the source offsets of one token in the current document.

internal void lsp_token_offsets(const Lexer* lexer,
                                u32          token_index,
                                usize*       out_start,
                                usize*       out_end)
{
    const Token* token = &lexer->tokens[token_index];
    *out_start         = token->offset;
    *out_end           = lex_token_end_offset(lexer, token);
}

//------------------------------------------------------------------------------
// Return the AST binding node that owns a given token index, if any.

internal u32 lsp_find_bind_node_at_token(const Ast* ast, u32 token_index)
{
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if ((node->kind == AK_Bind || node->kind == AK_Variable) &&
            node->token_index == token_index) {
            return i;
        }
    }

    return U32_MAX;
}

//------------------------------------------------------------------------------
// Return the AST symbol-reference node that owns a given token index, if any.

internal u32 lsp_find_symbol_ref_node_at_token(const Ast* ast, u32 token_index)
{
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind == AK_SymbolRef && node->token_index == token_index) {
            return i;
        }
    }

    return U32_MAX;
}

//------------------------------------------------------------------------------
// Return the interned symbol handle for one symbol token.

internal u32 lsp_symbol_handle_at_token(const Lexer* lexer, u32 token_index)
{
    u32 symbol_index = 0;
    for (u32 i = 0; i < array_count(lexer->tokens); ++i) {
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

//------------------------------------------------------------------------------
// Return the AST field node that owns a given token index, if any.

internal u32 lsp_find_field_node_at_token(const Ast* ast, u32 token_index)
{
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if ((node->kind == AK_Field || node->kind == AK_TupleField) &&
            node->token_index == token_index) {
            return i;
        }
    }

    return U32_MAX;
}

internal bool lsp_find_plex_literal_field_at_token(const Ast* ast,
                                                   u32        token_index,
                                                   u32* out_literal_node_index,
                                                   u32* out_field_index)
{
    for (u32 node_index = 0; node_index < array_count(ast->nodes);
         ++node_index) {
        const AstNode* node = &ast->nodes[node_index];
        if ((node->kind != AK_Plex && node->kind != AK_PlexUpdate) ||
            node->a >= array_count(ast->plex_literals)) {
            continue;
        }

        const AstPlexLiteralInfo* literal = &ast->plex_literals[node->a];
        for (u32 i = 0; i < literal->field_count; ++i) {
            u32 field_index = literal->first_field + i;
            if (field_index >= array_count(ast->plex_literal_fields)) {
                break;
            }
            if (ast->plex_literal_fields[field_index].token_index ==
                token_index) {
                *out_literal_node_index = node_index;
                *out_field_index        = field_index;
                return true;
            }
        }
    }

    return false;
}

internal bool lsp_find_plex_pattern_field_at_token(const Ast* ast,
                                                   u32        token_index,
                                                   u32*       out_pattern_index,
                                                   u32*       out_field_index)
{
    for (u32 pattern_index = 0; pattern_index < array_count(ast->patterns);
         ++pattern_index) {
        const AstPattern* pattern = &ast->patterns[pattern_index];
        if (pattern->kind != APK_Plex) {
            continue;
        }
        for (u32 i = 0; i < pattern->b; ++i) {
            u32 field_index = pattern->a + i;
            if (field_index >= array_count(ast->pattern_fields)) {
                break;
            }
            if (ast->pattern_fields[field_index].token_index == token_index) {
                *out_pattern_index = pattern_index;
                *out_field_index   = field_index;
                return true;
            }
        }
    }

    return false;
}

internal string lsp_field_hover_text(const LspDocument* doc,
                                     Arena*             arena,
                                     u32                field_node_index);

//------------------------------------------------------------------------------
// Return the call node that uses one field node as its callee.

internal u32 lsp_call_node_for_field_callee(const Ast* ast,
                                            u32        field_node_index)
{
    if (field_node_index >= array_count(ast->nodes)) {
        return U32_MAX;
    }

    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind != AK_Call || node->a >= array_count(ast->nodes)) {
            continue;
        }

        u32            callee_node_index = node->a;
        const AstNode* callee            = &ast->nodes[callee_node_index];
        if (callee->kind == AK_TypeApply &&
            callee->a < array_count(ast->type_applications)) {
            callee_node_index =
                ast->type_applications[callee->a].target_node_index;
        }

        if (callee_node_index == field_node_index) {
            return i;
        }
    }

    return U32_MAX;
}

internal u32 lsp_selected_method_decl_for_field(const LspDocument* doc,
                                                u32 field_node_index)
{
    u32 call_node_index =
        lsp_call_node_for_field_callee(&doc->front_end.ast, field_node_index);
    if (call_node_index == U32_MAX ||
        call_node_index >=
            array_count(doc->front_end.sema.node_method_call_decl_indices)) {
        return LSP_NO_DECL;
    }

    u32 decl_index =
        doc->front_end.sema.node_method_call_decl_indices[call_node_index];
    return decl_index == sema_no_decl() ? LSP_NO_DECL : decl_index;
}

internal bool lsp_record_field_type(const Sema* sema,
                                    u32         record_type,
                                    u32         field_symbol,
                                    u32*        out_field_type)
{
    if (record_type == sema_no_type() ||
        record_type >= array_count(sema->types)) {
        return false;
    }

    const SemaType* type = &sema->types[record_type];
    if (type->kind != STK_Plex && type->kind != STK_Union) {
        return false;
    }

    for (u32 i = 0; i < type->param_count; ++i) {
        u32 param_index = type->first_param_type + i;
        if (param_index >= array_count(sema->type_param_symbols)) {
            break;
        }
        if (sema->type_param_symbols[param_index] == field_symbol) {
            if (out_field_type != NULL) {
                *out_field_type = sema->type_param_types[param_index];
            }
            return true;
        }
    }

    return false;
}

internal u32 lsp_enum_variant_index(const Sema* sema,
                                    u32         enum_type,
                                    u32         variant_symbol)
{
    if (enum_type == sema_no_type() || enum_type >= array_count(sema->types) ||
        sema->types[enum_type].kind != STK_Enum) {
        return U32_MAX;
    }

    const SemaType* type = &sema->types[enum_type];
    for (u32 i = 0; i < type->param_count; ++i) {
        u32 param_index = type->first_param_type + i;
        if (param_index >= array_count(sema->type_param_symbols)) {
            break;
        }
        if (sema->type_param_symbols[param_index] == variant_symbol) {
            return i;
        }
    }
    return U32_MAX;
}

internal u32 lsp_enum_variant_payload_type(const Sema* sema,
                                           u32         enum_type,
                                           u32         variant_index)
{
    if (enum_type == sema_no_type() || enum_type >= array_count(sema->types) ||
        sema->types[enum_type].kind != STK_Enum ||
        variant_index >= sema->types[enum_type].param_count) {
        return sema_no_type();
    }

    u32 param_index = sema->types[enum_type].first_param_type + variant_index;
    return param_index < array_count(sema->type_param_types)
               ? sema->type_param_types[param_index]
               : sema_no_type();
}

internal u32 lsp_method_decl_for_bind_node(const LspDocument* doc,
                                           u32                bind_node_index)
{
    if (bind_node_index >= array_count(doc->front_end.ast.nodes)) {
        return LSP_NO_DECL;
    }

    for (u32 i = 0; i < array_count(doc->front_end.sema.methods); ++i) {
        u32             decl_index = doc->front_end.sema.methods[i].decl_index;
        const SemaDecl* decl       = NULL;
        if (lsp_sema_decl(&doc->front_end.sema, decl_index, &decl) &&
            decl->bind_node_index == bind_node_index) {
            return decl_index;
        }
    }

    return LSP_NO_DECL;
}

//------------------------------------------------------------------------------
// Return the AST module-reference node that owns a given token index, if any.

internal u32 lsp_find_modref_node_at_token(const Lexer* lexer,
                                           const Ast*   ast,
                                           u32          token_index)
{
    UNUSED(lexer);
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind != AK_ModRef ||
            node->a >= array_count(ast->module_paths)) {
            continue;
        }

        const AstModulePath* path          = &ast->module_paths[node->a];
        u32                  current_token = node->token_index;
        for (u32 j = 0; j < path->symbol_count; ++j) {
            if (current_token == token_index) {
                return i;
            }
            current_token += 1;
            if (j + 1 < path->symbol_count) {
                current_token += 1;
            }
        }
    }

    return U32_MAX;
}

//------------------------------------------------------------------------------
// Convert one filesystem path into a file:// URI for LSP responses.

internal string lsp_path_to_uri(Arena* arena, cstr path)
{
    StringBuilder sb = {0};
    sb_init(&sb, arena);
    sb_append_cstr(&sb, "file://");

#if OS_WINDOWS
    if (path[0] != '\0' && path[1] == ':') {
        sb_append_char(&sb, '/');
    }
#endif

    for (const char* cursor = path; *cursor != '\0'; ++cursor) {
        u8 ch = (u8)*cursor;
#if OS_WINDOWS
        if (ch == '\\') {
            sb_append_char(&sb, '/');
            continue;
        }
#endif
        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') ||
            (ch >= '0' && ch <= '9') || ch == '/' || ch == '-' || ch == '_' ||
            ch == '.' || ch == '~') {
            sb_append_char(&sb, (char)ch);
        } else {
            sb_format(&sb, "%%%02X", ch);
        }
    }

    return sb_to_string(&sb);
}

//------------------------------------------------------------------------------
// Build a location object for one declaration inside a specific module.

internal bool lsp_source_fragment_for_range(NerdSource          source,
                                            usize               start_offset,
                                            usize               end_offset,
                                            NerdSourceFragment* out_fragment,
                                            NerdSource*         out_source,
                                            usize*              out_start,
                                            usize*              out_end)
{
    for (u32 i = 0; i < array_count(source.fragments); ++i) {
        NerdSourceFragment fragment = source.fragments[i];
        if (start_offset < fragment.start || end_offset > fragment.end) {
            continue;
        }

        usize source_prefix_start = fragment.start - fragment.source_start;
        usize source_count        = fragment.end - source_prefix_start;
        *out_fragment             = fragment;
        *out_source               = (NerdSource){
            .source      = string_from(source.source.data + source_prefix_start,
                                       source_count),
            .source_path = fragment.source_path,
        };
        *out_start = start_offset - fragment.start + fragment.source_start;
        *out_end   = end_offset - fragment.start + fragment.source_start;
        return true;
    }
    return false;
}

internal JsonValue*
lsp_module_decl_location(LspModuleView module, Arena* arena, u32 decl_index)
{
    const SemaDecl* decl = NULL;
    if (!lsp_sema_decl(module.sema, decl_index, &decl)) {
        return NULL;
    }

    if (decl->bind_node_index == LSP_NO_DECL ||
        decl->bind_node_index >= array_count(module.ast->nodes)) {
        return NULL;
    }

    const AstNode* bind        = &module.ast->nodes[decl->bind_node_index];
    u32            token_index = bind->token_index;
    if (bind->kind == AK_FfiDef &&
        bind->a < array_count(module.ast->ffi_infos)) {
        token_index = module.ast->ffi_infos[bind->a].symbol_token_index;
    }
    usize start_offset;
    usize end_offset;
    lsp_token_offsets(module.lexer, token_index, &start_offset, &end_offset);

    JsonValue*         location      = json_new_object(arena);
    NerdSourceFragment fragment      = {0};
    NerdSource         mapped_source = {0};
    usize              mapped_start  = 0;
    usize              mapped_end    = 0;
    if (lsp_source_fragment_for_range(module.lexer->source,
                                      start_offset,
                                      end_offset,
                                      &fragment,
                                      &mapped_source,
                                      &mapped_start,
                                      &mapped_end)) {
        json_object_set_string(
            location,
            arena,
            "uri",
            lsp_path_to_uri(arena,
                            lsp_cstr_from_string(arena, fragment.source_path)));
        json_object_set_object(
            location,
            "range",
            lsp_make_range(arena, mapped_source, mapped_start, mapped_end));
    } else {
        json_object_set_string(
            location,
            arena,
            "uri",
            lsp_path_to_uri(arena, module.info->resolved_path));
        json_object_set_object(
            location,
            "range",
            lsp_make_range(
                arena, module.lexer->source, start_offset, end_offset));
    }
    return location;
}

//------------------------------------------------------------------------------
// Build a location object for one token in the open document.

internal JsonValue* lsp_token_location(const LspDocument* doc,
                                       Arena*             arena,
                                       string             uri,
                                       u32                token_index)
{
    if (token_index >= array_count(doc->front_end.lexer.tokens)) {
        return NULL;
    }

    usize start_offset = 0;
    usize end_offset   = 0;
    lsp_token_offsets(
        &doc->front_end.lexer, token_index, &start_offset, &end_offset);

    JsonValue* location = json_new_object(arena);
    json_object_set_string(location, arena, "uri", uri);
    json_object_set_object(
        location,
        "range",
        lsp_make_document_range(doc, arena, start_offset, end_offset));
    return location;
}

//------------------------------------------------------------------------------
// Build a location object for the start of one module source file.

internal JsonValue* lsp_module_file_location(LspModuleView module, Arena* arena)
{
    JsonValue* location = json_new_object(arena);
    json_object_set_string(location,
                           arena,
                           "uri",
                           lsp_path_to_uri(arena, module.info->resolved_path));
    json_object_set_object(
        location, "range", lsp_make_range(arena, module.lexer->source, 0, 0));
    return location;
}

//------------------------------------------------------------------------------
// Resolve one imported symbol through a module type back to its exporting decl.

internal JsonValue* lsp_imported_symbol_location(const LspDocument* doc,
                                                 Arena*             arena,
                                                 u32                module_type,
                                                 u32 symbol_handle)
{
    const SemaType* type = NULL;
    if (!lsp_sema_type(&doc->front_end.sema, module_type, &type)) {
        return NULL;
    }

    LspModuleView module = {0};
    if (!lsp_program_module_view_by_type(&doc->program, type, &module)) {
        return NULL;
    }

    for (u32 i = 0; i < type->param_count; ++i) {
        if (doc->front_end.sema.type_param_symbols[type->first_param_type +
                                                   i] != symbol_handle) {
            continue;
        }
        u32 decl_index = sema_no_decl();
        if (!lsp_module_export_decl(&module, i, NULL, &decl_index)) {
            return NULL;
        }
        return lsp_module_decl_location(module, arena, decl_index);
    }

    return NULL;
}

//------------------------------------------------------------------------------
// Return the semantic declaration index matching one symbol handle.

internal u32 lsp_find_decl_index_by_symbol_handle(const Sema* sema,
                                                  u32         symbol_handle)
{
    for (u32 i = 0; i < array_count(sema->decls); ++i) {
        if (sema->decls[i].symbol_handle == symbol_handle) {
            return i;
        }
    }

    return LSP_NO_DECL;
}

//------------------------------------------------------------------------------
// Return the semantic declaration index referenced by one token.

internal u32 lsp_find_decl_index_for_token(const LspDocument* doc,
                                           u32                token_index)
{
    u32 bind_node_index =
        lsp_find_bind_node_at_token(&doc->front_end.ast, token_index);
    if (bind_node_index != U32_MAX) {
        u32 method_decl = lsp_method_decl_for_bind_node(doc, bind_node_index);
        if (method_decl != LSP_NO_DECL) {
            return method_decl;
        }
        return lsp_find_decl_index_by_symbol_handle(
            &doc->front_end.sema, doc->front_end.ast.nodes[bind_node_index].a);
    }

    u32 ref_node_index =
        lsp_find_symbol_ref_node_at_token(&doc->front_end.ast, token_index);
    u32 decl_index = LSP_NO_DECL;
    if (ref_node_index != U32_MAX) {
        if (lsp_sema_node_decl(
                &doc->front_end.sema, ref_node_index, &decl_index)) {
            return decl_index;
        }

        const AstNode* ref = &doc->front_end.ast.nodes[ref_node_index];
        decl_index =
            lsp_find_decl_index_by_symbol_handle(&doc->front_end.sema, ref->a);
        const SemaDecl* decl = NULL;
        if (lsp_sema_decl(&doc->front_end.sema, decl_index, &decl) &&
            (decl->kind == SK_TypeAlias || decl->kind == SK_GenericTypeAlias)) {
            return decl_index;
        }
    }

    u32 field_node_index =
        lsp_find_field_node_at_token(&doc->front_end.ast, token_index);
    if (field_node_index != U32_MAX) {
        decl_index = lsp_selected_method_decl_for_field(doc, field_node_index);
        if (decl_index != LSP_NO_DECL) {
            return decl_index;
        }
    }
    if (lsp_sema_node_decl(
            &doc->front_end.sema, field_node_index, &decl_index)) {
        return decl_index;
    }

    return LSP_NO_DECL;
}

//------------------------------------------------------------------------------
// Return a syntax-level definition location for a symbol when semantic data is
// unavailable or incomplete. This is deliberately conservative: it points at a
// same-file binder with the same spelling, preferring a binder before the use.

internal JsonValue* lsp_ast_symbol_location(const LspDocument* doc,
                                            Arena*             arena,
                                            string             uri,
                                            u32                token_index,
                                            u32                symbol_handle)
{
    const Ast* ast    = &doc->front_end.ast;

    u32 first_token   = U32_MAX;
    u32 nearest_token = U32_MAX;

    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if ((node->kind != AK_Bind && node->kind != AK_Variable) ||
            node->a != symbol_handle) {
            continue;
        }

        if (first_token == U32_MAX || node->token_index < first_token) {
            first_token = node->token_index;
        }
        if (node->token_index <= token_index &&
            (nearest_token == U32_MAX || node->token_index > nearest_token)) {
            nearest_token = node->token_index;
        }
    }

    for (u32 i = 0; i < array_count(ast->params); ++i) {
        const AstParam* param = &ast->params[i];
        if (param->symbol_handle != symbol_handle) {
            continue;
        }

        if (first_token == U32_MAX || param->token_index < first_token) {
            first_token = param->token_index;
        }
        if (param->token_index <= token_index &&
            (nearest_token == U32_MAX || param->token_index > nearest_token)) {
            nearest_token = param->token_index;
        }
    }

    u32 best_token = nearest_token != U32_MAX ? nearest_token : first_token;
    if (best_token == U32_MAX) {
        return NULL;
    }

    return lsp_token_location(doc, arena, uri, best_token);
}

//------------------------------------------------------------------------------
// Return a syntax-level definition location for the symbol under one token.

internal JsonValue* lsp_ast_definition_location(const LspDocument* doc,
                                                Arena*             arena,
                                                string             uri,
                                                u32                token_index)
{
    u32 ref_node_index =
        lsp_find_symbol_ref_node_at_token(&doc->front_end.ast, token_index);
    if (ref_node_index != U32_MAX) {
        return lsp_ast_symbol_location(
            doc,
            arena,
            uri,
            token_index,
            doc->front_end.ast.nodes[ref_node_index].a);
    }

    u32 bind_node_index =
        lsp_find_bind_node_at_token(&doc->front_end.ast, token_index);
    if (bind_node_index != U32_MAX) {
        return lsp_token_location(doc, arena, uri, token_index);
    }

    return NULL;
}

internal string lsp_decl_hover_text(const LspDocument* doc,
                                    Arena*             arena,
                                    u32                decl_index);
internal string lsp_decl_doc_comment(const LspDocument* doc,
                                     Arena*             arena,
                                     const SemaDecl*    decl);
internal string lsp_method_hover_text(const LspDocument* doc,
                                      Arena*             arena,
                                      u32                decl_index);

internal bool lsp_path_is_module_part_file(cstr path)
{
    string filename = path_filename(s(path));
    return !string_eq(filename, s("mod.n")) &&
           path_has_extension(filename, ".n");
}

internal int lsp_compare_cstr_ptr(const void* lhs, const void* rhs)
{
    const cstr* a = lhs;
    const cstr* b = rhs;
    return strcmp(*a, *b);
}

internal JsonValue*
lsp_ast_export_location_in_file(Arena* arena, cstr resolved_path, string symbol)
{
    FileMap map    = {0};
    string  source = filemap_load(resolved_path, &map);
    if (source.data == NULL) {
        return NULL;
    }

    ErrorRenderMode previous_mode = error_system_mode();
    bool            previous_emit = error_system_should_emit_output();
    error_system_set_mode(ERROR_RENDER_DIAGNOSTICS);
    error_system_set_emit_output(false);

    JsonValue* location = NULL;
    Lexer      lexer    = {0};
    if (lex((NerdSource){.source = source, .source_path = s(resolved_path)},
            &lexer)) {
        Ast ast = ast_parse(&lexer);
        for (u32 i = 0; i < array_count(ast.nodes); ++i) {
            const AstNode* node = &ast.nodes[i];
            if (!ast_has_flag(node, ANF_Public)) {
                continue;
            }

            u32 symbol_handle = U32_MAX;
            u32 token_index   = node->token_index;
            if (node->kind == AK_Bind || node->kind == AK_Variable) {
                symbol_handle = node->a;
            } else if (node->kind == AK_FfiDef &&
                       node->a < array_count(ast.ffi_infos)) {
                symbol_handle = ast.ffi_infos[node->a].symbol_handle;
                token_index   = ast.ffi_infos[node->a].symbol_token_index;
            }
            if (symbol_handle == U32_MAX ||
                !string_eq(lex_symbol(&lexer, symbol_handle), symbol) ||
                token_index >= array_count(lexer.tokens)) {
                continue;
            }

            usize start = 0;
            usize end   = 0;
            lsp_token_offsets(&lexer, token_index, &start, &end);
            location = json_new_object(arena);
            json_object_set_string(
                location, arena, "uri", lsp_path_to_uri(arena, resolved_path));
            json_object_set_object(
                location,
                "range",
                lsp_make_range(arena, lexer.source, start, end));
            break;
        }
        ast_done(&ast);
    }
    lex_done(&lexer);

    error_system_set_mode(previous_mode);
    error_system_set_emit_output(previous_emit);
    filemap_unload(&map);
    return location;
}

internal JsonValue* lsp_ast_export_location_in_module(Arena* arena,
                                                      cstr   resolved_path,
                                                      string symbol)
{
    JsonValue* location =
        lsp_ast_export_location_in_file(arena, resolved_path, symbol);
    if (location != NULL ||
        !string_eq(path_filename(s(resolved_path)), s("mod.n"))) {
        return location;
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
            if (!is_directory && lsp_path_is_module_part_file(path)) {
                array_push(part_paths, path);
            }
        }
        dir_iter_done(&iter);
    }

    if (array_count(part_paths) > 1) {
        qsort(part_paths,
              array_count(part_paths),
              sizeof(part_paths[0]),
              lsp_compare_cstr_ptr);
    }

    for (u32 i = 0; i < array_count(part_paths); ++i) {
        location =
            lsp_ast_export_location_in_file(arena, part_paths[i], symbol);
        if (location != NULL) {
            break;
        }
    }

    array_free(part_paths);
    arena_done(&temp);
    return location;
}

internal JsonValue* lsp_ast_imported_symbol_location(const LspDocument* doc,
                                                     Arena*             arena,
                                                     string             symbol)
{
    const Ast* ast = &doc->front_end.ast;
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind != AK_Use || node->a >= array_count(ast->nodes)) {
            continue;
        }

        const AstNode* modref = &ast->nodes[node->a];
        if (modref->kind != AK_ModRef ||
            modref->a >= array_count(ast->module_paths)) {
            continue;
        }

        Arena temp = {0};
        arena_init(&temp);
        ModuleResolveResult resolved = {0};
        ModuleResolveStatus status =
            module_resolve_path(&temp,
                                doc->front_end.lexer.source,
                                &doc->front_end.lexer,
                                ast,
                                &ast->module_paths[modref->a],
                                &resolved);
        JsonValue* location = NULL;
        if (status == MRS_Found) {
            location = lsp_ast_export_location_in_module(
                arena, resolved.resolved_path, symbol);
        }
        arena_done(&temp);
        if (location != NULL) {
            return location;
        }
    }
    return NULL;
}

internal JsonValue* lsp_enum_variant_location(Arena*                arena,
                                              NerdSource            source,
                                              string                uri,
                                              const Lexer*          lexer,
                                              const AstEnumVariant* variant)
{
    if (variant->token_index >= array_count(lexer->tokens)) {
        return NULL;
    }

    usize start = 0;
    usize end   = 0;
    lsp_token_offsets(lexer, variant->token_index, &start, &end);

    JsonValue* location = json_new_object(arena);
    json_object_set_string(location, arena, "uri", uri);
    json_object_set_object(
        location, "range", lsp_make_range(arena, source, start, end));
    return location;
}

internal JsonValue* lsp_local_enum_variant_location(const LspDocument* doc,
                                                    Arena*             arena,
                                                    string             uri,
                                                    u32 enum_type,
                                                    u32 variant_symbol)
{
    enum_type = sema_materialise_type(&doc->front_end.sema, enum_type);

    for (u32 node_index = 0; node_index < array_count(doc->front_end.ast.nodes);
         ++node_index) {
        const AstNode* node = &doc->front_end.ast.nodes[node_index];
        if (node->kind != AK_TypeEnum ||
            node->a >= array_count(doc->front_end.ast.enum_types)) {
            continue;
        }

        u32 candidate_type = sema_no_type();
        if (!lsp_sema_node_type(
                &doc->front_end.sema, node_index, &candidate_type) ||
            sema_materialise_type(&doc->front_end.sema, candidate_type) !=
                enum_type) {
            continue;
        }

        const AstEnumTypeInfo* enum_info =
            &doc->front_end.ast.enum_types[node->a];
        for (u32 i = 0; i < enum_info->variant_count; ++i) {
            const AstEnumVariant* variant =
                &doc->front_end.ast.enum_variants[enum_info->first_variant + i];
            if (variant->symbol_handle == variant_symbol) {
                return lsp_enum_variant_location(arena,
                                                 doc->front_end.lexer.source,
                                                 uri,
                                                 &doc->front_end.lexer,
                                                 variant);
            }
        }
    }

    return NULL;
}

internal JsonValue* lsp_local_enum_variant_location_by_symbol(
    const LspDocument* doc, Arena* arena, string uri, u32 variant_symbol)
{
    for (u32 node_index = 0; node_index < array_count(doc->front_end.ast.nodes);
         ++node_index) {
        const AstNode* node = &doc->front_end.ast.nodes[node_index];
        if (node->kind != AK_TypeEnum ||
            node->a >= array_count(doc->front_end.ast.enum_types)) {
            continue;
        }

        const AstEnumTypeInfo* enum_info =
            &doc->front_end.ast.enum_types[node->a];
        for (u32 i = 0; i < enum_info->variant_count; ++i) {
            const AstEnumVariant* variant =
                &doc->front_end.ast.enum_variants[enum_info->first_variant + i];
            if (variant->symbol_handle == variant_symbol) {
                return lsp_enum_variant_location(arena,
                                                 doc->front_end.lexer.source,
                                                 uri,
                                                 &doc->front_end.lexer,
                                                 variant);
            }
        }
    }

    return NULL;
}

internal JsonValue* lsp_ast_enum_variant_location_in_file(Arena* arena,
                                                          cstr   resolved_path,
                                                          string enum_name,
                                                          string variant_name)
{
    FileMap map    = {0};
    string  source = filemap_load(resolved_path, &map);
    if (source.data == NULL) {
        return NULL;
    }

    ErrorRenderMode previous_mode = error_system_mode();
    bool            previous_emit = error_system_should_emit_output();
    error_system_set_mode(ERROR_RENDER_DIAGNOSTICS);
    error_system_set_emit_output(false);

    JsonValue* location = NULL;
    Lexer      lexer    = {0};
    if (lex((NerdSource){.source = source, .source_path = s(resolved_path)},
            &lexer)) {
        Ast ast = ast_parse(&lexer);
        for (u32 node_index = 0; node_index < array_count(ast.nodes);
             ++node_index) {
            const AstNode* node = &ast.nodes[node_index];
            if (node->kind != AK_Bind || !ast_has_flag(node, ANF_Public) ||
                node->b >= array_count(ast.nodes)) {
                continue;
            }

            if (enum_name.count > 0 &&
                !string_eq(lex_symbol(&lexer, node->a), enum_name)) {
                continue;
            }

            const AstNode* value = &ast.nodes[node->b];
            if (value->kind != AK_TypeEnum ||
                value->a >= array_count(ast.enum_types)) {
                continue;
            }

            const AstEnumTypeInfo* enum_info = &ast.enum_types[value->a];
            for (u32 i = 0; i < enum_info->variant_count; ++i) {
                const AstEnumVariant* variant =
                    &ast.enum_variants[enum_info->first_variant + i];
                if (variant->symbol_handle != U32_MAX &&
                    string_eq(lex_symbol(&lexer, variant->symbol_handle),
                              variant_name)) {
                    location = lsp_enum_variant_location(
                        arena,
                        lexer.source,
                        lsp_path_to_uri(arena, resolved_path),
                        &lexer,
                        variant);
                    break;
                }
            }
            if (location != NULL) {
                break;
            }
        }
        ast_done(&ast);
    }
    lex_done(&lexer);

    error_system_set_mode(previous_mode);
    error_system_set_emit_output(previous_emit);
    filemap_unload(&map);
    return location;
}

internal JsonValue* lsp_ast_enum_variant_location_in_module(Arena* arena,
                                                            cstr resolved_path,
                                                            string enum_name,
                                                            string variant_name)
{
    JsonValue* location = lsp_ast_enum_variant_location_in_file(
        arena, resolved_path, enum_name, variant_name);
    if (location != NULL ||
        !string_eq(path_filename(s(resolved_path)), s("mod.n"))) {
        return location;
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
            if (!is_directory && lsp_path_is_module_part_file(path)) {
                array_push(part_paths, path);
            }
        }
        dir_iter_done(&iter);
    }

    if (array_count(part_paths) > 1) {
        qsort(part_paths,
              array_count(part_paths),
              sizeof(part_paths[0]),
              lsp_compare_cstr_ptr);
    }

    for (u32 i = 0; i < array_count(part_paths); ++i) {
        location = lsp_ast_enum_variant_location_in_file(
            arena, part_paths[i], enum_name, variant_name);
        if (location != NULL) {
            break;
        }
    }

    array_free(part_paths);
    arena_done(&temp);
    return location;
}

internal JsonValue* lsp_ast_imported_enum_variant_location(
    const LspDocument* doc, Arena* arena, string variant_name)
{
    const Ast* ast = &doc->front_end.ast;
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind != AK_Use || node->a >= array_count(ast->nodes)) {
            continue;
        }

        const AstNode* modref = &ast->nodes[node->a];
        if (modref->kind != AK_ModRef ||
            modref->a >= array_count(ast->module_paths)) {
            continue;
        }

        Arena temp = {0};
        arena_init(&temp);
        ModuleResolveResult resolved = {0};
        ModuleResolveStatus status =
            module_resolve_path(&temp,
                                doc->front_end.lexer.source,
                                &doc->front_end.lexer,
                                ast,
                                &ast->module_paths[modref->a],
                                &resolved);
        JsonValue* location = NULL;
        if (status == MRS_Found) {
            location = lsp_ast_enum_variant_location_in_module(
                arena, resolved.resolved_path, s(""), variant_name);
        }
        arena_done(&temp);
        if (location != NULL) {
            return location;
        }
    }
    return NULL;
}

internal JsonValue*
lsp_local_qualified_enum_variant_location(const LspDocument* doc,
                                          Arena*             arena,
                                          string             uri,
                                          u32                enum_symbol,
                                          u32                variant_symbol)
{
    const Ast* ast = &doc->front_end.ast;
    for (u32 node_index = 0; node_index < array_count(ast->nodes);
         ++node_index) {
        const AstNode* node = &ast->nodes[node_index];
        if (node->kind != AK_Bind || node->a != enum_symbol ||
            node->b >= array_count(ast->nodes)) {
            continue;
        }

        const AstNode* value = &ast->nodes[node->b];
        if (value->kind != AK_TypeEnum ||
            value->a >= array_count(ast->enum_types)) {
            continue;
        }

        const AstEnumTypeInfo* enum_info = &ast->enum_types[value->a];
        for (u32 i = 0; i < enum_info->variant_count; ++i) {
            const AstEnumVariant* variant =
                &ast->enum_variants[enum_info->first_variant + i];
            if (variant->symbol_handle == variant_symbol) {
                return lsp_enum_variant_location(arena,
                                                 doc->front_end.lexer.source,
                                                 uri,
                                                 &doc->front_end.lexer,
                                                 variant);
            }
        }
    }

    return NULL;
}

internal JsonValue* lsp_ast_imported_qualified_enum_variant_location(
    const LspDocument* doc, Arena* arena, string enum_name, string variant_name)
{
    const Ast* ast = &doc->front_end.ast;
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind != AK_Use || node->a >= array_count(ast->nodes)) {
            continue;
        }

        const AstNode* modref = &ast->nodes[node->a];
        if (modref->kind != AK_ModRef ||
            modref->a >= array_count(ast->module_paths)) {
            continue;
        }

        Arena temp = {0};
        arena_init(&temp);
        ModuleResolveResult resolved = {0};
        ModuleResolveStatus status =
            module_resolve_path(&temp,
                                doc->front_end.lexer.source,
                                &doc->front_end.lexer,
                                ast,
                                &ast->module_paths[modref->a],
                                &resolved);
        JsonValue* location = NULL;
        if (status == MRS_Found) {
            location = lsp_ast_enum_variant_location_in_module(
                arena, resolved.resolved_path, enum_name, variant_name);
        }
        arena_done(&temp);
        if (location != NULL) {
            return location;
        }
    }
    return NULL;
}

internal bool
lsp_enum_type_has_variant(const Sema* sema, u32 enum_type, u32 variant_symbol)
{
    enum_type            = sema_materialise_type(sema, enum_type);
    const SemaType* type = NULL;
    if (!lsp_sema_type(sema, enum_type, &type) || type->kind != STK_Enum) {
        return false;
    }

    for (u32 i = 0; i < type->param_count; ++i) {
        if (sema->type_param_symbols[type->first_param_type + i] ==
            variant_symbol) {
            return true;
        }
    }
    return false;
}

internal JsonValue* lsp_contextual_enum_variant_location(const LspDocument* doc,
                                                         Arena* arena,
                                                         string uri,
                                                         u32    token_index)
{
    u32 ref_node_index =
        lsp_find_symbol_ref_node_at_token(&doc->front_end.ast, token_index);
    if (ref_node_index == U32_MAX) {
        return NULL;
    }

    const AstNode* ref       = &doc->front_end.ast.nodes[ref_node_index];
    u32            enum_type = sema_no_type();
    if (!lsp_sema_node_type(&doc->front_end.sema, ref_node_index, &enum_type) ||
        !lsp_enum_type_has_variant(&doc->front_end.sema, enum_type, ref->a)) {
        return NULL;
    }

    JsonValue* location =
        lsp_local_enum_variant_location(doc, arena, uri, enum_type, ref->a);
    if (location != NULL) {
        return location;
    }

    return lsp_ast_imported_enum_variant_location(
        doc, arena, lex_symbol(&doc->front_end.lexer, ref->a));
}

internal JsonValue* lsp_qualified_enum_variant_location(const LspDocument* doc,
                                                        Arena* arena,
                                                        string uri,
                                                        u32    field_node_index)
{
    if (field_node_index >= array_count(doc->front_end.ast.nodes)) {
        return NULL;
    }

    const AstNode* field = &doc->front_end.ast.nodes[field_node_index];
    if (field->kind != AK_Field) {
        return NULL;
    }

    if (field->a >= array_count(doc->front_end.ast.nodes)) {
        return NULL;
    }

    const AstNode* target = &doc->front_end.ast.nodes[field->a];
    if (target->kind != AK_SymbolRef) {
        return NULL;
    }

    JsonValue* location = lsp_local_qualified_enum_variant_location(
        doc, arena, uri, target->a, field->b);
    if (location != NULL) {
        return location;
    }

    return lsp_ast_imported_qualified_enum_variant_location(
        doc,
        arena,
        lex_symbol(&doc->front_end.lexer, target->a),
        lex_symbol(&doc->front_end.lexer, field->b));
}

internal JsonValue* lsp_enum_pattern_variant_location(const LspDocument* doc,
                                                      Arena*             arena,
                                                      string             uri,
                                                      u32 token_index)
{
    const Ast* ast = &doc->front_end.ast;
    for (u32 pattern_index = 0; pattern_index < array_count(ast->patterns);
         ++pattern_index) {
        const AstPattern* pattern = &ast->patterns[pattern_index];
        if (pattern->kind != APK_EnumVariant ||
            pattern->a >= array_count(ast->enum_patterns)) {
            continue;
        }

        const AstEnumPattern* enum_pattern = &ast->enum_patterns[pattern->a];
        if (enum_pattern->token_index != token_index ||
            enum_pattern->symbol_handle == U32_MAX) {
            continue;
        }

        string variant_name =
            lex_symbol(&doc->front_end.lexer, enum_pattern->symbol_handle);
        if (enum_pattern->qualifier_node_index != U32_MAX &&
            enum_pattern->qualifier_node_index < array_count(ast->nodes)) {
            const AstNode* qualifier =
                &ast->nodes[enum_pattern->qualifier_node_index];
            if (qualifier->kind == AK_SymbolRef && qualifier->a != U32_MAX) {
                string enum_name =
                    lex_symbol(&doc->front_end.lexer, qualifier->a);
                JsonValue* location = lsp_local_qualified_enum_variant_location(
                    doc, arena, uri, qualifier->a, enum_pattern->symbol_handle);
                if (location != NULL) {
                    return location;
                }

                return lsp_ast_imported_qualified_enum_variant_location(
                    doc, arena, enum_name, variant_name);
            }
        }

        JsonValue* location = lsp_local_enum_variant_location_by_symbol(
            doc, arena, uri, enum_pattern->symbol_handle);
        if (location != NULL) {
            return location;
        }

        return lsp_ast_imported_enum_variant_location(doc, arena, variant_name);
    }

    return NULL;
}

internal string lsp_hover_text_from_module_export(Arena* arena,
                                                  cstr   resolved_path,
                                                  string symbol)
{
    FileMap map    = {0};
    string  source = filemap_load(resolved_path, &map);
    if (source.data == NULL) {
        return s("");
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

    string        hover  = s("");
    LspModuleView module = {0};
    if (ok &&
        lsp_program_module_view(&program, program.root_module_index, &module)) {
        for (u32 i = 0; i < lsp_module_export_count(&module); ++i) {
            const SemaDecl* decl       = NULL;
            u32             decl_index = sema_no_decl();
            if (!lsp_module_export_decl(&module, i, &decl, &decl_index) ||
                decl->symbol_handle == U32_MAX ||
                !string_eq(lex_symbol(module.lexer, decl->symbol_handle),
                           symbol)) {
                continue;
            }

            LspDocument module_doc     = {0};
            module_doc.source          = module.lexer->source.source;
            module_doc.program         = program;
            module_doc.front_end.lexer = *module.lexer;
            module_doc.front_end.ast   = *module.ast;
            module_doc.front_end.sema  = *module.sema;
            hover = lsp_decl_hover_text(&module_doc, arena, decl_index);
            break;
        }
    }

    program_info_done(&program);
    filemap_unload(&map);
    return hover;
}

internal string lsp_imported_symbol_hover_text(const LspDocument* doc,
                                               Arena*             arena,
                                               string             symbol)
{
    const Ast* ast = &doc->front_end.ast;
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind != AK_Use || node->a >= array_count(ast->nodes)) {
            continue;
        }

        const AstNode* modref = &ast->nodes[node->a];
        if (modref->kind != AK_ModRef ||
            modref->a >= array_count(ast->module_paths)) {
            continue;
        }

        Arena temp = {0};
        arena_init(&temp);
        ModuleResolveResult resolved = {0};
        ModuleResolveStatus status =
            module_resolve_path(&temp,
                                doc->front_end.lexer.source,
                                &doc->front_end.lexer,
                                ast,
                                &ast->module_paths[modref->a],
                                &resolved);
        string hover = s("");
        if (status == MRS_Found) {
            hover = lsp_hover_text_from_module_export(
                arena, resolved.resolved_path, symbol);
        }
        arena_done(&temp);
        if (hover.count != 0) {
            return hover;
        }
    }
    return s("");
}

internal u32 lsp_find_local_index_for_token(const LspDocument* doc,
                                            u32                token_index)
{
    const Ast*  ast  = &doc->front_end.ast;
    const Sema* sema = &doc->front_end.sema;
    for (u32 i = 0; i < array_count(ast->patterns); ++i) {
        const AstPattern* pattern = &ast->patterns[i];
        if (pattern->token_index == token_index &&
            i < array_count(sema->pattern_local_indices)) {
            u32 local_index = sema->pattern_local_indices[i];
            if (lsp_sema_local(sema, local_index, NULL)) {
                return local_index;
            }
        }
    }

    for (u32 i = 0; i < array_count(ast->on_branches); ++i) {
        const AstOnBranch* branch = &ast->on_branches[i];
        if (branch->binder_token_index == token_index &&
            i < array_count(sema->on_branch_local_indices)) {
            return sema->on_branch_local_indices[i];
        }
    }

    u32 bind_node_index =
        lsp_find_bind_node_at_token(&doc->front_end.ast, token_index);
    u32 local_index = sema_no_local();
    if (lsp_sema_node_local(
            &doc->front_end.sema, bind_node_index, &local_index)) {
        return local_index;
    }

    u32 ref_node_index =
        lsp_find_symbol_ref_node_at_token(&doc->front_end.ast, token_index);
    if (lsp_sema_node_local(
            &doc->front_end.sema, ref_node_index, &local_index)) {
        return local_index;
    }

    return sema_no_local();
}

//------------------------------------------------------------------------------
// Evaluate a top-level constant declaration to a signed integer when possible.

internal bool
lsp_eval_decl_value(const LspDocument* doc, u32 decl_index, i64* out_value);

//------------------------------------------------------------------------------
// Evaluate one AST expression subtree to a signed integer when possible.

internal bool
lsp_eval_ast_node(const LspDocument* doc, u32 node_index, i64* out_value)
{
    const AstNode* node = &doc->front_end.ast.nodes[node_index];

    switch (node->kind) {
    case AK_IntegerLiteral:
        *out_value = (i64)ast_get_integer(&doc->front_end.lexer, node);
        return true;

    case AK_SymbolRef:
        {
            u32 decl_index = LSP_NO_DECL;
            if (!lsp_sema_node_decl(
                    &doc->front_end.sema, node_index, &decl_index)) {
                return false;
            }
            return lsp_eval_decl_value(doc, decl_index, out_value);
        }

    case AK_IntegerNegate:
        {
            i64 value = 0;
            if (!lsp_eval_ast_node(doc, node->a, &value)) {
                return false;
            }
            *out_value = -value;
            return true;
        }

    case AK_IntegerPlus:
    case AK_IntegerMinus:
    case AK_IntegerMultiply:
    case AK_IntegerDivide:
    case AK_IntegerModulo:
        {
            i64 left  = 0;
            i64 right = 0;
            if (!lsp_eval_ast_node(doc, node->a, &left) ||
                !lsp_eval_ast_node(doc, node->b, &right)) {
                return false;
            }

            switch (node->kind) {
            case AK_IntegerPlus:
                *out_value = left + right;
                return true;
            case AK_IntegerMinus:
                *out_value = left - right;
                return true;
            case AK_IntegerMultiply:
                *out_value = left * right;
                return true;
            case AK_IntegerDivide:
                if (right == 0) {
                    return false;
                }
                *out_value = left / right;
                return true;
            case AK_IntegerModulo:
                if (right == 0) {
                    return false;
                }
                *out_value = left % right;
                return true;
            default:
                return false;
            }
        }

    case AK_Expression:
    case AK_Statement:
        return lsp_eval_ast_node(doc, node->a, out_value);

    default:
        return false;
    }
}

//------------------------------------------------------------------------------
// Evaluate one top-level declaration when it represents a constant.

internal bool
lsp_eval_decl_value(const LspDocument* doc, u32 decl_index, i64* out_value)
{
    const SemaDecl* decl = NULL;
    if (!lsp_sema_decl(&doc->front_end.sema, decl_index, &decl)) {
        return false;
    }
    if (decl->kind != SK_Constant) {
        return false;
    }
    if (decl->value_node_index == U32_MAX ||
        decl->value_node_index >= array_count(doc->front_end.ast.nodes)) {
        return false;
    }

    return lsp_eval_ast_node(doc, decl->value_node_index, out_value);
}

//------------------------------------------------------------------------------
// Return the current signature text for one function declaration.

internal string lsp_default_param_source(const LspDocument* doc,
                                         const AstParam*    param)
{
    if (param->default_node_index == U32_MAX) {
        return s("...");
    }

    const Lexer* lexer = &doc->front_end.lexer;
    if (param->token_index >= array_count(lexer->tokens)) {
        return s("...");
    }

    u32 equal_token = U32_MAX;
    u32 depth       = 0;
    for (u32 i = param->token_index; i < array_count(lexer->tokens); ++i) {
        TokenKind kind = lexer->tokens[i].kind;
        if (depth == 0 && kind == TK_Equal) {
            equal_token = i;
            break;
        }
        if (kind == TK_LParen || kind == TK_LBracket || kind == TK_LBrace) {
            depth++;
        } else if (kind == TK_RParen || kind == TK_RBracket ||
                   kind == TK_RBrace) {
            if (depth == 0) {
                break;
            }
            depth--;
        } else if (depth == 0 && kind == TK_Comma) {
            break;
        }
    }
    if (equal_token == U32_MAX) {
        return s("...");
    }

    depth       = 0;
    usize start = lex_token_end_offset(lexer, &lexer->tokens[equal_token]);
    usize end   = start;
    for (u32 i = equal_token + 1; i < array_count(lexer->tokens); ++i) {
        TokenKind kind = lexer->tokens[i].kind;
        if (depth == 0 && (kind == TK_Comma || kind == TK_RParen)) {
            end = lexer->tokens[i].offset;
            break;
        }
        if (kind == TK_LParen || kind == TK_LBracket || kind == TK_LBrace) {
            depth++;
        } else if (kind == TK_RParen || kind == TK_RBracket ||
                   kind == TK_RBrace) {
            if (depth == 0) {
                end = lexer->tokens[i].offset;
                break;
            }
            depth--;
        }
        end = lex_token_end_offset(lexer, &lexer->tokens[i]);
    }

    while (start < end && (lexer->source.source.data[start] == ' ' ||
                           lexer->source.source.data[start] == '\t' ||
                           lexer->source.source.data[start] == '\n' ||
                           lexer->source.source.data[start] == '\r')) {
        start++;
    }
    while (end > start && (lexer->source.source.data[end - 1] == ' ' ||
                           lexer->source.source.data[end - 1] == '\t' ||
                           lexer->source.source.data[end - 1] == '\n' ||
                           lexer->source.source.data[end - 1] == '\r')) {
        end--;
    }
    string raw = {
        .data  = lexer->source.source.data + start,
        .count = end - start,
    };
    return raw;
}

internal string lsp_trim_source(const LspDocument* doc, usize start, usize end)
{
    const string source = doc->front_end.lexer.source.source;
    while (start < end &&
           (source.data[start] == ' ' || source.data[start] == '\t' ||
            source.data[start] == '\n' || source.data[start] == '\r')) {
        start++;
    }
    while (end > start &&
           (source.data[end - 1] == ' ' || source.data[end - 1] == '\t' ||
            source.data[end - 1] == '\n' || source.data[end - 1] == '\r')) {
        end--;
    }
    return (string){.data = source.data + start, .count = end - start};
}

internal string lsp_param_type_source(const LspDocument* doc,
                                      const AstParam*    param)
{
    const Lexer* lexer = &doc->front_end.lexer;
    if (param->type_node_index < array_count(doc->front_end.ast.nodes)) {
        const AstNode* node = &doc->front_end.ast.nodes[param->type_node_index];
        if (node->token_index < array_count(lexer->tokens)) {
            usize start = lexer->tokens[node->token_index].offset;
            usize end   = start;
            u32   depth = 0;
            for (u32 i = node->token_index; i < array_count(lexer->tokens);
                 ++i) {
                TokenKind kind = lexer->tokens[i].kind;
                if (depth == 0 && (kind == TK_Comma || kind == TK_RParen ||
                                   kind == TK_Equal)) {
                    end = lexer->tokens[i].offset;
                    break;
                }
                if (kind == TK_LParen || kind == TK_LBracket) {
                    depth++;
                } else if (kind == TK_RParen || kind == TK_RBracket) {
                    if (depth == 0) {
                        end = lexer->tokens[i].offset;
                        break;
                    }
                    depth--;
                }
                end = lex_token_end_offset(lexer, &lexer->tokens[i]);
            }
            return lsp_trim_source(doc, start, end);
        }
    }

    u32 colon = U32_MAX;
    for (u32 i = param->token_index; i < array_count(lexer->tokens); ++i) {
        TokenKind kind = lexer->tokens[i].kind;
        if (kind == TK_Colon) {
            colon = i;
            break;
        }
        if (kind == TK_Comma || kind == TK_RParen) {
            break;
        }
    }
    if (colon == U32_MAX) {
        return s("<unknown>");
    }

    usize start = lex_token_end_offset(lexer, &lexer->tokens[colon]);
    usize end   = start;
    u32   depth = 0;
    for (u32 i = colon + 1; i < array_count(lexer->tokens); ++i) {
        TokenKind kind = lexer->tokens[i].kind;
        if (depth == 0 &&
            (kind == TK_Comma || kind == TK_RParen || kind == TK_Equal)) {
            end = lexer->tokens[i].offset;
            break;
        }
        if (kind == TK_LParen || kind == TK_LBracket) {
            depth++;
        } else if (kind == TK_RParen || kind == TK_RBracket) {
            if (depth == 0) {
                end = lexer->tokens[i].offset;
                break;
            }
            depth--;
        }
        end = lex_token_end_offset(lexer, &lexer->tokens[i]);
    }
    return lsp_trim_source(doc, start, end);
}

internal string lsp_return_type_source(const LspDocument* doc,
                                       u32 return_type_node_index)
{
    if (return_type_node_index == U32_MAX) {
        return s("");
    }

    const Lexer*   lexer = &doc->front_end.lexer;
    const AstNode* node  = &doc->front_end.ast.nodes[return_type_node_index];
    usize          start = lexer->tokens[node->token_index].offset;
    usize          end   = start;
    u32            depth = 0;
    for (u32 i = node->token_index; i < array_count(lexer->tokens); ++i) {
        TokenKind kind = lexer->tokens[i].kind;
        if (depth == 0 && i > node->token_index) {
            usize previous_end =
                lex_token_end_offset(lexer, &lexer->tokens[i - 1]);
            for (usize j = previous_end; j < lexer->tokens[i].offset; ++j) {
                if (lexer->source.source.data[j] == '\n' ||
                    lexer->source.source.data[j] == '\r') {
                    return lsp_trim_source(doc, start, end);
                }
            }
        }
        if (depth == 0 && (kind == TK_LBrace || kind == TK_FatArrow)) {
            end = lexer->tokens[i].offset;
            break;
        }
        if (kind == TK_LParen || kind == TK_LBracket) {
            depth++;
        } else if (kind == TK_RParen || kind == TK_RBracket) {
            if (depth == 0) {
                end = lexer->tokens[i].offset;
                break;
            }
            depth--;
        }
        end = lex_token_end_offset(lexer, &lexer->tokens[i]);
    }
    return lsp_trim_source(doc, start, end);
}

internal bool lsp_decl_ast_signature(const LspDocument* doc,
                                     Arena*             arena,
                                     const SemaDecl*    decl,
                                     string*            out_signature)
{
    if (decl->import_module_index != sema_no_decl() &&
        decl->import_decl_index != sema_no_decl()) {
        LspModuleView module = {0};
        if (lsp_program_module_view(
                &doc->program, decl->import_module_index, &module)) {
            const SemaDecl* imported_decl = NULL;
            if (lsp_sema_decl(
                    module.sema, decl->import_decl_index, &imported_decl)) {
                LspDocument module_doc     = *doc;
                module_doc.source          = module.lexer->source.source;
                module_doc.front_end.lexer = *module.lexer;
                module_doc.front_end.ast   = *module.ast;
                module_doc.front_end.sema  = *module.sema;
                return lsp_decl_ast_signature(
                    &module_doc, arena, imported_decl, out_signature);
            }
        }
    }

    if (decl->value_node_index == sema_no_decl() ||
        (decl->kind != SK_GenericFunction &&
         decl->type_index >= array_count(doc->front_end.sema.types))) {
        return false;
    }

    const Ast*            ast        = &doc->front_end.ast;
    const AstNode*        value_node = &ast->nodes[decl->value_node_index];
    const AstFnSignature* signature  = NULL;

    if (value_node->kind == AK_FnDef) {
        const AstNode* fn_start = &ast->nodes[value_node->a];
        signature               = &ast->fn_signatures[fn_start->a];
    } else if (value_node->kind == AK_FfiDef) {
        const AstFfiInfo* ffi = &ast->ffi_infos[value_node->a];
        signature             = &ast->fn_signatures[ffi->signature_index];
    } else {
        return false;
    }

    const SemaType* type = NULL;
    if (decl->kind != SK_GenericFunction &&
        (!lsp_sema_type(&doc->front_end.sema, decl->type_index, &type) ||
         type->kind != STK_Function)) {
        return false;
    }
    bool  has_generic = signature->generic_params_index != U32_MAX;
    Arena build_arena = {0};
    Arena text_arena  = {0};
    arena_init(&build_arena);
    arena_init(&text_arena);

    StringBuilder sb = {0};
    sb_init(&sb, &build_arena);
    sb_append_cstr(&sb, "fn");
    if (has_generic) {
        const AstGenericParams* generic =
            &ast->generic_params[signature->generic_params_index];
        sb_append_cstr(&sb, " [");
        for (u32 i = 0; i < generic->symbol_count; ++i) {
            if (i > 0) {
                sb_append_cstr(&sb, ", ");
            }
            sb_append_string(
                &sb,
                lex_symbol(
                    &doc->front_end.lexer,
                    ast->generic_param_symbols[generic->first_symbol + i]));
        }
        sb_append_cstr(&sb, "]");
    }
    sb_append_cstr(&sb, " (");
    for (u32 i = 0; i < signature->param_count; ++i) {
        if (i > 0) {
            sb_append_cstr(&sb, ", ");
        }
        const AstParam* param = &ast->params[signature->first_param + i];
        if (param->symbol_handle != U32_MAX) {
            sb_append_string(
                &sb, lex_symbol(&doc->front_end.lexer, param->symbol_handle));
            sb_append_cstr(&sb, ": ");
        }
        string source_type = lsp_param_type_source(doc, param);
        if (!string_eq(source_type, s("<unknown>"))) {
            sb_append_string(&sb, source_type);
        } else {
            u32 param_type =
                i < type->param_count
                    ? doc->front_end.sema
                          .type_param_types[type->first_param_type + i]
                    : sema_no_type();
            sb_append_string(&sb,
                             sema_type_name(&doc->front_end.lexer,
                                            &doc->front_end.sema,
                                            &text_arena,
                                            param_type));
        }
        if (param->default_node_index != U32_MAX) {
            sb_append_cstr(&sb, " = ");
            sb_append_string(&sb, lsp_default_param_source(doc, param));
        }
    }
    if (signature->is_varargs) {
        if (signature->param_count > 0) {
            sb_append_cstr(&sb, ", ");
        }
        sb_append_cstr(&sb, "...");
    }
    sb_append_char(&sb, ')');
    if (signature->return_type_node_index != U32_MAX) {
        sb_append_cstr(&sb, " -> ");
        sb_append_string(
            &sb,
            lsp_return_type_source(doc, signature->return_type_node_index));
    } else if (!has_generic && type->return_type != sema_no_type()) {
        sb_append_cstr(&sb, " -> ");
        sb_append_string(&sb,
                         sema_type_name(&doc->front_end.lexer,
                                        &doc->front_end.sema,
                                        &text_arena,
                                        type->return_type));
    }

    string built   = sb_to_string(&sb);
    *out_signature = string_format(arena, STRINGP, STRINGV(built));
    arena_done(&text_arena);
    arena_done(&build_arena);
    return true;
}

internal string lsp_decl_signature(const LspDocument* doc,
                                   Arena*             arena,
                                   const SemaDecl*    decl)
{
    if (decl->kind != SK_Function && decl->kind != SK_GenericFunction &&
        decl->kind != SK_FfiFunction && decl->kind != SK_BuiltinFunction) {
        return s("<unknown>");
    }
    if (decl->kind == SK_BuiltinFunction) {
        string name = lex_symbol(&doc->front_end.lexer, decl->symbol_handle);
        if (string_eq(name, s("pr")) || string_eq(name, s("prn"))) {
            string rendered = sema_type_name(&doc->front_end.lexer,
                                             &doc->front_end.sema,
                                             arena,
                                             decl->type_index);
            return string_eq(rendered, s("<unknown>"))
                       ? s("fn (string) -> void")
                       : rendered;
        }
    }
    string name = lex_symbol(&doc->front_end.lexer, decl->symbol_handle);
    if (string_eq(name, s("main")) &&
        decl->value_node_index != sema_no_decl()) {
        const AstNode* fn_def =
            &doc->front_end.ast.nodes[decl->value_node_index];
        if (fn_def->kind == AK_FnDef && fn_def->b == AFK_Block) {
            const AstNode* fn_start = &doc->front_end.ast.nodes[fn_def->a];
            const AstFnSignature* signature =
                &doc->front_end.ast.fn_signatures[fn_start->a];
            bool has_explicit_return_type =
                signature->return_type_node_index != U32_MAX;
            bool has_return = false;
            for (u32 i = fn_def->a + 1; i < fn_start->b; ++i) {
                const AstNode* node = &doc->front_end.ast.nodes[i];
                if (node->kind == AK_Return) {
                    has_return = true;
                    break;
                }
                if (node->kind == AK_FnStart || node->kind == AK_Block) {
                    i = node->b - 1;
                }
            }
            if (!has_explicit_return_type && !has_return) {
                return s("fn () -> void");
            }
        }
    }
    string ast_signature = {0};
    if (lsp_decl_ast_signature(doc, arena, decl, &ast_signature)) {
        return ast_signature;
    }
    return sema_type_name(
        &doc->front_end.lexer, &doc->front_end.sema, arena, decl->type_index);
}

internal string lsp_trim_comment_text(string text)
{
    usize start = 0;
    usize end   = text.count;
    if (start < end && text.data[start] == ' ') {
        start++;
    }
    while (end > start &&
           (text.data[end - 1] == ' ' || text.data[end - 1] == '\t')) {
        end--;
    }
    return (string){.data = text.data + start, .count = end - start};
}

internal string lsp_decl_doc_comment(const LspDocument* doc,
                                     Arena*             arena,
                                     const SemaDecl*    decl)
{
    if (decl->import_module_index != sema_no_decl() &&
        decl->import_decl_index != sema_no_decl()) {
        LspModuleView module = {0};
        if (lsp_program_module_view(
                &doc->program, decl->import_module_index, &module)) {
            const SemaDecl* imported_decl = NULL;
            if (lsp_sema_decl(
                    module.sema, decl->import_decl_index, &imported_decl)) {
                LspDocument module_doc     = *doc;
                module_doc.source          = module.lexer->source.source;
                module_doc.front_end.lexer = *module.lexer;
                module_doc.front_end.ast   = *module.ast;
                module_doc.front_end.sema  = *module.sema;
                return lsp_decl_doc_comment(&module_doc, arena, imported_decl);
            }
        }
    }

    if (decl->bind_node_index == sema_no_decl() ||
        decl->bind_node_index >= array_count(doc->front_end.ast.nodes)) {
        return s("");
    }

    const AstNode* bind = &doc->front_end.ast.nodes[decl->bind_node_index];
    if (bind->token_index >= array_count(doc->front_end.lexer.tokens)) {
        return s("");
    }

    string source     = doc->front_end.lexer.source.source;
    usize  decl_start = doc->front_end.lexer.tokens[bind->token_index].offset;
    if (decl_start > source.count) {
        return s("");
    }

    usize line_start = decl_start;
    while (line_start > 0 && source.data[line_start - 1] != '\n') {
        line_start--;
    }

    Array(string) lines = NULL;
    usize cursor        = line_start;
    while (cursor > 0) {
        usize line_end = cursor - 1;
        if (line_end > 0 && source.data[line_end - 1] == '\r') {
            line_end--;
        }

        usize prev_start = line_end;
        while (prev_start > 0 && source.data[prev_start - 1] != '\n') {
            prev_start--;
        }

        usize text_start = prev_start;
        while (text_start < line_end && (source.data[text_start] == ' ' ||
                                         source.data[text_start] == '\t')) {
            text_start++;
        }

        if (text_start + 2 > line_end || source.data[text_start] != '-' ||
            source.data[text_start + 1] != '-') {
            break;
        }

        string comment = string_from(source.data + text_start + 2,
                                     line_end - text_start - 2);
        array_push(lines, lsp_trim_comment_text(comment));
        cursor = prev_start;
    }

    if (array_count(lines) == 0) {
        return s("");
    }

    StringBuilder sb = {0};
    sb_init(&sb, arena);
    for (u32 i = array_count(lines); i > 0; --i) {
        if (i != array_count(lines)) {
            sb_append_char(&sb, '\n');
        }
        sb_append_string(&sb, lines[i - 1]);
    }
    array_free(lines);
    return sb_to_string(&sb);
}

//------------------------------------------------------------------------------
// Infer the current hover-facing type for one AST node.

internal string lsp_infer_ast_type(const LspDocument* doc,
                                   Arena*             arena,
                                   u32                node_index)
{
    const AstNode* node = &doc->front_end.ast.nodes[node_index];
    UNUSED(node);

    u32 type_index = sema_no_type();
    if (!lsp_sema_node_type(&doc->front_end.sema, node_index, &type_index)) {
        return s("<unknown>");
    }

    return sema_type_name(
        &doc->front_end.lexer, &doc->front_end.sema, arena, type_index);
}

//------------------------------------------------------------------------------
// Return one markdown code block with the Nerd language tag.

internal string lsp_markdown_code_block(Arena* arena, string code)
{
    return string_format(arena, "```nerd\n" STRINGP "\n```", STRINGV(code));
}

//------------------------------------------------------------------------------
// Attach a markdown hover result to the response.

internal void
lsp_set_markdown_hover(JsonValue* response, Arena* arena, string value)
{
    JsonValue* result   = json_new_object(arena);
    JsonValue* contents = json_new_object(arena);
    json_object_set_string(contents, arena, "kind", s("markdown"));
    json_object_set_string(contents, arena, "value", value);
    json_object_set_object(result, "contents", contents);
    json_object_set_object(response, "result", result);
}

//------------------------------------------------------------------------------
// Return a small hover summary for one semantic declaration.

internal string lsp_decl_hover_text(const LspDocument* doc,
                                    Arena*             arena,
                                    u32                decl_index)
{
    const SemaDecl* decl = NULL;
    if (!lsp_sema_decl(&doc->front_end.sema, decl_index, &decl)) {
        return s("<unknown>");
    }
    string name = lex_symbol(&doc->front_end.lexer, decl->symbol_handle);
    string kind = s("value");
    string inferred_type = s("<unknown>");
    if (decl->kind == SK_TypeAlias) {
        kind          = s("type alias");
        inferred_type = sema_type_name(&doc->front_end.lexer,
                                       &doc->front_end.sema,
                                       arena,
                                       decl->type_index);
    } else if (decl->kind == SK_Constant) {
        kind = s("constant");
        if (decl->import_module_index != sema_no_decl()) {
            inferred_type = decl->type_index == sema_no_type()
                                ? s("<unknown>")
                                : sema_type_name(&doc->front_end.lexer,
                                                 &doc->front_end.sema,
                                                 arena,
                                                 decl->type_index);
        } else {
            inferred_type =
                lsp_infer_ast_type(doc, arena, decl->value_node_index);
        }
    } else if (decl->kind == SK_Variable) {
        kind          = s("variable");
        inferred_type = sema_type_name(&doc->front_end.lexer,
                                       &doc->front_end.sema,
                                       arena,
                                       decl->type_index);
    } else if (decl->kind == SK_Module) {
        kind          = s("module");
        inferred_type = sema_type_name(&doc->front_end.lexer,
                                       &doc->front_end.sema,
                                       arena,
                                       decl->type_index);
    } else if (decl->kind == SK_Trait) {
        kind          = s("trait");
        inferred_type = s("trait");
    } else {
        kind          = s("function");
        inferred_type = lsp_decl_signature(doc, arena, decl);
    }

    if (decl->kind == SK_Function || decl->kind == SK_GenericFunction ||
        decl->kind == SK_FfiFunction || decl->kind == SK_BuiltinFunction) {
        string comment = lsp_decl_doc_comment(doc, arena, decl);
        string suffix =
            comment.count == 0
                ? s("")
                : string_format(arena, "\n\n" STRINGP, STRINGV(comment));
        return string_format(arena,
                             STRINGP "\n\n- Kind: " STRINGP STRINGP,
                             STRINGV(lsp_markdown_code_block(
                                 arena,
                                 string_format(arena,
                                               STRINGP " :: " STRINGP,
                                               STRINGV(name),
                                               STRINGV(inferred_type)))),
                             STRINGV(kind),
                             STRINGV(suffix));
    }

    if (decl->kind == SK_TypeAlias) {
        return string_format(arena,
                             STRINGP "\n\n- Kind: " STRINGP
                                     "\n- Type: `" STRINGP "`",
                             STRINGV(lsp_markdown_code_block(
                                 arena,
                                 string_format(arena,
                                               STRINGP " :: " STRINGP,
                                               STRINGV(name),
                                               STRINGV(inferred_type)))),
                             STRINGV(kind),
                             STRINGV(inferred_type));
    }

    if (decl->kind == SK_Trait) {
        return string_format(
            arena,
            STRINGP "\n\n- Kind: " STRINGP,
            STRINGV(lsp_markdown_code_block(
                arena,
                string_format(arena, STRINGP " :: trait", STRINGV(name)))),
            STRINGV(kind));
    }

    i64 value = 0;
    if (decl->kind == SK_Constant &&
        lsp_eval_decl_value(doc, decl_index, &value)) {
        return string_format(
            arena,
            STRINGP "\n\n- Kind: " STRINGP "\n- Type: `" STRINGP "`"
                    "\n- Value: `%lld`",
            STRINGV(lsp_markdown_code_block(
                arena,
                string_format(
                    arena, STRINGP " :: %lld", STRINGV(name), value))),
            STRINGV(kind),
            STRINGV(inferred_type),
            value);
    }

    return string_format(
        arena,
        STRINGP "\n\n- Kind: " STRINGP "\n- Type: `" STRINGP "`",
        STRINGV(lsp_markdown_code_block(
            arena, string_format(arena, STRINGP, STRINGV(name)))),
        STRINGV(kind),
        STRINGV(inferred_type));
}

internal string lsp_method_hover_text(const LspDocument* doc,
                                      Arena*             arena,
                                      u32                decl_index)
{
    const SemaDecl* decl = NULL;
    if (!lsp_sema_decl(&doc->front_end.sema, decl_index, &decl)) {
        return s("");
    }

    const SemaMethod* method = NULL;
    for (u32 i = 0; i < array_count(doc->front_end.sema.methods); ++i) {
        if (doc->front_end.sema.methods[i].decl_index == decl_index) {
            method = &doc->front_end.sema.methods[i];
            break;
        }
    }
    if (method == NULL) {
        return lsp_decl_hover_text(doc, arena, decl_index);
    }

    string name      = lex_symbol(&doc->front_end.lexer, method->symbol_handle);
    string signature = lsp_decl_signature(doc, arena, decl);
    string rendered  = string_format(
        arena, STRINGP " :: " STRINGP, STRINGV(name), STRINGV(signature));
    string comment = lsp_decl_doc_comment(doc, arena, decl);
    string suffix =
        comment.count == 0
            ? s("")
            : string_format(arena, "\n\n" STRINGP, STRINGV(comment));

    return string_format(arena,
                         STRINGP "\n\n- Kind: method" STRINGP,
                         STRINGV(lsp_markdown_code_block(arena, rendered)),
                         STRINGV(suffix));
}

internal string lsp_local_hover_text(const LspDocument* doc,
                                     Arena*             arena,
                                     u32                local_index)
{
    const SemaLocal* local = NULL;
    if (!lsp_sema_local(&doc->front_end.sema, local_index, &local)) {
        return s("<unknown>");
    }
    string name = lex_symbol(&doc->front_end.lexer, local->symbol_handle);
    string type = sema_type_name(
        &doc->front_end.lexer, &doc->front_end.sema, arena, local->type_index);
    string kind =
        local->kind == SLK_Binder ? s("pattern binder") : s("local variable");

    return string_format(
        arena,
        STRINGP "\n\n- Kind: " STRINGP "\n- Type: `" STRINGP "`",
        STRINGV(lsp_markdown_code_block(
            arena, string_format(arena, STRINGP, STRINGV(name)))),
        STRINGV(kind),
        STRINGV(type));
}

internal string lsp_record_field_hover_text(const LspDocument* doc,
                                            Arena*             arena,
                                            u32                owner_type,
                                            u32                field_symbol,
                                            string             kind)
{
    u32 field_type = sema_no_type();
    if (!lsp_record_field_type(
            &doc->front_end.sema, owner_type, field_symbol, &field_type)) {
        return s("");
    }

    string name = lex_symbol(&doc->front_end.lexer, field_symbol);
    string type = sema_type_name(
        &doc->front_end.lexer, &doc->front_end.sema, arena, field_type);
    string owner = sema_type_name(
        &doc->front_end.lexer, &doc->front_end.sema, arena, owner_type);

    return string_format(
        arena,
        STRINGP "\n\n- Kind: " STRINGP "\n- Type: `" STRINGP "`"
                "\n- Owner: `" STRINGP "`",
        STRINGV(lsp_markdown_code_block(
            arena, string_format(arena, STRINGP, STRINGV(name)))),
        STRINGV(kind),
        STRINGV(type),
        STRINGV(owner));
}

internal string lsp_plex_literal_field_hover_text(const LspDocument* doc,
                                                  Arena*             arena,
                                                  u32 literal_node_index,
                                                  u32 field_index)
{
    if (literal_node_index >= array_count(doc->front_end.ast.nodes) ||
        field_index >= array_count(doc->front_end.ast.plex_literal_fields)) {
        return s("");
    }

    u32 owner_type = sema_no_type();
    if (!lsp_sema_node_type(
            &doc->front_end.sema, literal_node_index, &owner_type)) {
        return s("");
    }

    const SemaType* owner = NULL;
    if (!lsp_sema_type(&doc->front_end.sema, owner_type, &owner)) {
        return s("");
    }

    string kind = owner->kind == STK_Union ? s("union field") : s("plex field");
    return lsp_record_field_hover_text(
        doc,
        arena,
        owner_type,
        doc->front_end.ast.plex_literal_fields[field_index].symbol_handle,
        kind);
}

internal bool lsp_pattern_expected_type_from(const LspDocument* doc,
                                             u32                pattern_index,
                                             u32                value_type,
                                             u32  target_pattern_index,
                                             u32* out_type)
{
    if (pattern_index >= array_count(doc->front_end.ast.patterns)) {
        return false;
    }
    if (pattern_index == target_pattern_index) {
        *out_type = value_type;
        return true;
    }

    const Ast*        ast     = &doc->front_end.ast;
    const Sema*       sema    = &doc->front_end.sema;
    const SemaType*   value_t = NULL;
    const AstPattern* pattern = &ast->patterns[pattern_index];

    switch (pattern->kind) {
    case APK_Bind:
        return pattern->b != U32_MAX &&
               lsp_pattern_expected_type_from(
                   doc, pattern->b, value_type, target_pattern_index, out_type);
    case APK_Tuple:
        if (!lsp_sema_type(sema, value_type, &value_t) ||
            value_t->kind != STK_Tuple) {
            return false;
        }
        for (u32 i = 0; i < pattern->b; ++i) {
            u32 item_index = pattern->a + i;
            if (item_index >= array_count(ast->pattern_items) ||
                i >= value_t->param_count) {
                break;
            }
            u32 item_type =
                sema->type_param_types[value_t->first_param_type + i];
            if (lsp_pattern_expected_type_from(doc,
                                               ast->pattern_items[item_index],
                                               item_type,
                                               target_pattern_index,
                                               out_type)) {
                return true;
            }
        }
        return false;
    case APK_Plex:
        if (!lsp_sema_type(sema, value_type, &value_t) ||
            value_t->kind != STK_Plex) {
            return false;
        }
        for (u32 i = 0; i < pattern->b; ++i) {
            u32 field_index = pattern->a + i;
            if (field_index >= array_count(ast->pattern_fields)) {
                break;
            }
            const AstPlexPatternField* field =
                &ast->pattern_fields[field_index];
            u32 field_type = sema_no_type();
            if (!lsp_record_field_type(
                    sema, value_type, field->symbol_handle, &field_type)) {
                continue;
            }
            if (lsp_pattern_expected_type_from(doc,
                                               field->pattern_index,
                                               field_type,
                                               target_pattern_index,
                                               out_type)) {
                return true;
            }
        }
        return false;
    case APK_EnumVariant:
        if (!lsp_sema_type(sema, value_type, &value_t) ||
            value_t->kind != STK_Enum) {
            return false;
        }
        {
            const AstEnumPattern* enum_pattern =
                &ast->enum_patterns[pattern->a];
            u32 variant = lsp_enum_variant_index(
                sema, value_type, enum_pattern->symbol_handle);
            u32 payload_type =
                lsp_enum_variant_payload_type(sema, value_type, variant);
            const SemaType* payload = NULL;
            for (u32 i = 0; i < enum_pattern->pattern_count; ++i) {
                u32 item_index = enum_pattern->first_pattern + i;
                if (item_index >= array_count(ast->pattern_items)) {
                    break;
                }
                u32 item_type = payload_type;
                if (lsp_sema_type(sema, payload_type, &payload) &&
                    payload->kind == STK_Tuple && i < payload->param_count) {
                    item_type =
                        sema->type_param_types[payload->first_param_type + i];
                }
                if (lsp_pattern_expected_type_from(
                        doc,
                        ast->pattern_items[item_index],
                        item_type,
                        target_pattern_index,
                        out_type)) {
                    return true;
                }
            }
        }
        return false;
    default:
        return false;
    }
}

internal bool lsp_pattern_expected_type(const LspDocument* doc,
                                        u32                target_pattern_index,
                                        u32*               out_type)
{
    const Ast* ast = &doc->front_end.ast;
    for (u32 node_index = 0; node_index < array_count(ast->nodes);
         ++node_index) {
        const AstNode* node = &ast->nodes[node_index];
        if (node->kind == AK_On && node->b < array_count(ast->ons)) {
            u32 scrutinee_type = sema_no_type();
            if (!lsp_sema_node_type(
                    &doc->front_end.sema, node->a, &scrutinee_type)) {
                continue;
            }
            const AstOnInfo* on = &ast->ons[node->b];
            for (u32 i = 0; i < on->branch_count; ++i) {
                u32 branch_index = on->first_branch + i;
                if (branch_index >= array_count(ast->on_branches)) {
                    break;
                }
                const AstOnBranch* branch = &ast->on_branches[branch_index];
                for (u32 j = 0; j < branch->pattern_count; ++j) {
                    u32 item_index = branch->pattern_index + j;
                    if (item_index >= array_count(ast->pattern_items)) {
                        break;
                    }
                    if (lsp_pattern_expected_type_from(
                            doc,
                            ast->pattern_items[item_index],
                            scrutinee_type,
                            target_pattern_index,
                            out_type)) {
                        return true;
                    }
                }
            }
        } else if ((node->kind == AK_DestructureBind ||
                    node->kind == AK_DestructureVariable ||
                    node->kind == AK_DestructureAssign) &&
                   node->b < array_count(ast->nodes)) {
            u32 value_type = sema_no_type();
            if (lsp_sema_node_type(
                    &doc->front_end.sema, node->b, &value_type) &&
                lsp_pattern_expected_type_from(
                    doc, node->a, value_type, target_pattern_index, out_type)) {
                return true;
            }
        }
    }

    return false;
}

internal string lsp_plex_pattern_field_hover_text(const LspDocument* doc,
                                                  Arena*             arena,
                                                  u32 pattern_index,
                                                  u32 field_index)
{
    if (field_index >= array_count(doc->front_end.ast.pattern_fields)) {
        return s("");
    }
    u32 owner_type = sema_no_type();
    if (!lsp_pattern_expected_type(doc, pattern_index, &owner_type)) {
        return s("");
    }
    return lsp_record_field_hover_text(
        doc,
        arena,
        owner_type,
        doc->front_end.ast.pattern_fields[field_index].symbol_handle,
        s("plex field"));
}

internal string lsp_enum_variant_hover_text_for_type(const LspDocument* doc,
                                                     Arena*             arena,
                                                     u32 enum_type,
                                                     u32 variant_symbol)
{
    u32 variant =
        lsp_enum_variant_index(&doc->front_end.sema, enum_type, variant_symbol);
    if (variant == U32_MAX) {
        return s("");
    }

    string name  = lex_symbol(&doc->front_end.lexer, variant_symbol);
    string owner = sema_type_name(
        &doc->front_end.lexer, &doc->front_end.sema, arena, enum_type);
    u32 payload_type =
        lsp_enum_variant_payload_type(&doc->front_end.sema, enum_type, variant);
    string payload =
        payload_type == sema_no_type()
            ? s("")
            : string_format(arena,
                            "\n- Payload: `" STRINGP "`",
                            STRINGV(sema_type_name(&doc->front_end.lexer,
                                                   &doc->front_end.sema,
                                                   arena,
                                                   payload_type)));

    return string_format(
        arena,
        STRINGP "\n\n- Kind: enum variant\n- Owner: `" STRINGP "`" STRINGP,
        STRINGV(lsp_markdown_code_block(
            arena, string_format(arena, STRINGP, STRINGV(name)))),
        STRINGV(owner),
        STRINGV(payload));
}

internal string lsp_enum_variant_hover_text(const LspDocument* doc,
                                            Arena*             arena,
                                            u32                token_index)
{
    const Ast* ast = &doc->front_end.ast;
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind == AK_EnumVariant && node->token_index == token_index) {
            u32 enum_type = sema_no_type();
            if (lsp_sema_node_type(&doc->front_end.sema, i, &enum_type)) {
                string hover = lsp_enum_variant_hover_text_for_type(
                    doc, arena, enum_type, node->a);
                if (hover.count != 0) {
                    return hover;
                }
            }
        }
    }

    for (u32 pattern_index = 0; pattern_index < array_count(ast->patterns);
         ++pattern_index) {
        const AstPattern* pattern = &ast->patterns[pattern_index];
        if (pattern->kind == APK_EnumVariant &&
            pattern->a < array_count(ast->enum_patterns)) {
            const AstEnumPattern* enum_pattern =
                &ast->enum_patterns[pattern->a];
            if (enum_pattern->token_index != token_index) {
                continue;
            }
            u32 enum_type = sema_no_type();
            if (lsp_pattern_expected_type(doc, pattern_index, &enum_type)) {
                return lsp_enum_variant_hover_text_for_type(
                    doc, arena, enum_type, enum_pattern->symbol_handle);
            }
        } else if (pattern->kind == APK_Value &&
                   pattern->a < array_count(ast->nodes)) {
            const AstNode* value = &ast->nodes[pattern->a];
            if (value->token_index != token_index ||
                (value->kind != AK_EnumVariant &&
                 value->kind != AK_SymbolRef)) {
                continue;
            }
            u32 enum_type = sema_no_type();
            if (lsp_pattern_expected_type(doc, pattern_index, &enum_type)) {
                return lsp_enum_variant_hover_text_for_type(
                    doc, arena, enum_type, value->a);
            }
        }
    }

    return s("");
}

internal string lsp_expression_hover_text(const LspDocument* doc,
                                          Arena*             arena,
                                          u32                node_index)
{
    if (node_index >= array_count(doc->front_end.ast.nodes)) {
        return s("");
    }

    const AstNode* node = &doc->front_end.ast.nodes[node_index];
    if (node->token_index >= array_count(doc->front_end.lexer.tokens)) {
        return s("");
    }

    u32 type_index = sema_no_type();
    if (!lsp_sema_node_type(&doc->front_end.sema, node_index, &type_index)) {
        return s("");
    }

    string type = sema_type_name(
        &doc->front_end.lexer, &doc->front_end.sema, arena, type_index);
    if (type.count == 0 || string_eq(type, s("<unknown>"))) {
        return s("");
    }

    const Token* token = &doc->front_end.lexer.tokens[node->token_index];
    usize        end   = lex_token_end_offset(&doc->front_end.lexer, token);
    string       text =
        string_from(doc->front_end.lexer.source.source.data + token->offset,
                    end - token->offset);

    return string_format(arena,
                         STRINGP "\n\n- Type: `" STRINGP "`",
                         STRINGV(lsp_markdown_code_block(arena, text)),
                         STRINGV(type));
}

internal void lsp_append_space_preserving_newlines(StringBuilder* sb,
                                                   string         source,
                                                   usize          start,
                                                   usize          end)
{
    for (usize i = start; i < end && i < source.count; ++i) {
        sb_append_char(sb, source.data[i] == '\n' ? '\n' : ' ');
    }
}

internal u32 lsp_source_test_closing_brace_index(const Lexer* lexer,
                                                 u32          open_index)
{
    u32 depth = 0;
    for (u32 index = open_index; index < array_count(lexer->tokens); ++index) {
        const Token* token = &lexer->tokens[index];
        if (token->kind == TK_LBrace) {
            depth += 1;
        } else if (token->kind == TK_RBrace) {
            if (depth == 0) {
                return U32_MAX;
            }
            depth -= 1;
            if (depth == 0) {
                return index;
            }
        }
    }
    return U32_MAX;
}

internal string lsp_source_test_hover_source(Arena* arena, const Lexer* lexer)
{
    StringBuilder sb = {0};
    sb_init(&sb, arena);

    string source = lexer->source.source;
    usize  cursor = 0;
    u32    depth  = 0;
    u32    index  = 0;

    while (index < array_count(lexer->tokens)) {
        const Token* token = &lexer->tokens[index];
        u32 symbol_handle  = token->kind == TK_Symbol
                                 ? lsp_symbol_handle_at_token(lexer, index)
                                 : U32_MAX;
        if (depth == 0 && token->kind == TK_Symbol &&
            symbol_handle != U32_MAX &&
            string_eq_cstr(lex_symbol(lexer, symbol_handle), "test")) {
            if (index + 2 < array_count(lexer->tokens) &&
                lexer->tokens[index + 1].kind == TK_String &&
                lexer->tokens[index + 2].kind == TK_LBrace) {
                sb_append_string(
                    &sb,
                    string_from(source.data + cursor, token->offset - cursor));
                sb_format(&sb, "__lsp_source_test_%u :: fn () ", index);
                cursor = lexer->tokens[index + 2].offset;
                index += 2;
                continue;
            }

            if (index + 1 < array_count(lexer->tokens) &&
                lexer->tokens[index + 1].kind == TK_LBrace) {
                u32 close_index =
                    lsp_source_test_closing_brace_index(lexer, index + 1);
                if (close_index != U32_MAX) {
                    const Token* open  = &lexer->tokens[index + 1];
                    const Token* close = &lexer->tokens[close_index];
                    sb_append_string(&sb,
                                     string_from(source.data + cursor,
                                                 token->offset - cursor));
                    lsp_append_space_preserving_newlines(
                        &sb,
                        source,
                        token->offset,
                        lex_token_end_offset(lexer, open));
                    sb_append_string(
                        &sb,
                        string_from(
                            source.data + lex_token_end_offset(lexer, open),
                            close->offset - lex_token_end_offset(lexer, open)));
                    lsp_append_space_preserving_newlines(
                        &sb,
                        source,
                        close->offset,
                        lex_token_end_offset(lexer, close));
                    cursor = lex_token_end_offset(lexer, close);
                    index  = close_index + 1;
                    continue;
                }
            }
        }

        if (token->kind == TK_LBrace) {
            depth += 1;
        } else if (token->kind == TK_RBrace && depth > 0) {
            depth -= 1;
        }
        index += 1;
    }

    sb_append_string(&sb,
                     string_from(source.data + cursor, source.count - cursor));
    return sb_to_string(&sb);
}

internal bool lsp_offset_is_in_source_test(const Lexer* lexer, usize offset)
{
    u32 depth = 0;
    for (u32 index = 0; index < array_count(lexer->tokens); ++index) {
        const Token* token = &lexer->tokens[index];
        u32 symbol_handle  = token->kind == TK_Symbol
                                 ? lsp_symbol_handle_at_token(lexer, index)
                                 : U32_MAX;
        if (depth == 0 && token->kind == TK_Symbol &&
            symbol_handle != U32_MAX &&
            string_eq_cstr(lex_symbol(lexer, symbol_handle), "test")) {
            u32 open_index = U32_MAX;
            if (index + 2 < array_count(lexer->tokens) &&
                lexer->tokens[index + 1].kind == TK_String &&
                lexer->tokens[index + 2].kind == TK_LBrace) {
                open_index = index + 2;
            } else if (index + 1 < array_count(lexer->tokens) &&
                       lexer->tokens[index + 1].kind == TK_LBrace) {
                open_index = index + 1;
            }
            if (open_index == U32_MAX) {
                index += 1;
                continue;
            }

            u32 close_index =
                lsp_source_test_closing_brace_index(lexer, open_index);
            if (close_index != U32_MAX) {
                usize start = lexer->tokens[open_index].offset;
                usize end =
                    lex_token_end_offset(lexer, &lexer->tokens[close_index]);
                if (offset >= start && offset <= end) {
                    return true;
                }
                index = close_index;
                continue;
            }
        }

        if (token->kind == TK_LBrace) {
            depth += 1;
        } else if (token->kind == TK_RBrace && depth > 0) {
            depth -= 1;
        }
    }

    return false;
}

internal string lsp_source_test_hover_text(const LspDocument* doc,
                                           Arena*             arena,
                                           usize              offset)
{
    bool in_source_test =
        lsp_offset_is_in_source_test(&doc->front_end.lexer, offset);
    if (!in_source_test) {
        return s("");
    }

    u32 line = 0;
    u32 col  = 0;
    if (!lex_offset_to_line_col(
            doc->front_end.lexer.source, offset, &line, &col)) {
        return s("");
    }

    Arena temp = {0};
    arena_init(&temp);

    string generated =
        lsp_source_test_hover_source(&temp, &doc->front_end.lexer);
    if (string_eq(generated, doc->front_end.lexer.source.source)) {
        arena_done(&temp);
        return s("");
    }

    ErrorRenderMode previous_mode = error_system_mode();
    bool            previous_emit = error_system_should_emit_output();
    error_system_set_mode(ERROR_RENDER_DIAGNOSTICS);
    error_system_set_emit_output(false);

    ProgramInfo     program = {0};
    FrontEndOptions options = {
        .verbose              = false,
        .release              = false,
        .require_entry_point  = false,
        .skip_hir_generation  = true,
        .keep_partial_results = true,
    };
    bool ok = front_end_program(
        (NerdSource){
            .source      = generated,
            .source_path = doc->front_end.lexer.source.source_path,
        },
        &options,
        NULL,
        &program);

    error_system_set_mode(previous_mode);
    error_system_set_emit_output(previous_emit);

    string hover = s("");
    if (ok || array_count(program.modules) > 0) {
        for (u32 i = 0; i < array_count(program.modules); ++i) {
            program.modules[i].front_end.sema.program = &program;
        }

        LspModuleView module = {0};
        if (lsp_program_module_view(
                &program, program.root_module_index, &module)) {
            usize generated_offset = 0;
            if (lex_line_col_to_offset(
                    module.lexer->source, line, col, &generated_offset)) {
                u32    token_end = 0;
                Token* token     = lex_find(
                    (Lexer*)module.lexer, generated_offset, &token_end);
                if (token != NULL) {
                    u32 token_index =
                        lsp_token_index_from_pointer(module.lexer, token);
                    LspDocument generated_doc = {
                        .source    = generated,
                        .program   = program,
                        .front_end = program.modules[program.root_module_index]
                                         .front_end,
                    };

                    if (token->kind == TK_Symbol) {
                        u32 local_index = lsp_find_local_index_for_token(
                            &generated_doc, token_index);
                        if (local_index != sema_no_local()) {
                            hover = lsp_local_hover_text(
                                &generated_doc, arena, local_index);
                        }
                        if (hover.count == 0) {
                            u32 ref_node_index =
                                lsp_find_symbol_ref_node_at_token(
                                    &generated_doc.front_end.ast, token_index);
                            if (ref_node_index != U32_MAX) {
                                hover = lsp_expression_hover_text(
                                    &generated_doc, arena, ref_node_index);
                            }
                        }
                    } else if (token->kind == TK_Integer) {
                        u32 field_node_index = lsp_find_field_node_at_token(
                            &generated_doc.front_end.ast, token_index);
                        if (field_node_index != U32_MAX) {
                            hover = lsp_field_hover_text(
                                &generated_doc, arena, field_node_index);
                        }
                    }
                }
            }
        }
    }

    program_info_done(&program);
    arena_done(&temp);
    return hover;
}

internal bool
lsp_block_contains_node(const Ast* ast, u32 block_node_index, u32 node_index)
{
    if (block_node_index >= array_count(ast->nodes)) {
        return false;
    }
    const AstNode* block = &ast->nodes[block_node_index];
    return block->kind == AK_Block && node_index >= block->a &&
           node_index < block->b;
}

internal usize lsp_ast_type_node_end_offset(const LspDocument* doc,
                                            u32                type_node_index);

internal string lsp_member_bind_hover_text(const LspDocument* doc,
                                           Arena*             arena,
                                           u32                token_index)
{
    const Ast* ast             = &doc->front_end.ast;
    u32        bind_node_index = lsp_find_bind_node_at_token(ast, token_index);
    if (bind_node_index == U32_MAX ||
        bind_node_index >= array_count(ast->nodes)) {
        return s("");
    }

    const AstNode* bind = &ast->nodes[bind_node_index];
    if (bind->b >= array_count(ast->nodes)) {
        return s("");
    }

    string kind = s("");
    for (u32 i = 0; i < array_count(ast->trait_infos); ++i) {
        if (lsp_block_contains_node(
                ast, ast->trait_infos[i].body_node_index, bind_node_index)) {
            kind = s("trait member");
            break;
        }
    }
    if (kind.count == 0) {
        for (u32 i = 0; i < array_count(ast->impls); ++i) {
            if (lsp_block_contains_node(
                    ast, ast->impls[i].body_node_index, bind_node_index)) {
                kind = s("impl method");
                break;
            }
        }
    }
    if (kind.count == 0) {
        return s("");
    }

    const AstNode* value = &ast->nodes[bind->b];
    if (value->kind != AK_TypeFn && value->kind != AK_FnDef) {
        return s("");
    }

    const Lexer* lexer = &doc->front_end.lexer;
    if (bind->token_index >= array_count(lexer->tokens)) {
        return s("");
    }
    usize start = lexer->tokens[bind->token_index].offset;
    usize end   = 0;
    if (value->kind == AK_TypeFn) {
        end = lsp_ast_type_node_end_offset(doc, bind->b);
    } else {
        for (u32 i = bind->token_index; i < array_count(lexer->tokens); ++i) {
            TokenKind token_kind = lexer->tokens[i].kind;
            if (token_kind == TK_LBrace || token_kind == TK_FatArrow) {
                end = lexer->tokens[i].offset;
                break;
            }
        }
    }
    if (end <= start || end > lexer->source.source.count) {
        return s("");
    }

    string signature = lsp_trim_source(doc, start, end);
    return string_format(arena,
                         STRINGP "\n\n- Kind: " STRINGP,
                         STRINGV(lsp_markdown_code_block(arena, signature)),
                         STRINGV(kind));
}

//------------------------------------------------------------------------------
// Return hover text for language-known built-in types.

internal string lsp_builtin_type_hover_text(Arena* arena, string name)
{
    if (!string_eq(name, s("arena"))) {
        return s("");
    }

    return string_format(arena,
                         STRINGP
                         "\n\n- Kind: built-in type"
                         "\n- Type: `arena`"
                         "\n- Notes: opaque, pointer-stable allocation arena",
                         STRINGV(lsp_markdown_code_block(arena, s("arena"))));
}

//------------------------------------------------------------------------------
// AST-only field lookup helpers used when semantic analysis stopped before it
// attached type information to a field receiver.

internal u32 lsp_ast_type_symbol_with_self(const Lexer* lexer,
                                           const Ast*   ast,
                                           u32          type_node_index,
                                           u32          self_symbol)
{
    if (type_node_index >= array_count(ast->nodes)) {
        return U32_MAX;
    }

    const AstNode* type_node = &ast->nodes[type_node_index];
    if (type_node->kind == AK_Expression || type_node->kind == AK_Statement) {
        return lsp_ast_type_symbol_with_self(
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
        return lsp_ast_type_symbol_with_self(
            lexer, ast, type_node->a, self_symbol);
    }
    return U32_MAX;
}

internal u32 lsp_ast_type_symbol(const Lexer* lexer,
                                 const Ast*   ast,
                                 u32          type_node_index)
{
    return lsp_ast_type_symbol_with_self(lexer, ast, type_node_index, U32_MAX);
}

internal u32 lsp_ast_impl_self_symbol_at_token(const LspDocument* doc,
                                               u32                token_index)
{
    const Lexer* lexer = &doc->front_end.lexer;
    const Ast*   ast   = &doc->front_end.ast;
    if (token_index >= array_count(lexer->tokens)) {
        return U32_MAX;
    }

    usize offset = lexer->tokens[token_index].offset;
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

        return lsp_ast_type_symbol(lexer, ast, impl->target_type_node_index);
    }

    return U32_MAX;
}

internal u32 lsp_ast_receiver_type_symbol(const LspDocument* doc,
                                          u32 receiver_node_index,
                                          u32 field_token_index)
{
    const Lexer* lexer = &doc->front_end.lexer;
    const Ast*   ast   = &doc->front_end.ast;
    if (receiver_node_index >= array_count(ast->nodes)) {
        return U32_MAX;
    }

    const AstNode* receiver_node = &ast->nodes[receiver_node_index];
    if (receiver_node->kind != AK_SymbolRef || receiver_node->a == U32_MAX) {
        return U32_MAX;
    }

    u32   receiver_symbol = receiver_node->a;
    usize field_offset    = field_token_index < array_count(lexer->tokens)
                                ? lexer->tokens[field_token_index].offset
                                : lexer->source.source.count;

    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if ((node->kind != AK_Variable && node->kind != AK_Bind) ||
            node->a != receiver_symbol || node->b >= array_count(ast->nodes)) {
            continue;
        }

        const AstNode* value = &ast->nodes[node->b];
        if (value->kind == AK_AnnotatedValue || value->kind == AK_ZeroInit ||
            value->kind == AK_Undefined) {
            return lsp_ast_type_symbol(lexer, ast, value->a);
        }
    }

    u32   best_type_symbol = U32_MAX;
    usize best_offset      = 0;
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
            if (param->symbol_handle != receiver_symbol ||
                param->token_index >= array_count(lexer->tokens)) {
                continue;
            }
            usize param_offset = lexer->tokens[param->token_index].offset;
            if (param_offset > field_offset || param_offset < best_offset) {
                continue;
            }

            u32 self_symbol =
                lsp_ast_impl_self_symbol_at_token(doc, field_token_index);
            u32 type_symbol = lsp_ast_type_symbol_with_self(
                lexer, ast, param->type_node_index, self_symbol);
            if (type_symbol != U32_MAX) {
                best_type_symbol = type_symbol;
                best_offset      = param_offset;
            }
        }
    }

    return best_type_symbol;
}

internal const AstPlexField* lsp_ast_find_field_for_type_symbol(
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
        if (value->kind != AK_TypePlex) {
            continue;
        }
        if (value->a >= array_count(ast->plex_types)) {
            return NULL;
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
                return field;
            }
        }
    }
    return NULL;
}

internal usize lsp_ast_type_node_end_offset(const LspDocument* doc,
                                            u32                type_node_index)
{
    const Lexer* lexer = &doc->front_end.lexer;
    const Ast*   ast   = &doc->front_end.ast;
    if (type_node_index >= array_count(ast->nodes)) {
        return 0;
    }

    const AstNode* node = &ast->nodes[type_node_index];
    if (node->token_index >= array_count(lexer->tokens)) {
        return 0;
    }

    switch (node->kind) {
    case AK_Expression:
    case AK_Statement:
    case AK_TypePointer:
    case AK_TypeSlice:
        {
            usize end = lsp_ast_type_node_end_offset(doc, node->a);
            return end != 0 ? end
                            : lex_token_end_offset(
                                  lexer, &lexer->tokens[node->token_index]);
        }
    case AK_TypeArray:
    case AK_TypeDynamicArray:
        {
            usize end = lsp_ast_type_node_end_offset(doc, node->b);
            return end != 0 ? end
                            : lex_token_end_offset(
                                  lexer, &lexer->tokens[node->token_index]);
        }
    case AK_TypeTuple:
        {
            if (node->b != 0) {
                u32 last_item = node->a + node->b - 1;
                if (last_item < array_count(ast->tuple_items)) {
                    usize end = lsp_ast_type_node_end_offset(
                        doc, ast->tuple_items[last_item]);
                    if (end != 0) {
                        return end;
                    }
                }
            }
            return lex_token_end_offset(lexer,
                                        &lexer->tokens[node->token_index]);
        }
    case AK_TypeApply:
        {
            if (node->a < array_count(ast->type_applications)) {
                const AstTypeApplyInfo* apply =
                    &ast->type_applications[node->a];
                if (apply->arg_count != 0) {
                    u32 last_arg = apply->first_arg + apply->arg_count - 1;
                    if (last_arg < array_count(ast->tuple_items)) {
                        usize end = lsp_ast_type_node_end_offset(
                            doc, ast->tuple_items[last_arg]);
                        if (end != 0) {
                            return end;
                        }
                    }
                }
                usize end =
                    lsp_ast_type_node_end_offset(doc, apply->target_node_index);
                if (end != 0) {
                    return end;
                }
            }
            return lex_token_end_offset(lexer,
                                        &lexer->tokens[node->token_index]);
        }
    case AK_TypeFn:
        {
            if (node->a < array_count(ast->fn_signatures)) {
                const AstFnSignature* signature = &ast->fn_signatures[node->a];
                if (signature->return_type_node_index != U32_MAX) {
                    usize end = lsp_ast_type_node_end_offset(
                        doc, signature->return_type_node_index);
                    if (end != 0) {
                        return end;
                    }
                }
                if (signature->param_count != 0) {
                    u32 last_param =
                        signature->first_param + signature->param_count - 1;
                    if (last_param < array_count(ast->params)) {
                        usize end = lsp_ast_type_node_end_offset(
                            doc, ast->params[last_param].type_node_index);
                        if (end != 0) {
                            return end;
                        }
                    }
                }
            }
            return lex_token_end_offset(lexer,
                                        &lexer->tokens[node->token_index]);
        }
    default:
        return lex_token_end_offset(lexer, &lexer->tokens[node->token_index]);
    }
}

internal string lsp_ast_type_node_source(const LspDocument* doc,
                                         u32                type_node_index)
{
    if (type_node_index >= array_count(doc->front_end.ast.nodes)) {
        return s("<unknown>");
    }
    const AstNode* node = &doc->front_end.ast.nodes[type_node_index];
    if (node->token_index >= array_count(doc->front_end.lexer.tokens)) {
        return s("<unknown>");
    }

    usize start = doc->front_end.lexer.tokens[node->token_index].offset;
    usize end   = lsp_ast_type_node_end_offset(doc, type_node_index);
    if (end <= start || end > doc->front_end.lexer.source.source.count) {
        return s("<unknown>");
    }
    return lsp_trim_source(doc, start, end);
}

internal string lsp_ast_field_hover_text(const LspDocument* doc,
                                         Arena*             arena,
                                         u32                field_node_index)
{
    if (field_node_index >= array_count(doc->front_end.ast.nodes)) {
        return s("");
    }

    const AstNode* field = &doc->front_end.ast.nodes[field_node_index];
    if (field->kind != AK_Field) {
        return s("");
    }

    u32 type_symbol =
        lsp_ast_receiver_type_symbol(doc, field->a, field->token_index);
    if (type_symbol == U32_MAX) {
        return s("");
    }

    const AstPlexField* plex_field =
        lsp_ast_find_field_for_type_symbol(doc, type_symbol, field->b);
    if (plex_field == NULL) {
        return s("");
    }

    string type       = s("<unknown>");
    u32    type_index = sema_no_type();
    if (lsp_sema_node_type(
            &doc->front_end.sema, plex_field->type_node_index, &type_index)) {
        type = sema_type_name(
            &doc->front_end.lexer, &doc->front_end.sema, arena, type_index);
    }
    if (string_eq(type, s("<unknown>"))) {
        type = lsp_ast_type_node_source(doc, plex_field->type_node_index);
    }

    string name  = lex_symbol(&doc->front_end.lexer, field->b);
    string owner = lex_symbol(&doc->front_end.lexer, type_symbol);
    return string_format(
        arena,
        STRINGP "\n\n- Kind: plex field\n- Type: `" STRINGP "`"
                "\n- Owner: `" STRINGP "`",
        STRINGV(lsp_markdown_code_block(
            arena, string_format(arena, STRINGP, STRINGV(name)))),
        STRINGV(type),
        STRINGV(owner));
}

internal bool lsp_field_receiver_is_arena(const LspDocument* doc,
                                          u32                field_node_index)
{
    if (field_node_index >= array_count(doc->front_end.ast.nodes)) {
        return false;
    }

    const AstNode* field = &doc->front_end.ast.nodes[field_node_index];
    if (field->kind != AK_Field) {
        return false;
    }

    u32 target_type = sema_no_type();
    if (!lsp_sema_node_type(&doc->front_end.sema, field->a, &target_type)) {
        return false;
    }

    const SemaType* target = NULL;
    if (!lsp_sema_type(&doc->front_end.sema, target_type, &target)) {
        return false;
    }
    if (target->kind == STK_Arena) {
        return true;
    }
    if (target->kind != STK_Pointer) {
        return false;
    }

    const SemaType* pointee = NULL;
    return lsp_sema_type(
               &doc->front_end.sema, target->first_param_type, &pointee) &&
           pointee->kind == STK_Arena;
}

//------------------------------------------------------------------------------
// Return a hover summary for one plex/union field access.

internal string lsp_field_hover_text(const LspDocument* doc,
                                     Arena*             arena,
                                     u32                field_node_index)
{
    if (field_node_index >= array_count(doc->front_end.ast.nodes)) {
        return s("");
    }

    const AstNode* field = &doc->front_end.ast.nodes[field_node_index];
    if (field->kind == AK_TupleField) {
        u32 target_type = sema_no_type();
        if (!lsp_sema_node_type(&doc->front_end.sema, field->a, &target_type)) {
            return s("");
        }

        const SemaType* target = NULL;
        if (!lsp_sema_type(&doc->front_end.sema, target_type, &target) ||
            target->kind != STK_Tuple || field->b >= target->param_count) {
            return s("");
        }

        u32 field_type =
            doc->front_end.sema
                .type_param_types[target->first_param_type + field->b];
        string type = sema_type_name(
            &doc->front_end.lexer, &doc->front_end.sema, arena, field_type);
        string owner = sema_type_name(
            &doc->front_end.lexer, &doc->front_end.sema, arena, target_type);

        const Token* token = &doc->front_end.lexer.tokens[field->token_index];
        usize        end   = lex_token_end_offset(&doc->front_end.lexer, token);
        string       name =
            string_from(doc->front_end.lexer.source.source.data + token->offset,
                        end - token->offset);

        return string_format(
            arena,
            STRINGP "\n\n- Kind: tuple field\n- Type: `" STRINGP "`"
                    "\n- Owner: `" STRINGP "`",
            STRINGV(lsp_markdown_code_block(
                arena, string_format(arena, STRINGP, STRINGV(name)))),
            STRINGV(type),
            STRINGV(owner));
    }

    if (field->kind != AK_Field) {
        return lsp_ast_field_hover_text(doc, arena, field_node_index);
    }

    if (!lsp_field_receiver_is_arena(doc, field_node_index)) {
        u32 method_decl =
            lsp_selected_method_decl_for_field(doc, field_node_index);
        if (method_decl != LSP_NO_DECL) {
            return lsp_method_hover_text(doc, arena, method_decl);
        }
    }

    u32 target_type = sema_no_type();
    if (!lsp_sema_node_type(&doc->front_end.sema, field->a, &target_type)) {
        return lsp_ast_field_hover_text(doc, arena, field_node_index);
    }

    const SemaType* target = NULL;
    if (!lsp_sema_type(&doc->front_end.sema, target_type, &target)) {
        return lsp_ast_field_hover_text(doc, arena, field_node_index);
    }
    if (target->kind == STK_Pointer) {
        u32             pointee_type = target->first_param_type;
        const SemaType* pointee      = NULL;
        if (lsp_sema_type(&doc->front_end.sema, pointee_type, &pointee) &&
            (pointee->kind == STK_Plex || pointee->kind == STK_Union ||
             pointee->kind == STK_Box || pointee->kind == STK_DynamicArray ||
             pointee->kind == STK_Array || pointee->kind == STK_Slice ||
             pointee->kind == STK_String || pointee->kind == STK_Arena)) {
            target_type = pointee_type;
            target      = pointee;
        }
    }
    if (target->kind == STK_Box) {
        string name = lex_symbol(&doc->front_end.lexer, field->b);
        if (string_eq(name, s("free"))) {
            string owner = sema_type_name(&doc->front_end.lexer,
                                          &doc->front_end.sema,
                                          arena,
                                          target_type);
            return string_format(
                arena,
                "free\n\n- Kind: box method\n- Type: `fn () -> "
                "void`\n- Owner: `" STRINGP "`",
                STRINGV(owner));
        }
        u32             item_type = target->first_param_type;
        const SemaType* item      = NULL;
        if (lsp_sema_type(&doc->front_end.sema, item_type, &item)) {
            target_type = item_type;
            target      = item;
        }
    }

    if (target->kind == STK_Array || target->kind == STK_Slice ||
        target->kind == STK_String || target->kind == STK_DynamicArray) {
        string name       = lex_symbol(&doc->front_end.lexer, field->b);
        u32    field_type = sema_no_type();
        string type       = lsp_sema_node_type(
                                &doc->front_end.sema, field_node_index, &field_type)
                                ? sema_type_name(&doc->front_end.lexer,
                                                 &doc->front_end.sema,
                                                 arena,
                                                 field_type)
                                : s("<unknown>");
        string owner      = sema_type_name(
            &doc->front_end.lexer, &doc->front_end.sema, arena, target_type);
        string kind       = s("");
        bool   recognised = false;

        if (target->kind == STK_Array) {
            if (string_eq(name, s("count")) || string_eq(name, s("bytes"))) {
                kind       = s("array field");
                recognised = true;
            }
        } else if (target->kind == STK_String || target->kind == STK_Slice) {
            if (string_eq(name, s("data"))) {
                kind       = target->kind == STK_String ? s("string field")
                                                        : s("slice field");
                recognised = true;
            } else if (string_eq(name, s("count")) ||
                       string_eq(name, s("bytes"))) {
                kind       = target->kind == STK_String ? s("string field")
                                                        : s("slice field");
                recognised = true;
            }
        } else if (target->kind == STK_DynamicArray) {
            if (string_eq(name, s("data")) || string_eq(name, s("count")) ||
                string_eq(name, s("capacity"))) {
                kind       = s("dynamic array field");
                recognised = true;
            } else if (string_eq(name, s("push")) ||
                       string_eq(name, s("append")) ||
                       string_eq(name, s("reserve_to")) ||
                       string_eq(name, s("reserve_extra")) ||
                       string_eq(name, s("resize_to")) ||
                       string_eq(name, s("resize_undefined_to")) ||
                       string_eq(name, s("extend")) ||
                       string_eq(name, s("extend_undefined")) ||
                       string_eq(name, s("delete")) ||
                       string_eq(name, s("swap_delete")) ||
                       string_eq(name, s("pop")) ||
                       string_eq(name, s("clear")) ||
                       string_eq(name, s("free"))) {
                kind       = s("dynamic array method");
                recognised = true;
            }
        }

        if (recognised) {
            return string_format(
                arena,
                STRINGP "\n\n- Kind: " STRINGP "\n- Type: `" STRINGP "`"
                        "\n- Owner: `" STRINGP "`",
                STRINGV(lsp_markdown_code_block(
                    arena, string_format(arena, STRINGP, STRINGV(name)))),
                STRINGV(kind),
                STRINGV(type),
                STRINGV(owner));
        }
    }

    if (target->kind == STK_Arena) {
        string name      = lex_symbol(&doc->front_end.lexer, field->b);
        string signature = s("");
        if (string_eq(name, s("alloc"))) {
            signature = s("alloc :: fn [T] (self: ^arena) -> ^T");
        } else if (string_eq(name, s("alloc_array"))) {
            signature =
                s("alloc_array :: fn [T] (self: ^arena, count: usize) -> []T");
        } else if (string_eq(name, s("alloc_bytes"))) {
            signature =
                s("alloc_bytes :: fn (self: ^arena, count: usize) -> []u8");
        } else if (string_eq(name, s("reset"))) {
            signature = s("reset :: fn (self: ^arena) -> void");
        } else if (string_eq(name, s("mark"))) {
            signature = s("mark :: fn (self: ^arena) -> u32");
        } else if (string_eq(name, s("restore"))) {
            signature = s("restore :: fn (self: ^arena, mark: u32) -> void");
        } else if (string_eq(name, s("done"))) {
            signature = s("done :: fn (self: ^arena) -> void");
        }

        if (signature.count != 0) {
            return string_format(
                arena,
                STRINGP "\n\n- Kind: arena method\n- Owner: `arena`",
                STRINGV(lsp_markdown_code_block(arena, signature)));
        }
    }

    if (target->kind != STK_Plex && target->kind != STK_Union) {
        return s("");
    }

    for (u32 i = 0; i < target->param_count; ++i) {
        if (doc->front_end.sema
                .type_param_symbols[target->first_param_type + i] != field->b) {
            continue;
        }

        string name = lex_symbol(&doc->front_end.lexer, field->b);
        string type = sema_type_name(
            &doc->front_end.lexer,
            &doc->front_end.sema,
            arena,
            doc->front_end.sema.type_param_types[target->first_param_type + i]);
        string owner = sema_type_name(
            &doc->front_end.lexer, &doc->front_end.sema, arena, target_type);
        string kind =
            target->kind == STK_Union ? s("union field") : s("plex field");

        return string_format(
            arena,
            STRINGP "\n\n- Kind: " STRINGP "\n- Type: `" STRINGP "`"
                    "\n- Owner: `" STRINGP "`",
            STRINGV(lsp_markdown_code_block(
                arena, string_format(arena, STRINGP, STRINGV(name)))),
            STRINGV(kind),
            STRINGV(type),
            STRINGV(owner));
    }

    return s("");
}

//------------------------------------------------------------------------------
// Build a location object for one top-level declaration binding.

internal JsonValue* lsp_decl_location(const LspDocument* doc,
                                      Arena*             arena,
                                      string             uri,
                                      u32                decl_index)
{
    const SemaDecl* decl = NULL;
    if (!lsp_sema_decl(&doc->front_end.sema, decl_index, &decl)) {
        return NULL;
    }
    if (decl->import_module_index != sema_no_decl() &&
        decl->import_decl_index != sema_no_decl()) {
        LspModuleView module = {0};
        if (lsp_program_module_view(
                &doc->program, decl->import_module_index, &module)) {
            return lsp_module_decl_location(
                module, arena, decl->import_decl_index);
        }
    }

    if (decl->bind_node_index == LSP_NO_DECL) {
        return NULL;
    }
    const AstNode* bind = &doc->front_end.ast.nodes[decl->bind_node_index];
    u32            token_index = bind->token_index;
    if (bind->kind == AK_FfiDef &&
        bind->a < array_count(doc->front_end.ast.ffi_infos)) {
        token_index = doc->front_end.ast.ffi_infos[bind->a].symbol_token_index;
    }
    usize start_offset;
    usize end_offset;
    lsp_token_offsets(
        &doc->front_end.lexer, token_index, &start_offset, &end_offset);

    JsonValue* location = json_new_object(arena);
    json_object_set_string(location, arena, "uri", uri);
    json_object_set_object(
        location,
        "range",
        lsp_make_document_range(doc, arena, start_offset, end_offset));
    return location;
}

internal JsonValue* lsp_local_location(const LspDocument* doc,
                                       Arena*             arena,
                                       string             uri,
                                       u32                local_index)
{
    const SemaLocal* local = NULL;
    if (!lsp_sema_local(&doc->front_end.sema, local_index, &local)) {
        return NULL;
    }
    usize start_offset;
    usize end_offset;
    if (local->decl_token_index != U32_MAX) {
        lsp_token_offsets(&doc->front_end.lexer,
                          local->decl_token_index,
                          &start_offset,
                          &end_offset);
    } else {
        const AstNode* bind = &doc->front_end.ast.nodes[local->decl_node_index];
        lsp_token_offsets(&doc->front_end.lexer,
                          bind->token_index,
                          &start_offset,
                          &end_offset);
    }

    JsonValue* location = json_new_object(arena);
    json_object_set_string(location, arena, "uri", uri);
    json_object_set_object(
        location,
        "range",
        lsp_make_document_range(doc, arena, start_offset, end_offset));
    return location;
}

//------------------------------------------------------------------------------
// Build a location object for one field inside a local plex/union type node.

internal JsonValue* lsp_local_record_field_location(const LspDocument* doc,
                                                    Arena*             arena,
                                                    string             uri,
                                                    u32 type_index,
                                                    u32 field_symbol)
{
    const SemaType* target_type = NULL;
    if (!lsp_sema_type(&doc->front_end.sema, type_index, &target_type)) {
        return NULL;
    }

    for (u32 i = 0; i < array_count(doc->front_end.ast.nodes); ++i) {
        const AstNode* node = &doc->front_end.ast.nodes[i];
        if (node->kind != AK_TypePlex) {
            continue;
        }

        u32 candidate_type_index = sema_no_type();
        if (!lsp_sema_node_type(
                &doc->front_end.sema, i, &candidate_type_index)) {
            continue;
        }

        const SemaType* candidate_type = NULL;
        if (!lsp_sema_type(
                &doc->front_end.sema, candidate_type_index, &candidate_type)) {
            continue;
        }
        if (candidate_type->kind != target_type->kind ||
            candidate_type->param_count != target_type->param_count) {
            continue;
        }

        bool matches = true;
        for (u32 j = 0; j < target_type->param_count; ++j) {
            if (doc->front_end.sema
                        .type_param_symbols[candidate_type->first_param_type +
                                            j] !=
                    doc->front_end.sema
                        .type_param_symbols[target_type->first_param_type +
                                            j] ||
                doc->front_end.sema
                        .type_param_types[candidate_type->first_param_type +
                                          j] !=
                    doc->front_end.sema
                        .type_param_types[target_type->first_param_type + j]) {
                matches = false;
                break;
            }
        }
        if (!matches) {
            continue;
        }

        const AstPlexTypeInfo* plex = &doc->front_end.ast.plex_types[node->a];
        for (u32 j = 0; j < plex->field_count; ++j) {
            const AstPlexField* field =
                &doc->front_end.ast.plex_fields[plex->first_field + j];
            if (field->symbol_handle != field_symbol) {
                continue;
            }

            usize start_offset;
            usize end_offset;
            lsp_token_offsets(&doc->front_end.lexer,
                              field->token_index,
                              &start_offset,
                              &end_offset);

            JsonValue* location = json_new_object(arena);
            json_object_set_string(location, arena, "uri", uri);
            json_object_set_object(
                location,
                "range",
                lsp_make_document_range(doc, arena, start_offset, end_offset));
            return location;
        }
    }

    return NULL;
}

//------------------------------------------------------------------------------
// Build a location object for one field referenced from a local type node.

internal JsonValue* lsp_field_location_from_type_node(const LspDocument* doc,
                                                      Arena*             arena,
                                                      string             uri,
                                                      u32 type_node_index,
                                                      u32 field_symbol)
{
    if (type_node_index >= array_count(doc->front_end.ast.nodes)) {
        return NULL;
    }

    const AstNode* type_node = &doc->front_end.ast.nodes[type_node_index];
    if (type_node->kind == AK_TypePlex) {
        const AstPlexTypeInfo* plex =
            &doc->front_end.ast.plex_types[type_node->a];
        for (u32 i = 0; i < plex->field_count; ++i) {
            const AstPlexField* field =
                &doc->front_end.ast.plex_fields[plex->first_field + i];
            if (field->symbol_handle != field_symbol) {
                continue;
            }

            usize start_offset;
            usize end_offset;
            lsp_token_offsets(&doc->front_end.lexer,
                              field->token_index,
                              &start_offset,
                              &end_offset);

            JsonValue* location = json_new_object(arena);
            json_object_set_string(location, arena, "uri", uri);
            json_object_set_object(
                location,
                "range",
                lsp_make_document_range(doc, arena, start_offset, end_offset));
            return location;
        }
        return NULL;
    }

    if (type_node->kind == AK_SymbolRef) {
        u32             decl_index = sema_no_decl();
        const SemaDecl* decl       = NULL;
        if (lsp_sema_node_decl(
                &doc->front_end.sema, type_node_index, &decl_index) &&
            lsp_sema_decl(&doc->front_end.sema, decl_index, &decl) &&
            decl->kind == SK_TypeAlias &&
            decl->bind_node_index < array_count(doc->front_end.ast.nodes)) {
            const AstNode* bind =
                &doc->front_end.ast.nodes[decl->bind_node_index];
            return lsp_field_location_from_type_node(
                doc, arena, uri, bind->b, field_symbol);
        }
    }

    return NULL;
}

//------------------------------------------------------------------------------
// Return a field location when the current file has exactly one matching
// plex/union field symbol.

internal JsonValue* lsp_unique_record_field_location(const LspDocument* doc,
                                                     Arena*             arena,
                                                     string             uri,
                                                     u32 field_symbol)
{
    u32   matched_token_index = U32_MAX;
    usize match_count         = 0;

    for (u32 i = 0; i < array_count(doc->front_end.ast.plex_fields); ++i) {
        const AstPlexField* field = &doc->front_end.ast.plex_fields[i];
        if (field->symbol_handle != field_symbol) {
            continue;
        }
        matched_token_index = field->token_index;
        match_count += 1;
        if (match_count > 1) {
            return NULL;
        }
    }

    if (match_count != 1) {
        return NULL;
    }

    usize start_offset;
    usize end_offset;
    lsp_token_offsets(
        &doc->front_end.lexer, matched_token_index, &start_offset, &end_offset);

    JsonValue* location = json_new_object(arena);
    json_object_set_string(location, arena, "uri", uri);
    json_object_set_object(
        location,
        "range",
        lsp_make_document_range(doc, arena, start_offset, end_offset));
    return location;
}

internal JsonValue* lsp_ast_field_location(const LspDocument* doc,
                                           Arena*             arena,
                                           string             uri,
                                           u32                field_node_index)
{
    if (field_node_index >= array_count(doc->front_end.ast.nodes)) {
        return NULL;
    }

    const AstNode* field = &doc->front_end.ast.nodes[field_node_index];
    if (field->kind != AK_Field) {
        return NULL;
    }

    u32 type_symbol =
        lsp_ast_receiver_type_symbol(doc, field->a, field->token_index);
    if (type_symbol == U32_MAX) {
        return NULL;
    }

    const AstPlexField* plex_field =
        lsp_ast_find_field_for_type_symbol(doc, type_symbol, field->b);
    if (plex_field == NULL) {
        return NULL;
    }

    usize start_offset;
    usize end_offset;
    lsp_token_offsets(&doc->front_end.lexer,
                      plex_field->token_index,
                      &start_offset,
                      &end_offset);

    JsonValue* location = json_new_object(arena);
    json_object_set_string(location, arena, "uri", uri);
    json_object_set_object(
        location,
        "range",
        lsp_make_document_range(doc, arena, start_offset, end_offset));
    return location;
}

//------------------------------------------------------------------------------
// Build a location object for one plex/union field-access token.

internal JsonValue* lsp_field_location(const LspDocument* doc,
                                       Arena*             arena,
                                       string             uri,
                                       u32                field_node_index)
{
    if (field_node_index >= array_count(doc->front_end.ast.nodes)) {
        return NULL;
    }

    const AstNode* field = &doc->front_end.ast.nodes[field_node_index];
    if (field->kind != AK_Field) {
        return lsp_ast_field_location(doc, arena, uri, field_node_index);
    }

    u32 target_type = sema_no_type();
    if (!lsp_sema_node_type(&doc->front_end.sema, field->a, &target_type)) {
        return lsp_ast_field_location(doc, arena, uri, field_node_index);
    }

    const SemaType* target = NULL;
    if (!lsp_sema_type(&doc->front_end.sema, target_type, &target)) {
        return lsp_ast_field_location(doc, arena, uri, field_node_index);
    }
    if (target->kind == STK_Pointer) {
        u32             pointee_type = target->first_param_type;
        const SemaType* pointee      = NULL;
        if (lsp_sema_type(&doc->front_end.sema, pointee_type, &pointee) &&
            (pointee->kind == STK_Plex || pointee->kind == STK_Union)) {
            target_type = pointee_type;
            target      = pointee;
        }
    }

    if (target->kind != STK_Plex && target->kind != STK_Union) {
        return NULL;
    }

    if (field->a < array_count(doc->front_end.ast.nodes)) {
        u32              local_index = sema_no_local();
        const SemaLocal* local       = NULL;
        if (lsp_sema_node_local(&doc->front_end.sema, field->a, &local_index) &&
            lsp_sema_local(&doc->front_end.sema, local_index, &local)) {
            if (local->decl_node_index <
                array_count(doc->front_end.ast.nodes)) {
                const AstNode* bind =
                    &doc->front_end.ast.nodes[local->decl_node_index];
                u32 value_node_index = bind->b;
                if (value_node_index < array_count(doc->front_end.ast.nodes)) {
                    const AstNode* value =
                        &doc->front_end.ast.nodes[value_node_index];
                    if (value->kind == AK_AnnotatedValue ||
                        value->kind == AK_ZeroInit ||
                        value->kind == AK_Undefined) {
                        JsonValue* location = lsp_field_location_from_type_node(
                            doc, arena, uri, value->a, field->b);
                        if (location != NULL) {
                            return location;
                        }
                    } else if (value->kind == AK_Plex ||
                               value->kind == AK_PlexUpdate) {
                        const AstPlexLiteralInfo* literal =
                            &doc->front_end.ast.plex_literals[value->a];
                        if (literal->target_node_index != U32_MAX) {
                            JsonValue* location =
                                lsp_field_location_from_type_node(
                                    doc,
                                    arena,
                                    uri,
                                    literal->target_node_index,
                                    field->b);
                            if (location != NULL) {
                                return location;
                            }
                        }
                    }
                }
            }
        }
    }

    JsonValue* location =
        lsp_local_record_field_location(doc, arena, uri, target_type, field->b);
    if (location != NULL) {
        return location;
    }

    JsonValue* ast_location =
        lsp_ast_field_location(doc, arena, uri, field_node_index);
    if (ast_location != NULL) {
        return ast_location;
    }

    return lsp_unique_record_field_location(doc, arena, uri, field->b);
}

//------------------------------------------------------------------------------
// Build a request context for one position-based LSP query.

internal bool lsp_get_request_context(LspState*           state,
                                      const LspMessage*   message,
                                      const LspDocument** out_doc,
                                      string*             out_uri,
                                      usize*              out_offset,
                                      u32*                out_token_index,
                                      const Token**       out_token)
{
    JsonValue* uri_value =
        json_get_cstr(message->message, "params.textDocument.uri");
    JsonValue* line_value =
        json_get_cstr(message->message, "params.position.line");
    JsonValue* col_value =
        json_get_cstr(message->message, "params.position.character");

    if (!uri_value || !line_value || !col_value ||
        uri_value->kind != JSON_STRING || line_value->kind != JSON_NUMBER ||
        col_value->kind != JSON_NUMBER) {
        return false;
    }

    *out_uri           = json_string(uri_value);
    LspSyntaxView view = {0};
    if (!lsp_syntax_view(state, *out_uri, &view)) {
        return false;
    }
    *out_doc            = view.doc;

    u32 line            = (u32)json_integer(line_value);
    u32 col             = (u32)json_integer(col_value);

    usize visible_start = 0;
    if (lsp_document_visible_start(*out_doc, &visible_start) &&
        !string_eq(view.lexer->source.source, (*out_doc)->source)) {
        NerdSource visible_source = {
            .source      = (*out_doc)->source,
            .source_path = view.lexer->source.source_path,
        };
        usize visible_offset = 0;
        if (!lex_line_col_to_offset(
                visible_source, line, col, &visible_offset)) {
            return false;
        }
        *out_offset = visible_start + visible_offset;
    } else if (!lex_line_col_to_offset(
                   view.lexer->source, line, col, out_offset)) {
        return false;
    }

    u32    token_end = 0;
    Token* token     = lex_find(view.lexer, *out_offset, &token_end);
    if (!token) {
        return false;
    }

    *out_token_index = lsp_token_index_from_pointer(view.lexer, token);
    *out_token       = token;
    return true;
}

//------------------------------------------------------------------------------
// Return the LSP symbol kind for one semantic declaration.

internal int lsp_decl_symbol_kind(const SemaDecl* decl)
{
    if (decl->kind == SK_Function || decl->kind == SK_GenericFunction ||
        decl->kind == SK_FfiFunction) {
        return LSP_SYMBOL_KIND_FUNCTION;
    }
    if (decl->kind == SK_Trait) {
        return LSP_SYMBOL_KIND_INTERFACE;
    }
    return LSP_SYMBOL_KIND_CONSTANT;
}

//------------------------------------------------------------------------------
// Return a syntax-only symbol kind for CST bindings when declaration facts are
// not available.

internal bool lsp_cst_binding_value(const Cst*      cst,
                                    const CstNode*  bind,
                                    const CstNode** out_value)
{
    if (!cst || !bind || bind->b >= array_count(cst->nodes)) {
        return false;
    }

    const CstNode* value = &cst->nodes[bind->b];
    if (value->kind == CK_AnnotatedValue &&
        value->b < array_count(cst->nodes)) {
        value = &cst->nodes[value->b];
    }

    *out_value = value;
    return true;
}

internal int lsp_cst_binding_symbol_kind(const Cst* cst, const CstNode* bind)
{
    const CstNode* value = NULL;
    if (lsp_cst_binding_value(cst, bind, &value) &&
        (value->kind == CK_FnExpr || value->kind == CK_FnBlock ||
         value->kind == CK_FfiDef)) {
        return LSP_SYMBOL_KIND_FUNCTION;
    }
    if (value && value->kind == CK_Trait) {
        return LSP_SYMBOL_KIND_INTERFACE;
    }

    return bind->kind == CK_Variable ? LSP_SYMBOL_KIND_VARIABLE
                                     : LSP_SYMBOL_KIND_CONSTANT;
}

internal string lsp_cst_binding_detail(Arena*         arena,
                                       const Cst*     cst,
                                       const CstNode* bind)
{
    UNUSED(arena);

    const CstNode* value = NULL;
    if (lsp_cst_binding_value(cst, bind, &value) &&
        (value->kind == CK_FnExpr || value->kind == CK_FnBlock ||
         value->kind == CK_FfiDef)) {
        return s("function");
    }
    if (value && value->kind == CK_Trait) {
        return s("trait");
    }

    return bind->kind == CK_Variable ? s("variable") : s("binding");
}

//------------------------------------------------------------------------------
// Add one document symbol from a CST declaration-like node.

internal bool lsp_cst_symbol_node_info(const Cst*     cst,
                                       const CstNode* node,
                                       u32*           out_symbol_handle,
                                       u32*           out_token_index,
                                       int*           out_fallback_kind,
                                       string*        out_fallback_detail)
{
    if (!cst || !node) {
        return false;
    }

    if (node->kind == CK_FfiDef) {
        if (node->a >= array_count(cst->ffi_infos)) {
            return false;
        }
        const CstFfiInfo* ffi = &cst->ffi_infos[node->a];
        *out_symbol_handle    = ffi->symbol_handle;
        *out_token_index      = ffi->token_index;
        *out_fallback_kind    = LSP_SYMBOL_KIND_FUNCTION;
        *out_fallback_detail  = s("function");
        return true;
    }

    if (node->kind != CK_Bind && node->kind != CK_Variable) {
        return false;
    }

    *out_symbol_handle   = node->a;
    *out_token_index     = node->token_index;
    *out_fallback_kind   = lsp_cst_binding_symbol_kind(cst, node);
    *out_fallback_detail = lsp_cst_binding_detail(&temp_arena, cst, node);
    return true;
}

internal void lsp_append_document_symbol(Arena*             arena,
                                         JsonValue*         result,
                                         const LspDocument* doc,
                                         const CstNode*     node,
                                         u32                symbol_handle,
                                         u32                token_index,
                                         int                fallback_kind,
                                         string             fallback_detail,
                                         const Sema*        sema)
{
    u32             decl_index = LSP_NO_DECL;
    const SemaDecl* decl       = NULL;
    if (sema != NULL) {
        decl_index = lsp_find_decl_index_by_symbol_handle(sema, symbol_handle);
        if (decl_index != LSP_NO_DECL &&
            !lsp_sema_decl(sema, decl_index, &decl)) {
            decl_index = LSP_NO_DECL;
            decl       = NULL;
        }
    }

    usize start_offset;
    usize end_offset;
    lsp_token_offsets(
        &doc->front_end.lexer, token_index, &start_offset, &end_offset);

    JsonValue* symbol = json_new_object(arena);
    json_object_set_string(symbol,
                           arena,
                           "name",
                           lex_symbol(&doc->front_end.lexer, symbol_handle));
    json_object_set_number(symbol,
                           arena,
                           "kind",
                           decl ? lsp_decl_symbol_kind(decl) : fallback_kind);
    json_object_set_object(
        symbol,
        "range",
        lsp_make_document_range(doc, arena, start_offset, end_offset));
    json_object_set_object(
        symbol,
        "selectionRange",
        lsp_make_document_range(doc, arena, start_offset, end_offset));

    if (!decl) {
        json_object_set_string(symbol, arena, "detail", fallback_detail);
    } else if (decl->kind == SK_TypeAlias) {
        json_object_set_string(symbol, arena, "detail", s("type alias"));
    } else if (decl->kind == SK_Trait) {
        json_object_set_string(symbol, arena, "detail", s("trait"));
    } else if (decl->kind == SK_Constant) {
        i64 value = 0;
        if (lsp_eval_decl_value(doc, decl_index, &value)) {
            json_object_set_string(
                symbol,
                arena,
                "detail",
                string_format(arena, "constant = %lld", value));
        } else {
            json_object_set_string(symbol, arena, "detail", s("constant"));
        }
    } else {
        json_object_set_string(symbol, arena, "detail", s("function"));
    }

    json_array_push(result, symbol);
    UNUSED(node);
}

internal void lsp_append_ffi_block_document_symbols(Arena*             arena,
                                                    JsonValue*         result,
                                                    const LspDocument* doc,
                                                    const CstNode*     node,
                                                    const Sema*        sema)
{
    if (node->a >= array_count(doc->cst.ffi_block_infos)) {
        return;
    }
    const CstFfiBlockInfo* block = &doc->cst.ffi_block_infos[node->a];
    u32 block_end = block->first_ffi_info + block->ffi_info_count;
    for (u32 i = block->first_ffi_info; i < block_end; ++i) {
        if (i >= array_count(doc->cst.ffi_infos)) {
            break;
        }
        const CstFfiInfo* ffi = &doc->cst.ffi_infos[i];
        lsp_append_document_symbol(arena,
                                   result,
                                   doc,
                                   node,
                                   ffi->symbol_handle,
                                   ffi->token_index,
                                   LSP_SYMBOL_KIND_FUNCTION,
                                   s("function"),
                                   sema);
    }
}

internal void lsp_append_impl_document_symbol(Arena*             arena,
                                              JsonValue*         result,
                                              const LspDocument* doc,
                                              const CstNode*     node)
{
    if (node->a >= array_count(doc->cst.impls)) {
        return;
    }

    const AstImplInfo* ast_impl = NULL;
    for (u32 i = 0; i < array_count(doc->front_end.ast.nodes); ++i) {
        const AstNode* ast_node = &doc->front_end.ast.nodes[i];
        if (ast_node->kind != AK_Impl ||
            ast_node->token_index != node->token_index ||
            ast_node->a >= array_count(doc->front_end.ast.impls)) {
            continue;
        }
        ast_impl = &doc->front_end.ast.impls[ast_node->a];
        break;
    }

    string target = s("<unknown>");
    string trait  = s("");
    if (ast_impl != NULL) {
        target =
            lsp_ast_type_node_source(doc, ast_impl->target_type_node_index);
        if (ast_impl->trait_type_node_index != U32_MAX) {
            trait =
                lsp_ast_type_node_source(doc, ast_impl->trait_type_node_index);
        }
    }

    string name = trait.count == 0
                      ? string_format(arena, "impl " STRINGP, STRINGV(target))
                      : string_format(arena,
                                      "impl " STRINGP " for " STRINGP,
                                      STRINGV(trait),
                                      STRINGV(target));

    usize start_offset;
    usize end_offset;
    lsp_token_offsets(
        &doc->front_end.lexer, node->token_index, &start_offset, &end_offset);

    JsonValue* symbol = json_new_object(arena);
    json_object_set_string(symbol, arena, "name", name);
    json_object_set_number(symbol, arena, "kind", LSP_SYMBOL_KIND_INTERFACE);
    json_object_set_object(
        symbol,
        "range",
        lsp_make_document_range(doc, arena, start_offset, end_offset));
    json_object_set_object(
        symbol,
        "selectionRange",
        lsp_make_document_range(doc, arena, start_offset, end_offset));
    json_object_set_string(symbol, arena, "detail", s("impl"));
    json_array_push(result, symbol);
}

internal void lsp_append_document_symbol_node(Arena*             arena,
                                              JsonValue*         result,
                                              const LspDocument* doc,
                                              u32                node_index,
                                              const Sema*        sema);

internal void lsp_append_top_on_document_symbols(Arena*             arena,
                                                 JsonValue*         result,
                                                 const LspDocument* doc,
                                                 const CstNode*     node,
                                                 const Sema*        sema)
{
    if (node->a >= array_count(doc->cst.top_ons)) {
        return;
    }
    const CstTopOnInfo* info = &doc->cst.top_ons[node->a];
    if (info->body_node_index >= array_count(doc->cst.nodes)) {
        return;
    }
    const CstNode* body = &doc->cst.nodes[info->body_node_index];
    if (body->kind != CK_Block) {
        return;
    }
    for (u32 i = body->a; i < body->b; ++i) {
        lsp_append_document_symbol_node(arena, result, doc, i, sema);
        const CstNode* child = &doc->cst.nodes[i];
        if (child->kind == CK_TopOn &&
            child->a < array_count(doc->cst.top_ons)) {
            const CstTopOnInfo* child_info = &doc->cst.top_ons[child->a];
            if (child_info->body_node_index < array_count(doc->cst.nodes)) {
                const CstNode* child_body =
                    &doc->cst.nodes[child_info->body_node_index];
                if (child_body->kind == CK_Block && child_body->b > 0) {
                    i = child_body->b - 1;
                }
            }
        }
    }
}

internal void lsp_append_document_symbol_node(Arena*             arena,
                                              JsonValue*         result,
                                              const LspDocument* doc,
                                              u32                node_index,
                                              const Sema*        sema)
{
    if (node_index >= array_count(doc->cst.nodes)) {
        return;
    }
    const CstNode* node = &doc->cst.nodes[node_index];
    if (node->kind == CK_TopOn) {
        lsp_append_top_on_document_symbols(arena, result, doc, node, sema);
        return;
    }
    if (node->kind == CK_FfiBlock) {
        lsp_append_ffi_block_document_symbols(arena, result, doc, node, sema);
        return;
    }
    if (node->kind == CK_Impl) {
        lsp_append_impl_document_symbol(arena, result, doc, node);
        return;
    }

    u32    symbol_handle = U32_MAX;
    u32    token_index   = U32_MAX;
    int    kind          = LSP_SYMBOL_KIND_CONSTANT;
    string detail        = {0};
    if (!lsp_cst_symbol_node_info(
            &doc->cst, node, &symbol_handle, &token_index, &kind, &detail)) {
        return;
    }
    lsp_append_document_symbol(arena,
                               result,
                               doc,
                               node,
                               symbol_handle,
                               token_index,
                               kind,
                               detail,
                               sema);
}

//------------------------------------------------------------------------------
// Respond to hover requests with semantic information about the token under the
// cursor.

void lsp_handle_hover(LspState* state, const LspMessage* message)
{
    JsonValue*         response    = lsp_prepare_response(message);
    const LspDocument* doc         = NULL;
    string             uri         = {0};
    usize              offset      = 0;
    u32                token_index = 0;
    const Token*       token       = NULL;

    if (!lsp_get_request_context(
            state, message, &doc, &uri, &offset, &token_index, &token)) {
        lsp_cancel(response, message->arena);
        return;
    }

    switch (token->kind) {
    case TK_Integer:
        {
            u32 field_node_index =
                lsp_find_field_node_at_token(&doc->front_end.ast, token_index);
            if (field_node_index != U32_MAX) {
                string field_hover =
                    lsp_field_hover_text(doc, message->arena, field_node_index);
                if (field_hover.count != 0) {
                    lsp_set_markdown_hover(
                        response, message->arena, field_hover);
                    break;
                }
            }

            string source_test_hover =
                lsp_source_test_hover_text(doc, message->arena, offset);
            if (source_test_hover.count != 0) {
                lsp_set_markdown_hover(
                    response, message->arena, source_test_hover);
                break;
            }

            usize token_end =
                lex_token_end_offset(&doc->front_end.lexer, token);
            string raw_text = string_from(
                doc->front_end.lexer.source.source.data + token->offset,
                token_end - token->offset);
            lsp_set_markdown_hover(
                response,
                message->arena,
                string_format(
                    message->arena,
                    STRINGP "\n\n- Type: `" STRINGP "`",
                    STRINGV(lsp_markdown_code_block(message->arena, raw_text)),
                    STRINGV(s("untyped integer"))));
        }
        break;
    case TK_String:
        {
            usize token_end =
                lex_token_end_offset(&doc->front_end.lexer, token);
            string raw_text = string_from(
                doc->front_end.lexer.source.source.data + token->offset,
                token_end - token->offset);
            lsp_set_markdown_hover(
                response,
                message->arena,
                string_format(message->arena,
                              STRINGP "\n\n- Type: `string`",
                              STRINGV(lsp_markdown_code_block(message->arena,
                                                              raw_text))));
        }
        break;

    case TK_Symbol:
        {
            u32 local_index = lsp_find_local_index_for_token(doc, token_index);
            if (local_index != sema_no_local()) {
                lsp_set_markdown_hover(
                    response,
                    message->arena,
                    lsp_local_hover_text(doc, message->arena, local_index));
                break;
            }

            string member_hover =
                lsp_member_bind_hover_text(doc, message->arena, token_index);
            if (member_hover.count != 0) {
                lsp_set_markdown_hover(response, message->arena, member_hover);
                break;
            }

            u32 field_node_index =
                lsp_find_field_node_at_token(&doc->front_end.ast, token_index);
            if (field_node_index != U32_MAX) {
                string field_hover =
                    lsp_field_hover_text(doc, message->arena, field_node_index);
                if (field_hover.count != 0) {
                    lsp_set_markdown_hover(
                        response, message->arena, field_hover);
                    break;
                }
            }

            u32 literal_node_index  = U32_MAX;
            u32 literal_field_index = U32_MAX;
            if (lsp_find_plex_literal_field_at_token(&doc->front_end.ast,
                                                     token_index,
                                                     &literal_node_index,
                                                     &literal_field_index)) {
                string literal_field_hover =
                    lsp_plex_literal_field_hover_text(doc,
                                                      message->arena,
                                                      literal_node_index,
                                                      literal_field_index);
                if (literal_field_hover.count != 0) {
                    lsp_set_markdown_hover(
                        response, message->arena, literal_field_hover);
                    break;
                }
            }

            u32 pattern_index       = U32_MAX;
            u32 pattern_field_index = U32_MAX;
            if (lsp_find_plex_pattern_field_at_token(&doc->front_end.ast,
                                                     token_index,
                                                     &pattern_index,
                                                     &pattern_field_index)) {
                string pattern_field_hover = lsp_plex_pattern_field_hover_text(
                    doc, message->arena, pattern_index, pattern_field_index);
                if (pattern_field_hover.count != 0) {
                    lsp_set_markdown_hover(
                        response, message->arena, pattern_field_hover);
                    break;
                }
            }

            string enum_variant_hover =
                lsp_enum_variant_hover_text(doc, message->arena, token_index);
            if (enum_variant_hover.count != 0) {
                lsp_set_markdown_hover(
                    response, message->arena, enum_variant_hover);
                break;
            }

            u32 decl_index = lsp_find_decl_index_for_token(doc, token_index);
            if (decl_index != LSP_NO_DECL) {
                lsp_set_markdown_hover(
                    response,
                    message->arena,
                    lsp_decl_hover_text(doc, message->arena, decl_index));
                break;
            }

            u32 symbol_handle =
                lsp_symbol_handle_at_token(&doc->front_end.lexer, token_index);
            if (symbol_handle != U32_MAX) {
                string imported_hover = lsp_imported_symbol_hover_text(
                    doc,
                    message->arena,
                    lex_symbol(&doc->front_end.lexer, symbol_handle));
                if (imported_hover.count != 0) {
                    lsp_set_markdown_hover(
                        response, message->arena, imported_hover);
                    break;
                }
            }
            string builtin_hover =
                symbol_handle == U32_MAX
                    ? s("")
                    : lsp_builtin_type_hover_text(
                          message->arena,
                          lex_symbol(&doc->front_end.lexer, symbol_handle));
            if (builtin_hover.count != 0) {
                lsp_set_markdown_hover(response, message->arena, builtin_hover);
                break;
            }

            u32 ref_node_index = lsp_find_symbol_ref_node_at_token(
                &doc->front_end.ast, token_index);
            string expression_hover =
                ref_node_index == U32_MAX
                    ? s("")
                    : lsp_expression_hover_text(
                          doc, message->arena, ref_node_index);
            if (expression_hover.count != 0) {
                lsp_set_markdown_hover(
                    response, message->arena, expression_hover);
                break;
            }

            string source_test_hover =
                lsp_source_test_hover_text(doc, message->arena, offset);
            if (source_test_hover.count != 0) {
                lsp_set_markdown_hover(
                    response, message->arena, source_test_hover);
                break;
            }

            lsp_cancel(response, message->arena);
            return;
        }
        break;

    case TK_fn:
        {
            string signature  = s("fn ()");
            u32    decl_index = lsp_find_decl_index_for_token(doc, token_index);
            if (decl_index != LSP_NO_DECL) {
                const SemaDecl* decl = NULL;
                if (!lsp_sema_decl(&doc->front_end.sema, decl_index, &decl)) {
                    lsp_cancel(response, message->arena);
                    return;
                }
                signature = lsp_decl_signature(doc, message->arena, decl);
            }
            lsp_set_markdown_hover(
                response,
                message->arena,
                string_format(
                    message->arena,
                    STRINGP "\n\n- Kind: function expression"
                            "\n- Signature: `" STRINGP "`",
                    STRINGV(lsp_markdown_code_block(message->arena, signature)),
                    STRINGV(signature)));
        }
        break;

    default:
        lsp_cancel(response, message->arena);
        return;
    }

    UNUSED(uri);
    UNUSED(offset);
    lsp_send_response(message->arena, response);
}

//------------------------------------------------------------------------------
// Respond to definition requests with the binding location of the resolved
// declaration.

void lsp_handle_definition(LspState* state, const LspMessage* message)
{
    JsonValue*         response    = lsp_prepare_response(message);
    const LspDocument* doc         = NULL;
    string             uri         = {0};
    usize              offset      = 0;
    u32                token_index = 0;
    const Token*       token       = NULL;

    if (!lsp_get_request_context(
            state, message, &doc, &uri, &offset, &token_index, &token)) {
        lsp_cancel(response, message->arena);
        return;
    }

    if (token->kind != TK_Symbol) {
        lsp_cancel(response, message->arena);
        return;
    }

    u32 field_node_index =
        lsp_find_field_node_at_token(&doc->front_end.ast, token_index);
    if (field_node_index != U32_MAX) {
        u32 method_decl =
            lsp_selected_method_decl_for_field(doc, field_node_index);
        if (method_decl != LSP_NO_DECL) {
            JsonValue* method_location =
                lsp_decl_location(doc, message->arena, uri, method_decl);
            if (method_location != NULL) {
                json_object_set_object(response, "result", method_location);
                lsp_send_response(message->arena, response);
                return;
            }
        }

        JsonValue* field_location =
            lsp_field_location(doc, message->arena, uri, field_node_index);
        if (field_location != NULL) {
            json_object_set_object(response, "result", field_location);
            lsp_send_response(message->arena, response);
            return;
        }

        JsonValue* enum_variant_location = lsp_qualified_enum_variant_location(
            doc, message->arena, uri, field_node_index);
        if (enum_variant_location != NULL) {
            json_object_set_object(response, "result", enum_variant_location);
            lsp_send_response(message->arena, response);
            return;
        }
    }

    u32 modref_node_index = lsp_find_modref_node_at_token(
        &doc->front_end.lexer, &doc->front_end.ast, token_index);
    if (modref_node_index != U32_MAX) {
        u32             module_type = sema_no_type();
        const SemaType* type        = NULL;
        if (lsp_sema_node_type(
                &doc->front_end.sema, modref_node_index, &module_type) &&
            lsp_sema_type(&doc->front_end.sema, module_type, &type)) {
            LspModuleView module = {0};
            if (lsp_program_module_view_by_type(&doc->program, type, &module)) {
                JsonValue* location =
                    lsp_module_file_location(module, message->arena);
                json_object_set_object(response, "result", location);
                lsp_send_response(message->arena, response);
                return;
            }
        }

        const AstNode* modref = &doc->front_end.ast.nodes[modref_node_index];
        if (modref->a < array_count(doc->front_end.ast.module_paths)) {
            Arena temp_arena = {0};
            arena_init(&temp_arena);
            ModuleResolveResult resolved = {0};
            ModuleResolveStatus status =
                module_resolve_path(&temp_arena,
                                    doc->front_end.lexer.source,
                                    &doc->front_end.lexer,
                                    &doc->front_end.ast,
                                    &doc->front_end.ast.module_paths[modref->a],
                                    &resolved);
            if (status == MRS_Found) {
                LspModuleView module = {0};
                if (lsp_program_module_view_by_path(
                        &doc->program, resolved.resolved_path, &module)) {
                    JsonValue* location =
                        lsp_module_file_location(module, message->arena);
                    arena_done(&temp_arena);
                    json_object_set_object(response, "result", location);
                    lsp_send_response(message->arena, response);
                    return;
                }
            }
            arena_done(&temp_arena);
        }
    }

    u32 local_index = lsp_find_local_index_for_token(doc, token_index);
    if (local_index != sema_no_local()) {
        const SemaLocal* local = NULL;
        if (!lsp_sema_local(&doc->front_end.sema, local_index, &local)) {
            lsp_cancel(response, message->arena);
            return;
        }

        JsonValue* location = NULL;
        if (local->decl_node_index < array_count(doc->front_end.ast.nodes)) {
            const AstNode* decl_node =
                &doc->front_end.ast.nodes[local->decl_node_index];
            u32 module_type = sema_no_type();
            if (decl_node->kind == AK_Use &&
                lsp_sema_node_type(
                    &doc->front_end.sema, decl_node->a, &module_type)) {
                location = lsp_imported_symbol_location(
                    doc, message->arena, module_type, local->symbol_handle);
            }
        }
        if (location == NULL) {
            location =
                lsp_local_location(doc, message->arena, uri, local_index);
        }
        json_object_set_object(response, "result", location);
        lsp_send_response(message->arena, response);
        return;
    }

    u32 decl_index = lsp_find_decl_index_for_token(doc, token_index);
    if (decl_index == LSP_NO_DECL) {
        JsonValue* enum_pattern_location = lsp_enum_pattern_variant_location(
            doc, message->arena, uri, token_index);
        if (enum_pattern_location != NULL) {
            json_object_set_object(response, "result", enum_pattern_location);
            lsp_send_response(message->arena, response);
            return;
        }

        JsonValue* enum_variant_location = lsp_contextual_enum_variant_location(
            doc, message->arena, uri, token_index);
        if (enum_variant_location != NULL) {
            json_object_set_object(response, "result", enum_variant_location);
            lsp_send_response(message->arena, response);
            return;
        }

        JsonValue* ast_location =
            lsp_ast_definition_location(doc, message->arena, uri, token_index);
        if (ast_location != NULL) {
            json_object_set_object(response, "result", ast_location);
            lsp_send_response(message->arena, response);
            return;
        }
        u32 symbol_handle =
            lsp_symbol_handle_at_token(&doc->front_end.lexer, token_index);
        if (symbol_handle != U32_MAX) {
            JsonValue* imported_location = lsp_ast_imported_symbol_location(
                doc,
                message->arena,
                lex_symbol(&doc->front_end.lexer, symbol_handle));
            if (imported_location != NULL) {
                json_object_set_object(response, "result", imported_location);
                lsp_send_response(message->arena, response);
                return;
            }
        }
        lsp_cancel(response, message->arena);
        return;
    }

    JsonValue* location =
        lsp_decl_location(doc, message->arena, uri, decl_index);
    if (!location) {
        JsonValue* ast_location =
            lsp_ast_definition_location(doc, message->arena, uri, token_index);
        if (ast_location != NULL) {
            json_object_set_object(response, "result", ast_location);
            lsp_send_response(message->arena, response);
            return;
        }
        lsp_cancel(response, message->arena);
        return;
    }

    json_object_set_object(response, "result", location);
    lsp_send_response(message->arena, response);
}

//------------------------------------------------------------------------------
// Respond to document symbol requests with the top-level declarations in source
// order.

void lsp_handle_document_symbol(LspState* state, const LspMessage* message)
{
    JsonValue* uri_value =
        json_get_cstr(message->message, "params.textDocument.uri");
    JsonValue* response = lsp_prepare_response(message);

    if (!uri_value || uri_value->kind != JSON_STRING) {
        lsp_cancel(response, message->arena);
        return;
    }

    string        uri         = json_string(uri_value);
    LspSourceView source_view = {0};
    if (!lsp_source_view(state, uri, &source_view)) {
        json_object_set_array(
            response, "result", json_new_array(message->arena));
        lsp_send_response(message->arena, response);
        return;
    }
    const LspDocument* doc = source_view.doc;

    JsonValue* result      = json_new_array(message->arena);

    if (doc->cst_ready) {
        LspDeclarationView decl_view = {0};
        bool        has_decls = lsp_declaration_view(state, uri, &decl_view);
        const Sema* sema      = has_decls ? decl_view.sema : NULL;

        for (u32 i = 0; i < array_count(doc->cst.bindings); ++i) {
            lsp_append_document_symbol_node(
                message->arena, result, doc, doc->cst.bindings[i], sema);
        }
    }

    json_object_set_array(response, "result", result);
    lsp_send_response(message->arena, response);
}

//------------------------------------------------------------------------------
// Respond to workspace symbol requests with top-level semantic declarations
// from open documents and the modules already analysed through their imports.

internal u8 lsp_workspace_symbol_lower(u8 ch)
{
    return (ch >= 'A' && ch <= 'Z') ? (ch - 'A' + 'a') : ch;
}

internal bool lsp_workspace_symbol_is_internal(string name)
{
    return name.count >= 2 && name.data[0] == '_' && name.data[1] == '_';
}

internal bool lsp_workspace_symbol_matches(string name, string query)
{
    if (query.count == 0) {
        return true;
    }
    if (query.count > name.count) {
        return false;
    }

    for (usize start = 0; start + query.count <= name.count; ++start) {
        bool matched = true;
        for (usize i = 0; i < query.count; ++i) {
            if (lsp_workspace_symbol_lower(name.data[start + i]) !=
                lsp_workspace_symbol_lower(query.data[i])) {
                matched = false;
                break;
            }
        }
        if (matched) {
            return true;
        }
    }
    return false;
}

internal string lsp_workspace_path_to_uri(Arena* arena, string path)
{
    if (path.count >= strlen("file://") &&
        memcmp(path.data, "file://", strlen("file://")) == 0) {
        return path;
    }

    StringBuilder sb = {0};
    sb_init(&sb, arena);
    sb_append_cstr(&sb, "file://");

#if OS_WINDOWS
    if (path.count > 1 && path.data[0] == '/' && path.data[2] == ':') {
        path.data += 1;
        path.count -= 1;
    }
    if (path.count > 1 && path.data[1] == ':') {
        sb_append_char(&sb, '/');
    }
#endif

    for (usize i = 0; i < path.count; ++i) {
        u8 ch = path.data[i];
#if OS_WINDOWS
        if (ch == '\\') {
            sb_append_char(&sb, '/');
            continue;
        }
#endif
        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') ||
            (ch >= '0' && ch <= '9') || ch == '/' || ch == '-' || ch == '_' ||
            ch == '.' || ch == '~') {
            sb_append_char(&sb, (char)ch);
        } else {
            sb_format(&sb, "%%%02X", ch);
        }
    }

    return sb_to_string(&sb);
}

internal bool lsp_workspace_seen_module(Array(string) seen, string uri)
{
    for (usize i = 0; i < array_count(seen); ++i) {
        if (string_eq(seen[i], uri)) {
            return true;
        }
    }
    return false;
}

internal void lsp_workspace_append_symbol(Arena*       arena,
                                          JsonValue*   result,
                                          string       uri,
                                          NerdSource   source,
                                          const Lexer* lexer,
                                          const Ast*   ast,
                                          const Sema*  sema,
                                          u32          decl_index,
                                          string       query)
{
    const SemaDecl* decl = NULL;
    if (!lsp_sema_decl(sema, decl_index, &decl) ||
        decl->symbol_handle == U32_MAX || decl->kind == SK_Module ||
        decl->kind == SK_BuiltinFunction) {
        return;
    }

    string name = lex_symbol(lexer, decl->symbol_handle);
    if (lsp_workspace_symbol_is_internal(name) ||
        !lsp_workspace_symbol_matches(name, query) ||
        decl->bind_node_index >= array_count(ast->nodes)) {
        return;
    }

    u32 token_index = ast->nodes[decl->bind_node_index].token_index;
    if (token_index >= array_count(lexer->tokens)) {
        return;
    }

    usize start = 0;
    usize end   = 0;
    lsp_token_offsets(lexer, token_index, &start, &end);

    JsonValue* location = json_new_object(arena);
    json_object_set_string(location, arena, "uri", uri);
    json_object_set_object(
        location, "range", lsp_make_range(arena, source, start, end));

    JsonValue* symbol = json_new_object(arena);
    json_object_set_string(symbol, arena, "name", name);
    json_object_set_number(symbol, arena, "kind", lsp_decl_symbol_kind(decl));
    json_object_set_object(symbol, "location", location);
    json_array_push(result, symbol);
}

internal void lsp_workspace_collect_module(Arena*     arena,
                                           JsonValue* result,
                                           Array(string) * seen_modules,
                                           LspModuleView module,
                                           string        query)
{
    string path = module.info && module.info->resolved_path
                      ? s(module.info->resolved_path)
                      : module.lexer->source.source_path;
    string uri  = lsp_workspace_path_to_uri(arena, path);
    if (lsp_workspace_seen_module(*seen_modules, uri)) {
        return;
    }
    array_push(*seen_modules, uri);

    for (u32 i = 0; i < array_count(module.sema->decls); ++i) {
        lsp_workspace_append_symbol(arena,
                                    result,
                                    uri,
                                    module.lexer->source,
                                    module.lexer,
                                    module.ast,
                                    module.sema,
                                    i,
                                    query);
    }
}

void lsp_handle_workspace_symbol(LspState* state, const LspMessage* message)
{
    JsonValue* response = lsp_prepare_response(message);
    JsonValue* result   = json_new_array(message->arena);
    string     query    = {0};
    (void)lsp_get_string_param(message, "params.query", &query);

    Array(string) seen_modules = {0};

    MapIter      iter          = LspDocumentMap_iter();
    string       uri;
    LspDocument* doc;
    while (LspDocumentMap_next(&state->documents, &iter, &uri, &doc)) {
        if (!doc->sema_partial) {
            continue;
        }
        for (u32 i = 0; i < array_count(doc->program.modules); ++i) {
            LspModuleView module = {0};
            if (lsp_program_module_view(&doc->program, i, &module) &&
                module.lexer && module.ast && module.sema) {
                lsp_workspace_collect_module(
                    message->arena, result, &seen_modules, module, query);
            }
        }
    }

    json_object_set_array(response, "result", result);
    lsp_send_response(message->arena, response);
}

//------------------------------------------------------------------------------
