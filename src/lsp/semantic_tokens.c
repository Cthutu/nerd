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

//------------------------------------------------------------------------------
// Return the AST binding node that starts at a specific token index.

internal u32 lsp_semantic_find_bind_node(const Ast* ast, u32 token_index)
{
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind == AK_Bind && node->token_index == token_index) {
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
// Return the declaration index associated with one binding symbol handle.

internal u32 lsp_semantic_find_decl_index_by_symbol_handle(const Sema* sema,
                                                           u32 symbol_handle)
{
    for (u32 i = 0; i < array_count(sema->decls); ++i) {
        if (sema->decls[i].symbol_handle == symbol_handle) {
            return i;
        }
    }

    return U32_MAX;
}

//------------------------------------------------------------------------------
// Classify one symbol token by semantic role.

internal u32 lsp_semantic_symbol_type(const LspDocument* doc, u32 token_index)
{
    u32 bind_node_index =
        lsp_semantic_find_bind_node(&doc->front_end.ast, token_index);
    if (bind_node_index != U32_MAX) {
        u32 decl_index = lsp_semantic_find_decl_index_by_symbol_handle(
            &doc->front_end.sema, doc->front_end.ast.nodes[bind_node_index].a);
        if (decl_index != U32_MAX &&
            (doc->front_end.sema.decls[decl_index].kind == SK_Function ||
             doc->front_end.sema.decls[decl_index].kind ==
                 SK_BuiltinFunction)) {
            return LSP_SEMANTIC_FUNCTION;
        }
        return LSP_SEMANTIC_VARIABLE;
    }

    u32 ref_node_index =
        lsp_semantic_find_symbol_ref_node(&doc->front_end.ast, token_index);
    if (ref_node_index != U32_MAX &&
        ref_node_index < array_count(doc->front_end.sema.node_decl_indices)) {
        u32 decl_index = doc->front_end.sema.node_decl_indices[ref_node_index];
        if (decl_index != U32_MAX &&
            (doc->front_end.sema.decls[decl_index].kind == SK_Function ||
             doc->front_end.sema.decls[decl_index].kind ==
                 SK_BuiltinFunction)) {
            return LSP_SEMANTIC_FUNCTION;
        }
    }

    return LSP_SEMANTIC_VARIABLE;
}

//------------------------------------------------------------------------------
// Return whether a token kind should emit a semantic token.

internal bool
lsp_semantic_token_type(const LspDocument* doc, u32 token_index, u32* out_type)
{
    const Token* token = &doc->front_end.lexer.tokens[token_index];

    switch (token->kind) {
    case TK_Symbol:
        *out_type = lsp_semantic_symbol_type(doc, token_index);
        return true;

    case TK_fn:
    case TK_for:
    case TK_on:
    case TK_else:
    case TK_break:
    case TK_continue:
    case TK_return:
    case TK_true:
    case TK_false:
        *out_type = LSP_SEMANTIC_KEYWORD;
        return true;

    case TK_Integer:
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
    case TK_At:
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

    string       uri = json_string(uri_value);
    LspDocument* doc = LspDocumentMap_find(&state->documents, uri);
    if (!doc || !doc->analysis_ok) {
        lsp_cancel(response, message->arena);
        return;
    }

    JsonValue* result         = json_new_object(message->arena);
    JsonValue* data           = json_new_array(message->arena);
    u32        previous_line  = 0;
    u32        previous_start = 0;
    bool       have_previous  = false;

    for (u32 i = 0; i < array_count(doc->front_end.lexer.tokens); ++i) {
        u32 type = 0;
        if (!lsp_semantic_token_type(doc, i, &type)) {
            continue;
        }

        const Token* token = &doc->front_end.lexer.tokens[i];
        u32          line  = 0;
        u32          start = 0;
        bool         ok    = lex_offset_to_line_col(
            doc->front_end.lexer.source, token->offset, &line, &start);
        ASSERT(ok, "Expected valid token offset");
        UNUSED(ok);

        u32 end    = (u32)lex_token_end_offset(&doc->front_end.lexer, token);
        u32 length = end - token->offset;

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
