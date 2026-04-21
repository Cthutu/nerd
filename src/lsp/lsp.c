//------------------------------------------------------------------------------
// LSP main loop
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <core/core.h>
#include <lsp/lsp.h>
#include <object/object.h>

#if OS_WINDOWS
#    include <fcntl.h>
#    include <io.h>
#endif

#include <stdio.h>

//------------------------------------------------------------------------------
// LSP reading

JsonValue* lsp_read_message(Arena* arena)
{
    usize content_length = 0;
    char  header[256];
    bool  saw_header = false;

    for (;;) {
        if (fgets(header, sizeof(header), stdin) == NULL) {
            if (!saw_header) {
                return NULL;
            }
            kill("Unexpected EOF while reading LSP headers");
        }

        if (header[0] == '\r' || header[0] == '\n') {
            break;
        }

        saw_header = true;
        if (strncmp(header, "Content-Length:", 15) == 0) {
            content_length = (usize)strtoull(header + 15, NULL, 10);
        }
    }

    if (content_length == 0) {
        kill("LSP message missing Content-Length header");
    }

    u8* json_data = (u8*)arena_alloc(arena, content_length);
    if (fread(json_data, 1, content_length, stdin) != content_length) {
        kill("Failed to read LSP message body from stdin");
    }

    string json                  = string_from(json_data, content_length);

    // Deserialise message
    JsonParseResult parse_result = {0};
    JsonValue*      message      = json_parse(arena, json, &parse_result);
    if (!parse_result.ok) {
        kill("LSP JSON parse failed at byte %zu: %s",
             parse_result.error_offset,
             parse_result.error_message);
    }
    return message;
}

//------------------------------------------------------------------------------

void lsp_init(LspState* state) { LspDocumentMap_init(&state->documents, 16); }

void lsp_done(LspState* state)
{
    MapIter iter = LspDocumentMap_iter();

    string       key;
    LspDocument* value;
    while (LspDocumentMap_next(&state->documents, &iter, &key, &value)) {
        lsp_document_done(value);
    }

    LspDocumentMap_done(&state->documents);
}

//------------------------------------------------------------------------------

struct {
    cstr method;
    void (*handler)(LspState* state, const LspMessage*);
} lsp_handlers[] = {
    {"initialize", lsp_handle_initialise},
    {"initialized", lsp_handle_initialised},
    {"textDocument/didOpen", lsp_handle_did_open},
    {"textDocument/didChange", lsp_handle_did_change},
    {"textDocument/didClose", lsp_handle_did_close},
    {"textDocument/hover", lsp_handle_hover},
    {"textDocument/definition", lsp_handle_definition},
    {"textDocument/documentSymbol", lsp_handle_document_symbol},
};

//------------------------------------------------------------------------------

