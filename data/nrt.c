#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#if defined(_WIN32)
#include <windows.h>
#else
#include <sys/mman.h>
#include <unistd.h>
#endif

typedef uint8_t u8;

typedef struct {
    u8*    data;
    size_t count;
} NerdString;

typedef struct {
    u8*    base;
    u8*    current;
    size_t committed_size;
    size_t increment;
} NrtArena;

typedef struct {
    size_t heap_alloc_count;
    size_t heap_realloc_count;
    size_t heap_free_count;
    size_t heap_bytes_allocated;
    size_t heap_bytes_reallocated;
    size_t heap_bytes_freed;
    size_t heap_current_bytes;
    size_t heap_peak_bytes;
    size_t arena_init_count;
    size_t arena_done_count;
    size_t arena_alloc_count;
    size_t arena_bytes_allocated;
    size_t arena_commit_count;
    size_t arena_bytes_committed;
    size_t array_growth_count;
    size_t array_bytes_allocated;
} NrtMemoryStats;

typedef struct NrtMemoryHeader_t {
    size_t                    size;
    char*                     file;
    uint32_t                  line;
    uint64_t                  index;
    bool                      leaked;
    struct NrtMemoryHeader_t* next;
} NrtMemoryHeader;

void string_builder_reset(void);

static NrtMemoryHeader* g_nrt_memory_head        = NULL;
static uint64_t         g_nrt_memory_index       = 0;
static uint64_t         g_nrt_memory_break_index = 0;
static NrtMemoryStats   g_nrt_memory_stats       = {0};

static void nrt_eprintf(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
}

static void nrt_eprintfn(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    nrt_eprintf("\n");
}

static void nrt_write_bytes(FILE* stream, const void* data, size_t count)
{
    if (data != NULL && count > 0) {
        fwrite(data, 1, count, stream);
    }
}

static char* nrt_memory_copy_file(const u8* file_data, size_t file_count)
{
    char* copy = (char*)malloc(file_count + 1);
    if (copy == NULL) {
        nrt_eprintfn("fatal: memory source path allocation failed");
        abort();
    }
    if (file_data != NULL && file_count > 0) {
        memcpy(copy, file_data, file_count);
    }
    copy[file_count] = '\0';
    return copy;
}

static void nrt_memory_set_source(NrtMemoryHeader* header,
                                  const u8*       file_data,
                                  size_t          file_count,
                                  uint32_t        line)
{
    free(header->file);
    header->file = nrt_memory_copy_file(file_data, file_count);
    header->line = line;
}

static void nrt_memory_add_heap_current(size_t size)
{
    g_nrt_memory_stats.heap_current_bytes += size;
    if (g_nrt_memory_stats.heap_current_bytes >
        g_nrt_memory_stats.heap_peak_bytes) {
        g_nrt_memory_stats.heap_peak_bytes =
            g_nrt_memory_stats.heap_current_bytes;
    }
}

static void nrt_memory_sub_heap_current(size_t size)
{
    if (size > g_nrt_memory_stats.heap_current_bytes) {
        g_nrt_memory_stats.heap_current_bytes = 0;
        return;
    }
    g_nrt_memory_stats.heap_current_bytes -= size;
}

static void nrt_memory_link(NrtMemoryHeader* header)
{
    header->next      = g_nrt_memory_head;
    g_nrt_memory_head = header;
}

static void nrt_memory_unlink(NrtMemoryHeader* header)
{
    if (header == NULL) {
        return;
    }
    if (g_nrt_memory_head == header) {
        g_nrt_memory_head = header->next;
        header->next      = NULL;
        return;
    }

    NrtMemoryHeader* current = g_nrt_memory_head;
    while (current != NULL && current->next != header) {
        current = current->next;
    }
    if (current != NULL) {
        current->next = header->next;
        header->next  = NULL;
    }
}

