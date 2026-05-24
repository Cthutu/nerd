//------------------------------------------------------------------------------
// LSP code actions
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <lsp/lsp.h>

#include <compiler/error/error.h>
#include <compiler/modules/modules.h>

//------------------------------------------------------------------------------

internal JsonValue*
lsp_code_action_position(Arena* arena, NerdSource source, usize offset)
{
    u32 line = 0;
    u32 col  = 0;
    if (!lex_offset_to_line_col(source, offset, &line, &col)) {
        return NULL;
    }

    JsonValue* position = json_new_object(arena);
    json_object_set_number(position, arena, "line", line);
    json_object_set_number(position, arena, "character", col);
    return position;
}

internal cstr lsp_code_action_cstr(Arena* arena, string value)
{
    char* data = arena_alloc(arena, value.count + 1);
    memcpy(data, value.data, value.count);
    data[value.count] = '\0';
    return data;
}

internal bool lsp_code_action_is_ident_char(u8 c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
           (c >= '0' && c <= '9') || c == '_';
}

internal bool lsp_code_action_string_starts_with(string value, cstr prefix)
{
    string prefix_string = s(prefix);
    return value.count >= prefix_string.count &&
           memcmp(value.data, prefix_string.data, prefix_string.count) == 0;
}

internal bool lsp_code_action_string_contains(string haystack, string needle)
{
    if (needle.count == 0 || needle.count > haystack.count) {
        return false;
    }
    for (usize i = 0; i + needle.count <= haystack.count; ++i) {
        if (memcmp(haystack.data + i, needle.data, needle.count) == 0) {
            return true;
        }
    }
    return false;
}

internal void
lsp_code_action_skip_line_space(string source, usize line_end, usize* cursor)
{
    while (*cursor < line_end &&
           (source.data[*cursor] == ' ' || source.data[*cursor] == '\t')) {
        (*cursor)++;
    }
}

internal bool lsp_code_action_match_keyword(string source,
                                            usize  line_end,
                                            usize  cursor,
                                            cstr   keyword,
                                            usize  keyword_count)
{
    if (cursor + keyword_count > line_end ||
        memcmp(source.data + cursor, keyword, keyword_count) != 0) {
        return false;
    }

    return cursor + keyword_count == line_end ||
           !lsp_code_action_is_ident_char(source.data[cursor + keyword_count]);
}

internal string lsp_code_action_copy_string(Arena*    arena,
                                            const u8* data,
                                            usize     count)
{
    u8* copy = arena_alloc(arena, count);
    memcpy(copy, data, count);
    return (string){.data = copy, .count = count};
}

internal JsonValue*
lsp_code_action_range(Arena* arena, NerdSource source, usize start, usize end)
{
    JsonValue* start_position = lsp_code_action_position(arena, source, start);
    JsonValue* end_position   = lsp_code_action_position(arena, source, end);
    if (start_position == NULL || end_position == NULL) {
        return NULL;
    }

    JsonValue* range = json_new_object(arena);
    json_object_set_object(range, "start", start_position);
    json_object_set_object(range, "end", end_position);
    return range;
}

internal JsonValue* lsp_code_action_workspace_edit(Arena*     arena,
                                                   string     uri,
                                                   NerdSource source,
                                                   usize      insert_offset,
                                                   string     insert_text)
{
    JsonValue* range =
        lsp_code_action_range(arena, source, insert_offset, insert_offset);
    if (range == NULL) {
        return NULL;
    }

    JsonValue* edit = json_new_object(arena);
    json_object_set_object(edit, "range", range);
    json_object_set_string(edit, arena, "newText", insert_text);

    JsonValue* edits = json_new_array(arena);
    json_array_push(edits, edit);

    JsonValue* changes = json_new_object(arena);
    json_object_set_array(changes, lsp_code_action_cstr(arena, uri), edits);

    JsonValue* workspace_edit = json_new_object(arena);
    json_object_set_object(workspace_edit, "changes", changes);
    return workspace_edit;
}

internal JsonValue* lsp_code_action_workspace_range_edit(Arena*     arena,
                                                         string     uri,
                                                         JsonValue* range,
                                                         string     new_text)
{
    if (range == NULL || range->kind != JSON_OBJECT) {
        return NULL;
    }

    JsonValue* edit = json_new_object(arena);
    json_object_set_object(edit, "range", range);
    json_object_set_string(edit, arena, "newText", new_text);

    JsonValue* edits = json_new_array(arena);
    json_array_push(edits, edit);

    JsonValue* changes = json_new_object(arena);
    json_object_set_array(changes, lsp_code_action_cstr(arena, uri), edits);

    JsonValue* workspace_edit = json_new_object(arena);
    json_object_set_object(workspace_edit, "changes", changes);
    return workspace_edit;
}

internal usize lsp_code_action_line_start(string source, usize offset)
{
    if (offset > source.count) {
        offset = source.count;
    }
    while (offset > 0 && source.data[offset - 1] != '\n') {
        offset--;
    }
    return offset;
}

internal string lsp_code_action_line_indent(Arena* arena,
                                            string source,
                                            usize  offset)
{
    usize start = lsp_code_action_line_start(source, offset);
    usize end   = start;
    while (end < source.count &&
           (source.data[end] == ' ' || source.data[end] == '\t')) {
        end++;
    }

    u8* data = arena_alloc(arena, end - start);
    memcpy(data, source.data + start, end - start);
    return (string){.data = data, .count = end - start};
}

internal string lsp_code_action_trim_source(const LspDocument* doc,
                                            usize              start,
                                            usize              end)
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

internal void lsp_code_action_append_spaces(StringBuilder* sb, usize count)
{
    for (usize i = 0; i < count; ++i) {
        sb_append_char(sb, ' ');
    }
}

internal bool lsp_code_action_token_is_open(TokenKind kind)
{
    return kind == TK_LBrace || kind == TK_LParen || kind == TK_LBracket;
}

internal bool lsp_code_action_token_is_close(TokenKind kind)
{
    return kind == TK_RBrace || kind == TK_RParen || kind == TK_RBracket;
}

internal bool lsp_code_action_matching_close(const Lexer* lexer,
                                             u32          open_token_index,
                                             u32*         out_close_token_index)
{
    if (open_token_index >= array_count(lexer->tokens) ||
        lexer->tokens[open_token_index].kind != TK_LBrace) {
        return false;
    }

    u32 depth = 0;
    for (u32 i = open_token_index; i < array_count(lexer->tokens); ++i) {
        TokenKind kind = lexer->tokens[i].kind;
        if (lsp_code_action_token_is_open(kind)) {
            depth++;
        } else if (lsp_code_action_token_is_close(kind)) {
            if (depth == 0) {
                return false;
            }
            depth--;
            if (depth == 0) {
                *out_close_token_index = i;
                return kind == TK_RBrace;
            }
        }
    }
    return false;
}

internal u32 lsp_code_action_find_decl(const Sema* sema, u32 symbol)
{
    u32 decl_index = sema_no_decl();
    if (!lsp_sema_decl_by_symbol(sema, symbol, NULL, &decl_index)) {
        return sema_no_decl();
    }
    return decl_index;
}

internal bool lsp_code_action_resolve_type_node(const Ast*  ast,
                                                const Sema* sema,
                                                u32         node_index,
                                                u32*        out_type)
{
    if (node_index >= array_count(ast->nodes)) {
        return false;
    }

    const AstNode* node = &ast->nodes[node_index];
    switch (node->kind) {
    case AK_SymbolRef:
        {
            u32 decl_index = sema_no_decl();
            if (!lsp_sema_node_decl(sema, node_index, &decl_index)) {
                decl_index = lsp_code_action_find_decl(sema, node->a);
            }
            const SemaDecl* decl = NULL;
            if (lsp_sema_decl(sema, decl_index, &decl)) {
                *out_type = decl->type_index;
                return *out_type != sema_no_type();
            }
            for (u32 i = 0; i < array_count(ast->nodes); ++i) {
                const AstNode* candidate = &ast->nodes[i];
                if ((candidate->kind == AK_Bind ||
                     candidate->kind == AK_Variable) &&
                    ast_get_symbol(candidate) == node->a &&
                    lsp_sema_node_type(sema, i, out_type)) {
                    return true;
                }
            }
            return false;
        }
    case AK_Field:
        {
            u32 target_type = sema_no_type();
            if (!lsp_code_action_resolve_type_node(
                    ast, sema, node->a, &target_type)) {
                return false;
            }
            const SemaType* module = NULL;
            if (!lsp_sema_type(sema, target_type, &module) ||
                module->kind != STK_Module) {
                return false;
            }
            for (u32 i = 0; i < module->param_count; ++i) {
                u32 symbol           = U32_MAX;
                u32 param_type_index = sema_no_type();
                if (!lsp_sema_type_param(sema,
                                         module->first_param_type + i,
                                         &symbol,
                                         &param_type_index)) {
                    continue;
                }
                if (symbol == node->b) {
                    *out_type = param_type_index;
                    return *out_type != sema_no_type();
                }
            }
            return false;
        }
    default:
        return false;
    }
}

internal bool lsp_code_action_resolve_plex_literal_type(const Ast*  ast,
                                                        const Sema* sema,
                                                        u32         node_index,
                                                        u32*        out_type)
{
    if (node_index >= array_count(ast->nodes)) {
        return false;
    }

    const AstNode* node = &ast->nodes[node_index];
    if (node->kind != AK_Plex) {
        return false;
    }

    const AstPlexLiteralInfo* literal = &ast->plex_literals[node->a];
    if (literal->target_node_index != U32_MAX) {
        return lsp_code_action_resolve_type_node(
            ast, sema, literal->target_node_index, out_type);
    }

    if (lsp_sema_node_type(sema, node_index, out_type)) {
        return true;
    }

    return false;
}

