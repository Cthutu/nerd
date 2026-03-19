//------------------------------------------------------------------------------
// LSP communication utilities
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <lsp/lsp.h>

#include <stdio.h>

//------------------------------------------------------------------------------

void lsp_log(cstr format, ...)
{
    va_list args;
    va_start(args, format);
    epr("[lsp] ");
    eprv(format, args);
    epr("\n");
    va_end(args);
}

JsonValue* lsp_prepare_response(const LspMessage* message)
{
    JsonValue* response = json_new_object(message->arena);
    json_object_set_string(response, message->arena, "jsonrpc", s("2.0"));
    ASSERT(message->id != NULL, "LSP responses require a request id");
    switch (message->id->kind) {
    case JSON_NUMBER:
        json_object_set_number(
            response, message->arena, "id", json_float(message->id));
        break;
    case JSON_STRING:
        json_object_set_string(
            response, message->arena, "id", json_string(message->id));
        break;
    case JSON_NULL:
        json_object_set_null(response, message->arena, "id");
        break;
    default:
        kill("Unsupported LSP id type: %d", message->id->kind);
    }
    return response;
}

void lsp_send_response(Arena* arena, const JsonValue* response)
{
    string output = json_stringify(arena, response, .pretty = false);
    fprintf(stdout, "Content-Length: %zu\r\n\r\n", output.count);
    fwrite(output.data, 1, output.count, stdout);
    fflush(stdout);
}
