//------------------------------------------------------------------------------
// LSP code actions
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <lsp/lsp.h>

#include <compiler/modules/modules.h>

//------------------------------------------------------------------------------

internal JsonValue*
lsp_code_action_position(Arena* arena, NerdSource source, usize offset)
{
    u32 line = 0;
    u32 col  = 0;
    if (!lex_offset_to_line_col(source, offset, &line, &col)) {
        return NULL;
    }

    JsonValue* position = json_new_object(arena);
    json_object_set_number(position, arena, "line", line);
    json_object_set_number(position, arena, "character", col);
    return position;
}

internal cstr lsp_code_action_cstr(Arena* arena, string value)
{
    char* data = arena_alloc(arena, value.count + 1);
    memcpy(data, value.data, value.count);
    data[value.count] = '\0';
    return data;
}

internal JsonValue*
lsp_code_action_range(Arena* arena, NerdSource source, usize start, usize end)
{
    JsonValue* start_position = lsp_code_action_position(arena, source, start);
    JsonValue* end_position   = lsp_code_action_position(arena, source, end);
    if (start_position == NULL || end_position == NULL) {
        return NULL;
    }

    JsonValue* range = json_new_object(arena);
    json_object_set_object(range, "start", start_position);
    json_object_set_object(range, "end", end_position);
    return range;
}

internal usize lsp_code_action_line_start(string source, usize offset)
{
    if (offset > source.count) {
        offset = source.count;
    }
    while (offset > 0 && source.data[offset - 1] != '\n') {
        offset--;
    }
    return offset;
}

internal string lsp_code_action_line_indent(Arena* arena,
                                            string source,
                                            usize  offset)
{
    usize start = lsp_code_action_line_start(source, offset);
    usize end   = start;
    while (end < source.count &&
           (source.data[end] == ' ' || source.data[end] == '\t')) {
        end++;
    }

    u8* data = arena_alloc(arena, end - start);
    memcpy(data, source.data + start, end - start);
    return (string){.data = data, .count = end - start};
}

internal void lsp_code_action_append_spaces(StringBuilder* sb, usize count)
{
    for (usize i = 0; i < count; ++i) {
        sb_append_char(sb, ' ');
    }
}

internal bool lsp_code_action_token_is_open(TokenKind kind)
{
    return kind == TK_LBrace || kind == TK_LParen || kind == TK_LBracket;
}

internal bool lsp_code_action_token_is_close(TokenKind kind)
{
    return kind == TK_RBrace || kind == TK_RParen || kind == TK_RBracket;
}

internal bool lsp_code_action_matching_close(const Lexer* lexer,
                                             u32          open_token_index,
                                             u32*         out_close_token_index)
{
    if (open_token_index >= array_count(lexer->tokens) ||
        lexer->tokens[open_token_index].kind != TK_LBrace) {
        return false;
    }

    u32 depth = 0;
    for (u32 i = open_token_index; i < array_count(lexer->tokens); ++i) {
        TokenKind kind = lexer->tokens[i].kind;
        if (lsp_code_action_token_is_open(kind)) {
            depth++;
        } else if (lsp_code_action_token_is_close(kind)) {
            if (depth == 0) {
                return false;
            }
            depth--;
            if (depth == 0) {
                *out_close_token_index = i;
                return kind == TK_RBrace;
            }
        }
    }
    return false;
}

internal u32 lsp_code_action_find_decl(const Sema* sema, u32 symbol)
{
    for (u32 i = 0; i < array_count(sema->decls); ++i) {
        if (sema->decls[i].symbol_handle == symbol) {
            return i;
        }
    }
    return sema_no_decl();
}

