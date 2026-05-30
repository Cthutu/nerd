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
} LspRenameKind;

typedef struct {
    LspRenameKind kind;
    u32           index;
    u32           token_index;
    string        name;
    cstr          origin_path;
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
    return true;
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

internal void
lsp_rename_add_occurrence(Array(LspRenameOccurrence) * occurrences,
                          string                       uri,
                          const Lexer*                 lexer,
                          u32                          token_index,
                          bool                         is_declaration)
{
    if (!lexer || token_index == U32_MAX) {
        return;
    }

    usize start = 0;
    usize end   = 0;
    lsp_rename_token_offsets(lexer, token_index, &start, &end);

    for (u32 i = 0; i < array_count(*occurrences); ++i) {
        LspRenameOccurrence* occurrence = &(*occurrences)[i];
        usize occurrence_start = 0;
        usize occurrence_end   = 0;
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

internal void
lsp_rename_collect_module_decl_occurrences(Array(LspRenameOccurrence) *
                                               occurrences,
                                           Arena* arena,
                                           const ProgramInfo* program,
                                           u32 module_index,
                                           string uri,
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
    lsp_rename_collect_decl_occurrences(LspState* state,
                                        Arena* arena,
                                        const LspRenameRequestContext* context,
                                        const LspRenameTarget* target)
{
    Array(LspRenameOccurrence) occurrences = NULL;
    Array(cstr) seen_module_paths          = NULL;

    MapIter iter = LspDocumentMap_iter();
    string       uri = {0};
    LspDocument* doc = NULL;
    while (LspDocumentMap_next(&state->documents, &iter, &uri, &doc)) {
        if (!doc->bindings_ready) {
            continue;
        }

        LspModuleView root = {0};
        if (lsp_program_module_view(
                &doc->program, doc->program.root_module_index, &root)) {
            lsp_rename_collect_module_decl_occurrences(&occurrences,
                                                       arena,
                                                       &doc->program,
                                                       doc->program
                                                           .root_module_index,
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

            lsp_rename_collect_module_decl_occurrences(&occurrences,
                                                       arena,
                                                       &doc->program,
                                                       i,
                                                       (string){0},
                                                       target);
            array_push(seen_module_paths, module.info->resolved_path);
        }
    }

    if (array_count(occurrences) == 0 && context) {
        lsp_rename_collect_module_decl_occurrences(&occurrences,
                                                   arena,
                                                   &context->doc->program,
                                                   context->doc->program
                                                       .root_module_index,
                                                   context->uri,
                                                   target);
    }

    array_free(seen_module_paths);
    return occurrences;
}

internal JsonValue* lsp_rename_make_occurrence_location(Arena* arena,
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
    bool include_declaration = include_decl_value &&
                               include_decl_value->kind == JSON_BOOL &&
                               json_bool(include_decl_value);
    JsonValue* references    = json_new_array(message->arena);

    if (target.kind == LSP_RENAME_DECL) {
        Array(LspRenameOccurrence) occurrences =
            lsp_rename_collect_decl_occurrences(
                state, message->arena, &context, &target);
        for (u32 i = 0; i < array_count(occurrences); ++i) {
            if (!include_declaration && occurrences[i].is_declaration) {
                continue;
            }
            json_array_push(
                references,
                lsp_rename_make_occurrence_location(message->arena,
                                                    occurrences[i]));
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

    JsonValue* changes = json_new_object(message->arena);
    if (target.kind == LSP_RENAME_DECL) {
        Array(LspRenameEditGroup) groups      = NULL;
        Array(LspRenameOccurrence) occurrences =
            lsp_rename_collect_decl_occurrences(
                state, message->arena, &context, &target);

        for (u32 i = 0; i < array_count(occurrences); ++i) {
            LspRenameOccurrence occurrence = occurrences[i];
            usize start = 0;
            usize end   = 0;
            lsp_rename_token_offsets(
                occurrence.lexer, occurrence.token_index, &start, &end);

            JsonValue* edit  = json_new_object(message->arena);
            JsonValue* range = lsp_rename_make_range(
                message->arena, occurrence.lexer->source, start, end);
            json_object_set_object(edit, "range", range);
            json_object_set_string(
                edit, message->arena, "newText", json_string(new_name_value));
            json_array_push(lsp_rename_edit_group(message->arena,
                                                  &groups,
                                                  occurrence.uri),
                            edit);
        }

        for (u32 i = 0; i < array_count(groups); ++i) {
            json_object_set_array(changes,
                                  lsp_rename_cstr(message->arena,
                                                  groups[i].uri),
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
            json_object_set_string(
                edit, message->arena, "newText", json_string(new_name_value));
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
