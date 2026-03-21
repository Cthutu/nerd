//------------------------------------------------------------------------------
// Hover handling
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <lsp/lsp.h>

//------------------------------------------------------------------------------

void lsp_handle_hover(LspState* state, const LspMessage* message)
{
    JsonValue* response = lsp_prepare_response(message);
    JsonValue* result   = json_new_object(message->arena);

    // Extract the filename and position of the hover
    JsonValue* document_name =
        json_get_cstr(message->message, "params.textDocument.uri");
    JsonValue* line = json_get_cstr(message->message, "params.position.line");
    JsonValue* column =
        json_get_cstr(message->message, "params.position.character");

    if (!document_name || !line || !column ||
        document_name->kind != JSON_STRING || line->kind != JSON_NUMBER ||
        column->kind != JSON_NUMBER) {
        // Invalid hover request, return an empty response
        lsp_fail(response,
                 message->arena,
                 "Invalid hover request: missing or invalid parameters");
        return;
    }

    // Obtain the source code for the file
    LspDocument* doc =
        LspDocumentMap_find(&state->documents, json_string(document_name));
    if (!doc) {
        lsp_fail(response,
                 message->arena,
                 "Document not found: " STRINGP,
                 STRINGV(json_string(document_name)));
        return;
    }

    // Convert the line and column number to an offset into the source
    usize offset      = 0;
    u64   target_line = json_integer(line);
    u64   target_col  = json_integer(column);

    if (!lex_line_col_to_offset(
            doc->lexer.source, target_line, target_col, &offset)) {
        lsp_fail(response,
                 message->arena,
                 "Invalid hover position: line " STRINGP ", column " STRINGP,
                 STRINGV(json_string(line)),
                 STRINGV(json_string(column)));
        return;
    }

    // Given the offset, find the token that spans the offset.
    u32    token_end;
    Token* token = lex_find(&doc->lexer, offset, &token_end);
    if (!token) {
        lsp_cancel(response, message->arena);
        return;
    }

    // Create the tip for the token.  The lexer is not designed to have its
    // tokens access randomly, so we have to reprocess the values.
    string hover_text;
    switch (token->kind) {
    case TK_Integer:
        {
            hover_text =
                string_format(message->arena,
                              "%.*s (u64)",
                              token_end - token->offset,
                              doc->lexer.source.source.data + token->offset);
        }
        break;
    default:
        hover_text = string_format(message->arena,
                                   "Unknown token of kind `" STRINGP "`",
                                   STRINGV(token_kind_to_string(token->kind)));
    };

    // For demonstration, we return a fixed hover response
    JsonValue* contents = json_new_object(message->arena);
    json_object_set_string(
        contents, message->arena, "kind", string_from_cstr("plaintext"));
    json_object_set_string(contents, message->arena, "value", hover_text);

    json_object_set(result, "contents", contents);
    json_object_set_object(response, "result", result);

    lsp_send_response(message->arena, response);
}