internal bool lsp_code_action_resolve_type_node(const Ast*  ast,
                                                const Sema* sema,
                                                u32         node_index,
                                                u32*        out_type)
{
    if (node_index >= array_count(ast->nodes)) {
        return false;
    }

    const AstNode* node = &ast->nodes[node_index];
    switch (node->kind) {
    case AK_SymbolRef:
        {
            u32 decl_index = sema_no_decl();
            if (!lsp_sema_node_decl(sema, node_index, &decl_index)) {
                decl_index = lsp_code_action_find_decl(sema, node->a);
            }
            const SemaDecl* decl = NULL;
            if (lsp_sema_decl(sema, decl_index, &decl)) {
                *out_type = decl->type_index;
                return *out_type != sema_no_type();
            }
            for (u32 i = 0; i < array_count(ast->nodes); ++i) {
                const AstNode* candidate = &ast->nodes[i];
                if ((candidate->kind == AK_Bind ||
                     candidate->kind == AK_Variable) &&
                    ast_get_symbol(candidate) == node->a &&
                    lsp_sema_node_type(sema, i, out_type)) {
                    return true;
                }
            }
            return false;
        }
    case AK_Field:
        {
            u32 target_type = sema_no_type();
            if (!lsp_code_action_resolve_type_node(
                    ast, sema, node->a, &target_type)) {
                return false;
            }
            const SemaType* module = NULL;
            if (!lsp_sema_type(sema, target_type, &module) ||
                module->kind != STK_Module) {
                return false;
            }
            for (u32 i = 0; i < module->param_count; ++i) {
                if (sema->type_param_symbols[module->first_param_type + i] ==
                    node->b) {
                    *out_type =
                        sema->type_param_types[module->first_param_type + i];
                    return *out_type != sema_no_type();
                }
            }
            return false;
        }
    default:
        return false;
    }
}

internal bool lsp_code_action_resolve_plex_literal_type(const Ast*  ast,
                                                        const Sema* sema,
                                                        u32         node_index,
                                                        u32*        out_type)
{
    if (node_index >= array_count(ast->nodes)) {
        return false;
    }

    const AstNode* node = &ast->nodes[node_index];
    if (node->kind != AK_Plex) {
        return false;
    }

    const AstPlexLiteralInfo* literal = &ast->plex_literals[node->a];
    if (literal->target_node_index != U32_MAX) {
        return lsp_code_action_resolve_type_node(
            ast, sema, literal->target_node_index, out_type);
    }

    if (lsp_sema_node_type(sema, node_index, out_type)) {
        return true;
    }

    return false;
}

internal u32 lsp_code_action_unwrap_type_value_node(const Ast* ast,
                                                    u32        node_index)
{
    while (node_index < array_count(ast->nodes)) {
        const AstNode* node = &ast->nodes[node_index];
        if (node->kind == AK_AnnotatedValue || node->kind == AK_ZeroInit ||
            node->kind == AK_Undefined) {
            node_index = node->kind == AK_AnnotatedValue ? node->b : node->a;
            continue;
        }
        break;
    }
    return node_index;
}

internal u32 lsp_code_action_find_ast_type_alias(const Ast* ast, u32 symbol)
{
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind != AK_Bind || node->a != symbol ||
            node->b >= array_count(ast->nodes)) {
            continue;
        }

        u32 value_node_index =
            lsp_code_action_unwrap_type_value_node(ast, node->b);
        if (value_node_index < array_count(ast->nodes)) {
            return value_node_index;
        }
    }
    return U32_MAX;
}

internal bool lsp_code_action_ast_default_value(Arena*       arena,
                                                const Ast*   ast,
                                                const Lexer* lexer,
                                                u32          type_node_index,
                                                string*      out_value);

