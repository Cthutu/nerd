//------------------------------------------------------------------------------
// LSP signature-help support
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/sema/sema.h>
#include <lsp/lsp.h>

//------------------------------------------------------------------------------

internal bool lsp_signature_is_ident_char(u8 c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
           (c >= '0' && c <= '9') || c == '_';
}

internal string lsp_signature_trim_source(const LspDocument* doc,
                                          usize              start,
                                          usize              end)
{
    const string source = doc->front_end.lexer.source.source;
    while (start < end &&
           (source.data[start] == ' ' || source.data[start] == '\t' ||
            source.data[start] == '\n' || source.data[start] == '\r')) {
        start++;
    }
    while (end > start &&
           (source.data[end - 1] == ' ' || source.data[end - 1] == '\t' ||
            source.data[end - 1] == '\n' || source.data[end - 1] == '\r')) {
        end--;
    }
    return (string){.data = source.data + start, .count = end - start};
}

internal string lsp_signature_default_param_source(const LspDocument* doc,
                                                   const AstParam*    param)
{
    if (param->default_node_index == U32_MAX) {
        return s("...");
    }

    const Lexer* lexer = &doc->front_end.lexer;
    if (param->token_index >= array_count(lexer->tokens)) {
        return s("...");
    }

    u32 equal_token = U32_MAX;
    u32 depth       = 0;
    for (u32 i = param->token_index; i < array_count(lexer->tokens); ++i) {
        TokenKind kind = lexer->tokens[i].kind;
        if (depth == 0 && kind == TK_Equal) {
            equal_token = i;
            break;
        }
        if (kind == TK_LParen || kind == TK_LBracket || kind == TK_LBrace) {
            depth++;
        } else if (kind == TK_RParen || kind == TK_RBracket ||
                   kind == TK_RBrace) {
            if (depth == 0) {
                break;
            }
            depth--;
        } else if (depth == 0 && kind == TK_Comma) {
            break;
        }
    }
    if (equal_token == U32_MAX) {
        return s("...");
    }

    depth       = 0;
    usize start = lex_token_end_offset(lexer, &lexer->tokens[equal_token]);
    usize end   = start;
    for (u32 i = equal_token + 1; i < array_count(lexer->tokens); ++i) {
        TokenKind kind = lexer->tokens[i].kind;
        if (depth == 0 && (kind == TK_Comma || kind == TK_RParen)) {
            end = lexer->tokens[i].offset;
            break;
        }
        if (kind == TK_LParen || kind == TK_LBracket || kind == TK_LBrace) {
            depth++;
        } else if (kind == TK_RParen || kind == TK_RBracket ||
                   kind == TK_RBrace) {
            if (depth == 0) {
                end = lexer->tokens[i].offset;
                break;
            }
            depth--;
        }
        end = lex_token_end_offset(lexer, &lexer->tokens[i]);
    }

    return lsp_signature_trim_source(doc, start, end);
}

internal string lsp_signature_type_source(const LspDocument* doc,
                                          const AstParam*    param)
{
    const Lexer* lexer = &doc->front_end.lexer;
    u32          colon = U32_MAX;
    for (u32 i = param->token_index; i < array_count(lexer->tokens); ++i) {
        TokenKind kind = lexer->tokens[i].kind;
        if (kind == TK_Colon) {
            colon = i;
            break;
        }
        if (kind == TK_Comma || kind == TK_RParen) {
            break;
        }
    }
    if (colon == U32_MAX) {
        return s("<unknown>");
    }

    usize start = lex_token_end_offset(lexer, &lexer->tokens[colon]);
    usize end   = start;
    u32   depth = 0;
    for (u32 i = colon + 1; i < array_count(lexer->tokens); ++i) {
        TokenKind kind = lexer->tokens[i].kind;
        if (depth == 0 &&
            (kind == TK_Comma || kind == TK_RParen || kind == TK_Equal)) {
            end = lexer->tokens[i].offset;
            break;
        }
        if (kind == TK_LParen || kind == TK_LBracket) {
            depth++;
        } else if (kind == TK_RParen || kind == TK_RBracket) {
            if (depth == 0) {
                end = lexer->tokens[i].offset;
                break;
            }
            depth--;
        }
        end = lex_token_end_offset(lexer, &lexer->tokens[i]);
    }
    return lsp_signature_trim_source(doc, start, end);
}

