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

typedef struct NrtHeapFooter {
    size_t mapping_size;
    size_t requested_size;
} NrtHeapFooter;

#ifndef NDEBUG
typedef struct NrtHeapDebugHeader {
    struct NrtHeapDebugHeader* prev;
    struct NrtHeapDebugHeader* next;
    size_t                     requested_size;
    uint64_t                   index;
} NrtHeapDebugHeader;

static NrtHeapDebugHeader* g_nrt_heap_head       = NULL;
static uint64_t            g_nrt_heap_next_index = 0;
#endif

void string_builder_reset(void);
void nrt_mem_free(void* memory);
size_t nrt_mem_size(void* memory);
void nrt_mem_leak(void* memory);

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

static void* nrt_mem_reserve(size_t size, bool writable)
{
#if defined(_WIN32)
    DWORD protect = writable ? PAGE_READWRITE : PAGE_NOACCESS;
    DWORD type    = writable ? MEM_RESERVE | MEM_COMMIT : MEM_RESERVE;
    return VirtualAlloc(NULL, size, type, protect);
#else
    int prot = writable ? PROT_READ | PROT_WRITE : PROT_NONE;
    void* memory =
        mmap(NULL, size, prot, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
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

static void nrt_mem_release(void* memory, size_t size)
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

static void nrt_heap_abort(const char* message)
{
    nrt_eprintfn("fatal: %s", message);
    exit(127);
}

static size_t nrt_heap_prefix_size(size_t alignment)
{
    size_t prefix = sizeof(NrtHeapFooter);
#ifndef NDEBUG
    prefix += sizeof(NrtHeapDebugHeader);
#endif
    return nrt_align_up(prefix, alignment);
}

static NrtHeapFooter* nrt_heap_footer_from_user(void* memory)
{
    return memory == NULL ? NULL
                          : (NrtHeapFooter*)((u8*)memory -
                                             sizeof(NrtHeapFooter));
}

#ifndef NDEBUG
static NrtHeapDebugHeader* nrt_heap_debug_from_user(void* memory)
{
    if (memory == NULL) {
        return NULL;
    }
    size_t    page    = nrt_page_size();
    uintptr_t address = (uintptr_t)memory;
    return (NrtHeapDebugHeader*)(address - (address % page));
}

static void nrt_heap_unlink(NrtHeapDebugHeader* header)
{
    if (header == NULL) {
        return;
    }
    if (header->prev != NULL) {
        header->prev->next = header->next;
    } else if (g_nrt_heap_head == header) {
        g_nrt_heap_head = header->next;
    }
    if (header->next != NULL) {
        header->next->prev = header->prev;
    }
    header->prev = NULL;
    header->next = NULL;
}
#endif

void nrt_mem_print_leaks(void)
{
#ifndef NDEBUG
    if (g_nrt_heap_head == NULL) {
        return;
    }

    fflush(stdout);
    size_t count = 0;
    size_t total = 0;
    nrt_eprintfn("nrt: memory leaks detected");
    for (NrtHeapDebugHeader* current = g_nrt_heap_head; current != NULL;
         current                     = current->next) {
        count++;
        total += current->requested_size;
    }
    nrt_eprintfn("nrt: total %zu leaks, %zu bytes", count, total);
#endif
}

void* nrt_mem_alloc(size_t size,
                    size_t alignment,
                    const char* source_path,
                    uint32_t    line)
{
    (void)source_path;
    (void)line;
    if (alignment < 16) {
        alignment = 16;
    }

    size_t prefix = nrt_heap_prefix_size(alignment);
    size_t page   = nrt_page_size();
    if (size > SIZE_MAX - prefix) {
        nrt_heap_abort("heap allocation size overflow");
    }
    size_t mapping_size = nrt_align_up(prefix + size, page);
    void*  mapping      = nrt_mem_reserve(mapping_size, true);
    if (mapping == NULL) {
        nrt_heap_abort("heap allocation failed");
    }

    u8*            user   = (u8*)mapping + prefix;
    NrtHeapFooter* footer = nrt_heap_footer_from_user(user);
    footer->mapping_size  = mapping_size;
    footer->requested_size = size;

#ifndef NDEBUG
    NrtHeapDebugHeader* debug = (NrtHeapDebugHeader*)mapping;
    debug->prev               = NULL;
    debug->next               = g_nrt_heap_head;
    debug->requested_size     = size;
    debug->index              = ++g_nrt_heap_next_index;
    if (g_nrt_heap_head != NULL) {
        g_nrt_heap_head->prev = debug;
    }
    g_nrt_heap_head = debug;
#endif

    return user;
}

void* nrt_mem_realloc(void*       memory,
                      size_t      size,
                      size_t      alignment,
                      const char* source_path,
                      uint32_t    line)
{
    if (memory == NULL) {
        return nrt_mem_alloc(size, alignment, source_path, line);
    }

    size_t old_size = nrt_mem_size(memory);
    void*  result   = nrt_mem_alloc(size, alignment, source_path, line);
    if (result != NULL && old_size > 0 && size > 0) {
        memcpy(result, memory, old_size < size ? old_size : size);
    }
    nrt_mem_free(memory);
    return result;
}

void nrt_mem_free(void* memory)
{
    if (memory == NULL) {
        return;
    }
    NrtHeapFooter* footer = nrt_heap_footer_from_user(memory);
#ifndef NDEBUG
    nrt_heap_unlink(nrt_heap_debug_from_user(memory));
#endif
    size_t mapping_size = footer->mapping_size;
    size_t    page        = nrt_page_size();
    uintptr_t address     = (uintptr_t)memory;
    void*     mapping     = (void*)(address - (address % page));
    nrt_mem_release(mapping, mapping_size);
}

size_t nrt_mem_size(void* memory)
{
    NrtHeapFooter* footer = nrt_heap_footer_from_user(memory);
    return footer == NULL ? 0 : footer->requested_size;
}

void nrt_mem_leak(void* memory)
{
#ifndef NDEBUG
    nrt_heap_unlink(nrt_heap_debug_from_user(memory));
#else
    (void)memory;
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

    u8* memory = (u8*)nrt_mem_reserve(NRT_ARENA_RESERVE_SIZE, false);
    if (memory == NULL) {
        nrt_arena_abort("arena address reservation failed");
    }
    if (!nrt_arena_commit(memory, initial_size)) {
        nrt_mem_release(memory, NRT_ARENA_RESERVE_SIZE);
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
    nrt_mem_release(arena->base, NRT_ARENA_RESERVE_SIZE);
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

    u8* data = (u8*)nrt_mem_realloc(g_string_builder_data, capacity, 16, NULL, 0);
    if (data == NULL) {
        nrt_eprintfn("fatal: string builder allocation failed");
        abort();
    }
    nrt_mem_leak(data);
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
