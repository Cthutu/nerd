//------------------------------------------------------------------------------
// Test diff helpers
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <testing/diff.h>

//------------------------------------------------------------------------------

internal bool testing_line_is_blank(string text)
{
    for (usize i = 0; i < text.count; i++) {
        if (text.data[i] != ' ' && text.data[i] != '\t' &&
            text.data[i] != '\r') {
            return false;
        }
    }

    return true;
}

internal string testing_remove_horizontal_whitespace(Arena* arena, string text)
{
    StringBuilder sb = {0};
    sb_init(&sb, arena);

    for (usize i = 0; i < text.count; i++) {
        if (text.data[i] != ' ' && text.data[i] != '\t' &&
            text.data[i] != '\r') {
            sb_append_char(&sb, (char)text.data[i]);
        }
    }

    return sb_to_string(&sb);
}

internal bool testing_lines_differ_only_in_whitespace(string expected,
                                                      string actual)
{
    Arena arena = {0};
    arena_init(&arena);

    string expected_compact =
        testing_remove_horizontal_whitespace(&arena, expected);
    string actual_compact =
        testing_remove_horizontal_whitespace(&arena, actual);
    bool same = string_eq(expected_compact, actual_compact);

    arena_done(&arena);
    return same;
}

internal string testing_visualize_whitespace(Arena* arena, string text)
{
    StringBuilder sb = {0};
    sb_init(&sb, arena);

    if (text.count == 0) {
        sb_append_cstr(&sb, "<empty>");
        return sb_to_string(&sb);
    }

    for (usize i = 0; i < text.count; i++) {
        switch (text.data[i]) {
        case ' ':
            sb_append_cstr(&sb, "·");
            break;
        case '\t':
            sb_append_cstr(&sb, "⇥");
            break;
        case '\r':
            sb_append_cstr(&sb, "␍");
            break;
        default:
            sb_append_char(&sb, (char)text.data[i]);
            break;
        }
    }

    return sb_to_string(&sb);
}

internal string testing_next_line(string text, usize* cursor)
{
    if (*cursor >= text.count) {
        return (string){0};
    }

    usize start = *cursor;
    while (*cursor < text.count && text.data[*cursor] != '\n') {
        (*cursor)++;
    }

    usize end = *cursor;
    if (*cursor < text.count && text.data[*cursor] == '\n') {
        (*cursor)++;
    }

    return string_from(text.data + start, end - start);
}

void testing_diff_print(string expected, string actual)
{
    eprn("%sExpected%s / %sActual%s diff:",
         ANSI_GREEN,
         ANSI_RESET,
         ANSI_RED,
         ANSI_RESET);

    usize expected_cursor = 0;
    usize actual_cursor   = 0;
    usize line_number     = 1;

    while (expected_cursor < expected.count || actual_cursor < actual.count) {
        string expected_line = testing_next_line(expected, &expected_cursor);
        string actual_line   = testing_next_line(actual, &actual_cursor);
        bool   same          = string_eq(expected_line, actual_line);

        if (same) {
            eprn("%s %4zu | " STRINGP "%s",
                 ANSI_FAINT_WHITE,
                 line_number,
                 STRINGV(expected_line),
                 ANSI_RESET);
        } else {
            Arena arena = {0};
            arena_init(&arena);
            string expected_visible = expected_line;
            string actual_visible   = actual_line;

            if (testing_line_is_blank(expected_line) ||
                testing_line_is_blank(actual_line) ||
                testing_lines_differ_only_in_whitespace(expected_line,
                                                        actual_line)) {
                expected_visible =
                    testing_visualize_whitespace(&arena, expected_line);
                actual_visible =
                    testing_visualize_whitespace(&arena, actual_line);
            }

            eprn("%s-%4zu | " STRINGP "%s",
                 ANSI_GREEN,
                 line_number,
                 STRINGV(expected_visible),
                 ANSI_RESET);
            eprn("%s+%4zu | " STRINGP "%s",
                 ANSI_RED,
                 line_number,
                 STRINGV(actual_visible),
                 ANSI_RESET);

            arena_done(&arena);
        }

        line_number++;
    }
}

//------------------------------------------------------------------------------
