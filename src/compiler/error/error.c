//------------------------------------------------------------------------------
// Error module
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/error/error.h>

//------------------------------------------------------------------------------

global_variable bool  g_error_test_mode   = false;
global_variable bool  g_error_system_live = false;
global_variable Arena g_error_arena       = {0};

internal void error_info_done(ErrorInfo* error_info)
{
    array_free(error_info->references);
    array_free(error_info->notes);
    array_free(error_info->help_messages);
    *error_info = (ErrorInfo){0};
}

void error_system_init(bool test_mode)
{
    if (g_error_system_live) {
        arena_done(&g_error_arena);
    }

    arena_init(&g_error_arena);
    g_error_system_live = true;
    g_error_test_mode   = test_mode;
}

void error_system_done(void)
{
    if (!g_error_system_live) {
        return;
    }

    arena_done(&g_error_arena);
    g_error_arena       = (Arena){0};
    g_error_system_live = false;
    g_error_test_mode   = false;
}

bool error_ice(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    eprn("%sinternal compiler error:%s", ANSI_BOLD_BLUE, ANSI_RESET);
    eprv(format, args);
    eprn("");
    va_end(args);
    exit(1);
}

ErrorInfo error_init(
    u16 code, string source_file, usize primary_offset, cstr error_format, ...)
{
    va_list args;
    va_start(args, error_format);
    string error_message = string_formatv(&g_error_arena, error_format, args);
    va_end(args);

    return (ErrorInfo){
        .code           = code,
        .error_message  = error_message,
        .source_file    = source_file,
        .primary_offset = primary_offset,
    };
}

void error_add_reference(ErrorInfo*   error_info,
                         ErrorRefKind kind,
                         usize        offset,
                         usize        length,
                         cstr         format,
                         ...)
{
    va_list args;
    va_start(args, format);
    string message = string_formatv(&g_error_arena, format, args);
    va_end(args);

    array_push(error_info->references,
               (ErrorRef){
                   .kind     = ERROR_KIND_ERROR,
                   .ref_kind = kind,
                   .offset   = offset,
                   .length   = length,
                   .message  = message,
               });
}

void error_add_note(ErrorInfo* error_info, cstr format, ...)
{
    va_list args;
    va_start(args, format);
    string note = string_formatv(&g_error_arena, format, args);
    va_end(args);

    array_push(error_info->notes, note);
}

void error_add_help(ErrorInfo* error_info, cstr format, ...)
{
    va_list args;
    va_start(args, format);
    string help = string_formatv(&g_error_arena, format, args);
    va_end(args);

    array_push(error_info->help_messages, help);
}

void error_render(const ErrorInfo* error_info)
{
    if (g_error_test_mode) {
        eprn("{");
        eprn("  \"code\": \"%04u\",", error_info->code);
        eprn("  \"message\": \"" STRINGP "\",",
             STRINGV(error_info->error_message));
        eprn("  \"offset\": %zu", error_info->primary_offset);
        eprn("}");
        return;
    }

    eprn("%serror[%04u]:%s " STRINGP,
         ANSI_BOLD_RED,
         error_info->code,
         ANSI_RESET,
         STRINGV(error_info->error_message));
    eprn("  at offset %zu", error_info->primary_offset);

    for (usize i = 0; i < array_count(error_info->notes); i++) {
        eprn("%snote:%s " STRINGP,
             ANSI_CYAN,
             ANSI_RESET,
             STRINGV(error_info->notes[i]));
    }

    for (usize i = 0; i < array_count(error_info->help_messages); i++) {
        eprn("%shelp:%s " STRINGP,
             ANSI_CYAN,
             ANSI_RESET,
             STRINGV(error_info->help_messages[i]));
    }
}

bool error_0100_unexpected_character(string source_file, usize offset, char c)
{
    ErrorInfo error_info =
        error_init(100, source_file, offset, "Unexpected character '%c'", c);
    error_add_reference(&error_info,
                        ERROR_REF_PRIMARY,
                        offset,
                        1,
                        "Unexpected character '%c'",
                        c);
    error_add_help(&error_info, "Review the input near the failing character.");
    error_render(&error_info);
    error_info_done(&error_info);
    return false;
}

bool error_0101_integer_literal_too_large(string source_file, usize offset)
{
    ErrorInfo error_info =
        error_init(101, source_file, offset, "Integer literal is too large");
    error_add_reference(&error_info,
                        ERROR_REF_PRIMARY,
                        offset,
                        1,
                        "Literal overflow starts here");
    error_add_help(&error_info, "Use a smaller integer literal.");
    error_render(&error_info);
    error_info_done(&error_info);
    return false;
}

bool error_0102_file_too_many_tokens(string source_file)
{
    ErrorInfo error_info =
        error_init(102, source_file, 0, "Source file is too large");
    error_add_help(&error_info, "Split the input into a smaller source file.");
    error_render(&error_info);
    error_info_done(&error_info);
    return false;
}

//------------------------------------------------------------------------------
