//------------------------------------------------------------------------------
// Memory management
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <core/core.h>

#include <stdio.h>
#include <stdlib.h>

//------------------------------------------------------------------------------

typedef struct MemoryHeader_t {
    usize size; // Number of bytes allocated

#if CONFIG_DEBUG
    const char* file;  // File where the allocation was made
    int         line;  // Line number where the allocation was made
    u64         index; // Index of the allocation for debugging purposes

    struct MemoryHeader_t* next; // Pointer to the next header in a linked list
    bool leaked; // Flag to indicate if this block was leaked and therefore
                 // should not be in the linked list.  This is used to mark
                 // allocations with application lifetimes.
#endif           // CONFIG_DEBUG
} MemoryHeader;

// Global pointer to the head of the linked list of allocated memory blocks
#if CONFIG_DEBUG
static MemoryHeader* g_memory_head        = NULL;
static u64           g_memory_index       = 0; // Global index for allocations
static u64           g_memory_break_index = 0; // Index to break on allocation
#endif                                         // CONFIG_DEBUG

static MemoryStats g_memory_stats = {0};

internal void mem_stats_add_heap_current(usize size)
{
    g_memory_stats.heap_current_bytes += size;
    if (g_memory_stats.heap_current_bytes > g_memory_stats.heap_peak_bytes) {
        g_memory_stats.heap_peak_bytes = g_memory_stats.heap_current_bytes;
    }
}

internal void mem_stats_sub_heap_current(usize size)
{
    if (size > g_memory_stats.heap_current_bytes) {
        g_memory_stats.heap_current_bytes = 0;
        return;
    }
    g_memory_stats.heap_current_bytes -= size;
}

MemoryStats mem_stats_snapshot(void) { return g_memory_stats; }

MemoryStats mem_stats_delta(MemoryStats before, MemoryStats after)
{
    return (MemoryStats){
        .heap_alloc_count = after.heap_alloc_count - before.heap_alloc_count,
        .heap_realloc_count =
            after.heap_realloc_count - before.heap_realloc_count,
        .heap_free_count = after.heap_free_count - before.heap_free_count,
        .heap_bytes_allocated =
            after.heap_bytes_allocated - before.heap_bytes_allocated,
        .heap_bytes_reallocated =
            after.heap_bytes_reallocated - before.heap_bytes_reallocated,
        .heap_bytes_freed = after.heap_bytes_freed - before.heap_bytes_freed,
        .heap_current_bytes =
            after.heap_current_bytes >= before.heap_current_bytes
                ? after.heap_current_bytes - before.heap_current_bytes
                : 0,
        .heap_peak_bytes   = after.heap_peak_bytes,
        .arena_init_count  = after.arena_init_count - before.arena_init_count,
        .arena_done_count  = after.arena_done_count - before.arena_done_count,
        .arena_alloc_count = after.arena_alloc_count - before.arena_alloc_count,
        .arena_bytes_allocated =
            after.arena_bytes_allocated - before.arena_bytes_allocated,
        .arena_commit_count =
            after.arena_commit_count - before.arena_commit_count,
        .arena_bytes_committed =
            after.arena_bytes_committed - before.arena_bytes_committed,
        .array_growth_count =
            after.array_growth_count - before.array_growth_count,
        .array_bytes_allocated =
            after.array_bytes_allocated - before.array_bytes_allocated,
    };
}

bool mem_stats_profile_enabled(void)
{
    cstr value = getenv("NERD_MEMORY_PROFILE");
    return value != NULL && value[0] != '\0' && strcmp(value, "0") != 0;
}

