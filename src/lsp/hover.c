//------------------------------------------------------------------------------
// Symbol-aware LSP queries
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <lsp/lsp.h>

#define LSP_NO_DECL UINT32_MAX
#define LSP_SYMBOL_KIND_FUNCTION 12
#define LSP_SYMBOL_KIND_CONSTANT 14

//------------------------------------------------------------------------------
// Convert one source offset into an LSP position object.

internal JsonValue*
lsp_make_position(Arena* arena, NerdSource source, usize offset)
{
    u32 line = 0;
    u32 col  = 0;
    if (!lex_offset_to_line_col(source, offset, &line, &col)) {
        kill("Expected valid source offset: %zu", offset);
    }

    JsonValue* position = json_new_object(arena);
    json_object_set_number(position, arena, "line", line);
    json_object_set_number(position, arena, "character", col);
    return position;
}

//------------------------------------------------------------------------------
// Convert one source offset pair into an LSP range object.

internal JsonValue* lsp_make_range(Arena*     arena,
                                   NerdSource source,
                                   usize      start_offset,
                                   usize      end_offset)
{
    JsonValue* range = json_new_object(arena);
    json_object_set_object(
        range, "start", lsp_make_position(arena, source, start_offset));
    json_object_set_object(
        range, "end", lsp_make_position(arena, source, end_offset));
    return range;
}

//------------------------------------------------------------------------------
// Return the index of a token pointer inside the lexer's token table.

internal u32 lsp_token_index_from_pointer(const Lexer* lexer,
                                          const Token* token)
{
    ASSERT(token >= lexer->tokens &&
               token < lexer->tokens + array_count(lexer->tokens),
           "Expected token pointer to belong to the lexer");
    return (u32)(token - lexer->tokens);
}

//------------------------------------------------------------------------------
// Return the source offsets of one token in the current document.

internal void lsp_token_offsets(const Lexer* lexer,
                                u32          token_index,
                                usize*       out_start,
                                usize*       out_end)
{
    const Token* token = &lexer->tokens[token_index];
    *out_start         = token->offset;
    *out_end           = lex_token_end_offset(lexer, token);
}

//------------------------------------------------------------------------------
// Return the AST binding node that owns a given token index, if any.

internal u32 lsp_find_bind_node_at_token(const Ast* ast, u32 token_index)
{
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if ((node->kind == AK_Bind || node->kind == AK_Variable) &&
            node->token_index == token_index) {
            return i;
        }
    }

    return U32_MAX;
}

//------------------------------------------------------------------------------
// Return the AST symbol-reference node that owns a given token index, if any.

internal u32 lsp_find_symbol_ref_node_at_token(const Ast* ast, u32 token_index)
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
// Return the semantic declaration index matching one symbol handle.

internal u32 lsp_find_decl_index_by_symbol_handle(const Sema* sema,
                                                  u32         symbol_handle)
{
    for (u32 i = 0; i < array_count(sema->decls); ++i) {
        if (sema->decls[i].symbol_handle == symbol_handle) {
            return i;
        }
    }

    return LSP_NO_DECL;
}

//------------------------------------------------------------------------------
// Return the semantic declaration index referenced by one token.

internal u32 lsp_find_decl_index_for_token(const LspDocument* doc,
                                           u32                token_index)
{
    u32 bind_node_index =
        lsp_find_bind_node_at_token(&doc->front_end.ast, token_index);
    if (bind_node_index != U32_MAX) {
        return lsp_find_decl_index_by_symbol_handle(
            &doc->front_end.sema, doc->front_end.ast.nodes[bind_node_index].a);
    }

    u32 ref_node_index =
        lsp_find_symbol_ref_node_at_token(&doc->front_end.ast, token_index);
    if (ref_node_index != U32_MAX &&
        ref_node_index < array_count(doc->front_end.sema.node_decl_indices)) {
        return doc->front_end.sema.node_decl_indices[ref_node_index];
    }

    return LSP_NO_DECL;
}