internal bool lsp_code_action_ast_default_record(Arena*       arena,
                                                 const Ast*   ast,
                                                 const Lexer* lexer,
                                                 u32          plex_index,
                                                 string*      out_value)
{
    if (plex_index >= array_count(ast->plex_types)) {
        return false;
    }

    const AstPlexTypeInfo* plex = &ast->plex_types[plex_index];
    StringBuilder          sb   = {0};
    sb_init(&sb, arena);
    sb_append_cstr(&sb, "{ ");
    for (u32 i = 0; i < plex->field_count; ++i) {
        const AstPlexField* field = &ast->plex_fields[plex->first_field + i];
        string              value = {0};
        if (!lsp_code_action_ast_default_value(
                arena, ast, lexer, field->type_node_index, &value)) {
            return false;
        }
        if (i > 0) {
            sb_append_cstr(&sb, " ");
        }
        sb_append_string(&sb, lex_symbol(lexer, field->symbol_handle));
        sb_append_cstr(&sb, ": ");
        sb_append_string(&sb, value);
    }
    sb_append_cstr(&sb, " }");
    *out_value = sb_to_string(&sb);
    return true;
}

internal bool lsp_code_action_ast_default_enum(const Ast*   ast,
                                               const Lexer* lexer,
                                               u32          enum_index,
                                               string*      out_value)
{
    if (enum_index >= array_count(ast->enum_types)) {
        return false;
    }

    const AstEnumTypeInfo* enum_type = &ast->enum_types[enum_index];
    if (enum_type->variant_count == 0) {
        return false;
    }

    const AstEnumVariant* variant =
        &ast->enum_variants[enum_type->first_variant];
    if (variant->type_node_index != U32_MAX) {
        return false;
    }

    *out_value = lex_symbol(lexer, variant->symbol_handle);
    return true;
}

internal bool lsp_code_action_ast_default_value(Arena*       arena,
                                                const Ast*   ast,
                                                const Lexer* lexer,
                                                u32          type_node_index,
                                                string*      out_value)
{
    if (type_node_index >= array_count(ast->nodes)) {
        return false;
    }

    const AstNode* type_node = &ast->nodes[type_node_index];
    switch (type_node->kind) {
    case AK_SymbolRef:
        {
            string name = lex_symbol(lexer, type_node->a);
            if (string_eq_cstr(name, "bool")) {
                *out_value = s("no");
                return true;
            }
            if (string_eq_cstr(name, "string")) {
                *out_value = s("\"\"");
                return true;
            }
            if (string_eq_cstr(name, "i8") || string_eq_cstr(name, "i16") ||
                string_eq_cstr(name, "i32") || string_eq_cstr(name, "i64") ||
                string_eq_cstr(name, "u8") || string_eq_cstr(name, "u16") ||
                string_eq_cstr(name, "u32") || string_eq_cstr(name, "u64") ||
                string_eq_cstr(name, "isize") ||
                string_eq_cstr(name, "usize") || string_eq_cstr(name, "f32") ||
                string_eq_cstr(name, "f64")) {
                *out_value = s("0");
                return true;
            }

            u32 alias_node =
                lsp_code_action_find_ast_type_alias(ast, type_node->a);
            if (alias_node == U32_MAX) {
                return false;
            }

            const AstNode* alias = &ast->nodes[alias_node];
            if (alias->kind == AK_TypePlex) {
                return lsp_code_action_ast_default_record(
                    arena, ast, lexer, alias->a, out_value);
            }
            if (alias->kind == AK_TypeEnum) {
                return lsp_code_action_ast_default_enum(
                    ast, lexer, alias->a, out_value);
            }
            return false;
        }
    case AK_TypePointer:
    case AK_TypeSlice:
    case AK_TypeDynamicArray:
        *out_value = s("nil");
        return true;
    case AK_TypeTuple:
        {
            StringBuilder sb = {0};
            sb_init(&sb, arena);
            sb_append_char(&sb, '(');
            for (u32 i = 0; i < type_node->b; ++i) {
                string item = {0};
                if (!lsp_code_action_ast_default_value(
                        arena, ast, lexer, type_node->a + i, &item)) {
                    return false;
                }
                if (i > 0) {
                    sb_append_cstr(&sb, ", ");
                }
                sb_append_string(&sb, item);
            }
            sb_append_char(&sb, ')');
            *out_value = sb_to_string(&sb);
            return true;
        }
    case AK_TypePlex:
        return lsp_code_action_ast_default_record(
            arena, ast, lexer, type_node->a, out_value);
    case AK_TypeEnum:
        return lsp_code_action_ast_default_enum(
            ast, lexer, type_node->a, out_value);
    default:
        return false;
    }
}

