//------------------------------------------------------------------------------
// LSP rename support
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <lsp/lsp.h>

//------------------------------------------------------------------------------

typedef enum {
    LSP_RENAME_NONE,
    LSP_RENAME_LOCAL,
    LSP_RENAME_DECL,
    LSP_RENAME_SYMBOL,
    LSP_RENAME_MEMBER,
} LspRenameKind;

typedef struct {
    LspRenameKind kind;
    u32           index;
    u32           token_index;
    string        name;
    cstr          origin_path;
    usize         member_offset;
    const Lexer*  owner_lexer;
    u32           owner_node;
} LspRenameTarget;

typedef struct {
    const LspDocument* doc;
    LspDocument        scratch_doc;
    Lexer              scratch_lexer;
    Ast                scratch_ast;
    bool               has_scratch;
    string             uri;
    u32                token_index;
} LspRenameRequestContext;

typedef struct {
    string       uri;
    const Lexer* lexer;
    u32          token_index;
    bool         is_declaration;
    string       shorthand_name;
} LspRenameOccurrence;

typedef struct {
    string     uri;
    JsonValue* edits;
} LspRenameEditGroup;

//------------------------------------------------------------------------------

internal JsonValue*
lsp_rename_make_position(Arena* arena, NerdSource source, usize offset)
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

internal JsonValue* lsp_rename_make_range(Arena*     arena,
                                          NerdSource source,
                                          usize      start_offset,
                                          usize      end_offset)
{
    JsonValue* range = json_new_object(arena);
    json_object_set_object(
        range, "start", lsp_rename_make_position(arena, source, start_offset));
    json_object_set_object(
        range, "end", lsp_rename_make_position(arena, source, end_offset));
    return range;
}

internal bool lsp_rename_document_visible_start(const LspDocument* doc,
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

internal JsonValue* lsp_rename_make_document_range(const LspDocument* doc,
                                                   Arena*             arena,
                                                   usize start_offset,
                                                   usize end_offset)
{
    usize visible_start = 0;
    if (lsp_rename_document_visible_start(doc, &visible_start) &&
        start_offset >= visible_start && end_offset >= visible_start &&
        end_offset <= visible_start + doc->source.count) {
        NerdSource visible_source = {
            .source      = doc->source,
            .source_path = doc->front_end.lexer.source.source_path,
        };
        return lsp_rename_make_range(arena,
                                     visible_source,
                                     start_offset - visible_start,
                                     end_offset - visible_start);
    }

    return NULL;
}

internal u32 lsp_rename_token_index_from_pointer(const Lexer* lexer,
                                                 const Token* token)
{
    ASSERT(token >= lexer->tokens &&
               token < lexer->tokens + array_count(lexer->tokens),
           "Expected token pointer to belong to the lexer");
    return (u32)(token - lexer->tokens);
}

internal void lsp_rename_token_offsets(const Lexer* lexer,
                                       u32          token_index,
                                       usize*       out_start,
                                       usize*       out_end)
{
    const Token* token = &lexer->tokens[token_index];
    *out_start         = token->offset;
    *out_end           = lex_token_end_offset(lexer, token);
}

internal bool lsp_rename_token_is_visible(const LspDocument* doc,
                                          u32                token_index)
{
    if (token_index >= array_count(doc->front_end.lexer.tokens)) {
        return false;
    }

    usize start = 0;
    usize end   = 0;
    lsp_rename_token_offsets(&doc->front_end.lexer, token_index, &start, &end);
    usize visible_start = 0;
    return lsp_rename_document_visible_start(doc, &visible_start) &&
           start >= visible_start && end >= visible_start &&
           end <= visible_start + doc->source.count;
}

//------------------------------------------------------------------------------

internal bool lsp_rename_is_ident_start(u8 ch)
{
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_';
}

internal bool lsp_rename_is_ident_char(u8 ch)
{
    return lsp_rename_is_ident_start(ch) || (ch >= '0' && ch <= '9');
}

internal bool lsp_rename_valid_new_name(string name)
{
    if (name.count == 0 || !lsp_rename_is_ident_start(name.data[0])) {
        return false;
    }
    for (usize i = 1; i < name.count; ++i) {
        if (!lsp_rename_is_ident_char(name.data[i])) {
            return false;
        }
    }
    Lexer lexer = {0};
    bool valid = lex((NerdSource){.source = name, .source_path = s("<rename>")},
                     &lexer) &&
                 array_count(lexer.tokens) > 0 &&
                 lexer.tokens[0].kind == TK_Symbol;
    lex_done(&lexer);
    return valid;
}

internal cstr lsp_rename_cstr(Arena* arena, string value)
{
    char* data = arena_alloc(arena, value.count + 1);
    memcpy(data, value.data, value.count);
    data[value.count] = '\0';
    return data;
}

internal string lsp_rename_path_to_uri(Arena* arena, cstr path)
{
    StringBuilder sb = {0};
    sb_init(&sb, arena);
    sb_append_cstr(&sb, "file://");

#if OS_WINDOWS
    if (path[0] == '/' && path[1] != '\0' && path[2] == ':') {
        path++;
    }
    if (path[0] != '\0' && path[1] == ':') {
        sb_append_char(&sb, '/');
    }
#endif

    for (usize i = 0; path[i] != '\0'; ++i) {
        u8 ch = (u8)path[i];
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

internal void lsp_rename_request_context_done(LspRenameRequestContext* context)
{
    if (!context->has_scratch) {
        return;
    }

    ast_done(&context->scratch_ast);
    lex_done(&context->scratch_lexer);
}

internal bool lsp_rename_parse_scratch_doc(LspRenameRequestContext* context,
                                           string                   uri,
                                           const LspDocument*       source_doc)
{
    NerdSource source = {
        .source      = source_doc->source,
        .source_path = uri,
    };
    if (!lex(source, &context->scratch_lexer)) {
        return false;
    }

    context->scratch_ast = ast_parse(&context->scratch_lexer);
    if (array_count(context->scratch_ast.nodes) == 0) {
        ast_done(&context->scratch_ast);
        lex_done(&context->scratch_lexer);
        context->scratch_ast   = (Ast){0};
        context->scratch_lexer = (Lexer){0};
        return false;
    }

    context->scratch_doc = (LspDocument){
        .source        = source_doc->source,
        .front_end     = {.lexer = context->scratch_lexer,
                          .ast   = context->scratch_ast,
                          .readiness =
                              {
                                  .lexer = FRONT_END_PRODUCT_Complete,
                                  .ast   = FRONT_END_PRODUCT_Complete,
                                  .sema  = FRONT_END_PRODUCT_Missing,
                                  .hir   = FRONT_END_PRODUCT_Missing,
                              }},
        .analysis_ok   = false,
        .source_ready  = true,
        .tokens_ready  = true,
        .syntax_ready  = true,
        .sema_partial  = false,
        .sema_complete = false,
        .cst_ready     = false,
    };
    context->doc         = &context->scratch_doc;
    context->has_scratch = true;
    return true;
}

internal bool lsp_rename_get_context(LspState*                state,
                                     const LspMessage*        message,
                                     LspRenameRequestContext* context)
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

    context->uri       = json_string(uri_value);
    LspSourceView view = {0};
    if (!lsp_source_view(state, context->uri, &view)) {
        return false;
    }
    context->doc = view.doc;

    if (array_count(context->doc->front_end.lexer.tokens) == 0 ||
        array_count(context->doc->front_end.ast.nodes) == 0) {
        if (!lsp_rename_parse_scratch_doc(
                context, context->uri, context->doc)) {
            return false;
        }
    }

    u32   line          = (u32)json_integer(line_value);
    u32   col           = (u32)json_integer(col_value);
    usize offset        = 0;
    usize visible_start = 0;

    if (lsp_rename_document_visible_start(context->doc, &visible_start) &&
        !string_eq(context->doc->front_end.lexer.source.source,
                   context->doc->source)) {
        NerdSource visible_source = {
            .source      = context->doc->source,
            .source_path = context->doc->front_end.lexer.source.source_path,
        };
        usize visible_offset = 0;
        if (!lex_line_col_to_offset(
                visible_source, line, col, &visible_offset)) {
            return false;
        }
        offset = visible_start + visible_offset;
    } else if (!lex_line_col_to_offset(
                   context->doc->front_end.lexer.source, line, col, &offset)) {
        return false;
    }

    u32    token_end = 0;
    Token* token = lex_find(&context->doc->front_end.lexer, offset, &token_end);
    if (!token || token->kind != TK_Symbol) {
        return false;
    }

    context->token_index = lsp_rename_token_index_from_pointer(
        &context->doc->front_end.lexer, token);
    return true;
}

//------------------------------------------------------------------------------

internal u32 lsp_rename_find_bind_node_at_token(const Ast* ast, u32 token_index)
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

internal u32 lsp_rename_find_symbol_ref_node_at_token(const Ast* ast,
                                                      u32        token_index)
{
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind == AK_SymbolRef && node->token_index == token_index) {
            return i;
        }
    }
    return U32_MAX;
}

