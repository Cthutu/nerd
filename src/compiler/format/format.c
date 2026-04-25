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

#define FORMAT_WRAP_WIDTH 80

internal void format_emit_for_header_items(StringBuilder* sb,
                                           const Cst*     cst,
                                           const Lexer*   lexer,
                                           u32            first_item,
                                           u32            item_count);
internal void format_emit_indent(StringBuilder* sb, u32 indent_level);
internal bool format_node_is_block_form_on(const Cst* cst, u32 node_index);
internal void format_emit_on_block_multiline(StringBuilder* sb,
                                             const Cst*     cst,
                                             const Lexer*   lexer,
                                             u32            node_index,
                                             u32            indent_level);

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
    case CK_LogicalOr:
        return 10;
    case CK_LogicalAnd:
        return 15;
    case CK_BitwiseOr:
        return 20;
    case CK_BitwiseXor:
        return 25;
    case CK_BitwiseAnd:
        return 30;
    case CK_Equal:
    case CK_NotEqual:
        return 35;
    case CK_Less:
    case CK_LessEqual:
    case CK_Greater:
    case CK_GreaterEqual:
        return 40;
    case CK_IntegerPlus:
    case CK_IntegerMinus:
        return 50;
    case CK_IntegerMultiply:
    case CK_IntegerDivide:
    case CK_IntegerModulo:
        return 60;
    case CK_LogicalNot:
    case CK_IntegerNegate:
    case CK_AddressOf:
        return 70;
    case CK_Call:
        return 80;
    case CK_Cast:
        return 80;
    case CK_TupleField:
    case CK_Index:
    case CK_Slice:
    case CK_Field:
    case CK_Plex:
    case CK_PlexUpdate:
        return 80;
    case CK_StringConcat:
        return 85;
    case CK_InterpolatedString:
        return 85;
    default:
        return 100;
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

internal void format_emit_float_literal(StringBuilder* sb,
                                        const Lexer*   lexer,
                                        u32            token_index)
{
    ASSERT(token_index < array_count(lexer->tokens),
           "Float literal token index out of bounds");
    const Token* token = &lexer->tokens[token_index];
    ASSERT(token->kind == TK_Float, "Expected float token");

    usize start = token->offset;
    usize end   = lex_token_end_offset(lexer, token);
    if (end > lexer->source.source.count) {
        end = lexer->source.source.count;
    }

    sb_append_string(
        sb, string_from(lexer->source.source.data + start, end - start));
}

internal void format_emit_fn_signature(StringBuilder* sb,
                                       const Cst*     cst,
                                       const Lexer*   lexer,
                                       u32            signature_index,
                                       bool           include_return_type);
internal void format_emit_ffi_def(StringBuilder* sb,
                                  const Cst*     cst,
                                  const Lexer*   lexer,
                                  u32            ffi_info_index);
internal void format_emit_value(StringBuilder* sb,
                                const Cst*     cst,
                                const Lexer*   lexer,
                                u32            node_index);
internal void format_emit_block_contents(StringBuilder* sb,
                                         const Cst*     cst,
                                         const Lexer*   lexer,
                                         u32            block_node_index,
                                         u32            indent_level);
internal void format_emit_indent(StringBuilder* sb, u32 indent_level);
internal void format_emit_type_plex_multiline(StringBuilder* sb,
                                              const Cst*     cst,
                                              const Lexer*   lexer,
                                              u32            node_index,
                                              u32            indent_level);
internal void format_emit_expr(StringBuilder* sb,
                               const Cst*     cst,
                               const Lexer*   lexer,
                               u32            node_index,
                               int            parent_precedence);
internal void format_emit_expr_with_indent(StringBuilder* sb,
                                           const Cst*     cst,
                                           const Lexer*   lexer,
                                           u32            node_index,
                                           int            parent_precedence,
                                           u32            indent_level);

internal u32 g_format_expr_indent_level = 0;