internal bool lsp_code_action_default_value(Arena*       arena,
                                            const Lexer* lexer,
                                            const Sema*  sema,
                                            u32          type_index,
                                            string*      out_value)
{
    if (type_index == sema_no_type() ||
        type_index >= array_count(sema->types)) {
        return false;
    }

    type_index           = sema_materialise_type(sema, type_index);
    const SemaType* type = &sema->types[type_index];
    switch (type->kind) {
    case STK_Bool:
        *out_value = s("no");
        return true;
    case STK_String:
        *out_value = s("\"\"");
        return true;
    case STK_I8:
    case STK_I16:
    case STK_I32:
    case STK_I64:
    case STK_U8:
    case STK_U16:
    case STK_U32:
    case STK_U64:
    case STK_Isize:
    case STK_Usize:
    case STK_F32:
    case STK_F64:
        *out_value = s("0");
        return true;
    case STK_Pointer:
    case STK_Slice:
    case STK_DynamicArray:
        *out_value = s("nil");
        return true;
    case STK_Tuple:
        {
            StringBuilder sb = {0};
            sb_init(&sb, arena);
            sb_append_char(&sb, '(');
            for (u32 i = 0; i < type->param_count; ++i) {
                if (i > 0) {
                    sb_append_cstr(&sb, ", ");
                }
                string item = {0};
                if (!lsp_code_action_default_value(
                        arena,
                        lexer,
                        sema,
                        sema->type_param_types[type->first_param_type + i],
                        &item)) {
                    return false;
                }
                sb_append_string(&sb, item);
            }
            sb_append_char(&sb, ')');
            *out_value = sb_to_string(&sb);
            return true;
        }
    case STK_Plex:
        {
            StringBuilder sb = {0};
            sb_init(&sb, arena);
            sb_append_cstr(&sb, "{ ");
            for (u32 i = 0; i < type->param_count; ++i) {
                if (i > 0) {
                    sb_append_cstr(&sb, " ");
                }
                string field_value = {0};
                if (!lsp_code_action_default_value(
                        arena,
                        lexer,
                        sema,
                        sema->type_param_types[type->first_param_type + i],
                        &field_value)) {
                    return false;
                }
                sb_append_string(
                    &sb,
                    lex_symbol(
                        lexer,
                        sema->type_param_symbols[type->first_param_type + i]));
                sb_append_cstr(&sb, ": ");
                sb_append_string(&sb, field_value);
            }
            sb_append_cstr(&sb, " }");
            *out_value = sb_to_string(&sb);
            return true;
        }
    case STK_Enum:
        if (type->param_count > 0 &&
            sema->type_param_types[type->first_param_type] == sema_no_type()) {
            *out_value = lex_symbol(
                lexer, sema->type_param_symbols[type->first_param_type]);
            return true;
        }
        return false;
    default:
        return false;
    }
}

internal u32 lsp_code_action_find_plex_literal_at_offset(const LspDocument* doc,
                                                         usize offset,
                                                         u32*  out_close_token)
{
    const Ast*   ast   = &doc->front_end.ast;
    const Lexer* lexer = &doc->front_end.lexer;

    u32   best_node    = U32_MAX;
    usize best_width   = (usize)-1;
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind != AK_Plex) {
            continue;
        }
        u32 close_token = U32_MAX;
        if (!lsp_code_action_matching_close(
                lexer, node->token_index, &close_token)) {
            continue;
        }
        const AstPlexLiteralInfo* literal     = &ast->plex_literals[node->a];
        u32                       start_token = node->token_index;
        u32 target_node_index                 = literal->target_node_index;
        if (target_node_index < array_count(ast->nodes)) {
            start_token = ast->nodes[target_node_index].token_index;
            while (ast->nodes[target_node_index].kind == AK_Field) {
                u32 receiver_index = ast->nodes[target_node_index].a;
                if (receiver_index >= array_count(ast->nodes)) {
                    break;
                }
                start_token       = ast->nodes[receiver_index].token_index;
                target_node_index = receiver_index;
            }
        }
        usize start = lexer->tokens[start_token].offset;
        usize end   = lex_token_end_offset(lexer, &lexer->tokens[close_token]);
        if (offset < start || offset > end) {
            continue;
        }
        usize width = end - start;
        if (width < best_width) {
            best_node        = i;
            best_width       = width;
            *out_close_token = close_token;
        }
    }
    return best_node;
}