void* nrt_mem_alloc(size_t      size,
                    const u8*   file_data,
                    size_t      file_count,
                    uint32_t    line)
{
    NrtMemoryHeader* header =
        (NrtMemoryHeader*)malloc(sizeof(NrtMemoryHeader) + size);
    if (header == NULL) {
        nrt_eprintfn("fatal: memory allocation failed");
        abort();
    }

    header->size   = size;
    header->file   = NULL;
    header->line   = 0;
    header->leaked = false;
    header->index  = ++g_nrt_memory_index;
    nrt_memory_set_source(header, file_data, file_count, line);

    if (header->index == g_nrt_memory_break_index) {
        nrt_eprintfn("std.mem: allocation break reached at index %llu",
                     (unsigned long long)header->index);
    }

    nrt_memory_link(header);

    g_nrt_memory_stats.heap_alloc_count++;
    g_nrt_memory_stats.heap_bytes_allocated += size;
    nrt_memory_add_heap_current(size);

    return (void*)(header + 1);
}

void* nrt_mem_realloc(void*       ptr,
                      size_t      size,
                      const u8*   file_data,
                      size_t      file_count,
                      uint32_t    line)
{
    if (ptr == NULL) {
        return nrt_mem_alloc(size, file_data, file_count, line);
    }

    NrtMemoryHeader* old_header = ((NrtMemoryHeader*)ptr) - 1;
    size_t           old_size   = old_header->size;
    bool             was_leaked = old_header->leaked;

    if (!old_header->leaked) {
        nrt_memory_unlink(old_header);
    }

    NrtMemoryHeader* header =
        (NrtMemoryHeader*)realloc(old_header, sizeof(NrtMemoryHeader) + size);
    if (header == NULL) {
        nrt_eprintfn("fatal: memory reallocation failed");
        abort();
    }

    header->size   = size;
    header->leaked = was_leaked;
    header->index  = ++g_nrt_memory_index;
    nrt_memory_set_source(header, file_data, file_count, line);

    if (header->index == g_nrt_memory_break_index) {
        nrt_eprintfn("std.mem: allocation break reached at index %llu",
                     (unsigned long long)header->index);
    }

    if (!header->leaked) {
        nrt_memory_link(header);
    }

    g_nrt_memory_stats.heap_realloc_count++;
    g_nrt_memory_stats.heap_bytes_reallocated += size;
    if (size >= old_size) {
        nrt_memory_add_heap_current(size - old_size);
    } else {
        nrt_memory_sub_heap_current(old_size - size);
    }

    return (void*)(header + 1);
}

void nrt_mem_free(void* ptr)
{
    if (ptr == NULL) {
        return;
    }

    NrtMemoryHeader* header = ((NrtMemoryHeader*)ptr) - 1;

    g_nrt_memory_stats.heap_free_count++;
    g_nrt_memory_stats.heap_bytes_freed += header->size;
    nrt_memory_sub_heap_current(header->size);

    if (!header->leaked) {
        nrt_memory_unlink(header);
    }

    free(header->file);
    free(header);
}

size_t nrt_mem_size(const void* ptr)
{
    if (ptr == NULL) {
        return 0;
    }
    const NrtMemoryHeader* header = ((const NrtMemoryHeader*)ptr) - 1;
    return header->size;
}

void nrt_mem_leak(void* ptr)
{
    if (ptr == NULL) {
        return;
    }

    NrtMemoryHeader* header = ((NrtMemoryHeader*)ptr) - 1;
    if (header->leaked) {
        return;
    }
    header->leaked = true;
    nrt_memory_unlink(header);
}

void nrt_mem_break_on_alloc(uint64_t index)
{
    g_nrt_memory_break_index = index;
}

size_t nrt_mem_allocation_count(void)
{
    size_t           count   = 0;
    NrtMemoryHeader* current = g_nrt_memory_head;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}

size_t nrt_mem_total_allocated(void)
{
    size_t           total   = 0;
    NrtMemoryHeader* current = g_nrt_memory_head;
    while (current != NULL) {
        total += current->size;
        current = current->next;
    }
    return total;
}

