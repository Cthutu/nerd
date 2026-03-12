//------------------------------------------------------------------------------
// LSP module
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------
//> use: object

#pragma once

#include <core/core.h>
#include <object/object.h>

typedef struct LspMessage {
    string     method;
    JsonValue* id;
    JsonValue* message;
    Arena*     arena;
} LspMessage;

//------------------------------------------------------------------------------

void lsp_log(cstr format, ...);
int  lsp_run(void);

//------------------------------------------------------------------------------

JsonValue* lsp_prepare_response(const LspMessage* message);
void       lsp_send_response(Arena* arena, const JsonValue* response);

//------------------------------------------------------------------------------

void lsp_handle_initialize(const LspMessage* message);
void lsp_handle_shutdown(const LspMessage* message);
void lsp_handle_initialized(const LspMessage* message);
void lsp_handle_did_open(const LspMessage* message);
void lsp_handle_did_close(const LspMessage* message);

//------------------------------------------------------------------------------
