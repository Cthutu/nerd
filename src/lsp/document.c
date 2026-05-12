//------------------------------------------------------------------------------
// Document lifetime management for LSP
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/build/front/front.h>
#include <compiler/error/error.h>
#include <compiler/internal.h>
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

typedef struct {
    string     uri;
    JsonValue* diagnostics;
} LspDiagnosticGroup;

internal bool lsp_string_starts_with(string value, cstr prefix)
{
    string prefix_string = s(prefix);
    if (value.count < prefix_string.count) {
        return false;
    }
    return memcmp(value.data, prefix_string.data, prefix_string.count) == 0;
}

internal string lsp_path_string_to_uri(Arena* arena, string path)
{
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

internal string lsp_normalise_diagnostic_uri(Arena* arena, string uri)
{
    if (lsp_string_starts_with(uri, "<")) {
        return uri;
    }
    if (lsp_string_starts_with(uri, "file://")) {
#if OS_WINDOWS
        for (usize i = 0; i < uri.count; ++i) {
            if (uri.data[i] == '\\') {
                string path = {
                    .data  = uri.data + strlen("file://"),
                    .count = uri.count - strlen("file://"),
                };
                return lsp_path_string_to_uri(arena, path);
            }
        }
#endif
        return uri;
    }

    bool absolute_path = uri.count > 0 && uri.data[0] == '/';
#if OS_WINDOWS
    absolute_path = absolute_path || (uri.count > 1 && uri.data[1] == ':');
#endif

    if (!absolute_path) {
        return uri;
    }

    return lsp_path_string_to_uri(arena, uri);
}

internal string lsp_diagnostic_uri(Arena*     arena,
                                   JsonValue* diagnostic,
                                   string     fallback_uri)
{
    JsonValue* related = json_get_cstr(diagnostic, "relatedInformation");
    if (!related || related->kind != JSON_ARRAY) {
        return fallback_uri;
    }

    string first_uri = {0};
    for (usize i = 0; i < array_count(related->array.values); i++) {
        JsonValue* info     = related->array.values[i];
        JsonValue* location = json_get_cstr(info, "location");
        JsonValue* uri      = json_get_cstr(info, "location.uri");
        if (!location || location->kind != JSON_OBJECT || !uri ||
            uri->kind != JSON_STRING) {
            continue;
        }

        string normalised_uri =
            lsp_normalise_diagnostic_uri(arena, json_string(uri));
        if (!string_eq(normalised_uri, json_string(uri))) {
            json_object_set_string(location, arena, "uri", normalised_uri);
        }

        if (first_uri.count == 0) {
            first_uri = normalised_uri;
        }

        JsonValue* message = json_get_cstr(info, "message");
        if (message && message->kind == JSON_STRING) {
            string message_text = json_string(message);
            if (lsp_string_starts_with(message_text, "help:") ||
                lsp_string_starts_with(message_text, "note:")) {
                return normalised_uri;
            }
        }
    }

    return first_uri.count > 0 ? first_uri : fallback_uri;
}

internal JsonValue* lsp_diagnostic_group(Arena* arena,
                                         Array(LspDiagnosticGroup) * groups,
                                         string uri)
{
    for (usize i = 0; i < array_count(*groups); i++) {
        if (string_eq((*groups)[i].uri, uri)) {
            return (*groups)[i].diagnostics;
        }
    }

    JsonValue* diagnostics = json_new_array(arena);
    array_push(*groups,
               ((LspDiagnosticGroup){
                   .uri         = uri,
                   .diagnostics = diagnostics,
               }));
    return diagnostics;
}

internal void lsp_publish_grouped_diagnostics(Arena*     arena,
                                              string     document_uri,
                                              JsonValue* diagnostics)
{
    Array(LspDiagnosticGroup) groups = NULL;

    // Always clear the active document. Imported-module diagnostics are
    // published under their physical URI instead.
    lsp_diagnostic_group(arena, &groups, document_uri);

    if (diagnostics && diagnostics->kind == JSON_ARRAY) {
        for (usize i = 0; i < array_count(diagnostics->array.values); i++) {
            JsonValue* diagnostic = diagnostics->array.values[i];
            string uri = lsp_diagnostic_uri(arena, diagnostic, document_uri);
            json_array_push(lsp_diagnostic_group(arena, &groups, uri),
                            diagnostic);
        }
    }

    for (usize i = 0; i < array_count(groups); i++) {
        lsp_publish_diagnostics(arena, groups[i].uri, groups[i].diagnostics);
    }

    array_free(groups);
}

internal bool lsp_front_end_document(NerdSource             source,
                                     const FrontEndOptions* options,
                                     ProgramInfo*           out_program,
                                     FrontEndState*         out_front_end)
{
    *out_program   = (ProgramInfo){0};
    *out_front_end = (FrontEndState){0};
    bool ok        = front_end_program(source, options, NULL, out_program);
    if (array_count(out_program->modules) == 0 ||
        out_program->root_module_index >= array_count(out_program->modules)) {
        return false;
    }

    *out_front_end =
        out_program->modules[out_program->root_module_index].front_end;
    return ok;
}

internal void lsp_document_reset_runtime(LspDocument* doc)
{
    cst_done(&doc->cst);
    doc->cst = (Cst){0};
    program_info_done(&doc->program);
    arena_done(&doc->arena);
    doc->program             = (ProgramInfo){0};
    doc->front_end           = (FrontEndState){0};
    doc->analysis_ok         = false;
    doc->tokens_ready        = false;
    doc->syntax_ready        = false;
    doc->decls_ready         = false;
    doc->bindings_ready      = false;
    doc->type_facts_partial  = false;
    doc->type_facts_complete = false;
    doc->sema_partial        = false;
    doc->sema_complete       = false;
    doc->cst_ready           = false;
}

internal void lsp_document_set_readiness_from_front_end(LspDocument* doc)
{
    const FrontEndReadiness* readiness = &doc->front_end.readiness;
    doc->tokens_ready = front_end_product_is_available(readiness->lexer);
    doc->syntax_ready = front_end_product_is_available(readiness->ast);
    doc->decls_ready =
        front_end_product_is_available(readiness->semantic.declarations);
    doc->bindings_ready =
        front_end_product_is_available(readiness->semantic.bindings);
    doc->type_facts_partial =
        front_end_product_is_available(readiness->semantic.type_facts);
    doc->type_facts_complete =
        front_end_product_is_complete(readiness->semantic.type_facts);
    doc->sema_partial  = doc->decls_ready || doc->bindings_ready ||
                         doc->type_facts_partial ||
                         front_end_product_is_available(readiness->sema);
    doc->sema_complete = front_end_product_is_complete(readiness->sema);
}

internal void lsp_document_set_source(LspDocument* doc, string content)
{
    arena_reset(&doc->source_arena);
    u8* document_copy = (u8*)arena_alloc(&doc->source_arena, content.count);
    memcpy(document_copy, content.data, content.count);
    doc->source       = (string){.data = document_copy, .count = content.count};
    doc->source_ready = true;
}

internal bool
lsp_stage_document(LspDocument* staged, string uri, string content)
{
    *staged = (LspDocument){0};
    arena_init(&staged->arena);
    staged->analysis_ok         = false;
    staged->source_ready        = true;
    staged->tokens_ready        = false;
    staged->syntax_ready        = false;
    staged->decls_ready         = false;
    staged->bindings_ready      = false;
    staged->type_facts_partial  = false;
    staged->type_facts_complete = false;
    staged->sema_partial        = false;
    staged->sema_complete       = false;
    staged->cst_ready           = false;
    staged->source              = content;

    u8* uri_copy                = arena_alloc(&staged->arena, uri.count);
    memcpy(uri_copy, uri.data, uri.count);
    string stable_uri = {.data = uri_copy, .count = uri.count};

    lsp_log("Analysing document...");
    ErrorRenderMode previous_mode = error_system_mode();
    bool            previous_emit = error_system_should_emit_output();
    error_system_clear_last_rendered();
    error_system_set_mode(ERROR_RENDER_DIAGNOSTICS);
    error_system_set_emit_output(false);
    FrontEndOptions options = {
        .verbose              = false,
        .release              = false,
        .require_entry_point  = false,
        .skip_hir_generation  = true,
        .keep_partial_results = true,
    };
    bool ok = lsp_front_end_document(
        (NerdSource){
            .source      = content,
            .source_path = stable_uri,
        },
        &options,
        &staged->program,
        &staged->front_end);
    error_system_set_mode(previous_mode);
    error_system_set_emit_output(previous_emit);

    lsp_document_set_readiness_from_front_end(staged);
    if (!staged->tokens_ready || !staged->syntax_ready) {
        if (!ok) {
            lsp_log("Front-end analysis failed for current document contents");
        }
        program_info_done(&staged->program);
        return false;
    }
    if (!ok && staged->front_end.readiness.sema == FRONT_END_PRODUCT_Missing) {
        staged->front_end.readiness.sema = FRONT_END_PRODUCT_Partial;
        staged->front_end.readiness.semantic.declarations =
            FRONT_END_PRODUCT_Partial;
        staged->front_end.readiness.semantic.bindings =
            FRONT_END_PRODUCT_Partial;
        staged->front_end.readiness.semantic.type_facts =
            FRONT_END_PRODUCT_Partial;
        if (array_count(staged->program.modules) > 0 &&
            staged->program.root_module_index <
                array_count(staged->program.modules)) {
            FrontEndReadiness* root_readiness =
                &staged->program.modules[staged->program.root_module_index]
                     .front_end.readiness;
            root_readiness->sema                  = FRONT_END_PRODUCT_Partial;
            root_readiness->semantic.declarations = FRONT_END_PRODUCT_Partial;
            root_readiness->semantic.bindings     = FRONT_END_PRODUCT_Partial;
            root_readiness->semantic.type_facts   = FRONT_END_PRODUCT_Partial;
        }
        lsp_document_set_readiness_from_front_end(staged);
    }

    staged->analysis_ok = ok;
    if (!ok) {
        lsp_log("Keeping front-end products for editor features: lexer=%s, "
                "ast=%s, sema=%s, hir=%s",
                front_end_product_state_name(staged->front_end.readiness.lexer),
                front_end_product_state_name(staged->front_end.readiness.ast),
                front_end_product_state_name(staged->front_end.readiness.sema),
                front_end_product_state_name(staged->front_end.readiness.hir));
    }
    if (cst_parse(&staged->front_end.lexer, &staged->cst)) {
        staged->cst_ready = true;
    } else {
        lsp_log("CST parsing failed for current document contents");
    }

    return true;
}

internal bool lsp_analyse_document(LspDocument* doc, string uri)
{
    MemoryStats memory_before = compiler_memory_profile_begin();
    LspDocument staged        = {0};
    if (!lsp_stage_document(&staged, uri, doc->source)) {
        lsp_document_reset_runtime(&staged);
        compiler_memory_profile_end(
            COMPILER_STAGE_LSP, COMPILER_PHASE_LSP_ANALYSE, memory_before);
        return false;
    }

    lsp_document_reset_runtime(doc);
    doc->arena   = staged.arena;
    doc->program = staged.program;
    for (u32 i = 0; i < array_count(doc->program.modules); ++i) {
        doc->program.modules[i].front_end.sema.program = &doc->program;
    }
    doc->front_end =
        doc->program.modules[doc->program.root_module_index].front_end;
    doc->cst          = staged.cst;
    doc->analysis_ok  = staged.analysis_ok;
    doc->source_ready = staged.source_ready;
    lsp_document_set_readiness_from_front_end(doc);
    doc->cst_ready = staged.cst_ready;
    compiler_memory_profile_end(
        COMPILER_STAGE_LSP, COMPILER_PHASE_LSP_ANALYSE, memory_before);
    return staged.analysis_ok;
}

bool lsp_source_view(LspState* state, string uri, LspSourceView* out_view)
{
    LspDocument* doc = LspDocumentMap_find(&state->documents, uri);
    if (!doc || !doc->source_ready) {
        return false;
    }

    *out_view = (LspSourceView){
        .doc    = doc,
        .source = doc->source,
    };
    return true;
}

bool lsp_token_view(LspState* state, string uri, LspTokenView* out_view)
{
    LspDocument* doc = LspDocumentMap_find(&state->documents, uri);
    if (!doc || !doc->tokens_ready) {
        return false;
    }

    *out_view = (LspTokenView){
        .doc    = doc,
        .source = doc->source,
        .lexer  = &doc->front_end.lexer,
    };
    return true;
}

bool lsp_syntax_view(LspState* state, string uri, LspSyntaxView* out_view)
{
    LspDocument* doc = LspDocumentMap_find(&state->documents, uri);
    if (!doc || !doc->syntax_ready) {
        return false;
    }

    *out_view = (LspSyntaxView){
        .doc    = doc,
        .source = doc->source,
        .lexer  = &doc->front_end.lexer,
        .ast    = &doc->front_end.ast,
    };
    return true;
}

bool lsp_semantic_view(LspState* state, string uri, LspSemanticView* out_view)
{
    LspDocument* doc = LspDocumentMap_find(&state->documents, uri);
    if (!doc || !doc->sema_partial) {
        return false;
    }

    *out_view = (LspSemanticView){
        .doc    = doc,
        .source = doc->source,
        .lexer  = &doc->front_end.lexer,
        .ast    = &doc->front_end.ast,
        .sema   = &doc->front_end.sema,
    };
    return true;
}

bool lsp_declaration_view(LspState*           state,
                          string              uri,
                          LspDeclarationView* out_view)
{
    LspDocument* doc = LspDocumentMap_find(&state->documents, uri);
    if (!doc || !doc->decls_ready) {
        return false;
    }

    *out_view = (LspDeclarationView){
        .doc    = doc,
        .source = doc->source,
        .lexer  = &doc->front_end.lexer,
        .ast    = &doc->front_end.ast,
        .sema   = &doc->front_end.sema,
    };
    return true;
}

bool lsp_binding_view(LspState* state, string uri, LspBindingView* out_view)
{
    LspDocument* doc = LspDocumentMap_find(&state->documents, uri);
    if (!doc || !doc->bindings_ready) {
        return false;
    }

    *out_view = (LspBindingView){
        .doc    = doc,
        .source = doc->source,
        .lexer  = &doc->front_end.lexer,
        .ast    = &doc->front_end.ast,
        .sema   = &doc->front_end.sema,
    };
    return true;
}

bool lsp_type_fact_view(LspState* state, string uri, LspTypeFactView* out_view)
{
    LspDocument* doc = LspDocumentMap_find(&state->documents, uri);
    if (!doc || !doc->type_facts_partial) {
        return false;
    }

    *out_view = (LspTypeFactView){
        .doc    = doc,
        .source = doc->source,
        .lexer  = &doc->front_end.lexer,
        .ast    = &doc->front_end.ast,
        .sema   = &doc->front_end.sema,
    };
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
    lsp_publish_grouped_diagnostics(message->arena, uri, diagnostics);
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
    lsp_publish_grouped_diagnostics(message->arena, uri, diagnostics);
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
