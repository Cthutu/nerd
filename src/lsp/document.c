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
    if (uri && uri->kind == JSON_STRING) {
        lsp_log("Opened " STRINGP, STRINGV(json_string(uri)));
    } else {
        return;
    }

    if (text && text->kind == JSON_STRING) {
        string text_str  = json_string(text);

        bool   new_arena = false;
        Arena* arena =
            LspDocumentMap_entry(&state->documents, text_str, &new_arena);
        if (new_arena) {
            arena_init(arena);
        } else {
            arena_reset(arena);
        }

        u8*    document_copy = (u8*)arena_alloc(arena, text_str.count);
        string src_document  = json_string(text);
        memcpy(document_copy, src_document.data, src_document.count);

        lsp_log("Document content:\n" STRINGP, STRINGV(json_string(text)));
    }
}

void lsp_handle_did_close(LspState* state, const LspMessage* message)
{
    JsonValue* uri = json_get_cstr(message->message, "params.textDocument.uri");
    JsonValue* text =
        json_get_cstr(message->message, "params.textDocument.text");

    if (uri && uri->kind == JSON_STRING) {
        lsp_log("Closed " STRINGP, STRINGV(json_string(uri)));
    } else {
        return;
    }

    if (text && text->kind == JSON_STRING) {
        string text_str = json_string(text);
        LspDocumentMap_delete(&state->documents, text_str);
    }
}
