//------------------------------------------------------------------------------
// C Code dumping
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/cgen/cgen.h>

//------------------------------------------------------------------------------

internal usize digits_usize(usize n)
{
    usize digits = 1;
    while (n >= 10) {
        n /= 10;
        ++digits;
    }
    return digits;
}

void cgen_dump(const CGen* cgen)
{
    prn("\nC Code:\n");

    Arena  arena    = {0};
    arena_init(&arena);
    string rendered = cgen_render(cgen, &arena);
    const u8* data  = rendered.data;
    usize     len   = rendered.count;

    if (len == 0) {
        arena_done(&arena);
        return;
    }

    usize line_count = 0;
    for (usize i = 0; i < len; ++i) {
        if (data[i] == '\n') {
            ++line_count;
        }
    }
    if (data[len - 1] != '\n') {
        ++line_count;
    }

    usize line_digits = digits_usize(line_count);
    usize line_number = 1;
    usize line_start  = 0;

    for (usize i = 0; i <= len; ++i) {
        bool is_line_end = (i == len) || (data[i] == '\n');
        if (!is_line_end) {
            continue;
        }

        usize line_len = i - line_start;
        pr("%s%*zu%s %s" UNICODE_TABLE_VERTICAL "%s ",
           ANSI_FAINT_CYAN,
           (int)line_digits,
           line_number,
           ANSI_RESET,
           ANSI_FAINT_BLUE,
           ANSI_RESET);

        usize comment_index = line_len;
        for (usize j = 0; j + 1 < line_len; ++j) {
            if (data[line_start + j] == '/' &&
                data[line_start + j + 1] == '/') {
                comment_index = j;
                break;
            }
        }

        if (comment_index == line_len) {
            pr("%s%.*s%s",
               ANSI_WHITE,
               (int)line_len,
               (const char*)(data + line_start),
               ANSI_RESET);
        } else {
            pr("%s%.*s%s",
               ANSI_WHITE,
               (int)comment_index,
               (const char*)(data + line_start),
               ANSI_RESET);
            pr("%s%.*s%s",
               ANSI_GREEN,
               (int)(line_len - comment_index),
               (const char*)(data + line_start + comment_index),
               ANSI_RESET);
        }
        pr("\n");

        line_start = i + 1;
        ++line_number;
    }

    arena_done(&arena);
}