internal u32 lsp_rename_find_decl_by_symbol(const Sema* sema, u32 symbol_handle)
{
    for (u32 i = 0; i < array_count(sema->decls); ++i) {
        if (sema->decls[i].symbol_handle == symbol_handle) {
            return i;
        }
    }
    return sema_no_decl();
}

internal bool lsp_rename_decl_origin(const ProgramInfo* program,
                                     u32                current_module_index,
                                     const SemaDecl*    decl,
                                     cstr*              out_path,
                                     string*            out_name)
{
    if (!program || !decl || !out_path || !out_name) {
        return false;
    }

    u32 origin_module_index = current_module_index;
    u32 origin_decl_index   = U32_MAX;

    if (decl->import_module_index != sema_no_decl()) {
        origin_module_index = decl->import_module_index;
        origin_decl_index   = decl->import_decl_index;
    }

    LspModuleView module = {0};
    if (!lsp_program_module_view(program, origin_module_index, &module)) {
        return false;
    }

    const SemaDecl* origin_decl = decl;
    if (origin_decl_index != U32_MAX &&
        !lsp_sema_decl(module.sema, origin_decl_index, &origin_decl)) {
        return false;
    }

    if (!module.info->resolved_path) {
        return false;
    }

    *out_path = module.info->resolved_path;
    *out_name = lex_symbol(module.lexer, origin_decl->symbol_handle);
    return true;
}

internal bool lsp_rename_decl_matches(const ProgramInfo* program,
                                      u32                current_module_index,
                                      const SemaDecl*    decl,
                                      const LspRenameTarget* target)
{
    if (!target || target->kind != LSP_RENAME_DECL || !target->origin_path) {
        return false;
    }

    cstr   origin_path = NULL;
    string name        = {0};
    return lsp_rename_decl_origin(
               program, current_module_index, decl, &origin_path, &name) &&
           origin_path && strcmp(origin_path, target->origin_path) == 0 &&
           string_eq(name, target->name);
}

internal u32 lsp_rename_symbol_handle_at_token(const Lexer* lexer,
                                               u32          token_index)
{
    u32 symbol_index = 0;
    for (u32 i = 0; i < array_count(lexer->tokens); ++i) {
        if (lexer->tokens[i].kind != TK_Symbol) {
            continue;
        }
        if (i == token_index) {
            if (symbol_index < array_count(lexer->symbol_handles)) {
                return lexer->symbol_handles[symbol_index];
            }
            return U32_MAX;
        }
        symbol_index++;
    }
    return U32_MAX;
}

internal void lsp_rename_add_token(Array(u32) * tokens, u32 token_index);

internal void lsp_rename_collect_ast_symbol_tokens(const LspDocument* doc,
                                                   u32                symbol,
                                                   Array(u32) * tokens)
{
    const Ast* ast = &doc->front_end.ast;

    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        switch (node->kind) {
        case AK_Bind:
        case AK_Variable:
        case AK_SymbolRef:
            if (node->a == symbol) {
                lsp_rename_add_token(tokens, node->token_index);
            }
            break;
        default:
            break;
        }
    }

    for (u32 i = 0; i < array_count(ast->params); ++i) {
        const AstParam* param = &ast->params[i];
        if (param->symbol_handle == symbol) {
            lsp_rename_add_token(tokens, param->token_index);
        }
    }

    for (u32 i = 0; i < array_count(ast->patterns); ++i) {
        const AstPattern* pattern = &ast->patterns[i];
        if (pattern->kind == APK_Bind && pattern->a == symbol) {
            lsp_rename_add_token(tokens, pattern->token_index);
        }
    }

    for (u32 i = 0; i < array_count(ast->fors); ++i) {
        const AstForInfo* for_info = &ast->fors[i];
        if (for_info->index_symbol == symbol) {
            lsp_rename_add_token(tokens, for_info->index_token_index);
        }
        if (for_info->item_symbol == symbol) {
            lsp_rename_add_token(tokens, for_info->item_token_index);
        }
    }

    for (u32 i = 0; i < array_count(ast->on_branches); ++i) {
        const AstOnBranch* branch = &ast->on_branches[i];
        if (branch->binder_symbol_handle == symbol) {
            lsp_rename_add_token(tokens, branch->binder_token_index);
        }
    }
}

internal bool lsp_rename_token_list_contains(Array(u32) tokens, u32 token_index)
{
    for (u32 i = 0; i < array_count(tokens); ++i) {
        if (tokens[i] == token_index) {
            return true;
        }
    }
    return false;
}

internal bool lsp_rename_member_in_type(const ProgramInfo* program,
                                        LspModuleView      view,
                                        u32                type,
                                        string             name,
                                        u32                depth,
                                        LspRenameTarget*   target);

internal bool lsp_rename_member_declaration(LspModuleView    view,
                                            u32              owner,
                                            u32              token,
                                            u32              symbol,
                                            LspRenameTarget* target)
{
    if (!view.info || !view.info->resolved_path ||
        token >= array_count(view.lexer->tokens)) {
        return false;
    }
    *target = (LspRenameTarget){
        .kind          = LSP_RENAME_MEMBER,
        .index         = token,
        .token_index   = token,
        .name          = lex_symbol(view.lexer, symbol),
        .origin_path   = view.info->resolved_path,
        .member_offset = view.lexer->tokens[token].offset,
        .owner_lexer   = view.lexer,
        .owner_node    = owner,
    };
    return true;
}

