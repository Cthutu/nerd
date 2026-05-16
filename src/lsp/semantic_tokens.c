//------------------------------------------------------------------------------
// Semantic token handling
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <lsp/lsp.h>

typedef enum {
    LSP_SEMANTIC_VARIABLE,
    LSP_SEMANTIC_FUNCTION,
    LSP_SEMANTIC_KEYWORD,
    LSP_SEMANTIC_NUMBER,
    LSP_SEMANTIC_OPERATOR,
    LSP_SEMANTIC_STRING,
} LspSemanticTokenType;

typedef struct {
    u32 line;
    u32 start;
    u32 length;
    u32 type;
} LspSemanticToken;

typedef struct {
    usize start_offset;
    usize end_offset;
    u32   start_line;
    u32   start_col;
} LspSemanticVisibleRange;

//------------------------------------------------------------------------------
// Return whether `haystack` starts with `needle`.

internal bool lsp_semantic_string_starts_with(string haystack, string needle)
{
    if (needle.count > haystack.count) {
        return false;
    }

    return memcmp(haystack.data, needle.data, needle.count) == 0;
}

//------------------------------------------------------------------------------
// Find one string inside another.

internal bool
lsp_semantic_string_find(string haystack, string needle, usize* out_offset)
{
    if (needle.count == 0 || needle.count > haystack.count) {
        return false;
    }

    for (usize i = 0; i + needle.count <= haystack.count; ++i) {
        if (memcmp(haystack.data + i, needle.data, needle.count) == 0) {
            *out_offset = i;
            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------
// Return the slice of the analysed source that belongs to the open editor
// document. Folder modules can analyse a combined `mod.n` plus sibling parts
// source; LSP semantic token positions still have to be reported relative to
// the file the editor opened.

internal bool lsp_semantic_visible_range(const LspDeclarationView* view,
                                         LspSemanticVisibleRange*  out_range)
{
    const NerdSource analysed = view->lexer->source;
    string           source   = analysed.source;

    if (string_eq(source, view->source) ||
        lsp_semantic_string_starts_with(source, view->source)) {
        out_range->start_offset = 0;
    } else if (!lsp_semantic_string_find(
                   source, view->source, &out_range->start_offset)) {
        return false;
    }

    out_range->end_offset = out_range->start_offset + view->source.count;
    if (!lex_offset_to_line_col(analysed,
                                out_range->start_offset,
                                &out_range->start_line,
                                &out_range->start_col)) {
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
// Return the AST binding node that starts at a specific token index.

internal u32 lsp_semantic_find_bind_node(const Ast* ast, u32 token_index)
{
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (ast_node_is_binding_like(node) &&
            node->token_index == token_index) {
            return i;
        }
    }

    return U32_MAX;
}

//------------------------------------------------------------------------------
// Return the AST symbol-reference node that starts at a specific token index.

internal u32 lsp_semantic_find_symbol_ref_node(const Ast* ast, u32 token_index)
{
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind == AK_SymbolRef && node->token_index == token_index) {
            return i;
        }
    }

    return U32_MAX;
}

//------------------------------------------------------------------------------
// Return the AST field node that starts at a specific token index.

internal u32 lsp_semantic_find_field_node(const Ast* ast, u32 token_index)
{
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind == AK_Field && node->token_index == token_index) {
            return i;
        }
    }

    return U32_MAX;
}

//------------------------------------------------------------------------------
// Return the declaration index associated with one binding symbol handle.

// Return whether one declaration index names a function-like symbol.

internal bool lsp_semantic_decl_is_function(const Sema* sema, u32 decl_index)
{
    const SemaDecl* decl = NULL;
    if (!lsp_sema_decl(sema, decl_index, &decl)) {
        return false;
    }

    return decl->kind == SK_Function || decl->kind == SK_GenericFunction ||
           decl->kind == SK_FfiFunction || decl->kind == SK_BuiltinFunction;
}

//------------------------------------------------------------------------------
// Classify one symbol token by semantic role.

