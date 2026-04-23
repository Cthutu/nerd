//------------------------------------------------------------------------------
// Source formatting implementation
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/cst/cst.h>
#include <compiler/error/error.h>
#include <compiler/format/format.h>
#include <compiler/lexer/lexer.h>

#include <stdio.h>

//------------------------------------------------------------------------------
// Trim leading and trailing ASCII whitespace from a string.

internal string format_trim_ascii(string text)
{
    usize start = 0;
    usize end   = text.count;

    while (start < end &&
           (text.data[start] == ' ' || text.data[start] == '\t' ||
            text.data[start] == '\r' || text.data[start] == '\n')) {
        start++;
    }

    while (end > start &&
           (text.data[end - 1] == ' ' || text.data[end - 1] == '\t' ||
            text.data[end - 1] == '\r' || text.data[end - 1] == '\n')) {
        end--;
    }

    return string_from(text.data + start, end - start);
}

//------------------------------------------------------------------------------
// Return whether a line is comment-only and expose its indent and body.

internal bool format_parse_comment_line(string  line,
                                        string* out_indent,
                                        string* out_comment_body)
{
    usize indent_end = 0;
    while (indent_end < line.count &&
           (line.data[indent_end] == ' ' || line.data[indent_end] == '\t')) {
        indent_end++;
    }

    if (indent_end + 1 >= line.count || line.data[indent_end] != '-' ||
        line.data[indent_end + 1] != '-') {
        return false;
    }

    *out_indent      = string_from(line.data, indent_end);

    usize body_start = indent_end + 2;
    if (body_start < line.count && line.data[body_start] == ' ') {
        body_start++;
    }
    *out_comment_body =
        string_from(line.data + body_start, line.count - body_start);
    return true;
}

//------------------------------------------------------------------------------
// Append one wrapped comment paragraph.

internal void format_emit_comment_paragraph(StringBuilder* sb,
                                            string         indent,
                                            string         paragraph,
                                            usize          wrap_width)
{
    string trimmed = format_trim_ascii(paragraph);
    if (trimmed.count == 0) {
        sb_append_string(sb, indent);
        sb_append_cstr(sb, "--");
        sb_append_char(sb, '\n');
        return;
    }

    usize prefix_width = indent.count + 3;
    usize line_width =
        wrap_width > prefix_width ? wrap_width - prefix_width : 1;
    usize cursor = 0;

    while (cursor < trimmed.count) {
        while (cursor < trimmed.count && trimmed.data[cursor] == ' ') {
            cursor++;
        }
        if (cursor >= trimmed.count) {
            break;
        }

        usize probe         = cursor;
        usize line_len      = 0;
        usize last_word_end = cursor;

        while (probe < trimmed.count) {
            while (probe < trimmed.count && trimmed.data[probe] == ' ') {
                probe++;
            }
            if (probe >= trimmed.count) {
                break;
            }

            usize word_end = probe;
            while (word_end < trimmed.count && trimmed.data[word_end] != ' ') {
                word_end++;
            }

            usize word_len = word_end - probe;
            usize needed   = line_len == 0 ? word_len : line_len + 1 + word_len;
            if (line_len != 0 && needed > line_width) {
                break;
            }

            line_len      = needed;
            last_word_end = word_end;
            probe         = word_end;
        }

        if (last_word_end == cursor) {
            while (last_word_end < trimmed.count &&
                   trimmed.data[last_word_end] != ' ') {
                last_word_end++;
            }
        }

        sb_append_string(sb, indent);
        sb_append_cstr(sb, "-- ");
        sb_append_string(
            sb, string_from(trimmed.data + cursor, last_word_end - cursor));
        sb_append_char(sb, '\n');
        cursor = last_word_end;
    }
}

// Return the precedence of an expression node for parenthesised formatting.

internal int format_expr_precedence(const CstNode* node)
{
    switch (node->kind) {
    case CK_On:
        return 5;
    case CK_IntegerPlus:
    case CK_IntegerMinus:
        return 10;
    case CK_IntegerMultiply:
    case CK_IntegerDivide:
    case CK_IntegerModulo:
        return 20;
    case CK_IntegerNegate:
        return 30;
    case CK_Call:
        return 35;
    case CK_Cast:
        return 35;
    case CK_StringConcat:
        return 37;
    case CK_InterpolatedString:
        return 37;
    default:
        return 40;
    }
}

