//------------------------------------------------------------------------------
// Source formatting implementation
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/cst/cst.h>
#include <compiler/error/error.h>
#include <compiler/format/format.h>
#include <compiler/internal.h>
#include <compiler/lexer/lexer.h>

#include <stdio.h>

#define FORMAT_WRAP_WIDTH 80

typedef struct {
    Array(u16) newlines_before_token;
    Array(u32) first_comment_before_token;
    Array(u16) comment_count_before_token;
    Array(u32) trailing_comment_index_by_token;
} FormatTrivia;

global_variable const FormatTrivia* g_format_trivia = NULL;

internal void  format_emit_for_header_items(StringBuilder* sb,
                                            const Cst*     cst,
                                            const Lexer*   lexer,
                                            u32            first_item,
                                            u32            item_count);
internal void  format_emit_indent(StringBuilder* sb, u32 indent_level);
internal void  format_emit_top_on(StringBuilder* sb,
                                  const Cst*     cst,
                                  const Lexer*   lexer,
                                  u32            top_on_index,
                                  u32            indent_level);
internal void  format_emit_impl(StringBuilder* sb,
                                const Cst*     cst,
                                const Lexer*   lexer,
                                u32            impl_index,
                                u32            indent_level);
internal void  format_emit_test(StringBuilder* sb,
                                const Cst*     cst,
                                const Lexer*   lexer,
                                u32            node_index,
                                u32            indent_level);
internal bool  format_node_is_block_form_on(const Cst* cst, u32 node_index);
internal bool  format_plex_field_is_shorthand(const Cst*                 cst,
                                              const CstPlexLiteralField* field);
internal void  format_emit_on_block_multiline(StringBuilder* sb,
                                              const Cst*     cst,
                                              const Lexer*   lexer,
                                              u32            node_index,
                                              u32            indent_level);
internal usize format_rendered_expr_width(const Cst*   cst,
                                          const Lexer* lexer,
                                          u32          node_index,
                                          u32          indent_level);
internal void  format_advance_delimiter_depth(const Lexer* lexer,
                                              u32*         io_token_index,
                                              usize        end_offset,
                                              i32*         io_depth);
internal bool  format_emit_block_comments_before_offset(StringBuilder* sb,
                                                        const Lexer*   lexer,
                                                        u32*   io_comment_index,
                                                        usize  end_offset,
                                                        u32    indent_level,
                                                        usize* out_last_end);
internal bool  format_emit_block_comments_before_token(StringBuilder* sb,
                                                       const Lexer*   lexer,
                                                       u32*   io_comment_index,
                                                       u32    token_index,
                                                       u32    indent_level,
                                                       usize* out_last_end);
internal void  format_skip_block_comments_before_offset(const Lexer* lexer,
                                                        u32*  io_comment_index,
                                                        usize end_offset);
internal bool  format_comment_is_trailing_after_offset(NerdSource   source,
                                                       usize        end_offset,
                                                       LexerComment comment);
internal bool  format_node_has_trailing_comment(const Cst*   cst,
                                                const Lexer* lexer,
                                                u32          node_index);
internal bool  format_emit_trailing_comment_after_offset(StringBuilder* sb,
                                                         const Lexer*   lexer,
                                                         u32*  io_comment_index,
                                                         usize end_offset);
internal bool  format_emit_trailing_comment_after_token(StringBuilder* sb,
                                                        const Lexer*   lexer,
                                                        u32* io_comment_index,
                                                        u32  token_index);
internal bool  format_emit_trailing_comment_by_index(StringBuilder* sb,
                                                     const Lexer*   lexer,
                                                     u32* io_comment_index,
                                                     u32  comment_index);
internal bool
format_find_trailing_comment_index_after_offset(NerdSource   source,
                                                const Lexer* lexer,
                                                usize        end_offset,
                                                u32*         out_comment_index);
internal bool
format_find_trailing_comment_index_after_offset_scan(NerdSource   source,
                                                     const Lexer* lexer,
                                                     usize        end_offset,
                                                     u32* out_comment_index);
internal void format_emit_trailing_comment_text_aligned(StringBuilder* sb,
                                                        string comment_text,
                                                        usize  comment_column,
                                                        usize  current_column);
internal string
format_merged_trailing_comment_text(const Lexer* lexer,
                                    u32          first_comment_index,
                                    usize        comment_column,
                                    usize        before_offset,
                                    u32*         out_next_comment_index);
internal usize format_trailing_comment_group_end_offset(const Lexer* lexer,
                                                        u32 first_comment_index,
                                                        usize before_offset);
internal bool  format_emit_trailing_comment_for_node(StringBuilder* sb,
                                                     const Cst*     cst,
                                                     const Lexer*   lexer,
                                                     u32            node_index,
                                                     u32* io_comment_index);

//------------------------------------------------------------------------------
// Count newlines in a source range, saturating to the storage size used by
// formatter trivia.

internal u16 format_count_newlines_between(string source,
                                           usize  start,
                                           usize  end)
{
    if (end > source.count) {
        end = source.count;
    }
    if (start > end) {
        start = end;
    }

    u32 count = 0;
    for (usize i = start; i < end; ++i) {
        if (source.data[i] == '\n') {
            count++;
            if (count == U16_MAX) {
                break;
            }
        }
    }
    return (u16)count;
}

//------------------------------------------------------------------------------
// Build token-indexed trivia tables for a future token-stream formatter. The
// final slot is an EOF slot used for trivia after the last real token.

internal void format_trivia_build(const Lexer* lexer, FormatTrivia* out_trivia)
{
    *out_trivia     = (FormatTrivia){0};

    u32 token_count = (u32)array_count(lexer->tokens);
    u32 slot_count  = token_count + 1;
    array_requires_size(out_trivia->newlines_before_token, slot_count);
    array_requires_size(out_trivia->first_comment_before_token, slot_count);
    array_requires_size(out_trivia->comment_count_before_token, slot_count);
    array_requires_size(out_trivia->trailing_comment_index_by_token,
                        slot_count);

    for (u32 i = 0; i < slot_count; ++i) {
        out_trivia->newlines_before_token[i]           = 0;
        out_trivia->first_comment_before_token[i]      = U32_MAX;
        out_trivia->comment_count_before_token[i]      = 0;
        out_trivia->trailing_comment_index_by_token[i] = U32_MAX;
    }

    string source = lexer->source.source;
    for (u32 i = 0; i < token_count; ++i) {
        usize previous_end =
            i == 0 ? 0 : lex_token_end_offset(lexer, &lexer->tokens[i - 1]);
        usize current_start = lexer->tokens[i].offset;
        out_trivia->newlines_before_token[i] =
            format_count_newlines_between(source, previous_end, current_start);
    }

    usize final_start =
        token_count == 0
            ? 0
            : lex_token_end_offset(lexer, &lexer->tokens[token_count - 1]);
    out_trivia->newlines_before_token[token_count] =
        format_count_newlines_between(source, final_start, source.count);

    for (u32 i = 0; i < array_count(lexer->comments); ++i) {
        const LexerComment* comment = &lexer->comments[i];
        u32                 slot    = comment->token_index;
        if (slot > token_count) {
            slot = token_count;
        }

        if (slot > 0) {
            usize previous_end =
                lex_token_end_offset(lexer, &lexer->tokens[slot - 1]);
            if (format_comment_is_trailing_after_offset(
                    lexer->source, previous_end, *comment)) {
                out_trivia->trailing_comment_index_by_token[slot - 1] = i;
                continue;
            }
        }

        if (out_trivia->first_comment_before_token[slot] == U32_MAX) {
            out_trivia->first_comment_before_token[slot] = i;
        }
        if (out_trivia->comment_count_before_token[slot] < U16_MAX) {
            out_trivia->comment_count_before_token[slot]++;
        }
    }
}

internal void format_trivia_done(FormatTrivia* trivia)
{
    array_free(trivia->newlines_before_token);
    array_free(trivia->first_comment_before_token);
    array_free(trivia->comment_count_before_token);
    array_free(trivia->trailing_comment_index_by_token);
    *trivia = (FormatTrivia){0};
}

internal bool format_trivia_trailing_comment_after_token(
    const FormatTrivia* trivia, u32 token_index, u32* out_comment_index)
{
    if (trivia == NULL ||
        token_index >= array_count(trivia->trailing_comment_index_by_token)) {
        return false;
    }

    u32 comment_index = trivia->trailing_comment_index_by_token[token_index];
    if (comment_index == U32_MAX) {
        return false;
    }

    if (out_comment_index != NULL) {
        *out_comment_index = comment_index;
    }
    return true;
}

internal bool format_trivia_comments_before_token(const FormatTrivia* trivia,
                                                  u32  token_index,
                                                  u32* out_first_comment_index,
                                                  u32* out_comment_count)
{
    if (trivia == NULL ||
        token_index >= array_count(trivia->first_comment_before_token) ||
        token_index >= array_count(trivia->comment_count_before_token)) {
        return false;
    }

    u32 first_comment_index = trivia->first_comment_before_token[token_index];
    u32 comment_count       = trivia->comment_count_before_token[token_index];
    if (first_comment_index == U32_MAX || comment_count == 0) {
        return false;
    }

    if (out_first_comment_index != NULL) {
        *out_first_comment_index = first_comment_index;
    }
    if (out_comment_count != NULL) {
        *out_comment_count = comment_count;
    }
    return true;
}

internal void format_trivia_validate(const Lexer*        lexer,
                                     const FormatTrivia* trivia)
{
#if CONFIG_DEBUG
    for (u32 token_index = 0; token_index < array_count(lexer->tokens);
         ++token_index) {
        usize end_offset =
            lex_token_end_offset(lexer, &lexer->tokens[token_index]);
        u32  old_comment_index = U32_MAX;
        bool old_has_comment =
            format_find_trailing_comment_index_after_offset_scan(
                lexer->source, lexer, end_offset, &old_comment_index);

        u32  new_comment_index = U32_MAX;
        bool new_has_comment   = format_trivia_trailing_comment_after_token(
            trivia, token_index, &new_comment_index);

        ASSERT(old_has_comment == new_has_comment,
               "Format trivia trailing-comment classification mismatch");
        if (old_has_comment && new_has_comment) {
            ASSERT(old_comment_index == new_comment_index,
                   "Format trivia trailing-comment index mismatch");
        }
    }
#else
    UNUSED(lexer);
    UNUSED(trivia);
#endif
}

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
    case CK_Assign:
        return 1;
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
    case CK_ShiftLeft:
    case CK_ShiftRight:
        return 45;
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
        case '\0':
            sb_append_cstr(sb, "\\0");
            break;
        case '\a':
            sb_append_cstr(sb, "\\a");
            break;
        case '\b':
            sb_append_cstr(sb, "\\b");
            break;
        case '\f':
            sb_append_cstr(sb, "\\f");
            break;
        case '\v':
            sb_append_cstr(sb, "\\v");
            break;
        case '\\':
            sb_append_cstr(sb, "\\\\");
            break;
        case '"':
            sb_append_cstr(sb, "\\\"");
            break;
        default:
            if (text.data[i] < 0x20 || text.data[i] == 0x7f) {
                sb_format(sb, "\\x%02x", text.data[i]);
            } else {
                sb_append_char(sb, (char)text.data[i]);
            }
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

internal void format_emit_integer_literal(StringBuilder* sb,
                                          const Lexer*   lexer,
                                          u32            token_index)
{
    ASSERT(token_index < array_count(lexer->tokens),
           "Integer literal token index out of bounds");
    const Token* token = &lexer->tokens[token_index];
    ASSERT(token->kind == TK_Integer, "Expected integer token");

    usize start = token->offset;
    usize end   = lex_token_end_offset(lexer, token);
    if (end > lexer->source.source.count) {
        end = lexer->source.source.count;
    }

    sb_append_string(
        sb, string_from(lexer->source.source.data + start, end - start));
}

internal void   format_emit_fn_signature(StringBuilder* sb,
                                         const Cst*     cst,
                                         const Lexer*   lexer,
                                         u32            signature_index,
                                         bool           include_return_type);
internal void   format_emit_ffi_def(StringBuilder* sb,
                                    const Cst*     cst,
                                    const Lexer*   lexer,
                                    u32            ffi_info_index);
internal void   format_emit_ffi_block(StringBuilder* sb,
                                      const Cst*     cst,
                                      const Lexer*   lexer,
                                      u32            ffi_block_info_index,
                                      u32            indent_level);
internal void   format_emit_mod_ref(StringBuilder* sb,
                                    const Cst*     cst,
                                    const Lexer*   lexer,
                                    u32            module_path_index);
internal void   format_emit_module_path(StringBuilder* sb,
                                        const Cst*     cst,
                                        const Lexer*   lexer,
                                        u32            module_path_index);
internal void   format_emit_value(StringBuilder* sb,
                                  const Cst*     cst,
                                  const Lexer*   lexer,
                                  u32            node_index);
internal void   format_emit_block_contents(StringBuilder* sb,
                                           const Cst*     cst,
                                           const Lexer*   lexer,
                                           u32            block_node_index,
                                           u32            indent_level);
internal void   format_emit_indent(StringBuilder* sb, u32 indent_level);
internal void   format_emit_type_plex_multiline(StringBuilder* sb,
                                                const Cst*     cst,
                                                const Lexer*   lexer,
                                                u32            node_index,
                                                u32            indent_level);
internal void   format_emit_type_enum_multiline(StringBuilder* sb,
                                                const Cst*     cst,
                                                const Lexer*   lexer,
                                                u32            node_index,
                                                u32            indent_level);
internal void   format_emit_expr(StringBuilder* sb,
                                 const Cst*     cst,
                                 const Lexer*   lexer,
                                 u32            node_index,
                                 int            parent_precedence);
internal string format_assignment_operator(const Lexer*   lexer,
                                           const CstNode* stmt);
internal bool   format_use_sort_key(StringBuilder* sb,
                                    const Cst*     cst,
                                    const Lexer*   lexer,
                                    u32            node_index);
internal bool   format_emit_sorted_use_run(StringBuilder* sb,
                                           const Cst*     cst,
                                           const Lexer*   lexer,
                                           Array(u32) use_nodes,
                                           u32 indent_level);
internal bool
format_node_is_single_line(const Cst* cst, const Lexer* lexer, u32 node_index);
internal void
format_emit_string_literal(StringBuilder* sb, string text, bool is_c_string);
internal bool format_collect_plain_string_concat(StringBuilder* sb,
                                                 const Cst*     cst,
                                                 const Lexer*   lexer,
                                                 u32            node_index);
internal void format_emit_plex_literal_multiline(StringBuilder* sb,
                                                 const Cst*     cst,
                                                 const Lexer*   lexer,
                                                 u32            node_index,
                                                 u32            indent_level);
internal void format_emit_array_multiline(StringBuilder* sb,
                                          const Cst*     cst,
                                          const Lexer*   lexer,
                                          const CstNode* node,
                                          u32            indent_level);
internal void format_emit_value_with_indent(StringBuilder* sb,
                                            const Cst*     cst,
                                            const Lexer*   lexer,
                                            u32            node_index,
                                            u32            indent_level);
internal void format_emit_expr_with_indent(StringBuilder* sb,
                                           const Cst*     cst,
                                           const Lexer*   lexer,
                                           u32            node_index,
                                           int            parent_precedence,
                                           u32            indent_level);

internal u32 g_format_expr_indent_level = 0;

typedef struct {
    u32    node_index;
    string key;
    bool   is_public;
} FormatUseItem;

