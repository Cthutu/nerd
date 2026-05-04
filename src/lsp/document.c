//------------------------------------------------------------------------------
// Document lifetime management for LSP
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/build/front/front.h>
#include <compiler/error/error.h>
#include <lsp/lsp.h>

//------------------------------------------------------------------------------

bool lsp_get_string_param(const LspMessage* message,
                          cstr              param_path,
                          string*           out_str)
{
    JsonValue* value = json_get_cstr(message->message, param_path);
    if (value && value->kind == JSON_STRING) {
        *out_str = json_string(value);
        return true;
    } else {
        return false;
    }
}

bool lsp_get_u64_param(const LspMessage* message,
                       cstr              param_path,
                       u64*              out_value)
{
    JsonValue* value = json_get_cstr(message->message, param_path);
    if (value && value->kind == JSON_NUMBER) {
        *out_value = (u64)value->number;
        return true;
    } else {
        return false;
    }
}

//------------------------------------------------------------------------------

internal JsonValue* lsp_parse_last_diagnostics(Arena* arena)
{
    string diagnostics_json = error_system_last_rendered();
    error_system_clear_last_rendered();

    if (diagnostics_json.count == 0) {
        return json_new_array(arena);
    }

    JsonParseResult parse_result = {0};
    JsonValue* diagnostics = json_parse(arena, diagnostics_json, &parse_result);
    if (!parse_result.ok || !diagnostics || diagnostics->kind != JSON_ARRAY) {
        lsp_log("Failed to parse rendered diagnostics JSON");
        return json_new_array(arena);
    }

    return diagnostics;
}

internal bool lsp_front_end_document(NerdSource             source,
                                     const FrontEndOptions* options,
                                     ProgramInfo*           out_program,
                                     FrontEndState*         out_front_end)
{
    if (!front_end_program(source, options, NULL, out_program)) {
        return false;
    }

    *out_front_end =
        out_program->modules[out_program->root_module_index].front_end;
    return true;
}

internal void lsp_document_reset_runtime(LspDocument* doc)
{
    cst_done(&doc->cst);
    doc->cst = (Cst){0};
    program_info_done(&doc->program);
    arena_done(&doc->arena);
    doc->program     = (ProgramInfo){0};
    doc->front_end   = (FrontEndState){0};
    doc->analysis_ok = false;
    doc->has_cst     = false;
}

internal void lsp_document_set_source(LspDocument* doc, string content)
{
    arena_reset(&doc->source_arena);
    u8* document_copy = (u8*)arena_alloc(&doc->source_arena, content.count);
    memcpy(document_copy, content.data, content.count);
    doc->source = (string){.data = document_copy, .count = content.count};
}

internal bool
lsp_stage_document(LspDocument* staged, string uri, string content)
{
    *staged = (LspDocument){0};
    arena_init(&staged->arena);
    staged->analysis_ok = false;
    staged->has_cst     = false;
    staged->source      = content;

    lsp_log("Analysing document...");
    ErrorRenderMode previous_mode = error_system_mode();
    bool            previous_emit = error_system_should_emit_output();
    error_system_clear_last_rendered();
    error_system_set_mode(ERROR_RENDER_DIAGNOSTICS);
    error_system_set_emit_output(false);
    FrontEndOptions options = {
        .verbose             = false,
        .release             = false,
        .require_entry_point = false,
        .skip_ir_generation  = true,
    };
    bool ok = lsp_front_end_document(
        (NerdSource){
            .source      = content,
            .source_path = uri,
        },
        &options,
        &staged->program,
        &staged->front_end);
    error_system_set_mode(previous_mode);
    error_system_set_emit_output(previous_emit);

    if (!ok) {
        lsp_log("Front-end analysis failed for current document contents");
        program_info_done(&staged->program);
        return false;
    }

    staged->analysis_ok = true;
    if (cst_parse(&staged->front_end.lexer, &staged->cst)) {
        staged->has_cst = true;
    } else {
        lsp_log("CST parsing failed for current document contents");
    }

    return true;
}

internal bool lsp_analyse_document(LspDocument* doc, string uri)
{
    LspDocument staged = {0};
    if (!lsp_stage_document(&staged, uri, doc->source)) {
        lsp_document_reset_runtime(&staged);
        return false;
    }

    lsp_document_reset_runtime(doc);
    doc->arena       = staged.arena;
    doc->program     = staged.program;
    doc->front_end   = staged.front_end;
    doc->cst         = staged.cst;
    doc->analysis_ok = staged.analysis_ok;
    doc->has_cst     = staged.has_cst;
    return true;
}

usize lsp_offset_from_position(string source, u64 line, u64 character)
{
    u64   current_line      = 0;
    u64   current_character = 0;
    usize offset            = 0;
    while (offset < source.count) {
        if (current_line == line && current_character == character) {
            return offset;
        }
        u8 c = source.data[offset++];
        if (c == '\n') {
            current_line++;
            current_character = 0;
        } else {
            current_character++;
        }
    }
    return source.count;
}