internal bool lsp_rename_member_in_node(const ProgramInfo* program,
                                        LspModuleView      view,
                                        u32                owner,
                                        string             name,
                                        u32                depth,
                                        LspRenameTarget*   target)
{
    if (owner >= array_count(view.ast->nodes) || depth > 32) {
        return false;
    }
    const AstNode* node = &view.ast->nodes[owner];
    if (node->kind == AK_TypeEnum) {
        const AstEnumTypeInfo* info = &view.ast->enum_types[node->a];
        for (u32 i = 0; i < info->variant_count; ++i) {
            const AstEnumVariant* variant =
                &view.ast->enum_variants[info->first_variant + i];
            if (string_eq(lex_symbol(view.lexer, variant->symbol_handle),
                          name)) {
                return lsp_rename_member_declaration(view,
                                                     owner,
                                                     variant->token_index,
                                                     variant->symbol_handle,
                                                     target);
            }
        }
    } else if (node->kind == AK_TypePlex) {
        const AstPlexTypeInfo* info = &view.ast->plex_types[node->a];
        for (u32 i = 0; i < info->field_count; ++i) {
            const AstPlexField* field =
                &view.ast->plex_fields[info->first_field + i];
            if (field->symbol_handle != U32_MAX &&
                string_eq(lex_symbol(view.lexer, field->symbol_handle), name)) {
                return lsp_rename_member_declaration(view,
                                                     owner,
                                                     field->token_index,
                                                     field->symbol_handle,
                                                     target);
            }
            for (u32 j = 0;
                 field->bit_field_group && j < field->bit_field_count;
                 ++j) {
                const AstPlexBitField* bit =
                    &view.ast->plex_bit_fields[field->first_bit_field + j];
                if (bit->symbol_handle != U32_MAX &&
                    string_eq(lex_symbol(view.lexer, bit->symbol_handle),
                              name)) {
                    return lsp_rename_member_declaration(view,
                                                         owner,
                                                         bit->token_index,
                                                         bit->symbol_handle,
                                                         target);
                }
            }
            if (field->embedded &&
                field->type_node_index <
                    array_count(view.sema->node_type_indices) &&
                lsp_rename_member_in_type(
                    program,
                    view,
                    view.sema->node_type_indices[field->type_node_index],
                    name,
                    depth + 1,
                    target)) {
                return true;
            }
        }
    }
    return false;
}

internal bool lsp_rename_member_in_type(const ProgramInfo* program,
                                        LspModuleView      view,
                                        u32                type,
                                        string             name,
                                        u32                depth,
                                        LspRenameTarget*   target)
{
    if (depth > 32 || type >= array_count(view.sema->types)) {
        return false;
    }
    SemaType st = view.sema->types[type];
    if (st.kind == STK_Pointer || st.kind == STK_Box) {
        return lsp_rename_member_in_type(
            program, view, st.first_param_type, name, depth + 1, target);
    }
    if (st.kind != STK_Plex && st.kind != STK_Union && st.kind != STK_Enum) {
        return false;
    }

    // Follow imported record identity rather than comparing field spellings
    // or structural layouts from unrelated declarations.
    for (u32 i = 0; i < array_count(view.sema->imported_records); ++i) {
        const SemaImportedRecord* record = &view.sema->imported_records[i];
        if (record->destination_type != type) {
            continue;
        }
        for (u32 m = 0; m < array_count(program->modules); ++m) {
            LspModuleView origin = {0};
            if (lsp_program_module_view(program, m, &origin) &&
                origin.sema == record->source) {
                return lsp_rename_member_in_type(program,
                                                 origin,
                                                 record->source_type,
                                                 name,
                                                 depth + 1,
                                                 target);
            }
        }
    }
    // Named record aliases retain their declaration identity even when their
    // underlying structural types have identical fields.
    for (u32 i = 0; i < array_count(view.sema->decls); ++i) {
        const SemaDecl* decl = &view.sema->decls[i];
        if (decl->type_index == type &&
            decl->import_module_index != sema_no_decl()) {
            LspModuleView origin = {0};
            if (lsp_program_module_view(
                    program, decl->import_module_index, &origin) &&
                decl->import_decl_index < array_count(origin.sema->decls)) {
                const SemaDecl* original =
                    &origin.sema->decls[decl->import_decl_index];
                if (lsp_rename_member_in_type(program,
                                              origin,
                                              original->type_index,
                                              name,
                                              depth + 1,
                                              target)) {
                    return true;
                }
            }
        }
        if (decl->kind == SK_TypeAlias && decl->type_index == type &&
            decl->import_module_index == sema_no_decl() &&
            lsp_rename_member_in_node(program,
                                      view,
                                      decl->value_node_index,
                                      name,
                                      depth + 1,
                                      target)) {
            return true;
        }
    }
    // A generic application identifies its template even when this instance's
    // concrete element types differ from those of the most recent instance.
    for (u32 i = 0; i < array_count(view.ast->nodes); ++i) {
        const AstNode* node = &view.ast->nodes[i];
        if ((node->kind != AK_TypeApply && node->kind != AK_Index) ||
            i >= array_count(view.sema->node_type_indices) ||
            view.sema->node_type_indices[i] != type) {
            continue;
        }
        u32 base = node->kind == AK_TypeApply
                       ? view.ast->type_applications[node->a].target_node_index
                       : node->a;
        u32 decl_index = sema_no_decl();
        if (!lsp_sema_node_decl(view.sema, base, &decl_index) &&
            base < array_count(view.ast->nodes) &&
            view.ast->nodes[base].kind == AK_SymbolRef) {
            decl_index = lsp_rename_find_decl_by_symbol(
                view.sema, view.ast->nodes[base].a);
        }
        if (decl_index >= array_count(view.sema->decls)) {
            continue;
        }
        const SemaDecl* decl  = &view.sema->decls[decl_index];
        LspModuleView   owner = view;
        if (decl->import_module_index != sema_no_decl()) {
            if (!lsp_program_module_view(
                    program, decl->import_module_index, &owner) ||
                decl->import_decl_index >= array_count(owner.sema->decls)) {
                continue;
            }
            decl = &owner.sema->decls[decl->import_decl_index];
        }
        if (lsp_rename_member_in_node(program,
                                      owner,
                                      decl->value_node_index,
                                      name,
                                      depth + 1,
                                      target)) {
            return true;
        }
    }
    u32 owner = U32_MAX;
    for (u32 i = 0; i < array_count(view.ast->nodes); ++i) {
        if ((view.ast->nodes[i].kind == AK_TypePlex ||
             view.ast->nodes[i].kind == AK_TypeEnum) &&
            i < array_count(view.sema->node_type_indices) &&
            view.sema->node_type_indices[i] == type) {
            if (owner != U32_MAX) {
                return false;
            }
            owner = i;
        }
    }
    return owner != U32_MAX &&
           lsp_rename_member_in_node(
               program, view, owner, name, depth + 1, target);
}

internal LspModuleView lsp_rename_document_module(const LspDocument* doc)
{
    LspModuleView view = {.lexer = &doc->front_end.lexer,
                          .ast   = &doc->front_end.ast,
                          .sema  = &doc->front_end.sema};
    for (u32 m = 0; m < array_count(doc->program.modules); ++m) {
        LspModuleView candidate = {0};
        if (lsp_program_module_view(&doc->program, m, &candidate) &&
            candidate.lexer->tokens == view.lexer->tokens) {
            view.info         = candidate.info;
            view.module_index = m;
            break;
        }
    }
    return view;
}