void mem_stats_print_delta(cstr stage, cstr phase, MemoryStats stats)
{
    eprn("memory\t%s\t%s\theap_allocs=%zu\theap_reallocs=%zu\t"
         "heap_frees=%zu\theap_bytes=%zu\theap_realloc_bytes=%zu\t"
         "heap_freed_bytes=%zu\theap_net_bytes=%zu\theap_peak_bytes=%zu\t"
         "arena_inits=%zu\tarena_dones=%zu\tarena_allocs=%zu\t"
         "arena_bytes=%zu\tarena_commits=%zu\tarena_commit_bytes=%zu\t"
         "array_growths=%zu\tarray_bytes=%zu",
         stage,
         phase,
         stats.heap_alloc_count,
         stats.heap_realloc_count,
         stats.heap_free_count,
         stats.heap_bytes_allocated,
         stats.heap_bytes_reallocated,
         stats.heap_bytes_freed,
         stats.heap_current_bytes,
         stats.heap_peak_bytes,
         stats.arena_init_count,
         stats.arena_done_count,
         stats.arena_alloc_count,
         stats.arena_bytes_allocated,
         stats.arena_commit_count,
         stats.arena_bytes_committed,
         stats.array_growth_count,
         stats.array_bytes_allocated);
}

void mem_stats_record_arena_init(usize bytes_committed)
{
    g_memory_stats.arena_init_count++;
    mem_stats_record_arena_commit(bytes_committed);
}

void mem_stats_record_arena_done(void) { g_memory_stats.arena_done_count++; }

void mem_stats_record_arena_commit(usize bytes_committed)
{
    g_memory_stats.arena_commit_count++;
    g_memory_stats.arena_bytes_committed += bytes_committed;
}

void mem_stats_record_arena_alloc(usize bytes_allocated)
{
    g_memory_stats.arena_alloc_count++;
    g_memory_stats.arena_bytes_allocated += bytes_allocated;
}

void mem_stats_record_array_growth(usize bytes_allocated)
{
    g_memory_stats.array_growth_count++;
    g_memory_stats.array_bytes_allocated += bytes_allocated;
}

void* mem_alloc(usize size, const char* file, int line)
{
    MemoryHeader* header = (MemoryHeader*)malloc(sizeof(MemoryHeader) + size);
    if (!header) {
        fprintf(stderr, "Memory allocation failed at %s:%d\n", file, line);
        abort();
    }

    header->size = size;
    g_memory_stats.heap_alloc_count++;
    g_memory_stats.heap_bytes_allocated += size;
    mem_stats_add_heap_current(size);

#if CONFIG_DEBUG
    header->file   = file;
    header->line   = line;
    header->leaked = false;            // Initialise leaked flag
    header->index  = ++g_memory_index; // Assign and increment index

    // Check if we should break on this allocation
    if (header->index == g_memory_break_index) {
        DEBUG_BREAK();
    }

    // Add to linked list
    header->next  = g_memory_head;
    g_memory_head = header;
#endif // CONFIG_DEBUG

    return (void*)(header + 1);
}

void* mem_realloc(void* ptr, usize size, const char* file, int line)
{
    if (!ptr) {
        return mem_alloc(size, file, line);
    }

    MemoryHeader* old_header = (MemoryHeader*)ptr - 1;
    usize         old_size   = old_header->size;

#if CONFIG_DEBUG
    // Preserve the leaked flag from the old header
    bool was_leaked = old_header->leaked;
#endif // CONFIG_DEBUG

// Remove old header from linked list
#if CONFIG_DEBUG
    if (!old_header->leaked) {
        if (g_memory_head == old_header) {
            g_memory_head = old_header->next;
        } else {
            MemoryHeader* current = g_memory_head;
            while (current && current->next != old_header) {
                current = current->next;
            }
            if (current) {
                current->next = old_header->next;
            }
        }
    }
#endif // CONFIG_DEBUG

    MemoryHeader* header =
        (MemoryHeader*)realloc(old_header, sizeof(MemoryHeader) + size);
    if (!header) {
        fprintf(stderr, "Memory reallocation failed at %s:%d\n", file, line);
        abort();
    }

    header->size = size;
    g_memory_stats.heap_realloc_count++;
    g_memory_stats.heap_bytes_reallocated += size;
    if (size >= old_size) {
        mem_stats_add_heap_current(size - old_size);
    } else {
        mem_stats_sub_heap_current(old_size - size);
    }

#if CONFIG_DEBUG
    header->file   = file;
    header->line   = line;
    header->leaked = was_leaked; // Preserve the leaked flag
    header->index  = ++g_memory_index;

    if (header->index == g_memory_break_index) {
        DEBUG_BREAK(); // Break if this allocation matches the break index
    }

    // Add new header to linked list only if it's not leaked
    if (!header->leaked) {
        header->next  = g_memory_head;
        g_memory_head = header;
    }
#endif // CONFIG_DEBUG

    return (void*)(header + 1);
}