void nrt_mem_print_leaks(void)
{
    if (g_nrt_memory_head == NULL) {
        return;
    }

    size_t           leak_count   = 0;
    size_t           total_leaked = 0;
    NrtMemoryHeader* current      = g_nrt_memory_head;

    nrt_eprintfn("std.mem: memory leaks detected");
    while (current != NULL) {
        nrt_eprintfn("  [%llu] %s:%u %zu bytes",
                     (unsigned long long)current->index,
                     current->file != NULL ? current->file : "",
                     current->line,
                     current->size);
        leak_count++;
        total_leaked += current->size;
        current = current->next;
    }
    nrt_eprintfn("std.mem: total %zu leaks, %zu bytes",
                 leak_count,
                 total_leaked);
}

void nrt_mem_stats_snapshot(NrtMemoryStats* out)
{
    if (out != NULL) {
        *out = g_nrt_memory_stats;
    }
}

void nrt_mem_stats_delta(NrtMemoryStats*       out,
                         const NrtMemoryStats* before,
                         const NrtMemoryStats* after)
{
    if (out == NULL || before == NULL || after == NULL) {
        return;
    }

    *out = (NrtMemoryStats){
        .heap_alloc_count =
            after->heap_alloc_count - before->heap_alloc_count,
        .heap_realloc_count =
            after->heap_realloc_count - before->heap_realloc_count,
        .heap_free_count = after->heap_free_count - before->heap_free_count,
        .heap_bytes_allocated =
            after->heap_bytes_allocated - before->heap_bytes_allocated,
        .heap_bytes_reallocated =
            after->heap_bytes_reallocated - before->heap_bytes_reallocated,
        .heap_bytes_freed =
            after->heap_bytes_freed - before->heap_bytes_freed,
        .heap_current_bytes =
            after->heap_current_bytes >= before->heap_current_bytes
                ? after->heap_current_bytes - before->heap_current_bytes
                : 0,
        .heap_peak_bytes = after->heap_peak_bytes,
        .arena_init_count =
            after->arena_init_count - before->arena_init_count,
        .arena_done_count =
            after->arena_done_count - before->arena_done_count,
        .arena_alloc_count =
            after->arena_alloc_count - before->arena_alloc_count,
        .arena_bytes_allocated =
            after->arena_bytes_allocated - before->arena_bytes_allocated,
        .arena_commit_count =
            after->arena_commit_count - before->arena_commit_count,
        .arena_bytes_committed =
            after->arena_bytes_committed - before->arena_bytes_committed,
        .array_growth_count =
            after->array_growth_count - before->array_growth_count,
        .array_bytes_allocated =
            after->array_bytes_allocated - before->array_bytes_allocated,
    };
}

void nrt_mem_record_arena_init(size_t bytes_committed)
{
    g_nrt_memory_stats.arena_init_count++;
    g_nrt_memory_stats.arena_commit_count++;
    g_nrt_memory_stats.arena_bytes_committed += bytes_committed;
}

void nrt_mem_record_arena_done(void)
{
    g_nrt_memory_stats.arena_done_count++;
}

void nrt_mem_record_arena_commit(size_t bytes_committed)
{
    g_nrt_memory_stats.arena_commit_count++;
    g_nrt_memory_stats.arena_bytes_committed += bytes_committed;
}

void nrt_mem_record_arena_alloc(size_t bytes_allocated)
{
    g_nrt_memory_stats.arena_alloc_count++;
    g_nrt_memory_stats.arena_bytes_allocated += bytes_allocated;
}

void nrt_mem_record_array_growth(size_t bytes_allocated)
{
    g_nrt_memory_stats.array_growth_count++;
    g_nrt_memory_stats.array_bytes_allocated += bytes_allocated;
}

void nrt_pr(const void* data, size_t count)
{
    nrt_write_bytes(stdout, data, count);
}

void nrt_epr(const void* data, size_t count)
{
    nrt_write_bytes(stderr, data, count);
}

void nrt_prn(const void* data, size_t count)
{
    nrt_pr(data, count);
    fputc('\n', stdout);
}

void nrt_eprn(const void* data, size_t count)
{
    nrt_epr(data, count);
    fputc('\n', stderr);
}

