//------------------------------------------------------------------------------
// Test diff helpers
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <testing/diff.h>

//------------------------------------------------------------------------------

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
    prn("%sExpected%s / %sActual%s diff:",
        ANSI_RED,
        ANSI_RESET,
        ANSI_GREEN,
        ANSI_RESET);

    usize expected_cursor = 0;
    usize actual_cursor   = 0;
    usize line_number     = 1;

    while (expected_cursor < expected.count || actual_cursor < actual.count) {
        string expected_line = testing_next_line(expected, &expected_cursor);
        string actual_line   = testing_next_line(actual, &actual_cursor);
        bool   same          = string_eq(expected_line, actual_line);

        if (same) {
            prn("%s %4zu | " STRINGP "%s",
                ANSI_FAINT_BLACK,
                line_number,
                STRINGV(expected_line),
                ANSI_RESET);
        } else {
            prn("%s-%4zu | " STRINGP "%s",
                ANSI_RED,
                line_number,
                STRINGV(expected_line),
                ANSI_RESET);
            prn("%s+%4zu | " STRINGP "%s",
                ANSI_GREEN,
                line_number,
                STRINGV(actual_line),
                ANSI_RESET);
        }

        line_number++;
    }
}

//------------------------------------------------------------------------------
