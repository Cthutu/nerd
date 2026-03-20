//------------------------------------------------------------------------------
// Hover handling
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <lsp/lsp.h>

//------------------------------------------------------------------------------

void lsp_handle_hover(LspState* state, const LspMessage* message)
{
    MapIter      it = LspDocumentMap_iter();
    string       key;
    LspDocument* idoc;

    while (LspDocumentMap_next(&state->documents, &it, &key, &idoc)) {
        lsp_log("Document in state: " STRINGP, STRINGV(key));
    }

    JsonValue* response = lsp_prepare_response(message);
    JsonValue* result   = json_new_object(message->arena);

    // Extract the filename and position of the hover
    JsonValue* document_name =
        json_get_cstr(message->message, "params.textDocument.uri");
    JsonValue* line = json_get_cstr(message->message, "params.position.line");
    JsonValue* column =
        json_get_cstr(message->message, "params.position.character");

    lsp_log("document_name: %p, line: %p, column: %p",
            (void*)document_name,
            (void*)line,
            (void*)column);

    lsp_log("document_name kind: %d, line kind: %d, column kind: %d",
            document_name ? document_name->kind : -1,
            line ? line->kind : -1,
            column ? column->kind : -1);

    if (!document_name || !line || !column ||
        document_name->kind != JSON_STRING || line->kind != JSON_NUMBER ||
        column->kind != JSON_NUMBER) {
        // Invalid hover request, return an empty response
        json_object_set_null(response, message->arena, "result");
        lsp_send_response(message->arena, response);
        lsp_log("Invalid hover request: missing or invalid parameters");
        return;
    }

    lsp_log("Hover requested for document: " STRINGP " at line %zu, column %zu",
            STRINGV(json_string(document_name)),
            (usize)line->number,
            (usize)column->number);

    // Obtain the source code for the file
    LspDocument* doc =
        LspDocumentMap_find(&state->documents, json_string(document_name));
    if (!doc) {
        lsp_log("Document not found: " STRINGP,
                STRINGV(json_string(document_name)));
        json_object_set_null(response, message->arena, "result");
        lsp_send_response(message->arena, response);
        return;
    }

    // Convert the line and column number to an offset into the source
    usize offset      = 0;
    u64   target_line = json_integer(line) - 1;
    u64   target_col  = json_integer(column) - 1;
    for (usize i = 0; i < doc->arena.cursor; i++) {
        if (doc->arena.data[i] == '\n') {
            if (target_line == 0) {
                break;
            }
            target_line--;
        } else if (target_line == 0) {
            if (target_col == 0) {
                break;
            }
            target_col--;
        }
        offset++;
    }

    // For demonstration, we return a fixed hover response
    JsonValue* contents = json_new_object(message->arena);
    json_object_set_string(
        contents, message->arena, "kind", string_from_cstr("plaintext"));
    string hover_text =
        string_format(message->arena,
                      "Hover at offset %zu in document " STRINGP,
                      offset,
                      STRINGV(json_string(document_name)));
    json_object_set_string(contents, message->arena, "value", hover_text);

    json_object_set(result, "contents", contents);
    json_object_set_object(response, "result", result);

    lsp_send_response(message->arena, response);
}
