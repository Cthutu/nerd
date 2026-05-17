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
#include <unistd.h>
#endif

typedef uint8_t u8;

typedef struct {
    u8*    data;
    size_t count;
} NerdString;

typedef struct NrtArenaBlock {
    struct NrtArenaBlock* next;
    size_t                capacity;
    size_t                cursor;
    max_align_t           data[];
} NrtArenaBlock;

typedef struct {
    NrtArenaBlock* first;
    NrtArenaBlock* current;
    size_t         initial_size;
    size_t         increment;
} NrtArena;

void string_builder_reset(void);

static void nrt_epr(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
}

static void nrt_eprn(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    nrt_epr("\n");
}

void nerd_assert(bool condition,
                 const char* source_path,
                 uint32_t    line,
                 const NerdString* message)
{
    if (condition) {
        return;
    }
    nrt_epr("assertion failed at %s:%u: ", source_path, line);
    if (message != NULL && message->count > 0) {
        fwrite(message->data, 1, message->count, stderr);
    }
    nrt_epr("\n");
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

static NrtArenaBlock* nrt_arena_new_block(size_t capacity)
{
    NrtArenaBlock* block =
        (NrtArenaBlock*)malloc(sizeof(NrtArenaBlock) + capacity);
    if (block == NULL) {
        nrt_eprn("fatal: arena allocation failed");
        abort();
    }
    block->next     = NULL;
    block->capacity = capacity;
    block->cursor   = 0;
    return block;
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
    if (increment == 0) {
        increment = initial_size;
    }
    increment = nrt_align_up(increment, page);

    arena->first        = nrt_arena_new_block(initial_size);
    arena->current      = arena->first;
    arena->initial_size = initial_size;
    arena->increment    = increment;
}

void nrt_arena_done(NrtArena* arena)
{
    if (arena == NULL) {
        return;
    }
    NrtArenaBlock* block = arena->first;
    while (block != NULL) {
        NrtArenaBlock* next = block->next;
        free(block);
        block = next;
    }
    *arena = (NrtArena){0};
}

void nrt_arena_reset(NrtArena* arena)
{
    if (arena == NULL) {
        return;
    }
    for (NrtArenaBlock* block = arena->first; block != NULL;
         block                = block->next) {
        block->cursor = 0;
    }
    arena->current = arena->first;
}

void* nrt_arena_alloc(NrtArena* arena, size_t size, size_t alignment)
{
    if (arena == NULL) {
        return NULL;
    }
    if (arena->first == NULL) {
        nrt_arena_init(arena, 0, 0);
    }
    if (alignment == 0) {
        alignment = sizeof(void*);
    }

    NrtArenaBlock* block = arena->current;
    size_t         start = nrt_align_up(block->cursor, alignment);
    if (start + size > block->capacity) {
        size_t capacity = arena->increment;
        size_t needed   = nrt_align_up(size + alignment, nrt_page_size());
        if (capacity < needed) {
            capacity = needed;
        }
        NrtArenaBlock* next = block->next;
        while (next != NULL && next->capacity < needed) {
            next = next->next;
        }
        if (next == NULL) {
            next        = nrt_arena_new_block(capacity);
            next->next  = block->next;
            block->next = next;
        }
        block          = next;
        arena->current = block;
        start          = nrt_align_up(block->cursor, alignment);
    }

    block->cursor = start + size;
    return (void*)((u8*)block->data + start);
}

void nrt_temp_arena_reset(void) { string_builder_reset(); }

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
        nrt_eprn("fatal: string slice out of bounds");
        abort();
    }

    *out = (NerdString){.data = value->data + start, .count = end - start};
}

#define NERD_STRING_ARENA_CAPACITY (64 * 1024)

static _Thread_local u8     g_string_arena[NERD_STRING_ARENA_CAPACITY];
static _Thread_local size_t g_string_arena_cursor = 0;

void string_builder_reset(void) { g_string_arena_cursor = 0; }

size_t string_builder_mark(void) { return g_string_arena_cursor; }

void string_builder_append_string(const NerdString* str)
{
    if (str == NULL) {
        return;
    }
    if (g_string_arena_cursor + str->count > NERD_STRING_ARENA_CAPACITY) {
        nrt_eprn("fatal: string arena overflow");
        abort();
    }

    if (str->count > 0) {
        memcpy(g_string_arena + g_string_arena_cursor, str->data, str->count);
        g_string_arena_cursor += str->count;
    }
}

void string_builder_append_byte(u8 byte)
{
    if (g_string_arena_cursor + 1 > NERD_STRING_ARENA_CAPACITY) {
        nrt_eprn("fatal: string arena overflow");
        abort();
    }

    g_string_arena[g_string_arena_cursor++] = byte;
}

void string_builder_finish(NerdString* out, size_t start)
{
    if (out == NULL) {
        return;
    }
    *out = (NerdString){.data  = g_string_arena + start,
                        .count = g_string_arena_cursor - start};
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

size_t nerd_read_line(u8* buffer, size_t capacity)
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
