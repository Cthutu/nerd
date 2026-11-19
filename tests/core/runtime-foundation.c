// Runtime-owned primitives: libc is used only by the test oracle/host entry.
#include "../../data/nrt.c"
#include <limits.h>

#define CHECK(condition)                                                       \
    do {                                                                       \
        if (!(condition)) {                                                    \
            fprintf(stderr,                                                    \
                    "check failed at line %d: %s\n",                           \
                    __LINE__,                                                  \
                    #condition);                                               \
            exit(1);                                                           \
        }                                                                      \
    } while (0)

static void check_bytes(void)
{
    unsigned char source[513], expected[513], actual[513];
    for (size_t i = 0; i < sizeof(source); ++i) {
        source[i] = (unsigned char)i;
    }
    for (size_t count = 0; count <= 256; ++count) {
        for (size_t offset = 0; offset <= 32; ++offset) {
            memcpy(expected, source, sizeof(source));
            nrt_bytes_copy(actual, source, sizeof(source));
            CHECK(memcmp(actual, expected, sizeof(actual)) == 0);
            memmove(expected + offset, expected, count);
            nrt_bytes_move(actual + offset, actual, count);
            CHECK(memcmp(actual, expected, sizeof(actual)) == 0);
            memmove(expected, expected + offset, count);
            nrt_bytes_move(actual, actual + offset, count);
            CHECK(memcmp(actual, expected, sizeof(actual)) == 0);
            CHECK(nrt_bytes_compare(actual, expected, sizeof(actual)) == 0);
        }
    }
    nrt_bytes_copy(NULL, NULL, 0);
    nrt_bytes_move(NULL, NULL, 0);
    CHECK(nrt_bytes_compare(NULL, NULL, 0) == 0);
    CHECK(nrt_bytes_compare("\x80", "\x7f", 1) > 0);
    CHECK(nrt_bytes_compare("\x7f", "\x80", 1) < 0);
    CHECK(nrt_cstr_count("") == 0);
    CHECK(nrt_cstr_count("hello\0ignored") == 5);
    CHECK(nrt_cstr_equal("", ""));
    CHECK(nrt_cstr_equal("hello", "hello"));
    CHECK(!nrt_cstr_equal("hello", "hell"));
    CHECK(!nrt_cstr_equal("hell", "hello"));
}

#define CHECK_INTEGER(name, type, value, format, cast_type)                    \
    do {                                                                       \
        char       expected[64];                                               \
        NerdString actual;                                                     \
        type       input = (type)(value);                                      \
        int        length =                                                    \
            snprintf(expected, sizeof(expected), format, (cast_type)input);    \
        nrt_to_string_##name(&actual, input);                                  \
        CHECK(actual.count == (size_t)length);                                 \
        CHECK(memcmp(actual.data, expected, actual.count) == 0);               \
        CHECK(actual.data[actual.count] == 0);                                 \
        nrt_to_string_##name(NULL, input);                                     \
    } while (0)

static void check_integers(void)
{
    // Include the asymmetric signed minimum and every narrow integer value.
    for (int i = INT8_MIN; i <= UINT8_MAX; ++i) {
        CHECK_INTEGER(i8, int8_t, i, "%d", int);
        CHECK_INTEGER(u8, uint8_t, i, "%u", unsigned);
    }
    uint64_t bits = 0;
    for (size_t i = 0; i < 2048; ++i) {
        CHECK_INTEGER(i16, int16_t, bits, "%d", int);
        CHECK_INTEGER(u16, uint16_t, bits, "%u", unsigned);
        CHECK_INTEGER(i32, int32_t, bits, "%d", int);
        CHECK_INTEGER(u32, uint32_t, bits, "%u", unsigned);
        CHECK_INTEGER(i64, int64_t, bits, "%lld", long long);
        CHECK_INTEGER(u64, uint64_t, bits, "%llu", unsigned long long);
        CHECK_INTEGER(isize, intptr_t, bits, "%td", ptrdiff_t);
        CHECK_INTEGER(usize, uintptr_t, bits, "%zu", size_t);
        bits = bits * UINT64_C(6364136223846793005) + 1;
    }
    CHECK_INTEGER(i64, int64_t, INT64_MIN, "%lld", long long);
    CHECK_INTEGER(i64, int64_t, INT64_MAX, "%lld", long long);
    CHECK_INTEGER(u64, uint64_t, UINT64_MAX, "%llu", unsigned long long);
    CHECK_INTEGER(i16, int16_t, INT16_MIN, "%d", int);
    CHECK_INTEGER(i32, int32_t, INT32_MIN, "%d", int);
    for (uint64_t power = 1; power <= UINT64_C(10000000000000000000);) {
        CHECK_INTEGER(u64, uint64_t, power - 1, "%llu", unsigned long long);
        CHECK_INTEGER(u64, uint64_t, power, "%llu", unsigned long long);
        CHECK_INTEGER(u64, uint64_t, power + 1, "%llu", unsigned long long);
        if (power == UINT64_C(10000000000000000000)) {
            break;
        }
        power *= 10;
    }
}

static void check_mapping(void)
{
    size_t         page   = nrt_page_size();
    unsigned char* memory = nrt_mem_reserve(page * 2, false);
    CHECK(memory != NULL);
    CHECK(nrt_arena_commit(memory, page));
    memory[0]        = 17;
    memory[page - 1] = 29;
    CHECK(nrt_arena_commit(memory + page, page));
    CHECK(memory[0] == 17 && memory[page - 1] == 29);
    CHECK(memory[page] == 0 && memory[page * 2 - 1] == 0);
    memory[page * 2 - 1] = 31;
    CHECK(nrt_arena_commit(NULL, 0));
#if defined(NRT_LINUX_RAW_MEMORY)
    CHECK(!nrt_arena_commit(memory + 1, page));
    CHECK(nrt_mem_reserve(0, true) == NULL);
    CHECK(nrt_mem_reserve(SIZE_MAX, true) == NULL);
#endif
    nrt_mem_release(memory, page * 2);
    nrt_mem_release(NULL, 0);
}

static void check_variadic(int tag, ...)
{
    NrtVaList* cursor = nrt_va_create();
    CHECK(cursor->owner == cursor && cursor->next == NULL);
    va_start(cursor->args, tag);
    CHECK(nrt_va_next_i32(cursor) == 42);
    NrtVaList* copy = nrt_va_copy(cursor);
    CHECK(copy->owner == cursor);
    CHECK(nrt_va_next_u64(cursor) == UINT64_MAX);
    CHECK(nrt_va_next_u64(copy) == UINT64_MAX);
    CHECK(nrt_va_next_f64(cursor) == 1.25);
    CHECK(nrt_va_next_f64(copy) == 1.25);
    // Frees the owner and every copied cursor exactly once.
    nrt_va_done(cursor);
    nrt_va_done(NULL);
}

int main(void)
{
    check_bytes();
    check_integers();
    check_mapping();
    check_variadic(0, 42, UINT64_MAX, 1.25);
#ifndef NDEBUG
    CHECK(nrt_mem_live_head() == NULL);
#endif
    nrt_core_done();
    puts("runtime foundation passed");
    return 0;
}