internal u32 lsp_code_action_unwrap_type_value_node(const Ast* ast,
                                                    u32        node_index)
{
    while (node_index < array_count(ast->nodes)) {
        const AstNode* node = &ast->nodes[node_index];
        if (node->kind == AK_AnnotatedValue || node->kind == AK_ZeroInit ||
            node->kind == AK_Undefined) {
            node_index = node->kind == AK_AnnotatedValue ? node->b : node->a;
            continue;
        }
        break;
    }
    return node_index;
}

internal u32 lsp_code_action_find_ast_type_alias(const Ast* ast, u32 symbol)
{
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind != AK_Bind || node->a != symbol ||
            node->b >= array_count(ast->nodes)) {
            continue;
        }

        u32 value_node_index =
            lsp_code_action_unwrap_type_value_node(ast, node->b);
        if (value_node_index < array_count(ast->nodes)) {
            return value_node_index;
        }
    }
    return U32_MAX;
}

internal bool lsp_code_action_ast_default_value(Arena*       arena,
                                                const Ast*   ast,
                                                const Lexer* lexer,
                                                u32          type_node_index,
                                                string*      out_value);

internal bool lsp_code_action_ast_default_record(Arena*       arena,
                                                 const Ast*   ast,
                                                 const Lexer* lexer,
                                                 u32          plex_index,
                                                 string*      out_value)
{
    if (plex_index >= array_count(ast->plex_types)) {
        return false;
    }

    const AstPlexTypeInfo* plex = &ast->plex_types[plex_index];
    StringBuilder          sb   = {0};
    sb_init(&sb, arena);
    sb_append_cstr(&sb, "{ ");
    for (u32 i = 0; i < plex->field_count; ++i) {
        const AstPlexField* field = &ast->plex_fields[plex->first_field + i];
        string              value = {0};
        if (!lsp_code_action_ast_default_value(
                arena, ast, lexer, field->type_node_index, &value)) {
            return false;
        }
        if (i > 0) {
            sb_append_cstr(&sb, " ");
        }
        sb_append_string(&sb, lex_symbol(lexer, field->symbol_handle));
        sb_append_cstr(&sb, ": ");
        sb_append_string(&sb, value);
    }
    sb_append_cstr(&sb, " }");
    *out_value = sb_to_string(&sb);
    return true;
}

//------------------------------------------------------------------------------
// Import quick fixes

internal bool lsp_code_action_symbol_token_at_offset(const Lexer* lexer,
                                                     usize        offset,
                                                     u32* out_token_index)
{
    for (u32 i = 0; i < array_count(lexer->tokens); ++i) {
        const Token* token = &lexer->tokens[i];
        if (token->kind != TK_Symbol) {
            continue;
        }

        usize end = lex_token_end_offset(lexer, token);
        if (offset >= token->offset && offset <= end) {
            *out_token_index = i;
            return true;
        }
    }

    return false;
}

internal void lsp_code_action_add_token(Array(u32) * tokens, u32 token_index)
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

internal bool lsp_code_action_local_at_token(const LspDocument* doc,
                                             u32                token_index,
                                             u32*               out_local_index)
{
    const Ast*  ast  = &doc->front_end.ast;
    const Sema* sema = &doc->front_end.sema;

    for (u32 i = 0; i < array_count(sema->locals); ++i) {
        if (sema->locals[i].decl_token_index == token_index) {
            *out_local_index = i;
            return true;
        }
    }

    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        if (ast->nodes[i].token_index != token_index) {
            continue;
        }
        u32 local_index = sema_no_local();
        if (lsp_sema_node_local(sema, i, &local_index)) {
            *out_local_index = local_index;
            return true;
        }
    }

    return false;
}

internal Array(u32)
    lsp_code_action_local_tokens(const LspDocument* doc, u32 local_index)
{
    Array(u32) tokens      = NULL;
    const Ast*  ast        = &doc->front_end.ast;
    const Sema* sema       = &doc->front_end.sema;

    const SemaLocal* local = NULL;
    if (!lsp_sema_local(sema, local_index, &local)) {
        return tokens;
    }

    lsp_code_action_add_token(&tokens, local->decl_token_index);
    if (local->decl_node_index < array_count(ast->nodes)) {
        lsp_code_action_add_token(
            &tokens, ast->nodes[local->decl_node_index].token_index);
    }

    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        u32 node_local = sema_no_local();
        if (lsp_sema_node_local(sema, i, &node_local) &&
            node_local == local_index) {
            lsp_code_action_add_token(&tokens, ast->nodes[i].token_index);
        }
    }

    return tokens;
}

internal JsonValue* lsp_code_action_rename_tokens_edit(Arena*             arena,
                                                       string             uri,
                                                       const LspDocument* doc,
                                                       Array(u32) tokens,
                                                       string new_name)
{
    if (array_count(tokens) == 0) {
        return NULL;
    }

    JsonValue*   edits        = json_new_array(arena);
    const Lexer* lexer        = &doc->front_end.lexer;
    u32          pushed_count = 0;
    for (u32 i = 0; i < array_count(tokens); ++i) {
        u32 token_index = tokens[i];
        if (token_index >= array_count(lexer->tokens)) {
            continue;
        }

        const Token* token = &lexer->tokens[token_index];
        usize        end   = lex_token_end_offset(lexer, token);
        JsonValue*   range =
            lsp_code_action_range(arena, lexer->source, token->offset, end);
        if (range == NULL) {
            continue;
        }

        JsonValue* edit = json_new_object(arena);
        json_object_set_object(edit, "range", range);
        json_object_set_string(edit, arena, "newText", new_name);
        json_array_push(edits, edit);
        pushed_count++;
    }

    if (pushed_count == 0) {
        return NULL;
    }

    JsonValue* changes = json_new_object(arena);
    json_object_set_array(changes, lsp_code_action_cstr(arena, uri), edits);

    JsonValue* workspace_edit = json_new_object(arena);
    json_object_set_object(workspace_edit, "changes", changes);
    return workspace_edit;
}

internal JsonValue* lsp_code_action_rename_local_edit(Arena*             arena,
                                                      string             uri,
                                                      const LspDocument* doc,
                                                      u32    local_index,
                                                      string new_name)
{
    Array(u32) tokens = lsp_code_action_local_tokens(doc, local_index);
    JsonValue* edit =
        lsp_code_action_rename_tokens_edit(arena, uri, doc, tokens, new_name);
    array_free(tokens);
    return edit;
}

internal u32 lsp_code_action_symbol_at_token(const LspDocument* doc,
                                             u32                token_index)
{
    const Ast* ast = &doc->front_end.ast;
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->token_index != token_index) {
            continue;
        }
        switch (node->kind) {
        case AK_Bind:
        case AK_Variable:
        case AK_SymbolRef:
            return node->a;
        default:
            break;
        }
    }

    for (u32 i = 0; i < array_count(ast->params); ++i) {
        const AstParam* param = &ast->params[i];
        if (param->token_index == token_index) {
            return param->symbol_handle;
        }
    }

    for (u32 i = 0; i < array_count(ast->patterns); ++i) {
        const AstPattern* pattern = &ast->patterns[i];
        if (pattern->kind == APK_Bind && pattern->token_index == token_index) {
            return pattern->a;
        }
    }

    for (u32 i = 0; i < array_count(ast->fors); ++i) {
        const AstForInfo* for_info = &ast->fors[i];
        if (for_info->index_token_index == token_index) {
            return for_info->index_symbol;
        }
        if (for_info->item_token_index == token_index) {
            return for_info->item_symbol;
        }
    }

    for (u32 i = 0; i < array_count(ast->on_branches); ++i) {
        const AstOnBranch* branch = &ast->on_branches[i];
        if (branch->binder_token_index == token_index) {
            return branch->binder_symbol_handle;
        }
    }

    return U32_MAX;
}

internal void lsp_code_action_add_unused_local_rename(Arena*     arena,
                                                      JsonValue* actions,
                                                      string     uri,
                                                      const LspDocument* doc,
                                                      u32 token_index)
{
    u32    symbol = lsp_code_action_symbol_at_token(doc, token_index);
    string name = symbol != U32_MAX ? lex_symbol(&doc->front_end.lexer, symbol)
                                    : (string){0};
    if (name.count == 0) {
        return;
    }

    string new_name = {0};
    if (name.data[0] == '_') {
        if (name.count == 1) {
            return;
        }
        new_name = string_from(name.data + 1, name.count - 1);
    } else {
        StringBuilder sb = {0};
        sb_init(&sb, arena);
        sb_append_char(&sb, '_');
        sb_append_string(&sb, name);
        new_name = sb_to_string(&sb);
    }

    JsonValue* workspace_edit = NULL;
    u32        local_index    = sema_no_local();
    if (lsp_code_action_local_at_token(doc, token_index, &local_index)) {
        workspace_edit = lsp_code_action_rename_local_edit(
            arena, uri, doc, local_index, new_name);
    }
    if (workspace_edit == NULL) {
        return;
    }

    StringBuilder title = {0};
    sb_init(&title, arena);
    sb_append_cstr(&title, "Rename to ");
    sb_append_string(&title, new_name);

    JsonValue* action = json_new_object(arena);
    json_object_set_string(action, arena, "title", sb_to_string(&title));
    json_object_set_string(action, arena, "kind", s("quickfix"));
    json_object_set_object(action, "edit", workspace_edit);
    json_array_push(actions, action);
}