internal void format_emit_pattern(StringBuilder* sb,
                                  const Cst*     cst,
                                  const Lexer*   lexer,
                                  u32            pattern_index)
{
    const CstPattern* pattern = &cst->patterns[pattern_index];
    switch (pattern->kind) {
    case CPK_Value:
        format_emit_expr(sb, cst, lexer, pattern->a, 0);
        break;
    case CPK_Equal:
    case CPK_NotEqual:
    case CPK_Less:
    case CPK_LessEqual:
    case CPK_Greater:
    case CPK_GreaterEqual:
        sb_append_cstr(sb,
                       pattern->kind == CPK_Equal       ? "== "
                       : pattern->kind == CPK_NotEqual  ? "!= "
                       : pattern->kind == CPK_Less      ? "< "
                       : pattern->kind == CPK_LessEqual ? "<= "
                       : pattern->kind == CPK_Greater   ? "> "
                                                        : ">= ");
        format_emit_expr(sb, cst, lexer, pattern->a, 0);
        break;
    case CPK_Ignore:
        sb_append_char(sb, '_');
        break;
    case CPK_Bind:
        if (pattern->b != U32_MAX) {
            format_emit_pattern(sb, cst, lexer, pattern->b);
            sb_append_cstr(sb, " as ");
        }
        sb_append_string(sb, lex_symbol(lexer, pattern->a));
        break;
    case CPK_RangeExclusive:
    case CPK_RangeInclusive:
        format_emit_expr(sb, cst, lexer, pattern->a, 0);
        sb_append_cstr(sb, pattern->kind == CPK_RangeInclusive ? "..=" : "..");
        format_emit_expr(sb, cst, lexer, pattern->b, 0);
        break;
    case CPK_Tuple:
        sb_append_char(sb, '(');
        for (u32 i = 0; i < pattern->b; ++i) {
            if (i > 0) {
                sb_append_cstr(sb, ", ");
            }
            format_emit_pattern(
                sb, cst, lexer, cst->pattern_items[pattern->a + i]);
        }
        if (pattern->b == 1) {
            sb_append_char(sb, ',');
        }
        sb_append_char(sb, ')');
        break;
    case CPK_Plex:
        sb_append_cstr(sb, "{ ");
        for (u32 i = 0; i < pattern->b; ++i) {
            if (i > 0) {
                sb_append_cstr(sb, ", ");
            }
            const CstPlexPatternField* field =
                &cst->pattern_fields[pattern->a + i];
            sb_append_string(sb, lex_symbol(lexer, field->symbol_handle));
            const CstPattern* field_pattern =
                &cst->patterns[field->pattern_index];
            if (!(field_pattern->kind == CPK_Bind &&
                  field_pattern->b == U32_MAX &&
                  field_pattern->a == field->symbol_handle)) {
                sb_append_cstr(sb, ": ");
                format_emit_pattern(sb, cst, lexer, field->pattern_index);
            }
        }
        sb_append_cstr(sb, " }");
        break;
    case CPK_EnumVariant:
        {
            const CstEnumPattern* enum_pattern =
                &cst->enum_patterns[pattern->a];
            sb_append_string(sb,
                             lex_symbol(lexer, enum_pattern->symbol_handle));
            sb_append_char(sb, '(');
            for (u32 i = 0; i < enum_pattern->pattern_count; ++i) {
                if (i > 0) {
                    sb_append_cstr(sb, ", ");
                }
                format_emit_pattern(
                    sb,
                    cst,
                    lexer,
                    cst->pattern_items[enum_pattern->first_pattern + i]);
            }
            sb_append_char(sb, ')');
        }
        break;
    }
}

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
    case CK_FloatLiteral:
        format_emit_float_literal(sb, lexer, node->token_index);
        break;
    case CK_BoolLiteral:
        sb_append_cstr(sb, node->a != 0 ? "yes" : "no");
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
                if (format_node_is_block_form_on(cst, part->a)) {
                    sb_append_cstr(sb, "{\n");
                    u32 indent_level = g_format_expr_indent_level + 1;
                    format_emit_indent(sb, indent_level);
                    format_emit_on_block_multiline(
                        sb, cst, lexer, part->a, indent_level);
                    sb_append_char(sb, '\n');
                    format_emit_indent(sb, g_format_expr_indent_level);
                    sb_append_char(sb, '}');
                } else {
                    sb_append_char(sb, '{');
                    format_emit_expr(sb, cst, lexer, part->a, 0);
                    sb_append_char(sb, '}');
                }
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
    case CK_Tuple:
        sb_append_char(sb, '(');
        for (u32 i = 0; i < node->b; ++i) {
            if (i > 0) {
                sb_append_cstr(sb, ", ");
            }
            format_emit_expr(sb, cst, lexer, cst->tuple_items[node->a + i], 0);
        }
        if (node->b == 1) {
            sb_append_char(sb, ',');
        }
        sb_append_char(sb, ')');
        break;
    case CK_Array:
        sb_append_char(sb, '[');
        for (u32 i = 0; i < node->b; ++i) {
            if (i > 0) {
                sb_append_cstr(sb, ", ");
            }
            format_emit_expr(sb, cst, lexer, cst->tuple_items[node->a + i], 0);
        }
        sb_append_char(sb, ']');
        break;
    case CK_Plex:
    case CK_PlexUpdate:
        {
            const CstPlexLiteralInfo* plex = &cst->plex_literals[node->a];
            format_emit_expr(sb, cst, lexer, plex->target_node_index, 0);
            if (node->kind == CK_PlexUpdate) {
                sb_append_cstr(sb, " with");
            }
            sb_append_cstr(sb, " { ");
            for (u32 i = 0; i < plex->field_count; ++i) {
                if (i > 0) {
                    sb_append_cstr(sb, ", ");
                }
                const CstPlexLiteralField* field =
                    &cst->plex_literal_fields[plex->first_field + i];
                sb_append_string(sb, lex_symbol(lexer, field->symbol_handle));
                sb_append_cstr(sb, ": ");
                format_emit_expr(sb, cst, lexer, field->value_node_index, 0);
            }
            sb_append_cstr(sb, " }");
        }
        break;
    case CK_IntegerNegate:
        sb_append_char(sb, '-');
        format_emit_expr(sb, cst, lexer, node->a, node_precedence);
        break;
    case CK_LogicalNot:
        sb_append_char(sb, '!');
        format_emit_expr(sb, cst, lexer, node->a, node_precedence);
        break;
    case CK_AddressOf:
        sb_append_char(sb, '^');
        format_emit_expr(sb, cst, lexer, node->a, node_precedence);
        break;
    case CK_ReturnExpr:
        sb_append_cstr(sb, "return");
        if (node->a != U32_MAX) {
            sb_append_char(sb, ' ');
            format_emit_expr(sb, cst, lexer, node->a, 0);
        }
        break;
    case CK_BreakExpr:
        sb_append_cstr(sb, "break");
        if (node->b != U32_MAX) {
            sb_append_cstr(sb, " $");
            sb_append_string(sb, lex_symbol(lexer, node->b));
        }
        if (node->a != U32_MAX) {
            sb_append_char(sb, ' ');
            format_emit_expr(sb, cst, lexer, node->a, 0);
        }
        break;
    case CK_ContinueExpr:
        sb_append_cstr(sb, "continue");
        if (node->b != U32_MAX) {
            sb_append_cstr(sb, " $");
            sb_append_string(sb, lex_symbol(lexer, node->b));
        }
        break;
    case CK_ExprBlock:
        sb_append_cstr(sb, "$");
        if (node->b != U32_MAX) {
            sb_append_string(sb, lex_symbol(lexer, node->b));
            sb_append_char(sb, ' ');
        }
        sb_append_cstr(sb, "{\n");
        format_emit_block_contents(sb, cst, lexer, node->a, 1);
        sb_append_char(sb, '}');
        break;
    case CK_For:
        {
            const CstForInfo* for_info = &cst->fors[node->a];
            sb_append_cstr(sb, "for");
            bool is_c_style =
                for_info->init_count > 0 || for_info->update_count > 0;
            if (is_c_style) {
                sb_append_char(sb, ' ');
                if (for_info->init_count > 0) {
                    format_emit_for_header_items(sb,
                                                 cst,
                                                 lexer,
                                                 for_info->first_init,
                                                 for_info->init_count);
                }
                sb_append_cstr(sb, "; ");
                if (for_info->condition_node_index != U32_MAX) {
                    format_emit_expr(
                        sb, cst, lexer, for_info->condition_node_index, 0);
                }
                sb_append_cstr(sb, "; ");
                if (for_info->update_count > 0) {
                    format_emit_for_header_items(sb,
                                                 cst,
                                                 lexer,
                                                 for_info->first_update,
                                                 for_info->update_count);
                }
            } else if (for_info->condition_node_index != U32_MAX) {
                sb_append_char(sb, ' ');
                format_emit_expr(
                    sb, cst, lexer, for_info->condition_node_index, 0);
            }
            if (for_info->label_symbol != U32_MAX) {
                sb_append_cstr(sb, " $");
                sb_append_string(sb, lex_symbol(lexer, for_info->label_symbol));
            }
            sb_append_cstr(sb, " {\n");
            format_emit_block_contents(sb, cst, lexer, node->b, 2);
            format_emit_indent(sb, 1);
            sb_append_char(sb, '}');
            if (for_info->else_block_index != U32_MAX) {
                sb_append_cstr(sb, " else {\n");
                format_emit_block_contents(
                    sb, cst, lexer, for_info->else_block_index, 2);
                format_emit_indent(sb, 1);
                sb_append_char(sb, '}');
            }
        }
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
    case CK_BitwiseAnd:
        format_emit_expr(sb, cst, lexer, node->a, node_precedence);
        sb_append_cstr(sb, " & ");
        format_emit_expr(sb, cst, lexer, node->b, node_precedence + 1);
        break;
    case CK_BitwiseXor:
        format_emit_expr(sb, cst, lexer, node->a, node_precedence);
        sb_append_cstr(sb, " ^ ");
        format_emit_expr(sb, cst, lexer, node->b, node_precedence + 1);
        break;
    case CK_BitwiseOr:
        format_emit_expr(sb, cst, lexer, node->a, node_precedence);
        sb_append_cstr(sb, " | ");
        format_emit_expr(sb, cst, lexer, node->b, node_precedence + 1);
        break;
    case CK_Equal:
        format_emit_expr(sb, cst, lexer, node->a, node_precedence);
        sb_append_cstr(sb, " == ");
        format_emit_expr(sb, cst, lexer, node->b, node_precedence + 1);
        break;
    case CK_NotEqual:
        format_emit_expr(sb, cst, lexer, node->a, node_precedence);
        sb_append_cstr(sb, " != ");
        format_emit_expr(sb, cst, lexer, node->b, node_precedence + 1);
        break;
    case CK_Less:
        format_emit_expr(sb, cst, lexer, node->a, node_precedence);
        sb_append_cstr(sb, " < ");
        format_emit_expr(sb, cst, lexer, node->b, node_precedence + 1);
        break;
    case CK_LessEqual:
        format_emit_expr(sb, cst, lexer, node->a, node_precedence);
        sb_append_cstr(sb, " <= ");
        format_emit_expr(sb, cst, lexer, node->b, node_precedence + 1);
        break;
    case CK_Greater:
        format_emit_expr(sb, cst, lexer, node->a, node_precedence);
        sb_append_cstr(sb, " > ");
        format_emit_expr(sb, cst, lexer, node->b, node_precedence + 1);
        break;
    case CK_GreaterEqual:
        format_emit_expr(sb, cst, lexer, node->a, node_precedence);
        sb_append_cstr(sb, " >= ");
        format_emit_expr(sb, cst, lexer, node->b, node_precedence + 1);
        break;
    case CK_LogicalAnd:
        format_emit_expr(sb, cst, lexer, node->a, node_precedence);
        sb_append_cstr(sb, " && ");
        format_emit_expr(sb, cst, lexer, node->b, node_precedence + 1);
        break;
    case CK_LogicalOr:
        format_emit_expr(sb, cst, lexer, node->a, node_precedence);
        sb_append_cstr(sb, " || ");
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
        sb_append_cstr(sb, ".as(");
        format_emit_expr(sb, cst, lexer, node->b, 0);
        sb_append_char(sb, ')');
        break;
    case CK_TupleField:
        format_emit_expr(sb, cst, lexer, node->a, node_precedence);
        sb_format(sb, ".%u", node->b);
        break;
    case CK_Field:
        format_emit_expr(sb, cst, lexer, node->a, node_precedence);
        sb_append_char(sb, '.');
        sb_append_string(sb, lex_symbol(lexer, node->b));
        break;
    case CK_EnumVariant:
        sb_append_char(sb, '.');
        sb_append_string(sb, lex_symbol(lexer, node->a));
        break;
    case CK_Index:
        format_emit_expr(sb, cst, lexer, node->a, node_precedence);
        sb_append_char(sb, '[');
        format_emit_expr(sb, cst, lexer, node->b, 0);
        sb_append_char(sb, ']');
        break;
    case CK_Slice:
        {
            const CstSliceInfo* slice = &cst->slices[node->a];
            format_emit_expr(
                sb, cst, lexer, slice->target_node_index, node_precedence);
            sb_append_char(sb, '[');
            if (slice->start_node_index != U32_MAX) {
                format_emit_expr(sb, cst, lexer, slice->start_node_index, 0);
            }
            sb_append_cstr(sb, "..");
            if (slice->end_node_index != U32_MAX) {
                format_emit_expr(sb, cst, lexer, slice->end_node_index, 0);
            }
            sb_append_char(sb, ']');
        }
        break;
    case CK_RangeExclusive:
    case CK_RangeInclusive:
        format_emit_expr(sb, cst, lexer, node->a, 0);
        sb_append_cstr(sb, node->kind == CK_RangeExclusive ? ".." : "..=");
        format_emit_expr(sb, cst, lexer, node->b, 0);
        break;
    case CK_On:
        {
            const CstOnInfo* on = &cst->ons[node->b];
            sb_append_cstr(sb, "on");
            if (on->kind != COK_Condition) {
                sb_append_char(sb, ' ');
                format_emit_expr(sb, cst, lexer, node->a, node_precedence);
            }
            if (on->kind == COK_Bool) {
                const CstOnBranch* true_branch =
                    &cst->on_branches[on->first_branch];
                sb_append_cstr(sb, " => ");
                format_emit_expr(
                    sb, cst, lexer, true_branch->expr_node_index, 0);
                if (on->branch_count == 1) {
                    break;
                }
                const CstOnBranch* else_branch =
                    &cst->on_branches[on->first_branch + 1];
                sb_append_cstr(sb, " else ");
                format_emit_expr(
                    sb, cst, lexer, else_branch->expr_node_index, 0);
                break;
            }

            sb_append_cstr(sb, " { ");
            for (u32 i = 0; i < on->branch_count; ++i) {
                if (i > 0) {
                    sb_append_char(sb, ' ');
                }
                const CstOnBranch* branch =
                    &cst->on_branches[on->first_branch + i];
                if (branch->flags & COBF_Else) {
                    sb_append_cstr(sb, "else");
                } else if (on->kind == COK_Condition) {
                    format_emit_expr(
                        sb, cst, lexer, branch->guard_node_index, 0);
                } else {
                    for (u32 pattern = 0; pattern < branch->pattern_count;
                         ++pattern) {
                        if (pattern > 0) {
                            sb_append_cstr(sb, ", ");
                        }
                        format_emit_pattern(
                            sb,
                            cst,
                            lexer,
                            cst->pattern_items[branch->pattern_index +
                                               pattern]);
                    }
                }
                if (branch->binder_symbol_handle != U32_MAX) {
                    sb_append_cstr(sb, " as ");
                    sb_append_string(
                        sb, lex_symbol(lexer, branch->binder_symbol_handle));
                }
                if (branch->guard_node_index != U32_MAX &&
                    on->kind != COK_Condition) {
                    sb_append_cstr(sb, " on ");
                    format_emit_expr(
                        sb, cst, lexer, branch->guard_node_index, 0);
                }
                sb_append_cstr(sb, " => ");
                format_emit_expr(sb, cst, lexer, branch->expr_node_index, 0);
            }
            sb_append_cstr(sb, " }");
        }
        break;
    case CK_TypeEnum:
        {
            const CstEnumTypeInfo* enum_type = &cst->enum_types[node->a];
            sb_append_cstr(sb, "enum {");
            for (u32 i = 0; i < enum_type->variant_count; ++i) {
                const CstEnumVariant* variant =
                    &cst->enum_variants[enum_type->first_variant + i];
                sb_append_char(sb, ' ');
                sb_append_string(sb, lex_symbol(lexer, variant->symbol_handle));
                if (variant->type_node_index != U32_MAX) {
                    sb_append_char(sb, '(');
                    const CstNode* variant_type =
                        &cst->nodes[variant->type_node_index];
                    if (variant_type->kind == CK_TypeTuple) {
                        for (u32 item = 0; item < variant_type->b; ++item) {
                            if (item > 0) {
                                sb_append_cstr(sb, ", ");
                            }
                            format_emit_expr(
                                sb,
                                cst,
                                lexer,
                                cst->tuple_items[variant_type->a + item],
                                0);
                        }
                    } else {
                        format_emit_expr(
                            sb, cst, lexer, variant->type_node_index, 0);
                    }
                    sb_append_char(sb, ')');
                }
            }
            sb_append_cstr(sb, " }");
        }
        break;
    case CK_TypeFn:
        format_emit_fn_signature(sb, cst, lexer, node->a, true);
        break;
    case CK_TypeTuple:
        sb_append_char(sb, '(');
        for (u32 i = 0; i < node->b; ++i) {
            if (i > 0) {
                sb_append_cstr(sb, ", ");
            }
            format_emit_expr(sb, cst, lexer, cst->tuple_items[node->a + i], 0);
        }
        if (node->b == 1) {
            sb_append_char(sb, ',');
        }
        sb_append_char(sb, ')');
        break;
    case CK_TypeArray:
        sb_append_char(sb, '[');
        format_emit_expr(sb, cst, lexer, node->a, 0);
        sb_append_char(sb, ']');
        format_emit_expr(sb, cst, lexer, node->b, 0);
        break;
    case CK_TypeSlice:
        sb_append_cstr(sb, "[]");
        format_emit_expr(sb, cst, lexer, node->a, 0);
        break;
    case CK_TypePointer:
        sb_append_char(sb, '^');
        format_emit_expr(sb, cst, lexer, node->a, node_precedence);
        break;
    case CK_TypePlex:
        {
            const CstPlexTypeInfo* plex     = &cst->plex_types[node->a];
            bool                   is_union = (plex->flags & CPTF_Union) != 0;
            sb_append_cstr(sb, is_union ? "union" : "plex");
            if (!is_union && (plex->flags & CPTF_Packed)) {
                sb_append_cstr(sb, " #packed");
            } else if (!is_union && (plex->flags & CPTF_C)) {
                sb_append_cstr(sb, " #c");
            }
            sb_append_cstr(sb, " {");
            for (u32 i = 0; i < plex->field_count; ++i) {
                const CstPlexField* field =
                    &cst->plex_fields[plex->first_field + i];
                sb_append_char(sb, ' ');
                sb_append_string(sb, lex_symbol(lexer, field->symbol_handle));
                sb_append_char(sb, ' ');
                format_emit_expr(sb, cst, lexer, field->type_node_index, 0);
            }
            sb_append_cstr(sb, " }");
        }
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
    case CK_FfiDef:
        format_emit_ffi_def(sb, cst, lexer, node->a);
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

