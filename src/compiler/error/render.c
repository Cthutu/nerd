//------------------------------------------------------------------------------
// Error rendering
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/error/error.h>
#include <compiler/lexer/lexer.h>
#include <object/object.h>

#if OS_WINDOWS
#    include <windows.h>
#elif OS_POSIX
#    include <sys/ioctl.h>
#    include <unistd.h>
#endif

//------------------------------------------------------------------------------

internal void error_info_done(ErrorInfo* error_info)
{
    array_free(error_info->references);
    array_free(error_info->notes);
    array_free(error_info->help_messages);
    *error_info = (ErrorInfo){0};
}

internal u32 error_lsp_severity(ErrorKind kind)
{
    switch (kind) {
    case ERROR_KIND_ERROR:
        return 1;
    case ERROR_KIND_WARNING:
        return 2;
    case ERROR_KIND_INTERNAL:
        return 1;
    case ERROR_KIND_RUNTIME:
        return 1;
    default:
        return 1;
    }
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
    case ERROR_KIND_RUNTIME:
        return "runtime-error";
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
        case ERROR_KIND_RUNTIME:
            return ANSI_BOLD_RED;
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

internal usize error_terminal_width(void)
{
#if OS_WINDOWS
    CONSOLE_SCREEN_BUFFER_INFO info = {0};
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_ERROR_HANDLE), &info)) {
        return (usize)(info.srWindow.Right - info.srWindow.Left + 1);
    }
#elif OS_POSIX
    struct winsize ws = {0};
    if (ioctl(STDERR_FILENO, TIOCGWINSZ, &ws) == 0 && ws.ws_col > 0) {
        return (usize)ws.ws_col;
    }
#endif

#if OS_WINDOWS
    char  columns_env[32] = {0};
    DWORD columns_len =
        GetEnvironmentVariableA("COLUMNS", columns_env, sizeof(columns_env));
    if (columns_len > 0 && columns_len < sizeof(columns_env)) {
        usize width = (usize)strtoull(columns_env, NULL, 10);
        if (width > 0) {
            return width;
        }
    }
#elif OS_POSIX
    cstr columns_env = getenv("COLUMNS");
    if (columns_env != NULL && columns_env[0] != '\0') {
        usize width = (usize)strtoull(columns_env, NULL, 10);
        if (width > 0) {
            return width;
        }
    }
#endif

    return 80;
}

internal void
error_print_wrapped(string prefix, string styled_prefix, string text)
{
    usize terminal_width = error_terminal_width();
    usize prefix_width   = prefix.count;
    usize line_width =
        terminal_width > prefix_width ? terminal_width - prefix_width : 1;
    usize cursor     = 0;
    bool  first_line = true;

    while (true) {
        while (cursor < text.count &&
               (text.data[cursor] == ' ' || text.data[cursor] == '\t')) {
            cursor++;
        }

        if (cursor >= text.count) {
            if (first_line) {
                epr(STRINGP "\n", STRINGV(styled_prefix));
            }
            return;
        }

        if (!first_line) {
            epr("%*s", (int)prefix_width, "");
        } else {
            epr(STRINGP, STRINGV(styled_prefix));
        }

        usize line_start    = cursor;
        usize probe         = cursor;
        usize line_len      = 0;
        usize last_word_end = cursor;

        while (probe < text.count) {
            if (text.data[probe] == '\n') {
                last_word_end = probe;
                break;
            }

            while (probe < text.count &&
                   (text.data[probe] == ' ' || text.data[probe] == '\t')) {
                probe++;
            }
            if (probe >= text.count || text.data[probe] == '\n') {
                break;
            }

            usize word_end = probe;
            while (word_end < text.count && text.data[word_end] != ' ' &&
                   text.data[word_end] != '\t' && text.data[word_end] != '\n') {
                word_end++;
            }

            usize word_len = word_end - probe;
            usize needed   = line_len == 0 ? word_len : line_len + 1 + word_len;
            if (line_len != 0 && needed > line_width) {
                break;
            }
            if (line_len == 0 && word_len > line_width) {
                last_word_end = probe + line_width;
                break;
            }

            line_len      = needed;
            last_word_end = word_end;
            probe         = word_end;
        }

        if (last_word_end <= line_start) {
            last_word_end = MIN(line_start + line_width, text.count);
        }

        epr(STRINGP "\n",
            STRINGV(string_from(text.data + line_start,
                                last_word_end - line_start)));

        cursor = last_word_end;
        if (cursor < text.count && text.data[cursor] == '\n') {
            cursor++;
        }
        first_line = false;
    }
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
                                error_info->span.start,
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
    case ERROR_KIND_RUNTIME:
        primary_colour = ANSI_BOLD_RED;
        break;
    default:
        primary_colour = ANSI_BOLD_RED;
        break;
    }

    //
    // Output the main error message and code
    //

    string prefix        = {0};
    string styled_prefix = {0};
    if (error_info->kind == ERROR_KIND_RUNTIME) {
        prefix = string_format(
            &temp_arena, "%s: ", error_kind_label(error_info->kind));
        styled_prefix = string_format(&temp_arena,
                                      "%s%s:%s ",
                                      primary_colour,
                                      error_kind_label(error_info->kind),
                                      ANSI_RESET);
    } else {
        prefix        = string_format(&temp_arena,
                                      "%s[%04u]: ",
                                      error_kind_label(error_info->kind),
                                      error_info->code);
        styled_prefix = string_format(&temp_arena,
                                      "%s%s[%04u]:%s ",
                                      primary_colour,
                                      error_kind_label(error_info->kind),
                                      error_info->code,
                                      ANSI_RESET);
    }
    error_print_wrapped(prefix, styled_prefix, error_info->error_message);

    bool has_source = error_info->source.source.count > 0 ||
                      error_info->source.source_path.count > 0;
    //
    // Determine the line and column number
    //

    if (has_source) {
        u32  line;
        u32  column;
        bool got_line_and_column = lex_offset_to_line_col(
            error_info->source, error_info->span.start, &line, &column);

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
    }

    //
    // Output notes
    //

    for (usize i = 0; i < array_count(error_info->notes); i++) {
        error_print_wrapped(
            s("note: "),
            string_format(
                &temp_arena, "%snote:%s ", ANSI_BOLD_CYAN, ANSI_RESET),
            error_info->notes[i]);
    }

    //
    // Output help messages
    //

    for (usize i = 0; i < array_count(error_info->help_messages); i++) {
        error_print_wrapped(
            s("help: "),
            string_format(
                &temp_arena, "%shelp:%s ", ANSI_BOLD_CYAN, ANSI_RESET),
            error_info->help_messages[i]);
    }
}