internal bool lsp_code_action_has_unused_local_diagnostic(
    Arena* arena, const LspMessage* message, string name)
{
    JsonValue* diagnostics =
        json_get_cstr(message->message, "params.context.diagnostics");
    if (diagnostics == NULL || diagnostics->kind != JSON_ARRAY) {
        return false;
    }

    StringBuilder quoted_name = {0};
    sb_init(&quoted_name, arena);
    sb_append_char(&quoted_name, '`');
    sb_append_string(&quoted_name, name);
    sb_append_char(&quoted_name, '`');
    string quoted = sb_to_string(&quoted_name);

    for (u32 i = 0; i < array_count(diagnostics->array.values); ++i) {
        JsonValue* diagnostic    = diagnostics->array.values[i];
        JsonValue* message_value = json_get_cstr(diagnostic, "message");
        if (message_value == NULL || message_value->kind != JSON_STRING) {
            continue;
        }

        string text      = message_value->string;
        bool   is_unused = lsp_code_action_string_starts_with(text, "Unused ");
        bool   is_used_underscore =
            lsp_code_action_string_starts_with(text, "Used ") &&
            lsp_code_action_string_contains(text, s(" marked as unused"));
        if ((is_unused || is_used_underscore) &&
            lsp_code_action_string_contains(text, quoted)) {
            return true;
        }
    }

    return false;
}

internal bool lsp_code_action_parse_member_suggestion(string  help,
                                                      string* out_suggestion)
{
    string prefix = s("help: Did you mean `.");
    string suffix = s("`?");
    if (help.count < prefix.count + suffix.count ||
        memcmp(help.data, prefix.data, prefix.count) != 0 ||
        memcmp(help.data + help.count - suffix.count,
               suffix.data,
               suffix.count) != 0) {
        return false;
    }

    *out_suggestion = string_from(help.data + prefix.count,
                                  help.count - prefix.count - suffix.count);
    return out_suggestion->count > 0;
}

internal bool lsp_code_action_unknown_member_suggestion(JsonValue* diagnostic,
                                                        string* out_suggestion)
{
    JsonValue* message = json_get_cstr(diagnostic, "message");
    if (message == NULL || message->kind != JSON_STRING ||
        !lsp_code_action_string_starts_with(message->string,
                                            "Unknown member `")) {
        return false;
    }

    JsonValue* related = json_get_cstr(diagnostic, "relatedInformation");
    if (related == NULL || related->kind != JSON_ARRAY) {
        return false;
    }

    for (u32 i = 0; i < array_count(related->array.values); ++i) {
        JsonValue* info         = related->array.values[i];
        JsonValue* help_message = json_get_cstr(info, "message");
        if (help_message == NULL || help_message->kind != JSON_STRING) {
            continue;
        }
        if (lsp_code_action_parse_member_suggestion(help_message->string,
                                                    out_suggestion)) {
            return true;
        }
    }

    return false;
}

internal void lsp_code_action_add_unknown_member_suggestions(
    Arena* arena, JsonValue* actions, string uri, const LspMessage* message)
{
    JsonValue* diagnostics =
        json_get_cstr(message->message, "params.context.diagnostics");
    if (diagnostics == NULL || diagnostics->kind != JSON_ARRAY) {
        return;
    }

    for (u32 i = 0; i < array_count(diagnostics->array.values); ++i) {
        JsonValue* diagnostic = diagnostics->array.values[i];
        string     suggestion = {0};
        if (!lsp_code_action_unknown_member_suggestion(diagnostic,
                                                       &suggestion)) {
            continue;
        }

        JsonValue* range = json_get_cstr(diagnostic, "range");
        JsonValue* workspace_edit =
            lsp_code_action_workspace_range_edit(arena, uri, range, suggestion);
        if (workspace_edit == NULL) {
            continue;
        }

        StringBuilder title = {0};
        sb_init(&title, arena);
        sb_append_cstr(&title, "Change to ");
        sb_append_string(&title, suggestion);

        JsonValue* action = json_new_object(arena);
        json_object_set_string(action, arena, "title", sb_to_string(&title));
        json_object_set_string(action, arena, "kind", s("quickfix"));
        json_object_set_object(action, "edit", workspace_edit);
        json_array_push(actions, action);
    }
}

internal bool lsp_code_action_symbol_ref_is_resolved(const LspDocument* doc,
                                                     u32 token_index)
{
    if (!doc->bindings_ready) {
        return false;
    }

    const Ast*  ast  = &doc->front_end.ast;
    const Sema* sema = &doc->front_end.sema;
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind != AK_SymbolRef || node->token_index != token_index) {
            continue;
        }

        u32 decl  = i < array_count(sema->node_decl_indices)
                        ? sema->node_decl_indices[i]
                        : sema_no_decl();
        u32 local = i < array_count(sema->node_local_indices)
                        ? sema->node_local_indices[i]
                        : sema_no_local();
        return decl != sema_no_decl() || local != sema_no_local();
    }

    return true;
}

internal bool lsp_code_action_doc_declares_symbol(const LspDocument* doc,
                                                  string             symbol)
{
    const Ast*   ast   = &doc->front_end.ast;
    const Lexer* lexer = &doc->front_end.lexer;
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (!ast_node_is_binding_like(node)) {
            continue;
        }

        u32 node_symbol = ast_get_symbol(node);
        if (node_symbol != U32_MAX &&
            string_eq(lex_symbol(lexer, node_symbol), symbol)) {
            return true;
        }
    }

    return false;
}

internal u32 lsp_code_action_unwrap_expr_node(const Ast* ast, u32 node_index)
{
    while (node_index < array_count(ast->nodes) &&
           (ast->nodes[node_index].kind == AK_Expression ||
            ast->nodes[node_index].kind == AK_Statement)) {
        node_index = ast->nodes[node_index].a;
    }
    return node_index;
}

internal bool lsp_code_action_field_call_is_resolved(const LspDocument* doc,
                                                     u32 field_node_index)
{
    if (!doc->bindings_ready) {
        return false;
    }

    const Ast*  ast  = &doc->front_end.ast;
    const Sema* sema = &doc->front_end.sema;
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind != AK_Call) {
            continue;
        }

        u32 callee_index = lsp_code_action_unwrap_expr_node(ast, node->a);
        if (callee_index < array_count(ast->nodes) &&
            ast->nodes[callee_index].kind == AK_Index) {
            callee_index = lsp_code_action_unwrap_expr_node(
                ast, ast->nodes[callee_index].a);
        }
        if (callee_index != field_node_index) {
            continue;
        }

        u32 decl = i < array_count(sema->node_method_call_decl_indices)
                       ? sema->node_method_call_decl_indices[i]
                       : sema_no_decl();
        return decl != sema_no_decl();
    }

    return true;
}

internal bool lsp_code_action_symbol_needs_import(const LspDocument* doc,
                                                  u32 token_index)
{
    if (!lsp_code_action_symbol_ref_is_resolved(doc, token_index)) {
        return true;
    }

    const Ast* ast = &doc->front_end.ast;
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind == AK_Field && node->token_index == token_index &&
            !lsp_code_action_field_call_is_resolved(doc, i)) {
            return true;
        }
    }

    return false;
}

internal bool lsp_code_action_module_path_seen(Array(string) paths, string path)
{
    for (u32 i = 0; i < array_count(paths); ++i) {
        if (string_eq(paths[i], path)) {
            return true;
        }
    }
    return false;
}

internal void lsp_code_action_add_module_path(Array(string) * paths,
                                              Arena* arena,
                                              string path)
{
    if (path.count == 0 || lsp_code_action_module_path_seen(*paths, path)) {
        return;
    }

    array_push(*paths,
               lsp_code_action_copy_string(arena, path.data, path.count));
}

internal string lsp_code_action_module_path_from_file(Arena* arena,
                                                      cstr   root,
                                                      cstr   path)
{
    usize root_len = strlen(root);
    usize path_len = strlen(path);
    if (path_len <= root_len || strncmp(root, path, root_len) != 0) {
        return (string){0};
    }

    usize start = root_len;
    while (path[start] == '/' || path[start] == '\\') {
        start++;
    }

    usize end = path_len;
    if (end >= 6 && strcmp(path + end - 6, "/mod.n") == 0) {
        end -= 6;
    } else if (end >= 6 && strcmp(path + end - 6, "\\mod.n") == 0) {
        end -= 6;
    } else if (end >= 2 && strcmp(path + end - 2, ".n") == 0) {
        end -= 2;
    } else {
        return (string){0};
    }

    if (end <= start) {
        return (string){0};
    }

    StringBuilder sb = {0};
    sb_init(&sb, arena);
    for (usize i = start; i < end; ++i) {
        char c = path[i];
        sb_append_char(&sb, c == '/' || c == '\\' ? '.' : c);
    }
    return sb_to_string(&sb);
}

internal bool lsp_code_action_ast_exports_symbol(const Lexer* lexer,
                                                 const Ast*   ast,
                                                 string       symbol)
{
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind == AK_Impl && node->a < array_count(ast->impls)) {
            const AstImplInfo* impl = &ast->impls[node->a];
            if (impl->body_node_index >= array_count(ast->nodes)) {
                continue;
            }
            const AstNode* body = &ast->nodes[impl->body_node_index];
            if (body->kind != AK_Block) {
                continue;
            }
            for (u32 item = body->a; item < body->b; ++item) {
                const AstNode* member = &ast->nodes[item];
                if (!ast_node_is_binding_like(member) ||
                    !ast_has_flag(member, ANF_Public)) {
                    continue;
                }

                u32 member_symbol = ast_get_symbol(member);
                if (member_symbol != U32_MAX &&
                    string_eq(lex_symbol(lexer, member_symbol), symbol)) {
                    return true;
                }
            }
            continue;
        }

        if (!ast_node_is_binding_like(node) ||
            !ast_has_flag(node, ANF_Public)) {
            continue;
        }

        u32 node_symbol = ast_get_symbol(node);
        if (node_symbol != U32_MAX &&
            string_eq(lex_symbol(lexer, node_symbol), symbol)) {
            return true;
        }
    }

    return false;
}

