//------------------------------------------------------------------------------
// LSP module
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------
//> use: core object compiler/lexer

#pragma once

#include <compiler/lexer/lexer.h>
#include <core/core.h>
#include <object/object.h>

typedef struct LspMessage {
    string     method;  // The method name extracted from the message
    JsonValue* id;      // The ID extracted from the message
    JsonValue* message; // The full incoming message
    Arena*     arena;   // Use this to do create responses
} LspMessage;

typedef struct {
    Arena arena; // Arena for storing document content
    Lexer lexer; // Lexer for tokenizing the document
} LspDocument;

DEF_MAP(LspDocumentMap,
        LspDocument); // Map of document URI to internal document representation

typedef struct {
    LspDocumentMap documents;
} LspState;

//------------------------------------------------------------------------------

void lsp_log(cstr format, ...);
int  lsp_run(void);

//------------------------------------------------------------------------------
// LSP lifecycle

void lsp_init(LspState* state);
void lsp_done(LspState* state);

//------------------------------------------------------------------------------
// LSP document utilities

void lsp_document_done(LspDocument* doc);

//------------------------------------------------------------------------------
// LSP message handling

JsonValue* lsp_prepare_response(const LspMessage* message);
void       lsp_send_response(Arena* arena, const JsonValue* response);

void lsp_handle_initialise(LspState* state, const LspMessage* message);
void lsp_handle_shutdown(LspState* state, const LspMessage* message);
void lsp_handle_initialised(LspState* state, const LspMessage* message);
void lsp_handle_did_open(LspState* state, const LspMessage* message);
void lsp_handle_did_close(LspState* state, const LspMessage* message);
void lsp_handle_hover(LspState* state, const LspMessage* message);

//------------------------------------------------------------------------------
