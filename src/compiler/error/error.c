//------------------------------------------------------------------------------
// Error module
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/error/error.h>

//------------------------------------------------------------------------------

static thread_local ErrorContext  g_error_default = {.emit_output = true};
static thread_local ErrorContext* g_error_current = NULL;

internal ErrorContext* error_context_current(void)
{
    return g_error_current != NULL ? g_error_current : &g_error_default;
}

internal Arena* error_message_arena(void)
{
    Arena* arena = &error_context_current()->arena;
    if (arena->data == NULL) {
        arena_init(arena);
    }
    return arena;
}

ErrorContext* error_context_select(ErrorContext* context)
{
    ErrorContext* previous = g_error_current;
    g_error_current        = context;
    return previous;
}

void error_context_init(ErrorContext*   context,
                        ErrorRenderMode mode,
                        bool            capture)
{
    *context =
        (ErrorContext){.mode = mode, .emit_output = true, .capture = capture};
}

internal void error_context_clear_pending(ErrorContext* context)
{
    for (usize i = 0; i < array_count(context->pending); ++i) {
        ErrorInfo* info = &context->pending[i];
        array_free(info->references);
        array_free(info->notes);
        array_free(info->help_messages);
        array_free(info->source.fragments);
    }
    array_free(context->pending);
    arena_reset(&context->captured_arena);
}

void error_context_done(ErrorContext* context)
{
    ASSERT(g_error_current != context,
           "Restore the previous diagnostic context before cleanup");
    error_context_clear_pending(context);
    if (context->arena.data != NULL) {
        arena_done(&context->arena);
    }
    if (context->rendered_arena.data != NULL) {
        arena_done(&context->rendered_arena);
    }
    if (context->captured_arena.data != NULL) {
        arena_done(&context->captured_arena);
    }
    *context = (ErrorContext){.emit_output = true};
}

internal string error_context_copy_string(Arena* arena, string value)
{
    if (value.count == 0) {
        return (string){0};
    }
    u8* copy = arena_alloc(arena, value.count);
    memcpy(copy, value.data, value.count);
    return string_from(copy, value.count);
}

bool error_context_capture(const ErrorInfo* info)
{
    ErrorContext* context = error_context_current();
    if (!context->capture) {
        return false;
    }
    Arena* arena = &context->captured_arena;
    if (arena->data == NULL) {
        arena_init(arena);
    }
    ErrorInfo copy     = *info;
    copy.error_message = error_context_copy_string(arena, info->error_message);
    copy.source.source = error_context_copy_string(arena, info->source.source);
    copy.source.source_path =
        error_context_copy_string(arena, info->source.source_path);
    copy.source.fragments = NULL;
    copy.references       = NULL;
    copy.notes            = NULL;
    copy.help_messages    = NULL;
    for (usize i = 0; i < array_count(info->source.fragments); ++i) {
        NerdSourceFragment fragment = info->source.fragments[i];
        fragment.source = error_context_copy_string(arena, fragment.source);
        fragment.source_path =
            error_context_copy_string(arena, fragment.source_path);
        array_push(copy.source.fragments, fragment);
    }
    for (usize i = 0; i < array_count(info->references); ++i) {
        ErrorRef ref = info->references[i];
        ref.message  = error_context_copy_string(arena, ref.message);
        array_push(copy.references, ref);
    }
    for (usize i = 0; i < array_count(info->notes); ++i) {
        array_push(copy.notes,
                   error_context_copy_string(arena, info->notes[i]));
    }
    for (usize i = 0; i < array_count(info->help_messages); ++i) {
        array_push(copy.help_messages,
                   error_context_copy_string(arena, info->help_messages[i]));
    }
    array_push(context->pending, copy);
    return true;
}

void error_context_replay(ErrorContext* context)
{
    ASSERT(error_context_current() != context,
           "Cannot replay diagnostics into their own context");
    for (usize i = 0; i < array_count(context->pending); ++i) {
        ErrorInfo info = context->pending[i];
        error_render(&info);
        // Rendering consumes these arrays; source snapshots remain queue-owned.
        context->pending[i].references    = NULL;
        context->pending[i].notes         = NULL;
        context->pending[i].help_messages = NULL;
    }
    error_context_clear_pending(context);
}