internal string lsp_signature_return_type_source(const LspDocument* doc,
                                                 u32 return_type_node_index)
{
    if (return_type_node_index == U32_MAX) {
        return s("");
    }

    const Lexer*   lexer = &doc->front_end.lexer;
    const AstNode* node  = &doc->front_end.ast.nodes[return_type_node_index];
    usize          start = lexer->tokens[node->token_index].offset;
    usize          end   = start;
    u32            depth = 0;
    for (u32 i = node->token_index; i < array_count(lexer->tokens); ++i) {
        TokenKind kind = lexer->tokens[i].kind;
        if (depth == 0 && (kind == TK_LBrace || kind == TK_FatArrow)) {
            end = lexer->tokens[i].offset;
            break;
        }
        if (kind == TK_LParen || kind == TK_LBracket) {
            depth++;
        } else if (kind == TK_RParen || kind == TK_RBracket) {
            if (depth == 0) {
                end = lexer->tokens[i].offset;
                break;
            }
            depth--;
        }
        end = lex_token_end_offset(lexer, &lexer->tokens[i]);
    }
    return lsp_signature_trim_source(doc, start, end);
}

internal const AstFnSignature*
lsp_signature_decl_ast_signature(const LspDocument* doc, const SemaDecl* decl)
{
    if (decl->value_node_index == sema_no_decl()) {
        return NULL;
    }

    const Ast*     ast        = &doc->front_end.ast;
    const AstNode* value_node = &ast->nodes[decl->value_node_index];
    if (value_node->kind == AK_FnDef) {
        const AstNode* fn_start = &ast->nodes[value_node->a];
        return &ast->fn_signatures[fn_start->a];
    }
    if (value_node->kind == AK_FfiDef) {
        const AstFfiInfo* ffi = &ast->ffi_infos[value_node->a];
        return &ast->fn_signatures[ffi->signature_index];
    }
    return NULL;
}

