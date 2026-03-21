//------------------------------------------------------------------------------
// Document lifetime management for LSP
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

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

internal void lsp_lex_document(LspDocument* doc, string content)
{
    u8* document_copy = (u8*)arena_alloc(&doc->arena, content.count);
    memcpy(document_copy, content.data, content.count);

    string document_copy_str = {.data = document_copy, .count = content.count};

    lsp_log("Lexing document...");
    if (!lex(
            (NerdSource){
                .source      = document_copy_str,
                .source_path = s(""),
            },
            &doc->lexer)) {
        lsp_log("Lexing failed for current document contents");
        return;
    }
    lsp_log("Lexed %zu tokens", array_count(doc->lexer.tokens));

    for (usize i = 0; i < array_count(doc->lexer.tokens); i++) {
        Token* token = &doc->lexer.tokens[i];
        lsp_log("Token %zu: kind=\"" STRINGP "\", offset=%u",
                i,
                STRINGV(token_kind_to_string(token->kind)),
                token->offset);
    }

    lsp_log("Document content:\n" STRINGP, STRINGV(document_copy_str));
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
        arena_reset(&doc->arena);
    }

    lsp_lex_document(doc, text);
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

    lsp_lex_document(doc, text);
}

void lsp_document_done(LspDocument* doc)
{
    arena_done(&doc->arena);
    lex_done(&doc->lexer);
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

    lsp_document_done(doc);
    LspDocumentMap_delete(&state->documents, uri_str);
}