//------------------------------------------------------------------------------
// Test-mode error rendering

internal void error_test_render(const ErrorInfo* error_info)
{
    JsonValue* root = json_new_object(&temp_arena);
    json_object_set_string(
        root,
        &temp_arena,
        "code",
        string_format(&temp_arena, "%04u", error_info->code));
    json_object_set_string(
        root, &temp_arena, "message", error_info->error_message);
    json_object_set_string(root,
                           &temp_arena,
                           "source_file",
                           error_info->source.source_path.count > 0
                               ? error_info->source.source_path
                               : s("<input>"));

    u32 primary_line = 0;
    u32 primary_col  = 0;
    if (lex_offset_to_line_col(error_info->source,
                               error_info->span.start,
                               &primary_line,
                               &primary_col)) {
        JsonValue* location = json_new_object(&temp_arena);
        json_object_set_number(
            location, &temp_arena, "line", (f64)primary_line + 1);
        json_object_set_number(
            location, &temp_arena, "column", (f64)primary_col + 1);
        json_object_set_object(root, "primary_location", location);
    } else {
        json_object_set_null(root, &temp_arena, "primary_location");
    }

    JsonValue* refs = json_new_array(&temp_arena);
    for (usize i = 0; i < array_count(error_info->references); i++) {
        const ErrorRef* ref  = &error_info->references[i];
        JsonValue*      obj  = json_new_object(&temp_arena);
        u32             line = 0;
        u32             col  = 0;
        lex_offset_to_line_col(
            error_info->source, ref->span.start, &line, &col);

        json_object_set_cstr(obj,
                             &temp_arena,
                             "kind",
                             ref->ref_kind == ERROR_REF_PRIMARY ? "primary"
                                                                : "secondary");
        json_object_set_number(obj, &temp_arena, "line", (f64)line + 1);
        json_object_set_number(obj, &temp_arena, "column", (f64)col + 1);
        json_object_set_number(
            obj, &temp_arena, "length", (f64)(ref->span.end - ref->span.start));
        json_object_set_string(obj, &temp_arena, "message", ref->message);
        json_array_push(refs, obj);
    }
    json_object_set_array(root, "references", refs);

    JsonValue* notes = json_new_array(&temp_arena);
    for (usize i = 0; i < array_count(error_info->notes); i++) {
        json_array_push(notes,
                        json_new_string(&temp_arena, error_info->notes[i]));
    }
    json_object_set_array(root, "notes", notes);

    JsonValue* help = json_new_array(&temp_arena);
    for (usize i = 0; i < array_count(error_info->help_messages); i++) {
        json_array_push(
            help, json_new_string(&temp_arena, error_info->help_messages[i]));
    }
    json_object_set_array(root, "help", help);

    string rendered =
        json_stringify(&temp_arena, root, .pretty = true, .indent = 4);
    error_system_store_last_rendered(rendered);
    if (error_system_should_emit_output()) {
        eprn(STRINGP, STRINGV(rendered));
    }
    json_done(root);
}