internal bool lsp_code_action_file_exports_symbol(cstr path, string symbol)
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

    Lexer lexer = {0};
    Ast   ast   = {0};
    bool  found = false;
    if (lex((NerdSource){.source = source, .source_path = s(path)}, &lexer)) {
        ast   = ast_parse(&lexer);
        found = lsp_code_action_ast_exports_symbol(&lexer, &ast, symbol);
    }

    ast_done(&ast);
    lex_done(&lexer);
    error_system_set_mode(previous_mode);
    error_system_set_emit_output(previous_emit);
    filemap_unload(&map);
    return found;
}

internal bool
lsp_code_action_should_descend_module_dir(cstr root, cstr dir, string filename)
{
    if (root == NULL || dir == NULL || strcmp(root, dir) == 0) {
        return true;
    }

    return !string_eq_cstr(filename, "mods") &&
           !string_eq_cstr(filename, "_bin");
}

internal void lsp_code_action_find_modules_exporting_symbol_in_dir(
    Arena* arena, Array(string) * paths, cstr root, cstr dir, string symbol)
{
    DirIter iter = {0};
    if (!dir_iter_init(&iter, dir)) {
        return;
    }

    cstr path         = NULL;
    bool is_directory = false;
    while (dir_iter_next(&iter, arena, &path, &is_directory)) {
        string filename = path_filename(s(path));
        if (filename.count == 0 || filename.data[0] == '.') {
            continue;
        }

        if (is_directory) {
            if (!lsp_code_action_should_descend_module_dir(
                    root, path, filename)) {
                continue;
            }

            cstr mod_path = path_join(arena, path, "mod.n");
            if (path_exists(mod_path) &&
                lsp_code_action_file_exports_symbol(mod_path, symbol)) {
                string module_path = lsp_code_action_module_path_from_file(
                    arena, root, mod_path);
                lsp_code_action_add_module_path(paths, arena, module_path);
            }
            lsp_code_action_find_modules_exporting_symbol_in_dir(
                arena, paths, root, path, symbol);
            continue;
        }

        if (!path_has_extension(s(path), ".n") ||
            string_eq(filename, s("mod.n"))) {
            continue;
        }

        if (lsp_code_action_file_exports_symbol(path, symbol)) {
            string module_path =
                lsp_code_action_module_path_from_file(arena, root, path);
            lsp_code_action_add_module_path(paths, arena, module_path);
        }
    }

    dir_iter_done(&iter);
}

internal bool lsp_code_action_should_scan_module_root(cstr root)
{
    return root != NULL && path_exists(root) && path_is_directory(root) &&
           strcmp(root, "/") != 0 && strcmp(root, "\\") != 0 &&
           strcmp(root, ".") != 0;
}

internal void lsp_code_action_find_modules_exporting_symbol_in_env_roots(
    Arena* arena, Array(string) * paths, cstr lib_path, string symbol)
{
    if (lib_path == NULL || *lib_path == '\0') {
        return;
    }

#if OS_WINDOWS
    char separator = ';';
#else
    char separator = ':';
#endif

    const char* cursor = lib_path;
    while (*cursor != '\0') {
        const char* end = strchr(cursor, separator);
        usize       len = end != NULL ? (usize)(end - cursor) : strlen(cursor);
        if (len > 0) {
            char* root = arena_alloc(arena, len + 1);
            memcpy(root, cursor, len);
            root[len] = '\0';
            if (lsp_code_action_should_scan_module_root(root)) {
                lsp_code_action_find_modules_exporting_symbol_in_dir(
                    arena, paths, root, root, symbol);
            }
        }
        if (end == NULL) {
            break;
        }
        cursor = end + 1;
    }
}

internal void lsp_code_action_find_loaded_modules_exporting_symbol(
    Arena* arena, Array(string) * paths, const LspDocument* doc, string symbol)
{
    cstr cwd_mods     = path_canonical(arena, "mods");
    cstr exe_dir      = path_executable_dir(arena);
    cstr exe_mods     = path_join(arena, exe_dir, "mods");

    cstr current_root = NULL;
    cstr current_path =
        module_source_file_path(arena, doc->front_end.lexer.source);
    if (current_path != NULL) {
        current_root = path_dirname(arena, current_path);
    }

    cstr root_source_path =
        module_source_file_path(arena, doc->program.root_source);
    cstr program_root =
        root_source_path != NULL ? path_dirname(arena, root_source_path) : NULL;
    cstr current_mods =
        current_root != NULL ? path_join(arena, current_root, "mods") : NULL;
    cstr program_mods =
        program_root != NULL ? path_join(arena, program_root, "mods") : NULL;

    for (u32 i = 0; i < array_count(doc->program.modules); ++i) {
        LspModuleView module = {0};
        if (!lsp_program_module_view(&doc->program, i, &module) ||
            module.info->resolved_path == NULL) {
            continue;
        }

        for (u32 j = 0; j < lsp_module_export_count(&module); ++j) {
            const SemaDecl* decl = NULL;
            if (!lsp_module_export_decl(&module, j, &decl, NULL) ||
                !string_eq(lex_symbol(module.lexer, decl->symbol_handle),
                           symbol)) {
                continue;
            }

            string module_path = {0};
            if (cwd_mods != NULL) {
                module_path = lsp_code_action_module_path_from_file(
                    arena, cwd_mods, module.info->resolved_path);
            }
            if (module_path.count == 0 && path_exists(exe_mods)) {
                module_path = lsp_code_action_module_path_from_file(
                    arena, exe_mods, module.info->resolved_path);
            }
            if (module_path.count == 0 && current_mods != NULL &&
                path_exists(current_mods)) {
                module_path = lsp_code_action_module_path_from_file(
                    arena, current_mods, module.info->resolved_path);
            }
            if (module_path.count == 0 && program_mods != NULL &&
                path_exists(program_mods)) {
                module_path = lsp_code_action_module_path_from_file(
                    arena, program_mods, module.info->resolved_path);
            }
            if (module_path.count == 0 && current_root != NULL) {
                module_path = lsp_code_action_module_path_from_file(
                    arena, current_root, module.info->resolved_path);
            }
            if (module_path.count == 0 && program_root != NULL) {
                module_path = lsp_code_action_module_path_from_file(
                    arena, program_root, module.info->resolved_path);
            }
            lsp_code_action_add_module_path(paths, arena, module_path);
        }
    }
}

internal void lsp_code_action_find_modules_exporting_symbol(
    Arena* arena, Array(string) * paths, const LspDocument* doc, string symbol)
{
    lsp_code_action_find_loaded_modules_exporting_symbol(
        arena, paths, doc, symbol);

    cstr current_path =
        module_source_file_path(arena, doc->front_end.lexer.source);
    if (current_path != NULL) {
        cstr current_root = path_dirname(arena, current_path);
        if (lsp_code_action_should_scan_module_root(current_root)) {
            lsp_code_action_find_modules_exporting_symbol_in_dir(
                arena, paths, current_root, current_root, symbol);
        }
    }

    cstr root_source_path =
        module_source_file_path(arena, doc->program.root_source);
    if (root_source_path != NULL) {
        cstr program_root = path_dirname(arena, root_source_path);
        if (lsp_code_action_should_scan_module_root(program_root)) {
            lsp_code_action_find_modules_exporting_symbol_in_dir(
                arena, paths, program_root, program_root, symbol);
        }
    }

    cstr cwd_mods = path_canonical(arena, "mods");
    if (cwd_mods != NULL && path_exists(cwd_mods) &&
        path_is_directory(cwd_mods)) {
        lsp_code_action_find_modules_exporting_symbol_in_dir(
            arena, paths, cwd_mods, cwd_mods, symbol);
    }

    lsp_code_action_find_modules_exporting_symbol_in_env_roots(
        arena, paths, getenv("NERD_LIB_PATH"), symbol);

    cstr exe_dir  = path_executable_dir(arena);
    cstr mods_dir = path_join(arena, exe_dir, "mods");
    if (path_exists(mods_dir) && path_is_directory(mods_dir)) {
        lsp_code_action_find_modules_exporting_symbol_in_dir(
            arena, paths, mods_dir, mods_dir, symbol);
    }
}

internal usize lsp_code_action_use_insert_offset(string source)
{
    usize insert_offset   = 0;
    usize fallback_offset = 0;
    usize line_start      = 0;
    bool  in_header       = true;
    bool  saw_header      = false;
    bool  saw_use         = false;

    while (line_start < source.count) {
        usize line_end = line_start;
        while (line_end < source.count && source.data[line_end] != '\n') {
            line_end++;
        }
        usize next_line = line_end + (line_end < source.count ? 1 : 0);

        usize i         = line_start;
        lsp_code_action_skip_line_space(source, line_end, &i);

        bool blank     = i == line_end;
        bool comment   = i + 2 <= line_end && source.data[i] == '-' &&
                         source.data[i + 1] == '-';
        bool top_level = i == line_start;
        bool is_use    = false;

        if (top_level &&
            lsp_code_action_match_keyword(source, line_end, i, "pub", 3)) {
            i += 3;
            lsp_code_action_skip_line_space(source, line_end, &i);
        }

        if (top_level &&
            lsp_code_action_match_keyword(source, line_end, i, "use", 3)) {
            is_use = true;
        } else if (top_level && i < line_end &&
                   lsp_code_action_is_ident_char(source.data[i])) {
            while (i < line_end &&
                   lsp_code_action_is_ident_char(source.data[i])) {
                i++;
            }
            lsp_code_action_skip_line_space(source, line_end, &i);
            if (i + 2 <= line_end && source.data[i] == ':' &&
                source.data[i + 1] == ':') {
                i += 2;
                lsp_code_action_skip_line_space(source, line_end, &i);
                is_use = lsp_code_action_match_keyword(
                    source, line_end, i, "use", 3);
            }
        }

        if (in_header && !saw_use) {
            if (comment) {
                saw_header      = true;
                fallback_offset = next_line;
            } else if (blank) {
                if (saw_header) {
                    fallback_offset = next_line;
                }
            } else {
                in_header = false;
            }
        }

        if (is_use) {
            saw_use       = true;
            insert_offset = next_line;
        } else if (saw_use && !blank && !comment) {
            break;
        }

        line_start = next_line;
    }

    return saw_use ? insert_offset : fallback_offset;
}

