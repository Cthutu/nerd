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

    eprn("%serror[%04u]:%s " STRINGP,
         primary_colour,
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
             STRINGV(error_info->source.source_path),
             line + 1,
             column + 1);
    } else {
        eprn(" --> " STRINGP ":<unknown-position>",
             STRINGV(error_info->source.source_path));
    }

    //
    // Output the code snippet
    //

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
