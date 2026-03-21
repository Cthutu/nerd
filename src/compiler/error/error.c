//------------------------------------------------------------------------------
// Error module
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/error/error.h>

//------------------------------------------------------------------------------

global_variable bool  g_error_test_mode = false;
global_variable Arena g_error_arena     = {0};

void error_system_init(bool test_mode)
{
    arena_init(&g_error_arena);
    g_error_test_mode = test_mode;
}

void error_system_done(void)
{
    arena_done(&g_error_arena);
    g_error_arena     = (Arena){0};
    g_error_test_mode = false;
}

bool error_system_is_test_mode(void) { return g_error_test_mode; }

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

void error_system_reset(void) { arena_reset(&g_error_arena); }

internal ErrorInfo error_info_init(ErrorKind  kind,
                                   u16        code,
                                   NerdSource source,
                                   usize      primary_offset,
                                   cstr       error_format,
                                   va_list    args)
{
    string error_message = string_formatv(&g_error_arena, error_format, args);

    return (ErrorInfo){
        .kind           = kind,
        .code           = code,
        .error_message  = error_message,
        .source         = source,
        .primary_offset = primary_offset,
    };
}

ErrorInfo error_init(
    u16 code, NerdSource source, usize primary_offset, cstr error_format, ...)
{
    va_list args;
    va_start(args, error_format);
    ErrorInfo error_info = error_info_init(
        ERROR_KIND_ERROR, code, source, primary_offset, error_format, args);
    va_end(args);
    return error_info;
}

ErrorInfo warning_init(
    u16 code, NerdSource source, usize primary_offset, cstr error_format, ...)
{
    va_list args;
    va_start(args, error_format);
    ErrorInfo error_info = error_info_init(
        ERROR_KIND_WARNING, code, source, primary_offset, error_format, args);
    va_end(args);
    return error_info;
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