internal bool lsp_code_action_doc_has_use(Arena*             arena,
                                          const LspDocument* doc,
                                          string             module_path)
{
    const Ast*   ast   = &doc->front_end.ast;
    const Lexer* lexer = &doc->front_end.lexer;
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* use = &ast->nodes[i];
        if (use->kind != AK_Use || use->a >= array_count(ast->nodes)) {
            continue;
        }

        const AstNode* path_node = &ast->nodes[use->a];
        if (path_node->kind != AK_ModRef ||
            path_node->a >= array_count(ast->module_paths)) {
            continue;
        }

        cstr existing = module_path_to_qualified_name(
            arena, lexer, ast, &ast->module_paths[path_node->a]);
        if (existing != NULL && string_eq(s(existing), module_path)) {
            return true;
        }
    }
    return false;
}

internal void lsp_code_action_add_import_actions(Arena*             arena,
                                                 JsonValue*         actions,
                                                 string             uri,
                                                 const LspDocument* doc,
                                                 string             symbol)
{
    Array(string) module_paths = NULL;
    lsp_code_action_find_modules_exporting_symbol(
        arena, &module_paths, doc, symbol);

    usize insert_offset = lsp_code_action_use_insert_offset(doc->source);
    for (u32 i = 0; i < array_count(module_paths); ++i) {
        string module_path = module_paths[i];
        if (lsp_code_action_doc_has_use(arena, doc, module_path)) {
            continue;
        }

        StringBuilder text = {0};
        sb_init(&text, arena);
        sb_append_cstr(&text, "use ");
        sb_append_string(&text, module_path);
        sb_append_char(&text, '\n');
        if (doc->source.count > 0 &&
            (insert_offset == 0 || (insert_offset < doc->source.count &&
                                    doc->source.data[insert_offset] != '\n'))) {
            sb_append_char(&text, '\n');
        }

        JsonValue* workspace_edit =
            lsp_code_action_workspace_edit(arena,
                                           uri,
                                           doc->front_end.lexer.source,
                                           insert_offset,
                                           sb_to_string(&text));
        if (workspace_edit == NULL) {
            continue;
        }

        StringBuilder title = {0};
        sb_init(&title, arena);
        sb_append_cstr(&title, "Add use ");
        sb_append_string(&title, module_path);

        JsonValue* action = json_new_object(arena);
        json_object_set_string(action, arena, "title", sb_to_string(&title));
        json_object_set_string(action, arena, "kind", s("quickfix"));
        json_object_set_object(action, "edit", workspace_edit);
        json_array_push(actions, action);
    }

    array_free(module_paths);
}

internal usize lsp_code_action_type_node_end_offset(const LspDocument* doc,
                                                    u32 type_node_index)
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
            usize end = lsp_code_action_type_node_end_offset(doc, node->a);
            return end != 0 ? end
                            : lex_token_end_offset(
                                  lexer, &lexer->tokens[node->token_index]);
        }
    case AK_TypeArray:
    case AK_TypeDynamicArray:
        {
            usize end = lsp_code_action_type_node_end_offset(doc, node->b);
            return end != 0 ? end
                            : lex_token_end_offset(
                                  lexer, &lexer->tokens[node->token_index]);
        }
    case AK_TypeTuple:
        {
            if (node->b != 0) {
                u32 last_item = node->a + node->b - 1;
                if (last_item < array_count(ast->tuple_items)) {
                    usize end = lsp_code_action_type_node_end_offset(
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
                        usize end = lsp_code_action_type_node_end_offset(
                            doc, ast->tuple_items[last_arg]);
                        if (end != 0) {
                            return end;
                        }
                    }
                }
                usize end = lsp_code_action_type_node_end_offset(
                    doc, apply->target_node_index);
                if (end != 0) {
                    return end;
                }
            }
            return lex_token_end_offset(lexer,
                                        &lexer->tokens[node->token_index]);
        }
    default:
        return lex_token_end_offset(lexer, &lexer->tokens[node->token_index]);
    }
}

internal string lsp_code_action_type_node_source(const LspDocument* doc,
                                                 u32 type_node_index)
{
    const Ast*   ast   = &doc->front_end.ast;
    const Lexer* lexer = &doc->front_end.lexer;
    if (type_node_index >= array_count(ast->nodes)) {
        return s("<unknown>");
    }
    const AstNode* node = &ast->nodes[type_node_index];
    if (node->token_index >= array_count(lexer->tokens)) {
        return s("<unknown>");
    }

    usize start = lexer->tokens[node->token_index].offset;
    usize end   = lsp_code_action_type_node_end_offset(doc, type_node_index);
    if (end <= start || end > lexer->source.source.count) {
        return s("<unknown>");
    }
    return lsp_code_action_trim_source(doc, start, end);
}

internal bool lsp_code_action_ast_default_enum(const Ast*   ast,
                                               const Lexer* lexer,
                                               u32          enum_index,
                                               string*      out_value)
{
    if (enum_index >= array_count(ast->enum_types)) {
        return false;
    }

    const AstEnumTypeInfo* enum_type = &ast->enum_types[enum_index];
    if (enum_type->variant_count == 0) {
        return false;
    }

    const AstEnumVariant* variant =
        &ast->enum_variants[enum_type->first_variant];
    if (variant->type_node_index != U32_MAX) {
        return false;
    }

    *out_value = lex_symbol(lexer, variant->symbol_handle);
    return true;
}

internal bool lsp_code_action_ast_default_value(Arena*       arena,
                                                const Ast*   ast,
                                                const Lexer* lexer,
                                                u32          type_node_index,
                                                string*      out_value)
{
    if (type_node_index >= array_count(ast->nodes)) {
        return false;
    }

    const AstNode* type_node = &ast->nodes[type_node_index];
    switch (type_node->kind) {
    case AK_SymbolRef:
        {
            string name = lex_symbol(lexer, type_node->a);
            if (string_eq_cstr(name, "bool")) {
                *out_value = s("no");
                return true;
            }
            if (string_eq_cstr(name, "string")) {
                *out_value = s("\"\"");
                return true;
            }
            if (string_eq_cstr(name, "i8") || string_eq_cstr(name, "i16") ||
                string_eq_cstr(name, "i32") || string_eq_cstr(name, "i64") ||
                string_eq_cstr(name, "u8") || string_eq_cstr(name, "u16") ||
                string_eq_cstr(name, "u32") || string_eq_cstr(name, "u64") ||
                string_eq_cstr(name, "isize") ||
                string_eq_cstr(name, "usize") || string_eq_cstr(name, "f32") ||
                string_eq_cstr(name, "f64")) {
                *out_value = s("0");
                return true;
            }

            u32 alias_node =
                lsp_code_action_find_ast_type_alias(ast, type_node->a);
            if (alias_node == U32_MAX) {
                return false;
            }

            const AstNode* alias = &ast->nodes[alias_node];
            if (alias->kind == AK_TypePlex) {
                return lsp_code_action_ast_default_record(
                    arena, ast, lexer, alias->a, out_value);
            }
            if (alias->kind == AK_TypeEnum) {
                return lsp_code_action_ast_default_enum(
                    ast, lexer, alias->a, out_value);
            }
            return false;
        }
    case AK_TypePointer:
    case AK_TypeSlice:
    case AK_TypeDynamicArray:
        *out_value = s("nil");
        return true;
    case AK_TypeTuple:
        {
            StringBuilder sb = {0};
            sb_init(&sb, arena);
            sb_append_char(&sb, '(');
            for (u32 i = 0; i < type_node->b; ++i) {
                string item = {0};
                if (!lsp_code_action_ast_default_value(
                        arena, ast, lexer, type_node->a + i, &item)) {
                    return false;
                }
                if (i > 0) {
                    sb_append_cstr(&sb, ", ");
                }
                sb_append_string(&sb, item);
            }
            sb_append_char(&sb, ')');
            *out_value = sb_to_string(&sb);
            return true;
        }
    case AK_TypePlex:
        return lsp_code_action_ast_default_record(
            arena, ast, lexer, type_node->a, out_value);
    case AK_TypeEnum:
        return lsp_code_action_ast_default_enum(
            ast, lexer, type_node->a, out_value);
    default:
        return false;
    }
}