internal u32 lsp_find_local_index_for_token(const LspDocument* doc,
                                            u32                token_index)
{
    u32 bind_node_index =
        lsp_find_bind_node_at_token(&doc->front_end.ast, token_index);
    if (bind_node_index != U32_MAX &&
        bind_node_index < array_count(doc->front_end.sema.node_local_indices)) {
        u32 local_index =
            doc->front_end.sema.node_local_indices[bind_node_index];
        if (local_index != sema_no_local()) {
            return local_index;
        }
    }

    u32 ref_node_index =
        lsp_find_symbol_ref_node_at_token(&doc->front_end.ast, token_index);
    if (ref_node_index != U32_MAX &&
        ref_node_index < array_count(doc->front_end.sema.node_local_indices)) {
        return doc->front_end.sema.node_local_indices[ref_node_index];
    }

    return sema_no_local();
}

//------------------------------------------------------------------------------
// Evaluate a top-level constant declaration to a signed integer when possible.

internal bool
lsp_eval_decl_value(const LspDocument* doc, u32 decl_index, i64* out_value);

//------------------------------------------------------------------------------
// Evaluate one AST expression subtree to a signed integer when possible.

internal bool
lsp_eval_ast_node(const LspDocument* doc, u32 node_index, i64* out_value)
{
    const AstNode* node = &doc->front_end.ast.nodes[node_index];

    switch (node->kind) {
    case AK_IntegerLiteral:
        *out_value = (i64)ast_get_integer(&doc->front_end.lexer, node);
        return true;

    case AK_SymbolRef:
        {
            u32 decl_index = doc->front_end.sema.node_decl_indices[node_index];
            if (decl_index == LSP_NO_DECL) {
                return false;
            }
            return lsp_eval_decl_value(doc, decl_index, out_value);
        }

    case AK_IntegerNegate:
        {
            i64 value = 0;
            if (!lsp_eval_ast_node(doc, node->a, &value)) {
                return false;
            }
            *out_value = -value;
            return true;
        }

    case AK_IntegerPlus:
    case AK_IntegerMinus:
    case AK_IntegerMultiply:
    case AK_IntegerDivide:
    case AK_IntegerModulo:
        {
            i64 left  = 0;
            i64 right = 0;
            if (!lsp_eval_ast_node(doc, node->a, &left) ||
                !lsp_eval_ast_node(doc, node->b, &right)) {
                return false;
            }

            switch (node->kind) {
            case AK_IntegerPlus:
                *out_value = left + right;
                return true;
            case AK_IntegerMinus:
                *out_value = left - right;
                return true;
            case AK_IntegerMultiply:
                *out_value = left * right;
                return true;
            case AK_IntegerDivide:
                if (right == 0) {
                    return false;
                }
                *out_value = left / right;
                return true;
            case AK_IntegerModulo:
                if (right == 0) {
                    return false;
                }
                *out_value = left % right;
                return true;
            default:
                return false;
            }
        }

    case AK_Expression:
    case AK_Statement:
        return lsp_eval_ast_node(doc, node->a, out_value);

    default:
        return false;
    }
}

//------------------------------------------------------------------------------
// Evaluate one top-level declaration when it represents a constant.

internal bool
lsp_eval_decl_value(const LspDocument* doc, u32 decl_index, i64* out_value)
{
    const SemaDecl* decl = &doc->front_end.sema.decls[decl_index];
    if (decl->kind != SK_Constant) {
        return false;
    }

    return lsp_eval_ast_node(doc, decl->value_node_index, out_value);
}

//------------------------------------------------------------------------------
// Return the current signature text for one function declaration.