internal bool lsp_code_action_missing_plex_fields(Arena*             arena,
                                                  const LspDocument* doc,
                                                  u32                node_index,
                                                  string* out_insert_text)
{
    const Ast*                ast     = &doc->front_end.ast;
    const Lexer*              lexer   = &doc->front_end.lexer;
    const Sema*               sema    = &doc->front_end.sema;
    const AstNode*            node    = &ast->nodes[node_index];
    const AstPlexLiteralInfo* literal = &ast->plex_literals[node->a];

    u32 type_index                    = sema_no_type();
    if (!lsp_code_action_resolve_plex_literal_type(
            ast, sema, node_index, &type_index) ||
        type_index >= array_count(sema->types)) {
        return false;
    }

    type_index = sema_materialise_type(sema, type_index);
    if (sema->types[type_index].kind != STK_Plex) {
        return false;
    }

    const SemaType* plex = &sema->types[type_index];
    bool*           seen = arena_alloc(arena, sizeof(bool) * plex->param_count);
    memset(seen, 0, sizeof(bool) * plex->param_count);
    for (u32 i = 0; i < literal->field_count; ++i) {
        const AstPlexLiteralField* field =
            &ast->plex_literal_fields[literal->first_field + i];
        for (u32 j = 0; j < plex->param_count; ++j) {
            if (sema->type_param_symbols[plex->first_param_type + j] ==
                field->symbol_handle) {
                seen[j] = true;
                break;
            }
        }
    }

    string base_indent = lsp_code_action_line_indent(
        arena, doc->source, lexer->tokens[node->token_index].offset);
    StringBuilder sb = {0};
    sb_init(&sb, arena);
    usize field_name_width = 0;
    for (u32 i = 0; i < plex->param_count; ++i) {
        string name = lex_symbol(
            lexer, sema->type_param_symbols[plex->first_param_type + i]);
        if (name.count > field_name_width) {
            field_name_width = name.count;
        }
    }

    bool needs_leading_newline = literal->field_count > 0;
    u32  missing_count         = 0;
    for (u32 i = 0; i < plex->param_count; ++i) {
        if (seen[i]) {
            continue;
        }
        string value = {0};
        if (!lsp_code_action_default_value(
                arena,
                lexer,
                sema,
                sema->type_param_types[plex->first_param_type + i],
                &value)) {
            return false;
        }
        if (needs_leading_newline) {
            sb_append_char(&sb, '\n');
        }
        needs_leading_newline = true;
        sb_append_string(&sb, base_indent);
        sb_append_cstr(&sb, "    ");
        string name = lex_symbol(
            lexer, sema->type_param_symbols[plex->first_param_type + i]);
        sb_append_string(&sb, name);
        lsp_code_action_append_spaces(&sb, field_name_width - name.count);
        sb_append_cstr(&sb, ": ");
        sb_append_string(&sb, value);
        missing_count++;
    }

    if (missing_count == 0) {
        return false;
    }

    sb_append_char(&sb, '\n');
    sb_append_string(&sb, base_indent);
    *out_insert_text = sb_to_string(&sb);
    return true;
}

internal bool lsp_code_action_symbol_names_equal(const Lexer* a_lexer,
                                                 u32          a_symbol,
                                                 const Lexer* b_lexer,
                                                 u32          b_symbol)
{
    return string_eq(lex_symbol(a_lexer, a_symbol),
                     lex_symbol(b_lexer, b_symbol));
}