internal bool lsp_code_action_default_value(Arena*       arena,
                                            const Lexer* lexer,
                                            const Sema*  sema,
                                            u32          type_index,
                                            string*      out_value)
{
    if (type_index == sema_no_type() ||
        type_index >= array_count(sema->types)) {
        return false;
    }

    type_index           = sema_materialise_type(sema, type_index);
    const SemaType* type = &sema->types[type_index];
    switch (type->kind) {
    case STK_Bool:
        *out_value = s("no");
        return true;
    case STK_String:
        *out_value = s("\"\"");
        return true;
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
    case STK_F32:
    case STK_F64:
        *out_value = s("0");
        return true;
    case STK_Pointer:
    case STK_Slice:
    case STK_DynamicArray:
        *out_value = s("nil");
        return true;
    case STK_Tuple:
        {
            StringBuilder sb = {0};
            sb_init(&sb, arena);
            sb_append_char(&sb, '(');
            for (u32 i = 0; i < type->param_count; ++i) {
                if (i > 0) {
                    sb_append_cstr(&sb, ", ");
                }
                string item = {0};
                if (!lsp_code_action_default_value(
                        arena,
                        lexer,
                        sema,
                        sema->type_param_types[type->first_param_type + i],
                        &item)) {
                    return false;
                }
                sb_append_string(&sb, item);
            }
            sb_append_char(&sb, ')');
            *out_value = sb_to_string(&sb);
            return true;
        }
    case STK_Plex:
        {
            StringBuilder sb = {0};
            sb_init(&sb, arena);
            sb_append_cstr(&sb, "{ ");
            for (u32 i = 0; i < type->param_count; ++i) {
                if (i > 0) {
                    sb_append_cstr(&sb, " ");
                }
                string field_value = {0};
                if (!lsp_code_action_default_value(
                        arena,
                        lexer,
                        sema,
                        sema->type_param_types[type->first_param_type + i],
                        &field_value)) {
                    return false;
                }
                sb_append_string(
                    &sb,
                    lex_symbol(
                        lexer,
                        sema->type_param_symbols[type->first_param_type + i]));
                sb_append_cstr(&sb, ": ");
                sb_append_string(&sb, field_value);
            }
            sb_append_cstr(&sb, " }");
            *out_value = sb_to_string(&sb);
            return true;
        }
    case STK_Enum:
        if (type->param_count > 0 &&
            sema->type_param_types[type->first_param_type] == sema_no_type()) {
            *out_value = lex_symbol(
                lexer, sema->type_param_symbols[type->first_param_type]);
            return true;
        }
        return false;
    default:
        return false;
    }
}

internal u32 lsp_code_action_find_plex_literal_at_offset(const LspDocument* doc,
                                                         usize offset,
                                                         u32*  out_close_token)
{
    const Ast*   ast   = &doc->front_end.ast;
    const Lexer* lexer = &doc->front_end.lexer;

    u32   best_node    = U32_MAX;
    usize best_width   = (usize)-1;
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind != AK_Plex) {
            continue;
        }
        u32 close_token = U32_MAX;
        if (!lsp_code_action_matching_close(
                lexer, node->token_index, &close_token)) {
            continue;
        }
        const AstPlexLiteralInfo* literal     = &ast->plex_literals[node->a];
        u32                       start_token = node->token_index;
        u32 target_node_index                 = literal->target_node_index;
        if (target_node_index < array_count(ast->nodes)) {
            start_token = ast->nodes[target_node_index].token_index;
            while (ast->nodes[target_node_index].kind == AK_Field) {
                u32 receiver_index = ast->nodes[target_node_index].a;
                if (receiver_index >= array_count(ast->nodes)) {
                    break;
                }
                start_token       = ast->nodes[receiver_index].token_index;
                target_node_index = receiver_index;
            }
        }
        usize start = lexer->tokens[start_token].offset;
        usize end   = lex_token_end_offset(lexer, &lexer->tokens[close_token]);
        if (offset < start || offset > end) {
            continue;
        }
        usize width = end - start;
        if (width < best_width) {
            best_node        = i;
            best_width       = width;
            *out_close_token = close_token;
        }
    }
    return best_node;
}

internal bool lsp_code_action_missing_plex_fields(Arena*             arena,
                                                  const LspDocument* doc,
                                                  u32                node_index,
                                                  string* out_insert_text)
{
    const Ast*                ast     = &doc->front_end.ast;
    const Lexer*              lexer   = &doc->front_end.lexer;
    const Sema*               sema    = &doc->front_end.sema;
    const AstNode*            node    = &ast->nodes[node_index];
    const AstPlexLiteralInfo* literal = &ast->plex_literals[node->a];

    u32 type_index                    = sema_no_type();
    if (!lsp_code_action_resolve_plex_literal_type(
            ast, sema, node_index, &type_index) ||
        type_index >= array_count(sema->types)) {
        return false;
    }

    type_index = sema_materialise_type(sema, type_index);
    if (sema->types[type_index].kind != STK_Plex) {
        return false;
    }

    const SemaType* plex = &sema->types[type_index];
    bool*           seen = arena_alloc(arena, sizeof(bool) * plex->param_count);
    memset(seen, 0, sizeof(bool) * plex->param_count);
    for (u32 i = 0; i < literal->field_count; ++i) {
        const AstPlexLiteralField* field =
            &ast->plex_literal_fields[literal->first_field + i];
        for (u32 j = 0; j < plex->param_count; ++j) {
            if (sema->type_param_symbols[plex->first_param_type + j] ==
                field->symbol_handle) {
                seen[j] = true;
                break;
            }
        }
    }

    string base_indent = lsp_code_action_line_indent(
        arena, doc->source, lexer->tokens[node->token_index].offset);
    StringBuilder sb = {0};
    sb_init(&sb, arena);
    usize field_name_width = 0;
    for (u32 i = 0; i < plex->param_count; ++i) {
        string name = lex_symbol(
            lexer, sema->type_param_symbols[plex->first_param_type + i]);
        if (name.count > field_name_width) {
            field_name_width = name.count;
        }
    }

    bool needs_leading_newline = literal->field_count > 0;
    u32  missing_count         = 0;
    for (u32 i = 0; i < plex->param_count; ++i) {
        if (seen[i]) {
            continue;
        }
        string value = {0};
        if (!lsp_code_action_default_value(
                arena,
                lexer,
                sema,
                sema->type_param_types[plex->first_param_type + i],
                &value)) {
            return false;
        }
        if (needs_leading_newline) {
            sb_append_char(&sb, '\n');
        }
        needs_leading_newline = true;
        sb_append_string(&sb, base_indent);
        sb_append_cstr(&sb, "    ");
        string name = lex_symbol(
            lexer, sema->type_param_symbols[plex->first_param_type + i]);
        sb_append_string(&sb, name);
        lsp_code_action_append_spaces(&sb, field_name_width - name.count);
        sb_append_cstr(&sb, ": ");
        sb_append_string(&sb, value);
        missing_count++;
    }

    if (missing_count == 0) {
        return false;
    }

    sb_append_char(&sb, '\n');
    sb_append_string(&sb, base_indent);
    *out_insert_text = sb_to_string(&sb);
    return true;
}

internal bool lsp_code_action_symbol_names_equal(const Lexer* a_lexer,
                                                 u32          a_symbol,
                                                 const Lexer* b_lexer,
                                                 u32          b_symbol)
{
    return string_eq(lex_symbol(a_lexer, a_symbol),
                     lex_symbol(b_lexer, b_symbol));
}

internal bool
lsp_code_action_missing_ast_plex_fields_from_type(Arena*             arena,
                                                  const LspDocument* doc,
                                                  u32                node_index,
                                                  const Ast*         type_ast,
                                                  const Lexer*       type_lexer,
                                                  u32     type_plex_index,
                                                  string* out_insert_text)
{
    const Ast*                ast     = &doc->front_end.ast;
    const Lexer*              lexer   = &doc->front_end.lexer;
    const AstNode*            node    = &ast->nodes[node_index];
    const AstPlexLiteralInfo* literal = &ast->plex_literals[node->a];

    if (type_plex_index >= array_count(type_ast->plex_types)) {
        return false;
    }

    const AstPlexTypeInfo* plex = &type_ast->plex_types[type_plex_index];
    bool* seen = arena_alloc(arena, sizeof(bool) * plex->field_count);
    memset(seen, 0, sizeof(bool) * plex->field_count);
    for (u32 i = 0; i < literal->field_count; ++i) {
        const AstPlexLiteralField* field =
            &ast->plex_literal_fields[literal->first_field + i];
        for (u32 j = 0; j < plex->field_count; ++j) {
            const AstPlexField* plex_field =
                &type_ast->plex_fields[plex->first_field + j];
            if (lsp_code_action_symbol_names_equal(lexer,
                                                   field->symbol_handle,
                                                   type_lexer,
                                                   plex_field->symbol_handle)) {
                seen[j] = true;
                break;
            }
        }
    }

    string base_indent = lsp_code_action_line_indent(
        arena, doc->source, lexer->tokens[node->token_index].offset);
    StringBuilder sb = {0};
    sb_init(&sb, arena);
    usize field_name_width = 0;
    for (u32 i = 0; i < plex->field_count; ++i) {
        const AstPlexField* field =
            &type_ast->plex_fields[plex->first_field + i];
        string name = lex_symbol(type_lexer, field->symbol_handle);
        if (name.count > field_name_width) {
            field_name_width = name.count;
        }
    }

    bool needs_leading_newline = literal->field_count > 0;
    u32  missing_count         = 0;
    for (u32 i = 0; i < plex->field_count; ++i) {
        if (seen[i]) {
            continue;
        }

        const AstPlexField* field =
            &type_ast->plex_fields[plex->first_field + i];
        string value = {0};
        if (!lsp_code_action_ast_default_value(
                arena, type_ast, type_lexer, field->type_node_index, &value)) {
            return false;
        }

        if (needs_leading_newline) {
            sb_append_char(&sb, '\n');
        }
        needs_leading_newline = true;
        sb_append_string(&sb, base_indent);
        sb_append_cstr(&sb, "    ");
        string name = lex_symbol(type_lexer, field->symbol_handle);
        sb_append_string(&sb, name);
        lsp_code_action_append_spaces(&sb, field_name_width - name.count);
        sb_append_cstr(&sb, ": ");
        sb_append_string(&sb, value);
        missing_count++;
    }

    if (missing_count == 0) {
        return false;
    }

    sb_append_char(&sb, '\n');
    sb_append_string(&sb, base_indent);
    *out_insert_text = sb_to_string(&sb);
    return true;
}