internal void format_emit_expr_with_indent(StringBuilder* sb,
                                           const Cst*     cst,
                                           const Lexer*   lexer,
                                           u32            node_index,
                                           int            parent_precedence,
                                           u32            indent_level)
{
    u32 saved_indent           = g_format_expr_indent_level;
    g_format_expr_indent_level = indent_level;
    format_emit_expr(sb, cst, lexer, node_index, parent_precedence);
    g_format_expr_indent_level = saved_indent;
}

internal string format_render_on_branch_head(Arena*             arena,
                                             const Cst*         cst,
                                             const Lexer*       lexer,
                                             const CstOnBranch* branch)
{
    StringBuilder sb = {0};
    sb_init(&sb, arena);

    if (branch->flags & COBF_Else) {
        sb_append_cstr(&sb, "else");
    } else {
        for (u32 pattern = 0; pattern < branch->pattern_count; ++pattern) {
            if (pattern > 0) {
                sb_append_cstr(&sb, ", ");
            }
            format_emit_pattern(
                &sb,
                cst,
                lexer,
                cst->pattern_items[branch->pattern_index + pattern]);
        }
    }
    if (branch->binder_symbol_handle != U32_MAX) {
        sb_append_cstr(&sb, " as ");
        sb_append_string(&sb, lex_symbol(lexer, branch->binder_symbol_handle));
    }
    if (branch->guard_node_index != U32_MAX) {
        sb_append_cstr(&sb, " on ");
        format_emit_expr(&sb, cst, lexer, branch->guard_node_index, 0);
    }

    return sb_to_string(&sb);
}