//------------------------------------------------------------------------------
// Format one expression node with the minimal required parentheses.

internal void format_emit_string_text(StringBuilder* sb, string text)
{
    for (usize i = 0; i < text.count; ++i) {
        switch (text.data[i]) {
        case '\n':
            sb_append_cstr(sb, "\\n");
            break;
        case '\r':
            sb_append_cstr(sb, "\\r");
            break;
        case '\t':
            sb_append_cstr(sb, "\\t");
            break;
        case '\\':
            sb_append_cstr(sb, "\\\\");
            break;
        case '"':
            sb_append_cstr(sb, "\\\"");
            break;
        default:
            sb_append_char(sb, (char)text.data[i]);
            break;
        }
    }
}

internal void format_emit_fn_signature(StringBuilder* sb,
                                       const Cst*     cst,
                                       const Lexer*   lexer,
                                       u32            signature_index,
                                       bool           include_return_type);
internal void format_emit_value(StringBuilder* sb,
                                const Cst*     cst,
                                const Lexer*   lexer,
                                u32            node_index);
internal void format_emit_block_contents(StringBuilder* sb,
                                         const Cst*     cst,
                                         const Lexer*   lexer,
                                         u32            block_node_index,
                                         u32            indent_level);

internal void format_emit_expr(StringBuilder* sb,
                               const Cst*     cst,
                               const Lexer*   lexer,
                               u32            node_index,
                               int            parent_precedence)
{
    const CstNode* node            = &cst->nodes[node_index];
    int            node_precedence = format_expr_precedence(node);
    bool wrap = node->kind != CK_Group && node_precedence < parent_precedence;

    if (wrap) {
        sb_append_char(sb, '(');
    }

    switch (node->kind) {
    case CK_IntegerLiteral:
        sb_format(sb, "%u", (u32)cst_get_integer(cst, node));
        break;
    case CK_BoolLiteral:
        sb_append_cstr(sb, node->a != 0 ? "true" : "false");
        break;
    case CK_StringLiteral:
        sb_append_char(sb, '"');
        format_emit_string_text(sb, lexer->strings[node->a]);
        sb_append_char(sb, '"');
        break;
    case CK_StringConcat:
        format_emit_expr(sb, cst, lexer, node->a, node_precedence);
        sb_append_char(sb, ' ');
        format_emit_expr(sb, cst, lexer, node->b, node_precedence);
        break;
    case CK_InterpolatedString:
        sb_append_cstr(sb, "$\"");
        for (u32 i = node->a; i < node->b; ++i) {
            const CstNode* part = &cst->nodes[i];
            if (part->kind == CK_StringLiteral) {
                format_emit_string_text(sb, lexer->strings[part->a]);
                continue;
            }
            if (part->kind == CK_InterpPartExpr) {
                sb_append_char(sb, '{');
                format_emit_expr(sb, cst, lexer, part->a, 0);
                sb_append_char(sb, '}');
                continue;
            }
            error_ice("Unhandled interpolated string part kind: %u",
                      part->kind);
        }
        sb_append_char(sb, '"');
        break;
    case CK_SymbolRef:
        sb_append_string(sb, lex_symbol(lexer, cst_get_symbol(node)));
        break;
    case CK_Group:
        sb_append_char(sb, '(');
        format_emit_expr(sb, cst, lexer, node->a, 0);
        sb_append_char(sb, ')');
        break;
    case CK_IntegerNegate:
        sb_append_char(sb, '-');
        format_emit_expr(sb, cst, lexer, node->a, node_precedence);
        break;
    case CK_IntegerPlus:
        format_emit_expr(sb, cst, lexer, node->a, node_precedence);
        sb_append_cstr(sb, " + ");
        format_emit_expr(sb, cst, lexer, node->b, node_precedence + 1);
        break;
    case CK_IntegerMinus:
        format_emit_expr(sb, cst, lexer, node->a, node_precedence);
        sb_append_cstr(sb, " - ");
        format_emit_expr(sb, cst, lexer, node->b, node_precedence + 1);
        break;
    case CK_IntegerMultiply:
        format_emit_expr(sb, cst, lexer, node->a, node_precedence);
        sb_append_cstr(sb, " * ");
        format_emit_expr(sb, cst, lexer, node->b, node_precedence + 1);
        break;
    case CK_IntegerDivide:
        format_emit_expr(sb, cst, lexer, node->a, node_precedence);
        sb_append_cstr(sb, " / ");
        format_emit_expr(sb, cst, lexer, node->b, node_precedence + 1);
        break;
    case CK_IntegerModulo:
        format_emit_expr(sb, cst, lexer, node->a, node_precedence);
        sb_append_cstr(sb, " % ");
        format_emit_expr(sb, cst, lexer, node->b, node_precedence + 1);
        break;
    case CK_Call:
        format_emit_expr(sb, cst, lexer, node->a, node_precedence);
        sb_append_char(sb, '(');
        {
            const CstCallInfo* call = &cst->calls[node->b];
            for (u32 i = 0; i < call->arg_count; ++i) {
                if (i > 0) {
                    sb_append_cstr(sb, ", ");
                }
                format_emit_expr(
                    sb, cst, lexer, cst->call_args[call->first_arg + i], 0);
            }
        }
        sb_append_char(sb, ')');
        break;
    case CK_Cast:
        format_emit_expr(sb, cst, lexer, node->a, node_precedence);
        sb_append_cstr(sb, ".cast(");
        format_emit_expr(sb, cst, lexer, node->b, 0);
        sb_append_char(sb, ')');
        break;
    case CK_On:
        {
            const CstOnInfo* on = &cst->ons[node->b];
            sb_append_cstr(sb, "on ");
            format_emit_expr(sb, cst, lexer, node->a, node_precedence);
            sb_append_cstr(sb, " => ");
            format_emit_expr(sb, cst, lexer, on->true_expr_node_index, 0);
            sb_append_cstr(sb, " else ");
            format_emit_expr(sb, cst, lexer, on->false_expr_node_index, 0);
        }
        break;
    case CK_TypeFn:
        format_emit_fn_signature(sb, cst, lexer, node->a, true);
        break;
    case CK_FnExpr:
        format_emit_fn_signature(sb, cst, lexer, node->a, false);
        sb_append_cstr(sb, " => ");
        format_emit_expr(sb, cst, lexer, node->b, 0);
        break;
    case CK_FnBlock:
        format_emit_fn_signature(sb, cst, lexer, node->a, true);
        sb_append_cstr(sb, " {\n");
        format_emit_block_contents(sb, cst, lexer, node->b, 1);
        sb_append_cstr(sb, "}");
        break;
    default:
        error_ice("Unhandled CST node kind in formatter expression rendering: "
                  "%u",
                  node->kind);
        break;
    }

    if (wrap) {
        sb_append_char(sb, ')');
    }
}

