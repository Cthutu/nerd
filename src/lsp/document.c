//------------------------------------------------------------------------------
// Document lifetime management for LSP
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <lsp/lsp.h>

//------------------------------------------------------------------------------

void lsp_handle_did_open(LspState* state, const LspMessage* message)
{
    JsonValue* uri = json_get_cstr(message->message, "params.textDocument.uri");
    JsonValue* text =
        json_get_cstr(message->message, "params.textDocument.text");
    string uri_str;

    if (uri && uri->kind == JSON_STRING) {
        uri_str = json_string(uri);
        lsp_log("Opened " STRINGP, STRINGV(uri_str));
    } else {
        return;
    }

    if (text && text->kind == JSON_STRING) {
        string text_str      = json_string(text);

        bool         new_doc = false;
        LspDocument* doc =
            LspDocumentMap_entry(&state->documents, uri_str, &new_doc);
        if (new_doc) {
            arena_init(&doc->arena);
        } else {
            arena_reset(&doc->arena);
        }

        u8*    document_copy = (u8*)arena_alloc(&doc->arena, text_str.count);
        string src_document  = json_string(text);
        memcpy(document_copy, src_document.data, src_document.count);

        string document_copy_str = {.data  = document_copy,
                                    .count = src_document.count};

        lsp_log("Lexing document...");
        doc->lexer = lex(document_copy_str);
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