internal bool lsp_rename_member_type_expression(const ProgramInfo* program,
                                                LspModuleView      view,
                                                u32                node_index,
                                                string             name,
                                                u32                depth,
                                                LspRenameTarget*   target)
{
    if (depth > 32 || node_index >= array_count(view.ast->nodes)) {
        return false;
    }
    const AstNode* node = &view.ast->nodes[node_index];
    if (node->kind == AK_Expression || node->kind == AK_TypePointer) {
        return lsp_rename_member_type_expression(
            program, view, node->a, name, depth + 1, target);
    }
    if (node->kind == AK_TypeApply || node->kind == AK_Index) {
        u32 base = node->kind == AK_TypeApply
                       ? view.ast->type_applications[node->a].target_node_index
                       : node->a;
        return lsp_rename_member_type_expression(
            program, view, base, name, depth + 1, target);
    }
    if (node->kind == AK_SymbolRef || node->kind == AK_Field) {
        u32 decl_index = sema_no_decl();
        if (!lsp_sema_node_decl(view.sema, node_index, &decl_index) &&
            node->kind == AK_SymbolRef) {
            decl_index = lsp_rename_find_decl_by_symbol(view.sema, node->a);
        }
        if (decl_index < array_count(view.sema->decls)) {
            const SemaDecl* decl   = &view.sema->decls[decl_index];
            LspModuleView   origin = view;
            if (decl->import_module_index != sema_no_decl()) {
                if (!lsp_program_module_view(
                        program, decl->import_module_index, &origin) ||
                    decl->import_decl_index >=
                        array_count(origin.sema->decls)) {
                    return false;
                }
                decl = &origin.sema->decls[decl->import_decl_index];
            }
            if ((decl->kind == SK_TypeAlias ||
                 decl->kind == SK_GenericTypeAlias) &&
                decl->value_node_index != node_index &&
                lsp_rename_member_type_expression(program,
                                                  origin,
                                                  decl->value_node_index,
                                                  name,
                                                  depth + 1,
                                                  target)) {
                return true;
            }
        }
    }
    if (lsp_rename_member_in_node(
            program, view, node_index, name, depth + 1, target)) {
        return true;
    }
    u32 type = sema_no_type();
    return lsp_sema_node_type(view.sema, node_index, &type) &&
           lsp_rename_member_in_type(
               program, view, type, name, depth + 1, target);
}

// Follow annotations and constructor syntax before canonical semantic types:
// materialisation can intern two separately named records to the same layout.
internal bool lsp_rename_member_expression(const ProgramInfo* program,
                                           LspModuleView      view,
                                           u32                node_index,
                                           string             name,
                                           u32                depth,
                                           LspRenameTarget*   target)
{
    if (depth > 32 || node_index >= array_count(view.ast->nodes)) {
        return false;
    }
    const AstNode* node = &view.ast->nodes[node_index];
    if (node->kind == AK_Expression || node->kind == AK_Deref ||
        node->kind == AK_AddressOf) {
        return lsp_rename_member_expression(
            program, view, node->a, name, depth + 1, target);
    }
    if (node->kind == AK_Plex || node->kind == AK_PlexUpdate) {
        const AstPlexLiteralInfo* literal = &view.ast->plex_literals[node->a];
        if (literal->target_node_index != U32_MAX) {
            bool found =
                node->kind == AK_PlexUpdate
                    ? lsp_rename_member_expression(program,
                                                   view,
                                                   literal->target_node_index,
                                                   name,
                                                   depth + 1,
                                                   target)
                    : lsp_rename_member_type_expression(
                          program,
                          view,
                          literal->target_node_index,
                          name,
                          depth + 1,
                          target);
            if (found) {
                return true;
            }
        }
    }
    if (node->kind == AK_SymbolRef) {
        u32 local_index = sema_no_local();
        if (lsp_sema_node_local(view.sema, node_index, &local_index) &&
            local_index < array_count(view.sema->locals)) {
            const SemaLocal* local = &view.sema->locals[local_index];
            if (lsp_rename_member_type_expression(program,
                                                  view,
                                                  local->type_node_index,
                                                  name,
                                                  depth + 1,
                                                  target)) {
                return true;
            }
            if (local->value_node_index != node_index &&
                lsp_rename_member_expression(program,
                                             view,
                                             local->value_node_index,
                                             name,
                                             depth + 1,
                                             target)) {
                return true;
            }
            if (lsp_rename_member_in_type(program,
                                          view,
                                          local->type_index,
                                          name,
                                          depth + 1,
                                          target)) {
                return true;
            }
        } else if (lsp_rename_member_type_expression(
                       program, view, node_index, name, depth + 1, target)) {
            return true;
        }
    }
    u32 type = sema_no_type();
    return lsp_sema_node_type(view.sema, node_index, &type) &&
           lsp_rename_member_in_type(
               program, view, type, name, depth + 1, target);
}

internal bool lsp_rename_member_pattern(const LspDocument* doc,
                                        LspModuleView      view,
                                        u32                token,
                                        string             name,
                                        LspRenameTarget*   target,
                                        bool*              shorthand)
{
    const Ast*  ast  = view.ast;
    const Sema* sema = view.sema;
    for (u32 i = 0; i < array_count(ast->patterns); ++i) {
        const AstPattern* pattern = &ast->patterns[i];
        u32               type    = i < array_count(sema->pattern_type_indices)
                                        ? sema->pattern_type_indices[i]
                                        : sema_no_type();
        bool              matches = pattern->kind == APK_Value &&
                                    pattern->token_index == token &&
                                    type < array_count(sema->types) &&
                                    sema->types[type].kind == STK_Enum;
        if (matches) {
            u32 value = pattern->a;
            while (value < array_count(ast->nodes) &&
                   ast->nodes[value].kind == AK_Expression) {
                value = ast->nodes[value].a;
            }
            u32 local, decl;
            if (lsp_sema_node_local(sema, value, &local) ||
                lsp_sema_node_decl(sema, value, &decl)) {
                matches = false;
            }
        }
        if (pattern->kind == APK_EnumVariant) {
            matches = ast->enum_patterns[pattern->a].token_index == token;
        } else if (pattern->kind == APK_Plex) {
            for (u32 j = 0; j < pattern->b; ++j) {
                const AstPlexPatternField* field =
                    &ast->pattern_fields[pattern->a + j];
                if (field->token_index == token) {
                    matches = true;
                    *shorthand =
                        ast->patterns[field->pattern_index].kind == APK_Bind &&
                        ast->patterns[field->pattern_index].token_index ==
                            token;
                }
            }
        }
        if (matches) {
            if (pattern->kind == APK_EnumVariant &&
                lsp_rename_member_type_expression(
                    &doc->program,
                    view,
                    ast->enum_patterns[pattern->a].qualifier_node_index,
                    name,
                    0,
                    target)) {
                return true;
            }
            if (pattern->kind == APK_Plex && pattern->c > 0 &&
                lsp_rename_member_type_expression(
                    &doc->program, view, pattern->c - 1, name, 0, target)) {
                return true;
            }
            for (u32 n = 0; n < array_count(ast->nodes); ++n) {
                if (ast->nodes[n].kind != AK_On) {
                    continue;
                }
                const AstOnInfo* on = &ast->ons[ast->nodes[n].b];
                for (u32 b = 0; b < on->branch_count; ++b) {
                    const AstOnBranch* branch =
                        &ast->on_branches[on->first_branch + b];
                    for (u32 p = 0; p < branch->pattern_count; ++p) {
                        if (ast->pattern_items[branch->pattern_index + p] ==
                                i &&
                            lsp_rename_member_expression(&doc->program,
                                                         view,
                                                         ast->nodes[n].a,
                                                         name,
                                                         0,
                                                         target)) {
                            return true;
                        }
                    }
                }
            }
            if (lsp_rename_member_in_type(
                    &doc->program, view, type, name, 0, target)) {
                return true;
            }
        }
    }
    return false;
}