int lsp_run(void)
{
#if OS_WINDOWS
    _setmode(_fileno(stdin), _O_BINARY);
    _setmode(_fileno(stdout), _O_BINARY);
#endif

    lsp_log("LSP server running");

    Arena message_arena = {0};
    arena_init(&message_arena);

    struct {
        bool initialised;
        bool shutdown_requested;
        bool should_exit;
        int  exit_code;
    } state = {
        .initialised        = false,
        .shutdown_requested = false,
        .should_exit        = false,
        .exit_code          = 1,
    };

    LspState lsp_state = {0};
    lsp_init(&lsp_state);

    while (!state.should_exit) {
        JsonValue* message = lsp_read_message(&message_arena);
        if (!message) {
            lsp_log("LSP input closed");
            state.should_exit = true;
            state.exit_code   = state.shutdown_requested ? 0 : state.exit_code;
            break;
        }

        string output = json_stringify(
            &message_arena, message, .pretty = false, .indent = 2);
        lsp_log("Received message:\n" STRINGP, STRINGV(output));

        JsonValue* id      = json_get_cstr(message, "id");
        JsonValue* method  = json_get_cstr(message, "method");
        string     command = (method && method->kind == JSON_STRING)
                                 ? json_string(method)
                                 : s("<unknown>");

        lsp_log("MESSAGE: " STRINGP, STRINGV(command));
        if (id) {
            if (id->kind == JSON_NUMBER) {
                lsp_log("     ID: %lld", json_integer(id));
            } else if (id->kind == JSON_STRING) {
                lsp_log("     ID: " STRINGP, STRINGV(json_string(id)));
            }
        }

        // Check for shutdown or exit
        if (!method || method->kind != JSON_STRING) {
            lsp_log("Ignoring message without string method");
            arena_reset(&message_arena);
            continue;
        }

        string     method_str = json_string(method);
        LspMessage msg        = {
                   .id      = id,
                   .method  = method_str,
                   .message = message,
                   .arena   = &message_arena,
        };

        if (method && string_eq_cstr(method_str, "shutdown")) {
            lsp_log("Shutdown requested");
            state.shutdown_requested = true;
            state.exit_code          = 0;
            lsp_handle_shutdown(&lsp_state, &msg);
        } else if (method && string_eq_cstr(method_str, "exit")) {
            lsp_log("Exit requested");
            state.exit_code   = state.shutdown_requested ? 0 : 1;
            state.should_exit = true;
        } else {
            // Handle other messages
            bool handled = false;
            for (usize i = 0;
                 i < sizeof(lsp_handlers) / sizeof(lsp_handlers[0]);
                 i++) {
                if (string_eq_cstr(method_str, lsp_handlers[i].method)) {
                    lsp_handlers[i].handler(&lsp_state, &msg);
                    handled = true;
                    break;
                }
            }
            if (!handled) {
                lsp_log("No handler for method: " STRINGP, STRINGV(method_str));
            }
        }

        arena_reset(&message_arena);
    }

    lsp_done(&lsp_state);
    arena_done(&message_arena);
    lsp_log("LSP server exiting with code %d", state.exit_code);
    return state.exit_code;
}

//------------------------------------------------------------------------------

void lsp_handle_initialise(LspState* state, const LspMessage* message)
{
    UNUSED(state);

    JsonValue* response = lsp_prepare_response(message);
    JsonValue* result   = json_new_object(message->arena);

    JsonValue* client_name =
        json_get_cstr(message->message, "params.clientInfo.name");
    JsonValue* client_version =
        json_get_cstr(message->message, "params.clientInfo.version");
    if (client_name && client_name->kind == JSON_STRING) {
        string version = (client_version && client_version->kind == JSON_STRING)
                             ? json_string(client_version)
                             : s("unknown");
        lsp_log("Connected to: " STRINGP " (" STRINGP ")",
                STRINGV(json_string(client_name)),
                STRINGV(version));
    }

    Arena*     arena       = message->arena;
    JsonValue* server_info = json_new_object(arena);
    json_object_set_string(server_info, arena, "name", s("Nerd LSP"));
    json_object_set_string(server_info, arena, "version", s("0.1.0"));

    JsonValue* capabilities = json_new_object(arena);
    json_object_set_number(
        capabilities, arena, "textDocumentSync", 1); // Full sync
    json_object_set_bool(capabilities, arena, "hoverProvider", true);
    json_object_set_bool(capabilities, arena, "definitionProvider", true);
    json_object_set_bool(capabilities, arena, "documentSymbolProvider", true);

    json_object_set_object(result, "serverInfo", server_info);
    json_object_set(result, "capabilities", capabilities);
    json_object_set_object(response, "result", result);

    lsp_send_response(arena, response);
}

void lsp_handle_shutdown(LspState* state, const LspMessage* message)
{
    UNUSED(state);

    JsonValue* response = lsp_prepare_response(message);
    json_object_set_null(response, message->arena, "result");
    lsp_send_response(message->arena, response);
}

void lsp_handle_initialised(LspState* state, const LspMessage* message)
{
    UNUSED(state);
    UNUSED(message);
    lsp_log("Client initialised");
}