internal string lsp_decl_signature(const LspDocument* doc,
                                   Arena*             arena,
                                   const SemaDecl*    decl)
{
    if (decl->kind != SK_Function && decl->kind != SK_BuiltinFunction) {
        return s("<unknown>");
    }
    if (decl->kind == SK_BuiltinFunction) {
        string name = lex_symbol(&doc->front_end.lexer, decl->symbol_handle);
        if (string_eq(name, s("pr")) || string_eq(name, s("prn"))) {
            string rendered =
                sema_type_name(&doc->front_end.sema, arena, decl->type_index);
            return string_eq(rendered, s("<unknown>"))
                       ? s("fn (string) -> void")
                       : rendered;
        }
    }
    string name = lex_symbol(&doc->front_end.lexer, decl->symbol_handle);
    if (string_eq(name, s("main")) &&
        decl->value_node_index != sema_no_decl()) {
        const AstNode* fn_def =
            &doc->front_end.ast.nodes[decl->value_node_index];
        if (fn_def->kind == AK_FnDef && fn_def->b == AFK_Block) {
            const AstNode* fn_start = &doc->front_end.ast.nodes[fn_def->a];
            const AstFnSignature* signature =
                &doc->front_end.ast.fn_signatures[fn_start->a];
            bool has_explicit_return_type =
                signature->return_type_node_index != U32_MAX;
            bool has_return = false;
            for (u32 i = fn_def->a + 1; i < fn_start->b; ++i) {
                const AstNode* node = &doc->front_end.ast.nodes[i];
                if (node->kind == AK_Return) {
                    has_return = true;
                    break;
                }
                if (node->kind == AK_FnStart || node->kind == AK_Block) {
                    i = node->b - 1;
                }
            }
            if (!has_explicit_return_type && !has_return) {
                return s("fn () -> void");
            }
        }
    }
    return sema_type_name(&doc->front_end.sema, arena, decl->type_index);
}

//------------------------------------------------------------------------------
// Infer the current hover-facing type for one AST node.

internal string lsp_infer_ast_type(const LspDocument* doc,
                                   Arena*             arena,
                                   u32                node_index)
{
    const AstNode* node = &doc->front_end.ast.nodes[node_index];
    UNUSED(node);

    if (node_index >= array_count(doc->front_end.sema.node_type_indices)) {
        return s("<unknown>");
    }

    return sema_type_name(&doc->front_end.sema,
                          arena,
                          doc->front_end.sema.node_type_indices[node_index]);
}

//------------------------------------------------------------------------------
// Return one markdown code block with the Nerd language tag.

internal string lsp_markdown_code_block(Arena* arena, string code)
{
    return string_format(arena, "```nerd\n" STRINGP "\n```", STRINGV(code));
}

//------------------------------------------------------------------------------
// Attach a markdown hover result to the response.

internal void
lsp_set_markdown_hover(JsonValue* response, Arena* arena, string value)
{
    JsonValue* result   = json_new_object(arena);
    JsonValue* contents = json_new_object(arena);
    json_object_set_string(contents, arena, "kind", s("markdown"));
    json_object_set_string(contents, arena, "value", value);
    json_object_set_object(result, "contents", contents);
    json_object_set_object(response, "result", result);
}

//------------------------------------------------------------------------------
// Return a small hover summary for one semantic declaration.

internal string lsp_decl_hover_text(const LspDocument* doc,
                                    Arena*             arena,
                                    u32                decl_index)
{
    const SemaDecl* decl = &doc->front_end.sema.decls[decl_index];
    string name = lex_symbol(&doc->front_end.lexer, decl->symbol_handle);
    string kind = s("value");
    string inferred_type = s("<unknown>");
    if (decl->kind == SK_TypeAlias) {
        kind = s("type alias");
        inferred_type =
            sema_type_name(&doc->front_end.sema, arena, decl->type_index);
    } else if (decl->kind == SK_Constant) {
        kind          = s("constant");
        inferred_type = lsp_infer_ast_type(doc, arena, decl->value_node_index);
    } else if (decl->kind == SK_Variable) {
        kind = s("variable");
        inferred_type =
            sema_type_name(&doc->front_end.sema, arena, decl->type_index);
    } else {
        kind          = s("function");
        inferred_type = lsp_decl_signature(doc, arena, decl);
    }

    if (decl->kind == SK_Function || decl->kind == SK_BuiltinFunction) {
        return string_format(arena,
                             STRINGP "\n\n- Kind: " STRINGP,
                             STRINGV(lsp_markdown_code_block(
                                 arena,
                                 string_format(arena,
                                               STRINGP " :: " STRINGP,
                                               STRINGV(name),
                                               STRINGV(inferred_type)))),
                             STRINGV(kind),
                             STRINGV(inferred_type));
    }

    if (decl->kind == SK_TypeAlias) {
        return string_format(arena,
                             STRINGP "\n\n- Kind: " STRINGP
                                     "\n- Type: `" STRINGP "`",
                             STRINGV(lsp_markdown_code_block(
                                 arena,
                                 string_format(arena,
                                               STRINGP " :: " STRINGP,
                                               STRINGV(name),
                                               STRINGV(inferred_type)))),
                             STRINGV(kind),
                             STRINGV(inferred_type));
    }

    i64 value = 0;
    if (decl->kind == SK_Constant &&
        lsp_eval_decl_value(doc, decl_index, &value)) {
        return string_format(
            arena,
            STRINGP "\n\n- Kind: " STRINGP "\n- Type: `" STRINGP "`"
                    "\n- Value: `%lld`",
            STRINGV(lsp_markdown_code_block(
                arena,
                string_format(
                    arena, STRINGP " :: %lld", STRINGV(name), value))),
            STRINGV(kind),
            STRINGV(inferred_type),
            value);
    }

    return string_format(
        arena,
        STRINGP "\n\n- Kind: " STRINGP "\n- Type: `" STRINGP "`",
        STRINGV(lsp_markdown_code_block(
            arena, string_format(arena, STRINGP, STRINGV(name)))),
        STRINGV(kind),
        STRINGV(inferred_type));
}