internal u32 lsp_semantic_symbol_type(const LspDeclarationView* view,
                                      u32                       token_index)
{
    u32 bind_node_index = lsp_semantic_find_bind_node(view->ast, token_index);
    if (bind_node_index != U32_MAX) {
        const AstNode* bind       = NULL;
        u32            decl_index = U32_MAX;
        if (lsp_ast_node(view->ast, bind_node_index, &bind) &&
            lsp_sema_decl_by_symbol(view->sema, bind->a, NULL, &decl_index) &&
            lsp_semantic_decl_is_function(view->sema, decl_index)) {
            return LSP_SEMANTIC_FUNCTION;
        }
        return LSP_SEMANTIC_VARIABLE;
    }

    u32 ref_node_index =
        lsp_semantic_find_symbol_ref_node(view->ast, token_index);
    u32 decl_index = U32_MAX;
    if (lsp_sema_node_decl(view->sema, ref_node_index, &decl_index)) {
        if (lsp_semantic_decl_is_function(view->sema, decl_index)) {
            return LSP_SEMANTIC_FUNCTION;
        }
    }

    u32 field_node_index = lsp_semantic_find_field_node(view->ast, token_index);
    if (lsp_sema_node_decl(view->sema, field_node_index, &decl_index)) {
        if (lsp_semantic_decl_is_function(view->sema, decl_index)) {
            return LSP_SEMANTIC_FUNCTION;
        }
    }

    return LSP_SEMANTIC_VARIABLE;
}

//------------------------------------------------------------------------------
// Return whether one symbol token is the contextual source-test keyword.

internal bool lsp_semantic_is_test_keyword(const LspDeclarationView* view,
                                           u32 token_index)
{
    const Lexer* lexer = view->lexer;
    const Token* token = NULL;
    const Token* next  = NULL;
    if (!lsp_lexer_token(lexer, token_index, &token) ||
        !lsp_lexer_token(lexer, token_index + 1, &next)) {
        return false;
    }
    if (token->kind != TK_Symbol) {
        return false;
    }

    usize  end  = lex_token_end_offset(lexer, token);
    string text = string_from(lexer->source.source.data + token->offset,
                              end - token->offset);
    if (!string_eq_cstr(text, "test")) {
        return false;
    }

    return next->kind == TK_String;
}

//------------------------------------------------------------------------------
// Return whether a token kind should emit a semantic token.

internal bool lsp_semantic_token_type(const LspDeclarationView* view,
                                      u32                       token_index,
                                      u32*                      out_type)
{
    const Token* token = NULL;
    if (!lsp_lexer_token(view->lexer, token_index, &token)) {
        return false;
    }

    switch (token->kind) {
    case TK_Symbol:
        if (lsp_semantic_is_test_keyword(view, token_index)) {
            *out_type = LSP_SEMANTIC_KEYWORD;
            return true;
        }
        *out_type = lsp_semantic_symbol_type(view, token_index);
        return true;

    case TK_fn:
    case TK_for:
    case TK_on:
    case TK_else:
    case TK_defer:
    case TK_assert:
    case TK_break:
    case TK_again:
    case TK_return:
    case TK_plex:
    case TK_union:
    case TK_enum:
    case TK_ffi:
    case TK_use:
    case TK_pub:
    case TK_impl:
    case TK_pragma:
    case TK_with:
    case TK_in:
    case TK_as:
    case TK_yes:
    case TK_no:
    case TK_nil:
    case TK_undefined:
        *out_type = LSP_SEMANTIC_KEYWORD;
        return true;

    case TK_Integer:
    case TK_Float:
        *out_type = LSP_SEMANTIC_NUMBER;
        return true;
    case TK_String:
    case TK_InterpolatedStringStart:
    case TK_InterpolatedStringEnd:
        *out_type = LSP_SEMANTIC_STRING;
        return true;

    case TK_Plus:
    case TK_PlusEqual:
    case TK_Minus:
    case TK_MinusEqual:
    case TK_Star:
    case TK_StarEqual:
    case TK_Slash:
    case TK_SlashEqual:
    case TK_Percent:
    case TK_PercentEqual:
    case TK_AmpEqual:
    case TK_AmpAmpEqual:
    case TK_PipeEqual:
    case TK_PipePipeEqual:
    case TK_CaretEqual:
    case TK_Dot:
    case TK_LBracket:
    case TK_RBracket:
    case TK_At:
    case TK_Dollar:
    case TK_Colon:
    case TK_FatArrow:
    case TK_ThinArrow:
        *out_type = LSP_SEMANTIC_OPERATOR;
        return true;

    default:
        return false;
    }
}

