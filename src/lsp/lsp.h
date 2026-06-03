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
    bool          source_ready;   // Whether editor source is stored
    bool          tokens_ready;   // Derived lexer product availability
    bool          syntax_ready;   // Derived AST product availability
    bool          decls_ready;    // Declaration facts are available
    bool          bindings_ready; // Binding/reference facts are available
    bool          type_facts_partial;  // Best-effort type facts are available
    bool          type_facts_complete; // Checked type facts are available
    bool          sema_partial;        // Derived semantic product availability
    bool          sema_complete; // Derived complete semantic analysis state
    bool          cst_ready;     // Whether CST parsing succeeded
} LspDocument;

DEF_MAP(LspDocumentMap,
        LspDocument); // Map of document URI to internal document representation

typedef struct {
    Arena          arena;
    LspDocumentMap documents;
    string         workspace_root_source_path;
} LspState;

typedef struct {
    const LspDocument* doc;
    string             source;
} LspSourceView;

typedef struct {
    const LspDocument* doc;
    string             source;
    const Lexer*       lexer;
} LspTokenView;

typedef struct {
    const LspDocument* doc;
    string             source;
    const Lexer*       lexer;
    const Ast*         ast;
} LspSyntaxView;

typedef struct {
    const LspDocument* doc;
    string             source;
    const Lexer*       lexer;
    const Ast*         ast;
    const Sema*        sema;
} LspSemanticView;

typedef struct {
    const LspDocument* doc;
    string             source;
    const Lexer*       lexer;
    const Ast*         ast;
    const Sema*        sema;
} LspDeclarationView;

typedef struct {
    const LspDocument* doc;
    string             source;
    const Lexer*       lexer;
    const Ast*         ast;
    const Sema*        sema;
} LspBindingView;

typedef struct {
    const LspDocument* doc;
    string             source;
    const Lexer*       lexer;
    const Ast*         ast;
    const Sema*        sema;
} LspTypeFactView;

typedef struct {
    const ModuleInfo* info;
    u32               module_index;
    const Lexer*      lexer;
    const Ast*        ast;
    const Sema*       sema;
} LspModuleView;

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

void lsp_document_done(LspDocument* doc);
bool lsp_source_view(LspState* state, string uri, LspSourceView* out_view);
bool lsp_token_view(LspState* state, string uri, LspTokenView* out_view);
bool lsp_syntax_view(LspState* state, string uri, LspSyntaxView* out_view);
bool lsp_semantic_view(LspState* state, string uri, LspSemanticView* out_view);
bool lsp_declaration_view(LspState*           state,
                          string              uri,
                          LspDeclarationView* out_view);
bool lsp_binding_view(LspState* state, string uri, LspBindingView* out_view);
bool lsp_type_fact_view(LspState* state, string uri, LspTypeFactView* out_view);
bool lsp_program_module_view(const ProgramInfo* program,
                             u32                module_index,
                             LspModuleView*     out_view);
bool lsp_program_module_view_by_path(const ProgramInfo* program,
                                     cstr               resolved_path,
                                     LspModuleView*     out_view);
bool lsp_program_module_view_by_type(const ProgramInfo* program,
                                     const SemaType*    type,
                                     LspModuleView*     out_view);
bool lsp_get_string_param(const LspMessage* message,
                          cstr              param_path,
                          string*           out_str);
bool lsp_get_u64_param(const LspMessage* message,
                       cstr              param_path,
                       u64*              out_value);
usize lsp_offset_from_position(string source, u64 line, u64 character);
bool  lsp_ast_node(const Ast* ast, u32 node_index, const AstNode** out);
bool  lsp_lexer_token(const Lexer* lexer, u32 token_index, const Token** out);
bool  lsp_token_range(const Lexer* lexer,
                      u32          token_index,
                      usize*       out_start,
                      usize*       out_end);
bool  lsp_use_module_path_range(const LspDocument* doc,
                                u32                use_index,
                                usize*             out_start,
                                usize*             out_end);
bool  lsp_sema_decl(const Sema* sema, u32 decl_index, const SemaDecl** out);
bool  lsp_sema_local(const Sema* sema, u32 local_index, const SemaLocal** out);
bool  lsp_sema_scope(const Sema* sema, u32 scope_index, const SemaScope** out);
bool  lsp_sema_type(const Sema* sema, u32 type_index, const SemaType** out);
bool  lsp_sema_decl_by_symbol(const Sema*      sema,
                              u32              symbol_handle,
                              const SemaDecl** out_decl,
                              u32*             out_decl_index);
bool  lsp_decl_view_decl(const LspDeclarationView* view,
                         u32                       decl_index,
                         const SemaDecl**          out);
bool  lsp_decl_view_decl_by_symbol(const LspDeclarationView* view,
                                   u32                       symbol_handle,
                                   const SemaDecl**          out_decl,
                                   u32*                      out_decl_index);
bool  lsp_binding_view_local(const LspBindingView* view,
                             u32                   local_index,
                             const SemaLocal**     out);
bool  lsp_binding_view_scope(const LspBindingView* view,
                             u32                   scope_index,
                             const SemaScope**     out);
bool  lsp_binding_view_node_decl(const LspBindingView* view,
                                 u32                   node_index,
                                 u32*                  out_decl_index);
bool  lsp_binding_view_node_local(const LspBindingView* view,
                                  u32                   node_index,
                                  u32*                  out_local_index);
bool  lsp_binding_view_node_scope(const LspBindingView* view,
                                  u32                   node_index,
                                  u32*                  out_scope_index);
bool  lsp_type_fact_view_type(const LspTypeFactView* view,
                              u32                    type_index,
                              const SemaType**       out);
bool  lsp_type_fact_view_node_type(const LspTypeFactView* view,
                                   u32                    node_index,
                                   u32*                   out_type_index);
bool  lsp_type_fact_view_type_param(const LspTypeFactView* view,
                                    u32                    param_index,
                                    u32*                   out_symbol,
                                    u32*                   out_type_index);
bool  lsp_sema_node_decl(const Sema* sema, u32 node_index, u32* out_decl_index);
bool  lsp_sema_node_local(const Sema* sema,
                          u32         node_index,
                          u32*        out_local_index);
bool  lsp_sema_node_scope(const Sema* sema,
                          u32         node_index,
                          u32*        out_scope_index);
bool  lsp_sema_node_type(const Sema* sema, u32 node_index, u32* out_type_index);
bool  lsp_sema_type_param(const Sema* sema,
                          u32         param_index,
                          u32*        out_symbol,
                          u32*        out_type_index);
bool  lsp_module_export_decl(const LspModuleView* view,
                             u32                  export_index,
                             const SemaDecl**     out_decl,
                             u32*                 out_decl_index);
u32   lsp_module_export_count(const LspModuleView* view);

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
void lsp_handle_document_link(LspState* state, const LspMessage* message);
void lsp_handle_document_symbol(LspState* state, const LspMessage* message);
void lsp_handle_workspace_symbol(LspState* state, const LspMessage* message);
void lsp_handle_semantic_tokens_full(LspState*         state,
                                     const LspMessage* message);
void lsp_handle_completion(LspState* state, const LspMessage* message);
void lsp_handle_code_action(LspState* state, const LspMessage* message);
void lsp_handle_signature_help(LspState* state, const LspMessage* message);
void lsp_handle_references(LspState* state, const LspMessage* message);
void lsp_handle_prepare_rename(LspState* state, const LspMessage* message);
void lsp_handle_rename(LspState* state, const LspMessage* message);

//------------------------------------------------------------------------------