internal ErrorInfo error_info_init(ErrorKind  kind,
                                   NerdSource source,
                                   ErrorSpan  span,
                                   cstr       error_format,
                                   va_list    args);

void error_system_init(ErrorRenderMode mode)
{
    ASSERT(g_error_current == NULL,
           "System initialization requires the default diagnostic context");
    error_context_init(&g_error_default, mode, false);
}

void error_system_done(void)
{
    ASSERT(g_error_current == NULL,
           "System cleanup requires the default diagnostic context");
    error_context_done(&g_error_default);
}

void error_system_set_mode(ErrorRenderMode mode)
{
    error_context_current()->mode = mode;
}

void error_system_set_emit_output(bool emit_output)
{
    error_context_current()->emit_output = emit_output;
}

void error_system_clear_last_rendered(void)
{
    arena_reset(&error_context_current()->rendered_arena);
    error_context_current()->last_rendered = (string){0};
}

string error_system_last_rendered(void)
{
    return error_context_current()->last_rendered;
}

void error_system_store_last_rendered(string rendered)
{
    error_system_clear_last_rendered();
    if (rendered.count == 0) {
        return;
    }

    Arena* arena = &error_context_current()->rendered_arena;
    if (arena->data == NULL) {
        arena_init(arena);
    }
    u8* copy = (u8*)arena_alloc(arena, rendered.count);
    memcpy(copy, rendered.data, rendered.count);
    error_context_current()->last_rendered = string_from(copy, rendered.count);
}

