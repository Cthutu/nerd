//------------------------------------------------------------------------------
// Error rendering
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/error/error.h>
#include <compiler/lexer/lexer.h>

//------------------------------------------------------------------------------

internal void error_info_done(ErrorInfo* error_info)
{
    array_free(error_info->references);
    array_free(error_info->notes);
    array_free(error_info->help_messages);
    *error_info = (ErrorInfo){0};
}

internal cstr error_kind_label(ErrorKind kind)
{
    switch (kind) {
    case ERROR_KIND_WARNING:
        return "warning";
    case ERROR_KIND_ERROR:
        return "error";
    case ERROR_KIND_INTERNAL:
        return "internal";
    default:
        return "error";
    }
}

internal cstr error_reference_colour(const ErrorInfo* error_info,
                                     const ErrorRef*  ref)
{
    if (ref->ref_kind == ERROR_REF_PRIMARY) {
        switch (error_info->kind) {
        case ERROR_KIND_WARNING:
            return ANSI_BOLD_YELLOW;
        case ERROR_KIND_ERROR:
            return ANSI_BOLD_RED;
        case ERROR_KIND_INTERNAL:
            return ANSI_BOLD_BLUE;
        default:
            return ANSI_BOLD_RED;
        }
    }

    return ANSI_BOLD_CYAN;
}

internal ErrorSpan error_find_line_span(string source_text, u32 line)
{
    usize current_line = 0;
    usize line_start   = 0;

    for (usize i = 0; i < source_text.count; i++) {
        if (current_line == line) {
            usize line_end = i;
            while (line_end < source_text.count && source_text.data[line_end] &&
                   source_text.data[line_end] != '\n') {
                line_end++;
            }
            return (ErrorSpan){.start = line_start, .end = line_end};
        }

        if (source_text.data[i] == '\n') {
            current_line++;
            line_start = i + 1;
        }
    }

    if (current_line == line && line_start <= source_text.count) {
        return (ErrorSpan){.start = line_start, .end = source_text.count};
    }

    return (ErrorSpan){0};
}

internal u32 error_count_lines(string source_text)
{
    if (source_text.count == 0) {
        return 1;
    }

    u32 line_count = 1;
    for (usize i = 0; i < source_text.count; i++) {
        if (source_text.data[i] == '\n') {
            line_count++;
        }
    }
    return line_count;
}

internal usize error_decimal_width(u32 value)
{
    usize width = 1;
    while (value >= 10) {
        value /= 10;
        width++;
    }
    return width;
}

internal void error_print_gutter_blank(usize gutter_width)
{
    epr("%*s %s|%s\n", (int)gutter_width, "", ANSI_BOLD_CYAN, ANSI_RESET);
}

internal void
error_print_source_line(usize gutter_width, u32 line_number, string source_line)
{
    epr("%*u %s|%s " STRINGP "\n",
        (int)gutter_width,
        line_number,
        ANSI_BOLD_CYAN,
        ANSI_RESET,
        STRINGV(source_line));
}

internal bool error_reference_touches_line(const ErrorRef* ref,
                                           usize           line_start,
                                           usize           line_end)
{
    usize ref_start = ref->span.start;
    usize ref_end   = ref->span.end;

    if (line_start == line_end) {
        return ref_start == line_start;
    }

    return ref_start < line_end && ref_end > line_start;
}

internal void error_print_reference_line(const ErrorInfo* error_info,
                                         const ErrorRef*  ref,
                                         usize            gutter_width,
                                         usize            line_start,
                                         usize            line_end)
{
    usize line_length  = line_end - line_start;
    usize marker_start = 0;
    usize marker_end   = 0;
    usize ref_start    = ref->span.start;
    usize ref_end      = ref->span.end;

    if (ref_start > line_start) {
        marker_start = ref_start - line_start;
    }

    marker_end = line_length;
    if (ref_end <= line_end) {
        marker_end = ref_end - line_start;
    }

    if (marker_end <= marker_start) {
        marker_end = MIN(marker_start + 1, line_length == 0 ? 1 : line_length);
    }

    char marker_char   = ref->ref_kind == ERROR_REF_PRIMARY ? '^' : '~';
    cstr marker_colour = error_reference_colour(error_info, ref);

    epr("%*s %s|%s ", (int)gutter_width, "", ANSI_BOLD_CYAN, ANSI_RESET);
    for (usize i = 0; i < marker_start; i++) {
        epr("%c", ' ');
    }

    epr("%s", marker_colour);
    for (usize i = marker_start; i < marker_end; i++) {
        epr("%c", marker_char);
    }

    if (ref->message.count > 0) {
        epr(" " STRINGP, STRINGV(ref->message));
    } else {
        epr("%s", ANSI_RESET);
    }
    epr("%s\n", ANSI_RESET);
}

