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
    bool header_ok =
        fprintf(stdout, "Content-Length: %zu\r\n\r\n", output.count) >= 0;
    usize written  = fwrite(output.data, 1, output.count, stdout);
    bool  flush_ok = fflush(stdout) == 0;
    if (!header_ok || written != output.count || !flush_ok) {
        lsp_log("Failed to write LSP response");
    }
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

//------------------------------------------------------------------------------
// Checked semantic accessors used by editor features. These keep partial sema
// products from leaking unchecked side-table indices into feature handlers.

bool lsp_ast_node(const Ast* ast, u32 node_index, const AstNode** out)
{
    if (!ast || node_index == U32_MAX ||
        node_index >= array_count(ast->nodes)) {
        return false;
    }
    if (out) {
        *out = &ast->nodes[node_index];
    }
    return true;
}

bool lsp_lexer_token(const Lexer* lexer, u32 token_index, const Token** out)
{
    if (!lexer || token_index == U32_MAX ||
        token_index >= array_count(lexer->tokens)) {
        return false;
    }
    if (out) {
        *out = &lexer->tokens[token_index];
    }
    return true;
}

bool lsp_token_range(const Lexer* lexer,
                     u32          token_index,
                     usize*       out_start,
                     usize*       out_end)
{
    const Token* token = NULL;
    if (!lsp_lexer_token(lexer, token_index, &token)) {
        return false;
    }
    if (out_start) {
        *out_start = token->offset;
    }
    if (out_end) {
        *out_end = lex_token_end_offset(lexer, token);
    }
    return true;
}

bool lsp_sema_decl(const Sema* sema, u32 decl_index, const SemaDecl** out)
{
    if (!sema || decl_index == U32_MAX ||
        decl_index >= array_count(sema->decls)) {
        return false;
    }
    if (out) {
        *out = &sema->decls[decl_index];
    }
    return true;
}

bool lsp_sema_local(const Sema* sema, u32 local_index, const SemaLocal** out)
{
    if (!sema || local_index == U32_MAX ||
        local_index >= array_count(sema->locals)) {
        return false;
    }
    if (out) {
        *out = &sema->locals[local_index];
    }
    return true;
}

bool lsp_sema_scope(const Sema* sema, u32 scope_index, const SemaScope** out)
{
    if (!sema || scope_index == U32_MAX ||
        scope_index >= array_count(sema->scopes)) {
        return false;
    }
    if (out) {
        *out = &sema->scopes[scope_index];
    }
    return true;
}

bool lsp_sema_type(const Sema* sema, u32 type_index, const SemaType** out)
{
    if (!sema || type_index == sema_no_type() ||
        type_index >= array_count(sema->types)) {
        return false;
    }
    if (out) {
        *out = &sema->types[type_index];
    }
    return true;
}

bool lsp_sema_decl_by_symbol(const Sema*     sema,
                             u32             symbol_handle,
                             const SemaDecl** out_decl,
                             u32*            out_decl_index)
{
    if (!sema || symbol_handle == U32_MAX) {
        return false;
    }
    for (u32 i = 0; i < array_count(sema->decls); ++i) {
        const SemaDecl* decl = &sema->decls[i];
        if (decl->symbol_handle == symbol_handle) {
            if (out_decl) {
                *out_decl = decl;
            }
            if (out_decl_index) {
                *out_decl_index = i;
            }
            return true;
        }
    }
    return false;
}

bool lsp_sema_node_decl(const Sema* sema, u32 node_index, u32* out_decl_index)
{
    if (!sema || node_index == U32_MAX ||
        node_index >= array_count(sema->node_decl_indices)) {
        return false;
    }

    u32 decl_index = sema->node_decl_indices[node_index];
    if (!lsp_sema_decl(sema, decl_index, NULL)) {
        return false;
    }
    if (out_decl_index) {
        *out_decl_index = decl_index;
    }
    return true;
}

bool lsp_sema_node_local(const Sema* sema, u32 node_index, u32* out_local_index)
{
    if (!sema || node_index == U32_MAX ||
        node_index >= array_count(sema->node_local_indices)) {
        return false;
    }

    u32 local_index = sema->node_local_indices[node_index];
    if (!lsp_sema_local(sema, local_index, NULL)) {
        return false;
    }
    if (out_local_index) {
        *out_local_index = local_index;
    }
    return true;
}