internal void format_emit_pattern(StringBuilder* sb,
                                  const Cst*     cst,
                                  const Lexer*   lexer,
                                  u32            pattern_index,
                                  bool           explicit_binds)
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
            format_emit_pattern(sb, cst, lexer, pattern->b, explicit_binds);
            sb_append_cstr(sb, " as ");
        } else if (explicit_binds) {
            sb_append_cstr(sb, "as ");
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
            format_emit_pattern(sb,
                                cst,
                                lexer,
                                cst->pattern_items[pattern->a + i],
                                explicit_binds);
        }
        if (pattern->b == 1) {
            sb_append_char(sb, ',');
        }
        sb_append_char(sb, ')');
        break;
    case CPK_Plex:
        if (pattern->c != 0) {
            format_emit_expr(sb, cst, lexer, pattern->c - 1, 0);
            sb_append_char(sb, ' ');
        }
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
            if (!(!explicit_binds && field_pattern->kind == CPK_Bind &&
                  field_pattern->b == U32_MAX &&
                  field_pattern->a == field->symbol_handle)) {
                sb_append_cstr(sb, ": ");
                format_emit_pattern(
                    sb, cst, lexer, field->pattern_index, explicit_binds);
            }
        }
        sb_append_cstr(sb, " }");
        break;
    case CPK_EnumVariant:
        {
            const CstEnumPattern* enum_pattern =
                &cst->enum_patterns[pattern->a];
            if (enum_pattern->qualifier_node_index != U32_MAX) {
                format_emit_expr(
                    sb, cst, lexer, enum_pattern->qualifier_node_index, 0);
                sb_append_char(sb, '.');
            }
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
                    cst->pattern_items[enum_pattern->first_pattern + i],
                    explicit_binds);
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
        format_emit_integer_literal(sb, lexer, node->token_index);
        break;
    case CK_FloatLiteral:
        format_emit_float_literal(sb, lexer, node->token_index);
        break;
    case CK_BoolLiteral:
        sb_append_cstr(sb, node->a != 0 ? "yes" : "no");
        break;
    case CK_NilLiteral:
        sb_append_cstr(sb, "nil");
        break;
    case CK_StringLiteral:
        format_emit_string_literal(sb,
                                   lexer->strings[node->a],
                                   lexer->tokens[node->token_index].kind ==
                                       TK_CString);
        break;
    case CK_StringConcat:
        {
            StringBuilder concat = {0};
            sb_init(&concat, &temp_arena);
            if (format_collect_plain_string_concat(
                    &concat, cst, lexer, node_index)) {
                format_emit_string_literal(sb, sb_to_string(&concat), false);
            } else {
                format_emit_expr(sb, cst, lexer, node->a, node_precedence);
                sb_append_char(sb, ' ');
                format_emit_expr(sb, cst, lexer, node->b, node_precedence);
            }
        }
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
        if (!format_node_is_single_line(cst, lexer, node_index)) {
            format_emit_array_multiline(
                sb, cst, lexer, node, g_format_expr_indent_level);
            break;
        }
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
            if (!format_node_is_single_line(cst, lexer, node_index)) {
                format_emit_plex_literal_multiline(
                    sb, cst, lexer, node_index, g_format_expr_indent_level);
                break;
            }

            if (plex->target_node_index != U32_MAX) {
                format_emit_expr(sb, cst, lexer, plex->target_node_index, 0);
            }
            if (node->kind == CK_PlexUpdate) {
                sb_append_cstr(sb, " with");
            }

            sb_append_cstr(sb,
                           (plex->target_node_index != U32_MAX ||
                            node->kind == CK_PlexUpdate)
                               ? " { "
                               : "{ ");
            for (u32 i = 0; i < plex->field_count; ++i) {
                if (i > 0) {
                    sb_append_cstr(sb, ", ");
                }
                const CstPlexLiteralField* field =
                    &cst->plex_literal_fields[plex->first_field + i];
                string field_name = lex_symbol(lexer, field->symbol_handle);
                sb_append_string(sb, field_name);
                if (format_plex_field_is_shorthand(cst, field)) {
                    continue;
                }
                sb_append_cstr(sb, ": ");
                format_emit_expr(sb, cst, lexer, field->value_node_index, 0);
            }
            if (plex->flags & CPLF_ZeroMissing) {
                if (plex->field_count > 0) {
                    sb_append_cstr(sb, ", ");
                }
                sb_append_cstr(sb, "...");
            }
            sb_append_cstr(sb, " }");
        }
        break;
    case CK_Assign:
        format_emit_expr(sb, cst, lexer, node->a, node_precedence);
        sb_append_char(sb, ' ');
        sb_append_string(sb, format_assignment_operator(lexer, node));
        sb_append_char(sb, ' ');
        format_emit_expr(sb, cst, lexer, node->b, node_precedence);
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
    case CK_Deref:
        format_emit_expr(sb, cst, lexer, node->a, node_precedence);
        sb_append_char(sb, '^');
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
    case CK_BreakOn:
        {
            const CstNode* break_node = &cst->nodes[node->b];
            ASSERT(break_node->kind == CK_BreakExpr,
                   "Expected break expression");
            sb_append_cstr(sb, "break");
            if (break_node->b != U32_MAX) {
                sb_append_cstr(sb, " $");
                sb_append_string(sb, lex_symbol(lexer, break_node->b));
            }
            sb_append_cstr(sb, " on ");
            format_emit_expr(sb, cst, lexer, node->a, 0);
            sb_append_cstr(sb, " => ");
            format_emit_expr(sb, cst, lexer, break_node->a, 0);
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
        if (lexer->tokens[node->token_index].kind != TK_LBrace) {
            sb_append_cstr(sb, "$");
        }
        if (node->b != U32_MAX) {
            sb_append_string(sb, lex_symbol(lexer, node->b));
            sb_append_char(sb, ' ');
        }
        sb_append_cstr(sb, "{\n");
        format_emit_block_contents(
            sb, cst, lexer, node->a, g_format_expr_indent_level + 1);
        format_emit_indent(sb, g_format_expr_indent_level);
        sb_append_char(sb, '}');
        break;
    case CK_For:
        {
            const CstForInfo* for_info = &cst->fors[node->a];
            sb_append_cstr(sb, "for");
            if (for_info->iterable_node_index != U32_MAX) {
                sb_append_char(sb, ' ');
                if (for_info->index_symbol != U32_MAX) {
                    sb_append_string(sb,
                                     lex_symbol(lexer, for_info->index_symbol));
                    sb_append_cstr(sb, ", ");
                }
                sb_append_string(sb, lex_symbol(lexer, for_info->item_symbol));
                sb_append_cstr(sb, " in ");
                format_emit_expr(
                    sb, cst, lexer, for_info->iterable_node_index, 0);
            } else if (for_info->mode == CFM_CStyle) {
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
            u32 indent_level = g_format_expr_indent_level;
            sb_append_cstr(sb, " {\n");
            format_emit_block_contents(
                sb, cst, lexer, node->b, indent_level + 1);
            format_emit_indent(sb, indent_level);
            sb_append_char(sb, '}');
            if (for_info->else_block_index != U32_MAX) {
                sb_append_cstr(sb, " else {\n");
                format_emit_block_contents(sb,
                                           cst,
                                           lexer,
                                           for_info->else_block_index,
                                           indent_level + 1);
                format_emit_indent(sb, indent_level);
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
    case CK_ShiftLeft:
        format_emit_expr(sb, cst, lexer, node->a, node_precedence);
        sb_append_cstr(sb, " << ");
        format_emit_expr(sb, cst, lexer, node->b, node_precedence + 1);
        break;
    case CK_ShiftRight:
        format_emit_expr(sb, cst, lexer, node->a, node_precedence);
        sb_append_cstr(sb, " >> ");
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
        {
            const CstCastInfo* cast = &cst->casts[node->b];
            format_emit_expr(sb, cst, lexer, cast->type_node_index, 0);
            if (cast->extra_node_index != U32_MAX) {
                sb_append_cstr(sb, ", ");
                format_emit_expr(sb, cst, lexer, cast->extra_node_index, 0);
            }
        }
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
                            cst->pattern_items[branch->pattern_index + pattern],
                            true);
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
        format_emit_type_enum_multiline(sb, cst, lexer, node_index, 0);
        break;
    case CK_TypeFn:
        format_emit_fn_signature(sb, cst, lexer, node->a, true);
        break;
    case CK_TypeApply:
        {
            const CstTypeApplyInfo* apply = &cst->type_applications[node->a];
            format_emit_expr(sb, cst, lexer, apply->target_node_index, 0);
            sb_append_char(sb, '[');
            for (u32 i = 0; i < apply->arg_count; ++i) {
                if (i > 0) {
                    sb_append_cstr(sb, ", ");
                }
                format_emit_expr(
                    sb, cst, lexer, cst->tuple_items[apply->first_arg + i], 0);
            }
            sb_append_char(sb, ']');
        }
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
    case CK_TypeDynamicArray:
        sb_append_char(sb, '[');
        if (node->a != UINT32_MAX) {
            format_emit_expr(sb, cst, lexer, node->a, 0);
        }
        sb_append_cstr(sb, "..]");
        format_emit_expr(sb, cst, lexer, node->b, 0);
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
            if (plex->generic_params_index != U32_MAX) {
                const CstGenericParams* generic =
                    &cst->generic_params[plex->generic_params_index];
                sb_append_cstr(sb, " [");
                for (u32 i = 0; i < generic->symbol_count; ++i) {
                    if (i > 0) {
                        sb_append_cstr(sb, ", ");
                    }
                    sb_append_string(
                        sb,
                        lex_symbol(
                            lexer,
                            cst->generic_param_symbols[generic->first_symbol +
                                                       i]));
                }
                sb_append_char(sb, ']');
            }
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
    case CK_FfiBlock:
        format_emit_ffi_block(sb, cst, lexer, node->a, 0);
        break;
    case CK_ModRef:
        format_emit_mod_ref(sb, cst, lexer, node->a);
        break;
    case CK_Use:
        if (node->flags & CNF_Public) {
            sb_append_cstr(sb, "pub ");
        }
        sb_append_cstr(sb, "use ");
        if (cst->nodes[node->a].kind == CK_ModRef) {
            format_emit_module_path(sb, cst, lexer, cst->nodes[node->a].a);
        } else {
            format_emit_expr(sb, cst, lexer, node->a, 0);
        }
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

internal u32 g_format_value_indent_level = 0;

internal void format_emit_value_with_indent(StringBuilder* sb,
                                            const Cst*     cst,
                                            const Lexer*   lexer,
                                            u32            node_index,
                                            u32            indent_level)
{
    u32 saved_indent            = g_format_value_indent_level;
    g_format_value_indent_level = indent_level;
    format_emit_value(sb, cst, lexer, node_index);
    g_format_value_indent_level = saved_indent;
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
                cst->pattern_items[branch->pattern_index + pattern],
                true);
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
        format_emit_expr_with_indent(
            sb, cst, lexer, branch->expr_node_index, 0, indent_level + 1);
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
internal bool format_has_blank_line_between_offsets(NerdSource source,
                                                    usize      previous_end,
                                                    usize      current_start);

typedef struct {
    const Cst*   cst;
    const Lexer* lexer;
} FormatSyntaxContext;

internal void format_emit_indent(StringBuilder* sb, u32 indent_level)
{
    for (u32 i = 0; i < indent_level; ++i) {
        sb_append_cstr(sb, "    ");
    }
}

internal void format_emit_line_comment(StringBuilder* sb,
                                       u32            indent_level,
                                       string         comment_text)
{
    format_emit_indent(sb, indent_level);
    sb_append_cstr(sb, "--");
    sb_append_string(sb, comment_text);
    sb_append_char(sb, '\n');
}

internal void format_emit_spaces(StringBuilder* sb, usize count)
{
    for (usize i = 0; i < count; ++i) {
        sb_append_char(sb, ' ');
    }
}

internal usize format_sb_current_column(const StringBuilder* sb)
{
    usize column = 0;
    for (usize i = sb->size; i > 0; --i) {
        if (sb->data[i - 1] == '\n') {
            break;
        }
        ++column;
    }
    return column;
}

internal bool format_string_has_newline(string text)
{
    for (usize i = 0; i < text.count; ++i) {
        if (text.data[i] == '\n' || text.data[i] == '\r') {
            return true;
        }
    }
    return false;
}

internal int format_string_compare(string a, string b)
{
    usize count = a.count < b.count ? a.count : b.count;
    int   cmp   = memcmp(a.data, b.data, count);
    if (cmp != 0) {
        return cmp;
    }
    if (a.count < b.count) {
        return -1;
    }
    if (a.count > b.count) {
        return 1;
    }
    return 0;
}

internal usize format_find_string_split(string text, usize start, usize max_end)
{
    usize best = U32_MAX;
    for (usize i = start; i < max_end && i < text.count; ++i) {
        if (text.data[i] == ' ') {
            best = i + 1;
        }
    }
    if (best != U32_MAX && best > start) {
        return best;
    }
    return max_end < text.count ? max_end : text.count;
}

internal void
format_emit_string_literal(StringBuilder* sb, string text, bool is_c_string)
{
    usize column          = format_sb_current_column(sb);
    usize available_width = column < FORMAT_WRAP_WIDTH
                                ? FORMAT_WRAP_WIDTH - column
                                : FORMAT_WRAP_WIDTH;
    if (is_c_string || text.count + 2 <= available_width ||
        format_string_has_newline(text)) {
        if (is_c_string) {
            sb_append_char(sb, 'c');
        }
        sb_append_char(sb, '"');
        format_emit_string_text(sb, text);
        sb_append_char(sb, '"');
        return;
    }

    usize start = 0;
    while (start < text.count) {
        column          = format_sb_current_column(sb);
        available_width = column < FORMAT_WRAP_WIDTH
                              ? FORMAT_WRAP_WIDTH - column
                              : FORMAT_WRAP_WIDTH;
        usize segment_size =
            available_width > 2 ? available_width - 2 : FORMAT_WRAP_WIDTH - 2;
        usize end = start + segment_size;
        if (end > text.count) {
            end = text.count;
        } else {
            end = format_find_string_split(text, start, end);
        }

        sb_append_char(sb, '"');
        format_emit_string_text(sb,
                                string_from(text.data + start, end - start));
        sb_append_char(sb, '"');

        start = end;
        if (start < text.count) {
            sb_append_char(sb, '\n');
            format_emit_indent(sb, g_format_expr_indent_level + 1);
        }
    }
}

internal bool format_collect_plain_string_concat(StringBuilder* sb,
                                                 const Cst*     cst,
                                                 const Lexer*   lexer,
                                                 u32            node_index)
{
    const CstNode* node = &cst->nodes[node_index];
    if (node->kind == CK_StringLiteral) {
        if (lexer->tokens[node->token_index].kind == TK_CString) {
            return false;
        }
        sb_append_string(sb, lexer->strings[node->a]);
        return true;
    }
    if (node->kind != CK_StringConcat) {
        return false;
    }
    return format_collect_plain_string_concat(sb, cst, lexer, node->a) &&
           format_collect_plain_string_concat(sb, cst, lexer, node->b);
}

internal bool format_node_is_block_form_on(const Cst* cst, u32 node_index)
{
    const CstNode* node = &cst->nodes[node_index];
    return node->kind == CK_On && cst->ons[node->b].kind != COK_Bool;
}

typedef struct {
    u32    node_index;
    string symbol;
    string type;
    string op;
    string value;
    bool   is_bind;
    bool   is_assignment;
    bool   has_value;
    bool   is_public;
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

internal bool format_node_is_value_constant_payload(const CstNode* node)
{
    return node->kind != CK_FnExpr && node->kind != CK_FnBlock &&
           node->kind != CK_FfiDef && node->kind != CK_FfiBlock &&
           node->kind != CK_ModRef && node->kind != CK_For &&
           node->kind != CK_ExprBlock && node->kind != CK_TypePlex &&
           node->kind != CK_TypeEnum;
}

internal string format_assignment_operator(const Lexer*   lexer,
                                           const CstNode* stmt)
{
    u32 op_index = stmt->token_index;
    if (op_index >= array_count(lexer->tokens)) {
        return s("=");
    }

    TokenKind kind = lexer->tokens[op_index].kind;
    switch (kind) {
    case TK_Equal:
    case TK_PlusEqual:
    case TK_MinusEqual:
    case TK_StarEqual:
    case TK_SlashEqual:
    case TK_PercentEqual:
    case TK_AmpEqual:
    case TK_CaretEqual:
    case TK_PipeEqual:
    case TK_AmpAmpEqual:
    case TK_PipePipeEqual:
    case TK_ShiftLeftEqual:
    case TK_ShiftRightEqual:
        break;
    default:
        op_index = stmt->token_index + 1;
        if (op_index >= array_count(lexer->tokens)) {
            return s("=");
        }
        kind = lexer->tokens[op_index].kind;
        break;
    }

    switch (kind) {
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
    case TK_ShiftLeftEqual:
        return s("<<=");
    case TK_ShiftRightEqual:
        return s(">>=");
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

internal u32 format_find_matching_close_after_token(const Lexer* lexer,
                                                    u32          token_index,
                                                    TokenKind    open_kind,
                                                    TokenKind    close_kind)
{
    for (u32 i = token_index; i < array_count(lexer->tokens); ++i) {
        if (lexer->tokens[i].kind == open_kind) {
            return format_find_matching_close_token_index(
                lexer, i, open_kind, close_kind);
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
    case CK_NilLiteral:
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
        return format_find_matching_close_after_token(
            lexer, node->token_index, TK_LBrace, TK_RBrace);
    case CK_TypeApply:
        return format_find_matching_close_token_index(
            lexer, node->token_index, TK_LBracket, TK_RBracket);
    case CK_TupleField:
    case CK_Field:
        return node->token_index;
    case CK_IntegerNegate:
    case CK_LogicalNot:
    case CK_AddressOf:
    case CK_Deref:
    case CK_TypePointer:
    case CK_TypeSlice:
    case CK_Statement:
    case CK_Use:
        return format_node_end_token_index(cst, lexer, node->a);
    case CK_TypeArray:
        return format_node_end_token_index(cst, lexer, node->b);
    case CK_TypeDynamicArray:
        return node->b == U32_MAX
                   ? node->token_index
                   : format_node_end_token_index(cst, lexer, node->b);
    case CK_Defer:
    case CK_Assert:
        if (node->kind == CK_Defer) {
            return format_node_end_token_index(cst, lexer, node->a);
        }
        return node->b == U32_MAX
                   ? format_node_end_token_index(cst, lexer, node->a)
                   : format_node_end_token_index(cst, lexer, node->b);
    case CK_Return:
    case CK_ReturnExpr:
        return node->a == U32_MAX
                   ? node->token_index
                   : format_node_end_token_index(cst, lexer, node->a);
    case CK_Break:
    case CK_BreakExpr:
        if (node->a != U32_MAX) {
            return format_node_end_token_index(cst, lexer, node->a);
        }
        return node->b == U32_MAX ? node->token_index : node->token_index + 2;
    case CK_BreakOn:
        return format_node_end_token_index(cst, lexer, cst->nodes[node->b].a);
    case CK_Continue:
    case CK_ContinueExpr:
        return node->b == U32_MAX ? node->token_index : node->token_index + 2;
    case CK_For:
        return format_node_end_token_index(cst, lexer, node->b);
    case CK_IntegerPlus:
    case CK_IntegerMinus:
    case CK_IntegerMultiply:
    case CK_IntegerDivide:
    case CK_IntegerModulo:
    case CK_BitwiseAnd:
    case CK_BitwiseXor:
    case CK_BitwiseOr:
    case CK_ShiftLeft:
    case CK_ShiftRight:
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
    case CK_Array:
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
    case CK_FfiBlock:
        return format_find_matching_close_token_index(
            lexer, node->token_index, TK_LBrace, TK_RBrace);
    case CK_ModRef:
        {
            const CstModulePath* path = &cst->module_paths[node->a];
            u32                  keyword_width =
                lexer->tokens[node->token_index].kind == TK_use ? 1 : 0;
            return node->token_index + keyword_width + path->symbol_count * 2 -
                   2;
        }
    case CK_ExprBlock:
        return format_node_end_token_index(cst, lexer, node->a);
    case CK_Impl:
        {
            const CstImplInfo* impl = &cst->impls[node->a];
            return format_node_end_token_index(
                cst, lexer, impl->body_node_index);
        }
    case CK_Test:
        return format_node_end_token_index(cst, lexer, node->b);
    case CK_Block:
        return format_find_matching_close_token_index(
            lexer, node->token_index, TK_LBrace, TK_RBrace);
    case CK_ZeroInit:
        return format_node_end_token_index(cst, lexer, node->a);
    case CK_Undefined:
        return node->token_index;
    default:
        return node->token_index;
    }
}

internal FormatSyntaxContext format_syntax_context(const Cst*   cst,
                                                   const Lexer* lexer)
{
    return (FormatSyntaxContext){
        .cst   = cst,
        .lexer = lexer,
    };
}

internal u32 format_syntax_node_start_token(const FormatSyntaxContext* context,
                                            u32 node_index)
{
    return context->cst->nodes[node_index].token_index;
}

internal u32 format_syntax_node_end_token(const FormatSyntaxContext* context,
                                          u32 node_index)
{
    return format_node_end_token_index(context->cst, context->lexer, node_index);
}

internal usize format_syntax_node_start_offset(
    const FormatSyntaxContext* context, u32 node_index)
{
    u32 token_index = format_syntax_node_start_token(context, node_index);
    return context->lexer->tokens[token_index].offset;
}

internal usize format_syntax_node_end_offset(const FormatSyntaxContext* context,
                                             u32 node_index)
{
    u32 token_index = format_syntax_node_end_token(context, node_index);
    return lex_token_end_offset(context->lexer,
                                &context->lexer->tokens[token_index]);
}

internal bool format_syntax_has_blank_line_between_nodes(
    const FormatSyntaxContext* context,
    u32                        previous_node_index,
    u32                        current_node_index)
{
    return format_has_blank_line_between_offsets(
        context->lexer->source,
        format_syntax_node_end_offset(context, previous_node_index),
        format_syntax_node_start_offset(context, current_node_index));
}

internal bool format_has_comment_between_offsets(const Lexer* lexer,
                                                 usize        previous_end,
                                                 usize        current_start)
{
    if (previous_end >= current_start) {
        return false;
    }

    for (u32 i = 0; i < array_count(lexer->comments); ++i) {
        usize offset = lexer->comments[i].offset;
        if (offset >= current_start) {
            return false;
        }
        if (offset >= previous_end) {
            return true;
        }
    }
    return false;
}

internal usize format_first_comment_or_offset_between(const Lexer* lexer,
                                                      usize        previous_end,
                                                      usize current_start)
{
    if (previous_end >= current_start) {
        return current_start;
    }

    for (u32 i = 0; i < array_count(lexer->comments); ++i) {
        usize offset = lexer->comments[i].offset;
        if (offset >= current_start) {
            return current_start;
        }
        if (offset >= previous_end) {
            return offset;
        }
    }
    return current_start;
}

internal bool format_syntax_has_comment_between_nodes(
    const FormatSyntaxContext* context,
    u32                        previous_node_index,
    u32                        current_node_index)
{
    return format_has_comment_between_offsets(
        context->lexer,
        format_syntax_node_end_offset(context, previous_node_index),
        format_syntax_node_start_offset(context, current_node_index));
}

internal bool format_has_blank_line_between_offsets(NerdSource source,
                                                    usize      previous_end,
                                                    usize      current_start)
{
    if (previous_end >= current_start || current_start > source.source.count) {
        return false;
    }

    u32   previous_end_line  = 0;
    u32   previous_end_col   = 0;
    u32   current_start_line = 0;
    u32   current_start_col  = 0;
    usize previous_offset = previous_end > 0 ? previous_end - 1 : previous_end;

    if (!lex_offset_to_line_col(
            source, previous_offset, &previous_end_line, &previous_end_col)) {
        return false;
    }

    if (!lex_offset_to_line_col(
            source, current_start, &current_start_line, &current_start_col)) {
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

internal bool
format_node_is_single_line(const Cst* cst, const Lexer* lexer, u32 node_index)
{
    return format_statement_is_single_line(cst, lexer, node_index);
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

internal bool format_plex_field_is_shorthand(const Cst*                 cst,
                                             const CstPlexLiteralField* field)
{
    const CstNode* value = &cst->nodes[field->value_node_index];
    return value->kind == CK_SymbolRef && value->a == field->symbol_handle;
}

internal void format_emit_plex_literal_multiline(StringBuilder* sb,
                                                 const Cst*     cst,
                                                 const Lexer*   lexer,
                                                 u32            node_index,
                                                 u32            indent_level)
{
    const CstNode*            node            = &cst->nodes[node_index];
    const CstPlexLiteralInfo* plex            = &cst->plex_literals[node->a];
    usize                     max_field_width = 0;
    for (u32 i = 0; i < plex->field_count; ++i) {
        const CstPlexLiteralField* field =
            &cst->plex_literal_fields[plex->first_field + i];
        usize field_width = lex_symbol(lexer, field->symbol_handle).count;
        if (field_width > max_field_width) {
            max_field_width = field_width;
        }
    }
    Array(usize) field_code_widths     = NULL;
    Array(usize) field_end_offsets     = NULL;
    Array(bool) field_has_comments     = NULL;
    Array(u32) field_comment_indices   = NULL;
    Array(usize) field_comment_columns = NULL;
    usize field_indent_width           = (usize)(indent_level + 1) * 4;
    for (u32 i = 0; i < plex->field_count; ++i) {
        const CstPlexLiteralField* field =
            &cst->plex_literal_fields[plex->first_field + i];
        string field_name = lex_symbol(lexer, field->symbol_handle);
        usize  value_width =
            format_plex_field_is_shorthand(cst, field)
                ? 0
                : format_rendered_expr_width(
                      cst, lexer, field->value_node_index, indent_level + 1);
        usize code_width =
            field_indent_width + (format_plex_field_is_shorthand(cst, field)
                                      ? field_name.count
                                      : max_field_width + 3 + value_width);
        u32 value_end_token =
            format_node_end_token_index(cst, lexer, field->value_node_index);
        usize value_end_offset =
            lex_token_end_offset(lexer, &lexer->tokens[value_end_token]);
        u32  comment_index = U32_MAX;
        bool has_comment   = format_find_trailing_comment_index_after_offset(
            lexer->source, lexer, value_end_offset, &comment_index);
        array_push(field_code_widths, code_width);
        array_push(field_end_offsets, value_end_offset);
        array_push(field_has_comments, has_comment);
        array_push(field_comment_indices, comment_index);
        array_push(field_comment_columns, 0);
    }
    for (u32 i = 0; i < plex->field_count;) {
        u32   start          = i;
        usize comment_column = 0;
        usize previous_end   = 0;
        bool  have_previous  = false;
        while (i < plex->field_count) {
            const CstPlexLiteralField* field =
                &cst->plex_literal_fields[plex->first_field + i];
            usize field_start = lexer->tokens[field->token_index].offset;
            usize group_start = format_first_comment_or_offset_between(
                lexer, previous_end, field_start);
            if (have_previous &&
                format_has_blank_line_between_offsets(
                    lexer->source, previous_end, group_start)) {
                break;
            }
            if (field_has_comments[i] &&
                field_code_widths[i] + 1 > comment_column) {
                comment_column = field_code_widths[i] + 1;
            }
            previous_end = field_end_offsets[i];
            if (field_has_comments[i]) {
                previous_end =
                    lexer->comments[field_comment_indices[i]].end_offset;
            }
            have_previous = true;
            i++;
        }
        if (comment_column > 0) {
            for (u32 field_index = start; field_index < i; ++field_index) {
                field_comment_columns[field_index] = comment_column;
            }
        }
    }

    if (plex->target_node_index != U32_MAX) {
        format_emit_expr(sb, cst, lexer, plex->target_node_index, 0);
    }
    if (node->kind == CK_PlexUpdate) {
        sb_append_cstr(sb, " with");
    }
    sb_append_cstr(
        sb,
        (plex->target_node_index != U32_MAX || node->kind == CK_PlexUpdate)
            ? " {\n"
            : "{\n");

    u32 open_brace_token = node->token_index;
    while (open_brace_token < array_count(lexer->tokens) &&
           lexer->tokens[open_brace_token].kind != TK_LBrace) {
        open_brace_token++;
    }
    u32 comment_index = 0;
    if (open_brace_token < array_count(lexer->tokens)) {
        usize open_brace_end =
            lex_token_end_offset(lexer, &lexer->tokens[open_brace_token]);
        format_skip_block_comments_before_offset(
            lexer, &comment_index, open_brace_end);
    }

    usize previous_field_end_offset = 0;
    bool  have_previous_field       = false;

    for (u32 i = 0; i < plex->field_count; ++i) {
        const CstPlexLiteralField* field =
            &cst->plex_literal_fields[plex->first_field + i];
        string field_name         = lex_symbol(lexer, field->symbol_handle);
        usize  field_start_offset = lexer->tokens[field->token_index].offset;
        usize  group_start_offset = field_start_offset;
        if (comment_index < array_count(lexer->comments) &&
            lexer->comments[comment_index].offset < field_start_offset) {
            group_start_offset = lexer->comments[comment_index].offset;
        }
        if (have_previous_field &&
            format_has_blank_line_between_offsets(
                lexer->source, previous_field_end_offset, group_start_offset)) {
            sb_append_char(sb, '\n');
        }
        format_emit_block_comments_before_token(sb,
                                                lexer,
                                                &comment_index,
                                                field->token_index,
                                                indent_level + 1,
                                                NULL);
        format_emit_indent(sb, indent_level + 1);
        sb_append_string(sb, field_name);
        if (format_plex_field_is_shorthand(cst, field)) {
        } else {
            for (usize pad = field_name.count; pad <= max_field_width; ++pad) {
                sb_append_char(sb, ' ');
            }
            sb_append_char(sb, ':');
            sb_append_char(sb, ' ');
            format_emit_expr_with_indent(
                sb, cst, lexer, field->value_node_index, 0, indent_level + 1);
        }
        usize field_end_offset = field_end_offsets[i];
        if (field_has_comments[i]) {
            usize before_offset = lexer->source.source.count;
            if (i + 1 < plex->field_count) {
                const CstPlexLiteralField* next_field =
                    &cst->plex_literal_fields[plex->first_field + i + 1];
                before_offset = lexer->tokens[next_field->token_index].offset;
            }
            u32    first_comment_index = field_comment_indices[i];
            string comment_text =
                format_merged_trailing_comment_text(lexer,
                                                    first_comment_index,
                                                    field_comment_columns[i],
                                                    before_offset,
                                                    &comment_index);
            format_emit_trailing_comment_text_aligned(sb,
                                                      comment_text,
                                                      field_comment_columns[i],
                                                      field_code_widths[i]);
            if (comment_index > first_comment_index &&
                comment_index <= array_count(lexer->comments)) {
                field_end_offset =
                    lexer->comments[comment_index - 1].end_offset;
            }
        } else {
            sb_append_char(sb, '\n');
        }
        previous_field_end_offset = field_end_offset;
        have_previous_field       = true;
    }
    if (plex->flags & CPLF_ZeroMissing) {
        format_emit_indent(sb, indent_level + 1);
        sb_append_cstr(sb, "...\n");
    }
    u32   close_token  = format_node_end_token_index(cst, lexer, node_index);
    usize close_offset = lexer->tokens[close_token].offset;
    format_emit_block_comments_before_offset(
        sb, lexer, &comment_index, close_offset, indent_level + 1, NULL);
    format_emit_indent(sb, indent_level);
    sb_append_char(sb, '}');
    array_free(field_code_widths);
    array_free(field_end_offsets);
    array_free(field_has_comments);
    array_free(field_comment_indices);
    array_free(field_comment_columns);
}

internal bool format_plex_literals_have_same_shape(const Cst*     cst,
                                                   const CstNode* a,
                                                   const CstNode* b)
{
    if ((a->kind != CK_Plex && a->kind != CK_PlexUpdate) ||
        (b->kind != CK_Plex && b->kind != CK_PlexUpdate)) {
        return false;
    }

    const CstPlexLiteralInfo* plex_a = &cst->plex_literals[a->a];
    const CstPlexLiteralInfo* plex_b = &cst->plex_literals[b->a];
    if (plex_a->field_count != plex_b->field_count) {
        return false;
    }

    for (u32 i = 0; i < plex_a->field_count; ++i) {
        const CstPlexLiteralField* field_a =
            &cst->plex_literal_fields[plex_a->first_field + i];
        const CstPlexLiteralField* field_b =
            &cst->plex_literal_fields[plex_b->first_field + i];
        if (field_a->symbol_handle != field_b->symbol_handle) {
            return false;
        }
    }

    return (plex_a->flags & CPLF_ZeroMissing) ==
           (plex_b->flags & CPLF_ZeroMissing);
}

internal void
format_emit_plex_literal_single_line_aligned(StringBuilder* sb,
                                             const Cst*     cst,
                                             const Lexer*   lexer,
                                             const CstNode* node,
                                             const usize*   field_name_widths,
                                             const usize*   field_value_widths)
{
    const CstPlexLiteralInfo* plex = &cst->plex_literals[node->a];

    if (plex->target_node_index != U32_MAX) {
        format_emit_expr(sb, cst, lexer, plex->target_node_index, 0);
    }
    if (node->kind == CK_PlexUpdate) {
        sb_append_cstr(sb, " with");
    }
    sb_append_cstr(
        sb,
        (plex->target_node_index != U32_MAX || node->kind == CK_PlexUpdate)
            ? " { "
            : "{ ");
    for (u32 i = 0; i < plex->field_count; ++i) {
        const CstPlexLiteralField* field =
            &cst->plex_literal_fields[plex->first_field + i];
        string field_name = lex_symbol(lexer, field->symbol_handle);
        sb_append_string(sb, field_name);
        if (format_plex_field_is_shorthand(cst, field)) {
            if (i + 1 < plex->field_count || (plex->flags & CPLF_ZeroMissing)) {
                sb_append_cstr(sb, ", ");
            } else {
                sb_append_char(sb, ' ');
            }
            continue;
        }
        for (usize pad = field_name.count; pad <= field_name_widths[i]; ++pad) {
            sb_append_char(sb, ' ');
        }
        sb_append_char(sb, ':');
        sb_append_char(sb, ' ');
        format_emit_expr(sb, cst, lexer, field->value_node_index, 0);
        if (i + 1 < plex->field_count || (plex->flags & CPLF_ZeroMissing)) {
            usize value_width = format_rendered_expr_width(
                cst, lexer, field->value_node_index, 0);
            sb_append_char(sb, ',');
            for (usize pad = value_width; pad <= field_value_widths[i]; ++pad) {
                sb_append_char(sb, ' ');
            }
        } else {
            sb_append_char(sb, ' ');
        }
    }
    if (plex->flags & CPLF_ZeroMissing) {
        sb_append_cstr(sb, "... ");
    }
    sb_append_char(sb, '}');
}

internal void format_emit_array_multiline(StringBuilder* sb,
                                          const Cst*     cst,
                                          const Lexer*   lexer,
                                          const CstNode* node,
                                          u32            indent_level)
{
    sb_append_cstr(sb, "[\n");
    for (u32 i = 0; i < node->b; ++i) {
        u32            item_index = cst->tuple_items[node->a + i];
        const CstNode* item       = &cst->nodes[item_index];
        format_emit_indent(sb, indent_level + 1);

        if ((item->kind == CK_Plex || item->kind == CK_PlexUpdate) &&
            format_node_is_single_line(cst, lexer, item_index)) {
            u32 run_end = i + 1;
            while (run_end < node->b) {
                u32 next_index = cst->tuple_items[node->a + run_end];
                if (!format_node_is_single_line(cst, lexer, next_index) ||
                    !format_plex_literals_have_same_shape(
                        cst, item, &cst->nodes[next_index])) {
                    break;
                }
                ++run_end;
            }

            if (run_end > i + 1) {
                const CstPlexLiteralInfo* first_plex =
                    &cst->plex_literals[item->a];
                Array(usize) field_name_widths  = NULL;
                Array(usize) field_value_widths = NULL;
                for (u32 field = 0; field < first_plex->field_count; ++field) {
                    array_push(field_name_widths, 0);
                    array_push(field_value_widths, 0);
                }

                for (u32 cursor = i; cursor < run_end; ++cursor) {
                    const CstPlexLiteralInfo* current_plex =
                        &cst->plex_literals
                             [cst->nodes[cst->tuple_items[node->a + cursor]].a];
                    for (u32 field = 0; field < current_plex->field_count;
                         ++field) {
                        const CstPlexLiteralField* literal_field =
                            &cst->plex_literal_fields
                                 [current_plex->first_field + field];
                        usize field_name_width =
                            lex_symbol(lexer, literal_field->symbol_handle)
                                .count;
                        if (field_name_width > field_name_widths[field]) {
                            field_name_widths[field] = field_name_width;
                        }

                        usize value_width = format_rendered_expr_width(
                            cst,
                            lexer,
                            literal_field->value_node_index,
                            indent_level + 1);
                        if (value_width > field_value_widths[field]) {
                            field_value_widths[field] = value_width;
                        }
                    }
                }

                for (u32 cursor = i; cursor < run_end; ++cursor) {
                    if (cursor > i) {
                        format_emit_indent(sb, indent_level + 1);
                    }
                    u32 current_index = cst->tuple_items[node->a + cursor];
                    format_emit_plex_literal_single_line_aligned(
                        sb,
                        cst,
                        lexer,
                        &cst->nodes[current_index],
                        field_name_widths,
                        field_value_widths);
                    sb_append_cstr(sb, ",\n");
                }

                array_free(field_name_widths);
                array_free(field_value_widths);
                i = run_end - 1;
                continue;
            }
        }

        format_emit_expr_with_indent(
            sb, cst, lexer, item_index, 0, indent_level + 1);
        sb_append_cstr(sb, ",\n");
    }
    format_emit_indent(sb, indent_level);
    sb_append_char(sb, ']');
}

internal usize format_rendered_expr_width(const Cst*   cst,
                                          const Lexer* lexer,
                                          u32          node_index,
                                          u32          indent_level)
{
    Arena scratch = {0};
    arena_init(&scratch, 0, ARENA_DEFAULT_NUM_PAGES_GROW);

    StringBuilder sb = {0};
    sb_init(&sb, &scratch);
    format_emit_expr_with_indent(&sb, cst, lexer, node_index, 0, indent_level);
    usize width = sb.size;
    arena_done(&scratch);
    return width;
}

internal void format_emit_enum_header(StringBuilder*         sb,
                                      const Cst*             cst,
                                      const Lexer*           lexer,
                                      const CstEnumTypeInfo* enum_type)
{
    sb_append_cstr(sb, "enum");
    if (enum_type->generic_params_index == U32_MAX) {
        return;
    }

    const CstGenericParams* generic =
        &cst->generic_params[enum_type->generic_params_index];
    sb_append_cstr(sb, " [");
    for (u32 i = 0; i < generic->symbol_count; ++i) {
        if (i > 0) {
            sb_append_cstr(sb, ", ");
        }
        sb_append_string(
            sb,
            lex_symbol(lexer,
                       cst->generic_param_symbols[generic->first_symbol + i]));
    }
    sb_append_char(sb, ']');
}

internal usize format_emit_enum_variant_prefix(StringBuilder*        sb,
                                               const Cst*            cst,
                                               const Lexer*          lexer,
                                               const CstEnumVariant* variant)
{
    usize start_size = sb->size;
    sb_append_string(sb, lex_symbol(lexer, variant->symbol_handle));
    if (variant->type_node_index != U32_MAX) {
        sb_append_char(sb, '(');
        const CstNode* variant_type = &cst->nodes[variant->type_node_index];
        if (variant_type->kind == CK_TypeTuple) {
            for (u32 item = 0; item < variant_type->b; ++item) {
                if (item > 0) {
                    sb_append_cstr(sb, ", ");
                }
                format_emit_expr(sb,
                                 cst,
                                 lexer,
                                 cst->tuple_items[variant_type->a + item],
                                 0);
            }
        } else {
            format_emit_expr(sb, cst, lexer, variant->type_node_index, 0);
        }
        sb_append_char(sb, ')');
    }
    return sb->size - start_size;
}

internal void format_emit_enum_variant_code(StringBuilder*        sb,
                                            const Cst*            cst,
                                            const Lexer*          lexer,
                                            const CstEnumVariant* variant,
                                            usize aligned_prefix_width)
{
    usize prefix_width =
        format_emit_enum_variant_prefix(sb, cst, lexer, variant);
    if (variant->value_node_index != U32_MAX) {
        if (aligned_prefix_width > prefix_width) {
            format_emit_spaces(sb, aligned_prefix_width - prefix_width);
        }
        sb_append_cstr(sb, " = ");
        format_emit_expr(sb, cst, lexer, variant->value_node_index, 0);
    }
}

internal usize format_rendered_enum_variant_width(const Cst*            cst,
                                                  const Lexer*          lexer,
                                                  const CstEnumVariant* variant,
                                                  u32   indent_level,
                                                  usize aligned_prefix_width)
{
    Arena         scratch = {0};
    StringBuilder sb      = {0};
    arena_init(&scratch);
    sb_init(&sb, &scratch);
    format_emit_enum_variant_code(
        &sb, cst, lexer, variant, aligned_prefix_width);
    usize width = (usize)(indent_level + 1) * 4 + sb.size;
    arena_done(&scratch);
    return width;
}

internal usize format_rendered_enum_variant_prefix_width(
    const Cst* cst, const Lexer* lexer, const CstEnumVariant* variant)
{
    Arena         scratch = {0};
    StringBuilder sb      = {0};
    arena_init(&scratch);
    sb_init(&sb, &scratch);
    format_emit_enum_variant_prefix(&sb, cst, lexer, variant);
    usize width = sb.size;
    arena_done(&scratch);
    return width;
}

internal usize format_enum_variant_end_offset(const Cst*            cst,
                                              const Lexer*          lexer,
                                              const CstEnumVariant* variant)
{
    if (variant->value_node_index != U32_MAX) {
        u32 end_token =
            format_node_end_token_index(cst, lexer, variant->value_node_index);
        return lex_token_end_offset(lexer, &lexer->tokens[end_token]);
    }
    if (variant->type_node_index != U32_MAX) {
        u32 end_token =
            format_node_end_token_index(cst, lexer, variant->type_node_index);
        if (end_token + 1 < array_count(lexer->tokens) &&
            lexer->tokens[end_token + 1].kind == TK_RParen) {
            end_token++;
        }
        return lex_token_end_offset(lexer, &lexer->tokens[end_token]);
    }
    return lex_token_end_offset(lexer, &lexer->tokens[variant->token_index]);
}

internal void format_emit_type_enum_multiline(StringBuilder* sb,
                                              const Cst*     cst,
                                              const Lexer*   lexer,
                                              u32            node_index,
                                              u32            indent_level)
{
    const CstNode* node = &cst->nodes[node_index];
    ASSERT(node->kind == CK_TypeEnum, "Expected enum type node");

    const CstEnumTypeInfo* enum_type           = &cst->enum_types[node->a];
    Array(usize) variant_prefix_widths         = NULL;
    Array(usize) variant_aligned_prefix_widths = NULL;
    Array(usize) variant_code_widths           = NULL;
    Array(bool) variant_has_comments           = NULL;
    Array(u32) variant_comment_indices         = NULL;
    Array(usize) variant_comment_columns       = NULL;

    for (u32 i = 0; i < enum_type->variant_count; ++i) {
        const CstEnumVariant* variant =
            &cst->enum_variants[enum_type->first_variant + i];
        usize prefix_width =
            format_rendered_enum_variant_prefix_width(cst, lexer, variant);
        usize end_offset = format_enum_variant_end_offset(cst, lexer, variant);
        u32   comment_index = U32_MAX;
        bool  has_comment   = format_find_trailing_comment_index_after_offset(
            lexer->source, lexer, end_offset, &comment_index);
        array_push(variant_prefix_widths, prefix_width);
        array_push(variant_aligned_prefix_widths, prefix_width);
        array_push(variant_code_widths, 0);
        array_push(variant_has_comments, has_comment);
        array_push(variant_comment_indices, comment_index);
        array_push(variant_comment_columns, 0);
    }

    for (u32 i = 0; i < enum_type->variant_count;) {
        u32   start             = i;
        usize max_prefix_width  = 0;
        usize previous_end      = 0;
        bool  have_previous_end = false;
        while (i < enum_type->variant_count) {
            const CstEnumVariant* variant =
                &cst->enum_variants[enum_type->first_variant + i];
            usize variant_start = lexer->tokens[variant->token_index].offset;
            if (have_previous_end &&
                format_has_blank_line_between_offsets(
                    lexer->source, previous_end, variant_start)) {
                break;
            }
            if (variant->value_node_index != U32_MAX &&
                variant_prefix_widths[i] > max_prefix_width) {
                max_prefix_width = variant_prefix_widths[i];
            }
            previous_end = format_enum_variant_end_offset(cst, lexer, variant);
            if (variant_has_comments[i]) {
                usize before_offset = lexer->source.source.count;
                if (i + 1 < enum_type->variant_count) {
                    const CstEnumVariant* next_variant =
                        &cst->enum_variants[enum_type->first_variant + i + 1];
                    before_offset =
                        lexer->tokens[next_variant->token_index].offset;
                }
                previous_end = format_trailing_comment_group_end_offset(
                    lexer, variant_comment_indices[i], before_offset);
            }
            have_previous_end = true;
            i++;
        }
        if (max_prefix_width > 0) {
            for (u32 variant_index = start; variant_index < i;
                 ++variant_index) {
                variant_aligned_prefix_widths[variant_index] = max_prefix_width;
            }
        }
    }

    for (u32 i = 0; i < enum_type->variant_count; ++i) {
        const CstEnumVariant* variant =
            &cst->enum_variants[enum_type->first_variant + i];
        variant_code_widths[i] = format_rendered_enum_variant_width(
            cst,
            lexer,
            variant,
            indent_level,
            variant_aligned_prefix_widths[i]);
    }

    for (u32 i = 0; i < enum_type->variant_count;) {
        u32   start          = i;
        usize comment_column = 0;
        usize previous_end   = 0;
        bool  have_previous  = false;
        while (i < enum_type->variant_count) {
            const CstEnumVariant* variant =
                &cst->enum_variants[enum_type->first_variant + i];
            usize variant_start = lexer->tokens[variant->token_index].offset;
            if (have_previous &&
                format_has_blank_line_between_offsets(
                    lexer->source, previous_end, variant_start)) {
                break;
            }
            if (variant_has_comments[i] &&
                variant_code_widths[i] + 1 > comment_column) {
                comment_column = variant_code_widths[i] + 1;
            }
            previous_end = format_enum_variant_end_offset(cst, lexer, variant);
            if (variant_has_comments[i]) {
                usize before_offset = lexer->source.source.count;
                if (i + 1 < enum_type->variant_count) {
                    const CstEnumVariant* next_variant =
                        &cst->enum_variants[enum_type->first_variant + i + 1];
                    before_offset =
                        lexer->tokens[next_variant->token_index].offset;
                }
                previous_end = format_trailing_comment_group_end_offset(
                    lexer, variant_comment_indices[i], before_offset);
            }
            have_previous = true;
            i++;
        }
        if (comment_column > 0) {
            for (u32 variant_index = start; variant_index < i;
                 ++variant_index) {
                variant_comment_columns[variant_index] = comment_column;
            }
        }
    }

    format_emit_enum_header(sb, cst, lexer, enum_type);
    sb_append_cstr(sb, " {\n");

    u32 open_brace_token = node->token_index;
    while (open_brace_token < array_count(lexer->tokens) &&
           lexer->tokens[open_brace_token].kind != TK_LBrace) {
        open_brace_token++;
    }
    u32 comment_index = 0;
    if (open_brace_token < array_count(lexer->tokens)) {
        usize open_brace_end =
            lex_token_end_offset(lexer, &lexer->tokens[open_brace_token]);
        format_skip_block_comments_before_offset(
            lexer, &comment_index, open_brace_end);
    }

    usize previous_variant_end_offset = 0;
    bool  have_previous_variant       = false;

    for (u32 i = 0; i < enum_type->variant_count; ++i) {
        const CstEnumVariant* variant =
            &cst->enum_variants[enum_type->first_variant + i];
        usize variant_start_offset = lexer->tokens[variant->token_index].offset;
        usize group_start_offset   = variant_start_offset;
        if (comment_index < array_count(lexer->comments) &&
            lexer->comments[comment_index].offset < variant_start_offset) {
            group_start_offset = lexer->comments[comment_index].offset;
        }
        if (have_previous_variant &&
            format_has_blank_line_between_offsets(lexer->source,
                                                  previous_variant_end_offset,
                                                  group_start_offset)) {
            sb_append_char(sb, '\n');
        }
        format_emit_block_comments_before_token(sb,
                                                lexer,
                                                &comment_index,
                                                variant->token_index,
                                                indent_level + 1,
                                                NULL);
        format_emit_indent(sb, indent_level + 1);
        format_emit_enum_variant_code(
            sb, cst, lexer, variant, variant_aligned_prefix_widths[i]);
        usize variant_end_offset =
            format_enum_variant_end_offset(cst, lexer, variant);
        if (variant_has_comments[i]) {
            usize before_offset = lexer->source.source.count;
            if (i + 1 < enum_type->variant_count) {
                const CstEnumVariant* next_variant =
                    &cst->enum_variants[enum_type->first_variant + i + 1];
                before_offset = lexer->tokens[next_variant->token_index].offset;
            }
            u32    first_comment_index = variant_comment_indices[i];
            string comment_text =
                format_merged_trailing_comment_text(lexer,
                                                    first_comment_index,
                                                    variant_comment_columns[i],
                                                    before_offset,
                                                    &comment_index);
            format_emit_trailing_comment_text_aligned(
                sb,
                comment_text,
                variant_comment_columns[i],
                variant_code_widths[i]);
            if (comment_index > first_comment_index &&
                comment_index <= array_count(lexer->comments)) {
                variant_end_offset =
                    lexer->comments[comment_index - 1].end_offset;
            }
        } else {
            sb_append_char(sb, '\n');
        }
        previous_variant_end_offset = variant_end_offset;
        have_previous_variant       = true;
    }

    u32   close_token  = format_node_end_token_index(cst, lexer, node_index);
    usize close_offset = lexer->tokens[close_token].offset;
    format_emit_block_comments_before_offset(
        sb, lexer, &comment_index, close_offset, indent_level + 1, NULL);
    format_emit_indent(sb, indent_level);
    sb_append_char(sb, '}');

    array_free(variant_prefix_widths);
    array_free(variant_aligned_prefix_widths);
    array_free(variant_code_widths);
    array_free(variant_has_comments);
    array_free(variant_comment_indices);
    array_free(variant_comment_columns);
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
    Array(usize) field_code_widths     = NULL;
    Array(usize) field_end_offsets     = NULL;
    Array(bool) field_has_comments     = NULL;
    Array(u32) field_comment_indices   = NULL;
    Array(usize) field_comment_columns = NULL;
    usize field_indent_width           = (usize)(indent_level + 1) * 4;
    for (u32 i = 0; i < plex->field_count; ++i) {
        const CstPlexField* field = &cst->plex_fields[plex->first_field + i];
        usize               type_width =
            format_rendered_expr_width(cst, lexer, field->type_node_index, 0);
        usize code_width =
            field_indent_width + max_field_width + 1 + type_width;
        u32 type_end_token =
            format_node_end_token_index(cst, lexer, field->type_node_index);
        usize type_end_offset =
            lex_token_end_offset(lexer, &lexer->tokens[type_end_token]);
        u32  comment_index = U32_MAX;
        bool has_comment   = format_find_trailing_comment_index_after_offset(
            lexer->source, lexer, type_end_offset, &comment_index);
        array_push(field_code_widths, code_width);
        array_push(field_end_offsets, type_end_offset);
        array_push(field_has_comments, has_comment);
        array_push(field_comment_indices, comment_index);
        array_push(field_comment_columns, 0);
    }
    for (u32 i = 0; i < plex->field_count;) {
        u32   start          = i;
        usize comment_column = 0;
        usize previous_end   = 0;
        bool  have_previous  = false;
        while (i < plex->field_count) {
            const CstPlexField* field =
                &cst->plex_fields[plex->first_field + i];
            usize field_start = lexer->tokens[field->token_index].offset;
            usize group_start = format_first_comment_or_offset_between(
                lexer, previous_end, field_start);
            if (have_previous &&
                format_has_blank_line_between_offsets(
                    lexer->source, previous_end, group_start)) {
                break;
            }
            if (field_has_comments[i] &&
                field_code_widths[i] + 1 > comment_column) {
                comment_column = field_code_widths[i] + 1;
            }
            previous_end = field_end_offsets[i];
            if (field_has_comments[i]) {
                previous_end =
                    lexer->comments[field_comment_indices[i]].end_offset;
            }
            have_previous = true;
            i++;
        }
        if (comment_column > 0) {
            for (u32 field_index = start; field_index < i; ++field_index) {
                field_comment_columns[field_index] = comment_column;
            }
        }
    }

    bool is_union = (plex->flags & CPTF_Union) != 0;
    sb_append_cstr(sb, is_union ? "union" : "plex");
    if (plex->generic_params_index != U32_MAX) {
        const CstGenericParams* generic =
            &cst->generic_params[plex->generic_params_index];
        sb_append_cstr(sb, " [");
        for (u32 i = 0; i < generic->symbol_count; ++i) {
            if (i > 0) {
                sb_append_cstr(sb, ", ");
            }
            sb_append_string(
                sb,
                lex_symbol(
                    lexer,
                    cst->generic_param_symbols[generic->first_symbol + i]));
        }
        sb_append_char(sb, ']');
    }
    if (!is_union && (plex->flags & CPTF_Packed)) {
        sb_append_cstr(sb, " #packed");
    } else if (!is_union && (plex->flags & CPTF_C)) {
        sb_append_cstr(sb, " #c");
    }
    sb_append_cstr(sb, " {\n");

    u32 open_brace_token = node->token_index;
    while (open_brace_token < array_count(lexer->tokens) &&
           lexer->tokens[open_brace_token].kind != TK_LBrace) {
        open_brace_token++;
    }
    u32 comment_index = 0;
    if (open_brace_token < array_count(lexer->tokens)) {
        usize open_brace_end =
            lex_token_end_offset(lexer, &lexer->tokens[open_brace_token]);
        format_skip_block_comments_before_offset(
            lexer, &comment_index, open_brace_end);
    }

    usize previous_field_end_offset = 0;
    bool  have_previous_field       = false;

    for (u32 i = 0; i < plex->field_count; ++i) {
        const CstPlexField* field = &cst->plex_fields[plex->first_field + i];
        string field_name         = lex_symbol(lexer, field->symbol_handle);
        usize  field_start_offset = lexer->tokens[field->token_index].offset;
        usize  group_start_offset = field_start_offset;
        if (comment_index < array_count(lexer->comments) &&
            lexer->comments[comment_index].offset < field_start_offset) {
            group_start_offset = lexer->comments[comment_index].offset;
        }
        if (have_previous_field &&
            format_has_blank_line_between_offsets(
                lexer->source, previous_field_end_offset, group_start_offset)) {
            sb_append_char(sb, '\n');
        }
        format_emit_block_comments_before_token(sb,
                                                lexer,
                                                &comment_index,
                                                field->token_index,
                                                indent_level + 1,
                                                NULL);
        format_emit_indent(sb, indent_level + 1);
        sb_append_string(sb, field_name);
        for (usize pad = field_name.count; pad <= max_field_width; ++pad) {
            sb_append_char(sb, ' ');
        }
        format_emit_expr(sb, cst, lexer, field->type_node_index, 0);
        usize field_end_offset = field_end_offsets[i];
        if (field_has_comments[i]) {
            usize before_offset = lexer->source.source.count;
            if (i + 1 < plex->field_count) {
                const CstPlexField* next_field =
                    &cst->plex_fields[plex->first_field + i + 1];
                before_offset = lexer->tokens[next_field->token_index].offset;
            }
            u32    first_comment_index = field_comment_indices[i];
            string comment_text =
                format_merged_trailing_comment_text(lexer,
                                                    first_comment_index,
                                                    field_comment_columns[i],
                                                    before_offset,
                                                    &comment_index);
            format_emit_trailing_comment_text_aligned(sb,
                                                      comment_text,
                                                      field_comment_columns[i],
                                                      field_code_widths[i]);
            if (comment_index > first_comment_index &&
                comment_index <= array_count(lexer->comments)) {
                field_end_offset =
                    lexer->comments[comment_index - 1].end_offset;
            }
        } else {
            sb_append_char(sb, '\n');
        }
        previous_field_end_offset = field_end_offset;
        have_previous_field       = true;
    }
    u32   close_token  = format_node_end_token_index(cst, lexer, node_index);
    usize close_offset = lexer->tokens[close_token].offset;
    format_emit_block_comments_before_offset(
        sb, lexer, &comment_index, close_offset, indent_level + 1, NULL);
    format_emit_indent(sb, indent_level);
    sb_append_char(sb, '}');
    array_free(field_code_widths);
    array_free(field_end_offsets);
    array_free(field_has_comments);
    array_free(field_comment_indices);
    array_free(field_comment_columns);
}

internal bool format_collect_aligned_statement(Arena*       arena,
                                               const Cst*   cst,
                                               const Lexer* lexer,
                                               u32          node_index,
                                               bool include_inferred_variables,
                                               FormatAlignedStatement* out_stmt)
{
    const CstNode* node = &cst->nodes[node_index];
    if (node->kind == CK_Variable) {
        const CstNode* payload = &cst->nodes[node->b];
        string         type    = {0};
        string         value   = {0};

        if (payload->kind == CK_AnnotatedValue) {
            type = format_render_expr_to_string(arena, cst, lexer, payload->a);
            value =
                format_render_value_to_string(arena, cst, lexer, payload->b);
        } else if (payload->kind == CK_ZeroInit) {
            type = format_render_expr_to_string(arena, cst, lexer, payload->a);
        } else if (payload->kind == CK_Undefined) {
            type  = format_render_expr_to_string(arena, cst, lexer, payload->a);
            value = s("undefined");
        } else {
            if (!include_inferred_variables) {
                return false;
            }
            if (!format_statement_is_single_line(cst, lexer, node_index)) {
                return false;
            }
            value = format_render_value_to_string(arena, cst, lexer, node->b);
        }

        *out_stmt = (FormatAlignedStatement){
            .node_index = node_index,
            .symbol     = lex_symbol(lexer, cst_get_symbol(node)),
            .type       = type,
            .value      = value,
            .is_bind    = false,
            .has_value  = payload->kind != CK_ZeroInit,
            .is_public  = (node->flags & CNF_Public) != 0,
            .uses_standard_single_line = payload->kind != CK_AnnotatedValue &&
                                         payload->kind != CK_ZeroInit &&
                                         payload->kind != CK_Undefined,
        };
        return true;
    }

    if (node->kind == CK_Bind) {
        const CstNode* payload       = &cst->nodes[node->b];
        string         type          = {0};
        string         value         = {0};
        const CstNode* value_payload = payload;

        if (payload->kind == CK_AnnotatedValue) {
            type = format_render_expr_to_string(arena, cst, lexer, payload->a);
            value =
                format_render_value_to_string(arena, cst, lexer, payload->b);
            value_payload = &cst->nodes[payload->b];
        } else if (format_node_is_value_constant_payload(payload)) {
            value = format_render_value_to_string(arena, cst, lexer, node->b);
        } else {
            return false;
        }

        if (!format_node_is_value_constant_payload(value_payload)) {
            return false;
        }
        bool is_multiline_string_value =
            value_payload->kind == CK_StringLiteral ||
            value_payload->kind == CK_StringConcat;
        if (!format_statement_is_single_line(cst, lexer, node_index) &&
            !is_multiline_string_value) {
            return false;
        }

        *out_stmt = (FormatAlignedStatement){
            .node_index = node_index,
            .symbol     = lex_symbol(lexer, cst_get_symbol(node)),
            .type       = type,
            .value      = value,
            .is_bind    = true,
            .has_value  = true,
            .is_public  = (node->flags & CNF_Public) != 0,
            .uses_standard_single_line = false,
        };
        return true;
    }

    if (node->kind == CK_Assign) {
        if (!format_statement_is_single_line(cst, lexer, node_index)) {
            return false;
        }

        *out_stmt = (FormatAlignedStatement){
            .node_index = node_index,
            .symbol = format_render_expr_to_string(arena, cst, lexer, node->a),
            .op     = format_assignment_operator(lexer, node),
            .value  = format_render_expr_to_string(arena, cst, lexer, node->b),
            .is_assignment             = true,
            .has_value                 = true,
            .uses_standard_single_line = true,
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
            if (owner->kind == CK_ExprBlock && owner->a == node_index) {
                return true;
            }
            if (owner->kind == CK_On) {
                const CstOnInfo* on = &cst->ons[owner->b];
                for (u32 branch_index = 0; branch_index < on->branch_count;
                     ++branch_index) {
                    const CstOnBranch* branch =
                        &cst->on_branches[on->first_branch + branch_index];
                    if (branch->expr_node_index == node_index) {
                        return true;
                    }
                }
            }
            if (owner->kind == CK_TopOn &&
                cst->top_ons[owner->a].body_node_index == node_index) {
                return true;
            }
            if (owner->kind == CK_Bind) {
                u32 value_node_index = owner->b;
                if (cst->nodes[value_node_index].kind == CK_AnnotatedValue) {
                    value_node_index = cst->nodes[value_node_index].b;
                }
                if (cst->nodes[value_node_index].kind == CK_FnBlock &&
                    cst->nodes[value_node_index].b == node_index) {
                    return true;
                }
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

    for (u32 owner_index = 0; owner_index < end; ++owner_index) {
        const CstNode* owner = &cst->nodes[owner_index];
        if (owner->kind != CK_Bind) {
            continue;
        }

        u32 value_node_index = owner->b;
        if (cst->nodes[value_node_index].kind == CK_AnnotatedValue) {
            value_node_index = cst->nodes[value_node_index].b;
        }
        if (cst->nodes[value_node_index].kind == CK_FnBlock &&
            cst->nodes[value_node_index].b == node_index) {
            return true;
        }
    }

    for (u32 i = 0; i < end; ++i) {
        const CstNode* block = &cst->nodes[i];
        if (i == current_block || block->kind != CK_Block ||
            !(block->a <= node_index && node_index < block->b)) {
            continue;
        }
        if (current_block < array_count(cst->nodes)) {
            const CstNode* current = &cst->nodes[current_block];
            if (current->kind == CK_Block && block->a <= current_block &&
                current_block < block->b && current->a <= node_index &&
                node_index < current->b) {
                continue;
            }
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
            if (owner->kind == CK_ExprBlock && owner->a == i) {
                return true;
            }
            if (owner->kind == CK_On) {
                const CstOnInfo* on = &cst->ons[owner->b];
                for (u32 branch_index = 0; branch_index < on->branch_count;
                     ++branch_index) {
                    const CstOnBranch* branch =
                        &cst->on_branches[on->first_branch + branch_index];
                    if (branch->expr_node_index == i) {
                        return true;
                    }
                }
            }
            if (owner->kind == CK_TopOn &&
                cst->top_ons[owner->a].body_node_index == i) {
                return true;
            }
            if (owner->kind == CK_Bind) {
                u32 value_node_index = owner->b;
                if (cst->nodes[value_node_index].kind == CK_AnnotatedValue) {
                    value_node_index = cst->nodes[value_node_index].b;
                }
                if (cst->nodes[value_node_index].kind == CK_FnBlock &&
                    cst->nodes[value_node_index].b == i) {
                    return true;
                }
            }
        }
    }

    for (u32 i = node_index + 1; i < end; ++i) {
        const CstNode* node = &cst->nodes[i];
        if ((node->kind == CK_Return || node->kind == CK_ReturnExpr ||
             node->kind == CK_Defer) &&
            node->a == node_index) {
            return true;
        }
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
        if (node->kind == CK_For && node->b == node_index) {
            return true;
        }
        if (node->kind == CK_ExprBlock && node->a == node_index) {
            return true;
        }
        if (node->kind == CK_On) {
            const CstOnInfo* on = &cst->ons[node->b];
            for (u32 branch_index = 0; branch_index < on->branch_count;
                 ++branch_index) {
                const CstOnBranch* branch =
                    &cst->on_branches[on->first_branch + branch_index];
                if (branch->expr_node_index == node_index) {
                    return true;
                }
            }
        }
        if (node->kind == CK_TopOn &&
            cst->top_ons[node->a].body_node_index == node_index) {
            return true;
        }
        if (node->kind == CK_Call) {
            const CstCallInfo* call = &cst->calls[node->b];
            for (u32 arg_index = 0; arg_index < call->arg_count; ++arg_index) {
                if (cst->call_args[call->first_arg + arg_index] == node_index) {
                    return true;
                }
            }
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
    if (a.is_assignment || b.is_assignment) {
        return a.is_assignment && b.is_assignment;
    }
    return true;
}

internal void
format_emit_aligned_statement_group(StringBuilder*                sb,
                                    const FormatAlignedStatement* stmts,
                                    u32                           stmt_count,
                                    u32                           indent_level)
{
    usize max_symbol_width = 0;
    usize max_type_width   = 0;
    for (u32 i = 0; i < stmt_count; ++i) {
        usize symbol_width =
            stmts[i].symbol.count + (stmts[i].is_public ? 4 : 0);
        if (symbol_width > max_symbol_width) {
            max_symbol_width = symbol_width;
        }
        if (stmts[i].type.count > max_type_width) {
            max_type_width = stmts[i].type.count;
        }
    }
    bool has_type_column = max_type_width > 0;

    for (u32 i = 0; i < stmt_count; ++i) {
        format_emit_indent(sb, indent_level);
        if (stmts[i].is_public) {
            sb_append_cstr(sb, "pub ");
        }
        sb_append_string(sb, stmts[i].symbol);
        usize symbol_width =
            stmts[i].symbol.count + (stmts[i].is_public ? 4 : 0);
        for (usize pad = symbol_width; pad < max_symbol_width; ++pad) {
            sb_append_char(sb, ' ');
        }

        if (stmts[i].is_assignment) {
            sb_append_char(sb, ' ');
            sb_append_string(sb, stmts[i].op);
            sb_append_char(sb, ' ');
            sb_append_string(sb, stmts[i].value);
        } else if (!has_type_column) {
            if (stmts[i].is_bind) {
                usize value_start_width =
                    (usize)indent_level * 4 + max_symbol_width + 4;
                if (value_start_width + stmts[i].value.count <=
                    FORMAT_WRAP_WIDTH) {
                    sb_append_cstr(sb, " :: ");
                    sb_append_string(sb, stmts[i].value);
                } else {
                    sb_append_cstr(sb, " ::");
                    sb_append_char(sb, '\n');
                    format_emit_indent(sb, indent_level + 1);
                    sb_append_string(sb, stmts[i].value);
                }
            } else {
                sb_append_cstr(sb, " := ");
                sb_append_string(sb, stmts[i].value);
            }
        } else {
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
        }
        sb_append_char(sb, '\n');
    }
}

internal void format_emit_fn_signature_prefix(StringBuilder*        sb,
                                              const Cst*            cst,
                                              const Lexer*          lexer,
                                              const CstFnSignature* signature)
{
    sb_append_cstr(sb, "fn");
    if (signature->generic_params_index != U32_MAX) {
        const CstGenericParams* generic =
            &cst->generic_params[signature->generic_params_index];
        sb_append_cstr(sb, " [");
        for (u32 i = 0; i < generic->symbol_count; ++i) {
            if (i > 0) {
                sb_append_cstr(sb, ", ");
            }
            sb_append_string(
                sb,
                lex_symbol(
                    lexer,
                    cst->generic_param_symbols[generic->first_symbol + i]));
        }
        sb_append_char(sb, ']');
    }
}

internal void format_emit_fn_param(StringBuilder*  sb,
                                   const Cst*      cst,
                                   const Lexer*    lexer,
                                   const CstParam* param)
{
    if (param->symbol_handle != U32_MAX) {
        sb_append_string(sb, lex_symbol(lexer, param->symbol_handle));
        sb_append_cstr(sb, ": ");
    }
    format_emit_expr(sb, cst, lexer, param->type_node_index, 0);
    if (param->default_node_index != U32_MAX) {
        sb_append_cstr(sb, " = ");
        format_emit_expr(sb, cst, lexer, param->default_node_index, 0);
    }
}

internal usize format_fn_signature_param_name_width(
    const Cst* cst, const Lexer* lexer, const CstFnSignature* signature)
{
    usize name_width = 0;
    for (u32 i = 0; i < signature->param_count; ++i) {
        const CstParam* param = &cst->params[signature->first_param + i];
        if (param->symbol_handle == U32_MAX) {
            continue;
        }
        usize name_count = lex_symbol(lexer, param->symbol_handle).count;
        if (name_count > name_width) {
            name_width = name_count;
        }
    }
    return name_width;
}

internal void format_emit_fn_param_aligned(StringBuilder*  sb,
                                           const Cst*      cst,
                                           const Lexer*    lexer,
                                           const CstParam* param,
                                           usize           name_width)
{
    if (param->symbol_handle == U32_MAX || name_width == 0) {
        format_emit_fn_param(sb, cst, lexer, param);
        return;
    }

    string name = lex_symbol(lexer, param->symbol_handle);
    sb_append_string(sb, name);
    for (usize pad = name.count; pad <= name_width; ++pad) {
        sb_append_char(sb, ' ');
    }
    sb_append_cstr(sb, ": ");
    format_emit_expr(sb, cst, lexer, param->type_node_index, 0);
    if (param->default_node_index != U32_MAX) {
        sb_append_cstr(sb, " = ");
        format_emit_expr(sb, cst, lexer, param->default_node_index, 0);
    }
}

internal void format_emit_fn_signature_one_line(StringBuilder*        sb,
                                                const Cst*            cst,
                                                const Lexer*          lexer,
                                                const CstFnSignature* signature,
                                                bool include_return_type)
{
    format_emit_fn_signature_prefix(sb, cst, lexer, signature);
    sb_append_cstr(sb, " (");
    for (u32 i = 0; i < signature->param_count; ++i) {
        if (i > 0) {
            sb_append_cstr(sb, ", ");
        }

        const CstParam* param = &cst->params[signature->first_param + i];
        format_emit_fn_param(sb, cst, lexer, param);
    }
    if (signature->is_varargs) {
        if (signature->param_count > 0) {
            sb_append_cstr(sb, ", ");
        }
        sb_append_cstr(sb, "...");
    }
    sb_append_char(sb, ')');

    if (include_return_type && signature->return_type_node_index != U32_MAX) {
        sb_append_cstr(sb, " -> ");
        format_emit_expr(sb, cst, lexer, signature->return_type_node_index, 0);
    }
}

internal void format_emit_fn_signature(StringBuilder* sb,
                                       const Cst*     cst,
                                       const Lexer*   lexer,
                                       u32            signature_index,
                                       bool           include_return_type)
{
    const CstFnSignature* signature = &cst->fn_signatures[signature_index];

    Arena temp_arena                = {0};
    arena_init(&temp_arena);
    StringBuilder single_line = {0};
    sb_init(&single_line, &temp_arena);
    format_emit_fn_signature_one_line(
        &single_line, cst, lexer, signature, include_return_type);

    usize start_column = format_sb_current_column(sb);
    if (signature->param_count == 0 ||
        start_column + single_line.size <= FORMAT_WRAP_WIDTH) {
        sb_append_string(sb, sb_to_string(&single_line));
        arena_done(&temp_arena);
        return;
    }

    format_emit_fn_signature_prefix(sb, cst, lexer, signature);
    sb_append_cstr(sb, " (");
    usize param_column = format_sb_current_column(sb);
    usize name_width =
        format_fn_signature_param_name_width(cst, lexer, signature);
    for (u32 i = 0; i < signature->param_count; ++i) {
        if (i > 0) {
            sb_append_cstr(sb, ",\n");
            format_emit_spaces(sb, param_column);
        }
        const CstParam* param = &cst->params[signature->first_param + i];
        format_emit_fn_param_aligned(sb, cst, lexer, param, name_width);
    }
    if (signature->is_varargs) {
        if (signature->param_count > 0) {
            sb_append_cstr(sb, ",\n");
            format_emit_spaces(sb, param_column);
        }
        sb_append_cstr(sb, "...");
    }
    sb_append_char(sb, ')');

    if (include_return_type && signature->return_type_node_index != U32_MAX) {
        sb_append_cstr(sb, " -> ");
        format_emit_expr(sb, cst, lexer, signature->return_type_node_index, 0);
    }

    arena_done(&temp_arena);
}

typedef struct {
    usize name_width;
    usize bind_prefix_width;
} FormatFfiEntryWidths;

internal usize format_ffi_entry_bind_prefix_width(const Cst*   cst,
                                                  const Lexer* lexer,
                                                  u32          ffi_info_index)
{
    const CstFfiInfo* ffi = &cst->ffi_infos[ffi_info_index];
    if (ffi->foreign_symbol_handle == ffi->symbol_handle) {
        return 0;
    }
    usize width = lex_symbol(lexer, ffi->symbol_handle).count;
    if (ffi->flags & CNF_Public) {
        width += 4;
    }
    return width;
}

internal void format_emit_ffi_entry_prefix(StringBuilder*       sb,
                                           const Cst*           cst,
                                           const Lexer*         lexer,
                                           u32                  ffi_info_index,
                                           FormatFfiEntryWidths widths)
{
    const CstFfiInfo* ffi  = &cst->ffi_infos[ffi_info_index];
    string            name = lex_symbol(lexer, ffi->symbol_handle);
    bool  is_renamed       = ffi->foreign_symbol_handle != ffi->symbol_handle;
    usize prefix_width     = name.count;
    if (ffi->flags & CNF_Public) {
        sb_append_cstr(sb, "pub ");
        prefix_width += 4;
    }
    sb_append_string(sb, name);
    if (is_renamed) {
        for (usize pad = prefix_width; pad <= widths.bind_prefix_width; ++pad) {
            sb_append_char(sb, ' ');
        }
        sb_append_cstr(sb, ":: ");
        sb_append_string(sb, lex_symbol(lexer, ffi->foreign_symbol_handle));
        sb_append_char(sb, ' ');
    } else {
        for (usize pad = name.count; pad <= widths.name_width; ++pad) {
            sb_append_char(sb, ' ');
        }
    }
    sb_append_char(sb, '(');
}

internal void format_emit_ffi_entry_one_line(StringBuilder* sb,
                                             const Cst*     cst,
                                             const Lexer*   lexer,
                                             u32            ffi_info_index,
                                             FormatFfiEntryWidths widths)
{
    const CstFfiInfo*     ffi       = &cst->ffi_infos[ffi_info_index];
    const CstFnSignature* signature = &cst->fn_signatures[ffi->signature_index];

    format_emit_ffi_entry_prefix(sb, cst, lexer, ffi_info_index, widths);
    for (u32 i = 0; i < signature->param_count; ++i) {
        if (i > 0) {
            sb_append_cstr(sb, ", ");
        }
        const CstParam* param = &cst->params[signature->first_param + i];
        format_emit_fn_param(sb, cst, lexer, param);
    }
    if (signature->is_varargs) {
        if (signature->param_count > 0) {
            sb_append_cstr(sb, ", ");
        }
        sb_append_cstr(sb, "...");
    }
    sb_append_char(sb, ')');
    if (signature->return_type_node_index != U32_MAX) {
        sb_append_cstr(sb, " -> ");
        format_emit_expr(sb, cst, lexer, signature->return_type_node_index, 0);
    }
}

internal void format_emit_ffi_entry(StringBuilder*       sb,
                                    const Cst*           cst,
                                    const Lexer*         lexer,
                                    u32                  ffi_info_index,
                                    FormatFfiEntryWidths widths)
{
    const CstFfiInfo*     ffi       = &cst->ffi_infos[ffi_info_index];
    const CstFnSignature* signature = &cst->fn_signatures[ffi->signature_index];

    Arena temp_arena                = {0};
    arena_init(&temp_arena);
    StringBuilder single_line = {0};
    sb_init(&single_line, &temp_arena);
    format_emit_ffi_entry_one_line(
        &single_line, cst, lexer, ffi_info_index, widths);

    usize start_column = format_sb_current_column(sb);
    if (signature->param_count == 0 ||
        start_column + single_line.size <= FORMAT_WRAP_WIDTH) {
        sb_append_string(sb, sb_to_string(&single_line));
        arena_done(&temp_arena);
        return;
    }

    format_emit_ffi_entry_prefix(sb, cst, lexer, ffi_info_index, widths);
    usize param_column = format_sb_current_column(sb);
    usize param_name_width =
        format_fn_signature_param_name_width(cst, lexer, signature);
    for (u32 i = 0; i < signature->param_count; ++i) {
        if (i > 0) {
            sb_append_cstr(sb, ",\n");
            format_emit_spaces(sb, param_column);
        }
        const CstParam* param = &cst->params[signature->first_param + i];
        format_emit_fn_param_aligned(sb, cst, lexer, param, param_name_width);
    }
    if (signature->is_varargs) {
        if (signature->param_count > 0) {
            sb_append_cstr(sb, ",\n");
            format_emit_spaces(sb, param_column);
        }
        sb_append_cstr(sb, "...");
    }
    sb_append_char(sb, ')');
    if (signature->return_type_node_index != U32_MAX) {
        sb_append_cstr(sb, " -> ");
        format_emit_expr(sb, cst, lexer, signature->return_type_node_index, 0);
    }

    arena_done(&temp_arena);
}

internal void format_emit_ffi_def(StringBuilder* sb,
                                  const Cst*     cst,
                                  const Lexer*   lexer,
                                  u32            ffi_info_index)
{
    const CstFfiInfo* ffi = &cst->ffi_infos[ffi_info_index];

    sb_append_cstr(sb, "ffi ");
    format_emit_expr(sb, cst, lexer, ffi->library_node_index, 0);
    sb_append_char(sb, ' ');
    format_emit_ffi_entry(
        sb,
        cst,
        lexer,
        ffi_info_index,
        (FormatFfiEntryWidths){
            .name_width = lex_symbol(lexer, ffi->symbol_handle).count,
            .bind_prefix_width =
                format_ffi_entry_bind_prefix_width(cst, lexer, ffi_info_index),
        });
}

internal bool format_ffi_infos_have_blank_line_between(const Cst*   cst,
                                                       const Lexer* lexer,
                                                       u32 previous_ffi_info,
                                                       u32 current_ffi_info)
{
    const CstFfiInfo* current = &cst->ffi_infos[current_ffi_info];
    if (current->token_index == 0) {
        return false;
    }

    u32 previous_end_token_index = current->token_index - 1;
    if (previous_end_token_index >= array_count(lexer->tokens)) {
        return false;
    }

    usize previous_end =
        lex_token_end_offset(lexer, &lexer->tokens[previous_end_token_index]);
    usize current_start = lexer->tokens[current->token_index].offset;
    UNUSED(cst);
    UNUSED(previous_ffi_info);
    return format_has_blank_line_between_offsets(
        lexer->source, previous_end, current_start);
}

internal FormatFfiEntryWidths format_ffi_block_group_entry_widths(
    const Cst* cst, const Lexer* lexer, u32 first_ffi_info, u32 end_ffi_info)
{
    FormatFfiEntryWidths widths = {
        .name_width        = 0,
        .bind_prefix_width = 0,
    };
    for (u32 i = first_ffi_info; i < end_ffi_info; ++i) {
        usize name_width =
            lex_symbol(lexer, cst->ffi_infos[i].symbol_handle).count;
        if (name_width > widths.name_width) {
            widths.name_width = name_width;
        }
        usize bind_prefix_width =
            format_ffi_entry_bind_prefix_width(cst, lexer, i);
        if (bind_prefix_width > widths.bind_prefix_width) {
            widths.bind_prefix_width = bind_prefix_width;
        }
    }
    return widths;
}

internal void format_emit_ffi_block_group(StringBuilder* sb,
                                          const Cst*     cst,
                                          const Lexer*   lexer,
                                          u32            first_ffi_info,
                                          u32            end_ffi_info,
                                          u32            indent_level)
{
    FormatFfiEntryWidths widths = format_ffi_block_group_entry_widths(
        cst, lexer, first_ffi_info, end_ffi_info);
    for (u32 i = first_ffi_info; i < end_ffi_info; ++i) {
        format_emit_indent(sb, indent_level + 1);
        format_emit_ffi_entry(sb, cst, lexer, i, widths);
        sb_append_char(sb, '\n');
    }
}

internal void format_emit_ffi_block(StringBuilder* sb,
                                    const Cst*     cst,
                                    const Lexer*   lexer,
                                    u32            ffi_block_info_index,
                                    u32            indent_level)
{
    const CstFfiBlockInfo* block = &cst->ffi_block_infos[ffi_block_info_index];

    sb_append_cstr(sb, "ffi ");
    format_emit_expr(sb, cst, lexer, block->library_node_index, 0);
    sb_append_cstr(sb, " {\n");

    u32 group_start = block->first_ffi_info;
    u32 block_end   = block->first_ffi_info + block->ffi_info_count;
    for (u32 i = group_start + 1; i < block_end; ++i) {
        if (!format_ffi_infos_have_blank_line_between(cst, lexer, i - 1, i)) {
            continue;
        }
        format_emit_ffi_block_group(
            sb, cst, lexer, group_start, i, indent_level);
        sb_append_char(sb, '\n');
        group_start = i;
    }
    format_emit_ffi_block_group(
        sb, cst, lexer, group_start, block_end, indent_level);

    format_emit_indent(sb, indent_level);
    sb_append_char(sb, '}');
}

internal void format_emit_mod_ref(StringBuilder* sb,
                                  const Cst*     cst,
                                  const Lexer*   lexer,
                                  u32            module_path_index)
{
    sb_append_cstr(sb, "use ");
    format_emit_module_path(sb, cst, lexer, module_path_index);
}

internal void format_emit_module_path(StringBuilder* sb,
                                      const Cst*     cst,
                                      const Lexer*   lexer,
                                      u32            module_path_index)
{
    const CstModulePath* path = &cst->module_paths[module_path_index];
    for (u32 i = 0; i < path->symbol_count; ++i) {
        if (i > 0) {
            sb_append_char(sb, '.');
        }
        sb_append_string(
            sb,
            lex_symbol(lexer,
                       cst->module_path_symbols[path->first_symbol + i]));
    }
}

internal void format_emit_top_on(StringBuilder* sb,
                                 const Cst*     cst,
                                 const Lexer*   lexer,
                                 u32            top_on_index,
                                 u32            indent_level)
{
    const CstTopOnInfo* top_on = &cst->top_ons[top_on_index];

    format_emit_indent(sb, indent_level);
    sb_append_cstr(sb, "on ");
    if (top_on->is_negated) {
        sb_append_char(sb, '!');
    }
    sb_append_string(sb, lex_symbol(lexer, top_on->symbol_handle));
    sb_append_cstr(sb, " {\n");
    format_emit_block_contents(
        sb, cst, lexer, top_on->body_node_index, indent_level + 1);
    format_emit_indent(sb, indent_level);
    sb_append_char(sb, '}');
}

internal void format_emit_impl(StringBuilder* sb,
                               const Cst*     cst,
                               const Lexer*   lexer,
                               u32            impl_index,
                               u32            indent_level)
{
    const CstImplInfo* impl = &cst->impls[impl_index];

    format_emit_indent(sb, indent_level);
    sb_append_cstr(sb, "impl ");
    format_emit_expr(sb, cst, lexer, impl->target_type_node_index, 0);
    sb_append_cstr(sb, " {\n\n");
    format_emit_block_contents(
        sb, cst, lexer, impl->body_node_index, indent_level + 1);
    sb_append_char(sb, '\n');
    format_emit_indent(sb, indent_level);
    sb_append_char(sb, '}');
}

internal void format_emit_test(StringBuilder* sb,
                               const Cst*     cst,
                               const Lexer*   lexer,
                               u32            node_index,
                               u32            indent_level)
{
    const CstNode* test = &cst->nodes[node_index];

    format_emit_indent(sb, indent_level);
    sb_append_cstr(sb, "test ");
    format_emit_expr(sb, cst, lexer, test->a, 0);
    sb_append_cstr(sb, " {\n");
    format_emit_block_contents(sb, cst, lexer, test->b, indent_level + 1);
    format_emit_indent(sb, indent_level);
    sb_append_char(sb, '}');
}

internal bool format_emit_block_comments_before_offset(StringBuilder* sb,
                                                       const Lexer*   lexer,
                                                       u32*   io_comment_index,
                                                       usize  end_offset,
                                                       u32    indent_level,
                                                       usize* out_last_end)
{
    bool emitted = false;
    while (*io_comment_index < array_count(lexer->comments)) {
        LexerComment comment = lexer->comments[*io_comment_index];
        if (comment.offset >= end_offset) {
            break;
        }

        format_emit_line_comment(sb, indent_level, comment.text);
        emitted = true;
        if (out_last_end) {
            *out_last_end = comment.end_offset;
        }
        (*io_comment_index)++;
    }
    return emitted;
}

internal bool format_emit_block_comments_before_token(StringBuilder* sb,
                                                      const Lexer*   lexer,
                                                      u32*   io_comment_index,
                                                      u32    token_index,
                                                      u32    indent_level,
                                                      usize* out_last_end)
{
    if (g_format_trivia == NULL || token_index >= array_count(lexer->tokens)) {
        usize end_offset = token_index < array_count(lexer->tokens)
                               ? lexer->tokens[token_index].offset
                               : lexer->source.source.count;
        return format_emit_block_comments_before_offset(sb,
                                                        lexer,
                                                        io_comment_index,
                                                        end_offset,
                                                        indent_level,
                                                        out_last_end);
    }

    u32 first_comment_index = U32_MAX;
    u32 comment_count       = 0;
    if (!format_trivia_comments_before_token(g_format_trivia,
                                             token_index,
                                             &first_comment_index,
                                             &comment_count)) {
        return format_emit_block_comments_before_offset(
            sb,
            lexer,
            io_comment_index,
            lexer->tokens[token_index].offset,
            indent_level,
            out_last_end);
    }

    u32 end_comment_index = first_comment_index + comment_count;
    if (*io_comment_index < first_comment_index) {
        return format_emit_block_comments_before_offset(
            sb,
            lexer,
            io_comment_index,
            lexer->tokens[token_index].offset,
            indent_level,
            out_last_end);
    }
    if (*io_comment_index >= end_comment_index) {
        return false;
    }

    bool emitted = false;
    while (*io_comment_index < end_comment_index &&
           *io_comment_index < array_count(lexer->comments)) {
        LexerComment comment = lexer->comments[*io_comment_index];
        format_emit_line_comment(sb, indent_level, comment.text);
        emitted = true;
        if (out_last_end) {
            *out_last_end = comment.end_offset;
        }
        (*io_comment_index)++;
    }
    return emitted;
}

internal void format_skip_block_comments_before_offset(const Lexer* lexer,
                                                       u32*  io_comment_index,
                                                       usize end_offset)
{
    while (*io_comment_index < array_count(lexer->comments) &&
           lexer->comments[*io_comment_index].offset < end_offset) {
        (*io_comment_index)++;
    }
}

internal bool format_comment_is_trailing_after_offset(NerdSource   source,
                                                      usize        end_offset,
                                                      LexerComment comment)
{
    if (comment.offset < end_offset) {
        return false;
    }

    u32   end_line     = 0;
    u32   end_col      = 0;
    u32   comment_line = 0;
    u32   comment_col  = 0;
    usize line_offset  = end_offset > 0 ? end_offset - 1 : end_offset;
    if (!lex_offset_to_line_col(source, line_offset, &end_line, &end_col) ||
        !lex_offset_to_line_col(
            source, comment.offset, &comment_line, &comment_col)) {
        return false;
    }
    UNUSED(end_col);
    UNUSED(comment_col);
    if (end_line != comment_line) {
        return false;
    }

    for (usize i = end_offset; i < comment.offset && i < source.source.count;
         ++i) {
        if (source.source.data[i] != ' ' && source.source.data[i] != '\t' &&
            source.source.data[i] != '\r') {
            return false;
        }
    }
    return true;
}

internal bool format_node_has_trailing_comment(const Cst*   cst,
                                               const Lexer* lexer,
                                               u32          node_index)
{
    u32 end_token = format_node_end_token_index(cst, lexer, node_index);
    if (format_trivia_trailing_comment_after_token(
            g_format_trivia, end_token, NULL)) {
        return true;
    }

    usize end_offset = lex_token_end_offset(lexer, &lexer->tokens[end_token]);
    for (u32 i = 0; i < array_count(lexer->comments); ++i) {
        if (format_comment_is_trailing_after_offset(
                lexer->source, end_offset, lexer->comments[i])) {
            return true;
        }
    }
    return false;
}

internal bool format_emit_trailing_comment_by_index(StringBuilder* sb,
                                                    const Lexer*   lexer,
                                                    u32* io_comment_index,
                                                    u32  comment_index)
{
    if (comment_index >= array_count(lexer->comments) ||
        comment_index < *io_comment_index) {
        return false;
    }

    LexerComment comment = lexer->comments[comment_index];
    if (sb->size > 0 && sb->data[sb->size - 1] == '\n') {
        sb->data[sb->size - 1] = ' ';
    } else {
        sb_append_char(sb, ' ');
    }
    sb_append_cstr(sb, "--");
    sb_append_string(sb, comment.text);
    sb_append_char(sb, '\n');
    *io_comment_index = comment_index + 1;
    return true;
}

internal bool format_emit_trailing_comment_after_token(StringBuilder* sb,
                                                       const Lexer*   lexer,
                                                       u32* io_comment_index,
                                                       u32  token_index)
{
    u32 comment_index = U32_MAX;
    if (!format_trivia_trailing_comment_after_token(
            g_format_trivia, token_index, &comment_index)) {
        return false;
    }

    return format_emit_trailing_comment_by_index(
        sb, lexer, io_comment_index, comment_index);
}

internal bool format_emit_trailing_comment_after_offset(StringBuilder* sb,
                                                        const Lexer*   lexer,
                                                        u32*  io_comment_index,
                                                        usize end_offset)
{
    if (g_format_trivia != NULL) {
        for (u32 token_index = 0; token_index < array_count(lexer->tokens);
             ++token_index) {
            if (lex_token_end_offset(lexer, &lexer->tokens[token_index]) !=
                end_offset) {
                continue;
            }

            return format_emit_trailing_comment_after_token(
                sb, lexer, io_comment_index, token_index);
        }
    }

    while (*io_comment_index < array_count(lexer->comments) &&
           lexer->comments[*io_comment_index].offset < end_offset) {
        (*io_comment_index)++;
    }

    if (*io_comment_index >= array_count(lexer->comments)) {
        return false;
    }

    LexerComment comment = lexer->comments[*io_comment_index];
    if (!format_comment_is_trailing_after_offset(
            lexer->source, end_offset, comment)) {
        return false;
    }

    return format_emit_trailing_comment_by_index(
        sb, lexer, io_comment_index, *io_comment_index);
}

internal bool
format_find_trailing_comment_index_after_offset(NerdSource   source,
                                                const Lexer* lexer,
                                                usize        end_offset,
                                                u32*         out_comment_index)
{
    if (g_format_trivia != NULL) {
        for (u32 token_index = 0; token_index < array_count(lexer->tokens);
             ++token_index) {
            if (lex_token_end_offset(lexer, &lexer->tokens[token_index]) !=
                end_offset) {
                continue;
            }

            return format_trivia_trailing_comment_after_token(
                g_format_trivia, token_index, out_comment_index);
        }
    }

    return format_find_trailing_comment_index_after_offset_scan(
        source, lexer, end_offset, out_comment_index);
}

internal bool
format_find_trailing_comment_index_after_offset_scan(NerdSource   source,
                                                     const Lexer* lexer,
                                                     usize        end_offset,
                                                     u32* out_comment_index)
{
    for (u32 i = 0; i < array_count(lexer->comments); ++i) {
        LexerComment comment = lexer->comments[i];
        if (format_comment_is_trailing_after_offset(
                source, end_offset, comment)) {
            *out_comment_index = i;
            return true;
        }
    }
    return false;
}

internal void format_emit_trailing_comment_text_aligned(StringBuilder* sb,
                                                        string comment_text,
                                                        usize  comment_column,
                                                        usize  current_column)
{
    string text = format_trim_ascii(comment_text);
    if (comment_column <= current_column) {
        comment_column = current_column + 1;
    }
    for (usize pad = current_column; pad < comment_column; ++pad) {
        sb_append_char(sb, ' ');
    }
    sb_append_cstr(sb, "--");

    if (text.count == 0) {
        sb_append_char(sb, '\n');
        return;
    }

    usize first_prefix_width        = comment_column + 3;
    usize continuation_indent       = 4;
    usize continuation_prefix_width = comment_column + 2 + continuation_indent;
    usize first_width = FORMAT_WRAP_WIDTH > first_prefix_width
                            ? FORMAT_WRAP_WIDTH - first_prefix_width
                            : 1;
    usize continuation_width =
        FORMAT_WRAP_WIDTH > continuation_prefix_width
            ? FORMAT_WRAP_WIDTH - continuation_prefix_width
            : 1;

    bool  first_line = true;
    usize cursor     = 0;
    while (cursor < text.count) {
        while (cursor < text.count && text.data[cursor] == ' ') {
            cursor++;
        }
        if (cursor >= text.count) {
            break;
        }

        usize line_width    = first_line ? first_width : continuation_width;
        usize probe         = cursor;
        usize line_len      = 0;
        usize last_word_end = cursor;
        while (probe < text.count) {
            while (probe < text.count && text.data[probe] == ' ') {
                probe++;
            }
            if (probe >= text.count) {
                break;
            }

            usize word_end = probe;
            while (word_end < text.count && text.data[word_end] != ' ') {
                word_end++;
            }

            usize word_len = word_end - probe;
            usize needed   = line_len == 0 ? word_len : line_len + 1 + word_len;
            if (line_len != 0 && needed > line_width) {
                break;
            }
            if (line_len == 0 && word_len > line_width) {
                last_word_end = word_end;
                line_len      = word_len;
                probe         = word_end;
                break;
            }

            last_word_end = word_end;
            line_len      = needed;
            probe         = word_end;
        }

        sb_append_char(sb, ' ');
        sb_append_string(
            sb, string_from(text.data + cursor, last_word_end - cursor));
        sb_append_char(sb, '\n');
        cursor = last_word_end;

        if (cursor < text.count) {
            for (usize pad = 0; pad < comment_column; ++pad) {
                sb_append_char(sb, ' ');
            }
            sb_append_cstr(sb, "--");
            for (usize pad = 0; pad < continuation_indent; ++pad) {
                sb_append_char(sb, ' ');
            }
        }
        first_line = false;
    }
}

internal string format_merged_trailing_comment_text(const Lexer* lexer,
                                                    u32   first_comment_index,
                                                    usize comment_column,
                                                    usize before_offset,
                                                    u32* out_next_comment_index)
{
    if (out_next_comment_index) {
        *out_next_comment_index = first_comment_index;
    }
    if (first_comment_index >= array_count(lexer->comments)) {
        return (string){0};
    }
    if (out_next_comment_index) {
        *out_next_comment_index = first_comment_index + 1;
    }

    Arena         arena = {0};
    StringBuilder sb    = {0};
    arena_init(&arena);
    sb_init(&sb, &arena);

    LexerComment first = lexer->comments[first_comment_index];
    sb_append_string(&sb, format_trim_ascii(first.text));

    u32 previous_line = 0;
    u32 previous_col  = 0;
    lex_offset_to_line_col(
        lexer->source, first.offset, &previous_line, &previous_col);
    UNUSED(previous_col);

    for (u32 i = first_comment_index + 1; i < array_count(lexer->comments);
         ++i) {
        LexerComment comment = lexer->comments[i];
        if (comment.offset >= before_offset) {
            break;
        }

        u32 line = 0;
        u32 col  = 0;
        if (!lex_offset_to_line_col(
                lexer->source, comment.offset, &line, &col)) {
            break;
        }
        string raw_text = comment.text;
        if (line != previous_line + 1 || col != comment_column ||
            raw_text.count < 4 || raw_text.data[0] != ' ' ||
            raw_text.data[1] != ' ' || raw_text.data[2] != ' ' ||
            raw_text.data[3] != ' ') {
            break;
        }

        string text = format_trim_ascii(comment.text);
        if (text.count > 0) {
            if (sb.size > 0) {
                sb_append_char(&sb, ' ');
            }
            sb_append_string(&sb, text);
        }
        previous_line = line;
        if (out_next_comment_index) {
            *out_next_comment_index = i + 1;
        }
    }

    string merged =
        string_format(&temp_arena, STRINGP, STRINGV(sb_to_string(&sb)));
    arena_done(&arena);
    return merged;
}

internal usize format_trailing_comment_group_end_offset(const Lexer* lexer,
                                                        u32 first_comment_index,
                                                        usize before_offset)
{
    if (first_comment_index >= array_count(lexer->comments)) {
        return 0;
    }

    LexerComment first = lexer->comments[first_comment_index];
    usize        end   = first.end_offset;

    u32 first_line     = 0;
    u32 first_col      = 0;
    if (!lex_offset_to_line_col(
            lexer->source, first.offset, &first_line, &first_col)) {
        return end;
    }

    u32 previous_line = first_line;
    for (u32 i = first_comment_index + 1; i < array_count(lexer->comments);
         ++i) {
        LexerComment comment = lexer->comments[i];
        if (comment.offset >= before_offset) {
            break;
        }

        u32 line = 0;
        u32 col  = 0;
        if (!lex_offset_to_line_col(
                lexer->source, comment.offset, &line, &col)) {
            break;
        }
        string raw_text = comment.text;
        if (line != previous_line + 1 || col != first_col ||
            raw_text.count < 4 || raw_text.data[0] != ' ' ||
            raw_text.data[1] != ' ' || raw_text.data[2] != ' ' ||
            raw_text.data[3] != ' ') {
            break;
        }

        end           = comment.end_offset;
        previous_line = line;
    }

    return end;
}

internal bool format_emit_trailing_comment_for_node(StringBuilder* sb,
                                                    const Cst*     cst,
                                                    const Lexer*   lexer,
                                                    u32            node_index,
                                                    u32* io_comment_index)
{
    u32 end_token = format_node_end_token_index(cst, lexer, node_index);
    if (format_emit_trailing_comment_after_token(
            sb, lexer, io_comment_index, end_token)) {
        return true;
    }

    usize end_offset = lex_token_end_offset(lexer, &lexer->tokens[end_token]);
    return format_emit_trailing_comment_after_offset(
        sb, lexer, io_comment_index, end_offset);
}

internal void format_emit_block_contents(StringBuilder* sb,
                                         const Cst*     cst,
                                         const Lexer*   lexer,
                                         u32            block_node_index,
                                         u32            indent_level)
{
    const CstNode* block = &cst->nodes[block_node_index];
    ASSERT(block->kind == CK_Block, "Expected block node");
    FormatSyntaxContext syntax = format_syntax_context(cst, lexer);
    u32   previous_statement_index = U32_MAX;
    Arena align_arena              = {0};
    arena_init(&align_arena);

    u32   comment_index = 0;
    usize block_open_end =
        lex_token_end_offset(lexer, &lexer->tokens[block->token_index]);
    u32 block_close_token =
        format_node_end_token_index(cst, lexer, block_node_index);
    usize block_close_offset = lexer->tokens[block_close_token].offset;
    format_skip_block_comments_before_offset(
        lexer, &comment_index, block_open_end);

    for (u32 i = block->a; i < block->b; ++i) {
        if (!cst_node_is_block_statement(&cst->nodes[i]) ||
            format_node_is_owned_by_later_statement(
                cst, i, block->b, block_node_index)) {
            continue;
        }

        usize statement_start_offset =
            format_syntax_node_start_offset(&syntax, i);
        bool emitted_comments = false;
        if (comment_index < array_count(lexer->comments) &&
            lexer->comments[comment_index].offset < statement_start_offset) {
            if (previous_statement_index != U32_MAX) {
                usize previous_end_offset = format_syntax_node_end_offset(
                    &syntax, previous_statement_index);
                if (format_has_blank_line_between_offsets(
                        lexer->source,
                        previous_end_offset,
                        lexer->comments[comment_index].offset)) {
                    sb_append_char(sb, '\n');
                }
            }
            usize last_comment_end = 0;
            emitted_comments       = format_emit_block_comments_before_token(
                sb,
                lexer,
                &comment_index,
                cst->nodes[i].token_index,
                indent_level,
                &last_comment_end);
            if (emitted_comments &&
                format_has_blank_line_between_offsets(
                    lexer->source, last_comment_end, statement_start_offset)) {
                sb_append_char(sb, '\n');
            }
        }

        if (!emitted_comments && previous_statement_index != U32_MAX &&
            format_syntax_has_blank_line_between_nodes(
                &syntax, previous_statement_index, i)) {
            sb_append_char(sb, '\n');
        }
        if (!emitted_comments && previous_statement_index != U32_MAX &&
            format_statement_is_function_binding(cst, i) &&
            !format_statement_is_function_binding(cst,
                                                  previous_statement_index) &&
            !format_syntax_has_blank_line_between_nodes(
                &syntax, previous_statement_index, i)) {
            sb_append_char(sb, '\n');
        }

        if (cst->nodes[i].kind == CK_Use) {
            Array(u32) use_nodes = NULL;
            u32 last_use_index   = i;
            u32 cursor           = cst_block_statement_end_exclusive(cst, i);
            array_push(use_nodes, i);
            while (true) {
                u32 next_statement = format_next_block_statement(
                    cst, cursor, block->b, block_node_index);
                if (next_statement == U32_MAX ||
                    cst->nodes[next_statement].kind != CK_Use) {
                    break;
                }
                if (format_syntax_has_comment_between_nodes(
                        &syntax, last_use_index, next_statement)) {
                    break;
                }
                array_push(use_nodes, next_statement);
                last_use_index = next_statement;
                cursor         = next_statement + 1;
            }

            bool use_run_has_trailing_comment = false;
            for (u32 use_index = 0; use_index < array_count(use_nodes);
                 ++use_index) {
                if (format_node_has_trailing_comment(
                        cst, lexer, use_nodes[use_index])) {
                    use_run_has_trailing_comment = true;
                    break;
                }
            }
            if (use_run_has_trailing_comment) {
                for (u32 use_index = 0; use_index < array_count(use_nodes);
                     ++use_index) {
                    format_emit_block_statement(
                        sb, cst, lexer, use_nodes[use_index], indent_level);
                    format_emit_trailing_comment_for_node(
                        sb, cst, lexer, use_nodes[use_index], &comment_index);
                }
                previous_statement_index = last_use_index;
                i = cst_block_statement_end_exclusive(cst, last_use_index) - 1;
                array_free(use_nodes);
                continue;
            }

            if (!format_emit_sorted_use_run(
                    sb, cst, lexer, use_nodes, indent_level)) {
                array_free(use_nodes);
                break;
            }

            previous_statement_index = last_use_index;
            i = cst_block_statement_end_exclusive(cst, last_use_index) - 1;
            usize use_end_offset =
                format_syntax_node_end_offset(&syntax, last_use_index);
            format_skip_block_comments_before_offset(
                lexer, &comment_index, use_end_offset);
            array_free(use_nodes);
            continue;
        }

        FormatAlignedStatement first_aligned = {0};
        if (format_collect_aligned_statement(
                &align_arena, cst, lexer, i, false, &first_aligned)) {
            Array(FormatAlignedStatement) aligned = NULL;
            array_push(aligned, first_aligned);

            u32 last_aligned_index = i;
            u32 cursor             = i + 1;
            while (true) {
                u32 next_statement = format_next_block_statement(
                    cst, cursor, block->b, block_node_index);
                if (next_statement == U32_MAX) {
                    break;
                }

                bool has_blank_line =
                    format_syntax_has_blank_line_between_nodes(
                        &syntax, last_aligned_index, next_statement);
                bool has_comment = format_syntax_has_comment_between_nodes(
                    &syntax, last_aligned_index, next_statement);
                FormatAlignedStatement next_aligned = {0};
                if (!format_collect_aligned_statement(&align_arena,
                                                      cst,
                                                      lexer,
                                                      next_statement,
                                                      false,
                                                      &next_aligned)) {
                    break;
                }
                if (!format_aligned_statements_same_family(first_aligned,
                                                           next_aligned)) {
                    break;
                }
                if (has_blank_line) {
                    break;
                }
                if (has_comment) {
                    break;
                }

                array_push(aligned, next_aligned);
                last_aligned_index = next_statement;
                cursor             = next_statement + 1;
            }

            bool aligned_has_trailing_comment = false;
            for (u32 aligned_index = 0; aligned_index < array_count(aligned);
                 ++aligned_index) {
                if (format_node_has_trailing_comment(
                        cst, lexer, aligned[aligned_index].node_index)) {
                    aligned_has_trailing_comment = true;
                    break;
                }
            }

            if (!aligned_has_trailing_comment &&
                (array_count(aligned) > 1 ||
                 !aligned[0].uses_standard_single_line)) {
                format_emit_aligned_statement_group(
                    sb, aligned, (u32)array_count(aligned), indent_level);
                previous_statement_index = last_aligned_index;
                i                        = last_aligned_index;
                usize aligned_end_offset =
                    format_syntax_node_end_offset(&syntax, last_aligned_index);
                format_skip_block_comments_before_offset(
                    lexer, &comment_index, aligned_end_offset);

                u32 next_statement = format_next_block_statement(
                    cst, i + 1, block->b, block_node_index);
                if (!first_aligned.is_assignment && array_count(aligned) > 1 &&
                    next_statement != U32_MAX &&
                    !format_syntax_has_blank_line_between_nodes(
                        &syntax, last_aligned_index, next_statement)) {
                    FormatAlignedStatement ignored = {0};
                    if (!format_collect_aligned_statement(&align_arena,
                                                          cst,
                                                          lexer,
                                                          next_statement,
                                                          false,
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
        format_emit_trailing_comment_for_node(
            sb, cst, lexer, i, &comment_index);
        if (format_statement_is_function_binding(cst, i)) {
            u32 next_statement = format_next_block_statement(
                cst, i + 1, block->b, block_node_index);
            if (next_statement != U32_MAX &&
                !format_syntax_has_blank_line_between_nodes(
                    &syntax, i, next_statement)) {
                sb_append_char(sb, '\n');
            }
        }
        FormatAlignedStatement current_aligned = {0};
        if (format_collect_aligned_statement(
                &align_arena, cst, lexer, i, false, &current_aligned) &&
            !current_aligned.is_assignment) {
            u32 next_statement = format_next_block_statement(
                cst, i + 1, block->b, block_node_index);
            if (next_statement != U32_MAX &&
                !format_syntax_has_blank_line_between_nodes(
                    &syntax, i, next_statement)) {
                FormatAlignedStatement next_aligned = {0};
                if (format_collect_aligned_statement(&align_arena,
                                                     cst,
                                                     lexer,
                                                     next_statement,
                                                     false,
                                                     &next_aligned) &&
                    !format_aligned_statements_same_family(current_aligned,
                                                           next_aligned)) {
                    sb_append_char(sb, '\n');
                }
            }
        }
        previous_statement_index  = i;
        usize statement_end_offset = format_syntax_node_end_offset(&syntax, i);
        format_skip_block_comments_before_offset(
            lexer, &comment_index, statement_end_offset);
        i = cst_block_statement_end_exclusive(cst, i) - 1;
    }

    if (comment_index < array_count(lexer->comments) &&
        lexer->comments[comment_index].offset < block_close_offset &&
        previous_statement_index != U32_MAX) {
        usize previous_end_offset = format_syntax_node_end_offset(
            &syntax, previous_statement_index);
        if (format_has_blank_line_between_offsets(
                lexer->source,
                previous_end_offset,
                lexer->comments[comment_index].offset)) {
            sb_append_char(sb, '\n');
        }
    }
    format_emit_block_comments_before_offset(
        sb, lexer, &comment_index, block_close_offset, indent_level, NULL);

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
            cst->nodes[item->b].kind == CK_ZeroInit ||
            cst->nodes[item->b].kind == CK_Undefined) {
            sb_append_cstr(sb, ": ");
            format_emit_variable_payload(sb, cst, lexer, item->b);
        } else {
            sb_append_cstr(sb, " := ");
            format_emit_expr(sb, cst, lexer, item->b, 0);
        }
        return;
    }
    if (item->kind == CK_Bind) {
        if (item->flags & CNF_Public) {
            sb_append_cstr(sb, "pub ");
        }
        sb_append_string(sb, lex_symbol(lexer, cst_get_symbol(item)));
        sb_append_cstr(
            sb, cst->nodes[item->b].kind == CK_AnnotatedValue ? " : " : " :: ");
        if (cst->nodes[item->b].kind == CK_TypePlex) {
            format_emit_type_plex_multiline(sb, cst, lexer, item->b, 0);
        } else if (cst->nodes[item->b].kind == CK_TypeEnum) {
            format_emit_type_enum_multiline(sb, cst, lexer, item->b, 0);
        } else {
            format_emit_value_with_indent(sb, cst, lexer, item->b, 0);
        }
        return;
    }
    if (item->kind == CK_Assign) {
        format_emit_expr(sb, cst, lexer, item->a, 0);
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
    if (stmt->kind != CK_TopOn && stmt->kind != CK_Test) {
        format_emit_indent(sb, indent_level);
    }

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
        if (for_info->iterable_node_index != U32_MAX) {
            sb_append_char(sb, ' ');
            if (for_info->index_symbol != U32_MAX) {
                sb_append_string(sb, lex_symbol(lexer, for_info->index_symbol));
                sb_append_cstr(sb, ", ");
            }
            sb_append_string(sb, lex_symbol(lexer, for_info->item_symbol));
            sb_append_cstr(sb, " in ");
            format_emit_expr(sb, cst, lexer, for_info->iterable_node_index, 0);
        } else if (for_info->mode == CFM_CStyle) {
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

    if (stmt->kind == CK_Defer) {
        const CstNode* deferred = &cst->nodes[stmt->a];
        sb_append_cstr(sb, "defer");
        if (deferred->kind == CK_Block) {
            sb_append_cstr(sb, " {\n");
            format_emit_block_contents(
                sb, cst, lexer, stmt->a, indent_level + 1);
            format_emit_indent(sb, indent_level);
            sb_append_cstr(sb, "}\n");
        } else {
            sb_append_char(sb, ' ');
            if (deferred->kind == CK_Statement) {
                format_emit_expr_with_indent(
                    sb, cst, lexer, deferred->a, 0, indent_level);
                sb_append_char(sb, '\n');
            } else if (deferred->kind == CK_Return) {
                sb_append_cstr(sb, "return");
                if (deferred->a != U32_MAX) {
                    sb_append_char(sb, ' ');
                    format_emit_expr_with_indent(
                        sb, cst, lexer, deferred->a, 0, indent_level);
                }
                sb_append_char(sb, '\n');
            } else if (deferred->kind == CK_Break ||
                       deferred->kind == CK_Continue) {
                sb_append_cstr(
                    sb, deferred->kind == CK_Break ? "break" : "continue");
                if (deferred->b != U32_MAX) {
                    sb_append_cstr(sb, " $");
                    sb_append_string(sb, lex_symbol(lexer, deferred->b));
                }
                if (deferred->a != U32_MAX) {
                    sb_append_char(sb, ' ');
                    format_emit_expr_with_indent(
                        sb, cst, lexer, deferred->a, 0, indent_level);
                }
                sb_append_char(sb, '\n');
            } else {
                format_emit_expr_with_indent(
                    sb, cst, lexer, stmt->a, 0, indent_level);
                sb_append_char(sb, '\n');
            }
        }
        return;
    }

    if (stmt->kind == CK_Assert) {
        Arena temp_arena = {0};
        arena_init(&temp_arena);
        StringBuilder single_line = {0};
        sb_init(&single_line, &temp_arena);
        sb_append_cstr(&single_line, "assert ");
        format_emit_expr_with_indent(
            &single_line, cst, lexer, stmt->a, 0, indent_level);
        if (stmt->b != U32_MAX) {
            sb_append_cstr(&single_line, ", ");
            format_emit_expr_with_indent(
                &single_line, cst, lexer, stmt->b, 0, indent_level);
        }

        string rendered     = sb_to_string(&single_line);
        usize  start_column = (usize)indent_level * 4;
        if (!format_string_has_newline(rendered) &&
            start_column + rendered.count <= FORMAT_WRAP_WIDTH) {
            sb_append_string(sb, rendered);
            sb_append_char(sb, '\n');
            arena_done(&temp_arena);
            return;
        }

        sb_append_cstr(sb, "assert ");
        format_emit_expr_with_indent(sb, cst, lexer, stmt->a, 0, indent_level);
        if (stmt->b != U32_MAX) {
            sb_append_cstr(sb, ",\n");
            format_emit_indent(sb, indent_level + 1);
            format_emit_expr_with_indent(
                sb, cst, lexer, stmt->b, 0, indent_level);
        }
        sb_append_char(sb, '\n');
        arena_done(&temp_arena);
        return;
    }

    if (stmt->kind == CK_Break || stmt->kind == CK_BreakOn ||
        stmt->kind == CK_Continue) {
        if (stmt->kind == CK_BreakOn) {
            format_emit_expr_with_indent(
                sb, cst, lexer, node_index, 0, indent_level);
            sb_append_char(sb, '\n');
            return;
        }
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
            cst->nodes[stmt->b].kind == CK_ZeroInit ||
            cst->nodes[stmt->b].kind == CK_Undefined) {
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
        if (stmt->flags & CNF_Public) {
            sb_append_cstr(sb, "pub ");
        }
        sb_append_string(sb, lex_symbol(lexer, cst_get_symbol(stmt)));
        sb_append_cstr(
            sb, cst->nodes[stmt->b].kind == CK_AnnotatedValue ? " : " : " :: ");
        if (cst->nodes[stmt->b].kind == CK_TypePlex) {
            format_emit_type_plex_multiline(
                sb, cst, lexer, stmt->b, indent_level);
        } else if (cst->nodes[stmt->b].kind == CK_TypeEnum) {
            format_emit_type_enum_multiline(
                sb, cst, lexer, stmt->b, indent_level);
        } else {
            format_emit_value_with_indent(
                sb, cst, lexer, stmt->b, indent_level);
        }
        sb_append_char(sb, '\n');
        return;
    }

    if (stmt->kind == CK_DestructureBind) {
        format_emit_pattern(sb, cst, lexer, stmt->a, false);
        sb_append_cstr(
            sb, cst->nodes[stmt->b].kind == CK_AnnotatedValue ? " : " : " :: ");
        format_emit_value(sb, cst, lexer, stmt->b);
        sb_append_char(sb, '\n');
        return;
    }

    if (stmt->kind == CK_DestructureVariable) {
        format_emit_pattern(sb, cst, lexer, stmt->a, false);
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
        format_emit_pattern(sb, cst, lexer, stmt->a, false);
        sb_append_cstr(sb, " = ");
        format_emit_expr_with_indent(sb, cst, lexer, stmt->b, 0, indent_level);
        sb_append_char(sb, '\n');
        return;
    }

    if (stmt->kind == CK_Assign) {
        format_emit_expr_with_indent(sb, cst, lexer, stmt->a, 0, indent_level);
        sb_append_char(sb, ' ');
        sb_append_string(sb, format_assignment_operator(lexer, stmt));
        sb_append_char(sb, ' ');
        format_emit_expr_with_indent(sb, cst, lexer, stmt->b, 0, indent_level);
        sb_append_char(sb, '\n');
        return;
    }

    if (stmt->kind == CK_Use) {
        if (stmt->flags & CNF_Public) {
            sb_append_cstr(sb, "pub ");
        }
        sb_append_cstr(sb, "use ");
        if (cst->nodes[stmt->a].kind == CK_ModRef) {
            format_emit_module_path(sb, cst, lexer, cst->nodes[stmt->a].a);
        } else {
            format_emit_expr_with_indent(
                sb, cst, lexer, stmt->a, 0, indent_level);
        }
        sb_append_char(sb, '\n');
        return;
    }

    if (stmt->kind == CK_FfiDef) {
        if (stmt->flags & CNF_Public) {
            sb_append_cstr(sb, "pub ");
        }
        format_emit_ffi_def(sb, cst, lexer, stmt->a);
        sb_append_char(sb, '\n');
        return;
    }

    if (stmt->kind == CK_FfiBlock) {
        if (stmt->flags & CNF_Public) {
            sb_append_cstr(sb, "pub ");
        }
        format_emit_ffi_block(sb, cst, lexer, stmt->a, indent_level);
        sb_append_char(sb, '\n');
        return;
    }

    if (stmt->kind == CK_TopOn) {
        format_emit_top_on(sb, cst, lexer, stmt->a, indent_level);
        sb_append_char(sb, '\n');
        return;
    }

    if (stmt->kind == CK_Impl) {
        format_emit_impl(sb, cst, lexer, stmt->a, indent_level);
        sb_append_char(sb, '\n');
        return;
    }

    if (stmt->kind == CK_Test) {
        format_emit_test(sb, cst, lexer, node_index, indent_level);
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

internal bool format_use_sort_key(StringBuilder* sb,
                                  const Cst*     cst,
                                  const Lexer*   lexer,
                                  u32            node_index)
{
    const CstNode* node = &cst->nodes[node_index];
    if (node->kind != CK_Use) {
        return false;
    }

    if (cst->nodes[node->a].kind == CK_ModRef) {
        format_emit_module_path(sb, cst, lexer, cst->nodes[node->a].a);
        return true;
    }

    format_emit_expr(sb, cst, lexer, node->a, 0);
    return true;
}

internal void format_sort_use_items(Array(FormatUseItem) items)
{
    for (usize i = 1; i < array_count(items); ++i) {
        FormatUseItem item = items[i];
        usize         j    = i;
        while (j > 0 && format_string_compare(items[j - 1].key, item.key) > 0) {
            items[j] = items[j - 1];
            j--;
        }
        items[j] = item;
    }
}

internal void format_emit_use_statement_from_key(StringBuilder* sb,
                                                 FormatUseItem  item,
                                                 u32            indent_level)
{
    format_emit_indent(sb, indent_level);
    if (item.is_public) {
        sb_append_cstr(sb, "pub ");
    }
    sb_append_cstr(sb, "use ");
    sb_append_string(sb, item.key);
    sb_append_char(sb, '\n');
}

internal bool format_emit_sorted_use_run(StringBuilder* sb,
                                         const Cst*     cst,
                                         const Lexer*   lexer,
                                         Array(u32) use_nodes,
                                         u32 indent_level)
{
    Arena arena = {0};
    arena_init(&arena);

    Array(FormatUseItem) private_items = NULL;
    Array(FormatUseItem) public_items  = NULL;
    for (usize i = 0; i < array_count(use_nodes); ++i) {
        u32            node_index = use_nodes[i];
        const CstNode* node       = &cst->nodes[node_index];

        StringBuilder key_sb      = {0};
        sb_init(&key_sb, &arena);
        if (!format_use_sort_key(&key_sb, cst, lexer, node_index)) {
            arena_done(&arena);
            return false;
        }

        FormatUseItem item = {
            .node_index = node_index,
            .key        = sb_to_string(&key_sb),
            .is_public  = (node->flags & CNF_Public) != 0,
        };
        if (item.is_public) {
            array_push(public_items, item);
        } else {
            array_push(private_items, item);
        }
    }

    format_sort_use_items(private_items);
    format_sort_use_items(public_items);

    for (usize i = 0; i < array_count(private_items); ++i) {
        format_emit_use_statement_from_key(sb, private_items[i], indent_level);
    }
    if (array_count(private_items) > 0 && array_count(public_items) > 0) {
        sb_append_char(sb, '\n');
    }
    for (usize i = 0; i < array_count(public_items); ++i) {
        format_emit_use_statement_from_key(sb, public_items[i], indent_level);
    }

    array_free(private_items);
    array_free(public_items);
    arena_done(&arena);
    return true;
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

    if (node->kind == CK_Undefined) {
        format_emit_expr(sb, cst, lexer, node->a, 0);
        sb_append_cstr(sb, " = undefined");
        return;
    }

    switch (node->kind) {
    case CK_FnExpr:
        format_emit_fn_signature(sb, cst, lexer, node->a, false);
        if (format_node_is_block_form_on(cst, node->b)) {
            sb_append_cstr(sb, " =>\n");
            format_emit_indent(sb, g_format_value_indent_level + 1);
            format_emit_on_block_multiline(
                sb, cst, lexer, node->b, g_format_value_indent_level + 1);
        } else {
            sb_append_cstr(sb, " => ");
            format_emit_expr(sb, cst, lexer, node->b, 0);
        }
        break;
    case CK_FnBlock:
        format_emit_fn_signature(sb, cst, lexer, node->a, true);
        sb_append_cstr(sb, " {\n");
        format_emit_block_contents(
            sb, cst, lexer, node->b, g_format_value_indent_level + 1);
        format_emit_indent(sb, g_format_value_indent_level);
        sb_append_cstr(sb, "}");
        break;
    case CK_FfiDef:
        format_emit_ffi_def(sb, cst, lexer, node->a);
        break;
    case CK_FfiBlock:
        format_emit_ffi_block(
            sb, cst, lexer, node->a, g_format_value_indent_level);
        break;
    case CK_ModRef:
        format_emit_mod_ref(sb, cst, lexer, node->a);
        break;
    default:
        format_emit_expr_with_indent(
            sb, cst, lexer, node_index, 0, g_format_value_indent_level);
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

    if (node->kind == CK_Undefined) {
        format_emit_expr(sb, cst, lexer, node->a, 0);
        sb_append_cstr(sb, " = undefined");
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
    if (!lex_with_config(
            source, &(LexerConfig){.mode = LEXER_MODE_FORMAT}, &lexer)) {
        return false;
    }

    FormatTrivia trivia = {0};
    format_trivia_build(&lexer, &trivia);
    format_trivia_validate(&lexer, &trivia);
    const FormatTrivia* previous_trivia = g_format_trivia;
    g_format_trivia                     = &trivia;

    Cst cst                             = {0};
    if (!cst_parse(&lexer, &cst) || array_count(cst.bindings) == 0) {
        g_format_trivia = previous_trivia;
        format_trivia_done(&trivia);
        cst_done(&cst);
        lex_done(&lexer);
        return false;
    }

    Arena align_arena = {0};
    arena_init(&align_arena);

    FormatSyntaxContext syntax = format_syntax_context(&cst, &lexer);
    bool first_binding          = true;
    u32  previous_binding_index = U32_MAX;
    u32  comment_index          = 0;
    for (u32 i = 0; i < array_count(cst.bindings); ++i) {
        u32            node_index = cst.bindings[i];
        const CstNode* node       = &cst.nodes[node_index];

        if (!first_binding) {
            const CstNode* previous = &cst.nodes[previous_binding_index];
            if (previous->kind != CK_Use || node->kind != CK_Use) {
                sb_append_char(sb, '\n');
            }
        }

        if (node->kind == CK_Use) {
            Array(u32) use_nodes = NULL;
            array_push(use_nodes, node_index);

            u32 last_use_binding = i;
            for (u32 cursor = i + 1; cursor < array_count(cst.bindings);
                 ++cursor) {
                u32 next_index = cst.bindings[cursor];
                if (cst.nodes[next_index].kind != CK_Use) {
                    break;
                }
                if (format_syntax_has_comment_between_nodes(
                        &syntax, cst.bindings[last_use_binding], next_index)) {
                    break;
                }
                array_push(use_nodes, next_index);
                last_use_binding = cursor;
            }

            bool use_run_has_trailing_comment = false;
            for (u32 use_index = 0; use_index < array_count(use_nodes);
                 ++use_index) {
                if (format_node_has_trailing_comment(
                        &cst, &lexer, use_nodes[use_index])) {
                    use_run_has_trailing_comment = true;
                    break;
                }
            }
            if (use_run_has_trailing_comment) {
                for (u32 use_index = 0; use_index < array_count(use_nodes);
                     ++use_index) {
                    format_emit_block_statement(
                        sb, &cst, &lexer, use_nodes[use_index], 0);
                    format_emit_trailing_comment_for_node(
                        sb, &cst, &lexer, use_nodes[use_index], &comment_index);
                }
                i                      = last_use_binding;
                first_binding          = false;
                previous_binding_index = cst.bindings[last_use_binding];
                array_free(use_nodes);
                continue;
            }

            if (!format_emit_sorted_use_run(sb, &cst, &lexer, use_nodes, 0)) {
                array_free(use_nodes);
                break;
            }

            i                      = last_use_binding;
            first_binding          = false;
            previous_binding_index = cst.bindings[last_use_binding];
            format_emit_trailing_comment_for_node(
                sb, &cst, &lexer, previous_binding_index, &comment_index);
            array_free(use_nodes);
            continue;
        }

        if (node->kind == CK_FfiDef) {
            if (node->flags & CNF_Public) {
                sb_append_cstr(sb, "pub ");
            }
            format_emit_ffi_def(sb, &cst, &lexer, node->a);
            sb_append_char(sb, '\n');
            format_emit_trailing_comment_for_node(
                sb, &cst, &lexer, node_index, &comment_index);
            first_binding          = false;
            previous_binding_index = node_index;
            continue;
        }

        if (node->kind == CK_FfiBlock) {
            if (node->flags & CNF_Public) {
                sb_append_cstr(sb, "pub ");
            }
            format_emit_ffi_block(sb, &cst, &lexer, node->a, 0);
            sb_append_char(sb, '\n');
            format_emit_trailing_comment_for_node(
                sb, &cst, &lexer, node_index, &comment_index);
            first_binding          = false;
            previous_binding_index = node_index;
            continue;
        }

        if (node->kind == CK_TopOn) {
            format_emit_top_on(sb, &cst, &lexer, node->a, 0);
            sb_append_char(sb, '\n');
            format_emit_trailing_comment_for_node(
                sb, &cst, &lexer, node_index, &comment_index);
            first_binding          = false;
            previous_binding_index = node_index;
            continue;
        }

        if (node->kind == CK_Impl) {
            format_emit_impl(sb, &cst, &lexer, node->a, 0);
            sb_append_char(sb, '\n');
            format_emit_trailing_comment_for_node(
                sb, &cst, &lexer, node_index, &comment_index);
            first_binding          = false;
            previous_binding_index = node_index;
            continue;
        }

        if (node->kind == CK_Test) {
            format_emit_test(sb, &cst, &lexer, node_index, 0);
            sb_append_char(sb, '\n');
            format_emit_trailing_comment_for_node(
                sb, &cst, &lexer, node_index, &comment_index);
            first_binding          = false;
            previous_binding_index = node_index;
            continue;
        }

        FormatAlignedStatement first_aligned = {0};
        if (format_collect_aligned_statement(
                &align_arena, &cst, &lexer, node_index, true, &first_aligned)) {
            Array(FormatAlignedStatement) aligned = NULL;
            array_push(aligned, first_aligned);

            u32 last_aligned_binding = i;
            for (u32 cursor = i + 1; cursor < array_count(cst.bindings);
                 ++cursor) {
                u32  next_index     = cst.bindings[cursor];
                bool has_blank_line =
                    format_syntax_has_blank_line_between_nodes(
                        &syntax,
                        cst.bindings[last_aligned_binding],
                        next_index);
                bool has_comment = format_syntax_has_comment_between_nodes(
                    &syntax, cst.bindings[last_aligned_binding], next_index);
                FormatAlignedStatement next_aligned = {0};
                if (!format_collect_aligned_statement(&align_arena,
                                                      &cst,
                                                      &lexer,
                                                      next_index,
                                                      true,
                                                      &next_aligned) ||
                    !format_aligned_statements_same_family(first_aligned,
                                                           next_aligned)) {
                    break;
                }
                if (has_blank_line) {
                    break;
                }
                if (has_comment) {
                    break;
                }

                array_push(aligned, next_aligned);
                last_aligned_binding = cursor;
            }

            bool aligned_has_trailing_comment = false;
            for (u32 aligned_index = 0; aligned_index < array_count(aligned);
                 ++aligned_index) {
                if (format_node_has_trailing_comment(
                        &cst, &lexer, aligned[aligned_index].node_index)) {
                    aligned_has_trailing_comment = true;
                    break;
                }
            }

            if (!aligned_has_trailing_comment && array_count(aligned) > 1) {
                format_emit_aligned_statement_group(
                    sb, aligned, (u32)array_count(aligned), 0);
                i                      = last_aligned_binding;
                first_binding          = false;
                previous_binding_index = cst.bindings[last_aligned_binding];
                format_emit_trailing_comment_for_node(
                    sb, &cst, &lexer, previous_binding_index, &comment_index);
                array_free(aligned);
                continue;
            }

            array_free(aligned);
        }

        if (node->kind == CK_Bind) {
            if (node->flags & CNF_Public) {
                sb_append_cstr(sb, "pub ");
            }
            sb_append_string(sb, lex_symbol(&lexer, cst_get_symbol(node)));
            sb_append_cstr(
                sb,
                cst.nodes[node->b].kind == CK_AnnotatedValue ? " : " : " :: ");
            if (cst.nodes[node->b].kind == CK_TypePlex) {
                format_emit_type_plex_multiline(sb, &cst, &lexer, node->b, 0);
            } else if (cst.nodes[node->b].kind == CK_TypeEnum) {
                format_emit_type_enum_multiline(sb, &cst, &lexer, node->b, 0);
            } else {
                format_emit_value(sb, &cst, &lexer, node->b);
            }
        } else {
            sb_append_string(sb, lex_symbol(&lexer, cst_get_symbol(node)));
            if (cst.nodes[node->b].kind == CK_AnnotatedValue ||
                cst.nodes[node->b].kind == CK_ZeroInit ||
                cst.nodes[node->b].kind == CK_Undefined) {
                sb_append_cstr(sb, ": ");
                format_emit_variable_payload(sb, &cst, &lexer, node->b);
            } else {
                sb_append_cstr(sb, " := ");
                format_emit_expr(sb, &cst, &lexer, node->b, 0);
            }
        }
        sb_append_char(sb, '\n');
        format_emit_trailing_comment_for_node(
            sb, &cst, &lexer, node_index, &comment_index);
        first_binding          = false;
        previous_binding_index = node_index;
    }

    arena_done(&align_arena);
    g_format_trivia = previous_trivia;
    format_trivia_done(&trivia);
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
// Advance delimiter nesting for formatter block detection.

internal void format_advance_delimiter_depth(const Lexer* lexer,
                                             u32*         io_token_index,
                                             usize        end_offset,
                                             i32*         io_depth)
{
    while (*io_token_index < array_count(lexer->tokens)) {
        Token token = lexer->tokens[*io_token_index];
        if (token.offset >= end_offset) {
            break;
        }

        switch (token.kind) {
        case TK_LParen:
        case TK_LBracket:
        case TK_LBrace:
            (*io_depth)++;
            break;
        case TK_RParen:
        case TK_RBracket:
        case TK_RBrace:
            if (*io_depth > 0) {
                (*io_depth)--;
            }
            break;
        default:
            break;
        }

        (*io_token_index)++;
    }
}

typedef struct {
    StringBuilder* sb;
    const Lexer*   lexer;
    u32            indent_level;
    bool           at_line_start;
} FormatTokenState;

internal string format_token_text(const Lexer* lexer, u32 token_index)
{
    if (token_index >= array_count(lexer->tokens)) {
        return (string){0};
    }

    const Token* token = &lexer->tokens[token_index];
    usize        start = token->offset;
    usize        end   = lex_token_end_offset(lexer, token);
    if (start > lexer->source.source.count) {
        start = lexer->source.source.count;
    }
    if (end > lexer->source.source.count) {
        end = lexer->source.source.count;
    }
    if (end < start) {
        end = start;
    }
    return string_from(lexer->source.source.data + start, end - start);
}

internal void format_token_state_indent(FormatTokenState* state)
{
    if (!state->at_line_start) {
        return;
    }

    format_emit_indent(state->sb, state->indent_level);
    state->at_line_start = false;
}

internal void format_token_state_newline(FormatTokenState* state)
{
    if (state->sb->size > 0 && state->sb->data[state->sb->size - 1] != '\n') {
        sb_append_char(state->sb, '\n');
    }
    state->at_line_start = true;
}

internal void format_token_state_blank_line(FormatTokenState* state)
{
    format_token_state_newline(state);
    if (state->sb->size > 0 && state->sb->data[state->sb->size - 1] == '\n') {
        sb_append_char(state->sb, '\n');
    }
    state->at_line_start = true;
}

internal bool format_token_kind_is_binary_operator(TokenKind kind)
{
    switch (kind) {
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
    case TK_Equal:
    case TK_EqualEqual:
    case TK_BangEqual:
    case TK_Amp:
    case TK_AmpEqual:
    case TK_AmpAmp:
    case TK_AmpAmpEqual:
    case TK_Pipe:
    case TK_PipeEqual:
    case TK_PipePipe:
    case TK_PipePipeEqual:
    case TK_CaretEqual:
    case TK_Less:
    case TK_LessEqual:
    case TK_ShiftLeft:
    case TK_ShiftLeftEqual:
    case TK_Greater:
    case TK_GreaterEqual:
    case TK_ShiftRight:
    case TK_ShiftRightEqual:
    case TK_FatArrow:
    case TK_ThinArrow:
    case TK_Range:
    case TK_RangeInclusive:
    case TK_as:
    case TK_in:
        return true;
    default:
        return false;
    }
}

internal bool format_token_needs_space_between(TokenKind previous,
                                               TokenKind current,
                                               TokenKind next)
{
    if (previous == TK_EOF) {
        return false;
    }

    if (current == TK_RParen || current == TK_RBracket || current == TK_Comma ||
        current == TK_Dot || current == TK_Caret) {
        return false;
    }

    if (previous == TK_Colon && (current == TK_Colon || current == TK_Equal)) {
        return false;
    }

    if (current == TK_Colon && (next == TK_Colon || next == TK_Equal)) {
        return true;
    }

    if (current == TK_Colon) {
        return false;
    }

    if (previous == TK_LParen || previous == TK_LBracket ||
        previous == TK_Dot || previous == TK_Dollar || previous == TK_At ||
        previous == TK_Hash || previous == TK_Caret) {
        return false;
    }

    if (previous == TK_fn && current == TK_LParen) {
        return true;
    }

    if (previous == TK_Equal && current == TK_LBracket) {
        return true;
    }

    if (current == TK_LParen || current == TK_LBracket) {
        return false;
    }

    if (current == TK_LBrace) {
        return previous != TK_EOF;
    }

    if (format_token_kind_is_binary_operator(previous) ||
        format_token_kind_is_binary_operator(current)) {
        return true;
    }

    switch (previous) {
    case TK_Comma:
    case TK_Colon:
    case TK_fn:
    case TK_for:
    case TK_on:
    case TK_else:
    case TK_defer:
    case TK_assert:
    case TK_break:
    case TK_continue:
    case TK_return:
    case TK_plex:
    case TK_union:
    case TK_enum:
    case TK_ffi:
    case TK_use:
    case TK_pub:
    case TK_impl:
    case TK_with:
        return true;
    default:
        break;
    }

    switch (current) {
    case TK_fn:
    case TK_for:
    case TK_on:
    case TK_else:
    case TK_defer:
    case TK_assert:
    case TK_break:
    case TK_continue:
    case TK_return:
    case TK_plex:
    case TK_union:
    case TK_enum:
    case TK_ffi:
    case TK_use:
    case TK_pub:
    case TK_impl:
    case TK_with:
        return true;
    default:
        break;
    }

    return false;
}

internal bool format_emit_token_comments_before(FormatTokenState* state,
                                                const FormatTrivia* trivia,
                                                u32* io_comment_index,
                                                u32  token_index)
{
    u32 first_comment_index = U32_MAX;
    u32 comment_count       = 0;
    if (!format_trivia_comments_before_token(
            trivia, token_index, &first_comment_index, &comment_count)) {
        return false;
    }

    if (*io_comment_index < first_comment_index) {
        *io_comment_index = first_comment_index;
    }

    u32 end_comment_index = first_comment_index + comment_count;
    while (*io_comment_index < end_comment_index &&
           *io_comment_index < array_count(state->lexer->comments)) {
        format_emit_line_comment(
            state->sb,
            state->indent_level,
            state->lexer->comments[*io_comment_index].text);
        state->at_line_start = true;
        (*io_comment_index)++;
    }
    return true;
}

internal bool format_emit_token_stream_block(StringBuilder* sb,
                                             NerdSource     source)
{
    Lexer lexer = {0};
    if (!lex_with_config(
            source, &(LexerConfig){.mode = LEXER_MODE_FORMAT}, &lexer)) {
        return false;
    }

    FormatTrivia trivia = {0};
    format_trivia_build(&lexer, &trivia);
    format_trivia_validate(&lexer, &trivia);

    FormatTokenState state = {
        .sb            = sb,
        .lexer         = &lexer,
        .indent_level  = 0,
        .at_line_start = true,
    };

    u32       comment_index = 0;
    u32       multiline_bracket_depth = 0;
    TokenKind previous_kind = TK_EOF;
    for (u32 i = 0; i < array_count(lexer.tokens); ++i) {
        TokenKind kind = lexer.tokens[i].kind;
        TokenKind next_kind =
            i + 1 < array_count(lexer.tokens) ? lexer.tokens[i + 1].kind
                                              : TK_EOF;
        u16 next_newlines =
            i + 1 < array_count(lexer.tokens)
                ? trivia.newlines_before_token[i + 1]
                : trivia.newlines_before_token[array_count(lexer.tokens)];
        bool next_has_comments = i + 1 < array_count(lexer.tokens) &&
                                 format_trivia_comments_before_token(
                                     &trivia, i + 1, NULL, NULL);

        u16 newlines_before = trivia.newlines_before_token[i];
        bool has_comments_before = format_trivia_comments_before_token(
            &trivia, i, NULL, NULL);
        if (newlines_before > 1 && sb->size > 0 && !has_comments_before) {
            format_token_state_blank_line(&state);
        } else if (newlines_before > 0 && !state.at_line_start) {
            format_token_state_newline(&state);
        }

        format_emit_token_comments_before(&state, &trivia, &comment_index, i);

        if (kind == TK_RBrace ||
            (kind == TK_RBracket && multiline_bracket_depth > 0)) {
            if (!state.at_line_start) {
                format_token_state_newline(&state);
            }
            if (state.indent_level > 0) {
                state.indent_level--;
            }
            if (kind == TK_RBracket) {
                multiline_bracket_depth--;
            }
        }

        format_token_state_indent(&state);

        if (format_token_needs_space_between(previous_kind, kind, next_kind) &&
            sb->size > 0 && sb->data[sb->size - 1] != ' ' &&
            sb->data[sb->size - 1] != '\n') {
            sb_append_char(sb, ' ');
        }

        sb_append_string(sb, format_token_text(&lexer, i));

        if (kind == TK_LBrace) {
            state.indent_level++;
            format_token_state_newline(&state);
        } else if (kind == TK_LBracket &&
                   (next_newlines > 0 || next_has_comments)) {
            state.indent_level++;
            multiline_bracket_depth++;
            format_token_state_newline(&state);
        } else if (kind == TK_RBrace || kind == TK_Semicolon) {
            u32 trailing_comment = U32_MAX;
            if (format_trivia_trailing_comment_after_token(
                    &trivia, i, &trailing_comment)) {
                format_emit_trailing_comment_by_index(
                    sb, &lexer, &comment_index, trailing_comment);
                state.at_line_start = true;
            } else {
                format_token_state_newline(&state);
            }
        } else if (kind == TK_RBracket && multiline_bracket_depth == 0 &&
                   next_newlines > 0) {
            format_token_state_newline(&state);
        } else if (kind == TK_Comma) {
            if (next_newlines == 0 && !next_has_comments) {
                sb_append_char(sb, ' ');
            }
        } else {
            u32 trailing_comment = U32_MAX;
            if (format_trivia_trailing_comment_after_token(
                    &trivia, i, &trailing_comment)) {
                format_emit_trailing_comment_by_index(
                    sb, &lexer, &comment_index, trailing_comment);
                state.at_line_start = true;
            }
        }

        previous_kind = kind;
    }

    while (comment_index < array_count(lexer.comments)) {
        format_emit_line_comment(
            sb, state.indent_level, lexer.comments[comment_index].text);
        state.at_line_start = true;
        comment_index++;
    }

    if (sb->size > 0 && sb->data[sb->size - 1] != '\n') {
        sb_append_char(sb, '\n');
    }

    format_trivia_done(&trivia);
    lex_done(&lexer);
    return true;
}

//------------------------------------------------------------------------------
// Format source text, reflowing comments and normalising code blocks.

bool format_source(NerdSource source, Arena* arena, string* out_text)
{
    MemoryStats memory_before = compiler_memory_profile_begin();
    Lexer lexer = {0};
    if (!lex_with_config(
            source, &(LexerConfig){.mode = LEXER_MODE_FORMAT}, &lexer)) {
        compiler_memory_profile_end(COMPILER_STAGE_FORMATTER,
                                    COMPILER_PHASE_FORMAT_SOURCE,
                                    memory_before);
        return false;
    }

    FormatTrivia trivia = {0};
    format_trivia_build(&lexer, &trivia);
    format_trivia_validate(&lexer, &trivia);

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
            usize block_start     = offset;
            usize block_end       = has_newline ? line_end + 1 : line_end;
            u32   token_index     = 0;
            i32   delimiter_depth = 0;

            while (token_index < array_count(lexer.tokens) &&
                   lexer.tokens[token_index].offset < block_start) {
                token_index++;
            }
            format_advance_delimiter_depth(
                &lexer, &token_index, block_end, &delimiter_depth);

            while (block_end < text.count) {
                usize next_line_end = block_end;
                while (next_line_end < text.count &&
                       text.data[next_line_end] != '\n') {
                    next_line_end++;
                }

                bool   next_has_newline = next_line_end < text.count &&
                                          text.data[next_line_end] == '\n';
                string next_line   = string_from(text.data + block_end,
                                                 next_line_end - block_end);
                string next_indent = {0};
                string next_body   = {0};

                if (format_parse_comment_line(
                        next_line, &next_indent, &next_body) &&
                    delimiter_depth == 0) {
                    break;
                }

                block_end =
                    next_has_newline ? next_line_end + 1 : next_line_end;
                format_advance_delimiter_depth(
                    &lexer, &token_index, block_end, &delimiter_depth);
            }

            string block_text =
                string_from(text.data + block_start, block_end - block_start);
            bool ok = format_emit_code_block(
                &sb,
                (NerdSource){
                    .source      = block_text,
                    .source_path = source.source_path,
                });
            if (!ok) {
                ok = format_emit_token_stream_block(
                    &sb,
                    (NerdSource){
                        .source      = block_text,
                        .source_path = source.source_path,
                    });
            }
            if (!ok) {
                format_trivia_done(&trivia);
                lex_done(&lexer);
                compiler_memory_profile_end(COMPILER_STAGE_FORMATTER,
                                            COMPILER_PHASE_FORMAT_SOURCE,
                                            memory_before);
                return false;
            }
            offset                  = block_end;
            just_emitted_blank_line = false;
            continue;
        }

        Arena paragraph_arena = {0};
        arena_init(&paragraph_arena);
        string paragraph = {0};
        if (indent.count == 0 && sb.size > 0 && !just_emitted_blank_line) {
            sb_append_char(&sb, '\n');
            just_emitted_blank_line = true;
        }

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
    format_trivia_done(&trivia);
    lex_done(&lexer);
    compiler_memory_profile_end(
        COMPILER_STAGE_FORMATTER, COMPILER_PHASE_FORMAT_SOURCE, memory_before);
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