internal bool lsp_code_action_missing_ast_plex_fields(Arena*             arena,
                                                      const LspDocument* doc,
                                                      u32     node_index,
                                                      string* out_insert_text)
{
    const Ast*                ast     = &doc->front_end.ast;
    const AstNode*            node    = &ast->nodes[node_index];
    const AstPlexLiteralInfo* literal = &ast->plex_literals[node->a];

    if (literal->target_node_index >= array_count(ast->nodes)) {
        return false;
    }

    const AstNode* target = &ast->nodes[literal->target_node_index];
    if (target->kind != AK_SymbolRef) {
        return false;
    }

    u32 alias_node_index = lsp_code_action_find_ast_type_alias(ast, target->a);
    if (alias_node_index >= array_count(ast->nodes)) {
        return false;
    }

    const AstNode* alias = &ast->nodes[alias_node_index];
    if (alias->kind != AK_TypePlex ||
        alias->a >= array_count(ast->plex_types)) {
        return false;
    }

    return lsp_code_action_missing_ast_plex_fields_from_type(
        arena,
        doc,
        node_index,
        ast,
        &doc->front_end.lexer,
        alias->a,
        out_insert_text);
}

internal u32 lsp_code_action_ast_module_path_for_binding(const LspDocument* doc,
                                                         u32 module_symbol)
{
    const Ast* ast = &doc->front_end.ast;

    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind != AK_Bind || node->a != module_symbol ||
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

internal bool
lsp_code_action_missing_imported_ast_plex_fields(Arena*             arena,
                                                 const LspDocument* doc,
                                                 u32                node_index,
                                                 string* out_insert_text)
{
    const Ast*                ast     = &doc->front_end.ast;
    const Lexer*              lexer   = &doc->front_end.lexer;
    const AstNode*            node    = &ast->nodes[node_index];
    const AstPlexLiteralInfo* literal = &ast->plex_literals[node->a];

    if (literal->target_node_index >= array_count(ast->nodes)) {
        return false;
    }

    const AstNode* target = &ast->nodes[literal->target_node_index];
    if (target->kind != AK_Field || target->a >= array_count(ast->nodes)) {
        return false;
    }

    const AstNode* receiver = &ast->nodes[target->a];
    if (receiver->kind != AK_SymbolRef) {
        return false;
    }

    u32 module_path_index =
        lsp_code_action_ast_module_path_for_binding(doc, receiver->a);
    if (module_path_index == U32_MAX) {
        return false;
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
                            lexer,
                            ast,
                            &ast->module_paths[module_path_index],
                            &resolved);
    if (status != MRS_Found) {
        arena_done(&temp);
        return false;
    }

    LspModuleView module = {0};
    if (!lsp_program_module_view_by_path(
            &doc->program, resolved.resolved_path, &module)) {
        arena_done(&temp);
        return false;
    }

    const Ast*   module_ast   = module.ast;
    const Lexer* module_lexer = module.lexer;
    string       target_name  = lex_symbol(lexer, target->b);

    for (u32 i = 0; i < lsp_module_export_count(&module); ++i) {
        const SemaDecl* decl = NULL;
        if (!lsp_module_export_decl(&module, i, &decl, NULL)) {
            continue;
        }

        if (decl->kind != SK_TypeAlias ||
            !string_eq(lex_symbol(module_lexer, decl->symbol_handle),
                       target_name) ||
            decl->value_node_index >= array_count(module_ast->nodes)) {
            continue;
        }

        u32 value_node_index = lsp_code_action_unwrap_type_value_node(
            module_ast, decl->value_node_index);
        if (value_node_index >= array_count(module_ast->nodes)) {
            continue;
        }

        const AstNode* value = &module_ast->nodes[value_node_index];
        if (value->kind != AK_TypePlex) {
            continue;
        }

        bool ok =
            lsp_code_action_missing_ast_plex_fields_from_type(arena,
                                                              doc,
                                                              node_index,
                                                              module_ast,
                                                              module_lexer,
                                                              value->a,
                                                              out_insert_text);
        arena_done(&temp);
        return ok;
    }

    arena_done(&temp);
    return false;
}

internal bool lsp_code_action_impl_has_member(const Ast*     ast,
                                              const AstNode* impl_body,
                                              u32            symbol)
{
    if (impl_body->kind != AK_Block) {
        return false;
    }

    for (u32 i = impl_body->a; i < impl_body->b; ++i) {
        const AstNode* member = &ast->nodes[i];
        if (ast_node_is_binding_like(member) &&
            ast_get_symbol(member) == symbol) {
            return true;
        }
    }
    return false;
}

internal u32 lsp_code_action_find_trait_impl_at_offset(
    const LspDocument* doc, usize offset, u32* out_close_token_index)
{
    const Ast*   ast   = &doc->front_end.ast;
    const Lexer* lexer = &doc->front_end.lexer;

    u32   best_node    = U32_MAX;
    usize best_span    = (usize)-1;
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind != AK_Impl || node->a >= array_count(ast->impls)) {
            continue;
        }

        const AstImplInfo* impl = &ast->impls[node->a];
        if (impl->trait_type_node_index == U32_MAX ||
            impl->body_node_index >= array_count(ast->nodes)) {
            continue;
        }

        const AstNode* body = &ast->nodes[impl->body_node_index];
        if (body->kind != AK_Block) {
            continue;
        }

        u32 close_token = U32_MAX;
        if (!lsp_code_action_matching_close(
                lexer, body->token_index, &close_token)) {
            continue;
        }

        usize start = lexer->tokens[node->token_index].offset;
        usize end   = lex_token_end_offset(lexer, &lexer->tokens[close_token]);
        if (offset < start || offset > end) {
            continue;
        }

        usize span = end - start;
        if (span < best_span) {
            best_node              = i;
            best_span              = span;
            *out_close_token_index = close_token;
        }
    }

    return best_node;
}

internal bool lsp_code_action_trait_node_for_impl(const LspDocument* doc,
                                                  const AstImplInfo* impl,
                                                  u32* out_trait_node_index)
{
    const Ast*  ast  = &doc->front_end.ast;
    const Sema* sema = &doc->front_end.sema;
    if (impl->trait_type_node_index >= array_count(ast->nodes)) {
        return false;
    }

    const AstNode* trait_type = &ast->nodes[impl->trait_type_node_index];
    if (trait_type->kind == AK_SymbolRef) {
        for (u32 i = 0; i < array_count(ast->nodes); ++i) {
            const AstNode* candidate = &ast->nodes[i];
            if (candidate->kind != AK_Bind || candidate->a != trait_type->a ||
                candidate->b >= array_count(ast->nodes)) {
                continue;
            }

            if (ast->nodes[candidate->b].kind == AK_Trait) {
                *out_trait_node_index = candidate->b;
                return true;
            }
        }
    }

    u32 decl_index = sema_no_decl();
    if (!lsp_sema_node_decl(sema, impl->trait_type_node_index, &decl_index)) {
        if (trait_type->kind != AK_SymbolRef) {
            return false;
        }
        decl_index = lsp_code_action_find_decl(sema, trait_type->a);
    }

    const SemaDecl* decl = NULL;
    if (!lsp_sema_decl(sema, decl_index, &decl) || decl->kind != SK_Trait ||
        decl->import_module_index != sema_no_decl() ||
        decl->value_node_index >= array_count(ast->nodes)) {
        return false;
    }

    *out_trait_node_index = decl->value_node_index;
    return true;
}

internal void lsp_code_action_append_param_stub_name(Arena*          arena,
                                                     StringBuilder*  sb,
                                                     const Lexer*    lexer,
                                                     const AstParam* param,
                                                     u32             index)
{
    sb_append_char(sb, '_');
    if (param->symbol_handle != U32_MAX) {
        string name = lex_symbol(lexer, param->symbol_handle);
        if (name.count > 0 && name.data[0] == '_') {
            name = string_from(name.data + 1, name.count - 1);
        }
        if (name.count > 0) {
            sb_append_string(sb, name);
            return;
        }
    }

    sb_append_string(
        sb, index == 0 ? s("self") : string_format(arena, "param%u", index));
}

internal bool lsp_code_action_append_trait_member_stub(Arena*             arena,
                                                       StringBuilder*     sb,
                                                       const LspDocument* doc,
                                                       const AstNode* required,
                                                       string member_indent,
                                                       string body_indent)
{
    const Ast* ast = &doc->front_end.ast;
    if (required->kind != AK_Bind || required->b >= array_count(ast->nodes)) {
        return false;
    }

    const AstNode* value = &ast->nodes[required->b];
    if (value->kind != AK_TypeFn ||
        value->a >= array_count(ast->fn_signatures)) {
        return false;
    }

    const Lexer*          lexer     = &doc->front_end.lexer;
    const AstFnSignature* signature = &ast->fn_signatures[value->a];
    sb_append_string(sb, member_indent);
    sb_append_string(sb, lex_symbol(lexer, ast_get_symbol(required)));
    sb_append_cstr(sb, " :: fn (");
    for (u32 i = 0; i < signature->param_count; ++i) {
        if (i > 0) {
            sb_append_cstr(sb, ", ");
        }
        const AstParam* param = &ast->params[signature->first_param + i];
        lsp_code_action_append_param_stub_name(arena, sb, lexer, param, i);
        sb_append_cstr(sb, ": ");
        sb_append_string(
            sb, lsp_code_action_type_node_source(doc, param->type_node_index));
    }
    if (signature->is_varargs) {
        if (signature->param_count > 0) {
            sb_append_cstr(sb, ", ");
        }
        sb_append_cstr(sb, "...");
    }
    sb_append_char(sb, ')');
    if (signature->return_type_node_index != U32_MAX) {
        sb_append_cstr(sb, " -> ");
        sb_append_string(sb,
                         lsp_code_action_type_node_source(
                             doc, signature->return_type_node_index));
    }
    sb_append_cstr(sb, " {\n");
    if (signature->return_type_node_index != U32_MAX) {
        sb_append_string(sb, body_indent);
        sb_append_cstr(sb, "result: ");
        sb_append_string(sb,
                         lsp_code_action_type_node_source(
                             doc, signature->return_type_node_index));
        sb_append_char(sb, '\n');
        sb_append_string(sb, body_indent);
        sb_append_cstr(sb, "return result\n");
    }
    sb_append_string(sb, member_indent);
    sb_append_char(sb, '}');
    return true;
}