internal string lsp_local_hover_text(const LspDocument* doc,
                                     Arena*             arena,
                                     u32                local_index)
{
    const SemaLocal* local = &doc->front_end.sema.locals[local_index];
    string name = lex_symbol(&doc->front_end.lexer, local->symbol_handle);
    string type =
        sema_type_name(&doc->front_end.sema, arena, local->type_index);

    return string_format(
        arena,
        STRINGP "\n\n- Kind: local variable\n- Type: `" STRINGP "`",
        STRINGV(lsp_markdown_code_block(
            arena, string_format(arena, STRINGP, STRINGV(name)))),
        STRINGV(type));
}

//------------------------------------------------------------------------------
// Build a location object for one top-level declaration binding.

internal JsonValue* lsp_decl_location(const LspDocument* doc,
                                      Arena*             arena,
                                      string             uri,
                                      u32                decl_index)
{
    const SemaDecl* decl = &doc->front_end.sema.decls[decl_index];
    if (decl->bind_node_index == LSP_NO_DECL) {
        return NULL;
    }
    const AstNode* bind = &doc->front_end.ast.nodes[decl->bind_node_index];
    usize          start_offset;
    usize          end_offset;
    lsp_token_offsets(
        &doc->front_end.lexer, bind->token_index, &start_offset, &end_offset);

    JsonValue* location = json_new_object(arena);
    json_object_set_string(location, arena, "uri", uri);
    json_object_set_object(
        location,
        "range",
        lsp_make_range(
            arena, doc->front_end.lexer.source, start_offset, end_offset));
    return location;
}

internal JsonValue* lsp_local_location(const LspDocument* doc,
                                       Arena*             arena,
                                       string             uri,
                                       u32                local_index)
{
    const SemaLocal* local = &doc->front_end.sema.locals[local_index];
    const AstNode*   bind  = &doc->front_end.ast.nodes[local->decl_node_index];
    usize            start_offset;
    usize            end_offset;
    lsp_token_offsets(
        &doc->front_end.lexer, bind->token_index, &start_offset, &end_offset);

    JsonValue* location = json_new_object(arena);
    json_object_set_string(location, arena, "uri", uri);
    json_object_set_object(
        location,
        "range",
        lsp_make_range(
            arena, doc->front_end.lexer.source, start_offset, end_offset));
    return location;
}

//------------------------------------------------------------------------------
// Build a request context for one position-based LSP query.