ErrorRenderMode error_system_mode(void)
{
    return error_context_current()->mode;
}
bool error_system_should_emit_output(void)
{
    return error_context_current()->emit_output;
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

bool error_runtime(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    ErrorInfo error_info = error_info_init(
        ERROR_KIND_RUNTIME, (NerdSource){0}, (ErrorSpan){0}, format, args);
    va_end(args);
    error_render(&error_info);
    return false;
}

void error_system_reset(void) { arena_reset(&error_context_current()->arena); }

internal ErrorInfo error_info_init(ErrorKind  kind,
                                   NerdSource source,
                                   ErrorSpan  span,
                                   cstr       error_format,
                                   va_list    args)
{
    string error_message =
        string_formatv(error_message_arena(), error_format, args);

    return (ErrorInfo){
        .kind          = kind,
        .error_message = error_message,
        .source        = source,
        .span          = span,
    };
}

ErrorInfo error_init(NerdSource source, ErrorSpan span, cstr error_format, ...)
{
    va_list args;
    va_start(args, error_format);
    ErrorInfo error_info =
        error_info_init(ERROR_KIND_ERROR, source, span, error_format, args);
    va_end(args);
    return error_info;
}

ErrorInfo
warning_init(NerdSource source, ErrorSpan span, cstr error_format, ...)
{
    va_list args;
    va_start(args, error_format);
    ErrorInfo error_info =
        error_info_init(ERROR_KIND_WARNING, source, span, error_format, args);
    va_end(args);
    return error_info;
}

void error_add_reference(
    ErrorInfo* error_info, ErrorRefKind kind, ErrorSpan span, cstr format, ...)
{
    va_list args;
    va_start(args, format);
    string message = string_formatv(error_message_arena(), format, args);
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
    string note = string_formatv(error_message_arena(), format, args);

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
    string help = string_formatv(error_message_arena(), format, args);
    array_push(error_info->help_messages, help);
}

void error_add_help(ErrorInfo* error_info, cstr format, ...)
{
    va_list args;
    va_start(args, format);
    error_add_helpv(error_info, format, args);
    va_end(args);
}

internal void error_context_test_diagnostic(NerdSource source, u32 number)
{
    ErrorInfo info =
        number == 1
            ? error_init(source, (ErrorSpan){0, 1}, "message %u", number)
            : warning_init(source, (ErrorSpan){0, 1}, "message %u", number);
    error_add_reference(
        &info, ERROR_REF_PRIMARY, (ErrorSpan){0, 1}, "reference %u", number);
    error_add_note(&info, "note %u", number);
    error_add_help(&info, "help %u", number);
    error_render(&info);
}

bool error_context_self_test(void)
{
    bool        ok     = true;
    MemoryStats before = mem_stats_snapshot();
    for (u32 mode = ERROR_RENDER_TEST; mode <= ERROR_RENDER_DIAGNOSTICS;
         ++mode) {
        ErrorContext output, first, second;
        error_context_init(&output, (ErrorRenderMode)mode, false);
        error_context_init(&first, ERROR_RENDER_NORMAL, true);
        error_context_init(&second, ERROR_RENDER_NORMAL, true);
        ErrorContext* previous = error_context_select(&output);
        error_system_set_emit_output(false);
        Arena source_arena = {0};
        arena_init(&source_arena);
        NerdSource source = {
            .source = error_context_copy_string(&source_arena, s("x\n")),
            .source_path =
                error_context_copy_string(&source_arena, s("combined.n")),
        };
        array_push(
            source.fragments,
            ((NerdSourceFragment){
                .start        = 0,
                .end          = 2,
                .source_start = 0,
                .source = error_context_copy_string(&source_arena, s("y\n")),
                .source_path =
                    error_context_copy_string(&source_arena, s("fragment.n")),
            }));
        error_context_test_diagnostic(source, 1);
        Arena expected_arena = {0};
        arena_init(&expected_arena);
        string expected = error_context_copy_string(
            &expected_arena, error_system_last_rendered());
        error_context_select(&first);
        error_context_test_diagnostic(source, 1);
        error_context_test_diagnostic(source, 2);
        error_context_select(&second);
        error_runtime("other task");
        error_context_select(&first);
        ok = ok && array_count(first.pending) == 2 &&
             array_count(second.pending) == 1;
        // Neither reset nor destroying original input may invalidate the queue.
        error_system_reset();
        array_free(source.fragments);
        arena_done(&source_arena);
        error_context_select(&output);
        ok = ok && !error_system_should_emit_output() &&
             error_system_mode() == (ErrorRenderMode)mode;
        // Use a capturing coordinator to observe replay order and ownership.
        ErrorContext ordered;
        error_context_init(&ordered, ERROR_RENDER_NORMAL, true);
        error_context_select(&ordered);
        error_context_replay(&first);
        error_context_select(&output);
        ok = ok && array_count(first.pending) == 0 &&
             array_count(ordered.pending) == 2;
        ok = ok &&
             string_eq_cstr(ordered.pending[0].error_message, "message 1") &&
             string_eq_cstr(ordered.pending[1].error_message, "message 2");
        error_context_done(&first);
        ErrorInfo one = ordered.pending[0];
        error_render(&one);
        ordered.pending[0].references    = NULL;
        ordered.pending[0].notes         = NULL;
        ordered.pending[0].help_messages = NULL;
        ok = ok && string_eq(expected, error_system_last_rendered());
        error_context_done(&first);
        error_context_done(&second);
        error_context_done(&ordered);
        arena_done(&expected_arena);
        error_context_select(previous);
        error_context_done(&output);
    }
    ok = ok &&
         mem_stats_snapshot().heap_current_bytes == before.heap_current_bytes;
    if (ok) {
        prn("error-context ok");
    }
    return ok;
}

bool error_context_render_self_test(bool capture)
{
    ErrorContext tasks[2];
    for (u32 i = 0; i < 2; ++i) {
        error_context_init(&tasks[i], error_system_mode(), true);
    }
    Arena source_arena = {0};
    arena_init(&source_arena);
    NerdSource source = {
        .source = error_context_copy_string(&source_arena, s("x\n")),
        .source_path =
            error_context_copy_string(&source_arena, s("combined.n")),
    };
    array_push(source.fragments,
               ((NerdSourceFragment){
                   .start        = 0,
                   .end          = 2,
                   .source_start = 0,
                   .source = error_context_copy_string(&source_arena, s("y\n")),
                   .source_path = error_context_copy_string(&source_arena,
                                                            s("fragment.n")),
               }));
    if (capture) {
        // Simulate reverse task completion, then replay in coordinator order.
        ErrorContext* previous = error_context_select(&tasks[1]);
        error_context_test_diagnostic(source, 2);
        error_context_select(&tasks[0]);
        error_context_test_diagnostic(source, 1);
        error_context_select(previous);
    } else {
        error_context_test_diagnostic(source, 1);
        error_context_test_diagnostic(source, 2);
    }
    array_free(source.fragments);
    arena_done(&source_arena);
    for (u32 i = 0; i < 2; ++i) {
        error_context_replay(&tasks[i]);
        error_context_done(&tasks[i]);
    }
    prn("error-context-render ok");
    return true;
}