internal bool
lsp_code_action_missing_ast_plex_fields_from_type(Arena*             arena,
                                                  const LspDocument* doc,
                                                  u32                node_index,
                                                  const Ast*         type_ast,
                                                  const Lexer*       type_lexer,
                                                  u32     type_plex_index,
                                                  string* out_insert_text)
{
    const Ast*                ast     = &doc->front_end.ast;
    const Lexer*              lexer   = &doc->front_end.lexer;
    const AstNode*            node    = &ast->nodes[node_index];
    const AstPlexLiteralInfo* literal = &ast->plex_literals[node->a];

    if (type_plex_index >= array_count(type_ast->plex_types)) {
        return false;
    }

    const AstPlexTypeInfo* plex = &type_ast->plex_types[type_plex_index];
    bool* seen = arena_alloc(arena, sizeof(bool) * plex->field_count);
    memset(seen, 0, sizeof(bool) * plex->field_count);
    for (u32 i = 0; i < literal->field_count; ++i) {
        const AstPlexLiteralField* field =
            &ast->plex_literal_fields[literal->first_field + i];
        for (u32 j = 0; j < plex->field_count; ++j) {
            const AstPlexField* plex_field =
                &type_ast->plex_fields[plex->first_field + j];
            if (lsp_code_action_symbol_names_equal(lexer,
                                                   field->symbol_handle,
                                                   type_lexer,
                                                   plex_field->symbol_handle)) {
                seen[j] = true;
                break;
            }
        }
    }

    string base_indent = lsp_code_action_line_indent(
        arena, doc->source, lexer->tokens[node->token_index].offset);
    StringBuilder sb = {0};
    sb_init(&sb, arena);
    usize field_name_width = 0;
    for (u32 i = 0; i < plex->field_count; ++i) {
        const AstPlexField* field =
            &type_ast->plex_fields[plex->first_field + i];
        string name = lex_symbol(type_lexer, field->symbol_handle);
        if (name.count > field_name_width) {
            field_name_width = name.count;
        }
    }

    bool needs_leading_newline = literal->field_count > 0;
    u32  missing_count         = 0;
    for (u32 i = 0; i < plex->field_count; ++i) {
        if (seen[i]) {
            continue;
        }

        const AstPlexField* field =
            &type_ast->plex_fields[plex->first_field + i];
        string value = {0};
        if (!lsp_code_action_ast_default_value(
                arena, type_ast, type_lexer, field->type_node_index, &value)) {
            return false;
        }

        if (needs_leading_newline) {
            sb_append_char(&sb, '\n');
        }
        needs_leading_newline = true;
        sb_append_string(&sb, base_indent);
        sb_append_cstr(&sb, "    ");
        string name = lex_symbol(type_lexer, field->symbol_handle);
        sb_append_string(&sb, name);
        lsp_code_action_append_spaces(&sb, field_name_width - name.count);
        sb_append_cstr(&sb, ": ");
        sb_append_string(&sb, value);
        missing_count++;
    }

    if (missing_count == 0) {
        return false;
    }

    sb_append_char(&sb, '\n');
    sb_append_string(&sb, base_indent);
    *out_insert_text = sb_to_string(&sb);
    return true;
}

internal bool lsp_code_action_missing_ast_plex_fields(Arena*             arena,
                                                      const LspDocument* doc,
                                                      u32     node_index,
                                                      string* out_insert_text)
{
    const Ast*                ast     = &doc->front_end.ast;
    const AstNode*            node    = &ast->nodes[node_index];
    const AstPlexLiteralInfo* literal = &ast->plex_literals[node->a];

    if (literal->target_node_index >= array_count(ast->nodes)) {
        return false;
    }

    const AstNode* target = &ast->nodes[literal->target_node_index];
    if (target->kind != AK_SymbolRef) {
        return false;
    }

    u32 alias_node_index = lsp_code_action_find_ast_type_alias(ast, target->a);
    if (alias_node_index >= array_count(ast->nodes)) {
        return false;
    }

    const AstNode* alias = &ast->nodes[alias_node_index];
    if (alias->kind != AK_TypePlex ||
        alias->a >= array_count(ast->plex_types)) {
        return false;
    }

    return lsp_code_action_missing_ast_plex_fields_from_type(
        arena,
        doc,
        node_index,
        ast,
        &doc->front_end.lexer,
        alias->a,
        out_insert_text);
}