internal bool lsp_signature_decl_label(const LspDocument* doc,
                                       Arena*             arena,
                                       const SemaDecl*    decl,
                                       string*            out_label,
                                       JsonValue**        out_params)
{
    if (decl->kind != SK_Function && decl->kind != SK_GenericFunction &&
        decl->kind != SK_FfiFunction && decl->kind != SK_BuiltinFunction) {
        return false;
    }

    const AstFnSignature* signature =
        lsp_signature_decl_ast_signature(doc, decl);
    if (signature == NULL) {
        string name = lex_symbol(&doc->front_end.lexer, decl->symbol_handle);
        string type = sema_type_name(&doc->front_end.lexer,
                                     &doc->front_end.sema,
                                     arena,
                                     decl->type_index);
        *out_label  = string_format(
            arena, STRINGP ": " STRINGP, STRINGV(name), STRINGV(type));
        *out_params = json_new_array(arena);
        return true;
    }

    const Ast*      ast         = &doc->front_end.ast;
    const SemaType* type        = NULL;
    bool            has_generic = signature->generic_params_index != U32_MAX;
    if (!has_generic) {
        if (!lsp_sema_type(&doc->front_end.sema, decl->type_index, &type)) {
            return false;
        }
        if (type->kind != STK_Function) {
            return false;
        }
    }

    Arena build_arena = {0};
    Arena text_arena  = {0};
    arena_init(&build_arena);
    arena_init(&text_arena);

    JsonValue*    params = json_new_array(arena);
    StringBuilder sb     = {0};
    sb_init(&sb, &build_arena);
    sb_append_string(&sb,
                     lex_symbol(&doc->front_end.lexer, decl->symbol_handle));
    if (has_generic) {
        const AstGenericParams* generic =
            &ast->generic_params[signature->generic_params_index];
        sb_append_cstr(&sb, "[");
        for (u32 i = 0; i < generic->symbol_count; ++i) {
            if (i > 0) {
                sb_append_cstr(&sb, ", ");
            }
            sb_append_string(
                &sb,
                lex_symbol(
                    &doc->front_end.lexer,
                    ast->generic_param_symbols[generic->first_symbol + i]));
        }
        sb_append_cstr(&sb, "]");
    }
    sb_append_cstr(&sb, "(");
    for (u32 i = 0; i < signature->param_count; ++i) {
        if (i > 0) {
            sb_append_cstr(&sb, ", ");
        }

        Arena param_arena = {0};
        arena_init(&param_arena);

        const AstParam* param       = &ast->params[signature->first_param + i];
        StringBuilder   param_label = {0};
        sb_init(&param_label, &param_arena);
        if (param->symbol_handle != U32_MAX) {
            string param_name =
                lex_symbol(&doc->front_end.lexer, param->symbol_handle);
            sb_append_string(&param_label, param_name);
            sb_append_cstr(&param_label, ": ");
        }
        if (has_generic) {
            sb_append_string(&param_label,
                             lsp_signature_type_source(doc, param));
        } else {
            u32 param_type =
                i < type->param_count
                    ? doc->front_end.sema
                          .type_param_types[type->first_param_type + i]
                    : sema_no_type();
            sb_append_string(&param_label,
                             sema_type_name(&doc->front_end.lexer,
                                            &doc->front_end.sema,
                                            &text_arena,
                                            param_type));
        }
        if (param->default_node_index != U32_MAX) {
            sb_append_cstr(&param_label, " = ");
            sb_append_string(&param_label,
                             lsp_signature_default_param_source(doc, param));
        }

        string param_text = sb_to_string(&param_label);
        sb_append_string(&sb, param_text);

        JsonValue* param_item = json_new_object(arena);
        json_object_set_string(param_item, arena, "label", param_text);
        json_array_push(params, param_item);
        arena_done(&param_arena);
    }
    if (signature->is_varargs) {
        if (signature->param_count > 0) {
            sb_append_cstr(&sb, ", ");
        }
        sb_append_cstr(&sb, "...");
    }
    sb_append_cstr(&sb, ")");
    if (has_generic && signature->return_type_node_index != U32_MAX) {
        sb_append_cstr(&sb, " -> ");
        sb_append_string(&sb,
                         lsp_signature_return_type_source(
                             doc, signature->return_type_node_index));
    } else if (!has_generic && type->return_type != sema_no_type()) {
        sb_append_cstr(&sb, " -> ");
        sb_append_string(&sb,
                         sema_type_name(&doc->front_end.lexer,
                                        &doc->front_end.sema,
                                        &text_arena,
                                        type->return_type));
    }

    *out_label  = string_format(arena, STRINGP, STRINGV(sb_to_string(&sb)));
    *out_params = params;
    arena_done(&text_arena);
    arena_done(&build_arena);
    return true;
}

internal const SemaDecl* lsp_signature_find_decl(const LspDocument* doc,
                                                 string             name)
{
    if (!doc->sema_partial) {
        return NULL;
    }

    const Lexer* lexer = &doc->front_end.lexer;
    const Sema*  sema  = &doc->front_end.sema;
    for (u32 i = 0; i < array_count(sema->decls); ++i) {
        const SemaDecl* decl = &sema->decls[i];
        if (decl->symbol_handle != U32_MAX &&
            string_eq(lex_symbol(lexer, decl->symbol_handle), name) &&
            (decl->kind == SK_Function || decl->kind == SK_GenericFunction ||
             decl->kind == SK_FfiFunction ||
             decl->kind == SK_BuiltinFunction)) {
            return decl;
        }
    }

    for (u32 i = 0; i < array_count(sema->methods); ++i) {
        const SemaMethod* method = &sema->methods[i];
        if (method->symbol_handle != U32_MAX &&
            string_eq(lex_symbol(lexer, method->symbol_handle), name) &&
            method->decl_index < array_count(sema->decls)) {
            return &sema->decls[method->decl_index];
        }
    }

    return NULL;
}