void nerd_assert(bool condition,
                 const char* source_path,
                 uint32_t    line,
                 const NerdString* message)
{
    if (condition) {
        return;
    }
    nrt_eprintf("assertion failed at %s:%u: ", source_path, line);
    if (message != NULL && message->count > 0) {
        fwrite(message->data, 1, message->count, stderr);
    }
    nrt_eprintf("\n");
    exit(127);
}

bool string_eq(const NerdString* lhs, const NerdString* rhs)
{
    if (lhs == NULL || rhs == NULL) {
        return lhs == rhs;
    }
    if (lhs->count != rhs->count) {
        return false;
    }
    if (lhs->count == 0) {
        return true;
    }
    return memcmp(lhs->data, rhs->data, lhs->count) == 0;
}

static const size_t NRT_ARENA_RESERVE_SIZE = (size_t)1 << 32;
static const size_t NRT_ARENA_MAX_CURSOR   = UINT32_MAX;

static size_t nrt_page_size(void)
{
#if defined(_WIN32)
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    return (size_t)info.dwPageSize;
#else
    long page_size = sysconf(_SC_PAGESIZE);
    return page_size > 0 ? (size_t)page_size : 4096;
#endif
}

static size_t nrt_align_up(size_t value, size_t alignment)
{
    if (alignment == 0) {
        return value;
    }
    size_t remainder = value % alignment;
    return remainder == 0 ? value : value + alignment - remainder;
}

static void nrt_arena_abort(const char* message)
{
    nrt_eprintfn("fatal: %s", message);
    exit(127);
}

static void* nrt_arena_reserve(size_t size)
{
#if defined(_WIN32)
    return VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_READWRITE);
#else
    void* memory =
        mmap(NULL, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    return memory == MAP_FAILED ? NULL : memory;
#endif
}

static bool nrt_arena_commit(u8* memory, size_t size)
{
    if (size == 0) {
        return true;
    }
#if defined(_WIN32)
    return VirtualAlloc(memory, size, MEM_COMMIT, PAGE_READWRITE) != NULL;
#else
    return mprotect(memory, size, PROT_READ | PROT_WRITE) == 0;
#endif
}

static void nrt_arena_release(u8* memory, size_t size)
{
    if (memory == NULL) {
        return;
    }
#if defined(_WIN32)
    (void)size;
    VirtualFree(memory, 0, MEM_RELEASE);
#else
    munmap(memory, size);
#endif
}

void nrt_arena_init(NrtArena* arena, size_t initial_size, size_t increment)
{
    if (arena == NULL) {
        return;
    }

    size_t page = nrt_page_size();
    if (initial_size == 0) {
        initial_size = page;
    }
    initial_size = nrt_align_up(initial_size, page);
    if (initial_size > NRT_ARENA_MAX_CURSOR) {
        nrt_arena_abort("arena initial capacity exceeds 4 GiB");
    }
    if (increment == 0) {
        increment = initial_size;
    }
    increment = nrt_align_up(increment, page);
    if (increment > NRT_ARENA_MAX_CURSOR) {
        nrt_arena_abort("arena growth increment exceeds 4 GiB");
    }

    u8* memory = (u8*)nrt_arena_reserve(NRT_ARENA_RESERVE_SIZE);
    if (memory == NULL) {
        nrt_arena_abort("arena address reservation failed");
    }
    if (!nrt_arena_commit(memory, initial_size)) {
        nrt_arena_release(memory, NRT_ARENA_RESERVE_SIZE);
        nrt_arena_abort("arena initial commit failed");
    }

    arena->base           = memory;
    arena->current        = memory;
    arena->committed_size = initial_size;
    arena->increment      = increment;
}

void nrt_arena_done(NrtArena* arena)
{
    if (arena == NULL) {
        return;
    }
    nrt_arena_release(arena->base, NRT_ARENA_RESERVE_SIZE);
    *arena = (NrtArena){0};
}

void nrt_arena_reset(NrtArena* arena)
{
    if (arena == NULL) {
        return;
    }
    arena->current = arena->base;
}

uint32_t nrt_arena_mark(NrtArena* arena)
{
    if (arena == NULL || arena->base == NULL) {
        return 0;
    }
    return (uint32_t)(arena->current - arena->base);
}