internal bool lsp_rename_member_at_token(const LspDocument* doc,
                                         u32                token,
                                         LspRenameTarget*   target,
                                         bool*              shorthand)
{
    LspModuleView view  = lsp_rename_document_module(doc);
    const Ast*    ast   = view.ast;
    const Sema*   sema  = view.sema;
    const Lexer*  lexer = view.lexer;
    if (token >= array_count(lexer->tokens) ||
        lexer->tokens[token].kind != TK_Symbol) {
        return false;
    }
    string name =
        lex_symbol(lexer, lsp_rename_symbol_handle_at_token(lexer, token));
    *shorthand = false;
    // Declarations do not have an expression node for each member token.
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode*  node      = &ast->nodes[i];
        LspRenameTarget candidate = {0};
        if ((node->kind == AK_TypePlex || node->kind == AK_TypeEnum) &&
            lsp_rename_member_in_node(
                &doc->program, view, i, name, 0, &candidate) &&
            candidate.owner_lexer == lexer && candidate.index == token) {
            *target = candidate;
            return true;
        }
    }
    if (lsp_rename_member_pattern(doc, view, token, name, target, shorthand)) {
        return true;
    }
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        u32            type = sema_no_type();
        if (node->kind == AK_Field && node->token_index == token) {
            if (lsp_rename_member_expression(
                    &doc->program, view, node->a, name, 0, target)) {
                return true;
            }
            lsp_sema_node_type(sema, node->a, &type);
        } else if ((node->kind == AK_SymbolRef ||
                    node->kind == AK_EnumVariant) &&
                   node->token_index == token) {
            u32 local, decl;
            if (lsp_sema_node_local(sema, i, &local) ||
                lsp_sema_node_decl(sema, i, &decl)) {
                continue;
            }
            for (u32 l = 0; l < array_count(sema->locals); ++l) {
                u32 value = sema->locals[l].value_node_index;
                while (value < array_count(ast->nodes) &&
                       ast->nodes[value].kind == AK_Expression) {
                    value = ast->nodes[value].a;
                }
                if (value == i && lsp_rename_member_type_expression(
                                      &doc->program,
                                      view,
                                      sema->locals[l].type_node_index,
                                      name,
                                      0,
                                      target)) {
                    return true;
                }
            }
            lsp_sema_node_type(sema, i, &type);
            if (type >= array_count(sema->types) ||
                sema->types[type].kind != STK_Enum) {
                continue;
            }
        } else if (node->kind == AK_Plex || node->kind == AK_PlexUpdate) {
            const AstPlexLiteralInfo* literal = &ast->plex_literals[node->a];
            for (u32 j = 0; j < literal->field_count; ++j) {
                const AstPlexLiteralField* field =
                    &ast->plex_literal_fields[literal->first_field + j];
                if (field->token_index != token) {
                    continue;
                }
                lsp_sema_node_type(sema, i, &type);
                *shorthand =
                    field->value_node_index < array_count(ast->nodes) &&
                    ast->nodes[field->value_node_index].kind == AK_SymbolRef &&
                    ast->nodes[field->value_node_index].token_index == token;
                if (lsp_rename_member_expression(
                        &doc->program, view, i, name, 0, target)) {
                    return true;
                }
                break;
            }
        }
        if (type < array_count(sema->types) &&
            lsp_rename_member_in_type(
                &doc->program, view, type, name, 0, target)) {
            return true;
        }
    }
    return false;
}

internal bool lsp_rename_target_from_token(const LspDocument* doc,
                                           u32                token_index,
                                           LspRenameTarget*   out_target)
{
    const Ast*   ast   = &doc->front_end.ast;
    const Sema*  sema  = &doc->front_end.sema;
    const Lexer* lexer = &doc->front_end.lexer;

    if (token_index >= array_count(lexer->tokens) ||
        lexer->tokens[token_index].kind != TK_Symbol) {
        return false;
    }

    bool shorthand = false;
    if (lsp_rename_member_at_token(doc, token_index, out_target, &shorthand)) {
        out_target->token_index = token_index;
        return true;
    }

    for (u32 i = 0; i < array_count(sema->locals); ++i) {
        const SemaLocal* local = &sema->locals[i];
        if (local->decl_token_index == token_index) {
            *out_target = (LspRenameTarget){
                .kind        = LSP_RENAME_LOCAL,
                .index       = i,
                .token_index = token_index,
                .name        = lex_symbol(lexer, local->symbol_handle),
            };
            return true;
        }
    }

    u32 bind_node_index = lsp_rename_find_bind_node_at_token(ast, token_index);
    if (bind_node_index != U32_MAX) {
        u32              local_index = sema_no_local();
        const SemaLocal* local       = NULL;
        if (lsp_sema_node_local(sema, bind_node_index, &local_index) &&
            lsp_sema_local(sema, local_index, &local)) {
            *out_target = (LspRenameTarget){
                .kind        = LSP_RENAME_LOCAL,
                .index       = local_index,
                .token_index = token_index,
                .name        = lex_symbol(lexer, local->symbol_handle),
            };
            return true;
        }

        u32 decl_index =
            lsp_rename_find_decl_by_symbol(sema, ast->nodes[bind_node_index].a);
        const SemaDecl* decl = NULL;
        if (lsp_sema_decl(sema, decl_index, &decl) &&
            decl->bind_node_index != sema_no_decl()) {
            cstr   origin_path = NULL;
            string origin_name = {0};
            if (!lsp_rename_decl_origin(&doc->program,
                                        doc->program.root_module_index,
                                        decl,
                                        &origin_path,
                                        &origin_name)) {
                return false;
            }
            *out_target = (LspRenameTarget){
                .kind        = LSP_RENAME_DECL,
                .index       = decl_index,
                .token_index = token_index,
                .name        = origin_name,
                .origin_path = origin_path,
            };
            return true;
        }
    }

    u32 ref_node_index =
        lsp_rename_find_symbol_ref_node_at_token(ast, token_index);
    if (ref_node_index != U32_MAX) {
        u32              local_index = sema_no_local();
        const SemaLocal* local       = NULL;
        if (lsp_sema_node_local(sema, ref_node_index, &local_index) &&
            lsp_sema_local(sema, local_index, &local)) {
            *out_target = (LspRenameTarget){
                .kind        = LSP_RENAME_LOCAL,
                .index       = local_index,
                .token_index = token_index,
                .name        = lex_symbol(lexer, local->symbol_handle),
            };
            return true;
        }

        u32             decl_index = sema_no_decl();
        const SemaDecl* decl       = NULL;
        if (lsp_sema_node_decl(sema, ref_node_index, &decl_index) &&
            lsp_sema_decl(sema, decl_index, &decl)) {
            cstr   origin_path = NULL;
            string origin_name = {0};
            if (!lsp_rename_decl_origin(&doc->program,
                                        doc->program.root_module_index,
                                        decl,
                                        &origin_path,
                                        &origin_name)) {
                return false;
            }
            *out_target = (LspRenameTarget){
                .kind        = LSP_RENAME_DECL,
                .index       = decl_index,
                .token_index = token_index,
                .name        = origin_name,
                .origin_path = origin_path,
            };
            return true;
        }
    }

    u32 symbol = lsp_rename_symbol_handle_at_token(lexer, token_index);
    if (symbol == U32_MAX) {
        return false;
    }
    Array(u32) tokens = NULL;
    lsp_rename_collect_ast_symbol_tokens(doc, symbol, &tokens);
    if (lsp_rename_token_list_contains(tokens, token_index)) {
        *out_target = (LspRenameTarget){
            .kind        = LSP_RENAME_SYMBOL,
            .index       = symbol,
            .token_index = token_index,
            .name        = lex_symbol(lexer, symbol),
        };
        array_free(tokens);
        return true;
    }
    array_free(tokens);
    return false;
}

