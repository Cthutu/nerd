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

internal Array(string) testing_split_lines(string text)
{
    Array(string) lines = NULL;
    usize         cursor = 0;

    while (cursor < text.count) {
        array_push(lines, testing_next_line(text, &cursor));
    }

    if (text.count == 0) {
        array_push(lines, (string){0});
    }

    return lines;
}

internal isize testing_find_matching_line(Array(string) haystack,
                                          usize         start_index,
                                          string        needle,
                                          usize         max_lookahead)
{
    usize end_index = start_index + max_lookahead;
    if (end_index > array_count(haystack)) {
        end_index = array_count(haystack);
    }

    for (usize i = start_index; i < end_index; ++i) {
        if (string_eq(haystack[i], needle)) {
            return (isize)i;
        }
    }

    return -1;
}

internal void testing_diff_print_line_pair(usize  expected_line_number,
                                           string expected_line,
                                           usize  actual_line_number,
                                           string actual_line)
{
    bool same = string_eq(expected_line, actual_line);

    if (same) {
        eprn("%s %4zu | " STRINGP "%s",
             ANSI_FAINT_WHITE,
             expected_line_number,
             STRINGV(expected_line),
             ANSI_RESET);
        return;
    }

    Arena arena = {0};
    arena_init(&arena);
    string expected_visible = expected_line;
    string actual_visible   = actual_line;

    if (testing_line_is_blank(expected_line) ||
        testing_line_is_blank(actual_line) ||
        testing_lines_differ_only_in_whitespace(expected_line, actual_line)) {
        expected_visible = testing_visualize_whitespace(&arena, expected_line);
        actual_visible   = testing_visualize_whitespace(&arena, actual_line);
    }

    eprn("%s-%4zu | " STRINGP "%s",
         ANSI_GREEN,
         expected_line_number,
         STRINGV(expected_visible),
         ANSI_RESET);
    eprn("%s+%4zu | " STRINGP "%s",
         ANSI_RED,
         actual_line_number,
         STRINGV(actual_visible),
         ANSI_RESET);

    arena_done(&arena);
}

void testing_diff_print(string expected, string actual)
{
    eprn("%sExpected%s / %sActual%s diff:",
         ANSI_GREEN,
         ANSI_RESET,
         ANSI_RED,
         ANSI_RESET);

    Array(string) expected_lines = testing_split_lines(expected);
    Array(string) actual_lines   = testing_split_lines(actual);
    usize         expected_index = 0;
    usize         actual_index   = 0;
    usize         lookahead      = 6;

    while (expected_index < array_count(expected_lines) ||
           actual_index < array_count(actual_lines)) {
        string expected_line = expected_index < array_count(expected_lines)
                                   ? expected_lines[expected_index]
                                   : (string){0};
        string actual_line   = actual_index < array_count(actual_lines)
                                   ? actual_lines[actual_index]
                                   : (string){0};

        if (expected_index < array_count(expected_lines) &&
            actual_index < array_count(actual_lines) &&
            string_eq(expected_line, actual_line)) {
            testing_diff_print_line_pair(
                expected_index + 1, expected_line, actual_index + 1, actual_line);
            ++expected_index;
            ++actual_index;
            continue;
        }

        isize actual_match = expected_index < array_count(expected_lines)
                                 ? testing_find_matching_line(actual_lines,
                                                              actual_index + 1,
                                                              expected_line,
                                                              lookahead)
                                 : -1;
        isize expected_match = actual_index < array_count(actual_lines)
                                   ? testing_find_matching_line(expected_lines,
                                                                expected_index + 1,
                                                                actual_line,
                                                                lookahead)
                                   : -1;

        if (actual_match >= 0 &&
            (expected_match < 0 ||
             (usize)(actual_match - (isize)actual_index) <=
                 (usize)(expected_match - (isize)expected_index))) {
            while (actual_index < (usize)actual_match) {
                testing_diff_print_line_pair(expected_index + 1,
                                             (string){0},
                                             actual_index + 1,
                                             actual_lines[actual_index]);
                ++actual_index;
            }
            continue;
        }

        if (expected_match >= 0) {
            while (expected_index < (usize)expected_match) {
                testing_diff_print_line_pair(expected_index + 1,
                                             expected_lines[expected_index],
                                             actual_index + 1,
                                             (string){0});
                ++expected_index;
            }
            continue;
        }

        testing_diff_print_line_pair(
            expected_index + 1, expected_line, actual_index + 1, actual_line);
        if (expected_index < array_count(expected_lines)) {
            ++expected_index;
        }
        if (actual_index < array_count(actual_lines)) {
            ++actual_index;
        }
    }

    array_free(expected_lines);
    array_free(actual_lines);
}

//------------------------------------------------------------------------------
