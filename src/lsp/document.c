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
                                     FrontEndState*         out_front_end)
{
    ProgramInfo program = {0};
    if (!front_end_program(source, options, NULL, &program)) {
        return false;
    }

    *out_front_end = program.modules[program.root_module_index].front_end;
    program.modules[program.root_module_index].front_end = (FrontEndState){0};
    program_info_done(&program);
    return true;
}

internal bool lsp_analyse_document(LspDocument* doc, string uri, string content)
{
    doc->analysis_ok  = false;
    doc->has_cst      = false;

    u8* document_copy = (u8*)arena_alloc(&doc->arena, content.count);
    memcpy(document_copy, content.data, content.count);

    string document_copy_str = {.data = document_copy, .count = content.count};

    lsp_log("Analysing document...");
    ErrorRenderMode previous_mode = error_system_mode();
    bool            previous_emit = error_system_should_emit_output();
    error_system_clear_last_rendered();
    error_system_set_mode(ERROR_RENDER_DIAGNOSTICS);
    error_system_set_emit_output(false);
    FrontEndOptions options = {
        .verbose             = false,
        .release             = false,
        .require_entry_point = true,
    };
    bool ok = lsp_front_end_document(
        (NerdSource){
            .source      = document_copy_str,
            .source_path = uri,
        },
        &options,
        &doc->front_end);
    error_system_set_mode(previous_mode);
    error_system_set_emit_output(previous_emit);

    if (!ok) {
        lsp_log("Front-end analysis failed for current document contents");
        return false;
    }

    doc->analysis_ok = true;
    if (cst_parse(&doc->front_end.lexer, &doc->cst)) {
        doc->has_cst = true;
    } else {
        lsp_log("CST parsing failed for current document contents");
    }

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
    } else {
        cst_done(&doc->cst);
        front_end_results_done(&doc->front_end);
        arena_reset(&doc->arena);
        doc->cst = (Cst){0};
    }

    bool       ok          = lsp_analyse_document(doc, uri, text);
    JsonValue* diagnostics = ok ? json_new_array(message->arena)
                                : lsp_parse_last_diagnostics(message->arena);
    lsp_publish_diagnostics(message->arena, uri, diagnostics);
}

void lsp_handle_did_change(LspState* state, const LspMessage* message)
{
    string uri, text;

    bool got_uri =
        lsp_get_string_param(message, "params.textDocument.uri", &uri);
    bool got_text =
        lsp_get_string_param(message, "params.contentChanges[0].text", &text);

    if (!got_uri || !got_text) {
        lsp_log("Error: Missing or invalid required parameters for didChange");
        return;
    }

    LspDocument* doc = LspDocumentMap_find(&state->documents, uri);
    if (!doc) {
        lsp_log("Error: Attempted to change non-existent document: " STRINGP,
                STRINGV(uri));
        return;
    }

    cst_done(&doc->cst);
    doc->cst = (Cst){0};

    front_end_results_done(&doc->front_end);
    arena_reset(&doc->arena);

    bool       ok          = lsp_analyse_document(doc, uri, text);
    JsonValue* diagnostics = ok ? json_new_array(message->arena)
                                : lsp_parse_last_diagnostics(message->arena);
    lsp_publish_diagnostics(message->arena, uri, diagnostics);
}

void lsp_document_done(LspDocument* doc)
{
    cst_done(&doc->cst);
    doc->cst = (Cst){0};

    front_end_results_done(&doc->front_end);
    arena_done(&doc->arena);
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
