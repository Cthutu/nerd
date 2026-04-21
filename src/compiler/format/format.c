//------------------------------------------------------------------------------
// Source formatting implementation
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

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

internal bool format_parse_comment_line(string line,
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

    *out_indent = string_from(line.data, indent_end);

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
    usize line_width   = wrap_width > prefix_width ? wrap_width - prefix_width : 1;
    usize cursor       = 0;

    while (cursor < trimmed.count) {
        while (cursor < trimmed.count && trimmed.data[cursor] == ' ') {
            cursor++;
        }
        if (cursor >= trimmed.count) {
            break;
        }

        usize probe        = cursor;
        usize line_len     = 0;
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
            usize needed = line_len == 0 ? word_len : line_len + 1 + word_len;
            if (line_len != 0 && needed > line_width) {
                break;
            }

            line_len = needed;
            last_word_end = word_end;
            probe = word_end;
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

//------------------------------------------------------------------------------
// Append an unchanged source line.

internal void format_emit_raw_line(StringBuilder* sb, string line, bool has_newline)
{
    sb_append_string(sb, line);
    if (has_newline) {
        sb_append_char(sb, '\n');
    }
}

//------------------------------------------------------------------------------
// Format source text, currently preserving source and reflowing comment blocks.

bool format_source(NerdSource source, Arena* arena, string* out_text)
{
    Lexer lexer = {0};
    if (!lex_with_config(
            source, &(LexerConfig){.mode = LEXER_MODE_FORMAT}, &lexer)) {
        return false;
    }

    StringBuilder sb = {0};
    sb_init(&sb, arena);

    const string text       = source.source;
    const usize wrap_width  = 80;
    usize       offset      = 0;

    while (offset < text.count) {
        usize line_end = offset;
        while (line_end < text.count && text.data[line_end] != '\n') {
            line_end++;
        }

        bool   has_newline = line_end < text.count && text.data[line_end] == '\n';
        string line = string_from(text.data + offset, line_end - offset);

        string indent       = {0};
        string comment_body = {0};
        if (!format_parse_comment_line(line, &indent, &comment_body)) {
            format_emit_raw_line(&sb, line, has_newline);
            offset = has_newline ? line_end + 1 : line_end;
            continue;
        }

        Arena  paragraph_arena = {0};
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
            } else if (paragraph.count == 0) {
                paragraph = string_format(&paragraph_arena, STRINGP, STRINGV(body));
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
            line = string_from(text.data + offset, line_end - offset);

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
    bool ok = format_source((NerdSource){
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
        kill("Failed to open file for writing: %s", output_path);
    }

    usize written = fwrite(rendered.data, 1, rendered.count, file);
    fclose(file);
    if (written != rendered.count) {
        arena_done(&arena);
        filemap_unload(&map);
        kill("Failed to write formatted file: %s", output_path);
    }

    arena_done(&arena);
    filemap_unload(&map);
    return true;
}

//------------------------------------------------------------------------------