internal void format_emit_on_block_multiline(StringBuilder* sb,
                                             const Cst*     cst,
                                             const Lexer*   lexer,
                                             u32            node_index,
                                             u32            indent_level)
{
    const CstNode*   node = &cst->nodes[node_index];
    const CstOnInfo* on   = &cst->ons[node->b];
    ASSERT(node->kind == CK_On &&
               (on->kind == COK_Value || on->kind == COK_Condition),
           "Expected block-form on expression");

    Arena branch_arena = {0};
    arena_init(&branch_arena);
    Array(string) heads  = NULL;
    usize max_head_width = 0;
    bool  has_binder     = false;
    for (u32 i = 0; i < on->branch_count; ++i) {
        const CstOnBranch* branch = &cst->on_branches[on->first_branch + i];
        if (branch->binder_symbol_handle != U32_MAX) {
            has_binder = true;
        }
        string head =
            format_render_on_branch_head(&branch_arena, cst, lexer, branch);
        array_push(heads, head);
        if (head.count > max_head_width) {
            max_head_width = head.count;
        }
    }

    sb_append_cstr(sb, "on");
    if (on->kind == COK_Value) {
        sb_append_char(sb, ' ');
        format_emit_expr(sb, cst, lexer, node->a, 0);
    }
    sb_append_cstr(sb, " {\n");
    for (u32 i = 0; i < on->branch_count; ++i) {
        const CstOnBranch* branch = &cst->on_branches[on->first_branch + i];
        format_emit_indent(sb, indent_level + 1);
        if (on->kind == COK_Condition && !(branch->flags & COBF_Else)) {
            format_emit_expr(sb, cst, lexer, branch->guard_node_index, 0);
        } else {
            sb_append_string(sb, heads[i]);
        }
        if (has_binder) {
            for (usize pad = heads[i].count; pad < max_head_width; ++pad) {
                sb_append_char(sb, ' ');
            }
        }
        sb_append_cstr(sb, " => ");
        format_emit_expr(sb, cst, lexer, branch->expr_node_index, 0);
        sb_append_char(sb, '\n');
    }
    format_emit_indent(sb, indent_level);
    sb_append_char(sb, '}');

    array_free(heads);
    arena_done(&branch_arena);
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
internal u32  format_node_end_token_index(const Cst*   cst,
                                          const Lexer* lexer,
                                          u32          node_index);
internal bool format_has_blank_line_between_statements(const Cst*   cst,
                                                       const Lexer* lexer,
                                                       u32 previous_node_index,
                                                       u32 current_node_index);

internal void format_emit_indent(StringBuilder* sb, u32 indent_level)
{
    for (u32 i = 0; i < indent_level; ++i) {
        sb_append_cstr(sb, "    ");
    }
}

internal bool format_node_is_block_form_on(const Cst* cst, u32 node_index)
{
    const CstNode* node = &cst->nodes[node_index];
    return node->kind == CK_On && cst->ons[node->b].kind != COK_Bool;
}

typedef struct {
    string symbol;
    string type;
    string value;
    bool   is_bind;
    bool   has_value;
    bool   uses_standard_single_line;
} FormatAlignedStatement;

internal bool format_node_is_function_value(const Cst* cst, u32 node_index)
{
    const CstNode* node = &cst->nodes[node_index];
    if (node->kind == CK_AnnotatedValue) {
        node = &cst->nodes[node->b];
    }
    return node->kind == CK_FnExpr || node->kind == CK_FnBlock;
}

internal string format_assignment_operator(const Lexer*   lexer,
                                           const CstNode* stmt)
{
    u32 op_index = stmt->token_index + 1;
    if (op_index >= array_count(lexer->tokens)) {
        return s("=");
    }

    switch (lexer->tokens[op_index].kind) {
    case TK_PlusEqual:
        return s("+=");
    case TK_MinusEqual:
        return s("-=");
    case TK_StarEqual:
        return s("*=");
    case TK_SlashEqual:
        return s("/=");
    case TK_PercentEqual:
        return s("%=");
    case TK_AmpEqual:
        return s("&=");
    case TK_CaretEqual:
        return s("^=");
    case TK_PipeEqual:
        return s("|=");
    case TK_AmpAmpEqual:
        return s("&&=");
    case TK_PipePipeEqual:
        return s("||=");
    default:
        return s("=");
    }
}

internal bool format_statement_is_function_binding(const Cst* cst,
                                                   u32        node_index)
{
    const CstNode* node = &cst->nodes[node_index];
    return node->kind == CK_Bind && format_node_is_function_value(cst, node->b);
}

internal u32 format_find_matching_close_token_index(const Lexer* lexer,
                                                    u32          open_index,
                                                    TokenKind    open_kind,
                                                    TokenKind    close_kind)
{
    u32 depth = 0;
    for (u32 i = open_index; i < array_count(lexer->tokens); ++i) {
        TokenKind kind = lexer->tokens[i].kind;
        if (kind == open_kind) {
            depth++;
        } else if (kind == close_kind) {
            if (depth == 0) {
                return i;
            }
            depth--;
            if (depth == 0) {
                return i;
            }
        }
    }

    return open_index;
}

internal u32 format_fn_signature_end_token_index(const Cst*   cst,
                                                 const Lexer* lexer,
                                                 u32          fn_token_index,
                                                 u32          signature_index)
{
    const CstFnSignature* signature   = &cst->fn_signatures[signature_index];
    u32                   paren_index = U32_MAX;

    for (u32 i = fn_token_index; i < array_count(lexer->tokens); ++i) {
        if (lexer->tokens[i].kind == TK_LParen) {
            paren_index = i;
            break;
        }
    }

    if (paren_index == U32_MAX) {
        return 0;
    }

    u32 close_index = format_find_matching_close_token_index(
        lexer, paren_index, TK_LParen, TK_RParen);
    if (signature->return_type_node_index == U32_MAX) {
        return close_index;
    }

    return format_node_end_token_index(
        cst, lexer, signature->return_type_node_index);
}

internal u32 format_find_interpolated_string_end_token_index(const Lexer* lexer,
                                                             u32 token_index)
{
    for (u32 i = token_index + 1; i < array_count(lexer->tokens); ++i) {
        if (lexer->tokens[i].kind == TK_InterpolatedStringEnd) {
            return i;
        }
    }

    return token_index;
}

internal u32 format_node_end_token_index(const Cst*   cst,
                                         const Lexer* lexer,
                                         u32          node_index)
{
    const CstNode* node = &cst->nodes[node_index];

    switch (node->kind) {
    case CK_IntegerLiteral:
    case CK_FloatLiteral:
    case CK_StringLiteral:
    case CK_BoolLiteral:
    case CK_SymbolRef:
        return node->token_index;
    case CK_InterpolatedString:
        return format_find_interpolated_string_end_token_index(
            lexer, node->token_index);
    case CK_Group:
    case CK_Tuple:
    case CK_TypeTuple:
        return format_find_matching_close_token_index(
            lexer, node->token_index, TK_LParen, TK_RParen);
    case CK_Plex:
    case CK_PlexUpdate:
    case CK_TypePlex:
    case CK_TypeEnum:
        return format_find_matching_close_token_index(
            lexer, node->token_index, TK_LBrace, TK_RBrace);
    case CK_TupleField:
    case CK_Field:
        return node->token_index;
    case CK_IntegerNegate:
    case CK_Statement:
        return format_node_end_token_index(cst, lexer, node->a);
    case CK_Return:
    case CK_ReturnExpr:
    case CK_Break:
    case CK_BreakExpr:
    case CK_Continue:
    case CK_ContinueExpr:
        return node->a == U32_MAX
                   ? (node->b == U32_MAX ? node->token_index
                                         : node->token_index + 2)
                   : format_node_end_token_index(cst, lexer, node->a);
    case CK_For:
        return format_node_end_token_index(cst, lexer, node->b);
    case CK_IntegerPlus:
    case CK_IntegerMinus:
    case CK_IntegerMultiply:
    case CK_IntegerDivide:
    case CK_IntegerModulo:
    case CK_StringConcat:
    case CK_RangeExclusive:
    case CK_RangeInclusive:
    case CK_AnnotatedValue:
    case CK_Bind:
    case CK_Variable:
    case CK_DestructureBind:
    case CK_DestructureVariable:
    case CK_DestructureAssign:
    case CK_Assign:
        return format_node_end_token_index(cst, lexer, node->b);
    case CK_Call:
    case CK_Slice:
    case CK_Index:
        return format_find_matching_close_token_index(
            lexer,
            node->token_index,
            node->kind == CK_Call ? TK_LParen : TK_LBracket,
            node->kind == CK_Call ? TK_RParen : TK_RBracket);
    case CK_Cast:
        for (u32 i = node->token_index + 1; i < array_count(lexer->tokens);
             ++i) {
            if (lexer->tokens[i].kind == TK_LParen) {
                return format_find_matching_close_token_index(
                    lexer, i, TK_LParen, TK_RParen);
            }
        }
        return node->token_index;
    case CK_On:
        {
            const CstOnInfo* on = &cst->ons[node->b];
            if (on->kind == COK_Bool) {
                u32                branch_offset = on->branch_count > 1 ? 1 : 0;
                const CstOnBranch* last_branch =
                    &cst->on_branches[on->first_branch + branch_offset];
                return format_node_end_token_index(
                    cst, lexer, last_branch->expr_node_index);
            }

            if (on->kind == COK_Condition) {
                for (u32 i = node->token_index + 1;
                     i < array_count(lexer->tokens);
                     ++i) {
                    if (lexer->tokens[i].kind == TK_LBrace) {
                        return format_find_matching_close_token_index(
                            lexer, i, TK_LBrace, TK_RBrace);
                    }
                }
                const CstOnBranch* last_branch =
                    &cst->on_branches[on->first_branch + on->branch_count - 1];
                return format_node_end_token_index(
                    cst, lexer, last_branch->expr_node_index);
            }

            u32 scrutinee_end =
                format_node_end_token_index(cst, lexer, node->a);
            for (u32 i = scrutinee_end + 1; i < array_count(lexer->tokens);
                 ++i) {
                if (lexer->tokens[i].kind == TK_LBrace) {
                    return format_find_matching_close_token_index(
                        lexer, i, TK_LBrace, TK_RBrace);
                }
            }

            const CstOnBranch* last_branch =
                &cst->on_branches[on->first_branch + on->branch_count - 1];
            return format_node_end_token_index(
                cst, lexer, last_branch->expr_node_index);
        }
    case CK_TypeFn:
        return format_fn_signature_end_token_index(
            cst, lexer, node->token_index, node->a);
    case CK_FnExpr:
        return format_node_end_token_index(cst, lexer, node->b);
    case CK_FnBlock:
        return format_node_end_token_index(cst, lexer, node->b);
    case CK_FfiDef:
        return format_fn_signature_end_token_index(
            cst,
            lexer,
            node->token_index,
            cst->ffi_infos[node->a].signature_index);
    case CK_ExprBlock:
        return format_node_end_token_index(cst, lexer, node->a);
    case CK_Block:
        return format_find_matching_close_token_index(
            lexer, node->token_index, TK_LBrace, TK_RBrace);
    case CK_ZeroInit:
        return format_node_end_token_index(cst, lexer, node->a);
    default:
        return node->token_index;
    }
}

internal bool format_has_blank_line_between_statements(const Cst*   cst,
                                                       const Lexer* lexer,
                                                       u32 previous_node_index,
                                                       u32 current_node_index)
{
    const CstNode* current            = &cst->nodes[current_node_index];
    u32            previous_end_line  = 0;
    u32            previous_end_col   = 0;
    u32            current_start_line = 0;
    u32            current_start_col  = 0;
    u32            previous_end_token_index =
        format_node_end_token_index(cst, lexer, previous_node_index);
    usize previous_end_offset =
        lex_token_end_offset(lexer, &lexer->tokens[previous_end_token_index]);
    if (previous_end_offset > 0) {
        previous_end_offset--;
    }

    if (!lex_offset_to_line_col(lexer->source,
                                previous_end_offset,
                                &previous_end_line,
                                &previous_end_col)) {
        return false;
    }

    if (!lex_offset_to_line_col(lexer->source,
                                lexer->tokens[current->token_index].offset,
                                &current_start_line,
                                &current_start_col)) {
        return false;
    }

    UNUSED(previous_end_col);
    UNUSED(current_start_col);
    return current_start_line > previous_end_line + 1;
}

internal bool format_statement_is_single_line(const Cst*   cst,
                                              const Lexer* lexer,
                                              u32          node_index)
{
    const CstNode* node       = &cst->nodes[node_index];
    u32            start_line = 0;
    u32            start_col  = 0;
    u32            end_line   = 0;
    u32            end_col    = 0;
    u32   end_token_index = format_node_end_token_index(cst, lexer, node_index);
    usize end_offset =
        lex_token_end_offset(lexer, &lexer->tokens[end_token_index]);
    if (end_offset > 0) {
        end_offset--;
    }

    if (!lex_offset_to_line_col(lexer->source,
                                lexer->tokens[node->token_index].offset,
                                &start_line,
                                &start_col)) {
        return false;
    }

    if (!lex_offset_to_line_col(
            lexer->source, end_offset, &end_line, &end_col)) {
        return false;
    }

    UNUSED(start_col);
    UNUSED(end_col);
    return start_line == end_line;
}

internal string format_render_expr_to_string(Arena*       arena,
                                             const Cst*   cst,
                                             const Lexer* lexer,
                                             u32          node_index)
{
    StringBuilder sb = {0};
    sb_init(&sb, arena);
    format_emit_expr(&sb, cst, lexer, node_index, 0);
    return sb_to_string(&sb);
}

internal string format_render_value_to_string(Arena*       arena,
                                              const Cst*   cst,
                                              const Lexer* lexer,
                                              u32          node_index)
{
    StringBuilder sb = {0};
    sb_init(&sb, arena);
    format_emit_value(&sb, cst, lexer, node_index);
    return sb_to_string(&sb);
}

internal void format_emit_type_plex_multiline(StringBuilder* sb,
                                              const Cst*     cst,
                                              const Lexer*   lexer,
                                              u32            node_index,
                                              u32            indent_level)
{
    const CstNode* node = &cst->nodes[node_index];
    ASSERT(node->kind == CK_TypePlex, "Expected plex type node");

    const CstPlexTypeInfo* plex            = &cst->plex_types[node->a];
    usize                  max_field_width = 0;
    for (u32 i = 0; i < plex->field_count; ++i) {
        const CstPlexField* field = &cst->plex_fields[plex->first_field + i];
        usize field_width = lex_symbol(lexer, field->symbol_handle).count;
        if (field_width > max_field_width) {
            max_field_width = field_width;
        }
    }

    bool is_union = (plex->flags & CPTF_Union) != 0;
    sb_append_cstr(sb, is_union ? "union" : "plex");
    if (!is_union && (plex->flags & CPTF_Packed)) {
        sb_append_cstr(sb, " #packed");
    } else if (!is_union && (plex->flags & CPTF_C)) {
        sb_append_cstr(sb, " #c");
    }
    sb_append_cstr(sb, " {\n");
    for (u32 i = 0; i < plex->field_count; ++i) {
        const CstPlexField* field = &cst->plex_fields[plex->first_field + i];
        string field_name         = lex_symbol(lexer, field->symbol_handle);
        format_emit_indent(sb, indent_level + 1);
        sb_append_string(sb, field_name);
        for (usize pad = field_name.count; pad <= max_field_width; ++pad) {
            sb_append_char(sb, ' ');
        }
        format_emit_expr(sb, cst, lexer, field->type_node_index, 0);
        sb_append_char(sb, '\n');
    }
    format_emit_indent(sb, indent_level);
    sb_append_char(sb, '}');
}

internal bool format_collect_aligned_statement(Arena*       arena,
                                               const Cst*   cst,
                                               const Lexer* lexer,
                                               u32          node_index,
                                               FormatAlignedStatement* out_stmt)
{
    const CstNode* node = &cst->nodes[node_index];
    if (node->kind == CK_Variable) {
        const CstNode* payload = &cst->nodes[node->b];
        string         type    = {0};
        string         value   = {0};
        bool           typed   = false;

        if (payload->kind == CK_AnnotatedValue) {
            typed = true;
            type  = format_render_expr_to_string(arena, cst, lexer, payload->a);
            value =
                format_render_value_to_string(arena, cst, lexer, payload->b);
        } else if (payload->kind == CK_ZeroInit) {
            typed = true;
            type  = format_render_expr_to_string(arena, cst, lexer, payload->a);
        } else {
            value = format_render_expr_to_string(arena, cst, lexer, node->b);
        }

        *out_stmt = (FormatAlignedStatement){
            .symbol    = lex_symbol(lexer, cst_get_symbol(node)),
            .type      = type,
            .value     = value,
            .is_bind   = false,
            .has_value = payload->kind != CK_ZeroInit,
            .uses_standard_single_line = !typed,
        };
        return true;
    }

    if (node->kind == CK_Bind) {
        const CstNode* payload = &cst->nodes[node->b];
        string         type    = {0};
        string         value   = {0};
        bool           typed   = false;

        if (!format_statement_is_single_line(cst, lexer, node_index)) {
            return false;
        }

        if (payload->kind == CK_AnnotatedValue) {
            typed = true;
            type  = format_render_expr_to_string(arena, cst, lexer, payload->a);
            value =
                format_render_value_to_string(arena, cst, lexer, payload->b);
            payload = &cst->nodes[payload->b];
        } else {
            value = format_render_value_to_string(arena, cst, lexer, node->b);
        }

        if (payload->kind == CK_FnExpr || payload->kind == CK_FnBlock ||
            payload->kind == CK_FfiDef || payload->kind == CK_For ||
            payload->kind == CK_ExprBlock || payload->kind == CK_TypePlex) {
            return false;
        }

        *out_stmt = (FormatAlignedStatement){
            .symbol    = lex_symbol(lexer, cst_get_symbol(node)),
            .type      = type,
            .value     = value,
            .is_bind   = true,
            .has_value = true,
            .uses_standard_single_line = !typed,
        };
        return true;
    }

    return false;
}

internal bool format_node_is_owned_by_later_statement(const Cst* cst,
                                                      u32        node_index,
                                                      u32        end,
                                                      u32        current_block)
{
    if (cst->nodes[node_index].kind == CK_Block) {
        for (u32 owner_index = 0; owner_index < end; ++owner_index) {
            const CstNode* owner = &cst->nodes[owner_index];
            if (owner->kind == CK_For &&
                (owner->b == node_index ||
                 cst->fors[owner->a].else_block_index == node_index)) {
                return true;
            }
            if (owner->kind == CK_ExprBlock && owner->b == node_index) {
                return true;
            }
        }
    }

    for (u32 owner_index = 0; owner_index < end; ++owner_index) {
        const CstNode* owner = &cst->nodes[owner_index];
        if (owner->kind != CK_For) {
            continue;
        }
        const CstForInfo* for_info = &cst->fors[owner->a];
        for (u32 item = 0; item < for_info->init_count; ++item) {
            if (cst->for_items[for_info->first_init + item] == node_index) {
                return true;
            }
        }
        for (u32 item = 0; item < for_info->update_count; ++item) {
            if (cst->for_items[for_info->first_update + item] == node_index) {
                return true;
            }
        }
    }

    for (u32 i = 0; i < end; ++i) {
        const CstNode* block = &cst->nodes[i];
        if (i == current_block || block->kind != CK_Block ||
            !(block->a <= node_index && node_index < block->b)) {
            continue;
        }
        for (u32 owner_index = 0; owner_index < end; ++owner_index) {
            const CstNode* owner = &cst->nodes[owner_index];
            if (owner->kind == CK_For &&
                (owner->b == i || cst->fors[owner->a].else_block_index == i)) {
                return true;
            }
            if (owner->kind == CK_For) {
                const CstForInfo* for_info = &cst->fors[owner->a];
                for (u32 item = 0; item < for_info->init_count; ++item) {
                    if (cst->for_items[for_info->first_init + item] == i) {
                        return true;
                    }
                }
                for (u32 item = 0; item < for_info->update_count; ++item) {
                    if (cst->for_items[for_info->first_update + item] == i) {
                        return true;
                    }
                }
            }
            if (owner->kind == CK_ExprBlock && owner->b == i) {
                return true;
            }
        }
    }

    for (u32 i = node_index + 1; i < end; ++i) {
        const CstNode* node = &cst->nodes[i];
        if (node->kind == CK_Bind || node->kind == CK_Variable) {
            const CstNode* payload = &cst->nodes[node->b];
            if (node->b == node_index) {
                return true;
            }
            if (payload->kind == CK_AnnotatedValue &&
                payload->b == node_index) {
                return true;
            }
            continue;
        }
        if ((node->kind == CK_For || node->kind == CK_ExprBlock) &&
            node->b == node_index) {
            return true;
        }
        if (node->kind == CK_Statement && node->a == node_index) {
            return true;
        }
    }

    return false;
}

internal u32 format_next_block_statement(const Cst* cst,
                                         u32        start,
                                         u32        end,
                                         u32        current_block)
{
    for (u32 i = start; i < end; ++i) {
        if (cst_node_is_block_statement(&cst->nodes[i]) &&
            !format_node_is_owned_by_later_statement(
                cst, i, end, current_block)) {
            return i;
        }
    }

    return U32_MAX;
}

internal bool format_aligned_statements_same_family(FormatAlignedStatement a,
                                                    FormatAlignedStatement b)
{
    return a.is_bind == b.is_bind;
}

internal void
format_emit_aligned_statement_group(StringBuilder*                sb,
                                    const FormatAlignedStatement* stmts,
                                    u32                           stmt_count,
                                    u32                           indent_level)
{
    usize max_symbol_width = 0;
    usize max_type_width   = 0;
    bool  has_variables    = false;
    for (u32 i = 0; i < stmt_count; ++i) {
        if (stmts[i].symbol.count > max_symbol_width) {
            max_symbol_width = stmts[i].symbol.count;
        }
        if (!stmts[i].is_bind) {
            has_variables = true;
        }
        if (stmts[i].type.count > max_type_width) {
            max_type_width = stmts[i].type.count;
        }
    }
    bool has_type_column = has_variables || max_type_width > 0;

    for (u32 i = 0; i < stmt_count; ++i) {
        format_emit_indent(sb, indent_level);
        sb_append_string(sb, stmts[i].symbol);
        for (usize pad = stmts[i].symbol.count; pad < max_symbol_width; ++pad) {
            sb_append_char(sb, ' ');
        }

        if (!stmts[i].is_bind || has_type_column) {
            sb_append_cstr(sb, " : ");
            sb_append_string(sb, stmts[i].type);
            if (!stmts[i].has_value) {
                sb_append_char(sb, '\n');
                continue;
            }
            for (usize pad = stmts[i].type.count; pad <= max_type_width;
                 ++pad) {
                sb_append_char(sb, ' ');
            }
            usize value_start_width =
                (usize)indent_level * 4 + max_symbol_width + max_type_width + 6;
            if (value_start_width + stmts[i].value.count <= FORMAT_WRAP_WIDTH) {
                sb_append_char(sb, stmts[i].is_bind ? ':' : '=');
                sb_append_char(sb, ' ');
                sb_append_string(sb, stmts[i].value);
            } else {
                sb_append_char(sb, stmts[i].is_bind ? ':' : '=');
                sb_append_char(sb, '\n');
                format_emit_indent(sb, indent_level + 1);
                sb_append_string(sb, stmts[i].value);
            }
        } else {
            usize value_start_width =
                (usize)indent_level * 4 + max_symbol_width + 4;
            if (value_start_width + stmts[i].value.count <= FORMAT_WRAP_WIDTH) {
                sb_append_cstr(sb, " :: ");
                sb_append_string(sb, stmts[i].value);
            } else {
                sb_append_cstr(sb, " ::");
                sb_append_char(sb, '\n');
                format_emit_indent(sb, indent_level + 1);
                sb_append_string(sb, stmts[i].value);
            }
        }
        sb_append_char(sb, '\n');
    }
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

internal void format_emit_ffi_def(StringBuilder* sb,
                                  const Cst*     cst,
                                  const Lexer*   lexer,
                                  u32            ffi_info_index)
{
    const CstFfiInfo*     ffi       = &cst->ffi_infos[ffi_info_index];
    const CstFnSignature* signature = &cst->fn_signatures[ffi->signature_index];

    sb_append_cstr(sb, "ffi \"");
    format_emit_string_text(sb, lexer->strings[ffi->library_string_index]);
    sb_append_cstr(sb, "\" (");
    for (u32 i = 0; i < signature->param_count; ++i) {
        if (i > 0) {
            sb_append_cstr(sb, ", ");
        }
        const CstParam* param = &cst->params[signature->first_param + i];
        format_emit_expr(sb, cst, lexer, param->type_node_index, 0);
    }
    sb_append_char(sb, ')');
    if (signature->return_type_node_index != U32_MAX) {
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
    u32   previous_statement_index = U32_MAX;
    Arena align_arena              = {0};
    arena_init(&align_arena);

    for (u32 i = block->a; i < block->b; ++i) {
        if (!cst_node_is_block_statement(&cst->nodes[i]) ||
            format_node_is_owned_by_later_statement(
                cst, i, block->b, block_node_index)) {
            continue;
        }

        if (previous_statement_index != U32_MAX &&
            format_has_blank_line_between_statements(
                cst, lexer, previous_statement_index, i)) {
            sb_append_char(sb, '\n');
        }
        if (previous_statement_index != U32_MAX &&
            format_statement_is_function_binding(cst, i) &&
            !format_has_blank_line_between_statements(
                cst, lexer, previous_statement_index, i)) {
            sb_append_char(sb, '\n');
        }

        FormatAlignedStatement first_aligned = {0};
        if (format_collect_aligned_statement(
                &align_arena, cst, lexer, i, &first_aligned)) {
            Array(FormatAlignedStatement) aligned = NULL;
            array_push(aligned, first_aligned);

            u32 last_aligned_index = i;
            u32 cursor             = i + 1;
            while (true) {
                u32 next_statement = format_next_block_statement(
                    cst, cursor, block->b, block_node_index);
                if (next_statement == U32_MAX ||
                    format_has_blank_line_between_statements(
                        cst, lexer, last_aligned_index, next_statement)) {
                    break;
                }

                FormatAlignedStatement next_aligned = {0};
                if (!format_collect_aligned_statement(&align_arena,
                                                      cst,
                                                      lexer,
                                                      next_statement,
                                                      &next_aligned)) {
                    break;
                }
                if (!format_aligned_statements_same_family(first_aligned,
                                                           next_aligned)) {
                    break;
                }

                array_push(aligned, next_aligned);
                last_aligned_index = next_statement;
                cursor             = next_statement + 1;
            }

            if (array_count(aligned) > 1 ||
                !aligned[0].uses_standard_single_line) {
                format_emit_aligned_statement_group(
                    sb, aligned, (u32)array_count(aligned), indent_level);
                previous_statement_index = last_aligned_index;
                i                        = last_aligned_index;

                u32 next_statement       = format_next_block_statement(
                    cst, i + 1, block->b, block_node_index);
                if (array_count(aligned) > 1 && next_statement != U32_MAX &&
                    !format_has_blank_line_between_statements(
                        cst, lexer, last_aligned_index, next_statement)) {
                    FormatAlignedStatement ignored = {0};
                    if (!format_collect_aligned_statement(&align_arena,
                                                          cst,
                                                          lexer,
                                                          next_statement,
                                                          &ignored)) {
                        sb_append_char(sb, '\n');
                    } else if (!format_aligned_statements_same_family(
                                   first_aligned, ignored)) {
                        sb_append_char(sb, '\n');
                    }
                }

                array_free(aligned);
                continue;
            }

            array_free(aligned);
        }

        format_emit_block_statement(sb, cst, lexer, i, indent_level);
        if (format_statement_is_function_binding(cst, i)) {
            u32 next_statement = format_next_block_statement(
                cst, i + 1, block->b, block_node_index);
            if (next_statement != U32_MAX &&
                !format_has_blank_line_between_statements(
                    cst, lexer, i, next_statement)) {
                sb_append_char(sb, '\n');
            }
        }
        FormatAlignedStatement current_aligned = {0};
        if (format_collect_aligned_statement(
                &align_arena, cst, lexer, i, &current_aligned)) {
            u32 next_statement = format_next_block_statement(
                cst, i + 1, block->b, block_node_index);
            if (next_statement != U32_MAX &&
                !format_has_blank_line_between_statements(
                    cst, lexer, i, next_statement)) {
                FormatAlignedStatement next_aligned = {0};
                if (format_collect_aligned_statement(&align_arena,
                                                     cst,
                                                     lexer,
                                                     next_statement,
                                                     &next_aligned) &&
                    !format_aligned_statements_same_family(current_aligned,
                                                           next_aligned)) {
                    sb_append_char(sb, '\n');
                }
            }
        }
        previous_statement_index = i;
        i = cst_block_statement_end_exclusive(cst, i) - 1;
    }

    arena_done(&align_arena);
}

internal bool format_emit_call_with_block_on_arg(StringBuilder* sb,
                                                 const Cst*     cst,
                                                 const Lexer*   lexer,
                                                 u32            node_index,
                                                 u32            indent_level)
{
    const CstNode* node = &cst->nodes[node_index];
    if (node->kind != CK_Call) {
        return false;
    }

    const CstCallInfo* call = &cst->calls[node->b];
    if (call->arg_count != 1) {
        return false;
    }

    u32 arg_node = cst->call_args[call->first_arg];
    if (!format_node_is_block_form_on(cst, arg_node)) {
        return false;
    }

    format_emit_expr(sb, cst, lexer, node->a, 0);
    sb_append_char(sb, '(');
    format_emit_on_block_multiline(sb, cst, lexer, arg_node, indent_level);
    sb_append_char(sb, ')');
    return true;
}

internal void format_emit_for_header_item(StringBuilder* sb,
                                          const Cst*     cst,
                                          const Lexer*   lexer,
                                          u32            node_index)
{
    const CstNode* item = &cst->nodes[node_index];
    if (item->kind == CK_Variable) {
        sb_append_string(sb, lex_symbol(lexer, cst_get_symbol(item)));
        if (cst->nodes[item->b].kind == CK_AnnotatedValue ||
            cst->nodes[item->b].kind == CK_ZeroInit) {
            sb_append_cstr(sb, ": ");
            format_emit_variable_payload(sb, cst, lexer, item->b);
        } else {
            sb_append_cstr(sb, " := ");
            format_emit_expr(sb, cst, lexer, item->b, 0);
        }
        return;
    }
    if (item->kind == CK_Bind) {
        sb_append_string(sb, lex_symbol(lexer, cst_get_symbol(item)));
        sb_append_cstr(
            sb, cst->nodes[item->b].kind == CK_AnnotatedValue ? " : " : " :: ");
        if (cst->nodes[item->b].kind == CK_TypePlex) {
            format_emit_type_plex_multiline(sb, cst, lexer, item->b, 0);
        } else {
            format_emit_value(sb, cst, lexer, item->b);
        }
        return;
    }
    if (item->kind == CK_Assign) {
        sb_append_string(sb, lex_symbol(lexer, cst_get_symbol(item)));
        sb_append_char(sb, ' ');
        sb_append_string(sb, format_assignment_operator(lexer, item));
        sb_append_char(sb, ' ');
        format_emit_expr(sb, cst, lexer, item->b, 0);
        return;
    }
    ASSERT(item->kind == CK_Statement, "Expected for header statement item");
    format_emit_expr(sb, cst, lexer, item->a, 0);
}

internal void format_emit_for_header_items(StringBuilder* sb,
                                           const Cst*     cst,
                                           const Lexer*   lexer,
                                           u32            first_item,
                                           u32            item_count)
{
    for (u32 i = 0; i < item_count; ++i) {
        if (i > 0) {
            sb_append_cstr(sb, ", ");
        }
        format_emit_for_header_item(
            sb, cst, lexer, cst->for_items[first_item + i]);
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
        format_emit_block_contents(
            sb, cst, lexer, node_index, indent_level + 1);
        format_emit_indent(sb, indent_level);
        sb_append_cstr(sb, "}\n");
        return;
    }

    if (stmt->kind == CK_For) {
#if CONFIG_DEBUG
        const CstNode* body = &cst->nodes[stmt->b];
        ASSERT(body->kind == CK_Block, "Expected for body block");
#endif
        const CstForInfo* for_info = &cst->fors[stmt->a];
        sb_append_cstr(sb, "for");
        bool is_c_style =
            for_info->init_count > 0 || for_info->update_count > 0;
        if (is_c_style) {
            sb_append_char(sb, ' ');
            if (for_info->init_count > 0) {
                format_emit_for_header_items(
                    sb, cst, lexer, for_info->first_init, for_info->init_count);
            }
            sb_append_cstr(sb, "; ");
            if (for_info->condition_node_index != U32_MAX) {
                format_emit_expr(
                    sb, cst, lexer, for_info->condition_node_index, 0);
            }
            sb_append_cstr(sb, "; ");
            if (for_info->update_count > 0) {
                format_emit_for_header_items(sb,
                                             cst,
                                             lexer,
                                             for_info->first_update,
                                             for_info->update_count);
            }
        } else if (for_info->condition_node_index != U32_MAX) {
            sb_append_char(sb, ' ');
            format_emit_expr(sb, cst, lexer, for_info->condition_node_index, 0);
        }
        if (for_info->label_symbol != U32_MAX) {
            sb_append_cstr(sb, " $");
            sb_append_string(sb, lex_symbol(lexer, for_info->label_symbol));
        }
        sb_append_cstr(sb, " {\n");
        format_emit_block_contents(sb, cst, lexer, stmt->b, indent_level + 1);
        format_emit_indent(sb, indent_level);
        sb_append_cstr(sb, "}\n");
        if (for_info->else_block_index != U32_MAX) {
            format_emit_indent(sb, indent_level);
            sb_append_cstr(sb, "else {\n");
            format_emit_block_contents(
                sb, cst, lexer, for_info->else_block_index, indent_level + 1);
            format_emit_indent(sb, indent_level);
            sb_append_cstr(sb, "}\n");
        }
        return;
    }

    if (stmt->kind == CK_Return) {
        sb_append_cstr(sb, "return");
        if (stmt->a != U32_MAX) {
            if (format_node_is_block_form_on(cst, stmt->a)) {
                sb_append_char(sb, ' ');
                format_emit_on_block_multiline(
                    sb, cst, lexer, stmt->a, indent_level);
            } else {
                sb_append_char(sb, ' ');
                format_emit_expr_with_indent(
                    sb, cst, lexer, stmt->a, 0, indent_level);
            }
        }
        sb_append_char(sb, '\n');
        return;
    }

    if (stmt->kind == CK_Break || stmt->kind == CK_Continue) {
        sb_append_cstr(sb, stmt->kind == CK_Break ? "break" : "continue");
        if (stmt->b != U32_MAX) {
            sb_append_cstr(sb, " $");
            sb_append_string(sb, lex_symbol(lexer, stmt->b));
        }
        if (stmt->a != U32_MAX) {
            sb_append_char(sb, ' ');
            format_emit_expr_with_indent(
                sb, cst, lexer, stmt->a, 0, indent_level);
        }
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
            format_emit_expr_with_indent(
                sb, cst, lexer, stmt->b, 0, indent_level);
        }
        sb_append_char(sb, '\n');
        return;
    }

    if (stmt->kind == CK_Bind) {
        sb_append_string(sb, lex_symbol(lexer, cst_get_symbol(stmt)));
        sb_append_cstr(
            sb, cst->nodes[stmt->b].kind == CK_AnnotatedValue ? " : " : " :: ");
        if (cst->nodes[stmt->b].kind == CK_TypePlex) {
            format_emit_type_plex_multiline(
                sb, cst, lexer, stmt->b, indent_level);
        } else {
            format_emit_value(sb, cst, lexer, stmt->b);
        }
        sb_append_char(sb, '\n');
        return;
    }

    if (stmt->kind == CK_DestructureBind) {
        format_emit_pattern(sb, cst, lexer, stmt->a);
        sb_append_cstr(
            sb, cst->nodes[stmt->b].kind == CK_AnnotatedValue ? " : " : " :: ");
        format_emit_value(sb, cst, lexer, stmt->b);
        sb_append_char(sb, '\n');
        return;
    }

    if (stmt->kind == CK_DestructureVariable) {
        format_emit_pattern(sb, cst, lexer, stmt->a);
        if (cst->nodes[stmt->b].kind == CK_AnnotatedValue) {
            sb_append_cstr(sb, ": ");
            format_emit_variable_payload(sb, cst, lexer, stmt->b);
        } else {
            sb_append_cstr(sb, " := ");
            format_emit_expr_with_indent(
                sb, cst, lexer, stmt->b, 0, indent_level);
        }
        sb_append_char(sb, '\n');
        return;
    }

    if (stmt->kind == CK_DestructureAssign) {
        format_emit_pattern(sb, cst, lexer, stmt->a);
        sb_append_cstr(sb, " = ");
        format_emit_expr_with_indent(sb, cst, lexer, stmt->b, 0, indent_level);
        sb_append_char(sb, '\n');
        return;
    }

    if (stmt->kind == CK_Assign) {
        sb_append_string(sb, lex_symbol(lexer, cst_get_symbol(stmt)));
        sb_append_char(sb, ' ');
        sb_append_string(sb, format_assignment_operator(lexer, stmt));
        sb_append_char(sb, ' ');
        format_emit_expr_with_indent(sb, cst, lexer, stmt->b, 0, indent_level);
        sb_append_char(sb, '\n');
        return;
    }

    if (stmt->kind == CK_Statement) {
        if (format_node_is_block_form_on(cst, stmt->a)) {
            format_emit_on_block_multiline(
                sb, cst, lexer, stmt->a, indent_level);
        } else if (!format_emit_call_with_block_on_arg(
                       sb, cst, lexer, stmt->a, indent_level)) {
            format_emit_expr_with_indent(
                sb, cst, lexer, stmt->a, 0, indent_level);
        }
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
        if (format_node_is_block_form_on(cst, node->b)) {
            sb_append_cstr(sb, " =>\n");
            format_emit_indent(sb, 1);
            format_emit_on_block_multiline(sb, cst, lexer, node->b, 1);
        } else {
            sb_append_cstr(sb, " => ");
            format_emit_expr(sb, cst, lexer, node->b, 0);
        }
        break;
    case CK_FnBlock:
        format_emit_fn_signature(sb, cst, lexer, node->a, true);
        sb_append_cstr(sb, " {\n");
        format_emit_block_contents(sb, cst, lexer, node->b, 1);
        sb_append_cstr(sb, "}");
        break;
    case CK_FfiDef:
        format_emit_ffi_def(sb, cst, lexer, node->a);
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

    Arena align_arena = {0};
    arena_init(&align_arena);

    bool first_binding = true;
    for (u32 i = 0; i < array_count(cst.bindings); ++i) {
        u32            node_index = cst.bindings[i];
        const CstNode* node       = &cst.nodes[node_index];

        if (!first_binding) {
            sb_append_char(sb, '\n');
        }

        FormatAlignedStatement first_aligned = {0};
        if (format_collect_aligned_statement(
                &align_arena, &cst, &lexer, node_index, &first_aligned)) {
            Array(FormatAlignedStatement) aligned = NULL;
            array_push(aligned, first_aligned);

            u32 last_aligned_binding = i;
            for (u32 cursor = i + 1; cursor < array_count(cst.bindings);
                 ++cursor) {
                u32 next_index = cst.bindings[cursor];
                if (format_has_blank_line_between_statements(
                        &cst,
                        &lexer,
                        cst.bindings[last_aligned_binding],
                        next_index)) {
                    break;
                }

                FormatAlignedStatement next_aligned = {0};
                if (!format_collect_aligned_statement(&align_arena,
                                                      &cst,
                                                      &lexer,
                                                      next_index,
                                                      &next_aligned) ||
                    !format_aligned_statements_same_family(first_aligned,
                                                           next_aligned)) {
                    break;
                }

                array_push(aligned, next_aligned);
                last_aligned_binding = cursor;
            }

            if (array_count(aligned) > 1 && !aligned[0].is_bind) {
                format_emit_aligned_statement_group(
                    sb, aligned, (u32)array_count(aligned), 0);
                i             = last_aligned_binding;
                first_binding = false;
                array_free(aligned);
                continue;
            }

            array_free(aligned);
        }

        sb_append_string(sb, lex_symbol(&lexer, cst_get_symbol(node)));
        if (node->kind == CK_Bind) {
            sb_append_cstr(
                sb,
                cst.nodes[node->b].kind == CK_AnnotatedValue ? " : " : " :: ");
            if (cst.nodes[node->b].kind == CK_TypePlex) {
                format_emit_type_plex_multiline(sb, &cst, &lexer, node->b, 0);
            } else {
                format_emit_value(sb, &cst, &lexer, node->b);
            }
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

    arena_done(&align_arena);
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
    const usize  wrap_width              = FORMAT_WRAP_WIDTH;
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
// Format one source file into an arena-owned string.

bool format_file_to_string(cstr input_path, Arena* arena, string* out_text)
{
    FileMap map  = {0};
    string  text = filemap_load(input_path, &map);
    if (text.data == NULL) {
        return false;
    }

    bool ok = format_source(
        (NerdSource){
            .source      = text,
            .source_path = s(input_path),
        },
        arena,
        out_text);

    filemap_unload(&map);
    return ok;
}

//------------------------------------------------------------------------------
// Format one source file and save the result to the given output path.

bool format_file(cstr input_path, cstr output_path)
{
    Arena arena = {0};
    arena_init(&arena);

    string rendered = {0};
    bool   ok       = format_file_to_string(input_path, &arena, &rendered);
    if (!ok) {
        arena_done(&arena);
        return false;
    }

    FILE* file = fopen(output_path, "wb");
    if (!file) {
        arena_done(&arena);
        return error_runtime("Failed to open file for writing: %s",
                             output_path);
    }

    usize written      = fwrite(rendered.data, 1, rendered.count, file);
    bool  close_failed = fclose(file) != 0;
    if (written != rendered.count || close_failed) {
        arena_done(&arena);
        return error_runtime("Failed to write formatted file: %s", output_path);
    }

    arena_done(&arena);
    return true;
}

//------------------------------------------------------------------------------