//------------------------------------------------------------------------------

internal void lsp_rename_add_token(Array(u32) * tokens, u32 token_index)
{
    if (token_index == U32_MAX) {
        return;
    }
    for (u32 i = 0; i < array_count(*tokens); ++i) {
        if ((*tokens)[i] == token_index) {
            return;
        }
    }

    u32 insert_at = (u32)array_count(*tokens);
    while (insert_at > 0 && (*tokens)[insert_at - 1] > token_index) {
        insert_at--;
    }
    array_push(*tokens, token_index);
    for (u32 i = (u32)array_count(*tokens) - 1; i > insert_at; --i) {
        (*tokens)[i] = (*tokens)[i - 1];
    }
    (*tokens)[insert_at] = token_index;
}

internal Array(u32)
    lsp_rename_collect_tokens(const LspDocument* doc, LspRenameTarget target)
{
    Array(u32) tokens = NULL;
    const Ast*  ast   = &doc->front_end.ast;
    const Sema* sema  = &doc->front_end.sema;

    if (target.kind == LSP_RENAME_LOCAL) {
        const SemaLocal* local = NULL;
        if (!lsp_sema_local(sema, target.index, &local)) {
            return tokens;
        }

        lsp_rename_add_token(&tokens, local->decl_token_index);
        if (local->decl_node_index < array_count(ast->nodes)) {
            lsp_rename_add_token(
                &tokens, ast->nodes[local->decl_node_index].token_index);
        }

        for (u32 i = 0; i < array_count(ast->nodes); ++i) {
            u32 local_index = sema_no_local();
            if (lsp_sema_node_local(sema, i, &local_index) &&
                local_index == target.index &&
                ast->nodes[i].token_index != U32_MAX) {
                lsp_rename_add_token(&tokens, ast->nodes[i].token_index);
            }
        }
        return tokens;
    }

    if (target.kind == LSP_RENAME_DECL) {
        const SemaDecl* decl = NULL;
        if (!lsp_sema_decl(sema, target.index, &decl)) {
            return tokens;
        }

        if (decl->bind_node_index < array_count(ast->nodes)) {
            lsp_rename_add_token(&tokens,
                                 ast->nodes[decl->bind_node_index].token_index);
        }

        for (u32 i = 0; i < array_count(ast->nodes); ++i) {
            u32 decl_index = sema_no_decl();
            if (lsp_sema_node_decl(sema, i, &decl_index) &&
                decl_index == target.index &&
                ast->nodes[i].token_index != U32_MAX) {
                lsp_rename_add_token(&tokens, ast->nodes[i].token_index);
            }
        }
        return tokens;
    }

    if (target.kind == LSP_RENAME_SYMBOL) {
        lsp_rename_collect_ast_symbol_tokens(doc, target.index, &tokens);
        return tokens;
    }

    return tokens;
}

internal bool lsp_rename_token_is_declaration(const LspDocument* doc,
                                              LspRenameTarget    target,
                                              u32                token_index)
{
    const Ast*  ast  = &doc->front_end.ast;
    const Sema* sema = &doc->front_end.sema;

    if (target.kind == LSP_RENAME_LOCAL) {
        const SemaLocal* local = NULL;
        if (!lsp_sema_local(sema, target.index, &local)) {
            return false;
        }
        if (local->decl_token_index == token_index) {
            return true;
        }
        return local->decl_node_index < array_count(ast->nodes) &&
               ast->nodes[local->decl_node_index].token_index == token_index;
    }

    if (target.kind == LSP_RENAME_DECL) {
        const SemaDecl* decl = NULL;
        return lsp_sema_decl(sema, target.index, &decl) &&
               decl->bind_node_index < array_count(ast->nodes) &&
               ast->nodes[decl->bind_node_index].token_index == token_index;
    }

    if (target.kind == LSP_RENAME_SYMBOL) {
        for (u32 i = 0; i < array_count(ast->nodes); ++i) {
            const AstNode* node = &ast->nodes[i];
            if ((node->kind == AK_Bind || node->kind == AK_Variable) &&
                node->a == target.index && node->token_index == token_index) {
                return true;
            }
        }
        for (u32 i = 0; i < array_count(ast->params); ++i) {
            const AstParam* param = &ast->params[i];
            if (param->symbol_handle == target.index &&
                param->token_index == token_index) {
                return true;
            }
        }
        for (u32 i = 0; i < array_count(ast->patterns); ++i) {
            const AstPattern* pattern = &ast->patterns[i];
            if (pattern->kind == APK_Bind && pattern->a == target.index &&
                pattern->token_index == token_index) {
                return true;
            }
        }
        for (u32 i = 0; i < array_count(ast->fors); ++i) {
            const AstForInfo* for_info = &ast->fors[i];
            if ((for_info->index_symbol == target.index &&
                 for_info->index_token_index == token_index) ||
                (for_info->item_symbol == target.index &&
                 for_info->item_token_index == token_index)) {
                return true;
            }
        }
        for (u32 i = 0; i < array_count(ast->on_branches); ++i) {
            const AstOnBranch* branch = &ast->on_branches[i];
            if (branch->binder_symbol_handle == target.index &&
                branch->binder_token_index == token_index) {
                return true;
            }
        }
    }

    return false;
}

internal void lsp_rename_add_occurrence(Array(LspRenameOccurrence) *
                                            occurrences,
                                        string       uri,
                                        const Lexer* lexer,
                                        u32          token_index,
                                        bool         is_declaration)
{
    if (!lexer || token_index == U32_MAX) {
        return;
    }

    usize start = 0;
    usize end   = 0;
    lsp_rename_token_offsets(lexer, token_index, &start, &end);

    for (u32 i = 0; i < array_count(*occurrences); ++i) {
        LspRenameOccurrence* occurrence       = &(*occurrences)[i];
        usize                occurrence_start = 0;
        usize                occurrence_end   = 0;
        lsp_rename_token_offsets(occurrence->lexer,
                                 occurrence->token_index,
                                 &occurrence_start,
                                 &occurrence_end);
        if (string_eq(occurrence->uri, uri) && occurrence_start == start &&
            occurrence_end == end) {
            occurrence->is_declaration =
                occurrence->is_declaration || is_declaration;
            return;
        }
    }

    array_push(*occurrences,
               ((LspRenameOccurrence){
                   .uri            = uri,
                   .lexer          = lexer,
                   .token_index    = token_index,
                   .is_declaration = is_declaration,
               }));
}

