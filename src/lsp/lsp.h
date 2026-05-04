//------------------------------------------------------------------------------
// LSP module
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------
//> use: core object compiler/build compiler/cst

#pragma once

#include <compiler/build/build.h>
#include <compiler/cst/cst.h>
#include <core/core.h>
#include <object/object.h>

typedef struct LspMessage {
    string     method;  // The method name extracted from the message
    JsonValue* id;      // The ID extracted from the message
    JsonValue* message; // The full incoming message
    Arena*     arena;   // Use this to do create responses
} LspMessage;

typedef struct {
    Arena         source_arena;   // Arena for the current editor buffer
    Arena         arena;          // Arena for analysis data
    string        source;         // Current editor document source
    ProgramInfo   program;        // Program analysis for the current document
    FrontEndState front_end;      // Compiler front-end results for the document
    Cst           cst;            // Concrete syntax tree for editor tooling
    bool          analysis_ok;    // Whether front-end analysis succeeded
    bool          semantic_ready; // Whether semantic data is usable
    bool          has_cst;        // Whether CST parsing succeeded
} LspDocument;

DEF_MAP(LspDocumentMap,
        LspDocument); // Map of document URI to internal document representation

typedef struct {
    LspDocumentMap documents;
} LspState;

//------------------------------------------------------------------------------

void lsp_logv(cstr format, va_list args);
void lsp_log(cstr format, ...);

// Sends a null response and logs a message.
void lsp_fail(JsonValue* response, Arena* arena, cstr format, ...);

// Sends a null reponse, but not message is logged.
void lsp_cancel(JsonValue* response, Arena* arena);

// The main loop for the LSP server
int lsp_run(void);

//------------------------------------------------------------------------------
// LSP lifecycle

void lsp_init(LspState* state);
void lsp_done(LspState* state);

//------------------------------------------------------------------------------
// LSP document utilities

void  lsp_document_done(LspDocument* doc);
bool  lsp_get_string_param(const LspMessage* message,
                           cstr              param_path,
                           string*           out_str);
bool  lsp_get_u64_param(const LspMessage* message,
                        cstr              param_path,
                        u64*              out_value);
usize lsp_offset_from_position(string source, u64 line, u64 character);

//------------------------------------------------------------------------------
// LSP message handling

JsonValue* lsp_prepare_response(const LspMessage* message);
void       lsp_send_response(Arena* arena, JsonValue* response);
JsonValue* lsp_prepare_notification(Arena* arena, string method);
void lsp_publish_diagnostics(Arena* arena, string uri, JsonValue* diagnostics);

void lsp_handle_initialise(LspState* state, const LspMessage* message);
void lsp_handle_shutdown(LspState* state, const LspMessage* message);
void lsp_handle_initialised(LspState* state, const LspMessage* message);
void lsp_handle_did_open(LspState* state, const LspMessage* message);
void lsp_handle_did_change(LspState* state, const LspMessage* message);
void lsp_handle_did_close(LspState* state, const LspMessage* message);
void lsp_handle_hover(LspState* state, const LspMessage* message);
void lsp_handle_definition(LspState* state, const LspMessage* message);
void lsp_handle_document_symbol(LspState* state, const LspMessage* message);
void lsp_handle_semantic_tokens_full(LspState*         state,
                                     const LspMessage* message);
void lsp_handle_completion(LspState* state, const LspMessage* message);
void lsp_handle_signature_help(LspState* state, const LspMessage* message);

//------------------------------------------------------------------------------