internal void format_emit_variable_payload(StringBuilder* sb,
                                           const Cst*     cst,
                                           const Lexer*   lexer,
                                           u32            node_index);
internal void format_emit_fn_signature(StringBuilder* sb,
                                       const Cst*     cst,
                                       const Lexer*   lexer,
                                       u32            signature_index,
                                       bool           include_return_type);
internal void format_emit_block_contents(StringBuilder* sb,
                                         const Cst*     cst,
                                         const Lexer*   lexer,
                                         u32            block_node_index,
                                         u32            indent_level);
internal void format_emit_block_statement(StringBuilder* sb,
                                          const Cst*     cst,
                                          const Lexer*   lexer,
                                          u32            node_index,
                                          u32            indent_level);

internal void format_emit_indent(StringBuilder* sb, u32 indent_level)
{
    for (u32 i = 0; i < indent_level; ++i) {
        sb_append_cstr(sb, "    ");
    }
}

internal bool format_is_block_statement(const CstNode* node)
{
    return node->kind == CK_Block || node->kind == CK_Statement ||
           node->kind == CK_Return || node->kind == CK_Bind ||
           node->kind == CK_Variable || node->kind == CK_Assign;
}

internal void format_emit_fn_signature(StringBuilder* sb,
                                       const Cst*     cst,
                                       const Lexer*   lexer,
                                       u32            signature_index,
                                       bool           include_return_type)
{
    const CstFnSignature* signature = &cst->fn_signatures[signature_index];

    sb_append_cstr(sb, "fn (");
    for (u32 i = 0; i < signature->param_count; ++i) {
        if (i > 0) {
            sb_append_cstr(sb, ", ");
        }

        const CstParam* param = &cst->params[signature->first_param + i];
        if (param->symbol_handle != U32_MAX) {
            sb_append_string(sb, lex_symbol(lexer, param->symbol_handle));
            sb_append_cstr(sb, ": ");
        }
        format_emit_expr(sb, cst, lexer, param->type_node_index, 0);
    }
    sb_append_char(sb, ')');

    if (include_return_type && signature->return_type_node_index != U32_MAX) {
        sb_append_cstr(sb, " -> ");
        format_emit_expr(sb, cst, lexer, signature->return_type_node_index, 0);
    }
}