internal bool lsp_get_request_context(LspState*         state,
                                      const LspMessage* message,
                                      LspDocument**     out_doc,
                                      string*           out_uri,
                                      usize*            out_offset,
                                      u32*              out_token_index,
                                      const Token**     out_token)
{
    JsonValue* uri_value =
        json_get_cstr(message->message, "params.textDocument.uri");
    JsonValue* line_value =
        json_get_cstr(message->message, "params.position.line");
    JsonValue* col_value =
        json_get_cstr(message->message, "params.position.character");

    if (!uri_value || !line_value || !col_value ||
        uri_value->kind != JSON_STRING || line_value->kind != JSON_NUMBER ||
        col_value->kind != JSON_NUMBER) {
        return false;
    }

    *out_uri = json_string(uri_value);
    *out_doc = LspDocumentMap_find(&state->documents, *out_uri);
    if (!*out_doc) {
        return false;
    }
    if (!(*out_doc)->analysis_ok) {
        return false;
    }

    if (!lex_line_col_to_offset((*out_doc)->front_end.lexer.source,
                                (u32)json_integer(line_value),
                                (u32)json_integer(col_value),
                                out_offset)) {
        return false;
    }

    u32    token_end = 0;
    Token* token =
        lex_find(&(*out_doc)->front_end.lexer, *out_offset, &token_end);
    if (!token) {
        return false;
    }

    *out_token_index =
        lsp_token_index_from_pointer(&(*out_doc)->front_end.lexer, token);
    *out_token = token;
    return true;
}

//------------------------------------------------------------------------------
// Return the LSP symbol kind for one semantic declaration.

internal int lsp_decl_symbol_kind(const SemaDecl* decl)
{
    return decl->kind == SK_Function ? LSP_SYMBOL_KIND_FUNCTION
                                     : LSP_SYMBOL_KIND_CONSTANT;
}

//------------------------------------------------------------------------------
// Respond to hover requests with semantic information about the token under the
// cursor.

void lsp_handle_hover(LspState* state, const LspMessage* message)
{
    JsonValue*   response    = lsp_prepare_response(message);
    LspDocument* doc         = NULL;
    string       uri         = {0};
    usize        offset      = 0;
    u32          token_index = 0;
    const Token* token       = NULL;

    if (!lsp_get_request_context(
            state, message, &doc, &uri, &offset, &token_index, &token)) {
        lsp_cancel(response, message->arena);
        return;
    }

    switch (token->kind) {
    case TK_Integer:
        {
            usize token_end =
                lex_token_end_offset(&doc->front_end.lexer, token);
            string raw_text = string_from(
                doc->front_end.lexer.source.source.data + token->offset,
                token_end - token->offset);
            lsp_set_markdown_hover(
                response,
                message->arena,
                string_format(
                    message->arena,
                    STRINGP "\n\n- Type: `" STRINGP "`",
                    STRINGV(lsp_markdown_code_block(message->arena, raw_text)),
                    STRINGV(s("untyped integer"))));
        }
        break;
    case TK_String:
        {
            usize token_end =
                lex_token_end_offset(&doc->front_end.lexer, token);
            string raw_text = string_from(
                doc->front_end.lexer.source.source.data + token->offset,
                token_end - token->offset);
            lsp_set_markdown_hover(
                response,
                message->arena,
                string_format(message->arena,
                              STRINGP "\n\n- Type: `string`",
                              STRINGV(lsp_markdown_code_block(message->arena,
                                                              raw_text))));
        }
        break;

    case TK_Symbol:
        {
            u32 local_index = lsp_find_local_index_for_token(doc, token_index);
            if (local_index != sema_no_local()) {
                lsp_set_markdown_hover(
                    response,
                    message->arena,
                    lsp_local_hover_text(doc, message->arena, local_index));
                break;
            }

            u32 decl_index = lsp_find_decl_index_for_token(doc, token_index);
            if (decl_index == LSP_NO_DECL) {
                lsp_cancel(response, message->arena);
                return;
            }

            lsp_set_markdown_hover(
                response,
                message->arena,
                lsp_decl_hover_text(doc, message->arena, decl_index));
        }
        break;

    case TK_fn:
        {
            string signature  = s("fn ()");
            u32    decl_index = lsp_find_decl_index_for_token(doc, token_index);
            if (decl_index != LSP_NO_DECL) {
                signature =
                    lsp_decl_signature(doc,
                                       message->arena,
                                       &doc->front_end.sema.decls[decl_index]);
            }
            lsp_set_markdown_hover(
                response,
                message->arena,
                string_format(
                    message->arena,
                    STRINGP "\n\n- Kind: function expression"
                            "\n- Signature: `" STRINGP "`",
                    STRINGV(lsp_markdown_code_block(message->arena, signature)),
                    STRINGV(signature)));
        }
        break;

    default:
        lsp_cancel(response, message->arena);
        return;
    }

    UNUSED(uri);
    UNUSED(offset);
    lsp_send_response(message->arena, response);
}

