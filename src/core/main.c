//------------------------------------------------------------------------------
// Main entry point
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <core/core.h>

extern Mutex g_kore_output_mutex;
Arena        temp_arena;

//------------------------------------------------------------------------------

int run(int argc, char** argv);

int main(int argc, char** argv)
{
    mutex_init(&g_kore_output_mutex);

#if OS_WINDOWS
    UINT old_cp = GetConsoleCP();
    SetConsoleCP(CP_UTF8);
    UINT old_output_cp = GetConsoleOutputCP();
    SetConsoleOutputCP(CP_UTF8);
#endif // OS_WINDOWS

    // Initialise the global temporary arena
    arena_init(&temp_arena);

    int result = run(argc, argv);

    arena_done(&temp_arena);

#if OS_WINDOWS
    SetConsoleCP(old_cp);
    SetConsoleOutputCP(old_output_cp);
#endif // OS_WINDOWS

#if CONFIG_DEBUG
    mem_print_leaks();
#endif // CONFIG_DEBUG
    mutex_done(&g_kore_output_mutex);
    return result;
}

void kill(cstr format, ...)
{
    va_list args;
    va_start(args, format);
    eprv(format, args);
    va_end(args);
    epr("\n");
    abort();
}