bool lsp_sema_node_scope(const Sema* sema, u32 node_index, u32* out_scope_index)
{
    if (!sema || node_index == U32_MAX ||
        node_index >= array_count(sema->node_scope_indices)) {
        return false;
    }

    u32 scope_index = sema->node_scope_indices[node_index];
    if (!lsp_sema_scope(sema, scope_index, NULL)) {
        return false;
    }
    if (out_scope_index) {
        *out_scope_index = scope_index;
    }
    return true;
}

bool lsp_sema_node_type(const Sema* sema, u32 node_index, u32* out_type_index)
{
    if (!sema || node_index == U32_MAX ||
        node_index >= array_count(sema->node_type_indices)) {
        return false;
    }

    u32 type_index = sema->node_type_indices[node_index];
    if (!lsp_sema_type(sema, type_index, NULL)) {
        return false;
    }
    if (out_type_index) {
        *out_type_index = type_index;
    }
    return true;
}

bool lsp_sema_type_param(const Sema* sema,
                         u32         param_index,
                         u32*        out_symbol,
                         u32*        out_type_index)
{
    if (!sema || param_index == U32_MAX ||
        param_index >= array_count(sema->type_param_symbols) ||
        param_index >= array_count(sema->type_param_types)) {
        return false;
    }

    u32 symbol     = sema->type_param_symbols[param_index];
    u32 type_index = sema->type_param_types[param_index];
    if (out_symbol) {
        *out_symbol = symbol;
    }
    if (out_type_index) {
        *out_type_index = type_index;
    }
    return true;
}

//------------------------------------------------------------------------------
// Checked module accessors used by editor features. These keep feature handlers
// from inferring imported-module readiness from raw ProgramInfo arrays.

bool lsp_program_module_view(const ProgramInfo* program,
                             u32                module_index,
                             LspModuleView*     out_view)
{
    if (!program || module_index == U32_MAX ||
        module_index >= array_count(program->modules)) {
        return false;
    }

    const ModuleInfo* module = &program->modules[module_index];
    if (!module->resolved_path) {
        return false;
    }

    if (out_view) {
        *out_view = (LspModuleView){
            .info         = module,
            .module_index = module_index,
            .lexer        = &module->front_end.lexer,
            .ast          = &module->front_end.ast,
            .sema         = &module->front_end.sema,
        };
    }
    return true;
}

bool lsp_program_module_view_by_path(const ProgramInfo* program,
                                     cstr               resolved_path,
                                     LspModuleView*     out_view)
{
    if (!program || !resolved_path) {
        return false;
    }

    for (u32 i = 0; i < array_count(program->modules); ++i) {
        const ModuleInfo* module = &program->modules[i];
        if (module->resolved_path &&
            strcmp(module->resolved_path, resolved_path) == 0) {
            return lsp_program_module_view(program, i, out_view);
        }
    }
    return false;
}

bool lsp_program_module_view_by_type(const ProgramInfo* program,
                                     const SemaType*    type,
                                     LspModuleView*     out_view)
{
    if (!type || type->kind != STK_Module) {
        return false;
    }
    return lsp_program_module_view(program, type->return_type, out_view);
}

bool lsp_module_export_decl(const LspModuleView* view,
                            u32                  export_index,
                            const SemaDecl**     out_decl,
                            u32*                 out_decl_index)
{
    if (!view || !view->info || export_index == U32_MAX ||
        export_index >= array_count(view->info->export_decl_indices)) {
        return false;
    }

    u32 decl_index = view->info->export_decl_indices[export_index];
    if (!lsp_sema_decl(view->sema, decl_index, out_decl)) {
        return false;
    }
    if (out_decl_index) {
        *out_decl_index = decl_index;
    }
    return true;
}

u32 lsp_module_export_count(const LspModuleView* view)
{
    if (!view || !view->info) {
        return 0;
    }
    return (u32)array_count(view->info->export_decl_indices);
}