internal JsonValue*
error_make_lsp_range(Arena* arena, NerdSource source, ErrorSpan span)
{
    JsonValue* range = json_new_object(arena);
    JsonValue* start = json_new_object(arena);
    JsonValue* end   = json_new_object(arena);

    u32 start_line   = 0;
    u32 start_col    = 0;
    u32 end_line     = 0;
    u32 end_col      = 0;

    lex_offset_to_line_col(source, span.start, &start_line, &start_col);

    usize end_offset = span.end;
    if (end_offset <= span.start) {
        end_offset = MIN(span.start + 1, source.source.count);
    }
    lex_offset_to_line_col(source, end_offset, &end_line, &end_col);

    json_object_set_number(start, arena, "line", (f64)start_line);
    json_object_set_number(start, arena, "character", (f64)start_col);
    json_object_set_number(end, arena, "line", (f64)end_line);
    json_object_set_number(end, arena, "character", (f64)end_col);

    json_object_set_object(range, "start", start);
    json_object_set_object(range, "end", end);
    return range;
}

internal void error_diagnostics_render(const ErrorInfo* error_info)
{
    JsonValue* diagnostics = json_new_array(&temp_arena);
    JsonValue* diagnostic  = json_new_object(&temp_arena);
    string     source_uri  = error_info->source.source_path.count > 0
                                 ? error_info->source.source_path
                                 : s("<input>");

    json_object_set_object(diagnostic,
                           "range",
                           error_make_lsp_range(&temp_arena,
                                                error_info->source,
                                                error_info->span));
    json_object_set_number(diagnostic,
                           &temp_arena,
                           "severity",
                           (f64)error_lsp_severity(error_info->kind));
    json_object_set_string(
        diagnostic,
        &temp_arena,
        "code",
        string_format(&temp_arena, "%04u", error_info->code));
    json_object_set_cstr(diagnostic, &temp_arena, "source", "nerd");
    json_object_set_string(
        diagnostic, &temp_arena, "message", error_info->error_message);

    JsonValue* related = json_new_array(&temp_arena);
    for (usize i = 0; i < array_count(error_info->references); i++) {
        const ErrorRef* ref = &error_info->references[i];
        if (ref->ref_kind == ERROR_REF_PRIMARY) {
            continue;
        }

        JsonValue* info     = json_new_object(&temp_arena);
        JsonValue* location = json_new_object(&temp_arena);
        json_object_set_string(location, &temp_arena, "uri", source_uri);
        json_object_set_object(
            location,
            "range",
            error_make_lsp_range(&temp_arena, error_info->source, ref->span));
        json_object_set_object(info, "location", location);
        json_object_set_string(info, &temp_arena, "message", ref->message);
        json_array_push(related, info);
    }

    for (usize i = 0; i < array_count(error_info->notes); i++) {
        JsonValue* info     = json_new_object(&temp_arena);
        JsonValue* location = json_new_object(&temp_arena);
        json_object_set_string(location, &temp_arena, "uri", source_uri);
        json_object_set_object(location,
                               "range",
                               error_make_lsp_range(&temp_arena,
                                                    error_info->source,
                                                    error_info->span));
        json_object_set_object(info, "location", location);
        json_object_set_string(info,
                               &temp_arena,
                               "message",
                               string_format(&temp_arena,
                                             "note: " STRINGP,
                                             STRINGV(error_info->notes[i])));
        json_array_push(related, info);
    }

    for (usize i = 0; i < array_count(error_info->help_messages); i++) {
        JsonValue* info     = json_new_object(&temp_arena);
        JsonValue* location = json_new_object(&temp_arena);
        json_object_set_string(location, &temp_arena, "uri", source_uri);
        json_object_set_object(location,
                               "range",
                               error_make_lsp_range(&temp_arena,
                                                    error_info->source,
                                                    error_info->span));
        json_object_set_object(info, "location", location);
        json_object_set_string(
            info,
            &temp_arena,
            "message",
            string_format(&temp_arena,
                          "help: " STRINGP,
                          STRINGV(error_info->help_messages[i])));
        json_array_push(related, info);
    }

    if (array_count(json_array(related).values) > 0) {
        json_object_set_array(diagnostic, "relatedInformation", related);
    } else {
        json_done(related);
    }

    json_array_push(diagnostics, diagnostic);

    string rendered =
        json_stringify(&temp_arena, diagnostics, .pretty = false, .indent = 2);
    error_system_store_last_rendered(rendered);
    if (error_system_should_emit_output()) {
        eprn(STRINGP, STRINGV(rendered));
    }
    json_done(diagnostics);
}

//------------------------------------------------------------------------------
// Error rendering

void error_render(ErrorInfo* error_info)
{
    switch (error_system_mode()) {
    case ERROR_RENDER_TEST:
        error_test_render(error_info);
        break;
    case ERROR_RENDER_DIAGNOSTICS:
        error_diagnostics_render(error_info);
        break;
    case ERROR_RENDER_NORMAL:
    default:
        error_normal_render(error_info);
        break;
    }

    error_info_done(error_info);
    error_system_reset();
}
