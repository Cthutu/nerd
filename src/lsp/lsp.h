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
    string     method;  // The method name extracted from the message
    JsonValue* id;      // The ID extracted from the message
    JsonValue* message; // The full incoming message
    Arena*     arena;   // Use this to do create responses
} LspMessage;

DEF_MAP(LspDocumentMap,
        Arena); // Map of document URI to internal document representation

typedef struct {
    LspDocumentMap documents;
} LspState;

//------------------------------------------------------------------------------

void lsp_log(cstr format, ...);
int  lsp_run(void);

//------------------------------------------------------------------------------

void lsp_init(LspState* state);
void lsp_done(LspState* state);

//------------------------------------------------------------------------------

JsonValue* lsp_prepare_response(const LspMessage* message);
void       lsp_send_response(Arena* arena, const JsonValue* response);

//------------------------------------------------------------------------------

void lsp_handle_initialise(LspState* state, const LspMessage* message);
void lsp_handle_shutdown(LspState* state, const LspMessage* message);
void lsp_handle_initialised(LspState* state, const LspMessage* message);
void lsp_handle_did_open(LspState* state, const LspMessage* message);
void lsp_handle_did_close(LspState* state, const LspMessage* message);

//------------------------------------------------------------------------------