//------------------------------------------------------------------------------
// Respond to definition requests with the binding location of the resolved
// declaration.

void lsp_handle_definition(LspState* state, const LspMessage* message)
{
    JsonValue*   response    = lsp_prepare_response(message);
    LspDocument* doc         = NULL;
    string       uri         = {0};
    usize        offset      = 0;
    u32          token_index = 0;
    const Token* token       = NULL;

    if (!lsp_get_request_context(
            state, message, &doc, &uri, &offset, &token_index, &token)) {
        lsp_cancel(response, message->arena);
        return;
    }

    if (token->kind != TK_Symbol) {
        lsp_cancel(response, message->arena);
        return;
    }

    u32 local_index = lsp_find_local_index_for_token(doc, token_index);
    if (local_index != sema_no_local()) {
        JsonValue* location =
            lsp_local_location(doc, message->arena, uri, local_index);
        json_object_set_object(response, "result", location);
        lsp_send_response(message->arena, response);
        return;
    }

    u32 decl_index = lsp_find_decl_index_for_token(doc, token_index);
    if (decl_index == LSP_NO_DECL) {
        lsp_cancel(response, message->arena);
        return;
    }

    JsonValue* location =
        lsp_decl_location(doc, message->arena, uri, decl_index);
    if (!location) {
        lsp_cancel(response, message->arena);
        return;
    }

    json_object_set_object(response, "result", location);
    lsp_send_response(message->arena, response);
}

//------------------------------------------------------------------------------
// Respond to document symbol requests with the top-level declarations in source
// order.

void lsp_handle_document_symbol(LspState* state, const LspMessage* message)
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
        json_object_set_array(
            response, "result", json_new_array(message->arena));
        lsp_send_response(message->arena, response);
        return;
    }

    JsonValue* result = json_new_array(message->arena);

    if (doc->has_cst) {
        for (u32 i = 0; i < array_count(doc->cst.bindings); ++i) {
            const CstNode* bind       = &doc->cst.nodes[doc->cst.bindings[i]];
            u32            decl_index = lsp_find_decl_index_by_symbol_handle(
                &doc->front_end.sema, bind->a);
            if (decl_index == LSP_NO_DECL) {
                continue;
            }

            usize start_offset;
            usize end_offset;
            lsp_token_offsets(&doc->front_end.lexer,
                              bind->token_index,
                              &start_offset,
                              &end_offset);

            const SemaDecl* decl   = &doc->front_end.sema.decls[decl_index];
            JsonValue*      symbol = json_new_object(message->arena);
            json_object_set_string(symbol,
                                   message->arena,
                                   "name",
                                   lex_symbol(&doc->front_end.lexer, bind->a));
            json_object_set_number(
                symbol, message->arena, "kind", lsp_decl_symbol_kind(decl));
            json_object_set_object(symbol,
                                   "range",
                                   lsp_make_range(message->arena,
                                                  doc->front_end.lexer.source,
                                                  start_offset,
                                                  end_offset));
            json_object_set_object(symbol,
                                   "selectionRange",
                                   lsp_make_range(message->arena,
                                                  doc->front_end.lexer.source,
                                                  start_offset,
                                                  end_offset));

            if (decl->kind == SK_TypeAlias) {
                json_object_set_string(
                    symbol, message->arena, "detail", s("type alias"));
            } else if (decl->kind == SK_Constant) {
                i64 value = 0;
                if (lsp_eval_decl_value(doc, decl_index, &value)) {
                    json_object_set_string(symbol,
                                           message->arena,
                                           "detail",
                                           string_format(message->arena,
                                                         "constant = %lld",
                                                         value));
                } else {
                    json_object_set_string(
                        symbol, message->arena, "detail", s("constant"));
                }
            } else {
                json_object_set_string(
                    symbol, message->arena, "detail", s("function"));
            }

            json_array_push(result, symbol);
        }
    }

    json_object_set_array(response, "result", result);
    lsp_send_response(message->arena, response);
}

//------------------------------------------------------------------------------