void* mem_free(void* ptr, const char* file, int line)
{
    (void)file; // Suppress unused parameter warning
    (void)line; // Suppress unused parameter warning

    if (!ptr) {
        return nullptr;
    }

    MemoryHeader* header = (MemoryHeader*)ptr - 1;
    g_memory_stats.heap_free_count++;
    g_memory_stats.heap_bytes_freed += header->size;
    mem_stats_sub_heap_current(header->size);

#if CONFIG_DEBUG
    // Remove from linked list
    if (!header->leaked) {
        if (g_memory_head == header) {
            g_memory_head = header->next;
        } else {
            MemoryHeader* current = g_memory_head;
            while (current && current->next != header) {
                current = current->next;
            }
            if (current) {
                current->next = header->next;
            }
        }
    }
#endif // CONFIG_DEBUG

    free(header);
    return nullptr;
}

usize mem_size(const void* ptr)
{
    if (!ptr) {
        return 0;
    }

    const MemoryHeader* header = (const MemoryHeader*)ptr - 1;
    return header->size;
}

void mem_leak(void* ptr)
{
#if CONFIG_DEBUG

    if (!ptr) {
        return;
    }

    MemoryHeader* header = (MemoryHeader*)ptr - 1;
    header->leaked       = true; // Mark this block as leaked

    // Remove from linked list if it exists
    if (g_memory_head == header) {
        g_memory_head = header->next;
    } else {
        MemoryHeader* current = g_memory_head;
        while (current && current->next != header) {
            current = current->next;
        }
        if (current) {
            current->next = header->next;
        }
    }

#else
    UNUSED(ptr);

#endif // CONFIG_DEBUG
}

void mem_break_on_alloc(u64 index)
{
#if CONFIG_DEBUG
    g_memory_break_index = index;
#else
    UNUSED(index);
#endif
}

#if CONFIG_DEBUG

// Memory debugging utilities
void mem_print_leaks(void)
{
    MemoryHeader* current      = g_memory_head;
    usize         leak_count   = 0;
    usize         total_leaked = 0;

    if (!current) {
        return;
    }

    eprn(ANSI_BOLD_RED "┌──────────────────────────────────────┐" ANSI_RESET);
    eprn(ANSI_BOLD_RED "│        Memory leaks detected         │" ANSI_RESET);
    eprn(ANSI_BOLD_RED "└──────────────────────────────────────┘" ANSI_RESET);

    while (current) {
        eprn(ANSI_FAINT " %s" ANSI_RESET ANSI_BOLD "[%zu]" ANSI_RESET
                        " %s:%d " ANSI_BOLD_YELLOW "%zu bytes" ANSI_RESET,
             UNICODE_TREE_BRANCH,
             current->index,
             current->file,
             current->line,
             current->size);

        total_leaked += current->size;
        leak_count++;
        current = current->next;
    }

    eprn(" " ANSI_FAINT UNICODE_TREE_LAST_BRANCH ANSI_RESET ANSI_BOLD_RED
         "Total:" ANSI_RESET " %zu leaks, %zu bytes",
         leak_count,
         total_leaked);
}

usize mem_get_allocation_count(void)
{
    usize         count   = 0;
    MemoryHeader* current = g_memory_head;

    while (current) {
        count++;
        current = current->next;
    }

    return count;
}

usize mem_get_total_allocated(void)
{
    usize         total   = 0;
    MemoryHeader* current = g_memory_head;

    while (current) {
        total += current->size;
        current = current->next;
    }

    return total;
}

#endif // CONFIG_DEBUG

void mem_check(void* ptr)
{
    if (!ptr) {

        eprn(ANSI_BOLD_RED
             "┌──────────────────────────────────────┐" ANSI_RESET);
        eprn(ANSI_BOLD_RED
             "│        Out of memory error           │" ANSI_RESET);
        eprn(ANSI_BOLD_RED
             "└──────────────────────────────────────┘" ANSI_RESET);
        exit(1);
    }
}
