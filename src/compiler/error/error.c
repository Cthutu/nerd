//------------------------------------------------------------------------------
// Error module
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/error/error.h>

//------------------------------------------------------------------------------

global_variable ErrorRenderMode g_error_mode           = ERROR_RENDER_NORMAL;
global_variable bool            g_error_emit_output    = true;
global_variable Arena           g_error_arena          = {0};
global_variable Arena           g_error_rendered_arena = {0};
global_variable string          g_error_last_rendered  = {0};

internal ErrorInfo error_info_init(ErrorKind  kind,
                                   u16        code,
                                   NerdSource source,
                                   ErrorSpan  span,
                                   cstr       error_format,
                                   va_list    args);

void error_system_init(ErrorRenderMode mode)
{
    arena_init(&g_error_arena);
    arena_init(&g_error_rendered_arena);
    g_error_mode          = mode;
    g_error_emit_output   = true;
    g_error_last_rendered = (string){0};
}

void error_system_done(void)
{
    arena_done(&g_error_arena);
    arena_done(&g_error_rendered_arena);
    g_error_arena          = (Arena){0};
    g_error_rendered_arena = (Arena){0};
    g_error_mode           = ERROR_RENDER_NORMAL;
    g_error_emit_output    = true;
    g_error_last_rendered  = (string){0};
}

void error_system_set_mode(ErrorRenderMode mode) { g_error_mode = mode; }

void error_system_set_emit_output(bool emit_output)
{
    g_error_emit_output = emit_output;
}

void error_system_clear_last_rendered(void)
{
    arena_reset(&g_error_rendered_arena);
    g_error_last_rendered = (string){0};
}

string error_system_last_rendered(void) { return g_error_last_rendered; }

void error_system_store_last_rendered(string rendered)
{
    error_system_clear_last_rendered();
    if (rendered.count == 0) {
        return;
    }

    u8* copy = (u8*)arena_alloc(&g_error_rendered_arena, rendered.count);
    memcpy(copy, rendered.data, rendered.count);
    g_error_last_rendered = string_from(copy, rendered.count);
}

ErrorRenderMode error_system_mode(void) { return g_error_mode; }
bool error_system_should_emit_output(void) { return g_error_emit_output; }

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

bool error_runtime(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    ErrorInfo error_info = error_info_init(
        ERROR_KIND_RUNTIME, 0, (NerdSource){0}, (ErrorSpan){0}, format, args);
    va_end(args);
    error_render(&error_info);
    return false;
}

void error_system_reset(void) { arena_reset(&g_error_arena); }

internal ErrorInfo error_info_init(ErrorKind  kind,
                                   u16        code,
                                   NerdSource source,
                                   ErrorSpan  span,
                                   cstr       error_format,
                                   va_list    args)
{
    string error_message = string_formatv(&g_error_arena, error_format, args);

    return (ErrorInfo){
        .kind          = kind,
        .code          = code,
        .error_message = error_message,
        .source        = source,
        .span          = span,
    };
}

ErrorInfo
error_init(u16 code, NerdSource source, ErrorSpan span, cstr error_format, ...)
{
    va_list args;
    va_start(args, error_format);
    ErrorInfo error_info = error_info_init(
        ERROR_KIND_ERROR, code, source, span, error_format, args);
    va_end(args);
    return error_info;
}

ErrorInfo warning_init(
    u16 code, NerdSource source, ErrorSpan span, cstr error_format, ...)
{
    va_list args;
    va_start(args, error_format);
    ErrorInfo error_info = error_info_init(
        ERROR_KIND_WARNING, code, source, span, error_format, args);
    va_end(args);
    return error_info;
}

void error_add_reference(
    ErrorInfo* error_info, ErrorRefKind kind, ErrorSpan span, cstr format, ...)
{
    va_list args;
    va_start(args, format);
    string message = string_formatv(&g_error_arena, format, args);
    va_end(args);

    array_push(error_info->references,
               (ErrorRef){
                   .ref_kind = kind,
                   .span     = span,
                   .message  = message,
               });
}

void error_add_notev(ErrorInfo* error_info, cstr format, va_list args)
{
    string note = string_formatv(&g_error_arena, format, args);

    array_push(error_info->notes, note);
}

void error_add_note(ErrorInfo* error_info, cstr format, ...)
{
    va_list args;
    va_start(args, format);
    error_add_notev(error_info, format, args);
    va_end(args);
}

void error_add_helpv(ErrorInfo* error_info, cstr format, va_list args)
{
    string help = string_formatv(&g_error_arena, format, args);
    array_push(error_info->help_messages, help);
}

void error_add_help(ErrorInfo* error_info, cstr format, ...)
{
    va_list args;
    va_start(args, format);
    error_add_helpv(error_info, format, args);
    va_end(args);
}