internal void format_emit_block_contents(StringBuilder* sb,
                                         const Cst*     cst,
                                         const Lexer*   lexer,
                                         u32            block_node_index,
                                         u32            indent_level)
{
    const CstNode* block = &cst->nodes[block_node_index];
    ASSERT(block->kind == CK_Block, "Expected block node");

    for (u32 i = block->a; i < block->b; ++i) {
        if (!format_is_block_statement(&cst->nodes[i])) {
            continue;
        }
        format_emit_block_statement(sb, cst, lexer, i, indent_level);
        if (cst->nodes[i].kind == CK_Block) {
            i = cst->nodes[i].b - 1;
        }
    }
}

internal void format_emit_block_statement(StringBuilder* sb,
                                          const Cst*     cst,
                                          const Lexer*   lexer,
                                          u32            node_index,
                                          u32            indent_level)
{
    const CstNode* stmt = &cst->nodes[node_index];
    format_emit_indent(sb, indent_level);

    if (stmt->kind == CK_Block) {
        sb_append_cstr(sb, "{\n");
        for (u32 i = stmt->a; i < stmt->b; ++i) {
            if (!format_is_block_statement(&cst->nodes[i])) {
                continue;
            }
            format_emit_block_statement(sb, cst, lexer, i, indent_level + 1);
            if (cst->nodes[i].kind == CK_Block) {
                i = cst->nodes[i].b - 1;
            }
        }
        format_emit_indent(sb, indent_level);
        sb_append_cstr(sb, "}\n");
        return;
    }

    if (stmt->kind == CK_Return) {
        sb_append_cstr(sb, "return ");
        format_emit_expr(sb, cst, lexer, stmt->a, 0);
        sb_append_char(sb, '\n');
        return;
    }

    if (stmt->kind == CK_Variable) {
        sb_append_string(sb, lex_symbol(lexer, cst_get_symbol(stmt)));
        if (cst->nodes[stmt->b].kind == CK_AnnotatedValue ||
            cst->nodes[stmt->b].kind == CK_ZeroInit) {
            sb_append_cstr(sb, ": ");
            format_emit_variable_payload(sb, cst, lexer, stmt->b);
        } else {
            sb_append_cstr(sb, " := ");
            format_emit_expr(sb, cst, lexer, stmt->b, 0);
        }
        sb_append_char(sb, '\n');
        return;
    }

    if (stmt->kind == CK_Bind) {
        sb_append_string(sb, lex_symbol(lexer, cst_get_symbol(stmt)));
        sb_append_cstr(sb, " :: ");
        format_emit_value(sb, cst, lexer, stmt->b);
        sb_append_char(sb, '\n');
        return;
    }

    if (stmt->kind == CK_Assign) {
        sb_append_string(sb, lex_symbol(lexer, cst_get_symbol(stmt)));
        sb_append_cstr(sb, " = ");
        format_emit_expr(sb, cst, lexer, stmt->b, 0);
        sb_append_char(sb, '\n');
        return;
    }

    if (stmt->kind == CK_Statement) {
        format_emit_expr(sb, cst, lexer, stmt->a, 0);
        sb_append_char(sb, '\n');
    }
}

//------------------------------------------------------------------------------
// Format one top-level value node.

internal void format_emit_value(StringBuilder* sb,
                                const Cst*     cst,
                                const Lexer*   lexer,
                                u32            node_index)
{
    const CstNode* node = &cst->nodes[node_index];

    if (node->kind == CK_AnnotatedValue) {
        format_emit_expr(sb, cst, lexer, node->a, 0);
        sb_append_cstr(sb, " : ");
        format_emit_value(sb, cst, lexer, node->b);
        return;
    }

    if (node->kind == CK_ZeroInit) {
        format_emit_expr(sb, cst, lexer, node->a, 0);
        return;
    }

    switch (node->kind) {
    case CK_FnExpr:
        format_emit_fn_signature(sb, cst, lexer, node->a, false);
        sb_append_cstr(sb, " => ");
        format_emit_expr(sb, cst, lexer, node->b, 0);
        break;
    case CK_FnBlock:
        format_emit_fn_signature(sb, cst, lexer, node->a, true);
        sb_append_cstr(sb, " {\n");
        format_emit_block_contents(sb, cst, lexer, node->b, 1);
        sb_append_cstr(sb, "}");
        break;
    default:
        format_emit_expr(sb, cst, lexer, node_index, 0);
        break;
    }
}

