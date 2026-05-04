#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

typedef uint8_t u8;

#define DEF_SLICE(type)                                                        \
    typedef struct {                                                           \
        type*  data;                                                           \
        size_t count;                                                          \
    }

DEF_SLICE(u8) string;

static void epr(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
}

static void eprn(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    epr("\n");
}

static void
nerd_assert(bool condition, const char* source_path, unsigned line, string message)
{
    if (condition) {
        return;
    }
    epr("assertion failed at %s:%u: ", source_path, line);
    fwrite(message.data, 1, message.count, stderr);
    epr("\n");
    exit(127);
}

static bool string_eq(string lhs, string rhs)
{
    if (lhs.count != rhs.count) {
        return false;
    }
    if (lhs.count == 0) {
        return true;
    }
    return memcmp(lhs.data, rhs.data, lhs.count) == 0;
}

static bool string_is_utf8_boundary(string value, size_t index)
{
    if (index > value.count) {
        return false;
    }
    return index == value.count || (value.data[index] & 0xC0) != 0x80;
}

static string string_slice(string value, size_t start, size_t end)
{
    if (start > end || end > value.count ||
        !string_is_utf8_boundary(value, start) ||
        !string_is_utf8_boundary(value, end)) {
        eprn("fatal: string slice out of bounds");
        abort();
    }

    return (string){.data = value.data + start, .count = end - start};
}

#define NERD_STRING_ARENA_CAPACITY (64 * 1024)

static _Thread_local u8     g_string_arena[NERD_STRING_ARENA_CAPACITY];
static _Thread_local size_t g_string_arena_cursor = 0;

void string_builder_reset(void) { g_string_arena_cursor = 0; }

size_t string_builder_mark(void) { return g_string_arena_cursor; }

void string_builder_append_string(string str)
{
    if (g_string_arena_cursor + str.count > NERD_STRING_ARENA_CAPACITY) {
        eprn("fatal: string arena overflow");
        abort();
    }

    if (str.count > 0) {
        memcpy(g_string_arena + g_string_arena_cursor, str.data, str.count);
        g_string_arena_cursor += str.count;
    }
}

string string_builder_finish(size_t start)
{
    return (string){.data = g_string_arena + start,
                    .count = g_string_arena_cursor - start};
}

string to_string$string(string value) { return value; }

string to_string$bool(bool value)
{
    return value ? (string){.data = (u8*)"yes", .count = 3}
                 : (string){.data = (u8*)"no", .count = 2};
}

#define DEF_TO_STRING_INT(name, type, format, cast_type)                       \
    string to_string$##name(type value)                                        \
    {                                                                          \
        static u8 buffer[64];                                                  \
        int count =                                                            \
            snprintf((char*)buffer, sizeof(buffer), format, (cast_type)value); \
        if (count < 0) {                                                       \
            return (string){0};                                                \
        }                                                                      \
        return (string){.data = buffer, .count = (size_t)count};               \
    }

#define DEF_TO_STRING_FLOAT(name, type)                                        \
    string to_string$##name(type value)                                        \
    {                                                                          \
        static u8 buffer[64];                                                  \
        int count =                                                            \
            snprintf((char*)buffer, sizeof(buffer), "%g", (double)value);      \
        if (count < 0) {                                                       \
            return (string){0};                                                \
        }                                                                      \
        return (string){.data = buffer, .count = (size_t)count};               \
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