internal bool
lsp_code_action_missing_trait_impl_members(Arena*             arena,
                                           const LspDocument* doc,
                                           u32                impl_node_index,
                                           usize              insert_offset,
                                           string*            out_insert_text)
{
    const Ast* ast = &doc->front_end.ast;
    if (impl_node_index >= array_count(ast->nodes)) {
        return false;
    }

    const AstNode* impl_node = &ast->nodes[impl_node_index];
    if (impl_node->kind != AK_Impl || impl_node->a >= array_count(ast->impls)) {
        return false;
    }

    const AstImplInfo* impl = &ast->impls[impl_node->a];
    if (impl->body_node_index >= array_count(ast->nodes)) {
        return false;
    }
    const AstNode* impl_body = &ast->nodes[impl->body_node_index];
    if (impl_body->kind != AK_Block) {
        return false;
    }

    u32 trait_node_index = U32_MAX;
    if (!lsp_code_action_trait_node_for_impl(doc, impl, &trait_node_index)) {
        return false;
    }

    const AstNode* trait_node = &ast->nodes[trait_node_index];
    if (trait_node->kind != AK_Trait ||
        trait_node->a >= array_count(ast->trait_infos)) {
        return false;
    }

    const AstTraitInfo* trait = &ast->trait_infos[trait_node->a];
    if (trait->body_node_index >= array_count(ast->nodes)) {
        return false;
    }

    const AstNode* trait_body = &ast->nodes[trait->body_node_index];
    if (trait_body->kind != AK_Block) {
        return false;
    }

    string base_indent = lsp_code_action_line_indent(
        arena,
        doc->source,
        doc->front_end.lexer.tokens[impl_node->token_index].offset);
    StringBuilder member_indent = {0};
    sb_init(&member_indent, arena);
    sb_append_string(&member_indent, base_indent);
    sb_append_cstr(&member_indent, "    ");

    StringBuilder body_indent = {0};
    sb_init(&body_indent, arena);
    sb_append_string(&body_indent, sb_to_string(&member_indent));
    sb_append_cstr(&body_indent, "    ");

    StringBuilder sb = {0};
    sb_init(&sb, arena);
    if (insert_offset > 0 && doc->source.data[insert_offset - 1] != '\n') {
        sb_append_char(&sb, '\n');
    } else if (impl_body->a != impl_body->b) {
        sb_append_char(&sb, '\n');
    }

    u32 missing_count = 0;
    for (u32 i = trait_body->a; i < trait_body->b; ++i) {
        const AstNode* required = &ast->nodes[i];
        if (required->kind != AK_Bind) {
            continue;
        }

        if (lsp_code_action_impl_has_member(
                ast, impl_body, ast_get_symbol(required))) {
            continue;
        }

        if (missing_count > 0) {
            sb_append_cstr(&sb, "\n\n");
        }
        if (lsp_code_action_append_trait_member_stub(
                arena,
                &sb,
                doc,
                required,
                sb_to_string(&member_indent),
                sb_to_string(&body_indent))) {
            missing_count++;
        }
    }

    if (missing_count == 0) {
        return false;
    }

    sb_append_char(&sb, '\n');
    sb_append_string(&sb, base_indent);
    *out_insert_text = sb_to_string(&sb);
    return true;
}

internal void lsp_code_action_add_trait_impl_stub_action(Arena*     arena,
                                                         JsonValue* actions,
                                                         string     uri,
                                                         const LspDocument* doc,
                                                         usize offset)
{
    u32 close_token = U32_MAX;
    u32 impl_node_index =
        lsp_code_action_find_trait_impl_at_offset(doc, offset, &close_token);
    if (impl_node_index == U32_MAX || close_token == U32_MAX) {
        return;
    }

    usize  insert_offset = doc->front_end.lexer.tokens[close_token].offset;
    string insert_text   = {0};
    if (!lsp_code_action_missing_trait_impl_members(
            arena, doc, impl_node_index, insert_offset, &insert_text)) {
        return;
    }

    JsonValue* workspace_edit = lsp_code_action_workspace_edit(
        arena, uri, doc->front_end.lexer.source, insert_offset, insert_text);
    if (workspace_edit == NULL) {
        return;
    }

    JsonValue* action = json_new_object(arena);
    json_object_set_string(
        action, arena, "title", s("Stub missing trait members"));
    json_object_set_string(action, arena, "kind", s("quickfix"));
    json_object_set_object(action, "edit", workspace_edit);
    json_array_push(actions, action);
}

void lsp_handle_code_action(LspState* state, const LspMessage* message)
{
    JsonValue* response = lsp_prepare_response(message);
    JsonValue* actions  = json_new_array(message->arena);

    string uri          = {0};
    u64    line         = 0;
    u64    character    = 0;
    if (!lsp_get_string_param(message, "params.textDocument.uri", &uri) ||
        !lsp_get_u64_param(message, "params.range.start.line", &line) ||
        !lsp_get_u64_param(
            message, "params.range.start.character", &character)) {
        json_object_set_array(response, "result", actions);
        lsp_send_response(message->arena, response);
        return;
    }

    LspSyntaxView syntax = {0};
    if (!lsp_syntax_view(state, uri, &syntax)) {
        json_object_set_array(response, "result", actions);
        lsp_send_response(message->arena, response);
        return;
    }
    const LspDocument* doc = syntax.doc;

    lsp_code_action_add_unknown_member_suggestions(
        message->arena, actions, uri, message);

    usize offset      = lsp_offset_from_position(doc->source, line, character);
    u32   token_index = U32_MAX;
    bool  has_symbol_token = lsp_code_action_symbol_token_at_offset(
        &doc->front_end.lexer, offset, &token_index);
    if (has_symbol_token) {
        const Token* token = &doc->front_end.lexer.tokens[token_index];
        usize        end   = lex_token_end_offset(&doc->front_end.lexer, token);
        string       symbol =
            string_from(doc->front_end.lexer.source.source.data + token->offset,
                        end - token->offset);
        if (lsp_code_action_has_unused_local_diagnostic(
                message->arena, message, symbol)) {
            LspBindingView binding = {0};
            if (lsp_binding_view(state, uri, &binding)) {
                lsp_code_action_add_unused_local_rename(
                    message->arena, actions, uri, binding.doc, token_index);
            }
        }

        if (!lsp_code_action_doc_declares_symbol(doc, symbol) &&
            lsp_code_action_symbol_needs_import(doc, token_index)) {
            lsp_code_action_add_import_actions(
                message->arena, actions, uri, doc, symbol);
        }
    }

    LspTypeFactView view = {0};
    if (!lsp_type_fact_view(state, uri, &view)) {
        json_object_set_array(response, "result", actions);
        lsp_send_response(message->arena, response);
        return;
    }

    lsp_code_action_add_trait_impl_stub_action(
        message->arena, actions, uri, doc, offset);

    u32 close_token = U32_MAX;
    u32 node_index =
        lsp_code_action_find_plex_literal_at_offset(doc, offset, &close_token);
    if (node_index == U32_MAX || close_token == U32_MAX) {
        lsp_log("code action: no plex literal");
        json_object_set_array(response, "result", actions);
        lsp_send_response(message->arena, response);
        return;
    }

    string insert_text = {0};
    if (!lsp_code_action_missing_plex_fields(
            message->arena, doc, node_index, &insert_text) &&
        !lsp_code_action_missing_ast_plex_fields(
            message->arena, doc, node_index, &insert_text) &&
        !lsp_code_action_missing_imported_ast_plex_fields(
            message->arena, doc, node_index, &insert_text)) {
        lsp_log("code action: no missing fields");
        json_object_set_array(response, "result", actions);
        lsp_send_response(message->arena, response);
        return;
    }

    const Lexer* lexer         = &doc->front_end.lexer;
    usize        insert_offset = lexer->tokens[close_token].offset;
    JsonValue*   range         = lsp_code_action_range(
        message->arena, lexer->source, insert_offset, insert_offset);
    if (range == NULL) {
        json_object_set_array(response, "result", actions);
        lsp_send_response(message->arena, response);
        return;
    }

    JsonValue* edit = json_new_object(message->arena);
    json_object_set_object(edit, "range", range);
    json_object_set_string(edit, message->arena, "newText", insert_text);

    JsonValue* edits = json_new_array(message->arena);
    json_array_push(edits, edit);

    JsonValue* changes = json_new_object(message->arena);
    json_object_set_array(
        changes, lsp_code_action_cstr(message->arena, uri), edits);

    JsonValue* workspace_edit = json_new_object(message->arena);
    json_object_set_object(workspace_edit, "changes", changes);

    JsonValue* action = json_new_object(message->arena);
    json_object_set_string(
        action, message->arena, "title", s("Fill missing plex fields"));
    json_object_set_string(action, message->arena, "kind", s("quickfix"));
    json_object_set_object(action, "edit", workspace_edit);
    json_array_push(actions, action);

    json_object_set_array(response, "result", actions);
    lsp_send_response(message->arena, response);
}