internal void format_emit_variable_payload(StringBuilder* sb,
                                           const Cst*     cst,
                                           const Lexer*   lexer,
                                           u32            node_index)
{
    const CstNode* node = &cst->nodes[node_index];

    if (node->kind == CK_AnnotatedValue) {
        format_emit_expr(sb, cst, lexer, node->a, 0);
        sb_append_cstr(sb, " = ");
        format_emit_value(sb, cst, lexer, node->b);
        return;
    }

    if (node->kind == CK_ZeroInit) {
        format_emit_expr(sb, cst, lexer, node->a, 0);
        return;
    }

    sb_append_cstr(sb, "= ");
    format_emit_expr(sb, cst, lexer, node_index, 0);
}

//------------------------------------------------------------------------------
// Format a parsed code block with one binding per line.

internal bool format_emit_code_block(StringBuilder* sb, NerdSource source)
{
    Lexer lexer = {0};
    if (!lex(source, &lexer)) {
        return false;
    }

    Cst cst = {0};
    if (!cst_parse(&lexer, &cst) || array_count(cst.bindings) == 0) {
        cst_done(&cst);
        lex_done(&lexer);
        return false;
    }

    bool first_binding = true;
    for (u32 i = 0; i < array_count(cst.bindings); ++i) {
        const CstNode* node = &cst.nodes[cst.bindings[i]];

        if (!first_binding) {
            sb_append_char(sb, '\n');
        }

        sb_append_string(sb, lex_symbol(&lexer, cst_get_symbol(node)));
        if (node->kind == CK_Bind) {
            sb_append_cstr(
                sb,
                cst.nodes[node->b].kind == CK_AnnotatedValue ? " : " : " :: ");
            format_emit_value(sb, &cst, &lexer, node->b);
        } else {
            if (cst.nodes[node->b].kind == CK_AnnotatedValue ||
                cst.nodes[node->b].kind == CK_ZeroInit) {
                sb_append_cstr(sb, ": ");
                format_emit_variable_payload(sb, &cst, &lexer, node->b);
            } else {
                sb_append_cstr(sb, " := ");
                format_emit_expr(sb, &cst, &lexer, node->b, 0);
            }
        }
        sb_append_char(sb, '\n');
        first_binding = false;
    }

    cst_done(&cst);
    lex_done(&lexer);
    return true;
}

//------------------------------------------------------------------------------
// Return whether a line is entirely blank.

internal bool format_is_blank_line(string line)
{
    for (usize i = 0; i < line.count; ++i) {
        if (line.data[i] != ' ' && line.data[i] != '\t' &&
            line.data[i] != '\r') {
            return false;
        }
    }
    return true;
}

//------------------------------------------------------------------------------
// Format source text, reflowing comments and normalising code blocks.