internal void lsp_rename_collect_module_decl_occurrences(
    Array(LspRenameOccurrence) * occurrences,
    Arena*                 arena,
    const ProgramInfo*     program,
    u32                    module_index,
    string                 uri,
    const LspRenameTarget* target)
{
    LspModuleView module = {0};
    if (!lsp_program_module_view(program, module_index, &module)) {
        return;
    }

    const Ast*   ast   = module.ast;
    const Sema*  sema  = module.sema;
    const Lexer* lexer = module.lexer;
    if (uri.count == 0) {
        uri = lsp_rename_path_to_uri(arena, module.info->resolved_path);
    }

    if (target->kind == LSP_RENAME_MEMBER) {
        LspDocument member_doc     = {.source  = lexer->source.source,
                                      .program = *program};
        member_doc.front_end.lexer = *lexer;
        member_doc.front_end.ast   = *ast;
        member_doc.front_end.sema  = *sema;
        for (u32 t = 0; t < array_count(lexer->tokens); ++t) {
            if (lexer->tokens[t].kind != TK_Symbol ||
                !string_eq(
                    lex_symbol(lexer,
                               lsp_rename_symbol_handle_at_token(lexer, t)),
                    target->name)) {
                continue;
            }
            LspRenameTarget member    = {0};
            bool            shorthand = false;
            if (!lsp_rename_member_at_token(
                    &member_doc, t, &member, &shorthand) ||
                member.member_offset != target->member_offset ||
                strcmp(member.origin_path, target->origin_path) != 0) {
                continue;
            }
            bool declaration =
                strcmp(module.info->resolved_path, target->origin_path) == 0 &&
                lexer->tokens[t].offset == target->member_offset;
            lsp_rename_add_occurrence(occurrences, uri, lexer, t, declaration);
            if (shorthand) {
                for (u32 i = 0; i < array_count(*occurrences); ++i) {
                    if ((*occurrences)[i].lexer == lexer &&
                        (*occurrences)[i].token_index == t) {
                        (*occurrences)[i].shorthand_name = target->name;
                    }
                }
            }
        }
        return;
    }

    for (u32 i = 0; i < array_count(sema->decls); ++i) {
        const SemaDecl* decl = &sema->decls[i];
        if (!lsp_rename_decl_matches(program, module_index, decl, target) ||
            decl->bind_node_index >= array_count(ast->nodes)) {
            continue;
        }

        lsp_rename_add_occurrence(occurrences,
                                  uri,
                                  lexer,
                                  ast->nodes[decl->bind_node_index].token_index,
                                  true);
    }

    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        u32 decl_index = sema_no_decl();
        if (!lsp_sema_node_decl(sema, i, &decl_index)) {
            continue;
        }

        const SemaDecl* decl = NULL;
        if (!lsp_sema_decl(sema, decl_index, &decl) ||
            !lsp_rename_decl_matches(program, module_index, decl, target) ||
            ast->nodes[i].token_index == U32_MAX) {
            continue;
        }

        lsp_rename_add_occurrence(
            occurrences, uri, lexer, ast->nodes[i].token_index, false);
    }
}

internal bool lsp_rename_module_already_seen(Array(cstr) paths, cstr path)
{
    if (!path) {
        return true;
    }
    for (u32 i = 0; i < array_count(paths); ++i) {
        if (paths[i] && strcmp(paths[i], path) == 0) {
            return true;
        }
    }
    return false;
}

internal Array(LspRenameOccurrence)
    lsp_rename_collect_decl_occurrences(LspState*                      state,
                                        Arena*                         arena,
                                        const LspRenameRequestContext* context,
                                        const LspRenameTarget*         target)
{
    Array(LspRenameOccurrence) occurrences = NULL;
    Array(cstr) seen_module_paths          = NULL;

    MapIter      iter                      = LspDocumentMap_iter();
    string       uri                       = {0};
    LspDocument* doc                       = NULL;
    while (LspDocumentMap_next(&state->documents, &iter, &uri, &doc)) {
        if (!doc->bindings_ready) {
            continue;
        }

        LspModuleView root = {0};
        if (lsp_program_module_view(
                &doc->program, doc->program.root_module_index, &root)) {
            lsp_rename_collect_module_decl_occurrences(
                &occurrences,
                arena,
                &doc->program,
                doc->program.root_module_index,
                uri,
                target);
            array_push(seen_module_paths, root.info->resolved_path);
        }

        for (u32 i = 0; i < array_count(doc->program.modules); ++i) {
            LspModuleView module = {0};
            if (i == doc->program.root_module_index ||
                !lsp_program_module_view(&doc->program, i, &module) ||
                lsp_rename_module_already_seen(seen_module_paths,
                                               module.info->resolved_path)) {
                continue;
            }

            lsp_rename_collect_module_decl_occurrences(
                &occurrences, arena, &doc->program, i, (string){0}, target);
            array_push(seen_module_paths, module.info->resolved_path);
        }
    }

    if (array_count(occurrences) == 0 && context) {
        lsp_rename_collect_module_decl_occurrences(
            &occurrences,
            arena,
            &context->doc->program,
            context->doc->program.root_module_index,
            context->uri,
            target);
    }

    array_free(seen_module_paths);
    return occurrences;
}

internal JsonValue*
lsp_rename_make_occurrence_location(Arena*              arena,
                                    LspRenameOccurrence occurrence)
{
    usize start = 0;
    usize end   = 0;
    lsp_rename_token_offsets(
        occurrence.lexer, occurrence.token_index, &start, &end);

    JsonValue* range =
        lsp_rename_make_range(arena, occurrence.lexer->source, start, end);
    JsonValue* location = json_new_object(arena);
    json_object_set_string(location, arena, "uri", occurrence.uri);
    json_object_set_object(location, "range", range);
    return location;
}

internal JsonValue* lsp_rename_edit_group(Arena* arena,
                                          Array(LspRenameEditGroup) * groups,
                                          string uri)
{
    for (u32 i = 0; i < array_count(*groups); ++i) {
        if (string_eq((*groups)[i].uri, uri)) {
            return (*groups)[i].edits;
        }
    }

    JsonValue* edits = json_new_array(arena);
    array_push(*groups,
               ((LspRenameEditGroup){
                   .uri   = uri,
                   .edits = edits,
               }));
    return edits;
}

internal JsonValue* lsp_rename_make_location(const LspRenameRequestContext* ctx,
                                             Arena* arena,
                                             u32    token_index)
{
    usize start = 0;
    usize end   = 0;
    lsp_rename_token_offsets(
        &ctx->doc->front_end.lexer, token_index, &start, &end);

    JsonValue* range =
        lsp_rename_make_document_range(ctx->doc, arena, start, end);
    if (range == NULL) {
        return NULL;
    }

    JsonValue* location = json_new_object(arena);
    json_object_set_string(location, arena, "uri", ctx->uri);
    json_object_set_object(location, "range", range);
    return location;
}

//------------------------------------------------------------------------------

void lsp_handle_references(LspState* state, const LspMessage* message)
{
    JsonValue*              response = lsp_prepare_response(message);
    LspRenameRequestContext context  = {0};
    LspRenameTarget         target   = {0};

    if (!lsp_rename_get_context(state, message, &context) ||
        !lsp_rename_target_from_token(
            context.doc, context.token_index, &target)) {
        lsp_rename_request_context_done(&context);
        lsp_cancel(response, message->arena);
        return;
    }

    JsonValue* include_decl_value =
        json_get_cstr(message->message, "params.context.includeDeclaration");
    bool       include_declaration = include_decl_value &&
                                     include_decl_value->kind == JSON_BOOL &&
                                     json_bool(include_decl_value);
    JsonValue* references          = json_new_array(message->arena);

    if (target.kind == LSP_RENAME_DECL || target.kind == LSP_RENAME_MEMBER) {
        Array(LspRenameOccurrence) occurrences =
            lsp_rename_collect_decl_occurrences(
                state, message->arena, &context, &target);
        for (u32 i = 0; i < array_count(occurrences); ++i) {
            if (!include_declaration && occurrences[i].is_declaration) {
                continue;
            }
            json_array_push(references,
                            lsp_rename_make_occurrence_location(
                                message->arena, occurrences[i]));
        }

        json_object_set_array(response, "result", references);
        lsp_send_response(message->arena, response);
        array_free(occurrences);
        lsp_rename_request_context_done(&context);
        return;
    }

    Array(u32) tokens = lsp_rename_collect_tokens(context.doc, target);
    for (u32 i = 0; i < array_count(tokens); ++i) {
        u32 token = tokens[i];
        if ((!include_declaration &&
             lsp_rename_token_is_declaration(context.doc, target, token)) ||
            !lsp_rename_token_is_visible(context.doc, token)) {
            continue;
        }

        JsonValue* location =
            lsp_rename_make_location(&context, message->arena, token);
        if (location != NULL) {
            json_array_push(references, location);
        }
    }

    json_object_set_array(response, "result", references);
    lsp_send_response(message->arena, response);
    array_free(tokens);
    lsp_rename_request_context_done(&context);
}

