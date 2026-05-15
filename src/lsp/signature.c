//------------------------------------------------------------------------------
// LSP signature-help support
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/build/front/front.h>
#include <compiler/error/error.h>
#include <compiler/modules/modules.h>
#include <compiler/sema/sema.h>
#include <lsp/lsp.h>

//------------------------------------------------------------------------------

internal bool lsp_signature_is_ident_char(u8 c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
           (c >= '0' && c <= '9') || c == '_';
}

internal string lsp_signature_trim_source(const LspTypeFactView* view,
                                          usize                  start,
                                          usize                  end)
{
    const string source = view->source;
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

internal string lsp_signature_default_param_source(const LspTypeFactView* view,
                                                   const AstParam*        param)
{
    if (param->default_node_index == U32_MAX) {
        return s("...");
    }

    const Lexer* lexer = view->lexer;
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

    return lsp_signature_trim_source(view, start, end);
}

internal string lsp_signature_type_source(const LspTypeFactView* view,
                                          const AstParam*        param)
{
    const Lexer* lexer = view->lexer;
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
    return lsp_signature_trim_source(view, start, end);
}

internal string lsp_signature_return_type_source(const LspTypeFactView* view,
                                                 u32 return_type_node_index)
{
    if (return_type_node_index == U32_MAX ||
        return_type_node_index >= array_count(view->ast->nodes)) {
        return s("");
    }

    const Lexer*   lexer = view->lexer;
    const AstNode* node  = &view->ast->nodes[return_type_node_index];
    if (node->token_index >= array_count(lexer->tokens)) {
        return s("");
    }
    usize start = lexer->tokens[node->token_index].offset;
    usize end   = start;
    u32   depth = 0;
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
    return lsp_signature_trim_source(view, start, end);
}

internal const AstFnSignature*
lsp_signature_decl_ast_signature(const LspTypeFactView* view,
                                 const SemaDecl*        decl)
{
    if (decl->value_node_index == sema_no_decl() ||
        decl->value_node_index >= array_count(view->ast->nodes)) {
        return NULL;
    }

    const Ast*     ast        = view->ast;
    const AstNode* value_node = &ast->nodes[decl->value_node_index];
    if (value_node->kind == AK_FnDef) {
        if (value_node->a >= array_count(ast->nodes)) {
            return NULL;
        }
        const AstNode* fn_start = &ast->nodes[value_node->a];
        if (fn_start->a >= array_count(ast->fn_signatures)) {
            return NULL;
        }
        return &ast->fn_signatures[fn_start->a];
    }
    if (value_node->kind == AK_FfiDef) {
        if (value_node->a >= array_count(ast->ffi_infos)) {
            return NULL;
        }
        const AstFfiInfo* ffi = &ast->ffi_infos[value_node->a];
        if (ffi->signature_index >= array_count(ast->fn_signatures)) {
            return NULL;
        }
        return &ast->fn_signatures[ffi->signature_index];
    }
    return NULL;
}

internal bool lsp_signature_decl_is_callable(const LspTypeFactView* view,
                                             const SemaDecl*        decl)
{
    if (decl->kind == SK_Function || decl->kind == SK_GenericFunction ||
        decl->kind == SK_FfiFunction || decl->kind == SK_BuiltinFunction) {
        return true;
    }

    const SemaType* type = NULL;
    return lsp_sema_type(view->sema, decl->type_index, &type) &&
           type->kind == STK_Function;
}

internal const SemaDecl*
lsp_signature_resolve_callable_alias(const LspTypeFactView* view,
                                     const SemaDecl*        decl)
{
    const SemaDecl* current = decl;
    for (u32 depth = 0; depth < 8; ++depth) {
        if (current->value_node_index == sema_no_decl() ||
            current->value_node_index >= array_count(view->ast->nodes) ||
            view->ast->nodes[current->value_node_index].kind != AK_SymbolRef) {
            break;
        }

        u32 target_index = U32_MAX;
        if (!lsp_sema_node_decl(
                view->sema, current->value_node_index, &target_index)) {
            break;
        }

        const SemaDecl* target = NULL;
        if (!lsp_sema_decl(view->sema, target_index, &target) ||
            target == current ||
            !lsp_signature_decl_is_callable(view, target)) {
            break;
        }

        current = target;
    }

    return current;
}

internal bool lsp_signature_decl_label(const LspTypeFactView* view,
                                       Arena*                 arena,
                                       const SemaDecl*        decl,
                                       string*                out_label,
                                       JsonValue**            out_params)
{
    if (!lsp_signature_decl_is_callable(view, decl)) {
        return false;
    }

    if (decl->import_module_index != sema_no_decl() &&
        decl->import_decl_index != sema_no_decl()) {
        LspModuleView module = {0};
        if (lsp_program_module_view(
                &view->doc->program, decl->import_module_index, &module)) {
            const SemaDecl* imported_decl = NULL;
            if (lsp_sema_decl(
                    module.sema, decl->import_decl_index, &imported_decl)) {
                LspTypeFactView imported_view = {
                    .doc    = view->doc,
                    .source = module.lexer->source.source,
                    .lexer  = module.lexer,
                    .ast    = module.ast,
                    .sema   = module.sema,
                };
                return lsp_signature_decl_label(&imported_view,
                                                arena,
                                                imported_decl,
                                                out_label,
                                                out_params);
            }
        }
    }

    decl = lsp_signature_resolve_callable_alias(view, decl);

    const AstFnSignature* signature =
        lsp_signature_decl_ast_signature(view, decl);
    if (signature == NULL) {
        string name = lex_symbol(view->lexer, decl->symbol_handle);
        string type =
            sema_type_name(view->lexer, view->sema, arena, decl->type_index);
        *out_label = string_format(
            arena, STRINGP ": " STRINGP, STRINGV(name), STRINGV(type));
        *out_params = json_new_array(arena);
        return true;
    }

    const Ast*      ast         = view->ast;
    const SemaType* type        = NULL;
    bool            has_generic = signature->generic_params_index != U32_MAX;
    if (!has_generic && lsp_sema_type(view->sema, decl->type_index, &type) &&
        type->kind != STK_Function) {
        type = NULL;
    }

    Arena build_arena = {0};
    Arena text_arena  = {0};
    arena_init(&build_arena);
    arena_init(&text_arena);

    JsonValue*    params = json_new_array(arena);
    StringBuilder sb     = {0};
    sb_init(&sb, &build_arena);
    sb_append_string(&sb, lex_symbol(view->lexer, decl->symbol_handle));
    if (has_generic) {
        if (signature->generic_params_index >=
            array_count(ast->generic_params)) {
            arena_done(&text_arena);
            arena_done(&build_arena);
            return false;
        }
        const AstGenericParams* generic =
            &ast->generic_params[signature->generic_params_index];
        sb_append_cstr(&sb, "[");
        for (u32 i = 0; i < generic->symbol_count; ++i) {
            if (i > 0) {
                sb_append_cstr(&sb, ", ");
            }
            u32 symbol_index = generic->first_symbol + i;
            if (symbol_index >= array_count(ast->generic_param_symbols)) {
                arena_done(&text_arena);
                arena_done(&build_arena);
                return false;
            }
            sb_append_string(
                &sb,
                lex_symbol(view->lexer,
                           ast->generic_param_symbols[symbol_index]));
        }
        sb_append_cstr(&sb, "]");
    }
    sb_append_cstr(&sb, "(");
    if (signature->first_param + signature->param_count >
        array_count(ast->params)) {
        arena_done(&text_arena);
        arena_done(&build_arena);
        return false;
    }
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
            string param_name = lex_symbol(view->lexer, param->symbol_handle);
            sb_append_string(&param_label, param_name);
            sb_append_cstr(&param_label, ": ");
        }
        if (has_generic || type == NULL) {
            sb_append_string(&param_label,
                             lsp_signature_type_source(view, param));
        } else {
            u32 param_type       = sema_no_type();
            u32 param_type_index = type->first_param_type + i;
            if (i < type->param_count &&
                param_type_index < array_count(view->sema->type_param_types)) {
                param_type = view->sema->type_param_types[param_type_index];
            }
            sb_append_string(
                &param_label,
                sema_type_name(
                    view->lexer, view->sema, &text_arena, param_type));
        }
        if (param->default_node_index != U32_MAX) {
            sb_append_cstr(&param_label, " = ");
            sb_append_string(&param_label,
                             lsp_signature_default_param_source(view, param));
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
    if ((has_generic || type == NULL) &&
        signature->return_type_node_index != U32_MAX) {
        sb_append_cstr(&sb, " -> ");
        sb_append_string(&sb,
                         lsp_signature_return_type_source(
                             view, signature->return_type_node_index));
    } else if (type != NULL && type->return_type != sema_no_type()) {
        sb_append_cstr(&sb, " -> ");
        sb_append_string(
            &sb,
            sema_type_name(
                view->lexer, view->sema, &text_arena, type->return_type));
    }

    *out_label  = string_format(arena, STRINGP, STRINGV(sb_to_string(&sb)));
    *out_params = params;
    arena_done(&text_arena);
    arena_done(&build_arena);
    return true;
}

internal const SemaDecl* lsp_signature_find_decl(const LspTypeFactView* view,
                                                 string                 name)
{
    const Lexer* lexer = view->lexer;
    const Sema*  sema  = view->sema;
    for (u32 i = 0; i < array_count(sema->decls); ++i) {
        const SemaDecl* decl = NULL;
        if (!lsp_sema_decl(sema, i, &decl)) {
            continue;
        }
        if (decl->symbol_handle != U32_MAX &&
            string_eq(lex_symbol(lexer, decl->symbol_handle), name) &&
            lsp_signature_decl_is_callable(view, decl)) {
            return decl;
        }
    }

    for (u32 i = 0; i < array_count(sema->methods); ++i) {
        const SemaMethod* method = &sema->methods[i];
        const SemaDecl*   decl   = NULL;
        if (method->symbol_handle != U32_MAX &&
            string_eq(lex_symbol(lexer, method->symbol_handle), name) &&
            lsp_sema_decl(sema, method->decl_index, &decl)) {
            return decl;
        }
    }

    return NULL;
}

internal string lsp_signature_repair_source(Arena* arena,
                                            string source,
                                            usize  offset)
{
    StringBuilder sb = {0};
    sb_init(&sb, arena);
    sb_append_string(&sb, string_from(source.data, offset));

    usize cursor = offset;
    while (cursor > 0 &&
           (source.data[cursor - 1] == ' ' || source.data[cursor - 1] == '\t' ||
            source.data[cursor - 1] == '\n' ||
            source.data[cursor - 1] == '\r')) {
        cursor--;
    }
    if (cursor > 0 && source.data[cursor - 1] == ',') {
        sb_append_cstr(&sb, "__nerd_signature_probe");
    }
    sb_append_cstr(&sb, ")");

    if (offset < source.count) {
        sb_append_string(
            &sb, string_from(source.data + offset, source.count - offset));
    }
    return sb_to_string(&sb);
}

internal bool lsp_signature_repaired_type_fact_view(Arena*       arena,
                                                    string       uri,
                                                    string       source,
                                                    usize        offset,
                                                    ProgramInfo* out_program,
                                                    LspDocument* out_doc,
                                                    LspTypeFactView* out_view)
{
    string repaired = lsp_signature_repair_source(arena, source, offset);

    ErrorRenderMode previous_mode = error_system_mode();
    bool            previous_emit = error_system_should_emit_output();
    error_system_clear_last_rendered();
    error_system_set_mode(ERROR_RENDER_DIAGNOSTICS);
    error_system_set_emit_output(false);

    FrontEndOptions options = {
        .verbose              = false,
        .release              = false,
        .require_entry_point  = false,
        .skip_hir_generation  = true,
        .keep_partial_results = true,
    };
    bool ok =
        front_end_program((NerdSource){.source = repaired, .source_path = uri},
                          &options,
                          NULL,
                          out_program);
    error_system_set_mode(previous_mode);
    error_system_set_emit_output(previous_emit);

    if (array_count(out_program->modules) == 0 ||
        out_program->root_module_index >= array_count(out_program->modules)) {
        return false;
    }

    for (u32 i = 0; i < array_count(out_program->modules); ++i) {
        out_program->modules[i].front_end.sema.program = out_program;
    }

    ModuleInfo* root = &out_program->modules[out_program->root_module_index];
    UNUSED(ok);
    *out_doc = (LspDocument){
        .source       = repaired,
        .program      = *out_program,
        .front_end    = root->front_end,
        .source_ready = true,
    };
    *out_view = (LspTypeFactView){
        .doc    = out_doc,
        .source = repaired,
        .lexer  = &root->front_end.lexer,
        .ast    = &root->front_end.ast,
        .sema   = &root->front_end.sema,
    };
    return true;
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

internal bool
lsp_signature_match_ident_at(string source, usize* cursor, string ident)
{
    usize i = *cursor;
    if (i + ident.count > source.count ||
        memcmp(source.data + i, ident.data, ident.count) != 0) {
        return false;
    }
    if (i + ident.count < source.count &&
        lsp_signature_is_ident_char(source.data[i + ident.count])) {
        return false;
    }
    *cursor = i + ident.count;
    return true;
}

internal void lsp_signature_skip_space(string source, usize* cursor)
{
    while (*cursor < source.count &&
           (source.data[*cursor] == ' ' || source.data[*cursor] == '\t')) {
        (*cursor)++;
    }
}

internal string lsp_signature_source_slice_trim(string source,
                                                usize  start,
                                                usize  end)
{
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

internal string lsp_signature_normalize_source_param_label(Arena* arena,
                                                           string label)
{
    label     = lsp_signature_source_slice_trim(label, 0, label.count);

    u32 depth = 0;
    for (usize i = 0; i < label.count; ++i) {
        u8 c = label.data[i];
        if (c == '(' || c == '[' || c == '{') {
            depth++;
        } else if (c == ')' || c == ']' || c == '}') {
            if (depth > 0) {
                depth--;
            }
        } else if (c == ':' && depth == 0) {
            string name = lsp_signature_source_slice_trim(label, 0, i);
            string type =
                lsp_signature_source_slice_trim(label, i + 1, label.count);
            if (name.count == 0 || type.count == 0) {
                return label;
            }
            return string_format(
                arena, STRINGP ": " STRINGP, STRINGV(name), STRINGV(type));
        }
    }

    return label;
}

internal void
lsp_signature_append_normalized_source_param_label(StringBuilder* sb,
                                                   string         label)
{
    label     = lsp_signature_source_slice_trim(label, 0, label.count);

    u32 depth = 0;
    for (usize i = 0; i < label.count; ++i) {
        u8 c = label.data[i];
        if (c == '(' || c == '[' || c == '{') {
            depth++;
        } else if (c == ')' || c == ']' || c == '}') {
            if (depth > 0) {
                depth--;
            }
        } else if (c == ':' && depth == 0) {
            string name = lsp_signature_source_slice_trim(label, 0, i);
            string type =
                lsp_signature_source_slice_trim(label, i + 1, label.count);
            if (name.count > 0 && type.count > 0) {
                sb_append_string(sb, name);
                sb_append_cstr(sb, ": ");
                sb_append_string(sb, type);
                return;
            }
        }
    }

    sb_append_string(sb, label);
}

internal void lsp_signature_append_source_params_label(StringBuilder* sb,
                                                       string         source,
                                                       usize params_start,
                                                       usize params_end)
{
    usize item_start = params_start;
    u32   depth      = 0;
    u32   count      = 0;
    for (usize i = params_start; i <= params_end; ++i) {
        u8 c = i < params_end ? source.data[i] : ',';
        if (c == '(' || c == '[' || c == '{') {
            depth++;
        } else if (c == ')' || c == ']' || c == '}') {
            if (depth > 0) {
                depth--;
            }
        } else if (c == ',' && depth == 0) {
            string label =
                lsp_signature_source_slice_trim(source, item_start, i);
            if (label.count > 0) {
                if (count > 0) {
                    sb_append_cstr(sb, ", ");
                }
                lsp_signature_append_normalized_source_param_label(sb, label);
                count++;
            }
            item_start = i + 1;
        }
    }
}

internal void lsp_signature_add_source_params(Arena*     arena,
                                              JsonValue* params,
                                              string     source,
                                              usize      params_start,
                                              usize      params_end)
{
    usize item_start = params_start;
    u32   depth      = 0;
    for (usize i = params_start; i <= params_end; ++i) {
        u8 c = i < params_end ? source.data[i] : ',';
        if (c == '(' || c == '[' || c == '{') {
            depth++;
        } else if (c == ')' || c == ']' || c == '}') {
            if (depth > 0) {
                depth--;
            }
        } else if (c == ',' && depth == 0) {
            string label =
                lsp_signature_source_slice_trim(source, item_start, i);
            if (label.count > 0) {
                JsonValue* param = json_new_object(arena);
                json_object_set_string(
                    param,
                    arena,
                    "label",
                    lsp_signature_normalize_source_param_label(arena, label));
                json_array_push(params, param);
            }
            item_start = i + 1;
        }
    }
}

internal bool lsp_signature_source_decl_label(Arena*      arena,
                                              string      source,
                                              string      name,
                                              string*     out_label,
                                              JsonValue** out_params)
{
    for (usize line_start = 0; line_start < source.count;) {
        usize line_end = line_start;
        while (line_end < source.count && source.data[line_end] != '\n') {
            line_end++;
        }

        usize cursor = line_start;
        lsp_signature_skip_space(source, &cursor);
        if (lsp_signature_match_ident_at(source, &cursor, s("pub"))) {
            lsp_signature_skip_space(source, &cursor);
        }

        usize ident_start = cursor;
        while (cursor < line_end &&
               lsp_signature_is_ident_char(source.data[cursor])) {
            cursor++;
        }
        string ident = {.data  = source.data + ident_start,
                        .count = cursor - ident_start};
        if (!string_eq(ident, name)) {
            line_start = line_end + (line_end < source.count ? 1 : 0);
            continue;
        }

        lsp_signature_skip_space(source, &cursor);
        if (cursor + 1 < line_end && source.data[cursor] == ':' &&
            source.data[cursor + 1] == ':') {
            cursor += 2;
            lsp_signature_skip_space(source, &cursor);
            if (!lsp_signature_match_ident_at(source, &cursor, s("fn"))) {
                line_start = line_end + (line_end < source.count ? 1 : 0);
                continue;
            }
            lsp_signature_skip_space(source, &cursor);
        }
        if (cursor >= source.count || source.data[cursor] != '(') {
            line_start = line_end + (line_end < source.count ? 1 : 0);
            continue;
        }

        usize params_start = cursor + 1;
        usize params_end   = U32_MAX;
        u32   depth        = 0;
        for (usize i = cursor; i < source.count; ++i) {
            u8 c = source.data[i];
            if (c == '(') {
                depth++;
            } else if (c == ')') {
                if (depth == 0) {
                    break;
                }
                depth--;
                if (depth == 0) {
                    params_end = i;
                    cursor     = i + 1;
                    break;
                }
            }
        }
        if (params_end == U32_MAX) {
            line_start = line_end + (line_end < source.count ? 1 : 0);
            continue;
        }

        StringBuilder sb = {0};
        sb_init(&sb, arena);
        sb_append_string(&sb, name);
        sb_append_cstr(&sb, "(");
        lsp_signature_append_source_params_label(
            &sb, source, params_start, params_end);
        sb_append_cstr(&sb, ")");

        lsp_signature_skip_space(source, &cursor);
        if (cursor + 1 < source.count && source.data[cursor] == '-' &&
            source.data[cursor + 1] == '>') {
            cursor += 2;
            usize return_start = cursor;
            while (cursor < source.count && source.data[cursor] != '{' &&
                   source.data[cursor] != '\n' &&
                   !(cursor + 1 < source.count && source.data[cursor] == '=' &&
                     source.data[cursor + 1] == '>')) {
                cursor++;
            }
            string return_type =
                lsp_signature_source_slice_trim(source, return_start, cursor);
            if (return_type.count > 0) {
                sb_append_cstr(&sb, " -> ");
                sb_append_string(&sb, return_type);
            }
        }

        JsonValue* params = json_new_array(arena);
        lsp_signature_add_source_params(
            arena, params, source, params_start, params_end);
        *out_label  = sb_to_string(&sb);
        *out_params = params;
        return true;
    }

    return false;
}

internal cstr lsp_signature_text_module_relative(Arena* arena,
                                                 string module_path,
                                                 cstr   extension)
{
    StringBuilder sb = {0};
    sb_init(&sb, arena);
    for (usize i = 0; i < module_path.count; ++i) {
        if (module_path.data[i] == '.') {
#if OS_WINDOWS
            sb_append_char(&sb, '\\');
#else
            sb_append_char(&sb, '/');
#endif
        } else {
            sb_append_char(&sb, (char)module_path.data[i]);
        }
    }
    sb_append_cstr(&sb, extension);
    sb_append_null(&sb);
    return (cstr)sb_to_string(&sb).data;
}

internal bool lsp_signature_resolve_text_module_in_root(Arena* arena,
                                                        string module_path,
                                                        cstr   root,
                                                        cstr*  out_path)
{
    cstr module_file =
        path_join(arena,
                  root,
                  lsp_signature_text_module_relative(arena, module_path, ".n"));
    if (!path_exists(module_file) || path_is_directory(module_file)) {
        cstr module_dir = path_join(
            arena,
            root,
            lsp_signature_text_module_relative(arena, module_path, ""));
        module_file = path_join(arena, module_dir, "mod.n");
    }

    if (!path_exists(module_file) || path_is_directory(module_file)) {
        return false;
    }
    cstr canonical = path_canonical(arena, module_file);
    if (canonical == NULL) {
        return false;
    }
    *out_path = canonical;
    return true;
}

internal bool lsp_signature_resolve_text_module(Arena*             arena,
                                                const LspDocument* doc,
                                                string             module_path,
                                                string current_source_path,
                                                cstr*  out_path)
{
    NerdSource current_source = doc->front_end.lexer.source;
    if (current_source.source_path.count == 0) {
        current_source.source_path = current_source_path;
    }
    cstr current_path = module_source_file_path(arena, current_source);
    if (current_path != NULL &&
        lsp_signature_resolve_text_module_in_root(
            arena, module_path, path_dirname(arena, current_path), out_path)) {
        return true;
    }

    NerdSource root_source = doc->program.root_source.source_path.count > 0
                                 ? doc->program.root_source
                                 : doc->front_end.lexer.source;
    cstr       root_path   = module_source_file_path(arena, root_source);
    if (root_path != NULL &&
        lsp_signature_resolve_text_module_in_root(
            arena, module_path, path_dirname(arena, root_path), out_path)) {
        return true;
    }

    cstr lib_path = getenv("NERD_LIB_PATH");
    if (lib_path != NULL && *lib_path != '\0') {
#if OS_WINDOWS
        char separator = ';';
#else
        char separator = ':';
#endif
        const char* cursor = lib_path;
        while (*cursor != '\0') {
            const char* end = strchr(cursor, separator);
            usize len = end != NULL ? (usize)(end - cursor) : strlen(cursor);
            if (len > 0) {
                char* root = arena_alloc(arena, len + 1);
                memcpy(root, cursor, len);
                root[len] = '\0';
                if (lsp_signature_resolve_text_module_in_root(
                        arena, module_path, root, out_path)) {
                    return true;
                }
            }
            if (end == NULL) {
                break;
            }
            cursor = end + 1;
        }
    }

    cstr exe_dir  = path_executable_dir(arena);
    cstr mods_dir = path_join(arena, exe_dir, "mods");
    return lsp_signature_resolve_text_module_in_root(
        arena, module_path, mods_dir, out_path);
}

internal bool lsp_signature_source_use_decl_label(Arena*             arena,
                                                  const LspDocument* doc,
                                                  string      document_uri,
                                                  string      name,
                                                  string*     out_label,
                                                  JsonValue** out_params)
{
    Arena temp = {0};
    arena_init(&temp);

    bool found = false;
    for (usize line_start = 0; line_start < doc->source.count && !found;) {
        usize line_end = line_start;
        while (line_end < doc->source.count &&
               doc->source.data[line_end] != '\n') {
            line_end++;
        }

        string line   = {.data  = doc->source.data + line_start,
                         .count = line_end - line_start};
        usize  cursor = 0;
        lsp_signature_skip_space(line, &cursor);
        if (lsp_signature_match_ident_at(line, &cursor, s("pub"))) {
            lsp_signature_skip_space(line, &cursor);
        }
        if (!lsp_signature_match_ident_at(line, &cursor, s("use"))) {
            line_start = line_end + (line_end < doc->source.count ? 1 : 0);
            continue;
        }

        lsp_signature_skip_space(line, &cursor);
        usize path_start = cursor;
        while (cursor < line.count &&
               (lsp_signature_is_ident_char(line.data[cursor]) ||
                line.data[cursor] == '.')) {
            cursor++;
        }
        if (cursor == path_start) {
            line_start = line_end + (line_end < doc->source.count ? 1 : 0);
            continue;
        }

        string module_path = {.data  = line.data + path_start,
                              .count = cursor - path_start};
        cstr   resolved    = NULL;
        if (lsp_signature_resolve_text_module(
                &temp, doc, module_path, document_uri, &resolved)) {
            FileMap map    = {0};
            string  source = filemap_load(resolved, &map);
            if (source.data != NULL) {
                found = lsp_signature_source_decl_label(
                    arena, source, name, out_label, out_params);
                filemap_unload(&map);
            }
        }

        line_start = line_end + (line_end < doc->source.count ? 1 : 0);
    }

    arena_done(&temp);
    return found;
}

void lsp_handle_signature_help(LspState* state, const LspMessage* message)
{
    JsonValue* response = lsp_prepare_response(message);

    string uri          = {0};
    if (!lsp_get_string_param(message, "params.textDocument.uri", &uri)) {
        lsp_cancel(response, message->arena);
        return;
    }

    LspSourceView source_view = {0};
    if (!lsp_source_view(state, uri, &source_view)) {
        lsp_cancel(response, message->arena);
        return;
    }
    u64 line      = 0;
    u64 character = 0;
    (void)lsp_get_u64_param(message, "params.position.line", &line);
    (void)lsp_get_u64_param(message, "params.position.character", &character);
    usize offset =
        lsp_offset_from_position(source_view.source, line, character);

    string name         = {0};
    u32    active_param = 0;
    if (!lsp_signature_call_context(
            source_view.source, offset, &name, &active_param)) {
        json_object_set_null(response, message->arena, "result");
        lsp_send_response(message->arena, response);
        return;
    }

    LspTypeFactView view             = {0};
    ProgramInfo     repaired_program = {0};
    LspDocument     repaired_doc     = {0};
    bool            using_repaired   = false;
    if (!lsp_type_fact_view(state, uri, &view)) {
        using_repaired =
            lsp_signature_repaired_type_fact_view(message->arena,
                                                  uri,
                                                  source_view.source,
                                                  offset,
                                                  &repaired_program,
                                                  &repaired_doc,
                                                  &view);
    }
    if (!view.sema) {
        string     source_label      = {0};
        JsonValue* source_parameters = NULL;
        if (lsp_signature_source_decl_label(message->arena,
                                            source_view.source,
                                            name,
                                            &source_label,
                                            &source_parameters) ||
            lsp_signature_source_use_decl_label(message->arena,
                                                source_view.doc,
                                                uri,
                                                name,
                                                &source_label,
                                                &source_parameters)) {
            JsonValue* signature = json_new_object(message->arena);
            json_object_set_string(
                signature, message->arena, "label", source_label);
            json_object_set_array(signature, "parameters", source_parameters);

            JsonValue* signatures = json_new_array(message->arena);
            json_array_push(signatures, signature);

            JsonValue* result = json_new_object(message->arena);
            json_object_set_array(result, "signatures", signatures);
            json_object_set_number(
                result, message->arena, "activeSignature", 0);
            json_object_set_number(
                result, message->arena, "activeParameter", active_param);
            json_object_set_object(response, "result", result);
            lsp_send_response(message->arena, response);
            if (using_repaired) {
                program_info_done(&repaired_program);
            }
            return;
        }
        json_object_set_null(response, message->arena, "result");
        lsp_send_response(message->arena, response);
        if (using_repaired) {
            program_info_done(&repaired_program);
        }
        return;
    }

    const SemaDecl* decl = lsp_signature_find_decl(&view, name);
    if (decl == NULL && !using_repaired) {
        using_repaired =
            lsp_signature_repaired_type_fact_view(message->arena,
                                                  uri,
                                                  source_view.source,
                                                  offset,
                                                  &repaired_program,
                                                  &repaired_doc,
                                                  &view);
        if (using_repaired) {
            decl = lsp_signature_find_decl(&view, name);
        }
    }
    if (decl == NULL) {
        string     source_label      = {0};
        JsonValue* source_parameters = NULL;
        if (lsp_signature_source_decl_label(message->arena,
                                            source_view.source,
                                            name,
                                            &source_label,
                                            &source_parameters) ||
            lsp_signature_source_use_decl_label(message->arena,
                                                source_view.doc,
                                                uri,
                                                name,
                                                &source_label,
                                                &source_parameters)) {
        } else {
            json_object_set_null(response, message->arena, "result");
            lsp_send_response(message->arena, response);
            if (using_repaired) {
                program_info_done(&repaired_program);
            }
            return;
        }

        JsonValue* signature = json_new_object(message->arena);
        json_object_set_string(
            signature, message->arena, "label", source_label);
        json_object_set_string(
            signature,
            message->arena,
            "documentation",
            s("Named arguments use `name = value`; omitted "
              "parameters use declared defaults when available."));
        json_object_set_array(signature, "parameters", source_parameters);

        JsonValue* signatures = json_new_array(message->arena);
        json_array_push(signatures, signature);

        JsonValue* result = json_new_object(message->arena);
        json_object_set_array(result, "signatures", signatures);
        json_object_set_number(result, message->arena, "activeSignature", 0);
        json_object_set_number(
            result, message->arena, "activeParameter", active_param);
        json_object_set_object(response, "result", result);
        lsp_send_response(message->arena, response);
        if (using_repaired) {
            program_info_done(&repaired_program);
        }
        return;
    }

    string     label      = {0};
    JsonValue* parameters = NULL;
    bool       labelled   = lsp_signature_decl_label(
        &view, message->arena, decl, &label, &parameters);
    if (!labelled && !using_repaired) {
        using_repaired =
            lsp_signature_repaired_type_fact_view(message->arena,
                                                  uri,
                                                  source_view.source,
                                                  offset,
                                                  &repaired_program,
                                                  &repaired_doc,
                                                  &view);
        if (using_repaired) {
            decl     = lsp_signature_find_decl(&view, name);
            labelled = decl != NULL &&
                       lsp_signature_decl_label(
                           &view, message->arena, decl, &label, &parameters);
        }
    }
    if (!labelled) {
        labelled = lsp_signature_source_decl_label(message->arena,
                                                   source_view.source,
                                                   name,
                                                   &label,
                                                   &parameters) ||
                   lsp_signature_source_use_decl_label(message->arena,
                                                       source_view.doc,
                                                       uri,
                                                       name,
                                                       &label,
                                                       &parameters);
        if (!labelled) {
            json_object_set_null(response, message->arena, "result");
            lsp_send_response(message->arena, response);
            if (using_repaired) {
                program_info_done(&repaired_program);
            }
            return;
        }
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
    if (using_repaired) {
        program_info_done(&repaired_program);
    }
}

//------------------------------------------------------------------------------