internal bool lsp_signature_call_context(string  source,
                                         usize   offset,
                                         string* out_name,
                                         u32*    out_active_param)
{
    u32   depth       = 0;
    usize open_offset = U32_MAX;
    for (usize i = offset; i > 0; --i) {
        u8 c = source.data[i - 1];
        if (c == ')') {
            depth++;
        } else if (c == '(') {
            if (depth == 0) {
                open_offset = i - 1;
                break;
            }
            depth--;
        }
    }
    if (open_offset == U32_MAX) {
        return false;
    }

    u32 active = 0;
    depth      = 0;
    for (usize i = open_offset + 1; i < offset; ++i) {
        u8 c = source.data[i];
        if (c == '(' || c == '[' || c == '{') {
            depth++;
        } else if (c == ')' || c == ']' || c == '}') {
            if (depth > 0) {
                depth--;
            }
        } else if (c == ',' && depth == 0) {
            active++;
        }
    }

    usize end = open_offset;
    while (end > 0 &&
           (source.data[end - 1] == ' ' || source.data[end - 1] == '\t' ||
            source.data[end - 1] == '\n' || source.data[end - 1] == '\r')) {
        end--;
    }
    usize start = end;
    while (start > 0 && lsp_signature_is_ident_char(source.data[start - 1])) {
        start--;
    }
    if (start == end) {
        return false;
    }

    *out_name = (string){.data = source.data + start, .count = end - start};
    *out_active_param = active;
    return true;
}

void lsp_handle_signature_help(LspState* state, const LspMessage* message)
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

    u64 line      = 0;
    u64 character = 0;
    (void)lsp_get_u64_param(message, "params.position.line", &line);
    (void)lsp_get_u64_param(message, "params.position.character", &character);
    usize offset = lsp_offset_from_position(doc->source, line, character);

    string name  = {0};
    u32    active_param = 0;
    if (!lsp_signature_call_context(
            doc->source, offset, &name, &active_param)) {
        json_object_set_null(response, message->arena, "result");
        lsp_send_response(message->arena, response);
        return;
    }

    const SemaDecl* decl = lsp_signature_find_decl(doc, name);
    if (decl == NULL) {
        json_object_set_null(response, message->arena, "result");
        lsp_send_response(message->arena, response);
        return;
    }

    string     label      = {0};
    JsonValue* parameters = NULL;
    if (!lsp_signature_decl_label(
            doc, message->arena, decl, &label, &parameters)) {
        json_object_set_null(response, message->arena, "result");
        lsp_send_response(message->arena, response);
        return;
    }

    JsonValue* signature = json_new_object(message->arena);
    json_object_set_string(signature, message->arena, "label", label);
    json_object_set_string(
        signature,
        message->arena,
        "documentation",
        s("Named arguments use `name = value`; omitted "
          "parameters use declared defaults when available."));
    json_object_set_array(signature, "parameters", parameters);

    JsonValue* signatures = json_new_array(message->arena);
    json_array_push(signatures, signature);

    JsonValue* result = json_new_object(message->arena);
    json_object_set_array(result, "signatures", signatures);
    json_object_set_number(result, message->arena, "activeSignature", 0);
    json_object_set_number(
        result, message->arena, "activeParameter", active_param);
    json_object_set_object(response, "result", result);
    lsp_send_response(message->arena, response);
}

//------------------------------------------------------------------------------