//------------------------------------------------------------------------------

void lsp_handle_prepare_rename(LspState* state, const LspMessage* message)
{
    JsonValue*              response = lsp_prepare_response(message);
    LspRenameRequestContext context  = {0};
    LspRenameTarget         target   = {0};

    if (!lsp_rename_get_context(state, message, &context) ||
        !lsp_rename_target_from_token(
            context.doc, context.token_index, &target) ||
        !lsp_rename_token_is_visible(context.doc, context.token_index)) {
        lsp_rename_request_context_done(&context);
        lsp_cancel(response, message->arena);
        return;
    }

    usize start = 0;
    usize end   = 0;
    lsp_rename_token_offsets(
        &context.doc->front_end.lexer, context.token_index, &start, &end);

    JsonValue* result = json_new_object(message->arena);
    json_object_set_object(result,
                           "range",
                           lsp_rename_make_document_range(
                               context.doc, message->arena, start, end));
    json_object_set_string(result, message->arena, "placeholder", target.name);
    json_object_set_object(response, "result", result);
    lsp_send_response(message->arena, response);
    lsp_rename_request_context_done(&context);
}

void lsp_handle_rename(LspState* state, const LspMessage* message)
{
    JsonValue* new_name_value =
        json_get_cstr(message->message, "params.newName");
    JsonValue*              response = lsp_prepare_response(message);
    LspRenameRequestContext context  = {0};
    LspRenameTarget         target   = {0};

    if (!new_name_value || new_name_value->kind != JSON_STRING ||
        !lsp_rename_valid_new_name(json_string(new_name_value)) ||
        !lsp_rename_get_context(state, message, &context) ||
        !lsp_rename_target_from_token(
            context.doc, context.token_index, &target)) {
        lsp_rename_request_context_done(&context);
        lsp_cancel(response, message->arena);
        return;
    }

    if (target.kind == LSP_RENAME_MEMBER &&
        !string_eq(target.name, json_string(new_name_value))) {
        for (u32 m = 0; m < array_count(context.doc->program.modules); ++m) {
            LspModuleView owner = {0};
            if (!lsp_program_module_view(&context.doc->program, m, &owner) ||
                strcmp(owner.info->resolved_path, target.origin_path) != 0) {
                continue;
            }
            LspRenameTarget existing = {0};
            if (lsp_rename_member_in_node(&context.doc->program,
                                          owner,
                                          target.owner_node,
                                          json_string(new_name_value),
                                          0,
                                          &existing)) {
                lsp_rename_request_context_done(&context);
                lsp_cancel(response, message->arena);
                return;
            }
        }
    }

    JsonValue* changes = json_new_object(message->arena);
    if (target.kind == LSP_RENAME_DECL || target.kind == LSP_RENAME_MEMBER) {
        Array(LspRenameEditGroup) groups = NULL;
        Array(LspRenameOccurrence) occurrences =
            lsp_rename_collect_decl_occurrences(
                state, message->arena, &context, &target);

        for (u32 i = 0; i < array_count(occurrences); ++i) {
            LspRenameOccurrence occurrence = occurrences[i];
            usize               start      = 0;
            usize               end        = 0;
            lsp_rename_token_offsets(
                occurrence.lexer, occurrence.token_index, &start, &end);

            JsonValue* edit  = json_new_object(message->arena);
            JsonValue* range = lsp_rename_make_range(
                message->arena, occurrence.lexer->source, start, end);
            json_object_set_object(edit, "range", range);
            string replacement = json_string(new_name_value);
            if (occurrence.shorthand_name.count > 0) {
                replacement = string_format(message->arena,
                                            STRINGP ": " STRINGP,
                                            STRINGV(replacement),
                                            STRINGV(occurrence.shorthand_name));
            }
            json_object_set_string(
                edit, message->arena, "newText", replacement);
            json_array_push(
                lsp_rename_edit_group(message->arena, &groups, occurrence.uri),
                edit);
        }

        for (u32 i = 0; i < array_count(groups); ++i) {
            json_object_set_array(
                changes,
                lsp_rename_cstr(message->arena, groups[i].uri),
                groups[i].edits);
        }

        array_free(groups);
        array_free(occurrences);
    } else {
        Array(u32) tokens = lsp_rename_collect_tokens(context.doc, target);
        JsonValue* edits  = json_new_array(message->arena);
        for (u32 i = 0; i < array_count(tokens); ++i) {
            u32 token = tokens[i];
            if (!lsp_rename_token_is_visible(context.doc, token)) {
                continue;
            }

            usize start = 0;
            usize end   = 0;
            lsp_rename_token_offsets(
                &context.doc->front_end.lexer, token, &start, &end);

            JsonValue* edit  = json_new_object(message->arena);
            JsonValue* range = lsp_rename_make_document_range(
                context.doc, message->arena, start, end);
            if (range == NULL) {
                continue;
            }

            json_object_set_object(edit, "range", range);
            string       replacement = json_string(new_name_value);
            const Ast*   ast         = &context.doc->front_end.ast;
            const Lexer* lexer       = &context.doc->front_end.lexer;
            for (u32 f = 0; f < array_count(ast->plex_literal_fields); ++f) {
                const AstPlexLiteralField* field = &ast->plex_literal_fields[f];
                if (field->token_index == token &&
                    field->value_node_index < array_count(ast->nodes) &&
                    ast->nodes[field->value_node_index].kind == AK_SymbolRef &&
                    ast->nodes[field->value_node_index].token_index == token) {
                    replacement = string_format(
                        message->arena,
                        STRINGP ": " STRINGP,
                        STRINGV(lex_symbol(lexer, field->symbol_handle)),
                        STRINGV(replacement));
                    break;
                }
            }
            for (u32 f = 0; f < array_count(ast->pattern_fields); ++f) {
                const AstPlexPatternField* field = &ast->pattern_fields[f];
                if (field->token_index == token &&
                    ast->patterns[field->pattern_index].kind == APK_Bind &&
                    ast->patterns[field->pattern_index].token_index == token) {
                    replacement = string_format(
                        message->arena,
                        STRINGP ": " STRINGP,
                        STRINGV(lex_symbol(lexer, field->symbol_handle)),
                        STRINGV(replacement));
                    break;
                }
            }
            json_object_set_string(
                edit, message->arena, "newText", replacement);
            json_array_push(edits, edit);
        }

        json_object_set_array(
            changes, lsp_rename_cstr(message->arena, context.uri), edits);
        array_free(tokens);
    }

    JsonValue* workspace_edit = json_new_object(message->arena);
    json_object_set_object(workspace_edit, "changes", changes);
    json_object_set_object(response, "result", workspace_edit);
    lsp_send_response(message->arena, response);
    lsp_rename_request_context_done(&context);
}