void nrt_arena_restore(NrtArena* arena, uint32_t mark)
{
    if (arena == NULL || arena->base == NULL) {
        return;
    }
    size_t cursor = (size_t)mark;
    if (cursor > (size_t)(arena->current - arena->base)) {
        nrt_arena_abort("invalid arena restore mark");
    }
    arena->current = arena->base + cursor;
}

void* nrt_arena_alloc(NrtArena* arena, size_t size, size_t alignment)
{
    if (arena == NULL) {
        return NULL;
    }
    if (arena->base == NULL) {
        nrt_arena_init(arena, 0, 0);
    }
    if (alignment == 0) {
        alignment = sizeof(void*);
    }

    size_t cursor = (size_t)(arena->current - arena->base);
    size_t start  = nrt_align_up(cursor, alignment);
    if (start < cursor || size > NRT_ARENA_MAX_CURSOR - start) {
        nrt_arena_abort("arena exceeded 4 GiB capacity");
    }

    size_t end = start + size;
    if (end > arena->committed_size) {
        size_t page        = nrt_page_size();
        size_t grow_target = nrt_align_up(end, arena->increment);
        size_t commit_end  = nrt_align_up(grow_target, page);
        if (commit_end < end || commit_end > NRT_ARENA_RESERVE_SIZE) {
            nrt_arena_abort("arena exceeded 4 GiB capacity");
        }
        size_t commit_size = commit_end - arena->committed_size;
        if (!nrt_arena_commit(arena->base + arena->committed_size,
                              commit_size)) {
            nrt_arena_abort("arena memory commit failed");
        }
        arena->committed_size = commit_end;
    }

    arena->current = arena->base + end;
    return (void*)(arena->base + start);
}

void nrt_temp_arena_reset(void) { string_builder_reset(); }

NrtArena* nrt_temp_arena(void);

static bool string_is_utf8_boundary(const NerdString* value, size_t index)
{
    if (value == NULL || index > value->count) {
        return false;
    }
    return index == value->count || (value->data[index] & 0xC0) != 0x80;
}

void string_slice(NerdString* out,
                  const NerdString* value,
                  size_t start,
                  size_t end)
{
    if (out == NULL) {
        return;
    }
    if (value == NULL || start > end || end > value->count ||
        !string_is_utf8_boundary(value, start) ||
        !string_is_utf8_boundary(value, end)) {
        nrt_eprintfn("fatal: string slice out of bounds");
        abort();
    }

    *out = (NerdString){.data = value->data + start, .count = end - start};
}

#define NERD_STRING_BUILDER_INITIAL_CAPACITY 4096

static _Thread_local NrtArena g_temp_arena;
static _Thread_local u8*      g_string_builder_data     = NULL;
static _Thread_local size_t   g_string_builder_capacity = 0;
static _Thread_local size_t   g_string_builder_cursor   = 0;

NrtArena* nrt_temp_arena(void) { return &g_temp_arena; }

static void string_builder_ensure_capacity(size_t needed)
{
    if (needed <= g_string_builder_capacity) {
        return;
    }

    size_t capacity = g_string_builder_capacity;
    if (capacity == 0) {
        capacity = NERD_STRING_BUILDER_INITIAL_CAPACITY;
    }
    while (capacity < needed) {
        capacity *= 2;
    }

    u8* data = (u8*)realloc(g_string_builder_data, capacity);
    if (data == NULL) {
        nrt_eprintfn("fatal: string builder allocation failed");
        abort();
    }
    g_string_builder_data     = data;
    g_string_builder_capacity = capacity;
}

void string_builder_reset(void)
{
    g_string_builder_cursor = 0;
    nrt_arena_reset(&g_temp_arena);
}

size_t string_builder_mark(void) { return g_string_builder_cursor; }

void string_builder_append_string(const NerdString* str)
{
    if (str == NULL) {
        return;
    }
    string_builder_ensure_capacity(g_string_builder_cursor + str->count);

    if (str->count > 0) {
        memcpy(g_string_builder_data + g_string_builder_cursor,
               str->data,
               str->count);
        g_string_builder_cursor += str->count;
    }
}

