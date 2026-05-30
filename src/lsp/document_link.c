//------------------------------------------------------------------------------
// LSP document links
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/modules/modules.h>
#include <lsp/lsp.h>

//------------------------------------------------------------------------------

internal JsonValue*
lsp_document_link_position(Arena* arena, NerdSource source, usize offset)
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

internal JsonValue*
lsp_document_link_range(Arena* arena, NerdSource source, usize start, usize end)
{
    JsonValue* start_position =
        lsp_document_link_position(arena, source, start);
    JsonValue* end_position = lsp_document_link_position(arena, source, end);
    if (start_position == NULL || end_position == NULL) {
        return NULL;
    }

    JsonValue* range = json_new_object(arena);
    json_object_set_object(range, "start", start_position);
    json_object_set_object(range, "end", end_position);
    return range;
}

internal bool lsp_document_link_visible_start(const LspDocument* doc,
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

internal JsonValue* lsp_document_link_document_range(const LspDocument* doc,
                                                     Arena*             arena,
                                                     usize start_offset,
                                                     usize end_offset)
{
    usize visible_start = 0;
    if (!lsp_document_link_visible_start(doc, &visible_start) ||
        start_offset < visible_start || end_offset < visible_start ||
        end_offset > visible_start + doc->source.count) {
        return NULL;
    }

    NerdSource visible_source = {
        .source      = doc->source,
        .source_path = doc->front_end.lexer.source.source_path,
    };
    return lsp_document_link_range(arena,
                                   visible_source,
                                   start_offset - visible_start,
                                   end_offset - visible_start);
}

internal string lsp_document_link_path_to_uri(Arena* arena, cstr path)
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

internal string lsp_document_link_copy_string(Arena* arena, string value)
{
    u8* copy = arena_alloc(arena, value.count);
    memcpy(copy, value.data, value.count);
    return (string){.data = copy, .count = value.count};
}

internal bool lsp_document_link_use_target(Arena*               arena,
                                           const LspSyntaxView* view,
                                           const AstNode*       use,
                                           string*              out_uri)
{
    if (use->a >= array_count(view->ast->nodes)) {
        return false;
    }

    const AstNode* modref = &view->ast->nodes[use->a];
    if (modref->kind != AK_ModRef ||
        modref->a >= array_count(view->ast->module_paths)) {
        return false;
    }

    ModuleResolveResult resolved = {0};
    ModuleResolveStatus status =
        module_resolve_path(arena,
                            view->lexer->source,
                            view->lexer,
                            view->ast,
                            &view->ast->module_paths[modref->a],
                            &resolved);
    if (status != MRS_Found || resolved.resolved_path == NULL) {
        return false;
    }

    *out_uri = lsp_document_link_path_to_uri(arena, resolved.resolved_path);
    return true;
}

void lsp_handle_document_link(LspState* state, const LspMessage* message)
{
    JsonValue* response = lsp_prepare_response(message);

    string uri          = {0};
    if (!lsp_get_string_param(message, "params.textDocument.uri", &uri)) {
        JsonValue* empty = json_new_array(message->arena);
        json_object_set_array(response, "result", empty);
        lsp_send_response(message->arena, response);
        return;
    }

    LspSyntaxView view  = {0};
    JsonValue*    links = json_new_array(message->arena);
    if (!lsp_syntax_view(state, uri, &view)) {
        json_object_set_array(response, "result", links);
        lsp_send_response(message->arena, response);
        return;
    }

    for (u32 i = 0; i < array_count(view.ast->nodes); ++i) {
        const AstNode* use = &view.ast->nodes[i];
        if (use->kind != AK_Use) {
            continue;
        }

        usize start = 0;
        usize end   = 0;
        if (!lsp_use_module_path_range(view.doc, i, &start, &end)) {
            continue;
        }

        JsonValue* range = lsp_document_link_document_range(
            view.doc, message->arena, start, end);
        if (range == NULL) {
            continue;
        }

        string target = {0};
        Arena  temp   = {0};
        arena_init(&temp);
        bool found = lsp_document_link_use_target(&temp, &view, use, &target);
        if (found) {
            target = lsp_document_link_copy_string(message->arena, target);
        }
        arena_done(&temp);
        if (!found) {
            continue;
        }

        JsonValue* link = json_new_object(message->arena);
        json_object_set_object(link, "range", range);
        json_object_set_string(link, message->arena, "target", target);
        json_array_push(links, link);
    }

    json_object_set_array(response, "result", links);
    lsp_send_response(message->arena, response);
}

//------------------------------------------------------------------------------