internal void error_print_snippet(const ErrorInfo* error_info)
{
    string source_text = error_info->source.source;
    if (source_text.count == 0) {
        return;
    }

    u32 primary_line = 0;
    u32 primary_col  = 0;
    if (!lex_offset_to_line_col(error_info->source,
                                error_info->primary_offset,
                                &primary_line,
                                &primary_col)) {
        return;
    }

    UNUSED(primary_col);

    u32 first_line = primary_line;
    u32 last_line  = primary_line;
    for (usize i = 0; i < array_count(error_info->references); i++) {
        u32 ref_line = 0;
        u32 ref_col  = 0;
        if (lex_offset_to_line_col(error_info->source,
                                   error_info->references[i].span.start,
                                   &ref_line,
                                   &ref_col)) {
            first_line = MIN(first_line, ref_line);
            last_line  = MAX(last_line, ref_line);
        }
        UNUSED(ref_col);
    }

    u32   total_lines   = error_count_lines(source_text);
    u32   display_start = first_line > 2 ? first_line - 2 : 0;
    u32   display_end   = MIN(last_line + 2, total_lines - 1);
    usize gutter_width  = error_decimal_width(display_end + 1);

    error_print_gutter_blank(gutter_width);

    for (u32 line = display_start; line <= display_end; line++) {
        ErrorSpan line_span = error_find_line_span(source_text, line);
        string    line_text = string_from(source_text.data + line_span.start,
                                       line_span.end - line_span.start);
        error_print_source_line(gutter_width, line + 1, line_text);

        for (usize i = 0; i < array_count(error_info->references); i++) {
            const ErrorRef* ref = &error_info->references[i];
            if (error_reference_touches_line(
                    ref, line_span.start, line_span.end)) {
                error_print_reference_line(error_info,
                                           ref,
                                           gutter_width,
                                           line_span.start,
                                           line_span.end);
            }
        }
    }

    error_print_gutter_blank(gutter_width);
}

//------------------------------------------------------------------------------
// Normal error rendering

internal void error_normal_render(const ErrorInfo* error_info)
{
    //
    // Determine the primary colour
    //

    cstr primary_colour;
    switch (error_info->kind) {
    case ERROR_KIND_ERROR:
        primary_colour = ANSI_BOLD_RED;
        break;
    case ERROR_KIND_WARNING:
        primary_colour = ANSI_BOLD_YELLOW;
        break;
    case ERROR_KIND_INTERNAL:
        primary_colour = ANSI_BOLD_BLUE;
        break;
    default:
        primary_colour = ANSI_BOLD_RED;
        break;
    }

    //
    // Output the main error message and code
    //

    eprn("%s%s[%04u]:%s " STRINGP,
         primary_colour,
         error_kind_label(error_info->kind),
         error_info->code,
         ANSI_RESET,
         STRINGV(error_info->error_message));

    //
    // Determine the line and column number
    //

    u32  line;
    u32  column;
    bool got_line_and_column = lex_offset_to_line_col(
        error_info->source, error_info->primary_offset, &line, &column);

    if (got_line_and_column) {
        eprn(" --> " STRINGP ":%u:%u",
             STRINGV(error_info->source.source_path.count > 0
                         ? error_info->source.source_path
                         : s("<input>")),
             line + 1,
             column + 1);
    } else {
        eprn(" --> " STRINGP ":<unknown-position>",
             STRINGV(error_info->source.source_path.count > 0
                         ? error_info->source.source_path
                         : s("<input>")));
    }

    //
    // Output the code snippet
    //

    error_print_snippet(error_info);

    //
    // Output notes
    //

    for (usize i = 0; i < array_count(error_info->notes); i++) {
        eprn("%snote:%s " STRINGP,
             ANSI_BOLD_CYAN,
             ANSI_RESET,
             STRINGV(error_info->notes[i]));
    }

    //
    // Output help messages
    //

    for (usize i = 0; i < array_count(error_info->help_messages); i++) {
        eprn("%shelp:%s " STRINGP,
             ANSI_BOLD_CYAN,
             ANSI_RESET,
             STRINGV(error_info->help_messages[i]));
    }
}

//------------------------------------------------------------------------------
// Test-mode error rendering

internal void error_test_render(const ErrorInfo* error_info)
{
    eprn("{");
    eprn("  \"code\": \"%04u\",", error_info->code);
    eprn("  \"message\": \"" STRINGP "\",", STRINGV(error_info->error_message));
    eprn("  \"offset\": %zu", error_info->primary_offset);
    eprn("}");
}

//------------------------------------------------------------------------------
// Error rendering

void error_render(ErrorInfo* error_info)
{
    if (error_system_is_test_mode()) {
        error_test_render(error_info);
    } else {
        error_normal_render(error_info);
    }

    error_info_done(error_info);
    error_system_reset();
}