void string_builder_append_byte(u8 byte)
{
    string_builder_ensure_capacity(g_string_builder_cursor + 1);

    g_string_builder_data[g_string_builder_cursor++] = byte;
}

void string_builder_finish(NerdString* out, size_t start)
{
    if (out == NULL) {
        return;
    }
    if (start > g_string_builder_cursor) {
        nrt_eprintfn("fatal: invalid string builder mark");
        abort();
    }

    size_t count = g_string_builder_cursor - start;
    u8*    data  = NULL;
    if (count > 0) {
        data = (u8*)nrt_arena_alloc(&g_temp_arena, count, 1);
        memcpy(data, g_string_builder_data + start, count);
    }
    *out = (NerdString){.data = data, .count = count};
    g_string_builder_cursor = start;
}

void to_string$string(NerdString* out, const NerdString* value)
{
    if (out != NULL && value != NULL) {
        *out = *value;
    }
}

void to_string$bool(NerdString* out, bool value)
{
    if (out == NULL) {
        return;
    }
    *out = value ? (NerdString){.data = (u8*)"yes", .count = 3}
                 : (NerdString){.data = (u8*)"no", .count = 2};
}

#define DEF_TO_STRING_INT(name, type, format, cast_type)                       \
    void to_string$##name(NerdString* out, type value)                         \
    {                                                                          \
        static _Thread_local u8 buffer[64];                                    \
        int count =                                                            \
            snprintf((char*)buffer, sizeof(buffer), format, (cast_type)value); \
        if (out == NULL) {                                                     \
            return;                                                            \
        }                                                                      \
        if (count < 0) {                                                       \
            *out = (NerdString){0};                                            \
            return;                                                            \
        }                                                                      \
        *out = (NerdString){.data = buffer, .count = (size_t)count};           \
    }

#define DEF_TO_STRING_FLOAT(name, type)                                        \
    void to_string$##name(NerdString* out, type value)                         \
    {                                                                          \
        static _Thread_local u8 buffer[64];                                    \
        int count =                                                            \
            snprintf((char*)buffer, sizeof(buffer), "%g", (double)value);     \
        if (out == NULL) {                                                     \
            return;                                                            \
        }                                                                      \
        if (count < 0) {                                                       \
            *out = (NerdString){0};                                            \
            return;                                                            \
        }                                                                      \
        *out = (NerdString){.data = buffer, .count = (size_t)count};           \
    }

DEF_TO_STRING_INT(i8, int8_t, "%d", int)
DEF_TO_STRING_INT(i16, int16_t, "%d", int)
DEF_TO_STRING_INT(i32, int, "%d", int)
DEF_TO_STRING_INT(i64, int64_t, "%lld", long long)
DEF_TO_STRING_INT(u8, uint8_t, "%u", unsigned int)
DEF_TO_STRING_INT(u16, uint16_t, "%u", unsigned int)
DEF_TO_STRING_INT(u32, uint32_t, "%u", unsigned int)
DEF_TO_STRING_INT(u64, uint64_t, "%llu", unsigned long long)
DEF_TO_STRING_INT(isize, intptr_t, "%td", ptrdiff_t)
DEF_TO_STRING_INT(usize, uintptr_t, "%zu", size_t)
DEF_TO_STRING_FLOAT(f32, float)
DEF_TO_STRING_FLOAT(f64, double)

size_t nrt_read_line(u8* buffer, size_t capacity)
{
    size_t count = 0;
    int    ch    = 0;

    while (count < capacity && (ch = getchar()) != EOF) {
        if (ch == '\n') {
            return count;
        }
        if (ch == '\r') {
            int next = getchar();
            if (next != '\n' && next != EOF) {
                ungetc(next, stdin);
            }
            return count;
        }
        buffer[count++] = (u8)ch;
    }

    if (count == capacity) {
        while ((ch = getchar()) != EOF) {
            if (ch == '\n') {
                break;
            }
            if (ch == '\r') {
                int next = getchar();
                if (next != '\n' && next != EOF) {
                    ungetc(next, stdin);
                }
                break;
            }
        }
    }

    return count;
}