bool format_source(NerdSource source, Arena* arena, string* out_text)
{
    Lexer lexer = {0};
    if (!lex_with_config(
            source, &(LexerConfig){.mode = LEXER_MODE_FORMAT}, &lexer)) {
        return false;
    }

    StringBuilder sb = {0};
    sb_init(&sb, arena);

    const string text                    = source.source;
    const usize  wrap_width              = 80;
    usize        offset                  = 0;
    bool         just_emitted_blank_line = false;

    while (offset < text.count) {
        usize line_end = offset;
        while (line_end < text.count && text.data[line_end] != '\n') {
            line_end++;
        }

        bool has_newline = line_end < text.count && text.data[line_end] == '\n';
        string line      = string_from(text.data + offset, line_end - offset);
        if (format_is_blank_line(line)) {
            if (sb.size > 0 && !just_emitted_blank_line) {
                sb_append_char(&sb, '\n');
                just_emitted_blank_line = true;
            }
            offset = has_newline ? line_end + 1 : line_end;
            continue;
        }

        string indent       = {0};
        string comment_body = {0};
        if (!format_parse_comment_line(line, &indent, &comment_body)) {
            usize block_start = offset;
            usize block_end   = has_newline ? line_end + 1 : line_end;

            while (block_end < text.count) {
                usize next_line_end = block_end;
                while (next_line_end < text.count &&
                       text.data[next_line_end] != '\n') {
                    next_line_end++;
                }

                bool next_has_newline = next_line_end < text.count &&
                                        text.data[next_line_end] == '\n';
                string next_line   = string_from(text.data + block_end,
                                               next_line_end - block_end);
                string next_indent = {0};
                string next_body   = {0};

                if (format_parse_comment_line(
                        next_line, &next_indent, &next_body)) {
                    break;
                }

                block_end =
                    next_has_newline ? next_line_end + 1 : next_line_end;
            }

            Arena block_arena = {0};
            arena_init(&block_arena);
            u8* block_copy =
                (u8*)arena_alloc(&block_arena, block_end - block_start);
            memcpy(
                block_copy, text.data + block_start, block_end - block_start);
            bool ok = format_emit_code_block(
                &sb,
                (NerdSource){
                    .source = string_from(block_copy, block_end - block_start),
                    .source_path = source.source_path,
                });
            arena_done(&block_arena);
            if (!ok) {
                lex_done(&lexer);
                return false;
            }
            offset                  = block_end;
            just_emitted_blank_line = false;
            continue;
        }

        Arena paragraph_arena = {0};
        arena_init(&paragraph_arena);
        string paragraph = {0};

        while (true) {
            string body = format_trim_ascii(comment_body);
            if (body.count == 0) {
                if (paragraph.count > 0) {
                    format_emit_comment_paragraph(
                        &sb, indent, paragraph, wrap_width);
                    paragraph = (string){0};
                }
                sb_append_string(&sb, indent);
                sb_append_cstr(&sb, "--");
                sb_append_char(&sb, '\n');
                just_emitted_blank_line = false;
            } else if (paragraph.count == 0) {
                paragraph =
                    string_format(&paragraph_arena, STRINGP, STRINGV(body));
            } else {
                paragraph = string_format(&paragraph_arena,
                                          STRINGP " " STRINGP,
                                          STRINGV(paragraph),
                                          STRINGV(body));
            }

            offset = has_newline ? line_end + 1 : line_end;
            if (offset >= text.count) {
                break;
            }

            line_end = offset;
            while (line_end < text.count && text.data[line_end] != '\n') {
                line_end++;
            }
            has_newline = line_end < text.count && text.data[line_end] == '\n';
            line        = string_from(text.data + offset, line_end - offset);

            string next_indent = {0};
            string next_body   = {0};
            if (!format_parse_comment_line(line, &next_indent, &next_body) ||
                !string_eq(indent, next_indent)) {
                break;
            }

            comment_body = next_body;
        }

        if (paragraph.count > 0) {
            format_emit_comment_paragraph(&sb, indent, paragraph, wrap_width);
        }

        arena_done(&paragraph_arena);
        just_emitted_blank_line = false;
    }

    if (sb.size > 0 && sb.data[sb.size - 1] != '\n') {
        sb_append_char(&sb, '\n');
    }

    *out_text = sb_to_string(&sb);
    lex_done(&lexer);
    return true;
}

//------------------------------------------------------------------------------
// Format one source file and save the result to the given output path.

bool format_file(cstr input_path, cstr output_path)
{
    FileMap map  = {0};
    string  text = filemap_load(input_path, &map);
    if (text.data == NULL) {
        return false;
    }

    Arena arena = {0};
    arena_init(&arena);

    string rendered = {0};
    bool   ok       = format_source(
        (NerdSource){
                    .source      = text,
                    .source_path = s(input_path),
        },
        &arena,
        &rendered);
    if (!ok) {
        arena_done(&arena);
        filemap_unload(&map);
        return false;
    }

    FILE* file = fopen(output_path, "wb");
    if (!file) {
        arena_done(&arena);
        filemap_unload(&map);
        return error_runtime("Failed to open file for writing: %s",
                             output_path);
    }

    usize written = fwrite(rendered.data, 1, rendered.count, file);
    fclose(file);
    if (written != rendered.count) {
        arena_done(&arena);
        filemap_unload(&map);
        return error_runtime("Failed to write formatted file: %s", output_path);
    }

    arena_done(&arena);
    filemap_unload(&map);
    return true;
}

//------------------------------------------------------------------------------