internal bool lsp_apply_change(LspDocument* doc, JsonValue* change)
{
    if (!change || change->kind != JSON_OBJECT) {
        return false;
    }

    JsonValue* text_value = json_object_get_cstr(change, "text");
    if (!text_value || text_value->kind != JSON_STRING) {
        return false;
    }
    string text      = json_string(text_value);

    JsonValue* range = json_object_get_cstr(change, "range");
    if (!range || range->kind != JSON_OBJECT) {
        lsp_document_set_source(doc, text);
        return true;
    }

    JsonValue* start_line_value      = json_get_cstr(range, "start.line");
    JsonValue* start_character_value = json_get_cstr(range, "start.character");
    JsonValue* end_line_value        = json_get_cstr(range, "end.line");
    JsonValue* end_character_value   = json_get_cstr(range, "end.character");
    if (!start_line_value || !start_character_value || !end_line_value ||
        !end_character_value || start_line_value->kind != JSON_NUMBER ||
        start_character_value->kind != JSON_NUMBER ||
        end_line_value->kind != JSON_NUMBER ||
        end_character_value->kind != JSON_NUMBER) {
        return false;
    }

    usize start =
        lsp_offset_from_position(doc->source,
                                 (u64)json_integer(start_line_value),
                                 (u64)json_integer(start_character_value));
    usize end =
        lsp_offset_from_position(doc->source,
                                 (u64)json_integer(end_line_value),
                                 (u64)json_integer(end_character_value));
    if (end < start) {
        return false;
    }

    Arena temp = {0};
    arena_init(&temp);
    StringBuilder sb = {0};
    sb_init(&sb, &temp);
    sb_append_string(&sb, string_from(doc->source.data, start));
    sb_append_string(&sb, text);
    sb_append_string(
        &sb, string_from(doc->source.data + end, doc->source.count - end));
    string updated = sb_to_string(&sb);
    lsp_document_set_source(doc, updated);
    arena_done(&temp);
    return true;
}

void lsp_handle_did_open(LspState* state, const LspMessage* message)
{
    string uri, text;
    bool   got_uri =
        lsp_get_string_param(message, "params.textDocument.uri", &uri);
    bool got_text =
        lsp_get_string_param(message, "params.textDocument.text", &text);

    if (!got_uri || !got_text) {
        lsp_log("Error: Missing or invalid required parameters for didOpen");
        return;
    }

    bool         new_doc = false;
    LspDocument* doc = LspDocumentMap_entry(&state->documents, uri, &new_doc);
    if (new_doc) {
        arena_init(&doc->arena);
        arena_init(&doc->source_arena);
    }

    lsp_document_set_source(doc, text);
    bool       ok          = lsp_analyse_document(doc, uri);
    JsonValue* diagnostics = ok ? json_new_array(message->arena)
                                : lsp_parse_last_diagnostics(message->arena);
    lsp_publish_diagnostics(message->arena, uri, diagnostics);
}

void lsp_handle_did_change(LspState* state, const LspMessage* message)
{
    string uri;

    bool got_uri =
        lsp_get_string_param(message, "params.textDocument.uri", &uri);

    if (!got_uri) {
        lsp_log("Error: Missing or invalid required parameters for didChange");
        return;
    }

    LspDocument* doc = LspDocumentMap_find(&state->documents, uri);
    if (!doc) {
        lsp_log("Error: Attempted to change non-existent document: " STRINGP,
                STRINGV(uri));
        return;
    }

    JsonValue* changes =
        json_get_cstr(message->message, "params.contentChanges");
    if (!changes || changes->kind != JSON_ARRAY) {
        lsp_log("Error: Missing or invalid contentChanges for didChange");
        return;
    }

    for (usize i = 0; i < array_count(changes->array.values); ++i) {
        if (!lsp_apply_change(doc, changes->array.values[i])) {
            lsp_log("Error: Failed to apply document change");
            return;
        }
    }

    bool       ok          = lsp_analyse_document(doc, uri);
    JsonValue* diagnostics = ok ? json_new_array(message->arena)
                                : lsp_parse_last_diagnostics(message->arena);
    lsp_publish_diagnostics(message->arena, uri, diagnostics);
}

void lsp_document_done(LspDocument* doc)
{
    lsp_document_reset_runtime(doc);
    arena_done(&doc->source_arena);
    *doc = (LspDocument){0};
}

void lsp_handle_did_close(LspState* state, const LspMessage* message)
{
    JsonValue* uri = json_get_cstr(message->message, "params.textDocument.uri");
    string     uri_str;

    if (uri && uri->kind == JSON_STRING) {
        uri_str = json_string(uri);
        lsp_log("Closed " STRINGP, STRINGV(uri_str));
    } else {
        return;
    }

    LspDocument* doc = LspDocumentMap_find(&state->documents, uri_str);
    if (!doc) {
        lsp_log("Warning: Attempted to close non-existent document: " STRINGP,
                STRINGV(uri_str));
        return;
    }

    lsp_publish_diagnostics(
        message->arena, uri_str, json_new_array(message->arena));
    lsp_document_done(doc);
    LspDocumentMap_delete(&state->documents, uri_str);
}
