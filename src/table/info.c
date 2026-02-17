//------------------------------------------------------------------------------
// Info table implementation
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <table/table.h>
#include <time.h>

//------------------------------------------------------------------------------

internal cstr info_os_string(void)
{
#if OS_WINDOWS
    return "Windows";
#elif OS_LINUX
    return "Linux";
#elif OS_MACOS
    return "macOS";
#elif OS_BSD
    return "BSD";
#else
    return "Unknown";
#endif
}

internal cstr info_arch_string(void)
{
#if ARCH_X86
    return "x86";
#elif ARCH_X86_64
    return "x86_64";
#elif ARCH_ARM
    return "ARM";
#elif ARCH_ARM64
    return "ARM64";
#else
    return "Unknown";
#endif
}

internal cstr info_compiler_string(void)
{
#if COMPILER_CLANG
    return "Clang " STR(__clang_major__) "." STR(__clang_minor__) "." STR(
        __clang_patchlevel__);
#elif COMPILER_GCC
    return "GCC " __VERSION__;
#elif COMPILER_MSVC
    return "MSVC " STR(_MSC_VER);
#else
    return "Unknown";
#endif
}

internal cstr info_build_string(void)
{
#if CONFIG_DEBUG
    return "Debug";
#elif CONFIG_RELEASE
    return "Release";
#elif CONFIG_PROFILE
    return "Profile";
#else
    return "Unknown";
#endif
}

internal cstr info_time_source_string(void)
{
#if OS_WINDOWS
    return "Monotonic (QueryPerformanceCounter)";
#elif OS_POSIX
    return "Monotonic (clock_gettime CLOCK_MONOTONIC)";
#else
    return "Unknown";
#endif
}

internal cstr info_thread_time_support_string(void)
{
#if OS_WINDOWS
    return "Yes (GetThreadTimes)";
#elif OS_POSIX && defined(CLOCK_THREAD_CPUTIME_ID)
    return "Yes (CLOCK_THREAD_CPUTIME_ID)";
#elif OS_POSIX
    return "No (fallback CLOCK_MONOTONIC)";
#else
    return "Unknown";
#endif
}

void dump_info(void)
{
    Array(TableColumn) columns = NULL;
    array_push(columns,
               (TableColumn){.title = "Item", .colour = ANSI_CYAN},
               (TableColumn){.title = "Value", .colour = ANSI_GREEN});

    Table table;
    table_init(&table, columns, .title = "System Information");
    table_reserve_rows(&table, 6);

    {
        TableCell row[] = {table_cell_string(s("OS")),
                           table_cell_string(s(info_os_string()))};
        table_add_row(&table, row);
    }
    {
        TableCell row[] = {table_cell_string(s("CPU/Arch")),
                           table_cell_string(s(info_arch_string()))};
        table_add_row(&table, row);
    }
    {
        TableCell row[] = {table_cell_string(s("Compiler")),
                           table_cell_string(s(info_compiler_string()))};
        table_add_row(&table, row);
    }
    {
        TableCell row[] = {table_cell_string(s("Build")),
                           table_cell_string(s(info_build_string()))};
        table_add_row(&table, row);
    }
    {
        TableCell row[] = {table_cell_string(s("Time API")),
                           table_cell_string(s(info_time_source_string()))};
        table_add_row(&table, row);
    }
    {
        TableCell row[] = {
            table_cell_string(s("Per-thread timing")),
            table_cell_string(s(info_thread_time_support_string()))};
        table_add_row(&table, row);
    }

    table_print(&table);
    table_done(&table);
    array_free(columns);
}

//------------------------------------------------------------------------------