internal u32 lsp_code_action_ast_module_path_for_binding(const LspDocument* doc,
                                                         u32 module_symbol)
{
    const Ast* ast = &doc->front_end.ast;

    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind != AK_Bind || node->a != module_symbol ||
            node->b >= array_count(ast->nodes)) {
            continue;
        }

        const AstNode* value = &ast->nodes[node->b];
        if (value->kind == AK_Use && value->a < array_count(ast->nodes)) {
            value = &ast->nodes[value->a];
        }
        if (value->kind == AK_ModRef &&
            value->a < array_count(ast->module_paths)) {
            return value->a;
        }
    }

    return U32_MAX;
}

internal u32 lsp_code_action_find_program_module_by_path(
    const ProgramInfo* program, cstr resolved_path)
{
    for (u32 i = 0; i < array_count(program->modules); ++i) {
        const ModuleInfo* module = &program->modules[i];
        if (module->resolved_path != NULL &&
            strcmp(module->resolved_path, resolved_path) == 0) {
            return i;
        }
    }
    return U32_MAX;
}

internal bool
lsp_code_action_missing_imported_ast_plex_fields(Arena*             arena,
                                                 const LspDocument* doc,
                                                 u32                node_index,
                                                 string* out_insert_text)
{
    const Ast*                ast     = &doc->front_end.ast;
    const Lexer*              lexer   = &doc->front_end.lexer;
    const AstNode*            node    = &ast->nodes[node_index];
    const AstPlexLiteralInfo* literal = &ast->plex_literals[node->a];

    if (literal->target_node_index >= array_count(ast->nodes)) {
        return false;
    }

    const AstNode* target = &ast->nodes[literal->target_node_index];
    if (target->kind != AK_Field || target->a >= array_count(ast->nodes)) {
        return false;
    }

    const AstNode* receiver = &ast->nodes[target->a];
    if (receiver->kind != AK_SymbolRef) {
        return false;
    }

    u32 module_path_index =
        lsp_code_action_ast_module_path_for_binding(doc, receiver->a);
    if (module_path_index == U32_MAX) {
        return false;
    }

    Arena temp = {0};
    arena_init(&temp);
    NerdSource root_source = doc->program.root_source.source_path.count > 0
                                 ? doc->program.root_source
                                 : doc->front_end.lexer.source;
    ModuleResolveResult resolved = {0};
    ModuleResolveStatus status =
        module_resolve_path(&temp,
                            root_source,
                            lexer,
                            ast,
                            &ast->module_paths[module_path_index],
                            &resolved);
    if (status != MRS_Found) {
        arena_done(&temp);
        return false;
    }

    u32 module_index = lsp_code_action_find_program_module_by_path(
        &doc->program, resolved.resolved_path);
    if (module_index == U32_MAX) {
        arena_done(&temp);
        return false;
    }

    const ModuleInfo* module       = &doc->program.modules[module_index];
    const Ast*        module_ast   = &module->front_end.ast;
    const Lexer*      module_lexer = &module->front_end.lexer;
    string            target_name  = lex_symbol(lexer, target->b);

    for (u32 i = 0; i < array_count(module->export_decl_indices); ++i) {
        u32             decl_index = module->export_decl_indices[i];
        const SemaDecl* decl       = NULL;
        if (!lsp_sema_decl(&module->front_end.sema, decl_index, &decl)) {
            continue;
        }

        if (decl->kind != SK_TypeAlias ||
            !string_eq(lex_symbol(module_lexer, decl->symbol_handle),
                       target_name) ||
            decl->value_node_index >= array_count(module_ast->nodes)) {
            continue;
        }

        u32 value_node_index = lsp_code_action_unwrap_type_value_node(
            module_ast, decl->value_node_index);
        if (value_node_index >= array_count(module_ast->nodes)) {
            continue;
        }

        const AstNode* value = &module_ast->nodes[value_node_index];
        if (value->kind != AK_TypePlex) {
            continue;
        }

        bool ok =
            lsp_code_action_missing_ast_plex_fields_from_type(arena,
                                                              doc,
                                                              node_index,
                                                              module_ast,
                                                              module_lexer,
                                                              value->a,
                                                              out_insert_text);
        arena_done(&temp);
        return ok;
    }

    arena_done(&temp);
    return false;
}

