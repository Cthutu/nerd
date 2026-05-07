//------------------------------------------------------------------------------
// Symbol-aware LSP queries
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/modules/modules.h>
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
// Return the analysed-source offset where the open editor document begins.
// Folder modules analyse `mod.n` plus implicit sibling parts as one source, but
// LSP requests are still expressed relative to the file open in the editor.

internal bool lsp_document_visible_start(const LspDocument* doc,
                                         usize*             out_start)
{
    string analysed = doc->front_end.lexer.source.source;
    if (string_eq(analysed, doc->source)) {
        *out_start = 0;
        return true;
    }

    if (doc->source.count <= analysed.count &&
        memcmp(analysed.data, doc->source.data, doc->source.count) == 0) {
        *out_start = 0;
        return true;
    }

    if (doc->source.count == 0) {
        *out_start = 0;
        return true;
    }

    for (usize i = 0; i + doc->source.count <= analysed.count; ++i) {
        if (memcmp(analysed.data + i, doc->source.data, doc->source.count) ==
            0) {
            *out_start = i;
            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------
// Build a range relative to the open editor document when possible.

internal JsonValue* lsp_make_document_range(const LspDocument* doc,
                                            Arena*             arena,
                                            usize              start_offset,
                                            usize              end_offset)
{
    usize visible_start = 0;
    if (lsp_document_visible_start(doc, &visible_start) &&
        start_offset >= visible_start && end_offset >= visible_start &&
        end_offset <= visible_start + doc->source.count) {
        NerdSource visible_source = {
            .source      = doc->source,
            .source_path = doc->front_end.lexer.source.source_path,
        };
        return lsp_make_range(arena,
                              visible_source,
                              start_offset - visible_start,
                              end_offset - visible_start);
    }

    return lsp_make_range(
        arena, doc->front_end.lexer.source, start_offset, end_offset);
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
// Return the AST field node that owns a given token index, if any.

internal u32 lsp_find_field_node_at_token(const Ast* ast, u32 token_index)
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
// Return the AST module-reference node that owns a given token index, if any.

internal u32 lsp_find_modref_node_at_token(const Lexer* lexer,
                                           const Ast*   ast,
                                           u32          token_index)
{
    UNUSED(lexer);
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind != AK_ModRef ||
            node->a >= array_count(ast->module_paths)) {
            continue;
        }

        const AstModulePath* path          = &ast->module_paths[node->a];
        u32                  current_token = node->token_index;
        for (u32 j = 0; j < path->symbol_count; ++j) {
            if (current_token == token_index) {
                return i;
            }
            current_token += 1;
            if (j + 1 < path->symbol_count) {
                current_token += 1;
            }
        }
    }

    return U32_MAX;
}

//------------------------------------------------------------------------------
// Convert one filesystem path into a file:// URI for LSP responses.

internal string lsp_path_to_uri(Arena* arena, cstr path)
{
    StringBuilder sb = {0};
    sb_init(&sb, arena);
    sb_append_cstr(&sb, "file://");

#if OS_WINDOWS
    if (path[0] != '\0' && path[1] == ':') {
        sb_append_char(&sb, '/');
    }
#endif

    for (const char* cursor = path; *cursor != '\0'; ++cursor) {
        u8 ch = (u8)*cursor;
#if OS_WINDOWS
        if (ch == '\\') {
            sb_append_char(&sb, '/');
            continue;
        }
#endif
        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') ||
            (ch >= '0' && ch <= '9') || ch == '/' || ch == '-' || ch == '_' ||
            ch == '.' || ch == '~') {
            sb_append_char(&sb, (char)ch);
        } else {
            sb_format(&sb, "%%%02X", ch);
        }
    }

    return sb_to_string(&sb);
}

//------------------------------------------------------------------------------
// Build a location object for one declaration inside a specific module.

internal JsonValue*
lsp_module_decl_location(const ModuleInfo* module, Arena* arena, u32 decl_index)
{
    if (decl_index >= array_count(module->front_end.sema.decls)) {
        return NULL;
    }

    const SemaDecl* decl = &module->front_end.sema.decls[decl_index];
    if (decl->bind_node_index == LSP_NO_DECL ||
        decl->bind_node_index >= array_count(module->front_end.ast.nodes)) {
        return NULL;
    }

    const AstNode* bind = &module->front_end.ast.nodes[decl->bind_node_index];
    usize          start_offset;
    usize          end_offset;
    lsp_token_offsets(&module->front_end.lexer,
                      bind->token_index,
                      &start_offset,
                      &end_offset);

    JsonValue* location = json_new_object(arena);
    json_object_set_string(
        location, arena, "uri", lsp_path_to_uri(arena, module->resolved_path));
    json_object_set_object(
        location,
        "range",
        lsp_make_range(
            arena, module->front_end.lexer.source, start_offset, end_offset));
    return location;
}

//------------------------------------------------------------------------------
// Build a location object for one token in the open document.

internal JsonValue* lsp_token_location(const LspDocument* doc,
                                       Arena*             arena,
                                       string             uri,
                                       u32                token_index)
{
    if (token_index >= array_count(doc->front_end.lexer.tokens)) {
        return NULL;
    }

    usize start_offset = 0;
    usize end_offset   = 0;
    lsp_token_offsets(
        &doc->front_end.lexer, token_index, &start_offset, &end_offset);

    JsonValue* location = json_new_object(arena);
    json_object_set_string(location, arena, "uri", uri);
    json_object_set_object(
        location,
        "range",
        lsp_make_document_range(doc, arena, start_offset, end_offset));
    return location;
}

//------------------------------------------------------------------------------
// Build a location object for the start of one module source file.

internal JsonValue* lsp_module_file_location(const ModuleInfo* module,
                                             Arena*            arena)
{
    JsonValue* location = json_new_object(arena);
    json_object_set_string(
        location, arena, "uri", lsp_path_to_uri(arena, module->resolved_path));
    json_object_set_object(
        location,
        "range",
        lsp_make_range(arena, module->front_end.lexer.source, 0, 0));
    return location;
}

//------------------------------------------------------------------------------
// Resolve one imported symbol through a module type back to its exporting decl.

internal JsonValue* lsp_imported_symbol_location(const LspDocument* doc,
                                                 Arena*             arena,
                                                 u32                module_type,
                                                 u32 symbol_handle)
{
    if (module_type == sema_no_type() ||
        module_type >= array_count(doc->front_end.sema.types)) {
        return NULL;
    }

    const SemaType* type = &doc->front_end.sema.types[module_type];
    if (type->kind != STK_Module || doc->program.modules == NULL ||
        type->return_type >= array_count(doc->program.modules)) {
        return NULL;
    }

    const ModuleInfo* module = &doc->program.modules[type->return_type];
    for (u32 i = 0; i < type->param_count; ++i) {
        if (doc->front_end.sema.type_param_symbols[type->first_param_type +
                                                   i] != symbol_handle) {
            continue;
        }
        if (i >= array_count(module->export_decl_indices)) {
            return NULL;
        }
        return lsp_module_decl_location(
            module, arena, module->export_decl_indices[i]);
    }

    return NULL;
}

//------------------------------------------------------------------------------
// Resolve one loaded module by canonical path inside the current program.

internal u32 lsp_find_program_module_by_path(const ProgramInfo* program,
                                             cstr               resolved_path)
{
    if (program == NULL) {
        return U32_MAX;
    }

    for (u32 i = 0; i < array_count(program->modules); ++i) {
        if (strcmp(program->modules[i].resolved_path, resolved_path) == 0) {
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
    u32 decl_index = LSP_NO_DECL;
    if (ref_node_index != U32_MAX) {
        if (lsp_sema_node_decl(
                &doc->front_end.sema, ref_node_index, &decl_index)) {
            return decl_index;
        }

        const AstNode* ref = &doc->front_end.ast.nodes[ref_node_index];
        decl_index =
            lsp_find_decl_index_by_symbol_handle(&doc->front_end.sema, ref->a);
        const SemaDecl* decl = NULL;
        if (lsp_sema_decl(&doc->front_end.sema, decl_index, &decl) &&
            (decl->kind == SK_TypeAlias ||
             decl->kind == SK_GenericTypeAlias)) {
            return decl_index;
        }
    }

    u32 field_node_index =
        lsp_find_field_node_at_token(&doc->front_end.ast, token_index);
    if (lsp_sema_node_decl(
            &doc->front_end.sema, field_node_index, &decl_index)) {
        return decl_index;
    }

    return LSP_NO_DECL;
}

//------------------------------------------------------------------------------
// Return a syntax-level definition location for a symbol when semantic data is
// unavailable or incomplete. This is deliberately conservative: it points at a
// same-file binder with the same spelling, preferring a binder before the use.

internal JsonValue* lsp_ast_symbol_location(const LspDocument* doc,
                                            Arena*             arena,
                                            string             uri,
                                            u32                token_index,
                                            u32                symbol_handle)
{
    const Ast* ast    = &doc->front_end.ast;

    u32 first_token   = U32_MAX;
    u32 nearest_token = U32_MAX;

    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if ((node->kind != AK_Bind && node->kind != AK_Variable) ||
            node->a != symbol_handle) {
            continue;
        }

        if (first_token == U32_MAX || node->token_index < first_token) {
            first_token = node->token_index;
        }
        if (node->token_index <= token_index &&
            (nearest_token == U32_MAX || node->token_index > nearest_token)) {
            nearest_token = node->token_index;
        }
    }

    for (u32 i = 0; i < array_count(ast->params); ++i) {
        const AstParam* param = &ast->params[i];
        if (param->symbol_handle != symbol_handle) {
            continue;
        }

        if (first_token == U32_MAX || param->token_index < first_token) {
            first_token = param->token_index;
        }
        if (param->token_index <= token_index &&
            (nearest_token == U32_MAX || param->token_index > nearest_token)) {
            nearest_token = param->token_index;
        }
    }

    u32 best_token = nearest_token != U32_MAX ? nearest_token : first_token;
    if (best_token == U32_MAX) {
        return NULL;
    }

    return lsp_token_location(doc, arena, uri, best_token);
}

//------------------------------------------------------------------------------
// Return a syntax-level definition location for the symbol under one token.

internal JsonValue* lsp_ast_definition_location(const LspDocument* doc,
                                                Arena*             arena,
                                                string             uri,
                                                u32                token_index)
{
    u32 ref_node_index =
        lsp_find_symbol_ref_node_at_token(&doc->front_end.ast, token_index);
    if (ref_node_index != U32_MAX) {
        return lsp_ast_symbol_location(
            doc,
            arena,
            uri,
            token_index,
            doc->front_end.ast.nodes[ref_node_index].a);
    }

    u32 bind_node_index =
        lsp_find_bind_node_at_token(&doc->front_end.ast, token_index);
    if (bind_node_index != U32_MAX) {
        return lsp_token_location(doc, arena, uri, token_index);
    }

    return NULL;
}

internal u32 lsp_find_local_index_for_token(const LspDocument* doc,
                                            u32                token_index)
{
    const Ast*  ast  = &doc->front_end.ast;
    const Sema* sema = &doc->front_end.sema;
    for (u32 i = 0; i < array_count(ast->on_branches); ++i) {
        const AstOnBranch* branch = &ast->on_branches[i];
        if (branch->binder_token_index == token_index &&
            i < array_count(sema->on_branch_local_indices)) {
            return sema->on_branch_local_indices[i];
        }
    }

    u32 bind_node_index =
        lsp_find_bind_node_at_token(&doc->front_end.ast, token_index);
    u32 local_index = sema_no_local();
    if (lsp_sema_node_local(
            &doc->front_end.sema, bind_node_index, &local_index)) {
        return local_index;
    }

    u32 ref_node_index =
        lsp_find_symbol_ref_node_at_token(&doc->front_end.ast, token_index);
    if (lsp_sema_node_local(
            &doc->front_end.sema, ref_node_index, &local_index)) {
        return local_index;
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
            u32 decl_index = LSP_NO_DECL;
            if (!lsp_sema_node_decl(
                    &doc->front_end.sema, node_index, &decl_index)) {
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
    const SemaDecl* decl = NULL;
    if (!lsp_sema_decl(&doc->front_end.sema, decl_index, &decl)) {
        return false;
    }
    if (decl->kind != SK_Constant) {
        return false;
    }

    return lsp_eval_ast_node(doc, decl->value_node_index, out_value);
}

//------------------------------------------------------------------------------
// Return the current signature text for one function declaration.

internal string lsp_default_param_source(const LspDocument* doc,
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

    while (start < end && (lexer->source.source.data[start] == ' ' ||
                           lexer->source.source.data[start] == '\t' ||
                           lexer->source.source.data[start] == '\n' ||
                           lexer->source.source.data[start] == '\r')) {
        start++;
    }
    while (end > start && (lexer->source.source.data[end - 1] == ' ' ||
                           lexer->source.source.data[end - 1] == '\t' ||
                           lexer->source.source.data[end - 1] == '\n' ||
                           lexer->source.source.data[end - 1] == '\r')) {
        end--;
    }
    string raw = {
        .data  = lexer->source.source.data + start,
        .count = end - start,
    };
    return raw;
}

internal string lsp_trim_source(const LspDocument* doc, usize start, usize end)
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

internal string lsp_param_type_source(const LspDocument* doc,
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
    return lsp_trim_source(doc, start, end);
}

internal string lsp_return_type_source(const LspDocument* doc,
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
    return lsp_trim_source(doc, start, end);
}

internal bool lsp_decl_ast_signature(const LspDocument* doc,
                                     Arena*             arena,
                                     const SemaDecl*    decl,
                                     string*            out_signature)
{
    if (decl->value_node_index == sema_no_decl() ||
        (decl->kind != SK_GenericFunction &&
         decl->type_index >= array_count(doc->front_end.sema.types))) {
        return false;
    }

    const Ast*            ast        = &doc->front_end.ast;
    const AstNode*        value_node = &ast->nodes[decl->value_node_index];
    const AstFnSignature* signature  = NULL;

    if (value_node->kind == AK_FnDef) {
        const AstNode* fn_start = &ast->nodes[value_node->a];
        signature               = &ast->fn_signatures[fn_start->a];
    } else if (value_node->kind == AK_FfiDef) {
        const AstFfiInfo* ffi = &ast->ffi_infos[value_node->a];
        signature             = &ast->fn_signatures[ffi->signature_index];
    } else {
        return false;
    }

    const SemaType* type = decl->kind == SK_GenericFunction
                               ? NULL
                               : &doc->front_end.sema.types[decl->type_index];
    if (decl->kind != SK_GenericFunction && type->kind != STK_Function) {
        return false;
    }
    bool has_generic = signature->generic_params_index != U32_MAX;
    bool has_default = false;
    for (u32 i = 0; i < signature->param_count; ++i) {
        const AstParam* param = &ast->params[signature->first_param + i];
        if (param->default_node_index != U32_MAX) {
            has_default = true;
            break;
        }
    }
    if (!has_default && !has_generic) {
        return false;
    }

    Arena build_arena = {0};
    Arena text_arena  = {0};
    arena_init(&build_arena);
    arena_init(&text_arena);

    StringBuilder sb = {0};
    sb_init(&sb, &build_arena);
    sb_append_cstr(&sb, "fn");
    if (has_generic) {
        const AstGenericParams* generic =
            &ast->generic_params[signature->generic_params_index];
        sb_append_cstr(&sb, " [");
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
    sb_append_cstr(&sb, " (");
    for (u32 i = 0; i < signature->param_count; ++i) {
        if (i > 0) {
            sb_append_cstr(&sb, ", ");
        }
        const AstParam* param = &ast->params[signature->first_param + i];
        if (param->symbol_handle != U32_MAX) {
            sb_append_string(
                &sb, lex_symbol(&doc->front_end.lexer, param->symbol_handle));
            sb_append_cstr(&sb, ": ");
        }
        if (has_generic) {
            sb_append_string(&sb, lsp_param_type_source(doc, param));
        } else {
            u32 param_type =
                i < type->param_count
                    ? doc->front_end.sema
                          .type_param_types[type->first_param_type + i]
                    : sema_no_type();
            sb_append_string(&sb,
                             sema_type_name(&doc->front_end.lexer,
                                            &doc->front_end.sema,
                                            &text_arena,
                                            param_type));
        }
        if (param->default_node_index != U32_MAX) {
            sb_append_cstr(&sb, " = ");
            sb_append_string(&sb, lsp_default_param_source(doc, param));
        }
    }
    if (signature->is_varargs) {
        if (signature->param_count > 0) {
            sb_append_cstr(&sb, ", ");
        }
        sb_append_cstr(&sb, "...");
    }
    sb_append_char(&sb, ')');
    if (has_generic && signature->return_type_node_index != U32_MAX) {
        sb_append_cstr(&sb, " -> ");
        sb_append_string(
            &sb,
            lsp_return_type_source(doc, signature->return_type_node_index));
    } else if (!has_generic && type->return_type != sema_no_type()) {
        sb_append_cstr(&sb, " -> ");
        sb_append_string(&sb,
                         sema_type_name(&doc->front_end.lexer,
                                        &doc->front_end.sema,
                                        &text_arena,
                                        type->return_type));
    }

    string built   = sb_to_string(&sb);
    *out_signature = string_format(arena, STRINGP, STRINGV(built));
    arena_done(&text_arena);
    arena_done(&build_arena);
    return true;
}

internal string lsp_decl_signature(const LspDocument* doc,
                                   Arena*             arena,
                                   const SemaDecl*    decl)
{
    if (decl->kind != SK_Function && decl->kind != SK_GenericFunction &&
        decl->kind != SK_FfiFunction && decl->kind != SK_BuiltinFunction) {
        return s("<unknown>");
    }
    if (decl->kind == SK_BuiltinFunction) {
        string name = lex_symbol(&doc->front_end.lexer, decl->symbol_handle);
        if (string_eq(name, s("pr")) || string_eq(name, s("prn"))) {
            string rendered = sema_type_name(&doc->front_end.lexer,
                                             &doc->front_end.sema,
                                             arena,
                                             decl->type_index);
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
    string ast_signature = {0};
    if (lsp_decl_ast_signature(doc, arena, decl, &ast_signature)) {
        return ast_signature;
    }
    return sema_type_name(
        &doc->front_end.lexer, &doc->front_end.sema, arena, decl->type_index);
}

//------------------------------------------------------------------------------
// Infer the current hover-facing type for one AST node.

internal string lsp_infer_ast_type(const LspDocument* doc,
                                   Arena*             arena,
                                   u32                node_index)
{
    const AstNode* node = &doc->front_end.ast.nodes[node_index];
    UNUSED(node);

    u32 type_index = sema_no_type();
    if (!lsp_sema_node_type(&doc->front_end.sema, node_index, &type_index)) {
        return s("<unknown>");
    }

    return sema_type_name(&doc->front_end.lexer,
                          &doc->front_end.sema,
                          arena,
                          type_index);
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
    const SemaDecl* decl = NULL;
    if (!lsp_sema_decl(&doc->front_end.sema, decl_index, &decl)) {
        return s("<unknown>");
    }
    string name = lex_symbol(&doc->front_end.lexer, decl->symbol_handle);
    string kind = s("value");
    string inferred_type = s("<unknown>");
    if (decl->kind == SK_TypeAlias) {
        kind          = s("type alias");
        inferred_type = sema_type_name(&doc->front_end.lexer,
                                       &doc->front_end.sema,
                                       arena,
                                       decl->type_index);
    } else if (decl->kind == SK_Constant) {
        kind          = s("constant");
        inferred_type = lsp_infer_ast_type(doc, arena, decl->value_node_index);
    } else if (decl->kind == SK_Variable) {
        kind          = s("variable");
        inferred_type = sema_type_name(&doc->front_end.lexer,
                                       &doc->front_end.sema,
                                       arena,
                                       decl->type_index);
    } else if (decl->kind == SK_Module) {
        kind          = s("module");
        inferred_type = sema_type_name(&doc->front_end.lexer,
                                       &doc->front_end.sema,
                                       arena,
                                       decl->type_index);
    } else {
        kind          = s("function");
        inferred_type = lsp_decl_signature(doc, arena, decl);
    }

    if (decl->kind == SK_Function || decl->kind == SK_GenericFunction ||
        decl->kind == SK_FfiFunction || decl->kind == SK_BuiltinFunction) {
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
    const SemaLocal* local = NULL;
    if (!lsp_sema_local(&doc->front_end.sema, local_index, &local)) {
        return s("<unknown>");
    }
    string name = lex_symbol(&doc->front_end.lexer, local->symbol_handle);
    string type = sema_type_name(
        &doc->front_end.lexer, &doc->front_end.sema, arena, local->type_index);
    string kind =
        local->kind == SLK_Binder ? s("pattern binder") : s("local variable");

    return string_format(
        arena,
        STRINGP "\n\n- Kind: " STRINGP "\n- Type: `" STRINGP "`",
        STRINGV(lsp_markdown_code_block(
            arena, string_format(arena, STRINGP, STRINGV(name)))),
        STRINGV(kind),
        STRINGV(type));
}

//------------------------------------------------------------------------------
// AST-only field lookup helpers used when semantic analysis stopped before it
// attached type information to a field receiver.

internal u32 lsp_ast_type_symbol_with_self(const Lexer* lexer,
                                           const Ast*   ast,
                                           u32          type_node_index,
                                           u32          self_symbol)
{
    if (type_node_index >= array_count(ast->nodes)) {
        return U32_MAX;
    }

    const AstNode* type_node = &ast->nodes[type_node_index];
    if (type_node->kind == AK_Expression || type_node->kind == AK_Statement) {
        return lsp_ast_type_symbol_with_self(
            lexer, ast, type_node->a, self_symbol);
    }
    if (type_node->kind == AK_SymbolRef) {
        if (self_symbol != U32_MAX &&
            string_eq(lex_symbol(lexer, type_node->a), s("Self"))) {
            return self_symbol;
        }
        return type_node->a;
    }
    if (type_node->kind == AK_TypePointer) {
        return lsp_ast_type_symbol_with_self(
            lexer, ast, type_node->a, self_symbol);
    }
    return U32_MAX;
}

internal u32 lsp_ast_type_symbol(const Lexer* lexer,
                                 const Ast*   ast,
                                 u32          type_node_index)
{
    return lsp_ast_type_symbol_with_self(lexer, ast, type_node_index, U32_MAX);
}

internal u32 lsp_ast_impl_self_symbol_at_token(const LspDocument* doc,
                                               u32                token_index)
{
    const Lexer* lexer = &doc->front_end.lexer;
    const Ast*   ast   = &doc->front_end.ast;
    if (token_index >= array_count(lexer->tokens)) {
        return U32_MAX;
    }

    usize offset = lexer->tokens[token_index].offset;
    for (u32 impl_index = 0; impl_index < array_count(ast->impls);
         ++impl_index) {
        const AstImplInfo* impl = &ast->impls[impl_index];
        if (impl->body_node_index >= array_count(ast->nodes)) {
            continue;
        }

        u32 impl_node_index = U32_MAX;
        for (u32 node_index = 0; node_index < array_count(ast->nodes);
             ++node_index) {
            const AstNode* node = &ast->nodes[node_index];
            if (node->kind == AK_Impl && node->a == impl_index) {
                impl_node_index = node_index;
                break;
            }
        }
        if (impl_node_index == U32_MAX) {
            continue;
        }

        const AstNode* impl_node = &ast->nodes[impl_node_index];
        if (impl_node->token_index >= array_count(lexer->tokens) ||
            lexer->tokens[impl_node->token_index].offset > offset) {
            continue;
        }

        usize          end  = lexer->source.source.count;
        const AstNode* body = &ast->nodes[impl->body_node_index];
        if (body->kind == AK_Block) {
            end = 0;
            for (u32 node_index = body->a;
                 node_index < body->b && node_index < array_count(ast->nodes);
                 ++node_index) {
                const AstNode* node = &ast->nodes[node_index];
                if (node->token_index < array_count(lexer->tokens)) {
                    end = MAX(end,
                              lex_token_end_offset(
                                  lexer, &lexer->tokens[node->token_index]));
                }
            }
        }
        if (end != 0 && offset > end) {
            continue;
        }

        return lsp_ast_type_symbol(lexer, ast, impl->target_type_node_index);
    }

    return U32_MAX;
}

internal u32 lsp_ast_receiver_type_symbol(const LspDocument* doc,
                                          u32 receiver_node_index,
                                          u32 field_token_index)
{
    const Lexer* lexer = &doc->front_end.lexer;
    const Ast*   ast   = &doc->front_end.ast;
    if (receiver_node_index >= array_count(ast->nodes)) {
        return U32_MAX;
    }

    const AstNode* receiver_node = &ast->nodes[receiver_node_index];
    if (receiver_node->kind != AK_SymbolRef || receiver_node->a == U32_MAX) {
        return U32_MAX;
    }

    u32   receiver_symbol = receiver_node->a;
    usize field_offset    = field_token_index < array_count(lexer->tokens)
                                ? lexer->tokens[field_token_index].offset
                                : lexer->source.source.count;

    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if ((node->kind != AK_Variable && node->kind != AK_Bind) ||
            node->a != receiver_symbol || node->b >= array_count(ast->nodes)) {
            continue;
        }

        const AstNode* value = &ast->nodes[node->b];
        if (value->kind == AK_AnnotatedValue || value->kind == AK_ZeroInit ||
            value->kind == AK_Undefined) {
            return lsp_ast_type_symbol(lexer, ast, value->a);
        }
    }

    u32   best_type_symbol = U32_MAX;
    usize best_offset      = 0;
    for (u32 signature_index = 0;
         signature_index < array_count(ast->fn_signatures);
         ++signature_index) {
        const AstFnSignature* signature = &ast->fn_signatures[signature_index];
        for (u32 i = 0; i < signature->param_count; ++i) {
            u32 param_index = signature->first_param + i;
            if (param_index >= array_count(ast->params)) {
                break;
            }
            const AstParam* param = &ast->params[param_index];
            if (param->symbol_handle != receiver_symbol ||
                param->token_index >= array_count(lexer->tokens)) {
                continue;
            }
            usize param_offset = lexer->tokens[param->token_index].offset;
            if (param_offset > field_offset || param_offset < best_offset) {
                continue;
            }

            u32 self_symbol =
                lsp_ast_impl_self_symbol_at_token(doc, field_token_index);
            u32 type_symbol = lsp_ast_type_symbol_with_self(
                lexer, ast, param->type_node_index, self_symbol);
            if (type_symbol != U32_MAX) {
                best_type_symbol = type_symbol;
                best_offset      = param_offset;
            }
        }
    }

    return best_type_symbol;
}

internal const AstPlexField* lsp_ast_find_field_for_type_symbol(
    const LspDocument* doc, u32 type_symbol, u32 field_symbol)
{
    const Ast* ast = &doc->front_end.ast;
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind != AK_Bind || node->a != type_symbol ||
            node->b >= array_count(ast->nodes)) {
            continue;
        }

        const AstNode* value = &ast->nodes[node->b];
        if (value->kind != AK_TypePlex) {
            continue;
        }
        if (value->a >= array_count(ast->plex_types)) {
            return NULL;
        }

        const AstPlexTypeInfo* plex = &ast->plex_types[value->a];
        for (u32 field_index = 0; field_index < plex->field_count;
             ++field_index) {
            u32 ast_field_index = plex->first_field + field_index;
            if (ast_field_index >= array_count(ast->plex_fields)) {
                break;
            }

            const AstPlexField* field = &ast->plex_fields[ast_field_index];
            if (field->symbol_handle == field_symbol) {
                return field;
            }
        }
    }
    return NULL;
}

internal usize lsp_ast_type_node_end_offset(const LspDocument* doc,
                                            u32                type_node_index)
{
    const Lexer* lexer = &doc->front_end.lexer;
    const Ast*   ast   = &doc->front_end.ast;
    if (type_node_index >= array_count(ast->nodes)) {
        return 0;
    }

    const AstNode* node = &ast->nodes[type_node_index];
    if (node->token_index >= array_count(lexer->tokens)) {
        return 0;
    }

    switch (node->kind) {
    case AK_Expression:
    case AK_Statement:
    case AK_TypePointer:
    case AK_TypeSlice:
        {
            usize end = lsp_ast_type_node_end_offset(doc, node->a);
            return end != 0 ? end
                            : lex_token_end_offset(
                                  lexer, &lexer->tokens[node->token_index]);
        }
    case AK_TypeArray:
    case AK_TypeDynamicArray:
        {
            usize end = lsp_ast_type_node_end_offset(doc, node->b);
            return end != 0 ? end
                            : lex_token_end_offset(
                                  lexer, &lexer->tokens[node->token_index]);
        }
    case AK_TypeTuple:
        {
            if (node->b != 0) {
                u32 last_item = node->a + node->b - 1;
                if (last_item < array_count(ast->tuple_items)) {
                    usize end = lsp_ast_type_node_end_offset(
                        doc, ast->tuple_items[last_item]);
                    if (end != 0) {
                        return end;
                    }
                }
            }
            return lex_token_end_offset(lexer,
                                        &lexer->tokens[node->token_index]);
        }
    case AK_TypeApply:
        {
            if (node->a < array_count(ast->type_applications)) {
                const AstTypeApplyInfo* apply =
                    &ast->type_applications[node->a];
                if (apply->arg_count != 0) {
                    u32 last_arg = apply->first_arg + apply->arg_count - 1;
                    if (last_arg < array_count(ast->tuple_items)) {
                        usize end = lsp_ast_type_node_end_offset(
                            doc, ast->tuple_items[last_arg]);
                        if (end != 0) {
                            return end;
                        }
                    }
                }
                usize end =
                    lsp_ast_type_node_end_offset(doc, apply->target_node_index);
                if (end != 0) {
                    return end;
                }
            }
            return lex_token_end_offset(lexer,
                                        &lexer->tokens[node->token_index]);
        }
    default:
        return lex_token_end_offset(lexer, &lexer->tokens[node->token_index]);
    }
}

internal string lsp_ast_type_node_source(const LspDocument* doc,
                                         u32                type_node_index)
{
    if (type_node_index >= array_count(doc->front_end.ast.nodes)) {
        return s("<unknown>");
    }
    const AstNode* node = &doc->front_end.ast.nodes[type_node_index];
    if (node->token_index >= array_count(doc->front_end.lexer.tokens)) {
        return s("<unknown>");
    }

    usize start = doc->front_end.lexer.tokens[node->token_index].offset;
    usize end   = lsp_ast_type_node_end_offset(doc, type_node_index);
    if (end <= start || end > doc->front_end.lexer.source.source.count) {
        return s("<unknown>");
    }
    return lsp_trim_source(doc, start, end);
}

internal string lsp_ast_field_hover_text(const LspDocument* doc,
                                         Arena*             arena,
                                         u32                field_node_index)
{
    if (field_node_index >= array_count(doc->front_end.ast.nodes)) {
        return s("");
    }

    const AstNode* field = &doc->front_end.ast.nodes[field_node_index];
    if (field->kind != AK_Field) {
        return s("");
    }

    u32 type_symbol =
        lsp_ast_receiver_type_symbol(doc, field->a, field->token_index);
    if (type_symbol == U32_MAX) {
        return s("");
    }

    const AstPlexField* plex_field =
        lsp_ast_find_field_for_type_symbol(doc, type_symbol, field->b);
    if (plex_field == NULL) {
        return s("");
    }

    string type = s("<unknown>");
    if (plex_field->type_node_index <
        array_count(doc->front_end.sema.node_type_indices)) {
        u32 type_index =
            doc->front_end.sema.node_type_indices[plex_field->type_node_index];
        if (type_index != sema_no_type() &&
            type_index < array_count(doc->front_end.sema.types)) {
            type = sema_type_name(
                &doc->front_end.lexer, &doc->front_end.sema, arena, type_index);
        }
    }
    if (string_eq(type, s("<unknown>"))) {
        type = lsp_ast_type_node_source(doc, plex_field->type_node_index);
    }

    string name  = lex_symbol(&doc->front_end.lexer, field->b);
    string owner = lex_symbol(&doc->front_end.lexer, type_symbol);
    return string_format(
        arena,
        STRINGP "\n\n- Kind: plex field\n- Type: `" STRINGP "`"
                "\n- Owner: `" STRINGP "`",
        STRINGV(lsp_markdown_code_block(
            arena, string_format(arena, STRINGP, STRINGV(name)))),
        STRINGV(type),
        STRINGV(owner));
}

//------------------------------------------------------------------------------
// Return a hover summary for one plex/union field access.

internal string lsp_field_hover_text(const LspDocument* doc,
                                     Arena*             arena,
                                     u32                field_node_index)
{
    if (field_node_index >= array_count(doc->front_end.ast.nodes)) {
        return s("");
    }

    const AstNode* field = &doc->front_end.ast.nodes[field_node_index];
    if (field->kind != AK_Field ||
        field->a >= array_count(doc->front_end.sema.node_type_indices)) {
        return lsp_ast_field_hover_text(doc, arena, field_node_index);
    }

    u32 target_type = doc->front_end.sema.node_type_indices[field->a];
    if (target_type == sema_no_type() ||
        target_type >= array_count(doc->front_end.sema.types)) {
        return lsp_ast_field_hover_text(doc, arena, field_node_index);
    }

    const SemaType* target = &doc->front_end.sema.types[target_type];
    if (target->kind == STK_Pointer) {
        u32 pointee_type = target->first_param_type;
        if (pointee_type < array_count(doc->front_end.sema.types) &&
            (doc->front_end.sema.types[pointee_type].kind == STK_Plex ||
             doc->front_end.sema.types[pointee_type].kind == STK_Union ||
             doc->front_end.sema.types[pointee_type].kind ==
                 STK_DynamicArray)) {
            target_type = pointee_type;
            target      = &doc->front_end.sema.types[target_type];
        }
    }

    if (target->kind == STK_Slice || target->kind == STK_String ||
        target->kind == STK_DynamicArray) {
        string name = lex_symbol(&doc->front_end.lexer, field->b);
        string type = sema_type_name(
            &doc->front_end.lexer,
            &doc->front_end.sema,
            arena,
            doc->front_end.sema.node_type_indices[field_node_index]);
        string owner = sema_type_name(
            &doc->front_end.lexer, &doc->front_end.sema, arena, target_type);
        string kind       = s("");
        bool   recognised = false;

        if (target->kind == STK_String || target->kind == STK_Slice) {
            if (string_eq(name, s("data"))) {
                kind       = s("slice field");
                recognised = true;
            } else if (string_eq(name, s("count"))) {
                kind       = s("slice field");
                recognised = true;
            }
        } else if (target->kind == STK_DynamicArray) {
            if (string_eq(name, s("data")) || string_eq(name, s("count")) ||
                string_eq(name, s("capacity"))) {
                kind       = s("dynamic array field");
                recognised = true;
            } else if (string_eq(name, s("push")) ||
                       string_eq(name, s("append")) ||
                       string_eq(name, s("reserve")) ||
                       string_eq(name, s("resize")) ||
                       string_eq(name, s("resize_undefined")) ||
                       string_eq(name, s("delete")) ||
                       string_eq(name, s("swap_delete")) ||
                       string_eq(name, s("pop")) ||
                       string_eq(name, s("clear")) ||
                       string_eq(name, s("free"))) {
                kind       = s("dynamic array method");
                recognised = true;
            }
        }

        if (recognised) {
            return string_format(
                arena,
                STRINGP "\n\n- Kind: " STRINGP "\n- Type: `" STRINGP "`"
                        "\n- Owner: `" STRINGP "`",
                STRINGV(lsp_markdown_code_block(
                    arena, string_format(arena, STRINGP, STRINGV(name)))),
                STRINGV(kind),
                STRINGV(type),
                STRINGV(owner));
        }
    }

    if (target->kind != STK_Plex && target->kind != STK_Union) {
        return s("");
    }

    for (u32 i = 0; i < target->param_count; ++i) {
        if (doc->front_end.sema
                .type_param_symbols[target->first_param_type + i] != field->b) {
            continue;
        }

        string name = lex_symbol(&doc->front_end.lexer, field->b);
        string type = sema_type_name(
            &doc->front_end.lexer,
            &doc->front_end.sema,
            arena,
            doc->front_end.sema.type_param_types[target->first_param_type + i]);
        string owner = sema_type_name(
            &doc->front_end.lexer, &doc->front_end.sema, arena, target_type);
        string kind =
            target->kind == STK_Union ? s("union field") : s("plex field");

        return string_format(
            arena,
            STRINGP "\n\n- Kind: " STRINGP "\n- Type: `" STRINGP "`"
                    "\n- Owner: `" STRINGP "`",
            STRINGV(lsp_markdown_code_block(
                arena, string_format(arena, STRINGP, STRINGV(name)))),
            STRINGV(kind),
            STRINGV(type),
            STRINGV(owner));
    }

    return s("");
}

//------------------------------------------------------------------------------
// Build a location object for one top-level declaration binding.

internal JsonValue* lsp_decl_location(const LspDocument* doc,
                                      Arena*             arena,
                                      string             uri,
                                      u32                decl_index)
{
    const SemaDecl* decl = &doc->front_end.sema.decls[decl_index];
    if (decl->import_module_index != sema_no_decl() &&
        decl->import_decl_index != sema_no_decl() &&
        decl->import_module_index < array_count(doc->program.modules)) {
        return lsp_module_decl_location(
            &doc->program.modules[decl->import_module_index],
            arena,
            decl->import_decl_index);
    }

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
        lsp_make_document_range(doc, arena, start_offset, end_offset));
    return location;
}

internal JsonValue* lsp_local_location(const LspDocument* doc,
                                       Arena*             arena,
                                       string             uri,
                                       u32                local_index)
{
    const SemaLocal* local = &doc->front_end.sema.locals[local_index];
    usize            start_offset;
    usize            end_offset;
    if (local->decl_token_index != U32_MAX) {
        lsp_token_offsets(&doc->front_end.lexer,
                          local->decl_token_index,
                          &start_offset,
                          &end_offset);
    } else {
        const AstNode* bind = &doc->front_end.ast.nodes[local->decl_node_index];
        lsp_token_offsets(&doc->front_end.lexer,
                          bind->token_index,
                          &start_offset,
                          &end_offset);
    }

    JsonValue* location = json_new_object(arena);
    json_object_set_string(location, arena, "uri", uri);
    json_object_set_object(
        location,
        "range",
        lsp_make_document_range(doc, arena, start_offset, end_offset));
    return location;
}

//------------------------------------------------------------------------------
// Build a location object for one field inside a local plex/union type node.

internal JsonValue* lsp_local_record_field_location(const LspDocument* doc,
                                                    Arena*             arena,
                                                    string             uri,
                                                    u32 type_index,
                                                    u32 field_symbol)
{
    if (type_index >= array_count(doc->front_end.sema.types)) {
        return NULL;
    }

    const SemaType* target_type = &doc->front_end.sema.types[type_index];

    for (u32 i = 0; i < array_count(doc->front_end.ast.nodes); ++i) {
        const AstNode* node = &doc->front_end.ast.nodes[i];
        if (node->kind != AK_TypePlex ||
            i >= array_count(doc->front_end.sema.node_type_indices)) {
            continue;
        }

        u32 candidate_type_index = doc->front_end.sema.node_type_indices[i];
        if (candidate_type_index == sema_no_type() ||
            candidate_type_index >= array_count(doc->front_end.sema.types)) {
            continue;
        }

        const SemaType* candidate_type =
            &doc->front_end.sema.types[candidate_type_index];
        if (candidate_type->kind != target_type->kind ||
            candidate_type->param_count != target_type->param_count) {
            continue;
        }

        bool matches = true;
        for (u32 j = 0; j < target_type->param_count; ++j) {
            if (doc->front_end.sema
                        .type_param_symbols[candidate_type->first_param_type +
                                            j] !=
                    doc->front_end.sema
                        .type_param_symbols[target_type->first_param_type +
                                            j] ||
                doc->front_end.sema
                        .type_param_types[candidate_type->first_param_type +
                                          j] !=
                    doc->front_end.sema
                        .type_param_types[target_type->first_param_type + j]) {
                matches = false;
                break;
            }
        }
        if (!matches) {
            continue;
        }

        const AstPlexTypeInfo* plex = &doc->front_end.ast.plex_types[node->a];
        for (u32 j = 0; j < plex->field_count; ++j) {
            const AstPlexField* field =
                &doc->front_end.ast.plex_fields[plex->first_field + j];
            if (field->symbol_handle != field_symbol) {
                continue;
            }

            usize start_offset;
            usize end_offset;
            lsp_token_offsets(&doc->front_end.lexer,
                              field->token_index,
                              &start_offset,
                              &end_offset);

            JsonValue* location = json_new_object(arena);
            json_object_set_string(location, arena, "uri", uri);
            json_object_set_object(
                location,
                "range",
                lsp_make_document_range(doc, arena, start_offset, end_offset));
            return location;
        }
    }

    return NULL;
}

//------------------------------------------------------------------------------
// Build a location object for one field referenced from a local type node.

internal JsonValue* lsp_field_location_from_type_node(const LspDocument* doc,
                                                      Arena*             arena,
                                                      string             uri,
                                                      u32 type_node_index,
                                                      u32 field_symbol)
{
    if (type_node_index >= array_count(doc->front_end.ast.nodes)) {
        return NULL;
    }

    const AstNode* type_node = &doc->front_end.ast.nodes[type_node_index];
    if (type_node->kind == AK_TypePlex) {
        const AstPlexTypeInfo* plex =
            &doc->front_end.ast.plex_types[type_node->a];
        for (u32 i = 0; i < plex->field_count; ++i) {
            const AstPlexField* field =
                &doc->front_end.ast.plex_fields[plex->first_field + i];
            if (field->symbol_handle != field_symbol) {
                continue;
            }

            usize start_offset;
            usize end_offset;
            lsp_token_offsets(&doc->front_end.lexer,
                              field->token_index,
                              &start_offset,
                              &end_offset);

            JsonValue* location = json_new_object(arena);
            json_object_set_string(location, arena, "uri", uri);
            json_object_set_object(
                location,
                "range",
                lsp_make_document_range(doc, arena, start_offset, end_offset));
            return location;
        }
        return NULL;
    }

    if (type_node->kind == AK_SymbolRef &&
        type_node_index < array_count(doc->front_end.sema.node_decl_indices)) {
        u32 decl_index = doc->front_end.sema.node_decl_indices[type_node_index];
        if (decl_index != sema_no_decl() &&
            decl_index < array_count(doc->front_end.sema.decls)) {
            const SemaDecl* decl = &doc->front_end.sema.decls[decl_index];
            if (decl->kind == SK_TypeAlias &&
                decl->bind_node_index < array_count(doc->front_end.ast.nodes)) {
                const AstNode* bind =
                    &doc->front_end.ast.nodes[decl->bind_node_index];
                return lsp_field_location_from_type_node(
                    doc, arena, uri, bind->b, field_symbol);
            }
        }
    }

    return NULL;
}

//------------------------------------------------------------------------------
// Return a field location when the current file has exactly one matching
// plex/union field symbol.

internal JsonValue* lsp_unique_record_field_location(const LspDocument* doc,
                                                     Arena*             arena,
                                                     string             uri,
                                                     u32 field_symbol)
{
    u32   matched_token_index = U32_MAX;
    usize match_count         = 0;

    for (u32 i = 0; i < array_count(doc->front_end.ast.plex_fields); ++i) {
        const AstPlexField* field = &doc->front_end.ast.plex_fields[i];
        if (field->symbol_handle != field_symbol) {
            continue;
        }
        matched_token_index = field->token_index;
        match_count += 1;
        if (match_count > 1) {
            return NULL;
        }
    }

    if (match_count != 1) {
        return NULL;
    }

    usize start_offset;
    usize end_offset;
    lsp_token_offsets(
        &doc->front_end.lexer, matched_token_index, &start_offset, &end_offset);

    JsonValue* location = json_new_object(arena);
    json_object_set_string(location, arena, "uri", uri);
    json_object_set_object(
        location,
        "range",
        lsp_make_document_range(doc, arena, start_offset, end_offset));
    return location;
}

internal JsonValue* lsp_ast_field_location(const LspDocument* doc,
                                           Arena*             arena,
                                           string             uri,
                                           u32                field_node_index)
{
    if (field_node_index >= array_count(doc->front_end.ast.nodes)) {
        return NULL;
    }

    const AstNode* field = &doc->front_end.ast.nodes[field_node_index];
    if (field->kind != AK_Field) {
        return NULL;
    }

    u32 type_symbol =
        lsp_ast_receiver_type_symbol(doc, field->a, field->token_index);
    if (type_symbol == U32_MAX) {
        return NULL;
    }

    const AstPlexField* plex_field =
        lsp_ast_find_field_for_type_symbol(doc, type_symbol, field->b);
    if (plex_field == NULL) {
        return NULL;
    }

    usize start_offset;
    usize end_offset;
    lsp_token_offsets(&doc->front_end.lexer,
                      plex_field->token_index,
                      &start_offset,
                      &end_offset);

    JsonValue* location = json_new_object(arena);
    json_object_set_string(location, arena, "uri", uri);
    json_object_set_object(
        location,
        "range",
        lsp_make_document_range(doc, arena, start_offset, end_offset));
    return location;
}

//------------------------------------------------------------------------------
// Build a location object for one plex/union field-access token.

internal JsonValue* lsp_field_location(const LspDocument* doc,
                                       Arena*             arena,
                                       string             uri,
                                       u32                field_node_index)
{
    if (field_node_index >= array_count(doc->front_end.ast.nodes)) {
        return NULL;
    }

    const AstNode* field = &doc->front_end.ast.nodes[field_node_index];
    if (field->kind != AK_Field ||
        field->a >= array_count(doc->front_end.sema.node_type_indices)) {
        return lsp_ast_field_location(doc, arena, uri, field_node_index);
    }

    u32 target_type = doc->front_end.sema.node_type_indices[field->a];
    if (target_type == sema_no_type() ||
        target_type >= array_count(doc->front_end.sema.types)) {
        return lsp_ast_field_location(doc, arena, uri, field_node_index);
    }

    const SemaType* target = &doc->front_end.sema.types[target_type];
    if (target->kind == STK_Pointer) {
        u32 pointee_type = target->first_param_type;
        if (pointee_type < array_count(doc->front_end.sema.types) &&
            (doc->front_end.sema.types[pointee_type].kind == STK_Plex ||
             doc->front_end.sema.types[pointee_type].kind == STK_Union)) {
            target_type = pointee_type;
            target      = &doc->front_end.sema.types[target_type];
        }
    }

    if (target->kind != STK_Plex && target->kind != STK_Union) {
        return NULL;
    }

    if (field->a < array_count(doc->front_end.ast.nodes) &&
        field->a < array_count(doc->front_end.sema.node_local_indices)) {
        u32 local_index = doc->front_end.sema.node_local_indices[field->a];
        if (local_index != sema_no_local() &&
            local_index < array_count(doc->front_end.sema.locals)) {
            const SemaLocal* local = &doc->front_end.sema.locals[local_index];
            if (local->decl_node_index <
                array_count(doc->front_end.ast.nodes)) {
                const AstNode* bind =
                    &doc->front_end.ast.nodes[local->decl_node_index];
                u32 value_node_index = bind->b;
                if (value_node_index < array_count(doc->front_end.ast.nodes)) {
                    const AstNode* value =
                        &doc->front_end.ast.nodes[value_node_index];
                    if (value->kind == AK_AnnotatedValue ||
                        value->kind == AK_ZeroInit ||
                        value->kind == AK_Undefined) {
                        JsonValue* location = lsp_field_location_from_type_node(
                            doc, arena, uri, value->a, field->b);
                        if (location != NULL) {
                            return location;
                        }
                    } else if (value->kind == AK_Plex ||
                               value->kind == AK_PlexUpdate) {
                        const AstPlexLiteralInfo* literal =
                            &doc->front_end.ast.plex_literals[value->a];
                        if (literal->target_node_index != U32_MAX) {
                            JsonValue* location =
                                lsp_field_location_from_type_node(
                                    doc,
                                    arena,
                                    uri,
                                    literal->target_node_index,
                                    field->b);
                            if (location != NULL) {
                                return location;
                            }
                        }
                    }
                }
            }
        }
    }

    JsonValue* location =
        lsp_local_record_field_location(doc, arena, uri, target_type, field->b);
    if (location != NULL) {
        return location;
    }

    JsonValue* ast_location =
        lsp_ast_field_location(doc, arena, uri, field_node_index);
    if (ast_location != NULL) {
        return ast_location;
    }

    return lsp_unique_record_field_location(doc, arena, uri, field->b);
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
    if (!(*out_doc)->sema_partial) {
        return false;
    }

    u32 line            = (u32)json_integer(line_value);
    u32 col             = (u32)json_integer(col_value);

    usize visible_start = 0;
    if (lsp_document_visible_start(*out_doc, &visible_start) &&
        !string_eq((*out_doc)->front_end.lexer.source.source,
                   (*out_doc)->source)) {
        NerdSource visible_source = {
            .source      = (*out_doc)->source,
            .source_path = (*out_doc)->front_end.lexer.source.source_path,
        };
        usize visible_offset = 0;
        if (!lex_line_col_to_offset(
                visible_source, line, col, &visible_offset)) {
            return false;
        }
        *out_offset = visible_start + visible_offset;
    } else if (!lex_line_col_to_offset(
                   (*out_doc)->front_end.lexer.source, line, col, out_offset)) {
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
    return decl->kind == SK_Function || decl->kind == SK_GenericFunction ||
                   decl->kind == SK_FfiFunction
               ? LSP_SYMBOL_KIND_FUNCTION
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
            if (decl_index != LSP_NO_DECL) {
                lsp_set_markdown_hover(
                    response,
                    message->arena,
                    lsp_decl_hover_text(doc, message->arena, decl_index));
                break;
            }

            u32 field_node_index =
                lsp_find_field_node_at_token(&doc->front_end.ast, token_index);
            if (field_node_index != U32_MAX) {
                string field_hover =
                    lsp_field_hover_text(doc, message->arena, field_node_index);
                if (field_hover.count != 0) {
                    lsp_set_markdown_hover(
                        response, message->arena, field_hover);
                    break;
                }
            }

            lsp_cancel(response, message->arena);
            return;
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

    u32 field_node_index =
        lsp_find_field_node_at_token(&doc->front_end.ast, token_index);
    if (field_node_index != U32_MAX) {
        JsonValue* field_location =
            lsp_field_location(doc, message->arena, uri, field_node_index);
        if (field_location != NULL) {
            json_object_set_object(response, "result", field_location);
            lsp_send_response(message->arena, response);
            return;
        }
    }

    u32 modref_node_index = lsp_find_modref_node_at_token(
        &doc->front_end.lexer, &doc->front_end.ast, token_index);
    if (modref_node_index != U32_MAX) {
        if (modref_node_index <
            array_count(doc->front_end.sema.node_type_indices)) {
            u32 module_type =
                doc->front_end.sema.node_type_indices[modref_node_index];
            if (module_type != sema_no_type() &&
                module_type < array_count(doc->front_end.sema.types)) {
                const SemaType* type = &doc->front_end.sema.types[module_type];
                if (type->kind == STK_Module && doc->program.modules != NULL &&
                    type->return_type < array_count(doc->program.modules)) {
                    JsonValue* location = lsp_module_file_location(
                        &doc->program.modules[type->return_type],
                        message->arena);
                    json_object_set_object(response, "result", location);
                    lsp_send_response(message->arena, response);
                    return;
                }
            }
        }

        const AstNode* modref = &doc->front_end.ast.nodes[modref_node_index];
        if (modref->a < array_count(doc->front_end.ast.module_paths)) {
            Arena temp_arena = {0};
            arena_init(&temp_arena);
            ModuleResolveResult resolved = {0};
            ModuleResolveStatus status =
                module_resolve_path(&temp_arena,
                                    doc->front_end.lexer.source,
                                    &doc->front_end.lexer,
                                    &doc->front_end.ast,
                                    &doc->front_end.ast.module_paths[modref->a],
                                    &resolved);
            if (status == MRS_Found) {
                u32 module_index = lsp_find_program_module_by_path(
                    &doc->program, resolved.resolved_path);
                if (module_index != U32_MAX) {
                    JsonValue* location = lsp_module_file_location(
                        &doc->program.modules[module_index], message->arena);
                    arena_done(&temp_arena);
                    json_object_set_object(response, "result", location);
                    lsp_send_response(message->arena, response);
                    return;
                }
            }
            arena_done(&temp_arena);
        }
    }

    u32 local_index = lsp_find_local_index_for_token(doc, token_index);
    if (local_index != sema_no_local()) {
        const SemaLocal* local    = &doc->front_end.sema.locals[local_index];
        JsonValue*       location = NULL;
        if (local->decl_node_index < array_count(doc->front_end.ast.nodes)) {
            const AstNode* decl_node =
                &doc->front_end.ast.nodes[local->decl_node_index];
            if (decl_node->kind == AK_Use &&
                decl_node->a <
                    array_count(doc->front_end.sema.node_type_indices)) {
                location = lsp_imported_symbol_location(
                    doc,
                    message->arena,
                    doc->front_end.sema.node_type_indices[decl_node->a],
                    local->symbol_handle);
            }
        }
        if (location == NULL) {
            location =
                lsp_local_location(doc, message->arena, uri, local_index);
        }
        json_object_set_object(response, "result", location);
        lsp_send_response(message->arena, response);
        return;
    }

    u32 decl_index = lsp_find_decl_index_for_token(doc, token_index);
    if (decl_index == LSP_NO_DECL) {
        JsonValue* ast_location =
            lsp_ast_definition_location(doc, message->arena, uri, token_index);
        if (ast_location != NULL) {
            json_object_set_object(response, "result", ast_location);
            lsp_send_response(message->arena, response);
            return;
        }
        lsp_cancel(response, message->arena);
        return;
    }

    JsonValue* location =
        lsp_decl_location(doc, message->arena, uri, decl_index);
    if (!location) {
        JsonValue* ast_location =
            lsp_ast_definition_location(doc, message->arena, uri, token_index);
        if (ast_location != NULL) {
            json_object_set_object(response, "result", ast_location);
            lsp_send_response(message->arena, response);
            return;
        }
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
    if (!doc || !doc->sema_partial) {
        json_object_set_array(
            response, "result", json_new_array(message->arena));
        lsp_send_response(message->arena, response);
        return;
    }

    JsonValue* result = json_new_array(message->arena);

    if (doc->cst_ready) {
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
            json_object_set_object(
                symbol,
                "range",
                lsp_make_document_range(
                    doc, message->arena, start_offset, end_offset));
            json_object_set_object(
                symbol,
                "selectionRange",
                lsp_make_document_range(
                    doc, message->arena, start_offset, end_offset));

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
