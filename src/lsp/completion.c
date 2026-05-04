//------------------------------------------------------------------------------
// LSP completion support
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/sema/sema.h>
#include <lsp/lsp.h>

//------------------------------------------------------------------------------

internal void
lsp_completion_add(Arena* arena, JsonValue* items, string label, u32 kind)
{
    JsonValue* item = json_new_object(arena);
    json_object_set_string(item, arena, "label", label);
    json_object_set_number(item, arena, "kind", kind);
    json_array_push(items, item);
}

internal u32 lsp_completion_decl_kind(SemaDeclKind kind)
{
    switch (kind) {
    case SK_Function:
    case SK_FfiFunction:
    case SK_BuiltinFunction:
    case SK_GenericFunction:
        return 3; // Function
    case SK_Module:
        return 9; // Module
    case SK_TypeAlias:
    case SK_GenericTypeAlias:
        return 22; // Struct
    case SK_Constant:
        return 21; // Constant
    case SK_Variable:
        return 6; // Variable
    }
    return 1; // Text
}

internal u32 lsp_completion_local_kind(SemaLocalKind kind)
{
    switch (kind) {
    case SLK_Function:
        return 3; // Function
    case SLK_TypeAlias:
        return 22; // Struct
    case SLK_Constant:
        return 21; // Constant
    case SLK_Param:
    case SLK_Variable:
    case SLK_Binder:
        return 6; // Variable
    }
    return 1; // Text
}

internal void lsp_completion_add_keywords(Arena* arena, JsonValue* items)
{
    static cstr keywords[] = {
        "assert",
        "break",
        "continue",
        "defer",
        "else",
        "enum",
        "ffi",
        "fn",
        "for",
        "impl",
        "on",
        "pub",
        "return",
        "test",
        "union",
        "use",
    };

    for (usize i = 0; i < sizeof(keywords) / sizeof(keywords[0]); ++i) {
        lsp_completion_add(arena, items, s(keywords[i]), 14); // Keyword
    }
}

internal void lsp_completion_add_symbols(Arena*             arena,
                                         JsonValue*         items,
                                         const LspDocument* doc)
{
    if (!doc->analysis_ok) {
        return;
    }

    const Lexer* lexer = &doc->front_end.lexer;
    const Sema*  sema  = &doc->front_end.sema;
    for (u32 i = 0; i < array_count(sema->decls); ++i) {
        const SemaDecl* decl = &sema->decls[i];
        if (decl->symbol_handle == U32_MAX) {
            continue;
        }
        lsp_completion_add(arena,
                           items,
                           lex_symbol(lexer, decl->symbol_handle),
                           lsp_completion_decl_kind(decl->kind));
    }

    for (u32 i = 0; i < array_count(sema->locals); ++i) {
        const SemaLocal* local = &sema->locals[i];
        if (local->symbol_handle == U32_MAX) {
            continue;
        }
        lsp_completion_add(arena,
                           items,
                           lex_symbol(lexer, local->symbol_handle),
                           lsp_completion_local_kind(local->kind));
    }
}

void lsp_handle_completion(LspState* state, const LspMessage* message)
{
    JsonValue* response = lsp_prepare_response(message);

    string uri          = {0};
    if (!lsp_get_string_param(message, "params.textDocument.uri", &uri)) {
        lsp_cancel(response, message->arena);
        return;
    }

    LspDocument* doc = LspDocumentMap_find(&state->documents, uri);
    if (!doc) {
        lsp_cancel(response, message->arena);
        return;
    }

    JsonValue* items = json_new_array(message->arena);
    lsp_completion_add_keywords(message->arena, items);
    lsp_completion_add_symbols(message->arena, items, doc);
    json_object_set_array(response, "result", items);
    lsp_send_response(message->arena, response);
}

//------------------------------------------------------------------------------