void lsp_handle_code_action(LspState* state, const LspMessage* message)
{
    JsonValue* response = lsp_prepare_response(message);
    JsonValue* actions  = json_new_array(message->arena);

    string uri          = {0};
    u64    line         = 0;
    u64    character    = 0;
    if (!lsp_get_string_param(message, "params.textDocument.uri", &uri) ||
        !lsp_get_u64_param(message, "params.range.start.line", &line) ||
        !lsp_get_u64_param(
            message, "params.range.start.character", &character)) {
        json_object_set_array(response, "result", actions);
        lsp_send_response(message->arena, response);
        return;
    }

    LspSemanticView view = {0};
    if (!lsp_semantic_view(state, uri, &view)) {
        json_object_set_array(response, "result", actions);
        lsp_send_response(message->arena, response);
        return;
    }
    const LspDocument* doc = view.doc;

    usize offset      = lsp_offset_from_position(doc->source, line, character);
    u32   close_token = U32_MAX;
    u32   node_index =
        lsp_code_action_find_plex_literal_at_offset(doc, offset, &close_token);
    if (node_index == U32_MAX || close_token == U32_MAX) {
        lsp_log("code action: no plex literal");
        json_object_set_array(response, "result", actions);
        lsp_send_response(message->arena, response);
        return;
    }

    string insert_text = {0};
    if (!lsp_code_action_missing_plex_fields(
            message->arena, doc, node_index, &insert_text) &&
        !lsp_code_action_missing_ast_plex_fields(
            message->arena, doc, node_index, &insert_text) &&
        !lsp_code_action_missing_imported_ast_plex_fields(
            message->arena, doc, node_index, &insert_text)) {
        lsp_log("code action: no missing fields");
        json_object_set_array(response, "result", actions);
        lsp_send_response(message->arena, response);
        return;
    }

    const Lexer* lexer         = &doc->front_end.lexer;
    usize        insert_offset = lexer->tokens[close_token].offset;
    JsonValue*   range         = lsp_code_action_range(
        message->arena, lexer->source, insert_offset, insert_offset);
    if (range == NULL) {
        json_object_set_array(response, "result", actions);
        lsp_send_response(message->arena, response);
        return;
    }

    JsonValue* edit = json_new_object(message->arena);
    json_object_set_object(edit, "range", range);
    json_object_set_string(edit, message->arena, "newText", insert_text);

    JsonValue* edits = json_new_array(message->arena);
    json_array_push(edits, edit);

    JsonValue* changes = json_new_object(message->arena);
    json_object_set_array(
        changes, lsp_code_action_cstr(message->arena, uri), edits);

    JsonValue* workspace_edit = json_new_object(message->arena);
    json_object_set_object(workspace_edit, "changes", changes);

    JsonValue* action = json_new_object(message->arena);
    json_object_set_string(
        action, message->arena, "title", s("Fill missing plex fields"));
    json_object_set_string(action, message->arena, "kind", s("quickfix"));
    json_object_set_object(action, "edit", workspace_edit);
    json_array_push(actions, action);

    json_object_set_array(response, "result", actions);
    lsp_send_response(message->arena, response);
}
