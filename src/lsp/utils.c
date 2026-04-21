//------------------------------------------------------------------------------
// LSP communication utilities
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <lsp/lsp.h>

#include <stdio.h>

//------------------------------------------------------------------------------

void lsp_logv(cstr format, va_list args)
{
    epr("[lsp] ");
    eprv(format, args);
    epr("\n");
}

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

JsonValue* lsp_prepare_notification(Arena* arena, string method)
{
    JsonValue* notification = json_new_object(arena);
    json_object_set_string(notification, arena, "jsonrpc", s("2.0"));
    json_object_set_string(notification, arena, "method", method);
    return notification;
}

void lsp_send_response(Arena* arena, JsonValue* response)
{
    string output = json_stringify(arena, response, .pretty = false);
    lsp_log("Sending message:\n" STRINGP, STRINGV(output));
    fprintf(stdout, "Content-Length: %zu\r\n\r\n", output.count);
    fwrite(output.data, 1, output.count, stdout);
    fflush(stdout);
    json_done(response);
}

void lsp_publish_diagnostics(Arena* arena, string uri, JsonValue* diagnostics)
{
    JsonValue* notification =
        lsp_prepare_notification(arena, s("textDocument/publishDiagnostics"));
    JsonValue* params = json_new_object(arena);
    json_object_set_string(params, arena, "uri", uri);
    json_object_set_array(params, "diagnostics", diagnostics);
    json_object_set_object(notification, "params", params);
    lsp_send_response(arena, notification);
}

void lsp_cancel(JsonValue* response, Arena* arena)
{
    json_object_set_null(response, arena, "result");
    lsp_send_response(arena, response);
}

void lsp_fail(JsonValue* response, Arena* arena, cstr format, ...)
{
    va_list args;
    va_start(args, format);
    lsp_logv(format, args);
    va_end(args);
    lsp_cancel(response, arena);
}