//------------------------------------------------------------------------------
// Append one token to the semantic token stream in LSP delta encoding.

internal void lsp_semantic_push_encoded(JsonValue* array,
                                        Arena*     arena,
                                        u32        previous_line,
                                        u32        previous_start,
                                        u32        line,
                                        u32        start,
                                        u32        length,
                                        u32        type)
{
    u32 delta_line  = line - previous_line;
    u32 delta_start = delta_line == 0 ? start - previous_start : start;

    json_array_push(array, json_new_number(arena, delta_line));
    json_array_push(array, json_new_number(arena, delta_start));
    json_array_push(array, json_new_number(arena, length));
    json_array_push(array, json_new_number(arena, type));
    json_array_push(array, json_new_number(arena, 0));
}

//------------------------------------------------------------------------------
// Respond to semantic token requests with full-document token data.

void lsp_handle_semantic_tokens_full(LspState* state, const LspMessage* message)
{
    JsonValue* uri_value =
        json_get_cstr(message->message, "params.textDocument.uri");
    JsonValue* response = lsp_prepare_response(message);

    if (!uri_value || uri_value->kind != JSON_STRING) {
        lsp_cancel(response, message->arena);
        return;
    }

    string             uri  = json_string(uri_value);
    LspDeclarationView view = {0};
    if (!lsp_declaration_view(state, uri, &view)) {
        lsp_cancel(response, message->arena);
        return;
    }

    JsonValue*              result         = json_new_object(message->arena);
    JsonValue*              data           = json_new_array(message->arena);
    u32                     previous_line  = 0;
    u32                     previous_start = 0;
    bool                    have_previous  = false;
    LspSemanticVisibleRange visible        = {0};
    if (!lsp_semantic_visible_range(&view, &visible)) {
        lsp_cancel(response, message->arena);
        return;
    }

    for (u32 i = 0; i < array_count(view.lexer->tokens); ++i) {
        u32 type = 0;
        if (!lsp_semantic_token_type(&view, i, &type)) {
            continue;
        }

        const Token* token = NULL;
        usize        end   = 0;
        if (!lsp_lexer_token(view.lexer, i, &token) ||
            !lsp_token_range(view.lexer, i, NULL, &end)) {
            continue;
        }
        if (token->offset < visible.start_offset || end > visible.end_offset) {
            continue;
        }

        u32  line  = 0;
        u32  start = 0;
        bool ok    = lex_offset_to_line_col(
            view.lexer->source, token->offset, &line, &start);
        ASSERT(ok, "Expected valid token offset");
        UNUSED(ok);

        u32 length = (u32)(end - token->offset);
        if (line == visible.start_line) {
            start -= visible.start_col;
        }
        line -= visible.start_line;

        if (!have_previous) {
            lsp_semantic_push_encoded(
                data, message->arena, 0, 0, line, start, length, type);
            have_previous = true;
        } else {
            lsp_semantic_push_encoded(data,
                                      message->arena,
                                      previous_line,
                                      previous_start,
                                      line,
                                      start,
                                      length,
                                      type);
        }

        previous_line  = line;
        previous_start = start;
    }

    json_object_set_array(result, "data", data);
    json_object_set_object(response, "result", result);
    lsp_send_response(message->arena, response);
}

//------------------------------------------------------------------------------
